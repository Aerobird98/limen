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

// TODO: Not all of these are in use.
#define MEMORY_MAX           65535
#define ARRAY_CAPACITY_MAX   32767
#define ARRAY_COUNT_MAX      30000
#define ARRAY_GROW_THRESHOLD 8
#define ARRAY_GROW_FACTOR    2
#define VALUE_MAX            127

// A generic allocation function that handles all explicit memory management.
//
// It's used like so:
//
// - To allocate new memory, <memory> is NULL and <was> is zero. It should return the allocated
//   memory or NULL on failure.
//
// - To attempt to grow an existing allocation, <memory> is the memory, <was> is its previous size,
//   and <will> is the desired size. It should return <memory> if it was able to grow it in place,
//   or a new memory if it had to move it.
//
// - To shrink memory, <memory>, <was>, and <will> are the same as above but it will always return
//   <memory>.
//
// - To free memory, <memory> will be the memory to free and <will> and <was> will be zero. It
//   should return NULL.
//
// TODO: Consider limiting memory usage. A reasonable maximum could be defined
//       trough usage tests.
void *reallocate(void *memory, size_t was, size_t will);

#define ALLOCATE(type)                 (type *)reallocate(NULL, 0, sizeof(type))
#define GROW(type, memory, will)       (type *)realocate(memory, sizeof(type), will)
#define SRINK(type, memory, will)      GROW(type, memory, will)
#define FREE(type, memory)             reallocate(memory, sizeof(type), 0)
#define ALLOCATE_ARRAY(type, capacity) (type *)reallocate(NULL, 0, sizeof(type) * (capacity))
#define GROW_CAPACITY(capacity) \
    ((capacity) < ARRAY_GROW_THRESHOLD ? ARRAY_GROW_THRESHOLD : (capacity) * (ARRAY_GROW_FACTOR))
#define GROW_ARRAY(type, memory, count, capacity) \
    (type *)reallocate(memory, sizeof(type) * (count), sizeof(type) * (capacity))
#define SRINK_ARRAY(type, memory, count, capacity) GROW_ARRAY(type, memory, count, capacity)
#define FREE_ARRAY(type, memory, capacity)         reallocate(memory, sizeof(type) * (capacity), 0)

typedef enum eResult {
    RESULT_OK,                   // Everything went fine.
    RESULT_MISMATCHED_COMMAS,    // Every comma should have a corresponding character in user
                                 // data. Their numbers must match.
    RESULT_MISMATCHED_BRACKETS,  // Every opening bracket should have a corresponding closing
                                 // bracket in user code. Their numbers must match.
    RESULT_ARRAY_UNDERFLOW,      // An Array pointer offset went below zero.
    RESULT_UNKNOWN,              // Something went wrong.
} Result;

// RESULT_ARRAY_OVERFLOW,            An Array pointer offset went above maximum.
// RESULT_ARRAY_NOT_ENOUGH_CAPACITY, An array reached its maximum capacity.

// A byte.
//
// NOTE: Should be in the range of zero to 255.
typedef unsigned char Byte;

// A dynamic byte array implementation that uses the generic allocation function trough a series of
// MACROS to grow when new values are written to it.
//
// TODO: This implementation could run into issues if the count or capacity value
//       overflows an int. It sould be quite rare, yet consider limiting capacity
//       and count.
typedef struct sByteArray {
    int count;
    int capacity;
    Byte *values;
} ByteArray;

void initByteArray(ByteArray *array);
void writeByteArray(ByteArray *array, Byte value);
void freeByteArray(ByteArray *array);

// Store informations between evaluations.
//
// NOTE: This sould be read-only.
typedef struct sState {
    ByteArray prompt;        // Validated prompt read from user data.
    ByteArray instructions;  // Validated instructions read from user code.
    ByteArray stream;        // Stream for the validated instructions to operate on.
    ByteArray response;      // Response of the validated instructions.

    Byte *pp;  // Pointer pointing at the current value in the prompt.
    Byte *ip;  // Pointer pointing at the current instruction.
    Byte *sp;  // Pointer pointing at the current value on the stream.

    int ppc;  // Pointer offset. Counts where the prompt pointer is.
    int ipc;  // Pointer offset. Counts where the instruction pointer is.
    int spc;  // Pointer offset. Counts where the stream pointer is.

    int brackets;  // Mismatched bracket count for error checks and loop management.
    int commas;    // Mismatched comma count for error checks.
} State;

void initState(State *state);
void freeState(State *state);

// Evaluate provided user code into a response.
Result eval(State *state, const Byte *code, const Byte *data);

#endif
