#ifndef SKELETON_HPP
#define SKELETON_HPP



#include <string>
#include "RmiServer.hpp"


class Skeleton {

    Rmi::RmiServer *mServer;

    public:

        std::string getObjectReference() const;
        void startServer();
        void stopServer();
        virtual ~Skeleton() { stopServer(); }
};



#endif



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
