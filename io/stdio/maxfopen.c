#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

int main()
{
	FILE * fp;
	char * err;
	int count = 0;
	while(1){
		fp = fopen("tmp","w");
		if(NULL == fp)
		{
		   perror("fopen() ");
		   break;	
		}
		count++;
	}

	printf("count = %d", count);

	exit(0);
}
