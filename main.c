#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s -c file.txt       # compress\n", program_name);
    fprintf(stderr, "  %s -d file.huff      # decompress\n", program_name);
}

int main(int argc, char **argv){
    if(argc != 3){
        print_usage(argv[0]);
        exit(1);
    }

    char mode = argv[1][1]; //prende 'c' o 'd' dal flag "-c" / "-d"

    if(argv[1][0] != '-' || (mode != 'c' && mode != 'd')){
        fprintf(stderr, "Error: non valid flag! '%s'\n", argv[1]);
        print_usage(argv[0]);
        exit(1); 
    }

    // Aggiungere controlli se abbiamo in ingresso un txt o meno

    char *filename = argv[2];
    printf("Mode: %s\n", mode == 'c' ? "compression" : "decompression");
    printf("File: %s\n", filename);

    if(mode == 'c'){
        huffman_compress(filename);
    } else{    
        huffman_decompress(filename);
    }

    return 0;
}
