#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#define BUFF_SIZE 1024
int main()
{
    pid_t id;
    int pfd1[2];
    int pfd2[2];
    pipe(pfd1);
    pipe(pfd2);
    int buffer[2];
    int n[3]={320,408};
    id=fork();
    if(0==id)
    {
        close(pfd1[1]);
        read(pfd1[0],buffer,BUFF_SIZE);
        buffer[2]=buffer[1]+buffer[0];
        close(pfd2[0]);
        write(pfd2[1],buffer,20);
    
    }
    else
    {
        close(pfd1[0]);
        close(pfd2[1]);
        write(pfd1[1],n,20);
        close(pfd2[1]);
        read(pfd2[0],n,BUFF_SIZE);
        printf("sum=%d\n",n[2]);
    }
    return 0;
}