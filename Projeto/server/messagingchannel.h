#ifndef MESSAGINGCHANNEL_H
#define MESSAGINGCHANNEL_H

#include "server.h"

void init_messagingchannel (char*);
int open_tcp_socket (char*);
int process_tcp_message (char*);

#endif
