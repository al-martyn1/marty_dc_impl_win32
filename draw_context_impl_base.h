#pragma once

#include "marty_draw_context/i_draw_context.h"
#include "marty_draw_context/draw_helpers.h"

#include "marty_draw_context/dc_debug/uw_log.h"

#include "encoding/encoding.h"

#include "marty_cpp/marty_cpp.h"
#include "marty_draw_context//marty_mbs.h"

#include <vector>
#include <map>
#include <utility>
#include <array>

#include <cmath>
#include "mathHelpers.h"
#include "dc_win32_enums.h"


#define DC_LOG()    UW_LOG_DBGOUT()
// #define DC_LOG()    UW_LOG_NUL()


#if defined(WIN32) || defined(_WIN32)

marty_draw_context::DebugStreamImplBase& operator<<(marty_draw_context::DebugStreamImplBase& s, const LOGFONTW &lf )
{
    s<<"LOGFONT.lfHeight : "<<lf.lfHeight<<"\n";
    s<<"LOGFONT.lfWidth : "<<lf.lfWidth<<"\n";
    s<<"LOGFONT.lfEscapement : "<<lf.lfEscapement<<"\n";
    s<<"LOGFONT.lfOrientation : "<<lf.lfOrientation<<"\n";
    s<<"LOGFONT.lfWeight : "<<lf.lfWeight<<"\n";
    s<<"LOGFONT.lfItalic : "<<lf.lfItalic<<"\n";
    s<<"LOGFONT.lfUnderline : "<<lf.lfUnderline<<"\n";
    s<<"LOGFONT.lfStrikeOut : "<<lf.lfStrikeOut<<"\n";
    s<<"LOGFONT.lfCharSet : "<<lf.lfCharSet<<"\n";
    s<<"LOGFONT.lfOutPrecision : "<<lf.lfOutPrecision<<"\n";
    s<<"LOGFONT.lfOutPrecision : "<<lf.lfOutPrecision<<"\n";
    s<<"LOGFONT.lfClipPrecision : "<<lf.lfClipPrecision<<"\n";
    s<<"LOGFONT.lfQuality : "<<lf.lfQuality<<"\n";
    s<<"LOGFONT.lfPitchAndFamily : "<<lf.lfPitchAndFamily<<"\n";


    std::wstring faceName; faceName.reserve(sizeof(lf.lfFaceName)/sizeof(lf.lfFaceName[0]));

    for(auto i=0u; lf.lfFaceName[i]!=0 && i!=(sizeof(lf.lfFaceName)/sizeof(lf.lfFaceName[0])); ++i)
    {
        faceName.append(1,lf.lfFaceName[i]);
    }

    s<<"LOGFONT.lfFaceName : "<<faceName<<"\n";

    return s;
}

#endif


namespace marty_draw_context {



class DrawContextImplBase : public marty_draw_context::IDrawContext
{

public:

    typedef marty_draw_context::DrawCoord  DrawCoord;
    typedef marty_draw_context::DrawScale  DrawScale;
    typedef marty_draw_context::DrawPoint  DrawPoint;
    typedef marty_draw_context::DrawSize   DrawSize ;
    typedef marty_draw_context::ColorRef   ColorRef ;


protected:

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



    struct MarkerInfo
    {
        DrawCoord                pos;
        DrawCoord::value_type    size;
        // Additional info?

        // Need for C++17, not need for C++20
        MarkerInfo() : pos(), size() {}
        MarkerInfo(const DrawCoord &p, const DrawCoord::value_type &sz) : pos(p), size(sz) {}

    };


    std::map<marty_draw_context::FontParamsW, int>          fontsByParams  ;
    std::map<int, marty_draw_context::FontParamsW>          fontsParamsById;
    std::vector<MarkerInfo>                                 markers;

    std::map<marty_draw_context::PenParamsWithColor, int>   pensByParams;
    std::map<int, marty_draw_context::PenParamsWithColor>   pensParamsById;

    std::map<marty_draw_context::ColorRef, int>             brushesByParams;


    bool                         collectMarkers = false;
    int                          defCosmeticPen = -1;

    std::string                  encName = "CP866"; // UTF-8 65001
    unsigned                     encCodepageId = 866;

    DrawCoord                    m_offset         = { (DrawCoord::value_type)0, (DrawCoord::value_type)0 };
    DrawCoord                    m_scaledOffset   = { (DrawCoord::value_type)0, (DrawCoord::value_type)0 };
    float_t                      m_penScale       = (float_t)1;
    DrawScale                    m_scale          = { (DrawScale::value_type)1, (DrawScale::value_type)1 };


    // Оценивал на длине 1050 пикселей, mid point - 0.7, от светлозеленого к красному через синий
    //static const std::size_t gradientColorsBufSize = 16; // Очень заметны зоны градиента
    //static const std::size_t gradientColorsBufSize = 32; // Очень заметны зоны градиента
    //static const std::size_t gradientColorsBufSize = 64; // Очень заметны на коротком конце, слабо различимы на длинном
    //static const std::size_t gradientColorsBufSize = 128; // Слабо различимы на коротком конце, почти неразличимы на длинном конце (на синем, посередине, чуть лучше различимы)
    //static const std::size_t gradientColorsBufSize = 256; // Совсем слабо различимы на коротком конце
    static const std::size_t gradientColorsBufSize = 512; // Как-будто различимо, но на самом деле уже вроде нет
    //static const std::size_t gradientColorsBufSize = 1024; // Хватит всем. Достаточно уже насиловать стек



    virtual void freeAllocatedRc() override
    {
        fontsByParams.clear();
        fontsParamsById.clear();

        pensByParams.clear();
        pensParamsById.clear();

        brushesByParams.clear();
    }

    //virtual bool getCharWidth (std::uint32_t charCode, float_t &w) = 0;

    // virtual bool getCharWidths(const std::wstring &text, std::vector<float_t> &widths, int fontId=-1 /* use current font */ ) override
    // {
    //     return getCharWidths(text.c_str(), widths, fontId);
    // }

    template<typename LogfontStruct, typename CharType>
    void fillLogfontStruct( LogfontStruct &lf, int height, int escapement, int orientation, int weight, marty_draw_context::FontStyleFlags fontStyleFlags, const CharType *pFontFace ) const
    {
        std::memset(&lf, 0, sizeof(lf));

        lf.lfHeight          = height      ;  // LONG высота, in logical units
        lf.lfWidth           = 0           ;  // LONG
        lf.lfEscapement      = escapement  ;  // LONG ориентация (всего текста), для TTF любой градус, для растровых - 0-90-180-270
        lf.lfOrientation     = orientation ;  // LONG ориентация отельных символов
        lf.lfWeight          = weight      ;  // LONG толщина (жыр)
        lf.lfItalic          = (BYTE)((fontStyleFlags&FontStyleFlags::italic    ) != 0 ? TRUE : FALSE); // BYTE
        lf.lfUnderline       = (BYTE)((fontStyleFlags&FontStyleFlags::underlined) != 0 ? TRUE : FALSE); // BYTE
        lf.lfStrikeOut       = (BYTE)((fontStyleFlags&FontStyleFlags::strikeout ) != 0 ? TRUE : FALSE); // BYTE
        // ANSI_CHARSET/DEFAULT_CHARSET
        lf.lfCharSet         = DEFAULT_CHARSET; // BYTE // !!!
        lf.lfOutPrecision    = OUT_OUTLINE_PRECIS; // OUT_DEVICE_PRECIS; // OUT_TT_PRECIS; // OUT_OUTLINE_PRECIS; // OUT_TT_PRECIS; // BYTE
        lf.lfClipPrecision   = CLIP_DEFAULT_PRECIS; // BYTE
        lf.lfQuality         = CLEARTYPE_QUALITY; // DRAFT_QUALITY; // PROOF_QUALITY; // ANTIALIASED_QUALITY; // BYTE // !!!
        lf.lfPitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE; // FF_DECORATIVE; // BYTE

        if (lf.lfHeight>0)
           lf.lfHeight = -lf.lfHeight;

        typedef typename std::remove_reference<decltype(lf.lfFaceName[0])>::type   TargetCharType;
        typedef typename std::make_unsigned<CharType>::type                        UnsignedSourceCharType;

        //lf.lfHeight = -lf.lfHeight;
        int iff=0;
        for(; iff<(LF_FACESIZE-1) && pFontFace[iff] /* *fontFace */ ; ++iff)
            lf.lfFaceName[iff] = static_cast< TargetCharType >( static_cast<UnsignedSourceCharType>(pFontFace[iff]) );
            
        lf.lfFaceName[iff] = 0;

    }

    template<typename LogfontStruct, typename FontParamsStruct>
    void fillLogfontStruct( LogfontStruct &lf, const FontParamsStruct &fp ) const
    {
        int scaledHeight = int(floatToInt(getScaledSize(DrawCoord(fp.height,fp.height)).y));
        fillLogfontStruct( lf, scaledHeight, fp.escapement, fp.orientation, (int)fp.weight, fp.fontStyleFlags, fp.fontFace.c_str() );
    }


    template<typename CharType>
    std::size_t checkCalcStringSize(const CharType *pStr, std::size_t size) const
    {
        if (!pStr)
        {
            return 0u;
        }

        if (size!=(std::size_t)-1)
        {
            return size;
        }

        //!!! Наверно надо заменить на strlen/wcslen
        size = 0u;

        while(pStr[size])
        {
            ++size;
        }

        return size;
    }

    //! Длина символа в wchar_t'ах - поддержка сурогатных пар, возвращает 0/1/2, 0 - достигли конца строки
    virtual std::size_t getCharLen(const wchar_t *text, std::size_t textSize=(std::size_t)-1) const override
    {
        textSize = checkCalcStringSize(text, textSize);

        if (textSize<2u) // 0 или 1 - не важно. Если у нас началась суррогатная пара, но остался только один символ в строке - мы эту ошибку игнорим
        {
            return textSize;
        }

        // Тут у нас в строке гарантированно 2+ символов есть

        wchar_t ch = *text;

        if (ch<0xD800u || ch>0xDFFFu)
        {
            return 1u;
        }

        if (ch>=0xDC00u)
        {
            // вообще-то это ошибка, но мы просто игнорим её и инвалида пропускаем, как символ длинны 1
            return 1u;
        }

        return 2u;

    }

    //! Длина текста в символах с учетом суррогатных пар
    virtual std::size_t getTextCharsLen(const wchar_t *text, std::size_t textSize=(std::size_t)-1) const override
    {
        textSize = checkCalcStringSize(text, textSize);
        if (!textSize)
        {
            return textSize;
        }

        std::size_t sizeTotal = 0u;

        std::size_t curCharLen = getCharLen(text, textSize);
        while(curCharLen && textSize)
        {
            //sizeTotal += curCharLen;
            ++sizeTotal;
            text      += curCharLen;
            textSize  -= curCharLen;
            curCharLen = getCharLen(text, textSize);
        }

        return sizeTotal;
    }

    //! Возвращает Unicode символ, формируя его (возможно) из суррогатной пары
    virtual std::uint32_t getChar32(const wchar_t *text, std::size_t textSize=(std::size_t)-1) const override
    {
        textSize = checkCalcStringSize(text, textSize);
        if (!textSize)
        {
            return 0u;
        }

        wchar_t ch = *text;

        if (ch<0xD800u || ch>0xDFFFu)
        {
            return (std::uint32_t)ch;
        }

        if (ch>=0xDC00u)
        {
            // вообще-то это ошибка, но мы просто игнорим её и инвалида возвращаем как символ
            return (std::uint32_t)ch;
        }

        if (textSize<2u)
        {
            // суррогатная пара внезапно закончилась по окончании текста, вообще-то это ошибка, но мы её игнорим
            // инвалида возвращаем как символ
            return (std::uint32_t)ch;
        }

        std::uint32_t ch32 = ((std::uint32_t)(ch&0x03FFu)) << 10;

        ++text;
        wchar_t ch2 = *text;

        if (ch2<0xDC00u || ch2>0xDFFFu)
        {
            // суррогатная пара содержит левое значение, вообще-то это ошибка, но мы её игнорим
            // инвалида возвращаем как символ
            return (std::uint32_t)ch;
        }

        return (ch32 | (std::uint32_t)(ch2&0x03FFu));

    }

    std::basic_string<std::uint32_t> makeString32(const wchar_t *text, std::size_t textSize=(std::size_t)-1) const
    {
        textSize = checkCalcStringSize(text, textSize);

        std::basic_string<std::uint32_t> res;

        std::size_t curCharLen = getCharLen(text, textSize);
        while(curCharLen && textSize)
        {
            std::uint32_t ch32 = getChar32(text, textSize);
            res.append(1,ch32);
            text      += curCharLen;
            textSize  -= curCharLen;
            curCharLen = getCharLen(text, textSize);
        }

        return res;

    }

    std::basic_string<std::uint32_t> makeString32(const std::wstring &text) const
    {
        return makeString32(text.c_str(), text.size());
    }
    

    virtual int getCharWidthIntImpl(std::uint32_t ch) const = 0;


