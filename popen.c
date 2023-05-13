#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<ctype.h>

int get_ipaddr(char *interface,char *ipaddr,int ipaddr_size);

int main(int argc,char **argv)
{
         char            ipaddr[16];
	 char            *interface = "ens33";
	 


	   memset(ipaddr,0,sizeof(ipaddr));
         
	   if(get_ipaddr(interface,ipaddr,sizeof(ipaddr))<0)
	   {      
	
	        printf("ERROR:get IP failure:%s\n",strerror(errno));
		return -1;
	   }
           printf("get network interface %s IP address [%s]\n",interface,ipaddr);
	   return 0;
}
int get_ipaddr(char *interface,char *ipaddr,int ipaddr_size){
         char            buf[1024];
	 char            *ptr;
	 char            *ip_start;
	 char            *ip_end;
	 FILE            *fp;
	 int             len;
	 int             rv;

	 if(!interface||!ipaddr||ipaddr_size<16)
	 {
	    printf("Invalid input arguments\n");
	    return -1;
	 }
         
	 memset(buf,0,sizeof(buf));
	 snprintf(buf,sizeof(buf),"ifconfig %s",interface);
         if(NULL == (fp=popen(buf,"r")))
	 {
	      printf("popen() to excute command \"%s\" failure :%s\n",buf ,strerror(errno));
	      return -2;
	 }

        rv = -3;//set default return value to -3 means parser failure
	while(fgets(buf,sizeof(buf),fp))
	{
	       if(strstr(buf,"netmask"))
	       {
	           ptr = strstr(buf,"inet");
		   if(!ptr)
		   {
		     break;
		   }
	           ptr+=strlen("inet");
		   while(isblank(*ptr))
			   ptr++;
		   ip_start = ptr;
		   while(!isblank(*ptr))
		   ip_end = ptr;
		   memset(ipaddr,0,sizeof(ipaddr));
		   len = ip_end - ip_start;
		   len = len >ipaddr_size ? ipaddr_size: len;
		   memcpy(ipaddr,ip_start,len);
		   rv = 0;//parser ip address ok and set rv to 0
		   break;
	       }
	
	}
	return rv;

}
