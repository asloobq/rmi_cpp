#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include "DevInterface_skel.hpp"

using namespace std;



class DevInterfaceImpl : public DevInterface {
    public:
        virtual int my_method1(int, const string &, int);
        virtual string my_method2(int, int);
};

int
DevInterfaceImpl::my_method1(int arg1, const string &arg2, int arg3) {
    //Sample implementation for this method
    int result = arg1 + arg3 + arg2.length();
    return result;
}

string
DevInterfaceImpl::my_method2(int arg1, int arg2) {
    std::string result = "arg1 =  " + arg1;
    result = result + " arg2 = ";
    result.push_back(arg2);
    return result;
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
