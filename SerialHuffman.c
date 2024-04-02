/*
Instituto Tecnológico de Costa Rica | Ing. Computación

Principios de Sistemas Operativos - Proyecto #1 - Cómpresión por Código Huffman: Versión Serial

Elaborado Por:
Adrián Herrera
Rebeca Madrigal
Ronaldo Vindas

*/

#define _DEFAULT_SOURCE                     // DT_REG is not part of ISO C11 extensions, so use this, You can use feature macros to enable additional extensions. As readdir manual mentions, you need _DEFAULT_SOURCE macro to enable file type constants. You can do this in the source code before including dirent.h

#include <stdio.h> 
#include <stdlib.h> 
#include <dirent.h>                         //Librería POSIX
#include <string.h>

#define MAX_TREE_HT 256
#define MAX_FILENAME_LENGTH 256
#define MAX_CONTENT_LENGTH 10000            //Tamaño máximo del contenido de cada archivo
#define NUM_THREADS 4                       // Número de hilos a utilizar
#define BLOCK_SIZE 1024 

// ================================================================================================================

//1) =================== Creación del Struct de Árbol Huffman ===================

struct HuffmanTree* tree;

struct Node{                                                                    //Nodo del Aŕbol Heap                                                           
    char character;
    unsigned frequency;                                                                     
    struct Node *left, *right;                                              //Cada nodo enlaza con un hijo izquierdo y uno derecho.
};


struct HuffmanTree{                                                             //Árbol Huffman
    unsigned size;                                                              //Tamaño del Árbol actual
    unsigned capacity;                                                          //Capacidad?
    struct Node** array;                                                       //Arreglo de NOdos
}; 



//=================== Métodos de Árbol Huffman ===================

struct Node* newNode(char character, unsigned frequency){ 
    /*
    Función que crea un nodo nuevo del Árbol Huffman (llamado MinHeap o Heap). Solo crea el nodo, no lo inserta también en el árbol
    -Entradas: character: caracter ASCII, frequency: número de apariciones del caractér
    -Salidas: Nodo con sus datos asignados
    -Restricciones: N/A
    */

    struct Node* temp = (struct Node*)malloc(sizeof(struct Node)); 
    if (temp == NULL) {
        perror("Error al asignar memoria para un nuevo nodo");
        exit(EXIT_FAILURE);
    }
    temp->left = temp->right = NULL;                                         
    temp->character = character; 
    temp->frequency = frequency;
  
    return temp; 
} 

struct HuffmanTree* createTree(unsigned capacity) { 
    /*
    Función que crea un Árbol Huffman de una capacidad especificada.
    -Entradas: capacity: capacidad del árbol
    -Salidas: Struct del Árbol Huffman con sus datos asignados.
    -Restricciones: N/A
    */
    struct HuffmanTree* tree = (struct HuffmanTree*)malloc(sizeof(struct HuffmanTree)); 
    if (tree == NULL) {
        perror("Error al asignar memoria para el árbol Huffman");
        exit(EXIT_FAILURE);
    }
    tree->size = 0;                                                            
    tree->capacity = capacity; 
    tree->array = (struct Node**)malloc( tree->capacity * sizeof(struct Node*)); 
    if (tree->array == NULL) {
        free(tree);
        perror("Error al asignar memoria para el array de nodos del árbol Huffman");
        exit(EXIT_FAILURE);
    }
    return tree; 
}


void swapNodes( struct Node** a, struct Node** b){
    /*
    Función que intercambia la posición de dos nodos.
    -Entradas: Nodo a:  Nodo 1 a intercambiar, Nodo b: Nodo 2 a intercambiar.
    -Salidas: Struct del Árbol Huffman con sus datos asignados.
    -Restricciones: N/A
    */

    struct Node* temp = *a;                                                    
    *a = *b; 
    *b = temp; 
}