    std::wstring makeStopCharsString(DrawTextFlags flags, const wchar_t *stopChars) const
    {
        std::wstring wstrStopChars;
        if (stopChars)
        {
            wstrStopChars = stopChars;
        }
        else
        {
            if ((flags&DrawTextFlags::stopOnCr)!=0)
            {
                wstrStopChars.append(1u, L'\r');
            }
            if ((flags&DrawTextFlags::stopOnLf)!=0)
            {
                wstrStopChars.append(1u, L'\n');
            }
        }
    
        return wstrStopChars;
    }

    std::basic_string<std::uint32_t> makeStopCharsString32(DrawTextFlags flags, const wchar_t *stopChars) const
    {
        return makeString32(makeStopCharsString(flags, stopChars));
    }


    //! Длина текста в символах с учетом суррогатных пар и с учетом (или нет) комбайнинг символов
    virtual std::size_t getTextCharsLenEx(DrawTextFlags flags, const wchar_t *text, std::size_t textSize=(std::size_t)-1, const wchar_t *skipChars=0) const override
    {
        textSize = checkCalcStringSize(text, textSize);
        if (!textSize)
        {
            return textSize;
        }

        std::basic_string<std::uint32_t> wstrSkipChars32 = makeStopCharsString32(flags, skipChars);


        std::size_t sizeTotal = 0u;

        std::size_t curCharLen = getCharLen(text, textSize);
        while(curCharLen && textSize)
        {
            //sizeTotal += curCharLen;
            bool isCombining = false;
            std::uint32_t ch32 = getChar32(text, textSize);
            auto tmpW = getCharWidthIntImpl(ch32);
            if (tmpW==0)
            {
                isCombining = true;
            }

            if (isCombining && (flags&DrawTextFlags::combiningAsSeparateGlyph)==0)
            {
            }
            else
            {
                std::string::size_type chPos32 = wstrSkipChars32.find(ch32,0);
                if (chPos32!=std::string::npos)
                {
                    // found skip char
                }
                else
                {
                    ++sizeTotal;
                }
            }
            
            text      += curCharLen;
            textSize  -= curCharLen;
            curCharLen = getCharLen(text, textSize);
        }

        return sizeTotal;
    
    }

    virtual bool drawTextColoredEx( const DrawCoord               &startPos
                                  , const DrawCoord::value_type   &widthLim
                                  , DrawCoord::value_type         *pNextPosX //!< OUT, Положение вывода для символа, следующего за последним выведенным
                                  , DrawCoord::value_type         *pOverhang //!< OUT, Вынос элементов символа за пределы NextPosX - актуально, как минимум, для iatalic стиля шрифта
                                  , DrawTextFlags                 flags
                                  , const wchar_t                 *text
                                  , std::size_t                   textSize=(std::size_t)-1
                                  , std::size_t                   *pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                                  , const std::uint32_t           *pColors=0
                                  , std::size_t                   nColors=0
                                  , std::size_t                   *pSymbolsDrawn=0
                                  , const wchar_t                 *stopChars=0
                                  , int                           fontId=-1
                                  ) override
    {
        textSize = checkCalcStringSize(text, textSize);
        if (!textSize)
        {
            return false;
        }

        if (fontId<0)
        {
            fontId = getCurFont();
        }

        auto fontSaver = FontSaver(this, fontId);


        std::vector<DrawCoord::value_type> widths;
        if (!getCharWidths(widths, text, textSize, fontId))
        {
            return false;
        }

        SimpleFontMetrics fontMetrics;
        if (!getSimpleFontMetrics(fontMetrics, fontId))
        {
            return false;
        }

        std::basic_string<std::uint32_t> wstrStopChars32 = makeStopCharsString32(flags, stopChars);

        size_t nCharsProcessed  = 0;
        size_t nSymbolsDrawn     = 0;

        DrawCoord pos = startPos;
        DrawCoord::value_type xPosMax = pos.x + widthLim;

        std::vector<marty_draw_context::DrawCoord::value_type>::const_iterator wit = widths.begin();
        std::size_t curCharLen = getCharLen(text, textSize);
        for( ; textSize && curCharLen!=0 && wit!=widths.end() // && nCharsProcessed<textSize
             ; ++wit, curCharLen = getCharLen(text, textSize)
           )
        {
            ATLASSERT(curCharLen<=textSize);
            if (curCharLen>textSize)
            {
                break;
            }

            std::uint32_t ch32 = getChar32(text, textSize);
            std::string::size_type chPos32 = wstrStopChars32.find(ch32,0);
            if (chPos32!=std::string::npos)
            {
                break; // found stop char
            }


            const auto &curCharWidth = *wit;

            auto testPosX = pos.x;

            bool isCombining = curCharWidth<0.0001;

            if (isCombining && (flags&DrawTextFlags::combiningAsSeparateGlyph)==0)
            {
                if (nSymbolsDrawn)
                    --nSymbolsDrawn;
            }

            if ((flags&DrawTextFlags::fitGlyphStartPos)!=0)
            {
                // Стартовая позиция должна влезать в лимит
            }
            else
            {
                // Глиф должен влезать целиком, с учетом свеса
                testPosX += curCharWidth;
                // Если текущий символ нулевой ширины - то "свес" не добавляем, текущий символ должен быть отрисован
                // А раз не добавляем свес, и ширина нулевая, то условие не отличается от предыдущего символа, и текущий тоже будет отрисован
                // Если же ширина не нулевая, то для теста надо добавить свес
                if (isCombining)
                {
                    testPosX += fontMetrics.overhang;
                }
            }

            if (testPosX>=xPosMax)
            {
                break;
            }

            if ((flags&DrawTextFlags::calcOnly)==0)
            {
                // Не только считаем, но и рисуем

                std::uint32_t curUintTextColor = (std::uint32_t)-1;
                if (pColors && nSymbolsDrawn<nColors)
                {
                    curUintTextColor = pColors[nSymbolsDrawn];
                }
    
                auto textColorSaver = (curUintTextColor==(std::uint32_t)-1)
                                    ? TextColorSaver(this)
                                    : TextColorSaver(this, ColorRef::fromUnsigned(curUintTextColor) )
                                    ;

                textOut(pos, text, curCharLen);
            }

            pos.x           += curCharWidth;
            text            += curCharLen;
            textSize        -= curCharLen;
            nCharsProcessed += curCharLen;
            ++nSymbolsDrawn;
            
        }

        if (pNextPosX)
        {
            *pNextPosX = pos.x;
        }

        if (pOverhang)
        {
            *pOverhang = fontMetrics.overhang;
        }

        if (pCharsProcessed)
        {
            *pCharsProcessed = nCharsProcessed;
        }

        if (pSymbolsDrawn)
        {
            *pSymbolsDrawn = nSymbolsDrawn;
        }
        
        return true;
        
    }

    virtual bool drawTextColored  ( const DrawCoord               &startPos
                                  , const DrawCoord::value_type   &widthLim
                                  , DrawTextFlags                 flags
                                  , const wchar_t                 *text
                                  , std::size_t                   textSize=(std::size_t)-1
                                  , const std::uint32_t           *pColors=0
                                  , std::size_t                   nColors=0
                                  , const wchar_t                 *stopChars=0
                                  , int                           fontId=-1
                                  ) override
    {
        return drawTextColoredEx( startPos, widthLim
                                , 0 // pNextPosX //!< OUT, Положение вывода для символа, следующего за последним выведенным
                                , 0 // pOverhang //!< OUT, Вынос элементов символа за пределы NextPosX - актуально, как минимум, для iatalic стиля шрифта
                                , flags
                                , text
                                , textSize
                                , 0 // pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                                , pColors
                                , nColors
                                , 0 // pSymbolsDrawn=0
                                , stopChars
                                , fontId
                                );
    }

    virtual DrawingPrecise setDrawingPrecise(DrawingPrecise p) override
    {
        MARTY_IDC_ARG_USED(p);
        return DrawingPrecise::defPrecise;
    }

    virtual DrawingPrecise getDrawingPrecise() override
    {
        return DrawingPrecise::defPrecise;
    }

    virtual DcOffsetScale getOffsetScale() override
    {
        DcOffsetScale dcOs;
        dcOs.offset   = m_offset  ;
        dcOs.scale    = m_scale   ;
        dcOs.penScale = m_penScale;
        return dcOs;
    }

    virtual void restoreOffsetScale(const DcOffsetScale &dcOs) override
    {
        m_offset   = dcOs.offset  ;
        m_scale    = dcOs.scale   ;
        m_penScale = dcOs.penScale;
        updateScaledOffset();
    }

    virtual marty_draw_context::ColorRef getPenColor(int penId) override
    {
        std::map<int, marty_draw_context::PenParamsWithColor>::const_iterator pit = pensParamsById.find(penId);
        if (pit==pensParamsById.end())
        {
            return marty_draw_context::ColorRef{0,0,0};
        }

        return pit->second.color;
    }

    virtual PenParams getPenParams(int penId) override
    {
        std::map<int, marty_draw_context::PenParamsWithColor>::const_iterator pit = pensParamsById.find(penId);
        if (pit==pensParamsById.end())
        {
            return marty_draw_context::PenParams();
        }

        return PenParams(pit->second);
    }


    virtual bool getFontParamsById( int id, marty_draw_context::FontParamsW &fp ) override
    {
        if (id<0)
            return false;

        std::map<int, marty_draw_context::FontParamsW>::const_iterator fpIt = fontsParamsById.find(id);
        if (fpIt==fontsParamsById.end())
            return false;

        fp = fpIt->second;

        return true;
    }

    virtual bool getFontParamsById( int id, marty_draw_context::FontParamsA &fp ) override
    {
        marty_draw_context::FontParamsW fpw;
        if (!getFontParamsById(id,fpw))
            return false;

        fp = makeFontParamsA(fpw);

        return true;
    }


    int findPenByParams(const marty_draw_context::PenParamsWithColor &pp) const
    {
        std::map<marty_draw_context::PenParamsWithColor, int>::const_iterator it = pensByParams.find(pp);
        if (it!=pensByParams.end())
            return it->second;
        return -1;
    }

    void savePenByParams(const marty_draw_context::PenParamsWithColor &pp, int penId)
    {
        if (penId<0)
            return;

        std::map<marty_draw_context::PenParamsWithColor, int>::const_iterator it = pensByParams.find(pp);
        if (it!=pensByParams.end())
            return;

        pensByParams[pp] = penId;
        pensParamsById[penId] = pp;

    }

    int findBrushByParams(marty_draw_context::ColorRef c) const
    {
        std::map<marty_draw_context::ColorRef, int>::const_iterator it = brushesByParams.find(c);
        if (it!=brushesByParams.end())
            return it->second;
        return -1;
    }

    void saveBrushByParams(marty_draw_context::ColorRef c, int brushId)
    {
        if (brushId<0)
            return;

        std::map<marty_draw_context::ColorRef, int>::const_iterator it = brushesByParams.find(c);
        if (it!=brushesByParams.end())
            return;

        brushesByParams[c] = brushId;
    }



    virtual DrawCoord getScaledPos( const DrawCoord &c ) const override
    {
        return m_scaledOffset + m_scale*c;
    }

    virtual DrawCoord getScaledSize( const DrawCoord &c ) const override
    {
        return m_scale*c;
    }

    virtual DrawCoord::value_type getScaledSizeX( const DrawCoord::value_type &c ) const override
    {
        return m_scale.x*c;
    }

    virtual DrawCoord::value_type getScaledSizeY( const DrawCoord::value_type &c ) const override
    {
        return m_scale.y*c;
    }

    void updateScaledOffset()
    {
        m_scaledOffset = { m_offset.x*m_scale.x, m_offset.y*m_scale.y };
    }

    virtual marty_draw_context::DrawSize getDialigBaseUnits() override
    {
        long dbu = ::GetDialogBaseUnits();

        // The low-order word of the return value contains the horizontal dialog box base unit,
        // and the high-order word contains the vertical dialog box base unit.
        unsigned cx = LOWORD(dbu);
        unsigned cy = HIWORD(dbu);

        return mapRawToLogicSize(DrawCoord{cx,cy});
    }

    virtual marty_draw_context::DrawCoord mapRawToLogicPos( const marty_draw_context::DrawCoord &c ) const override
    {
        auto tmp = c - m_scaledOffset;
        return tmp / m_scale;
    }

    virtual marty_draw_context::DrawCoord mapRawToLogicSize( const marty_draw_context::DrawCoord &c ) const override
    {
        auto tmp = c;
        return tmp / m_scale;
    }

    virtual DrawCoord::value_type mapRawToLogicSizeX( const DrawCoord::value_type &c ) const override
    {
        auto tmp = c;
        return tmp / m_scale.x;
    }

    virtual DrawCoord::value_type mapRawToLogicSizeY( const DrawCoord::value_type &c ) const override
    {
        auto tmp = c;
        return tmp / m_scale.y;
    }

    virtual marty_draw_context::DrawScale setScale( const marty_draw_context::DrawScale &scale ) override
    {
        auto prev = m_scale;
        m_scale = scale;
        updateScaledOffset();
        return prev;
    }

    virtual marty_draw_context::DrawScale getScale( ) override
    {
        return m_scale;
    }


    virtual float_t setPenScale( float_t scale ) override
    {
        auto prev = m_penScale;
        m_penScale = scale;
        return prev;
    }

    virtual float_t getPenScale( ) override
    {
        return m_penScale;
    }


