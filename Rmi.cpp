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
    std::cout<<"\n Connecting \n";

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

    std::cout << "\n Waiting to write";
    int n = write(sockfd, data.c_str(), static_cast<int>(data.length()));
    if (n < 0) { 
         error("ERROR writing to socket");
    }

    /*std::cout<<"\n Waiting for results";
    char buffer[256];
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n < 0) {
         error("ERROR reading from socket");
    }*/

    std::ostringstream retBuff("") ;
    //retBuff << buffer;
    return retBuff.str();
}

void Rmi::asyncCall(std::string objRefIn, int methodIdIn, std::string methodNameIn, std::string methodSignIn,
                    Params& paramsListIn, std::string bufferIn ) { 
    std::cout<<"\n In asyncCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    /*char buffer[256];
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
    disconnect();*/
}


int Rmi::intCall(std::string objRefIn, int methodIdIn, std::string methodNameIn, std::string methodSignIn,
                 Params& paramsListIn, std::string bufferIn) {
    std::cout<<"\n In Rmi::intCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    //add method sign
    //unsigned int methodSignLen = static_cast<int>(methodSignIn.length());
    //std::string methodSignPacket((char *) &methodSignLen, 4);
    //methodSignPacket.append(methodSignIn);

    //add method name
    //unsigned int methodNameLen = static_cast<int>(methodNameIn.length());
    //std::string methodNamePacket((char *) &methodNameLen, 4);
    //methodNamePacket.append(methodNameIn);

    //add object ref
    unsigned int objRefLen = objRefIn.length();
    std::string objRefPacket((char *) &objRefLen, 4);
    objRefPacket.append(objRefIn);

    //add methodId
    std::string methodIdPacket((char *) &methodIdIn, 4);
    objRefPacket.append(methodIdPacket);

    unsigned int bufLen = bufferIn.length();
    std::string bufPacket((char *) &bufLen, 4);
    bufPacket.append(bufferIn);
    //std::cout<<"\n bufPacket length = "<<bufPacket.length();
    //printf("\n %d %d %s %d", bufferIn.c_str(), bufferIn.c_str() + 4, bufferIn.c_str() + 8, bufferIn.c_str() + 20);

    //objRefPacket.append(methodNamePacket);
    //objRefPacket.append(methodSignPacket);
    objRefPacket.append(bufPacket);

//    std::ostringstream obuffer("");
//    obuffer << "\nRmi::intCall name = "<< methodNameIn.c_str() <<" sign = "<< methodSignIn.c_str() <<" \n";


//    std::string packet = obuffer.str();
    std::string packet = objRefPacket;
    connectToServer();
    std::string retVal = call(sockfd, packet);
    std::cout<<"\n return value = "<<retVal;
    disconnect();

    return -1;
}

std::string Rmi::stringCall(std::string objRefIn, int methodIdIn, std::string methodNameIn, std::string methodSignIn,
                            Params& paramsListIn, std::string bufferIn) {
    //return "";
    std::cout<<"\n In Rmi::stringCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    /*//add method sign
    unsigned int methodSignLen = static_cast<int>(methodSignIn.length());
    std::string methodSignPacket((char *) &methodSignLen, 4);
    methodSignPacket.append(methodSignIn);

    //add method name
    unsigned int methodNameLen = static_cast<int>(methodNameIn.length());
    std::string methodNamePacket((char *) &methodNameLen, 4);
    methodNamePacket.append(methodNameIn);*/

    //add object ref
    unsigned int objRefLen = static_cast<int>(objRefIn.length());
    std::string objRefPacket((char *) &objRefLen, 4);
    objRefPacket.append(objRefIn);

    //add methodId
    std::string methodIdPacket((char *) &methodIdIn, 4);
    objRefPacket.append(methodIdPacket);

    unsigned int bufLen = bufferIn.length();
    std::string bufPacket((char *) &bufLen, 4);
    bufPacket.append(bufferIn);

    //objRefPacket.append(methodNamePacket);
    //objRefPacket.append(methodSignPacket);
    objRefPacket.append(bufPacket);

    std::string packet = objRefPacket;
    connectToServer();
    std::string retVal = call(sockfd, packet);
    std::cout<<"\n return value = "<<retVal;
    disconnect();

   return "";
}

}
