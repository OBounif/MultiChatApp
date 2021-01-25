#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For byte ordering

#include "Server.h"
#include "Utility.h"
#include "config_parser.h"

int __InitServer()
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


void main()
{
	__ParseCfgFile("config");
	__InitServer();
}