void minHeapify(struct HuffmanTree* tree, int index) { 
    /*
    Función que balancea los valores de los nodos hijos de forma que el nodo hijo sea igual o mayor al nodo padre.
    -Entradas: Árbol a:  árbol a tratar, index: índice del nodo menor
    -Salidas: N/A
    -Restricciones: N/A
    */
    
    int smallest = index; 
    int left = 2 * index + 1; 
    int right = 2 * index + 2; 
    if (left < tree->size && tree->array[left]->frequency < tree->array[smallest]->frequency) 
        smallest = left; 
  
    if (right < tree->size && tree->array[right]->frequency < tree->array[smallest]->frequency) 
        smallest = right; 

    if (smallest != index) { 
        swapNodes(&tree->array[smallest], &tree->array[index]); 
        minHeapify(tree, smallest); 
    } 
} 


int treeSizeOne(struct HuffmanTree* tree) { 
    /*
    Función que revisa si el tamaño del árbol es de 1.
    -Entradas: tree: Árbol Huffman
    -Salidas: Booleano true si su tamaño es 1, False sino
    -Restricciones: N/A
    */
  
    return (tree->size == 1); 
} 

struct Node* extractMin(struct HuffmanTree* tree){ 
    /*
    Función que averigua el nodo del árbol con el valor mínimo 
    -Entradas: tree: Árbol Huffman
    -Salidas: Nodo Mínimo del Árbol
    -Restricciones: N/A
    */
  
    struct Node* temp = tree->array[0]; 
    tree->array[0] = tree->array[tree->size - 1]; 
    --tree->size; 
    minHeapify(tree, 0); 
  
    return temp; 
} 

void insertTree(struct HuffmanTree* tree, struct Node* node){ 
    /*
    Función que inserta un nodo en el Árbol Huffman 
    -Entradas: tree: Árbol Huffman
    -Salidas: Árbol con nuevo nodo insertado
    -Restricciones: N/A
    */
  
    ++tree->size; 
    int i = tree->size - 1; 
  
    while (i > 0 && node->frequency < tree->array[(i - 1) / 2]->frequency) { 
        tree->array[i] = tree->array[(i - 1) / 2]; 
        i = (i - 1) / 2; 
    } 
    tree->array[i] = node;
} 

void buildTree(struct HuffmanTree* tree){ 
    /*
    Función que construye el árbol Huffman y lo balancea
    -Entradas: tree: Árbol Huffman
    -Salidas: N/A
    -Restricciones: N/A
    */
  
    int n = tree->size - 1; 
    int i;  
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(tree, i); 
} 

void printTree(int arr[], int n){
    /*
    Función que imprime el árbol Huffman
    -Entradas: arr: Árbol Huffman (arreglo), n: tamaño del árbol arreglo
    -Salidas: N/A
    -Restricciones: N/A
    */ 
    int i; 
    for (i = 0; i < n; ++i) 
        printf("%d", arr[i]); 
    printf("\n"); 
} 

int isLeaf(struct Node* node){ 
    /*
    Función que imprime el árbol Huffman
    -Entradas: node: Nodo a comprobar
    -Salidas: Booleano True: Si el nodo no tiene hijos, Booleano False: Si el nodo tiene hijos
    -Restricciones: N/A
    */ 
  
    return !(node->left) && !(node->right); 
} 

struct HuffmanTree* createAndBuildTree(char data[], int frequency[], int size) { 
    /*
    Función que combina las funciones de crear y construcción del árbol.
    -Entradas: data: arreglo de caracteres, frequency: arreglo de frecuencia de aparición de caractéres, size: Tamaño de los arreglos
    -Salidas: Booleano True: Si el nodo no tiene hijos, Booleano False: Si el nodo tiene hijos
    -Restricciones: N/A
    */ 
   
    struct HuffmanTree* tree = createTree(size); 
  
    for (int i = 0; i < size; ++i) 
        tree->array[i] = newNode(data[i], frequency[i]); 
  
    tree->size = size; 
    buildTree(tree); 
  
    return tree; 
} 


