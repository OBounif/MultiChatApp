#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <mysql.h>


#include "Client.h"
#include "Utility.h"
#include "Database.h"

static unsigned __GetClientId(char const*,MYSQL*);
static Client* __BuildClient(char*,char*,char*,char*);
static bool __IsExist(char const*,MYSQL *);


static char* SQL_INSERT="INSERT INTO USERS (userName,password,lastIp,role,isBanned) values('%s','%s','%s','%s',%d)";
static char* SQL_DELETE="DELETE from USERS where id=%d"; 
static char* SQL_UPDATE="UPDATE USERS SET %s='%s' where id=%d";
static char* SQL_SELECT="SELECT * from USERS where userName='%s'"; 
static char* SQL_SELECTID="SELECT ID from USERS where userName='%s'"; 


Client* __AddClient(char* userName,char* password,char* lastIp,char*rank)
{
	MYSQL *connection = __GetMysqlCon();
	Client* client=NULL;
	char buffer[BUFFER_LENGTH];
	Bzero(buffer,sizeof buffer);

	if(!connection)
	{	
		print_logerr("{ADD}Connection has not been initialized");	
		return NULL;
	}
	
	if(__IsExist(userName,connection))
	{
		print_logerr("Client Exist");
		return NULL;
	}
	
	client=__BuildClient(userName,password,lastIp,rank);
	if(!client)
		return NULL;
	
	/*Zeroing buffer*/
	Bzero(buffer,sizeof buffer);
	/*Writing the query on the buffer*/
	sprintf(buffer,SQL_INSERT,client->userName,client->password,client->lastIp,client->rank,client->isBanned);		
	
	
				
	if(mysql_query(connection,buffer))	
	{
		__PrintDError("{ADD}Error while executing insert statement");
		__FreeClient(&client);
		return NULL;
	}

	
	client->id=mysql_insert_id(connection); // Get last inserted id row work only if we used auto-increment opt
		
	return client;
}



Client* __GetClient(char const* userName)
{
	Client* client=NULL;
	MYSQL *connection = __GetMysqlCon();
	char buffer[BUFFER_LENGTH];
	
	Bzero(buffer,sizeof buffer);
	
	sprintf(buffer,SQL_SELECT,userName);		
	
	if(!connection)
	{	
		print_logerr("{GET}Connection has not been initialized");	
		return NULL;
	}
	
	if(mysql_query(connection,buffer))	
	{
		__PrintDError("{GET}Error while executing select statement");
		return NULL;
	}
		
	MYSQL_RES *result=mysql_store_result(connection);
	if(!result)
	{
		__PrintDError("{GET}Error while storing results ");
		return NULL;
	}

	MYSQL_ROW row;
	int num_field=mysql_num_fields(result);
	row=mysql_fetch_row(result);
		
	
	if(row)
	{	
		client=__BuildClient(row[1],row[2],row[3],row[4]);	
		client->id=atoi(row[0]);
		client->isBanned=atoi(row[5]);
	}
	
	mysql_free_result(result);	

	return client;
}

bool __ModClient(unsigned id,char const* field,char const* value)
{
	MYSQL *connection = __GetMysqlCon();
	char buffer[BUFFER_LENGTH];
	
	Bzero(buffer,sizeof buffer);
	
	sprintf(buffer,SQL_UPDATE,field,value,id);

	if(!connection)
	{	
		print_logerr("{MOD}Connection has not been initialized");	
		return false;
	}
	
	if(mysql_query(connection,buffer))	
	{
		__PrintDError("{MOD}Error while executing update statement");
		return false;
	}
	else
	{
		if(mysql_affected_rows(connection)!=1)
			return false;
				
	}
	return true;		
}


bool __RemoveClient(unsigned id)
{
	MYSQL *connection = __GetMysqlCon();
	char buffer[BUFFER_LENGTH];
	
	Bzero(buffer,sizeof buffer);
	
	sprintf(buffer,SQL_DELETE,id);

	if(!connection)
	{	
		print_logerr("{REM}Connection has not been initialized");	
		return false;
	}
	
	if(mysql_query(connection,buffer))	
	{
		__PrintDError("{REM}Error while executing delete statement");
		return false;
	}
	else
	{
		if(mysql_affected_rows(connection)!=1)
			return false;
				
	}
	return true;
}



void __FreeClient(Client** cl)
{
	free((*cl)->userName);
	free((*cl)->password);
	free((*cl)->lastIp);
	free((*cl)->rank);

	*cl=NULL;
}


static Client* __BuildClient(char* userName,char* password,char* lastIp,char* rank)
{
	Client* new=malloc(sizeof(Client));
	
	if(!new)	
	{
		print_logerr("There is no enough memory for a new client");
		return NULL;
	}
	
	
	new->userName=malloc(NICKNAME*sizeof(char));
	new->password=malloc(PASSWORD*sizeof(char));
	new->lastIp=malloc(LAST_IP*sizeof(char));
	new->rank=malloc(RANK*sizeof(char));
		
	strcpy(new->userName,userName);	
	strcpy(new->password,password);	
	strcpy(new->lastIp,lastIp ? lastIp : "NULL");	
	strcpy(new->rank,rank ? rank: "NULL");	
	new->isBanned=false;

	return new;
}



static bool __IsExist(char const* userName,MYSQL *connection)
{
	char buffer[BUFFER_LENGTH];
	Bzero(buffer,sizeof buffer);

	sprintf(buffer,SQL_SELECTID,userName);		
	
	if(mysql_query(connection,buffer))	
	{
		print_logerr("{ISE}Connection has not been initialized");	
		return true;
	}

	MYSQL_RES *result=mysql_store_result(connection);
	if(!result)
	{
		__PrintDError("{ISE}rror while storing results ");
		return true;
	}
	MYSQL_ROW row;
		
	if((row=mysql_fetch_row(result)))
		return true;

	mysql_free_result(result);	
	
	return false;
}


static unsigned __GetClientId(char const* userName,MYSQL* connection)
{
	unsigned id=-1;	
	char buffer[BUFFER_LENGTH];
	Bzero(buffer,sizeof buffer);
	sprintf(buffer,SQL_SELECTID,userName);		

	if(mysql_query(connection,buffer))
	{
		__PrintDError("Error while executing select statement");
		return -1;
	}	
	
	MYSQL_RES *result=mysql_store_result(connection);
	if(!result)
	{
		__PrintDError("Error while storing results ");
		return -1;
	}
	MYSQL_ROW row;
	
	int num_field=mysql_num_fields(result);

	if((row=mysql_fetch_row(result)))
	{
		id=(unsigned)atoi(row[0]);			
	}

	mysql_free_result(result);	

	return id;	
}
