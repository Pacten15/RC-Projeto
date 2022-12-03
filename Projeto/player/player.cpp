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
using namespace std;

string received;


/*TODO
Esta implementação está errada:
-- Os argumentos -n e -p são opcionais (o enunciado até comenta o que fazer em caso de omissão)
-- Os argumentos -n e -p não têm de aparecer nesta ordem específica, e pode aparecer só um de ambos
 Com esta implementação no caso de não passarem argumentos, ou passarem de outra forma sem
ser a única que foi prevista nesta função, que sao possibilidade válidas,
a função vai devolver um vector<string> que nem sequer está iniciado, e se o resto do código
depender desta informação, vai falhar.

Isto já não é um erro, mas tenho ideia de que é boa práctica e mais confortável usar
libraries que já façam o que pretendes. Getopt trata dos argumentos da linha de comando,
se calhar dá-te jeito usar, mas tb podes criar a tua própria função se preferires.
*/
vector<string> get_data_command(char** command)
{
    vector<string> v;
    if(strcmp(string(command[1]).c_str(),"-n")==0 && strcmp(string(command[3]).c_str(),"-p")==0)
    {
        string ip = string(command[2]);
        string port = string(command[4]);
        v.push_back(ip);
        v.push_back(port); 
    }
    return v;
}

/* Não tenho a certeza, mas acho que cout << v1 << endl; faz o mesmo que esta função,
caso seja verdade esta função é redundante.
 */
void printVector(vector<string> v1)
{
    for(uint i=0;i<v1.size();i++)
    {
        cout << v1[i] << " ";

    }
    cout << '\n';
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
    vector<string> v = get_data_command(argv);
    string ip = v[0];
    string port = v[1];
    string player_command;
    //string returned; //TODO: variável não usada
    cin >> player_command;
    if(strcmp(player_command.c_str(),"start")==0){
        string message;
        cin >> message;
        string message_to_send;
        message_to_send.append("SNG ");
        message_to_send.append(message);
        message_to_send.append("\n");
        send_to_udp_server(message_to_send,port,ip);
    }


    return 0;
}


