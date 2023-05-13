#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>
#include<execinfo.h>

int       g_sigstop = 0;

void siganl_stop(int signum)
{
     if(SIGTERM == signum)
     {
        printf("SIGTERM signal detected\n");
     }
     else if(SIGALRM == signum)
     {
       printf("SIGALRM signal detected\n");
       g_sigstop = 1;
     }
}
void signal_user(int signum)
{
        if(SIGUSR1 ==signum)
        {
         printf("SIGUSR1 signal detected\n");
	}

}
