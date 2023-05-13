#include<stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include<errno.h>
#include <sys/types.h>
#include <unistd.h>


     pid_t          pid;
int main(){
      pid = fork();
      if(pid<0)
      {
         printf("fork() failure :%s\n",strerror(errno));
	 return -1;
      }
      if(0==pid){
         printf("child process PID[%d] start running,parent process PID is [%d]\n",getpid(),getppid());
	 return 0;
      }   
      else{
         printf("parent process PID [%d]continue running,and child process PID is [%d]\n",getpid(),pid);
      return 0;
      
      }
    
     }


