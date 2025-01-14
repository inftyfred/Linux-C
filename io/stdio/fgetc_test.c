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

	while(fgetc(fp) != EOF)
	{
		count++;
	}

	printf("count = %d", count);
	
	fclose(fp);
	exit(0);
}
