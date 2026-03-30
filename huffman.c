#include "huffman.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* --------------------- Compression --------------------- */

// Da continuare
void count_frequencies(FILE* fp, unsigned long* frequencies){
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    rewind(fp);

    int byte;
    while((byte = fgetc(fp)) != EOF){
        frequencies[byte]++;
    }
}

void print_heap(MinHeap* heap){
    for(int i = 0; i < heap->size; i++){
        printf("Val: %c, Freq: %ld\n", heap->queue[i]->val, heap->queue[i]->freq);
    }
}

void swap(Node **a, Node **b){
    Node* temp = *a;

    *a = *b;
    *b = temp;

}

void heap_insert(MinHeap* heap, Node* node){
    // Inserisco sempre in fondo
    heap->queue[heap->size] = node;
    unsigned long index = heap->size;
    heap->size++;

    while(index > 0 && 
            heap->queue[(index - 1) / 2]->freq > 
            heap->queue[index]->freq){
        swap(&heap->queue[index], &heap->queue[(index - 1) / 2]);
        index = (index - 1) / 2;
    }

}

MinHeap* build_priority_queue(unsigned long* freq){
    if(!freq) exit(1);   

    MinHeap* heap = malloc(sizeof(MinHeap));
    memset(heap, 0, sizeof(MinHeap));  

    for(long i = 0; i < SIZE; i++){
        if(freq[i] != 0){
            Node* node = malloc(sizeof(Node));
            
            if(!node){
                printf("Error: memory not allocated.\n");
                exit(1);
            }

            node->val = i;
            node->freq = freq[i];
            node->left = NULL;
            node->right = NULL;

            heap_insert(heap, node);
        }
    }

    return heap;
}

Node* heap_extract_min(MinHeap* heap){
    if(!heap) exit(1);
    
    Node* minNode = heap->queue[0];
    heap->size--;
    heap->queue[0] = heap->queue[heap->size];

    //Sift Down
    int i = 0;
    while(1){
        int left = 2*i + 1;
        int right = 2*i + 2;
        int min = i; // assumiamo che il nodo corrente sia il minore

        if(left < heap->size && 
                heap->queue[left]->freq < heap->queue[min]->freq)
            min = left;
            
        if(right < heap->size && 
                heap->queue[right]->freq < heap->queue[min]->freq)
            min = right;

        if(min == i) break; // Nodo nel posto giusto

        swap(&heap->queue[i], &heap->queue[min]);
        i = min;        
    }

    return minNode;
}

Node* build_tree(MinHeap* heap){
    if(!heap) exit(1);

    while(heap->size > 1){
        // Estraggo due nodi
        Node* left  = heap_extract_min(heap);
        Node* right = heap_extract_min(heap);
        
        // Credo il loro padre
        Node* root = malloc(sizeof(Node));
        root->freq = left->freq + right->freq;
        root->val = 0; //Nodo interno non ha carattere
        root->left = left;
        root->right = right;

        // Lo inserisco nell'heap
        heap_insert(heap, root);
    }

    // Ritorno il puntatore alla testa
    return heap_extract_min(heap);
}

void generate_codes(Node* node, Code* table, unsigned char* buffer, int depth){
    if(!node) return;

    if(!node->left && !node->right){
        memcpy(table[node->val].bits, buffer, (depth/8)+1);
        table[node->val].length = depth;
        return;
    } 
    
    int byte_index = depth / 8;
    int bit_index = depth % 8;
    // Il bit più significativo viene prima
    int pos = 7 - bit_index;
    unsigned char mask = (unsigned char)(1 << pos);
   
    buffer[byte_index] |= mask;
    generate_codes(node->left, table, buffer, depth + 1);
    
    buffer[byte_index] &= ~mask;
    generate_codes(node->right, table, buffer, depth + 1);
}

void write_compressed(char* filename_input){
    // Creo il file
    char filename_output[strlen(filename_input)];
    strncpy(filename_output, filename_input, strlen(filename_input)-3); // 3 perchè devo togliere txt
    strcat(filename_output, "huff");
    printf("%s\n",filename_output);

    FILE *fp = fopen(filename_output, "w");
    
    // Generiamo l'Header: Numero di Foglie, Albero Serializzato, Numero totale di bit nel body
    // Intestazione, scrivo HF
    fwrite("HF", 1, 2, fp);
    
    

    // Generiamo il Body: bit-code dei caratteri
    

    fclose(fp);
}

/* -------------------- Decompression -------------------- */


/* --------------------- Functions ----------------------- */
void huffman_compress(char* filename_input){
    FILE* fp = fopen(filename_input, "rb");
    printf("%s\n",filename_input);

    if(!fp){
        fprintf(stderr, "Error: file not found '%s'\n", filename_input);
        exit(1);
    }

    unsigned long frequencies[SIZE];
    memset(frequencies, 0, sizeof(frequencies));

    count_frequencies(fp, frequencies);
    
    MinHeap* heap = build_priority_queue(frequencies);
    print_heap(heap);
    
    Node* root = build_tree(heap);
    
    Code table[SIZE];
    memset(table, 0, sizeof(Code)*SIZE);
    unsigned char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    int depth = 0;
    generate_codes(root, table, buffer, depth);
    
    write_compressed(filename_input);    

    fclose(fp);
}

void huffman_decompress(char* filename_input){
    return;
}
