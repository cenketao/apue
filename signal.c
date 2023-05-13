#include<stdio.h>
#include <unistd.h>
#include <signal.h>

int  g_stop = 0;
void sig_handle(int signum)
   {
       printf("Catch signal [%d]\n",signum);
       g_stop = 1;
   }

int main(void)
{
        signal(SIGINT,sig_handle);
	signal(SIGINT,sig_handle);
	while(!g_stop)
	{
	   ;
	}
	printf("power off gprs\n");
}

