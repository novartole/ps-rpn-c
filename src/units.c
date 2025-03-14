#include "units.h"
#include "evaluate.h"
#include "memory.h"
#include "numbers.h"
#include "tokens.h"
#include "translate.h"
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Units unitsNew() {
  Units units;
  units.tokens = tokensNew();
  units.numbers = numbersNew();
  return units;
}

MemoryResult unitsPushToken(Units *units, Token token) {
  return tokensPush(&units->tokens, token);
}

MemoryResult unitsPushNumber(Units *units, NUMBER number) {
  return numbersPush(&units->numbers, number);
}

bool unitsPop(Units *units, Token *token, NUMBER *number) {
  if (!tokensPop(&units->tokens, token))
    return false;
  return TOKEN_NUMBER == *token ? numbersPop(&units->numbers, number) : true;
}

bool unitsGet(const Units *units, size_t index, Token *token, NUMBER *number) {
  if (!tokensGet(&units->tokens, index, token))
    return false;
  if (TOKEN_NUMBER != *token)
    return true;
  bool hasNumber = false;
  size_t tIndex = 0;
  size_t nIndex = 0;
  Token candidate;
  while (tIndex <= index && tokensGet(&units->tokens, tIndex++, &candidate))
    if (TOKEN_NUMBER == candidate) {
      if (hasNumber)
        nIndex += 1;
      else
        hasNumber = true;
    }
  return numbersGet(&units->numbers, nIndex, number);
}

bool unitsPeek(const Units *units, size_t offset, Token *token,
               NUMBER *number) {
  size_t last;
  if (!tokensPeek(&units->tokens, offset, token, &last))
    return false;
  if (TOKEN_NUMBER != *token)
    return true;
  size_t tIndex = 0;
  size_t nIndex = 0;
  Token candidate;
  while (tIndex <= last && tokensGet(&units->tokens, tIndex++, &candidate))
    if (TOKEN_NUMBER == candidate)
      nIndex += 1;
  return numbersGet(&units->numbers, nIndex, number);
}

void unitsReset(Units *units) {
  tokensReset(&units->tokens);
  numbersReset(&units->numbers);
}

static void doublePrint(double number) { printf("%lf", number); }

static void floatPrint(float number) { printf("%f", number); }

void unitsPrint(const Units *units) {
#define PRINT(number)                                                          \
  _Generic((number), double: doublePrint, float: floatPrint)(number)
  size_t tIndex = 0;
  size_t nIndex = 0;
  Token token;
  while (tokensGet(&units->tokens, tIndex, &token)) {
    printf("%s%s", tIndex > 0 ? " " : "", tokenToString(token));
    if (TOKEN_NUMBER == token) {
      NUMBER number;
      if (numbersGet(&units->numbers, nIndex++, &number))
        PRINT(number);
      else
        printf("_");
    }
    tIndex += 1;
  }
  printf("\n");
#undef PRINT
}

static void reverse(Units *units) {
  tokensReverse(&units->tokens);
  numbersReverse(&units->numbers);
}

static MemoryResult pushNumber(Units *units, NUMBER number) {
  MemoryResult result;
  return MEMORY_OK != (result = unitsPushToken(units, TOKEN_NUMBER))
             ? result
             : unitsPushNumber(units, number);
}

static TranslateResult intoTranslateResult(MemoryResult result) {
  switch (result) {
  case MEMORY_OK:
    return TRANSLATE_OK;
  case MEMORY_NO_MEMORY:
    return TRANSLATE_NO_MEMORY;
  }
}

static TranslateResult siftLeftByPrecedence(TokenPrecendence precedence,
                                            Units *output, Units *stack) {
  Token token;
  NUMBER number;
  while (unitsPeek(stack, 0, &token, &number)) {
    if (TOKEN_NUMBER == token)
      return TRANSLATE_UNEXPECTED;
    TokenType type = tokenType(token);
    if (TOKEN_TYPE_UNARY == type ||
        (TOKEN_TYPE_PAREN != type && tokenPrecedence(token) >= precedence)) {
      unitsPop(stack, &token, &number);
      MemoryResult result;
      if (MEMORY_OK != (result = unitsPushToken(output, token)))
        return intoTranslateResult(result);
    } else
      break;
  }
  return TRANSLATE_OK;
}

static TranslateResult siftLeftByParen(Units *output, Units *stack) {
  Token token;
  NUMBER number;
  while (unitsPop(stack, &token, &number)) {
    if (TOKEN_NUMBER == token)
      return TRANSLATE_UNEXPECTED;
    if (TOKEN_LEFT_PAREN == token)
      return TRANSLATE_OK;
    MemoryResult result;
    if (MEMORY_OK != (result = unitsPushToken(output, token)))
      return intoTranslateResult(result);
  }
  return TRANSLATE_INCONSISTENT;
}

static TranslateResult translate(Token token, Units *output, Units *stack) {
  MemoryResult mResult = MEMORY_OK;
  switch (tokenType(token)) {
  case TOKEN_TYPE_VALUE:
    mResult = unitsPushToken(output, token);
    break;
  case TOKEN_TYPE_UNARY:
    mResult = unitsPushToken(stack, token);
    break;
  case TOKEN_TYPE_BINARY: {
    TranslateResult tResult;
    if (TRANSLATE_OK !=
        (tResult = siftLeftByPrecedence(tokenPrecedence(token), output, stack)))
      return tResult;
    mResult = unitsPushToken(stack, token);
    break;
  }
  default:
    if (TOKEN_LEFT_PAREN == token) {
      mResult = unitsPushToken(stack, token);
    } else if (TOKEN_RIGHT_PAREN == token) {
      return siftLeftByParen(output, stack);
    } else
      return TRANSLATE_INCONSISTENT;
  }
  return MEMORY_OK != mResult ? intoTranslateResult(mResult) : TRANSLATE_OK;
}

