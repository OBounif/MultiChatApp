#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H


typedef enum
{
	NONE=50,
	HELP_N=51,
	MP_N=52,
	WHO_N=53,
	LOGIN_N=55,
	REGISTER_N=56,
	BAN_N=57,
	LOGOUT_N=58,

	NONE_ERROR=104,

	TRY_AGAIN=105,
	NOT_AVAILABLE=106,
	UNKNOWN_C=107

}Msg_type;



typedef struct
{
	Msg_type type;
	char* from_user;
	char* to_user;
	char* data;
}Msg;


Msg* __ParsePACKET(PACKET*);
void __FreeMsg(Msg**);

#endif
