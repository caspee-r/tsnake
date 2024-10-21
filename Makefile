# Define compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -pedantic -ggdb -Os

# Target executable
TARGET = tsnake
SRC = tsnake.c

# Default rule to build the executable
all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)


# Clean rule to remove compiled files
clean:
	rm -f $(TARGET)