struct Node* buildHuffmanTree(char data[], int freq[], int size) { 
    /*
    Función que crea el árbol Huffman.
    -Entradas: data: arreglo de caracteres, frequency: arreglo de frecuencia de aparición de caractéres, size: Tamaño de los arreglos
    -Salidas: Árbol Huffman creado
    -Restricciones: N/A
    */ 

    struct Node *left, *right, *top; 
  
    
    struct HuffmanTree* tree = createAndBuildTree(data, freq, size);                             //Se Crea un árbol de un mimsmo tamaño y capacidad. 
    
    while (!treeSizeOne(tree)) {                                                                      //Se itera siempre y cuando el tamaño del árbol no sea 1
  
        left = extractMin(tree);                                                                    //Se extraen los dos caracterés con menor frecuencia del árbol                    
        right = extractMin(tree); 
         
        top = newNode('$', left->frequency + right->frequency);                                     //Se crea un nuevo nodo con una frecuencia equivalente a la suma de las frecuencias de los dos nodos anteriores. Se le asigna el caracter especial "$"
  
        top->left = left;                                                                           //Se les asigna como hijos a los nodos creados anteriormente
        top->right = right; 
  
        insertTree(tree, top);                                                           
    } 
    return extractMin(tree);                                                                        //El último nodo será la raíz y se completa el árbol
} 

void printCodes(struct Node* root, int codeArray[], int top) { 
    /*
    Función que crea el código binario para cada caractér.
    -Entradas: root: nodo raíz, arr: arreglo que almacenará el código Huffman binario, size: Tamaño de los arreglos
    -Salidas: Árbol Huffman creado
    -Restricciones: N/A
    */ 
  
    if (root->left) {                                                                                //Asigna "0" a los nodos izquierdos
        codeArray[top] = 0; 
        printCodes(root->left, codeArray, top + 1); 
    } 
  
    if (root->right) {                                                                               //Asigna "1" a los nodos izquierdos           
        codeArray[top] = 1; 
        printCodes(root->right, codeArray, top + 1); 
    } 
  
    if (isLeaf(root)) {                                                                             //Si el nodo es Hoja, entonces tiene un caractér, se imprime el caractér y su código binario
        printf("%c: ", root->character); 
        printTree(codeArray, top); 
    } 

} 

void HuffmanCodes(char *data, int *freq, int size){ 
    /*
    Función que crea el árbol Huffman y crea el código binario comprimido.
    -Entradas: data: arreglo de caracteres, frequency: arreglo de frecuencia de aparición de caractéres, size: Tamaño de los arreglos
    -Salidas: Código Huffman creado
    -Restricciones: N/A
    */ 
    struct Node* root = buildHuffmanTree(data, freq, size);                             //Se construye el árbol
    int arr[MAX_TREE_HT], top = 0; 
    printCodes(root, arr, top);                                                         //Se imprime el código binario
} 




//2)=================== Lectura de Carpeta de Archivos ===================

void mergeFiles(const char *inputDirectory, const char *outputFile){
    /*
    Función que combina todos los Txt de una carpeta en un solo archivo TXT.
    -Entradas: inputDirectory: dirección de la carpeta. outputFile: Nombre de archivo TXT a crear
    -Salidas: Archivo TXT con la combinación de los demás archivos
    -Restricciones: Solo admite archivos TXT
    */

    DIR *directory;
    struct dirent *ent;
    char inputFile[MAX_FILENAME_LENGTH];
    char content[MAX_CONTENT_LENGTH];
    char separator[] = "€\n";
    if ((directory = opendir(inputDirectory)) != NULL) {                                                    
        FILE *output = fopen(outputFile, "w");                                                             
        if (output == NULL) {
            perror("Error al abrir el archivo de salida TXT");
            exit(EXIT_FAILURE);
        }
        while ((ent = readdir(directory)) != NULL) {                                                        
            if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL) {                             
                sprintf(inputFile, "%s/%s", inputDirectory, ent->d_name);                                   
                FILE *input = fopen(inputFile, "r");                                                         
                if (input == NULL) {
                    perror("Error al abrir el archivo de entrada TXT");
                    fclose(output);
                    exit(EXIT_FAILURE);
                }
                while (fgets(content, sizeof(content), input) != NULL) {                                     
                    fputs(content, output);                                                                  
                }
                fputs(separator, output); // Agregar símbolo de separación al final del archivo
                fclose(input);                                                                               
            }
        }
        fclose(output);                                                                                       
        closedir(directory);                                                                                  
    } else {                                                                                                  
        perror("Error al abrir el directorio");
        exit(EXIT_FAILURE);
    }
}


