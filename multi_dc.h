#pragma once

#include "marty_draw_context/multi_dc_impl.h"

#include "gdi_draw_context.h"

#if !defined(MARTY_DRAW_CONTEXT_NO_GDIPLUS)
    #include "gdiplus_draw_context.h"
#endif




namespace marty_draw_context {


inline
MultiDrawContext makeMultiDrawContext(HDC hdc, bool prefferGdiPlus = false)
{
    MultiDrawContext mdc;

    std::shared_ptr<IDrawContext> gdiDc   = std::make_shared<GdiDrawContext>(hdc);

    #if !defined(MARTY_DRAW_CONTEXT_NO_GDIPLUS)

        std::shared_ptr<IDrawContext> gdiPlusDc = std::make_shared<GdiPlusDrawContext>(hdc);

    #else

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
std::shared_ptr<IDrawContext> makeSharedMultiDrawContext(HDC hdc, bool prefferGdiPlus = false)
{
    std::shared_ptr<MultiDrawContext> mdc = std::make_shared<MultiDrawContext>();

    std::shared_ptr<IDrawContext> gdiDc   = std::make_shared<GdiDrawContext>(hdc);

    #if !defined(MARTY_DRAW_CONTEXT_NO_GDIPLUS)

        std::shared_ptr<IDrawContext> gdiPlusDc = std::make_shared<GdiPlusDrawContext>(hdc);

    #else

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


