#include "huffman.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* --------------------- Compression --------------------- */

// Da continuare
void count_frequencies(FILE* fp, unsigned long* frequencies, unsigned char* file_buffer, long size, long *index){
    int byte;
    while((byte = fgetc(fp)) != EOF){
        frequencies[byte]++;

        if(*index >= size){
            fprintf(stderr, "Buffer overflow\n");
            exit(1);
        }   
        file_buffer[(*index)++] = (unsigned char)byte;
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
    if(heap->size >= SIZE){
        fprintf(stderr, "Error: heap overflow\n");
        exit(1);
    }

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
            heap->leaves_counter++;
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

    if(heap->size == 0){
        fprintf(stderr, "Heap underflow\n");
        exit(1);
    }
    
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
        
        // Creo il loro padre
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
        memcpy(table[node->val].bits, buffer, (depth + 7)/8);
        table[node->val].length = depth;
        return;
    }

    if(depth >= 256){
        fprintf(stderr, "Code too long\n");
        exit(1);
    } 
    
    int byte_index = depth / 8;
    int bit_index = depth % 8;
    // Il bit più significativo viene prima
    int pos = 7 - bit_index;

    // LEFT (bit = 1)
    unsigned char left_buffer[32];
    memcpy(left_buffer, buffer, 32);

    left_buffer[byte_index] |= (1 << pos);
    generate_codes(node->left, table, left_buffer, depth + 1);

    // RIGHT (bit = 0)
    unsigned char right_buffer[32];
    memcpy(right_buffer, buffer, 32);

    right_buffer[byte_index] &= ~(1 << pos);
    generate_codes(node->right, table, right_buffer, depth + 1);
}

void pre_order_visit_tree(Node *root, FILE *fp, unsigned char *byte, int *bit_index){
    if(!root) return;

    int flag = (!root->left && !root->right) ? 1 : 0;

    *byte |= (flag << (7 - *bit_index));
    (*bit_index)++;

    if(*bit_index == 8){
        fwrite(byte, 1, 1, fp);
        *bit_index = 0;
        *byte = 0;
    } 
    
    if(flag) // foglia
        fwrite(&root->val, sizeof(root->val), 1, fp);
    else {
        pre_order_visit_tree(root->left, fp, byte, bit_index);
        pre_order_visit_tree(root->right, fp, byte, bit_index);
    }
}

void free_tree(Node *node){
    if(!node) return;

    free_tree(node->left);
    free_tree(node->right);

    free(node);
}

void write_compressed(char *filename_input, MinHeap *heap, Node* root, unsigned char *file_buffer, long index, Code* table){
    // Creo il file
    char filename_output[strlen(filename_input) + 5];
    int len = strlen(filename_input);
    memcpy(filename_output, filename_input, len - 3);
    filename_output[len - 3] = '\0';
    strcat(filename_output, "huff");
    printf("%s\n",filename_output);

    FILE *fp = fopen(filename_output, "wb");
    if(!fp){
        perror("fopen");
        exit(1);
    }
    
    // Generiamo l'Header: Numero di Foglie, Albero Serializzato, Numero totale di bit nel body
    // Intestazione, scrivo HF
    fwrite("HF", 1, 2, fp);
    // Versione della codifica
    fwrite("V1.0", 1, 4, fp);
    // Numero foglie
    unsigned char leaves = (unsigned char) heap->leaves_counter;
    fwrite(&leaves, sizeof(leaves), 1, fp);
    // Albero Serializzato
    unsigned char byte = 0;
    int bit_index = 0;
    pre_order_visit_tree(root, fp, &byte, &bit_index);
    free_tree(root);
    // Eventuale byte parziale
    if(bit_index != 0) fwrite(&byte, 1, 1, fp);
    // Numero totale di bit del body
    uint64_t total_bits = 0;
    for(long i = 0; i < index; i++){
        unsigned char val = file_buffer[i];
        total_bits += table[val].length;
    }
    fwrite(&total_bits, sizeof(total_bits), 1, fp);

    // Generiamo il Body: bit-code dei caratteri
    unsigned char byte_buffer = 0;
    bit_index = 0;

    for(long i = 0; i < index; i++){
        unsigned char val = file_buffer[i];
        Code code = table[val];

        // Per ogni bit della codifica
        for(int j = 0; j < code.length; j++){
            int byte_pos = j / 8;
            int bit_pos = 7 - (j%8); // Il più significativo prima
            int bit = (code.bits[byte_pos] >> bit_pos) & 1;
            byte_buffer |= (bit << (7 - bit_index));
            bit_index++;

            if(bit_index == 8){
                if(fwrite(&byte, 1, 1, fp) != 1){
                    perror("fwrite");
                    exit(1);
                }
                byte_buffer = 0;
                bit_index = 0;
            }
        }
    }

    if(bit_index != 0) fwrite(&byte_buffer, 1, 1, fp);

    free(file_buffer);

    fclose(fp);
}

/* -------------------- Decompression -------------------- */


/* --------------------- Functions ----------------------- */
void huffman_compress(char* filename_input){
    int len = strlen(filename_input);

    if(len < 4 || strcmp(filename_input + len - 4, ".txt") != 0){
        fprintf(stderr, "Error: input must be a .txt file\n");
        exit(1);
    }

    FILE* fp = fopen(filename_input, "rb");
    printf("%s\n",filename_input);

    if(!fp){
        fprintf(stderr, "Error: file not found '%s'\n", filename_input);
        exit(1);
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);

    if(size <= 0){
        fprintf(stderr, "Invalid file size\n");
        exit(1);
    }

    if(size > LIMIT_SIZE){
        fprintf(stderr, "File too large\n");
        exit(1);
    }

    rewind(fp);
    
    unsigned char *file_buffer = malloc(size);
    if(!file_buffer){
        fprintf(stderr,"Memory error\n");
        exit(1);
    }
    long index = 0;

    unsigned long frequencies[SIZE];
    memset(frequencies, 0, sizeof(frequencies));

    count_frequencies(fp, frequencies, file_buffer, size, &index);
    
    MinHeap* heap = build_priority_queue(frequencies);
    print_heap(heap);
    
    Node* root = build_tree(heap);
    
    Code table[SIZE];
    memset(table, 0, sizeof(Code)*SIZE);
    unsigned char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    int depth = 0;
    generate_codes(root, table, buffer, depth);
    
    write_compressed(filename_input, heap, root, file_buffer, index, table);    

    fclose(fp);
}

void huffman_decompress(char* filename_input){
    int len = strlen(filename_input);

    if(len < 5 || strcmp(filename_input + len - 5, ".huff") != 0){
        fprintf(stderr, "Error: input must be a .huff file\n");
        exit(1);
    }

    return;
}
