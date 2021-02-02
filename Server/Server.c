#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For byte ordering

#include "Server.h"
#include "User.h"
#include "Holder.h"
#include "Utility.h"
#include "Message_parser.h"
#include "config_parser.h"
#include "Color.h"

static void __SendPACKET(int,PACKET*);
static void  __SendBroadCast(PACKET*,int);



static char const* LOGIN_ERROR=RED_N" UserName or Password are incorrectes"RESET;
static char const* NOT_LOGGED=RED_N" You should login for joining the chat"RESET;

static char const* SERVER_INFO=WHITE_B" [SERVER INFO] : %s"RESET;
static char const* WELCOME_MESSAGE=GREEN_N"\n\tWelcome to ChatRoom V1.0\n\tYou Should login for join the chat"RESET;
static char const* GOODBYE_MESSAGE=GREEN_N" GOOD BYE HAVA A NICE DAY"RESET;






static char const* HELP_MESSAGE=	WHITE_B
					"who : see who is online\npm : send private message\n"
					"kick : kick a user from the server\n"
					"ban : ban permantetly a user from the server\n"
					"close : disconect from the server"
					RESET;
static char const* TRY_AGAIN_MESSAGE=GREEN_N" An Internal Error has occured try again later"RESET;
static char const* M_T_L=RED_N" Message too long"RESET;


static char const* USER_NOT_FOUND=RED_N" User %s not found"RESET;
static char const* USER_JOINED=GREEN_N" %s "RESET BLUE_N"has joined the chat"RESET;
static char const* USER_A_L_I=RED_N" You are already logged in"RESET;

static char const* BAN_MESSAGE=RED_N" YOU HAVE BEEN BANNED PERMANETLY BY "RESET WHITE_B"%s"RESET;
static char const* KICK_MESSAGE=RED_N" YOU HAVE BEEN KICKED BY "RESET WHITE_B"%s"RESET;




static char const* COMMAND_NOT_FOUND=RED_N"Command not found"RESET;
static char const* COMMAND_NOT_AVAILABLE=GREEN_N"Command not available"RESET;


int __InitServer(void)
{
	int server_fd; 
	unsigned len_inet;
	struct sockaddr_in srv_addr;
	unsigned port;
	unsigned backlog;
	char const*port_str=(char const*)__Get("SERVER_PORT");
	char const*ip_str=(char const*)__Get("SERVER_IP");
	char const*backlog_str=(char const*)__Get("SERVER_BACKLOG");

	if(!port_str)
		bail("[SERVER] Cannot find port number in configuration file");
	if(!ip_str)
		bail("[SERVER] Cannot find ip in configuration file");
	if(!ip_str)
		bail("[SERVER] No backlog number in configuration file");
	
	port=atoi(port_str);
	backlog=atoi(backlog_str);
	
	
	/** Zeroing serv_addr bytes **/
	Bzero(&srv_addr,sizeof (struct sockaddr_in));
	
	/******** Forming IPv4 Address *********/
	
	srv_addr.sin_family=AF_INET;
	
	if(port < MIN_PORTNUMBER || port > MAX_PORTNUMBER )	
		bail("[SERVER] Invalid port number");

	srv_addr.sin_port=htons(port); /* Port must be in byte order */

	if(!inet_aton(ip_str,&srv_addr.sin_addr))
		bail("[SERVER] Bad address");
	
	
	len_inet=sizeof (struct sockaddr_in);
	/********* Create a Tcp socket *****************/

	server_fd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(server_fd<0)
		bail("socket()");

	/****** Bind the socket to the address *********/
	if(bind(server_fd,(struct sockaddr*)&srv_addr,len_inet)<0)
		bail("bind()");
	else
		fprintf(stdout,"[SERVER] Binding Server to %s:%s\n",ip_str,port_str);
	
	/****** Make it a listening socket *************/
	if(listen(server_fd,backlog)<0)
		bail("listen()");
	
	return server_fd;
}




