CFLAGS = -std=c99 -pedantic -Wall -Wextra -Wvla -Werror -g
CC = gcc
OBJ = src/my_strace.o src/utils.o
BIN = my_strace
SRC = src/my_strace.c src/utils.c

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(BIN)
