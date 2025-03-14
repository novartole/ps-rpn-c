#ifndef GRAPTH_TOKEN_H
#define GRAPTH_TOKEN_H

#include "numbers.h"
#include <stdbool.h>

typedef enum {
  TOKEN_NEG,
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_SIN,
  TOKEN_COS,
  TOKEN_TG,
  TOKEN_CTG,
  TOKEN_SQRT,
  TOKEN_LN,
  TOKEN_NUMBER,
  TOKEN_X
} Token;

typedef enum {
  PREC_NONE,   // number, x
  PREC_TERM,   // +, -
  PREC_FACTOR, // *, /
  PREC_UNARY,  // -, cos, ctg, ln, sin, sqrt
  PREC_CALL    // (, )
} TokenPrecendence;

typedef enum {
  TOKEN_TYPE_VALUE,
  TOKEN_TYPE_UNARY,
  TOKEN_TYPE_PAREN,
  TOKEN_TYPE_BINARY,
} TokenType;

DEFINE_STACK(Token, Tokens)

Tokens tokensNew();
MemoryResult tokensPush(Tokens *, Token);
bool tokensPop(Tokens *, Token *);
bool tokensGet(const Tokens *, size_t, Token *);
bool tokensPeek(const Tokens *, size_t, Token *, size_t *);
void tokensReverse(Tokens *);
void tokensReset(Tokens *);

TokenPrecendence tokenPrecedence(Token);
TokenType tokenType(Token);
const char *tokenToString(Token);

#endif
