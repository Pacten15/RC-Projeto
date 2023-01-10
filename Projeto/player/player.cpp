
#include<iostream>
#include<fstream>
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
#define GN 30;
string received_udp;




//* String and vector of strings manipulation and screening of the structures *//


void print_string(string s)
{
    printf("%s\n", s.c_str());
}

/*Function to process the word translating to the board of the game*/
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

/*Function that puts all the elements of a string on their upper case form*/
string all_string_upper(string s)
{
    string s_result;
    for(int i=0;i<s.size();i++)
    {
      char letter = s[i];
      s_result.push_back(toupper(letter));
    }
    return s_result;
}

/*Creates a string that makes the simples server requests
readeble by the server*/
string format_message(string command,string message)
{
    string formated_message;
    formated_message.append(command);
    formated_message.append(" ");
    formated_message.append(message);
    formated_message.append("\n");
    return formated_message;
}


void print_vector(vector<string> v1)
{
    for(uint i=0;i<v1.size();i++)
    {
        cout << v1[i] << " ";

    }
    cout << '\n';
}


/*Gives the number of cha*(strings) 
that exist in a char**(string vector)*/
int get_number_of_elements(char ** argument)
{
    vector<string> v;
    int i=0;
    while(argument[i] != NULL)
        i++;
    return i;
}


/*Creates a vector<string> containing 
all the substrings separeted with a space " " that exist 
on a string*/
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
    s.erase(s.length()-1,1);/*removes the \n from the last word*/
    result.push_back(s);/*puts the last word*/
    return result;
}



int count_spaces_on_string(string message)
{
    int counter=0;
    for(int i=0;i<message.size();i++)
    {
        char letter = message[i];
        if(letter == ' ')
        {
            counter++;
        }
    }
    return counter;
}

/*function to indicate the index of the n space that exists on a string*/
int find_n_index_of_spaces(string s,int n)
{
    int in,counter,index;
    counter=0;
    for(in=0;in<s.size();in++)
    {
        if(s[in] == ' '){
            counter++;
        }
        if(counter == n)
            break;
    }
    return in;
}

//*Board Manipulation*//

/*Consider board as the spaces where the letters are going to be placed*/

/*Draw the underscores where the letters will be placed*/
string draw_board(int n)
{
    string board;
    for(int i=0;i<n;i++)
    {
        board.append("_ ");
    }
    return board;
}

/*replace the underscores of the positions refered on the play sent from the server with 
the letter sent to the server*/
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

/*function that gives a vector containing the indexes of the spots of the 
board where a letter hasnt been placed*/
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

/*function that fills the underscores on a board with the letter from the input*/
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

/**Command**/

/* Retrieve ip and port of server where the game is running*/

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
            int g_n = GN;
            port = to_string(58000 + g_n);
            

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
        int g_n = GN;
        port = to_string(58000 + g_n);

    }
    res.push_back(ip);
    res.push_back(port);
    return res;
}



/*****************************************************************************************************************************/
/*****************************************************************************************************************************/



/**Secundary functions used on the function send_tcp_message
formatting the message received to the parameters received and
place the data from the message to a file**/

/*function to process all the content from a scorboard message and afterwards put the data into a file*/
void process_response_scoreboard(string message_retrived)
{
    if(strcmp(message_retrived.c_str(),"The Scoreboard is Empty\n") != 0){
        vector<string> parsed_message = parse_string(message_retrived);
        string file_name = parsed_message[2];
        int size_data = stoi(parsed_message[3]);
        string file_directory = "Score_boards/";
        file_directory.append(file_name);/*Diretoria do ficheiro*/
        ofstream response_file(file_directory);/*Criacao do ficheiro*/
        char buffer2[size_data];/*numero de bytes que o ficheiro tem*/
        int index_begin_file = message_retrived.find('\n')+1;
        string file_data_not_process = message_retrived.substr(index_begin_file,size_data);/*string that only has the data_file*/
        /*conversions to use strcpy and put the data into a buffer that is placed on the file*/
        const char* data_cos_char = file_data_not_process.data();
        char* data_char = (char*)data_cos_char;
        strcpy(buffer2,data_char);
        response_file << buffer2;

        response_file.close();
        cout << "Received Scoreboard File: " << file_directory << " (" << size_data << ")" << "\n";
        cout << file_data_not_process;
        
    }
    else
        cout << "The Scoreboard is Empty\n";
}


