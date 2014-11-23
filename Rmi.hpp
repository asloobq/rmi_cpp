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
        std::string call(int, std::string);
        // Probably not the right way to define methods but 
        // doing it just for the understanding

        // Makes the actual function call to the server
        // Arg1 object reference stringified
        // Arg2 method name '<method name>'
        // Arg3 method param types '<type 1> <type 2>'
        // Arg4 method arguments list
        void asyncCall(std::string, int, std::string, std::string, Params&, std::string);
        int intCall(std::string, int, std::string, std::string, Params&, std::string);
        std::string stringCall(std::string, int, std::string, std::string, Params&, std::string);

};

}
#endif //RMI_HPP
