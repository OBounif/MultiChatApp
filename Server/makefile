## Specials targets
.PHONY: clean

##Avoid implicites target
.SUFFIXES:

##MACR DEFINITIONS

CC= gcc
DEBUG=false
RM=rm -rf


OBJS= Client.o\
    Server.o\
    mainSrv.o


ifeq ($(DEBUG),true)
	CFLAGS= -g -Wall -v `mysql_config --cflags`
else
	CFLAGS=`mysql_config --cflags`
endif

all:$(OBJS)

ifeq ($(DEBUG),true)
	@echo "Génération en mode debug"
else
	@echo "Génération en mode Release"
endif


%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) *.o 

