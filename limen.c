#include "limen.h"

void *reallocate(void *memory, size_t was, size_t will) {
    // The behavior of realloc() when the size is 0 is implementation defined. It
    // may return a non-NULL pointer which must not be dereferenced but nevertheless
    // should be freed. To prevent that, we avoid calling realloc() with a zero size.
    if (will == 0) {
        free(memory);
        return NULL;
    }

    return realloc(memory, will);
}

void initCharArray(CharArray *array) {
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
}

void writeCharArray(CharArray *array, unsigned char value) {
    if (array->capacity < array->count + 1) {
        int wasCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(wasCapacity);

        array->values = GROW_ARRAY(unsigned char, array->values, wasCapacity, array->capacity);
    }

    array->values[array->count++] = value;
}

void freeCharArray(CharArray *array) {
    FREE_ARRAY(unsigned char, array->values, array->capacity);
    initCharArray(array);
}

void initState(State *state) {
    initCharArray(&state->instructions);
    initCharArray(&state->stream);

    state->ip = NULL;
    state->sp = NULL;

    state->ipc = 0;
    state->spc = 0;

    state->brackets = 0;

    initCharArray(&state->response);
}

void freeState(State *state) {
    freeCharArray(&state->instructions);
    freeCharArray(&state->stream);

    FREE(unsigned char, state->ip);
    FREE(unsigned char, state->sp);

    state->ipc = 0;
    state->spc = 0;

    state->brackets = 0;

    freeCharArray(&state->response);
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
    // Compile time
    //
    // At this phase we parse user code into valid instructions. Effectively Compiling
    // code into a run time format.
    while (*code != '\0') {
        if (*code <= 127) {
            if (*code >= 32) {
                // Skip every character besides the eight instructions.
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
            }

        } else {
            // Error out on none ASCII characters.
            return EVAL_ERROR_UNKNOWN_CHARACTER;
        }

        // Step one character.
        code++;
    }

    // Terminate the instructions array with a NULL character.
    writeCharArray(&state->instructions, '\0');

    // Grow the stream by one.
    writeCharArray(&state->stream, '\0');

    // Set IP to point at the first instruction.
    state->ip = &state->instructions.values[0];
    // Set SP to point at the first value on the stream.
    state->sp = &state->stream.values[0];

    // If there are open brackets.
    if (state->brackets != 0) {
        // Set the counter back to zero.
        state->brackets = 0;

        // Error out.
        return EVAL_MISMATCHED_BRACKETS;
    }

    // Run time
    //
    // At this phase we run valid instructions evaluating them into a response.
    // Effectively manipulating the stream and altering state.
    while (*state->ip != '\0') {
#if DEBUG >= 1
        debugPrintInstructions(state);
#endif

        switch (*state->ip) {
            // Increment the value at the stream pointer. Ensure that the value wraps
            // around to 0 after reaching 127.
            case '+': {
                *state->sp = (*state->sp + 1) & 127;
                break;
            }
            // Decrement the value at the stream pointer. Ensure that the value wraps
            // around to 127 after reaching 0.
            case '-': {
                *state->sp = (*state->sp - 1) & 127;
                break;
            }
            // Moves the stream pointer to the next value.
            case '>': {
                state->spc++;

                if (state->stream.count <= state->spc) {
                    writeCharArray(&state->stream, '\0');
                }

                state->sp++;
                break;
            }
            // Moves the stream pointer to the previous value.
            case '<': {
                state->spc--;

                if (0 > state->spc) {
                    state->spc++;
                    return EVAL_ERROR_STACK_UNDERFLOW;
                }

                state->sp--;
                break;
            }
            case '.': {
                writeCharArray(&state->response, *state->sp);
                break;
            }
            case ',': {
                // TODO: Implement this properly.
                //
                // As for the time being, this instruction is not as widely used,
                // so its absence will not be a problem.
                //
                // I could think of two possible ways to tackle this:
                //
                //- Use an <in> CharArray, like <response> which could be initialized
                //  trough eval itself like the users code. It should be optional
                //  tough. It require us to provide the input alongside user code
                //  in the exact same order of the , instructions found in the code.
                //
                //- Just consume the next instruction after this one. It goes well
                //  with the REPL.
                //
                // NOTE: We have to use a CharArray because we need to validate input
                //      as well as user code. Consuming the next instruction however
                //      as an input character is fine as long as we validate it first
                //      at compile time. In both approaches we also need to account for
                //      error handling:
                //
                //      - What should happen when <in> is empty or do not contain that
                //        many characters when we evaluate an , instruction?
                //
                //      - What happens when , is the last valid instruction found in the
                //        users code? Consume the NULL character? No, that would be
                //        horrible! if the next character is the NULL one, that should be
                //        an error which could be detected at compile time. We should not
                //        worry about it at run time!
                break;
            }
            case '[': {
                if (*state->sp == 0) {
                    state->ipc++;
                    state->ip++;

                    state->brackets++;
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
                    state->ipc--;
                    state->ip--;

                    state->brackets++;
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

#if DEBUG >= 1
        debugPrintStream(state);
#endif

        state->ipc++;
        state->ip++;
    }

    // Terminate the response array with a NULL character.
    writeCharArray(&state->response, '\0');

#if DEBUG >= 1
    debugPrintInstructions(state);
    debugPrintStream(state);
#endif

    return EVAL_OK;
}
