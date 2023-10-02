#pragma once

#include "marty_draw_context/multi_dc_impl.h"

#include "gdi_draw_context.h"

#if !defined(MARTY_DRAW_CONTEXT_NO_GDIPLUS)
    #include "gdiplus_draw_context.h"
#endif




namespace marty_draw_context {


struct MultiDrawContextGdi : public MultiDrawContext
{

protected:

    HDC                 m_hdc; // m_dc.m_hDC
    HdcReleaseMode      m_hdcReleaseMode = HdcReleaseMode::doNothing;
    HWND                m_hwnd;


    static
    HDC hdcRelease( HDC hdc
                  , HdcReleaseMode   releaseMode
                  , HWND             hwnd          = 0
                  , LPPAINTSTRUCT    lpPaintStruct = 0
                  )
    {
        switch(releaseMode)
        {
            case HdcReleaseMode::endPaint    : 
            {
                ATLASSERT(hwnd!=0);
                ATLASSERT(lpPaintStruct!=0);

                if (hwnd==0)
                {
                    throw std::runtime_error("hdcRelease(HdcReleaseMode::endPaint): hwnd==0");
                }

                if (lpPaintStruct==0)
                {
                    throw std::runtime_error("hdcRelease(HdcReleaseMode::endPaint): lpPaintStruct==0");
                }

                ::EndPaint(hwnd, lpPaintStruct);

                break;
            }

            case HdcReleaseMode::releaseDc   :
            {
                ATLASSERT(hwnd!=0);

                if (hwnd==0)
                {
                    throw std::runtime_error("hdcRelease(HdcReleaseMode::endPaint): hwnd==0");
                }

                ::ReleaseDC(hwnd, hdc);

                break;
            }

            case HdcReleaseMode::deleteDc    :
            {
                ::DeleteDC(hdc);

                break;
            }

            case HdcReleaseMode::doNothing   :
            case HdcReleaseMode::invalid:    [[fallthrough]];

            default: {}
        }

        return (HDC)0;
    }
    
public:

    

    MultiDrawContextGdi(HDC hdc, HdcReleaseMode hdcReleaseMode=HdcReleaseMode::doNothing, HWND hwnd=(HWND)0)
    : MultiDrawContext()
    , m_hdc(hdc)
    , m_hdcReleaseMode(hdcReleaseMode)
    , m_hwnd(hwnd)
    {
    }

    ~MultiDrawContextGdi()
    {
        m_hdc = hdcRelease(m_hdc, m_hdcReleaseMode, m_hwnd /* , lpPaintStruct */ );
    }

}; // struct MultiDrawContextGdi




inline
MultiDrawContext makeMultiDrawContext(HDC hdc, bool prefferGdiPlus = false, HdcReleaseMode hdcReleaseMode=HdcReleaseMode::doNothing, HWND hwnd=(HWND)0)
{
    MultiDrawContextGdi mdc = MultiDrawContextGdi(hdc, hdcReleaseMode, hwnd);

    std::shared_ptr<IDrawContext> gdiDc   = std::make_shared<GdiDrawContext>(hdc);

    #if !defined(MARTY_DRAW_CONTEXT_NO_GDIPLUS)

        mdc.engineName = "GDI";
        std::shared_ptr<IDrawContext> gdiPlusDc = std::make_shared<GdiPlusDrawContext>(hdc);

    #else

        mdc.engineName = prefferGdiPlus ? "GDI+/GDI" : "GDI/GDI+";
        std::shared_ptr<IDrawContext> gdiPlusDc = gdiDc;

    #endif

    if (prefferGdiPlus)
    {
        mdc.defDc       = gdiPlusDc;
        mdc.pixelDc     = gdiDc    ;
        mdc.textDc      = gdiDc    ;
        mdc.smoothingDc = gdiPlusDc;
    }
    else
    {
        mdc.defDc       = gdiDc    ;
        mdc.pixelDc     = gdiDc    ;
        mdc.textDc      = gdiDc    ;
        mdc.smoothingDc = gdiPlusDc;
    }

    mdc.init();

    return mdc;

}


inline
std::shared_ptr<IDrawContext> makeSharedMultiDrawContext(HDC hdc, bool prefferGdiPlus = false, HdcReleaseMode hdcReleaseMode=HdcReleaseMode::doNothing, HWND hwnd=(HWND)0)
{
    std::shared_ptr<MultiDrawContextGdi> mdc = std::make_shared<MultiDrawContextGdi>(hdc, hdcReleaseMode, hwnd);

    std::shared_ptr<IDrawContext> gdiDc   = std::make_shared<GdiDrawContext>(hdc);

    #if !defined(MARTY_DRAW_CONTEXT_NO_GDIPLUS)

        mdc->engineName = "GDI";
        std::shared_ptr<IDrawContext> gdiPlusDc = std::make_shared<GdiPlusDrawContext>(hdc);

    #else

        mdc->engineName = prefferGdiPlus ? "GDI+/GDI" : "GDI/GDI+";
        std::shared_ptr<IDrawContext> gdiPlusDc = gdiDc;

    #endif

    if (prefferGdiPlus)
    {
        mdc->defDc       = gdiPlusDc;
        mdc->pixelDc     = gdiDc    ;
        mdc->textDc      = gdiDc    ;
        mdc->smoothingDc = gdiPlusDc;
    }
    else
    {
        mdc->defDc       = gdiDc    ;
        mdc->pixelDc     = gdiDc    ;
        mdc->textDc      = gdiDc    ;
        mdc->smoothingDc = gdiPlusDc;
    }

    mdc->init();

    return mdc;

}



} // namespace marty_draw_context


