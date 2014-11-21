#ifndef RMISERVER_HPP
#define RMISERVER_HPP
namespace Rmi {

/* clients list */
struct Client {
    int fd;
};

/*
  A single instance of the server needs to keep track of and service all objects.
*/
class RmiServer {

    // support 20 clients for now
    struct Client mClientList[20];
    int mClientCount;

//    void chat();

    public:
        RmiServer();
        virtual ~RmiServer();
        void startServer();
        void startServer2();
        void stopServer();

};

}
#endif //RMISERVER_HPP
