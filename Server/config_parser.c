#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#include "Utility.h"
#include "config_parser.h"



static void __AddEntry(const char*,void*,size_t);
static void __FreePDataIntern(Map*);
static void __ParseLine(char const*);


static Map *parsed_data=NULL;



void __ParseCfgFile(char const* file)
{
	FILE* in=fopen(file,"r");
	char buffer[BUFFER_LENGTH]={0}; //The standard guarantees that a line can contain up to 255 characters

	if(!in)
		bail("[CONFIG_PARSER] Failed to open configuration file");
	
	while(fgets(buffer,sizeof buffer,in))
	{
		if(buffer[0]=='#')
		{
			Bzero(buffer,sizeof(buffer));
			continue;
		}
		__ParseLine(buffer);
		Bzero(buffer,sizeof(buffer));
	}	

	if(fclose(in)==EOF)
		print_logerr("[CONFIG_PARSER] Failed to close stream");	
}


void* __Get(char const* key)
{
	if(!key)
		return NULL;

	Map* curr=parsed_data;

	while(curr)
	{
		if(curr->key==NULL)
			continue;
		if(!strcmp(curr->key,key))
			return curr->data;	
		
		curr=curr->next;
	}

	return NULL;
}


void __FreePData(void)
{
	__FreePDataIntern(parsed_data);
	parsed_data=NULL;
}




static void __AddEntry(char const* key,void* data,size_t data_length)
{
	Map* new=(Map*)malloc(sizeof(Map));
	
	if(!new)
		bail("[CONFIG_PARSER] Not enough Memory");

	
	size_t key_length=strlen(key);

	new->key=(char*)malloc(key_length*sizeof(char));
	new->data=(void*)malloc(data_length*sizeof(unsigned char));
	new->data_length=(size_t*)malloc(sizeof(size_t));
	
	*new->data_length=data_length;		
	if(!data)
		new->data=NULL;
	else
		strcpy(new->data,data);
	
	if(!key)
		new->key=NULL;
	else
		strcpy(new->key,key);
	
	if(!parsed_data)
		new->next=NULL;
	else
		new->next=parsed_data;

	parsed_data=new;
}

static void __FreePDataIntern(Map* entry)
{

	if(!entry)
		return;
	 __FreePDataIntern(entry->next);
	
	free(entry->key);
	free(entry->data);
	free(entry->data_length);
	free(entry);
}

static void __ParseLine(char const* line)
{
	unsigned char ch=0,i;
	size_t length=strlen(line);		
	
	char* l_side=NULL;
	char* r_side=NULL;

	for(i=0;i<length && i<BUFFER_LENGTH ;i++)
	{	
		if( *(line+i) == '=' || (i==length-1) || (i==BUFFER_LENGTH-1))
		{
			/*
			 *	Adding NULL-terminated string  character and jump to the right side	
			 */
			char *tmp=realloc(l_side,(++ch)*sizeof(char));
			if(!tmp)
			{
				free(l_side);
				bail("[CONFIG_PARSER] Not enough Memory");
			}
			l_side=tmp;		
			*(l_side+ch-1)='\0';
			
			goto LEFT_SIDE;	
		}
		else if(isspace(*(line+i)))
			continue;
		else
		{
			char *tmp=realloc(l_side,(++ch)*sizeof(char));
			
			if(!tmp)
			{
				free(l_side);
				bail("[CONFIG_PARSER] Not enough Memory");
			}
			l_side=tmp;	
			*(l_side+ch-1)=*(line + i);

		}
	}
	LEFT_SIDE:
		++i; // Avoid "="
		for(ch=0;i<length && i<BUFFER_LENGTH;i++)
		{
			if( (i==length-1) || (i==BUFFER_LENGTH-1))
			{
				/*
				 *	Adding NULL-terminated string  character and jump to the right side	
				 */
				char *tmp=realloc(r_side,(++ch)*sizeof(char));
				if(!tmp)
				{
					free(r_side);
					bail("[CONFIG_PARSER] Not enough Memory");
				}
				r_side=tmp;		
				*(r_side+ch-1)='\0';
			}
			else if(isspace(*(line+i)))
				continue;
			else
			{
				char *tmp=realloc(r_side,(++ch)*sizeof(char));
				if(!tmp)
				{
					free(r_side);
					bail("[CONFIG_PARSER] Not enough Memory");
				}
				r_side=tmp;
				*(r_side+ch-1)=*(line + i);
			}
		
		}
		
	 
	if(!l_side || !r_side)
		return;		
	__AddEntry(l_side,r_side,(!r_side)?0:strlen(r_side));

	free(l_side);
	free(r_side);
}

