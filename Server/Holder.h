#ifndef HOLDER_H
#define HOLDER_H

typedef struct Holder
{
	int sock;
	char* h_name;
	char* ip;
	Client* client;
	struct Holder* next;
}Holder;


#endif
