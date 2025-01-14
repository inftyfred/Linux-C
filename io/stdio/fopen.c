#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

int main()
{
	FILE * fp;
	char * err;

	fp = fopen("tmp","w");
	if(NULL == fp)
	{
//	 perror("fopen() ");
	 err = strerror(errno);
	 fprintf(stderr, "fopen() failed! errno = %s\n",err);
	 exit(1);
	}
	else
	{
		puts("ok");
		fclose(fp);
	}
	exit(0);
}
