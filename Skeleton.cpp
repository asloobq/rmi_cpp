#include "Skeleton.hpp"
#include <assert.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <exception>

#define Error(str) do {                         \
                perror (#str);                  \
        } while(0)

#define PORT_NO "10003"
#define BACKLOG 10

//std::map<std::string, Skeleton*> sSkelMap;

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

            //read length of typeName
            size_t typeLen;
            int ret = recv(sock, (char*) &typeLen, sizeof (typeLen), 0);
            if(ret == sizeof(typeLen)) {
                remLength -= sizeof (typeLen);
                //read type name
                std::cout << "sock = " << sock << " typename length = " << typeLen << std::endl;
                std::vector<char> typeName(typeLen);
                ret = recv(sock, &typeName[0], typeLen, 0);
                if(ret == typeLen) {
                    remLength -= typeLen;
                    std::string typeNameStr(&typeName[0], typeLen);
                    std::cout << "sock = " << sock << " typename = " << typeNameStr.c_str() << std::endl;
            //read length of objRef
            size_t objRefLen;
            ret = recv(sock, (char*) &objRefLen, sizeof(objRefLen), 0);

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
                            //From the object type name, get the Skel object
                            //instance
                            //std::string skelName("DevInterface_skel");
                            Skeleton *skelObj;
                            try {
                                skelObj = Skeleton::sSkelMap.at(typeNameStr);
                                skelObj->callIntMethod(objRefStr, methodId, data, resInt, resStr);
                            } catch (std::exception& ex) {
                                std::cout << "Exception ex = " << ex.what() << __FILE__ << __LINE__ << std::endl;
                                std::cout << "skel map = " << &(Skeleton::sSkelMap) << " size is " << Skeleton::sSkelMap.size() << std::endl;
                            }
                            //skelIn->callIntMethod(objRefStr, methodId, data, resInt, resStr);
                            int retType = skelObj->getReturnType(methodId);
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
                    printError(ret, remLength);
                    return;
                }
            } else {
                printError(ret, remLength);
                return;
            }
            
        } else {
            printError(ret, sizeof(totalLength));
        }

    std::cout<<"\nsock = "<<sock<<" Exiting";
}


void
initializeServer(Skeleton *skelIn) {
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //use my ip

    if(( rv = getaddrinfo(NULL, PORT_NO, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
     }


// loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if(listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    std::vector<std::thread*> threadsList;
    std::thread *t;
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        std::cout<<"Skeleton.cpp initializeServer map " << &(Skeleton::sSkelMap) << " size  = " << Skeleton::sSkelMap.size() <<std::endl;

        printf("server: got connection from %s\n", s);
                t = new std::thread(getRequestFromClient, new_fd, skelIn);
                threadsList.push_back(t);
    }

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
    getServerInstance();
}

void
Skeleton::stopServer() {
    getServerInstance().join();
}

std::thread&
Skeleton::getServerInstance() {
    static std::thread sServerThread(initializeServer, this);
    return sServerThread;
}

std::map<std::string, Skeleton*> Skeleton::sSkelMap;

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
