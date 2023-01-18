#include <stdio.h>

#include "limen.h"

// TODO: Implement REPL.
int main(int argc, const char *argv[]) {
    if (argc < 2 || argc > 2) {
        fprintf(stderr, "Usage: %s <code>\n", argv[0]);
        return 64;  // EX_USAGE: The command was used incorrectly.
    }

    State state;

    initState(&state);

    EvalResult result = eval(&state, (const unsigned char *)argv[1]);

    switch (result) {
        case EVAL_OK:
            // Everything went well.
            break;
        case EVAL_MISMATCHED_BRACKETS:
            fprintf(stderr, "Error: Mismatched brackets.\n");
            return 65;  // EX_DATAERR: The input data was incorrect.
        case EVAL_ERROR_UNKNOWN_CHARACTER:
            fprintf(stderr, "Error: Unknown character.\n");
            return 65;  // EX_DATAERR: The input data was incorrect.
        case EVAL_ERROR_STACK_UNDERFLOW:
            fprintf(stderr, "Error: Stack underflow.\n");
            return 70;  // EX_SOFTWARE: An internal software error has been detected.
        case EVAL_ERROR_UNKNOWN:
            fprintf(stderr, "Error: Unknown error.\n");
            return 69;  // EX_UNAVAILABLE:  Something did not work and do not know why.
    }

    fprintf(stdout, "%s\n", state.out.values);

    fprintf(stdout, "%i %i ", state.stream.count, state.stream.capacity);
    for (int i = 0; i < state.stream.count; i++) {
        if (i == state.pc)
            fprintf(stdout, "[*%i]", state.stream.values[i]);
        else
            fprintf(stdout, "[%i]", state.stream.values[i]);
    }

    fprintf(stdout, "\n");

    freeState(&state);

    return 0;  // EX_OK: Successful evaluation.
}

