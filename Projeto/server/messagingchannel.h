#ifndef MESSAGINGCHANNEL_H
#define MESSAGINGCHANNEL_H

#include "server.h"

typedef struct _scorelist {
	int* score;
	char** PLID;
	char** word;
	int* n_suc;
	int* n_tot;
	int n_scores;
} scorelist;

void init_messagingchannel (char*);
int open_tcp_socket (char*, struct addrinfo**);
char* process_tcp_message (char*);
int find_top_scores (scorelist*);
int find_last_game(char*, char*);

#endif
