## Specials targets
.PHONY: clean

##Avoid implicites target
.SUFFIXES:

##MACR DEFINITIONS

LDFLAGS= ##link not here
CC= gcc
DEBUG=false
RM=rm -rf


OBJS= User.o\
    Message_parser.o\
    config_parser.o\
    Database.o\
    Utility.o\
    mainSrv.o\
    Server.o\
    Holder.o

ifeq ($(DEBUG),true)
	CFLAGS= -g -Wall -v `mysql_config --cflags`
else
	CFLAGS=-w `mysql_config --cflags`
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

