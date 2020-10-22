#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */

int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	// Complete the function

	int bytes = 0;
	char buffer[nBytes]; //No se va a usar mas espacio (?)
	
	if (origin == NULL) //Comprobacion de archivo no nulo
		return -1;

	//Version con fread y fwrite (byte a byte)

	while (bytes < nBytes && (c = fread(buffer, 1, 1, origin)) > 0) { //Si el numero de bytes escritos no supera el dado, o no se lee
								
				ret = fwrite (buffer, 1, c, destination); //buffer, 1 byte, tamaño total, salida
				
				if (ret != c) //Comprobacion de errores (?)
				{
					fclose(origin);
					fclose(destination);
					return -1;
				}

				bytes++;
			}

	return bytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	// Complete the function
	char* fileBuffer;

	//Allocate memory
	fseek(file, 0, SEEK_END);
	long fileSize = ftell (file);
	rewind(file);

	fileBuffer = (char*) malloc(sizeof(char)*fileSize);
	if (fileBuffer == NULL)
		{
			return NULL;
		}

	//Store the content

	for (int i = 0; i < fileSize; i++)
	{
		fileBuffer[i] = getc(file);
	}

	fclose(file);

	return fileBuffer; //return !NULL?
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry* readHeader(FILE * tarFile, stHeaderEntry **header, int *nFiles)
{
	// Complete the function

	stHeaderEntry* array = NULL;
	int nr_files = 0;

	if (fread(&nr_files, sizeof(int), 1, tarFile) == 0) //nr_files como buffer, sizeof(int) por ser mirarse numero de archivos, 1, tarFile como archivo de lectura
	{
		printf("ERROR\n");
		return NULL;
	}

	//Lectura de numero de ficheros del tarFile y se vuelca en nrfiles

	array = malloc(sizeof(stHeaderEntry)*nr_files);

	for (int i = 0; i < nr_files; i++) //Para cada archivo
	{
		//hacer cosas
	}



	//Lectura de metainformacion de tarfile y se vuelca en array

	(*nFiles) = nr_files;
	(*header) = array;

	return (EXIT_SUCCESS);
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	// Complete the function

	//Se abre archivo mtar (Open?)

	//Se reserva memoria para el array de stHeaderEntry

	//Se deja hueco para la cabecera para ir a donde empiezan los datos

	for (int i = 0; i < nFiles; i++) //Para cada archivo
 	{
		//abrir inputFile
		
		copynFile(inputFile, tarFile, INT_MAX); //Viene asi en las transparencias

		close(inputFile);

		//Rellenar stHeaderEntry con la ruta y tamaño de inputFile
	}

	//Volver al byte 0

	//Escribir numero de ficheros, nFiles

	//free cosas

	//close(tarFile);


	if (0)
		return EXIT_FAILURE;

	if (1)
		return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	// Complete the function
	return EXIT_FAILURE;
}
