CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wvla -Werror -g -fsanitize=address
CC = gcc
OBJ = src/my_db.o src/utils.o src/commands.c
BIN = my_db
SRC = src/my_db.c src/utils.c src/commands.o

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(BIN)
