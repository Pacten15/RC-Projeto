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

		process_udp_message(buffer, word_file_name);

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
	if(fd==-1) {
		cout << "Erro no socket. Abortando." << endl;
		exit(-1);
	}
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    hints.ai_flags=AI_PASSIVE;
    
    errcode=getaddrinfo(NULL,GSport,&hints,&res);
	if(errcode!=0) {
		cout << "Erro no getaddrinfo(" << errcode << "). Abortando." << endl;
		cout << gai_strerror(errcode) << endl;
		exit(-1);
	}
    
    n=bind(fd,res->ai_addr, res->ai_addrlen);
    if(n==-1){
		cout << "Erro no bind. Abortando." << endl;
        exit(1);
    }

	freeaddrinfo(res);

	return fd;
}

void process_udp_message (char* message, char* word_file_name) {

	int plid;
	char* aux;
	FILE* fp = NULL;
	int flag = 0;
	static game_info ginfo;

	aux = strchr(message, ' ') + 1;
	plid = stoi(aux, 0, 10);
	if ( plid < 0 && plid > 999999 ) {
		plid = -1;
	} else {
		update_game_info(&ginfo, plid);
		plid = ginfo.plid;
	}

	if ( message[0] == 'S' &&
			message[1] == 'N' && 
			message[2] == 'G' ) {

		if ( plid < 0 ) {
			strcpy(message, "RSG NOK\n");
			return;
		}
		if ( ginfo.plays == -1 ) {
			fp = fopen(ginfo.game_filename, "w");
			if ( fp == NULL ) {
				strcpy(message, "RSG NOK\n");
				return;
			}
			generate_word(&ginfo, word_file_name);
			if ( strcmp(ginfo.word, "\0") == 0 ) {
				strcpy(message, "RSG NOK\n");
				fclose(fp);
				return;
			}

			char buffer[128], buffer2[256];
			strcpy(buffer, word_file_name);
			aux = strrchr(buffer, '/');
			if ( aux == NULL ) {
				strcpy(buffer, "\0");
			} else {
				aux++;
				*aux = '\0';
			}
			sprintf(buffer2, "%s%s", buffer, ginfo.file);
			strncpy(ginfo.file, buffer2, 128);
			
			fprintf(fp, "%s %s\n", ginfo.word, ginfo.file);

			ginfo.plays = 0;
			fclose(fp);
		} else if ( ginfo.plays > 0 ) {
			strcpy(message, "RSG NOK\n");
			return;
		}

		sprintf(message, "RSG OK %ld %d\n", strlen(ginfo.word), ginfo.max_errors);
		return;

	} else if ( message[0] == 'P' &&
			message[1] == 'L' &&
			message[2] == 'G' ) {

		char letter;

		if ( plid < 0 || ginfo.plays < 0 ) {
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
				if ( ginfo.letter_guesses[i] == 0 ) {
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
				archive_game(&ginfo, 'W');
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

			fprintf(fp, "T %c\n", letter);

			if ( ginfo.errors < ginfo.max_errors ) {
				// NOK
				sprintf(message, "RLG NOK %d %d\n", ginfo.max_errors-ginfo.errors, ginfo.plays);
			} else {
				// OVR
				sprintf(message, "RLG OVR %d\n", ginfo.plays);
				archive_game(&ginfo, 'F');
			}

		}
		fclose(fp);

	} else if ( message[0] == 'P' &&
			message[1] == 'W' &&
			message[2] == 'G' ) {
		//PWG plid word trial
		//RWG status trial

		char wordguess[32];

		if ( plid < 0 || ginfo.plays < 0 ) {
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
			sprintf(message, "RWG INV %d\n", ginfo.plays);
			return;
		}

		fp = fopen(ginfo.game_filename, "a");

		ginfo.plays++;
		if ( strcmp(ginfo.word, wordguess) == 0 ) {
			// WIN
			sprintf(message, "RWG WIN %d\n", ginfo.plays);
			fprintf(fp, "G %s\n", ginfo.word);
			fclose(fp);
			archive_game(&ginfo, 'W');
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
				archive_game(&ginfo, 'F');
				return;
			}

		}

	} else if ( message[0] == 'Q' &&
			message[1] == 'U' &&
			message[2] == 'T' ) {
		
		if ( plid < 0 || ginfo.plays < 0 ) {
			strcpy(message, "RQT ERR\n");
			return;
		}
		strcpy(message, "RQT OK\n");
		archive_game(&ginfo, 'Q');
		return;

	} else if ( message[0] == 'R' &&
			message[1] == 'E' &&
			message[2] == 'V' ) {
		
		if ( plid < 0 || ginfo.plays < 0 ) {
			strcpy(message, "RRV ERR\n");
			return;
		}
		sprintf(message, "RRV %s\n", ginfo.word);
		archive_game(&ginfo, 'Q');
		return;

	} else {
		strcpy(message, "ERR\n");
	}

	return;
}

