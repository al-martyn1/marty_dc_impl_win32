#pragma once


// https://www.codeproject.com/Articles/1112/Starting-with-GDI
// https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-using-gdi--use
// https://peter.bloomfield.online/using-gdi-plus-in-cpp-win32-programming/
// https://forums.codeguru.com/showthread.php?272334-Help-using-GDI-inside-OnPaint-method

#include "marty_draw_context/i_draw_context.h"
#include "draw_context_impl_base.h"

#include <atlgdi.h>
#include <vector>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <map>
#include <utility>
#include <memory>
#include <cmath>

#include "gdiplus_inc.h"
#include "mathHelpers.h"


namespace marty_draw_context {


class GdiPlusDrawContext : public DrawContextImplBase // IDrawContext
{

public:

    typedef marty_draw_context::DrawCoord  DrawCoord;
    typedef marty_draw_context::DrawScale  DrawScale;
    typedef marty_draw_context::DrawPoint  DrawPoint;
    typedef marty_draw_context::DrawSize   DrawSize ;
    typedef marty_draw_context::ColorRef   ColorRef ;


protected:


    typedef std::unique_ptr< Gdiplus::Pen >          HPen        ;
    typedef std::unique_ptr< Gdiplus::SolidBrush >   HSolidBrush ;
    typedef std::unique_ptr< Gdiplus::Font >         HFont       ;
    typedef std::unique_ptr< Gdiplus::FontFamily >   HFontFamily ;
    typedef std::unique_ptr< Gdiplus::GraphicsPath > HPath       ;

    // CDCHandle           m_dc; // non-managed GDI Handle class object
    HDC                 m_hdc;
    Gdiplus::Graphics   m_g;

    // bool      m_pathStarted;
    HPath               m_curPath;


    DrawCoord m_curPos;

    std::vector<HPen>         m_hPens;
    //HPen                      m_defPen;
    int                       m_curPenId;

    std::vector<HSolidBrush>  m_hBrushes;
    //HSolidBrush               m_defBrush;
    int                       m_curBrushId;

    std::vector<HFont>        m_hFonts;
    std::map<std::wstring,HFontFamily>  m_hFontFamilies;

    ColorRef                  m_textColor   = {0,0,0};
    ColorRef                  m_textBkColor = {255,255,255};

    //HFont                     m_defFont;
    int                       m_curFontId;




    GdiPlusDrawContext( const GdiPlusDrawContext& ) = delete;
    GdiPlusDrawContext& operator=( const GdiPlusDrawContext& ) = delete;


    Gdiplus::LineCap getNativeLineEndcapFlags(marty_draw_context::LineEndcapStyle style )
    {
        // https://docs.microsoft.com/en-us/windows/win32/api/gdiplusenums/ne-gdiplusenums-linecap
        switch(style)
        {
            case marty_draw_context::LineEndcapStyle::round  : return Gdiplus::LineCapRound;
            case marty_draw_context::LineEndcapStyle::square : return Gdiplus::LineCapSquare;
            case marty_draw_context::LineEndcapStyle::flat   : [[fallthrough]];
            case marty_draw_context::LineEndcapStyle::invalid: [[fallthrough]];
            default:                                          return Gdiplus::LineCapFlat;
        }
    }

    Gdiplus::LineJoin getNativeLineJoinFlags(marty_draw_context::LineJoinStyle style )
    {
        switch(style)
        {
            case marty_draw_context::LineJoinStyle::round  : return Gdiplus::LineJoinRound;
            case marty_draw_context::LineJoinStyle::bevel  : return Gdiplus::LineJoinBevel;
            case marty_draw_context::LineJoinStyle::mitter : [[fallthrough]];
            case marty_draw_context::LineJoinStyle::invalid: [[fallthrough]];
            default:                                        return Gdiplus::LineJoinMiter;
        };
    }


    Gdiplus::SmoothingMode getNativeSmoothingMode(marty_draw_context::SmoothingMode m )
    {
        switch(m)
        {
            case marty_draw_context::SmoothingMode::invalid    : return Gdiplus::SmoothingModeInvalid;
            case marty_draw_context::SmoothingMode::highSpeed  : return Gdiplus::SmoothingModeHighSpeed;
            case marty_draw_context::SmoothingMode::highQuality: return Gdiplus::SmoothingModeHighQuality;
            case marty_draw_context::SmoothingMode::none       : return Gdiplus::SmoothingModeNone;
            case marty_draw_context::SmoothingMode::antiAlias  : return Gdiplus::SmoothingModeAntiAlias;
            case marty_draw_context::SmoothingMode::defMode    : [[fallthrough]];
            default                                            : return Gdiplus::SmoothingModeDefault;
        }
    }

    std::wstring toWideString(const char* str) const
    {
        std::wstring res;

        for(; str && *str; ++str)
            res.append(1,(wchar_t)(unsigned char)*str);

        return res;
    }

    std::wstring toWideString(const std::string &str) const
    {
        return toWideString(str.c_str());
    }

    std::wstring toWideString(const wchar_t* str) const
    {
        return std::wstring(str);
    }

    std::wstring toWideString(const std::wstring &str) const
    {
        return str;
    }

