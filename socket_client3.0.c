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

#define MSG_STR "Hello,LingYun IoT Studio\n"

void print_usage(char *progname)
{
  printf("%s usage:\n",progname);
  printf("--ipaddr(-i):sepcify server ipaddress\n");
  printf("--port(-p):sepcify server POTR\n");
  printf("--domain(-d):sepcify server hostname\n");
  printf("--help(-h):printf help information\n");
  return ;

}


int main(int argc,char **argv)
{
	char                  *domain=NULL;
	char                  str[32];
	char                  **pptr=NULL;
        int                   sockfd = -1;
	int                   rv = -1;
	char                  *servip=NULL;
	int                   port;	
	char                  buf[1024];
	struct hostent        *host=NULL;
	struct sockaddr_in    servaddr;
	struct option         opts[] = {
	     {"ipaddr",required_argument,NULL,'i'},
	     {"port",required_argument,NULL,'p'},
	     {"domain",required_argument,NULL,'d'},
	     {"help",no_argument,NULL,'h'},
             {NULL,0,NULL,0}	
	};
	int                    ch;
        while((ch=getopt_long(argc,argv,"i:p:d:",opts,NULL))!=-1)
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
	printf("server IP :%s\n",servip);
	sockfd =socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
	   printf("creat socket failure:%s\n",strerror(errno));
	   return -2;
	}
	printf("creat socket[%d] successfully\n",sockfd);
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_aton(servip,&servaddr.sin_addr);//将字符串格式的ip地址转化成网络字节序
       // endhostent();//释放结构体
     //   host = NULL;//防止野指针
        rv = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)); 
        if(rv<0)
	{
	    printf("connect to server[%s:%d]failure :%s\n",servip,port,strerror(errno));
	    return -3;
	}
        printf("connect toserver[%s:%d] successfully\n",servip,port);
	while(1)
  {
        rv = write(sockfd,MSG_STR,strlen(MSG_STR));
	if(rv <0)
	{
	   printf("write to server by sockfd[%d] failure :%s\n",sockfd,strerror(errno));
	   return -4;
	}
        rv = read(sockfd,buf,sizeof(buf));
	if(rv<0)
	{
	  printf("read data from server by sockfd[%d] failure:%s\n",sockfd,strerror(errno));
	  return -5;
	}
        else if(0==rv)
	{
	  printf("socket[%d] get disconnected\n",sockfd);
	  return -6;
	}
        else if(rv>0)
	{
	  printf("read %d bytes data from server:%s\n",rv,buf);
	}

   }
    close(sockfd);
    
}
