/*! \file
    \brief TestWinDrawContextXX tests family - entry point for GUI/drawing tests
*/
// TestDrawContext.cpp : main source file for TestDrawContextXXX.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atldlgs.h>
#include <atlscrl.h>
#include <atlprint.h>
#include <atlcrack.h>

#include "resource.h"

#include "View.h"
#include "props.h"
#include "list.h"

#if (TESTWINDRAWCONTEXT<4)
   #include "MainFrm.h"
#else
   #include "MainFrm2.h"
#endif

#include "marty_dc_impl_win32/gdiplus_inc.h"

#include "marty_draw_context/dc_debug/uw_log.h"


CAppModule _Module;


int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CMainFrame wndMain;

    if(wndMain.CreateEx() == NULL)
    {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }

    wndMain.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();
    return nRet;
}


int mainImpl(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(iccx);
    iccx.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    BOOL bRet = ::InitCommonControlsEx(&iccx);
    MARTY_ARG_USED(bRet);
    ATLASSERT(bRet);

    HRESULT hRes = _Module.Init(NULL, hInstance);
    MARTY_ARG_USED(hRes);
    ATLASSERT(SUCCEEDED(hRes));

    // https://peter.bloomfield.online/using-gdi-plus-in-cpp-win32-programming/
    // https://stackoverflow.com/questions/63766173/c-win32-api-gdi-rectangle-antialiasing-not-working-properly-with-transparent
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR                    gdiplusToken;

    // Initialize GDI+.
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    UW_LOG()<<"Hello TestWinDrawContext\n";

    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return nRet;
}

