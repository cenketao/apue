#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <fcntl.h>
#include <unistd.h>



#define TEST_DIR "dir"
int main(int argc,char **argv)
{
	int             rv;
	int             fd1;
	int             fd2;
	DIR             *dirp;
	struct dirent   *direntp;
       //quanxian755
       if(mkdir(TEST_DIR,0755)<0)
       {
            printf("create directory '%s' failure:%s\n",TEST_DIR,strerror(errno));
return -1;
       }
       //gengailujing
       if(chdir(TEST_DIR)<0)
       {
	       printf("Change directory to '%s' failure:%s\n",TEST_DIR,strerror(errno));
	       rv = -2;
	       goto cleanup;
       }
       //chuanjian file1.txt ,quanxian644
       if( (fd1=creat("file1.txt",0644)) < 0 )
       {
	       printf("create file1.txt failure :%s\n",strerror(errno));
	       rv = -3;
	       goto cleanup;
       }
       //chuanjian file2.txt ,quanxian644
        if((fd2 = creat("file2.txt",0644))<0)
       {
                printf("create file2.txt failure :%s\n",strerror(errno));
		rv = -4;
		goto cleanup;
       }
//huidoufumulu
        if(chdir("../")<0)
	{
		printf("Change directory to '%s' failure:%s\n",TEST_DIR,strerror(errno));
		rv = -5;
		goto cleanup;
	}
	//dakai  dir
	if((dirp=opendir(TEST_DIR)) == NULL)
	{
		rv = -6;
		printf("opendir %s error %s\n",TEST_DIR,strerror(errno));
		goto cleanup;
	}
	//liechu dir suoyouwenjianhewnejianjia
	while((direntp = readdir(dirp))!= NULL)
	{
		printf("Find file: %s\n",direntp->d_name);
	}
	closedir(dirp);
cleanup:
	if(fd1>=0)
	{
		close(fd1);
	}
	if(fd2>=0)
	{
		close(fd2);
	}
	return 0;

}