    virtual DrawCoord setOffset( const DrawCoord &c ) override
    {
        auto prev = m_offset;
        m_offset  = c;
        updateScaledOffset();
        return prev;
    }

    virtual DrawCoord getOffset( ) override
    {
        return m_offset;
    }

    virtual void setStringEncoding(const std::string &_encName) override
    {
        encoding::EncodingsApi* pApi = encoding::getEncodingsApi();
        encName = _encName;
        encCodepageId = pApi->getCodePageByName(encName);
    }

    virtual std::string getStringEncoding() override
    {
        return encName;
    }

    virtual bool setCollectMarkers( bool cmMode ) override
    {
        std::swap(collectMarkers,cmMode);
        return cmMode;
    }

    virtual bool getCollectMarkers( ) override
    {
        return collectMarkers;
    }


    DrawCoord::value_type markerDefSize = 1;

    virtual DrawCoord::value_type markerSetDefSize( const DrawCoord::value_type &sz ) override
    {
        DrawCoord::value_type res = markerDefSize;
        markerDefSize = sz;
        return res;
    }

    virtual DrawCoord::value_type markerGetDefSize( ) override
    {
        return markerDefSize;
    }

    virtual bool markerAdd( const DrawCoord &pos, const DrawCoord::value_type sz ) override
    {
        if (!collectMarkers)
            return false;
        markers.emplace_back(pos,sz);
        return true;
    }

    virtual bool markerAdd( const DrawCoord &pos ) override
    {
        return markerAdd(pos,markerDefSize);
    }

    virtual void markersClear( ) override
    {
        markers.clear();
    }

    virtual void markersDraw( int penId ) override
    {
        int prevPenId = -1;
        if (penId<0)
           prevPenId = selectPen(getDefaultCosmeticPen());
        else
           prevPenId = selectPen( penId );

        auto prevSmoothingMode = setSmoothingMode(marty_draw_context::SmoothingMode::none);

        auto _1 = mapRawToLogicSize(DrawCoord(1,1));

        for(const auto &m : markers)
        {
            auto sz = DrawCoord(m.size,m.size);
            rect( m.pos-sz, m.pos+sz+_1 );
        }


        selectPen( prevPenId );
        setSmoothingMode(prevSmoothingMode);
        markersClear();
    }

    virtual marty_draw_context::SmoothingMode setSmoothingMode(marty_draw_context::SmoothingMode m ) override
    {
        MARTY_ARG_USED(m);
        return marty_draw_context::SmoothingMode::defMode;
    }

    virtual marty_draw_context::SmoothingMode getSmoothingMode( ) override
    {
        return marty_draw_context::SmoothingMode::defMode;
    }



    virtual int setDefaultCosmeticPen( int penId ) override
    {
        std::swap(penId,defCosmeticPen);
        return penId;
    }

    virtual int getDefaultCosmeticPen( ) override
    {
        return defCosmeticPen;
    }

    virtual float_t getFitFontHeight(const marty_draw_context::DrawSize &boxSize) override
    {
        // fp.height = rectSize.y * 0.8;
        // https://wordyblend.com/tipografika-i-zolotoe-sechenie/
        // fp.height = rectSize.y * 0.61;
        // fp.height = rectSize.y * 0.7;
        // fp.height = rectSize.y * 0.9;
        // fp.height = rectSize.y * 0.95;
        // fp.height = rectSize.y * 0.98;
        // fp.height = rectSize.y * 1.0;
        return boxSize.y * 0.95;
    }

    template<typename StringType>
    float_t getAwgFontWidthImpl(const marty_draw_context::FontParamsT<StringType> &fp)
    {
        // return fp.height*0.7;
        return fp.height*0.65;
        // return fp.height*0.62;
        // return fp.height*0.60;
    }

    virtual float_t getAwgFontWidth(const marty_draw_context::FontParamsA &fp) override
    {
        return getAwgFontWidthImpl(fp);
    }

    virtual float_t getAwgFontWidth(const marty_draw_context::FontParamsW &fp) override
    {
        return getAwgFontWidthImpl(fp);
    }

    template<typename StringType>
    float_t getFitMarginWidthImpl(const marty_draw_context::FontParamsT<StringType> &fp)
    {
        auto awgWidth = getAwgFontWidth(fp);
        return awgWidth/4;
    }

    virtual float_t getFitMarginWidth(const marty_draw_context::FontParamsA &fp) override
    {
        return getFitMarginWidthImpl(fp);
    }

    virtual float_t getFitMarginWidth(const marty_draw_context::FontParamsW &fp) override
    {
        return getFitMarginWidthImpl(fp);
    }


    template<typename StringType>
    marty_draw_context::FontParamsW makeFontParamsW( const marty_draw_context::FontParamsT<StringType> &other ) const
    {
        marty_draw_context::FontParamsW fpRes;
        fpRes.height         = other.height        ;
        fpRes.escapement     = other.escapement    ;
        fpRes.orientation    = other.orientation   ;
        fpRes.weight         = other.weight        ;
        fpRes.fontStyleFlags = other.fontStyleFlags;

        for(auto ch : other.fontFace)
            fpRes.fontFace.append(1, (wchar_t)ch);

        return fpRes;
    }

    template<typename StringType>
    marty_draw_context::FontParamsA makeFontParamsA( const marty_draw_context::FontParamsT<StringType> &other ) const
    {
        marty_draw_context::FontParamsA fpRes;
        fpRes.height         = other.height        ;
        fpRes.escapement     = other.escapement    ;
        fpRes.orientation    = other.orientation   ;
        fpRes.weight         = other.weight        ;
        fpRes.fontStyleFlags = other.fontStyleFlags;

        for(auto ch : other.fontFace)
            fpRes.fontFace.append(1, (char)ch);

        return fpRes;
    }

    virtual int getProportionalFontCharWidthScaleApprox(const marty_draw_context::FontParamsA &fp) override
    {
        // FontParamsA fpa = makeFontParamsA(fp);

        if (fp.proportionalFontScale>0)
            return fp.proportionalFontScale;

        struct FontNameFindInfo
        {
            const char *name ;
            int         width;

            //FontNameFindInfo(const char *n=0, int w=-1) : name(n), width(w) {}
        };

        static const
        std::array<FontNameFindInfo, 2> namesForDetect =
            {
            { { "times"                 , 124 }
            , { "arial"                 , 138 }
            }
            };

        auto namesForDetectSize = namesForDetect.size();
        MARTY_ARG_USED(namesForDetectSize);
        auto fontFaceLower = marty_cpp::toLower(fp.fontFace);
        std::size_t testIdx = (std::size_t)-1;
        for( const auto &testFontInfo : namesForDetect )
        {
            ++testIdx;
            auto pos = fontFaceLower.find(testFontInfo.name);
            if (pos!=fp.fontFace.npos) // found
            {
                return namesForDetect[testIdx].width;
            }
        }

        return 100;


    }

    virtual int getProportionalFontCharWidthScaleApprox(const marty_draw_context::FontParamsW &fp) override
    {
        return getProportionalFontCharWidthScaleApprox(makeFontParamsA(fp));
    }



    virtual int getMonospacedFontCharWidthApprox(const marty_draw_context::FontParamsA &fp) override
    {
        if (fp.monospaceFontCharWidth>0)
            return fp.monospaceFontCharWidth;

        if (fp.monospaceFontCharWidth==0)
            return fp.monospaceFontCharWidth;

        // FontParamsA fpa = makeFontParamsA(fp);

        struct FontNameFindInfo
        {
            const char *name ;
            int         width;

            //FontNameFindInfo(const char *n=0, int w=-1) : name(n), width(w) {}

        };

        // https://stackoverflow.com/questions/70797173/monospace-font-characters-are-not-fixed-width

        // https://en.wikipedia.org/wiki/List_of_monospaced_typefaces

        // by default - 75% of font height

        // double {} - https://en.cppreference.com/w/cpp/container/array

        // вроде бы 50 было достаточно, но нет
        static const
        std::array<FontNameFindInfo, 51> namesForDetect =
            {
            { { "cascadia code"                                , 59 } // "Cascadia Code"
            , { "century schoolbook monospace"                 , 59 } // "Century Schoolbook Monospace"
            , { "computer modern mono"                         , 59 } // "Computer Modern Mono"
            , { "computer modern typewriter"                   , 59 } // "Computer Modern Typewriter"
            , { "latin modern mono"                            , 59 } // "Latin Modern Mono"
            , { "um typewriter"                                , 59 } // "UM Typewriter"
            , { "consolas"                                     , 59 } // "Consolas"
            , { "courier"                                      , 59 } // "Courier"
            , { "cutive mono"                                  , 59 } // "Cutive Mono"
            , { "dejavu sans mono"                             , 59 } // "DejaVu Sans Mono"
            , { "droid sans mono"                              , 59 } // "Droid Sans Mono"
            , { "everson mono"                                 , 59 } // "Everson Mono"
            , { "fira mono"                                    , 59 } // "Fira Mono"
            , { "fixedsys"                                     , 59 } // "Fixedsys"
            , { "fixed"                                        , 59 } // "Fixed"
            , { "freemono"                                     , 59 } // "FreeMono"
            , { "go mono"                                      , 59 } // "Go Mono"
            , { "hyperfont"                                    , 59 } // "HyperFont"
            , { "ibm mda"                                      , 59 } // "IBM MDA"
            , { "ibm plex mono"                                , 59 } // "IBM Plex Mono"
            , { "inconsolata"                                  , 59 } // "Inconsolata"
            , { "iosevka"                                      , 59 } // "Iosevka"
            , { "letter gothic"                                , 59 } // "Letter Gothic"
            , { "liberation mono"                              , 59 } // "Liberation Mono"
            , { "lucida console"                               , 59 } // "Lucida Console"
            , { "menlo"                                        , 59 } // "Menlo"
            , { "monaco"                                       , 59 } // "Monaco"
            , { "monofur"                                      , 59 } // "Monofur"
            , { "monospace"                                    , 59 } // "Monospace"
            , { "nimbus mono"                                  , 59 } // "Nimbus Mono"
            , { "noto mono"                                    , 59 } // "Noto Mono"
            , { "ocr-a"                                        , 59 } // "OCR-A"
            , { "ocr-b"                                        , 59 } // "OCR-B"
            , { "overpass mono"                                , 59 } // "Overpass Mono"
            , { "oxygen mono"                                  , 59 } // "Oxygen Mono"
            , { "pragmatapro"                                  , 59 } // "PragmataPro"
            , { "prestige elite"                               , 59 } // "Prestige Elite"
            , { "profont"                                      , 59 } // "ProFont"
            , { "pt mono"                                      , 59 } // "PT Mono"
            , { "roboto mono"                                  , 59 } // "Roboto Mono"
            , { "source code pro"                              , 59 } // "Source Code Pro"
            , { "terminus"                                     , 59 } // "Terminus"
            , { "tex gyre cursor"                              , 59 } // "Tex Gyre Cursor"
            , { "ubuntu mono"                                  , 59 } // "Ubuntu Mono"

            //, { ""                                             , 59 } // ""

            , { "mono"                                         , 59 } // "Mono"
            , { "monospace"                                    , 59 } // "Monospace"
            , { "typewriter"                                   , 59 } // "Typewriter"
            , { "consolas"                                     , 59 } // "Consolas"
            , { "courier"                                      , 59 } // "Courier"
            , { "fixed"                                        , 59 } // "Fixed"
            , { "HyperFont"                                    , 59 } // "HyperFont"
            // , { ""                                             , 59 } // ""
            }
            }
            ;

        auto namesForDetectSize = namesForDetect.size();
        MARTY_ARG_USED(namesForDetectSize);
        auto fontFaceLower = marty_cpp::toLower(fp.fontFace);
        std::size_t testIdx = (std::size_t)-1;
        for( const auto &testFontInfo : namesForDetect )
        {
            ++testIdx;
            auto pos = fontFaceLower.find(testFontInfo.name);
            if (pos!=fp.fontFace.npos) // found
            {
                return namesForDetect[testIdx].width;
            }
        }

        return 0;
    }

    virtual int getMonospacedFontCharWidthApprox(const marty_draw_context::FontParamsW &fp) override
    {
        marty_draw_context::FontParamsA fpa = makeFontParamsA( fp );
        return getMonospacedFontCharWidthApprox(fpa);
    }

    virtual int getMonospacedFontCharWidthApprox(int fontId) override
    {
        marty_draw_context::FontParamsA fpa;
        if (!getFontParamsById(fontId,fpa))
        {
            return 0; // -1;
        }

        return getMonospacedFontCharWidthApprox(fpa);
    }

    // int getProportionalFontCharWidthApproxImpl(wchar_t ch)

