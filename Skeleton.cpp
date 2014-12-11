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
#include <mutex>

#define Error(str) do {                         \
                perror (#str);                  \
        } while(0)

#define BACKLOG 100000000
#define RET_TYPE_VOID 0
#define RET_TYPE_INT 1
#define RET_TYPE_STRING 2
#define DEBUG 0

/*
    Title : A stream socket server demo
    http://beej.us/guide/bgnet/examples/server.c
*/
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
        std::cerr<<"\n Total bytes read ="<<bytesRead<<" actualLength = "<<actualLength;
    }
}

/*
 Services a client request.
*/
void
getRequestFromClient (int sock, Skeleton *skelIn) {
    if(DEBUG) {
        std::cout<<"\nsock = "<<sock<<" Waiting to read";
    }
    size_t totalLength;
    //bad code with lots of ifs

    //read the size of packet
    int ret = recv(sock, (char*) &totalLength, sizeof(totalLength), 0);
    size_t remLength = totalLength;
    //check if 4 bytes read properly
    if(ret == sizeof(totalLength)) {
        //verify length
        if(DEBUG) {
            std::cout<<"\nsock = "<<sock<<" totalLength = "<<totalLength<<"\n";
        }
        std::vector<char> buffer(totalLength);
        //read entire packet
        ret = recv(sock, (char*) &buffer[0], totalLength, 0);
        //totalLength var is set to zero at this point. Don't know why
        if(ret > 0 &&  ((unsigned int) ret == remLength)) {
            char *bufPtr = buffer.data();

            //read length of typeName
            size_t typeLen;
            memcpy(&typeLen, bufPtr, sizeof(typeLen));
            bufPtr += sizeof(typeLen);
            remLength -= sizeof(typeLen);

            if(DEBUG) {
                std::cout << "sock = " << sock << " typename length = " << typeLen << std::endl;
            }

            std::vector<char> typeName(typeLen);
            memcpy(&typeName[0], bufPtr, typeLen);
            bufPtr += typeLen;
            remLength -= typeLen;

            std::string typeNameStr(&typeName[0], typeLen);
            if(DEBUG) {
                std::cout << "sock = " << sock << " typename = " << typeNameStr.c_str() << std::endl;
            }

            //read length of objRef
            size_t objRefLen;
            memcpy(&objRefLen, bufPtr, sizeof(objRefLen));
            bufPtr += sizeof(objRefLen);
            remLength -= sizeof(objRefLen);

            if(DEBUG) {
                std::cout<<"\nsock = "<<sock<<" objRefLen = "<<objRefLen;
            }

            //read obj ref
            std::vector<char> objRef(objRefLen);
            memcpy(&objRef[0], bufPtr, objRefLen);
            bufPtr += objRefLen;
            remLength -= objRefLen;

            std::string objRefStr(&objRef[0], objRefLen);
            if(DEBUG) {
                std::cout<<"\nsock = "<<sock<<" objref = "<<objRefStr;
            }

            //read method Id (Note: its a normal int and NOT size_t
            int methodId;
            memcpy(&methodId, bufPtr, sizeof(methodId));
            bufPtr += sizeof(methodId);
            remLength -= sizeof(methodId);

            if(DEBUG) {
                std::cout<<"\nsock = "<<sock<<" methodId = "<<methodId;
            }

            //read remaining bytes
            if(remLength == 0) {
                std::cerr<<"\nsock = "<<sock<<" Nothing to read";
                return;
            }

            std::vector<char> data(remLength);
            memcpy(&data[0], bufPtr, remLength);

            if(DEBUG) {
                std::cout<<"\nsock = "<<sock<<" remaining data ="<<&data[0];
            }

            int resInt; //Note: this should be an int
            std::string resStr("");
            //From the object type name, get the Skel object
            //instance
            Skeleton *skelObj;
            try {
                skelObj = Skeleton::sSkelMap.at(typeNameStr);
            } catch (std::exception& ex) {
                std::cerr << "Exception ex = " << ex.what() << __FILE__ << __LINE__ << std::endl;
                std::cerr << "skel map = " << &(Skeleton::sSkelMap) << " size is " << Skeleton::sSkelMap.size() << std::endl;
                return;
            }
            // make the method call
            skelObj->callMethod(objRefStr, methodId, data, resInt, resStr);
            int retType = skelObj->getReturnType(methodId);
            //format result based on return type
            if(retType == RET_TYPE_VOID) {
                // do nothing
            } else if(retType == RET_TYPE_INT) { //int
                if(DEBUG) {
                    std::cout<<"\nsock = "<<sock<<" result = "<<resInt;
                }
                std::string res((char *) &resInt, sizeof(resInt));
                //send result back
                ret = write(sock, res.c_str(), res.length());
                if(ret < 0 || ((unsigned int) ret != res.length())) {
                    printError(ret, res.length());
                    return;
                } else {}

            } else if(retType == RET_TYPE_STRING) { //string
                if(DEBUG) {
                    std::cout<<"\nsock = "<<sock<<" result = "<<resStr;
                }
                //write length of string
                size_t len = resStr.length();
                std::string resPacket((char *) &len, sizeof(len));
                resPacket.append(resStr);
                //send result back
                ret = write(sock, resPacket.c_str(), resPacket.length());
                if(ret < 0 || ((unsigned int) ret != resPacket.length())) {
                    printError(ret, resPacket.length());
                    return;
                } else {}
            } else {
                assert(false);
            }
        }
    } else {
        printError(ret, sizeof(totalLength));
    }

    if(DEBUG) {
        std::cout<<"\nsock = "<<sock<<" Exiting";
    }
    close(sock);
}


