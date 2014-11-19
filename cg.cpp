#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Emitter.hpp"

using namespace std;



struct ToUpper {
    char operator() (char c) const  { return std::toupper(c); }
};

struct Method {
    Method(const string &n, const string &rt, const vector<string> &pts)
     : name(n), return_type(rt), param_types(pts) {}
    string name;
    string return_type;
    vector<string> param_types;
};



void
emit_params(Emitter *, const vector<string> &param_types);



int
main(int argc, char *argv[]) {

    char *ec_s;
    int ec;

    assert(argc == 2);

    string intf_name, uc_intf_name;
    vector<Method> methods;

    /*
     * Read in the interface file.
     */
    {
        ifstream idf(argv[1]);
        assert(idf.good());

        char buf[256];
        idf.getline(buf, sizeof buf);
        assert(idf.good());

        intf_name = buf;
        assert(intf_name.size() <= 100);
        // cout << "interface name: " << intf_name << endl;

        idf.getline(buf, sizeof buf);
        int n_methods = atoi(buf);

        // Read the methods.
        for (int i = 0; i < n_methods; i++) {

            // Return type.
            idf.getline(buf, sizeof buf);
            string return_type(buf);

            // Method name.
            idf.getline(buf, sizeof buf);
            string method_name(buf);

            // Number of params.
            idf.getline(buf, sizeof buf);
            int n_params = atoi(buf);
            // cout << "n params " << n_params << endl;

            vector<string> param_types;
            for (int j = 0; j < n_params; j++) {
                idf.getline(buf, sizeof buf);
                param_types.push_back(buf);
                // cout << "param type: " << buf << endl;
            }

            methods.push_back(Method(method_name, return_type, param_types));
        }

        uc_intf_name.resize(intf_name.size(), ' ');
        // Create the upper case version.
        std::transform (intf_name.begin(), intf_name.end(), uc_intf_name.begin(), ToUpper());
    }

    cout << "interface name: " << intf_name << endl;
    for (size_t i = 0; i < methods.size(); i++) {
        cout << "    method name: " << methods[i].name << endl;
        cout << "        return type: " << methods[i].return_type << endl;
        for (size_t j = 0; j < methods[i].param_types.size(); j++) {
            cout << "        param " << j + 1 << " type: " << methods[i].param_types[j] << endl;
        }
    }

    /*
     * Create the interface header file.
     */

    {
        ofstream file((intf_name + ".hpp").c_str());
        assert(file.good());

	// Putting intf_name as the second argument will cause all occurrences of
	// $$ in the output string to be replaced with the contents of intf_name.
        Emitter emitter(&file, intf_name);

        emitter.emitLine("/*  This file is generated.  DO NOT MODIFY DIRECTLY!  */");
        emitter.emitLine("");
        emitter.emitLineF("#ifndef %s_HPP", uc_intf_name.c_str());
        emitter.emitLineF("#define %s_HPP", uc_intf_name.c_str());
        emitter.emitLine("");
        emitter.emitLine("#include <string>");
        emitter.emitLine("");
        emitter.emitLine("class $$ {");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "public:");
        emitter.increment_indent_level();
	emitter.emitLine("virtual ~$$() {}");
        // Output the methods.
        for (size_t i = 0; i < methods.size(); i++) {
            emitter.emitLineStartF("virtual %s %s(",
             methods[i].return_type == "int" ? "int" : "std::string",
             methods[i].name.c_str());
            emit_params(&emitter, methods[i].param_types);
            emitter.emitLineEnd(") = 0;");
        }
	// The -2 as the first argument causes the indent level to be decremented
	// by 2 before outputting the line.
        emitter.emitLine(-2, "};");
        emitter.emitLine("");
        emitter.emitLine("#endif");
    }

    /*
     * Create the interface source file.
     * You will probably not need to put anything here.
     */

    {
        ofstream file((intf_name + ".cpp").c_str());
        assert(file.good());

        Emitter emitter(&file, intf_name);

        emitter.emitLine("/*  This file is generated.  DO NOT MODIFY DIRECTLY!  */");
        emitter.emitLine("");
        emitter.emitLine("#include \"$$.hpp\"");
    }

    /*
     * Create the stub header file.
     */

    {
        ofstream file((intf_name + "_stub.hpp").c_str());
        assert(file.good());

        Emitter emitter(&file, intf_name);

        emitter.emitLine("/*  This file is generated.  DO NOT MODIFY DIRECTLY!  */");
        emitter.emitLine("");
        emitter.emitLineF("#ifndef %s_STUB_HPP", uc_intf_name.c_str());
        emitter.emitLineF("#define %s_STUB_HPP", uc_intf_name.c_str());
        emitter.emitLine("");
        emitter.emitLine("#include \"$$.hpp\"");
        emitter.emitLine("#include <string>");
        emitter.emitLine("#include \"Rmi.hpp\"");
        emitter.emitLine("");
        emitter.emitLine("class $$_stub : public $$ {");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "std::string mObjRef;");
        emitter.emitLine("Rmi::Rmi *mRmiObj;");
        emitter.emitLine("public:");
        emitter.increment_indent_level();
        emitter.emitLine("explicit $$_stub(const std::string &);");
        // Emit the methods.
        for (size_t i = 0; i < methods.size(); i++) {
            // cout << "    method name: " << methods[i].name << endl;
            emitter.emitLineStartF("virtual %s %s(",
             methods[i].return_type == "int" ? "int" : "std::string",
             methods[i].name.c_str());
            emit_params(&emitter, methods[i].param_types);
            emitter.emitLineEnd(");");
        }

        // Emit the destructor
        emitter.emitLine("virtual ~$$_stub();");

	// The -2 as the first argument causes the indent level to be decremented
	// by 2 before outputting the line.
        emitter.emitLine(-2, "};");
        emitter.emitLine("");
        emitter.emitLine("#endif");
    }

    /*
     * Create the stub source file.
     */

    {
        ofstream file((intf_name + "_stub.cpp").c_str());
        assert(file.good());

        Emitter emitter(&file, intf_name);

        emitter.emitLine("/*  This file is generated.  DO NOT MODIFY DIRECTLY!  */");
        emitter.emitLine("");
        emitter.emitLine("#include \"$$_stub.hpp\"");
        emitter.emitLine("#include <iostream>");
        emitter.emitLine("#include <assert.h>");
        emitter.emitLine("");

        // Emit the constructor.
        emitter.emitLine("$$_stub::$$_stub(const std::string &objRefIn) : mObjRef(objRefIn) {");
        emitter.emitLine("");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1,"mRmiObj = new Rmi::Rmi();");
        emitter.emitLine("mRmiObj->connect();");
	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        emitter.emitLine(-1, "}");

	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        //emitter.emitLine(1,
        // "std::cerr << \"'$$_stub::$$_stub($$ *)' needs to be filled in.\" << std::endl;");
	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        //emitter.emitLine(-1, "}");

        for (size_t i = 0; i < methods.size(); i++) {
            // Put a blank line if not the first method.
            if (i > 0) {
                emitter.emitLine("");
            }
            // cout << "    method name: " << methods[i].name << endl;
            emitter.emitLineStartF("%s $$_stub::%s(",
             methods[i].return_type == "int" ? "int" : "std::string",
             methods[i].name.c_str());
            emit_params(&emitter, methods[i].param_types);
            emitter.emitLineEnd(") {");
            // KEC: emitter.emitLine(1, "assert(false);");
            emitter.emitLine(1, "");
            if (methods[i].return_type == "int") {
                emitter.emitLine("return -1;");
            } else if (methods[i].return_type == "string") {
                emitter.emitLine("return \"\";");
            } else {
                assert(false);
            }
            emitter.emitLine(-1, "}");
        }

        // Emit the destructor
        emitter.emitLine("$$_stub::~$$_stub() {");
        emitter.emitLine("");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "mRmiObj->disconnect();");
	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        emitter.emitLine(-1, "}");

    }

    /*
     * Create the skeleton header file.
     */

    {
        ofstream file((intf_name + "_skel.hpp").c_str());
        assert(file.good());

        Emitter emitter(&file, intf_name);

        emitter.emitLine("/*  This file is generated.  DO NOT MODIFY DIRECTLY!  */");
        emitter.emitLine("");
        emitter.emitLineF("#ifndef %s_SKEL_HPP", uc_intf_name.c_str());
        emitter.emitLineF("#define %s_SKEL_HPP", uc_intf_name.c_str());
        emitter.emitLine("");
        emitter.emitLine("#include \"Skeleton.hpp\"");
        emitter.emitLine("#include \"$$.hpp\"");
        emitter.emitLine("");
        emitter.emitLine("class $$_skel : public Skeleton {");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "$$ *mInterface;");
        emitter.emitLine("public:");
        emitter.increment_indent_level();
        emitter.emitLine("explicit $$_skel($$ *);");
        emitter.emitLine("");
        emitter.emitLine("virtual std::string getObjectReference() const;");
	// The -2 as the first argument causes the indent level to be decremented
	// by 2 before outputting the line.
        emitter.emitLine(-2, "};");
        emitter.emitLine("");
        emitter.emitLine("#endif");
    }

    /*
     * Create the skel source file.
     */

    {
        ofstream file((intf_name + "_skel.cpp").c_str());
        assert(file.good());

        Emitter emitter(&file, intf_name);

        emitter.emitLine("/*  This file is generated.  DO NOT MODIFY DIRECTLY!  */");
        emitter.emitLine("");
        emitter.emitLine("#include \"$$_skel.hpp\"");
        emitter.emitLine("#include <iostream>");
        emitter.emitLine("#include <assert.h>");
        emitter.emitLine("");

        // Emit the constructor.
        emitter.emitLine("$$_skel::$$_skel($$ *interfaceIn) : mInterface(interfaceIn) {}");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        //emitter.emitLine(1,
        // "std::cerr << \"'$$_skel::$$_skel($$ *)' needs to be filled in.\" << std::endl;");
	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        //emitter.emitLine(-1, "}");
        emitter.emitLine("");
        // Emit the getObjectReference method
        emitter.emitLine("std::string $$_skel::getObjectReference() const {");
        // The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "std::string ref = \"myObjRef1\"; //need to generate from object ");
        emitter.emitLine("return ref;");
	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        emitter.emitLine(-1, "}");
    }
}

void
emit_params(Emitter *emitter, const vector<string> &param_types) {
    for (size_t i = 0; i < param_types.size(); i++) {
        if (i > 0) {
            emitter->emitString(", ");
        }
        if (param_types[i] == "int") {
            emitter->emitString("int ");
        } else if (param_types[i] == "string") {
            emitter->emitString("const std::string &");
        } else {
            assert(false);
        }
        emitter->emitStringF("p%d", int(i) + 1);
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
