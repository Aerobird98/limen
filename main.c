#include <stdio.h>

#include "limen.h"

int main(int argc, const char *argv[]) {
    // Define exit code as EX_OK: Successful evaluation.
    int ex = 0;

    if (argc > 3 || argc < 3) {
        fprintf(stderr, "Usage: %s <code> <data>\n", argv[0]);
        // Set exit code to EX_USAGE: The command was used incorrectly.
        ex = 64;
    }

    // TODO: Implement the REPL.

    // User code is the first command-line argument.
    const Byte *code = (const Byte *)argv[1];
    // User data is the second command-line argument.
    const Byte *data = (const Byte *)argv[2];

    // TODO: Handle bad library usage.
    //
    // NOTE: Order matters.

    // Declare state.
    State state;

    // Initialize state.
    initState(&state);

    // Run eval on a piece of code, altering state.
    eval(&state, code, data);

    // Check result, visualize response, report errors and set exit code based on what happened.
    switch (state.result) {
        case RESULT_OK: {
            // Visualize state.
            for (int i = 0; i < state.stream.count; i++) {
                if (i == state.spc) {
                    fprintf(stderr, "[*%i]", state.stream.values[i]);
                } else {
                    fprintf(stderr, "[%i]", state.stream.values[i]);
                }
            }
            fprintf(stderr, "\n");
            fprintf(stdout, "%s\n", state.response.values);
            // Set exit code to EX_OK: Successful evaluation.
            ex = 0;
            break;
        }
        case RESULT_MISMATCHED_PARENS: {
            fprintf(stderr, "Error: Mismatched parens.\n");
            // Set exit code to EX_DATAERR: The input data was incorrect.
            ex = 65;
            break;
        }
        case RESULT_MISMATCHED_COMMAS: {
            fprintf(stderr, "Error: Mismatched commas.\n");
            // Set exit code to EX_DATAERR: The input data was incorrect.
            ex = 65;
            break;
        }
        case RESULT_MISMATCHED_BRACKETS: {
            fprintf(stderr, "Error: Mismatched brackets.\n");
            // Set exit code to EX_DATAERR: The input data was incorrect.
            ex = 65;
            break;
        }
        case RESULT_ARRAY_UNDERFLOW: {
            fprintf(stderr, "Error: Array underflow.\n");
            // Set exit code to EX_SOFTWARE: An internal software error has been detected.
            ex = 70;
            break;
        }
        case RESULT_ARRAY_OVERFLOW: {
            fprintf(stderr, "Error: Array overflow.\n");
            // Set exit code to EX_SOFTWARE: An internal software error has been detected.
            ex = 70;
            break;
        }
        case RESULT_NOT_ENOUGH_MEMORY: {
            fprintf(stderr, "Error: Not enough memory.\n");
            // Set exit code to EX_SOFTWARE: An internal software error has been detected.
            ex = 70;
            break;
        }
        case RESULT_UNKNOWN: {
            fprintf(stderr, "Error: Unknown error.\n");
            // Set exit code to EX_UNAVAILABLE:  Something did not work and do not know why.
            ex = 69;
            break;
        }
    }

    // Free state.
    freeState(&state);

    // NOTE: After `freeState`, you can call `eval` once again, because freeing state also
    //       re-initializes it to its initial values; No need to call `initState` more than once on
    //       any given state.

    // Return exit code.
    return ex;
}
