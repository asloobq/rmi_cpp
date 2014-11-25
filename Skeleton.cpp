#include "Skeleton.hpp"
#include <assert.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unistd.h>

#define Error(str) do {                         \
                perror (#str);                  \
        } while(0)
int
max (int a, int b) {
        return a > b ? a : b;
}

void
printError(int bytesRead, int actualLength) {
    if (bytesRead == 0) {
        std::cerr<<"\n Socket connection closed";
    } else if (bytesRead < 0) {
        std::cerr<<"\n ERROR reading from socket";
    } else {
        std::cout<<"\n Total bytes read ="<<bytesRead<<" actualLength = "<<actualLength;
    }
}

/*********** DOSTUFF() ******************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void
getRequestFromClient (int sock, Skeleton *skelIn)
{
    std::cout<<"\nsock = "<<sock<<" Waiting to read";
    size_t totalLength;
    //bad code with lots of ifs

        //read the size of packet
        int ret = recv(sock, (char*) &totalLength, sizeof(totalLength), 0);
        size_t remLength = totalLength;
        //check if 4 bytes read properly
        if(ret == sizeof(totalLength)) {
            //verify length
            std::cout<<"\nsock = "<<sock<<" totalLength = "<<totalLength<<"\n";

            //read length of objRef
            size_t objRefLen;
            int ret = recv(sock, (char*) &objRefLen, sizeof(objRefLen), 0);

            if(ret == sizeof(objRefLen)) {
                remLength -= sizeof(objRefLen);
                //read obj ref
                std::cout<<"\nsock = "<<sock<<" objRefLen = "<<objRefLen;
                std::vector<char> objRef(objRefLen);
                ret = recv(sock, &objRef[0], objRefLen, 0);

                if(ret == objRefLen) {
                    remLength -= objRefLen;
                    std::string objRefStr(&objRef[0], objRefLen);
                    std::cout<<"\nsock = "<<sock<<" objref = "<<objRefStr;

                    //read method Id (Note: its a normal int and NOT size_t
                    int methodId;
                    int ret = recv(sock, (char*) &methodId, sizeof(methodId), 0);

                    if(ret == sizeof(methodId)) {
                        remLength -= sizeof(methodId);
                        std::cout<<"\nsock = "<<sock<<" methodId = "<<methodId;

                        //read remaining bytes
                        if(remLength == 0) {
                            std::cout<<"\nsock = "<<sock<<" Nothing to read";
                            return;
                        }
                        std::vector<char> data(remLength);
                        ret = recv(sock, &data[0], remLength, 0);

                        if(ret == remLength) {
                            std::string dataStr(&data[0], remLength);
                            std::cout<<"\nsock = "<<sock<<" remaining data ="<<&data[0];

                            int resInt; //Note: this should be an int
                            std::string resStr("");
                            skelIn->callIntMethod(objRefStr, methodId, data, resInt, resStr);

                            int retType = skelIn->getReturnType(methodId);
                            if(retType == 0) {
                            } else if(retType == 1) { //int
                                std::cout<<"\nsock = "<<sock<<" result = "<<resInt;
                                std::string res((char *) &resInt, sizeof(resInt));
                                ret = write(sock, res.c_str(), res.length());
                                if(ret != res.length()) {
                                    printError(ret, res.length());
                                    return;
                                }

                            } else if(retType == 2) { //string
                                std::cout<<"\nsock = "<<sock<<" result = "<<resStr;
                                //write length of string
                                size_t len = resStr.length();
                                std::string resPacket((char *) &len, sizeof(len));
                                resPacket.append(resStr);
                                ret = write(sock, resPacket.c_str(), resPacket.length());
                                if(ret != resPacket.length()) {
                                    printError(ret, resPacket.length());
                                    return;
                                }
                            } else {
                                assert(false);
                            }
   
                        } else {
                            printError(ret, remLength);
                            return;
                        }
                    } else {
                        printError(ret, sizeof(methodId));
                        return;
                    }
                } else {
                    printError(ret, objRefLen); 
                    return;
                }
            } else {
                printError(ret, sizeof(objRefLen));
                return;
            }
        } else {
            printError(ret, sizeof(totalLength));
        }

    std::cout<<"\nsock = "<<sock<<" Exiting";
}


void
initializeServer(Skeleton *skelIn) {
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        Error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 10003; //atoi(10002);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     int yes = 1;
     if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
         Error("ERROR on setsockopt");
     }

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
              Error("ERROR on binding");
     }

     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     std::vector<std::thread*> threadsList;
     std::thread *t;
     while (1) {
         std::cout<<"\n Waiting for clients to connect";
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) {
             Error("ERROR on accept");
         }
         std::cout<<"\n New client connected = "<<newsockfd<<"\n";

         t = new std::thread(getRequestFromClient, newsockfd, skelIn);
         threadsList.push_back(t);
     } // end of while

     std::cout<<"\n Exiting";
     for(auto it = threadsList.begin(); it != threadsList.end(); it++) {
        t = *it;
        t->join();
     }
     close(sockfd);
}

std::string
Skeleton::getObjectReference() const {
    std::cerr << "'Skeleton::getObjectReference() const' needs to be filled in." << std::endl;
    return "an_object_ref";
}

void 
Skeleton::callIntMethod(std::string, int, std::vector<char>, int&, std::string&) {
    std::cerr << "'Skeleton::callIntMethod(std::string, int, std::vector<char>)' needs to be filled in." << std::endl;
}

int
Skeleton::getReturnType(int) {
    std::cerr << "'Skeleton::getReturnType(int)' needs to be filled in." << std::endl;
    return -1;
}

void
Skeleton::startServer() {
    mServerThread = new std::thread(initializeServer, this);
}

void
Skeleton::stopServer() {
    mServerThread->join();
}


/*
 * These lines should go at the end of every source code file
 * to set the indent, tabs, and other options.  Feel free to
 * add other appropriate options here.
 *
 */

/* Local Variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
/* vim: set filetype=cpp tabstop=8 shiftwidth=4 softtabstop=4 tw=79 expandtab: */
