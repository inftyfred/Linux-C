#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    char *linebuf;
    int a = 0;
    size_t linesize;

    if(argc < 2)
    {
        fprintf(stderr,"Usage...\n");
        exit(1);
    }

    fp = fopen(argv[1],"r");
    if(NULL == fp)
    {
        perror("fopen:");
        exit(1);
    }

	linebuf = NULL;
	linesize = 0;

    while(1)
    {
        if(getline(&linebuf, &linesize,fp) < 0)//代表文件读写结束
            break;
        printf("%ld\n", strlen(linebuf));
		printf("%ld\n",  linesize );
    }


    fclose(fp);
    exit(0);
}

