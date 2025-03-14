#include "common.h"
#include "evaluate.h"
#include "memory.h"
#include "parse.h"
#include "string.h"
#include "translate.h"
#include "units.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFINITION_AREA_MIN 0.0
#define DEFINITION_AREA_MAX (4.0 * M_PI)
#define VALUE_RANGE_MIN -1.0
#define VALUE_RANGE_MAX 1.0
#define WINDOW_WIDTH 80
#define WINDOW_HIGHT 25
#ifdef DEBUG
#define WINDOW_CENTER_X 0
#define WINDOW_CENTER_Y (WINDOW_HIGHT / 2)
#endif
#define WINDOW_SYMBOL '.'
#define PLOT_SYMBOL '*'
#define AXIS_SYMBOL '#'

typedef enum {
  EXIT_OK,
  EXIT_READ_ERROR = 166,
  EXIT_PARSE_ERROR,
  EXIT_TRANSLATE_ERROR,
  EXIT_EVALUATE_ERROR
} ExitCode;

static ExitCode read(String *s);
static ExitCode parse(const String *, Units *);
static ExitCode translate(Units *);
static ExitCode evaluate(const Units *, NUMBER[WINDOW_WIDTH]);
static ExitCode print(const NUMBER[WINDOW_WIDTH]);

int main() {
  String string = stringNew();
  Units units = unitsNew();
  NUMBER values[WINDOW_WIDTH];
  ExitCode exitCode = read(&string) || parse(&string, &units) ||
                      translate(&units) || evaluate(&units, values) ||
                      print(values);
  unitsReset(&units);
  stringReset(&string);
  exit(exitCode);
}

static void stringDebug(const char *name, const String *s) {
#ifdef DEBUG
  printf("%s:\n  ", name);
  stringPrint(s);
#endif
}

static ExitCode read(String *s) {
  if (MEMORY_OK != stringRead(s, stdin))
    return EXIT_READ_ERROR;
  stringDebug("input", s);
  return EXIT_OK;
}

static void unitsDebug(const char *name, const Units *units) {
#ifdef DEBUG
  printf("%s:\n  ", name);
  unitsPrint(units);
#endif
}

static ExitCode parse(const String *s, Units *units) {
  if (PARSE_OK != cstringParse(stringAsCString(s), units))
    return EXIT_PARSE_ERROR;
  unitsDebug("parsed", units);
  return EXIT_OK;
}

static ExitCode translate(Units *units) {
  if (TRANSLATE_OK != unitsTranslate(units))
    return EXIT_TRANSLATE_ERROR;
  unitsDebug("translated", units);
  return EXIT_OK;
}

static void doublePrint(double number) { printf("%lf", number); }

static void floatPrint(float number) { printf("%f", number); }

static void valuesDebug(const char *name, const NUMBER *values, size_t length) {
#ifdef DEBUG
#define PRINT(number)                                                          \
  _Generic((number), double: doublePrint, float: floatPrint)(number)
  printf("%s:\n  ", name);
  for (size_t i = 0; i < length; i++) {
    printf("%s%zu:", i > 0 ? " " : "", i);
    PRINT(values[i]);
  }
  printf("\n");
#undef PRINT
#endif
}

static ExitCode evaluate(const Units *units, NUMBER values[WINDOW_WIDTH]) {
  size_t steps = WINDOW_WIDTH;
  NUMBER step = (DEFINITION_AREA_MAX - DEFINITION_AREA_MIN) / steps;
  NUMBER result;
  for (size_t i = 0; i < steps; i++) {
    if (EVALUATE_OK !=
        unitsEvaluate(units, DEFINITION_AREA_MIN + i * step, &result))
      return EXIT_EVALUATE_ERROR;
    values[i] = result;
  }
  valuesDebug("values", values, steps);
  return EXIT_OK;
}

static bool intoOrdinate(NUMBER value, size_t *y) {
  size_t steps = WINDOW_HIGHT;
  if (value < VALUE_RANGE_MIN || VALUE_RANGE_MAX < value)
    return false;
  NUMBER step = (VALUE_RANGE_MAX - VALUE_RANGE_MIN) / steps;
  size_t index = 0;
  while (index < steps) {
    NUMBER current = VALUE_RANGE_MAX - index * step;
    NUMBER next = fmax(VALUE_RANGE_MAX - (index + 1) * step, VALUE_RANGE_MIN);
    if (current >= value && value >= next) {
      *y = index;
      return true;
    }
    index += 1;
  }
  return false;
}

static void includeAxis(char window[WINDOW_HIGHT][WINDOW_WIDTH]) {
  for (size_t x = 0; x < WINDOW_WIDTH; x++)
    window[WINDOW_CENTER_Y][x] = AXIS_SYMBOL;
  for (size_t y = 0; y < WINDOW_HIGHT; y++)
    window[y][WINDOW_CENTER_X] = AXIS_SYMBOL;
}

static ExitCode print(const NUMBER values[WINDOW_WIDTH]) {
  char window[WINDOW_HIGHT][WINDOW_WIDTH];
  memset(window, WINDOW_SYMBOL, WINDOW_HIGHT * WINDOW_WIDTH);
#ifdef DEBUG
  includeAxis(window);
#endif
  size_t y;
  for (size_t x = 0; x < WINDOW_WIDTH; x++)
    if (intoOrdinate(values[x], &y))
      window[y][x] = PLOT_SYMBOL;
  for (size_t y = 0; y < WINDOW_HIGHT; y++) {
    for (size_t x = 0; x < WINDOW_WIDTH; x++)
      printf("%c", window[y][x]);
    printf("\n");
  }
  return EXIT_OK;
}

static void foo(int a) {}
