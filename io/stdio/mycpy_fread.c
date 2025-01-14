#include<stdio.h>
#include<stdlib.h>

#define BUFFSIZE 1024

int main(int argc, char **argv)
{
	FILE *fp1,*fp2;
	char buf[BUFFSIZE];
	int n;

	if(argc < 3)
	{
		fprintf(stderr,"Usage:%s <srcfile> <cpfile>",argv[0]);
		exit(1);
	}


	fp1 = fopen(argv[1],"r");
	if(NULL == fp1)
	{	
		perror("fp1:");
		exit(1);
	}
	fp2 = fopen(argv[2],"w");
	if(NULL == fp2)
	{
		fclose(fp1);
		perror("fp2:");
		exit(1);
	}
	
	while((n= fread(buf,1,BUFFSIZE, fp1)) > 0)
	{
		fwrite(buf,1,n,fp2);
	}
	fclose(fp2);
	fclose(fp1);

	exit(0);
}
