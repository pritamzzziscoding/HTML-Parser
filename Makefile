# Makefile for the C HTML Parser Project

# --- Variables ---
# Compiler and flags
CC = gcc
# CFLAGS: -Wall (all warnings), -Wextra (more warnings), -g (debug symbols)
#         -Isrc (add 'src' to include path), -std=c99 (use C99 standard)
CFLAGS = -g -Isrc -std=c99

# Directories
SRC_DIR = src
TEST_DIR = tests
OBJ_DIR = obj
BIN_DIR = bin

# --- Main Application ---
# Source files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/dom.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/utils.c
# Object files (placed in OBJ_DIR, mirroring the source structure)
#
# --- THIS IS THE CORRECTED LINE ---
# It now correctly produces obj/src/main.o, obj/src/dom.o, etc.
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))
#
# Executable name
TARGET = $(BIN_DIR)/html_parser

# --- Test Application ---
# Test source files
TEST_SRCS = $(TEST_DIR)/test_runner.c $(TEST_DIR)/test_lexer.c $(TEST_DIR)/test_parser.c \
            $(SRC_DIR)/dom.c $(SRC_DIR)/lexer.c $(SRC_DIR)/parser.c $(SRC_DIR)/utils.c
# Test object files (also mirrors structure, e.g., obj/tests/test_runner.o)
TEST_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(TEST_SRCS))
# Test executable name
TEST_TARGET = $(BIN_DIR)/run_tests

# --- Phony Rules (goals that aren't files) ---
.PHONY: all clean test run

# --- Main Rules ---

# Default rule: build the main executable
all: $(TARGET)

# Rule to link the main executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@printf "Successfully built executable at %s\n" $(TARGET)

# Rule to build the test executable
test: $(TEST_TARGET)
	@printf "Successfully built test runner at %s\n" $(TEST_TARGET)

# Rule to run the tests
run-tests: test
	@./$(TEST_TARGET)

# --- Pattern Rules for Compilation ---
# These rules now correctly match the object file paths

# Rule to compile a .c file from 'src' into a .o file in 'obj/src'
# e.g., creates 'obj/src/main.o' from 'src/main.c'
$(OBJ_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile a .c file from 'tests' into a .o file in 'obj/tests'
# e.g., creates 'obj/tests/test_runner.o' from 'tests/test_runner.c'
$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to link the test executable
$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

# --- Utility Rules ---

# Clean up build files
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@printf "Cleaned build artifacts.\n"

# A simple rule to run the parser on a test file
run: $(TARGET)
	@./$(TARGET) tests/inputs/test2.html