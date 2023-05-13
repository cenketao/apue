#include<stdio.h>
#include<unistd.h>


#define TEST_FILE "access.c"
int main(void)
{
	if(access(TEST_FILE,F_OK)!=0)
	{
		printf("File %s not exist!\n",TEST_FILE);
		return 0;
	}
	printf("File %s exist!\n",TEST_FILE);
	if (access(TEST_FILE,R_OK)==0)
		printf("READ OK\n");
	if (access(TEST_FILE,W_OK)==0)
                printf("WRITE OK\n");
	if (access(TEST_FILE,X_OK)==0)
                printf("EXEC OK\n");
	return 0;
}



