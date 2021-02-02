#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>


void Bzero(void *ptr,size_t size)
{
	memset(ptr,0,size);	
}

void bail(const char* str)
{
	fprintf(stderr,"%s : %s\n",str,strerror(errno));
	exit(EXIT_FAILURE);
}

void print_log(const char* str)
{
	fprintf(stdout,"%s\n",str);
}

void print_logerr(const char* str)
{
	fprintf(stderr,"%s\n",str);
}