    int getProportionalFontCharWidthApproxImpl(wchar_t ch)
    {
        // Times New Roman, 22 шрифт, экранная высота - 14
        // Для других шрифтов может пропорции символов могут быть другие
        // Но в целом, для пропорционального шрифта, думаю, примерно нормас

        if ( ch==L'|' || ch==L'\'' )
           return 14; // 2/14

        if ( ch==L'/' || ch==L'\\' || ch==L':' || ch==L';' || ch==L',' || ch==L'.' || ch==L'i' || ch==L'j' || ch==L'l' || ch==L't' || ch==L'[' || ch==L']' || ch==L' ' )
           return 21; // 3/14

        if ( ch==L'!' || ch==L'f' || ch==L'I' || ch==L'-' || ch==L'(' || ch==L')' || ch==L'r' )
           return 28; // 4/14

        if ( ch==L'\"' || ch==L'J' || ch==L'a' || ch==L'c' || ch==L'e' || ch==L's' || ch==L'z' || ch==L'?' )
           return 35; // 5/14

        if ( ch==L'*' || ch==L'{' || ch==L'}' || ch==L'P' || ch==L'b' || ch==L'd' || ch==L'g' || ch==L'h'
          || ch==L'k' || ch==L'n' || ch==L'o' || ch==L'p' || ch==L'q' || ch==L'u' || ch==L'v' || ch==L'x'
          || ch==L'y' || ch==L'^' || ch==L'#' || ch==L'$' || ch==L'_' || ch==L'~' || ch==L'0' || ch==L'1'
          || ch==L'2' || ch==L'3' || ch==L'4' || ch==L'5' || ch==L'6' || ch==L'7' || ch==L'8' || ch==L'9'
           )
           return 43; // 6/14

        if ( ch==L'+' || ch==L'<' || ch==L'=' || ch==L'>' || ch==L'E' || ch==L'F' || ch==L'L' || ch==L'S' || ch==L'T' )
           return 50; // 7/14

        if ( ch==L'A' || ch==L'B' || ch==L'C' || ch==L'G' || ch==L'H' || ch==L'K' || ch==L'R' || ch==L'Z' )
           return 57; // 8/14

        if ( ch==L'm' || ch==L'w' || ch==L'&' || ch==L'D' || ch==L'N' || ch==L'O' || ch==L'Q' || ch==L'U' || ch==L'V' || ch==L'X' || ch==L'Y' )
           return 64; // 9/14

        if ( ch==L'%' )
           return 71; // 10/14

        if ( ch==L'M' || ch==L'W' || ch==L'@' )
           return 79; // 11/14

        return 46; // Средняя ширина
    }

    virtual int getProportionalFontCharWidthApprox(const marty_draw_context::FontParamsA &fp, wchar_t ch) override
    {
        int w = getProportionalFontCharWidthApproxImpl(ch);
        return w*getProportionalFontCharWidthScaleApprox(fp) / 100;
    }

    virtual int getProportionalFontCharWidthApprox(const marty_draw_context::FontParamsW &fp, wchar_t ch) override
    {
        int w = getProportionalFontCharWidthApproxImpl(ch);
        return w*getProportionalFontCharWidthScaleApprox(fp) / 100;
    }


    virtual int getProportionalFontCharWidthApprox(const marty_draw_context::FontParamsA &fp, char    ch) override
    {
        return getProportionalFontCharWidthApprox(fp, (wchar_t)ch);
    }

    virtual int getProportionalFontCharWidthApprox(const marty_draw_context::FontParamsW &fp, char    ch) override
    {
        return getProportionalFontCharWidthApprox(fp, (wchar_t)ch);
    }


    virtual int getFontCharWidthApprox(int fontId, wchar_t ch, int scale = 100) override
    {
        int cw = getMonospacedFontCharWidthApprox(fontId);
        if (cw<=0) // Not detected monospace font
        {
            marty_draw_context::FontParamsA fp;
            getFontParamsById(fontId,fp);
            cw = getProportionalFontCharWidthApprox(fp,ch);
        }

        cw *= scale;
        cw /= 100;

        return cw;
    }

    virtual int getFontCharWidthApprox(int fontId, char    ch, int scale = 100) override
    {
        return getFontCharWidthApprox(fontId, (wchar_t)ch, scale);
    }

    virtual int getFontCharWidthApprox(const marty_draw_context::FontParamsA &fp, wchar_t ch, int scale = 100) override
    {
        int cw = getMonospacedFontCharWidthApprox(fp);
        if (cw<=0) // Not detected monospace font
        {
            cw = getProportionalFontCharWidthApprox(fp, ch);
        }

        cw *= scale;
        cw /= 100;

        return cw;
    }

    virtual int getFontCharWidthApprox(const marty_draw_context::FontParamsW &fp, wchar_t ch, int scale = 100) override
    {
        int cw = getMonospacedFontCharWidthApprox(fp);
        if (cw<=0) // Not detected monospace font
        {
            cw = getProportionalFontCharWidthApprox(fp, ch);
        }

        cw *= scale;
        cw /= 100;

        return cw;
    }

    virtual int getFontCharWidthApprox(const marty_draw_context::FontParamsA &fp, char    ch, int scale = 100) override
    {
        return getFontCharWidthApprox(fp, (wchar_t)ch, scale);
    }

    virtual int getFontCharWidthApprox(const marty_draw_context::FontParamsW &fp, char    ch, int scale = 100) override
    {
        return getFontCharWidthApprox(fp, (wchar_t)ch, scale);
    }


    template <typename FontParamsType, typename CharType>
    float_t calcDrawnTextWidthApproxImpl(const FontParamsType &fp, const CharType *text, std::size_t nChars, int widthScale = 100)
    {
        //if (fp<)
        //fontParamsW.monospaceFontCharWidth = getMonospacedFontCharWidthApprox(fontParamsW);

        bool isUtf = false; // (encCodepageId==cpid_UTF8);

        if (sizeof(CharType)==1)
            isUtf = (encCodepageId== encoding::EncodingsApi::cpid_UTF8);

        int collectedWidth = 0;

        // Исходим из того, что у нас нет композитов, одни прекомпоузы. Это надо заранее сделать текст прекомпозитным
        while(nChars)
        {
            std::size_t chLen = marty_mbs::getCharLen(text, nChars, isUtf);
            if (chLen>nChars)
            {
                throw std::runtime_error("DrawContextImplBase::calcDrawnTextWidthApprox: something goes wrong (1)");
            }

            int charWidth = -1;

            if (chLen==1)
            {
                charWidth = getFontCharWidthApprox(fp, *text, widthScale);
            }
            else
            {
                charWidth = getFontCharWidthApprox(fp, (CharType)0xFFu, widthScale); // будет какое-то среднее
            }

            if (charWidth<=0)
            {
                throw std::runtime_error("DrawContextImplBase::calcDrawnTextWidthApprox: something goes wrong (2)");
            }

            collectedWidth += charWidth;

            nChars -= chLen;
            text   += chLen;
        }

        float_t floatCollectedWidth = (float_t)collectedWidth;

        floatCollectedWidth = floatCollectedWidth * fp.height;
        floatCollectedWidth = floatCollectedWidth / 100;

        if (getMonospacedFontCharWidthApprox(fp)<0)
        {
            floatCollectedWidth = floatCollectedWidth * getFontWeightWidthScale(fp.weight);
            floatCollectedWidth = floatCollectedWidth / 100;
        }

        return floatCollectedWidth;
    }

    virtual float_t calcDrawnTextWidthApprox(const marty_draw_context::FontParamsA &fp, const char*    text, std::size_t nChars, int widthScale = 100) override
    {
        return calcDrawnTextWidthApproxImpl(fp, text, nChars, widthScale);
    }

    virtual float_t calcDrawnTextWidthApprox(const marty_draw_context::FontParamsW &fp, const char*    text, std::size_t nChars, int widthScale = 100) override
    {
        return calcDrawnTextWidthApproxImpl(fp, text, nChars, widthScale);
    }

    virtual float_t calcDrawnTextWidthApprox(const marty_draw_context::FontParamsA &fp, const wchar_t* text, std::size_t nChars, int widthScale = 100) override
    {
        return calcDrawnTextWidthApproxImpl(fp, text, nChars, widthScale);
    }

    virtual float_t calcDrawnTextWidthApprox(const marty_draw_context::FontParamsW &fp, const wchar_t* text, std::size_t nChars, int widthScale = 100) override
    {
        return calcDrawnTextWidthApproxImpl(fp, text, nChars, widthScale);
    }

    virtual float_t calcDrawnTextWidthApprox(int   fontId         , const char*    text, std::size_t nChars, int widthScale = 100) override
    {
        marty_draw_context::FontParamsA fpa;
        if (!getFontParamsById(fontId,fpa))
        {
            return (float_t)0; // -1;
        }

        return calcDrawnTextWidthApprox(fpa, text, nChars, widthScale);
    }

    virtual float_t calcDrawnTextWidthApprox(int   fontId         , const wchar_t* text, std::size_t nChars, int widthScale = 100) override
    {
        marty_draw_context::FontParamsA fpa;
        if (!getFontParamsById(fontId,fpa))
        {
            return (float_t)0; // -1;
        }

        return calcDrawnTextWidthApprox(fpa, text, nChars, widthScale);
    }



    template<typename StringType>
    int getFontByParams( const marty_draw_context::FontParamsT<StringType> &fp )
    {
        auto fpw = makeFontParamsW(fp);
        std::map<marty_draw_context::FontParamsW, int>::const_iterator it = fontsByParams.find(fpw);
        if (it!=fontsByParams.end())
            return it->second;

        return createFont( fp );
    }

    virtual int  makeFontByParams( const marty_draw_context::FontParamsA &fp ) override { return getFontByParams(fp); }
    virtual int  makeFontByParams( const marty_draw_context::FontParamsW &fp ) override { return getFontByParams(fp); }
    virtual int  makeFontByParams(marty_draw_context::FontParamsA fp, float_t height ) override { fp.height = height; return getFontByParams(fp); }
    virtual int  makeFontByParams(marty_draw_context::FontParamsW fp, float_t height ) override { fp.height = height; return getFontByParams(fp); }


    virtual std::wstring decodeString( const std::string &str ) override
    {
        encoding::EncodingsApi* pApi = encoding::getEncodingsApi();
        // std::string convertedText = pApi->convert( text, encCodepageId, EncodingsApi::cpid_UTF16 );
        // std::wstring wText = pApi->decode(convertedText, EncodingsApi::cpid_UTF16 );
        return pApi->decode(str, encCodepageId );
    }

    virtual bool textOut( const DrawCoord &pos, const std::wstring &text ) = 0;
    virtual bool textOut( const DrawCoord &pos, const wchar_t *text, std::size_t textSize=(std::size_t)-1 ) = 0;

    virtual bool textOut( const DrawCoord &pos, const char    *text, std::size_t textSize=(std::size_t)-1 ) override
    {
        textSize = checkCalcStringSize(text, textSize);
        if (!textSize)
        {
            return true;
        }

        return textOut(pos, std::string(text, textSize));
    }

    virtual bool textOut( const DrawCoord &pos, const std::string &text ) override
    {
        // EncodingsApi* pApi = getEncodingsApi();
        //
        // // std::string convertedText = pApi->convert( text, encCodepageId, EncodingsApi::cpid_UTF16 );
        // // std::wstring wText = pApi->decode(convertedText, EncodingsApi::cpid_UTF16 );
        // std::wstring wText = pApi->decode(text, encCodepageId );
        return textOut( pos, decodeString(text) );
    }

    virtual bool textOut( const DrawCoord &pos, int fontId, const marty_draw_context::ColorRef &rgb, const std::string &text ) override
    {
        auto                         prevFontId    = selectFont(fontId);
        marty_draw_context::ColorRef prevTextColor = setTextColor(rgb);
        auto res = textOut( pos, text );
        selectFont(prevFontId);
        setTextColor(prevTextColor);
        return res;
    }

    virtual bool textOut( const DrawCoord &pos, int fontId, const marty_draw_context::ColorRef &rgb, const std::wstring &text ) override
    {
        auto                         prevFontId    = selectFont(fontId);
        marty_draw_context::ColorRef prevTextColor = setTextColor(rgb);
        auto res = textOut( pos, text );
        selectFont(prevFontId);
        setTextColor(prevTextColor);
        return res;
    }

    virtual int  createFont( float_t height, int escapement, int orientation, marty_draw_context::FontWeight weight, marty_draw_context::FontStyleFlags fontStyleFlags, const char    *fontFace ) = 0;
    virtual int  createFont( float_t height, int escapement, int orientation, marty_draw_context::FontWeight weight, marty_draw_context::FontStyleFlags fontStyleFlags, const wchar_t *fontFace ) = 0;

    virtual int  createFont( const marty_draw_context::FontParamsA &fp ) override
    {
        return createFont( fp.height, fp.escapement, fp.orientation, fp.weight, fp.fontStyleFlags, fp.fontFace.c_str() );
    }

    virtual int  createFont( const marty_draw_context::FontParamsW &fp ) override
    {
        return createFont( fp.height, fp.escapement, fp.orientation, fp.weight, fp.fontStyleFlags, fp.fontFace.c_str() );
    }

    virtual int  createFont(marty_draw_context::FontParamsA fp, const marty_draw_context::FontParamsA::string_type &fontFace ) override
    {
        fp.fontFace = fontFace;
        return createFont(fp);
    }

    virtual int  createFont(marty_draw_context::FontParamsW fp, const marty_draw_context::FontParamsW::string_type &fontFace ) override
    {
        fp.fontFace = fontFace;
        return createFont(fp);
    }

    virtual int  selectNewFont( const marty_draw_context::FontParamsA &fp ) override
    {
        int fontId = createFont(fp);
        selectFont(fontId);
        return fontId; // return new penId, not prev
    }

    virtual int  selectNewFont( const marty_draw_context::FontParamsW &fp ) override
    {
        int fontId = createFont(fp);
        selectFont(fontId);
        return fontId; // return new penId, not prev
    }

