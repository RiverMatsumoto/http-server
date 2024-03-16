CC = gcc
CFLAGS = -Iinclude -Wall
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

SRC_FILES_SERVER = $(wildcard $(SRC_DIR)/server/*.c)
SRC_FILES_CLIENT = $(wildcard $(SRC_DIR)/client/*.c)

OBJ_FILES_SERVER = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES_SERVER))
OBJ_FILES_CLIENT = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES_CLIENT))

SERVER = $(BIN_DIR)/server
CLIENT = $(BIN_DIR)/client

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ_FILES_SERVER)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^

$(CLIENT): $(OBJ_FILES_CLIENT)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean