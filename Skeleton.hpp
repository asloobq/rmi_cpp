#ifndef SKELETON_HPP
#define SKELETON_HPP

#include <string>
#include <vector>
#include <thread>

class Skeleton {

    std::thread *mServerThread;
    public:

        std::string getObjectReference() const;
        void startServer();
        void stopServer();
        virtual ~Skeleton() { stopServer(); }
        virtual void callIntMethod(std::string, int, std::vector<char>, int&, std::string&);
        virtual int getReturnType(int);
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
