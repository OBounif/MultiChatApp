#ifndef SERVER_H
#define SERVER_H

#define MAX_PORTNUMBER 60000
#define MIN_PORTNUMBER 0

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



int __InitServer();


#endif
