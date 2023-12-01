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
#include <functional>

#include <simplesquirrel/simplesquirrel.hpp>

#ifdef _MSC_VER
#pragma comment(lib, "_squirrel.lib")
#pragma comment(lib, "_simplesquirrel.lib")
#endif

#include "marty_draw_context/bindings/simplesquirrel.h"
#include "marty_vk/bindings/simplesquirrel.h"
#include "marty_fs_adapters/simple_file_api.h"

#include "nutHelpers.h"

#define VIEW04_LOG_SQUIRREL_CALLS


#define CRACK_ON_KEY_UPDOWN_FLAGS_GET_REPETITION_STATE_FLAG(nFlags) (((nFlags)>>(30-16))&0x01)
#define CRACK_ON_KEY_UPDOWN_FLAGS_GET_PREV_DOWN_STATE_FLAG(nFlags)  CRACK_ON_KEY_UPDOWN_FLAGS_GET_REPETITION_STATE_FLAG((nFlags))



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
    umba::lout.flush();
}

inline
void smpPrintLn(ssq::Object val)
{
    using umba::lout;
    lout << encoding::toUtf8(dotnut::System::objectToStringHelper(val)) << "\n";
    umba::lout.flush();
}


inline
void voidDoNothing()
{}

class CBitmapView : public CScrollWindowImpl<CBitmapView>
{
public:
    //DECLARE_WND_CLASS_EX(NULL, 0, -1)
    DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, -1)
    

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
    bool               scriptSomethingFailed = false;
    bool               m_bMouseTracking      = false;
    bool               m_timerUpdateDisabled = false;

    CPoint             cachedBitmapSize;
    HBITMAP            hbitmapCached = 0;

    //<functional>
    std::function<void()> toggleFullScreen = voidDoNothing; // ([]() -> int& { static int i{0x2A}; return i; }); // OK


    void trackMouseEvent()
    {
        TRACKMOUSEEVENT trme;
        trme.cbSize      = sizeof(trme);
        trme.dwFlags     = TME_HOVER | TME_LEAVE;
        trme.hwndTrack   = m_hWnd;
        trme.dwHoverTime = (DWORD)HOVER_DEFAULT;

        if (_TrackMouseEvent(&trme))
        {
            m_bMouseTracking = true;
        }
    }

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
        umba::lout.flush();
        setStatusText(msg);
    }

    void logException()
    {
        auto msg = getExceptionString();
        umba::lout << msg << "\n";
        umba::lout.flush();
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

        bool scriptSomethingFailedPrev = scriptSomethingFailed;
        scriptSomethingFailed = false;

        if (!sqScript.empty())
        {
            try
            {
                vm = ssq::VM(1024, ssq::Libs::MATH |  /* ssq::Libs::SYSTEM | */  ssq::Libs::STRING);
    
                dotnut::simplesquirrel::performBinding(vm, "DotNut");

                vm.addFunc( _SC("smpprint"  ) , &smpPrint   );
                vm.addFunc( _SC("smpprintln") , &smpPrintLn );


                ssq::sqstring preparedScriptText1 = _SC("Game <- {}")
                                                  + marty_vk::simplesquirrel::enumsExposeMakeScript("Vk")
                                                  + marty_draw_context::simplesquirrel::performBinding(vm, sqScript, "Drawing")
                                                  ;

                if (scriptSomethingFailedPrev)
                {
                    lout << encoding::toUtf8(preparedScriptText1);
                    lout << "\n----------\n\n";
                }
                // #if defined(DEBUG) || defined(_DEBUG)
                //     #if defined(LOG_SQUIRREL_SOURCES)
                //         lout << encoding::toUtf8(preparedScriptText1);
                //         lout << "\n----------\n\n";
                //     #endif
                // #endif
    
                ssq::Script script = vm.compileSource(preparedScriptText1.c_str(), sqScriptFilename.c_str());
    
                vm.run(script);

                setStatusReady();
                
            } 
            MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)
            

            if (scriptSomethingFailed)
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
                    #if defined(VIEW04_LOG_SQUIRREL_CALLS)
                    using umba::lout;
                    lout << "try squirrel Game.onLoad\n";
                    umba::lout.flush();
                    #endif

                    ssq::Function sqOnLoad = marty_simplesquirrel::findFunc(vm, "Game.onLoad");
    
                    //auto res = 
                    vm.callFunc(sqOnLoad, vm,  /* appHost, */  bFirstTime);
    
                    //bool needUpdate = 
                    //marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onLoad returned"));
                    // if (needUpdate)
                    // {
                    //     invalidateClientArea();
                    // }

                    // При загрузке скрипта форсим рисование безусловно
                    invalidateClientArea();

                    setStatusReady();
                } 
                MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)

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

        #if 0
        using namespace std;
        #include "tmp.cpp"

        auto res1 = Solution().solveEquation("x+5-3+x=6+x-2");
        auto res2 = Solution().solveEquation("x=x");
        auto res3 = Solution().solveEquation("2x=x");

        lout << res1 << "\n";
        lout << res2 << "\n";
        lout << res3 << "\n";
        #endif

        Sleep(300);
        
    }

    CBitmapView(const CBitmapView& ) = delete;

    ~CBitmapView()
    {
        if (hbitmapCached)
        {
            ::DeleteObject(hbitmapCached);
        }
    }



    BOOL PreTranslateMessage(MSG* pMsg)
    {
        MARTY_ARG_USED(pMsg);
        return FALSE;
    }

    void SetBitmap(HBITMAP hBitmap)
    {
        MARTY_ARG_USED(hBitmap);
    }



    void OnMouseMoveEvents( marty_draw_context::MouseMoveEventType    moveEventType
                          , marty_draw_context::MouseButtonStateFlags mbStateFlags
                          , const CPoint &point
                          )
    {
        // При marty_draw_context::MouseMoveEventType::leave mbStateFlags и point не имеют валидного значения

        if (scriptSomethingFailed)
        {
            return;
        }

        try
        {
            #if defined(VIEW04_LOG_SQUIRREL_CALLS)
            using umba::lout;
            lout << "try squirrel Game.onMouseMoveEvents, moveEventType: " << enum_serialize(moveEventType) << ", mbStateFlags: " << enum_serialize_flags(mbStateFlags) << "\n";
            umba::lout.flush();
            #endif

            ssq::Function sqOnMouseMoveEvents = marty_simplesquirrel::findFunc(vm, "Game.onMouseMoveEvents");

            auto idc = makeDcForMouseHandler();
            marty_draw_context::IDrawContext *pDc = &idc;
            appHost.sys.info.graphicsBackendInfo.name = marty_simplesquirrel::to_sqstring(pDc->getEngineName());
            prepareDrawContext(pDc);

            marty_draw_context::simplesquirrel::DrawingContext sqDc = marty_draw_context::simplesquirrel::DrawingContext(vm.getHandle(), pDc);

            CPoint clientSize = getClientSizePoint();

            sqDc.ctxSizeX = (int)clientSize.x;
            sqDc.ctxSizeY = (int)clientSize.y;

            auto res = vm.callFunc(sqOnMouseMoveEvents, vm,  /* appHost,  */ &sqDc, (int)moveEventType, (int)mbStateFlags, marty_draw_context::simplesquirrel::DrawingCoords((float)point.x, (float)point.y));

            marty_draw_context::CallbackResultFlags resultFlags = (marty_draw_context::CallbackResultFlags)marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onMouseMoveEvents returned"));
            processCallbackResult(resultFlags);

            setStatusReady();

        }
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)
    }

    void OnMouseButtonEvents( marty_draw_context::MouseButton           mouseButton
                            , marty_draw_context::MouseButtonEvent      buttonEvent
                            , marty_draw_context::MouseButtonStateFlags mbStateFlags
                            , const CPoint &point
                            )
    {
        if (scriptSomethingFailed)
        {
            return;
        }

        try
        {
            #if defined(VIEW04_LOG_SQUIRREL_CALLS)
            using umba::lout;
            lout << "try squirrel Game.onMouseButtonEvents\n";
            umba::lout.flush();
            #endif

            ssq::Function sqOnMouseButtonEvents = marty_simplesquirrel::findFunc(vm, "Game.onMouseButtonEvents");

            auto idc = makeDcForMouseHandler();
            marty_draw_context::IDrawContext *pDc = &idc;
            appHost.sys.info.graphicsBackendInfo.name = marty_simplesquirrel::to_sqstring(pDc->getEngineName());
            prepareDrawContext(pDc);

            marty_draw_context::simplesquirrel::DrawingContext sqDc = marty_draw_context::simplesquirrel::DrawingContext(vm.getHandle(), pDc);

            CPoint clientSize = getClientSizePoint();

            sqDc.ctxSizeX = (int)clientSize.x;
            sqDc.ctxSizeY = (int)clientSize.y;

            auto res = vm.callFunc(sqOnMouseButtonEvents, vm,  /* appHost,  */ &sqDc, (int)mouseButton, (int)buttonEvent, (int)mbStateFlags, marty_draw_context::simplesquirrel::DrawingCoords((float)point.x, (float)point.y));

            marty_draw_context::CallbackResultFlags resultFlags = (marty_draw_context::CallbackResultFlags)marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onMouseButtonEvents returned"));
            processCallbackResult(resultFlags);

            setStatusReady();

        }
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)

    }

    // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel
    // Для Wheel событий - отдельный колбэк
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
    {
        marty_draw_context::MouseButtonStateFlags mbStateFlags = (marty_draw_context::MouseButtonStateFlags)nFlags;

        if (scriptSomethingFailed)
        {
            return TRUE;
        }

        try
        {
            #if defined(VIEW04_LOG_SQUIRREL_CALLS)
            using umba::lout;
            lout << "try squirrel Game.onMouseWheel\n";
            umba::lout.flush();
            #endif

            ssq::Function sqOnMouseWheel = marty_simplesquirrel::findFunc(vm, "Game.onMouseWheel");

            auto idc = makeDcForMouseHandler();
            marty_draw_context::IDrawContext *pDc = &idc;
            appHost.sys.info.graphicsBackendInfo.name = marty_simplesquirrel::to_sqstring(pDc->getEngineName());
            prepareDrawContext(pDc);

            marty_draw_context::simplesquirrel::DrawingContext sqDc = marty_draw_context::simplesquirrel::DrawingContext(vm.getHandle(), pDc);

            CPoint clientSize = getClientSizePoint();

            sqDc.ctxSizeX = (int)clientSize.x;
            sqDc.ctxSizeY = (int)clientSize.y;

            auto res = vm.callFunc(sqOnMouseWheel, vm,  /* appHost,  */ &sqDc, (int)zDelta, (int)mbStateFlags, marty_draw_context::simplesquirrel::DrawingCoords((float)point.x, (float)point.y));

            marty_draw_context::CallbackResultFlags resultFlags = (marty_draw_context::CallbackResultFlags)marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onMouseWheel returned"));
            processCallbackResult(resultFlags);

            setStatusReady();

        }
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)

        return TRUE;
    }


    void OnMouseHover(WPARAM wParam /* MK_* */ , CPoint ptPos)
    {
        m_bMouseTracking = false;
        OnMouseMoveEvents(marty_draw_context::MouseMoveEventType::hover, (marty_draw_context::MouseButtonStateFlags)wParam, ptPos);
    }

    void OnMouseLeave()
    {
        m_bMouseTracking = false;
        OnMouseMoveEvents(marty_draw_context::MouseMoveEventType::leave, marty_draw_context::MouseButtonStateFlags::none, CPoint(0,0));
    }

    // https://learn.microsoft.com/en-us/windows/win32/gdi/drawing-with-the-mouse
    void OnMouseMove(UINT nFlags, CPoint point)
    {
        if (!m_bMouseTracking)
        {
            // OnMouseMoveEvents(marty_draw_context::MouseMoveEventType::enter, (marty_draw_context::MouseButtonStateFlags)nFlags, point);
            trackMouseEvent();
        }

        OnMouseMoveEvents(marty_draw_context::MouseMoveEventType::move, (marty_draw_context::MouseButtonStateFlags)nFlags, point);
    }


    void OnLButtonDown(UINT nFlags, CPoint point)   { OnMouseButtonEvents( marty_draw_context::MouseButton::leftButton  , marty_draw_context::MouseButtonEvent::pressed    , (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnLButtonUp(UINT nFlags, CPoint point)     { OnMouseButtonEvents( marty_draw_context::MouseButton::leftButton  , marty_draw_context::MouseButtonEvent::released   , (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnLButtonDblClk(UINT nFlags, CPoint point) { OnMouseButtonEvents( marty_draw_context::MouseButton::leftButton  , marty_draw_context::MouseButtonEvent::doubleClick, (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnRButtonDown(UINT nFlags, CPoint point)   { OnMouseButtonEvents( marty_draw_context::MouseButton::rightButton , marty_draw_context::MouseButtonEvent::pressed    , (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnRButtonUp(UINT nFlags, CPoint point)     { OnMouseButtonEvents( marty_draw_context::MouseButton::rightButton , marty_draw_context::MouseButtonEvent::released   , (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnRButtonDblClk(UINT nFlags, CPoint point) { OnMouseButtonEvents( marty_draw_context::MouseButton::rightButton , marty_draw_context::MouseButtonEvent::doubleClick, (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnMButtonDown(UINT nFlags, CPoint point)   { OnMouseButtonEvents( marty_draw_context::MouseButton::middleButton, marty_draw_context::MouseButtonEvent::pressed    , (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnMButtonUp(UINT nFlags, CPoint point)     { OnMouseButtonEvents( marty_draw_context::MouseButton::middleButton, marty_draw_context::MouseButtonEvent::released   , (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }
    void OnMButtonDblClk(UINT nFlags, CPoint point) { OnMouseButtonEvents( marty_draw_context::MouseButton::middleButton, marty_draw_context::MouseButtonEvent::doubleClick, (marty_draw_context::MouseButtonStateFlags)nFlags, point ); }

    void OnXButtonDown(int fwButton, int dwKeys, CPoint point)
    {
        if (fwButton&XBUTTON1)
        {
            OnMouseButtonEvents( marty_draw_context::MouseButton::xButton1, marty_draw_context::MouseButtonEvent::pressed    , (marty_draw_context::MouseButtonStateFlags)dwKeys, point );
        }
        if (fwButton&XBUTTON2)
        {
            OnMouseButtonEvents( marty_draw_context::MouseButton::xButton2, marty_draw_context::MouseButtonEvent::pressed    , (marty_draw_context::MouseButtonStateFlags)dwKeys, point );
        }
    }

    void OnXButtonUp(int fwButton, int dwKeys, CPoint point)
    {
        if (fwButton&XBUTTON1)
        {
            OnMouseButtonEvents( marty_draw_context::MouseButton::xButton1, marty_draw_context::MouseButtonEvent::released   , (marty_draw_context::MouseButtonStateFlags)dwKeys, point );
        }
        if (fwButton&XBUTTON2)
        {
            OnMouseButtonEvents( marty_draw_context::MouseButton::xButton2, marty_draw_context::MouseButtonEvent::released   , (marty_draw_context::MouseButtonStateFlags)dwKeys, point );
        }
    }

    void OnXButtonDblClk(int fwButton, int dwKeys, CPoint point)
    {
        if (fwButton&XBUTTON1)
        {
            OnMouseButtonEvents( marty_draw_context::MouseButton::xButton1, marty_draw_context::MouseButtonEvent::doubleClick, (marty_draw_context::MouseButtonStateFlags)dwKeys, point );
        }
        if (fwButton&XBUTTON2)
        {
            OnMouseButtonEvents( marty_draw_context::MouseButton::xButton2, marty_draw_context::MouseButtonEvent::doubleClick, (marty_draw_context::MouseButtonStateFlags)dwKeys, point );
        }
    }


    BEGIN_MSG_MAP(CBitmapView)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_KEYUP(OnKeyUp)
        MSG_WM_SIZE(OnSize)
        MSG_WM_SIZING(OnSizing)

        MSG_WM_MOUSEHOVER(OnMouseHover)
        MSG_WM_MOUSELEAVE(OnMouseLeave)

        MSG_WM_MOUSEWHEEL(OnMouseWheel)

        MSG_WM_MOUSEMOVE(OnMouseMove)

        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)

        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_RBUTTONUP(OnRButtonUp)
        MSG_WM_RBUTTONDBLCLK(OnRButtonDblClk)

        MSG_WM_MBUTTONDOWN(OnMButtonDown)
        MSG_WM_MBUTTONUP(OnMButtonUp)
        MSG_WM_MBUTTONDBLCLK(OnMButtonDblClk)

        MSG_WM_XBUTTONDOWN(OnXButtonDown)
        MSG_WM_XBUTTONUP(OnXButtonUp)
        MSG_WM_XBUTTONDBLCLK(OnXButtonDblClk)

        CHAIN_MSG_MAP(CScrollWindowImpl<CBitmapView>);

    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct)
    {
        MARTY_ARG_USED(lpCreateStruct);

        trackMouseEvent();

        reloadScript(true);

        //mainTimerId = ::SetTimer(m_hWnd, 1, 10, 0);

        return 0;
    }

    void invalidateClientArea()
    {
        InvalidateRect(0, FALSE);
    }


    void processCallbackResult(marty_draw_context::CallbackResultFlags resultFlags)
    {
        if ((resultFlags&marty_draw_context::CallbackResultFlags::repaint)!=0)
        {
            invalidateClientArea();
        }

        if ((resultFlags&marty_draw_context::CallbackResultFlags::captureMouse)!=0)
        {
            ::SetCapture(m_hWnd);
        }
        else if ((resultFlags&marty_draw_context::CallbackResultFlags::releaseCapture)!=0)
        {
            ::ReleaseCapture();
        }

        if ((resultFlags&marty_draw_context::CallbackResultFlags::disableTimerUpdate)!=0)
        {
            m_timerUpdateDisabled = true;
        }
        else if ((resultFlags&marty_draw_context::CallbackResultFlags::enableTimerUpdate)!=0)
        {
            m_timerUpdateDisabled = false;
        }

    }

    void OnTimer(UINT_PTR nIDEvent)
    {
        using umba::lout;
        using namespace umba::omanip;

        if (scriptSomethingFailed)
        {
            return;
        }

        if (m_timerUpdateDisabled)
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
                    #if defined(VIEW04_LOG_SQUIRREL_CALLS)
                    using umba::lout;
                    lout << "try squirrel Game.onUpdate\n";
                    umba::lout.flush();
                    #endif
        
                    ssq::Function sqOnUpdate = marty_simplesquirrel::findFunc(vm, "Game.onUpdate");
                    auto res = vm.callFunc(sqOnUpdate, vm,  /* appHost,  */ tickDelta);
    
                    marty_draw_context::CallbackResultFlags resultFlags = (marty_draw_context::CallbackResultFlags)marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onUpdate returned"));
                    processCallbackResult(resultFlags);
                    //needUpdate = true;

                    timerHandlerFails = false;
                }
                catch (ssq::NotFoundException&)
                {}

                setStatusReady();

            } 
            MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)

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

        if (nChar==VK_F11)
        {
            auto prevDown = CRACK_ON_KEY_UPDOWN_FLAGS_GET_PREV_DOWN_STATE_FLAG(nFlags);
            if (!prevDown)
            {
                SetMsgHandled(TRUE);
                toggleFullScreen();
            }
        }
        else if (nChar==VK_F5)
        {
            if (nRepCnt<2)
            {
                reloadScript();
                invalidateClientArea();
            }

            return;
        }
        else
        {
            OnKeyEvent(true, (std::uint32_t)nChar, (std::uint32_t)nRepCnt, (std::uint32_t)nFlags);
        }

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

        if (scriptSomethingFailed)
        {
            return;
        }

        try{
            #if defined(VIEW04_LOG_SQUIRREL_CALLS)
            using umba::lout;
            lout << "try squirrel Game.onKeyEvent\n";
            umba::lout.flush();
            #endif

            ssq::Function sqOnKeyEvent = marty_simplesquirrel::findFunc(vm, "Game.onKeyEvent");

            auto res = vm.callFunc(sqOnKeyEvent, vm,  /* appHost,  */ bDown, nChar, nRepCnt);

            marty_draw_context::CallbackResultFlags resultFlags = (marty_draw_context::CallbackResultFlags)marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onKeyEvent returned"));
            processCallbackResult(resultFlags);

            setStatusReady();

        } 
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)
        
    }


    marty_draw_context::simplesquirrel::DrawingCoords getClientSize() const
    {
        marty_draw_context::simplesquirrel::DrawingCoords sz;

        RECT clientRect{0,0};
        ::GetClientRect(m_hWnd, &clientRect);

        auto cx = clientRect.right  - clientRect.left; // + 1;
        auto cy = clientRect.bottom - clientRect.top ; // + 1;
        // lout << "OnPaint: cx: " << cx << ", cy: " << cy <<"\n";
        sz.x = (float)(cx);
        sz.y = (float)(cy);

        return sz;
    }


    void OnSize(UINT nType, ::CSize size)
    {
        MARTY_ARG_USED(nType);
        MARTY_ARG_USED(size);

        if (scriptSomethingFailed)
        {
            return;
        }

        try
        {
           try
           {
                #if defined(VIEW04_LOG_SQUIRREL_CALLS)
                using umba::lout;
                lout << "try squirrel Game.onWindowSize\n";
                umba::lout.flush();
                #endif
    
                ssq::Function sqOnSizeEvent = marty_simplesquirrel::findFunc(vm, "Game.onWindowSize");

                marty_draw_context::simplesquirrel::DrawingCoords sz = getClientSize();
    
                auto res = vm.callFunc(sqOnSizeEvent, vm,  /* appHost,  */ (int)nType, sz);
    
                marty_draw_context::CallbackResultFlags resultFlags = (marty_draw_context::CallbackResultFlags)marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onWindowSize returned"));
                processCallbackResult(resultFlags);
    
                setStatusReady();
           
           }
           catch (ssq::NotFoundException&)
           {}

        } 
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)

    }

    void OnSizing(UINT fwSide, LPRECT pRect)
    {
        MARTY_ARG_USED(fwSide);
        MARTY_ARG_USED(pRect);

        if (scriptSomethingFailed)
        {
            return;
        }

        try
        {
           try
           {
                #if defined(VIEW04_LOG_SQUIRREL_CALLS)
                using umba::lout;
                lout << "try squirrel Game.onWindowSizing\n";
                umba::lout.flush();
                #endif

                ssq::Function sqOnSizeEvent = marty_simplesquirrel::findFunc(vm, "Game.onWindowSizing");

                marty_draw_context::simplesquirrel::DrawingCoords sz = getClientSize();
    
                auto res = vm.callFunc(sqOnSizeEvent, vm,  /* appHost,  */ (int)fwSide, sz);
    
                marty_draw_context::CallbackResultFlags resultFlags = (marty_draw_context::CallbackResultFlags)marty_simplesquirrel::fromObjectConvertHelper<int>(res, _SC("Game::onWindowSizing returned"));
                processCallbackResult(resultFlags);
    
                setStatusReady();
           
           }
           catch (ssq::NotFoundException&)
           {}

        } 
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)

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


    CPoint getClientSizePoint()
    {
        RECT clientRect{0,0};
        ::GetClientRect(m_hWnd, &clientRect);

        auto cx = clientRect.right  - clientRect.left; // + 1;
        auto cy = clientRect.bottom - clientRect.top ; // + 1;

        return CPoint(cx,cy);
    }

    void DoPaint(CDCHandle dc)
    {
        CPoint clientSize = getClientSizePoint();
        cachedBitmapSize  = clientSize;

        CDC memDc = ::CreateCompatibleDC(dc.m_hDC);

        HBITMAP hMemBmp  = ::CreateCompatibleBitmap ( dc.m_hDC, clientSize.x, clientSize.y );
        HBITMAP hOldBmp = memDc.SelectBitmap(hMemBmp);

        RECT clRect;
        clRect.left   = 0;
        clRect.top    = 0;
        clRect.right  = clientSize.x;
        clRect.bottom = clientSize.y;
        //::FillRect(memDc, &clRect, (HBRUSH)COLOR_WINDOW);
        memDc.FillRect(&clRect, (HBRUSH)COLOR_WINDOW);
        //memDc.FillRect(&clRect, (HBRUSH)COLOR_HOTLIGHT);
        
        auto idc = makeMultiDc(memDc.m_hDC, marty_draw_context::HdcReleaseMode::doNothing, m_hWnd);

        IDrawContext *pDc = &idc;

        appHost.sys.info.graphicsBackendInfo.name = marty_simplesquirrel::to_sqstring(pDc->getEngineName());

        DoPaintImpl(pDc);

        ::BitBlt( dc.m_hDC       // A handle to the destination device context.
                , 0, 0           //dstX, dstY   // The x/y-coordinates, in logical units, of the upper-left corner of the destination rectangle.
                , clientSize.x, clientSize.y         // The width/height, in logical units, of the source and destination rectangles.
                , memDc.m_hDC    // hdcCopyFrom  // A handle to the source device context.
                , 0, 0           // The x/y-coordinate, in logical units, of the upper-left corner of the source rectangle.
                , SRCCOPY        // A raster-operation code - Copies the source rectangle directly to the destination rectangle.
                );

        ::SelectObject(memDc, hOldBmp);

        if (hbitmapCached)
        {
            ::DeleteObject(hbitmapCached);
        }

        hbitmapCached = hMemBmp;

    }

    marty_draw_context::MultiDrawContextGdi makeMultiDc(HDC hdc, marty_draw_context::HdcReleaseMode hdcReleaseMode, HWND hwnd)
    {
        #ifdef TEST_DC_USE_GDIPLUS
            return marty_draw_context::makeMultiDrawContextGdi(hdc, true  /* prefferGdiPlus */, hdcReleaseMode, hwnd);
        #else
            return marty_draw_context::makeMultiDrawContextGdi(hdc, false /* prefferGdiPlus */, hdcReleaseMode, hwnd);
        #endif
    }

    void copyCachedBitmapToHdc(HDC hdc)
    {
        if (!hbitmapCached)
        {
            #if defined(VIEW04_LOG_SQUIRREL_CALLS)
            using umba::lout;
            lout << "copyCachedBitmapToHdc, hbitmapCached\n";
            umba::lout.flush();
            #endif

            return;
        }

        CDC memDc       = ::CreateCompatibleDC(hdc);
        HBITMAP hOldBmp = memDc.SelectBitmap(hbitmapCached);

        ::BitBlt( hdc            // A handle to the destination device context.
                , 0, 0           //dstX, dstY   // The x/y-coordinates, in logical units, of the upper-left corner of the destination rectangle.
                , cachedBitmapSize.x, cachedBitmapSize.y         // The width/height, in logical units, of the source and destination rectangles.
                , memDc.m_hDC    // hdcCopyFrom  // A handle to the source device context.
                , 0, 0           // The x/y-coordinate, in logical units, of the upper-left corner of the source rectangle.
                , SRCCOPY        // A raster-operation code - Copies the source rectangle directly to the destination rectangle.
                );

        ::SelectObject(memDc, hOldBmp);
    }

    void prepareDrawContext( marty_draw_context::IDrawContext *pDc )
    {
        pDc->setStringEncoding("UTF-8");
        pDc->setBkMode( BkMode::transparent );
        pDc->setSmoothingMode(SmoothingMode::antiAlias); // highSpeed highQuality antiAlias defMode none
    }

    marty_draw_context::MultiDrawContextGdi makeDcForMouseHandler()
    {
        HDC hdc = ::GetDC(m_hWnd);
        copyCachedBitmapToHdc(hdc);
        marty_draw_context::MultiDrawContextGdi mdc  = makeMultiDc(hdc, marty_draw_context::HdcReleaseMode::releaseDc, m_hWnd);
        return mdc;
    }

    
    void DoPaintImpl( marty_draw_context::IDrawContext *pDc )
    {
        if (scriptSomethingFailed)
        {
            #if defined(VIEW04_LOG_SQUIRREL_CALLS)
            using umba::lout;
            lout << "DoPaintImpl, scriptSomethingFailed\n";
            umba::lout.flush();
            #endif

            return;
        }

        auto startTick = umba::time_service::getCurTimeMs();

        try{
            #if defined(VIEW04_LOG_SQUIRREL_CALLS)
            using umba::lout;
            lout << "try squirrel Game.onPaint\n";
            umba::lout.flush();
            #endif

            ssq::Function sqOnPaint = marty_simplesquirrel::findFunc(vm, "Game.onPaint");

            prepareDrawContext(pDc);

            marty_draw_context::simplesquirrel::DrawingContext sqDc = marty_draw_context::simplesquirrel::DrawingContext(vm.getHandle(), pDc);

            CPoint clientSize = getClientSizePoint();

            sqDc.ctxSizeX = (int)clientSize.x;
            sqDc.ctxSizeY = (int)clientSize.y;

            vm.callFunc(sqOnPaint, vm,  /* appHost,  */ &sqDc);
            //vm.callFunc(sqOnPaint, vm, sqDc);

            setStatusReady();

        } 
        MARTY_DC_IMPL_WIN32_CATCH_LOG_BULKA_EXCEPTIONS_EX(scriptSomethingFailed)
        

        auto endTick = umba::time_service::getCurTimeMs();

        MARTY_ARG_USED(startTick);
        MARTY_ARG_USED(endTick);

        //lout << "OnPaint times: " << (endTick-startTick) << "\n";

    }

};

#endif // __VIEW_H__
