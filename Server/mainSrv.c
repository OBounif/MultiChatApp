#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Server.h"

#define MAX_CLIENTS 50

int main(int argv,char* argc[])
{
	
	fd_set read_set,wread_set;
	fd_set write_set,wwrite_set;
	
	unsigned int max_fds; /** Max fds + 1 **/
	
	int len_inet=sizeof(struct sockaddr_in);
	int len_inet2=sizeof(struct sockaddr_in);
	struct sockaddr_in cln_addr;
	int srv_socket;
	int cln_socket;

	struct hostent *hp;
	FILE *log;
	char buf[MSG_SIZE];
	
	time_t tm=time(NULL);
	struct tm *date=NULL;
	unsigned int cln_count=0;

	Msg *msg=malloc(sizeof(Msg));
	if(msg==NULL)
	{
		fprintf(stderr,"Not enought space for msg buffer\n");
		exit(1);
	}
	
	
	/**** Use the address from the command line ****/
	if(argv>=2)
		srv_socket=init_Server(argc[1],0,10);
	else if (argv>=3)
		srv_socket=init_Server(argc[1],htons(atoi(argc[2])),10);
	else
		srv_socket=init_Server(NULL,0,10);

	if(!initDatabaseConnection())
	{
		close(srv_socket);
		exit(1);	
	}	
	FD_ZERO(&read_set); /**** Intialize read_set ****/
	
	FD_SET(srv_socket,&read_set); /**** Adding server socket to the set otherwise we cant receive connection event ****/
	
	max_fds=srv_socket + 1 ;
	
	/*** Open file log ***/
	log=fopen("log.file","a+");
	if (log==NULL)
		fprintf(stderr,"Error while opening file : %s\n",strerror(errno));
	while(true)
	{
		
		/**** Copy the read_set on workingread__set this is because select call is destructive which mean select return the set will contain only socket that have data to read ***/

		wread_set=read_set;

		if(select(max_fds,&wread_set,NULL,NULL,NULL)<0)
			bail("select()");
	
		for( int i=0;i<max_fds;i++)
		{
			/*** Refreshing the time ***/
			tm=time(NULL);
			date=localtime(&tm);
			char *mem=asctime(date);
			*(mem+strlen(mem)-1)='\0'; // removing the \n charactere. Info : asctime works on a static buffer

			/** Zeroing Buffer memory **/
			ZeroMemory(msg,sizeof (Msg));	
			
			/**** Zeroing Client Address byte s***/
			ZeroMemory(&cln_addr,len_inet);	
			
			if(FD_ISSET(i,&wread_set))
			{
			
				if(i==srv_socket)
				{
					/** This mean that the server has a connection that need to be accepted.
					    Connections are considered a read event that select will respond to 
					**/
					cln_socket=accept(srv_socket,(struct sockaddr*)&cln_addr,&len_inet);
					
					if(++cln_count>MAX_CLIENTS)
					{
						cln_count--;
						close(cln_socket);
						continue;
					}
					else
					{  					
						max_fds=((cln_socket+1)>max_fds)?(cln_socket+1):max_fds;

						
						/**************** Save client information *******************/
						hp=gethostbyaddr(&cln_addr.sin_addr,sizeof(cln_addr.sin_addr),cln_addr.sin_family);
							
						if(log!=NULL)
						{
							if(!hp)
								fprintf(log,"[%s] %s:%s\n",((date==NULL)?"NULL":mem),inet_ntoa(cln_addr.sin_addr),hstrerror(h_errno));
							else	
								fprintf(log,"[%s] : %s:%s\n",((date==NULL)?"NULL":mem),inet_ntoa(cln_addr.sin_addr),hp->h_name);
							fflush(log);
						}
						
						fprintf(stdout,"[%s] : %s:%s client connected\n",((date==NULL)?"NULL":mem),inet_ntoa(cln_addr.sin_addr),(hp==NULL)?"Unkown":hp->h_name);

						/****** Holding client socket in a holder *******/
						if(!addHolder(cln_socket,(hp==NULL)?"Unkown":hp->h_name,inet_ntoa(cln_addr.sin_addr)))
						{
							close(cln_socket);
							continue;
						}
						else
							fprintf(stdout,"[%s] : Adding a holder for sock %d\n",(date==NULL)?"NULL":mem,cln_socket);
						/*************** Add client_socket to the read_set **********/
						FD_SET(cln_socket,&read_set);
					}
				}
				else
				{
					Holder* holder=findClientbyHolder(i);
					char buff1[20];
					strcpy(buff1,holder->ip);
					
					if(recv(i,msg,sizeof(Msg),0)==0)
					{
						if(holder->client==NULL)
						{
							if(!removeHolder(i))
								fprintf(stderr,"[%s] : Cant remove force_end_task holder for client %s\n",(date==NULL)?"NULL":mem,buff1);
							else
								fprintf(stdout,"[%s] removing forced_end_process holder for client  %s\n",(date==NULL)?"NULL":mem,buff1);
							FD_CLR(i,&read_set);
							close(i);
						}
						else
						{
							char buff[50];
							msg->type=NONE;
							strcpy(msg->user,"NONE");
							sprintf(buff,"[!] %s has leave the chat\n",holder->client->nickName);
							strcpy(msg->msg,buff);
							if(!removeHolder(i))
								fprintf(stderr,"[%s] Cant remove forced_end_process holder for sock %s\n",(date==NULL)?"NULL":mem,buff1);
							else
								fprintf(stdout,"[%s] removing forced_end_process holder for sock  %s\n",(date==NULL)?"NULL":mem,buff1);

							FD_CLR(i,&read_set);
							close(i);
							broadCast(msg,i);
						}
					}
					else
					{	
						if(msg->type==NONE)
						{
							printf("Ww\n");
							broadCast(msg,i);
						}
						else if (msg->type==LOGIN)
						{
							fprintf(stdout,"[%s] : Receiving Login request from sock %s\n",(date==NULL)?"NULL":mem,buff1);
							processLogin(i,msg);
						}
						else if (msg->type==LOGOUT)
						{
							if(!processLogout(i,msg))
								continue;
							else
								FD_CLR(i,&read_set);
						}
						//else if (msg->type==MP)
						//else if (msg->type==WHO)
						//else if (msg->type==HELP)
						else
						{	
							removeHolder(i);	
							FD_CLR(i,&read_set);
							close(i);
						}
					}
				}
			}
		}

	}
	
	/****** Control never get here ******/ 

	return 0;
}