//3)=================== Lista de Aparición y Cálculo de Frecuencia de Palabras ===================


void countCharacters(char *letters, double *freqArr, int *lettersCount) {
    /*
    Función que cuenta la cantidad de apariciones de caractéres y en base a ello calcula la frecuencia de aparición de cada uno.
    -Entradas: N/A
    -Salidas: Arreglo con tasas de Frecuencias
    -Restricciones: N/A
    */

    int characterCount = 0;
    int currentCharacter;

    FILE *inputFile = fopen("MergedTXT", "r");
    if (inputFile == NULL) {
        perror("Error al abrir el archivo MergedTXT");
        exit(EXIT_FAILURE);
    }

    while ((currentCharacter = fgetc(inputFile)) != EOF) {
        for (int i = 0; i < 93; i++) {
            if (currentCharacter == letters[i]) {
                lettersCount[i]++;
            }
        }
        characterCount++;
    }

    fclose(inputFile);

    printf("El archivo MergedTXT tiene %d caracteres\n ", characterCount);

    for (int i = 0; i < 93; i++) {
        freqArr[i] = (double)lettersCount[i] / characterCount * 100;
        printf("Caracter '%c' aparece %d veces, frecuencia: %.2f%%, precision: %f \n", letters[i], lettersCount[i], freqArr[i], freqArr[i]);
    }
}                                                                                           
    

char *extractUniqueCharacters(const char *filename, int *numUniqueChars) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }
    int uniqueChars[256] = {0};
    int numChars = 0;
    int currentCharacter;
    while ((currentCharacter = fgetc(file)) != EOF) {
        uniqueChars[currentCharacter] = 1;
    }
    fclose(file);
    for (int i = 0; i < 256; i++) {
        if (uniqueChars[i]) {
            numChars++;
        }
    }
    char *uniqueCharArr = (char *)malloc(numChars * sizeof(char));
    if (uniqueCharArr == NULL) {
        perror("Error al asignar memoria para almacenar caracteres únicos");
        exit(EXIT_FAILURE);
    }
    int index = 0;
    for (int i = 0; i < 256; i++) {
        if (uniqueChars[i]) {
            uniqueCharArr[index++] = i;
        }
    }
    *numUniqueChars = numChars; 
    return uniqueCharArr;
}

void replaceNonUTF8Characters(char *str) {
    while (*str) {
        unsigned char current = (unsigned char)(*str);
        if (current >= 0x80) { 
            if (current == 0xE1) { 
                *str = 'a';
            } else if (current == 0xC1) { 
                *str = 'A';
            } else if (current == 0xE9) { 
                *str = 'e';
            } else if (current == 0xC9) { 
                *str = 'E';
            } else if (current == 0xED) { 
                *str = 'i';
            } else if (current == 0xCD) { 
                *str = 'I';
            } else if (current == 0xF3) { 
                *str = 'o';
            } else if (current == 0xD3) { 
                *str = 'O';
            } else if (current == 0xFA) { 
                *str = 'u';
            } else if (current == 0xDA) { 
                *str = 'U';
            } else { //Si es un caracter lo remplaza por "-"
                *str = '-';
            }
        }
        str++;
    }
}

//4)=================== Compresión ===================