void
waitForConnections(Skeleton *skelIn, int mSockfd) {

    int new_fd;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    if(DEBUG) {
        printf("server: waiting for connections...\n");
    }

    std::vector<std::thread*> threadsList;
    std::thread *t;
    bool isThreadCreated;
    size_t releasedThread = -1;
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(mSockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);

        if(DEBUG) {
            printf("server: got connection from %s\n", s);
        }

        isThreadCreated = false;
        while(!isThreadCreated) {
            try {
                t = new std::thread(getRequestFromClient, new_fd, skelIn);
                threadsList.push_back(t);
                isThreadCreated = true;
            } catch (std::exception &ex) {
                if(DEBUG) {
                    std::cerr << "Unable to create thread" << ex.what() << std::endl;
                }
                if(threadsList.size() > (releasedThread+1)) {
                    (threadsList.at(++releasedThread))->join();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                /*if(threadsList.size() > 0) {
                    std::thread *current = threadsList.at(0);
                    while(threadsList.size() > 0 && !current->joinable()) {
                        threadsList.erase(threadsList.begin());
                        current = threadsList.at(0);
                    }
                    current->join();
                    threadsList.erase(threadsList.begin());
                }*/
            }
        }
    }

    if(DEBUG) {
        std::cout<<"\n Exiting";
    }
    for(auto it = threadsList.begin(); it != threadsList.end(); it++) {
        t = *it;
        t->join();
    }
    close(mSockfd);
}

bool
Skeleton::initializeServer() {
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int yes = 1;
    int bindSucceeded = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //use my ip


    while(!bindSucceeded) {
        mPortNo.clear();
        mPortNo = mPortNo + std::to_string(mPortNoInt);
        //std::cout << "port no = " <<mPortNo << std::endl;

    if(( rv = getaddrinfo(NULL, mPortNo.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
     }


// loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((mSockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if(setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            return false;
        }

        if (bind(mSockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(mSockfd);
            //perror("server: bind");
            continue;
        }
 
        bindSucceeded = 1;
        break;
    }

    if (p == NULL)  {
       ++mPortNoInt; //try next port
    }
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return false;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if(listen(mSockfd, BACKLOG) == -1) {
        perror("listen");
        return false;
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return false;
    }

    if(gethostname(s, sizeof s) == 0) {
        if(DEBUG) {
            fprintf(stderr, "my name is = %s\n", s);
        }
        mServerName = s;
    } else {
        perror("error getting host name");
        return false;
    }

    return true;
}

std::string
Skeleton::getObjectReference() const {
    std::cerr << "'Skeleton::getObjectReference() const' needs to be filled in." << std::endl;
    return "an_object_ref";
}

void 
Skeleton::callMethod(std::string, int, std::vector<char>, int&, std::string&) {
    std::cerr << "'Skeleton::callMethod(std::string, int, std::vector<char>)' needs to be filled in." << std::endl;
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
    std::mutex serverInitMutex;
    serverInitMutex.lock();
    if(mSockfd == -1) {
        if(!initializeServer()) {
            exit(1); //unable to start server
        }
    }
    serverInitMutex.unlock();

    static std::thread sServerThread(waitForConnections, this, mSockfd); 
    return sServerThread;
}

std::map<std::string, Skeleton*> Skeleton::sSkelMap;
int Skeleton::mSockfd = -1;
std::string Skeleton::mServerName;
std::string Skeleton::mPortNo;
int Skeleton::mPortNoInt = 10000;
std::mutex serverInitMutex;

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
