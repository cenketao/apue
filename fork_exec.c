#include<stdio.h>
#include<unistd.h>
#include <fcntl.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<ctype.h>
//标准输出重定向文件，/tem路径是在linux系统内存里做的一个文件系统，放在这里不用硬盘写会快一些
#define TMP_FILE    "/tmp/.ifconfig.log"


int main(int argc,char **argv)
{
          pid_t                  pid;
	  int                    fd;
	  char                   buf[1024];
	  int                    rv;
	  char                   *ptr;
	  FILE                   *fp;
	  char                   *ip_start;
	  char                   *ip_end;
	  char                   ipaddr[16];

	  //父进程打开这个文件，子进程将会继承父进程打开的文件描述符，这样父子进程都可以通过各自的文件描述符访问同一个文件了

	  if((fd = open(TMP_FILE,O_RDWR|O_CREAT|O_TRUNC,0644))<0)
	  {
	     printf("Redirect standard output to file failure :%s\n",strerror(errno));
	     return -1;
	  }
        //父进程开始创建进程
	pid = fork();
	if(pid<0)
	{
	      printf("fork()create child process failure :%s\n",strerror(errno));
	      return -1;
	}
        else if(0==pid)//子进程运行
	{
	    printf("Child process start excute ifconfig program\n");
	    //子进程会继承父进程打开的文件描述符，此时子进程重定向标准输出到父进程所打开的文件里
	    dup2(fd,STDOUT_FILENO);
	    /*
	     下面这句execl（...）函数是让子进程开始执行带参数的ifconfig命令：ifconfig ens33
	     execl()会导致子进程彻底丢掉父进程的文本段，数据段，并加载/sbin/ifconfig这个程序的文本段，数据段重新建立进程内存空间。
	     execl()函数的第一个参数是所要执行程序的路径,ifconfig命令（程序）的路径是/sbin/ifconfig;
	     接下来的参数是命令及其相关选项，参数，每个命令，选项，参数都用双引号（“”）括起来，并以NULL结束*/
	    /*
	     ifconfig ens33 命令在执行是会将命令的执行结果输出到标准输出上。而这时子进程已经重定向标准输出到文件中去，所以ifconfig命令打印的结果会输出到文件中去，这样父进程就会从该文件里读到子进程执行该命令的结果；*/
	    
	     execl("/sbin/ifconfig","ifconfig","ens33",NULL);
/*  execl()函数并不会返回，因为他去执行另一个程序了，如果execl（）返回了，说明该系统调用出错了；*/
	     printf("Child process excute another program,will not return here.Return here means execl() error\n");
	     return -1;
	}
        else 
	{
	     //父进程等待3s
	     sleep(3);
	}

	//子进程因为调用了execl();它会丢掉父进程的文本段，所以子进程不会执行到这里了。只有父进程会继续执行这后面的代码
	memset(buf,0,sizeof(buf));
	//父进程这时候读是读不到内容的，这是因为子进程往文件里写入内容时将文件偏移量修改至文件尾
	rv = read(fd,buf,sizeof(buf));
	printf("Read %d bytes data dierectly read after child process write\n",rv);
	//父进程如果要读则需要将文件偏移量设置到文件头才能读到内容
	memset(buf,0,sizeof(buf));
	lseek(fd,0,SEEK_SET);
	rv = read(fd,buf,sizeof(buf));
	printf("Read %d bytes data after lseek:\n %s",rv,buf);
	//如果使用read（）读的话，一下子就读了N多个字节进buffer，但有时我们希望一行一行地读取文件的内容，这时可以使用fdopen（）函数将文件描述符fd转化为文件流fp
	fp = fdopen(fd,"r");
        fseek(fp,0,SEEK_SET);//重新设置文件偏移量到文件头
	while(fgets(buf,sizeof(buf),fp))//fgets()从文件里一下子读一行，如果读到文件尾则返回NULL
	{
                 /*
		  包含IP地址的那一行有netmask关键字，如果在该行中找到关键字就可以从里面解析出IP地址*/
		if(strstr(buf,"netmask"))
		{
		 //查找inet关键字，inet关键字后面跟的是IP地址；
		 ptr = strstr(buf,"inet");
		 if(!ptr)
		 {
		       break;
		 }
                 ptr+= strlen("inet");
		 //inet 关键字后面是空白符，我们不确定是空格还是TAB，所以这里用lsblan()函数断如果字符还是空白符就往后跳；
                 while(isblank(*ptr))
			 ptr++;
		 //跳过空白符后跟着就是IP地址的起始字符；
		 ip_start = ptr;
		 //ip地址后面跟着空白符，跳过所有的非空白符，即IP地址的部分：xxx.xxx.xxx.xxx
		 while(!isblank(*ptr))
			 ptr++;
		 ip_end = ptr;
		 //使用memcpy()函数将IP地址拷贝放到存放IP地址的buffer中，其中ip——end-ip——start就是IP地址的长度,ip_start就是IP地址的起始位置；
                 memset(ipaddr,0,sizeof(ipaddr));
		 memcpy(ipaddr,ip_start,ip_end-ip_start);
		 break;

		}



        
	} 
    printf("Parser and get IP address : %s\n",ipaddr);
    fclose(fp);
    unlink(TMP_FILE);
    return 0;

}
