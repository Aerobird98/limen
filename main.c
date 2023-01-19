#include <stdio.h>

#include "limen.h"

static void printState(State *state) {
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

int main(int argc, const char *argv[]) {
    // TODO: Implement REPL.
    if (argc < 2 || argc > 2) {
        fprintf(stderr, "Usage: %s <code>\n", argv[0]);
        return 64;  // EX_USAGE: The command was used incorrectly.
    }

    State state;

    initState(&state);

    // TODO: As of now we cannot run eval twice on the same state. We would need to reinitialize the
    //       instructions and response CharArrays in order to be able to.
    EvalResult result = eval(&state, (const unsigned char *)argv[1]);

    switch (result) {
        case EVAL_OK: {
            fprintf(stdout, "%s\n", state.response.values);
            printState(&state);
            break;
        }
        case EVAL_MISMATCHED_BRACKETS: {
            fprintf(stderr, "Error: Mismatched brackets.\n");
            return 65;  // EX_DATAERR: The input data was incorrect.
        }
        case EVAL_ERROR_UNKNOWN_CHARACTER: {
            fprintf(stderr, "Error: Unknown character.\n");
            return 65;  // EX_DATAERR: The input data was incorrect.
        }
        case EVAL_ERROR_STACK_UNDERFLOW: {
            fprintf(stderr, "Error: Stack underflow.\n");
            printState(&state);
            return 70;  // EX_SOFTWARE: An internal software error has been detected.
        }
        case EVAL_ERROR_UNKNOWN: {
            fprintf(stderr, "Error: Unknown error.\n");
            return 69;  // EX_UNAVAILABLE:  Something did not work and do not know why.
        }
    }

    /* Explicit cast because size_t has different sizes on 32-bit and 64-bit and
     * we need a consistent type for the format string.
     */
    // fprintf(stdout, "State used %lu bytes.\n", (unsigned long)sizeof(state));

    freeState(&state);

    return 0;  // EX_OK: Successful evaluation.
}
