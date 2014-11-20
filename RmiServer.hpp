#ifndef RMISERVER_HPP
#define RMISERVER_HPP
namespace Rmi {

class RmiServer {
    public:
        RmiServer();
        virtual ~RmiServer();
        void startServer();
        void stopServer();
};

}
#endif //RMISERVER_HPP
