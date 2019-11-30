//sender
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <stdio.h>
       #include <unistd.h>

       int main()
       {
           int fds;
           fds=open("desdf",o_WRONLY);
           write(fds,"Hello\n",6);
           close(fds);
           return 0;
       }