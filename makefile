# DEBUG Level.
GG ?= 0

# Name of the program.
NM ?= limen

# Command used to copy files to another location.
CP ?= install -C -m 755

# Command used to delete files.
RM ?= rm -f

# Compiler.
CC ?= gcc

# Destination directory.
DESTDIR ?= ~/../usr

# Compiler flags.
CFLAGS := -std=c99 -Wpedantic -Werror -Wall -Wextra -Wno-unused-parameter

# Append addiional compiler flags based on DEBUG level.
ifneq ($(GG),0)
	CFLAGS += -Og -g3 -DDEBUG=$(GG)
else
	CFLAGS += -O3
endif

# Directories.
LIBDIR := $(DESTDIR)/lib
BINDIR := $(DESTDIR)/bin

# Files.
HEADERS := $(wildcard *.h)
SOURCES := $(wildcard *.c)
OBJECTS := $(notdir $(SOURCES:.c=.o))

# Compile all object files.
%.o: %.c $(HEADERS)
	@ $(CC) -c $(CFLAGS) -o $@ $<

# Link all the object files and libraries.
$(NM): $(OBJECTS)
	@ $(CC) $(CFLAGS) $^ -o $@

# Build all targets.
.PHONY: all
all: $(NM)

# Install all built files.
.PHONY: install
install:
	@ $(CP) $(NM) $(BINDIR)/

# Uninstall all built files.
.PHONY: uninstall
uninstall:
	@ $(RM) $(BINDIR)/$(NM)

# Clean all built files.
.PHONY: clean
clean:
	@ $(RM) $(OBJECTS)
	@ $(RM) $(NM)
