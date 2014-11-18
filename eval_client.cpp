#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "EvalInterfaceBasic_stub.hpp"
#include "EvalInterfaceManyMethods_stub.hpp"
#include "EvalInterfaceConcurrency_stub.hpp"

#define check_response(x) assert(x)



using namespace std;



struct ConcurrencyTestInfo {
    ConcurrencyTestInfo(const string &r, int i)
     : ref(r), its(i) {}
    const string ref;
    const int its;
};



vector<string> get_refs();
extern "C" void * run_concurrency_test(void *vp);



int
main(int argc, char *argv[]) {

    int ec;

    timeval start_tv, stop_tv;
    int methods_per_iteration = 1;

    if (!(argc >= 3)) {
        fprintf(stderr, "Usage: eval_client <test_name> <iterations>\n");
        fprintf(stderr, "    The <test_name> argument should be one of:\n");
        fprintf(stderr, "        basic\n");
        fprintf(stderr, "        many_methods\n");
        fprintf(stderr, "        many_objects\n");
        fprintf(stderr, "        concurrency\n");
        fprintf(stderr, "    The 'many_objects' test requires an additional argument\n");
        fprintf(stderr, "    indicating the number of objects:\n");
        fprintf(stderr, "        eval_client many_objects <iterations> <number_of_objects>\n");
        fprintf(stderr, "    In all cases, the arguments given the eval_server and to eval_client\n");
        fprintf(stderr, "    must match each other.\n");
        exit(1);
    }

    int its = atoi(argv[2]);
    if (its < 1 || its > 1000000000) {
        fprintf(stderr, "The number of iterations, %d, must be between 1 and 1,000,000,000.\n", its);
        exit(1);
    }

    /*
     * Test a single call to a function.
     */
    if (string(argv[1]) == "basic") {

        printf("Running %d iterations of the basic test.\n", its);

        vector<string> obj_refs = get_refs();
        assert(obj_refs.size() == 1);
        EvalInterfaceBasic_stub stub(obj_refs[0]);

        ec = gettimeofday(&start_tv, 0);
        assert(ec == 0);

        for (int i = 0; i < its; i++) {
            int n = stub.method1(i, "test string", i/2);
            int c = i + 11 + i/2;
            check_response(n == c);
            string s = stub.method2(i/3, 2*i);
            char buf[50];
            sprintf(buf, "%d:%d", i/3, 2*i);
            check_response(string(buf) == s);
        }

        ec = gettimeofday(&stop_tv, 0);
        assert(ec == 0);

        methods_per_iteration = 2;

    /*
     * Test lots of methods.
     */
    } else if (string(argv[1]) == "many_methods") {

        vector<string> obj_refs = get_refs();
        assert(obj_refs.size() == 1);
        EvalInterfaceManyMethods_stub stub(obj_refs[0]);

        ec = gettimeofday(&start_tv, 0);
        assert(ec == 0);

        for (int i = 0; i < its; i++) {
            #include "calls_cpp"
        }

        ec = gettimeofday(&stop_tv, 0);
        assert(ec == 0);

        methods_per_iteration = 100;

    /*
     * Test lots of objects.
     */
    } else if (string(argv[1]) == "many_objects") {

        if (argc != 4) {
            fprintf(stderr, "The 'many_objects' test requires an additional argument\n");
            fprintf(stderr, "indicating the number of objects:\n");
            fprintf(stderr, "    eval_client many_objects <iterations> <number_of_objects>\n");
            fprintf(stderr, "The arguments given the eval_server and to eval_client\n");
            fprintf(stderr, "must match each other.\n");
            exit(1);
        }

        int n2 = atoi(argv[3])/2;

        // Create name.
        char buf[100];
        sprintf(buf, "/tmp/or_pipe-%s", getlogin());

        vector<EvalInterfaceBasic_stub *> obj_list1;
        vector<EvalInterfaceManyMethods_stub *> obj_list2;

        {
            // Open refs stream.
            ifstream in(buf);

            for (int i = 0; i < n2; i++) {

                string ref1, ref2;

                assert(in.good()); getline(in, ref1);
                assert(in.good()); getline(in, ref2);
                EvalInterfaceBasic_stub *s1 = new EvalInterfaceBasic_stub(ref1);
                EvalInterfaceManyMethods_stub *s2 = new EvalInterfaceManyMethods_stub(ref2);
                obj_list1.push_back(s1);
                obj_list2.push_back(s2);
            }
        }

        ec = gettimeofday(&start_tv, 0);
        assert(ec == 0);

        for (int i = 0; i < n2; i++) {
            for (int j = 0; j < its; j++) {

                int n = obj_list1[i]->method1(i, "hello", i/2);
                int c = i + 5 + i/2;
                check_response(n == c);

                n = obj_list2[i]->method10(i);
                check_response(n == i);
            }
        }

        ec = gettimeofday(&stop_tv, 0);
        assert(ec == 0);

        methods_per_iteration = 2*n2;

    /*
     * Test concurrent invocations on the server.
     *     Mainly to test performance.  Doesn't really stress test.
     */
    } else if (string(argv[1]) == "concurrency") {

        vector<string> obj_refs = get_refs();
        assert(obj_refs.size() == 1);

        ConcurrencyTestInfo info(obj_refs[0], its);
        pthread_t tid[10];

        ec = gettimeofday(&start_tv, 0);
        assert(ec == 0);

        for (int i = 0; i < 10; i++) {
            ec = pthread_create(&tid[i], 0, run_concurrency_test, &info);
            assert(ec == 0);
        }

        for (int i = 0; i < 10; i++) {
            void *vp;
            ec = pthread_join(tid[i], &vp);
            assert(ec == 0);
        }

        ec = gettimeofday(&stop_tv, 0);
        assert(ec == 0);

        methods_per_iteration = 10;


    } else {
        assert(false);
    }

    double dt =
     (stop_tv.tv_sec + stop_tv.tv_usec/1000000.0)
     - (start_tv.tv_sec + start_tv.tv_usec/1000000.0);
    printf("Elapsed time: %.6f; rate: %g iterations per second, %g methods per second.\n",
     dt, its/dt, (its*methods_per_iteration)/dt);
}

void *
run_concurrency_test(void *vp) {

    ConcurrencyTestInfo *info = (ConcurrencyTestInfo *) vp;

    EvalInterfaceConcurrency_stub obj(info->ref);
    
    for (int i = 0; i < info->its; i++) {
        obj.method1();
    }

    return 0;
}

vector<string>
get_refs() {
    // Create name.
    char buf[100];
    sprintf(buf, "/tmp/or_pipe-%s", getlogin());
    ifstream in(buf);
    vector<string> obj_refs;
    string ref;
    while (getline(in, ref), in.good()) {
        obj_refs.push_back(ref);
    }
    /*
    for (size_t i = 0; i < obj_refs.size(); i++) {
        cout << "ref " << i << ": " << obj_refs[i] << endl;
    }
    */
    return obj_refs;
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
