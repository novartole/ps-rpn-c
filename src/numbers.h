#ifndef GRAPTH_NUMBERS_H
#define GRAPTH_NUMBERS_H

#include "memory.h"
#include <stdbool.h>

#define NUMBER double

DEFINE_STACK(NUMBER, Numbers)

Numbers numbersNew();
MemoryResult numbersPush(Numbers *, NUMBER);
bool numbersPop(Numbers *, NUMBER *);
bool numbersGet(const Numbers *, size_t, NUMBER *);
void numbersReverse(Numbers *);
void numbersReset(Numbers *);

#endif
