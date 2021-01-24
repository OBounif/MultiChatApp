#include <stdio.h>
#include <stdlib.h>

#include <mysql.h>
#include "Database.h"
#include "Utility.h"
#include "config_parser.h"
#include <string.h>


MYSQL* connection=NULL;
bool mysqlServerStatus=false;



bool initDatabaseConnection(void)
{
	print_log("Initiate mysql connection ...");	
	
	connection=mysql_init(NULL);
	if(!connection)
	{
		print_logerr("Failed to initiate connection");
		print_logerr(mysql_error(connection));
		
		
		mysqlServerStatus=false;
	}	
	
	print_log("Connection has been initiated succesful\nEstablishing connection to Database ...");
	mysqlServerStatus=true;
	
	__ParseCfgFile("config");
	
	if(mysql_real_connect(connection,(const char*)__Get("DB_HOST"),(const char*)__Get("DB_USER"),(const char*)__Get("DB_PASSWORD"),(const char*)__Get("DB_NAME"),0,NULL,0)==NULL)
	{
		print_logerr("Failed to establish connection to Database");
		print_logerr(mysql_error(connection));
		freeDatabaseConnection();		
		
		mysqlServerStatus=false;
	}
	else
		print_log("Connection established successfullt to the database");

	return mysqlServerStatus;
}


void freeDatabaseConnection(void)
{
	print_log("Closing mysql connection ...");
	mysql_close(connection);
}


bool getServerStatus(void)
{
	return mysqlServerStatus;
}