void __HandleMessage(int sock,PACKET* ms)
{
	Msg* msg=__ParsePACKET(ms);
	PACKET* packet=malloc(sizeof(PACKET));
	Holder* holder=__GetHolderBySock(sock);	
	Client* client=NULL;
	
	char buffer[DATA_SIZE];	
	Bzero(packet,sizeof(PACKET));
	Bzero(buffer,sizeof(buffer));
	
	if(!holder)
	{
		print_logerr("[SERVER] Anonnymous data : User not found"); 
		__FreeMsg(&msg);
		free(packet);
		return;
	}
	
	if(!(holder->client))	
	{
		
		if(msg->type == LOGIN_N || msg->type == REGISTER_N || msg->type == UNKNOWN_C || msg->type == NOT_AVAILABLE );
		else
		{
			strcpy(packet->msg,NOT_LOGGED);
			__SendPACKET(sock,packet);
			__FreeMsg(&msg);
			free(packet);
			return;
		}
	}

	if(!packet)
	{
		print_logerr("[SERVER] Not enough memory for PACKET");
		__FreeMsg(&msg);
		free(packet);
		return ;
	}
	switch(msg->type)
	{
		case NONE :
			if(!strcasecmp(holder->client->rank,"Admin"))
				sprintf(buffer,RED_N"%s"RESET" : %s",holder->client->userName,msg->data);
			else
				sprintf(buffer,GREEN_N"%s"RESET" : %s",holder->client->userName,msg->data);
				
			strcpy(packet->msg,buffer);	
			__SendBroadCast(packet,sock);
			break;
		case HELP_N:
			strcpy(packet->msg,HELP_MESSAGE);
			__SendPACKET(sock,packet);
			break;
		case MP_N:
			holder=__GetHolderBySock(sock);
			
			sprintf(buffer,BLUE_N"from "RESET"%s : %s",holder->client->userName,msg->data);
			strcpy(packet->msg,buffer);
			
			holder=__GetHolderByCName(msg->to_user);
			if(!holder)
			{
				Bzero(buffer,sizeof(buffer));
				sprintf(buffer,USER_NOT_FOUND,msg->to_user);
				strcpy(packet->msg,buffer);
				__SendPACKET(sock,packet);
			}
			else
				__SendPACKET(holder->sock,packet);
			break;
		case WHO_N:
			holder=__GetHolder();	
			
			while(holder)
			{
				if(holder->client)
				{
					if(!strcasecmp(holder->client->rank,"Admin"))
						sprintf(buffer,"%s"RED_N" %s"RESET,buffer,holder->client->userName);
					else
						sprintf(buffer,"%s"GREEN_N" %s"RESET,buffer,holder->client->userName);
				}
				holder=holder->next;
			}
			
			sprintf(packet->msg,"\n%s",buffer);
			__SendPACKET(sock,packet);
			break;
		
		case LOGIN_N:
			client=__GetClient(msg->from_user);
			if(!client)
			{
				strcpy(packet->msg,LOGIN_ERROR);
				__SendPACKET(sock,packet);
				return;
			}
			if(!strcmp(client->userName,msg->from_user) && !strcmp(client->password,msg->data))
			{
				if(__HSetClient(sock,client))
				{
					sprintf(buffer,USER_JOINED,msg->from_user);
					strcpy(packet->msg,buffer);
					__SendBroadCast(packet,sock);
					strcpy(packet->msg,GREEN_N"You have joined the chat have fun!"RESET);
					__SendPACKET(sock,packet);
				}
				else
				{
					strcpy(packet->msg,USER_A_L_I);
					__SendPACKET(sock,packet);
				}
			}
			else
			{
				strcpy(packet->msg,LOGIN_ERROR);
				__SendPACKET(sock,packet);
			}

			break;
		case REGISTER_N:
										
			break;
		
		case BAN_N:

			break;
		
		case NONE_ERROR:
			strcpy(packet->msg,M_T_L);
			__SendPACKET(sock,packet);
			break;
		case NOT_AVAILABLE:
			strcpy(packet->msg,COMMAND_NOT_AVAILABLE);
			__SendPACKET(sock,packet);
			break;
		case UNKNOWN_C:
			strcpy(packet->msg,COMMAND_NOT_FOUND);
			__SendPACKET(sock,packet);
			break;
		
		default:
			print_logerr("UNKNOWN Message type" );
			break;
	}

	__FreeMsg(&msg);
	free(packet);
}

void __SendWELCM(int sock)
{
	PACKET* packet=malloc(sizeof(PACKET));	
	Bzero(packet,sizeof(PACKET));
	if(!packet)
	{
		print_logerr("[SERVER] Not enough memory for PACKET");
		return ;
	}

	strcpy(packet->msg,WELCOME_MESSAGE);
	if( send(sock,packet,sizeof(PACKET),0) < 0)
	{
		print_logerr("[SERVER] Send Error");
	}
	
	free(packet);
}


static void __SendPACKET(int sock,PACKET *ms)
{
	if( send(sock,ms,sizeof(PACKET),0) < 0)
	{
		print_logerr("[SERVER] Send Error");
	}
}


static void  __SendBroadCast(PACKET*ms,int sock)
{
	Holder* holder=__GetHolder();
	
	while(holder)
	{
		if(holder->sock!=sock)	
			if(holder->client)
				if(send(holder->sock,ms,sizeof(PACKET),0)<0)
					print_logerr("[SERVER] Send Error");
		
		holder=holder->next;
	}
}



