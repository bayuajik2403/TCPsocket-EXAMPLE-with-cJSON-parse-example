/* I learn socket from https://www.geeksforgeeks.org/socket-programming-cc/ 
  and add modification marked by "// added"
*/


#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <iostream>
// we can define port here
// #define PORT 8080 

int main(int argc, char const *argv[]) 
{ 
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
    
    // added
    // we can just remove cin if we want to hardcode port and ip server
    int port;
    std::string ipServer;

    std::cout<<"enter server IP address: ";
    std::cin>>ipServer;
    std::cout<<"enter server port address: ";
    std::cin>>port;

    const char * charIPserver = ipServer.c_str();

    int sockfd = 0; 
    int valread; 

    struct sockaddr_in serv_addr; 
    char buffer[1024] = {0}; 


    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        std::cout<< "Socket creation fail"<<std::endl;
        return -1; 
    } 
   
    // server and port is specified in serv_addr
    serv_addr.sin_family = AF_INET; 
    // notes that we can hardcode port here 
    serv_addr.sin_port = htons(port);
    
       
    // convert IPv4 and IPv6 addresses from text to binary form 
    // notes that we can hardcode charIPserver here
    if(inet_pton(AF_INET, charIPserver, &serv_addr.sin_addr)<=0)  
    { 
        std::cout<<"invalid address / address not supported, it must be IPv4"<<std::endl; 
        return -1; 
    } 

    /*
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

    The connect() system call connects the socket referred to by the file descriptor sockfd 
    to the address specified by addr. Server’s address and port is specified in addr.
    */

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        std::cout<<"connection failed"<<std::endl;
        return -1; 
    } 

    // added so it can send multiple message
    while (true)
    {
        std::string message = "";
        std::cout<<"enter message: ";
        std::cin>>message;

        // we can send command by typing manually or hardcode 
        // type 1 to send play or 2 to send stop
        if (message=="1"){
            char play[] = "{\"command\":\"play\"}";
            send(sockfd , play , strlen(play) , 0 );
        } else if (message=="2"){
            char stop[] = "{\"command\":\"stop\"}";
            send(sockfd , stop , strlen(stop) , 0 );
        } else {
            const char * sendMessage = message.c_str();

            // send the message
            send(sockfd , sendMessage , strlen(sendMessage) , 0 );
        }
        // or we can just use switch case to replace above code.

        //emptying message
        message.clear();

        // read reply from server
        valread = read( sockfd , buffer, 1024); 
        printf("%s\n",buffer ); 
        // emptying buffer
        memset(buffer, 0, sizeof(buffer));
    }
      
    return 0; 
} 