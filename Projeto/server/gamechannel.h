#ifndef GAMECHANNEL_H
#define GAMECHANNEL_H

#include "server.h"

typedef struct _game_info {
	int plid = -1;
	int plays = -1;
	int letter_guesses[32];
	char word[32];
	char file[128];
	int errors = 0;
	int max_errors = 0;
	char game_filename[128];
} game_info;

void init_gamechannel (char*, char*);
int open_udp_socket (char*);
void process_udp_message (char*);
void update_game_info (game_info* ginfo, int plid);
bool check_letter(char*, char);
void archive_game (char*);
void generate_word (game_info*);

#endif
