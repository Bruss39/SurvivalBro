CC = gcc
CPPFLAGS = -Iinclude
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
MODULES = src/entity.c src/genome.c src/population.c src/spot.c src/world.c
HEADERS = $(wildcard include/*.h)

ifeq ($(OS),Windows_NT)
EXE = .exe
endif

.PHONY: all test

all: survivalbro$(EXE)

survivalbro$(EXE): src/main.c $(MODULES) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) src/main.c $(MODULES) -o $@

survivalbro-tests$(EXE): tests/test_simulation.c $(MODULES) $(HEADERS) Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_simulation.c $(MODULES) -o $@

test: survivalbro-tests$(EXE)
	./survivalbro-tests$(EXE)
