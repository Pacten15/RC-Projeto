#include "messagingchannel.h"

void init_messagingchannel (char* GSport) {

	int fd = open_tcp_socket(GSport);

	char* reply;
	char request[16];
	socklen_t addrlen;
	int n = -1, len, newfd;
	struct sockaddr_in addr;

	while (1) {

		addrlen = sizeof(addr);
		if ( ( newfd = accept(fd, (struct sockaddr*)&addr, &addrlen) ) == -1 ) {
			continue;
		}

		if ( fork() == 0 ) {

			close(fd);

			n = read(newfd, request, 16);
			if ( n == -1 ) {
				close(newfd);
				break;
			}

			reply = process_tcp_message(request);

			n = strlen(reply);
			write(newfd, reply, n);

			delete[] reply;
			close(newfd);
			break;

		} else {
			close(newfd);
			continue;
		}
	}

	return;
}

int open_tcp_socket (char* GSport) {

	struct addrinfo hints, *res;
	int fd = -1, n = -1, errcode;
	
    fd = socket(AF_INET, SOCK_STREAM, 0); //TCP socket
    if (fd==-1) exit(1); //error

    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode = getaddrinfo(NULL, GSport, &hints, &res);
    if ( (errcode) != 0 )/*error*/exit(1);

    n = bind(fd,res->ai_addr,res->ai_addrlen);
    if( n == -1 ) /*error*/ exit(1);

	if ( listen(fd, 5) == -1 ) {
		exit(1);
	}

	freeaddrinfo(res);
    
	return fd;
}