/*function to process all the content from a state message and afterwards put the data into a file*/
void process_respose_state(string message_retrived)
{
    if(strcmp(message_retrived.c_str(),"The player Has No Associated Games\n") != 0){
        vector<string> parsed_message = parse_string(message_retrived);
        string file_data_not_process;
        string file_name = parsed_message[2]; 
        int size_data = stoi(parsed_message[3]);/*size of data file*/
        string file_directory = "States/";
        file_directory.append(file_name);/*Directory of the file*/
        ofstream response_file(file_directory);/*criation of the file on the directory*/
        char buffer2[size_data];/*buffer to store only the data file*/
        int start_file_index = find_n_index_of_spaces(message_retrived,5);
        file_data_not_process = message_retrived.substr(start_file_index,size_data);/*string that only has the data_file*/
        /*conversions to use strcpy and put the data into a buffer that is placed on the file*/
        const char* data_cos_char = file_data_not_process.data();
        char* data_char = (char*)data_cos_char;
        strcpy(buffer2,data_char);
        response_file << buffer2;
        response_file.close();
        cout << "Received State File: " << file_directory << " (" << size_data << ")" << "\n";
        cout << file_data_not_process << "\n\n";
    }
    else
        cout << "The player Has No Associated Games\n\n";

}



/**Senders**/

/*Function that receives a message,the port and ip 
of the server in wich the games is running, sending 
the message to the server and putting the answer into 
a string globally allocated*/                                                                    
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
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    
    if(fd==-1) /*error*/exit(1);
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket
	char buffer2[256];
	gethostname(buffer2, 256);
    errcode=getaddrinfo(buffer2,port_char,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);
    n=sendto(fd,message_char,message_lenght,0,res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);
    addrlen=sizeof(addr);
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,sizeof timeout);
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) 
        received_udp = "";
    else
        received_udp = buffer;
    freeaddrinfo(res);
    close(fd);
}




