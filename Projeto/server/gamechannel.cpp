#include "gamechannel.h"

void init_gamechannel (char* GSport, char* word_file_name) {

	int fd = open_udp_socket(GSport);

	int n = -1, len;
	char buffer[128];
	socklen_t addrlen;
	struct sockaddr_in addr;

	addrlen = sizeof(addr);
	while (1) {

		n = recvfrom(fd, buffer, 128, 0,
				(struct sockaddr*)&addr, &addrlen);
		if ( n == -1 ) {
			cout << "recvfrom error" << endl;
			exit(1);
		}

		process_udp_message(buffer);

		n = sendto(fd, buffer, 128, 0,
				(struct sockaddr*)&addr, addrlen);
		if ( n == -1 ) {
			cout << "sendto error" << endl;
		}

	}

	close(fd);

	return;
}

int open_udp_socket (char* GSport) {

	int fd, errcode;
	ssize_t n;
	struct addrinfo hints,*res;

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    hints.ai_flags=AI_PASSIVE;
    
    errcode=getaddrinfo(NULL,GSport,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);
    
    n=bind(fd,res->ai_addr, res->ai_addrlen);
    if(n==-1){
		cout << "Erro no bind. Abortando." << endl;
        exit(1);
    } /*error*/ 

	freeaddrinfo(res);

	return fd;
}

void process_udp_message (char* message) {

	int plid;
	char* aux;
	FILE* fp = NULL;
	int flag = 0;
	static game_info ginfo;

	aux = strchr(message, ' ') + 1;
	if ( strlen(aux) != 6 ) {
		plid = -1;
	} else {
		plid = stoi(aux, 0, 10);
		if ( plid >= 0 && plid <= 999999 ) {
			update_game_info(&ginfo, plid);
			plid = ginfo.plid;
		} else {
			plid = -1;
		}
	}

	if ( strncmp(message, "SNG", 3) ) {

		if ( plid < 0 ) {
			strcpy(message, "RSG NOK\n");
			return;
		}
		if ( ginfo.plays == -1 ) {
			fp = fopen(ginfo.game_filename, "w");
			if ( fp = NULL ) {
				strcpy(message, "RSG NOK\n");
				return;
			}
			generate_word(&ginfo);
			fprintf(fp, "%s %s\n", ginfo.word, ginfo.file);
			ginfo.plays = 0;
		}

		sprintf(message, "RSG OK %ld %d\n", strlen(ginfo.word)-1, ginfo.max_errors);
		return;

	} else if ( strncmp(message, "PLG", 3) ) {

		char letter;

		if ( plid < 0 ) {
			strcpy(message, "RLG ERR\n");
			return;
		}

		aux = strchr(message, ' ') + 1;
		aux = strchr(aux, ' ') + 1;
		letter = aux[0];

		//trials
		aux = strchr(aux, ' ') + 1;
		flag = stoi(aux, 0, 10);
		if ( flag != ginfo.plays ) {
			// INV
			sprintf(message, "RLG INV %d\n", ginfo.plays);
			return;
		}

		fp = fopen(ginfo.game_filename, "a");

		if ( check_letter(ginfo.word, letter) ) {
			
			flag = 0;
			for ( int i = 0; i < 32; i++ ) {
				if ( ginfo.word[i] == letter) {
					if ( ginfo.letter_guesses[i] == 1 ) {
						flag = 1;
						break;
					}
					ginfo.letter_guesses[i] = 1;
				}
			}

			if ( flag ) {
				// DUP
				sprintf(message, "RLG DUP %d\n", ginfo.plays);
				fclose(fp);
				return;
			}

			flag = 1;
			for ( int i = 0; i < strlen(ginfo.word); i++ ) {
				if ( ginfo.word[i] == 0 ) {
					flag = 0;
					break;
				}
			}
			if ( flag ) {
				// WIN
				ginfo.plays++;
				fprintf(fp, "T %c\n", letter);
				fclose(fp);
				sprintf(message, "RLG WIN %d\n", ginfo.plays);
				archive_game(ginfo.game_filename);
				return;
			} else {
				// OK
				ginfo.plays++;
				fprintf(fp, "T %c\n", letter);
				fclose(fp);
				sprintf(message, "RLG OK %d", ginfo.plays);

				flag = 0;
				for ( int i = 0; i < strlen(ginfo.word); i++ ) {
					if ( ginfo.word[i] == letter ) {
						flag++;
					}
				}
				sprintf(message, "%s %d", message, flag);
				for ( int i = 0; i < strlen(ginfo.word); i++ ) {
					if ( ginfo.word[i] == letter ) {
						sprintf(message, "%s %d", message, i);
					}
				}
				sprintf(message, "%s\n", message);

				return;
			}

		} else {

			ginfo.plays++;
			ginfo.errors++;
			if ( ginfo.errors < ginfo.max_errors ) {
				// NOK
				sprintf(message, "RLG NOK %d %d\n", ginfo.max_errors-ginfo.errors, ginfo.plays);
			} else {
				// OVR
				sprintf(message, "RLG OVR %d\n", ginfo.plays);
				archive_game(ginfo.game_filename);
			}

		}
		fclose(fp);

	} else if ( strncmp(message, "PWG", 3) ) {
		//PWG plid word trial
		//RWG status trial

		char wordguess[32];

		if ( plid < 0 ) {
			strcpy(message, "RWG ERR\n");
			return;
		}

		aux = strchr(message, ' ') + 1;
		aux = strchr(aux, ' ') + 1;
		strcpy(wordguess, aux);
		strtok(wordguess, " ");

		//trials
		aux = strchr(aux, ' ') + 1;
		flag = stoi(aux, 0, 10);
		if ( flag != ginfo.plays ) {
			// INV
			sprintf(message, "RLG INV %d\n", ginfo.plays);
			return;
		}

		fp = fopen(ginfo.game_filename, "a");

		ginfo.plays++;
		if ( strcmp(ginfo.word, wordguess) == 0 ) {
			// WIN
			sprintf(message, "RWG WIN %d\n", ginfo.plays);
			fprintf(fp, "G %s\n", ginfo.word);
			fclose(fp);
			archive_game(ginfo.game_filename);
			return;
		} else {

			ginfo.errors++;
			if ( ginfo.errors < ginfo.max_errors ) {
				// NOK
				sprintf(message, "RWG NOK %d\n", ginfo.plays);
				fprintf(fp, "G %s\n", ginfo.word);
				fclose(fp);
				return;
			} else {
				// OVR
				sprintf(message, "RWG OVR %d\n", ginfo.plays);
				fprintf(fp, "G %s\n", ginfo.word);
				fclose(fp);
				archive_game(ginfo.game_filename);
				return;
			}

		}

	} else if ( strncmp(message, "QUT", 3) ) {
		
		if ( plid < 0 ) {
			strcpy(message, "RQT ERR\n");
			return;
		}
		archive_game(ginfo.game_filename);
		strcpy(message, "RQT OK\n");
		return;

	} else if ( strncmp(message, "REV", 3) ) {
		
		if ( plid < 0 ) {
			strcpy(message, "RRV ERR\n");
			return;
		}
		sprintf(message, "RRV %s\n", ginfo.word);
		return;

	} else {
		strcpy(message, "ERR\n");
	}

	return;
}

