#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main()
{
	putchar('a');
	putchar('\n');
	write(1,"b",1);

	putchar('a');
	printf("test\n");
	write(1,"b",1);

	putchar('a');
	write(1,"b",1);


	exit(0);
}
