/* I learn socket from https://www.geeksforgeeks.org/socket-programming-cc/ 
  and add modification marked by "// added"
*/

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 9595

// added
//#include "cjson/cjson/cJSON.h"
#include "cJSON.h"

#include <iostream>

// added
void handleMessage(char *buffer, int server_fd){
    // create json structure
    cJSON *checkCmd = cJSON_Parse(buffer, false);
	// char to save command
    char command[50] = {0};

	if (NULL == checkCmd){
		std::cout << "failed when parse string" << std::endl;
        char serverReply[] = "server receive your message";
    	memset(serverReply, 0, sizeof(serverReply));
	}
    
    // copy string command from client to char command
    strncpy(command, cJSON_GetObjectItem(checkCmd, "command")->valuestring, 50);

    if (NULL != strstr(command, "play")){
        // we could decide what to do here such as send reply or call a function
        // in this case, i just send reply
        char serverReply[] = "what do you want to play?";
    	send(server_fd , serverReply , strlen(serverReply) , 0 );
        memset(serverReply, 0, sizeof(serverReply)); 
	} else if (NULL != strstr(command, "stop")){
	    char serverReply[] = "okay I will stop";
    	send(server_fd , serverReply , strlen(serverReply) , 0 );
        memset(serverReply, 0, sizeof(serverReply));
    } else {
        char serverReply[] = "server receive your message";
    	send(server_fd , serverReply , strlen(serverReply) , 0 );
        memset(serverReply, 0, sizeof(serverReply));
    }

    cJSON_Delete(checkCmd);
    
}

int main(int argc, char const *argv[]) 
{ 
    // server_fd is sockfd
    int server_fd; 
    int new_socket; 
    int valread;
 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    //char const *replyMessage = "your message was received by the server"; 
    
    /*

    socket creation:    
    *int sockfd = socket(domain, type, protocol)
    
    -sockfd is basically an int you can name it whatever you like

    -domain is communication domain e.g., 
    AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol)

    -type: communication type
    SOCK_STREAM: TCP(reliable, connection oriented)
    SOCK_DGRAM: UDP(unreliable, connectionless)
    
    -protocol: Protocol value for Internet Protocol(IP), which is 0. 

    */
   
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 

     /*  
    int setsockopt(int sockfd, int level, int optname,  
                   const void *optval, socklen_t optlen);

    This helps in manipulating options for the socket referred by the file 
    descriptor sockfd. This is completely optional, but it helps in reuse 
    of address and port. Prevents error such as: “address already in use”.               
    */

    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    
    /*
    int bind(int sockfd, const struct sockaddr *addr, 
                          socklen_t addrlen);
    
    After creation of the socket, bind function binds the socket to the address and 
    port number specified in addr(custom data structure). In the example code, we bind 
    the server to the localhost, hence we use INADDR_ANY to specify the IP address.
    */
    
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    std::cout<<"bind success"<<std::endl;
    /*
    int listen(int sockfd, int backlog);

    listen will wait until client attempt to connect. Backlog defines the maximum length 
    to which the queue of pending connections for sockfd may grow. If a connection request 
    arrives when the queue is full, the client may receive an error with an indication of 
    ECONNREFUSED.
    */

    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    std::cout<<"server listening to port: "<<PORT<<std::endl;

    /*
    int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)

    Creates a new connected socket, and returns a new file descriptor referring to that 
    socket. At this point, connection is established between client and server, and they are 
    ready to transfer data.
    */
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }


    // added so it can receive multiple message
    while(true){ 
        valread = read( new_socket , buffer, 1024); 
        printf("%s\n",buffer ); 

        // added
        char command[]="{\"command\"";
        if(strstr(buffer,command) != NULL){    //Strstr says does b contain a
            handleMessage(buffer,new_socket);
        } else {
            char serverReply[] = "server receive your message";
    	    send( new_socket , serverReply , strlen(serverReply) , 0 );
            //emptying serverReply
            memset(serverReply, 0, sizeof(serverReply));   
        }
        
        //emptying buffer
        memset(buffer, 0, sizeof(buffer));
        

        
    } 
    return 0; 
} 
