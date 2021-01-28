#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H


typedef enum
{
	NONE,
	HELP_N,
	MP_N,
	WHO_N,
	LOGOUT_N,
	LOGIN_N,
	REGISTER_N,
	BAN_N,

	HELP_ERROR,
	MP_ERROR,
	WHO_ERROR,
	LOGOUT_ERROR,
	LOGIN_ERROR,
	NONE_ERROR,
	REGISTER_ERROR,
	BAN_ERROR,

	TRY_AGAIN,
	NOT_AVAILABLE,
	UNKNOWN_C

}Msg_type;



typedef struct
{
	Msg_type type;
	char* from_user;
	char* to_user;
	char* data;
}Msg;


Msg* __ParsePACKET(PACKET*);

#endif
