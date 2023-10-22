// View.h : interface of the CBitmapView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __VIEW_H__
#define __VIEW_H__

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/cmd_line.h"
#include "umba/filename.h"
#include "umba/time_service.h"

#include "dotNutBase/bindings/simplesquirrel.h"
#include "dotNutBase/dotNutBase.h"


#include "marty_dc_impl_win32/gdi_draw_context.h"
#include "marty_dc_impl_win32/gdiplus_draw_context.h"
#include "marty_dc_impl_win32/multi_dc.h"



#include <array>

#include <simplesquirrel/simplesquirrel.hpp>

#ifdef _MSC_VER
#pragma comment(lib, "_squirrel.lib")
#pragma comment(lib, "_simplesquirrel.lib")
#endif

#include "marty_draw_context/bindings/simplesquirrel.h"
#include "marty_vk/bindings/simplesquirrel.h"
#include "marty_fs_adapters/simple_file_api.h"

#include "nutHelpers.h"


// #define LOG_SQUIRREL_SOURCES


typedef marty_draw_context::DrawCoord                         DrawCoord                 ;
typedef marty_draw_context::DrawSize                          DrawSize                  ;
typedef marty_draw_context::DrawScale                         DrawScale                 ;
typedef marty_draw_context::GradientParams                    GradientParams            ;
typedef marty_draw_context::ColorRef                          ColorRef                  ;
typedef marty_draw_context::GradientType                      GradientType              ;
typedef marty_draw_context::GradientRoundRectFillFlags        GradientRoundRectFillFlags;
typedef marty_draw_context::SmoothingMode                     SmoothingMode             ;
typedef marty_draw_context::BkMode                            BkMode                    ;
typedef marty_draw_context::LineEndcapStyle                   LineEndcapStyle           ;
typedef marty_draw_context::LineJoinStyle                     LineJoinStyle             ;
typedef marty_draw_context::ColorRef                          ColorRef                  ;
typedef marty_draw_context::PenParamsWithColor                PenParamsWithColor        ;
typedef marty_draw_context::IDrawContext                      IDrawContext              ;
typedef marty_draw_context::HorAlign                          HorAlign                  ;
typedef marty_draw_context::FontStyleFlags                    FontStyleFlags            ;
typedef marty_draw_context::FontParamsA                       FontParamsA               ;
typedef marty_draw_context::FontWeight                        FontWeight                ;

typedef marty_draw_context::GdiDrawContext                    GdiDrawContext    ;
typedef marty_draw_context::GdiPlusDrawContext                GdiPlusDrawContext;



// #define TEST_DC_USE_GDIPLUS


#define TEST_DC_DRAWRECT
#define TEST_DC_FISHTAIL
#define TEST_DC_ROUND_SQUARE
#define TEST_DC_SNAKE

#define TEST_DC_FONTS
#define TEST_DC_SPIDERS
#define TEST_DC_GRADIENT_RECT
#define TEST_DC_GRADIENT_ROUNDRECT
#define TEST_DC_GRADIENT_CIRCLE
#define TEST_DC_LINEJOINSTYLE
#define TEST_DC_ARCTO


// Про PNG - https://4pda.to/forum/index.php?showtopic=44099


inline
void smpPrint(ssq::Object val)
{
    using umba::lout;
    lout << encoding::toUtf8(dotnut::System::objectToStringHelper(val));
}

inline
void smpPrintLn(ssq::Object val)
{
    using umba::lout;
    lout << encoding::toUtf8(dotnut::System::objectToStringHelper(val)) << "\n";
}




class CBitmapView : public CScrollWindowImpl<CBitmapView>
{
public:
    DECLARE_WND_CLASS_EX(NULL, 0, -1)

    //CBitmap m_bmp;
    //SIZE m_size;

    // static const Flag NONE = 0x0000;
    // static const Flag IO = 0x0001;
    // static const Flag BLOB = 0x0002;
    // static const Flag MATH = 0x0004;
    // static const Flag SYSTEM = 0x0008;
    // static const Flag STRING = 0x0010;
    // static const Flag ALL = 0xFFFF;

