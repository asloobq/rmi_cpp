#include "Rmi.hpp"

void Rmi::connect() { std::cout<<"\n Connected \n";}

void Rmi::disconnect() { std::cout<<"\n Disconnected \n";}

void Rmi::asyncCall(Params&) {}

int Rmi::intCall(Params&) {}
