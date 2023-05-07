/*********************************************************************************
 *      Copyright:  (C) 2023 CKT
 *                  All rights reserved.
 *
 *       Filename:  project.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(21/04/23)
 *         Author:  Cenketao <1697781120@qq.com>
 *      ChangeLog:  1, Release initial version on "21/04/23 21:44:25"
 *                 
 ********************************************************************************/
#include"project.h"
#include<stdio.h>
#include<errno.h>
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include<netinet/in.h>
 #include <arpa/inet.h>
#include <libgen.h>
#include <linux/tcp.h>
#include"database.h"
#include"packinfo.h"
#include"logger.h"

void print_usage(char *progname)
{
  printf("%s usage:\n",progname);
  printf("You input ip address or domain one of two\n");
  printf("--ipaddr(-i):sepcify server ipaddress\n");
  printf("--port(-p):sepcify server POTR\n");
  printf("--domain(-d):sepcify server hostname\n");
  printf("--time(-t):interval time(s) to get temperature\n");
  printf("--help(-h):printf help information\n");
  return ;

}


int main(int argc,char **argv)
{
	    char                 *dbname = "test.db";
        int                   socket_connected = 0;
        char                  *domain = NULL;
        int                   sockfd;
        int                   rv1,rv2,rv3,rv4,rv5,rv6=-1;
        int                   reva = -1;
        int                   interval_time = -1;
        char                  *servip=NULL;
        int                   port;
        char                  buf[1024];
        char                  buf1[128];
		char                  buf2[128];	
		message_s s = {"hello",20.55,"2023-4-29 12.54.36"};  
		message_s *pack_info = &s;
        struct option         opts[] = {
             {"ipaddr",required_argument,NULL,'i'},
             {"port",required_argument,NULL,'p'},
             {"domain",required_argument,NULL,'d'},
             {"time",required_argument,NULL,'t'},
             {"help",no_argument,NULL,'h'},
             {NULL,0,NULL,0}
        };
		struct tcp_info        info;
		int                    len = sizeof(info);
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
        if(logger_init("stdout",LOG_LEVEL_INFO)<0)
             {
                  fprintf(stderr,"initial logger system failure\n");
                  return -1;
             }
        if(logger_init("running.log",LOG_LEVEL_DEBUG)<0)
             {
                  fprintf(stderr,"initial logger system failure\n");
                   return -2;
              }

        if(!port | !(!servip ^ !domain))//ip或域名二选一
             {
                  print_usage(argv[0]);
                  return -3;
             }
        if(interval_time<0)
             {
                  log_warn("Please input the correct acquisition time value\n");
                  return -4;
             }
 
		if((sockfd = sock_init(servip,domain,port))<0)
             {
                 log_error("socket failure!");
             }



 while(1)

 {

        if((rv1=get_serialnumber(pack_info->name))<0)
        {
          log_error("get serialnumber failure!");
          return -4;
        }
        if((rv2=get_temperature(&pack_info->temp))<0)
        {
           log_error("get temperature failure!");
           return -5;
        }
        if((rv3=get_time(pack_info->time1))<0)
        {
           log_error("get time failure!");
           return -6;
        }
        snprintf(buf1,sizeof(buf1)," %s/%f/%s\n",pack_info->name,pack_info->temp,pack_info->time1);

		getsockopt(sockfd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t*)&len);
		if((info.tcpi_state ==1))
		{
            log_info("socket connected!");
            socket_connected = 1;
		}
		else
		{
            log_warn("sockfd disconnected!");
			socket_connected = 0;
		}
        signal(SIGPIPE,SIG_IGN);
		if(socket_connected)
		{ 
			rv4 = sqlite_init(dbname);
			rv5 = get_table(dbname,pack_info);
			if(rv5<0)
			{
               log_error("get Table failure!");
			}
			snprintf(buf2,sizeof(buf2),"%s/%f/%s\n",pack_info->name,pack_info->temp,pack_info->time1);
		    reva = write(sockfd,buf2,strlen(buf2));
			log_info("database write to server successfully!");
			rv6 = sqlite_delect(dbname);
            log_info("DELETE sqlite database successfully!");
            reva = write(sockfd,buf1,strlen(buf1));
			if(reva<0)
			{
                log_error("write to server failure!");
			}
			log_info("socket write to server successfully! ");
            sqlite_close();
		}
		if(!socket_connected)
		{
			log_warn("Breaking and write to database!");
            rv6 = sqlite_init(dbname);
			rv6 = sqlite_insert(dbname,pack_info);
			if(rv6<0)
			{
                 log_error("INSERT to database failure!");
			}
			log_info("write to database successfully!");
            sqlite_close();
		}
        if(reva <0)
        {
             log_error("write to server failure!");
	    }
	
        reva = read(sockfd,buf,sizeof(buf));
        if(reva<0)
           {
                  log_error("read data from server by sockfd[%d] failure!",sockfd);
           }
         else if(reva = 0)
           {
                  log_warn("sockfd[%d] get disconnected!",sockfd);
           }
         else if(reva > 0)
           {
             log_info("read %d bytes data from server!",reva,buf);
           }

       sleep(interval_time-1);
 }
    close(sockfd);
}


