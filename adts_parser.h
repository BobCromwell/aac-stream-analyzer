#ifndef ADTS_PARSER_HEADER
#define ADTS_PARSER_HEADER

#include <stdio.h>

namespace aac_sa {

struct ADTSParseResult {
  int bit_rate;
  float length;
  int frame_count;
};

class ADTSParser {
public:
  static int parse(FILE* aac_file, ADTSParseResult* result);
};
} // namespace aac_sa

#endif // ADTS_PARSER_HEADER
