#include <sys/types.h>
#include <sys/stat.h>       
#include <stdio.h>
#include <unistd.h>

int main()
{ 
    unsigned char buffer[6];
    int fdr;
    fdr=open("desdf",O_RDONLY);
    read(fdr,buffer,6);
    printf("\n Rec. %s",buffer);
    close(fdr);
    return 0;
}