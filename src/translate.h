#ifndef GRAPTH_TRANSLATE_H
#define GRAPTH_TRANSLATE_H

#include "units.h"

typedef enum {
  TRANSLATE_OK,
  TRANSLATE_INCONSISTENT,
  TRANSLATE_NO_MEMORY,
  TRANSLATE_UNEXPECTED
} TranslateResult;

TranslateResult unitsTranslate(Units *);

#endif
