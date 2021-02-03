#Specials target
.PHONY:clean, $(SERVER_C), $(CLIENT_C)
.SUFFIXES:

#MACR DEFINITIONS

CC= gcc
LDFLAGS= `mysql_config --cflags --libs`
DEBUG=false
RM= rm -rf

CLIENT=CLIENT
SERVER=SERVER

CLIENT_C=CLIENT_c
SERVER_C=SERVER_C

OBJS_SERVER=Server/*.o
OBJS_CLIENT=Client/*.o

ifeq ($(DEBUG),true)
	CFLAGS= -g -Wall -v
else
	CFLAGS=
endif
	

all : $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_C) 

ifeq ($(DEBUG),true)
	@echo "Génération Server en mode debug"
else
	@echo "Génération Server en mode Release"
endif
	$(CC) $(OBJS_SERVER) -o $@ $(LDFLAGS) 



$(CLIENT): $(CLIENT_C)

ifeq ($(DEBUG),true)
	@echo "Génération Client en mode debug"
else
	@echo "Génération Client en mode Release"
endif
	$(CC) $(OBJS_CLIENT) -o $@ 

$(SERVER_C):
	make -C Server all
$(CLIENT_C):
	make -C Client all

clean:
	make -C Server clean
	make -C Client clean 
	$(RM) -rf *.o $(SERVER) $(CLIENT)
