#include "gamechannel.h"

void init_gamechannel (char* GSport, char* word_file_name) {

	int fd = open_udp_socket(GSport);

	return;
}

int open_udp_socket (char* GSport) {

	int fd, errcode;
	ssize_t n;
	struct addrinfo hints,*res;

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);
    
    memset(&hints,0,sizeof hints);
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

	return fd;
}
