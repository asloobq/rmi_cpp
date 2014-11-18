Evaluation Application
----------------------

There are three different object interfaces.

    EvalInterfaceBasic

        A simple interface.

    EvalInterfaceManyMethods

        Has one hundred methods in the interface.

    EvalInterfaceConcurrency

        Has one method, but the implementation
        on the server side will block for a
        short period of time, to simulate
        a more involved server-side operation.

There are four tests.

    basic

        Tests the speed of basic calls, using
        the EvalInterfaceBasic interface.

    many_methods

        Tests the speed when there are many methods
        in the interface, using the EvalInterfaceManyMethods
        interface.

    many_objects

        Tests the speed when there are many objects
        on the server.  One half of the objects
        are created using EvalInterfaceBasic, and
        the other half are created using
        EvalInterfaceManyMethods.

    concurrency

        Tests the speed when there are concurrent
        calls to the server side object.
        Uses EvalInterfaceConcurrency.

To run the evaluation test, run the server with

    ./eval_server <test_type>

and the client with

    ./eval_client <test_type> <iterations>

where <test_type> is one of the test types, and <iterations>
is the number of iterations to perform.  For the many_objects
test, an additional argument is required to indicate the number
of objects.

    ./eval_server many_objects <number_of_objects>
    ./eval_client many_objects <iterations> <number_of_objects>

It doesn't matter whether you execute the client first or the
server.
