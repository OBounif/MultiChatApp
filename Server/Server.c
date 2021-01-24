#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <mysql.h>
//#include <my_global.h>

#include "Server.h"
#include "Client.h"


MYSQL *connection=NULL;
bool mysqlServerStatus=false;

Holder* holder=NULL;
bool loginStatus=false;


void ZeroMemory(void *ptr,size_t size)
{
	memset(ptr,0,size);
}

void bail(const char* str)
{
	fprintf(stderr,"%s : %s\n",str,strerror(errno));
	exit(1);
}

int init_Server(char *addr,int port,int backlog)
{
	int server_fd; 
	int status;
	unsigned len_inet;
	struct sockaddr_in srv_addr;
	
	/** Zeroing serv_addr bytes **/
	ZeroMemory(&srv_addr,sizeof (struct sockaddr_in));
	
	/******** Forming IPv4 Address *********/
	
	srv_addr.sin_family=AF_INET;
	if(ntohs(port)<0 || ntohs(port) > 49151)
	{
		fprintf(stderr,"Bad port Number\n");
		exit(1);
	}

	srv_addr.sin_port=(port==0)?htons(DEFAULT_PORT):port; /* Port must be in byte order */

	if (addr==NULL)
		srv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	else if(!strcmp(addr,"*"))
		srv_addr.sin_addr.s_addr=htonl(INADDR_ANY);	
	else if(!inet_aton(addr,&srv_addr.sin_addr))
	{
		fprintf(stderr,"Bad address\n");
		exit(1);
	}

	len_inet=sizeof (struct sockaddr_in);
	/********* Create a Tcp socket *****************/

	server_fd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(server_fd<0)
		bail("socket()");

	/****** Bind the socket to the address *********/
	if(bind(server_fd,(struct sockaddr*)&srv_addr,len_inet)<0)
		bail("bind()");
	
	/****** Make it a listening socket *************/
	if(listen(server_fd,backlog)<0)
		bail("listen()");


	return server_fd;
}



Client* CheckLogin(char* nickName,char *password)
{
	
	char buff[250];
	sprintf(buff,"SELECT * from users where nickName=\"%s\"&& password=\"%s\"",nickName,password);
	if(mysql_query(connection,buff))
	{
		fprintf(stderr,"Mysq Error while verifying login \n");
		loginStatus=true;
		return NULL;
	}
		MYSQL_RES *res=mysql_store_result(connection);
		if(mysql_num_rows(res)==1)
		{			
			MYSQL_ROW row=mysql_fetch_row(res);
			Client* tmp=addClient(atoi(row[0]),row[1],row[2],row[3],row[4],row[5],row[6],atoi(row[7]));
			if(tmp==NULL)
			{
				loginStatus=true;
				return NULL;
			}
		return tmp;
		}
		mysql_free_result(res);
return NULL ;
}

Holder* findClientbyHolder(int sock)
{
	Holder* Cursor=holder;
	while(Cursor!=NULL)
	{
		if(Cursor->sock==sock)
			return Cursor;
		Cursor=Cursor->next;
	}
	return NULL;
}


bool findClient(char* nickName)
{
	Holder* Cursor=holder;
	while(Cursor!=NULL)
	{
		
		if(!strcmp(Cursor->client->nickName,nickName))
			return true;
	
		Cursor=Cursor->next;
	}
	return false;
}

bool addHolder(int sock,char* h_name,char* ip)
{
	Holder *new=malloc(sizeof(Holder));
	
	if(new==NULL)
	{
		fprintf(stderr,"There is no enough memory for new Holder\n");
		loginStatus=true;
		return false;
	}
	new->sock=sock;
	new->h_name=malloc(20*sizeof(char));
	new->ip=malloc(20*sizeof(char));
	new->client=NULL;
	
	strcpy(new->h_name,h_name);
	strcpy(new->ip,ip);

	if(holder==NULL)
	{
		new->next=NULL;
		holder=new;
	}
	else
	{
		new->next=holder;
		holder=new;
	}
	return true;
}


bool setClient(int sock,Client* client)
{
	Holder *Cursor=holder;
	while (Cursor!=NULL)
	{
		if(Cursor->sock == sock)
		{
			Cursor->client=client;
			return true;
		}
		Cursor=Cursor->next;
	}
	return false;
}

