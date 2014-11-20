#include "Rmi.hpp"
#include <iostream>

namespace Rmi {

void Rmi::connect() { std::cout<<"\n Connected \n";}

void Rmi::disconnect() { std::cout<<"\n Disconnected \n";}

void Rmi::asyncCall(std::string objRefIn, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) { 
    std::cout<<"\n asyncCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";
}

int Rmi::intCall(std::string objRefIn, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) {
    std::cout<<"\n Rmi::intCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";
    return -1;
}

std::string Rmi::stringCall(std::string, std::string methodNameIn, std::string methodSignIn, Params& paramsListIn) {
    std::cout<<"\n Rmi::stringCall name = " << methodNameIn.c_str() << " sign = "<< methodSignIn.c_str() <<"\n";
    return "";
}

}