    virtual int createSolidPen( float_t width, marty_draw_context::LineEndcapStyle endcaps, marty_draw_context::LineJoinStyle join
                              , std::uint8_t r, std::uint8_t g, std::uint8_t b
                              ) = 0;
    //                           override
    // {
    //     return IDrawContext::createSolidPen( width, endcaps, join, r, g, b );
    // }

    virtual int createSolidPen( float_t width, marty_draw_context::LineEndcapStyle endcaps, marty_draw_context::LineJoinStyle join
                              , const marty_draw_context::ColorRef &rgb
                              ) = 0;
    //                           override
    // {
    //     return IDrawContext::createSolidPen( width, endcaps, join, rgb );
    // }

    virtual int createSolidPen( const marty_draw_context::PenParams &penParams, const marty_draw_context::ColorRef &colorRef ) override
    {
        return createSolidPen( penParams.width, penParams.endcaps, penParams.join, colorRef );
    }

    virtual int createSolidPen( const marty_draw_context::PenParamsWithColor &penParams ) override
    {
        return createSolidPen( penParams.width, penParams.endcaps, penParams.join, penParams.color );
    }

    virtual int selectNewSolidPen( const marty_draw_context::PenParams &penParams, const marty_draw_context::ColorRef &colorRef ) override
    {
        int newPenId = createSolidPen(penParams, colorRef);
        selectPen(newPenId);
        return newPenId; // return new penId, not prev
    }

    virtual int selectNewSolidPen( const marty_draw_context::PenParamsWithColor &penParams ) override
    {
        int newPenId = createSolidPen(penParams);
        selectPen(newPenId);
        return newPenId; // return new penId, not prev
    }


    virtual int selectNewSolidBrush( std::uint8_t r, std::uint8_t g, std::uint8_t b ) override
    {
        int newBrushId = createSolidBrush(r,g,b);
        selectBrush(newBrushId);
        return newBrushId; // return new brushId, not prev
    }

    virtual int selectNewSolidBrush( const marty_draw_context::ColorRef &rgb )  override
    {
        return selectNewSolidBrush(rgb.r, rgb.g, rgb.b);
    }

    //! Рисует набор горизонтальных и вертикальных линий, если две точки задают диагональную линию - это ошибка
    virtual bool roundRectFigure( const DrawCoord::value_type &cornersR
                                , std::size_t numPoints
                                , const DrawCoord             *pPoints
                                ) override
    {
        if (numPoints<2 || !pPoints)
            return false;

        DrawCoord::value_type dblR = 2*cornersR;

        if (isPathStarted())
            closeFigure();

        // DC_LOG()<<"------- roundRectFigure\n";


        // Надо рисовать в цикле линию от текущей точки к следующей
        // Прямоугольник
        // pPoints[0], pPoints[1]
        // pPoints[1], pPoints[2]
        // pPoints[2], pPoints[3]
        // pPoints[3], pPoints[0]
        // При этом нужно нарисовать дугу, соединяющую предыдущую линию и текущую.
        // Текущую линию надо укоротить с начала и с конца
        // Для этого нам надо иметь координаты предыдущей и следующей линии

        for( std::size_t i=0; i!=numPoints; ++i)
        {
            std::size_t p  = i==0  ? (numPoints-1) : (i-1);
            std::size_t n  = (i+1) %  numPoints;
            std::size_t n2 = (i+2) %  numPoints;

            DrawCoord  prevStart  = pPoints[p];
            DrawCoord  prevEnd    = pPoints[i];
            DrawCoord  curStart   = pPoints[i];
            DrawCoord  curEnd     = pPoints[n];
            DrawCoord  nextStart  = pPoints[n];
            DrawCoord  nextEnd    = pPoints[n2];

            DrawCoord  curEndOrg     = pPoints[n];
            DrawCoord  nextStartOrg  = pPoints[n];


            auto prevLt = detectLineType( prevStart, prevEnd );
            if (!isLineTypeVH(prevLt))
            {
                DC_LOG()<<"Prev line is non-VH\n";
                return false;
            }

            auto curLt  = detectLineType( curStart, curEnd );
            if (!isLineTypeVH(curLt))
            {
                DC_LOG()<<"Cur line is non-VH\n";
                return false;
            }

            auto nextLt  = detectLineType( nextStart, nextEnd );
            if (!isLineTypeVH(nextLt))
            {
                DC_LOG()<<"Next line is non-VH\n";
                return false;
            }
            // Сейчас у нас есть данные о трех линиях: предыдущая, текущая, следущая в виде пар координат:
            // { prevStart, prevEnd }, { curStart , curEnd }, { nextStart, nextEnd }

            // Все три линии - либо вертикальные, либо горизонтальные

            // Нам надо отрисовать текущую линию и закругление после неё

            // Чтобы отрисовать текущую линию, нам надо знать радиусы закруглений на стыке с предыдущей и следующей линией,
            // и укоротить текущую с обеих сторон, предыдущую - только конец, следующую - только начало


            DrawCoord::value_type prevLen = 0;
            DrawCoord::value_type curLen  = 0;
            DrawCoord::value_type nextLen = 0;

            auto prevLd = getVhLineDirection( prevStart, prevEnd, prevLt, &prevLen );
            MARTY_ARG_USED(prevLd);
            auto curLd  = getVhLineDirection( curStart , curEnd , curLt , &curLen  );
            auto nextLd = getVhLineDirection( nextStart, nextEnd, nextLt, &nextLen );

            DrawCoord::value_type rStart = cornersR;

            if (!adjustRoundedCornerLinesLen( prevStart, prevEnd, prevLt, prevLen
                                            , curStart , curEnd , curLt , curLen
                                            , dblR, rStart
                                            )
               )
            {
                DC_LOG()<<"Failed to adjust rounded corner lines len (adjustRoundedCornerLinesLen) for prev line\n";
                return false;
            }


            DrawCoord::value_type rEnd = cornersR;

            if (!adjustRoundedCornerLinesLen( curStart , curEnd , curLt , curLen
                                            , nextStart, nextEnd, nextLt, nextLen
                                            , dblR, rEnd
                                            )
               )
            {
                DC_LOG()<<"Failed to adjust rounded corner lines len (adjustRoundedCornerLinesLen) for cur line\n";
                return false;
            }

            markerAdd(curStart);
            markerAdd(curEnd);


            if (i==0)
            {
                // DC_LOG()<<"Line from                : ("<<curStart<<")\n";
                moveTo( curStart );
            }

            lineTo( curEnd   );
            // DC_LOG()<<"Line to                  : ("<<curEnd<<")\n";


            // Закругление после текущей линии

            dblR = rEnd*2;

            DrawCoord leftTop          ;
            DrawCoord rightBottom      ;
            DrawCoord arcStartRefPoint ;
            DrawCoord arcEndRefPoint   ;

            bool directionCounterclockwise = false;

            // DrawCoord  curEndOrg     = pPoints[n];
            // DrawCoord  nextStartOrg  = pPoints[n];

            arcStartRefPoint = curEnd;
            arcEndRefPoint   = nextStart;

            // fromBottomToTop = fromRightToLeft
            // fromTopToBottom = fromLeftToRight


            if (curLt==nextLt)
            {
                // Текущая и следующая линии - одинакового типа, закруглений не требуется
                // DC_LOG()<<"Lines (cur&next) are same\n";
            }
            else if (curLt== marty_draw_context::LineType::vertical)
            {
                //DC_LOG()<<"Lines (cur&next) are same\n";

                // текущая линия - вертикальная, значит следующая - горизонтальная, влево или вправо
                // текущая линия может идти как снизу вверх, так и сверху вниз

                if (curLd== marty_draw_context::LineDirection::fromTopToBottom)
                {
                    if (nextLd== marty_draw_context::LineDirection::fromLeftToRight)
                    {
                        leftTop         .x = curEndOrg.x;
                        leftTop         .y = curEndOrg.y-dblR;
                        rightBottom     .x = curEndOrg.x+dblR /* +1 */ ;
                        rightBottom     .y = curEndOrg.y      /* +1 */ ;
                        directionCounterclockwise = marty_draw_context::ArcDirection::CounterClockwise;
                        //dblR
                    }
                    else // curLd==LineDirection::fromRightToLeft
                    {
                        leftTop         .x = curEndOrg.x-dblR;
                        leftTop         .y = curEndOrg.y-dblR;
                        rightBottom     .x = curEndOrg.x;
                        rightBottom     .y = curEndOrg.y;
                        directionCounterclockwise = false;
                    }
                }
                else // curLd==LineDirection::fromBottomToTop
                {
                    if (nextLd== marty_draw_context::LineDirection::fromLeftToRight)
                    {
                        leftTop         .x = curEndOrg.x;
                        leftTop         .y = curEndOrg.y;
                        rightBottom     .x = curEndOrg.x+dblR;
                        rightBottom     .y = curEndOrg.y+dblR;
                        directionCounterclockwise = marty_draw_context::ArcDirection::Clockwise;
                    }
                    else // curLd==LineDirection::fromRightToLeft
                    {
                        leftTop         .x = curEndOrg.x-dblR;
                        leftTop         .y = curEndOrg.y;
                        rightBottom     .x = curEndOrg.x;
                        rightBottom     .y = curEndOrg.y+dblR;
                        directionCounterclockwise = marty_draw_context::ArcDirection::CounterClockwise;
                    }
                }
            }
            else
            {
                // текущая линия - горизонтальная, значит следующая - вертикальная, вверх или вниз
                // текущая линия может идти как слева направо, так и справа налево

                if (curLd== marty_draw_context::LineDirection::fromLeftToRight)
                {
                    if (nextLd== marty_draw_context::LineDirection::fromTopToBottom)
                    {
                        leftTop         .x = curEndOrg.x-dblR;
                        leftTop         .y = curEndOrg.y;
                        rightBottom     .x = curEndOrg.x;
                        rightBottom     .y = curEndOrg.y+dblR;
                        directionCounterclockwise = marty_draw_context::ArcDirection::Clockwise;
                    }
                    else // curLd==LineDirection::fromBottomToTop
                    {
                        leftTop         .x = curEndOrg.x-dblR;
                        leftTop         .y = curEndOrg.y-dblR;
                        rightBottom     .x = curEndOrg.x;
                        rightBottom     .y = curEndOrg.y;
                        directionCounterclockwise = marty_draw_context::ArcDirection::CounterClockwise;
                    }
                }
                else // curLd==LineDirection::fromRightToLeft
                {
                    if (nextLd== marty_draw_context::LineDirection::fromTopToBottom)
                    {
                        leftTop         .x = curEndOrg.x;
                        leftTop         .y = curEndOrg.y;
                        rightBottom     .x = curEndOrg.x+dblR;
                        rightBottom     .y = curEndOrg.y+dblR;
                        directionCounterclockwise = marty_draw_context::ArcDirection::CounterClockwise;
                    }
                    else // curLd==LineDirection::fromBottomToTop
                    {
                        leftTop         .x = curEndOrg.x;
                        leftTop         .y = curEndOrg.y-dblR;
                        rightBottom     .x = curEndOrg.x+dblR;
                        rightBottom     .y = curEndOrg.y;
                        directionCounterclockwise = marty_draw_context::ArcDirection::Clockwise;
                    }
                }
            }

            if (curLt!=nextLt)
            {
                // if (!(arcStartRefPoint.x==0 && arcStartRefPoint.x==arcStartRefPoint.y && arcStartRefPoint.y==arcEndRefPoint.x && arcEndRefPoint.x==arcEndRefPoint.y))
                if (!(dblR<DrawCoord::value_type(0.2)))
                {
                    DrawCoord arcCenter = (leftTop+rightBottom) / DrawCoord(2,2);

                    markerAdd(arcCenter);
                    markerAdd(leftTop);
                    markerAdd(rightBottom);
                    markerAdd(arcStartRefPoint);
                    markerAdd(arcEndRefPoint);

                    // DC_LOG()<<"Arc center               : ("<<arcCenter<<")\n";
                    //  
                    // DC_LOG()<<"Arc rect leftTop         : ("<<leftTop<<")\nArc rect rightBottom     : ("<<rightBottom<<")\nArc rect arcStartRefPoint: ("<<arcStartRefPoint<<")\nArc rect arcEndRefPoint  : ("<<arcEndRefPoint<<")\nArc ccw                  : "<<directionCounterclockwise<<"\n";

                    if (!ellipticArcTo( leftTop, rightBottom, arcStartRefPoint, arcEndRefPoint, directionCounterclockwise ))
                    {
                        return false;
                    }

                    // moveTo( curStart ); //???
                    // ellipticArcTo( leftTop, rightBottom, arcStartRefPoint, arcEndRefPoint, !directionCounterclockwise );
                }

                // DC_LOG()<<"---\n";
                // DC_LOG()<<"Line to                  : ("<<nextStart<<")\n";

                lineTo( nextStart );
            }

        }

        return true;

    }

