#include<sys/types.h> 
#include<sys/socket.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include <stdlib.h>
#include<getopt.h>
#include <netdb.h>

#define MSG_STR "Hello,i am testing!"

void print_usage(char *progname)
{
  printf("%s usage:\n",progname);
  printf("--ipaddr(-i):sepcify server ipaddress\n");
  printf("--port(-p):sepcify server POTR\n");
  printf("--domain(-d):sepcify server hostname\n");
  printf("--time(-t):interval time(s) to get temperature\n");
  printf("--help(-h):printf help information\n");
  return ;

}
int sock_init(char *servip,char *domain,int port);

int main(int argc,char **argv)
{

	    char                  *domain = NULL;
     	int                   sockfd;
        int                   reva = -1;
		int                   interval_time = -1;
        char                  *servip=NULL;
        int                   port;
        char                  buf[1024];
        struct option         opts[] = {
             {"ipaddr",required_argument,NULL,'i'},
             {"port",required_argument,NULL,'p'},
             {"domain",required_argument,NULL,'d'},
			 {"time",required_argument,NULL,'t'},
             {"help",no_argument,NULL,'h'},
             {NULL,0,NULL,0}
        };
        int                    ch;
        while((ch=getopt_long(argc,argv,"i:p:d:t:h",opts,NULL))!=-1)
        {
                   switch(ch)
                   {
                        case 'i':
                                servip = optarg;
                                break;
                        case 'p':
                                 port=atoi(optarg);
                                 break;
                        case 'd':
                                 domain = optarg;
                                 break;
						case 't':
								 interval_time = atoi(optarg);
								 break;
                        case 'h':
                                 print_usage(argv[0]);
                                 break;
                        default:
                                 printf("unknow return val %d\n",ch);
                   }

        }
        if(!port | !(!servip ^ !domain))//ip或域名二选一
            {
             print_usage(argv[0]);
             return -1;
             }
        if(interval_time<0)
             {
               printf("Please input the correct acquisition time value\n");
			   return -2;
             }

    	if((sockfd = sock_init(servip,domain,port))<0)
        	{
	          printf("socket failure:%s\n",strerror(errno));
	          return -3;
        	}

 while(1)
  
 {
	 while(1)
	  {
        reva = write(sockfd,MSG_STR,strlen(MSG_STR));
        if(reva <0)
        {
           printf("write to server by sockfd[%d] failure :%s\n",sockfd,strerror(errno));
           return -6;
        }
		sleep(interval_time);
	  } 
	     reva = read(sockfd,buf,sizeof(buf));
         if(reva<0)
           {
                  printf("read data from server by sockfd[%d] failure:%s\n",sockfd,strerror(errno));
                  return -4 ;
           }
         else if(reva = 0)
           {
                  printf("sockfd[%d] get disconnected\n",sockfd);
                  return -5;
           }
         else if(reva > 0)
           {
             printf("read %d bytes data from server:%s\n",reva,buf);
           }

   
 }
    close(sockfd);

}

int sock_init(char *servip,char *domain,int port)
{
	char                  str[32];
	char                  **pptr;
	int                   sockfd = -1;
	struct hostent        *host = NULL;
	struct sockaddr_in    servaddr;
	int                   rv = 0;
	 //解析IP地址

	if(!servip)//输入域名
         {
                 if(NULL==(host = gethostbyname(domain)))
                 {
                   printf("gethostname error :%s\n",strerror(errno));
                   return -1;
                 }
         switch(host->h_addrtype)
              {
                  case  AF_INET:
                  case  AF_INET6:
                  pptr = host->h_addr_list;
                  for(; *pptr!=NULL;pptr++)

                          printf("server IP address : %s\n",inet_ntop(host->h_addrtype,*pptr,str,sizeof(str)));
                          printf("%s\n",str);

                  servip = str;
                  break;
                  default:
                  printf("unknown address type\n");
                  break;
              }
         }

	sockfd =socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
	   printf("creat socket failure:%s\n",strerror(errno));
	   rv = -1;
	   goto CleanUp;
	}
	printf("creat socket[%d] successfully\n",sockfd);
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_aton(servip,&servaddr.sin_addr);//将字符串格式的ip地址转化成网络字节序
        rv = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)); 
        if(rv<0)
	{
	    printf("connect to server[%s:%d]failure :%s\n",servip,port,strerror(errno));
	    rv = -2;
	    goto CleanUp;
	}
        printf("connect toserver[%s:%d] successfully\n",servip,port);

CleanUp:
	if(rv<0)
		close(sockfd);
	else
                rv = sockfd;
	return rv;
}

