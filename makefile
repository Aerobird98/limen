# Optional parameters.
GG ?= 0
NM ?= limen
CC ?= gcc
RM ?= rm

# Compiler flags.
CFLAGS := -std=c99 -Wpedantic -Werror -Wall -Wextra -Wno-unused-parameter

# DEBUG configuration.
ifneq ($(GG),0)
	CFLAGS += -Og -g3 -DDEBUG=$(GG)
else
	CFLAGS += -O3
endif

# Files.
HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
OBJECTS := $(notdir $(SOURCES:.c=.o))

# Targets ---------------------------------------------------------------------

# Build all targets.
.PHONY: all
all: $(NM)

# Link the interpreter.
$(NM): $(OBJECTS)
	@ $(CC) $(CFLAGS) $^ -o $@

# Compile object files.
%.o: %.c $(HEADERS)
	@ $(CC) -c $(CFLAGS) -o $@ $<

# Clean all built files.
.PHONY: clean
clean:
	@ $(RM) $(OBJECTS)
	@ $(RM) $(NM)