    //! Implemented offten with serios of the lineTo and ellipticArcTo calls
    // virtual bool roundRect( const DrawCoord::value_type &cornersR
    //                       , const DrawCoord             &leftTop
    //                       , const DrawCoord             &rightBottom
    //                       ) override
    // {
    //     MARTY_IDC_ARG_USED(cornersR);
    //  
    //     DrawCoord coords[4] = { leftTop
    //                           , { rightBottom.x, leftTop.y } // rightTop
    //                           , rightBottom
    //                           , { leftTop.x, rightBottom.y } // leftBottom
    //                           };
    //  
    //     auto res = roundRectFigure( cornersR, sizeof(coords)/sizeof(coords[0]), &coords[0] );
    //     MARTY_IDC_ARG_USED(res);
    //     if (isPathStarted())
    //     {
    //         if (!closeFigure())
    //             return false;
    //     }
    //     return true;
    // }

    template<typename StringType>
    bool drawTextFitHeighExImpl(marty_draw_context::FontParamsT<StringType>      fp
                               , DrawCoord::value_type                           fontHeightScale
                               , const DrawCoord                                 &rectPos
                               , const DrawCoord                                 &rectSize
                               , const StringType                                &text//, const StringType &fontFace
                               , marty_draw_context::HorAlign                    a = marty_draw_context::HorAlign::left
                               , float_t                                         xMarginScale = 0
                               , DrawCoord                                       *pTextPos = 0
                               , DrawCoord                                       *pTextSize = 0
                               )
    {
        // DC_LOG()<<"drawTextFitHeighImpl for "<<text<<"\n";
        fp.height = getFitFontHeight(rectSize) * fontHeightScale;
        // DC_LOG()<<"Height: "<<fp.height<<"\n";
        // DC_LOG()<<"fontsByParams.size(): "<<fontsByParams.size()<<"\n";


        //fp.fontFace
        int fontId = getFontByParams(fp);
        auto prevFontId = selectFont(fontId);

        auto awgWidth = getAwgFontWidth(fp);

        // auto marginX = awgWidth/3;
        // auto marginY = awgWidth/3;
        if (xMarginScale<0.01)
            xMarginScale = 1.0;
        auto marginX = getFitMarginWidth(fp)*xMarginScale;

        auto offsY = (rectSize.y - fp.height)/2;

        auto textWidth = awgWidth*(float_t)text.size();
        float_t offsX = 0;

        switch(a)
        {
            case HorAlign::left   :
                                       offsX = marginX;
                                       break;

            case HorAlign::center :    // offsX = (rectSize.x - textWidth)/2;
                                       offsX = (rectSize.x - textWidth)/2;
                                       break;

            case HorAlign::right  :
                                       offsX = rectSize.x - marginX - textWidth;
                                       break;

            case HorAlign::invalid:    [[fallthrough]];

            default: {}
        }

        if (offsX<marginX)
            offsX = marginX;


        auto leftTop = rectPos+DrawCoord{offsX,offsY};

        if (pTextPos)
           *pTextPos  = DrawCoord{offsX,offsY};
        if (pTextSize)
           *pTextSize = DrawCoord{ textWidth, fp.height };

        // rect( leftTop, leftTop+DrawCoord{textWidth,fp.height} );

        auto res = textOut( leftTop, text );
        selectFont(prevFontId);

        return res;
    }


    // virtual bool drawTextFitHeighEx( const FontParamsA &fp, DrawCoord::value_type fontHeightScale, const DrawCoord &rectPos, const DrawCoord &rectSize, const std::string  &text /* , const std::string  &fontFace="Courier New"  */ , HorAlign a = HorAlign::left, float_t xMarginScale = 0, DrawCoord *pTextPos = 0, DrawCoord *pTextSize = 0 ) = 0;
    // virtual bool drawTextFitHeighEx( const FontParamsW &fp, DrawCoord::value_type fontHeightScale, const DrawCoord &rectPos, const DrawCoord &rectSize, const std::wstring &text /* , const std::wstring &fontFace=L"Courier New" */ , HorAlign a = HorAlign::left, float_t xMarginScale = 0, DrawCoord *pTextPos = 0, DrawCoord *pTextSize = 0 ) = 0;


    virtual bool drawTextFitHeighEx( const marty_draw_context::FontParamsA &fp
                                   , DrawCoord::value_type fontHeightScale
                                   , const DrawCoord &rectPos, const DrawCoord &rectSize
                                   , const std::string &text//, const std::string  &fontFace
                                   , marty_draw_context::HorAlign a = marty_draw_context::HorAlign::left
                                   , float_t xMarginScale = 0
                                   , DrawCoord *pTextPos = 0, DrawCoord *pTextSize = 0
                                   ) override
    {
        return drawTextFitHeighExImpl( fp, fontHeightScale, rectPos, rectSize, text, a, xMarginScale, pTextPos, pTextSize );
    }

    virtual bool drawTextFitHeighEx( const marty_draw_context::FontParamsW &fp
                                   , DrawCoord::value_type fontHeightScale
                                   , const DrawCoord &rectPos, const DrawCoord &rectSize
                                   , const std::wstring &text//, const std::wstring  &fontFace
                                   , marty_draw_context::HorAlign a = marty_draw_context::HorAlign::left
                                   , float_t xMarginScale = 0
                                   , DrawCoord *pTextPos = 0, DrawCoord *pTextSize = 0
                                   ) override
    {
        return drawTextFitHeighExImpl( fp, fontHeightScale, rectPos, rectSize, text, a, xMarginScale, pTextPos, pTextSize );
    }

    virtual bool drawTextFitHeigh( const marty_draw_context::FontParamsA &fp
                                 , const DrawCoord &rectPos, const DrawCoord &rectSize
                                 , const std::string &text//, const std::string  &fontFace
                                 , marty_draw_context::HorAlign a = marty_draw_context::HorAlign::left
                                 , float_t xMarginScale = 0
                                 , DrawCoord *pTextPos = 0, DrawCoord *pTextSize = 0
                                 ) override
    {
        return drawTextFitHeighExImpl( fp, (DrawCoord::value_type)(1.0), rectPos, rectSize, text, a, xMarginScale, pTextPos, pTextSize );
    }

    virtual bool drawTextFitHeigh( const marty_draw_context::FontParamsW &fp
                                 , const DrawCoord &rectPos, const DrawCoord &rectSize
                                 , const std::wstring &text//, const std::wstring  &fontFace
                                 , marty_draw_context::HorAlign a = marty_draw_context::HorAlign::left
                                 , float_t xMarginScale = 0
                                 , DrawCoord *pTextPos = 0, DrawCoord *pTextSize = 0
                                 ) override
    {
        return drawTextFitHeighExImpl( fp, (DrawCoord::value_type)(1.0), rectPos, rectSize, text, a, xMarginScale, pTextPos, pTextSize );
    }


    virtual bool arcTo(const DrawCoord &centerPos, const DrawCoord &endPos, bool directionCounterclockwise, DrawCoord *pResEndPos = 0 ) override
    {
        DrawCoord startPos = getCurPos( );
        const DrawCoord &startVectorStartPos = centerPos;
        const DrawCoord &startVectorEndPos   = startPos;

        auto startVector = startVectorEndPos-startVectorStartPos;

        const DrawCoord &endVectorStartPos   = centerPos;
        const DrawCoord &endVectorEndPos     = endPos;

        auto endVector = endVectorEndPos-endVectorStartPos;

        // DC_LOG() << "GdiDcBase::arcTo (1)\n";
        // DC_LOG() << "  startPos           : " << startPos            << "\n";
        // DC_LOG() << "  startVectorStartPos: " << startVectorStartPos << "\n";
        // DC_LOG() << "  startVectorEndPos  : " << startVectorEndPos   << "\n";
        // DC_LOG() << "  startVector        : " << startVector         << "\n";
        // DC_LOG() << "  endVectorStartPos  : " << endVectorStartPos   << "\n";
        // DC_LOG() << "  endVectorEndPos    : " << endVectorEndPos     << "\n";
        // DC_LOG() << "  endVector          : " << endVector           << "\n";


        double startAngleRadians;
        double endAngleRadians;
        double vectorLen;

        if (!math_helpers::calcVectorAngle((double)startVector.x, -(double)startVector.y, startAngleRadians, &vectorLen))
        {
            // DC_LOG()<<"pDc->arcTo - something goes wrong\n";
            return false;
        }

        if (!math_helpers::calcVectorAngle((double)endVector.x, -(double)endVector.y, endAngleRadians))
        {
            // DC_LOG()<<"pDc->arcTo - something goes wrong\n";
            return false;
        }

        // DC_LOG() << "  startAngleRadians  : " << startAngleRadians   << "\n";
        // DC_LOG() << "  endAngleRadians    : " << endAngleRadians     << "\n";
        // DC_LOG() << "  startAngle         : " << math_helpers::angleToDegrees(startAngleRadians)  << "\n";
        // DC_LOG() << "  endAngle           : " << math_helpers::angleToDegrees(endAngleRadians  )  << "\n";


        auto endPointX = centerPos.x + vectorLen*std::cos(endAngleRadians);
        auto endPointY = centerPos.y - vectorLen*std::sin(endAngleRadians);

        auto exactEndPos = DrawCoord{ endPointX, endPointY };

        // DC_LOG() << "  endPointX          : " << endPointX           << "\n";
        // DC_LOG() << "  endPointY          : " << endPointY           << "\n";
        // DC_LOG() << "  exactEndPos        : " << exactEndPos         << "\n";
        // DC_LOG()<<"    direction          : " << (directionCounterclockwise ? "CCW" : "CW") << "\n";


        if (pResEndPos)
        {
            *pResEndPos = exactEndPos;
        }

        // DC_LOG()<<"pDc->arcTo - centerPos    : " << "{" << centerPos.x << "," << centerPos.y << "}" << "\n";
        // DC_LOG()<<"             startPos     : " << "{" << startPos .x << "," << startPos .y << "}" << "\n";
        // DC_LOG()<<"             endPos       : " << "{" << endPos   .x << "," << endPos   .y << "}" << "\n";
        // DC_LOG()<<"             startAngle   : " <<        math_helpers::angleToDegrees(startAngleRadians) << "\n";
        // DC_LOG()<<"             endAngle     : " <<        math_helpers::angleToDegrees(endAngleRadians  ) << "\n";
        // DC_LOG()<<"             exactEndPos  : " << "{" << exactEndPos.x << "," << exactEndPos.y << "}" << "\n";
        // DC_LOG()<<"             direction    : " <<        directionCounterclockwise << "\n";


        double left   = (double)centerPos.x - vectorLen;
        double right  = (double)centerPos.x + vectorLen;
        double top    = (double)centerPos.y - vectorLen;
        double bottom = (double)centerPos.y + vectorLen;

        // DC_LOG() << "  left               : " << left          << "\n";
        // DC_LOG() << "  right              : " << right         << "\n";
        // DC_LOG() << "  top                : " << top           << "\n";
        // DC_LOG() << "  bottom             : " << bottom        << "\n";
        // DC_LOG() << "\n";
        //  
        // DC_LOG() << "Calling GdiDcBase::ellipticArcTo from GdiDcBase::arcTo (1)\n";

        if (!ellipticArcTo( DrawCoord{ left , top    }
                          , DrawCoord{ right, bottom }
                          , startPos
                          , exactEndPos
                          , directionCounterclockwise
                          )
           )
        {
            return false;
        }

        if (!moveTo(exactEndPos))
        {
            return false;
        }

        return true;
    }

    virtual bool arcTo(const DrawCoord &centerPos, const DrawCoord::value_type angleDegrees, DrawCoord *pResEndPos = 0 ) override
    {
        DrawCoord startPos = getCurPos( );
        const DrawCoord &startVectorStartPos = centerPos;
        const DrawCoord &startVectorEndPos   = startPos;

        auto startVector = startVectorEndPos-startVectorStartPos;

        double startAngleRadians;
        double vectorLen;

        // DC_LOG() << "GdiDcBase::arcTo (2)\n";
        // DC_LOG() << "  centerPos          : " << centerPos           << "\n";
        // DC_LOG() << "  startVectorStartPos: " << startVectorStartPos << "\n";
        // DC_LOG() << "  startVectorEndPos  : " << startVectorEndPos   << "\n";
        // DC_LOG() << "  startVector        : " << startVector         << "\n";


        if (!math_helpers::calcVectorAngle((double)startVector.x, -(double)startVector.y, startAngleRadians, &vectorLen))
        {
            // DC_LOG()<<"pDc->arcTo - something goes wrong\n";
            return false;
        }

        double arcAngleRadians = math_helpers::angleToRadians( double(angleDegrees) );

        auto endPointAngleRadians = startAngleRadians + arcAngleRadians;

        auto endPointX = centerPos.x + vectorLen*std::cos(endPointAngleRadians);
        auto endPointY = centerPos.y - vectorLen*std::sin(endPointAngleRadians);

        auto endPos = DrawCoord{ endPointX, endPointY };

        if (pResEndPos)
        {
            *pResEndPos = endPos;
        }

        bool directionCounterclockwise = angleDegrees>0
                                       ? true  // CounterClockWise
                                       : false // ClockWise
                                       ;

        // DC_LOG() << "  endPointX          : " << endPointX           << "\n";
        // DC_LOG() << "  endPointY          : " << endPointY           << "\n";
        // DC_LOG() << "  endPos             : " << endPos              << "\n";
        // DC_LOG() << "  direction          : " << (directionCounterclockwise ? "CCW" : "CW") << "\n";

        // DC_LOG() << "  angleDegrees        : " << angleDegrees         << "\n";
        // DC_LOG() << "  vectorLen           : " << vectorLen            << "\n";
        // DC_LOG() << "  startAngleRadians   : " << startAngleRadians    << "\n";
        // DC_LOG() << "  endPointAngleRadians: " << endPointAngleRadians << "\n";
        // DC_LOG() << "  startAngle          : " << math_helpers::angleToDegrees(startAngleRadians)    << "\n";
        // DC_LOG() << "  endPointAngle       : " << math_helpers::angleToDegrees(endPointAngleRadians) << "\n";

        double left   = (double)centerPos.x - vectorLen;
        double right  = (double)centerPos.x + vectorLen;
        double top    = (double)centerPos.y - vectorLen;
        double bottom = (double)centerPos.y + vectorLen;

        // DC_LOG() << "  left               : " << left          << "\n";
        // DC_LOG() << "  right              : " << right         << "\n";
        // DC_LOG() << "  top                : " << top           << "\n";
        // DC_LOG() << "  bottom             : " << bottom        << "\n";
        // DC_LOG() << "\n";
        //  
        // DC_LOG() << "Calling GdiDcBase::ellipticArcTo from GdiDcBase::arcTo (2)\n";


        if (!ellipticArcTo( DrawCoord{ left , top    }
                          , DrawCoord{ right, bottom }
                          , startPos
                          , endPos
                          , directionCounterclockwise
                          )
           )
        {
            return false;
        }

        if (!moveTo(endPos))
        {
            return false;
        }

        return true;

    }




