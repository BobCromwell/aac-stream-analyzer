#include "adts_parser.h"

#include <stdlib.h>

#include "logger.h"

typedef struct {
    long bytes_into_buffer;
    long bytes_consumed;
    long file_offset;
    unsigned char *buffer;
    int at_eof;
    FILE *infile;
} aac_buffer;


static int fill_buffer(aac_buffer *b)
{
    int bread;

    if (b->bytes_consumed > 0)
    {
        if (b->bytes_into_buffer)
        {
            memmove((void*)b->buffer, (void*)(b->buffer + b->bytes_consumed),
                b->bytes_into_buffer*sizeof(unsigned char));
        }

        if (!b->at_eof)
        {
            bread = fread((void*)(b->buffer + b->bytes_into_buffer), 1,
                b->bytes_consumed, b->infile);

            if (bread != b->bytes_consumed)
                b->at_eof = 1;

            b->bytes_into_buffer += bread;
        }

        b->bytes_consumed = 0;

        if (b->bytes_into_buffer > 3)
        {
            if (memcmp(b->buffer, "TAG", 3) == 0)
                b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 11)
        {
            if (memcmp(b->buffer, "LYRICSBEGIN", 11) == 0)
                b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 8)
        {
            if (memcmp(b->buffer, "APETAGEX", 8) == 0)
                b->bytes_into_buffer = 0;
        }
    }

    return 1;
}

static void advance_buffer(aac_buffer *b, int bytes)
{
    b->file_offset += bytes;
    b->bytes_consumed = bytes;
    b->bytes_into_buffer -= bytes;
	if (b->bytes_into_buffer < 0)
		b->bytes_into_buffer = 0;
}

static int adts_sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0};


static int adts_parse(aac_buffer *b, int *bitrate, float *length, int *frame_count)
{
    int frames, frame_length;
    int t_framelength = 0;
    int samplerate;
    float frames_per_sec, bytes_per_frame;

    /* Read all frames to ensure correct time and bitrate */
    for (frames = 0; /* */; frames++)
    {
        fill_buffer(b);

        if (b->bytes_into_buffer > 7)
        {
            /* check syncword */
            if (!((b->buffer[0] == 0xFF)&&((b->buffer[1] & 0xF6) == 0xF0)))
                break;

            if (frames == 0)
                samplerate = adts_sample_rates[(b->buffer[2]&0x3c)>>2];

            frame_length = ((((unsigned int)b->buffer[3] & 0x3)) << 11)
                | (((unsigned int)b->buffer[4]) << 3) | (b->buffer[5] >> 5);

            t_framelength += frame_length;

            if (frame_length > b->bytes_into_buffer)
                break;

            advance_buffer(b, frame_length);
        } else {
            break;
        }
    }

    frames_per_sec = (float)samplerate/1024.0f;
    if (frames != 0)
        bytes_per_frame = (float)t_framelength/(float)(frames*1000);
    else
        bytes_per_frame = 0;
    *bitrate = (int)(8. * bytes_per_frame * frames_per_sec + 0.5);
    if (frames_per_sec != 0)
        *length = (float)frames/frames_per_sec;
    else
        *length = 1;

    *frame_count = frames;

    return 1;
}

#define MAX_CHANNELS 6
#define FAAD_MIN_STREAMSIZE 768 /* 6144 bits/channel */

namespace aac_sa {
  int ADTSParser::parse(FILE* aac_file, ADTSParseResult* result) {
    aac_buffer b;
    memset(&b, 0, sizeof(aac_buffer));

    b.infile = aac_file;

    fseek(b.infile, 0, SEEK_END);
    ftell(b.infile);
    fseek(b.infile, 0, SEEK_SET);

    if (!(b.buffer = (unsigned char*)malloc(FAAD_MIN_STREAMSIZE*MAX_CHANNELS)))
    {
        return -1;
    }
    memset(b.buffer, 0, FAAD_MIN_STREAMSIZE*MAX_CHANNELS);

    int bread = fread(b.buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, b.infile);
    b.bytes_into_buffer = bread;
    b.bytes_consumed = 0;
    b.file_offset = 0;

    if (bread != FAAD_MIN_STREAMSIZE*MAX_CHANNELS) {
      b.at_eof = 1;
    }

    int tagsize = 0;
    if (!memcmp(b.buffer, "ID3", 3))
    {
        /* high bit is not used */
        tagsize = (b.buffer[6] << 21) | (b.buffer[7] << 14) |
          (b.buffer[8] <<  7) | (b.buffer[9] <<  0);

        tagsize += 10;
        advance_buffer(&b, tagsize);
        fill_buffer(&b);
    }

    if ((b.buffer[0] == 0xFF) && ((b.buffer[1] & 0xF6) == 0xF0))
    {
      int bit_rate = 0;
      float length = 0;
      int frame_count = 0;
      adts_parse(&b, &bit_rate, &length, &frame_count);
      result->bit_rate = bit_rate;
      result->length = length;
      result->frame_count = frame_count;
    }
   return 0;
  }
} // namespace aac_sa
