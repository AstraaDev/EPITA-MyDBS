CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wvla -Werror -g
CC = gcc
OBJ = src/my_db.o
BIN = my_db
SRC = src/my_db.c

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(BIN)
