#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;

	char * fileBuffer; //Añadido para lectura con fread 
	long fileSize;

	char* p;

	int block_size;
	int c,ret;

	if (argc!=3) {
		fprintf(stderr,"Usage: %s <file_name>\n <block_size>",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Version con fread y fwrite

	//Obtain file size ??
	fseek(file, 0, SEEK_END);
	fileSize = ftell (file);
	rewind(file);

	//Allocate memory to contain the whole file ??
	fileBuffer = (char*) malloc(sizeof(char)*fileSize);
	if (fileBuffer == NULL)
		{
			err(3,"Memory Allocation error");
		}

	*/

	//Version con parametro de blocksize

	block_size = strtol(argv[2], &p, 10); //De string a int. Que string (argumento 2, el 1 es el nombre del archivo), puntero, base de conversion
	
	fileBuffer = (char*) malloc(sizeof(char)*block_size);
	if (fileBuffer == NULL)
		{
			err(3,"Memory Allocation error");
		}

	/* Read file byte by byte */

	//Version con while

	while ((c = fread(fileBuffer, 1, block_size, file)) > 0) { //Comprobación de errores
			
			//Print byte to stdout 
			
			ret = fwrite (fileBuffer, 1, c, stdout); //buffer, 1 byte, tamaño total, salida
			
			if (ret != c)
			{
				fclose(file);
				err(4,"putc() failed!!");
			}
		}

	/* Version con for
	for (int i =0; i < fileSize; i++)
	{
		c = fread(fileBuffer, 1, fileSize, file); //buffer, 1 byte, tamaño total, archivo de lectura
		ret = fwrite (fileBuffer, 1, c, stdout); //buffer, 1 byte, tamaño total, salida
	}
	*/
	

	fclose(file);
	return 0;
}

/*

CODIGO ORIGINAL

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c,ret;

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	// Open file 
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	// Read file byte by byte
	while ((c = getc(file)) != EOF) {
		// Print byte to stdout
		ret=putc((unsigned char) c, stdout);

		if (ret==EOF){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}

	fclose(file);
	return 0;
}
*/