static MemoryResult extend(Units *dest, Units *src) {
  Token token;
  NUMBER number;
  MemoryResult result;
  while (unitsPop(src, &token, &number)) {
    if (MEMORY_OK != (result = unitsPushToken(dest, token)))
      return result;
    if (TOKEN_NUMBER == token && MEMORY_OK != unitsPushNumber(dest, number))
      return result;
  }
  return MEMORY_OK;
}

TranslateResult unitsTranslate(Units *units) {
  Units output = unitsNew();
  Units stack = unitsNew();
  reverse(units);
  Token token;
  NUMBER number;
  while (unitsPop(units, &token, &number)) {
    if (TOKEN_NUMBER == token) {
      MemoryResult result;
      if (MEMORY_OK != (result = pushNumber(&output, number))) {
        unitsReset(&output);
        unitsReset(&stack);
        return intoTranslateResult(result);
      }
      continue;
    }
    TranslateResult result;
    if (TRANSLATE_OK != (result = translate(token, &output, &stack))) {
      unitsReset(&output);
      unitsReset(&stack);
      return result;
    }
  }
  extend(&output, &stack);
  unitsReset(&stack);
  unitsReset(units);
  *units = output;
  return TRANSLATE_OK;
}

static EvaluateResult intoEvaluateResult(MemoryResult result) {
  switch (result) {
  case MEMORY_OK:
    return EVALUATE_OK;
  case MEMORY_NO_MEMORY:
    return EVALUATE_NO_MEMORY;
  }
}

static EvaluateResult evaluateBinary(NUMBER (*op)(NUMBER, NUMBER),
                                     Units *stack) {
  Token tokens[2];
  NUMBER numbers[2];
  for (unsigned char i = 0; i < 2; i++) {
    if (!unitsPop(stack, tokens + i, numbers + i))
      return EVALUATE_EMPTY_STACK;
    if (TOKEN_NUMBER != tokens[i])
      return EVALUATE_UNEXPECTED;
  }
  NUMBER result = op(numbers[1], numbers[0]);
  return isnan(result) ? EVALUATE_NAN
                       : intoEvaluateResult(pushNumber(stack, result));
}

static NUMBER add(NUMBER a, NUMBER b) { return a + b; }

static NUMBER sub(NUMBER a, NUMBER b) { return a - b; }

static NUMBER mul(NUMBER a, NUMBER b) { return a * b; }

static NUMBER divide(NUMBER a, NUMBER b) { return a / b; }

static EvaluateResult evaluateUnary(NUMBER (*op)(NUMBER), Units *stack) {
  Token token;
  NUMBER number;
  if (!unitsPop(stack, &token, &number))
    return EVALUATE_EMPTY_STACK;
  if (TOKEN_NUMBER != token)
    return EVALUATE_UNEXPECTED;
  number = op(number);
  return isnan(number) ? EVALUATE_NAN
                       : intoEvaluateResult(pushNumber(stack, number));
}

static NUMBER negate(NUMBER number) { return -number; };

static NUMBER ctg(NUMBER number) {
#define CTG _Generic((number), double: catan, float: catanf)
  return CTG(number);
#undef CTG
}

EvaluateResult unitsEvaluate(const Units *units, NUMBER value, NUMBER *result) {
#define SIN _Generic((number), double: sin, float: sinf)
#define COS _Generic((number), double: cos, float: cosf)
#define TAN _Generic((number), double: tan, float: tanf)
#define SQRT _Generic((number), double: sqrt, float: sqrtf)
#define LN _Generic((number), double: log, float: logf)
  Units stack = unitsNew();
  size_t index = 0;
  Token token;
  NUMBER number;
  MemoryResult mResult;
  EvaluateResult eResult;
  while (unitsGet(units, index++, &token, &number)) {
    switch (token) {
    case TOKEN_ADD:
      if (EVALUATE_OK != (eResult = evaluateBinary(add, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_SUB:
      if (EVALUATE_OK != (eResult = evaluateBinary(sub, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_MUL:
      if (EVALUATE_OK != (eResult = evaluateBinary(mul, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_DIV:
      if (EVALUATE_OK != (eResult = evaluateBinary(divide, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_NEG:
      if (EVALUATE_OK != (eResult = evaluateUnary(negate, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_SIN:
      if (EVALUATE_OK != (eResult = evaluateUnary(SIN, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_COS:
      if (EVALUATE_OK != (eResult = evaluateUnary(COS, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_TG:
      if (EVALUATE_OK != (eResult = evaluateUnary(TAN, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_CTG:
      if (EVALUATE_OK != (eResult = evaluateUnary(ctg, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_SQRT:
      if (EVALUATE_OK != (eResult = evaluateUnary(SQRT, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_LN:
      if (EVALUATE_OK != (eResult = evaluateUnary(LN, &stack)))
        return (unitsReset(&stack), eResult);
      break;
    case TOKEN_NUMBER:
      if (MEMORY_OK != (mResult = pushNumber(&stack, number)))
        return (unitsReset(&stack), intoEvaluateResult(mResult));
      break;
    case TOKEN_X:
      if (MEMORY_OK != (mResult = pushNumber(&stack, value)))
        return (unitsReset(&stack), intoEvaluateResult(mResult));
      break;
    default:
      return (unitsReset(&stack), EVALUATE_UNEXPECTED);
    }
  }
  bool isSome = unitsPop(&stack, &token, result);
  unitsReset(&stack);
  return isSome ? EVALUATE_OK : EVALUATE_EMPTY_STACK;
#undef SIN
#undef COS
#undef TAN
#undef SQRT
#undef LN
}
