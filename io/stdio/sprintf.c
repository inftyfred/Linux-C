#include<stdlib.h>
#include<stdio.h>

#define BUFFSIZE  1024

int main(int argc, char **argv)
{
	char buf[BUFFSIZE];
	int year=2023,month=9,day=18;
	
	sprintf(buf,"%d-%d-%d",year,month,day);
	puts(buf);

	exit(0);
}
