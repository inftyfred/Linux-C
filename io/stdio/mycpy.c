#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
	FILE *fp1,*fp2;
	int ch;
	
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
		perror("fp2:"):
		exit(1);
	}
	while(1)
	{
		ch = fgetc(fp1);
		if(EOF == ch)
		{
			break;
		}
		fputc(ch,fp2);
	}
	fclose(fp2);
	fclose(fp1);


	exit(0);
}
