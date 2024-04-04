#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

struct Node {
    char character;
    struct Node* left, * right;
};

struct Node* buildHuffmanTree(char* data[], char* codes[], int size) {
    struct Node* root = malloc(sizeof(struct Node));
    if (root == NULL) {
        perror("Error al asignar memoria para el nodo raíz del árbol Huffman");
        exit(EXIT_FAILURE);
    }
    root->left = root->right = NULL;
    for (int i = 0; i < size; i++) {
        struct Node* current = root;
        char* code = codes[i];
        for (int j = 0; j < strlen(code); j++) {
            if (code[j] == '0') {
                if (current->left == NULL) {
                    current->left = malloc(sizeof(struct Node));
                    if (current->left == NULL) {
                        perror("Error al asignar memoria para un nodo del árbol Huffman");
                        exit(EXIT_FAILURE);
                    }
                    current->left->left = current->left->right = NULL;
                }
                current = current->left;
            }
            else if (code[j] == '1') {
                if (current->right == NULL) {
                    current->right = malloc(sizeof(struct Node));
                    if (current->right == NULL) {
                        perror("Error al asignar memoria para un nodo del árbol Huffman");
                        exit(EXIT_FAILURE);
                    }
                    current->right->left = current->right->right = NULL;
                }
                current = current->right;
            }
            else {
                fprintf(stderr, "Error: Código Huffman inválido\n");
                exit(EXIT_FAILURE);
            }
        }
        current->character = *data[i];
    }
    return root;
}

void decompressData(FILE* input, FILE* output, struct Node* root, int originalSize) {
    struct Node* current = root;
    int bit;
    int size = 0;
    while (size < originalSize) {
        bit = fgetc(input);
        if (bit == EOF) {
            fprintf(stderr, "Error: Archivo de entrada demasiado corto\n");
            exit(EXIT_FAILURE);
        }
        if (bit == '0' && current->left != NULL)
            current = current->left;
        else if (bit == '1' && current->right != NULL)
            current = current->right;

        if (current->left == NULL && current->right == NULL) {
            fputc(current->character, output);
            current = root;
            size++;
        }
    }
}

int main() {
    FILE* input = fopen("MergedTXT.txt", "r");
    if (input == NULL) {
        perror("Error al abrir el archivo comprimido");
        exit(EXIT_FAILURE);
    }
    FILE* output = fopen("decompressed.txt", "w");
    if (output == NULL) {
        perror("Error al crear el archivo descomprimido");
        exit(EXIT_FAILURE);
    }

    int originalSize;
    fscanf(input, "%d\n", &originalSize);

    char* data[MAX_TREE_HT];
    char* codes[MAX_TREE_HT];
    int size = 0;

    while (1) {
        data[size] = malloc(sizeof(char));
        if (data[size] == NULL) {
            perror("Error al asignar memoria para los datos comprimidos");
            exit(EXIT_FAILURE);
        }
        if (fscanf(input, "%c:", &(*data[size])) == EOF)
            break;
        codes[size] = malloc(MAX_TREE_HT * sizeof(char));
        if (codes[size] == NULL) {
            perror("Error al asignar memoria para los códigos Huffman");
            exit(EXIT_FAILURE);
        }
        fscanf(input, " %[^\n]\n", codes[size]);
        size++;
    }

    struct Node* root = buildHuffmanTree(data, codes, size);

    decompressData(input, output, root, originalSize);

    printf("El archivo se ha descomprimido correctamente.\n");

    fclose(input);
    fclose(output);

    // Liberar memoria
    for (int i = 0; i < size; i++) {
        free(data[i]);
        free(codes[i]);
    }
    return 0;
}