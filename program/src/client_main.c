/*********************************************************************************
 *      Copyright:  (C) 2023 Cenketao<1697781120@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client_main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年05月10日)
 *         Author:  Cenketao <1697781120@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年05月10日 20时01分17秒"
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
#include"sock_client.h"

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
                message_s s1 = {"hello",20.55,"2023-4-29 12.54.36"};
                message_s s2 = {"world",55.20,"2023-4-29 12.36.54"};
                message_s *pack_info1 = &s1;
                message_s *pack_info2 = &s2;
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
        if((rv1 = sqlite_init(dbname))<0)
                {
                 log_error("database initial false!");
                                 return -6;
                    }

         socket_connected = get_sock_state(sockfd);

       if(socket_connected)
         {
            if((rv2 = database_check_data(dbname))>0)
            {
               if((rv3 = get_table(dbname,pack_info2))<0)
                  {
                      log_error("get database table!");
                  }
                snprintf(buf2,sizeof(buf2),"%s/%f/%s\n",pack_info2->name,pack_info2->temp,pack_info2->time1);

               if( reva = write(sockfd,buf2,strlen(buf2))<0);
                  {
                      log_error("database write to server failure!");
                  }
                log_info("database write to server successfully!");
               if(( rv6 = sqlite_delect(dbname))<0);
                  {
                    log_error("database delect failure!");
                  }
                log_info("DELETE sqlite database successfully!");

             printf("set database data ok!\n");
            }

           if((rv4 = database_check_data(dbname))==0)
           {
              printf("database don't have data!\n");
           }
            
		   sqlite_close();
        }
 while(1)

 {

        if((rv1=get_serialnumber(pack_info1->name))<0)
        {
          log_error("get serialnumber failure!");
          return -4;
        }
        if((rv2=get_temperature(&pack_info1->temp))<0)
        {
           log_error("get temperature failure!");
           return -5;
        }
        if((rv3=get_time(pack_info1->time1))<0)
        {
           log_error("get time failure!");
           return -6;
        }
        snprintf(buf1,sizeof(buf1)," %s/%f/%s\n",pack_info1->name,pack_info1->temp,pack_info1->time1);

        signal(SIGPIPE,SIG_IGN);
        socket_connected = get_sock_state(sockfd);//返回0表示断开socke,返回1表示socket连接
        if(socket_connected)
         {
             reva = write(sockfd,buf1,strlen(buf1));
             if(reva<0)
              {
                log_error("write to server failure!");
              }

           log_info("socket write to server successfully! ");

         }

         if(!socket_connected)
         {
             if((rv4 = sqlite_init(dbname))<0)
             {
                log_error("database initial false!");
             }

             log_warn("Breaking and write to database!");
			 
             rv6 = sqlite_insert(dbname,pack_info1);
             if(rv6<0)
              {
                 log_error("INSERT to database failure!");
              }

              log_info("write to database successfully!");

              sqlite_close();
         }

       sleep(interval_time-1);
 }
    close(sockfd);
}

