#include <stdlib.h>
#include <string.h>

#include "Client.h"



Client* addClient(unsigned id,char* nick,char* passw,char* email,char* lastIp,char* rank,struct tm lastConnection,bool isBanned)
{
	Client* new=malloc(sizeof(Client));
	
	if(new ==NULL)	
	{
		fprintf(stderr,"There is no enough memory for new client\n");
		return NULL;
	}
	
	
	new->nickName=malloc(NICKNAME*sizeof(char));
	new->password=malloc(PASSWORD*sizeof(char));
	new->email=malloc(EMAIL*sizeof(char));
	new->lastIp=malloc(LAST_IP*sizeof(char));
	new->rank=malloc(RANK*sizeof(char));
	new->lastConnection=malloc(LAST_CONNECTION_IP*sizeof(char));
	
	new->id=id;	
	strcpy(new->nickName,nick);	
	strcpy(new->password,passw);	
	strcpy(new->email,email ? email : "NULL");	
	strcpy(new->lastIp,lastIp ? lastIp : "NULL");	
	strcpy(new->rank,rank ? rank:"NULL");	
	strcpy(new->lastConnection,lastConnection ? lastConnection : "NULL");
	new->isBanned=false;
	
	
			
	return new;
}


