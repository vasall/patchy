SRC_DIR 	:= src
OBJ_DIR 	:= obj
BIN_DIR 	:= bin

BIN 		:= $(BIN_DIR)/prog
SRC 		:= $(wildcard $(SRC_DIR)/*.c)
OBJ 		:= $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC		:= gcc
CFLAGS   	:= -g -O0 -ansi -std=c89 -I./inc -pedantic -D_POSIX_C_SOURCE=200809L
ERRFLAGS	:= -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition 

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ) | $(BIN_DIR)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(ERRFLAGS) -c $< -o $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)
