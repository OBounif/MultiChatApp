#ifndef SERVER_H
#define SERVER_H

#define MAX_PORTNUMBER 60000
#define MIN_PORTNUMBER 0


#define DATA_SIZE 255
#define USER_LENGTH 20 
/*
 *	
 *	BYTE ORIENTED
	
 	typedef struct
	{
		unsigned id;
		void *data;
	}PACKET;
*
*/

/*	Character oriented	*/

typedef struct
{
	char user[USER_LENGTH];
	char msg[DATA_SIZE];
}PACKET;



void __HandleMessage(int,PACKET*);
int __InitServer(void);

#endif
