# Makefile minimo para compilar o projeto

CC = gcc
TARGET = image_processor
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/image.c $(SRC_DIR)/histogram.c $(SRC_DIR)/gui.c $(SRC_DIR)/utils.c
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/image.o $(OBJ_DIR)/histogram.o $(OBJ_DIR)/gui.o $(OBJ_DIR)/utils.o

CFLAGS = -std=c99 -Wall -Wextra -O2 -I$(INC_DIR) $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf)
LDLIBS = $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf) -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