/*Function that receives a message,the port and ip 
of the server in which the games is running and depending on the message sent to the server
it creates a file with data received from it*/
string send_to_tcp_server(string message,string port,string ip)
{
    int fd,errcode,n;
    ssize_t n_bytes,n_left,n_written,n_read;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    vector<string> message_parsed = parse_string(message);
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

    /*Write to the server the player command*/
    n_bytes = message_lenght;
    char buffer[message_lenght];
    char *ptr;
    ptr = strcpy(buffer,message_cos_char);
    n_left = n_bytes;
    while(n_left>0)
    {
        n_written = write(fd,ptr,n_left);
        if(n_written<=0){
            printf("erro ao escrever\n");
            exit(1);
        }
        n_left -= n_written;
        ptr += n_written; 
    }
    memset(buffer,0,sizeof(buffer));
    char buffer_read[128];
    string received_message = "";

    /*Read data from the server to a file in the player directory*/
    if(strcmp(message.c_str(),"GSB\n")==0)
    {
        string status;
        /*reads the first two elemements from the message*/
        while(true){
            memset(buffer_read,0,sizeof(buffer_read));
            n_read = read(fd,buffer_read,1);
            if(n_read==-1){
                printf("Erro de Leitura\n");
                exit(1);
            }
            else
            {
                received_message.append(buffer_read);
                /*condition that stops the read when the as read two elements of the message sent from a server*/
                if(count_spaces_on_string(received_message) == 2)
                {
                    /*obtain the second element of the message that contains the status*/
                    status = received_message.substr(find_n_index_of_spaces(received_message,1)+1,find_n_index_of_spaces(received_message,2)-find_n_index_of_spaces(received_message,1)-1);
                    break;
                }
            }
        }
        if(strcmp(status.c_str(),"OK")==0)
        {
            /*reads the rest of the message containing the name of the file, size and his data*/
            while(true){
                memset(buffer_read,0,sizeof(buffer_read));
                n_read = read(fd,buffer_read,32);
                if(n_read==-1){
                    printf("Erro de Leitura\n");
                    exit(1);
                }   
                else if(n_read == 0 || n_read == EOF)
                {
                    break;
                }
                else
                {
                    received_message.append(buffer_read);
                }
            }
        }
        else{
            received_message = "The Scoreboard is Empty\n\n";
        }
            
    }


    if(strcmp(message_parsed[0].c_str(),"GHL")==0){
        int imag_size;
        string file_name;
        string status;
        /*reads the first two elemements from the message*/
        while(true){
            memset(buffer_read,0,sizeof(buffer_read));
            n_read = read(fd,buffer_read,1);
            if(n_read==-1){
                printf("Erro de Leitura\n");
                exit(1);
            }
            else
            {
                received_message.append(buffer_read);
                /*condition that stops the read when the as read two elements of the message sent from a server*/
                if(count_spaces_on_string(received_message) == 2)
                {
                    /*obtain the second element of the message that contains the status*/
                    status = received_message.substr(find_n_index_of_spaces(received_message,1)+1,find_n_index_of_spaces(received_message,2)-find_n_index_of_spaces(received_message,1)-1);
                    break;
                }
            }
        }
        if(strcmp(status.c_str(),"OK")==0){
            /*Read the next two elements containing the file name*/
            while(true)
            {
                memset(buffer_read,0,sizeof(buffer_read));
                n_read = read(fd,buffer_read,1);
                if(n_read==-1){
                    printf("Erro de Leitura\n");
                    exit(1);
                }
                else if(n_read == 0 || n_read == EOF){
                    break;
                }
                else
                {
                    received_message.append(buffer_read);
                    /*condition that stops the read when the as read four elements of the message sent from a server*/
                    if(count_spaces_on_string(received_message) == 4)
                    {
                        string file_name_char = received_message.substr(find_n_index_of_spaces(received_message,2)+1,find_n_index_of_spaces(received_message,3)-find_n_index_of_spaces(received_message,2)-1);
                        string size_imag_char = received_message.substr(find_n_index_of_spaces(received_message,3)+1,find_n_index_of_spaces(received_message,4)-find_n_index_of_spaces(received_message,3)-1);
                        imag_size = stoi(size_imag_char);
                        file_name = file_name_char;
                        break;
                    }
                }
            }

            char buffer_imag[256];
            string file_directory = "Hints/";
            file_directory.append(file_name);
            ofstream response_file(file_directory);
            ssize_t bytes_to_read = 128;
            size_t number_to_read_full_buffer = imag_size/bytes_to_read;/*number of times we can read 128 bytes from the server */
            /*number of bytes left to read when its need to read less of 128 bytes*/
            size_t number_left = imag_size - (number_to_read_full_buffer*bytes_to_read);
            int counter = 0;

            /*Reads from the server the image data that can be read in cycles of 128 bytes,
            registers into counter the real number of bytes read and puts the bytes into the file*/
            for(int i=0;i<number_to_read_full_buffer;i++)
            {
                memset(buffer_imag,0,sizeof(buffer_imag));/*clean buffer for next interation*/
                n_read = read(fd,buffer_imag,bytes_to_read);
                if(n_read==-1){
                    printf("Erro de Leitura\n");
                    exit(1);
                }
                else if(n_read == 0 || n_read == EOF)
                    break;
                else
                {
                    for(int j=0;j<n_read;j++)
                        response_file << buffer_imag[j];
                }
                counter += n_read;
            }
            /*The read sometimes doesnt read the number of bytes we ask to*/
            if(counter != number_to_read_full_buffer)
            {
                char rest[128 + number_to_read_full_buffer*128-counter];/*buffer with the size of the bytes not read during the previous cycle*/
                n_read = read(fd,rest,number_to_read_full_buffer*128-counter);
                for(int j=0;j<number_to_read_full_buffer*128-counter;j++)
                        response_file << rest[j];
            }
            /*read the rest of the bytes left */
            if(number_left !=0){
                n_read = read(fd,buffer_imag,number_left);
                if(n_read==-1){
                    printf("Erro de Leitura\n");
                    exit(1);
                }
                else
                {
                    for(int j=0;j<n_read;j++)
                        response_file << buffer_imag[j];
                }
            }
            
            
            response_file.close();
            cout << "Received Hint File: " << file_directory << " (" << imag_size << ")" << "\n\n";
            
        }
        else 
            received_message = "Theres No Hint For This Play Session\n\n";
        
    }


    if(strcmp(message_parsed[0].c_str(),"STA")==0)
    {
        string status;
        /*reads the first two elemements from the message*/
        while(true){
            memset(buffer_read,0,sizeof(buffer_read));
            n_read = read(fd,buffer_read,1);
            if(n_read==-1){
                printf("Erro de Leitura\n");
                exit(1);
            }
            else
            {          
                received_message.append(buffer_read);
                /*condition that stops the read when the as read two elements of the message sent from a server*/
                if(count_spaces_on_string(received_message) == 2)
                {
                    /*obtain the second element of the message that contains the status*/
                    status = received_message.substr(find_n_index_of_spaces(received_message,1)+1,find_n_index_of_spaces(received_message,2)-find_n_index_of_spaces(received_message,1)-1);
                    break;
                }
            }
        }
        if(strcmp(status.c_str(),"ACT")==0 || strcmp(status.c_str(),"FIN")==0)
        {
            while(true){
                memset(buffer_read,0,sizeof(buffer_read));
                n_read = read(fd,buffer_read,32);
                if(n_read==-1){
                    printf("Erro de Leitura\n");
                    exit(1);
                }   
                else if(n_read == 0 || n_read == EOF)
                {
                    break;
                }
                else
                {
                    received_message.append(buffer_read);
                }
            } 
        }
        else
            received_message = "The player Has No Associated Games\n";
    }
    freeaddrinfo(res);
    close(fd);
    return received_message;
}