    ssq::VM            vm; // (1024, ssq::Libs::MATH | ssq::Libs::SYSTEM | ssq::Libs::STRING);
    dotnut::AppHost    appHost;

    UINT_PTR           mainTimerId    = 0;
    std::uint32_t      lastTimerTick  = 0;
    bool               loadingFailed = false;


    std::uint32_t getTickDelta()
    {
        std::uint32_t curTick = ::GetTickCount();

        if (!lastTimerTick)
        {
            lastTimerTick = curTick;
            return 0;
        }

        return curTick - lastTimerTick;
    }

    void updateLastTick()
    {
        lastTimerTick = ::GetTickCount();
    }

    HWND findStatusBar() const
    {
        HWND hwndParent = ::GetParent(m_hWnd);

        // ID status bar 0x0000E801
        // class is "msctls_statusbar32"

        return ::FindWindowExA(hwndParent, 0, "msctls_statusbar32", 0);
    }

    void setStatusText(const std::string &t) const
    {
        HWND hwndStatusBar = findStatusBar();
        ::SetWindowTextW(hwndStatusBar, encoding::fromUtf8(t).c_str());
    }

    void setStatusReady() const
    {
        setStatusText("Ready");
    }

    template<typename ExceptiuonType>
    std::string getExceptionString(const char *title, const ExceptiuonType &e) const
    {
        MARTY_ARG_USED(title);

        std::string resStr;

        #if 0
        if (title)
        {
            resStr.append(title);
        }

        if (!resStr.empty())
        {
            resStr.append(": ");
        }
        #endif

        resStr.append(e.what());

        return resStr;
    }

    std::string getExceptionString(const ssq::CompileException &e) const
    {
        return getExceptionString("Failed to load file", e);
    }

    std::string getExceptionString(const ssq::TypeException &e) const
    {
        return getExceptionString("Type error", e);
    }

    std::string getExceptionString(const ssq::RuntimeException &e) const
    {
        return getExceptionString("Runtime error", e);
    }

    std::string getExceptionString(const ssq::NotFoundException &e) const
    {
        return getExceptionString("Not found", e);
    }

    std::string getExceptionString(const std::out_of_range &e) const
    {
        return getExceptionString("Out of range", e);
    }

    std::string getExceptionString() const
    {
        return std::string("Unknown error");
    }

    template<typename ExceptiuonType>
    void logException(const ExceptiuonType &e)
    {
        auto msg = getExceptionString(e);
        umba::lout << msg << "\n";
        setStatusText(msg);
    }

    void logException()
    {
        auto msg = getExceptionString();
        umba::lout << msg << "\n";
        setStatusText(msg);
    }

    #define MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION_EX(exceptionType, actionFailed)       \
        catch(const exceptionType &e)                                                           \
        {                                                                                       \
            logException(e);                                                                    \
            actionFailed = true;                                                                \
        }

    #define MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(actionFailed)                     \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION_EX(ssq::CompileException, actionFailed)   \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION_EX(ssq::TypeException, actionFailed)      \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION_EX(ssq::RuntimeException, actionFailed)   \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION_EX(ssq::NotFoundException, actionFailed)  \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION_EX(std::out_of_range, actionFailed)       \
        catch(...)                                                                              \
        {                                                                                       \
            logException();                                                                     \
            actionFailed = true;                                                                \
        }

    #define MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION(exceptionType)                        \
        catch(const exceptionType &e)                                                           \
        {                                                                                       \
            logException(e);                                                                    \
        }

    #define MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS()                                    \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION(ssq::CompileException)                    \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION(ssq::TypeException)                       \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION(ssq::RuntimeException)                    \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION(ssq::NotFoundException)                   \
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTION(std::out_of_range)                        \
        catch(...)                                                                              \
        {                                                                                       \
            logException();                                                                     \
        }




