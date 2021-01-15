#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int my_system(char* command)
{
	int status = 0;
	pid_t pid_ = fork();

	if (pid_ == 0) //Child
	{
		execlp("sh", "sh", "-c", command, (char*) NULL); //Ejecucion de comando
		exit(-1);
	}

	if (wait(&status) == -1)
		perror("wait");

	return status;
}

int main(int argc, char* argv[])
{
	if (argc!=2)
	{
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	return system(argv[1]);
}

