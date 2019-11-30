#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/types.h>
int main()
{
    
    int id;
  //  int pid,ppid;
    printf("Before: pid:%d,ppid:%d \n",getpid(),getppid());
    id= fork();
    if(0==id)
    printf("After: child: pid:%d,ppid:%d \n",getpid(),getppid());
    else
    printf("After: parent: pid:%d,ppid:%d \n",getpid(),getppid());
//    printf("Before: pid:%d,ppid:%d \n",getpid(),getppid());
return 0;
}