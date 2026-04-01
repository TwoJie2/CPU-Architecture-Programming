CC=gcc
CFLAGS=-O2 -std=c11 -Wall -Wextra
SRC=src/main.c src/dot.c src/reduce.c src/timer.c
OUT=lab1.exe

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	-del /Q $(OUT) 2>nul || true
