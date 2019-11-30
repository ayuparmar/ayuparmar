#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/types.h>
int add(int, int);
int main()
{
    int buffer[]={10,20},i;
    int c;
    int pfd1[2];
    int pfd2[2];
    pid_t id;
    pipe(pfd1);// parent
    pipe(pfd2);
    id= fork();

    if(0==id)
    {   close(pfd1[1]);
        read(pfd1[0],buffer,8);
        for(i=0;i<2;i++)
        printf("\nChild RECEIVED From Parent: %d",buffer[i]);
        close(pfd1[0]);

        c=add(buffer[0],buffer[1]);
        close(pfd2[0]);
        write(pfd2[1],&c,4);
        close(pfd2[1]);
    }
    else // parent
    {
        close(pfd1[0]);
        write(pfd1[1],buffer,8);
        close(pfd1[1]);
        
        close(pfd2[1]);
        read(pfd2[0],&c,4);
        printf("\nParent RECEIVED From child: %d",c);
        close(pfd2[0]);
        
    }
}
int add(int a, int b)
{
    return a+b;
}
