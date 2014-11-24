#include <assert.h>
#include <string>
#include <iostream>
#include <vector>
#include "DevInterface_stub.hpp"

using namespace std;



int
main(int argc, char *argv[]) {

    assert(argc == 2);

    DevInterface_stub stub(argv[1]);

    int ec = stub.my_method1(123, "test string", 456);
    cout << "Got " << ec << " back from my_method1." << endl;
    assert(ec == 590);

    string ec_s = stub.my_method2(12345, 67890);
    cout << "Got " << ec_s << " back from my_method2." << endl;
    assert(ec_s == "arg1 = 12345 arg2 = 67890");

    vector<int> arrayInts;
    for(int i = 100; i < 1000; i += 100) {
        arrayInts.push_back(i);
    }

    int result = stub.my_method3(99, arrayInts, 1000);
    cout << "Got " << result << " back from my_method3." << endl;
    assert(result == 5599);

    vector<string> arrayStrings;
    arrayStrings.push_back("one");
    arrayStrings.push_back("two");
    arrayStrings.push_back("three");
    arrayStrings.push_back("four");

    stub.my_method4(999, arrayStrings, "numbers");
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
