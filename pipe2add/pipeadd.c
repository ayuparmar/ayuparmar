#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
   int buffer[2]={2,4};
    int buf[10];
    pid_t id;
    int pfd1[2];
    int pfd2[2];
    int sum=0,a,b,c;
    pipe(pfd1);
     pipe(pfd2);
    id=fork();
    if(0==id)
    {
        close(pfd1[1]);
        read(pfd1[0],buf,2);
        sum=buf[0]+buf[1];
        //printf("Child Received from parent: %d\n",sum);
        close(pfd1[0]);

        close(pfd2[0]);
        write(pfd2[1],&sum,sizeof(int));
        close(pfd2[1]);
    }
    else
    {

        close(pfd1[0]);
        write(pfd1[1],buffer,2);
        close(pfd1[1]);
        

        close(pfd2[1]);
        read(pfd2[0],&sum,sizeof(int));
        printf("\n Sum Recrived from Child: %d",sum);
        close(pfd2[0]);
    }
}