bool removeHolder(int sock)
{
	Holder *Cursor=holder;
	if(Cursor->sock==sock)
	{
		Holder *tmp=Cursor;
		holder=holder->next;
		if(tmp->client==NULL);
		else if(!removeClient(tmp->client->id))
			return false;
		
		free(tmp->h_name);
		free(tmp->ip);
		free(tmp);
		return true;
	}
	else
		while(Cursor!=NULL)
		{
			if(Cursor->next->sock==sock);
			{	
				Holder *tmp=Cursor->next;
				
				if(tmp->client!=NULL);
				else if(!removeClient(tmp->client->id))
					return false;
				
				if(tmp->next==NULL)
					Cursor->next=NULL;
				else
					Cursor->next=tmp->next;
				free(tmp->h_name);
				free(tmp->ip);
				free(tmp);
				return true;
			}	
			Cursor=Cursor->next;
		}
	return false;
}



bool initDatabaseConnection(void)
{
	printf("Initiate mysql connection ..\n");
	connection=mysql_init(NULL);
	if( connection==NULL)
		{
			fprintf(stderr,"Failed to initiate connection : %s\n",mysql_error(connection));
			mysqlServerStatus=false;
			return false;
		}
	fprintf(stdout,"Connection has been initiated successfuly\nEstablishing connection to Database ..\n");
	mysqlServerStatus=true;
	
	if(mysql_real_connect(connection,DB_HOST,DB_USER,DB_PASSWORD,DB_NAME,0,NULL,0)==NULL)
	{
		fprintf(stderr,"Failed to establish connection to Database : %s\n",mysql_error(connection));
		mysqlServerStatus=false;
		freeDatabaseConnection();
	}
	return mysqlServerStatus;
}


void broadCast(Msg* ms,int sock)
{
	Holder* Cursor=holder;
	while (Cursor!=NULL)
	{
		if(Cursor->client!=NULL)
			if(Cursor->sock!=sock)
				send(Cursor->sock,ms,sizeof(Msg),0);
		Cursor=Cursor->next;
	}
}

void freeDatabaseConnection(void)
{
	mysql_close(connection);
}

bool getServerStatus()
{
	return mysqlServerStatus;
}
bool getloginStatus()
{
	return loginStatus;
}

void processLogin(int sock,Msg* ms)
{
	Client* cl=CheckLogin(ms->user,ms->msg);
	char buff[MSG_SIZE];
	char user[20];
	if (cl==NULL)
	{
		ms->type=LOGIN_ERROR;
		strcpy(ms->msg,"Username or password incorrect !\n");
		send(sock,ms,sizeof(Msg),0);
		return;
	}
	else
	{
		if(!setClient(sock,cl))
		{
			ms->type=LOGIN_ERROR;
			strcpy(ms->msg,"Internal error try again later\n");
			send(sock,ms,sizeof(Msg),0);
			return;
		}
		ms->type=LOGIN;
		strcpy(ms->msg,"OK!");
		send(sock,ms,sizeof(Msg),0);


		ms->type=NONE;
		strcpy(user,ms->user);
		strcpy(ms->user,"[SERVER MESSAGE]");
		sprintf(buff,"\033[0;31m[!]\033[0m \033[0;36m%s\033[0m has joined the chat\n",user);
		strcpy(ms->msg,buff);
		broadCast(ms,sock);
	}
}

bool processLogout(int sock,Msg* ms)
{
	char buff[50];
	char user[20];
	if(!removeHolder(sock))
	{
		ms->type=LOGOUT_ERROR;
		strcpy(ms->msg,"Internal error try again later\n");
		send(sock,ms,sizeof(Msg),0);
		return false;
	}
	else
	{	
		ms->type=LOGOUT;
		strcpy(ms->msg,"OK!");
		send(sock,ms,sizeof(Msg),0);
		

		ms->type=NONE;
		strcpy(user,ms->user);
		strcpy(ms->user,"[SERVER MESSAGE]");	
		//sprintf(buff,"\033[0;31m[!]\033[0m \033[0;36m%s\033[0m has leave the chat\n",user);
		strcpy(ms->msg,buff);
		broadCast(ms,sock);
		close(sock);
		return true;
	}	
}

