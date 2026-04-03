# Huffman Compressor in C
# Descrizione

Questo progetto implementa un compressore e decompressore basato sull’algoritmo di Huffman coding.

Permette di:

comprimere file .txt in formato .huff
decomprimere file .huff nel file originale

La compressione è lossless, quindi i dati vengono recuperati esattamente.

# Compilazione
gcc -o huffman main.c huffman.c
o
make
▶️ Utilizzo
## Compressione
./huffman -c file.txt

## Genera:

file.huff
## Decompressione
./huffman -d file.huff

## Genera:

file.txt

# Funzionalità principali
Implementazione completa dell’algoritmo di Huffman
Costruzione dell’albero tramite min-heap
Codifica a livello di bit (non stringhe)
Serializzazione dell’albero nel file compresso
Formato .huff custom
Gestione di casi particolari (es. un solo simbolo)

# Formato .huff

Il file compresso contiene:

[HF][V1.0][leaves][padding][tree][total_bits][body]
## Dettaglio

HF → identificatore file
V1.0 → versione formato
leaves → numero di simboli distinti
tree → albero di Huffman serializzato (pre-order)
total_bits → numero totale di bit nel corpo
body → dati compressi

# Albero di Huffman

L’albero viene salvato in pre-order:

1 → foglia + carattere (8 bit)
0 → nodo interno

Questo permette di ricostruire esattamente l’albero in fase di decompressione.

# Come funziona

## Compressione
Conteggio frequenze dei caratteri
Costruzione albero di Huffman
Generazione codici binari
Scrittura file .huff

## Decompressione
Lettura header
Ricostruzione albero
Decodifica del bitstream
Scrittura file originale

# Limitazioni
Accetta solo file .txt
Carica l’intero file in memoria

