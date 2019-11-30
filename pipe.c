#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
   unsigned char buffer[9];
    int (pfd[2]);                                             //Creating two file descripter fd[1]-Write,fd[0]-Read file descripter
    pid_t id;                       
    pipe(pfd);                                                   //Creating Pipe
    printf(" hello AYU\n PID  : %d\n",getpid());                    
   id=fork();                                                    // Calling Fork and  and creating child
     // fork();
      printf(" CHILD PID  : %d, PARENT PID: %d \n",getpid(),getppid());
    if ( 0 == id)
    {
        close(pfd[1]);
        read(pfd[0],buffer,9);
        printf("Child Reveived from parent: %s",buffer);
    }
    else
    {
        close(pfd[0]);
        write(pfd[1],"Kush Raho\n",9);
       // printf("Parent :%d,  Granparent: %d\n",getpid(),getppid());*/
    }

}
