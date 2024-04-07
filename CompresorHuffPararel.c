#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


#define NUM_THREADS 4
#define NUM_FILES 100

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

// Estructura para datos de entrada de cada hilo
typedef struct {
    char file_name[256];
    int thread_id;
} ThreadData;

// Declaración de la función buildHuffmanTree
HuffmanNode* buildHuffmanTree(char data[], int frequency[], int size);

// Crear un nodo para el árbol de Huffman
HuffmanNode* createHuffmanNode(char data, unsigned frequency) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (node == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nodo Huffman.\n");
        exit(EXIT_FAILURE);
    }
    node->left = node->right = NULL;
    node->data = data;
    node->frequency = frequency;
    return node;
}

// Crear una cola de prioridad vacía
MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    if (minHeap == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la cola de prioridad (MinHeap).\n");
        exit(EXIT_FAILURE);
    }
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HuffmanNode**)malloc(capacity * sizeof(HuffmanNode*));
    if (minHeap->array == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el arreglo de nodos de la cola de prioridad.\n");
        free(minHeap);
        exit(EXIT_FAILURE);
    }
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

// Función para liberar la memoria de un árbol Huffman
void freeHuffmanTree(HuffmanNode* root) {
    if (root == NULL) {
        return;
    }
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}


void* compressFiles(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    int start_index = (data->thread_id - 1) * (NUM_FILES / NUM_THREADS);
    int end_index = data->thread_id * (NUM_FILES / NUM_THREADS);

    printf("Thread %d processing files from index %d to %d\n", data->thread_id, start_index, end_index - 1);

    // Simulated file processing
    for (int i = start_index; i < end_index; ++i) {
        printf("Thread %d processing file %d\n", data->thread_id, i + 1);
        // Add your file processing logic here
    }

    pthread_exit(NULL);
}

// Función de compresión de archivo para cada hilo
void* compressFileThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char inputFileName[256];
    sprintf(inputFileName, "LibrosTXT/%s.txt", data->file_name);

    int frequency[256] = {0}; // Inicializar la frecuencia de cada caracter a 0
    char character;

    FILE* inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Error al abrir el archivo: %s\n", inputFileName);
        pthread_exit(NULL);
    }

    // Calcular la frecuencia de cada caracter en el archivo de texto
    while ((character = fgetc(inputFile)) != EOF) {
        frequency[(unsigned char)character]++;
    }

    fclose(inputFile);

    // Crear arreglos para almacenar los caracteres y sus frecuencias
    char charData[256];
    int freq[256];
    int index = 0;

    // Llenar los arreglos con los caracteres y sus frecuencias
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            charData[index] = (char)i;
            freq[index] = frequency[i];
            index++;
        }
    }

    // Construir el árbol de Huffman
    HuffmanNode* root = buildHuffmanTree(charData, freq, index);

    // Escribir la salida comprimida en el archivo output.bin
    FILE* outputFile = fopen("output.bin", "ab");
    if (outputFile == NULL) {
        fprintf(stderr, "Error al abrir el archivo output.bin\n");
        pthread_exit(NULL);
    }

    // Escribir los datos comprimidos en el archivo .bin
    inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Error al abrir el archivo: %s\n", inputFileName);
        fclose(outputFile);
        pthread_exit(NULL);
    }

    int codeBuffer = 0;
    int bitCount = 0;
    while ((character = fgetc(inputFile)) != EOF) {
        for (int i = 0; i < index; i++) {
            if (charData[i] == character) {
                // Escribir el índice (código) comprimido en el archivo
                codeBuffer = (codeBuffer << 1) | freq[i];
                bitCount++;
                if (bitCount == 8) {
                    fputc(codeBuffer, outputFile);
                    codeBuffer = 0;
                    bitCount = 0;
                }
                break;
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);

    // Liberar memoria del árbol Huffman
    freeHuffmanTree(root);

    printf("Compresión completada para el archivo %s en hilo %d.\n", data->file_name, data->thread_id);

    pthread_exit(NULL);
}

// Implementación de la función buildHuffmanTree
HuffmanNode* buildHuffmanTree(char data[], int frequency[], int size) {
    MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i) {
        minHeap->array[i] = createHuffmanNode(data[i], frequency[i]);
    }

    minHeap->size = size;
    buildMinHeap(minHeap);

    while (!isSizeOne(minHeap)) {
        HuffmanNode* left = extractMin(minHeap);
        HuffmanNode* right = extractMin(minHeap);

        HuffmanNode* newNode = createHuffmanNode('$', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        insertMinHeap(minHeap, newNode);
    }

    HuffmanNode* root = extractMin(minHeap);
    free(minHeap->array);
    free(minHeap);

    return root;
}

int main() {
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    // Crear y ejecutar hilos para procesar diferentes archivos en paralelo
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].thread_id = i + 1;
        pthread_create(&threads[i], NULL, compressFiles, (void*)&thread_data[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Todos los archivos han sido comprimidos.\n");

    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución %f segundos.\n", cpu_time_used);

    return 0;
}
