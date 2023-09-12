/*! \file
    \brief Squirrel test
*/

#include "umba/umba.h"
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <random>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <functional>


#include <simplesquirrel/simplesquirrel.hpp>

#ifdef _MSC_VER
#pragma comment(lib, "_squirrel.lib")
#pragma comment(lib, "_simplesquirrel.lib")
#endif


#define TEST_CLASS_BIND

const wchar_t *sqSrc1 = L"\
local arr = [ 1234, \"Test\" ];\n\
\n\
function set(val) {\n\
    arr = val;\n\
}\n\
\n\
function get() {\n\
    return arr;\n\
}\n\
\n\
function printContents(){\n\
    for(local i = 0; i < arr.len(); i++){\n\
\n\
    }\n\
    print(\"myCppFunc: \" + myCppFunc(1,2));\n\
    print(\"index 0: \" + arr[0]);\n\
    print(\"index 1: \" + arr[1]);\n\
    //print(\"index 2: \" + arr[2]);\n\
}\n\
\n\
\n\
"
#if defined(TEST_CLASS_BIND)
L"\n\
function classHandler(cppClass){\n\
    print(\"cppClass.getMessage: \" + cppClass.getMessage());\n\
    cppClass.setMessage(\"New Msg\");\n\
    print(\"cppClass.getMessage: \" + cppClass.getMessage());\n\
    print(\"cppClass.getMessageLowerCase: \" + cppClass.getMessageLowerCase());\n\
}\n\
\n\
\n\
"
#endif
;

ssq::sqstring myCppFunc(int a, int b)
{
    return ssq::to_sqstring(a + b);
}



#if defined(TEST_CLASS_BIND)
class CppClass {
public:
    CppClass(const ssq::sqstring& msg):message(msg) {
        
    }

    CppClass():message() {
    }

    void setMessage(const ssq::sqstring& msg) {
        message = msg;
    }

    ssq::sqstring getMessage() /* const */  {
        return message;
    }


    static ssq::Class expose(ssq::VM& vm) {
        // auto cls = vm.addClass("CppClass", ssq::Class::Ctor<CppClass(std::string, int)>());
        // Alternative approach using lambda:
        auto cls = vm.addClass( _SC("CppClass")
                              , []() -> CppClass*
                                {
                                    return new CppClass();
                                }
                              , false // dont release
                              );

        // Не работает
        cls.addFunc(_SC("setMessage"), &CppClass::setMessage);
        cls.addFunc(_SC("getMessage"), &CppClass::getMessage);

        // Это тоже - тут какая-то рекурсия возникает
        cls.addFunc(_SC("getMessageLowerCase"), [](CppClass* self) -> ssq::sqstring {
            ssq::sqstring data = self->message;
            //std::transform(data.begin(), data.end(), data.begin(), ::tolower);
            return _SC("DATA"); // data;
        });


        // Сопли от примеров симпсквирела
        // cls.addFunc("getInteger", &CppClass::getInteger);
        // cls.addFunc("getMessageLowerCase", [](CppClass* self) -> std::string {
        //     std::string data = self->message;
        //     std::transform(data.begin(), data.end(), data.begin(), ::tolower);
        //     return data;
        // });



        cls.addVar(_SC("message"), &CppClass::message);
        // cls.addConstVar("integer", &CppClass::integer);

        return cls;
    }

    // We will leave them public so Squirrel 
    // can access them for the purpose of this example
    ssq::sqstring message;
    // const int integer;
};
#endif


int main( int argc, char* argv[] )
{
    UMBA_USED(argc);
    UMBA_USED(argv);

    //ssq::VM vm(1024, ssq::Libs::STRING | ssq::Libs::IO | ssq::Libs::MATH);
    ssq::VM vm(1024, ssq::Libs::ALL);

    try
    {

        ssq::Script script1 = vm.compileSource(sqSrc1);
        // ssq::Script scriptB = vm.compileFile(/* path to source file */);
    
        // ssq::Script script = vm.compileFile("example_arrays.nut");

        // vm.addFunc(_SC("myCppFunc"), myCppFunc);
        vm.addFunc(_SC("myCppFunc"), std::function<ssq::sqstring(int, int)>(myCppFunc));

        #if defined(TEST_CLASS_BIND)
        CppClass::expose(vm);
        #endif
    
        vm.run(script1);
    
        ssq::Function funcPrintContents = vm.findFunc(_SC("printContents"));
        vm.callFunc(funcPrintContents, vm);

        #if defined(TEST_CLASS_BIND)
        CppClass cls;
        cls.message = _SC("Original Message");

        ssq::Function funcClassHandler = vm.findFunc(_SC("classHandler"));
        vm.callFunc(funcClassHandler, vm, &cls);
        #endif


    } catch (ssq::CompileException& e) {
        std::cerr << "Failed to run file: " << e.what() << std::endl;
        return -1;
    } catch (ssq::TypeException& e) {
        std::cerr << "Something went wrong passing objects: " << e.what() << std::endl;
        return -1;
    } catch (ssq::RuntimeException& e) {
        std::cerr << "Something went wrong during execution: " << e.what() << std::endl;
        return -1;
    } catch (ssq::NotFoundException& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }


    return 0;
}