void update_game_info (game_info* ginfo, int plid) {

	FILE* fp = NULL;
	
	ginfo->plid = plid;

	sprintf(ginfo->game_filename, "GAMES/GAME_%06d", plid);
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

	ginfo->plays = 0;
	ginfo->errors = 0;
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
				} else {
					ginfo->errors++;
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

void archive_game (game_info* ginfo, const char code) {

	char buffer[128];
	sprintf(buffer, "GAMES/%06d", ginfo->plid);
	struct stat st = {0};
	if ( stat(buffer, &st) == -1 ) {
		mkdir(buffer, 0700);
	}

	time_t now = time(0);
	tm ltm;
	localtime_r(&now, &ltm);

	sprintf(buffer, "GAMES/%06d/%04d%02d%02d_%02d%02d%02d_%c.txt",
			ginfo->plid,
			1900+ltm.tm_year, ltm.tm_mon, ltm.tm_mday,
			ltm.tm_hour, ltm.tm_min, ltm.tm_sec,
			code);

	FILE* fp = fopen(ginfo->game_filename, "r");
	fseek(fp, 0L, SEEK_END);
	int fsize = ftell(fp);
	fclose(fp);

	char* fdata = (char*) malloc(fsize*sizeof(char));

	int fd = open(ginfo->game_filename, O_RDONLY);
	read(fd, fdata, fsize);
	close(fd);

	fp = fopen(buffer, "w");
	if ( fp != NULL ) fclose(fp);

	fd = open(buffer, O_WRONLY);
	write(fd, fdata, fsize);
	close(fd);

	remove(ginfo->game_filename);

	if ( code == 'W' ) {

		int score;
		if ( ginfo->plays != 0 ) {
			score = ( (ginfo->plays - ginfo->errors) * 100 ) / ginfo->plays;
		} else {
			score = 0;
		}

		sprintf(buffer, "SCORES/%03d_%06d_%04d%02d%02d_%02d%02d%02d.txt",
				score, ginfo->plid,
				1900+ltm.tm_year, ltm.tm_mon, ltm.tm_mday,
				ltm.tm_hour, ltm.tm_min, ltm.tm_sec);

		FILE* fp = fopen (buffer, "w");
		if ( fp == NULL ) {
			//TODO;
		}

		fprintf(fp, "%03d %06d %s %d %d", 
				score, ginfo->plid, ginfo->word,
				ginfo->plays-ginfo->errors, ginfo->plays);
		fclose(fp);

	}

	return;
}

void generate_word (game_info* ginfo, char* word_file_name) {

	char* buffer;
	FILE* fp = NULL;
	static int n = 0;
	static int next_word = 0;
	static char** word_file = NULL;

	if ( word_file == NULL ) {

		buffer = new char[64];

		fp = fopen(word_file_name, "r+");
		if ( fp == NULL ) {
			strcpy(ginfo->word, "\0");
		}

		while ( fgets(buffer, 64, fp) != NULL ) {
			n++;
		}

		if ( n > 0 ) {
			word_file = new char*[n];
			for ( int i = 0; i < n; i++ ) {
				word_file[i] = new char[64];
			}
		} else {
			strcpy(ginfo->word, "\0");
			strcpy(ginfo->file, "\0");
			delete[] buffer;
			fclose(fp);
			return;
		}

		rewind(fp);
		for ( int i = 0; i < n; i++ ) {
			fgets(buffer, 64, fp);
			strcpy(word_file[i], buffer);
		}

		delete[] buffer;
		fclose(fp);

	}

	if ( next_word >= n ) {
		next_word = 0;
	}
	buffer = word_file[next_word];
	char* word = strsep(&buffer, " ");
	strcpy(ginfo->word, word);
	char* file = strsep(&buffer, "\n");
	strcpy(ginfo->file, file);
	file[strlen(file)] = '\n';
	*(file-1) = ' ';
	word_file[next_word++] = word;

	/* Random word selection

	while ( fgets(buffer, 64, fp) != NULL ) {
		n++;
	}

	time_t t;
	srand(time(&t));
	n = rand() % n + 1;

	rewind(fp);
	for ( int i = 0; i < n; i++ ) {
		fgets(buffer, 64, fp);
	}

	char* word = strsep(&buffer, " ");
	strcpy(ginfo->word, word);
	char* file = strsep(&buffer, "\n");
	strcpy(ginfo->file, file);
	
	delete[] word;

	*/

	if ( (strlen(ginfo->word)-1) <= 6 ) {
		ginfo->max_errors = 7;
	} else if ( (strlen(ginfo->word)-1) >= 7 && (strlen(ginfo->word)-1) <= 10 ) {
		ginfo->max_errors = 8;
	} else {
		ginfo->max_errors = 9;
	}

	return;
}
