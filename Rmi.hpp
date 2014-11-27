#ifndef RMI_HPP
#define RMI_HPP

#include <string>
#include "Params.hpp"

namespace Rmi {

class Rmi {
    int sockfd;
    public:
        void connectToServer();
        void disconnect();
        std::string call(int, std::string, int);
        // Probably not the right way to define methods but 
        // doing it just for the understanding

        // Makes the actual function call to the server
        // Arg1 class name
        // Arg2 object reference stringified
        // Arg3 method name '<method name>'
        // Arg4 method arguments list
        void asyncCall(std::string, std::string, int, std::string);
        int intCall(std::string, std::string, int, std::string);
        std::string stringCall(std::string, std::string, int, std::string);
        std::string pack(std::string, std::string, int, std::string);

};

}
#endif //RMI_HPP