    // UHD or Ultra High-definition - 3840 x 2160
    // 16384 хватит для всех
    // Или 8192?
    // Или 4096?
    // 512?

    std::uint32_t* makeGradientRgbArray( const marty_draw_context::ColorRef     &gradientColorBegin
                                       , const marty_draw_context::ColorRef     &gradientColorEnd
                                       , const std::size_t  gradientNumVals
                                       , std::uint32_t      *pColorsBuf // must be enough
                                       )
    {
        if (!gradientNumVals)
            return pColorsBuf;

        int br = (int)gradientColorBegin.r; br <<= 16;
        int bg = (int)gradientColorBegin.g; bg <<= 16;
        int bb = (int)gradientColorBegin.b; bb <<= 16;

        int er = (int)gradientColorEnd.r; er <<= 16;
        int eg = (int)gradientColorEnd.g; eg <<= 16;
        int eb = (int)gradientColorEnd.b; eb <<= 16;

        int dr = er - br; dr /= (int)gradientNumVals;
        int dg = eg - bg; dg /= (int)gradientNumVals;
        int db = eb - bb; db /= (int)gradientNumVals;

        auto addDelta = []( int v, int d ) -> int
            {
                v += d;
                if (v<0)
                    v = 0;

                return v;
            };

        for(std::size_t i=0u; i!=gradientNumVals; ++i, pColorsBuf++)
        {
            br = addDelta(br, dr);
            bg = addDelta(bg, dg);
            bb = addDelta(bb, db);

            marty_draw_context::ColorRef clr;
            clr.r = (std::uint8_t)(br >> 16);
            clr.g = (std::uint8_t)(bg >> 16);
            clr.b = (std::uint8_t)(bb >> 16);
            *pColorsBuf = clr.toUnsigned();
        }

        return pColorsBuf;
    }

    void makeGradientRgbArray( const marty_draw_context::ColorRef              &gradientColorBegin
                             , const marty_draw_context::ColorRef              &gradientColorMid
                             , const marty_draw_context::ColorRef              &gradientColorEnd
                             , const marty_draw_context::DrawCoord::value_type &gradientMidPoint
                             , const std::size_t                               gradientNumVals
                             , std::uint32_t                                   *pColorsBuf // must be enough
                             )
    {
        if (gradientNumVals>4096)
        {
            throw std::runtime_error("makeGradientRgbArray: gradient count too much");
        }

        marty_draw_context::DrawCoord::value_type gradientMidPointScaled = gradientMidPoint*(DrawCoord::value_type)gradientNumVals;

        std::size_t firstPartSize  = (std::size_t)gradientMidPointScaled;
        std::size_t secondPartSize = gradientNumVals - firstPartSize;

        pColorsBuf =
        makeGradientRgbArray( gradientColorBegin, gradientColorMid, firstPartSize , pColorsBuf );
        makeGradientRgbArray( gradientColorMid  , gradientColorEnd, secondPartSize, pColorsBuf );
    }


    virtual void drawLineInLogicalCoordsForFillGradientRect
                                 ( int left
                                 , int top
                                 , int right
                                 , int bottom
                                 , const marty_draw_context::ColorRef &clr
                                 ) = 0;

    virtual void drawCircleInLogicalCoordsForFillGradientCircle
                                 ( int x
                                 , int y
                                 , int r
                                 , const marty_draw_context::ColorRef &clr
                                 ) = 0;

    virtual bool fillGradientRect( const DrawCoord             &leftTop
                                 , const DrawCoord             &rightBottom
                                 , const marty_draw_context::ColorRef              &gradientColorBegin
                                 , const marty_draw_context::ColorRef              &gradientColorMid
                                 , const marty_draw_context::ColorRef              &gradientColorEnd
                                 , const DrawCoord::value_type &gradientMidPoint
                                 , marty_draw_context::GradientType                gradientType
                                 , bool                        excludeFrame
                     ) override
    {
        // return rect( leftTop, rightBottom );

        if (isPathStarted())
        {
            return false;
        }

        DrawCoord scaledLeftTop     = getScaledPos(leftTop);
        DrawCoord scaledRightBottom = getScaledPos(rightBottom);

        int left   = int(floatToInt(scaledLeftTop    .x));
        int top    = int(floatToInt(scaledLeftTop    .y));
        int right  = int(floatToInt(scaledRightBottom.x));
        int bottom = int(floatToInt(scaledRightBottom.y));

        if (excludeFrame)
        {
            ++left;
            ++top;
            --right;
            --bottom;
        }


        if (bottom<top)
            std::swap(bottom,top);

        if (right<left)
            std::swap(right,left);


        int startPos = 0, endPos = 0;

        if (gradientType== marty_draw_context::GradientType::vertical)
        {
            // при вертикальном градиенте рисуем горизонтальные линии одного цвета
            // градиент идёт сверху вниз
            startPos = top, endPos = bottom;
        }
        else
        {
            // при горизонтальном градиенте рисуем вертикальные линии одного цвета
            // градиент идёт слева направо
            startPos = left, endPos = right;
        }

        std::size_t gradientNumVals = (std::size_t)(endPos-startPos);


        std::uint32_t gradientColorsBuf[gradientColorsBufSize+1];

        std::size_t stepSize = 1;

        while(gradientNumVals>=gradientColorsBufSize)
        {
            gradientNumVals /= 2;
            stepSize        *= 2;
        }

        makeGradientRgbArray( gradientColorBegin, gradientColorMid , gradientColorEnd
                            , gradientMidPoint  , gradientNumVals+1, &gradientColorsBuf[0]
                            );

        std::size_t gradientIdx = 0;
        for(int curPos=startPos; curPos<=endPos; ++gradientIdx)
        {
            std::size_t stepIdx = 0;
            for(; stepIdx!=stepSize && curPos<=endPos; ++curPos, ++stepIdx)
            {
                marty_draw_context::ColorRef clr = marty_draw_context::ColorRef::fromUnsigned(gradientColorsBuf[gradientIdx]);

                // draw line here

                if (gradientType== marty_draw_context::GradientType::vertical)
                {
                    // при вертикальном градиенте рисуем горизонтальные линии одного цвета
                    drawLineInLogicalCoordsForFillGradientRect( left, curPos, right+1, curPos, clr );
                }
                else
                {
                    // при горизонтальном градиенте рисуем вертикальные линии одного цвета
                    drawLineInLogicalCoordsForFillGradientRect( curPos, top, curPos, bottom+1, clr );
                }
            }
        }

        return true;
    }

// enum class GradientRoundRectFillFlags : std::uint32_t
// {
//     invalid       = (std::uint32_t)(-1),
//     round         = 0x00,
//     roundBoth     = 0x00,
//     fillFull      = 0x00,
//     squareBegin   = 0x01,
//     squareEnd     = 0x02,
//     noFillBegin   = 0x04,
//     noFillEnd     = 0x08
//  
// }; // enum class GradientRoundRectFillFlags : std::uint32_t

