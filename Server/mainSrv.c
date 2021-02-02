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
#include <signal.h>
#include <mysql.h>

#include "User.h"
#include "Holder.h"
#include "Server.h"
#include "Database.h"
#include "config_parser.h"
#include "Utility.h"

#define MAX_CLIENTS 50

void sighandler(int signum)
{
	print_log("Shuting down the Server");

 	__FreeDatabaseConnection();
 	__FreePData();
	__FreeAllHolders();

	exit(EXIT_SUCCESS);
}


int main(int argv,char* argc[])
{

	if(signal(SIGINT,sighandler)==SIG_ERR)
		bail("signal handler()");
	

	fd_set read_set,wread_set; 
	fd_set write_set,wwrite_set;
	
	unsigned int max_fds; /** Max fds + 1 **/
	unsigned int cln_count=0;
	
	int len_inet=sizeof(struct sockaddr_in);
	int len_inet2=sizeof(struct sockaddr_in);
	
	struct sockaddr_in cln_addr;

	int srv_socket;
	int cln_socket;
	unsigned r_byte;
	
	struct hostent *hp;
	
	FILE *log;
	
	time_t tm=time(NULL);
	struct tm *date=NULL;
	
	PACKET *packet=malloc(sizeof(PACKET));
	if(!packet)
	{
		bail("[SERVER] Not enough Memory");
	}
	
	if(argv < 2 )
	{
		fprintf(stderr,"usage : %s <config_file> \n",argc[0]);
		return EXIT_FAILURE;
	}
	
	__ParseCfgFile(argc[1]);
	__InitDatabaseConnection();
	srv_socket=__InitServer();
			

	FD_ZERO(&read_set); /**** Intialize read_set ****/
	FD_SET(srv_socket,&read_set); /**** Adding server socket to the set otherwise we can't receive connections events ****/
	
	max_fds=srv_socket + 1 ;
	
	/*** Open file log ***/
	log=fopen("log.file","a+");
	if (log==NULL)
		print_logerr("[SERVER] Cannot open logfile");
	
	while(true)
	{
		
		Bzero(packet,sizeof(PACKET));
		/**** Copy the read_set on working read__set this is because select call is destructive which mean select return the set will contain only socket that have data to read ***/
		wread_set=read_set;

		if(select(max_fds,&wread_set,NULL,NULL,NULL)<0) // <--- select will block until an events arrives
			bail("select()");
	
		
		for( int i=0;i<max_fds;i++)
		{
			/*** Refreshing the time ***/
			tm=time(NULL);
			date=localtime(&tm);
			char *mem=asctime(date);
			*(mem+strlen(mem)-1)='\0'; // removing the \n charactere. Info : asctime works on a static buffer

			
			/**** Zeroing Client Address byte ***/
			Bzero(&cln_addr,len_inet);	
			
			if(FD_ISSET(i,&wread_set))
			{
			
				if(i==srv_socket)
				{
					/* This mean that the server has a connection that need to be accepted.
					    Connections are considered a read event that select will respond to 
					*/
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
								fprintf(log,"[%s] %s:%s\n",((!date)?"NULL":mem),inet_ntoa(cln_addr.sin_addr),hstrerror(h_errno));
							else	
								fprintf(log,"[%s] : %s:%s\n",((!date)?"NULL":mem),inet_ntoa(cln_addr.sin_addr),hp->h_name);
							fflush(log);
						}
						
						fprintf(stdout,"[%s] : %s:%s Client connected\n",((!date)?"NULL":mem),inet_ntoa(cln_addr.sin_addr),(!hp)?"Unkown":hp->h_name);
							

						/****** Adding holder which gonna hold the client *******/
						if(!__AddHolder(cln_socket,(!hp)?"UNKNOWN":hp->h_name,inet_ntoa(cln_addr.sin_addr)))
						{
							close(cln_socket);
							continue;
						}
						else
							fprintf(stdout,"[%s] : Adding a holder for sock %d\n",(!date)?"NULL":mem,cln_socket);
						

						
						/*************** Add client_socket to the read_set **********/
						FD_SET(cln_socket,&read_set);
					
						__SendWELCM(cln_socket);
					}
					
				}
				else
				{
					r_byte=(int)recv(i,packet,sizeof(PACKET),0);
					if( !r_byte )
					{
						if(!__RemoveHolder(i))
							fprintf(stderr,"[%s] : Can't remove holder for sock %d\n",(!date)?"NULL":mem,i);
						else
							fprintf(stdout,"[%s] Removing holder for sock  %d\n",(!date)?"NULL":mem,i);
						FD_CLR(i,&read_set);
						close(i);
						continue;	
					}
					if( r_byte < 0)
					{
						print_log("[SERVER] Read PACKET ERROR ");
						continue;
					}	
					
					fprintf(stdout,"[SERVER] READ PACKET SIZE OF %d from sock %d\n",r_byte,i);	
					__HandleMessage(i,packet);
				}
			}
		}

	}
	
	/****** Control never get here ******/ 

	return EXIT_SUCCESS;
}
