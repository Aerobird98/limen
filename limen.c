#include "limen.h"

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    return realloc(pointer, newSize);
}

void initCharArray(CharArray *array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void writeCharArray(CharArray *array, unsigned char value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);

        array->values = GROW_ARRAY(unsigned char, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeCharArray(CharArray *array) {
    FREE_ARRAY(unsigned char, array->values, array->capacity);
    initCharArray(array);
}

void initState(State *state) {
    initCharArray(&state->instructions);
    initCharArray(&state->stream);

    writeCharArray(&state->stream, '\0');

    state->ip = NULL;
    state->sp = NULL;

    state->pc = 0;
    state->brackets = 0;

    initCharArray(&state->out);
}

void freeState(State *state) {
    freeCharArray(&state->instructions);
    freeCharArray(&state->stream);

    FREE(unsigned char, state->ip);
    FREE(unsigned char, state->sp);

    state->pc = 0;
    state->brackets = 0;

    freeCharArray(&state->out);
}

#if DEBUG > 0
static void debugPrintStream(State *state) {
    printf("%i %i ", state->stream.count, state->stream.capacity);
    for (int i = 0; i < state->stream.count; i++) {
        if (i == state->pc)
            printf("[*%d]", state->stream.values[i]);
        else
            printf("[%d]", state->stream.values[i]);
    }
    printf("\n");
}
#endif

EvalResult eval(State *state, const unsigned char *code) {
    while (*code != '\0') {
        if (*code - 127 <= 0) {
            if (*code - 32 >= 0)
                switch (*code) {
                    case '+': {
                        writeCharArray(&state->instructions, '+');
                        break;
                    }
                    case '-': {
                        writeCharArray(&state->instructions, '-');
                        break;
                    }
                    case '>': {
                        writeCharArray(&state->instructions, '>');
                        break;
                    }
                    case '<': {
                        writeCharArray(&state->instructions, '<');
                        break;
                    }
                    case '.': {
                        writeCharArray(&state->instructions, '.');
                        break;
                    }
                    case ',': {
                        writeCharArray(&state->instructions, ',');
                        break;
                    }
                    case '[': {
                        state->brackets++;
                        writeCharArray(&state->instructions, '[');
                        break;
                    }
                    case ']': {
                        state->brackets--;
                        writeCharArray(&state->instructions, ']');
                        break;
                    }
                }

        } else
            return EVAL_ERROR_UNKNOWN_CHARACTER;

        code++;
    }

    if (state->brackets != 0) {
        return EVAL_MISMATCHED_BRACKETS;
    }

    writeCharArray(&state->instructions, '\0');

    state->ip = &state->instructions.values[0];
    state->sp = &state->stream.values[0];

    /*
    for (unsigned char ch = *state->ip; ch != '\0'; state->ip++) {
    }
    */

    unsigned char ch;
    while ((ch = *state->ip++) != '\0') {
        switch (ch) {
            case '+': {
                *state->sp = (*state->sp + 1) & 127;
                break;
            }
            case '-': {
                *state->sp = (*state->sp - 1) & 127;
                break;
            }
            case '>': {
                state->pc++;
                if (state->stream.count <= state->pc) {
                    writeCharArray(&state->stream, '\0');
                }

                state->sp++;
                break;
            }
            case '<': {
                state->pc--;
                if (0 > state->pc) {
                    return EVAL_ERROR_STACK_UNDERFLOW;
                }

                state->sp--;
                break;
            }
            case '.': {
                writeCharArray(&state->out, *state->sp);
                break;
            }
            case ',': {
                /* TODO: Implement this properly.
                 * As for the time being,
                 * this instruction is not as widely used,
                 * so its absence will not be a problem.
                 *
                 * I could think of two possible ways to tackle this:
                 *
                 * - Just consume the next instruction after this one.
                 * - Or use an <in> CharArray, like <out> which could be
                 *      A: initialized trough some argument to initState.
                 *      B: initialized trough the C API after state initialization.
                 *      C: initialized trough eval itself.
                 *
                 * NOTE: We have to use a CharArray because we need to validate input as well as user code.
                 *       Consuming the next instruction however as an input character is fine as long as we
                 *       validate it first at compile time.
                 */
                break;
            }
            case '[': {
                if (*state->sp == 0) {
                    state->brackets++;
                    while (state->brackets != 0) {
                        if (*state->ip == '[') {
                            state->brackets++;
                        } else if (*state->ip == ']') {
                            state->brackets--;
                        }

                        state->ip++;
                    }
                }
                break;
            }
            case ']': {
                if (*state->sp != 0) {
                    state->ip--;
                    state->ip--;
                    state->brackets++;
                    while (state->brackets != 0) {
                        if (*state->ip == ']') {
                            state->brackets++;

                        } else if (*state->ip == '[') {
                            state->brackets--;
                        }

                        state->ip--;
                    }
                    state->ip++;
                }
                break;
            }
        }

#if DEBUG >= 2
        debugPrintStream(state);
#endif
    }

    writeCharArray(&state->out, '\0');

#if DEBUG >= 1
    debugPrintStream(state);
#endif

    return EVAL_OK;
}
