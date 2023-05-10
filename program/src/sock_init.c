/*********************************************************************************
 *      Copyright:  (C) 2023 Cenketao<1697781120@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  clienttest2.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年04月19日)
 *         Author:  Cenketao <1697781120@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月19日 14时32分32秒"
 *                 
 ********************************************************************************/
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<getopt.h>
#include<netdb.h>
#include"logger.h"
#include <libgen.h>
#include <linux/tcp.h>

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
           log_error("creat socket failure:%s!",strerror(errno));
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
            log_error("connect to server[%s:%d]failure :%s!",servip,port,strerror(errno));
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

int get_sock_state(int sockfd)
{
	     struct tcp_info  info;
		 int              len = sizeof(info);
		 int              rv = -1;
		 if(sockfd<0)
		 {
              printf("the sockfd argument incorrect!\n");
			  return -1;
		 }
		 memset(&info,0,sizeof(info));

         rv = getsockopt(sockfd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t*)&len);
         if(rv<0)
		 {
             log_error("get socket state failure!");
			 return -2;
		 }
		 if((info.tcpi_state ==1))
                {
                  log_info("socket[%d] connected!",sockfd);
                  return 1;
                }
                else
                {
                  log_warn("sockfd[%d] disconnected!",sockfd);
				  return 0;
                  
                }
}
