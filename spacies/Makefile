#	
#	Raylib Game Makefile
#	

NAME=spacies

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
BIN=$(BIN_DIR)/$(NAME)

CC=gcc
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

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR) 
