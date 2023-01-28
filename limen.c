// This file is part of limen which is distributed under the terms of the MIT License
//
// Copyright (c) 2023 Ádám Török, Aerobird98
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

    array->values = NULL;

    array->capacity = 0;
    array->count = 0;
}

void initState(State *state) {
    initCharArray(&state->prompt);
    initCharArray(&state->instructions);
    initCharArray(&state->stream);
    initCharArray(&state->response);

    state->pp = NULL;
    state->ip = NULL;
    state->sp = NULL;

    state->ppc = 0;
    state->ipc = 0;
    state->spc = 0;

    state->brackets = 0;
    state->commas = 0;
}

void freeState(State *state) {
    freeCharArray(&state->prompt);
    freeCharArray(&state->instructions);
    freeCharArray(&state->stream);
    freeCharArray(&state->response);

    FREE(unsigned char, state->pp);
    FREE(unsigned char, state->ip);
    FREE(unsigned char, state->sp);

    state->pp = NULL;
    state->ip = NULL;
    state->sp = NULL;

    state->ppc = 0;
    state->ipc = 0;
    state->spc = 0;

    state->brackets = 0;
    state->commas = 0;
}

#if DEBUG > 0
static void debugPrintInstructions(State *state) {
    for (int i = 0; i < state->instructions.count; i++) {
        fprintf(stderr, "%c", state->instructions.values[i]);
    }

    fprintf(stderr, " %i %i\n", state->instructions.count, state->instructions.capacity);

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
    for (int i = 0; i < state->stream.count; i++) {
        if (i == state->spc) {
            fprintf(stderr, "[*%i]", state->stream.values[i]);
        } else {
            fprintf(stderr, "[%i]", state->stream.values[i]);
        }
    }

    fprintf(stderr, " %i %i\n", state->stream.count, state->stream.capacity);
}

static void debugPrintResponse(State *state) {
    for (int i = 0; i < state->response.count; i++) {
        fprintf(stderr, "%c", state->response.values[i]);
    }

    fprintf(stderr, " %i %i\n", state->response.count, state->response.capacity);
}
#endif

Result eval(State *state, const unsigned char *code, const unsigned char *data) {
    // Parse-Compile-time.
    //
    // At this phase we parse user code into validated instructions. Effectively Compiling code into
    // a safe-to-run run-time representation.

    while (*data != '\0') {
        // Skip non ASCII characters.
        if (*data <= 127 && *data >= 32) {
            // Increment the comma counter.
            state->commas++;
            writeCharArray(&state->prompt, *data);
        }

        // Step one character.
        data++;
    }

    // Terminate the prompt array by writing a NULL character.
    writeCharArray(&state->prompt, '\0');

    while (*code != '\0') {
        // Skip non ASCII characters.
        if (*code <= 127 && *code >= 32) {
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
                    // Decrement the comma counter.
                    state->commas--;
                    writeCharArray(&state->instructions, ',');
                    break;
                }
                case '[': {
                    // Increment the bracket counter.
                    state->brackets++;
                    writeCharArray(&state->instructions, '[');
                    break;
                }
                case ']': {
                    // Decrement the bracket counter.
                    state->brackets--;
                    writeCharArray(&state->instructions, ']');
                    break;
                }
            }
        }

        // Step one character.
        code++;
    }

    // Terminate the instructions array by writing a NULL character.
    writeCharArray(&state->instructions, '\0');

    // Grow the stream by writing a Null character.
    writeCharArray(&state->stream, '\0');

    // Set PP to point at the first value in the prompt.
    state->pp = &state->prompt.values[0];
    // Set IP to point at the first instruction.
    state->ip = &state->instructions.values[0];
    // Set SP to point at the first value on the stream.
    state->sp = &state->stream.values[0];

    // If there are mismatched commas.
    if (state->commas != 0) {
        // Set the comma counter back to zero.
        state->commas = 0;

        // Error out.
        return RESULT_ERROR_MISMATCHED_COMMAS;
    }

    // If there are mismatched brackets.
    if (state->brackets != 0) {
        // Set the bracket counter back to zero.
        state->brackets = 0;

        // Error out.
        return RESULT_ERROR_MISMATCHED_BRACKETS;
    }

    // Run-time.
    //
    // At this phase we run validated instructions evaluating them into a response.
    // Effectively manipulating the stream and altering state.

    // Run isntructions while IP is not pointing at the NULL character.
    while (*state->ip != '\0') {
#if DEBUG >= 1
        debugPrintInstructions(state);
#endif

        // Based on the current instruction.
        switch (*state->ip) {
            // Increment the value at the stream pointer.
            case '+': {
                // Ensure that the value wraps around to zero after reaching its maximum.
                *state->sp = (*state->sp + 1) & VALUE_MAX;
                break;
            }
            // Decrement the value at the stream pointer.
            case '-': {
                // Ensure that the value wraps around to its maximum after reaching zero.
                *state->sp = (*state->sp - 1) & VALUE_MAX;
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
                    return RESULT_ERROR_STREAM_UNDERFLOW;
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
            // Set the value at the stream pointer to the value at the prompt pointer.
            case ',': {
                *state->sp = *state->pp;
                // Increment the prompt counter.
                state->ppc++;
                // Move the prompt pointer forward.
                state->pp++;
                break;
            }
            // Jumps to the matching ].
            case '[': {
                // If the value at the stream pointer is zero.
                if (*state->sp == 0) {
                    // Increment the instruction counter.
                    state->ipc++;
                    // Move the instruction pointer forward past the ] instruction.
                    state->ip++;

                    // Increment the bracket counter.
                    state->brackets++;
                    // Move the instruction pointer forward while the brackets counter is not zero.
                    // Skip every instruction until we reach the matching ] instruction.
                    while (state->brackets != 0) {
                        // If the current instruction is a [.
                        if (*state->ip == '[') {
                            // Increment the bracket counter.
                            state->brackets++;
                        } else if (*state->ip == ']') {
                            // If the current instruction is a ]. Decrement the bracket counter.
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
                    // Decrement the instruction counter.
                    state->ipc--;
                    // Move the instruction pointer backward before the [ instruction.
                    state->ip--;

                    // Increment the bracket counter.
                    state->brackets++;
                    // Move the instruction pointer backward while the brackets counter is not zero.
                    // Skip every instruction until we reach the matching [ instruction.
                    while (state->brackets != 0) {
                        // If the current instruction is a ].
                        if (*state->ip == ']') {
                            // Increment the bracket counter.
                            state->brackets++;

                        } else if (*state->ip == '[') {
                            // If the current instruction is a [. Decrement the bracket counter.
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
    return RESULT_OK;
}
