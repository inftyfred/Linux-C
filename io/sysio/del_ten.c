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
	int count = 0;
	off_t ten_pos,ele_pos;
	off_t offset;
	int len = 0;

	if(argc<2)
	{
		fprintf(stderr,"Usage %s <delfile>",argv[0]);
		exit(1);
	}

	fd = open(argv[1],O_RDWR);

	while(1)
	{
		read(fd,buf,1);
		if('\n' ==  buf[0])
		{	++count;	
			if(9 == count)
				ten_pos = lseek(fd,0,SEEK_CUR);	
			if(10 == count)
			{
				ele_pos = lseek(fd,0,SEEK_CUR);
				offset = ele_pos-ten_pos;
				break;
			}
		}
	}

	while(1)
	{
		lseek(fd,ele_pos,SEEK_SET);
		len = read(fd,buf,offset);
		lseek(fd,ten_pos,SEEK_SET);
		write(fd,buf,len);
		if(len == 0)
			break;
		ele_pos+=len;
		ten_pos+=len;
	}

	len = lseek(fd,0,SEEK_END);

	if((ftruncate(fd,len-offset)) < 0)
	{
		perror("ftruncate:");
	}

	close(fd);
	exit(0);
}
