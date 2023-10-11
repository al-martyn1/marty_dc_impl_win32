#pragma once




#include "marty_draw_context/i_draw_context.h"
#include "draw_context_impl_base.h"

#include <atlgdi.h>
#include <vector>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <map>
#include <utility>


//#if defined(TRACE_)



namespace marty_draw_context {


class GdiDrawContext : public DrawContextImplBase // IDrawContext
{

public:

    typedef marty_draw_context::DrawCoord  DrawCoord;
    typedef marty_draw_context::DrawScale  DrawScale;
    typedef marty_draw_context::DrawPoint  DrawPoint;
    typedef marty_draw_context::DrawSize   DrawSize ;
    typedef marty_draw_context::ColorRef   ColorRef ;


protected:

    // CDCHandle m_dc; // non-managed GDI Handle class object
    HDC                 m_hdc; // m_dc.m_hDC
    HdcReleaseMode      m_hdcReleaseMode = HdcReleaseMode::doNothing;
    HWND                m_hwnd;

    bool      m_pathStarted;

    DrawCoord m_curPos;

    std::vector<HPEN> m_hPens;
    HPEN              m_defGdiPen;
    int               m_curPenId;

    std::vector<HBRUSH> m_hBrushes;
    HBRUSH              m_defGdiBrush;
    int                 m_curBrushId;

    std::vector<HFONT> m_hFonts;
    HFONT              m_defGdiFont;
    int                m_curFontId;


    GdiDrawContext( const GdiDrawContext& ) = delete;
    GdiDrawContext& operator=( const GdiDrawContext& ) = delete;


    DWORD getNativeLineEndcapFlags(marty_draw_context::LineEndcapStyle style )
    {
        switch(style)
        {
            case marty_draw_context::LineEndcapStyle::round  : return PS_ENDCAP_ROUND;
            case marty_draw_context::LineEndcapStyle::square : return PS_ENDCAP_SQUARE;
            case marty_draw_context::LineEndcapStyle::flat   : [[fallthrough]];
            case marty_draw_context::LineEndcapStyle::invalid: [[fallthrough]];

            default:                                          return PS_ENDCAP_FLAT;
        }
    }

    DWORD getNativeLineJoinFlags(marty_draw_context::LineJoinStyle style )
    {
        switch(style)
        {
            case marty_draw_context::LineJoinStyle::mitter : return PS_JOIN_MITER;
            case marty_draw_context::LineJoinStyle::round  : return PS_JOIN_ROUND;
            case marty_draw_context::LineJoinStyle::bevel  : [[fallthrough]];
            case marty_draw_context::LineJoinStyle::invalid: [[fallthrough]];
            default:                                        return PS_JOIN_BEVEL;
        };
    }


    void init()
    {
        //SetTextAlign (m_dc.m_hDC, GetTextAlign(m_dc.m_hDC) & (~TA_CENTER) | TA_LEFT );

        // получили текущие (дефолтные) перо/кисть/фонт, замещая их стоковыми
        m_defGdiPen   = (HPEN)  ::SelectObject(m_hdc, ::GetStockObject(NULL_PEN));
        m_defGdiBrush = (HBRUSH)::SelectObject(m_hdc, ::GetStockObject(NULL_BRUSH));
        m_defGdiFont  = (HFONT )::SelectObject(m_hdc, ::GetStockObject(DEVICE_DEFAULT_FONT));

        // восстанавливаем на всякий случай те объекты, которые были в HDC
        ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiPen  );
        ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiBrush);
        ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiFont );
        
        //m_curPenId   = createSolidPen( 0, marty_draw_context::LineEndcapStyle::square, marty_draw_context::LineJoinStyle::bevel, 0, 0, 0 );
        //m_curBrushId = createSolidBrush( 0, 0, 0 );

    }

public:

// HDC       m_hdc; // m_dc.m_hDC

    GdiDrawContext(const CDCHandle &dc, HdcReleaseMode hdcReleaseMode=HdcReleaseMode::doNothing, HWND hwnd=(HWND)0)
    : DrawContextImplBase()
    , m_hdc(dc.m_hDC)
    , m_hdcReleaseMode(hdcReleaseMode)
    , m_hwnd(hwnd)
    , m_pathStarted(false)
    , m_curPos()
    , m_hPens()
    , m_defGdiPen(0)
    , m_curPenId(-1)
    , m_hBrushes()
    , m_defGdiBrush(0)
    , m_curBrushId(-1)
    , m_hFonts()
    , m_defGdiFont(0)
    , m_curFontId(-1)
    {
        init();
    }

    GdiDrawContext(const CDC &dc, HdcReleaseMode hdcReleaseMode=HdcReleaseMode::doNothing, HWND hwnd=(HWND)0)
    : DrawContextImplBase()
    , m_hdc(dc.m_hDC)
    , m_hdcReleaseMode(hdcReleaseMode)
    , m_hwnd(hwnd)
    , m_pathStarted(false)
    , m_curPos()
    , m_hPens()
    , m_defGdiPen(0)
    , m_curPenId(-1)
    , m_hBrushes()
    , m_defGdiBrush(0)
    , m_curBrushId(-1)
    , m_hFonts()
    , m_defGdiFont(0)
    , m_curFontId(-1)
    {
        init();
    }

    GdiDrawContext(HDC hdc, HdcReleaseMode hdcReleaseMode=HdcReleaseMode::doNothing, HWND hwnd=(HWND)0)
    : DrawContextImplBase()
    , m_hdc(hdc)
    , m_hdcReleaseMode(hdcReleaseMode)
    , m_hwnd(hwnd)
    , m_pathStarted(false)
    , m_curPos()
    , m_hPens()
    , m_defGdiPen(0)
    , m_curPenId(-1)
    , m_hBrushes()
    , m_defGdiBrush(0)
    , m_curBrushId(-1)
    , m_hFonts()
    , m_defGdiFont(0)
    , m_curFontId(-1)
    {
        init();
    }



    ~GdiDrawContext()
    {
        // ::SelectObject(m_hdc, ::GetStockObject(NULL_PEN));
        // ::SelectObject(m_hdc, ::GetStockObject(NULL_BRUSH));
        // ::SelectObject(m_hdc, ::GetStockObject(DEVICE_DEFAULT_FONT));

        freeAllocatedRc();

        m_hdc = hdcRelease(m_hdc, m_hdcReleaseMode, m_hwnd /* , lpPaintStruct */ );

    }

    virtual void freeAllocatedRc() override
    {
        DcResourcesState st;
        st.nPens      =  0;
        st.penId      = -1;
        st.nBrushes   =  0;
        st.brushId    = -1;
        st.nFonts     =  0;
        st.fontId     = -1;
        st.textColor  = ColorRef::fromUnsigned(0);
        st.bkColor    = ColorRef::fromUnsigned(0x00FFFFFF);

        // Это - не нужно, 
        // ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiPen  );
        // ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiBrush);
        // ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiFont );

        DrawContextImplBase::freeAllocatedRc();

        restoreResourcesState(st);

        /*
        // Чистим ручки
        {
            //SelectObject(m_hdc, (HGDIOBJ)m_defGdiPen);
            for( auto hpen : m_hPens )
                auto res = ::DeleteObject((HGDIOBJ)hpen);
            m_hPens.clear();
            m_curPenId = -1;
        }

        {
            //SelectObject(m_hdc, (HGDIOBJ)m_defGdiBrush);
            for( auto hbrush : m_hBrushes )
                DeleteObject((HGDIOBJ)hbrush);
            m_hBrushes.clear();
            m_curBrushId = -1;
        }

        {
            // if (m_defGdiFont!=0)
            //     SelectObject(m_hdc, (HGDIOBJ)m_defGdiFont);
            for( auto hfont : m_hFonts )
                DeleteObject((HGDIOBJ)hfont);
            m_hFonts.clear();
            m_curFontId = -1;
        }

        init();
        */
    }

    virtual void restoreResourcesState(const DcResourcesState &rcState) override
    {
        std::size_t nPens     = (std::size_t)rcState.nPens;
        std::size_t nBrushes  = (std::size_t)rcState.nBrushes;
        std::size_t nFonts    = (std::size_t)rcState.nFonts;

        if (((std::size_t)rcState.penId)<nPens)
        {
            //m_curPenId = rcState.penId;
            selectPen(rcState.penId);
        }
        else
        {
            //m_curPenId = -1;
            selectPen(-1);
        }

        if (((std::size_t)rcState.brushId)<nBrushes)
        {
            //m_curBrushId = rcState.brushId;
            selectBrush(rcState.brushId);
        }
        else
        {
            //m_curBrushId = -1;
            selectBrush(-1);
        }

        if (((std::size_t)rcState.fontId)<nFonts)
        {
            //m_curFontId = rcState.fontId;
            selectFont(rcState.fontId);
        }
        else
        {
            //m_curFontId = -1;
            selectFont(-1);
        }

        

        setTextColor(rcState.textColor);
        setBkColor(rcState.bkColor);

        if (nPens<m_hPens.size())
        {
            for(std::size_t i=nPens; i!=m_hPens.size(); ++i)
            {
                auto res = ::DeleteObject((HGDIOBJ)m_hPens[i]);
                MARTY_ARG_USED(res);
                ATLASSERT(res);
            }

            m_hPens.resize(nPens);
        }

        if (nBrushes<m_hBrushes.size())
        {
            for(std::size_t i=nBrushes; i!=m_hBrushes.size(); ++i)
            {
                auto res = ::DeleteObject((HGDIOBJ)m_hBrushes[i]);
                MARTY_ARG_USED(res);
                ATLASSERT(res);
            }

            m_hBrushes.resize(nBrushes);
        }

        if (nFonts<m_hFonts.size())
        {
            for(std::size_t i=nFonts; i!=m_hFonts.size(); ++i)
            {
                auto res = ::DeleteObject((HGDIOBJ)m_hFonts[i]);
                MARTY_ARG_USED(res);
                ATLASSERT(res);
            }
            m_hFonts.resize(nFonts);
        }

    }

    virtual DcResourcesState getResourcesState() override
    {
        DcResourcesState rcState;

        rcState.nPens     = (int)m_hPens.size();
        rcState.penId     = (int)m_curPenId;
        rcState.nBrushes  = (int)m_hBrushes.size();
        rcState.brushId   = (int)m_curBrushId;
        rcState.nFonts    = (int)m_hFonts.size();
        rcState.fontId    = (int)m_curFontId;

        rcState.textColor = getTextColor();
        rcState.bkColor   = getBkColor();

        return rcState;
    }

    virtual std::string getEngineName() override
    {
        return std::string("GDI");
    }

    virtual void flushBits() override
    {
    }

    virtual int getCharWidthIntImpl(std::uint32_t ch32) const override
    {
        INT tmpW = 0;
        if (!::GetCharWidth32W(m_hdc, ch32, ch32, &tmpW))
        {
            return 0;
        }

        return tmpW;
    }

    bool getCharWidth (std::uint32_t charCode, float_t &w) const
    {
        // FLOAT f = 0.0f;
        // if (!::GetCharWidthFloatW(m_hdc, charCode, charCode, &f))
        INT tmpW = 0;
        if (!::GetCharWidth32W(m_hdc, charCode, charCode, &tmpW))
        {
            return false;
        }

        // float_t scale = 1.0;
        // FontParamsW fpw;
        // if (m_curFontId>=0 && getFontParamsById(m_curFontId, fpw))
        // {
        //     //scale = mapRawToLogicSize( DrawCoord{fpw.height,fpw.height} ).y;
        //     //scale = getScaledSize( DrawCoord{fpw.height,fpw.height} ).y;
        //     scale = fpw.height;
        // }
        //float_t scale = mapRawToLogicSize( DrawCoord{tmpW,tmpW} ).y;
        
        // w = mapRawToLogicSize( DrawCoord{f,f} ).y;
        //bool getFontParamsById( int id, marty_draw_context::FontParamsW &fp ) override
        //w = scale*f;

        w = mapRawToLogicSizeX(tmpW);

        return true;
    }

    virtual bool getCharWidths(std::vector<float_t> &widths, const wchar_t *text, std::size_t textSize=(std::size_t)-1, int fontId=-1 /* use current font */ ) const override
    {
        textSize = checkCalcStringSize(text, textSize);

        widths.clear();
        auto totalCharsN = getTextCharsLen(text, textSize);
        if (!totalCharsN)
        {
            return false; // или true? Всё ока, просто нет символов в строке?
        }

        widths.reserve(totalCharsN); // резервируем место, чтобы не было аллокаций, и не стрельнуло исключение

        int prevFont = -1;
        if (fontId>=0)
        {
            GdiDrawContext *pNcThis = const_cast<GdiDrawContext*>(this);
            prevFont = pNcThis->selectFont(fontId);
        }

        bool bRes = true;
        std::unordered_map<std::uint32_t, float_t> cache;

        std::size_t curCharLen = getCharLen(text, textSize);
        while(curCharLen && textSize)
        {
            //sizeTotal += curCharLen;
            std::uint32_t ch32 = getChar32(text, textSize);

            std::unordered_map<std::uint32_t, float_t>::const_iterator cit = cache.find(ch32);
            if (cit!=cache.end())
            {
                widths.emplace_back(cit->second);
            }
            else
            {
                float_t w = 0.0f;
                if (!getCharWidth(ch32, w))
                {
                    bRes = false;
                    break;
                }

                cache[ch32] = w;
                widths.emplace_back(w);
            }

            text      += curCharLen;
            textSize  -= curCharLen;
            curCharLen = getCharLen(text, textSize);
        }

        if (fontId>=0)
        {
            GdiDrawContext *pNcThis = const_cast<GdiDrawContext*>(this);
            pNcThis->selectFont(prevFont);
        }

        return bRes;

    }

    virtual bool getSimpleFontMetrics(SimpleFontMetrics &m, int fontId=-1) const override
    {
        int prevFont = -1;
        if (fontId>=0)
        {
            GdiDrawContext *pNcThis = const_cast<GdiDrawContext*>(this);
            prevFont = pNcThis->selectFont(fontId);
        }

        TEXTMETRICW textMetric;
        bool bRes = ::GetTextMetrics(m_hdc, &textMetric) ? true : false;
        if (bRes)
        {
            m.height    = mapRawToLogicSizeX(textMetric.tmHeight);
            m.ascent    = mapRawToLogicSizeX(textMetric.tmAscent);
            m.descent   = mapRawToLogicSizeX(textMetric.tmDescent);
            m.overhang  = mapRawToLogicSizeX(textMetric.tmOverhang);
        }

        if (fontId>=0)
        {
            GdiDrawContext *pNcThis = const_cast<GdiDrawContext*>(this);
            pNcThis->selectFont(prevFont);
        }

        return bRes;
    }

    virtual DrawSize calcDrawnTextSizeExact (int   fontId         , const char*    text, std::size_t nChars) override
    {
        nChars = checkCalcStringSize(text, nChars);
        std::string  strText  = std::string(text,nChars);
        std::wstring wStrText = decodeString(strText);

        return calcDrawnTextSizeExact(fontId, wStrText.data(), wStrText.size());
    }

    virtual DrawSize calcDrawnTextSizeExact (int   fontId         , const wchar_t* text, std::size_t nChars) override
    {
        FontSaver fontSaver(this, fontId);

        SIZE calculatedSz = { 0, 0 };

        ::GetTextExtentPoint32W( m_hdc, text, (int)nChars, &calculatedSz );

        return mapRawToLogicSize( DrawCoord{calculatedSz.cx,calculatedSz.cy} );
    }



    virtual void logFontsInfo() override
    {
        DC_LOG()<<"---------\nTotal fonts: "<<m_hFonts.size()<<"\n";

        for(auto i=0u; i!=m_hFonts.size(); ++i)
        {
            DC_LOG()<<"---\nFont #"<<i<<"\n";
        }
        //DC_LOG()

        //std::vector<HFONT> m_hFonts;
    }


    virtual int createSolidPen( float_t width, marty_draw_context::LineEndcapStyle endcaps, marty_draw_context::LineJoinStyle join
                              , const ColorRef &rgb
                              ) override
    {
        return createSolidPen( width, endcaps, join
                             , rgb.r, rgb.g, rgb.b
                             );
    }

    virtual int createSolidPen( float_t width, marty_draw_context::LineEndcapStyle endcaps, marty_draw_context::LineJoinStyle join
                              , std::uint8_t r, std::uint8_t g, std::uint8_t b
                              ) override // Geometric pen
    {
        marty_draw_context::PenParamsWithColor penParams = { width, endcaps, join, ColorRef{r,g,b} };

        int existingPenId = findPenByParams(penParams);
        if (existingPenId>=0)
             return existingPenId;

        float_t scaledWidth = m_penScale*width;

        LOGBRUSH lb;
        lb.lbStyle = BS_SOLID;
        lb.lbColor = RGB(r,g,b);
        lb.lbHatch = 0;

        DWORD penStyle = PS_SOLID ;

        HPEN hpen = 0;

        if (width<=0.01)
        {
            penStyle |= PS_COSMETIC | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;
            hpen = ExtCreatePen( penStyle
                               , (DWORD)1u // pen width
                               , &lb // LOGBRUSH
                               , 0, 0 // not a user styled pen
                               );
        }
        else
        {
            penStyle |= PS_GEOMETRIC | PS_INSIDEFRAME | getNativeLineEndcapFlags(endcaps) | getNativeLineJoinFlags(join);
            hpen = ExtCreatePen( penStyle
                               , (DWORD)int(floatToInt(scaledWidth))
                               , &lb // LOGBRUSH
                               , 0, 0 // not a user styled pen
                               );
        }

        if (hpen==0)
            return -1;

        int penIdx = (int)m_hPens.size();

        m_hPens.push_back(hpen);

        savePenByParams(penParams, penIdx);

        return penIdx;

    }

    virtual void drawCircleInLogicalCoordsForFillGradientCircle
                                 ( int x
                                 , int y
                                 , int r
                                 , const marty_draw_context::ColorRef &clr
                                 ) override
    {
        LOGBRUSH lb;
        lb.lbStyle = BS_SOLID;
        lb.lbColor = RGB(clr.r, clr.g, clr.b);
        lb.lbHatch = 0;

        DWORD penStyle = PS_SOLID | PS_COSMETIC | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;
        HPEN hpen = ExtCreatePen( penStyle
                                , 1 // pen width
                                , &lb // LOGBRUSH
                                , 0, 0 // not a user styled pen
                                );

        HGDIOBJ hPrevPen = SelectObject(m_hdc, (HGDIOBJ)hpen);

        HBRUSH hbrush = CreateSolidBrush(RGB(clr.r, clr.g, clr.b));

        HGDIOBJ hPrevBrush = SelectObject(m_hdc, (HGDIOBJ)hbrush);

        // MoveToEx(m_hdc, left , top   , 0);
        // LineTo  (m_hdc, right, bottom);
        auto left   = x-r;
        auto top    = y-r;
        auto right  = x+r; // +1;
        auto bottom = y+r; // +1;
        ::Ellipse(m_hdc, left, top, right, bottom);

        SelectObject(m_hdc, hPrevPen);
        SelectObject(m_hdc, hPrevBrush);

        DeleteObject((HGDIOBJ)hpen);
    }

    virtual void drawLineInLogicalCoordsForFillGradientRect
                                 ( int left
                                 , int top
                                 , int right
                                 , int bottom
                                 , const marty_draw_context::ColorRef &clr
                                 ) override
    {
        LOGBRUSH lb;
        lb.lbStyle = BS_SOLID;
        lb.lbColor = RGB(clr.r, clr.g, clr.b);
        lb.lbHatch = 0;

        DWORD penStyle = PS_SOLID | PS_COSMETIC | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;
        HPEN hpen = ExtCreatePen( penStyle
                                , 1 // pen width
                                , &lb // LOGBRUSH
                                , 0, 0 // not a user styled pen
                                );

        HGDIOBJ hPrevPen = SelectObject(m_hdc, (HGDIOBJ)hpen);

        MoveToEx(m_hdc, left , top   , 0);
        LineTo  (m_hdc, right, bottom);

        SelectObject(m_hdc, hPrevPen);

        DeleteObject((HGDIOBJ)hpen);
    }


    virtual int createSolidBrush( const marty_draw_context::ColorRef &rgb ) override
    {
        return createSolidBrush( rgb.r, rgb.g, rgb.b );
    }

    virtual int createSolidBrush( std::uint8_t r, std::uint8_t g, std::uint8_t b ) override
    {
        marty_draw_context::ColorRef brushColorRef = {r,g,b};
        int existingBrushId = findBrushByParams(brushColorRef);
        if (existingBrushId>=0)
             return existingBrushId;

        HBRUSH hBrush = CreateSolidBrush( RGB(r,g,b) );

        if (hBrush==0)
            return -1;

        int brushIdx = (int)m_hBrushes.size();

        m_hBrushes.push_back(hBrush);

        saveBrushByParams(brushColorRef,brushIdx);

        return brushIdx;
    }


    HFONT createFontImpl( int height, int escapement, int orientation, int weight, marty_draw_context::FontStyleFlags fontStyleFlags, const char *fontFace )
    {
        LOGFONTA lf;
        fillLogfontStruct( lf, height, escapement, orientation, weight, fontStyleFlags, fontFace );

        // int iff=0;
        // for(; iff<(LF_FACESIZE-1) && fontFace[iff] /* *fontFace */ ; ++iff)
        //     lf.lfFaceName[iff] = fontFace[iff];
        // lf.lfFaceName[iff] = 0;

        return CreateFontIndirectA(&lf);
    }

    HFONT createFontImpl( int height, int escapement, int orientation, int weight, marty_draw_context::FontStyleFlags fontStyleFlags, const wchar_t *fontFace )
    {
        LOGFONTW lf;
        fillLogfontStruct( lf, height, escapement, orientation, weight, fontStyleFlags, fontFace );

        // int iff=0;
        // for(; iff<(LF_FACESIZE-1) && fontFace[iff] /* *fontFace */; ++iff)
        //     lf.lfFaceName[iff] = fontFace[iff];
        // lf.lfFaceName[iff] = 0;

        return CreateFontIndirectW(&lf);
    }

    virtual int createFont( float_t height, int escapement, int orientation, marty_draw_context::FontWeight weight, marty_draw_context::FontStyleFlags fontStyleFlags, const char *fontFace ) override
    {
        HFONT hfont = createFontImpl( int(floatToInt(getScaledSize(DrawCoord(height,height)).y))
                                    , escapement, orientation, (int)weight, fontStyleFlags, fontFace );
        if (hfont==0)
            return -1;

        int fontIdx = (int)m_hFonts.size();

        m_hFonts.push_back(hfont);
        auto fontParamsW = makeFontParamsW(marty_draw_context::FontParamsA{height, escapement, orientation, weight, fontStyleFlags, fontFace} );

        fontParamsW.monospaceFontCharWidth = -1;
        fontParamsW.monospaceFontCharWidth = getMonospacedFontCharWidthApprox(fontParamsW);

        fontParamsW.proportionalFontScale = -1;
        fontParamsW.proportionalFontScale = getProportionalFontCharWidthScaleApprox(fontParamsW);

        fontsByParams  [ fontParamsW ] = fontIdx;
        fontsParamsById[ fontIdx ]     = fontParamsW;

        return fontIdx;
    }

    virtual int createFont( float_t height, int escapement, int orientation, marty_draw_context::FontWeight weight, marty_draw_context::FontStyleFlags fontStyleFlags, const wchar_t *fontFace ) override
    {
        HFONT hfont = createFontImpl( int(floatToInt(getScaledSize(DrawCoord(height,height)).y))
                                    , escapement, orientation, (int)weight, fontStyleFlags, fontFace );
        if (hfont==0)
            return -1;

        int fontIdx = (int)m_hFonts.size();

        m_hFonts.push_back(hfont);
        auto fontParamsW = marty_draw_context::FontParamsW{ (marty_draw_context::FontParamsW::font_height_t)height, escapement, orientation, weight, fontStyleFlags, std::wstring(fontFace)};

        fontParamsW.monospaceFontCharWidth = -1;
        fontParamsW.monospaceFontCharWidth = getMonospacedFontCharWidthApprox(fontParamsW);

        fontsByParams  [ fontParamsW ] = fontIdx;
        fontsParamsById[ fontIdx ]     = fontParamsW;

        return fontIdx;
    }

    virtual int  createFont( const marty_draw_context::FontParamsA &fp ) override
    {
        return createFont( fp.height, fp.escapement, fp.orientation, fp.weight, fp.fontStyleFlags, fp.fontFace.c_str() );
    }

    virtual int  createFont( const marty_draw_context::FontParamsW &fp ) override
    {
        return createFont( fp.height, fp.escapement, fp.orientation, fp.weight, fp.fontStyleFlags, fp.fontFace.c_str() );
    }

    virtual int selectFont( int fontId ) override
    {
        if (fontId<0 || fontId>=(int)m_hFonts.size())
        {
            ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiFont );
            fontId = -1;
        }
        else
        {
            ::SelectObject(m_hdc, (HGDIOBJ)m_hFonts[(std::size_t)fontId]);
        }

        //if (m_defGdiFont==0)
        //    m_defGdiFont = prevFont;

        std::swap(fontId,m_curFontId);
        return fontId;
    }

    virtual int  getCurFont() override
    {
        return m_curFontId;
    }

    virtual bool textOut( const DrawCoord &pos, const std::wstring &text ) override
    {
        if (text.empty())
        {
            return true;
        }

        auto scaledPos = getScaledPos(pos);
        //DC_LOG()<<"textOut at "<<scaledPos<<"\n";
        return TextOutW(m_hdc, int(floatToInt(scaledPos.x)), int(floatToInt(scaledPos.y)), text.data(), (int)text.size() ) ? true : false;
    }

    virtual bool textOut( const DrawCoord &pos, const wchar_t *text, std::size_t textSize=(std::size_t)-1 ) override
    {
        textSize = checkCalcStringSize(text, textSize);
        if (!textSize)
        {
            return true;
        }

        auto scaledPos = getScaledPos(pos);
        return TextOutW(m_hdc, int(floatToInt(scaledPos.x)), int(floatToInt(scaledPos.y)), text, (int)textSize ) ? true : false;
    }

    virtual marty_draw_context::ColorRef setTextColor( std::uint8_t r, std::uint8_t g, std::uint8_t b ) override
    {
        auto prevColor = SetTextColor(m_hdc, RGB(r,g,b) );
        return marty_draw_context::ColorRef{GetRValue(prevColor), GetGValue(prevColor), GetBValue(prevColor)};
    }

    virtual marty_draw_context::ColorRef setBkColor( std::uint8_t r, std::uint8_t g, std::uint8_t b ) override
    {
        auto prevColor = SetBkColor(m_hdc, RGB(r,g,b) );
        return marty_draw_context::ColorRef{GetRValue(prevColor), GetGValue(prevColor), GetBValue(prevColor)};
    }

    virtual marty_draw_context::ColorRef setTextColor( const marty_draw_context::ColorRef &rgb ) override
    {
        return setTextColor( rgb.r, rgb.g, rgb.b );
    }

    virtual marty_draw_context::ColorRef setBkColor( const marty_draw_context::ColorRef &rgb ) override
    {
        return setBkColor( rgb.r, rgb.g, rgb.b );
    }

    virtual ColorRef getBkColor() override
    {
        auto prevColor = SetBkColor(m_hdc, RGB(0,0,0) );
        SetBkColor(m_hdc, prevColor );
        return marty_draw_context::ColorRef{GetRValue(prevColor), GetGValue(prevColor), GetBValue(prevColor)};
    }

    virtual marty_draw_context::ColorRef getTextColor( ) override
    {
        auto prevColor = SetTextColor(m_hdc, RGB(0,0,0) ); // get prev color
        SetTextColor(m_hdc, prevColor ); // restore color
        return marty_draw_context::ColorRef{GetRValue(prevColor), GetGValue(prevColor), GetBValue(prevColor)};
    }



    virtual marty_draw_context::BkMode setBkMode(marty_draw_context::BkMode mode ) override
    {
        return (SetBkMode(m_hdc, mode== marty_draw_context::BkMode::opaque ? OPAQUE : TRANSPARENT )==OPAQUE) 
             ? marty_draw_context::BkMode::opaque 
             : marty_draw_context::BkMode::transparent;
    }

    virtual marty_draw_context::BkMode getBkMode() override
    {
        auto mode = SetBkMode(m_hdc, TRANSPARENT);
        SetBkMode(m_hdc, mode);
        return mode==OPAQUE ? marty_draw_context::BkMode::opaque : marty_draw_context::BkMode::transparent;
    }


    virtual int selectBrush( int brushId ) override
    {
        if (brushId<0 || brushId>=(int)m_hBrushes.size())
        {
            ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiBrush);
            brushId = -1;
        }
        else
        {
            ::SelectObject(m_hdc, (HGDIOBJ)m_hBrushes[(std::size_t)brushId]);
        }

        std::swap(m_curBrushId,brushId);

        return brushId;
    }

    virtual int getCurBrush() override
    {
        return m_curBrushId;
    }

        
    virtual int selectPen( int penId ) override
    {
        if (penId<0 || penId>=(int)m_hPens.size())
        {
            ::SelectObject(m_hdc, (HGDIOBJ)m_defGdiPen  );
            penId = -1;
        }
        else
        {
            ::SelectObject(m_hdc, (HGDIOBJ)m_hPens[(std::size_t)penId]);
        }

        std::swap(m_curPenId,penId);

        return penId;
    }

    virtual int getCurPen() override
    {
        return m_curPenId;
    }


    virtual DrawCoord getCurPos( ) override
    {
        return m_curPos;
    }

    virtual bool beginPath()                                override
    {
        ATLASSERT(m_pathStarted==false);
        if (BeginPath(m_hdc))
        {
            m_pathStarted = true;
            return true;
        }

        return false;
    }

    virtual bool beginPath( const DrawCoord &to )           override
    {
        if (!beginPath())
            return false;

        return moveTo(to);
    }


    virtual bool endPath( bool bStroke = true, bool bFill = false ) override
    {
        ATLASSERT(m_pathStarted!=false);

        if (!EndPath(m_hdc))
            return false;

        m_pathStarted = false;

        bool res = true;

        if (bFill && bStroke)
        {
            if (!StrokeAndFillPath(m_hdc))
                res = false;
        }
        else if (bFill)
        {
            if (!FillPath(m_hdc))
                res = false;
        }
        else if (bStroke)
        {
            if (!StrokePath(m_hdc))
                res = false;
        }

        return res;

    }

    virtual bool closeFigure()                              override
    {
        ATLASSERT(m_pathStarted!=false);
        return CloseFigure(m_hdc) ? true : false;
    }

    virtual bool isPathStarted() override
    {
        return m_pathStarted ? true : false;
    }


    virtual bool moveTo( const DrawCoord &to ) override
    {
        auto scaledCoordTo = getScaledPos(to);

        auto fx = floatToInt(scaledCoordTo.x);
        auto fy = floatToInt(scaledCoordTo.y);
        auto ix = int(fx);
        auto iy = int(fy);

        // DC_LOG() << "GdiDc::moveTo\n";
        // DC_LOG() << "  fx: " << fx << "\n";
        // DC_LOG() << "  fy: " << fy << "\n";
        // DC_LOG() << "  ix: " << ix << "\n";
        // DC_LOG() << "  iy: " << iy << "\n";
        // DC_LOG() << "\n";

        // using umba::lout;
        // using namespace umba::omanip;
        // lout << "CLR1: " << hex << clr1.toUnsigned() << "\n";


        if (MoveToEx(m_hdc, ix, iy, 0 ))
        {
            m_curPos = to;
            return true;
        }

        return false;
    }

    virtual bool lineTo( const DrawCoord &to ) override
    {
        auto scaledCoordTo = getScaledPos(to);
        if (LineTo(m_hdc, int(floatToInt(scaledCoordTo.x)), int(floatToInt(scaledCoordTo.y)) ))
        {
            m_curPos = to;
            return true;
        }

        return false;
    }

    virtual bool ellipticArcTo( const DrawCoord &leftTop
                              , const DrawCoord &rightBottom
                              , const DrawCoord &arcStartRefPoint
                              , const DrawCoord &arcEndRefPoint    // как бы ref point, но не в ней ли дуга закончится должна?
                              , bool             directionCounterclockwise
                              ) override
    {
        DrawCoord leftTopSc           = getScaledPos(leftTop         );
        DrawCoord rightBottomSc       = getScaledPos(rightBottom     );
        DrawCoord arcStartRefPointSc  = getScaledPos(arcStartRefPoint);
        DrawCoord arcEndRefPointSc    = getScaledPos(arcEndRefPoint  );

        auto ellipseCenterPos = (leftTop+rightBottom) / DrawCoord(2,2);

        markerAdd(ellipseCenterPos);

        if (SetArcDirection(m_hdc, directionCounterclockwise ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE )==FALSE)
            return false;

        //TODO: !!! Тут единички всегда прибавлять или только когда размер четный?
        if (!ArcTo(m_hdc, int(floatToInt(leftTopSc.x))              , int(floatToInt(leftTopSc.y))
                       , int(floatToInt(rightBottomSc.x))+1        , int(floatToInt(rightBottomSc.y))+1
                       , int(floatToInt(arcStartRefPointSc.x))     , int(floatToInt(arcStartRefPointSc.y))
                       , int(floatToInt(arcEndRefPointSc.x))       , int(floatToInt(arcEndRefPointSc.y))
                       )
           )
           return false;

        // Тут текущая позиция у нас потеряла синхронизацию с тем, что в GDI

        // if (m_dc.LineTo( int(floatToInt(scaledCoordTo.x)), int(floatToInt(scaledCoordTo.y)) ))
        // {
        //     m_curPos = to;
        //     return true;
        // }


        return true;
    }

    virtual bool ellipse    (const DrawCoord &lt, const DrawCoord &rb) override
    {
        auto ltSc = getScaledPos(lt);
        auto rbSc = getScaledPos(rb);

        HBRUSH transperrantBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
        HBRUSH prevBrush         = (HBRUSH)SelectObject( m_hdc, (HGDIOBJ)transperrantBrush);
        ::Ellipse(m_hdc, floatToInt(ltSc.x), floatToInt(ltSc.y), floatToInt(rbSc.x), floatToInt(rbSc.y));
        SelectObject( m_hdc, (HGDIOBJ)prevBrush);

        return true;
    }

    virtual bool fillEllipse(const DrawCoord &lt, const DrawCoord &rb, bool drawFrame) override
    {
        auto ltSc = getScaledPos(lt);
        auto rbSc = getScaledPos(rb);

        HPEN prevPen = 0;
        if (!drawFrame)
        {
            HPEN transperrantPen = (HPEN)::GetStockObject(NULL_PEN);
            prevPen         = (HPEN)SelectObject( m_hdc, (HGDIOBJ)transperrantPen);
        }

        ::Ellipse(m_hdc, floatToInt(ltSc.x), floatToInt(ltSc.y), floatToInt(rbSc.x), floatToInt(rbSc.y));

        if (!drawFrame)
        {
            SelectObject( m_hdc, (HGDIOBJ)prevPen);
        }

        return true;
    }

    virtual bool circle    (const DrawCoord &centerPos, const DrawCoord::value_type &r) override
    {
        auto cpSc = getScaledPos (centerPos);
        cpSc.x -= 1;
        auto rSc  = getScaledSize(DrawCoord(r,r));
        auto ltSc = cpSc - rSc;
        auto rbSc = cpSc + rSc;

        HBRUSH transperrantBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
        HBRUSH prevBrush         = (HBRUSH)SelectObject( m_hdc, (HGDIOBJ)transperrantBrush);
        ::Ellipse(m_hdc, floatToInt(ltSc.x), floatToInt(ltSc.y), floatToInt(rbSc.x), floatToInt(rbSc.y));

        #if 0
        {
            auto x = floatToInt(cpSc.x);
            auto y = floatToInt(cpSc.y);
            auto le = x-9;
            auto to = y-9;
            auto ri = x+9;
            auto bo = y+9;

            ::Ellipse(m_hdc, le, to, ri, bo);
        }
        #endif


        SelectObject( m_hdc, (HGDIOBJ)prevBrush);

        return true;
    }

    virtual bool fillCircle(const DrawCoord &centerPos, const DrawCoord::value_type &r, bool drawFrame) override
    {
        auto cpSc = getScaledPos (centerPos);
        cpSc.x -= 1;
        auto rSc  = getScaledSize(DrawCoord(r,r));
        auto ltSc = cpSc - rSc;
        auto rbSc = cpSc + rSc;

        HPEN prevPen = 0;
        if (!drawFrame)
        {
            HPEN transperrantPen = (HPEN)::GetStockObject(NULL_PEN);
            prevPen         = (HPEN)SelectObject( m_hdc, (HGDIOBJ)transperrantPen);
        }

        ::Ellipse(m_hdc, floatToInt(ltSc.x), floatToInt(ltSc.y), floatToInt(rbSc.x), floatToInt(rbSc.y));

        if (!drawFrame)
        {
            SelectObject( m_hdc, (HGDIOBJ)prevPen);
        }

        return true;
    }


    // У RoundRect'ов очень плохо с симетричностью
    // Тут про GDI+ - https://www.codeproject.com/Articles/27228/A-class-for-creating-round-rectangles-in-GDI-with

    virtual bool fillRoundRect( const DrawCoord::value_type &cornersR
                          , const DrawCoord             &leftTop
                          , const DrawCoord             &rightBottom
                          , bool                         drawFrame
                          ) override
    {
        MARTY_ARG_USED(drawFrame);

        // auto wh    = DrawCoord(rightBottom.x - leftTop.x, rightBottom.y - leftTop.y);
        // auto wh_2  = DrawCoord(wh.x/2.0, wh.y/2.0);
        // auto rMax  = std::min(wh_2.x, wh_2.y);
        // auto r     = std::min(rMax, cornersR);
        //  
        // auto ltSc = getScaledPos(leftTop    );
        // auto rbSc = getScaledPos(rightBottom);
        //  
        // auto rEllipseSc = getScaledSize(DrawCoord{r,r});

        auto wh    = DrawCoord(rightBottom.x - leftTop.x, rightBottom.y - leftTop.y);
        auto wh_2  = DrawCoord(wh.x/2.0, wh.y/2.0);
        //auto rMax  = std::min(wh_2.x, wh_2.y);
        //auto r     = std::min(rMax, cornersR);

        auto ltSc      = getScaledPos(leftTop    );
        auto rbSc      = getScaledPos(rightBottom);
        auto whSc      = DrawCoord(rbSc.x - ltSc.x, rbSc.y - ltSc.y);
        auto whSc_2    = DrawCoord(floatToInt(whSc.x)/2, floatToInt(whSc.y)/2);
        auto whScMaxR  = std::min(whSc_2.x, whSc_2.y);
        auto whScR     = std::min(whScMaxR, getScaledSize(DrawCoord{cornersR,cornersR}).x);

        //auto rEllipseSc = getScaledSize(DrawCoord{r,r});

        auto finalR = floatToInt(whScR);
        auto finalD = 2*finalR;

        HPEN prevPen = 0;
        if (!drawFrame)
        {
            HPEN transperrantPen = (HPEN)::GetStockObject(NULL_PEN);
            prevPen         = (HPEN)SelectObject( m_hdc, (HGDIOBJ)transperrantPen);
        }
        auto res = ::RoundRect( m_hdc, floatToInt(ltSc.x), floatToInt(ltSc.y), floatToInt(rbSc.x), floatToInt(rbSc.y)
                              //, floatToInt(rEllipseSc.x), floatToInt(rEllipseSc.y)
                              , finalD, finalD
                              ) ? true : false;
        if (!drawFrame)
        {
            SelectObject( m_hdc, (HGDIOBJ)prevPen);
        }

        return res;
    }

    virtual bool roundRect( const DrawCoord::value_type &cornersR
                          , const DrawCoord             &leftTop
                          , const DrawCoord             &rightBottom
                          ) override
    {
        auto wh    = DrawCoord(rightBottom.x - leftTop.x, rightBottom.y - leftTop.y);
        auto wh_2  = DrawCoord(wh.x/2.0, wh.y/2.0);
        //auto rMax  = std::min(wh_2.x, wh_2.y);
        //auto r     = std::min(rMax, cornersR);

        auto ltSc      = getScaledPos(leftTop    );
        auto rbSc      = getScaledPos(rightBottom);
        auto whSc      = DrawCoord(rbSc.x - ltSc.x, rbSc.y - ltSc.y);
        auto whSc_2    = DrawCoord(floatToInt(whSc.x)/2, floatToInt(whSc.y)/2);
        auto whScMaxR  = std::min(whSc_2.x, whSc_2.y);
        auto whScR     = std::min(whScMaxR, getScaledSize(DrawCoord{cornersR,cornersR}).x);

        //auto rEllipseSc = getScaledSize(DrawCoord{r,r});

        auto finalR = floatToInt(whScR);
        auto finalD = 2*finalR;

        HBRUSH transperrantBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
        HBRUSH prevBrush         = (HBRUSH)SelectObject( m_hdc, (HGDIOBJ)transperrantBrush);
        // https://www.functionx.com/bcb/gdi/circleshapes.htm
        auto res = RoundRect( m_hdc, floatToInt(ltSc.x), floatToInt(ltSc.y), floatToInt(rbSc.x), floatToInt(rbSc.y)
                            // , floatToInt(rEllipseSc.x), floatToInt(rEllipseSc.y)
                            , finalD, finalD
                            ) ? true : false;
        SelectObject( m_hdc, (HGDIOBJ)prevBrush);
     
        return res;
    }

    virtual bool rect( const DrawCoord             &leftTop
                     , const DrawCoord             &rightBottom
                     ) override
    {
        auto leftTopSc     = getScaledPos(leftTop    );
        auto rightBottomSc = getScaledPos(rightBottom);

        HBRUSH transperrantBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
        HBRUSH prevBrush         = (HBRUSH)SelectObject( m_hdc, (HGDIOBJ)transperrantBrush);
        ::Rectangle(m_hdc, floatToInt(leftTopSc.x), floatToInt(leftTopSc.y), floatToInt(rightBottomSc.x), floatToInt(rightBottomSc.y));
        SelectObject( m_hdc, (HGDIOBJ)prevBrush);

        return true;
    }

    // У RoundRect'ов очень плохо с симетричностью
    // Тут про GDI+ - https://www.codeproject.com/Articles/27228/A-class-for-creating-round-rectangles-in-GDI-with

    virtual  bool fillRect( const DrawCoord             &leftTop
                     , const DrawCoord             &rightBottom
                     , bool                         drawFrame
                     ) override
    {
        MARTY_ARG_USED(drawFrame);

        DrawCoord leftTopSc     = getScaledPos(leftTop         );
        DrawCoord rightBottomSc = getScaledPos(rightBottom     );

        HPEN prevPen = 0;
        if (!drawFrame)
        {
            HPEN transperrantPen = (HPEN)::GetStockObject(NULL_PEN);
            prevPen         = (HPEN)SelectObject( m_hdc, (HGDIOBJ)transperrantPen);
        }
        ::Rectangle(m_hdc, floatToInt(leftTopSc.x), floatToInt(leftTopSc.y), floatToInt(rightBottomSc.x), floatToInt(rightBottomSc.y));
        if (!drawFrame)
        {
            SelectObject( m_hdc, (HGDIOBJ)prevPen);
        }

        return true;
    }



    #if 0
    virtual bool ellipticArcTo( const DrawCoord &leftTop
                              , const DrawCoord &rightBottom
                              , const DrawCoord &arcStartRefPoint
                              , const DrawCoord &arcEndRefPoint
                              , bool             directionCounterclockwise
                              ) override
    {


                            if (!leftTop || !rightBottom || !arcStart || !arcEnd) return EC_INVALID_PARAM;
                            CPoint ltPos, rbPos, asPos, aePos;

                            coordToDcCoord( ltPos, *leftTop     );
                            coordToDcCoord( rbPos, *rightBottom );
                            coordToDcCoord( asPos, *arcStart    );
                            coordToDcCoord( aePos, *arcEnd      );

                            // direction : 0 - clockwise, 1 counterclockwise
                            SetArcDirection( hdc, direction>0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE );
                            Arc( hdc, ltPos.x, ltPos.y, rbPos.x, rbPos.y
                               , asPos.x, asPos.y, aePos.x, aePos.y
                               );
                            return EC_OK;

    }

    // Implemented offten with serios of the lineTo and ellipticArcTo calls
    virtual bool roundRect( const DrawCoord             &leftTop
                          , const DrawCoord             &rightBottom
                          , const DrawCoord::value_type &cornersR
                          ) override

    #endif


    #if 0
    virtual bool arcTo ( const DrawCoord &to, const DrawCoord &centerPos ) override
    {
        ATLASSERT(m_pathStarted!=false);

        /*
        BOOL ArcTo( [in] HDC hdc,      // A handle to the device context where drawing takes place.

                    // left, top, right, bottom of bounding rectangle
                    [in] int left,     // The x-coordinate, in logical units, of the upper-left corner of the bounding rectangle.
                    [in] int top,      // The y-coordinate, in logical units, of the upper-left corner of the bounding rectangle.
                    [in] int right,    // The x-coordinate, in logical units, of the lower-right corner of the bounding rectangle.
                    [in] int bottom,   // The y-coordinate, in logical units, of the lower-right corner of the bounding rectangle.

                    // the endpoint of the starting point radial
                    [in] int xr1,      // The x-coordinate, in logical units, of the endpoint of the radial defining the starting point of the arc.
                    [in] int yr1,      // The y-coordinate, in logical units, of the endpoint of the radial defining the starting point of the arc.

                    // the endpoint of the ending point radial
                    [in] int xr2,      // The x-coordinate, in logical units, of the endpoint of the radial defining the ending point of the arc.
                    [in] int yr2       // The y-coordinate, in logical units, of the endpoint of the radial defining the ending point of the arc.
                  );
        */

        auto scaledCoordTo    = m_scale*to;
        auto scaledCenterPos  = m_scale*centerPos;

        return false;

    }
    #endif

