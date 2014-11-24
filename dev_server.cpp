#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include "DevInterface_skel.hpp"

using namespace std;



class DevInterfaceImpl : public DevInterface {
    public:
        virtual int my_method1(int, const string &, int);
        virtual string my_method2(int, int);
        virtual int my_method3(int p1, const std::vector<int> &p2, int p3);
        virtual void my_method4(int p1, const std::vector<string> &p2, const std::string &p3);
};

int
DevInterfaceImpl::my_method1(int arg1, const string &arg2, int arg3) {
    //Sample implementation for this method
    int result = arg1 + arg3 + arg2.length();
    return result;
}

string
DevInterfaceImpl::my_method2(int arg1, int arg2) {

    std::stringstream result;
    result << "arg1 = " << arg1 << " arg2 = " << arg2;
    std::cout << "\n In my_method2 result = "<<result.str().c_str();
    return result.str();
}

int
DevInterfaceImpl::my_method3(int p1, const std::vector<int> &p2, int p3) {
    int result = 0;
    result += p1;
    result += p3;
    for(auto it = p2.begin(); it != p2.end(); it++) {
        result += (*it);
    }
    std::cout<<endl<< "In my_method3 result = " << result;
    return result;
}

void
DevInterfaceImpl::my_method4(int p1, const std::vector<string> &p2, const std::string &p3) {

    std::cout << endl << "In my_method4 p1="<<p1<<" p3="<<p3.c_str() << ". ";
    for(auto it = p2.begin(); it != p2.end(); it++) {
        std::cout << (*it).c_str() << " ";
    }
    
}

int
main() {

    int ec;

    DevInterfaceImpl server_obj;
    DevInterface_skel skel(&server_obj);

    cout << skel.getObjectReference() << endl;

    // Invocations are handled in sub-threads, so main thread
    // just sleeps forever.  We can't exit, though, since
    // that will blow away the objects on the stack.
    while (true) {
        sleep(1000);
    }
}



/*
 * These lines should go at the end of every source code file
 * to set the indent, tabs, and other options.  Feel free to
 * add other appropriate options here.
 *
 */

/* Local Variables: */
/* c-basic-offset: 4 */
/* indent-tabs-mode: nil */
/* End: */
/* vim: set filetype=cpp tabstop=8 shiftwidth=4 softtabstop=4 tw=79 expandtab: */
