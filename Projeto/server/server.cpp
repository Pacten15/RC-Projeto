#include <unistd.h>
#include <stdlib.h>
#include <iostream>

#define GN 3

using namespace std;

bool verbose = false;

void commandlinearguments (int argc, char** argv, char** word_file_name, char** GSport) {

	int opt;

	sprintf(*GSport, "%d", 58000+GN);

	while ( (opt = getopt(argc, argv, "p:v")) != -1 ) {
		switch (opt) {
			case 'p':
				cout << optarg << endl;
				*GSport = optarg;
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

int main (int argc, char** argv) {

	char* GSport = new char[6];
	char* word_file_name = NULL;

	commandlinearguments(argc, argv, &word_file_name, &GSport);
	cout << word_file_name <<endl;
	cout << GSport << endl;
	cout << verbose << endl;

	return 0;

}
