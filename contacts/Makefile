CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude
LDFLAGS = -lsqlite3

TARGET = contacts

SRC = src/main.c src/contact.c src/storage.c src/validation.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)