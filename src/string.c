#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

String stringNew() {
  String string;
  string.values = NULL;
  string.length = 0;
  string.capacity = 0;
  return string;
}

MemoryResult stringRead(String *s, FILE *f) {
  char ch;
  while ('\n' != (ch = fgetc(f)) && !feof(f)) {
    STACK_PUSH(s, char, ch);
    if (NULL == s->values)
      return MEMORY_NO_MEMORY;
  }
  if (s->length > 0)
    STACK_PUSH(s, char, '\0');
  return NULL == s->values ? MEMORY_NO_MEMORY : MEMORY_OK;
}

MemoryResult stringFromCString(String *string, const char *s) {
  size_t size = strlen(s);
  char *allocated = malloc(size * sizeof(char));
  if (NULL == allocated)
    return MEMORY_NO_MEMORY;
  strcpy(allocated, s);
  string->values = allocated;
  string->capacity = string->length = size;
  return MEMORY_OK;
}

const char *stringAsCString(const String *s) { return s->values; }

void stringReset(String *s) {
  if (NULL == s)
    return;
  if (NULL != s->values)
    free(s->values);
  s->length = s->capacity = 0;
}

void stringPrint(const String *s) { printf("'%s'\n", s->values); }
