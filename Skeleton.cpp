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

/*********** DOSTUFF() ******************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void getRequestFromClient (int sock)
{
    std::cout<<"\n Waiting to read from client = "<<sock<<"\n";
    unsigned int length;

//    while(1) {
//        std::cout<<"\n next \n";

        //read the size of packet
        int ret = recv(sock, (char*) &length, 4, 0);
        //check if 4 bytes read properly
        if(ret == 4) {
            //verify length
            std::cout<<"\n length = "<<length<<"\n";

            //read remaining bytes
            std::vector<char> data(length);
            ret = recv(sock, &data[0], length, 0);

            if(ret == length) {
                std::string dataStr(&data[0], length);
                ret = write(sock,"return value XYZ", 18);

                if (ret < 0) {
                     Error("ERROR writing to socket");
                } else {
                    std::cout<<"total bytes read ="<<length<<"\n";
                    std::cout<<"to call method = "<<&data[0]<<"\n";
                }

            } else {
                std::cout<<"\n incorrect amount of data read "<<ret<<" actual length = "<<length<<"\n";
                std::cout<<"\n incorrect data = "<<&data[0];
                return;
            }
        } else if (ret > 0 ) {
            std::cout<<"\n incorrect length read "<<ret<<"\n";
            return;
        } else if (ret == 0) {
            std::cout<<"\n Client socket closed = "<<sock;
        } else {
            Error("ERROR reading from socket");
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
    std::cout<<"\n Exiting sock = "<<sock;
}


std::string
Skeleton::getObjectReference() const {
    std::cerr << "'Skeleton::getObjectReference() const' needs to be filled in." << std::endl;
    return "an_object_ref";
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
         std::cout<<"\n Waiting for client\n";
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) {
             Error("ERROR on accept");
         }
         std::cout<<"\n New client connected = "<<newsockfd<<"\n";

         t = new std::thread(getRequestFromClient, newsockfd);
         threadsList.push_back(t);
     } // end of while

     std::cout<<"\n Exiting \n";
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
