#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#define NICKNAME 20
#define PASSWORD 20
#define EMAIL	 20 
#define LAST_IP	 30
#define RANK	 20


typedef struct Client 
{
	unsigned id;
	char *userName;
	char *email;
	char *password;
	char *lastIp;
	char *rank;
	struct tm lastConnection;
	bool isBanned;

}Client;


Client* addClient(unsigned,char*,char*,char*,char*,char*,char*,bool);
Client* modClient(unsigned,char*,char*,char*,char*,char*,char*,bool);
Client* getClient(unsigned)

bool removeClient(unsigned);

#endif

