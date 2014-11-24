#include "Skeleton.hpp"
#include <assert.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <vector>

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
    unsigned int totalLength;
    //bad code with lots of ifs
//    while(1) {
//        std::cout<<"\n next \n";

        //read the size of packet
        int ret = recv(sock, (char*) &totalLength, 4, 0);
        unsigned int remLength = totalLength;
        //check if 4 bytes read properly
        if(ret == 4) {
            //verify length
            std::cout<<"\nsock = "<<sock<<" totalLength = "<<totalLength<<"\n";

            //read length of objRef
            int objRefLen;
            int ret = recv(sock, (char*) &objRefLen, 4, 0);

            if(ret == 4) {
                remLength -= 4;
                //read obj ref
                std::cout<<"\nsock = "<<sock<<" objRefLen = "<<objRefLen;
                std::vector<char> objRef(objRefLen);
                ret = recv(sock, &objRef[0], objRefLen, 0);

                if(ret == objRefLen) {
                    remLength -= objRefLen;
                    std::string objRefStr(&objRef[0], objRefLen);
                    std::cout<<"\nsock = "<<sock<<" objref = "<<objRefStr;

                    //read method Id
                    int methodId;
                    int ret = recv(sock, (char*) &methodId, 4, 0);

                    if(ret == 4) {
                        remLength -= 4;
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
                            int result = skelIn->callIntMethod(objRefStr, methodId, data);
                            std::cout<<"\nsock = "<<sock<<" result = "<<result;
                        } else {
                            printError(ret, remLength);
                            return;
                        }
                    } else {
                        printError(ret, 4);
                        return;
                    }

                    /*//read length of the method name
                    int methodNameLen;
                    int ret = recv(sock, (char*) &methodNameLen, 4, 0);

                    if(ret == 4) {
                        //read method name
                        std::cout<<"\nsock = "<<sock<<" methodNameLen = "<<methodNameLen;
                        std::vector<char> methodName(methodNameLen);
                        ret = recv(sock, &methodName[0], methodNameLen, 0);

                        if(ret == methodNameLen) {
                            std::cout<<"\nsock = "<<sock<<" methodName = "<<&methodName[0];

                            //method sign length
                            int methodSignLen;
                            int ret = recv(sock, (char*) &methodSignLen, 4, 0);
                            if(ret == 4) {
                                //read method sign
                                std::cout<<"\nsock = "<<sock<<" methodSignLen = "<<methodSignLen;
                                std::vector<char> methodSign(methodSignLen);
                                ret = recv(sock, &methodSign[0], methodSignLen, 0);

                                if(ret == methodSignLen) {
                                    std::cout<<"\nsock = "<<sock<<" methodSign = "<<&methodSign[0];                               
                                } else {
                                    printError(ret, 4);
                                    return;
                                }
                            } else {
                                printError(ret, 4);
                                return;
                            }
                        } else {
                            printError(ret, 4);
                            return;
                        }

                    } else {
                        printError(ret, 4);
                        return;
                    }


                    //read remaining bytes
                    unsigned int remLength = totalLength - objRefLen;
                    if(remLength == 0) {
                        std::cout<<"\nsock = "<<sock<<" Nothing to read";
                        return;
                    }
                    std::vector<char> data(remLength);
                    ret = recv(sock, &data[0], remLength, 0);

                    if(ret == remLength) {
                        std::string dataStr(&data[0], remLength);
                        std::cout<<"\nsock = "<<sock<<" remaining data ="<<&data[0];

                        ret = write(sock,"return value XYZ", 18);
                        if (ret == 18) {
                        } else {
                            if (ret < 0) {
                                Error("ERROR writing to socket");
                            } else {
                                std::cout<<"total bytes written ="<<ret<<"\n";
                                std::cout<<"to call method = "<<&data[0]<<"\n";
                            }
                            return;
                        }

                    } else {
                        printError(ret, remLength);
                        return;
                    } */
                } else {
                    printError(ret, objRefLen); 
                    return;
                }
            } else {
                printError(ret, 4);
                return;
            }
        } else {
            printError(ret, 4);
        }
        //n = recv(sock, buffer, 255, 0);
        /*if (n < 0) {
            Error("ERROR reading from socket");
        } else if(n > 0) {
            printf("total bytes read = %d\n", n);
            printf("to call method = %s\n", buffer);
        n = write(sock,"return value XYZ",18);
        if (n < 0) {
            Error("ERROR writing to socket");
        }
        } else {
            break;
        }*/
  //  }
    std::cout<<"\nsock = "<<sock<<" Exiting";
}


std::string
Skeleton::getObjectReference() const {
    std::cerr << "'Skeleton::getObjectReference() const' needs to be filled in." << std::endl;
    return "an_object_ref";
}

int 
Skeleton::callIntMethod(std::string, int, std::vector<char>) {
    std::cerr << "'Skeleton::callIntMethod(std::string, int, std::vector<char>)' needs to be filled in." << std::endl;
    return -1;
}


void
Skeleton::startServer() {
//    mServer = new Rmi::RmiServer();
//    mServer->startServer();
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     //if (argc < 2) {
     //    fprintf(stderr,"ERROR, no port provided\n");
     //    exit(1);
     //}
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        Error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = 10003; //atoi(10002);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
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

         t = new std::thread(getRequestFromClient, newsockfd, this);
         threadsList.push_back(t);
     } // end of while

     std::cout<<"\n Exiting";
     for(auto it = threadsList.begin(); it != threadsList.end(); it++) {
        t = *it;
        t->join();
     }
     close(sockfd);
}

void
Skeleton::stopServer() {
//    if(mServer) {
//        mServer->stopServer();
//    }
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
