#include "limen.h"

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
    /* The behavior of realloc() when the size is 0 is implementation defined. It
     * may return a non-NULL pointer which must not be dereferenced but nevertheless
     * should be freed. To prevent that, we avoid calling realloc() with a zero size.
     */
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

    state->ipc = 0;
    state->spc = 0;

    state->brackets = 0;

    initCharArray(&state->out);
}

void freeState(State *state) {
    freeCharArray(&state->instructions);
    freeCharArray(&state->stream);

    FREE(unsigned char, state->ip);
    FREE(unsigned char, state->sp);

    state->ipc = 0;
    state->spc = 0;

    state->brackets = 0;

    freeCharArray(&state->out);
}

#if DEBUG > 0
static void debugPrintInstructions(State *state) {
    for (int i = 0; i < state->instructions.count; i++) {
        fprintf(stderr, "%c", state->instructions.values[i]);
    }

    fprintf(stderr, "\n");

    for (int i = 0; i < state->instructions.count; i++) {
        if (i == state->ipc) {
            fprintf(stderr, "%c", '^');
        } else {
            fprintf(stderr, "%c", '-');
        }
    }

    fprintf(stderr, "\n");
}

static void debugPrintStream(State *state) {
    fprintf(stderr, "%i %i ", state->stream.count, state->stream.capacity);
    for (int i = 0; i < state->stream.count; i++) {
        if (i == state->spc) {
            fprintf(stderr, "[*%i]", state->stream.values[i]);
        } else {
            fprintf(stderr, "[%i]", state->stream.values[i]);
        }
    }

    fprintf(stderr, "\n");
}
#endif

EvalResult eval(State *state, const unsigned char *code) {
    /* Compile time */
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

    /* Run time */
    state->ip = &state->instructions.values[0];
    state->sp = &state->stream.values[0];

    while (*state->ip != '\0') {
#if DEBUG >= 2
        debugPrintInstructions(state);
#endif

        switch (*state->ip) {
            case '+': {
                *state->sp = (*state->sp + 1) & 127;
                break;
            }
            case '-': {
                *state->sp = (*state->sp - 1) & 127;
                break;
            }
            case '>': {
                state->spc++;
                if (state->stream.count <= state->spc) {
                    writeCharArray(&state->stream, '\0');
                }

                state->sp++;
                break;
            }
            case '<': {
                state->spc--;
                if (0 > state->spc) {
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
                 *       validate it first at compile time. In both approaches we also need to account for
                 *       error handling: what should happen when <in> is empty or do not contain that many
                 *       characters when we evaluate an , instruction?
                 */
                break;
            }
            case '[': {
                if (*state->sp == 0) {
                    state->brackets++;
                    state->ipc++;
                    state->ip++;
                    while (state->brackets != 0) {
                        if (*state->ip == '[') {
                            state->brackets++;
                        } else if (*state->ip == ']') {
                            state->brackets--;
                        }

                        state->ipc++;
                        state->ip++;
                    }
                }
                break;
            }
            case ']': {
                if (*state->sp != 0) {
                    state->brackets++;
                    state->ipc--;
                    state->ip--;
                    while (state->brackets != 0) {
                        if (*state->ip == ']') {
                            state->brackets++;

                        } else if (*state->ip == '[') {
                            state->brackets--;
                        }

                        state->ipc--;
                        state->ip--;
                    }
                }
                break;
            }
        }

#if DEBUG >= 2
        debugPrintStream(state);
#endif

        state->ipc++;
        state->ip++;
    }

    writeCharArray(&state->out, '\0');

#if DEBUG >= 1
    debugPrintInstructions(state);
    debugPrintStream(state);
#endif

    return EVAL_OK;
}
