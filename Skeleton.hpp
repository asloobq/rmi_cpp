#ifndef SKELETON_HPP
#define SKELETON_HPP

#include <string>
#include <vector>
#include <thread>
#include <map>

class Skeleton {

    public:
        static std::map<std::string, Skeleton*> sSkelMap;

        std::string getObjectReference() const;
        void startServer();
        void stopServer();
        virtual ~Skeleton() { stopServer(); }
        virtual void callIntMethod(std::string, int, std::vector<char>, int&, std::string&);
        virtual int getReturnType(int);
        std::thread& getServerInstance();

        Skeleton() {
            //insert a dummy object 
            sSkelMap.insert(std::make_pair<std::string, Skeleton*>("asdf", NULL));
        }
        template<typename T>
        std::map<std::string, T*>& getMapInstance() {
            static std::map<std::string, T*> sObjectMap;
            return sObjectMap;
        }

        //std::map<std::string, Skeleton*>&
        //getSkelMapInstance() {
//            static std::map<std::string, Skeleton*> sSkelMap;
        //    return sSkelMap;
        //}

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
