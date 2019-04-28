
#ifndef __USER_FUNCTION_H_
#define __USER_FUNCTION_H_


#include <hsf.h>


void user_send( int udp_flag, char *s );
void USER_FUNC user_function_cmd_received(int udp_flag,  char *pusrdata);
unsigned char strtohex(char a, char b);



#endif