    void reloadScript( bool bFirstTime = false)
    {
        using umba::lout;
        using namespace umba::omanip;

        std::string strScript;
        std::string fullScriptFileName;
        nut_helpers::findNut( strScript, fullScriptFileName );

        #if defined(UNICODE) || defined(_UNICODE)

            ssq::sqstring sqScript         = encoding::fromUtf8(strScript);
            ssq::sqstring sqScriptFilename = encoding::fromUtf8(fullScriptFileName);

        #else

            ssq::sqstring sqScript         = strScript;
            ssq::sqstring sqScriptFilename = fullScriptFileName;

        #endif

        loadingFailed = false;

        if (!sqScript.empty())
        {
            try
            {
                vm = ssq::VM(1024, ssq::Libs::MATH |  /* ssq::Libs::SYSTEM | */  ssq::Libs::STRING);
    
                dotnut::simplesquirrel::performBinding(vm, "DotNut");

                vm.addFunc( _SC("smpprint"  ) , &smpPrint   );
                vm.addFunc( _SC("smpprintln") , &smpPrintLn );


                ssq::sqstring preparedScriptText1 = _SC("Game <- {}")
                                                  + marty_draw_context::simplesquirrel::performBinding(vm, sqScript, "Drawing")
                                                  + marty_vk::simplesquirrel::enumsExposeMakeScript("Vk")
                                                  ;

                #if defined(DEBUG) || defined(_DEBUG)
                    #if defined(LOG_SQUIRREL_SOURCES)
                        lout << encoding::toUtf8(preparedScriptText1);
                        lout << "\n----------\n\n";
                    #endif
                #endif
    
                ssq::Script script = vm.compileSource(preparedScriptText1.c_str(), sqScriptFilename.c_str());
    
                vm.run(script);

                setStatusReady();
                
            } 
            MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(loadingFailed)
            

            if (loadingFailed)
            {
                if (mainTimerId)
                {
                    ::KillTimer(m_hWnd, mainTimerId);
                    mainTimerId = 0;
                }

            }
            else // loaded
            {
                try{
                    ssq::Function sqOnLoad = marty_simplesquirrel::findFunc(vm, "Game.onLoad");
    
                    auto res = vm.callFunc(sqOnLoad, vm,  /* appHost, */  bFirstTime);
    
                    bool needUpdate = marty_simplesquirrel::fromObjectConvertHelper<bool>(res, _SC("Game::onLoad returned"));
                    if (needUpdate)
                    {
                        invalidateClientArea();
                    }

                    setStatusReady();
                } 
                MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS()

                mainTimerId = ::SetTimer(m_hWnd, 1, 25 /* ms */ , 0);

            }

        }
    }

    CBitmapView() : vm(1024, ssq::Libs::MATH  /* | ssq::Libs::SYSTEM */ | ssq::Libs::STRING)
    {
        using umba::lout;
        using namespace umba::omanip;

        appHost.sys.printHandler = [&](const ssq::sqstring &str)
        {
            lout << encoding::toUtf8(str);
        };

        Sleep(300);
        
    }

