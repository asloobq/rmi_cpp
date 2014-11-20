#ifndef RMI_HPP
#define RMI_HPP

#include <string>
#include "Params.hpp"

namespace Rmi {

class Rmi {

    public:
        void connect();
        void disconnect();
        // Probably not the right way to define methods but 
        // doing it just for the understanding
        void asyncCall(std::string, std::string, Params&);
        int intCall(std::string, std::string, Params&);
        std::string stringCall(std::string, std::string, Params&);

};

}
#endif //RMI_HPP