    void init()
    {
        //SetTextAlign (m_dc.m_hDC, GetTextAlign(m_dc.m_hDC) & (~TA_CENTER) | TA_LEFT );

        m_curPenId = createSolidPen( 0, marty_draw_context::LineEndcapStyle::square, marty_draw_context::LineJoinStyle::mitter, 0, 0, 0 );

        /*
        DWORD penStyle = PS_COSMETIC | PS_NULL | PS_ENDCAP_FLAT | PS_JOIN_BEVEL;

        LOGBRUSH lb;
        lb.lbStyle = BS_NULL; // BS_SOLID
        lb.lbColor = 0;
        lb.lbHatch = 0;

        HPEN hpen = ExtCreatePen( penStyle
                                , 1
                                , &lb // LOGBRUSH
                                , 0 // not a user styled pen
                                , 0 // not a user styled pen
                                );
        // if (hpen==0)
        //     throw std

        ATLASSERT(hpen!=0);
        m_hPens.push_back(hpen);

        m_defPen = (HPEN)SelectObject( m_dc.m_hDC, (HGDIOBJ)hpen);


        HBRUSH hBrush = CreateSolidBrush( 0 ); // black brush
        ATLASSERT(hBrush!=0);
        m_hBrushes.push_back(hBrush);

        m_defBrush = (HBRUSH)SelectObject( m_dc.m_hDC, (HGDIOBJ)hBrush);
        */
    }

public:

    GdiPlusDrawContext(CDCHandle dc)
    : DrawContextImplBase()
    , m_hdc(dc.m_hDC)
    , m_g(dc.m_hDC)
    //, m_pathStarted(false)
    , m_curPath()     // https://docs.microsoft.com/en-us/windows/win32/api/gdipluspath/nf-gdipluspath-graphicspath-graphicspath(fillmode)
    , m_curPos()
    , m_hPens()
    //, m_defPen()
    , m_curPenId(-1)
    , m_hBrushes()
    //, m_defBrush()
    , m_curBrushId(-1)
    , m_hFonts()
    //, m_defFont()
    , m_curFontId(-1)
    {
        init();
    }

    GdiPlusDrawContext(HDC hdc)
    : DrawContextImplBase()
    , m_hdc(hdc)
    , m_g(hdc)
    //, m_pathStarted(false)
    , m_curPath()     // https://docs.microsoft.com/en-us/windows/win32/api/gdipluspath/nf-gdipluspath-graphicspath-graphicspath(fillmode)
    , m_curPos()
    , m_hPens()
    //, m_defPen()
    , m_curPenId(-1)
    , m_hBrushes()
    //, m_defBrush()
    , m_curBrushId(-1)
    , m_hFonts()
    //, m_defFont()
    , m_curFontId(-1)
    {
        init();
    }

    ~GdiPlusDrawContext()
    {
        /*
        SelectObject(m_dc.m_hDC, (HGDIOBJ)m_defPen);

        for( auto hpen : m_hPens )
            DeleteObject((HGDIOBJ)hpen);


        SelectObject(m_dc.m_hDC, (HGDIOBJ)m_defBrush);

        for( auto hbrush : m_hBrushes )
            DeleteObject((HGDIOBJ)hbrush);


        if (m_defFont!=0)
            SelectObject(m_dc.m_hDC, (HGDIOBJ)m_defFont);

        for( auto hfont : m_hFonts )
            DeleteObject((HGDIOBJ)hfont);
        */
    }

    virtual void freeAllocatedRc() override
    {
        DrawContextImplBase::freeAllocatedRc();

        m_hPens.clear();
        m_hBrushes.clear();
        m_hFonts.clear();

        m_curPenId   = -1;
        m_curBrushId = -1;
        m_curFontId  = -1;

        init();

    }

    virtual DrawSize calcDrawnTextSizeExact (int   fontId         , const char*    text, std::size_t nChars) override
    {
        std::string  strText  = (nChars==(std::size_t)-1) ? std::string(text) : std::string(text,nChars);
        std::wstring wStrText = decodeString(strText);

        return calcDrawnTextSizeExact(fontId, wStrText.data(), wStrText.size());
    }

    virtual DrawSize calcDrawnTextSizeExact (int   fontId         , const wchar_t* text, std::size_t nChars) override
    {
        // FontSaver fontSaver(this, fontId);

        if (fontId<0 || fontId>=(int)m_hFonts.size())
            return DrawSize{0,0};

        // https://learn.microsoft.com/en-us/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-measurestring(constwchar_int_constfont_constrectf__conststringformat_rectf_int_int)
        // m_hFonts[m_curFontId].get()

        Gdiplus::RectF layoutRect(0.0f, 0.0f, 100.0f, 50.0f);

        const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();

        Gdiplus::RectF boundRect;

        m_g.MeasureString(text, (int)nChars, m_hFonts[(std::size_t)m_curFontId].get(), layoutRect, pStringFormat, &boundRect);

        Gdiplus::SizeF boundSize;

        boundRect.GetSize(&boundSize);

        return mapRawToLogicSize( DrawCoord{boundSize.Width, boundSize.Height} );
    }

    virtual void logFontsInfo() override
    {
    }


