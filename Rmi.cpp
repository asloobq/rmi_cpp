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
#include <arpa/inet.h>

namespace Rmi {

#define MSGLENGTH 1024
#define RET_TYPE_VOID 0
#define RET_TYPE_INT 1
#define RET_TYPE_STRING 2
#define PORT_NO "10003"

#define error(str) do {                         \
                perror (#str);                  \
        } while(0)

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Rmi::connectToServer() {
    std::cout<<"\n Connecting \n";

    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo("localhost", PORT_NO, &hints, &servinfo)) != 0) {
        fprintf(stderr,"usage: client hostname\n");
        return;
    }

// loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return ;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
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
