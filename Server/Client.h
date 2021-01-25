#ifndef CLIENT_H
#define CLIENT_H


#define BUFFER_LENGTH 300

#define NICKNAME 20
#define PASSWORD 20
#define EMAIL	 20 
#define LAST_IP	 20
#define RANK	 20


typedef struct Client 
{
	unsigned id;
	char *userName;
	char *password;
	char *lastIp;
	char *rank;
	bool isBanned;

}Client;


Client* __AddClient(char*,char*,char*,char*);
Client* __GetClient(char const*);

void __FreeClient(Client**);

bool __ModClient(unsigned,char const*,char const*);
bool __RemoveClient(unsigned);

#endif

