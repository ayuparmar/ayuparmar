#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/types.h>

int main()
{
    unsigned char buffer[6], buf[5];
    int pfd1[2];
    int pfd2[2];
    int i=1;
    pid_t id;
    pipe(pfd1);// parent
    pipe(pfd2);
    id= fork();

    if(0==id)
    {   close(pfd1[1]);
        read(pfd1[0],buffer,6);
        printf("Child RECEIVED From Parent: %s",buffer);
        close(pfd1[0]);
        
        close(pfd2[0]);
        write(pfd2[1],"cdac\n",5);
        close(pfd2[1]);
        
    }
    else // parent
    {
        close(pfd1[0]);
        write(pfd1[1],"hello\n",6);
        close(pfd1[1]);
        
        close(pfd2[1]);
        read(pfd2[0],buf,5);
        printf("\nParent RECEIVED From child: %s",buf);
        close(pfd2[0]);
        
    }
}