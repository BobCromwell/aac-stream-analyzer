#include <stdio.h>

#include "logger.h"
#include "adts_parser.h"

aac_sa::Logger* logger = aac_sa::Logger::instance();

int analysis_aac_file(char* aac_file_name)
{
  FILE* aac_file  = fopen(aac_file_name, "rb");
  if (aac_file == NULL) {
    logger->error("can't open input file: %s", aac_file_name);
    return -1;
  }
  aac_sa::ADTSParseResult* adts_parse_result = new aac_sa::ADTSParseResult;
  int code = aac_sa::ADTSParser::parse(aac_file, adts_parse_result);
  if (code == 0) {
    logger->info("bitrate %d, length: %f frame_count: %d\n", \
                 adts_parse_result->bit_rate, \
                 adts_parse_result->length, \
                 adts_parse_result->frame_count \
                 );
  }
  delete adts_parse_result;
  fclose(aac_file);
  return 0;
}

int main(int argc, char *argv[])
{
  const int kMaxFileNameLength = 255;
  char aac_file_name[kMaxFileNameLength];
  //
  if (argc < 2) {
    logger->error("specify the file name");
  }
  strcpy(aac_file_name, argv[1]);
  analysis_aac_file(aac_file_name);
}
