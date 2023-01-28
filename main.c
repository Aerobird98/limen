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

    // If there are mismatched arguments.
    //
    // TODO: Implement the REPL. We need something like the uv library to have consistent behavior
    //       arcoss operating systems.
    if (argc < 3 || argc > 3) {
        fprintf(stderr, "Usage: %s <code> <data>\n", argv[0]);
        // Set exit code to EX_USAGE: The command was used incorrectly.
        ex = 64;
        // Return exit code.
        return ex;
    }

    const unsigned char *code = (const unsigned char *)argv[1];
    const unsigned char *data = (const unsigned char *)argv[2];

    // Declare state.
    State state;

    // Initialize state.
    initState(&state);

    // Run eval on a piece of code, manipulating state and returning a result.
    //
    // TODO: As of now we cannot run eval twice on the same state, tough it is possible to do so
    //       which invalidates state. We would need to reinitialize the prompt, instructions and
    //       response CharArrays and their counters and pointers in order to be able to, tough afer
    //       evaluation we need them for state visualization. Do we need to keep the stream, its
    //       counter and its pointer, as is, between evaluations? Inside a REPL session maybe? As of
    //       now we need to free state then reinitialize it to be able to reuse it between
    //       consecutive evaluations.
    Result result = eval(&state, code, data);

    // Handle result and set exit code based on that.
    switch (result) {
        case RESULT_OK: {
            fprintf(stdout, "%s\n", state.response.values);
            // Set exit code to EX_OK: Successful evaluation.
            ex = 0;
            break;
        }
        case RESULT_ERROR_MISMATCHED_COMMAS: {
            fprintf(stderr, "Error: Mismatched commas.\n");
            // Set exit code to EX_DATAERR: The input data was incorrect.
            ex = 65;
            break;
        }
        case RESULT_ERROR_MISMATCHED_BRACKETS: {
            fprintf(stderr, "Error: Mismatched brackets.\n");
            // Set exit code to EX_DATAERR: The input data was incorrect.
            ex = 65;
            break;
        }
        case RESULT_ERROR_STREAM_UNDERFLOW: {
            fprintf(stderr, "Error: Stream underflow.\n");
            // Set exit code to EX_SOFTWARE: An internal software error has been detected.
            ex = 70;
            break;
        }
        case RESULT_ERROR_UNKNOWN: {
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