void generateHuffmanCodes(struct Node* root, char *code[], char *currentCode, int index) {
    if (root->left) {
        currentCode[index] = '0';
        generateHuffmanCodes(root->left, code, currentCode, index + 1);
    }
    if (root->right) {
        currentCode[index] = '1';
        generateHuffmanCodes(root->right, code, currentCode, index + 1);
    }
    if (isLeaf(root)) {
        currentCode[index] = '\0';
        code[root->character] = strdup(currentCode); 
    }
}

void writeCompressedData(const char *inputFileName, char **code) {
    FILE *input = fopen(inputFileName, "r");
    if (input == NULL) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    char outputFileName[strlen(inputFileName) + 6]; 
    sprintf(outputFileName, "%s.txt", inputFileName); 
    FILE *output = fopen(outputFileName, "w");
    if (output == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(input);
        exit(EXIT_FAILURE);
    }
    int currentCharacter;
    while ((currentCharacter = fgetc(input)) != EOF) {
        if (currentCharacter >= 0 && currentCharacter < 111 && code[currentCharacter] != NULL) {
            fputs(code[currentCharacter], output);
        } else {
            fprintf(stderr, "Error: Caracter comprimido no encontrado para el código ASCII %d\n", currentCharacter);
        }
    }
    for (int i = 0; i < 111; ++i) {
        if (code[i] != NULL) {
            free(code[i]);
        }
    }
    fclose(input);
    fclose(output);
}


void freeTree(struct HuffmanTree* tree) {
    for (int i = 0; i < tree->size; ++i) {
        free(tree->array[i]);
    }
    free(tree->array);
    free(tree);
}



//=================== MAIN ===================
int main(){
    //mergeFiles("/home/rebecamadrigal/Escritorio/Proyecto1-SistemasOperativos-C-digoHuffman/Libros TXT Proyecto", "MergedTXT");                 //Nota: Recuerde cambiar la ruta por una relativa
    //FILE *inputFile = fopen("MergedTXT", "r");
    //char letters[94/*256*/] = {                                                                                         //Nota, arreglos dentro de funciones deben indicar el tamaño al declararse, sino saldrá un error "Incomplete Types".
    //    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
    //    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
    //    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
    //    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    //    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    //    '[', ']', '{', '}', '<', '>', '+', '=', '*', '&', '^', '%', '$', '#', 
    //    '@', '~', '/', '\\', '|',/*'€'*/
    //}; 
    //mergeFiles("/home/rebecamadrigal/Escritorio/Proyecto1-SistemasOperativos-C-digoHuffman/Libros TXT Proyecto", "MergedTXT");
    FILE *fileIn = fopen("MergedTXT", "r");
    if (fileIn == NULL) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }
    FILE *fileOut = fopen("MergeMod.txt", "w");
    if (fileOut == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(fileIn);
        exit(EXIT_FAILURE);
    }
    size_t bytesRead; 
    unsigned char buffer[4096];
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fileIn)) > 0) {
        replaceNonUTF8Characters(buffer);
        fwrite(buffer, 1, bytesRead, fileOut);
    }
    fclose(fileIn);
    fclose(fileOut);
    printf("Se han reemplazado los caracteres no UTF-8 en el archivo y se ha creado MergeMod.txt.\n");

/*
    int numUniqueChars; 
    char *uniqueCharArr = extractUniqueCharacters("letters.txt", &numUniqueChars);
    int lettersCount[94] = {0};
    double freqArr[94] = {0};
    countCharacters(uniqueCharArr, freqArr, lettersCount);
    struct Node* root = buildHuffmanTree(uniqueCharArr, lettersCount, numUniqueChars);
    int arr[MAX_TREE_HT], top = 0;
    printCodes(root, arr, top);
    char *code[256] = {NULL};
    char currentCode[MAX_TREE_HT];
    generateHuffmanCodes(root, code, currentCode, 0);
    writeCompressedData("MergedTXT", code);
    free(uniqueCharArr);
    freeTree(tree);

*/

    return 0;
}