#include "huffman.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* --------------------- Compression --------------------- */

// Da continuare
void count_frequencies(FILE* fp, long* frequencies){
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    rewind(fp);

    for(long i = 0; i < size; i++){
        int byte = fgetc(fp);
        frequencies[byte]++;
    }
    
    return;
}
/*
Queue build_priority_queue(unsigned int* freq){
    if(!freq) return;        

    return queue;
}

Tree build_tree(){
    
    return tree;
}

void generate_codes(Tree* head){
    if(!head) return;

    return;
}

void write_compressed(){
    return;
}
*/
/* -------------------- Decompression -------------------- */


/* --------------------- Functions ----------------------- */
void huffman_compress(char* filename_input){
    FILE* fp = fopen(filename_input, "r");

    if(!fp){
        fprintf(stderr, "Error: file not found '%s'\n", filename_input);
        exit(1);
    }

    long frequencies[SIZE];
    memset(frequencies, 0, sizeof(frequencies));

    count_frequencies(fp, frequencies);
    /*
    Queue queue = build_priority_queue();
    Tree tree = build_tree();
    generate_codes(&tree);
    write_compressed();    
    */

    fclose(fp);
}

void huffman_decompress(char* filename_input){
    return;
}
