#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	

	return my_system(argv[1]);
}

int my_system(char *command)
{
	pid_t child = fork();
	int status = 0;

	if(child == 0){
		execlp("sh", "sh", "-c", command, (char *) NULL);
		exit(-1);
	}

	if(wait(&status) == -1){
		perror("wait");
	}

	return status;
}