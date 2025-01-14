#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFSIZE 1024

int main(int argc, char **argv)
{
	int fd;
	char buf[BUFFSIZE];
	off_t pos;
	int flag;

	if(argc<2)
	{
		fprintf(stderr,"Usage %s <delfile>",argv[0]);
		exit(1);
	}

	fd = open(argv[1],O_RDWR);

	while(1)
	{
		flag = read(fd,buf,1);
		printf("%c\n",buf[0]);
		pos = lseek(fd,0,SEEK_CUR);
		printf("%ld\n",pos);
		if(flag <=  0)
			break;
	}

	close(fd);
	exit(0);
}
