#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c,ret;
	int size; 
	char*  aux;
	char* pEnd;


	if (argc<3) {
		fprintf(stderr,"Usage: %s <file_name>\n <block_size>",argv[0]);
		exit(1);
	}

	size = strtol(argv[2], &pEnd, 10);//cambiar por strtol o sscanf (int)(*argv[2]);

	aux = malloc(size);

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	while ((c = fread(aux, 1, size, file)) > 0) {
		/* Print byte to stdout */
		ret = fwrite(aux, 1, c, stdout);

		if (ret != c){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}

	fclose(file);
	return 0;
}
