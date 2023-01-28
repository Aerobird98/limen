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

#ifndef limen_h
#define limen_h

#include <stdlib.h>

#ifdef DEBUG
    #include <stdio.h>
#endif

// NOTE: These could be good default values, consider exposing these trough a config struct passed
//       to state on initialization to allow for per state user configuration.
#define VALUE_MAX            127
#define MEMORY_MAX           65535
#define ARRAY_CAPACITY_MAX   32767
#define ARRAY_COUNT_MAX      30000
#define ARRAY_GROW_THRESHOLD 8
#define ARRAY_GROW_FACTOR    2

// A generic allocation function that handles all explicit memory management.
//
// It's used like so:
//
// - To allocate new memory, <memory> is NULL and <was> is zero. It should
//   return the allocated memory or NULL on failure.
//
// - To attempt to grow an existing allocation, <memory> is the memory,
//   <was> is its previous size, and <will> is the desired size.
//   It should return <memory> if it was able to grow it in place, or a new
//   memory if it had to move it.
//
// - To shrink memory, <memory>, <was>, and <will> are the same as above
//   but it will always return <memory>.
//
// - To free memory, <memory> will be the memory to free and <will> and
//   <was> will be zero. It should return NULL.
//
// NOTE: Use provided MACROS.
void *reallocate(void *memory, size_t was, size_t will);

#define ALLOCATE(type)                 (type *)reallocate(NULL, 0, sizeof(type))
#define GROW(type, memory, will)       (type *)realocate(memory, sizeof(type), will)
#define SRINK(type, memory, will)      GROW(type, memory, will)
#define FREE(type, memory)             reallocate(memory, sizeof(type), 0)
#define ALLOCATE_ARRAY(type, capacity) (type *)reallocate(NULL, 0, sizeof(type) * (capacity))
#define GROW_CAPACITY(capacity) \
    ((capacity) < ARRAY_GROW_THRESHOLD ? ARRAY_GROW_THRESHOLD : (capacity) * (ARRAY_GROW_FACTOR))
#define GROW_ARRAY(type, memory, wasCapacity, capacity) \
    (type *)reallocate(memory, sizeof(type) * (wasCapacity), sizeof(type) * (capacity))
#define SRINK_ARRAY(type, memory, wasCapacity, capacity) \
    GROW_ARRAY(type, memory, wasCapacity, capacity)
#define FREE_ARRAY(type, memory, capacity) reallocate(memory, sizeof(type) * (capacity), 0)

// A dynamic unsigned char array implementation that uses the generic allocation
// function trough a series of MACROS to grow when new values are written to it.
//
// NOTE: This implementation could run into issues if the count or capacity value
//       overflows an int. It sould be quite rare, yet consider limiting capacity
//       and count accounting for the occasional null byte at the end.
typedef struct sCharArray {
    int count;
    int capacity;
    unsigned char *values;
} CharArray;

void initCharArray(CharArray *array);
void writeCharArray(CharArray *array, unsigned char value);
void freeCharArray(CharArray *array);

// Store informations between evaluations.
//
// This shold be read-only and sould only be modified trough the provided library functions.
//
// NOTE: It would be wise to limit the memory usage of any given state. A reasonable maximum could
//       be defined based on usage tests.
typedef struct sState {
    CharArray prompt;        // Validated prompt read from user data.
    CharArray instructions;  // Validated instructions read from user code.
    CharArray stream;        // Stream for the validated instructions to operate on.
    CharArray response;      // Response of the validated instructions.

    unsigned char *pp;  // Pointer pointing at the current value in the prompt.
    unsigned char *ip;  // Pointer pointing at the current instruction.
    unsigned char *sp;  // Pointer pointing at the current value on the stream.

    int ppc;  // Counts where the prompt pointer is.
    int ipc;  // Counts where the instruction pointer is.
    int spc;  // Counts where the stream pointer is.

    int brackets;  // Mismatched bracket count for error checks and loop management.
    int commas;    // Mismatched comma count for error checks.

} State;

void initState(State *state);
void freeState(State *state);

typedef enum eResult {
    RESULT_OK,
    RESULT_ERROR_MISMATCHED_COMMAS,
    RESULT_ERROR_MISMATCHED_BRACKETS,
    RESULT_ERROR_STREAM_UNDERFLOW,
    RESULT_ERROR_UNKNOWN,
} Result;

// Evaluate provided user code into a response.
Result eval(State *state, const unsigned char *code, const unsigned char *data);

#endif
