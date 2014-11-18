#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "EvalInterfaceBasic_skel.hpp"
#include "EvalInterfaceManyMethods_skel.hpp"
#include "EvalInterfaceConcurrency_skel.hpp"

using namespace std;



/*
 * EvalInterfaceBasic
 */

class EvalInterfaceBasicImpl : public EvalInterfaceBasic {
    public:
        virtual int method1(int, const string &, int);
        virtual string method2(int, int);
};

int
EvalInterfaceBasicImpl::method1(int i1, const string &s, int i2) {
    return i1 + s.size() + i2;
}

string
EvalInterfaceBasicImpl::method2(int i1, int i2) {
    char buf[50];
    sprintf(buf, "%d:%d", i1, i2);
    return buf;
}

/*
 * EvalInterfaceManyMethods
 */

class EvalInterfaceManyMethodsImpl : public EvalInterfaceManyMethods {
    public:
        #include "method_decls"
};

#include "method_defs"

/*
 * EvalInterfaceConcurrency
 */

class EvalInterfaceConcurrencyImpl : public EvalInterfaceConcurrency {
    public:
        virtual int method1();
};


int
EvalInterfaceConcurrencyImpl::method1() {

    int ec;

    timespec req, rem;

    // Sleep for 1 millisec.
    req.tv_sec = 0;
    req.tv_nsec = 1000000;

    while (true) {
        ec = nanosleep(&req, &rem);
        if (ec != 0) {
            int en = errno;
            if (en == EINTR) {
                req = rem;
                continue;
            }
            assert(false);
        }
        break;
    }

    return 1234;
}



void wait_for_invocations();



int
main(int argc, char *argv[]) {

    string pipe_name;
    int ec;

    if (!(argc >= 2)) {
        fprintf(stderr, "Usage: eval_server <test_name>\n");
        fprintf(stderr, "    The <test_name> argument should be one of:\n");
        fprintf(stderr, "        basic\n");
        fprintf(stderr, "        many_methods\n");
        fprintf(stderr, "        many_objects\n");
        fprintf(stderr, "        concurrency\n");
        fprintf(stderr, "    The 'many_objects' test requires an additional argument\n");
        fprintf(stderr, "    indicating the number of objects:\n");
        fprintf(stderr, "        eval_server many_objects <number_of_objects>\n");
        fprintf(stderr, "    In all cases, the arguments given the eval_server and to eval_client\n");
        fprintf(stderr, "    must match each other.\n");
        exit(1);
    }

    // Create FIFO for communicating the object refs.
    {
        // Create name.
        char buf[100];
        sprintf(buf, "/tmp/or_pipe-%s", getlogin());
        ec = mknod(buf, 0600 | S_IFIFO, 0);
        if (ec != 0) {
            int en = errno;
            assert(en == EEXIST);
        }
        pipe_name = buf;
    }

    ofstream out(pipe_name.c_str());

    if (string(argv[1]) == "basic") {

        EvalInterfaceBasicImpl server_obj;
        EvalInterfaceBasic_skel skel(&server_obj);

        out << skel.getObjectReference() << endl;
        out.close();

        wait_for_invocations();

    } else if (string(argv[1]) == "many_methods") {

        EvalInterfaceManyMethodsImpl server_obj;
        EvalInterfaceManyMethods_skel skel(&server_obj);

        out << skel.getObjectReference() << endl;
        out.close();

        wait_for_invocations();

    } else if (string(argv[1]) == "many_objects") {

        if (argc != 3) {
            fprintf(stderr, "The 'many_objects' test requires an additional argument\n");
            fprintf(stderr, "indicating the number of objects:\n");
            fprintf(stderr, "    eval_server many_objects <number_of_objects>\n");
            fprintf(stderr, "The arguments given the eval_server and to eval_client\n");
            fprintf(stderr, "must match each other.\n");
            exit(1);
        }
        
        int n = atoi(argv[2]);

        vector<EvalInterfaceBasic_skel *> obj1_list;
        vector<EvalInterfaceManyMethods_skel *> obj3_list;

        for (int i = 0; i < n/2; i++) {
            EvalInterfaceBasic_skel *s1 = new EvalInterfaceBasic_skel(new EvalInterfaceBasicImpl);
            EvalInterfaceManyMethods_skel *s3 = new EvalInterfaceManyMethods_skel(new EvalInterfaceManyMethodsImpl);
            out << s1->getObjectReference() << endl;
            out << s3->getObjectReference() << endl;
            obj1_list.push_back(s1);
            obj3_list.push_back(s3);
        }

        out.close();

        wait_for_invocations();

    } else if (string(argv[1]) == "concurrency") {

        EvalInterfaceConcurrencyImpl server_obj;
        EvalInterfaceConcurrency_skel skel(&server_obj);

        out << skel.getObjectReference() << endl;
        out.close();

        wait_for_invocations();

    } else {

        assert(false);
    }
}

void
wait_for_invocations() {
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
