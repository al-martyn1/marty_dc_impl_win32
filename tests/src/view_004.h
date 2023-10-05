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

        bool loadingFailed = false;

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
                // lout << encoding::toUtf8(preparedScriptText1);
                // lout << "\n----------\n\n";
    
                ssq::Script script = vm.compileSource(preparedScriptText1.c_str(), sqScriptFilename.c_str());
    
                vm.run(script);
                
            } catch (ssq::CompileException& e) {
                lout << "Failed to run file: " << e.what() << "\n";
                loadingFailed = true;
                //return -1;
            } catch (ssq::TypeException& e) {
                lout << "Something went wrong passing objects: " << e.what() << "\n";
                loadingFailed = true;
                //return -1;
            } catch (ssq::RuntimeException& e) {
                lout << "Something went wrong during execution: " << e.what() << "\n";
                loadingFailed = true;
                //return -1;
            } catch (ssq::NotFoundException& e) {
                lout << e.what() << "\n";
                loadingFailed = true;
                //return -1;
            } catch (std::out_of_range& e) {
                lout << e.what() << "\n";
                //return -1;
            } catch (...) {
                lout << "Unknown error" << "\n";
                loadingFailed = true;
                //return -1;
            }


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
                    ssq::Function sqOnPaint = marty_simplesquirrel::findFunc(vm, "Game.onLoad");
    
                    auto res = vm.callFunc(sqOnPaint, vm,  /* appHost, */  bFirstTime);
    
                    bool needUpdate = marty_simplesquirrel::fromObjectConvertHelper<bool>(res, _SC("Game::onLoad returned"));
                    if (needUpdate)
                    {
                        invalidateClientArea();
                    }
        
                } catch (ssq::CompileException& e) {
                    lout << "Failed to run file: " << e.what() << "\n";
                    //return -1;
                } catch (ssq::TypeException& e) {
                    lout << "Something went wrong passing objects: " << e.what() << "\n";
                    //return -1;
                } catch (ssq::RuntimeException& e) {
                    lout << "Something went wrong during execution: " << e.what() << "\n";
                    //return -1;
                } catch (ssq::NotFoundException& e) {
                    lout << e.what() << "\n";
                    //return -1;
                } catch (std::out_of_range& e) {
                    lout << e.what() << "\n";
                    //return -1;
                } catch (...) {
                    lout << "Unknown error" << "\n";
                    //return -1;
                }


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

        if (nIDEvent==mainTimerId)
        {
            std::uint32_t tickDelta = getTickDelta();
            updateLastTick();

            bool timerHandlerFails = true;

            try{

                try
                {
                    ssq::Function sqOnPaint = marty_simplesquirrel::findFunc(vm, "Game.onUpdate");
                    auto res = vm.callFunc(sqOnPaint, vm,  /* appHost,  */ tickDelta);
    
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

            } catch (ssq::CompileException& e) {
                lout << "Failed to run file: " << e.what() << "\n";
                //return -1;
            } catch (ssq::TypeException& e) {
                lout << "Something went wrong passing objects: " << e.what() << "\n";
                //return -1;
            } catch (ssq::RuntimeException& e) {
                lout << "Something went wrong during execution: " << e.what() << "\n";
                //return -1;
            } catch (ssq::NotFoundException& e) {
                lout << e.what() << "\n";
                //return -1;
            } catch (std::out_of_range& e) {
                lout << e.what() << "\n";
                //return -1;
            } catch (...) {
                lout << "Unknown error" << "\n";
                //return -1;
            }

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

        try{
            ssq::Function sqOnPaint = marty_simplesquirrel::findFunc(vm, "Game.onKeyEvent");

            auto res = vm.callFunc(sqOnPaint, vm,  /* appHost,  */ bDown, nChar, nRepCnt);

            bool needUpdate = marty_simplesquirrel::fromObjectConvertHelper<bool>(res, _SC("Game::onKeyEvent returned"));
            if (needUpdate)
            {
                invalidateClientArea();
            }

        } catch (ssq::CompileException& e) {
            lout << "Failed to run file: " << e.what() << "\n";
            //return -1;
        } catch (ssq::TypeException& e) {
            lout << "Something went wrong passing objects: " << e.what() << "\n";
            //return -1;
        } catch (ssq::RuntimeException& e) {
            lout << "Something went wrong during execution: " << e.what() << "\n";
            //return -1;
        } catch (ssq::NotFoundException& e) {
            lout << e.what() << "\n";
            //return -1;
        } catch (...) {
            lout << "Unknown error" << "\n";
            //return -1;
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

        auto cx = clientRect.right  - clientRect.left + 1;
        auto cy = clientRect.bottom - clientRect.top  + 1;


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

            auto cx = clientRect.right  - clientRect.left + 1;
            auto cy = clientRect.bottom - clientRect.top  + 1;
            // lout << "OnPaint: cx: " << cx << ", cy: " << cy <<"\n";
            sqDc.ctxSizeX = (int)(cx);
            sqDc.ctxSizeY = (int)(cy);

            vm.callFunc(sqOnPaint, vm,  /* appHost,  */ &sqDc);
            //vm.callFunc(sqOnPaint, vm, sqDc);

        } catch (ssq::CompileException& e) {
            lout << "Failed to run file: " << e.what() << "\n";
            //return -1;
        } catch (ssq::TypeException& e) {
            lout << "Something went wrong passing objects: " << e.what() << "\n";
            //return -1;
        } catch (ssq::RuntimeException& e) {
            lout << "Something went wrong during execution: " << e.what() << "\n";
            //return -1;
        } catch (ssq::NotFoundException& e) {
            lout << e.what() << "\n";
            //return -1;
        } catch (...) {
            lout << "Unknown error" << "\n";
            //return -1;
        }

        auto endTick = umba::time_service::getCurTimeMs();

        MARTY_ARG_USED(startTick);
        MARTY_ARG_USED(endTick);

        //lout << "OnPaint times: " << (endTick-startTick) << "\n";

    }

};

#endif // __VIEW_H__