void update_game_info (game_info* ginfo, int plid) {

	FILE* fp = NULL;
	
	ginfo->plid = plid;

	sprintf(ginfo->game_filename, "GAMES/GAME_%d", plid);
	fp = fopen(ginfo->game_filename, "r");
	if ( fp == NULL ) {
		ginfo->plays = -1;
		strcpy(ginfo->word, "\0");
		strcpy(ginfo->file, "\0");
		ginfo->errors = 0;
		ginfo->max_errors = 0;
		for ( int i = 0; i < 32; i++ ) {
			ginfo->letter_guesses[i] = 0;
		}
		ginfo->plid = -1;
		return;
	}

	char buffer[128];
	fgets(buffer, 128, fp);
	sscanf(buffer, "%s %s", ginfo->word, ginfo->file);

	if ( (strlen(ginfo->word)-1) <= 6 ) {
		ginfo->max_errors = 7;
	} else if ( (strlen(ginfo->word)-1) >= 7 && (strlen(ginfo->word)-1) <= 10 ) {
		ginfo->max_errors = 8;
	} else {
		ginfo->max_errors = 9;
	}

	int aux = 0;
	ginfo->plays = 0;
	while ( fgets(buffer, 128, fp) != NULL ) {
		ginfo->plays++;
		switch(buffer[0]) {
			case 'T':
				if ( check_letter(ginfo->word, buffer[2]) ) {
					for ( int i = 0; i < 32; i++ ) {
						if ( buffer[2] == ginfo->word[i] ) {
							ginfo->letter_guesses[i] = 1;
						}
					}
					ginfo->errors++;
				} else {
					aux = 0;
				}
				break;
			case 'G':
				if ( strcmp(ginfo->word, (buffer+2)) != 0 ) {
					ginfo->errors++;
				}
				break;
			default:
				continue;
		}
	}

	return;
}

bool check_letter (char* word, char letter) {

	if ( strchr(word, letter) != NULL ) {
		return true;
	}
	return false;
}

void archive_game (char* filename) {
	return;
}

void generate_word (game_info* ginfo) {
	return;
}
