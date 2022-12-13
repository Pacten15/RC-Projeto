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
#include<algorithm>
using namespace std;

string received_udp;



//* String and vector of strings manipulation and screening of the structures *//


void print_string(string s)
{
    printf("%s\n", s.c_str());
}

string format_result(string result)
{
    string f_result;
    for(int i=0;i<result.size();i++)
    {
      char letter = result[i];
      f_result.push_back(toupper(letter));
      f_result.push_back(' ');
    }
    return f_result;
}

void printVector(vector<string> v1)
{
    for(uint i=0;i<v1.size();i++)
    {
        cout << v1[i] << " ";

    }
    cout << '\n';
}



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

//*Board Manipulation*//

string draw_board(int n)
{
    string board;
    for(int i=0;i<n;i++)
    {
        board.append("_ ");
    }
    return board;
}

string place_in_board(string board,string letter,vector<string> play)
{
   int i;
   for(i=0; i < stoi(play[3]); i++)
   {
       int pos = stoi(play[4+i]);

       char char_letter = letter[0];
       char char_upper_letter = toupper(char_letter);
       string upper_letter = "";
       upper_letter = char_upper_letter;
       if(pos == 0)
       {
        board.replace(pos,1,upper_letter);
       }
       else
       {
        board.replace((pos-1)*2,1,upper_letter);
       }
   }
   return board;
}

vector<int> find_empty_spots(string board)
{
    vector<int> empty_positions;
    for(int i=0;i<board.size();i++)
    {
        char spot = board[i];
        if(spot == '_'){
            empty_positions.push_back(i);
        }
    }
    return empty_positions;
}

string fill_board(string board,string letter)
{
    vector<int> empty_positions = find_empty_spots(board);
    for(int i=0;i<empty_positions.size();i++)
    {
        int pos = empty_positions[i];
        board.replace(pos,1,letter);
    }
    return board;
}

//**************************************************//

//* Retrieve the data needed to execute the program *//

vector<string> get_data_command(char** command)
{
    string ip;
    string port;
    vector<string> res;
    if(get_number_of_elements(command)==5)
    {
        if(strcmp(string(command[1]).c_str(),"-n")==0 && strcmp(string(command[3]).c_str(),"-p")==0)
        {
            ip = string(command[2]);
            port = string(command[4]);
        }
        if(strcmp(string(command[1]).c_str(),"-p")==0 && strcmp(string(command[3]).c_str(),"-n")==0)
        {
            ip = string(command[4]);
            port = string(command[2]);
        }
        
    }
    else if(get_number_of_elements(command)== 3)
    {
        if(strcmp(string(command[1]).c_str(),"-n")==0)
        {
            ip = string(command[2]);
            port = "58011";
            

        }
        if(strcmp(string(command[1]).c_str(),"-p")==0)
        {
            char buffer2[128];
            gethostname(buffer2,128);
            ip = string(buffer2);
            port = string(command[2]);

        }
    }
    else
    {
        char buffer2[128];
        gethostname(buffer2,128);
        ip = string(buffer2);
        port = "58011";

    }
    res.push_back(ip);
    res.push_back(port);
    return res;
}

string format_message(string command,string message)
{
    string formated_message;
    formated_message.append(command);
    formated_message.append(" ");
    formated_message.append(message);
    formated_message.append("\n");
    return formated_message;
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
    received_udp = buffer;

    freeaddrinfo(res);
    close(fd);
}

/**
int receive_file_From_Server(char buffer)
    while ((read = tfs_read(source_file, buffer, BLOCK_SIZE)) > 0) {
        // make sure the entire buffer is written
        if (fwrite(buffer, sizeof(char), (size_t)read, dest_file) !=
            (size_t)read * sizeof(char)) {
            // ignore result since we return -1 anyway
            fclose(dest_file);
            tfs_close(source_file);
            return -1;
        }
**/

