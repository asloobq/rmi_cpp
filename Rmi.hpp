#ifndef RMI_HPP
#define RMI_HPP

#include "Params.hpp"

class Rmi {

    public:
        void connect();
        void disconnect();
        // Probably not the right way to define methods but 
        // doing it just for the understanding
        void asyncCall(Params&);
        int intCall(Params&);
        std::string stringCall(Params&);

};

#endif //RMI_HPP
