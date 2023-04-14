#include<sys/types.h> 
#include<sys/socket.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include <stdlib.h>
#include<getopt.h>


#define MSG_STR "hello lingyun client"



void print_usage(char *progname)
{
  printf("%s usage:\n",progname);
  printf("--port(-p):sepcify server listen POTR\n");
  printf("--help(-h):print this help information\n");
  return ;

}


int main(int argc,char **argv)
{
	pid_t                 pid;
	int                   clifd;
        int                   sockfd = -1;
	int                   rv = -1;
	socklen_t             len;
	struct sockaddr_in    servaddr;
	struct sockaddr_in    cliaddr;
        char                  *servip;
	int                   port=0;
	struct option         opts[] = {
	     {"port",required_argument,NULL,'p'},
	     {"help",no_argument,NULL,'h'},
	     {NULL,0,NULL,0}
	
	};
	int                    ch;
	int                    on = 1;
        while((ch=getopt_long(argc,argv,"p:h",opts,NULL))!=-1)
        {
	           switch(ch)
		   {
			case 'p':
				 port=atoi(optarg);
				 break;
			case 'h':
				 print_usage(argv[0]);
				 return 0;
		   }
	
	}
	if(!port)
	{
	  print_usage(argv[0]);
	  return 0;
	}
	sockfd =socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
	   printf("creat socket failure:%s\n",strerror(errno));
	   return -1;
	}
	printf("creat socket[%d] successfully\n",sockfd);

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));//防止端口被占用

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//监听所有ip
	//inet_aton(servip,&servaddr.sin_addr);将字符串格式的ip地址转化成网络字节序
        rv = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)); 
        if(rv<0)
	{
	    printf("socket [%d]bind on port [%d] failure:%s\n",sockfd,port,strerror(errno));
	    return -2;
	}
	listen(sockfd,13);
	printf("Start to listen port[%d] successfully\n",port);
	while(1)
  {        
	printf("Start accept new client incoming...\n");    
        clifd = accept(sockfd,(struct sockaddr *)&cliaddr,&len);
	if(clifd<0)
	{
	      printf("Accept new client failure:%s\n",strerror(errno));
	      continue;
	}
	printf("Accept new client[%s:%d] successfully\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
        pid = fork();//从连接客户端后创建子进程
	if(pid<0){
	       printf("fock creat child process failure : %s\n",strerror(errno));
	       return -3;
	}
        else if(pid>0)//父进程运行，要关闭clident
	{
	   close(clifd);//继续接听新的客户端，所以关闭client_fd来通信
	   continue;
	}
        else if(0==pid)
    {   
        char         buf[1024];
	close(sockfd);//关闭listen_fd，只用client_fd来通信
        printf("child process start to commuicate with socket client\n");
        memset(buf,0,sizeof(buf));
        rv = read(clifd,buf,sizeof(buf));
	if(rv<0)
	{
	  printf("read data from client by sockfd[%d] failure:%s\n",clifd,strerror(errno));
	  close(clifd);
	  continue;
	}
        else if(0==rv)
	{
	  printf("socket[%d] get disconnected\n",clifd);
	  close(clifd);
	  continue;
	}
        else if(rv>0)
        {
	  printf("read %d bytes data from client:%s\n",rv,buf);
	}
	rv = write(clifd ,MSG_STR,strlen(MSG_STR));
	if(rv<0)
	{
	  printf("write to client by sockfd[%d] failure :%s\n",sockfd,strerror(errno));
           close(clifd);
	   exit(0);
	}
	sleep(1);
	printf("close clifd socket[%d] and child process exit\n",clifd);
	close(clifd);
	exit(0);
    }   
  }     
    close(sockfd);
    return 0;
}
