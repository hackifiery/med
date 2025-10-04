.DEFAULT_GOAL := all
.PHONY: all

CC = g++
CFLAGS=-Wall -Wextra -Wpedantic -std=c++20

med: src/med.cpp src/editor.hpp src/keyboard.hpp
	$(CC) $(CFLAGS) src/med.cpp -I src/ -o med
clean:
	rm -r med
all: med