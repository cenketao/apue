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



struct message
        {
           char             name[20];
           char             time1[20];
           float            temp;
        }message1;


int main(int argc,char **argv)
{

        char                  *domain = NULL;
        int                   sockfd;
        int                   rv1,rv2,rv3=-1;
        int                   reva = -1;
        int                   interval_time = -1;
        char                  *servip=NULL;
        int                   port;
        char                  buf[1024];
        char                  buf1[128];
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
      

        if((rv1=get_serialnumber(message1.name))<0)
        {
          printf("get serialnumber failure:%s\n",strerror(errno));
          return -4;
        }
        if((rv2=get_temperature(&message1.temp))<0)
        {
           printf("get temperature failure :%s\n",strerror(errno));
           return -5;
        }
        if((rv3=get_time(message1.time1))<0)
        {
           printf("get time failure :%s\n",strerror(errno));
           //return -6;
        }
        printf("%s/%f℃/%s\n",message1.name,message1.temp,message1.time1);
        snprintf(buf1,sizeof(buf1)," %s/%f℃/%s\n",message1.name,message1.temp,message1.time1);

        signal(SIGPIPE,SIG_IGN);		
        reva = write(sockfd,buf1,strlen(buf1));

        if(reva <0)
        {
           printf("write to server by sockfd[%d] failure :%s\n",sockfd,strerror(errno));
        }
        reva = read(sockfd,buf,sizeof(buf));
        if(reva<0)
           {
                  printf("read data from server by sockfd[%d] failure:%s\n",sockfd,strerror(errno));
           }
         else if(reva = 0)
           {
                  printf("sockfd[%d] get disconnected\n",sockfd);
           }
         else if(reva > 0)
           {
             printf("read %d bytes data from server:%s\n",reva,buf);
           }

       sleep(interval_time-1);
 }
    close(sockfd);

}