string send_to_tcp_server(string message,string port,string ip)
{
    extern int errno;
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[256];
    fd=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if (fd==-1){
        printf("erro ao criar o socket");
        exit(1);
    }  //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    const char* message_cos_char = message.data();
    char* message_char = (char*)message_cos_char;
    size_t message_lenght = message.length();
    const char* port_char = port.c_str();
    const char* ip_char = ip.c_str();
    errcode=getaddrinfo(ip_char,port_char,&hints,&res);
    if(errcode!=0){
        printf("erro a obter o endereço");
        exit(1);
    }
    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        printf("erro ao conectar ao servidor\n");
        exit(1);
    }
    
    
    n=write(fd,message_char,message_lenght);
    if(n==-1){
        printf("erro ao escrever\n");
        exit(1);
    }
    string received_message = "";
    n = read(fd,buffer,1);
    while(n!=-1)
    {
        n=read(fd,buffer,1);
        received_message.append(buffer);
    }
    
    if(n==-1){
        printf("erro ao ler\n");
        exit(1);
    }
    
    
    freeaddrinfo(res);
    close(fd);
    return received_message;
}



int main(int argc,char** argv)
{
    vector<string> id_port = get_data_command(argv);
    string ip = id_port[0];
    string port = id_port[1];
    string player_command;
    string player_id;
    string board;
    int end_player = 0;
    int num_trials=1;
    while(end_player == 0)
    {
        //************************************************************** UDP INTERACTION******************************************************//
        cin >> player_command;
        if(strcmp(player_command.c_str(),"start")==0 || strcmp(player_command.c_str(),"sg")==0){
            cin >> player_id;
            string message_to_send = format_message("SNG",player_id);
            send_to_udp_server(message_to_send,port,ip);
            player_command = "";
            vector<string> game_settings = parse_string(received_udp);
            if(strcmp(game_settings[0].c_str(),"RSG")==0 && strcmp(game_settings[1].c_str(),"OK")==0)
                board = draw_board(stoi(game_settings[2]));
                cout << "New game started (max ";
                cout << stoi(game_settings[3]);
                cout << " errors): ";
                cout << board;
                cout << "\n";
        }
        else if(strcmp(player_command.c_str(),"play")==0 || strcmp(player_command.c_str(),"pl")==0){
            string letter;
            cin >> letter;
            string message;
            message.append(player_id);
            message.append(" ");
            message.append(letter);
            message.append(" ");
            message.append(to_string(num_trials));
            string message_to_send = format_message("PLG",message);
            num_trials++;
            player_command = "";
            send_to_udp_server(message_to_send,port,ip);
            vector<string> board_possibl_mod = parse_string(received_udp);
            if(strcmp(board_possibl_mod[1].c_str(),"OK")==0)
            {
                board = place_in_board(board,letter,board_possibl_mod);
                cout << "Yes, ";
                cout << letter;
                cout << " is part of the word: ";
                cout << board;
                cout << "\n";
            }
            else if(strcmp(board_possibl_mod[1].c_str(),"WIN")==0)
            {
                board = fill_board(board,letter);
                cout << "WELL DONE! You Guessed: ";
                cout << board;
                cout << " \n";
            }
            else if(strcmp(board_possibl_mod[1].c_str(),"DUP")==0)
            {
                cout << "The Letter Was Already Sent\n";
                num_trials--;
            }
            else if(strcmp(board_possibl_mod[1].c_str(),"OVR")==0)
            {
                cout << "Theres No More Trials!!!: GAME OVER\n";
                num_trials = 1;
            }
            else if(strcmp(board_possibl_mod[1].c_str(),"INV")==0)
            {
                cout << "Sent Wrong Trial Number !!!\n";
            }
            else if(strcmp(board_possibl_mod[1].c_str(),"ERR")==0)
            {
                cout << "Incorrect Message Syntax or Invalid Player Id or This Player ID has No Ongoing Game\n";
                num_trials--;
            }
        }
        else if(strcmp(player_command.c_str(),"guess")==0 || strcmp(player_command.c_str(),"sw")==0)
        {
            string message;
            string message_to_send;
            string word;
            cin >> word;
            message.append(player_id);
            message.append(" ");
            message.append(word);
            message.append(" ");
            message.append(to_string(num_trials));
            message_to_send = format_message("PWG",message);
            send_to_udp_server(message_to_send,port,ip);
            num_trials++;
            player_command = "";
            vector<string> possibl_guess = parse_string(received_udp);
            if(strcmp(possibl_guess[1].c_str(),"WIN")==0)
            {
                cout << "WELL DONE! YOU GUESSED: ";
                cout << format_result(word);
                cout << "\n";
            }
            else if(strcmp(possibl_guess[1].c_str(),"NOK")==0)
            {
                cout << "Wrong Guess\n Still Guesseble\n";
            }
            else if(strcmp(possibl_guess[1].c_str(),"OVR")==0)
            {
                cout << "Theres No More Trials!!!: GAME OVER\n";
                num_trials=1;
            }
            else if(strcmp(possibl_guess[1].c_str(),"INV")==0)
            {
                cout << "Sent Wrong Trial Number !!!\n";
            }
            else if(strcmp(possibl_guess[1].c_str(),"ERR")==0)
            {
                cout << "Incorrect Message Syntax or Invalid Player Id or This Player ID has No Ongoing Game\n";
                num_trials--;
            }
        }
        else if(strcmp(player_command.c_str(),"rev")==0)
        {
            string message_to_send = format_message("REV",player_id);
            send_to_udp_server(message_to_send,port,ip);
            player_command = "";
            vector<string> status_game_rvl = parse_string(received_udp);
            if(strcmp(status_game_rvl[1].c_str(),"ERR")==0)
            {
                cout << "No Termination Of An Ongoing Game\n";  
            } 
            else if(strcmp(status_game_rvl[1].c_str(),"OK")==0)
            {
                cout << "Terminated An Ongoing Game\n";
                num_trials = 1;
            }
            else
            {
                cout << status_game_rvl[1].c_str() << "\n";
            }
            
        }
        else if(strcmp(player_command.c_str(),"quit")==0)
        {
            string message_to_send = format_message("QUT",player_id);
            send_to_udp_server(message_to_send,port,ip);
            player_command = "";
            board = "";
            cout << "game quit\n";
            num_trials = 1;
            vector<string> exit_quit_result = parse_string(received_udp);
            if(strcmp(exit_quit_result[1].c_str(),"ERR")==0)
            {
                cout << "No Termination Of An Ongoing Game\n";  
            } 
            else if(strcmp(exit_quit_result[1].c_str(),"OK")==0)
            {
                cout << "Terminated An Ongoing Game\n";   
            }
        }
       else if(strcmp(player_command.c_str(),"exit")==0)
       {
        string message_to_send = format_message("QUT","099265");
        send_to_udp_server(message_to_send,port,ip);
        player_command = "";
        vector<string> exit_quit_result = parse_string(received_udp);
        if(strcmp(exit_quit_result[1].c_str(),"OK")==0)
        {
           cout << "Terminated An Ongoing Game\n";
        } 
        else if(strcmp(exit_quit_result[1].c_str(),"ERR")==0)
        {
            cout << "No Termination Of An Ongoing Game\n";             
        }
        end_player = 1;
       }
       else if(strcmp(player_command.c_str(),"NC KILLGAME")==0)
       {
        string message_to_send = format_message("NC KILLGAME",player_id);
        send_to_udp_server(message_to_send,port,ip);
        player_command = "";
       }
       else if(strcmp(player_command.c_str(),"NC KILLDIR")==0)
       {
         string message_to_send = format_message("NC KILLDIR",player_id);
         send_to_udp_server(message_to_send,port,ip);
         player_command = "";
       }
        //******************************************************************TCP INTERACTION*********************************************************//
        else if(strcmp(player_command.c_str(),"scoreboard")==0 || strcmp(player_command.c_str(),"sb")==0)
        {
             string message_to_send = "GSB\n";
             string message_received = send_to_tcp_server(message_to_send,port,ip);
             player_command = "";
             cout << message_received;

             


        }

    }

    return 0;
}