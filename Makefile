CC=cc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Os -static
BUILD_DIR=./build

all: mkdir cp ls

mkdir: src/mkdir/mkdir.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/mkdir src/mkdir/mkdir.c

cp: src/cp/cp.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/cp src/cp/cp.c

ls: src/ls/main.c
	$(CC) $(CFLAGS) -D_BSD_SOURCE -o $(BUILD_DIR)/ls src/ls/main.c src/ls/ls.c src/ls/put.c src/ls/strmode.c src/ls/make_entry.c

clean:
	rm -rf build/
