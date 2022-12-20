#ifndef MESSAGINGCHANNEL_H
#define MESSAGINGCHANNEL_H

#include "server.h"

typedef struct _scorelist {
	int score[10];
	char PLID[8][10];
	char word[32][10];
	int n_suc[10];
	int n_tot[10];
	int n_scores;
} scorelist;

void init_messagingchannel (char*);
int open_tcp_socket (char*);
char* process_tcp_message (char*);
int find_top_scores (scorelist*);
int calculate_replysize (scorelist*, int*);

#endif
