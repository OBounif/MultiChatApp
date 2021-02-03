#ifndef HOLDER_H
#define HOLDER_H

#define H_NAMESIZE 100
#define IP_SIZE 20


typedef struct Holder
{
	int sock;
	char* h_name;
	char* ip;
	Client* client;
	struct Holder* next;
}Holder;



Holder* __GetHolderBySock(int);
Holder* __GetHolderByCName(char*);
Holder* __GetHolder(void);

bool __AddHolder(int,char*,char*ip);
bool __HSetClient(int,Client*);
bool __RemoveHolder(int);

void __FreeAllHolders(void);
#endif
