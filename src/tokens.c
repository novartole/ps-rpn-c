#include "tokens.h"
#include <stdlib.h>

Tokens tokensNew() {
  Tokens tokens;
  tokens.values = NULL;
  tokens.length = 0;
  tokens.capacity = 0;
  return tokens;
}

MemoryResult tokensPush(Tokens *tokens, Token token) {
  STACK_PUSH(tokens, Token, token);
  return NULL == tokens->values ? MEMORY_NO_MEMORY : MEMORY_OK;
}

bool tokensPop(Tokens *tokens, Token *token) {
  STACK_POP(tokens, token);
  return NULL != token;
}

bool tokensGet(const Tokens *tokens, size_t index, Token *token) {
  if (index >= tokens->length)
    return false;
  *token = tokens->values[index];
  return true;
}

bool tokensPeek(const Tokens *tokens, size_t offset, Token *token,
                size_t *index) {
  if (offset >= tokens->length)
    return false;
  *index = tokens->length - 1 - offset;
  return tokensGet(tokens, *index, token);
}

void tokensReverse(Tokens *tokens) { STACK_REVERSE(Token, tokens); }

void tokensReset(Tokens *tokens) {
  if (NULL == tokens)
    return;
  if (NULL != tokens->values)
    free(tokens->values);
  tokens->length = tokens->capacity = 0;
}

TokenPrecendence tokenPrecedence(Token token) {
  switch (token) {
  case TOKEN_NUMBER:
  case TOKEN_X:
    return PREC_NONE;
  case TOKEN_ADD:
  case TOKEN_SUB:
    return PREC_TERM;
  case TOKEN_MUL:
  case TOKEN_DIV:
    return PREC_FACTOR;
  case TOKEN_COS:
  case TOKEN_CTG:
  case TOKEN_LN:
  case TOKEN_NEG:
  case TOKEN_SIN:
  case TOKEN_SQRT:
  case TOKEN_TG:
    return PREC_UNARY;
  case TOKEN_LEFT_PAREN:
  case TOKEN_RIGHT_PAREN:
    return PREC_CALL;
  }
}

TokenType tokenType(Token token) {
  switch (token) {
  case TOKEN_NUMBER:
  case TOKEN_X:
    return TOKEN_TYPE_VALUE;
  case TOKEN_COS:
  case TOKEN_CTG:
  case TOKEN_LN:
  case TOKEN_NEG:
  case TOKEN_SIN:
  case TOKEN_SQRT:
  case TOKEN_TG:
    return TOKEN_TYPE_UNARY;
  case TOKEN_LEFT_PAREN:
  case TOKEN_RIGHT_PAREN:
    return TOKEN_TYPE_PAREN;
  case TOKEN_ADD:
  case TOKEN_SUB:
  case TOKEN_MUL:
  case TOKEN_DIV:
    return TOKEN_TYPE_BINARY;
  }
}

const char *tokenToString(Token token) {
  switch (token) {
  case TOKEN_NEG:
    return "-";
  case TOKEN_ADD:
    return "+";
  case TOKEN_SUB:
    return "--";
  case TOKEN_MUL:
    return "*";
  case TOKEN_DIV:
    return "/";
  case TOKEN_LEFT_PAREN:
    return "(";
  case TOKEN_RIGHT_PAREN:
    return ")";
  case TOKEN_SIN:
    return "sin";
  case TOKEN_COS:
    return "cos";
  case TOKEN_TG:
    return "tg";
  case TOKEN_CTG:
    return "ctg";
  case TOKEN_SQRT:
    return "sqrt";
  case TOKEN_LN:
    return "ln";
  case TOKEN_X:
    return "x";
  default:
    return "";
  }
}
