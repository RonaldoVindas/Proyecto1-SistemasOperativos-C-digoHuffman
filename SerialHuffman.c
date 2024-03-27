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

//#define MAX_TREE_HT 100
#define MAX_FILENAME_LENGTH 256
#define MAX_CONTENT_LENGTH 10000            //Tamaño máximo del contenido de cada archivo
#define NUM_THREADS 4                       // Número de hilos a utilizar


// ================================================================================================================

//1) =================== Creación del Struct de Árbol Huffman ===================



struct Node{                                                                    //Nodo del Aŕbol Heap                                                           
    char character;
    unsigned frequency;                                                                     
    struct heapNode *left, *right;                                              //Cada nodo enlaza con un hijo izquierdo y uno derecho.
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

    struct Node* temp = (struct Node*)malloc( sizeof(struct HuffmanTree) ); 
  
    temp-> left = temp ->right = NULL;                                         //Los hijos( izq, der) son nulos pues al momento de crearse el nodo, aún no tiene nodos hijos.
    temp-> character = character; 
    temp-> frequency = frequency; 
  
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

    tree->size = 0;                                                            //Tamaño al crearse es de 0
    tree->capacity = capacity; 
    tree->array = (struct Node**)malloc( tree->capacity * sizeof(struct Node*)); 
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
    Función que intercambia la posición de dos nodos.
    -Entradas: Nodo a:  Nodo 1 a intercambiar, Nodo b: Nodo 2 a intercambiar.
    -Salidas: Struct del Árbol Huffman con sus datos asignados.
    -Restricciones: N/A
    */
    
    int smallest = index; 
    int left = 2 * index + 1; 
    int right = 2 * index + 2; 
  
    if (left < tree->size 
        && tree->array[left]->frequency 
               < tree->array[smallest]->frequency) 
        smallest = left; 
  
    if (right < tree->size 
        && tree->array[right]->frequency 
               < tree->array[smallest]->frequency) 
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

//insertTreee (insertMinHeap)

//buildTree (buildMinHeap)

//printArray(printArr)

//isLeaf(isLeaf)

//createNBuildTree(createAndBuildMinHeap)

//buildHuffmanTree(buildHuffmanTree)

//codesHuffman(HuffmanCodes)

//printCodes(printCodes)



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
    
    if ((directory = opendir(inputDirectory)) != NULL) {                                                       //Se abre la carpeta de archivos     
        FILE *output = fopen(outputFile, "w");                                                                 //Se abre el archivo de salida en modo escritura                
        if (output == NULL) {
            perror("Error al abrir el archivo de salida TXT");
            exit(EXIT_FAILURE);
        }
        while ((ent = readdir(directory)) != NULL) {                                                           //Itera sobre cada archivo de la carpeta hasta llegar a NULL           
            if (ent->d_type == DT_REG && strstr(ent->d_name, ".txt") != NULL) {                                //Se revisa que el archivo actual sea un archivo TXT.
                sprintf(inputFile, "%s/%s", inputDirectory, ent->d_name);                                      //Hace la ruta completa del archivo de entrada           
                FILE *input = fopen(inputFile, "r");                                                           //Se abre el archivo de entrada en modo lectura       

                if (input == NULL) {
                    perror("Error al abrir el archivo de entrada");
                    exit(EXIT_FAILURE);
                }  

                while (fgets(content, MAX_CONTENT_LENGTH, input) != NULL) { 
                    fputs(content, output);                                                                     //Lee el contenido del archivo y escribe en el archivo TXT nuevoo
                }                
                fclose(input);                                                                                  //Se debe cerrar cada archivo de entrada
            }
        }
        
        fclose(output);                                                                                         //Se debe cerrar el archivo de salida         
        closedir(directory);                                                                                    //Se cierra el directorio
    } else {
        perror("Error al abrir la carpeta");
        exit(EXIT_FAILURE);
    }
}


//3)=================== Lista de Aparición y Cálculo de Frecuencia de Palabras ===================


void countCharacters(char *letters, double *freqArr/*FILE *inputFile*/){
    /*
    Función que cuenta la cantidad de apariciones de caractéres y en base a ello calcula la frecuencia de aparición de cada uno.
    -Entradas: N/A
    -Salidas: Arreglo con tasas de Frecuencias
    -Restricciones: N/A
    */

    
                                                                                    
    int lettersCount[111] = {0};
    
    int characterCount = 0;
    int currentCharacter;

    
    FILE *inputFile = fopen("MergedTXT", "r"); 
    if (inputFile == NULL) { 
        printf("No se pudo abrir el archivo %s .", inputFile); 
    } 
    
    while ((currentCharacter = fgetc(inputFile)) != EOF) {                                                          //Cuenta la cantidad de caracteres                                       
        for (int i = 0; i < 110; i++) {                                                                             //Se Itera sobre todo el arreglo de caractéres permitidos                 
            if (currentCharacter == letters[i]) {                                                                   //Si la letra está en el arreglo, se aumenta el contador                  
                lettersCount[i]++;
            }
        }
        characterCount++;                                                                                           //Aumenta la cantidad de caractéres en 
    }

    fclose(inputFile);                                                                                              //Se cierra el archivo

    printf("The file %s has %d characters\n ", 
           inputFile, characterCount); 

    
    for (int i = 0; i < 110; i++) {
        freqArr[i] = (double)lettersCount[i] / characterCount * 100;                                               //Calcula la frecuencia de aparición en porcentaje
        printf("Caracter '%c' aparece %d veces, frecuencia: %.2f%%, precision: %f \n", letters[i], lettersCount[i], freqArr[i], freqArr[i]); 
    }
                                                                                            
    
    
}
//4)=================== Compresión ===================


//=================== MAIN ===================
int main(){
    mergeFiles("/home/ronaldo/Descargas/Proyecto1-Operativos/Proyecto1-SistemasOperativos-C-digoHuffman/Libros TXT Proyecto", "MergedTXT");                 //Nota: Recuerde cambiar la ruta por una relativa

    //FILE *inputFile = fopen("MergedTXT", "r");


    char letters[111 /*256*/] = {                                                                                         //Nota, arreglos dentro de funciones deben indicar el tamaño al declararse, sino saldrá un error "Incomplete Types".
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        /*'á',   'é',     'í',    'ó',   'ú',    'ü',    'ñ',    'Á',     'É',   'Í',    'Ó',    'Ú',    'Ü',     'Ñ', */       //Hay problemas reconociendo caractéres especiales
        '\0x3B', '\xE9', '\xED', '\xF3', '\xFA', '\xFC', '\xF1', '\xC1', '\xC9', '\xCD', '\xD3', '\xDA', '\xDC', '\xD1',         //Valores hexadecimales de caractéres especiales 
        ',', '.', /*';',*/' \x3B', ':', '!', '?', /*'¡'*/ '\xA1', /*'¿',*/'\xBF' , '\'', '"', '(', ')', '-', '_', 
        '[', ']', '{', '}', '<', '>', '+', '=', '*', '&', '^', '%', '$', '#', 
        '@', '~', '/', '\\', '|'
    }; 

    double freqArr[111] = {0};

     
    countCharacters(letters, freqArr);
    //fclose(inputFile); 
    return 0;
}



