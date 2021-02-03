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
static void __ProcessBanC(char*,Msg*,int);
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

	Bzero(new,sizeof(Msg));
	new->from_user=NULL;
	new->to_user=NULL;		

	if(packet->msg[0]!='/')
		goto NORMAL_MESSAGE;
	else
		goto COMMAND_MESSAGE;

NORMAL_MESSAGE:
	new->type=NONE;	
	i=0;
	new->data=malloc(sizeof(char));
	if(!new->data)
	{
		print_logerr("[MESSAGE PARSER] Not enough Memory");
		new->type=TRY_AGAIN;
		goto END;	
	}
	
	for(;i<(DATA_SIZE-1) && packet->msg[i]!='\0';i++)
	{		
		if(packet->msg[i]=='\n')
			continue;
		
		char* tmp=realloc(new->data,(++length)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			new->type=TRY_AGAIN;
			goto END;	
		}
		new->data=tmp;
		new->data[length-1]=packet->msg[i];	
	}
	if(i<=DATA_SIZE-1 || packet->msg[i]=='\0' )
	{
		char* tmp=realloc(new->data,(++length)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			new->type=TRY_AGAIN;
			goto END;	
		}
		new->data=tmp;
		new->data[length-1]='\0';
	}
	goto END;

COMMAND_MESSAGE:
	i++;
	char* command=NULL;				
	
	
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
		if(packet->msg[i]=='\n')
			continue;
		
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
	else if(!strcasecmp(command,"ban"))
		 __ProcessBanC(packet->msg,new,i);
	else
		new->type=UNKNOWN_C;

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
	
	//Read to_user
	ms->to_user=malloc(sizeof(char));
	ms->type=MP_N;

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
			return ;
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
			return ;
		}
	
		ms->to_user=tmp;
		ms->to_user[ch-1]='\0';
	}

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
		if(data[index]=='\n')
		{
			index++;
			continue;
		}
		char* tmp=realloc(ms->data,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->data);
			ms->type=TRY_AGAIN;
			return ;
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
			return ;
		}
	
		ms->data=tmp;
		ms->data[ch-1]='\0';
	}

}

static void __ProcessLoginC(char* data,Msg* ms,int index)
{
	
	unsigned ch=0;
	//Read from_user aka userName
	ms->from_user=malloc(sizeof(char));
	ms->type=LOGIN_N;
	
	if(!ms->from_user)
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
		
		char* tmp=realloc(ms->from_user,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->from_user);
			ms->type=TRY_AGAIN;
			return;
		}
		ms->from_user=tmp;
		ms->from_user[ch-1]=data[index];
	}
	
	if(index <= DATA_SIZE-1 || data[index]=='\0')
	{
		char* tmp=realloc(ms->from_user,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->from_user);
			ms->type=TRY_AGAIN;
			return;
		}
	
		ms->from_user=tmp;
		ms->from_user[ch-1]='\0';
	}

	//Read data aka password
	ch=0;
	ms->data=malloc(sizeof(char));
	if(!ms->data)
	{
		print_logerr("[MESSAGE PARSER] Not enough memory"); 	
		ms->type=TRY_AGAIN;
		return ;
	}
	
	while(index < DATA_SIZE-1 && isspace(data[index]))
		index++;


	while(index < DATA_SIZE-1 && data[index]!='\0')
	{
		if(data[index]=='\n')
		{
			index++;
			continue;
		}
		char* tmp=realloc(ms->data,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->data);
			ms->type=TRY_AGAIN;
			return;
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
			return;
		}
	
		ms->data=tmp;
		ms->data[ch-1]='\0';
	}
}


static void __ProcessRegC(char* data,Msg* ms,int index)
{
	ms->type=NOT_AVAILABLE;
}

static void __ProcessLogoutC(char* data,Msg* ms,int index)
{
	ms->type=NOT_AVAILABLE;
}


static void __ProcessBanC(char* data,Msg* ms,int index)
{

	/****************************/
	unsigned ch=0;
	
	//Read to_user aka userName
	ms->from_user=malloc(sizeof(char));

	if(!ms->from_user)
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
		
		char* tmp=realloc(ms->from_user,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->from_user);
			ms->type=TRY_AGAIN;
			return ;
		}
		ms->from_user=tmp;
		ms->from_user[ch-1]=data[index];
	}
	
	if(index <= DATA_SIZE-1 || data[index]=='\0')
	{
		char* tmp=realloc(ms->from_user,(++ch)*sizeof(char));
		if(!tmp)
		{
			print_logerr("[MESSAGE PARSER] Not enough memory"); 	
			free(ms->from_user);
			ms->type=TRY_AGAIN;
			return ;
		}
	
		ms->from_user=tmp;
		ms->from_user[ch-1]='\0';
	}

	while(index < DATA_SIZE && data[index]!='\0')
	{
		if(!isspace(data[index]))
		{
			ms->type=UNKNOWN_C;
			return;
		}	
		index++;
	}

	ms->type=NOT_AVAILABLE;	
}

void __FreeMsg(Msg** ms)
{
	free((*ms)->from_user);
	free((*ms)->to_user);
	free((*ms)->data);
	free(*ms);
	
	*ms=NULL;
}
