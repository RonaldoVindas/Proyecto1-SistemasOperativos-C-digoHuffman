/*
Instituto Tecnológico de Costa Rica | Ing. Computación

Principios de Sistemas Operativos - Proyecto #1 - Cómpresión por Código Huffman: Versión Serial

Elaborado Por:
Adrián Herrera
Rebeca Madrigal
Ronaldo Vindas

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>                         //Librería POSIX
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

// Estructura para nodos del árbol de Huffman
typedef struct HuffmanNode {
    char data;
    unsigned frequency;
    struct HuffmanNode *left, *right;
} HuffmanNode;

// Estructura para la cola de prioridad (min-heap)
typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    HuffmanNode** array;
} MinHeap;

// Crear un nodo para el árbol de Huffman
HuffmanNode* createHuffmanNode(char data, unsigned frequency) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->left = node->right = NULL;
    node->data = data;
    node->frequency = frequency;
    return node;
}

// Crear una cola de prioridad vacía
MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HuffmanNode**)malloc(minHeap->capacity * sizeof(HuffmanNode*));
    return minHeap;
}

// Intercambiar dos nodos en la cola de prioridad
void swapHuffmanNode(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* temp = *a;
    *a = *b;
    *b = temp;
}

// Funciones para mantener la propiedad de min-heap
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->frequency < minHeap->array[smallest]->frequency)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->frequency < minHeap->array[smallest]->frequency)
        smallest = right;

    if (smallest != idx) {
        swapHuffmanNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

HuffmanNode* extractMin(MinHeap* minHeap) {
    HuffmanNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, HuffmanNode* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->frequency < minHeap->array[(i - 1) / 2]->frequency) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// Función para verificar si un nodo es una hoja
int isLeaf(HuffmanNode* root) {
    return !(root->left) && !(root->right);
}

// Crear e insertar en el árbol de Huffman
MinHeap* createAndBuildMinHeap(char data[], int frequency[], int size) {
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = createHuffmanNode(data[i], frequency[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

// Construir el árbol de Huffman
HuffmanNode* buildHuffmanTree(char data[], int frequency[], int size) {
    HuffmanNode *left, *right, *top;
    MinHeap* minHeap = createAndBuildMinHeap(data, frequency, size);
    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = createHuffmanNode('$', left->frequency + right->frequency);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Imprimir el código Huffman desde el árbol
void printCodes(HuffmanNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }
    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }
    if (isLeaf(root)) {
        printf("%c: ", root->data);
        for (int i = 0; i < top; ++i)
            printf("%d", arr[i]);
        printf("\n");
    }
}

// Función principal para comprimir el archivo de texto y generar el archivo .bin
void compressFile(FILE *inputFile, FILE *outputFile) {
    int frequency[256] = {0}; // Inicializar la frecuencia de cada caracter a 0
    char character;

    // Calcular la frecuencia de cada caracter en el archivo de texto
    while ((character = fgetc(inputFile)) != EOF) {
        frequency[(unsigned char)character]++;
    }

    // Crear arreglos para almacenar los caracteres y sus frecuencias
    char data[256];
    int freq[256];
    int index = 0;

    // Llenar los arreglos con los caracteres y sus frecuencias
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            data[index] = (char)i;
            freq[index] = frequency[i];
            index++;
        }
    }

    // Construir el árbol de Huffman
    HuffmanNode* root = buildHuffmanTree(data, freq, index);

    // Imprimir los códigos Huffman
    int arr[256], top = 0;
    printCodes(root, arr, top);

    // Escribir el árbol Huffman en el archivo .bin
    fwrite(frequency, sizeof(int), 256, outputFile);

    // Escribir los datos comprimidos en el archivo .bin
    rewind(inputFile); // Regresar al inicio del archivo
    while ((character = fgetc(inputFile)) != EOF) {
        for (int i = 0; i < index; i++) {
            if (data[i] == character) {
                fwrite(&i, sizeof(int), 1, outputFile);
                break;
            }
        }
    }

    // Liberar memoria
    fclose(inputFile);
    fclose(outputFile);
}

// Función principal para el proceso de compresión concurrente
void compressForks() {
    /*Función que incluye el proceso que se va a paralelizar, divide qué proceso hace cada fork, cada fork 
    se encarga de trabajar uno de los libros de la carpeta
    -Entradas: N/A
    -Salidas: N/A
    -Restricciones: N/A
    */

    int contador = 1;
    char primeraParte[] = "LibrosTXT/";
    char segundaParte[] = ".txt";
    char completo[500];

       

    while (contador < 99) { 
        sprintf(completo, "%s%d%s", primeraParte, contador, segundaParte);
        FILE *inputFile = fopen(completo, "r");
        FILE *outputFile = fopen("outputFork.bin", "wb");

        if (inputFile == NULL || outputFile == NULL) {
            printf("Error al abrir el archivo.\n");
            exit(1);
        }

        pid_t pid = fork();                                         //Para cada libro se crea un nuevo proceso, fork 
                                                                    //retorna 0 al proceso hijo, y al padre le regresa el PID de su hijo,
                                                                    // sino retorna -1 y no crea el proceso hijo
        
        if (pid == -1) {
            fprintf(stderr, "Error al crear el proceso hijo.\n");
            exit(1);
        } else if (pid == 0) {                                      //Al proceso hijo se le asigna qué debe de hacer
            compressFile(inputFile, outputFile);
            exit(0);                                                //Sale del proceso hijo después de hacer su asignación
        } else {                                                    //El proceso padre debe esperar a que todos los procesos hijos terminen
            wait(NULL); 
            contador++;
        }
        
        
        fclose(inputFile);
        fclose(outputFile);
    }
}

int main() {
     int contador = 1;
    char primeraParte[] = "LibrosTXT/";
    char segundaParte[] = ".txt";
    char completo[500];

    clock_t start_time, end_time;
    double cpu_time_used;

    start_time = clock();                                                       //Inicio de cronómetro  

    compressForks();

    end_time = clock();                                                         //Final del programa

    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución: %f segundos\n", cpu_time_used);

    printf("Compresión completada.\n");
    return 0;
}
