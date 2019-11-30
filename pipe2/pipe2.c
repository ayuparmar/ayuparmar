#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>


int main()
{
    unsigned char buffer[20],buf[20];
    pid_t id;
    int pfd1[2];
    int pfd2[2];
    pipe(pfd1);
    pipe(pfd2);
    id=fork();
    if(0==id)
    {
        close(pfd1[1]);
        read(pfd1[0],buffer,20);
        printf("Child Received from parent: %s\n",buffer);
        close(pfd1[0]);

        close(pfd2[0]);
        write(pfd2[1],"Amma Pai Lagu\n",20);
        close(pfd2[1]);
    }
    else
    {
        close(pfd1[0]);
        write(pfd1[1],"Kaise Ho\n",20);
        close(pfd1[1]);

        close(pfd2[1]);
        read(pfd2[0],buf,20);
        printf("\n Parent Received from Child: %s",buf);
        close(pfd2[0]);
    }
}