#include "messagingchannel.h"

void init_messagingchannel (char* GSport) {

	int fd = open_tcp_socket(GSport);

	char buffer[128];
	socklen_t addrlen;
	int n = -1, len, newfd;
	struct sockaddr_in addr;

	while (1) {

		addrlen = sizeof(addr);
		if ( ( newfd = accept(fd, (struct sockaddr*)&addr, &addrlen) ) == -1 ) {
			continue;
		}
		n = read(newfd, buffer, 128);
		if ( n == -1 ) {
			close(newfd);
			continue;
		}

		len = process_tcp_message(buffer);

		n = write(newfd, buffer, n);
		if ( n == -1 ) {
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

	if ( listen(fd, 1) == -1 ) {
		exit(1);
	}

	freeaddrinfo(res);
    
	return fd;
}

int process_tcp_message (char* buffer) {

	int len = 0;

	return len;
}
