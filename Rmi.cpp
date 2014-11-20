#include "Rmi.hpp"
#include <iostream>

namespace Rmi {

void Rmi::connect() { std::cout<<"\n Connected \n";}

void Rmi::disconnect() { std::cout<<"\n Disconnected \n";}

void Rmi::asyncCall(std::string objRefIn, std::string functionSignIn, Params& paramsListIn) { 
    std::cout<<"\n \n";
}

int Rmi::intCall(std::string objRefIn, std::string functionSignIn, Params& paramsListI) {
    std::cout<<"\n Rmi::intCall\n";
    return -1;
}

std::string Rmi::stringCall(std::string, std::string, Params&) {
    std::cout<<"\n Rmi::stringCall\n";
    return "";
}

}
