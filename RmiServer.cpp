#include "RmiServer.hpp"
#include <iostream>
#include <sys/socket.h>
//#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
//#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <vector>

namespace Rmi {

#define Error(str) do {                         \
                perror (#str);                  \
        } while(0)
int
max (int a, int b) {
        return a > b ? a : b;
}



/*********** DOSTUFF() ******************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void getRequestFromClient (int sock)
{
    std::cout<<"\n Waiting to read from client = "<<sock<<"\n";
    int n;
    char buffer[256]; //what can be the max size?

    while(1) {
        std::cout<<"\n next \n";
        bzero(buffer,256);
        n = read(sock, buffer,255);
        if (n < 0) {
            Error("ERROR reading from socket");
        }
        printf("to call method = %s\n", buffer);
        n = write(sock,"return value XYZ",18);
        if (n < 0) {
            Error("ERROR writing to socket");
        }
    }
}

void
chat (Client *mClientList, int mClientCount) {

    std::cout<<"\n Chat server called\n";

    char s[1024];
    fd_set rset; //A set of file descriptors
    int t, ret, i;
    struct timeval tm;

    tm.tv_sec = 2;
    tm.tv_usec = 0;

    FD_ZERO (&rset); //clear the set

    for (;;) {
                // wait for clients to connect
                for (;!mClientCount && sleep (2);)
                        ;

                // to be thread safe, create a new list from the existing
                // one and perform operations on that. this is easy and
                // good programming.
                t = 0;
                for (i = 0; i < 20; i++)
                        if (mClientList[i].fd != -1) {
                                FD_SET (mClientList[i].fd, &rset); // Add the FD to the set
                                t = max (mClientList[i].fd, t);
                        }
                // allow a program to monitor multiple file descriptors,
                // waiting until one or more of them become "ready"
                // for some class of I/O operation.
                // A file descriptor is considered ready if it is 
                // possible to perform the corresponding operation
                // without blocking
                // ready = select(int nfds, fd_set *readfds, fd_set *writefds,
                //                fd_set *exceptfds, const struct timespec *timeout,
                //                const sigset_t *sigmask);
                // returns the number of file descriptors contained in the three 
                // returned descriptor sets.
                ret = select (t + 1, &rset, NULL, NULL, &tm);
                // If no file descriptor was selected
                if (ret == -1)
                        continue;
// read from the file descriptor that was selected.
                for (i = 0; i < 20; i++) {
                        if (mClientList[i].fd != -1)
                                if (FD_ISSET (mClientList[i].fd, &rset)) { //check if the FD is in the set
                                        ret = recv (mClientList[i].fd, s, 1023, 0);
                                        s[ret] = '\0';
                                        //write (stdout, s, ret);
                                        std::cout<<"msg = "<<s<<std::endl;
                                        //_write (cl[i].fd, s, ret);
                                }
                }
        }


    //return (void *) 0;
}

RmiServer::RmiServer() {
    //start server
    //startServer();
}

RmiServer:: ~RmiServer() {
    //stop server
    stopServer();
}

void RmiServer::startServer() {

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
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) {
             Error("ERROR on accept");
         }
         std::cout<<"\n New client connected = "<<newsockfd<<"\n";

         t = new std::thread(getRequestFromClient, newsockfd);
         threadsList.push_back(t);
         /*
         pid = fork();
         if (pid < 0)
             Error("ERROR on fork");
         if (pid == 0)  {
             //in child process
             close(sockfd); //loose control from the old socket
             dostuff(newsockfd); //do stuff with the client socket
             exit(0);
         }
         else close(newsockfd); //in parent. nothing to do with the new client socket
         */
     } /* end of while */
     close(sockfd);

     for(auto it = threadsList.begin(); it != threadsList.end(); it++) {
        t = *it; //(*it).join();
        t->join();
     }
}

void RmiServer::startServer2() {
    std::cout<<"\n Server Started \n";
    int fd1;

    fd1 = socket(AF_INET, SOCK_STREAM, 0);
    if(fd1 == -1) {
        Error("Socket failed.");
    }

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons (10000);

    //Bind the socket to the address
    if(bind (fd1, (struct sockaddr*) &addr, sizeof (addr)) == -1) {
        Error ("bind failed");
    }

    std::cout<<("\nbinding socket successful");
    //listen on the socket for connections. 3 is the size of the backlog queue
    if(listen (fd1, 3) < 0) {
        Error ("listen failed");
    }

    socklen_t len = sizeof(struct sockaddr);

    for(int i = 0; i < 20; i++) {
        mClientList[i].fd = -1;
    }

    //pthread_t thread;
    //start a new thread to wait for clients
    //pthread_create(&thread, NULL, chat, NULL);
    std::thread t(chat, mClientList, mClientCount);

    int fd2;
    struct sockaddr_in addr1;
    mClientCount = 0;

    for (;;) {
        fd2 = accept(fd1, (struct sockaddr*) &addr1, &len);
        if(fd2 == -1) {
            Error ("accept failed");
        }

        mClientList[mClientCount].fd = fd2;
        mClientCount++;

        std::cout<<"\n A client is connected\n";
    }

}

void RmiServer::stopServer() {
    std::cout<<"\n Server Stopped \n";
}

}