char* process_tcp_message (char* request) {

	char* reply;
	char buffer[512];

	time_t now = time(0);
	struct tm ltm;
	localtime_r(&now, &ltm);

	if ( request[0] == 'G' &&
			request[1] == 'S' && 
			request[2] == 'B' ) {

		scorelist list;
		if ( find_top_scores(&list) == 0 ) {
			reply = (char*) malloc(16*sizeof(char));
			strcpy(reply, "RSB EMPTY");
			return NULL;
		}

		int fsize = 0;
		char* fdata = new char[512]();
		if ( fdata == NULL ) return NULL;

		for ( int i = 0; (i<list.n_scores) && (i<10); i++ ) {
			sprintf(buffer, "%s%03d %s %s %02d %02d\n", fdata,
					list.score[i], list.PLID[i], list.word[i],
					list.n_suc[i], list.n_tot[i]);
			strncpy(fdata, buffer, 512);
		}
		fsize = strlen(fdata);

		char fname[32];
		sprintf(fname, "SB_%04d%02d%02d_%02d%02d%02d", 
				1900+ltm.tm_year, ltm.tm_mon, ltm.tm_mday,
				ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
		sprintf(buffer, "RSB OK %s %d ", fname, fsize);
		int size = strlen(buffer) + fsize;

		reply = (char*) malloc(size*sizeof(char));
		sprintf(reply, "RSB OK %s %d %s",
				fname, fsize, fdata);

		FILE* fp = fopen(fname, "w");
		fprintf(fp, "%s", fdata);
		delete[] fdata;
		fclose(fp);

	} else if ( request[0] == 'G' &&
			request[1] == 'H' &&
			request[2] == 'L' ) {

		char* aux = strchr(request, ' ') + 1; 
		int plid = stoi(aux, 0, 10);
		if ( plid < 0 && plid > 999999 ) {
			reply = new char[8];
			strcpy(reply, "ERR\n");
			return reply;
		}

		game_info ginfo;
		update_game_info(&ginfo, plid);

		if ( ginfo.plays == -1 ) {
			reply = new char[10];
			strcpy(reply, "RHL NOK\n");
			return reply;
		}

		char* fname = ginfo.file;

		FILE* fp = fopen(fname, "r");
		fseek(fp, 0L, SEEK_END);
		int fsize = ftell(fp);
		fclose(fp);

		char* fdata = (char*) malloc(fsize*sizeof(char));
		if ( fdata == NULL ) {
			reply = new char[8];
			strcpy(reply, "ERR\n");
			return reply;
		}

		int fd = open(fname, O_RDONLY);
		int n = read(fd, fdata, fsize);
		close(fd);

		aux = strrchr(fname, '/') + 1;
		if ( (aux-1) == NULL ) aux = fname;

		sprintf(buffer, "RHL FIN %s %d ", aux, fsize);
		int len = strlen(buffer) + fsize + 2;
		reply = (char*) malloc(len*sizeof(char));
		sprintf(reply, "%s%s\n", buffer, fdata);

		free(fdata);
		return reply;

	} else if ( request[0] == 'S' &&
			request[1] == 'T' &&
			request[2] == 'A' ) {

		char* aux = strchr(request, ' ') + 1; 
		int plid = stoi(aux, 0, 10);
		if ( plid < 0 && plid > 999999 ) {
			reply = new char[8];
			strcpy(reply, "ERR\n");
			return reply;
		}

		bool fin = false;
		char fname[64];
		FILE* fp = NULL;
		sprintf(fname, "GAMES/GAME_%06d", plid);
		if ( ( fp = fopen(fname, "r") ) == NULL ) {
			fin = true;
			if ( !find_last_game(aux, fname) ) {
				// NOK
				reply = new char[10];
				strcpy(reply, "RST NOK\n");
				return reply;
			}
		}

		// FIN & ACT
		fp = fopen(fname, "r");
		fseek(fp, 0L, SEEK_END);
		int fsize = ftell(fp);
		fclose(fp);

		char* fdata = (char*) malloc((fsize+1)*sizeof(char));
		if ( fdata == NULL ) {
			reply = new char[8];
			strcpy(reply, "ERR\n");
			return reply;
		}

		int fd = open(fname, O_RDONLY);
		int n = read(fd, fdata, fsize);
		close(fd);
		fdata[n] = '\0';

		aux = strrchr(fname, '/') + 1;
		if ( (aux-1) == NULL ) aux = fname;

		if ( fin ) {
			sprintf(buffer, "RST FIN %s %d ", aux, fsize);
		} else {
			sprintf(buffer, "RST ACT %s %d ", aux, fsize);
		}
		int len = strlen(buffer) + fsize + 2;
		reply = (char*) malloc(len*sizeof(char));
		sprintf(reply, "%s%s\n", buffer, fdata);

		free(fdata);
		return reply;

	} else {
		reply = new char[8];
		strcpy(reply, "ERR\n");
		return reply;
	}

	return NULL;
}

int find_top_scores (scorelist* list) {

	struct dirent** filelist;
	int n_entries, i_file;
	FILE* fp = NULL;
	char fname[300];

	list->score = new int[10];
	list->n_suc = new int[10];
	list->n_tot = new int[10];
	list->PLID = new char*[10];
	list->word = new char*[10];
	for ( int i = 0; i < 10; i++ ) {
		list->PLID[i] = new char[16];
		list->word[i] = new char[32];
	}

	n_entries = scandir("SCORES/", &filelist, 0, alphasort);

	i_file = 0;
	if ( n_entries < 0 ) {
		return 0;
	} else {
		while ( n_entries-- ) {
			if ( filelist[n_entries]->d_name[0] != '.' ) {
				sprintf(fname, "SCORES/%s", filelist[n_entries]->d_name);
				fp = fopen(fname, "r");
				if ( fp != NULL ) {
					fscanf(fp, "%d %s %s %d %d", 
							&list->score[i_file], list->PLID[i_file],
							list->word[i_file], &list->n_suc[i_file],
							&list->n_tot[i_file]);
					fclose(fp);
					i_file++;
				}
			}

			free(filelist[n_entries]);
			if ( i_file == 10 ) {
				break;
			}

		}

		free(filelist);

	}

	list->n_scores = i_file;
	return i_file;

}

int find_last_game (char* plid, char* fname) {

	struct dirent** filelist;
	int n_entries, found;
	char dirname[20];

	int plid_int = stoi(plid, 0, 10);
	sprintf(dirname,"GAMES/%06d/", plid_int);
	n_entries = scandir(dirname, &filelist, 0, alphasort);

	found = 0;
	if ( n_entries <= 0 ) {
		return 0;
	} else {
		while ( n_entries-- ) {
			if ( filelist[n_entries]->d_name[0] != '.' ) {
				sprintf(fname, "GAMES/%06d/%s", plid_int, filelist[n_entries]->d_name);
				found = 1;
			}
			free(filelist[n_entries]);
			if ( found ) {
				break;
			}
		}
		free (filelist);
		return found;
	}

}
