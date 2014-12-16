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

#define DEBUG 0

#define error(str) do {                         \
                perror (#str);                  \
        } while(0)

/*
    Title : A stream socket client demo
    http://beej.us/guide/bgnet/examples/client.c
*/

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Rmi::connectToServer(std::string serverNameIn, std::string portNoIn) {
    if(DEBUG) {
        std::cout<<"\n Connecting \n";
    }

    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(serverNameIn.c_str(), portNoIn.c_str(), &hints, &servinfo)) != 0) {
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
    if(DEBUG) {
        printf("client: connecting to %s\n", s);
    }

    freeaddrinfo(servinfo); // all done with this structure
}

void Rmi::disconnect() { 
    close(sockfd);
    if(DEBUG) {
        std::cout<<"\n Disconnected \n";
    }
}

std::string
Rmi::call(int sockfd, std::string packet, int retType) {

    //get length of whole packet
    size_t length = static_cast<int>(packet.length());
    if(DEBUG) {
        std::cout<<"\n Packet size = "<< length<<"\n";
    }
    std::string data((char *) &length, sizeof(length));
    data.append(packet);

    if(DEBUG) {
        std::cout<<"Sending out "<<data.c_str()<<"\n";
        std::cout <<"size = "<<data.length()<<"\n";

        std::cout << "\n Waiting to write";
    }
    int n = write(sockfd, data.c_str(), static_cast<int>(data.length()));
    if (n < 0) { 
         error("ERROR writing to socket");
    }

    std::ostringstream retBuff("") ;

    if(retType == RET_TYPE_INT) {
        if(DEBUG) {
            std::cout<<"\n Waiting for results";
        }
        size_t result;                
        int ret = recv(sockfd, (char *)&result, sizeof(result), 0);
        if (ret < 0) {
            error("ERROR reading from socket");
        } else if(ret != sizeof(result)) {
            error("ERROR incorrect number of bytes read");
        }
        retBuff << result;
    } else if(retType == RET_TYPE_STRING) {
        if(DEBUG) {
            std::cout<<"\n Waiting for results";
        }
        size_t len;
        int ret = recv(sockfd, &len, sizeof(len), 0);
        if(ret == sizeof(len)) {
            char *buffer = new char[len+1];
            int ret = recv(sockfd, buffer, len, 0);
            buffer[len] = '\0';
            if((ret >= 0) && ((unsigned)ret == len)) {
                retBuff << buffer;
            } else {
                if (ret < 0) {
                    error("ERROR reading from socket");
                } else {
                    error("ERROR incorrect number of bytes read");
                }
            }
            delete [] buffer;

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

//Packs the call information in a byte array
std::string
Rmi::pack(std::string typeNameIn, std::string objRefIn, int methodIdIn, std::string bufferIn) {

    //add type name length
    size_t typeLen = typeNameIn.length();
    std::string packet((char *) &typeLen, sizeof (typeLen));

    //add typename
    packet.append(typeNameIn);

    //add object ref length
    size_t objRefLen = objRefIn.length();
    packet.append((char *) &objRefLen, sizeof(objRefLen));

    //add object ref
    packet.append(objRefIn);

    //add methodId
    packet.append((char *) &methodIdIn, sizeof(methodIdIn));

    //add buffer length
    size_t bufLen = bufferIn.length();
    packet.append((char *) &bufLen, sizeof(bufLen));

    //add buffer
    packet.append(bufferIn);

    return packet;
}

void Rmi::asyncCall(std::string serverNameIn, std::string portNoIn, std::string typeNameIn,
                    std::string objRefIn, int methodIdIn, std::string bufferIn ) {
    if(DEBUG) {
        std::cout<<"\n In asyncCall id = " << methodIdIn << std::endl;
    }

    std::string packet = pack(typeNameIn, objRefIn, methodIdIn, bufferIn);
    connectToServer(serverNameIn, portNoIn);
    call(sockfd, packet, RET_TYPE_VOID);
    disconnect();

}


int Rmi::intCall(std::string serverNameIn, std::string portNoIn, std::string typeNameIn,
                 std::string objRefIn, int methodIdIn, std::string bufferIn) {
    if(DEBUG) {
        std::cout<<"\n In Rmi::intCall id = " << methodIdIn << " typename = " << typeNameIn <<std::endl;
    }

    std::string packet = pack(typeNameIn, objRefIn, methodIdIn, bufferIn);
    connectToServer(serverNameIn, portNoIn);
    std::string retVal = call(sockfd, packet, RET_TYPE_INT);
    int result = atoi(retVal.c_str());
    if(DEBUG) {
        printf("\n return value = %d", result);
    }
    disconnect();

    return result;
}

std::string Rmi::stringCall(std::string serverNameIn, std::string portNoIn, std::string typeNameIn,
                            std::string objRefIn, int methodIdIn, std::string bufferIn) {
    if(DEBUG) {
        std::cout<<"\n In Rmi::stringCall id = " << methodIdIn << std::endl;
    }

    std::string packet = pack(typeNameIn, objRefIn, methodIdIn, bufferIn);
    connectToServer(serverNameIn, portNoIn);
    std::string retVal = call(sockfd, packet, RET_TYPE_STRING);
    if(DEBUG) {
        std::cout<<"\n return value = "<<retVal;
    }
    disconnect();

   return retVal;
}

}
