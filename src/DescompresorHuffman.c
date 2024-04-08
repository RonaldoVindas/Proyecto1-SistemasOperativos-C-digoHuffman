#include <stdio.h>
#include <stdlib.h>

typedef struct HuffmanNode {
    char data;
    unsigned frequency;
    struct HuffmanNode *left, *right;
} HuffmanNode;

// Función para reconstruir el árbol de Huffman desde el archivo binario
HuffmanNode* rebuildHuffmanTree(FILE *inputFile) {
    unsigned frequency[256];
    fread(frequency, sizeof(unsigned), 256, inputFile);

    // Reconstruir el árbol de Huffman
    // Crear nodos para cada caracter con su respectiva frecuencia
    HuffmanNode* nodes[256];
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            nodes[i] = (HuffmanNode*)malloc(sizeof(HuffmanNode));
            nodes[i]->data = (char)i;
            nodes[i]->frequency = frequency[i];
            nodes[i]->left = NULL;
            nodes[i]->right = NULL;
        } else {
            nodes[i] = NULL;
        }
    }

    // Construir el árbol de Huffman
    while (1) {
        // Encontrar los dos nodos con la menor frecuencia restantes
        int minIdx1 = -1, minIdx2 = -1;
        for (int i = 0; i < 256; i++) {
            if (nodes[i] != NULL) {
                if (minIdx1 == -1 || nodes[i]->frequency < nodes[minIdx1]->frequency) {
                    minIdx2 = minIdx1;
                    minIdx1 = i;
                } else if (minIdx2 == -1 || nodes[i]->frequency < nodes[minIdx2]->frequency) {
                    minIdx2 = i;
                }
            }
        }

        // Si solo queda un nodo, este será la raíz del árbol
        if (minIdx2 == -1)
            return nodes[minIdx1];

        // Crear un nuevo nodo interno con los dos nodos de menor frecuencia como hijos
        HuffmanNode* newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode));
        newNode->data = '$'; // Carácter especial para representar un nodo interno
        newNode->frequency = nodes[minIdx1]->frequency + nodes[minIdx2]->frequency;
        newNode->left = nodes[minIdx1];
        newNode->right = nodes[minIdx2];

        // Marcar los nodos usados como nulos para que no se consideren en futuras iteraciones
        nodes[minIdx1] = NULL;
        nodes[minIdx2] = newNode;
    }
}

// Función para decodificar los datos comprimidos y escribir el archivo de texto original
void decompressFile(FILE *inputFile, FILE *outputFile, HuffmanNode* root) {
    HuffmanNode* currentNode = root;
    int bitIndex = 7;
    char byte;

    while (fread(&byte, sizeof(char), 1, inputFile) != 0) {
        while (bitIndex >= 0) {
            if ((byte >> bitIndex) & 1) {
                currentNode = currentNode->right;
            } else {
                currentNode = currentNode->left;
            }

            if (currentNode->left == NULL && currentNode->right == NULL) {
                fprintf(outputFile, "%c", currentNode->data);
                currentNode = root;
            }

            bitIndex--;
        }
        bitIndex = 7;
    }
}

int main() {
    FILE *inputFile, *outputFile;
    inputFile = fopen("outputSerial.bin", "rb"); 
    outputFile = fopen("decompressed.txt", "w"); 

    if (inputFile == NULL || outputFile == NULL) {
        printf("Error al abrir el archivo.\n");
        exit(1);
    }

    // Reconstruir el árbol de Huffman desde el archivo binario
    HuffmanNode* root = rebuildHuffmanTree(inputFile);

    // Restablecer la posición del puntero del archivo después de leer las frecuencias
    fseek(inputFile, 0, SEEK_SET);

    // Decodificar los datos comprimidos y escribir el archivo de texto original
    decompressFile(inputFile, outputFile, root);

    // Cerrar archivos
    fclose(inputFile);
    fclose(outputFile);

    printf("Descompresión completada.\n");

    return 0;
}
