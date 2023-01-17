#include <stdlib.h>

#ifdef DEBUG
    #include <stdio.h>
#endif

#define ARRAY_MAX_CAPACITY 8
#define ARRAY_GROW_FACTOR  2

void *reallocate(void *pointer, size_t oldSize, size_t newSize);

#define ALLOCATE(type)                             (type *)reallocate(NULL, 0, sizeof(type))
#define FREE(type, pointer)                        reallocate(pointer, sizeof(type), 0)
#define ALLOCATE_ARRAY(type, count)                (type *)reallocate(NULL, 0, sizeof(type) * (count))
#define GROW_CAPACITY(capacity)                    ((capacity) < ARRAY_MAX_CAPACITY ? ARRAY_MAX_CAPACITY : (capacity) * (ARRAY_GROW_FACTOR))
#define GROW_ARRAY(type, pointer, oldCount, count) (type *)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (count))
#define FREE_ARRAY(type, pointer, oldCount)        reallocate(pointer, sizeof(type) * (oldCount), 0)

/* A dynamic unsigned char array implementation
 * that grows when new values are written to it.
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
    CharArray instructions;  // Valid Instructions read from user code.
    CharArray stream;        // Stream for the Instructions to operate on.

    unsigned char *ip;  // Pointer pointing at the current Instruction.
    unsigned char *sp;  // Pointer pointing at the current Value on the stream.

    int pc;        // Counts where the Stream Pointer is.
    int brackets;  // Counts open brackets, used for both error checks and loop management.

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

// Evaluate provided user code into <out>.
EvalResult eval(State *state, const unsigned char *code);
