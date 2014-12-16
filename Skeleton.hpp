#ifndef SKELETON_HPP
#define SKELETON_HPP

#include <string>
#include <vector>
#include <thread>
#include <map>
#include <mutex>

class Skeleton {

    static int mSockfd; //server socket file descriptor

    bool initializeServer();

    protected:
    static std::string mServerName;
    static std::string mPortNo;
    static int mPortNoInt;

    public:
        // key is name of the interface
        // value if of derived type (any which extends from Skeleton)
        static std::map<std::string, Skeleton*> sSkelMap;
        static std::mutex mMapMutex;

        std::string getObjectReference() const;
        void startServer();
        void stopServer();
        virtual ~Skeleton() { stopServer(); }
        virtual void callMethod(std::string, int, std::vector<char>, int&, std::string&);
        virtual int getReturnType(int);
        std::thread& getServerInstance();

        Skeleton() {
            //insert a dummy object 
            sSkelMap.insert(std::make_pair<std::string, Skeleton*>("Skeleton", NULL));
        }
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
