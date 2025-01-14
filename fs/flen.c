#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


static int flen(const char *f)
{
	struct stat statf;

	if( stat(f,&statf) < 0)
	{
		perror("stat()");
		exit(1);
	}
	return statf.st_size;
}


int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage....\n");
		exit(1);
	}


	printf("%d\n",flen(argv[1]));

	exit(0);
}
