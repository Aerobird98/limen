#ifndef limen_h
#define limen_h

#include <stdlib.h>

#ifdef DEBUG
    #include <stdio.h>
#endif

#define MEMORY_MAX           65535
#define ARRAY_CAPACITY_MAX   65535
#define ARRAY_COUNT_MAX      65535
#define ARRAY_GROW_THRESHOLD 8
#define ARRAY_GROW_FACTOR    2

/* A generic allocation function that handles all explicit memory management.
 * It's used like so:
 *
 * - To allocate new memory, [memory] is NULL and [oldSize] is zero. It should
 *   return the allocated memory or NULL on failure.
 *
 * - To attempt to grow an existing allocation, [memory] is the memory,
 *   [oldSize] is its previous size, and [newSize] is the desired size.
 *   It should return [memory] if it was able to grow it in place, or a new
 *   pointer if it had to move it.
 *
 * - To shrink memory, [memory], [oldSize], and [newSize] are the same as above
 *   but it will always return [memory].
 *
 * - To free memory, [memory] will be the memory to free and [newSize] and
 *   [oldSize] will be zero. It should return NULL.
 */
void *reallocate(void *pointer, size_t oldSize, size_t newSize);

#define ALLOCATE(type)                             (type *)reallocate(NULL, 0, sizeof(type))
#define FREE(type, pointer)                        reallocate(pointer, sizeof(type), 0)
#define ALLOCATE_ARRAY(type, count)                (type *)reallocate(NULL, 0, sizeof(type) * (count))
#define GROW_CAPACITY(capacity)                    ((capacity) < ARRAY_GROW_THRESHOLD ? ARRAY_GROW_THRESHOLD : (capacity) * (ARRAY_GROW_FACTOR))
#define GROW_ARRAY(type, pointer, oldCount, count) (type *)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (count))
#define FREE_ARRAY(type, pointer, oldCount)        reallocate(pointer, sizeof(type) * (oldCount), 0)

/* A dynamic unsigned char array implementation that uses the generic allocation
 * function trough a series of macros to grow when new values are written to it.
 *
 * NOTE: This implementation could run into issues if the count or capacity value
 *       overflows an int. It sould be quite rare, yet consider limiting capacity
 *       and count accounting for the occasional null byte at the end.
 *
 *       It would be wise to define a limit for the interpreter in terms of memory
 *       usage. It could be defined trough common usage tests.
 */
typedef struct sCharArray {
    int count;
    int capacity;
    unsigned char *values;
} CharArray;

void initCharArray(CharArray *array);
void writeCharArray(CharArray *array, unsigned char value);
void freeCharArray(CharArray *array);

typedef struct sState {
    CharArray instructions;  // Valid instructions read from user code.
    CharArray stream;        // Stream for the instructions to operate on.

    unsigned char *ip;  // Pointer pointing at the current instruction.
    unsigned char *sp;  // Pointer pointing at the current value on the stream.

    int ipc;  // Counts where the instruction pointer is.
    int spc;  // Counts where the stream pointer is.

    int brackets;  // Open brackets count, used for error checks and loop management.

    CharArray out;  // Evaluated code are stored in <out>.
} State;

void initState(State *state);
void freeState(State *state);

typedef enum eEvalResult {
    EVAL_OK,
    EVAL_MISMATCHED_BRACKETS,
    EVAL_ERROR_UNKNOWN_CHARACTER,
    EVAL_ERROR_STACK_UNDERFLOW,
    EVAL_ERROR_UNKNOWN,
} EvalResult;

/* Evaluate provided user code into <out>. */
EvalResult eval(State *state, const unsigned char *code);

#endif
