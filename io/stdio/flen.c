#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
	FILE *fp;
	int count = 0;

	fp = fopen(argv[1],"r");
	if(NULL == fp)
	{
		perror("fopen:");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);

	printf("flen: %ld", ftell(fp));


	exit(0);
}
