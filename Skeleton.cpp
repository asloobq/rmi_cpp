#include "Skeleton.hpp"
#include <assert.h>
#include <iostream>

using namespace std;



string
Skeleton::getObjectReference() const {
    cerr << "'Skeleton::getObjectReference() const' needs to be filled in." << endl;
    return "an_object_ref";
}

void
Skeleton::startServer() {
    mServer = new Rmi::RmiServer();
    mServer->startServer();
}

void
Skeleton::stopServer() {
    if(mServer) {
        mServer->stopServer();
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
