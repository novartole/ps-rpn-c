#include "parse.h"
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool isDigit(char ch) { return '0' <= ch && ch <= '9'; }

static ParseResult toNumber(const char *s, NUMBER *number) {
#define TO_NUMBER(ofType, s)                                                   \
  _Generic((ofType), double *: strtod, float *: strtof)(s, NULL)
  *number = TO_NUMBER(number, s);
  return ERANGE == errno ? PARSE_NAN : PARSE_OK;
#undef TO_NUMBER
}

static ParseResult consumeNumber(const char *s, NUMBER *number, int *offset) {
  size_t i = 1;
  while (isDigit(s[i]))
    i += 1;
  if ('.' == s[i]) {
    if (!isDigit(s[++i]))
      return PARSE_NAN;
    while (isDigit(s[i]))
      i += 1;
  }
  char sNumber[i + 1];
  strncpy(sNumber, s, i);
  sNumber[i] = '\0';
  ParseResult result;
  if (PARSE_OK == (result = toNumber(sNumber, number)))
    *offset = i;
  return result;
}

static ParseResult consumeFunction(const char *s, Token *unit, int *offset) {
  size_t sLength = strlen(s);
  switch (*s) {
  case 't':
    if (sLength >= 3 && 0 == memcmp("tg(", s, 3)) {
      *unit = TOKEN_TG;
      *offset = 2;
      return PARSE_OK;
    }
    break;
  case 'l':
    if (sLength >= 3 && 0 == memcmp("ln(", s, 3)) {
      *unit = TOKEN_LN;
      *offset = 2;
      return PARSE_OK;
    }
    break;
  case 'c':
    if (sLength >= 4) {
      if (0 == memcmp("cos(", s, 4)) {
        *unit = TOKEN_COS;
        *offset = 3;
        return PARSE_OK;
      } else if (0 == memcmp("ctg(", s, 4)) {
        *unit = TOKEN_CTG;
        *offset = 3;
        return PARSE_OK;
      }
    }
    break;
  case 's':
    if (sLength >= 4 && 0 == memcmp("sin(", s, 4)) {
      *unit = TOKEN_SIN;
      *offset = 3;
      return PARSE_OK;
    } else if (sLength >= 5 && 0 == memcmp("sqrt(", s, 5)) {
      *unit = TOKEN_SQRT;
      *offset = 4;
      return PARSE_OK;
    }
    break;
  }
  return PARSE_UNDEFINED;
}

ParseResult cstringParse(const char *s, Units *units) {
  if (NULL == units)
    return PARSE_NULL;
  size_t sLength;
  if (0 == (sLength = strlen(s)))
    return PARSE_OK;
  Token prev = -1;
  size_t i = 0;
  while (i < sLength) {
    Token cur;
    int offset = 1;
    switch (s[i]) {
    case ' ':
      i += offset;
      continue;
    case '+':
      cur = TOKEN_ADD;
      break;
    case '-':
      if (TOKEN_NUMBER == prev || TOKEN_RIGHT_PAREN == prev)
        cur = TOKEN_SUB;
      else
        cur = TOKEN_NEG;
      break;
    case '*':
      cur = TOKEN_MUL;
      break;
    case '/':
      cur = TOKEN_DIV;
      break;
    case '(':
      cur = TOKEN_LEFT_PAREN;
      break;
    case ')':
      cur = TOKEN_RIGHT_PAREN;
      break;
    case 'x':
      cur = TOKEN_X;
      break;
    default: {
      ParseResult result;
      if (isDigit(s[i])) {
        NUMBER number;
        if (PARSE_OK != (result = consumeNumber(s + i, &number, &offset)))
          return result;
        cur = TOKEN_NUMBER;
        unitsPushNumber(units, number);
      } else if (PARSE_OK != (result = consumeFunction(s + i, &cur, &offset)))
        return result;
    }
    }
    unitsPushToken(units, cur);
    prev = cur;
    i += offset;
  }
  return PARSE_OK;
}
