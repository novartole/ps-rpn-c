#ifndef GRAPTH_MEMORY_H
#define GRAPTH_MEMORY_H

#include <stddef.h>

typedef enum {
  MEMORY_OK,
  MEMORY_NO_MEMORY,
} MemoryResult;

#define DEFAULT_CAPACITY 8
#define GROW_CAPACITY(length)                                                  \
  ((length < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : (2 * length))
#define STACK_PUSH(stack, Type, value)                                         \
  do {                                                                         \
    if ((stack)->length == (stack)->capacity) {                                \
      Type *values = (stack)->values;                                          \
      size_t capacity = GROW_CAPACITY((stack)->capacity);                      \
      if (NULL == (stack)->values) {                                           \
        values = malloc(capacity * sizeof(Type));                              \
      } else {                                                                 \
        values = realloc((stack)->values, capacity * sizeof(Type));            \
      }                                                                        \
      if (NULL != values) {                                                    \
        (stack)->values = values;                                              \
        (stack)->capacity = capacity;                                          \
      }                                                                        \
    }                                                                          \
    if (NULL != (stack)->values) {                                             \
      (stack)->values[(stack)->length++] = value;                              \
    }                                                                          \
  } while (0);
#define STACK_POP(stack, value)                                                \
  do {                                                                         \
    if (0 == (stack)->length) {                                                \
      (value) = NULL;                                                          \
    } else {                                                                   \
      *(value) = (stack)->values[--(stack)->length];                           \
    }                                                                          \
  } while (0);
#define STACK_REVERSE(Type, stack)                                             \
  do {                                                                         \
    size_t length;                                                             \
    if ((length = (stack)->length) > 1) {                                      \
      Type temp;                                                               \
      size_t start = 0;                                                        \
      size_t end = length - 1;                                                 \
      while (start < end) {                                                    \
        temp = (stack)->values[start];                                         \
        (stack)->values[start] = (stack)->values[end];                         \
        (stack)->values[end] = temp;                                           \
        start += 1;                                                            \
        end -= 1;                                                              \
      }                                                                        \
    }                                                                          \
  } while (0);

#define DEFINE_STACK(T, N)                                                     \
  typedef struct {                                                             \
    T *values;                                                                 \
    size_t length;                                                             \
    size_t capacity;                                                           \
  } N;

#endif
