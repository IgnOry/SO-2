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

	int bytes = 0, c = 0, ret = 0;

	//Da error
	char* buffer; //No se va a usar mas espacio (?)
	

	if (origin == NULL) //Comprobacion de archivo no nulo
		return -1;

	//Version con fread y fwrite (byte a byte)

	buffer = (char*) malloc(sizeof(char)*nBytes);

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

		printf("debug\n");

		if ((array[i].name == loadstr(tarFile)) == NULL) //name es campo del struct stHeaderEntry
		//Si es null, es que no se ha leido bien, a dar error
		{ 
			printf("ERROR: loading string failed.\n");
			return NULL;
		}

		int auxSize = 0;

		fread(&auxSize, sizeof(unsigned int), 1, tarFile); //Lee el tamaño del archivo, y se guarda en variable auxiliar
		array[i].size = auxSize; //size es campo del struct
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

	FILE* input;
	FILE* output;
	stHeaderEntry* header;

	int copiedBytes = 0, headerBytes = 0, i = 0;

	//Se abre archivo mtar (Open?)

	//Se reserva memoria para el array de stHeaderEntry

	header = malloc(sizeof(stHeaderEntry) * nFiles);

	headerBytes += sizeof(int); //Numero de archivos?
	headerBytes += nFiles * sizeof(unsigned int); //Espacio necesario para cada archivo?

	//Se deja hueco para la cabecera para ir a donde empiezan los datos

	for (int i = 0; i < nFiles; i++) //Para cada archivo
		{
			headerBytes += strlen(fileNames[i] + 1); //+1 para fin de archivo?
		}

	//Abrir archivo de salida

	output = fopen(tarName, "w"); //"w" para escritura
	fseek(output, headerBytes, SEEK_SET); //Para no comerse informacion previa, se pone el puntero despues de la informacion ya guardada

	//Lectura de datos

	for (int i = 0; i < nFiles; i++) //Para cada archivo
 	{
		//abrir inputFile
		if ((input = fopen(fileNames[i], "r")) == NULL) //"r" para lectura
		//Si da null, es que hay error
		{
			printf("ERROR: file does not exist: %s \n", fileNames[i]);
			return EXIT_FAILURE;
		}
		
		//AQUI DENTRO PETA

		copiedBytes = copynFile(input, output, INT_MAX); //Viene asi en las transparencias

		if (copiedBytes == -1) {
			printf("ERROR: invalid copied bytes\n");
			return EXIT_FAILURE;
		}
		else {
			header[i].size = copiedBytes;
			header[i].name = malloc(sizeof(fileNames[i]) + 1);
			strcpy(header[i].name, fileNames[i]);
		}

		if (fclose(input) == EOF) {	//Por si hay fallo en el cierre del fichero
			printf("ERROR: couldn't close file\n");
			return EXIT_FAILURE;
		}

		//Rellenar stHeaderEntry con la ruta y tamaño de inputFile
	}

	//Escritura de datos

	//Volver al byte 0
	if (fseek(output, 0, SEEK_SET) != 0) //por si al colocar el puntero al principio del archivo hay error
	{
		printf("ERROR: Couldn't set pointer at the beginning of the file\n");
		return EXIT_FAILURE;
	}
	else //Aunque igual el else no hace falta porque el return rompe flujo
	
		fwrite(&nFiles, sizeof(int), 1, output); //Escribe el numero de archivos

	for (int i = 0; i < nFiles; i++) //Para cada archivo
 	//Se escriben los datos guardados en el otro for
	{
		fwrite(header[i].name, strlen(header[i].name) + 1, 1, output); 
		fwrite(&header[i].size, sizeof(unsigned int), 1, output);
	}
	
	//free cosas
	for (i = 0; i < nFiles; i++) {
		free(header[i].name);
	}
	free(header);

	if (fclose(output) == EOF)
	{
		printf("ERROR: couldn't close file\n");
		return EXIT_FAILURE;
	}
	else
	{
		printf("mtar file created!\n");
		return EXIT_SUCCESS;
	}
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

	FILE* input;
	FILE* output;
	stHeaderEntry* header;

	int copiedBytes = 0, nFiles = 0;

	if ((input = fopen(tarName, "r")) == NULL) 
	//Si hay error al leer el archivo de origen
	{
		printf("ERROR: file couldn't be read: %s \n", tarName);
		return EXIT_FAILURE;
	}

	printf("debug\n");

	readHeader(input, &header, &nFiles);

	if (header == NULL)
	{
		printf("ERROR: error loading the headers \n");
		return EXIT_FAILURE;
	}


	for (int i = 0; i < nFiles; i++) //Para cada archivo
	{
		if ((output = fopen(header[i].name, "w")) == NULL) //Intentar abrir cada archivo en modo escritura
		{
			return EXIT_FAILURE;
		}

		else
		{
			copiedBytes = copynFile(input, output, header[i].size);

			if (copiedBytes == -1)
			{
				printf("ERROR: invalid copied bytes\n");
				return EXIT_FAILURE;
			}
		}

		if (fclose(output) != 0)
		{
			printf("ERROR: couldn't close file\n");
			return EXIT_FAILURE;
		}
		
		for (i = 0; i < nFiles; i++) {
			free(header[i].name);
		}

		free(header);

		if (fclose(input) == EOF) 
		{
			printf("ERROR: couldn't close file\n");
			return (EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
