#pragma once


// gdiplus.h requires min/max macros to be defined

#ifndef max
    #define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif



#include <gdiplus.h>
#if defined(_MSC_VER)
    #pragma comment (lib,"Gdiplus.lib")
#endif


#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif

