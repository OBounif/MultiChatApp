.PHONY:
.SUFFIXES:

#MACR DEFINITIONS

CC= gcc
LDFLAGS= `mysql_config --cflags --libs`
DEBUG=true
RM= rm -rf

CLIENT=CLIENT
SERVER=SERVER

OBJS_SERVER=Server/*.o
OBJS_CLIENT=CLIENT/*.O

ifeq ($(DEBUG),true)
	CFLAGS= -g -Wall -v
else
	CFLAGS=
endif
	

all : $(SERVER)

$(SERVER): $(OBJS_SERVER)

ifeq ($(DEBUG),true)
	@echo "Génération Server en mode debug"
else
	@echo "Génération Server en mode Release"
endif
	$(CC) $(OBJS_SERVER) -o $@ $(LDFLAGS) 


$(CLIENT): $(OBJS_CLIENT)

ifeq ($(DEBUG),true)
	@echo "Génération Client en mode debug"
else
	@echo "Génération Client en mode Release"
endif
	$(CC) $(OBJS_CLIENT) -o $@ $(LDFLAGS)


##make -C Client clean

clean:
	make -C Server clean
	$(RM) -rf *.o $(SERVER)
