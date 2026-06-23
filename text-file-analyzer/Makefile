# The compiler we are using. 'gcc' is the GNU C Compiler.
CC = gcc

# Compiler flags that control the build process:
# -g:      Includes debugging information in the executable (essential for GDB/Valgrind).
# -Wall:   Turns on 'all' reasonably common compiler warnings.
# -Wextra: Turns on even more warnings not covered by -Wall.
# -std=c11: Enforces the C11 standard for our code.
CFLAGS = -g -Wall -Wextra -std=c11

# The name of the final executable file we want to build.
TARGET = analyzer

# A list of all the source (.c) files in our project.
SOURCES = main.c analyzer.c hashtable.c

# Automatically generate a list of object (.o) files from our list of source files.
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean re

# The 'all' target is the default goal.
all: $(TARGET)

# LINKING rule: links all object files into the final executable.
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# COMPILATION rule: compiles any .c file into a .o object file.
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# The 'clean' target removes all generated files.
clean:
	rm -f $(OBJECTS) $(TARGET)

# The 're' target forces a complete rebuild.
re: clean all