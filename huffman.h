#ifndef HUFFMAN_H
#define HUFFMAN_H

#define SIZE 256
#define LIMIT_SIZE (100 * 1024 * 1024) // 100 MB

typedef struct Node{
    unsigned char val;
    unsigned long freq;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct MinHeap{
    Node* queue[SIZE]; // Al peggio ho 256 posti, in caso vado a ridurre   
    int size; 
    unsigned int leaves_counter;
} MinHeap;

typedef struct Code{
    // Al peggio devo usare 255 bit per una codifica, faccio quindi 32*8 = 256
    unsigned char bits[32];
    int length;
} Code;

void huffman_compress(char* filename_input);
void huffman_decompress(char* filename_input);

#endif
