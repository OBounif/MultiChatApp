#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "Server.h"
#include "Message_parser.h"
#include "Utility.h"

static void __ProcessHelpC(char*,Msg*,int);
static void __ProcessWhoC(char*,Msg*,int);
static void __ProcessMpC(char*,Msg*,int);
static void __ProcessLoginC(char*,Msg*,int);
static void __ProcessRegC(char*,Msg*,int);
static void __ProcessLogoutC(char*,Msg*,int);




Msg*  __ParsePACKET(PACKET* packet)
{
	unsigned i=0;
	unsigned length=0;
	Msg* new = (Msg*) malloc(sizeof(Msg));
	if(!new)
	{
		print_logerr("[MESSAGE PARSER] Not enough memory"); 	
		return NULL;
	}

	Bzero(new,0);
	new->from_user=NULL;
	new->to_user=NULL;		

	if(packet->msg[0]!='/')
		goto NORMAL_MESSAGE;
	else
		goto COMMAND_MESSAGE;

NORMAL_MESSAGE:
	new->type=NONE;	
	length=0;
	i=0;
	
	while(packet->msg[i++]!='\0' && i<(DATA_SIZE-1))
		length++;	
				
	if(length > 0)
	{
		if(i==DATA_SIZE-1)
		{
			/* Long Message are rejected  */
			packet->msg[DATA_SIZE-1]='\0'; // <-- for not having a beautiful segfault error when i use this variable for reading or copying ...
			new->type=NONE_ERROR;		
		}
		else
		{
			new->data=malloc(length*sizeof(char));	
			strcpy(new->data,packet->msg);
		}
	}
	goto END;

COMMAND_MESSAGE:
	i++;
	char* command=NULL;				
	
	
	if(isspace(packet->msg[i]))
		printf("%d\n",i);
	
	while( isspace(packet->msg[i]) && i <(DATA_SIZE-1) )	
	{
		i++;
	}

	if(i==DATA_SIZE-1)
	{
		new->type=UNKNOWN_C;
		goto END;
	}
	
	for(;packet->msg[i]!='\0' && i<(DATA_SIZE-1);i++)
	{
		if(isblank(packet->msg[i]))
			break;

		
		char* tmp=realloc(command,(++length)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(command);
			new->type=TRY_AGAIN;
			goto END;	
		}		
		command=tmp;	
		*(command+length-1)=packet->msg[i];		
		
	}
			
	if(i <= DATA_SIZE-1 || packet->msg[i] == '\0')
	{
		/* Adding terminating string */	

		char* tmp=realloc(command,(++length)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(command);
			new->type=TRY_AGAIN;
			goto END;	
		}		
		command=tmp;	
		*(command+length-1)='\0';		
	}

	printf("Command = %s\n",command);

	if(!strcasecmp(command,"who"))
		 __ProcessWhoC(packet->msg,new,i);
	else if(!strcasecmp(command,"help"))
		 __ProcessHelpC(packet->msg,new,i);
	else if(!strcasecmp(command,"mp"))
		 __ProcessMpC(packet->msg,new,i);
	else if(!strcasecmp(command,"close"))
		 __ProcessLogoutC(packet->msg,new,i);
	else if(!strcasecmp(command,"login"))
		 __ProcessLoginC(packet->msg,new,i);
	else if(!strcasecmp(command,"logout"))
		 __ProcessLogoutC(packet->msg,new,i);
	else if(!strcasecmp(command,"register"))
		 __ProcessRegC(packet->msg,new,i);
	else
	{
	
	}	

	free(command);
END:	
	return new;

}


static void __ProcessHelpC(char* data,Msg* ms,int index)
{
	while(index < DATA_SIZE && data[index]!='\0')
	{
		if(!isspace(data[index]))
		{
			ms->type=UNKNOWN_C;
			return;
		}	
		index++;
	}
	ms->type=HELP_N;
	return;
}

static void __ProcessWhoC(char* data,Msg* ms,int index)
{
	while(index < DATA_SIZE && data[index]!='\0')
	{
		if(!isspace(data[index]))
		{
			ms->type=UNKNOWN_C;
			return;
		}	
		index++;
	}

	ms->type=WHO_N;
}

static void __ProcessMpC(char* data,Msg* ms,int index)
{
	unsigned ch=0;
	printf("test\n");	
	
	//Read to_user
	ms->to_user=malloc(sizeof(char));

	if(!ms->to_user)
	{
		print_logerr("[MESSAGE PARSER] Not enough memory"); 	
		ms->type=TRY_AGAIN;
		return ;
	}
	
	while(index < DATA_SIZE && isspace(data[index++]));		
	
	for(--index;index < DATA_SIZE-1 && data[index]!='\0';index++)
	{
		if(isspace(data[index]))
			break;		
		
		char* tmp=realloc(ms->to_user,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->to_user);
			ms->type=TRY_AGAIN;
		}
		ms->to_user=tmp;
		ms->to_user[ch-1]=data[index];
	}
	
	if(index <= DATA_SIZE-1 || data[index]=='\0')
	{
		char* tmp=realloc(ms->to_user,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->to_user);
			ms->type=TRY_AGAIN;
		}
	
		ms->to_user=tmp;
		ms->to_user[ch-1]='\0';
	}

	printf("User = %s\n",ms->to_user);
	//Read data
	ch=0;
	ms->data=malloc(sizeof(char));
	if(!ms->data)
	{
		print_logerr("[MESSAGE PARSER] Not enough memory"); 	
		ms->type=TRY_AGAIN;
		return ;
	}

	while(index < DATA_SIZE-1 && data[index]!='\0')
	{
		char* tmp=realloc(ms->data,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->data);
			ms->type=TRY_AGAIN;
		}
		ms->data=tmp;
		ms->data[ch-1]=data[index];

		index++;
	}
	
	if(index <= DATA_SIZE-1 || data[index]=='\0')
	{
		char* tmp=realloc(ms->data,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->data);
			ms->type=TRY_AGAIN;
		}
	
		ms->data=tmp;
		ms->data[ch-1]='\0';
	}

	printf("Data=%s\n",ms->data);
}

static void __ProcessLoginC(char* data,Msg* ms,int index)
{
}

static void __ProcessLogoutC(char* data,Msg* ms,int index)
{

}

static void __ProcessRegC(char* data,Msg* ms,int index)
{

}


void main()
{
	PACKET packet;
	int i=0;
	char c;
	while(1)
	{
		while(c!=EOF && c!='\n')
		{
			c=getchar();	
			packet.msg[i++]=c;
		}
		c=0;
		packet.msg[i]='\0';
		i=0;
		__ParsePACKET(&packet);
		
	}

}


