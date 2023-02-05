#include <stdio.h>

#include "limen.h"

static void printState(State *state) {
    for (int i = 0; i < state->prompt.count; i++) {
        fprintf(stderr, "%c", state->prompt.values[i]);
    }

    fprintf(stderr, "\n");

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
    // Define exit code as EX_OK: Successful evaluation.
    int ex = 0;

    // TODO: Implement the REPL. We need something like the uv library (libuv) to have consistent
    //       behavior arcoss operating systems.

    // If there are mismatched command-line arguments.
    if (argc < 3 || argc > 3) {
        fprintf(stderr, "Usage: %s <code> <data>\n", argv[0]);
        // Set exit code to EX_USAGE: The command was used incorrectly.
        ex = 64;
        // Return exit code.
        return ex;
    }

    // User code is the first command-line argument.
    const Byte *code = (const Byte *)argv[1];
    // User data is the second command-line argument.
    const Byte *data = (const Byte *)argv[2];

    // TODO: Handle bad library usage.
    //
    // NOTE: Calling eval before initializing or mutiple times after initialization without an
    //       inbetween free call, freeing state before initializing or initializing twice or more
    //       before or after freeing all causes undefined behavior. When we free the state, we also
    //       reinitialize it to its default values, so it is safe to call free multiple times after
    //       initialization; but do not ever try to call eval twice or more on a state without
    //       inbetween freeing it; that causes undefined behavior.

    // Declare state.
    State state;

    // Initialize state.
    initState(&state);

    // Run eval on a piece of code, manipulating state and returning a result.
    Result result = eval(&state, code, data);

    // Handle result, report errors and set exit code based on those.
    switch (result) {
        case RESULT_OK: {
            fprintf(stdout, "%s\n", state.response.values);
            // Set exit code to EX_OK: Successful evaluation.
            ex = 0;
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

    // Vizualize state.
    printState(&state);

    // Free state.
    freeState(&state);

    // Return exit code.
    return ex;
}
