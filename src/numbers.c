#include "numbers.h"
#include <stdlib.h>

Numbers numbersNew() {
  Numbers numbers;
  numbers.values = NULL;
  numbers.length = 0;
  numbers.capacity = 0;
  return numbers;
}

MemoryResult numbersPush(Numbers *numbers, NUMBER number) {
  STACK_PUSH(numbers, NUMBER, number);
  return NULL == numbers->values ? MEMORY_NO_MEMORY : MEMORY_OK;
}

bool numbersPop(Numbers *numbers, NUMBER *number) {
  STACK_POP(numbers, number);
  return NULL != number;
}

bool numbersGet(const Numbers *numbers, size_t index, NUMBER *number) {
  if (index >= numbers->length)
    return false;
  *number = numbers->values[index];
  return true;
}

void numbersReverse(Numbers *numbers) { STACK_REVERSE(NUMBER, numbers); }

void numbersReset(Numbers *numbers) {
  if (NULL == numbers)
    return;
  if (NULL != numbers->values)
    free(numbers->values);
  numbers->length = numbers->capacity = 0;
}
