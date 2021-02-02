#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "User.h"
#include "Holder.h"
#include "Utility.h"

static bool __IsExist(unsigned);
static void __FreeHolder(Holder*);
static void __FreeAllHoldersIntern(Holder*);


static Holder *holder=NULL;



bool __AddHolder(int sock,char* h_name,char* ip)
{
	Holder*  new =(Holder*) malloc(sizeof(Holder));

	if(!new)
	{
		print_logerr("[HOLDER] Not enough memory ");
		return false;
	}
	
	new->h_name=malloc(H_NAMESIZE*sizeof(char*));
	new->ip=malloc(IP_SIZE*sizeof(char*));
	new->client=NULL;
	
	
	new->sock=sock;
	strcpy(new->h_name,(h_name)?h_name:"NULL");		
	strcpy(new->ip,ip);		
	
	
	
	if(!holder)
		new->next=NULL;
	else
		new->next=holder;
	
	holder=new;
		
	return true;	
}

bool __HSetClient(int sock,Client *cl)
{
	Holder* curr=holder;
	
	if(__IsExist(cl->id))
		return false;
	
	while(curr)
	{
		if(curr->sock == sock)
		{
			curr->client=cl;
			return true;
		}
		curr=curr->next;
	}	
	
	return false;
}

bool __RemoveHolder(int sock)
{
	Holder* curr=holder;
	Holder*tmp;
	
	/* Checking if its the first holder */
	
	if(curr->sock == sock)	
	{
		tmp=curr;
		holder=curr->next;

		__FreeHolder(holder);
		return true;
	}	
	
	/* Checking in the middle */
	while(curr->next)
	{
		if(curr->next->sock == sock)
		{
			tmp=curr->next;
			curr->next=curr->next->next;

			__FreeHolder(tmp);

			return true;	
		}		
		curr=curr->next;	
	}
	
	return false;	
}

Holder* __GetHolderByCName(char* name)
{
	Holder* curr=holder;

	while(curr)
	{
		if(curr->client)
			if(!strcmp(curr->client->userName,name))
				return curr;
		curr=curr->next;
	}

	return NULL;
}

Holder* __GetHolderBySock(int sock)
{
	Holder* curr=holder;
	while(curr)
	{
		if(curr->sock==sock)
				return curr;
		curr=curr->next;
	}

	return NULL;
}

Holder* __GetHolder(void)
{
	return holder;
}

void __FreeAllHolders()
{
	__FreeAllHoldersIntern(holder);
	holder=NULL;
}

static void __FreeAllHoldersIntern(Holder* holder)
{
	if(!holder)
		return;
	__FreeAllHoldersIntern(holder->next);
	__FreeHolder(holder);
}
static bool __IsExist(unsigned id)
{

	Holder* curr=holder;
	while(curr)
	{
		if(curr->client)
			if(curr->client->id == id)
				return true;
		curr=curr->next;
	}
	return false;
}


static void __FreeHolder(Holder *holder)
{
	if(!holder)
		return;

	free(holder->h_name);
	free(holder->ip);
	__FreeClient(&holder->client);
	free(holder);			
}
