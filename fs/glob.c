#include<stdio.h>
#include<stdlib.h>
#include<glob.h>

#define PAT "/my/*.c"

#if 1
static int errfunc_(const char *errpath, int errno)
{
	puts(errpath);
	printf("error !\n");
	fprintf(stderr,"ERROR MSG:%s\n",strerror(errno));
	return 1;
}
#endif

int main(int argc ,char **argv)
{
	glob_t globres;
	int err,i;
	err = glob(PAT,GLOB_ERR,errfunc_,&globres);
	if(err)
	{
		printf("error code:%d\n",err);
		exit(1);
	}
	
	for(i=0; i<globres.gl_pathc; i++)
	{
		puts(globres.gl_pathv[i]);
	}

	globfree(&globres);

    exit(0);
}
