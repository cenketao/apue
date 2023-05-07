#include<stdio.h>
#include<string.h>
#include<time.h>


int get_time(char *buf)
{
      time_t            tmp;
      struct tm         *timp;

      time(&tmp);

      timp = localtime(&tmp);

 snprintf(buf,32,"%d-%d-%d %d:%d:%d",(1900+timp->tm_year),(1+timp->tm_mon),timp->tm_mday,timp->tm_hour,timp->tm_min,timp->tm_sec);

}
