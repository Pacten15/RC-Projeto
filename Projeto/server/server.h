#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <ctime>
#include <fstream>
#include <dirent.h>

#include "gamechannel.h"
#include "messagingchannel.h"

#define GN 30

using namespace std;

void commandlinearguments (int, char**, char**, char**);
void init_server (char*, char*);

#endif