    virtual bool fillGradientRoundRect( DrawCoord::value_type                      cornersR
                                 , const DrawCoord                                 &leftTop
                                 , const DrawCoord                                 &rightBottom
                                 , const marty_draw_context::ColorRef              &gradientColorBegin
                                 , const marty_draw_context::ColorRef              &gradientColorMid
                                 , const marty_draw_context::ColorRef              &gradientColorEnd
                                 , const DrawCoord::value_type                     &gradientMidPoint
                                 , marty_draw_context::GradientType                gradientType
                                 , bool                                            excludeFrame
                                 , DrawCoord::value_type                           fillBreakPos
                                 , marty_draw_context::GradientRoundRectFillFlags  fillFlags // = GradientRoundRectFillFlags::roundBoth | GradientRoundRectFillFlags::fillFull
                     ) override
    {
        MARTY_ARG_USED(cornersR);

        if (isPathStarted())
        {
            return false;
        }

        if (fillBreakPos<0)
        {
           fillBreakPos = 0;
        }

        if (fillBreakPos>1)
        {
           fillBreakPos = 1;
        }

        bool squareBegin   = false;
        bool squareEnd     = false;
        bool noFillBegin   = false;
        bool noFillEnd     = false;


        if ((fillFlags& marty_draw_context::GradientRoundRectFillFlags::squareBegin)!=0)
        {
            squareBegin = true;
        }

        if ((fillFlags& marty_draw_context::GradientRoundRectFillFlags::squareEnd)!=0)
        {
            squareEnd = true;
        }

        if ((fillFlags& marty_draw_context::GradientRoundRectFillFlags::noFillBegin)!=0)
        {
            noFillBegin = true;
        }

        if ((fillFlags& marty_draw_context::GradientRoundRectFillFlags::noFillEnd)!=0)
        {
            noFillEnd = true;
        }


        // DC_LOG() << "\n";
        // DC_LOG() << "--- fillGradientRoundRect\n";
        //  
        // DC_LOG() << "\nSquare & fill\n";
        // DC_LOG() << "squareBegin: " << squareBegin << "\n";
        // DC_LOG() << "squareEnd  : " << squareEnd   << "\n";
        // DC_LOG() << "noFillBegin: " << noFillBegin << "\n";
        // DC_LOG() << "noFillEnd  : " << noFillEnd   << "\n";


        DrawCoord wh = DrawCoord{rightBottom.x - leftTop.x, rightBottom.y - leftTop.y};
        wh.x = wh.x<0 ? -wh.x : wh.x;
        wh.y = wh.y<0 ? -wh.y : wh.y;

        // DC_LOG() << "wh.x   : " << wh.x << "\n";
        // DC_LOG() << "wh.y   : " << wh.y << "\n";
        // DC_LOG() << "R      : " << cornersR << "\n";
        // DC_LOG() << "---\n";

        // Двойной радиус не должен быть больше меньшего из (ширина,высота)
        DrawCoord::value_type R = cornersR;
        {
            DrawCoord::value_type minSize = std::min(wh.x, wh.y);
            DrawCoord::value_type minSize_2 = minSize/2;

            if (R>minSize_2)
            {
                R = minSize_2;
            }
        }


        DrawCoord scaledR = getScaledSize(DrawCoord{R,R});

        // DC_LOG() << "scaledR: " << scaledR << "\n";
        // DC_LOG() << "---\n";

        DrawCoord scaledLeftTop     = getScaledPos(leftTop);
        DrawCoord scaledRightBottom = getScaledPos(rightBottom);

        int left   = int(floatToInt(scaledLeftTop    .x));
        int top    = int(floatToInt(scaledLeftTop    .y));
        int right  = int(floatToInt(scaledRightBottom.x));
        int bottom = int(floatToInt(scaledRightBottom.y));

        if (excludeFrame)
        {
            ++left;
            ++top;
            --right;
            --bottom;
        }

        // DC_LOG() << "Input pos\n";
        // DC_LOG() << "left   : " << leftTop    .x  << "\n";
        // DC_LOG() << "top    : " << leftTop    .y  << "\n";
        // DC_LOG() << "right  : " << rightBottom.x  << "\n";
        // DC_LOG() << "bottom : " << rightBottom.y  << "\n";
        // DC_LOG() << "---\n";
        // DC_LOG() << "Scaled pos\n";
        // DC_LOG() << "left   : " << left    << "\n";
        // DC_LOG() << "top    : " << top     << "\n";
        // DC_LOG() << "right  : " << right   << "\n";
        // DC_LOG() << "bottom : " << bottom  << "\n";
        // DC_LOG() << "---\n";
        // DC_LOG() << "m_scale.x : " << m_scale.x  << "\n";
        // DC_LOG() << "m_scale.y : " << m_scale.y  << "\n";
        // DC_LOG() << "---\n";
        // DC_LOG() << "m_offset.x : " << m_offset.x  << "\n";
        // DC_LOG() << "m_offset.y : " << m_offset.y  << "\n";
        // DC_LOG() << "---\n";
        // DC_LOG() << "m_scaledOffset.x : " << m_scaledOffset.x  << "\n";
        // DC_LOG() << "m_scaledOffset.y : " << m_scaledOffset.y  << "\n";
        // DC_LOG() << "---\n";


        if (bottom<top)
            std::swap(bottom,top);

        if (right<left)
            std::swap(right,left);

        // Нам нужен цикл по той координате, по которой у нас градиент

        // при вертикальном градиенте рисуем горизонтальные линии одного цвета
        // градиент идёт сверху вниз

        // при горизонтальном градиенте рисуем вертикальные линии одного цвета
        // градиент идёт слева направо

        //------------------------------

        int gradientR                 = floatToInt(gradientType== marty_draw_context::GradientType::vertical ? scaledR.y : scaledR.x);
        //int rLineLen                  = floatToInt(gradientType==GradientType::vertical ? scaledR.y : scaledR.x);
        DrawCoord::value_type scaleXY = gradientType== marty_draw_context::GradientType::vertical ? m_scale.y/m_scale.x : m_scale.x/m_scale.y; // Возможно, что перепутано

        std::vector<int> roundingLinesLen; roundingLinesLen.reserve((std::size_t)gradientR);


        static const double pi   = 3.1415926535897932;
        static const double pi_2 = pi / 2;
        //double angleDelta = pi_2 / floatToDouble(gradientR+1);

        // DC_LOG() << "Angle Delta: " << math_helpers::angleToDegrees(angleDelta) << ", scaleXY: " << scaleXY<< "\n";
        //------------------------------
        // Делаем закругление !!! Нужен синус
        for(int i=0; i<gradientR; ++i)
        {
            auto cosA     = floatToDouble(i)/floatToDouble(gradientR);
            auto sqrCosA  = cosA*cosA;
            auto sqrSinA  = 1-sqrCosA;
            auto sinA     = std::sqrt(sqrSinA);
            double sinAR  = sinA*floatToDouble(gradientR);
            double scaledSinAR = floatToDouble(sinAR*scaleXY);

            roundingLinesLen.emplace_back(floatToInt(scaledSinAR));

        }

        //------------------------------

        int startPos = 0, endPos = 0;
        int lineLen  = 0;
        //, DrawCoord::value_type       fillBreakPos
        DrawCoord::value_type       scaledFillBreakPos = 0;

    //DrawScale                    m_scale          = { (DrawScale::value_type)1, (DrawScale::value_type)1 };

        if (gradientType== marty_draw_context::GradientType::vertical)
        {
            // при вертикальном градиенте рисуем горизонтальные линии одного цвета
            // градиент идёт сверху вниз
            // для цикла по градиенту
            startPos = top;
            endPos   = bottom;

            lineLen  = right-left+1;

            fillBreakPos = fillBreakPos*wh.y;
            scaledFillBreakPos = getScaledSize(DrawCoord{fillBreakPos,fillBreakPos}).y;

        }
        else
        {
            // при горизонтальном градиенте рисуем вертикальные линии одного цвета
            // градиент идёт слева направо
            // для цикла по градиенту
            startPos = left;
            endPos   = right;

            lineLen  = bottom-top+1;

            fillBreakPos = fillBreakPos*wh.x;
            scaledFillBreakPos = getScaledSize(DrawCoord{fillBreakPos,fillBreakPos}).x;

        }


        //------------------------------
        std::size_t numLines = (std::size_t)(endPos-startPos+1);

        std::vector<int> lineStartPositions; lineStartPositions.reserve(numLines);
        std::vector<int> lineLens          ; lineLens.reserve(numLines);

        //------------------------------
        //std::size_t midLineLen = lineLen;
        std::size_t totalLen   = (std::size_t)(endPos-startPos +1 );
        //std::size_t gradientR2 = 2*(std::size_t)gradientR;
        std::size_t midSize    = totalLen; // (totalLen>=gradientR2) ? (std::size_t)(totalLen-gradientR2) : 0;

        if (!squareBegin)
        {
            if (midSize >= (std::size_t)gradientR)
            {
                midSize -= (std::size_t)gradientR;
            }
            else
            {
                midSize  = 0;
            }
        }

        if (!squareEnd)
        {
            if (midSize >= (std::size_t)gradientR)
            {
                midSize -= (std::size_t)gradientR;
            }
            else
            {
                midSize  = 0;
            }
        }


        MARTY_ARG_USED(midSize);

        //------------------------------
        // Копируем закругление - начало 

        // DC_LOG()<<"\n---\nMake rounding, beginning\n";
        if (!squareBegin)
        {
            for(int i=0; i<gradientR; ++i)
            {
                auto idx = (std::size_t)(gradientR-i -1 );
             
                auto startPosTmp = gradientType== marty_draw_context::GradientType::vertical ? left : top;
                startPosTmp += gradientR - roundingLinesLen[idx];
                lineStartPositions.emplace_back(startPosTmp);
    
                auto len = lineLen - 2*(gradientR-roundingLinesLen[idx]); // 2*gradientR - roundingLinesLen[idx];
    
                lineLens.emplace_back(len);
    
            }
        }


        // Делаем середину
        // DC_LOG()<<"Make body\n";

        {
            auto startPosTmp = gradientType== marty_draw_context::GradientType::vertical ? left : top;
            lineStartPositions.insert(lineStartPositions.end(), midSize, startPosTmp);
            lineLens          .insert(lineLens          .end(), midSize, lineLen );
        }

        // Делаем закругление на конце
        // DC_LOG()<<"Make ending\n";
        if (!squareEnd)
        {
            for(int i=0; i<gradientR; ++i)
            {
                auto idx = (std::size_t)i;
    
                auto startPosTmp = gradientType== marty_draw_context::GradientType::vertical ? left : top;
                startPosTmp += gradientR - roundingLinesLen[idx];
                lineStartPositions.emplace_back(startPosTmp);
             
                auto len = lineLen - 2*(gradientR-roundingLinesLen[idx]); // 2*gradientR - roundingLinesLen[idx];
             
                lineLens.emplace_back(len);
             
            }
        }

        //------------------------------
        // Дублируем последнее на всякий случай
        // DC_LOG()<<"Make rounding, ending\n";
        lineStartPositions.emplace_back(lineStartPositions.back());
        lineLens          .emplace_back(lineLens          .back());



        std::size_t gradientNumVals = (std::size_t)(endPos-startPos);


        std::uint32_t gradientColorsBuf[gradientColorsBufSize+1];

        std::size_t stepSize = 1;

        while(gradientNumVals>=gradientColorsBufSize)
        {
            gradientNumVals /= 2;
            stepSize        *= 2;
        }

        makeGradientRgbArray( gradientColorBegin, gradientColorMid , gradientColorEnd
                            , gradientMidPoint  , gradientNumVals+1, &gradientColorsBuf[0]
                            );

        std::size_t gradientIdx = 0;
        auto shiftedScaledFillBreakPos = floatToInt(scaledFillBreakPos) + startPos;

        // DC_LOG()<<"\n---\nMake draw\n";
        // DC_LOG()<<"Draw from " << startPos << " to " << endPos << " mid point (raw break): " << shiftedScaledFillBreakPos << "\n";

        for(int curPos=startPos; curPos<=endPos; ++gradientIdx)
        {
            std::size_t stepIdx = 0;
            for(; stepIdx!=stepSize && curPos<=endPos; ++curPos, ++stepIdx)
            {
                marty_draw_context::ColorRef clr = marty_draw_context::ColorRef::fromUnsigned(gradientColorsBuf[gradientIdx]);

                // draw line here
                std::size_t idx = (std::size_t)(curPos-startPos);

                if (idx>=lineStartPositions.size() || idx>=lineLens.size())
                {
                    continue;
                }

                int lineStartTmp = lineStartPositions[idx];
                int lineLenTmp   = lineLens          [idx];

                bool startPartDrawing = curPos<shiftedScaledFillBreakPos;

                if (startPartDrawing)
                {
                    if (noFillBegin)
                        continue;
                }
                else
                {
                    if (noFillEnd)
                        continue;
                }

                if (gradientType== marty_draw_context::GradientType::vertical)
                {
                    // при вертикальном градиенте рисуем горизонтальные линии одного цвета
                    //drawLineInLogicalCoordsForFillGradientRect( left, curPos, right+1, curPos, clr );

                    auto lleft  = lineStartTmp;
                    auto lright = lineStartTmp + lineLenTmp + 1;
                    MARTY_ARG_USED(lleft);
                    MARTY_ARG_USED(lright);
                    //DC_LOG() << "Y: " << curPos << ", lleft: " << lleft << ", lright: " << lright << "\n";
                                                             // left    top     right   bottom
                    drawLineInLogicalCoordsForFillGradientRect( lleft,  curPos, lright, curPos, clr );
                }
                else
                {
                    // при горизонтальном градиенте рисуем вертикальные линии одного цвета
                    //drawLineInLogicalCoordsForFillGradientRect( curPos, top, curPos, bottom+1, clr );

                    auto ltop    = lineStartTmp;
                    auto lbottom = lineStartTmp + lineLenTmp + 1;
                    MARTY_ARG_USED(ltop);
                    MARTY_ARG_USED(lbottom);
                    //DC_LOG() << "X: " << curPos << ", ltop: " << ltop << ", lbottom: " << lbottom << "\n";
                                                             // left    top     right   bottom
                    drawLineInLogicalCoordsForFillGradientRect( curPos, ltop,   curPos, lbottom, clr );
                }
            }
        }

        return true;
    
    }

    virtual bool fillGradientRect( const DrawCoord                                 &leftTop
                                 , const DrawCoord                                 &rightBottom
                                 , const marty_draw_context::GradientParams        &gradientParams
                                 , marty_draw_context::GradientType                gradientType
                                 , bool                                            excludeFrame
                                 ) override
    {
        return fillGradientRect( leftTop, rightBottom
                               , gradientParams.colorBegin, gradientParams.colorMid, gradientParams.colorEnd, gradientParams.midPoint
                               , gradientType, excludeFrame
                               );
    }

    virtual bool fillGradientRoundRect( DrawCoord::value_type                           cornersR
                                      , const DrawCoord                                 &leftTop
                                      , const DrawCoord                                 &rightBottom
                                      , const marty_draw_context::GradientParams        &gradientParams
                                      , marty_draw_context::GradientType                gradientType
                                      , bool                                            excludeFrame
                                      , DrawCoord::value_type                           fillBreakPos
                                      , marty_draw_context::GradientRoundRectFillFlags  fillFlags // = GradientRoundRectFillFlags::roundBoth | GradientRoundRectFillFlags::fillFull
                                      ) override
    {
        return fillGradientRoundRect( cornersR, leftTop, rightBottom
                                    , gradientParams.colorBegin, gradientParams.colorMid, gradientParams.colorEnd, gradientParams.midPoint
                                    , gradientType, excludeFrame
                                    , fillBreakPos, fillFlags
                                    );
    }

    virtual bool fillGradientCircle( const DrawCoord                                 &pos
                                   , const DrawCoord::value_type                     &r
                                   , const marty_draw_context::GradientParams        &gradientParams
                                   , bool                                            excludeFrame
                                   ) override
    {
        return fillGradientCircle(pos, r, gradientParams.colorBegin, gradientParams.colorMid, gradientParams.colorEnd, gradientParams.midPoint, excludeFrame);
    }

    virtual bool fillGradientCircle( const DrawCoord                                 &pos
                                   , const DrawCoord::value_type                     &r
                                   , const marty_draw_context::ColorRef              &gradientColorBegin
                                   , const marty_draw_context::ColorRef              &gradientColorMid
                                   , const marty_draw_context::ColorRef              &gradientColorEnd
                                   , const DrawCoord::value_type                     &gradientMidPoint
                                   , bool                                            excludeFrame
                                   ) override
    {
        DrawCoord scaledR = getScaledSize(DrawCoord{r,r});
        int       intR    = floatToInt(scaledR.x);

        if (excludeFrame && intR > 0)
        {
            --intR;
        }


        std::size_t gradientNumVals = (std::size_t)intR;

        std::uint32_t gradientColorsBuf[gradientColorsBufSize+1];

        std::size_t stepSize = 1;

        while(gradientNumVals>=gradientColorsBufSize)
        {
            gradientNumVals /= 2+1;
            stepSize        *= 2;
        }

        makeGradientRgbArray( gradientColorBegin, gradientColorMid , gradientColorEnd
                            , gradientMidPoint  , gradientNumVals+1, &gradientColorsBuf[0]
                            );


        DrawCoord scaledPos = getScaledPos(pos);

        auto x = floatToInt(scaledPos.x);
        auto y = floatToInt(scaledPos.y);

        std::size_t gradientIdx = 0;
        for(int curR=intR+1; curR>=0; ++gradientIdx)
        {
            std::size_t stepIdx = 0;
            for(; stepIdx!=stepSize && curR>=0; --curR, ++stepIdx)
            {
                marty_draw_context::ColorRef clr = marty_draw_context::ColorRef::fromUnsigned(gradientColorsBuf[gradientIdx]);
                drawCircleInLogicalCoordsForFillGradientCircle(x, y, curR, clr);
            }
        }

        return true;

    }




}; // class DrawContextImplBase




} // namespace marty_draw_context

