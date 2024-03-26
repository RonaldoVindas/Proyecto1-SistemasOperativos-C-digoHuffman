/*
Instituto Tecnológico de Costa Rica | Ing. Computación

Principios de Sistemas Operativos - Proyecto #1

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


// ================================================================================================================


//=================== Estructura de Árbol Huffman ===================



//=================== Lista de Aparición y Cálculo de Frecuencia de Palabras ===================


int countCharacters(){
    char letters[30000] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'á', 'é', 'í', 'ó', 'ú', 'ü', 'ñ', 'Á', 'É', 'Í', 'Ó', 'Ú', 'Ü', 'Ñ',
    ',', '.', ';', ':', '!', '?', '¡', '¿', '\'', '"', '(', ')', '-', '_', 
    '[', ']', '{', '}', '<', '>', '+', '=', '*', '&', '^', '%', '$', '#', 
    '@', '~', '/', '\\', '|'};                                                                               //Nota, arreglos dentro de funciones deben indicar el tamaño al declararse, sino saldrá un error "Incomplete Types".
    int lettersCount[8];
    int characterCount = 0;
    int currentCharacter;
    //int index;
    //int isUnique;
    //char inputFile[MAX_FILENAME_LENGTH];
     
    FILE *inputFile = fopen("MergedTXT", "r"); 
    if (inputFile == NULL) { 
        printf("Could not open file %s", inputFile); 
        return 0; 
    } 

    
    
    for (char c = getc(inputFile); c != EOF; c = getc(inputFile)){                                                 //Cuenta la cantidad de caracteres
        for(int i = 0; i<8; i++){
            if(c == letters[i]){
                lettersCount[i] = lettersCount[i] + 1;
            }
        }
        characterCount = characterCount + 1; 
  
    }

    
    fclose(inputFile);                                                                                            //Se cierra el archivo
  
    
    printf("The file %s has %d characters\n ", 
           inputFile, characterCount); 

    for(int i = 0; i<8; i++){
        printf("The file %c has %d characters\n ", letters[i], lettersCount[i]); 
     
    }
}
//=================== Compresión ===================


//=================== MAIN ===================
int main(){
 mergeFiles("/home/ronaldo/Descargas/Proyecto1-Operativos/Proyecto1-SistemasOperativos-C-digoHuffman/Libros TXT Proyecto", "MergedTXT");
 countCharacters();

}



