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
		int size = calculate_replysize(&list, &fsize);
		
		char* fdata = new char[fsize+1]();
		if ( fdata == NULL ) return NULL;

		for ( int i = 0; (i<list.n_scores) && (i<10); i++ ) {
			sprintf(fdata, "%s%03d %s %s %02d %02d\n", fdata,
					list.score[i], list.PLID[i], list.word[i],
					list.n_suc[i], list.n_tot[i]);
		}

		char fname[32];
		sprintf(fname, "SB_%04d%02d%02d_%02d%02d%02d", 
				1900+ltm.tm_year, ltm.tm_mon, ltm.tm_mday,
				ltm.tm_hour, ltm.tm_min, ltm.tm_sec);

		reply = (char*) malloc(size*sizeof(char));
		sprintf(reply, "RSB OK %s %d %s",
				fname, fsize, fdata);

		char path[40];
		sprintf(path, "SCORES/%s", fname);
		FILE* fp = fopen(path, "w");
		fprintf(fp, "%s", fdata);
		delete[] fdata;
		fclose(fp);

	} else if ( request[0] == 'G' &&
			request[1] == 'H' &&
			request[2] == 'D' ) {

	} else if ( request[0] == 'S' &&
			request[1] == 'T' &&
			request[2] == 'A' ) {

	}

	return NULL;
}

int find_top_scores (scorelist* list) {

	struct dirent** filelist;
	int n_entries, i_file;
	FILE* fp = NULL;
	char fname[300];

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

int calculate_replysize (scorelist* list, int* fsize) {
	
	int rsize = 0;
	char buffer[32];

	for ( int i = 0; (i < list->n_scores) && (i < 10); i++ ) {
		rsize += 3 + 1 + // score
			strlen(list->word[i]) + 1 + // PLID
			strlen(list->word[i]) + 1 + // word
			2 + 1 + // n_suc
			2 + 1 + // n_tot
			1; // \n
	}
	*fsize = rsize;

	sprintf(buffer, "%d", *fsize);

	fsize += 3 + 1 + // RSB
		2 + 1 + // OK
		18 + 1 + // Fname(SB_YYYYMMDD_HHMMSS)
		strlen(buffer) + 1; // Fsize

	return rsize;
}
