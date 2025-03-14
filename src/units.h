#ifndef GRAPTH_UNITS_H
#define GRAPTH_UNITS_H

#include "memory.h"
#include "numbers.h"
#include "tokens.h"

typedef struct {
  Tokens tokens;
  Numbers numbers;
} Units;

Units unitsNew();
MemoryResult unitsPushToken(Units *, Token);
MemoryResult unitsPushNumber(Units *, NUMBER);
bool unitsPop(Units *, Token *, NUMBER *);
bool unitsGet(const Units *, size_t, Token *, NUMBER *);
bool unitsPeek(const Units *, size_t, Token *, NUMBER *);
void unitsReset(Units *);

void unitsPrint(const Units *);

#endif
