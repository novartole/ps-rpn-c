#ifndef GRAPTH_STRING_H
#define GRAPTH_STRING_H

#include "memory.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

DEFINE_STACK(char, String)

String stringNew();
MemoryResult stringRead(String *, FILE *);
MemoryResult stringFromCString(String *, const char *);
const char *stringAsCString(const String *);
void stringReset(String *);

void stringPrint(const String *);

#endif
