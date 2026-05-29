#	
#	Raylib Game Makefile
#	

NAME=pong
VERSION=1.0.0

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
BIN=$(BIN_DIR)/$(NAME)

CC=gcc
CPPFLAGS=-Iinclude
CFLAGS=-g -Wall
LDFLAGS=
LDLIBS=-lraylib


.PHONY: run clean release_win

release_win: CPPFLAGS = -Iinclude
release_win: CFLAGS = -Wall -O2 -DVERSION=\"$(VERSION)\" -DRELEASE
release_win: LDFLAGS = -mwindows
release_win: LDLIBS = -lraylib -lgdi32 -lwinmm

release_win: $(BIN)

run: $(BIN)
	$(BIN)

$(BIN): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR) 
