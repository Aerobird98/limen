#ifndef limen_h
#define limen_h

#include <stdlib.h>

#ifdef DEBUG
    #include <stdio.h>
#endif

#define MEMORY_MAX           65535
#define ARRAY_CAPACITY_MAX   32767
#define ARRAY_COUNT_MAX      30000
#define ARRAY_GROW_THRESHOLD 8
#define ARRAY_GROW_FACTOR    2

// A generic allocation function that handles all explicit memory management.
//
// It's used like so:
//
// - To allocate new memory, <memory> is NULL and <was> is zero. It should
//   return the allocated memory or NULL on failure.
//
// - To attempt to grow an existing allocation, <memory> is the memory,
//   <was> is its previous size, and <will> is the desired size.
//   It should return <memory> if it was able to grow it in place, or a new
//   memory if it had to move it.
//
// - To shrink memory, <memory>, <was>, and <will> are the same as above
//   but it will always return <memory>.
//
// - To free memory, <memory> will be the memory to free and <will> and
//   <was> will be zero. It should return NULL.
//
// NOTE: Use provided MACROS.
void *reallocate(void *memory, size_t was, size_t will);

#define ALLOCATE(type)                 (type *)reallocate(NULL, 0, sizeof(type))
#define GROW(type, memory, will)       (type *)realocate(memory, sizeof(type), will)
#define SRINK(type, memory, will)      GROW(type, memory, will)
#define FREE(type, memory)             reallocate(memory, sizeof(type), 0)
#define ALLOCATE_ARRAY(type, capacity) (type *)reallocate(NULL, 0, sizeof(type) * (capacity))
#define GROW_CAPACITY(capacity) \
    ((capacity) < ARRAY_GROW_THRESHOLD ? ARRAY_GROW_THRESHOLD : (capacity) * (ARRAY_GROW_FACTOR))
#define GROW_ARRAY(type, memory, wasCapacity, capacity) \
    (type *)reallocate(memory, sizeof(type) * (wasCapacity), sizeof(type) * (capacity))
#define SRINK_ARRAY(type, memory, wasCapacity, capacity) \
    GROW_ARRAY(type, memory, wasCapacity, capacity)
#define FREE_ARRAY(type, memory, capacity) reallocate(memory, sizeof(type) * (capacity), 0)

// A dynamic unsigned char array implementation that uses the generic allocation
// function trough a series of MACROS to grow when new values are written to it.
//
// NOTE: This implementation could run into issues if the count or capacity value
//       overflows an int. It sould be quite rare, yet consider limiting capacity
//       and count accounting for the occasional null byte at the end.
typedef struct sCharArray {
    int count;
    int capacity;
    unsigned char *values;
} CharArray;

void initCharArray(CharArray *array);
void writeCharArray(CharArray *array, unsigned char value);
void freeCharArray(CharArray *array);

// The state contains useful informations about the state of the interpreter.
//
// This sould only be modified trough the provided library functions. Otherwise
// should be read-only!
//
// NOTE: It would be wise to define a limit for any given state in terms of
//       memory usage. A reasonable maximum could be defined trough usage tests.
typedef struct sState {
    CharArray instructions;  // Validated instructions read from user code.
    CharArray stream;        // Stream for the instructions to operate on.

    unsigned char *ip;  // Pointer pointing at the current instruction.
    unsigned char *sp;  // Pointer pointing at the current value on the stream.

    int ipc;  // Counts where the instruction pointer is.
    int spc;  // Counts where the stream pointer is.

    int brackets;  // Open brackets count for error checks and loop management.

    CharArray response;  // Response of the validated instructions.
} State;

void initState(State *state);
void freeState(State *state);

typedef enum eEvalResult {
    EVAL_OK,
    EVAL_MISMATCHED_BRACKETS,
    EVAL_ERROR_UNKNOWN_CHARACTER,
    EVAL_ERROR_STREAM_UNDERFLOW,
    EVAL_ERROR_UNKNOWN,
} EvalResult;

// Evaluate provided user code into a response stored in state.
EvalResult eval(State *state, const unsigned char *code);

#endif
