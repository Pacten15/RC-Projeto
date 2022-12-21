#include "server.h"

bool verbose = false;

void commandlinearguments (int argc, char** argv, char** word_file_name, char** GSport) {

	int opt;

	sprintf(*GSport, "%d", 58000+GN);

	while ( (opt = getopt(argc, argv, "p:v")) != -1 ) {
		switch (opt) {
			case 'p':
				cout << optarg << endl;
				sprintf(*GSport, "%s\n", optarg);
				break;
			case 'v':
				verbose = true;
				break;
		}
	}
	if ( argv[optind] != NULL ) {
		*word_file_name = argv[optind];
	} else {
		cout << "Word file not specified. Aborting." << endl;
		exit(1);
	}

	return;

}

void init_server (char* GSport, char* word_file_name) {

	if ( fork() == 0 ) {
		// UDP
		init_gamechannel(GSport, word_file_name);
	} else {
		//TCP
		init_messagingchannel(GSport);
	}

	return;
}

int main (int argc, char** argv) {

	char* GSport = new char[8];
	char* word_file_name = NULL;

	commandlinearguments(argc, argv, &word_file_name, &GSport);

	//init_server(GSport, word_file_name);

	char message[32];

	char* reply;

	strcpy(message, "SNG 1");
	process_udp_message(message, word_file_name);
	cout << message;

	strcpy(message, "GHD 1");
	reply = process_tcp_message(message);
	cout << reply;

	char* aux = strchr(reply, '0') + 2;

	FILE* fp = fopen("image.jpg", "w");
	fclose(fp);

	int fd = open("image.jpg", O_WRONLY);
	write(fd, aux, 55440);
	close(fd);

	strcpy(message, "PWG 1 pig 0");
	process_udp_message(message, word_file_name);
	cout << message;

	strcpy(message, "STA 1");
	reply = process_tcp_message(message);
	cout << reply;

	return 0;

}
