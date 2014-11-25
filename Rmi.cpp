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
#define RET_TYPE_VOID 0
#define RET_TYPE_INT 1
#define RET_TYPE_STRING 2

#define error(str) do {                         \
                perror (#str);                  \
        } while(0)


void Rmi::connectToServer() {
    std::cout<<"\n Connecting \n";

    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

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
}

void Rmi::disconnect() { 
    close(sockfd);
    std::cout<<"\n Disconnected \n";
}

std::string
Rmi::call(int sockfd, std::string packet, int retType) {

    std::cout<< "\n buffer ="<<packet.c_str();
    //get length of whole packet
    size_t length = static_cast<int>(packet.length());
    std::cout<<"\n Packet size = "<< length<<"\n";
    std::string data((char *) &length, sizeof(length));
    data.append(packet);

    std::cout<<"Sending out "<<data.c_str()<<"\n";
    std::cout <<"size = "<<data.length()<<"\n";

    std::cout << "\n Waiting to write";
    int n = write(sockfd, data.c_str(), static_cast<int>(data.length()));
    if (n < 0) { 
         error("ERROR writing to socket");
    }

    std::ostringstream retBuff("") ;

    if(retType == RET_TYPE_INT) {
        std::cout<<"\n Waiting for results";
        size_t result;                
        int ret = recv(sockfd, (char *)&result, sizeof(result), 0);
        if (ret < 0) {
            error("ERROR reading from socket");
        } else if(ret != sizeof(result)) {
            error("ERROR incorrect number of bytes read");
        }
        retBuff << result;
    } else if(retType == RET_TYPE_STRING) {
        std::cout<<"\n Waiting for results";
        size_t len;
        int ret = recv(sockfd, &len, sizeof(len), 0);
        if(ret == sizeof(len)) {
            char *buffer = new char[len+1];
            int ret = recv(sockfd, buffer, len, 0);
            buffer[len] = '\0';
            if( ret == len) {
                retBuff << buffer;
            } else {
                if (ret < 0) {
                    error("ERROR reading from socket");
                } else {
                    error("ERROR incorrect number of bytes read");
                }
            }
            
        } else {
            if (ret < 0) {
                error("ERROR reading from socket");
            } else {
                error("ERROR incorrect number of bytes read");
            }
        }
    }

    return retBuff.str();
}

void Rmi::asyncCall(std::string objRefIn, int methodIdIn, std::string methodNameIn, std::string methodSignIn,
                    Params& paramsListIn, std::string bufferIn ) { 
    std::cout<<"\n In asyncCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    //add object ref
    size_t objRefLen = objRefIn.length();
    std::string objRefPacket((char *) &objRefLen, sizeof(objRefLen));
    objRefPacket.append(objRefIn);

    //add methodId
    std::string methodIdPacket((char *) &methodIdIn, sizeof(methodIdIn));
    objRefPacket.append(methodIdPacket);

    size_t bufLen = bufferIn.length();
    std::string bufPacket((char *) &bufLen, sizeof(bufLen));
    bufPacket.append(bufferIn);

    objRefPacket.append(bufPacket);

    std::string packet = objRefPacket;
    connectToServer();
    call(sockfd, packet, RET_TYPE_VOID);
    disconnect();

}


int Rmi::intCall(std::string objRefIn, int methodIdIn, std::string methodNameIn, std::string methodSignIn,
                 Params& paramsListIn, std::string bufferIn) {
    std::cout<<"\n In Rmi::intCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    //add object ref
    size_t objRefLen = objRefIn.length();
    std::string objRefPacket((char *) &objRefLen, sizeof(objRefLen));
    objRefPacket.append(objRefIn);

    //add methodId
    std::string methodIdPacket((char *) &methodIdIn, sizeof(methodIdIn));
    objRefPacket.append(methodIdPacket);

    size_t bufLen = bufferIn.length();
    std::string bufPacket((char *) &bufLen, sizeof(bufLen));
    bufPacket.append(bufferIn);

    objRefPacket.append(bufPacket);

    std::string packet = objRefPacket;
    connectToServer();
    std::string retVal = call(sockfd, packet, RET_TYPE_INT);
    int result = atoi(retVal.c_str());
    printf("\n return value = %d", result);
    disconnect();

    return result;
}

std::string Rmi::stringCall(std::string objRefIn, int methodIdIn, std::string methodNameIn, std::string methodSignIn,
                            Params& paramsListIn, std::string bufferIn) {
    //return "";
    std::cout<<"\n In Rmi::stringCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";

    //add object ref
    size_t objRefLen = objRefIn.length();
    std::string objRefPacket((char *) &objRefLen, sizeof(objRefLen));
    objRefPacket.append(objRefIn);

    //add methodId
    std::string methodIdPacket((char *) &methodIdIn, sizeof(methodIdIn));
    objRefPacket.append(methodIdPacket);

    size_t bufLen = bufferIn.length();
    std::string bufPacket((char *) &bufLen, sizeof(bufLen));
    bufPacket.append(bufferIn);   //add object ref

    objRefPacket.append(bufPacket);

    std::string packet = objRefPacket;
    connectToServer();
    std::string retVal = call(sockfd, packet, RET_TYPE_STRING);
    std::cout<<"\n return value = "<<retVal;
    disconnect();

   return retVal;
}

}