    virtual marty_draw_context::SmoothingMode setSmoothingMode(marty_draw_context::SmoothingMode m ) override
    {
        auto prevMode = m_g.GetSmoothingMode();
        m_g.SetSmoothingMode(getNativeSmoothingMode(m));
        return (marty_draw_context::SmoothingMode)prevMode;
    }

    virtual marty_draw_context::SmoothingMode getSmoothingMode( ) override
    {
        return (marty_draw_context::SmoothingMode)m_g.GetSmoothingMode();
    }


    virtual void drawCircleInLogicalCoordsForFillGradientCircle
                                 ( int x
                                 , int y
                                 , int r
                                 , const ColorRef &clr
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
        auto right  = x+r;
        auto bottom = y+r;
        Ellipse(m_hdc, left, top, right, bottom);

        SelectObject(m_hdc, hPrevPen);
        SelectObject(m_hdc, hPrevBrush);

        DeleteObject((HGDIOBJ)hpen);
    }

    virtual void drawLineInLogicalCoordsForFillGradientRect
                                 ( int left
                                 , int top
                                 , int right
                                 , int bottom
                                 , const ColorRef &clr
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


    // https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-pen-flat

    // https://eli.thegreenplace.net/2012/06/20/c11-using-unique_ptr-with-standard-library-containers
    // https://stackoverflow.com/questions/3283778/why-can-i-not-push-back-a-unique-ptr-into-a-vector
    // https://docs.microsoft.com/en-gb/windows/win32/gdiplus/-gdiplus-using-a-pen-to-draw-lines-and-rectangles-use
    // https://docs.microsoft.com/en-gb/windows/win32/gdiplus/-gdiplus-using-a-pen-to-draw-lines-and-shapes-use

    virtual int createSolidPen( float_t width, marty_draw_context::LineEndcapStyle endcaps, marty_draw_context::LineJoinStyle join
                              , std::uint8_t r, std::uint8_t g, std::uint8_t b
                              ) override // Geometric pen
    {
        marty_draw_context::PenParamsWithColor penParams = { width, endcaps, join, ColorRef{r,g,b} };

        int existingPenId = findPenByParams(penParams);
        if (existingPenId>=0)
             return existingPenId;

        float_t scaledWidth = m_penScale*width;

        int penIdx = (int)m_hPens.size();

        if (width<=0.01)
        {
            m_hPens.emplace_back( std::make_unique<Gdiplus::Pen>(Gdiplus::Color(r,g,b), 1.0f ) );
        }
        else
        {
            m_hPens.emplace_back( std::make_unique<Gdiplus::Pen>(Gdiplus::Color(r,g,b), floatToFloat(scaledWidth) ) );
        }

        //if (Gdiplus::Ok!=m_hPens.back()->SetLineCap(getNativeLineEndcapFlags(endcaps), getNativeLineEndcapFlags(endcaps), Gdiplus::DashCapFlat ))
        m_hPens.back()->SetLineCap(getNativeLineEndcapFlags(endcaps), getNativeLineEndcapFlags(endcaps), Gdiplus::DashCapFlat );
        m_hPens.back()->SetLineJoin(getNativeLineJoinFlags(join));

        savePenByParams(penParams, penIdx);

        return penIdx;
    }



    virtual int createSolidPen( float_t width, marty_draw_context::LineEndcapStyle endcaps, marty_draw_context::LineJoinStyle join
                              , const ColorRef &rgb
                              ) override
    {
        return createSolidPen( width, endcaps, join
                             , rgb.r, rgb.g, rgb.b
                             );
    }

    virtual int selectPen( int penId ) override
    {
        if (penId<0 || penId>=(int)m_hPens.size())
            return -1;

        std::swap(m_curPenId,penId);

        return penId;
    }

    virtual int getCurPen() override
    {
        return m_curPenId;
    }


    #if 0
    // https://docs.microsoft.com/ru-ru/windows/win32/api/wingdi/nf-wingdi-roundrect
    // The RoundRect function draws a rectangle with rounded corners. The rectangle is outlined by using the current pen and filled by using the current brush.
    virtual bool roundRect( const DrawCoord::value_type &cornersR
                          , const DrawCoord             &leftTop
                          , const DrawCoord             &rightBottom
                          ) override
    {

        /*
        DrawCoord::value_type dblR = cornersR*2;

        auto leftTopScaled     = getScaledCoord(leftTop    );
        auto rightBottomScaled = getScaledCoord(rightBottom);
        auto rScaled           = getScaledCoord(DrawCoord{dblR,dblR});

        // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-rectangle

        return m_dc.RoundRect( int(floatToInt(leftTopScaled.x    )), int(floatToInt(leftTopScaled.y    ))
                             , int(floatToInt(rightBottomScaled.x)), int(floatToInt(rightBottomScaled.y))
                             , int(floatToInt(rScaled.x))          , int(floatToInt(rScaled.y))
                             ) ? true : false;
        */

        return true;
    }
    #endif


    virtual int createSolidBrush( const ColorRef &rgb ) override
    {
        return createSolidBrush( rgb.r, rgb.g, rgb.b );
    }

    virtual int createSolidBrush( std::uint8_t r, std::uint8_t g, std::uint8_t b ) override
    {
        ColorRef brushColorRef = {r,g,b};
        int existingBrushId = findBrushByParams(brushColorRef);
        if (existingBrushId>=0)
             return existingBrushId;

        int brushIdx = (int)m_hBrushes.size();

        m_hBrushes.emplace_back( std::make_unique<Gdiplus::SolidBrush>(Gdiplus::Color(r,g,b)) );

        saveBrushByParams(brushColorRef,brushIdx);

        return brushIdx;
    }

    virtual int selectBrush( int brushId ) override
    {
        if (brushId<0 || brushId>=(int)m_hBrushes.size())
            return -1;

        //SelectObject( m_dc.m_hDC, (HGDIOBJ)m_hBrushes[brushId]);

        std::swap(m_curBrushId,brushId);

        return brushId;
    }

    virtual int getCurBrush() override
    {
        return m_curBrushId;
    }


    #if 0
    template<typename LogfontStruct>
    void fillLogfontStruct( LogfontStruct &lf, int height, int escapement, int orientation, int weight, int fontStyleFlags )
    {
        std::memset(&lf, 0, sizeof(lf));

        lf.lfHeight          = height      ;  // LONG высота, in logical units
        lf.lfWidth           = 0           ;  // LONG
        lf.lfEscapement      = escapement  ;  // LONG ориентация (всего текста), для TTF любой градус, для растровых - 0-90-180-270
        lf.lfOrientation     = orientation ;  // LONG ориентация отельных символов
        lf.lfWeight          = weight      ;  // LONG толщина (жыр)
        lf.lfItalic          = fontStyleFlags&FontStyleFlags::italic     ? TRUE : FALSE; // BYTE
        lf.lfUnderline       = fontStyleFlags&FontStyleFlags::underlined ? TRUE : FALSE; // BYTE
        lf.lfStrikeOut       = fontStyleFlags&FontStyleFlags::strikeout  ? TRUE : FALSE; // BYTE
        lf.lfCharSet         = ANSI_CHARSET; // BYTE
        lf.lfOutPrecision    = OUT_TT_PRECIS; // BYTE
        lf.lfClipPrecision   = CLIP_DEFAULT_PRECIS; // BYTE
        lf.lfQuality         = ANTIALIASED_QUALITY; // BYTE
        lf.lfPitchAndFamily  = DEFAULT_PITCH | FF_DECORATIVE; // BYTE

    }

    HFONT createFontImpl( int height, int escapement, int orientation, int weight, int fontStyleFlags, const char *fontFace )
    {

        LOGFONTA lf;
        fillLogfontStruct( lf, height, escapement, orientation, weight, fontStyleFlags );

        int iff=0;
        for(; iff<(LF_FACESIZE-1) && *fontFace; ++iff)
            lf.lfFaceName[iff] = fontFace[iff];
        lf.lfFaceName[iff] = 0;

        return CreateFontIndirectA(&lf);
    }

    #endif


    Gdiplus::FontFamily* getFontFamily(const std::wstring &fontFace)
    {
        std::map<std::wstring,HFontFamily>::const_iterator it = m_hFontFamilies.find(fontFace);
        if (it==m_hFontFamilies.end())
        {
            m_hFontFamilies[fontFace] = std::make_unique<Gdiplus::FontFamily>(fontFace.c_str(),(Gdiplus::FontCollection*)0);
            it = m_hFontFamilies.find(fontFace);
        }
        return it->second->IsAvailable() ? it->second.get() : (Gdiplus::FontFamily*)0;
    }

    // Gdiplus::FontFamily* getFontFamily(const wchar_t *fontFace)     { return getFontFamily(std::wstring(fontFace)); }
    // Gdiplus::FontFamily* getFontFamily(const std::string &fontFace) { return getFontFamily(toWideString(fontFace)); }
    // Gdiplus::FontFamily* getFontFamily(const char *fontFace)        { return getFontFamily(toWideString(fontFace)); }

    int createFontImpl( float_t height, int escapement, int orientation, marty_draw_context::FontWeight weight, marty_draw_context::FontStyleFlags fontStyleFlags, const std::wstring &fontFace )
    {
        auto pff = getFontFamily(fontFace);
        if (!pff)
            return -1;

        int fontIdx = (int)m_hFonts.size();

        int fontStyle = Gdiplus::FontStyleRegular;

        if ((int)weight>500)
            fontStyle |= Gdiplus::FontStyleBold;

        if ((fontStyleFlags& marty_draw_context::FontStyleFlags::italic)!=0)
            fontStyle |= Gdiplus::FontStyleItalic;

        if ((fontStyleFlags& marty_draw_context::FontStyleFlags::underlined)!=0)
            fontStyle |= Gdiplus::FontStyleUnderline;

        if ((fontStyleFlags& marty_draw_context::FontStyleFlags::strikeout)!=0)
            fontStyle |= Gdiplus::FontStyleStrikeout;

        // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-setmapmode
        // https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-getmapmode
        // MM_HIENGLISH/MM_HIMETRIC/MM_LOENGLISH/MM_LOMETRIC/MM_TEXT/MM_TWIPS
        // UnitInch
        // UnitMillimeter
        // UnitPixel

        auto fontSize = getScaledSize(DrawCoord(height,height)).x;
        //fontSize = fontSize*90/100;

        m_hFonts.emplace_back( std::make_unique<Gdiplus::Font>( pff
                                                              , floatToFloat(fontSize)
                                                              , fontStyle
                                                              , Gdiplus::UnitWorld
                                                              )
                             );

        auto fontParamsW = makeFontParamsW(marty_draw_context::FontParamsW{(marty_draw_context::FontParamsW::font_height_t)height, escapement, orientation, weight, fontStyleFlags, fontFace} );

        fontParamsW.monospaceFontCharWidth = -1;
        fontParamsW.monospaceFontCharWidth = getMonospacedFontCharWidthApprox(fontParamsW);

        fontParamsW.proportionalFontScale = -1;
        fontParamsW.proportionalFontScale = getProportionalFontCharWidthScaleApprox(fontParamsW);

        fontsByParams  [ fontParamsW ] = fontIdx;
        fontsParamsById[ fontIdx ]     = fontParamsW;

        return fontIdx;
    }

    virtual int createFont( float_t height, int escapement, int orientation, marty_draw_context::FontWeight weight, marty_draw_context::FontStyleFlags fontStyleFlags, const char *fontFace ) override
    {
        return createFontImpl( height, escapement, orientation, weight, fontStyleFlags, toWideString(fontFace) );
    }

    virtual int createFont( float_t height, int escapement, int orientation, marty_draw_context::FontWeight weight, marty_draw_context::FontStyleFlags fontStyleFlags, const wchar_t *fontFace ) override
    {
        return createFontImpl( height, escapement, orientation, weight, fontStyleFlags, fontFace );
    }

    virtual int  createFont( const marty_draw_context::FontParamsA &fp ) override
    {
        return createFontImpl( fp.height, fp.escapement, fp.orientation, fp.weight, fp.fontStyleFlags, toWideString(fp.fontFace) );
    }

    virtual int  createFont( const marty_draw_context::FontParamsW &fp ) override
    {
        return createFontImpl( fp.height, fp.escapement, fp.orientation, fp.weight, fp.fontStyleFlags, toWideString(fp.fontFace) );
    }

    virtual int selectFont( int fontId ) override
    {
        if (fontId<0 || fontId>=(int)m_hFonts.size())
            return -1;

        std::swap(m_curFontId, fontId);

        return fontId;
    }

    virtual int  getCurFont() override
    {
        return m_curFontId;
    }


    // virtual bool textOut( const DrawCoord &pos, const std::string &text ) override
    // {
    //     return textOut( pos, toWideString(text) ); //TODO: Need correct conversion to wide string
    // }

    virtual bool textOut( const DrawCoord &pos, const std::wstring &text ) override
    {
        if (m_curFontId<0 || m_curFontId>=(int)m_hFonts.size())
            return false;

        Gdiplus::SolidBrush textBrush = Gdiplus::SolidBrush(Gdiplus::Color(m_textColor.r,m_textColor.g,m_textColor.b));

        auto scaledPos = getScaledPos(pos);
        //DC_LOG()<<"textOut at "<<scaledPos<<"\n";
        Gdiplus::PointF posF = Gdiplus::PointF(floatToFloat(scaledPos.x),floatToFloat(scaledPos.y));

        // Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat(0,0);
        // stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
        // stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
        // stringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
        // stringFormat.SetHotkeyPrefix(Gdiplus::HotkeyPrefixNone);

        // https://stackoverflow.com/questions/54024997/how-to-properly-left-align-text-with-drawstring
        // https://docs.microsoft.com/en-us/windows/win32/api/gdiplusstringformat/nf-gdiplusstringformat-stringformat-generictypographic

        const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();

        //m_g.DrawString( text.c_str(), -1, m_hFonts[m_curFontId].get(), posF, &textBrush );
        //m_g.DrawString( text.c_str(), -1, m_hFonts[m_curFontId].get(), posF, &stringFormat, &textBrush );

        // https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-antialiasing-with-text-use
        m_g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        m_g.DrawString( text.c_str(), -1, m_hFonts[(std::size_t)m_curFontId].get(), posF, pStringFormat, &textBrush );

        return true;
    }


    virtual ColorRef setTextColor( std::uint8_t r, std::uint8_t g, std::uint8_t b ) override
    {
        ColorRef newColor = { r, g, b };
        std::swap(newColor,m_textColor);
        return newColor;
    }

    virtual ColorRef setBkColor( std::uint8_t r, std::uint8_t g, std::uint8_t b ) override
    {
        ColorRef newColor = { r, g, b };
        std::swap(newColor,m_textBkColor);
        return newColor;
    }

    virtual ColorRef setTextColor( const ColorRef &rgb ) override
    {
        return setTextColor( rgb.r, rgb.g, rgb.b );
    }

    virtual ColorRef getTextColor( ) override
    {
        return m_textColor;
    }

    virtual ColorRef setBkColor( const ColorRef &rgb ) override
    {
        return setBkColor( rgb.r, rgb.g, rgb.b );
    }

    virtual marty_draw_context::BkMode setBkMode(marty_draw_context::BkMode mode ) override
    {
        MARTY_ARG_USED(mode);
        // SetBkMode( m_dc.m_hDC, mode==BkMode::opaque ? OPAQUE : TRANSPARENT );
        return marty_draw_context::BkMode::transparent;
    }

    // virtual BkMode getBkMode( ) override
    // {
    //     return BkMode::transparent;
    // }


    virtual DrawCoord getCurPos( ) override
    {
        return m_curPos;
    }

    virtual bool beginPath()                                override
    {
        ATLASSERT(!m_curPath);

        m_curPath = std::make_unique< Gdiplus::GraphicsPath >( Gdiplus::FillModeWinding );

        return true;
    }

//---
#if 0
    virtual bool moveTo( const DrawCoord &to ) override
    {
        auto scaledCoordTo = getScaledPos(to);

        if (m_dc.MoveTo( int(floatToInt(scaledCoordTo.x)), int(floatToInt(scaledCoordTo.y)), 0 ))
        {
            m_curPos = to;
            return true;
        }

        return false;
    }

    virtual bool lineTo( const DrawCoord &to ) override
    {
        auto scaledCoordTo = getScaledPos(to);
        if (m_dc.LineTo( int(floatToInt(scaledCoordTo.x)), int(floatToInt(scaledCoordTo.y)) ))
        {
            m_curPos = to;
            return true;
        }

        return false;
    }
#endif
//---
    virtual bool moveTo( const DrawCoord &to ) override
    {
        // DC_LOG() << "GdiDc::moveTo\n";
        // DC_LOG() << "  Do nothing\n\n";

        m_curPos = to;
        // auto scaledCoordTo = getScaledPos(to);
        // m_curPos = scaledCoordTo;
        return true;
    }

    virtual bool lineTo( const DrawCoord &to ) override
    {
        // auto scaledFrom = m_scale*m_curPos;
        // auto scaledTo   = m_scale*to;
        auto scaledFrom  = getScaledPos(m_curPos); // getScaledPos(to);
        auto scaledTo    = getScaledPos(to);

        if (!m_curPath)
        {
            ATLASSERT(m_curPenId>=0 && m_curPenId<(int)m_hPens.size());
            if (Gdiplus::Ok!=m_g.DrawLine( m_hPens[(std::size_t)m_curPenId].get()
                                         , floatToFloat(scaledFrom.x)
                                         , floatToFloat(scaledFrom.y)
                                         , floatToFloat(scaledTo.x)
                                         , floatToFloat(scaledTo.y)
                                         )
               ) return false;
        }
        else
        {
            if (Gdiplus::Ok!=m_curPath->AddLine( floatToFloat(scaledFrom.x)
                                               , floatToFloat(scaledFrom.y)
                                               , floatToFloat(scaledTo.x)
                                               , floatToFloat(scaledTo.y)
                                               )
               ) return false;
        }

        m_curPos = to;

        return true;

    }

    virtual bool rect( const DrawCoord             &leftTop
                     , const DrawCoord             &rightBottom
                     ) override
    {
        // https://docs.microsoft.com/en-us/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-drawrectangle(constpen_real_real_real_real)
        if (!moveTo(leftTop)) return false;
        if (!lineTo(DrawCoord(rightBottom.x, leftTop.y))) return false;
        if (!lineTo(rightBottom)) return false;
        if (!lineTo(DrawCoord(leftTop.x   , rightBottom.y))) return false;
        if (!lineTo(leftTop)) return false;
        return true;
    }

    virtual  bool fillRect( const DrawCoord             &leftTop
                     , const DrawCoord             &rightBottom
                     ) override
    {
        if (m_curBrushId<0)
            return false;

        if (m_curBrushId>=(int)m_hBrushes.size())
            return false;

        DrawCoord leftTopSc           = getScaledPos(leftTop         );
        DrawCoord rightBottomSc       = getScaledPos(rightBottom     );

        DrawCoord whSc = DrawCoord{ rightBottomSc.x-leftTopSc.x+1, rightBottomSc.y-leftTopSc.y+1 };

        return m_g.FillRectangle( m_hBrushes[(std::size_t)m_curBrushId].get()
                                , floatToFloat(leftTopSc.x)
                                , floatToFloat(leftTopSc.y)
                                , floatToFloat(whSc.x)
                                , floatToFloat(whSc.y)
                                )==Gdiplus::Ok ? true : false;
    }

    // https://docs.microsoft.com/en-us/windows/win32/api/gdipluspath/nl-gdipluspath-graphicspath
    virtual bool beginPath( const DrawCoord &to )           override
    {
        if (!beginPath())
            return false;

        return moveTo(to);
    }


    virtual bool endPath( bool bStroke = true, bool bFill = false ) override
    {
        ATLASSERT(m_curPath);

        /*
        if (bStroke || bFill)
        {
            // https://docs.microsoft.com/en-us/windows/win32/api/gdipluspath/nf-gdipluspath-graphicspath-outline
            if (!bFill)
            {
                m_curPath->Outline( 0, 0 ); // matrix, flatness
            }
        }
        */

        if (!bStroke)
            closeFigure();

        if (bFill)
        {
            ATLASSERT(m_curBrushId>=0 && m_curBrushId<(int)m_hBrushes.size());

        //SelectObject( m_dc.m_hDC, (HGDIOBJ)m_hBrushes[brushId]);

            // https://docs.microsoft.com/en-us/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-fillpath
            m_g.FillPath(m_hBrushes[(std::size_t)m_curBrushId].get(), m_curPath.get());
        }

        if (bStroke)
        {
            ATLASSERT(m_curPenId>=0 && m_curPenId<(int)m_hPens.size());

            // https://docs.microsoft.com/en-us/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-drawpath
            m_g.DrawPath(m_hPens[(std::size_t)m_curPenId].get(), m_curPath.get());
            // Gdiplus::Ok!=m_g.
        }

        m_curPath = 0;

        return true;
    }

    virtual bool closeFigure()                              override
    {
        ATLASSERT(m_curPath);
        m_curPath->CloseFigure();
        return true;
    }

    virtual bool isPathStarted() override
    {
        return m_curPath ? true : false;
    }


    double calcCos( const DrawCoord &c ) const
    {
        auto x =  floatToDouble(c.x);
        auto y = -floatToDouble(c.y);

        return x/std::sqrt(x*x+y*y);
    }

    double calcAngleRad( const DrawCoord &c ) const
    {
        //return std::acos(calcCos(c));
        if (c.y<0)
            return -std::acos(calcCos(c));
        else
            return std::acos(calcCos(c));

    }

    double calcAngleDeg( const DrawCoord &c ) const
    {

        return calcAngleRad(c)*floatToDouble(180)/3.14159265358979323846;
    }


    virtual bool ellipticArcTo( const DrawCoord &leftTop
                              , const DrawCoord &rightBottom
                              , const DrawCoord &arcStartRefPoint
                              , const DrawCoord &arcEndRefPoint    // как бы ref point, но не в ней ли дуга закончится должна?
                              , bool             directionCounterclockwise
                              ) override
    {
        //auto scaledFrom = m_scale*m_curPos;

        DrawCoord leftTopSc           = getScaledPos(leftTop         );
        DrawCoord rightBottomSc       = getScaledPos(rightBottom     );
        DrawCoord arcStartRefPointSc  = getScaledPos(arcStartRefPoint);
        DrawCoord arcEndRefPointSc    = getScaledPos(arcEndRefPoint  );

        DrawCoord widthHeightSc       = rightBottomSc - leftTopSc;

        DrawCoord arcCenterPoint      = (leftTop+rightBottom)/DrawCoord(2,2); // floatToFloat(2);
        DrawCoord arcStartVec         = arcStartRefPoint - arcCenterPoint;
        DrawCoord arcEndVec           = arcEndRefPoint   - arcCenterPoint;

        markerAdd(arcCenterPoint);

        // DC_LOG() << "---------\n";
        // DC_LOG() << "GdiPlusDrawContext::ellipticArcTo\n";
        // DC_LOG() << "m_scaledOffset    : " << m_scaledOffset   << "\n";
        // DC_LOG() << "m_scale           : " << m_scale          << "\n";
        // DC_LOG() << "m_scale*leftTop   : " << m_scale*leftTop  << "\n";
        // DC_LOG() << "leftTop           : " << leftTop          << "\n";
        // DC_LOG() << "rightBottom       : " << rightBottom      << "\n";
        // DC_LOG() << "arcStartRefPoint  : " << arcStartRefPoint << "\n";
        // DC_LOG() << "arcEndRefPoint    : " << arcEndRefPoint   << "\n";
        // DC_LOG() << "leftTopSc         : " << leftTopSc          << "\n";
        // DC_LOG() << "rightBottomSc     : " << rightBottomSc      << "\n";
        // DC_LOG() << "arcStartRefPointSc: " << arcStartRefPointSc << "\n";
        // DC_LOG() << "arcEndRefPointSc  : " << arcEndRefPointSc   << "\n";
        // DC_LOG() << "widthHeightSc     : " << widthHeightSc  << "\n";
        // DC_LOG() << "arcCenterPoint    : " << arcCenterPoint << "\n";
        // DC_LOG() << "arcStartVec       : " << arcStartVec    << "\n";
        // DC_LOG() << "arcEndVec         : " << arcEndVec      << "\n";
        // DC_LOG() << "startRefVec       : {" << arcStartVec.x << "," << arcStartVec.y << "}\n";
        // DC_LOG() << "endRefVec         : {" << arcEndVec  .x << "," << arcEndVec  .y << "}\n";
        // DC_LOG() << "CCW               : "  << directionCounterclockwise << "\n";


        #if 1
        double startAngleRadians;
        double endAngleRadians  ;
        double vectorLen;

        //int yScale = -1;
        int yScale = 1;

        if (!math_helpers::calcVectorAngle((double)arcStartVec.x, yScale*(double)arcStartVec.y, startAngleRadians, &vectorLen))
        {
            // DC_LOG()<<"GdiPlusDrawContext::ellipticArcTo - something goes wrong (1)\n";
            return false;
        }

        if (!math_helpers::calcVectorAngle((double)arcEndVec  .x, yScale*(double)arcEndVec.y, endAngleRadians))
        {
            // DC_LOG()<<"GdiPlusDrawContext::ellipticArcTo - something goes wrong (2)\n";
            return false;
        }

        double startAngleDeg = math_helpers::angleToDegrees( startAngleRadians );
        double endAngleDeg   = math_helpers::angleToDegrees( endAngleRadians   );

        auto degStart = math_helpers::normalizeAngleDegreesMod180(startAngleDeg);
        auto degEnd   = math_helpers::normalizeAngleDegreesMod180(endAngleDeg  );

        double startAngleDeg360 = math_helpers::normalizeAngleDegrees360( startAngleDeg );
        double endAngleDeg360   = math_helpers::normalizeAngleDegrees360( endAngleDeg   );

        MARTY_ARG_USED(degEnd);

        // double startAngleDegMod360 = math_helpers::normalizeAngleDegreesMod360( startAngleDeg );
        // double endAngleDegMod360   = math_helpers::normalizeAngleDegreesMod360( endAngleDeg   );
        //
        // while(startAngleDegMod360>endAngleDegMod360)
        // {
        //     startAngleDegMod360 = startAngleDegMod360 - 360.0;
        // }

        while(startAngleDeg360>endAngleDeg360)
        {
            startAngleDeg360 = startAngleDeg360 - 360.0;
        }


        // DC_LOG() << "start360    : "  << startAngleDeg360 << "\n";
        // DC_LOG() << "end360      : "  << endAngleDeg360   << "\n";


        //auto degSweep = degEnd - degStart;
        //auto degSweep = startAngleDeg360 - endAngleDeg360;
        auto degSweep = endAngleDeg360 - startAngleDeg360;



        // DC_LOG() << "degSweep    : "  << degSweep << "\n";
        // //degSweep = math_helpers::normalizeAngleDegreesMod180(degSweep);
        // DC_LOG() << "degSweep2   : "  << degSweep << "\n";

        if (directionCounterclockwise)
        {
            // degSweep = degSweep-360;
            //degSweep = -degSweep;
            //degSweep = math_helpers::normalizeAngleDegreesComplement360(degSweep);

            // degSweep = math_helpers::normalizeAngleDegreesComplement360(degSweep);
            // DC_LOG() << "degSweep3   : "  << degSweep << "\n";
            // degSweep = -degSweep;
            // DC_LOG() << "degSweep4   : "  << degSweep << "\n";
            // DC_LOG() << "degSweep    : "  << degSweep << " - converted 360\n";

            //degSweep = startAngleDeg360 - endAngleDeg360;

            degSweep = 360-degSweep;
            degSweep = -degSweep;

        }
        else
        {
            // DC_LOG() << "degSweep3   : "  << degSweep << "\n";
            // DC_LOG() << "degSweep4   : "  << degSweep << "\n";
            // DC_LOG() << "degSweep    : "  << degSweep << " - not converted\n";

        }

        //degSweep = math_helpers::normalizeAngleDegreesMod180(degSweep);
        //degSweep = math_helpers::normalizeAngleDegrees360(degSweep);
        degSweep = math_helpers::normalizeAngleDegreesMod360(degSweep);


        #else


        auto degStart = calcAngleDeg(arcStartVec);
        auto degEnd   = calcAngleDeg(arcEndVec);
        auto degSweep = degEnd-degStart;


        if (degStart>=180 && !directionCounterclockwise)
        {
            degSweep += 360;
        }

        if (directionCounterclockwise)
        {
            //std::swap(degStart,degEnd);
            degSweep = degSweep-360;
            //std::swap(arcStartRefPointCopy,arcEndRefPointCopy);
        }

        while(degSweep>360)
            degSweep -= 360;
        while(degSweep<-360)
            degSweep += 360;

        #endif


        DrawCoord arcStartRefPointCopy = arcStartRefPoint;
        DrawCoord arcEndRefPointCopy   = arcEndRefPoint   ;


        // DC_LOG() << " Final\n";
        // DC_LOG() << "degStart    : "  << degStart << "\n";
        // DC_LOG() << "degEnd      : "  << degEnd   << "\n";
        // DC_LOG() << "degSweep    : "  << degSweep << "\n";


        if (!m_curPath)
        {
            // // https://docs.microsoft.com/en-us/windows/win32/api/gdiplusgraphics/nf-gdiplusgraphics-graphics-drawarc(constpen_real_real_real_real_real_real)
            ATLASSERT(m_curPenId>=0 && m_curPenId<(int)m_hPens.size());
            if (Gdiplus::Ok!=m_g.DrawArc( m_hPens[(std::size_t)m_curPenId].get()
                                        , floatToFloat(leftTopSc.x)
                                        , floatToFloat(leftTopSc.y)
                                        , floatToFloat(widthHeightSc.x)
                                        , floatToFloat(widthHeightSc.y)
                                        , Gdiplus::REAL(degStart)
                                        , Gdiplus::REAL(degSweep)
                                        )
               ) return false;
        }
        else
        {
            // https://docs.microsoft.com/en-us/windows/win32/api/gdipluspath/nf-gdipluspath-graphicspath-addarc(real_real_real_real_real_real)
            if (Gdiplus::Ok!=m_curPath->AddArc( floatToFloat(leftTopSc.x)
                                              , floatToFloat(leftTopSc.y)
                                              , floatToFloat(widthHeightSc.x)
                                              , floatToFloat(widthHeightSc.y)
                                              , Gdiplus::REAL(degStart)
                                              , Gdiplus::REAL(degSweep)
                                              )
               ) return false;
        }

        m_curPos = arcEndRefPointCopy;

        return true;

    }



}; // class GdiPlusDrawContext


} // namespace marty_draw_context
