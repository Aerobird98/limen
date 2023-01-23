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
    fprintf(stderr, "%i %i ", state->instructions.count, state->instructions.capacity);
    for (int i = 0; i < state->instructions.count; i++) {
        fprintf(stderr, "%c", state->instructions.values[i]);
    }

    fprintf(stderr, "\n");

    fprintf(stderr, "%i %i ", state->instructions.count, state->instructions.capacity);
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

static void debugPrintResponse(State *state) {
    fprintf(stderr, "%i %i ", state->response.count, state->response.capacity);
    for (int i = 0; i < state->response.count; i++) {
        fprintf(stderr, "%c", state->response.values[i]);
    }

    fprintf(stderr, "\n");
}
#endif

EvalResult eval(State *state, const unsigned char *code) {
    // Compile time.
    //
    // At this phase we parse user code into valid instructions. Effectively Compiling
    // code into a safe run time format.
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

    // Terminate the instructions array by writing a NULL character.
    writeCharArray(&state->instructions, '\0');

    // Grow the stream by writing a Null character.
    writeCharArray(&state->stream, '\0');

    // Set IP to point at the first instruction.
    state->ip = &state->instructions.values[0];
    // Set SP to point at the first value on the stream.
    state->sp = &state->stream.values[0];

    // If there are open brackets.
    if (state->brackets != 0) {
        // Set the bracket counter back to zero.
        state->brackets = 0;

        // Error out.
        return EVAL_MISMATCHED_BRACKETS;
    }

    // Run time.
    //
    // At this phase we run valid instructions evaluating them into a response.
    // Effectively manipulating the stream and altering state.
    while (*state->ip != '\0') {
#if DEBUG >= 1
        debugPrintInstructions(state);
#endif

        // Based on the current instruction.
        switch (*state->ip) {
            // Increment the value at the stream pointer.
            case '+': {
                // Ensure that the value wraps around to zero after reaching 127.
                *state->sp = (*state->sp + 1) & 127;
                break;
            }
            // Decrement the value at the stream pointer.
            case '-': {
                // Ensure that the value wraps around to 127 after reaching zero.
                *state->sp = (*state->sp - 1) & 127;
                break;
            }
            // Move the stream pointer to the next value.
            case '>': {
                // Increment the stream counter.
                state->spc++;

                // If we moved outside the stream.
                if (state->stream.count <= state->spc) {
                    // Grow the stream by writing a Null character.
                    writeCharArray(&state->stream, '\0');
                }

                // Move the stream pointer forward on the stream.
                state->sp++;
                break;
            }
            // Move the stream pointer to the previous value.
            case '<': {
                // Decrement the stream counter.
                state->spc--;

                // If we moved outside the stream.
                if (0 > state->spc) {
                    // // Set the stream counter back to zero.
                    state->spc = 0;

                    // Error out.
                    return EVAL_ERROR_STREAM_UNDERFLOW;
                }

                // Move the stream pointer backward on the stream.
                state->sp--;
                break;
            }
            // Write the value at the stream pointer into the response array.
            case '.': {
                writeCharArray(&state->response, *state->sp);

#if DEBUG >= 1
                debugPrintResponse(state);
#endif

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
                //  tough. It could require us to provide the input alongside user code
                //  in the exact same order of the , instructions found in the user code. The
                //  solution is to count characters in the <in> CharArray at compile time then
                //  decrement the counter after every , instruction found in user code. If the
                //  counter is not equals zero, error out before run time. How to handle that in the
                //  REPL? With this route, we need to provide input before evaluation. For the
                //  command-line is fine tough.
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
                //      - What happens when , is the last valid instruction found in user code?
                //        Consume the NULL character? No, that would be horrible! if the next
                //        character is the NULL one, that should be an error which could be detected
                //        at compile time. We should not worry about it at run time!
                break;
            }
            // Jumps to the matching ].
            case '[': {
                // If the value at the stream pointer is zero.
                if (*state->sp == 0) {
                    // Move past the ] instruction.
                    // Increment the instruction counter.
                    state->ipc++;
                    // Move the instruction pointer forward.
                    state->ip++;

                    // Skip every instruction until we reach the matching ] instruction.
                    // Increment the brackets counter.
                    state->brackets++;
                    // Move the instruction pointer forward while the brackets counter is not zero.
                    while (state->brackets != 0) {
                        // If the current instruction is a [.
                        if (*state->ip == '[') {
                            // Increment the brackets counter.
                            state->brackets++;
                        } else if (*state->ip == ']') {
                            // If the current instruction is a ]. Decrement the brackets counter.
                            state->brackets--;
                        }

                        // Increment the instruction counter.
                        state->ipc++;
                        // Move the instruction pointer forward.
                        state->ip++;
                    }
                }
                break;
            }
            // Jumps to the matching [.
            case ']': {
                // If the value at the stream pointer is not zero.
                if (*state->sp != 0) {
                    // Move before the [ instruction.
                    // Decrement the instruction counter.
                    state->ipc--;
                    // Move the instruction pointer backward.
                    state->ip--;

                    // Skip every instruction until we reach the matching [ instruction.
                    // Increment the brackets counter.
                    state->brackets++;
                    // Move the instruction pointer backward while the brackets counter is not zero.
                    while (state->brackets != 0) {
                        // If the current instruction is a ].
                        if (*state->ip == ']') {
                            // Increment the brackets counter.
                            state->brackets++;

                        } else if (*state->ip == '[') {
                            // If the current instruction is a [. Decrement the brackets counter.
                            state->brackets--;
                        }

                        // Decrement the instruction counter.
                        state->ipc--;
                        // Move the instruction pointer backward.
                        state->ip--;
                    }
                }
                break;
            }
        }

#if DEBUG >= 1
        debugPrintStream(state);
#endif

        // Increment the instruction counter.
        state->ipc++;
        // Move the instruction pointer to the next instruction.
        state->ip++;
    }

    // Terminate the response array by writing a NULL character.
    writeCharArray(&state->response, '\0');

#if DEBUG >= 1
    debugPrintInstructions(state);
    debugPrintStream(state);
#endif

    // Evaluation was successfull.
    return EVAL_OK;
}
