#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"


void sixfive(int fd)
{   char c;
    int n,num=0,have=0,valid=1;

    while((n=read(fd,&c,1))>0){
        if (strchr(" -\r\t\n./,", c)){
            if (valid && have && (num % 5 == 0 || num % 6 == 0))
                printf("%d\n", num);

            num=0;
            have=0;
            valid=1;
        }
        else if (c>='0' && c<='9'){
            if (valid){
                num=num*10 + (c-'0');
                have=1;
            }
        }
        else {
            valid=0;
        }
    }
    if (n<0){
        fprintf(2,"sixfive:read error\n");
        exit(1);
    }
    if (valid && have && (num%5==0 || num%6==0))
    printf("%d\n",num);
}

int main (int argc, char *argv[])
{
    int fd,i;
    if (argc == 1){
        sixfive(0);
        exit(0);
    }
    for (i=1;i<argc;i++){
        if ((fd = open(argv[i], O_RDONLY))<0){
            fprintf(2,"sixfive:cannot open %s\n",argv[i]);
            exit(1);
        }
    sixfive(fd);
    close(fd);
    }
    exit(0);
}
