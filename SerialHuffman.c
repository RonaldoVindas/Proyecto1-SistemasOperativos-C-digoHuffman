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



//=================== Lectura de Carpeta de Archivos ===================

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
    
    if ((directory = opendir(inputDirectory)) != NULL) {                                                        //Se abre la carpeta de archivos     
        FILE *output = fopen(outputFile, "w");                                                                  //Se abre el archivo de salida en modo escritura                
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
        
        fclose(output);                                                                                        //Se debe cerrar el archivo de salida         
        closedir(directory);                                                                                    //Se cierra el directorio
    } else {
        perror("Error al abrir la carpeta");
        exit(EXIT_FAILURE);
    }
}



//=================== Lista de Aparición y Cálculo de Frecuencia de Palabras ===================


int countCharacters(){
    //char letters[29];                                                                               //Nota, arreglos dentro de funciones deben indicar el tamaño al declararse, sino saldrá un error "Incomplete Types".
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
  
    for (char c = getc(inputFile); c != EOF; c = getc(inputFile))                                                 //Cuenta la cantidad de caracteres
        characterCount = inputFile + 1; 
  
     
    fclose(inputFile);                                                                                            //Se cierra el archivo
  
    /*
    printf("The file %s has %d characters\n ", 
           fp, characterCount); */
}




//=================== Compresión ===================


//=================== MAIN ===================
int main(){
 mergeFiles("/home/ronaldo/Descargas/Proyecto1-Operativos/Proyecto1-SistemasOperativos-C-digoHuffman/Libros TXT Proyecto", "MergedTXT");
 countCharacters();

}



