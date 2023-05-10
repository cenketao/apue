#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <libgen.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define  ARRAY_SIZE(x)    (sizeof(x)/sizeof(x[0]))

static inline void msleep(unsigned long ms);
static inline void print_usage(char *progname);
int socket_server_init(char *listen_ip,int listen_port);//每一次sock都是一样，所以抽象成一个函数

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int      sqlite_init(char *sql);
char     sql_create[]="CREATE TABLE TEMPERATURE("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "NAME CHAR(20)         NOT NULL,"
                      "TEMPERATURE CHAR(20)  NOT NULL,"
					  "TIME CHAR(20)         NOT NULL);";

char     sql_insert[256];
char     sql_select[]="SELECT *from TEMPERATURE";
struct message
        {
           char             name[20];
           char             time1[20];
           char             temp[20];
        }message1;

int main(int argc,char **argv)
{
    int              listenfd,connefd;
    int              serv_port = 0;
    int              daemon_run = 0;
    char             *progname = NULL;
    int              opt;
    fd_set           rdset;
    int              rv;    
    int              i,j;
    int              found;
    int              maxfd = 0;
    char             buf[1024];
	char             *q = "/";
    int              fds_arry[1024];
    struct option    long_options[] = 
{
       {"daemon",no_argument,NULL,'b'},
       {"port",required_argument,NULL,'p'},
       {"help",no_argument,NULL,'h'},
       {NULL,0,NULL,0}

};
             progname = basename(argv[0]);
             /* parser the command line parmeters*/
            while((opt = getopt_long(argc,argv,"bp:h",long_options,NULL)) != -1)
            {
              switch(opt)
                {
                  case 'b':
	               daemon_run = 1;
	               break;
                  case 'p':
	               serv_port = atoi(optarg);
	               break;
                  case 'h':
	               print_usage(progname);
	               return EXIT_SUCCESS;
                  default:
	               break;
                 }
 
 
           }
         if(!serv_port)
         {
              print_usage(progname);
              return -1;
         }

        if((listenfd = socket_server_init(NULL,serv_port))<0)
         {
             printf("ERROR:%s server listen on port %d  failure\n",argv[0],serv_port);
             return -2;
         }
         printf("%s server start to listen on port %d\n",argv[0],serv_port);
         /* set program running on background*/
	 if(daemon_run)
          {
	    daemon(0,0);
	  }
          for(i=0;i<ARRAY_SIZE(fds_arry);i++)
          {
	    fds_arry[i] = -1;
	  }
            fds_arry[0] = listenfd;
	  for( ; ; )
          {
	    FD_ZERO(&rdset);
	    for(i=0;i<ARRAY_SIZE(fds_arry);i++)
	    {
	      if(fds_arry[i]<0)
		      continue;
	      maxfd = fds_arry[i]>maxfd ? fds_arry[i] : maxfd;
	      FD_SET(fds_arry[i],&rdset);
	    }
	    /*program will blocked here*/
	    rv = select(maxfd+1,&rdset,NULL,NULL,NULL);
	    if(rv<0)
	    {
	       printf("select failure :%s\n",strerror(errno));
	       break;
	    }
	    else if(0==rv)
	    {
	        printf(" select get timeout\n");
		continue;
	    }
	    /*listen socket get event means new client start connect now*/
            if(FD_ISSET(listenfd,&rdset))
	    {
	      if((connefd=accept(listenfd,(struct sockaddr *)NULL,NULL))<0)
	      {
	          printf("accpt new client failure :%s\n",strerror(errno));
		  continue;
	      }
	      
	      found = 0;
	      for(i=0;i<ARRAY_SIZE(fds_arry);i++)
	      {
	        if(fds_arry[i]<0)
		{
		  printf("accept new client[%d] and add it into array\n",connefd);

		  fds_arry[i] = connefd;
		  found = 1;
		  break;
		}
	      }
               if(!found)
                 {
		   printf("accept new client[%d] but full ,so refuse it\n",connefd);
		   close(connefd);
		 }
	    }
	    else/*data arrive from already connected client*/
	    {
	          for(i=0;i<ARRAY_SIZE(fds_arry);i++)
		  {
		     if(fds_arry[i]<0||!FD_ISSET(fds_arry[i],&rdset))
			     continue;
		     if((rv = read(fds_arry[i],buf,sizeof(buf)))<=0)
		     {
		       printf("socket[%d] read failure or get disconncet.\n",fds_arry[i]);
		       fds_arry[i] = -1;
		     }
		     else
		     {
		       printf("From socket[%d]  get some temperature information and save to database\n ",fds_arry[i]);
		       /*convert letter from lowercase to uppercase */
		       snprintf(message1.name,sizeof(message1.name),"%s",strtok(buf,q));
               snprintf(message1.temp,sizeof(message1.temp),"%s",strtok(NULL,q));
               snprintf(message1.time1,sizeof(message1.time1),"%s",strtok(NULL,q));
               snprintf(sql_insert,sizeof(sql_insert),"INSERT INTO TEMPERATURE VALUES (NULL,'%s','%s','%s');", message1.name,message1.temp,message1.time1);
			  sqlite_init(sql_create);
              sqlite_init(sql_insert);
              sqlite_init(sql_select);


		       if(write(fds_arry[i],buf,rv)<0)
		       {
		         printf("socket [%d write failure:%s\n]",fds_arry[i],strerror(errno));
			 close(fds_arry[i]);
			 fds_arry[i] = -1;
		       }
		     }
		  }
	    }

	  }
CleanUp:
close(listenfd);
return 0;
}
static inline void msleep(unsigned long ms)
{
       struct timeval       tv;
       tv.tv_sec = ms/1000;
       tv.tv_usec = (ms%1000)*1000;
       select(0,NULL,NULL,NULL,&tv);
}
static inline void print_usage(char *progname)
{
    printf("Usage : %s[POTION]...\n",progname);
	printf("%s is a socket server program,which used to verify client and echo back string from it\n",progname);
	printf("\nMandatory arguments to long options are mandatory for short options too:\n");
	printf("-b[daemon] set program running on background\n");
	printf("-p[port  ] Socket server port address\n");
	printf("-h[help  ] Display this help information\n ");
	printf("\nExample:%s -b -p 8900\n",progname);
	return ;
}
int socket_server_init(char *listen_ip,int listen_port)
{
    struct sockaddr_in        servaddr;
    int                       rv = 0;
    int                       on = 1;
    int                       listenfd;
    if((listenfd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
       printf("Use socket() to create a TCP socket failure :%s\n",strerror(errno));
       return -1;
    }
    /*Set socket port reuseable,fix 'Address already in use' bug when socket server restart*/
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(listen_port);
    
    if(!listen_ip)//listen all the local IP address
    {
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else//liten the specified IP address
    {
      if(inet_pton(AF_INET,listen_ip,&servaddr.sin_addr)<=0)
      {
        printf("inet_pton() set listen IP address failure.\n");
	rv = -2;
	goto CleanUp;
      }
    }
  if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
  {
     printf("Use bind() to bind the TCP socket failure :%s\n",strerror(errno));
     rv = -3;
     goto CleanUp;
  }
  if(listen(listenfd,13)<0)
  {
    printf("Use bind() to bind the TCP socket failure:%s\n",strerror(errno));
    rv = -4;
    goto CleanUp;
  }
CleanUp:
  if(rv<0)
    close(listenfd);
  else
    rv = listenfd;
    return rv;

}
int sqlite_init(char *sql)
  {
     sqlite3 *db;
     char *zErrMsg = 0;
     int  rc;

     /* Open database */
     rc = sqlite3_open("test.db", &db);
     if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
     }
     else{
        fprintf(stdout, "Opened database successfully\n");
     }

     /* Create SQL statement */

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
     if( rc != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     }
     else
     {
        fprintf(stdout, "Table created successfully\n");
     }
     sqlite3_close(db);
     return 0;
  }
