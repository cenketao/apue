/*********************************************************************************
 *      Copyright:  (C) 2023 CKT
 *                  All rights reserved.
 *
 *       Filename:  database.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(05/05/23)
 *         Author:  Cenketao <1481155734@qq.com>
 *      ChangeLog:  1, Release initial version on "05/05/23 16:08:01"
 *                 
 ********************************************************************************/

#include"database.h"
#include<stdio.h>
#include <stdlib.h>
#include<sqlite3.h>
 
int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
static sqlite3    *db=NULL;

int sqlite_init(char *db_name)
  {
     char *zErrMsg = 0;
     int  rc;
         char sql[128];
         if(!db_name)
         {
          printf("ERROR:Invalid input arguments\n");
                  return -1;
         }

     /* Open database */
     rc = sqlite3_open(db_name, &db);
     if( rc )
         {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -2;
     }
      memset(sql,0,sizeof(sql));
          snprintf(sql,sizeof(sql),"CREATE TABLE TEMPERATURE(ID INTEGER PRIMARY KEY AUTOINCREMENT,NAME CHAR(20),TEMPERATURE CHAR(20),TIME CHAR(20));");
     /* Create SQL statement */

     /* Execute SQL statement */
     rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
     if( rc != SQLITE_OK )
         {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     }
     else
     {
        fprintf(stdout, "Table created successfully\n");
     }
     return 0;
  }

int sqlite_insert(char *dbname,message_s *pack_info)
{
           char  sql[512] = {0};
                   int   rc      = -1;
           char  *zErrMsg = 0;

            memset(sql,0,sizeof(sql));
            sprintf(sql,"INSERT INTO TEMPERATURE VALUES (NULL,'%s','%f','%s');",pack_info->name,pack_info->temp,pack_info->time1);
           rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
           if( rc != SQLITE_OK )
           {
             fprintf(stderr, "SQL error: %s\n", zErrMsg);
             sqlite3_free(zErrMsg);
           }
           else
           {
             fprintf(stdout, "Table iinsert  successfully\n");
           }
       return 0;

}

int  get_table(char *dbname,message_s *pack_info)
  {
         char *zErrMsg = 0;
         int  rc;
         char **dbResult;
         int  nRow = 0;
         int  nColumn = 0;//查出的总行数，存储列数
         char sql[128]="SELECT *from TEMPERATURE LIMIT 1";
     /* Open database */
     rc = sqlite3_open(dbname, &db);
     if( rc )
     {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
     }
     /* Create SQL statement */

     /* Execute SQL statement */
     rc = sqlite3_get_table(db, sql,&dbResult, &nRow,&nColumn, &zErrMsg);
     if( rc != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
     }
     else
     {
        fprintf(stdout, "Table select successfully\n");
     }
         strcpy(pack_info->name,dbResult[5]);
         pack_info->temp=atof(dbResult[6]);
         strcpy(pack_info->time1,dbResult[7]);

     return nRow;
  }
int sqlite_delect(char *dbname)
{
      char sql[128] = {0};
          int  rc = -1;
          char *zErrMsg = 0;
    memset(sql,0,sizeof(sql));
        snprintf(sql,sizeof(sql),"DELETE FROM TEMPERATURE WHERE rowid IN (SELECT rowid FROM TEMPERATURE LIMIT 1);");
        rc  = sqlite3_exec(db,sql,callback,0,&zErrMsg);
        if(rc != SQLITE_OK)
    {
        printf("Sqlite_delete_data error:%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    printf("Delete first data successfully!\n");
    return 0;
}
int sqlite_close(void)
{
    char        *zErrMsg = NULL;

    if( SQLITE_OK != sqlite3_close(db) )
    {
        printf("Error close database: %s\n", zErrMsg );
        sqlite3_free(zErrMsg);
    }
    printf("successfully close database\n");
    return 0;
}


