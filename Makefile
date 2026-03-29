all:
	mkdir -p build
	gcc -Wall -g -o build/programma main.c huffman.c

clean:
	rm -rf build

.PHONY: all clean
