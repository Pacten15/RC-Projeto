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

	init_server(GSport, word_file_name);

	/*
	char message[256][2][10];
	int max = 0;

	message[max][0][0] = 'U';
	strcpy(message[max++][1], "SNG 1\n");

	message[max][0][0] = 'U';
	strcpy(message[max++][1], "PLG 1 r 0\n");

	message[max][0][0] = 'U';
	strcpy(message[max++][1], "PLG 1 a 1\n");

	message[max][0][0] = 'U';
	strcpy(message[max++][1], "PLG 1 b 2\n");

	message[max][0][0] = 'U';
	strcpy(message[max++][1], "PLG 1 k 3\n");

	message[max][0][0] = 'U';
	strcpy(message[max++][1], "PLG 1 k 4\n");

	message[max][0][0] = 'U';
	strcpy(message[max++][1], "PLG 1 c 4\n");

	char* reply;
	for ( int i = 0; i < max; i++ ) {
		if ( message[i][0][0] == 'T' ) {

			reply = process_tcp_message(message[i][1]);
			cout << reply;

		} else {

			process_udp_message(message[i][1], word_file_name);
			cout << message[i][1];

		}
	}
	*/

	return 0;

}
