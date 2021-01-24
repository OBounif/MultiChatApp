#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <mysql.h>


#include "Client.h"
#include "Utility.h"
#include "Database.h"

static Client* __BuildClient(char*,char*,char*,char*);
static bool __IsExist(char const*,MYSQL *);


char* SQL_INSERT="INSERT INTO USERS (userName,password,lastIp,role,isBanned) values('%s','%s','%s','%s',%d)";
char* SQL_SELECT="SELECT ID from USERS where userName='%s'"; 

Client* __AddClient(char* userName,char* password,char* lastIp,char*rank)
{
	MYSQL *connection = __GetMysqlCon();
	Client* client=NULL;
	char buffer[BUFFER_LENGTH];
	Bzero(buffer,sizeof buffer);

	if(!connection)
	{	
		print_logerr("Connection is not initiaded Cannot add client");	
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
		__PrintDError("Error while executing insert statement");
		__FreeClient(&client);
		return NULL;
	}

	return client;
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
		print_logerr("There is no enough memory for a new client try again later");
		return NULL;
	}
	
	
	new->userName=malloc(NICKNAME*sizeof(char));
	new->password=malloc(PASSWORD*sizeof(char));
	new->lastIp=malloc(LAST_IP*sizeof(char));
	new->rank=malloc(RANK*sizeof(char));
		
	strcpy(new->userName,userName);	
	strcpy(new->password,password);	
	strcpy(new->lastIp,lastIp ? lastIp : "NULL");	
	strcpy(new->rank,rank);	
	new->isBanned=false;

	return new;
}



static bool __IsExist(char const* userName,MYSQL *connection)
{
	char buffer[BUFFER_LENGTH];
	Bzero(buffer,sizeof buffer);

	sprintf(buffer,SQL_SELECT,userName);		
	
	if(mysql_query(connection,buffer))	
	{
		__PrintDError("Error while executing select statement");
		return NULL;
	}

	MYSQL_RES *result=mysql_store_result(connection);
	MYSQL_ROW row;
		
	if((row=mysql_fetch_row(result)))
		return true;

	
	mysql_free_result(result);	
	
	return false;
}



void main()
{
	__InitDatabaseConnection();
	__AddClient("test","test","192.168.1.1","ADMINISTRATOR");
}
