#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <unistd.h>
#include <shadow.h>
#include <string.h>

int main(int argc ,char  **argv)
{
	
    char *input_pass;
    char *crypted_pass;
    struct spwd *shadowline;

	if(argc < 2)
    {
        fprintf(stderr,"Usage...\n");//argv[1] name
        exit(1);
    }

    input_pass = getpass("Please Enter pwd:");//获取密码，不进行显示输入

    if(input_pass == NULL){
        fprintf(stderr,"getpass error\n");
        exit(1);
    }


    shadowline = getspnam(argv[1]);

    if(shadowline == NULL)
    {
        fprintf(stderr,"getspnam error\n");
        exit(1);
    }

    crypted_pass = crypt(input_pass,shadowline->sp_pwdp);//crypt会自动截取需要的指令

    if(crypted_pass == NULL){
        fprintf(stderr,"crypt error\n");
        exit(1);
    }

    if(strcmp(shadowline->sp_pwdp,crypted_pass) == 0)
    {
        puts("ok!");
    }
    else{
        puts("fail!");
    }
	
	exit(0);
}
