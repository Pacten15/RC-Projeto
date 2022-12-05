#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#define PORT "58001"
extern int errno;
int fd,errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints,*res;
struct sockaddr_in addr;
char buffer[128];

int main(void){

    fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (fd==-1){
        printf("erro ao criar o socket");
        exit(1);
    }  //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    char buffer2[128];
    gethostname(buffer2,128);
    errcode=getaddrinfo(buffer2,PORT,&hints,&res);
    if(errcode!=0){
        printf("erro a obter o endereço");
        exit(1);
    }
    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        printf("erro ao conectar ao servidor\n");
        exit(1);
    }
    
    
    n=write(fd,"Hello!\n",7);
    if(n==-1){
        printf("erro ao escrever\n");
        exit(1);
    }
    
    n=read(fd,buffer,128);
    if(n==-1){
        printf("erro ao ler\n");
        exit(1);
    }

    write(1,"echo: ",6);
    write(1,buffer,n);
    
    freeaddrinfo(res);
    close(fd);
}
