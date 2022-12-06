#include<iostream>
#include<vector>
#include<stdio.h>
#include<string>
#include<stack>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <bitset>

#define GN 30

using namespace std;

string received;


int get_number_of_elements(char ** argument)
{
    vector<string> v;
    int i=0;
    while(argument[i] != NULL)
        i++;
    return i;
}

vector<string> parse_string(string s)
{
    vector<string> result;
    string delimita = " ";
    size_t pointer = 0;
    string sub_s;
    while((pointer = s.find(delimita))!= std::string::npos)
    {
        sub_s = s.substr(0,pointer);
        result.push_back(sub_s);
        s.erase(0,pointer + delimita.length());
        
    }
    s.erase(s.length()-1,1);
    result.push_back(s);
    return result;
}

string draw_board(int n)
{
    string board;
    for(int i=0;i<n;i++)
    {
        board.append("_ ");
    }
    return board;
}


void printVector(vector<string> v1)
{
    for(uint i=0;i<v1.size();i++)
    {
        cout << v1[i] << " ";

    }
    cout << '\n';
}

/* TODO
 * Continua sem contemplar a opção do -p ser chamado antes que o -n
 */
vector<string> get_data_command(int argc, char** argv)
{

	int opt;
    string ip;
	char port[6];
    vector<string> res;

	sprintf(port, "%d", 58000+GN);

	char buffer[128];
	gethostname(buffer,128);
	ip = string(buffer);

	while ( (opt = getopt(argc, argv, "p:n:")) != -1 ) {
		switch (opt) {
			case 'p':
				strncpy(port, optarg, 5);
				break;
			case 'n':
				ip = optarg;
				break;
		}
	}
	
    res.push_back(ip);
    res.push_back(port);
	return res;

}



void send_to_udp_server(string message,string port,string ip)
{
 
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128];
    const char* message_cos_char = message.data();
    char* message_char = (char*)message_cos_char;
    size_t message_lenght = message.length();
    const char* port_char = port.c_str();
    const char* ip_char = ip.c_str();
    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket
    errcode=getaddrinfo(ip_char,port_char,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);
    n=sendto(fd,message_char,message_lenght,0,res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ exit(1);
    write(1,buffer,n);
    received = buffer;

    freeaddrinfo(res);
    close(fd);
}


int main(int argc,char** argv)
{
    vector<string> id_port = get_data_command(argc, argv);
    string ip = id_port[0];
    string port = id_port[1];
    string player_command;
    string returned;
    cin >> player_command;
    if(strcmp(player_command.c_str(),"start")==0){
        string message;
        cin >> message;
        string message_to_send;
        message_to_send.append("SNG ");
        message_to_send.append(message);
        message_to_send.append("\n");
        send_to_udp_server(message_to_send,port,ip);
        vector<string> game_settings = parse_string(received);
        string board = draw_board(stoi(game_settings[2]));
        cout << "New game started (max ";
        cout << game_settings[3];
        cout << " errors): ";
        cout << board;
        cout << "\n";
    }
    return 0;
}
