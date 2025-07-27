#
#	Raylib Game Makefile
#

BIN_DIR=bin
BIN=$(BIN_DIR)/spacies

SRC_DIR=src
SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ_DIR=obj
RES_DIR=resources
RES=$(wildcard $(RES_DIR)/*.bin)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC)) $(patsubst $(RES_DIR)/%.bin, $(OBJ_DIR)/%.o, $(RES))

CC=gcc
LD=ld
CPPFLAGS=-Iinclude
CFLAGS=-g -Wall
LDLIBS=-lm -lraylib

.PHONY: run clean 

run: $(BIN)
	$(BIN)

$(BIN): $(OBJ) | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(RES_DIR)/%.bin | $(OBJ_DIR)
	$(LD) -r -b binary $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR) 
