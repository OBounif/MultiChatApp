#include <stdio.h>
#include <stdlib.h>

#include <mysql.h>
#include "Database.h"
#include "Utility.h"


#define DB_NAME "ChatRoom" //DATABASE NAME
#define DB_PASSWORD "A!a123456789" // DATABASE PASSWORD
#define DB_USER "root" // DATABASE USERNAME
#define DB_HOST "localhost" //DATABASE HOST

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
	
	if(mysql_real_connect(connection,DB_HOST,DB_USER,DB_PASSWORD,DB_NAME,0,NULL,0)==NULL)
	{
		print_logerr("Failed to establish connection to Database");
		print_logerr(mysql_error(connection));
		freeDatabaseConnection();		
		
		mysqlServerStatus=false;
	}	

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


void main()
{
	initDatabaseConnection();
}
