#ifndef GRAPTH_EVALUATE_H
#define GRAPTH_EVALUATE_H

#include "numbers.h"
#include "units.h"

typedef enum {
  EVALUATE_OK,
  EVALUATE_NO_MEMORY,
  EVALUATE_UNEXPECTED,
  EVALUATE_EMPTY_STACK,
  EVALUATE_NAN
} EvaluateResult;

EvaluateResult unitsEvaluate(const Units *, NUMBER, NUMBER *);

#endif
