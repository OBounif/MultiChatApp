#ifndef SERVER_H
#define SERVER_H

#include <stddef.h> //for the size_t defintion
#include <stdbool.h>
#include "Client.h"

#define DEFAULT_PORT 2500

#define DB_NAME "ChatRoom" //DATABASE NAME
#define DB_PASSWORD "A!a123456789" // DATABASE PASSWORD
#define DB_USER "root" // DATABASE USERNAME
#define DB_HOST "localhost" //DATABASE HOST

#define MSG_SIZE 500
#define PACKET_SIZE 700
typedef enum
{
	NONE,HELP,MP,WHO,LOGOUT,LOGIN,HELP_ERROR,MP_ERROR,WHO_ERROR,LOGOUT_ERROR,LOGIN_ERROR,NONE_ERROR,UNKOWN
}Msg_type;

typedef struct
{
	Msg_type type;
	char user[20];	
	char msg[MSG_SIZE];
}Msg;

typedef struct Holder
{
int sock;
char* h_name;
char* ip;
Client* client;
struct Holder* next;
}Holder;

/* 
- ZeroMemory():
  ARGUMENTS:	
	void*: Memory that u want zeroing.
	size_t : the size of the memory.


- bail():
  ARGUMENTS:
  	const char* : String describing the function or just the name of the function.


 -init_server():
  ARGUMENTS:
	addr : pointer to an IPv4 address.
	port:  Port number should be in network byte order (htons(number)).Remember that port number less than 1024 are reserved for privileged programs (running as root).
	backlog : The lenght of the queu which gonna hold connections.

	If addr==NULL then the server will use the loopback interface.
	If port==0 then the server will use the default port number .

	Return value : 
		the function will return the file unit of the socket 	
*/

void ZeroMemory(void*,size_t);
int init_Server(char*,int,int);
void bail(const char*);

Client*  CheckLogin(char*,char*);
Client*  signUp(char*,char*);

bool deleteClient(int);
char* help_list();
char* adminCommands();
char* whoIsHere();

bool findClient(char*);
Holder* findClientbyHolder(int sock); /**/
bool setClient(int,Client*);

bool getServerStatus(void);
bool initDatabaseConnection(void);
void freeDatabaseConnection(void);

bool addHolder(int,char*,char*);
bool removeHolder(int);

void broadCast(Msg*,int);

void processSignUp(int);/**/
void processLogin(int,Msg*);
bool processLogout(int,Msg*);/**/
void processMsg(int);/**/

#endif
