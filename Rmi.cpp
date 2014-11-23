#include "Rmi.hpp"
#include <iostream>
#include <sstream>
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

    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //char buffer[256];
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
    //printf("Please enter the message: ");
    //bzero(buffer,256);
    //fgets(buffer,255,stdin);
    //n = write(sockfd,buffer,strlen(buffer));
    //if (n < 0) 
    //     error("ERROR writing to socket");
    //bzero(buffer,256);
    //n = read(sockfd,buffer,255);
    //if (n < 0) 
    //     error("ERROR reading from socket");
    //printf("%s\n",buffer);
    //close(sockfd);

}

void Rmi::disconnect() { 
    close(sockfd);
    std::cout<<"\n Disconnected \n";
}

std::string
Rmi::call(int sockfd, std::string packet) {

    std::cout<< "\n buffer ="<<packet.c_str();
    //get length of whole packet
    int length = static_cast<int>(packet.length());
    std::cout<<"\n Packet size = "<< length<<"\n";
    std::string data((char *) &length, 4);
    data.append(packet);

    std::cout<<"Sending out "<<data.c_str()<<"\n";
    std::cout <<"size = "<<data.length()<<"\n";

    connectToServer();
    int n = write(sockfd, data.c_str(), static_cast<int>(data.length()));
    if (n < 0) { 
         error("ERROR writing to socket");
    }

    char buffer[256];
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n < 0) {
         error("ERROR reading from socket");
    }

    std::ostringstream retBuff("") ;
    retBuff << buffer;
    return retBuff.str();
}

void Rmi::asyncCall(std::string objRefIn, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) { 
    std::cout<<"\n In asyncCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    char buffer[256];
    bzero(buffer,256);
    sprintf(buffer, "\n asyncCall name = %s sign = %s \n", methodNameIn.c_str(), methodSignIn.c_str());

    connectToServer();
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) { 
         error("ERROR writing to socket");
    }

    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n < 0) {
         error("ERROR reading from socket");
    }
    printf("return value = %s\n",buffer);
    disconnect();
}


int Rmi::intCall(std::string objRefIn, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) {
    return -1;
    std::cout<<"\n In Rmi::intCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    std::ostringstream obuffer("");
    obuffer << "\nRmi::intCall name = "<< methodNameIn.c_str() <<" sign = "<< methodSignIn.c_str() <<" \n";
    std::string packet = obuffer.str();

    connectToServer();
    std::string retVal = call(sockfd, packet);
    std::cout<<"\n return value = "<<retVal;
    disconnect();

    return -1;
}

std::string Rmi::stringCall(std::string, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) {
    std::cout<<"\n In Rmi::stringCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";
    
    std::ostringstream obuffer("");
    obuffer << "\nRmi::stringCall name = "<< methodNameIn.c_str() <<" sign = "<< methodSignIn.c_str() <<" \n";
    std::string packet = obuffer.str();

    connectToServer();
    std::string retVal = call(sockfd, packet);
    std::cout<<"\n return value = "<<retVal;
    disconnect();

   return "";
}

}
