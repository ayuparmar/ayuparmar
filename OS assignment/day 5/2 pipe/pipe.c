#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/types.h>

int main()
{
    int buffer[11];
    int pfd[2];
    pid_t id;
    pipe(pfd);
    id= fork();
    
    if(0==id)
    {
        close(pfd[1]);
        read(pfd[0],buffer,11);
        printf("Child RECEIVED From Parent: %s",buffer);
        close(pfd[0]);
    }
    else
    {
        close(pfd[0]);
        write(pfd[1],"hello cdac\n",11);
        close(pfd[1]);
    }
}