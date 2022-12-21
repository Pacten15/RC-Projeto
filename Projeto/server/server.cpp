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

	char message[256][2];
	int i = 0;

	char* reply;
	while ( i >= 0 ) {
		switch(i) {
			case 0:
				message[0][0] = 'U';
				strcpy(message[1], "SNG 1\n");
				break;
			case 1:
				message[0][0] = 'U';
				strcpy(message[1], "PLG 1 c 0\n");
				break;
			case 2:
				message[0][0] = 'U';
				strcpy(message[1], "PLG 1 a 1\n");
				break;
			case 3:
				message[0][0] = 'U';
				strcpy(message[1], "PLG 1 m 2\n");
				break;
			case 4:
				message[0][0] = 'U';
				strcpy(message[1], "PLG 1 r 3\n");
				break;
			case 5:
				message[0][0] = 'U';
				strcpy(message[1], "PLG 1 r 4\n");
				break;
			case 6:
				message[0][0] = 'U';
				strcpy(message[1], "PLG 1 b 4\n");
				break;
			case 7:
				message[0][0] = 'T';
				strcpy(message[1], "GSB\n");
				break;
			case 8:
				message[0][0] = 'U';
				strcpy(message[1], "PWG 1 abracadabra 5\n");
				break;
			case 9:
				message[0][0] = 'T';
				strcpy(message[1], "GSB\n");
				break;
			default:
				i = -1;
				break;
		}

		if ( i == -1 ) break;
		i++;

		if ( message[0][0] == 'T' ) {

			cout << message[1];
			reply = process_tcp_message(message[1]);
			cout << reply << endl;
			delete[] reply;

		} else {

			cout << message[1];
			process_udp_message(message[1], word_file_name);
			cout << message[1] << endl;

		}
	}

	return 0;

}
