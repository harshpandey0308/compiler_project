# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11

# Executable name
TARGET = compiler

# Source files
SRC = main.c \
        tokenizer.c \
        parser.c \
        semantic.c \
        TACcode.c \
        optimizer.c \
        ASMCODE.c \
		preprocessor.c \
		VM.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile .c into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Remove generated files
clean:
	del /Q	*.o
	del	/Q	compiler.exe

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run