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

std::string
createMethodSignature(Method &method);

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
        emitter.emitLine("#include <vector>");
        emitter.emitLine("");
        emitter.emitLine("class $$ {");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "public:");
        emitter.increment_indent_level();
	emitter.emitLine("virtual ~$$();");
        // Output the methods.
        for (size_t i = 0; i < methods.size(); i++) {
            emitter.emitLineStartF("virtual %s %s(",
             methods[i].return_type == "int" ? "int" : methods[i].return_type == "async" ? "void" : "std::string",
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
        emitter.emitLine("");
        emitter.emitLine("$$::~$$() {}"); 
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
        emitter.emitLine("std::string mTypeName;");
        emitter.emitLine("public:");
        emitter.increment_indent_level();
        emitter.emitLine("explicit $$_stub(const std::string &);");
        // Emit the methods.
        for (size_t i = 0; i < methods.size(); i++) {
            emitter.emitLineStartF("virtual %s %s(",
             methods[i].return_type == "int" ? "int" : methods[i].return_type == "async" ? "void" : "std::string",
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
        emitter.emitLine("#include <cassert>");
        emitter.emitLine("#include <cstring>");
        emitter.emitLine("");

        // Emit the constructor.
        emitter.emitLine("$$_stub::$$_stub(const std::string &objRefIn) : mObjRef(objRefIn) {");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "mRmiObj = new Rmi::Rmi();");
        emitter.emitLine("mTypeName = std::string(\"$$\");");
       	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        emitter.emitLine(-1, "}");

        for (size_t i = 0; i < methods.size(); i++) {
            // Put a blank line if not the first method.
            if (i > 0) {
                emitter.emitLine("");
            }
            emitter.emitLineStartF("%s $$_stub::%s(",
             methods[i].return_type == "int" ? "int" : methods[i].return_type == "async" ? "void" : "std::string",
             methods[i].name.c_str());
            emit_params(&emitter, methods[i].param_types);
            emitter.emitLineEnd(") {");
            // KEC: emitter.emitLine(1, "assert(false);");
            //Emit method body
            emitter.increment_indent_level();
            //Marshall the arguments
            emitter.emitLine("char *buf = new char[10000];"); //should be dynamic
            emitter.emitLine("char *bufPtr = buf;");
            emitter.emitLine("size_t len;");
            int paramNo = 1;
            for(std::vector<string>::iterator it = methods[i].param_types.begin();
                it != methods[i].param_types.end(); it++, paramNo++) {
                if((*it).compare("int") == 0) {
                    emitter.emitLineF("memcpy(bufPtr, &p%d, sizeof(p%d));", paramNo, paramNo);
                    emitter.emitLineF("bufPtr += sizeof(p%d);", paramNo);
                    emitter.emitLine("");
                } else if((*it).compare("string") == 0) {
                    //copy length of the string
                    emitter.emitLineF("len = p%d.length();", paramNo);
                    emitter.emitLine("memcpy(bufPtr, &len, sizeof(len));");
                    emitter.emitLine("bufPtr += sizeof(len);");
                    //copy string itself
                    emitter.emitLineF("memcpy(bufPtr, p%d.c_str(), len);", paramNo);
                    emitter.emitLineF("bufPtr += len;", paramNo);
                    emitter.emitLine("");
                } else if((*it).compare("array of int") == 0) {
                    //put the array length
                    emitter.emitLineF("len = p%d.size();", paramNo);
                    emitter.emitLine("memcpy(bufPtr, &len, sizeof(len));");
                    emitter.emitLine("bufPtr += sizeof(len);");
                    //copy elements one by one
                    emitter.emitLine("for(int i = 0; i < len; i++) {");
                    emitter.emitLineF(1, "memcpy(bufPtr, &p%d[i], sizeof(int));", paramNo);
                    emitter.emitLine("bufPtr += sizeof(int);");
                    emitter.emitLine(-1, "}");
                    emitter.emitLine("");
                } else if((*it).compare("array of string") == 0) {
                    //put the array length
                    emitter.emitLineF("len = p%d.size();", paramNo);
                    emitter.emitLine("memcpy(bufPtr, &len, sizeof(len));");
                    emitter.emitLine("bufPtr += sizeof(len);");
                    //copy elements one by one
                    emitter.emitLine("size_t elemSize;");
                    emitter.emitLine("for(int i = 0; i < len; i++) {");
                    //put length of current string
                    emitter.emitLineF(1, "elemSize = p%d.at(i).length();", paramNo);
                    emitter.emitLine("memcpy(bufPtr, &elemSize, sizeof(elemSize));");
                    emitter.emitLine("bufPtr += sizeof(elemSize);");
                    //put the actual string
                    emitter.emitLineF("memcpy(bufPtr, p%d.at(i).c_str(), elemSize);", paramNo);
                    emitter.emitLine("bufPtr += elemSize;");
                    emitter.emitLine(-1, "}");
                    emitter.emitLine("");
                } else {
                    assert(false);
                }
            }

            emitter.emitLine("std::string bufStr(buf, bufPtr - buf);");
            emitter.emitLine("delete[] buf;");

            std::string sign = createMethodSignature(methods[i]);
            if (methods[i].return_type == "int") {
                emitter.emitLine("int result = 0;");
                emitter.emitLineF("result = mRmiObj->intCall(mTypeName, mObjRef, %d, bufStr);", i);
                emitter.emitLine("return result;");
            } else if (methods[i].return_type == "string") {
                emitter.emitLine("std::string result;");
                emitter.emitLineF("result = mRmiObj->stringCall(mTypeName, mObjRef, %d, bufStr);", i);
                emitter.emitLine("return result;");
            } else {
                emitter.emitLineF("mRmiObj->asyncCall(mTypeName, mObjRef, %d, bufStr);", i);
            }
            emitter.emitLine(-1, "}");
        }

        // Emit the destructor
        emitter.emitLine("");
        emitter.emitLine("$$_stub::~$$_stub() {");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "delete mRmiObj;");
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
        emitter.emitLine(1, "$$* mInterface;");
        emitter.emitLine("static std::map<std::string, $$*> sObjectMap;");
        emitter.emitLine("public:");
        emitter.increment_indent_level();
        emitter.emitLine("explicit $$_skel($$ *);");
        emitter.emitLine("");
        emitter.emitLine("virtual std::string getObjectReference() const;");
        emitter.emitLine("void callMethod(std::string, int, std::vector<char>, int&, std::string&);");
        emitter.emitLine("int getReturnType(int);");
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
        emitter.emitLine("#include <cassert>");
        emitter.emitLine("#include <cstring>");
        emitter.emitLine("#include <sstream>");
        emitter.emitLine("#include <string>");
        emitter.emitLine("#include <utility>");
        emitter.emitLine("");
        emitter.emitLine("std::map<std::string, $$*> $$_skel::sObjectMap;");
        emitter.emitLine("");
        // Emit the constructor.
        emitter.emitLine("$$_skel::$$_skel($$ *interfaceIn) : mInterface(interfaceIn) {");
	// The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.increment_indent_level();
        emitter.emitLine("std::stringstream ss;");
        emitter.emitLine("ss << mInterface;");
        emitter.emitLine("std::pair<std::string, $$*> pair = std::make_pair(ss.str(), mInterface);");
        emitter.emitLine("sObjectMap.insert(pair);");
        emitter.emitLine("std::cout<<\"Inserting object \"<<ss.str().c_str()<< \" in map \" << &sObjectMap << std::endl;");
        emitter.emitLine("size_t isPresent = Skeleton::sSkelMap.count(\"$$_skel\");");
        emitter.emitLine("if(isPresent == 0) {");
        emitter.emitLine(1, "Skeleton::sSkelMap.insert(std::make_pair<std::string, Skeleton*>(\"$$\", this));");
        emitter.emitLine("std::cout<<\"Inserting string $$ in map \" << &(Skeleton::sSkelMap) << std::endl;");
        emitter.emitLine(-1,"}");
        emitter.emitLine("startServer();");
	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        emitter.emitLine(-1, "}");
        emitter.emitLine("");
        // Emit the getObjectReference method
        emitter.emitLine("std::string $$_skel::getObjectReference() const {");
        // The 1 as the first argument causes the indent level to be incremented
	// before outputting the line.
        emitter.emitLine(1, "std::stringstream ss;");
        emitter.emitLine("ss << mInterface;");
        emitter.emitLine("return ss.str();");
	// The -1 as the first argument causes the indent level to be decremented
	// before outputting the line.
        emitter.emitLine(-1, "}");
        emitter.emitLine("");
        emitter.emitLine("void $$_skel::callMethod(std::string objRefIn, int methodIdIn, std::vector<char> dataIn,");
        emitter.emitLine("                           int &resInt, std::string &resStr) {");
        emitter.emitLine(1, "");
        emitter.emitLine("int result = -1;");
        emitter.emitLine("char *buf = dataIn.data();");
        emitter.emitLine("char *bufPtr = buf;");
        emitter.emitLine("size_t len;");
        emitter.emitLine("size_t totalLen;");
        emitter.emitLine("memcpy(&totalLen, bufPtr, sizeof (totalLen));");
        emitter.emitLine("bufPtr += sizeof (totalLen);");
        emitter.emitLine("std::cout<<std::endl<<\" total length of packet = \"<<totalLen;");
        //get correct object
        emitter.emitLine("std::cout << \"map.size() = \" << sObjectMap.size() << std::endl;");
        emitter.emitLine("$$* objRef;");
        emitter.emitLine("try {");
        emitter.emitLine(1, "objRef = sObjectMap.at(objRefIn);");
        emitter.emitLine(-1, "} catch (std::exception &ex) {");
        emitter.emitLine(1, "std::cout << \"Could not find object \" << objRef << __FILE__ << \":\"<<__LINE__ << std::endl;");
        emitter.emitLine("return;");
        emitter.emitLine(-1, "}");
        emitter.emitLine("switch(methodIdIn) {");
        emitter.increment_indent_level();
        for (size_t i = 0; i < methods.size(); i++) {
            emitter.emitLineF("case %d: std::cout<<\"\\nCalling method %s\";", i, methods[i].name.c_str());
            emitter.emitLine(1, "{");
            //unmarshall the arguments
            int paramNo = 1;
            for(std::vector<string>::iterator it = methods[i].param_types.begin();
                it != methods[i].param_types.end(); it++, paramNo++) {
                if((*it).compare("int") == 0) {
                    emitter.emitLineF("int p%d;", paramNo);
                    emitter.emitLineF("memcpy(&p%d, bufPtr, sizeof (int));", paramNo);
                    emitter.emitLine("bufPtr += sizeof (int);");
                    emitter.emitLineF("std::cout<<std::endl<<\"p%d = \"<<p%d;", paramNo, paramNo);
                    emitter.emitLine("");
                } else if((*it).compare("string") == 0) {
                    //read length of string
                    emitter.emitLine("memcpy(&len, bufPtr, sizeof (len));");
                    emitter.emitLine("bufPtr += sizeof (len);");
                    emitter.emitLine("std::cout<<std::endl<<\"length of string = \"<<len;");
                    emitter.emitLineF("std::string p%d(bufPtr, len);", paramNo);
                    emitter.emitLine("bufPtr += len;");
                    emitter.emitLineF("std::cout<<std::endl<<\"p%d = \"<<p%d.c_str();", paramNo, paramNo);
                    emitter.emitLine("");
                } else if((*it).compare("array of int") == 0) {
                    emitter.emitLineF("std::vector<int> p%d;", paramNo);
                    //read the number of elements
                    emitter.emitLine("memcpy(&len, bufPtr, sizeof (len));");
                    emitter.emitLine("bufPtr += sizeof(len);");
                    //copy elements one by one
                    emitter.emitLine("int elem;"); //Note : This has to be an int
                    emitter.emitLine("for(int i = 0; i < len; i++) {");
                    //read elements one by one
                    emitter.emitLine(1, "memcpy(&elem, bufPtr, sizeof(elem));");
                    emitter.emitLineF("p%d.push_back(elem);", paramNo);
                    emitter.emitLine("bufPtr += sizeof(elem);");
                    emitter.emitLine(-1, "}");
                    emitter.emitLine("");
                } else if((*it).compare("array of string") == 0) {
                    emitter.emitLineF("std::vector<std::string> p%d;", paramNo);
                    //read the number of elements
                    emitter.emitLine("memcpy(&len, bufPtr, sizeof (len));");
                    emitter.emitLine("bufPtr += sizeof(len);");
                    //copy elements one by one
                    emitter.emitLine("size_t elemSize;");
                    emitter.emitLine("for(int i = 0; i < len; i++) {");
                    //read the size of the element
                    emitter.emitLine(1, "memcpy(&elemSize, bufPtr, sizeof(elemSize));");
                    emitter.emitLine("bufPtr += sizeof(elemSize);");
                    //read the actual string
                    emitter.emitLineF("p%d.push_back(std::string(bufPtr, elemSize));", paramNo);
                    emitter.emitLine("bufPtr += elemSize;");
                    emitter.emitLine(-1, "}");
                    emitter.emitLine("");
                }
            }

            //call the method using the arguments created
            if(methods[i].return_type.compare("int") == 0) {
                emitter.emitStringF("resInt = objRef->%s(", methods[i].name.c_str());
            } else if(methods[i].return_type.compare("string") == 0) {
                emitter.emitStringF("std::string res = objRef->%s(", methods[i].name.c_str());
            }else {
                emitter.emitStringF("objRef->%s(", methods[i].name.c_str());
            }

            if(methods[i].param_types.size() > 0) {
                size_t arg = 1;
                emitter.emitStringF("p%d", arg++);
                for(; arg < paramNo; arg++) {
                    emitter.emitStringF(", p%d", arg);
                }
            }
            emitter.emitString(");");
            if(methods[i].return_type.compare("string") == 0) {
                emitter.emitString("resStr.append(res);");
            }
            emitter.emitLine(-1, "}");
            emitter.emitLine("break;");
        }
        emitter.emitLine("default: assert(false);");
        emitter.decrement_indent_level();
        emitter.emitLine("}");
        emitter.emitLine(-1, "}");

        //int getReturnType
        emitter.emitLine("");
        emitter.emitLine("int $$_skel::getReturnType(int methodIdIn) {");
        emitter.emitLine(1, "");
        emitter.emitLine("switch(methodIdIn) {");
        emitter.increment_indent_level();
        for (size_t i = 0; i < methods.size(); i++) {
            emitter.emitLineF("case %d: ", i);
            if(methods[i].return_type == "async") {
                emitter.emitLine("return 0;");
            } else if(methods[i].return_type == "int") {
                emitter.emitLine("return 1;");
            } else if(methods[i].return_type == "string") {
                emitter.emitLine("return 2;");
            }
        }
        emitter.emitLine("default: assert(false);");
        emitter.decrement_indent_level();
        emitter.emitLine("}");
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
        } else if (param_types[i] == "array of int") {
            emitter->emitString("const std::vector<int> &");
        } else if (param_types[i] == "array of string") {
            emitter->emitString("const std::vector<std::string> &");
        } else {
            std::cout<<"\n emit_params = \""<<param_types[i]<<"\" i = "<<i<<std::endl;
            assert(false);
        }
        emitter->emitStringF("p%d", int(i) + 1);
    }
}

/*
  Creates the method signature which will be used to uniquely
  identify the method by the server.
  <method name> <type1> <type2> ...
*/
std::string
createMethodSignature(Method& method) {
    
    std::string sign("");
    for(std::vector<std::string>::iterator it = method.param_types.begin(); it != method.param_types.end(); ++it) {
        sign = sign + *it;
        sign = sign + " ";
    }

    return sign;
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
