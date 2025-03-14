#ifndef GRAPTH_SCAN_H
#define GRAPTH_SCAN_H

#include "units.h"

typedef enum { PARSE_OK, PARSE_NAN, PARSE_UNDEFINED, PARSE_NULL } ParseResult;

ParseResult cstringParse(const char *, Units *);

#endif