void instructions()
{
    cout << "                   GAME INSTRUCTIONS                   \n\n";
    cout << "write start or sg + player_id  -> start a game\n";
    cout << "write play or pl + a letter    -> guess a letter from the game word\n";
    cout << "write guess or sw + a word     -> guess the word from the game\n";
    cout << "write rev                      -> reveal the word and end game\n";
    cout << "write scoreboard or sb         -> Sends text file with the 10-top scores of the game\n";
    cout << "write hit or h                 -> help to guess the word by a image file\n";
    cout << "write state or st              -> get state of the current game\n";   
    cout << "write quit                     -> terminate ongoing game\n";
    cout << "write exit                     -> terminates program and a possible ongoing game\n\n"; 
    
}


int main(int argc,char** argv)
{
    vector<string> id_port = get_data_command(argv);
    string ip = id_port[0];
    string port = id_port[1];
    string player_command;
    string player_id;
    string board;
    string previous_player_id;
    string previous_message_play;
    string previous_message_guess;
    int end_player = 0;
    int num_trials=1;
    int num_error=0;
    int max_errors;
    instructions();
    while(end_player == 0)
    {
        //************************************************************** UDP INTERACTION******************************************************//

        
        cin >> player_command;
        /*compare the player comand with the possible commands*/
        if(strcmp(player_command.c_str(),"start")==0 || strcmp(player_command.c_str(),"sg")==0){
            cin >> player_id;
            string message_to_send = format_message("SNG",player_id);
            send_to_udp_server(message_to_send,port,ip);
            player_command = "";
            vector<string> game_settings = parse_string(received_udp);
            if(strcmp(game_settings[0].c_str(),"RSG")==0 && strcmp(game_settings[1].c_str(),"OK")==0)
            {
                board = draw_board(stoi(game_settings[2]));
                max_errors = stoi(game_settings[3]);
                cout << "New game started (max ";
                cout << max_errors;
                cout << " errors): ";
                cout << board << "\n";
                cout << "Trial: " << num_trials << "\n";
                cout << "Number of Errors: " << num_error << "/" << max_errors << "\n\n";
            }
            else if(strcmp(game_settings[0].c_str(),"RSG")==0 && strcmp(game_settings[1].c_str(),"ERR")==0)
            {
                "Syntax Of Message Wrong Or Invalid Player ID\n";
            }
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
            if(letter.size() == 0 || letter.size()>1)
            {
                cout << "play command only accepts one character use guess to use multiple characters\n\n";
            }
            send_to_udp_server(message_to_send,port,ip);
            vector<string> board_possibl_mod = parse_string(received_udp);
            if(received_udp.size() != 0){
                previous_message_play = message_to_send;
                if(strcmp(board_possibl_mod[1].c_str(),"OK")==0)
                {
                    board = place_in_board(board,letter,board_possibl_mod);
                    cout << "Yes, ";
                    cout << letter;
                    cout << " is part of the word: ";
                    cout << board << "\n";
                    cout << "Trial: " << num_trials  << "\n";
                    cout << "Number of Errors: " << num_error << "/" << max_errors << "\n\n";

                }
                else if(strcmp(board_possibl_mod[1].c_str(),"NOK")==0)
                {
                    num_error++;
                    cout << "The Letter Does Not Belong To The Word: "<< max_errors - num_error << " Allowable Errors\n";
                    cout << "Current Board: " << board << "\n";
                    cout << "Trial Number: " << num_trials << "\n\n";
                }
                else if(strcmp(board_possibl_mod[1].c_str(),"WIN")==0)
                {
                    board = fill_board(board,letter);
                    cout << "WELL DONE! You Guessed: ";
                    num_trials = 1;
                    num_error=0;
                    max_errors=0;
                    cout << all_string_upper(board);
                    cout << " \n\n";
                }
                else if(strcmp(board_possibl_mod[1].c_str(),"DUP")==0)
                {
                    cout << "The Letter Was Already Sent\n";
                    cout << "Current Board: " << board << "\n\n";
                    num_trials--;
                }
                else if(strcmp(board_possibl_mod[1].c_str(),"OVR")==0)
                {
                    cout << "Theres No More Trials!!!: GAME OVER\n\n";
                    num_trials = 1;
                    num_error = 0;
                    max_errors = 0;
                }
                else if(strcmp(board_possibl_mod[1].c_str(),"INV")==0)
                {
                    cout << "Sent Wrong Trial Number or Same Trial Number Different Letter !!!\n\n";
                }
                else if(strcmp(board_possibl_mod[1].c_str(),"ERR")==0)
                {
                    cout << "Incorrect Message Syntax or Invalid Player Id or This Player ID Has No Ongoing Game\n\n";
                    num_trials--;
                }
                else
                {
                    num_trials--;
                }
            }
            else if(received_udp.size() == 0){
                send_to_udp_server(previous_message_play,port,ip);
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
            if(received_udp.size() != 0)
            {
                previous_message_guess = message_to_send;
                if(strcmp(possibl_guess[1].c_str(),"WIN")==0)
                {
                    cout << "WELL DONE! YOU GUESSED: ";
                    cout << format_result(word);
                    num_trials = 1;
                    num_error = 0;
                    max_errors = 0;
                    cout << "\n";
                }
                else if(strcmp(possibl_guess[1].c_str(),"NOK")==0)
                {
                    num_error++;
                    cout << "Wrong Guess: " << max_errors - num_error << " Allowable Errors\n";
                    cout << "Trial Number: " << num_trials << "\n\n";
                }
                else if(strcmp(possibl_guess[1].c_str(),"OVR")==0)
                {
                    cout << "Theres No More Trials!!!: GAME OVER\n";
                    num_trials=1;
                    num_error = 0;
                    max_errors = 0;
                }
                else if(strcmp(possibl_guess[1].c_str(),"INV")==0)
                {
                    cout << "Sent Wrong Trial Number !!!\n\n";
                }
                else if(strcmp(possibl_guess[1].c_str(),"ERR")==0)
                {
                    cout << "Incorrect Message Syntax or Invalid Player Id or Player ID has No Ongoing Game\n\n";
                    num_trials--;
                }
                else
                {
                    num_trials--;
                }
            }
            else
            {
                cout << "out of time";
                send_to_udp_server(previous_message_guess,port,ip);
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
                cout << "No Termination Of An Ongoing Game\n\n";  
            } 
            else if(strcmp(status_game_rvl[1].c_str(),"OK")==0)
            {
                cout << "Termination Of An Ongoing Game\n\n";
                num_trials = 1;
                num_error = 0;
                max_errors = 0;
                player_id = "";
                board = "";
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
            cout << "game quit\n";
            vector<string> exit_quit_result = parse_string(received_udp);
            if(strcmp(exit_quit_result[1].c_str(),"ERR")==0)
            {
                cout << "No Termination Of An Ongoing Game\n\n";  
            } 
            else if(strcmp(exit_quit_result[1].c_str(),"OK")==0)
            {
                cout << "Terminated An Ongoing Game\n\n";
                num_trials = 1;
                num_error = 0;
                max_errors = 0;
                board = "";
                previous_player_id = player_id;
                player_id = "";

            }
            else if(strcmp(exit_quit_result[1].c_str(),"NOK")==0)
            {
                cout << "Theres No Ongoing Game To Be Terminated\n\n";             
            }
        }
       else if(strcmp(player_command.c_str(),"exit")==0)
       {
        if(player_id.size() != 0){
            string message_to_send = format_message("QUT",player_id);
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
            else if(strcmp(exit_quit_result[1].c_str(),"NOK")==0)
            {
                cout << "Theres No Ongoing Game To Be Terminated\n";             
            }
        }
        end_player = 1;
       }




        //******************************************************************TCP INTERACTION*********************************************************//



        else if(strcmp(player_command.c_str(),"scoreboard")==0 || strcmp(player_command.c_str(),"sb")==0)
        {
             string message_to_send = "GSB\n";
             string message_received = send_to_tcp_server(message_to_send,port,ip);
             player_command = "";
             process_response_scoreboard(message_received);
        }
        else if(strcmp(player_command.c_str(),"hint")==0 || strcmp(player_command.c_str(),"h")==0)
        {
            string message = "GHL";
            string message_to_send = format_message("GHL",player_id);
            string message_received = send_to_tcp_server(message_to_send,port,ip);
            player_command = "";
        }
        else if(strcmp(player_command.c_str(),"state")==0 || strcmp(player_command.c_str(),"st")==0)
        {
            string message = "STA";
            string message_to_send;
            if(player_id.size() != 0)
               message_to_send = format_message("STA",player_id);
            else
                message_to_send = format_message("STA",previous_player_id);
            string message_received = send_to_tcp_server(message_to_send,port,ip);
            player_command = "";
            process_respose_state(message_received);
        }
        else
            cout << "Command Not Recognized\n\n";
        


    }

    return 0;
}