#if 0

#ifndef M_PI
    #define M_PI       3.14159265358979323846
#endif


// Win GDI

                        CLIMETHOD(drawEllipticArc) (THIS_ const STRUCT_CLI_DRAWING_CPOINT*    leftTop /* [in,ref] ::cli::drawing::CPoint  leftTop  */
                                                        , const STRUCT_CLI_DRAWING_CPOINT*    rightBottom /* [in,ref] ::cli::drawing::CPoint  rightBottom  */
                                                        , const STRUCT_CLI_DRAWING_CPOINT*    arcStart /* [in,ref] ::cli::drawing::CPoint  arcStart  */
                                                        , const STRUCT_CLI_DRAWING_CPOINT*    arcEnd /* [in,ref] ::cli::drawing::CPoint  arcEnd  */
                                                        , BOOL    direction /* [in] bool  direction  */
                                                   )
                           {
                            if (!leftTop || !rightBottom || !arcStart || !arcEnd) return EC_INVALID_PARAM;
                            CPoint ltPos, rbPos, asPos, aePos;

                            coordToDcCoord( ltPos, *leftTop     );
                            coordToDcCoord( rbPos, *rightBottom );
                            coordToDcCoord( asPos, *arcStart    );
                            coordToDcCoord( aePos, *arcEnd      );

                            // direction : 0 - clockwise, 1 counterclockwise
                            SetArcDirection( hdc, direction>0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE );
                            Arc( hdc, ltPos.x, ltPos.y, rbPos.x, rbPos.y
                               , asPos.x, asPos.y, aePos.x, aePos.y
                               );
                            return EC_OK;
                           }





