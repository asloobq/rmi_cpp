#include <assert.h>
#include <string>
#include <iostream>
#include "DevInterface_stub.hpp"

using namespace std;



int
main(int argc, char *argv[]) {

    assert(argc == 2);

    DevInterface_stub stub(argv[1]);

    int ec = stub.my_method1(123, "test string", 456);
    cout << "Got " << ec << " back from my_method1." << endl;

    string ec_s = stub.my_method2(12345, 67890);
    cout << "Got " << ec_s << " back from my_method2." << endl;
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
