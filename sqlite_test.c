/*********************************************************************************
 *      Copyright:  (C) 2023 Cenketao<1697781120@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite_test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年04月18日)
 *         Author:  Cenketao <1697781120@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月18日 17时56分20秒"
 *                 
 *******************************************************************************/

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include<math.h>
int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   rc = sqlite3_open("test.db", &db);

   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }
   sqlite3_close(db);
}
