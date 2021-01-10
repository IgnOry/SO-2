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

    int totalBytes = 0;
    int outputByte = 0;
    int readByte = 0; 

    if (origin == NULL) {
    	return -1;
    }
    
    
    while((totalBytes < nBytes) && (readByte = getc(origin)) != EOF) {
        if ((ferror(origin) != 0)) {
        	return -1;
        } 

        outputByte = putc(readByte, destination);

        if (outputByte == EOF) {
        	return -1;
        }
        totalBytes++;
    }

    return totalBytes;
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
	int filenameLength = 0, index = 0;
    char *name;
    char bit;

    while((bit = getc(file) != '\0')) {
        filenameLength++;
    }

    name =  malloc(sizeof(char) * (filenameLength + 1));
    fseek(file, -(filenameLength + 1), SEEK_CUR);

    for (index = 0; index < filenameLength+1; index++) {
        name[index] = getc(file);
    }
    
    return name;
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
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	int n = 0, i = 0, aux = 0;
	stHeaderEntry* read = NULL;

	if (fread(&n, sizeof(int), 1, tarFile) == 0) {
		printf("ERROR: can't read the mtar file.\n");
		return NULL;
	}

	read = malloc(sizeof(stHeaderEntry) * n);

	for (i = 0; i < n; i++) {

		if ((read[i].name = loadstr(tarFile)) == NULL) {
			printf("ERROR: loading string failed.\n");
			return NULL;
		}

		fread(&aux, sizeof(unsigned int), 1, tarFile);
		read[i].size = aux;
	}

	(*nFiles) = n;

	return read;
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
	FILE * in;
	FILE * out;
	stHeaderEntry* headers;

	int copiedBytes = 0, headerBytes = 0, i = 0;

	headers = malloc(sizeof(stHeaderEntry) * nFiles);
	headerBytes += sizeof(int);
	headerBytes += nFiles * sizeof(unsigned int);

	for (i = 0; i < nFiles; i++) {
		headerBytes += strlen(fileNames[i]) + 1;
	}

	out = fopen(tarName, "w");
	fseek(out, headerBytes, SEEK_SET);


	//reading
	for (i = 0; i < nFiles; i++) {

		if ((in = fopen(fileNames[i], "r")) == NULL) {

			printf("ERROR: file does not exist: %s \n", fileNames[i]);
			return EXIT_FAILURE;
		}

		copiedBytes = copynFile(in, out, INT_MAX);	//INT_MAX is max int just to make sure you copy everything

		if (copiedBytes == -1) {
			printf("ERROR: invalid copied bytes\n");
			return EXIT_FAILURE;
		}
		else {
			headers[i].size = copiedBytes;
			headers[i].name = malloc(sizeof(fileNames[i]) + 1);
			strcpy(headers[i].name, fileNames[i]);
		}

		if (fclose(in) == EOF) {	//in case closing the file fails
			printf("ERROR: couldn't close file\n");
			return EXIT_FAILURE;
		}
	}

	//writing
	if (fseek(out, 0, SEEK_SET) != 0) {	//in case returning to the beginning of the file fails
		printf("ERROR: couldn't close file\n");
		return EXIT_FAILURE;
	}
	else
		fwrite(&nFiles, sizeof(int), 1, out);

	for (i = 0; i < nFiles; i++) {
		fwrite(headers[i].name, strlen(headers[i].name) + 1, 1, out);
		fwrite(&headers[i].size, sizeof(unsigned int), 1, out);
	}

	//cleaning up!
	for (i = 0; i < nFiles; i++) {
		free(headers[i].name);
	}
	free(headers);

	if (fclose(out) == EOF) {	//in case closing the file fails
		printf("ERROR: couldn't close file\n");
		return EXIT_FAILURE;
	}

	printf("mtar file created!\n");

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
	FILE* in = NULL;
	FILE* out = NULL;
	stHeaderEntry* headers;
	

	int n = 0, i = 0, copiedBytes = 0;

	if ((in = fopen(tarName, "r")) == NULL) {

		printf("ERROR: file does not exist: %s \n", tarName);
		return EXIT_FAILURE;
	}

	if ((headers = readHeader(in, &n)) == NULL) {

		printf("ERROR: error while loading the headers \n");

		return EXIT_FAILURE;
	}

	for (i = 0; i < n; i++) {

		if ((out = fopen(headers[i].name, "w")) == NULL) {
			return EXIT_FAILURE;
		} 
		else {
			copiedBytes = copynFile(in, out, headers[i].size); 
			if (copiedBytes == -1) {
				printf("ERROR: invalid copied bytes\n");
				return EXIT_FAILURE;
			}
		}

		if (fclose(out) != 0) {
			printf("ERROR: couldn't close file\n");
			return EXIT_FAILURE;
		}
	}

	for (i = 0; i < n; i++) {
		free(headers[i].name);
	}

	free(headers); 

	if (fclose(in) == EOF) {
		printf("ERROR: couldn't close file\n");
		return (EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