    CBitmapView(const CBitmapView& ) = delete;

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        MARTY_ARG_USED(pMsg);
        return FALSE;
    }

    void SetBitmap(HBITMAP hBitmap)
    {
        MARTY_ARG_USED(hBitmap);
    }

    BEGIN_MSG_MAP(CBitmapView)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_KEYUP(OnKeyUp)
        MSG_WM_SIZE(OnSize)
        MSG_WM_SIZING(OnSizing)
        CHAIN_MSG_MAP(CScrollWindowImpl<CBitmapView>);
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct)
    {
        MARTY_ARG_USED(lpCreateStruct);

        reloadScript(true);

        //mainTimerId = ::SetTimer(m_hWnd, 1, 10, 0);

        return 0;
    }

    void invalidateClientArea()
    {
        InvalidateRect(0, FALSE);
    }

    void OnTimer(UINT_PTR nIDEvent)
    {
        using umba::lout;
        using namespace umba::omanip;

        if (loadingFailed)
        {
            return;
        }

        if (nIDEvent==mainTimerId)
        {
            std::uint32_t tickDelta = getTickDelta();
            updateLastTick();

            bool timerHandlerFails = true;

            try{

                try
                {
                    ssq::Function sqOnUpdate = marty_simplesquirrel::findFunc(vm, "Game.onUpdate");
                    auto res = vm.callFunc(sqOnUpdate, vm,  /* appHost,  */ tickDelta);
    
                    bool needUpdate = marty_simplesquirrel::fromObjectConvertHelper<bool>(res, _SC("Game::onUpdate returned"));
                    //needUpdate = true;
                    if (needUpdate)
                    {
                        invalidateClientArea();
                    }

                    timerHandlerFails = false;
                }
                catch (ssq::NotFoundException&)
                {}

                setStatusReady();

            } 
            MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS()

            if (timerHandlerFails)
            {
                ::KillTimer(m_hWnd, mainTimerId);
                mainTimerId = 0;
            }

        }
    }


    // func((UINT)wParam, (UINT)lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16));
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        if (nChar==VK_F5)
        {
            if (nRepCnt<2)
            {
                reloadScript();
                invalidateClientArea();
            }

            return;
        }

        OnKeyEvent(true, (std::uint32_t)nChar, (std::uint32_t)nRepCnt, (std::uint32_t)nFlags);
    }

    void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        if (nChar==VK_F5)
        {
            return;
        }

        OnKeyEvent(false, (std::uint32_t)nChar, (std::uint32_t)nRepCnt, (std::uint32_t)nFlags);
    }

    void OnKeyEvent(bool bDown, std::uint32_t nChar, std::uint32_t nRepCnt, std::uint32_t nFlags)
    {
        using umba::lout;
        using namespace umba::omanip;

        MARTY_ARG_USED(nFlags);

        if (loadingFailed)
        {
            return;
        }

        try{
            ssq::Function sqOnKeyEvent = marty_simplesquirrel::findFunc(vm, "Game.onKeyEvent");

            auto res = vm.callFunc(sqOnKeyEvent, vm,  /* appHost,  */ bDown, nChar, nRepCnt);

            bool needUpdate = marty_simplesquirrel::fromObjectConvertHelper<bool>(res, _SC("Game::onKeyEvent returned"));
            if (needUpdate)
            {
                invalidateClientArea();
            }

            setStatusReady();

        } 
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS()
        
    }

    void OnSize(UINT nType, CSize size)
    {
        MARTY_ARG_USED(nType);
        MARTY_ARG_USED(size);

        if (loadingFailed)
        {
            return;
        }

    }

    void OnSizing(UINT fwSide, LPRECT pRect)
    {
        MARTY_ARG_USED(fwSide);
        MARTY_ARG_USED(pRect);

        if (loadingFailed)
        {
            return;
        }

    }

    LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        // RECT rect;
        // GetClientRect(&rect);
        // int x = 0;
        // int y = 0;

        MARTY_ARG_USED(wParam);

        /*
        if(!m_bmp.IsNull())
        {
            x = m_size.cx + 1;
            y = m_size.cy + 1;
        }
        */
        /*
        CDCHandle dc = (HDC)wParam;
        if(rect.right > m_sizeAll.cx)
        {
            RECT rectRight = rect;
            rectRight.left = x;
            rectRight.bottom = y;
            dc.FillRect(&rectRight, COLOR_WINDOW);
        }
        if(rect.bottom > m_sizeAll.cy)
        {
            RECT rectBottom = rect;
            rectBottom.top = y;
            dc.FillRect(&rectBottom, COLOR_WINDOW);
        }
        */
        /*
        if(!m_bmp.IsNull())
        {
            dc.MoveTo(m_size.cx, 0);
            dc.LineTo(m_size.cx, m_size.cy);
            dc.LineTo(0, m_size.cy);
        }
        */
        return 0;
    }

    void DoPaint(CDCHandle dc)
    {

        RECT clientRect{0,0};
        ::GetClientRect(m_hWnd, &clientRect);

        auto cx = clientRect.right  - clientRect.left; // + 1;
        auto cy = clientRect.bottom - clientRect.top ; // + 1;


        enum resolution
        {
            w = 800,
            h = 600
        };



        // HDC memDc = ::CreateCompatibleDC(dc.m_hDC);
        CDC memDc = ::CreateCompatibleDC(dc.m_hDC);

        HBITMAP hMemBmp  = ::CreateCompatibleBitmap ( dc.m_hDC, cx, cy );
        //HBITMAP hOldBmp = (HBITMAP)::SelectObject(memDc.m_hDC, hMemBmp);
        HBITMAP hOldBmp = memDc.SelectBitmap(hMemBmp);

        RECT clRect;
        clRect.left   = 0;
        clRect.top    = 0;
        clRect.right  = cx;
        clRect.bottom = cy;
        //::FillRect(memDc, &clRect, (HBRUSH)COLOR_WINDOW);
        memDc.FillRect(&clRect, (HBRUSH)COLOR_WINDOW);
        //memDc.FillRect(&clRect, (HBRUSH)COLOR_HOTLIGHT);
        

        #if 0
        ::BitBlt( memDc     // A handle to the destination device context.
                , 0, 0         //dstX, dstY   // The x/y-coordinates, in logical units, of the upper-left corner of the destination rectangle.
                , cx, cy       // The width/height, in logical units, of the source and destination rectangles.
                , dc.m_hDC        // hdcCopyFrom  // A handle to the source device context.
                , 0, 0         // The x/y-coordinate, in logical units, of the upper-left corner of the source rectangle.
                , SRCCOPY      // A raster-operation code - Copies the source rectangle directly to the destination rectangle.
                );
        #endif


        #ifdef TEST_DC_USE_GDIPLUS
            auto idc = marty_draw_context::makeMultiDrawContext(memDc.m_hDC, true  /* prefferGdiPlus */);
        #else
            auto idc = marty_draw_context::makeMultiDrawContext(memDc.m_hDC, false /* prefferGdiPlus */);
        #endif

        IDrawContext *pDc = &idc;

        appHost.sys.info.graphicsBackendInfo.name = marty_simplesquirrel::to_sqstring(pDc->getEngineName());

        DoPaintImpl(pDc);

        ::BitBlt( dc.m_hDC       // A handle to the destination device context.
                , 0, 0           //dstX, dstY   // The x/y-coordinates, in logical units, of the upper-left corner of the destination rectangle.
                , cx, cy         // The width/height, in logical units, of the source and destination rectangles.
                , memDc.m_hDC    // hdcCopyFrom  // A handle to the source device context.
                , 0, 0           // The x/y-coordinate, in logical units, of the upper-left corner of the source rectangle.
                , SRCCOPY        // A raster-operation code - Copies the source rectangle directly to the destination rectangle.
                );

        ::SelectObject(memDc, hOldBmp);
        ::DeleteObject(hMemBmp);
        //::DeleteDC(memDc);

    }

    void DoPaintImpl( marty_draw_context::IDrawContext *pDc )
    {
        using umba::lout;
        using namespace umba::omanip;

        if (loadingFailed)
        {
            return;
        }

        auto startTick = umba::time_service::getCurTimeMs();

        try{
            ssq::Function sqOnPaint = marty_simplesquirrel::findFunc(vm, "Game.onPaint");

            pDc->setStringEncoding("UTF-8");
            pDc->setBkMode( BkMode::transparent );
            pDc->setSmoothingMode(SmoothingMode::antiAlias); // highSpeed highQuality antiAlias defMode none


            //lout << "DoPaintImpl: call onPaint from script\n";
            marty_draw_context::simplesquirrel::DrawingContext sqDc = marty_draw_context::simplesquirrel::DrawingContext(vm.getHandle(), pDc);

            RECT clientRect{0,0};
            ::GetClientRect(m_hWnd, &clientRect);

            auto cx = clientRect.right  - clientRect.left; // + 1;
            auto cy = clientRect.bottom - clientRect.top ; // + 1;
            // lout << "OnPaint: cx: " << cx << ", cy: " << cy <<"\n";
            sqDc.ctxSizeX = (int)(cx);
            sqDc.ctxSizeY = (int)(cy);

            vm.callFunc(sqOnPaint, vm,  /* appHost,  */ &sqDc);
            //vm.callFunc(sqOnPaint, vm, sqDc);

            setStatusReady();

        } 
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS()
        

        auto endTick = umba::time_service::getCurTimeMs();

        MARTY_ARG_USED(startTick);
        MARTY_ARG_USED(endTick);

        //lout << "OnPaint times: " << (endTick-startTick) << "\n";

    }

};

#endif // __VIEW_H__