// Wx


                        double getDegreesAngleHelper( const CPoint &center, const CPoint &point )
                           {
                            CPoint distance;
                            distance.x = point.x - center.x;
                            distance.y = point.y - center.y;
                            //double radius = sqrt( (double)(distance.x*distance.x) + (double)(distance.y*distance.y) );
                            //double x = distance.x;
                            //double y = distance.y;
                            return atan2((double)distance.y, (double)distance.x) * 180 / M_PI; // - 90.0;
                           }

                        CLIMETHOD(drawEllipticArc) (THIS_ const STRUCT_CLI_DRAWING_CPOINT*    leftTop /* [in,ref] ::cli::drawing::CPoint  leftTop  */
                                                        , const STRUCT_CLI_DRAWING_CPOINT*    rightBottom /* [in,ref] ::cli::drawing::CPoint  rightBottom  */
                                                        , const STRUCT_CLI_DRAWING_CPOINT*    arcStart /* [in,ref] ::cli::drawing::CPoint  arcStart  */
                                                        , const STRUCT_CLI_DRAWING_CPOINT*    arcEnd /* [in,ref] ::cli::drawing::CPoint  arcEnd  */
                                                        , BOOL    direction /* [in] bool  direction  */
                                                   )
                           {
                            if (!leftTop || !rightBottom || !arcStart || !arcEnd) return EC_INVALID_PARAM;
                            CPoint ltPos, rbPos, asPos, aePos;

                            coordToDcCoord( ltPos, *leftTop     );
                            coordToDcCoord( rbPos, *rightBottom );
                            coordToDcCoord( asPos, *arcStart    );
                            coordToDcCoord( aePos, *arcEnd      );

                            CPoint ellipseSize;
                            ellipseSize.x = rbPos.x - ltPos.x + 1;
                            ellipseSize.y = rbPos.y - ltPos.y + 1;
                            CPoint ellipseCenter;
                            ellipseCenter.x = ltPos.x + ellipseSize.x/2;
                            ellipseCenter.y = ltPos.y + ellipseSize.y/2;

                            double startAngle = getDegreesAngleHelper( ellipseCenter, asPos );
                            double endAngle   = getDegreesAngleHelper( ellipseCenter, aePos );
                            // direction : 0 - clockwise, 1 counterclockwise
                            if (!direction)
                               {
                                startAngle = -startAngle;
                                endAngle   = -endAngle;
                               }

                            pdc->DrawEllipticArc( wxCoord(ltPos.x), wxCoord(ltPos.y)
                                                , wxCoord(ellipseSize.x), wxCoord(ellipseSize.y)
                                                , startAngle, endAngle
                                                );
                            return EC_OK;
                           }

#endif


}; // class GdiDrawContext


} // namespace marty_draw_context
