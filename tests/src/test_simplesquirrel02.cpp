/*! \file
    \brief Squirrel binding test for DrawColor and DrawCoords C++ types, and DrawContext enumerations
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


// #pragma warning(disable:4717)

#include <simplesquirrel/simplesquirrel.hpp>

// 

// #pragma warning(disable:4717)

#include "marty_draw_context/bindings/simplesquirrel.h"



#define TEST_CLASS_BIND

const wchar_t *sqSrc1 = L"\
enum Stuff {\n\
  first = 10\n\
  second = 11\n\
  third = 1.2\n\
}\n\
\n\
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
    print(\"myCppFunc : \" + myCppFunc(1,2));\n\
    print(\"myCppFunc1: \" + myCppFunc1(1,2));\n\
    print(\"index 0: \" + arr[0]);\n\
    print(\"index 1: \" + arr[1]);\n\
    //print(\"index 2: \" + arr[2]);\n\
}\n\
\n\
\n\
"
/* 21 lines */

// type(obj)

#if defined(TEST_CLASS_BIND)
L"\n\
\n\
function printTableDump(tbl, indent)\n\
{\n\
    foreach(key,val in tbl) \n\
    {\n\
        local strTypeofKey  = typeof key;\n\
        local strTypeKeyRaw = type(key);\n\
        local strTypeofVal  = typeof val;\n\
        local strTypeValRaw = type(val);\n\
\n\
        print( indent + \"'\" + key + \"' (\" + strTypeofKey + \"/\" + strTypeKeyRaw + \") -> '\" + val + \"' (\" + strTypeofVal + \"/\" + strTypeValRaw + \")\");\n\
        if (strTypeValRaw==\"table\")\n\
        {\n\
            printTableDump(val, indent+\"  \");\n\
        }\n\
\n\
    }\n\
}\n\
\n\
\n\
\n\
function classHandler(cppClass)\n\
{\n\
\n\
    print(\"--- Root table\");\n\
    printTableDump(getroottable(), \"\");\n\
    print(\"--- Const table\");\n\
    printTableDump(getconsttable(), \"\");\n\
    print(\"--- Continue\");\n\
    print(type(type(\"String\")));\n\
    print(\"--- Continue\");\n\
\n\
\n\
    print(\"cppClass.getMessage: \" + cppClass.getMessage());\n\
    cppClass.setMessage(\"New Msg\");\n\
    print(\"cppClass.getMessage: \" + cppClass.getMessage());\n\
\n\
    local dc   = DrawContext;\n\
    local Draw = DrawContext;\n\
\n\
    printDrawColor(dc.Color());\n\
    local tmpClr = Draw.Color();\n\
    tmpClr.r     = 3;\n\
    tmpClr.g     = 2;\n\
    tmpClr.b     = 1;\n\
    printDrawColor(tmpClr);\n\
    tmpClr.r     = 130;\n\
    tmpClr.g     = 131;\n\
    tmpClr.b     = 132;\n\
    printDrawColor(tmpClr);\n\
    printDrawColor(Draw.Color.fromUnsigned(255));\n\
    printDrawColor(Draw.Color.fromString(\"red\"));\n\
    printDrawColor(Draw.Color.fromString(\"green\"));\n\
    printDrawColor(Draw.Color.fromString(\"blue\"));\n\
\n\
    printDrawCoords(Draw.Coords(1,2));\n\
    printDrawCoords(Draw.Coords(1.2,3.4));\n\
    printDrawCoords(Draw.Coords(\"5.6\",\"7.8\"));\n\
    // printDrawCoords(Draw.Coords(\"AA\",\"Bbb\"));\n\
\n\
    print(\"---\");\n\
\n\
    print(\"Stuff.first : \" + Stuff.first);\n\
    //print(\"TestEnumG.One : \" + TestEnumG.One);                         // !!!\n\
    //print(\"dc.TestEnum.One : \" + DrawContext.TestEnum.One);            // !!!\n\
}\n\
\n\
\n\
function printDrawColor(clr){\n\
    print(\"---\");\n\
    print(\"clr.r     : \" + clr.r);\n\
    print(\"clr.g     : \" + clr.g);\n\
    print(\"clr.b     : \" + clr.b);\n\
    print(\"clr.toString: \" + clr.toString());\n\
}\n\
\n\
\n\
function printDrawCoords(c){\n\
    print(\"---\");\n\
    print(\"c.x     : \" + c.x);\n\
    print(\"c.y     : \" + c.y);\n\
}\n\
\n\
\n\
"
#endif
;


const wchar_t *sqSrc2 = L"\
";


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

        // https://isocpp.org/wiki/faq/pointers-to-members

        // Не работает
        cls.addFunc(_SC("setMessage"), &CppClass::setMessage);
        cls.addFunc(_SC("getMessage"), &CppClass::getMessage);

        // Это тоже - тут какая-то рекурсия возникает
        // cls.addFunc(_SC("getMessageLowerCase"), [](CppClass* self) -> ssq::sqstring {
        //     ssq::sqstring data = self->message;
        //     //std::transform(data.begin(), data.end(), data.begin(), ::tolower);
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

        vm.addFunc(_SC("myCppFunc1"), myCppFunc);
        vm.addFunc(_SC("myCppFunc" ), std::function<ssq::sqstring(int, int)>(myCppFunc));

        #if defined(TEST_CLASS_BIND)

        CppClass::expose(vm);


        ssq::Table tDraw = 
        vm.addTable(_SC("DrawContext"));
        marty_draw_context::simplesquirrel::DrawColor::expose(tDraw /*vm*/, _SC("Color"));
        marty_draw_context::simplesquirrel::DrawCoords::expose(tDraw /*vm*/, _SC("Coords"));


        ssq::Enum testEnumG = vm.addEnum(_SC("TestEnumG"));
        testEnumG.addSlot(_SC("One")   , 1);
        testEnumG.addSlot(_SC("Two")   , 2);
        testEnumG.addSlot(_SC("Three") , 3);

        ssq::Enum testEnumT = tDraw.addEnumGlobal(_SC("TestEnumT"));
        testEnumT.addSlot(_SC("One")   , 4);
        testEnumT.addSlot(_SC("Two")   , 5);
        testEnumT.addSlot(_SC("Three") , 6);

        // template<typename T> void setConst(const char* name, const T& value) {

        

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



