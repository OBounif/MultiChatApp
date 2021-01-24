#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "Database.h"
#include "Utility.h"
#include "config_parser.h"



MYSQL* connection=NULL;
bool mysqlServerStatus=false;



void __InitDatabaseConnection(void)
{
	print_log("Initiate mysql connection ...");	
	
	connection=mysql_init(NULL);
	if(!connection)
		__AnormalAbortCon("Failed to initiate connection");
	
	print_log("Connection has been initiated succesful\nEstablishing connection to Database ...");
	mysqlServerStatus=true;
	
	__ParseCfgFile("config");
	
	if(mysql_real_connect(connection,(const char*)__Get("DB_HOST"),(const char*)__Get("DB_USER"),(const char*)__Get("DB_PASSWORD"),(const char*)__Get("DB_NAME"),0,NULL,0)==NULL)
	{
		__AnormalAbortCon("Failed to establish connection to Database");
	}
	else
		print_log("Connection established successfullt to the database");

}


void __AnormalAbortCon(char const* str)
{
	print_logerr(str);
	print_logerr(mysql_error(connection));
	mysql_close(connection);
	mysqlServerStatus=false;
}

void __FreeDatabaseConnection(void)
{
	print_log("Closing mysql connection ...");
	mysql_close(connection);
}

bool __GetMysqlConStatus(void)
{
	return mysqlServerStatus;
}

MYSQL* __GetMysqlCon(void)
{
	return connection;
}


void __PrintDError(char const* str)
{
	fprintf(stderr,"%s : %s\n",str,mysql_error(connection));
}
