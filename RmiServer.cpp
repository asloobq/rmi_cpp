#include "RmiServer.hpp"
#include <iostream>

namespace Rmi {

RmiServer::RmiServer() {
    //start server
    startServer();
}

RmiServer:: ~RmiServer() {
    //stop server
    stopServer();
}

void RmiServer::startServer() {
    std::cout<<"\n Server Started \n";
}

void RmiServer::stopServer() {
    std::cout<<"\n Server Stopped \n";
}

}
