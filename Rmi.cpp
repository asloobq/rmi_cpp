#include "Rmi.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

namespace Rmi {

#define MSGLENGTH 1024

#define error(str) do {                         \
                perror (#str);                  \
        } while(0)

//int
//max (int a, int b)
//{
//        return a > b ? a : b;
//}

void Rmi::connectToServer() {
    std::cout<<"\n Connected \n";

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    //if (argc < 3) {
    //   fprintf(stderr,"usage %s hostname port\n", argv[0]);
    //   exit(0);
    //}
    portno = 10003; //atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname("localhost");//get from user/server
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    //make function call
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);

}

void Rmi::disconnect() { std::cout<<"\n Disconnected \n";}

void Rmi::asyncCall(std::string objRefIn, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) { 
    std::cout<<"\n asyncCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";
}


int Rmi::intCall(std::string objRefIn, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) {
    std::cout<<"\n Rmi::intCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";
    return -1;
}

std::string Rmi::stringCall(std::string, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) {
    std::cout<<"\n Rmi::stringCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";
    return "";
}

}
