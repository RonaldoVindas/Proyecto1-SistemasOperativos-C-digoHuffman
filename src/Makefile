CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SOURCES = CompresorHuffman.c DescompresorHuffman.c ForkHuffman.c ParallelHuffman.c

OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(addprefix $(SRC_DIR)/, $(SOURCES)))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
    $(CC) $(CFLAGS) -c $< -o $@

executables: $(OBJECTS)
    $(CC) $(CFLAGS) $(BUILD_DIR)/CompresorHuffman.o -o $(BIN_DIR)/CompresorHuffman
    $(CC) $(CFLAGS) $(BUILD_DIR)/DescompresorHuffman.o -o $(BIN_DIR)/DescompresorHuffman
    $(CC) $(CFLAGS) $(BUILD_DIR)/ForkHuffman.o -o $(BIN_DIR)/ForkHuffman
    $(CC) $(CFLAGS) $(BUILD_DIR)/ParallelHuffman.o -o $(BIN_DIR)/ParallelHuffman

clean:
    rm -rf $(BUILD_DIR)/* $(BIN_DIR)/*

.DEFAULT_GOAL := executables

.PHONY: clean executables
