// View.h : interface of the CBitmapView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __VIEW_H__
#define __VIEW_H__

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/time_service.h"


#include "marty_dc_impl_win32/gdi_draw_context.h"
#include "marty_dc_impl_win32/gdiplus_draw_context.h"

#include "test_drawings.h"

#include <array>


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
#define TEST_DC_DRAW_PARA
#define TEST_DC_SPIDERS
#define TEST_DC_GRADIENT_RECT
#define TEST_DC_GRADIENT_ROUNDRECT
#define TEST_DC_GRADIENT_CIRCLE
#define TEST_DC_LINEJOINSTYLE
#define TEST_DC_ARCTO
#define TEST_DC_GRADIENTRECT_WITH_RECT_FRAME




class CBitmapView : public CScrollWindowImpl<CBitmapView>
{
public:
    DECLARE_WND_CLASS_EX(NULL, 0, -1)

    //CBitmap m_bmp;
    //SIZE m_size;

    DrawCoord paraPos      = DrawCoord(50, 42);
    DrawCoord paraLimits   = DrawCoord(42, 20);

    CBitmapView()
    {
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

    BEGIN_MSG_MAP(CBitmapView)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MSG_WM_KEYDOWN(OnKeyDown)
        CHAIN_MSG_MAP(CScrollWindowImpl<CBitmapView>);
    END_MSG_MAP()

    LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RECT rect;
        GetClientRect(&rect);
        int x = 0;
        int y = 0;
        /*
        if(!m_bmp.IsNull())
        {
            x = m_size.cx + 1;
            y = m_size.cy + 1;
        }
        */
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

    void invalidateClientArea()
    {
        InvalidateRect(0, TRUE);
    }

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        if (nChar==VK_F5)
        {
            invalidateClientArea();
        }
        else if (nChar==VK_ADD)
        {
            if (paraLimits.x<100)
            {
                paraLimits.x += 0.2;
                invalidateClientArea();
            }
        }
        else if (nChar==VK_SUBTRACT)
        {
            if (paraLimits.x>4)
            {
                paraLimits.x -= 0.2;
                invalidateClientArea();
            }
        }

    }


    void DoPaint(CDCHandle dc)
    {
        using namespace underwood;
        using umba::lout;
        using namespace umba::omanip;

        #ifdef TEST_DC_USE_GDIPLUS
        GdiPlusDrawContext idc = dc;
        #else
        GdiDrawContext     idc = dc;
        #endif

        //auto scale = 1.4;
        //auto scale = 8;

        auto scale  = 5;
        auto offset = 2;
        // auto scale  = 1;
        // auto offset = 0;
        
        IDrawContext *pDc = &idc;

        pDc->setOffset(DrawScale(offset,offset));
        pDc->setScale(DrawScale(scale,scale));
        pDc->setPenScale(scale);

        auto startTick = umba::time_service::getCurTimeMs();

        DoPaintImpl(pDc);

        auto endTick = umba::time_service::getCurTimeMs();

        lout << "OnPaint times: " << (endTick-startTick) << "\n";

    }

    void DoPaintImpl( marty_draw_context::IDrawContext *pDc )
    {
        using namespace underwood;

        using umba::lout;
        using namespace umba::omanip;

        bool markersCollectMode = false;
        MARTY_ARG_USED(markersCollectMode);


        //pDc->setSmoothingMode( SmoothingMode::defMode );
        //pDc->setSmoothingMode( SmoothingMode::none );
        //pDc->setSmoothingMode( SmoothingMode::highQuality );
        pDc->setSmoothingMode( SmoothingMode::antiAlias );
        pDc->setBkMode( BkMode::transparent );

        pDc->markerSetDefSize( 0.5 );

        //pDc->setCollectMarkers(true);


        marty_draw_context::FontParamsA genericFontParamsH8 = { 8  // height
                                          , 0   // escapement
                                          , 0   // orientation
                                          , marty_draw_context::FontWeight::normal
                                          , marty_draw_context::FontStyleFlags::italic // normal
                                          };
        marty_draw_context::FontParamsA genericFontParamsH20  = genericFontParamsH8; genericFontParamsH20  .height =  20;
        marty_draw_context::FontParamsA genericFontParamsH3   = genericFontParamsH8; genericFontParamsH3   .height =  3;
        marty_draw_context::FontParamsA genericFontParamsH3_2 = genericFontParamsH8; genericFontParamsH3_2 .height =  3.2;
        marty_draw_context::FontParamsA genericFontParamsH3_5 = genericFontParamsH8; genericFontParamsH3_5 .height =  3.5;
        marty_draw_context::FontParamsA genericFontParamsH4   = genericFontParamsH8; genericFontParamsH4   .height =  4;
        marty_draw_context::FontParamsA genericFontParamsH6   = genericFontParamsH8; genericFontParamsH6   .height =  6;


        auto arialFontId     = pDc->createFont( genericFontParamsH6, "Arial"          );
        auto timesFontId     = pDc->createFont( genericFontParamsH6, "Times New Roman");
        auto courierFontId   = pDc->createFont( genericFontParamsH6, "Courier New"    );
        // https://learn.microsoft.com/ru-ru/windows/win32/intl/international-fonts-and-text-display
        // https://learn.microsoft.com/ru-ru/windows/win32/intl/using-ms-shell-dlg-and-ms-shell-dlg-2
        auto shellDlgFontId  = pDc->createFont( genericFontParamsH6, "MS Shell Dlg"   );
        auto shellDlg2FontId = pDc->createFont( genericFontParamsH6, "MS Shell Dlg 2" );
        auto lucidaFontId    = pDc->createFont( genericFontParamsH6, "Lucida Console" );
        auto fixedsysFontId  = pDc->createFont( genericFontParamsH6, "Fixedsys" );

        //auto timesSmallFontId = pDc->createFont( genericFontParamsH4, "Times New Roman");
        auto timesSmallFont3Id = pDc->createFont( genericFontParamsH3, "Times New Roman");
        auto timesSmallFont3_2Id = pDc->createFont( genericFontParamsH3_2, "Times New Roman");
        auto timesSmallFont3_5Id = pDc->createFont( genericFontParamsH3_5, "Times New Roman");
        auto timesSmallFont4Id = pDc->createFont( genericFontParamsH4, "Times New Roman");


        auto labelsFontId  = pDc->createFont( genericFontParamsH20, "Courier New"    );
        MARTY_ARG_USED(arialFontId  );
        MARTY_ARG_USED(timesFontId  );
        MARTY_ARG_USED(courierFontId);
        MARTY_ARG_USED(shellDlgFontId);
        MARTY_ARG_USED(shellDlg2FontId);
        MARTY_ARG_USED(lucidaFontId);
        MARTY_ARG_USED(fixedsysFontId);
        MARTY_ARG_USED(labelsFontId );
        MARTY_ARG_USED(timesSmallFont3Id);
        MARTY_ARG_USED(timesSmallFont3_2Id);
        MARTY_ARG_USED(timesSmallFont3_5Id);
        MARTY_ARG_USED(timesSmallFont4Id);

        auto penId         = pDc->selectNewSolidPen( PenParamsWithColor{ 0.5, LineEndcapStyle::round, LineJoinStyle::round, ColorRef{0, 168, 0} } );
        MARTY_ARG_USED(penId);

        auto redFramePenId = pDc->selectNewSolidPen( PenParamsWithColor{ 0, LineEndcapStyle::round, LineJoinStyle::round, ColorRef{220, 0, 0} } );
        MARTY_ARG_USED(redFramePenId);

        auto pixelPen = pDc->createSolidPen( 0 , LineEndcapStyle::round, LineJoinStyle::round, 0x80, 0, 0x80 ); // Purple
        MARTY_ARG_USED(pixelPen);

        auto brushId       = pDc->selectNewSolidBrush( 0, 0, 168 );
        MARTY_ARG_USED(brushId);

        auto yellowBrushId = pDc->createSolidBrush( 255, 242, 0 );
        auto pinkBrushId   = pDc->createSolidBrush( 255, 160, 128 );
        MARTY_ARG_USED(yellowBrushId);
        MARTY_ARG_USED(pinkBrushId);


        const marty_draw_context::IDrawContext::float_t normalKeySize1 = 12; // 50; // 40; // 80; // 120, 140, 30
        const marty_draw_context::DrawCoord::value_type keyFrameWidth  = normalKeySize1/30; // 1  /20


        auto lgrayPenId    = pDc->createSolidPen( keyFrameWidth /* width */ , LineEndcapStyle::round, LineJoinStyle::round, 160, 160, 160 );
        auto llgrayPenId   = pDc->createSolidPen( keyFrameWidth /* width */ , LineEndcapStyle::round, LineJoinStyle::round, 192, 192, 192 );
        auto dgrayPenId    = pDc->createSolidPen( keyFrameWidth /* width */ , LineEndcapStyle::round, LineJoinStyle::round, 128, 128, 128 );
        auto orangePenId   = pDc->createSolidPen( 0.1                       , LineEndcapStyle::round, LineJoinStyle::round, 0xFF, 0x7F, 0x50  /* D.Colors.Orange */  );
        MARTY_ARG_USED(lgrayPenId );
        MARTY_ARG_USED(llgrayPenId);
        MARTY_ARG_USED(dgrayPenId );
        MARTY_ARG_USED(orangePenId);

        MARTY_ARG_USED(llgrayPenId);


        auto cosmeticPenId  = pDc->createSolidPen( 0, LineEndcapStyle::round, LineJoinStyle::round, 0, 0, 0 );
        auto cosmeticPenId2 = pDc->createSolidPen( 0, LineEndcapStyle::round, LineJoinStyle::round, 168, 168, 168 );
        auto markersPenId   = pDc->createSolidPen( 0, LineEndcapStyle::round, LineJoinStyle::round, 0, 0, 0 );
        MARTY_ARG_USED(cosmeticPenId );
        MARTY_ARG_USED(cosmeticPenId2);
        MARTY_ARG_USED(markersPenId  );

        pDc->setDefaultCosmeticPen(cosmeticPenId);

        // pDc->beginPath();
        // test_drawRect_10_20(pDc);
        // test_drawFishTail_5_10(pDc);
        // pDc->endPath( true );

        #if defined(TEST_DC_DRAWRECT) || defined(TEST_DC_FISHTAIL) || defined(TEST_DC_ROUND_SQUARE) || defined(TEST_DC_SNAKE)
        // TEST_DC_ROUND_SQUARE

            auto fishTailPos = DrawCoord(65,0);
           
            pDc->beginPath();
           
            #if defined(TEST_DC_DRAWRECT)
            test_drawRect_10_20(pDc, fishTailPos);
            #endif
           
            #if defined(TEST_DC_FISHTAIL)
            test_drawFishTail_5_10(pDc, fishTailPos);
            #endif
           
            #if defined(TEST_DC_SNAKE)
            test_drawSnake_10_60(pDc, DrawCoord(115,0), 4 /* 0.8 */  /* 0.5 */ );
            #endif
           
            pDc->endPath( true, true );

            #if defined(TEST_DC_ROUND_SQUARE)
            test_drawRoundSquare(pDc,DrawCoord(90,10), DrawCoord(30, 20), 4);
            #endif

        #endif

        #if defined(TEST_DC_GRADIENTRECT_WITH_RECT_FRAME)
        {
            // auto scale  = 5;
            // auto offset = 2;

            #if defined(TEST_DC_GRADIENT_RECT) || defined(TEST_DC_GRADIENT_ROUNDRECT)

                auto startPos = DrawCoord(180, 0);

            #else

                auto startPos = DrawCoord(0, 0);

            #endif
            

            using namespace marty_draw_context;
            
            auto prevPen  = pDc->selectPen(pixelPen);

            GradientParams gradientParams;
            gradientParams.colorBegin = ColorRef::deserialize("Green");
            gradientParams.colorMid   = ColorRef::deserialize("Blue");
            gradientParams.colorEnd   = ColorRef::deserialize("Red");
            gradientParams.midPoint   = 0.5;

            auto dx = DrawCoord( 12,  0);
            auto dy = DrawCoord(  0, 12);

            auto rectLt = startPos; // (2*5+130*5, 2*5) = (660, 10)
            auto rectSz = DrawCoord( 10,10); // (5*5,5*5) = (25, 25)
            auto rectRb = rectLt+rectSz    ; // (685, 35)

            pDc->rect(rectLt, rectRb);

            auto sz1px = pDc->mapRawToLogicSize(DrawCoord(1,1));
            auto sz2px = pDc->mapRawToLogicSize(DrawCoord(2,2));
            auto sz3px = pDc->mapRawToLogicSize(DrawCoord(3,3));

            auto rectGrLt = rectLt+sz2px;
            auto rectGrRb = rectRb-sz2px;

            pDc->fillGradientRect(rectGrLt, rectGrRb, gradientParams, GradientType::vertical, false);


            rectLt   += dy;
            rectRb   += dy;
            pDc->fillRect(rectLt, rectRb, false);

            rectGrLt += dy;
            rectGrRb += dy;
            pDc->fillGradientRect(rectGrLt, rectGrRb, gradientParams, GradientType::vertical, false);


            rectLt   += dy;
            rectRb   += dy;
            pDc->fillRect(rectLt, rectRb, true);

            rectGrLt += dy;
            rectGrRb += dy;
            pDc->fillGradientRect(rectGrLt, rectGrRb, gradientParams, GradientType::vertical, false);



            rectLt   += dy;
            rectRb   += dy;
            pDc->roundRect(2, rectLt, rectRb);

            rectGrLt += dy;
            rectGrRb += dy;
            pDc->fillGradientRect(rectGrLt, rectGrRb, gradientParams, GradientType::vertical, false);


            rectLt   += dy;
            rectRb   += dy;
            pDc->fillRoundRect(2, rectLt, rectRb, false);

            rectGrLt += dy;
            rectGrRb += dy;
            pDc->fillGradientRect(rectGrLt, rectGrRb, gradientParams, GradientType::vertical, false);


            rectLt   += dy;
            rectRb   += dy;
            pDc->fillRoundRect(2, rectLt, rectRb, true);

            rectGrLt += dy;
            rectGrRb += dy;
            pDc->fillGradientRect(rectGrLt, rectGrRb, gradientParams, GradientType::vertical, false);



    // virtual bool roundRect( const DrawCoord::value_type &cornersR
    //                       , const DrawCoord             &leftTop
    //                       , const DrawCoord             &rightBottom
    //                       ) override

    // virtual bool fillRoundRect( const DrawCoord::value_type &cornersR
    //                       , const DrawCoord             &leftTop
    //                       , const DrawCoord             &rightBottom
    //                       , bool                         drawFrame
    //                       ) override





            pDc->selectPen(prevPen);

        }
        #endif
        #if 0
        local pixelPen  = dc.createSolidPen(D.PenParams(0, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Purple);
        dc.selectPen(pixelPen);
    
        local rectLt = D.Coords(130,0);
        local rectSz = D.Coords(5,5);
        dc.rect(rectLt, rectLt+rectSz);
    
        local sz1px = dc.mapRawToLogicSize(D.Coords(1,1));
        local sz2px = dc.mapRawToLogicSize(D.Coords(2,2));
        local sz3px = dc.mapRawToLogicSize(D.Coords(3,3));
        //dc.rect(rectLt+sz2px, rectLt+rectSz-sz2px);
    
        local gradientParams = D.GradientParams(D.Colors.Green, D.Colors.Blue, D.Colors.Red, 0.5);
        dc.fillGradientRect(rectLt+sz1px, rectLt+rectSz-sz2px, gradientParams, D.GradientType.Vertical, true);
        // bool fillGradientRect(DrawingCoords leftTop, DrawingCoords rightBottom, DrawingGradientParams gradientParams, int gradientType, bool excludeFrame) const
        #endif

        std::vector<std::uint32_t> letterColors;
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("GreenYellow").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Red").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Grey").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Blue").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Cyan").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("DarkCyan").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("MediumCyan").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Brown").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Whitesmoke").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Moccasin").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("Orchid").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("OrangeRed").toUnsigned());
        letterColors.emplace_back(marty_draw_context::ColorRef::deserialize("DarkKhaki").toUnsigned());
        //letterColors[8] = marty_draw_context::ColorRef::deserialize("").toUnsigned());

        std::vector<std::uint32_t> letterBk;
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("Brown").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("DarkCyan").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("GreenYellow").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("Red").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("Grey").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("MediumCyan").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("Blue").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("Grey").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("DarkGrey").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("SpringGreen").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("SkyBlue").toUnsigned());
        letterBk.emplace_back(marty_draw_context::ColorRef::deserialize("PaleGreen").toUnsigned());


        #ifdef TEST_DC_FONTS

        {

            auto drawSampleTextImpl = [&]( DrawCoord pos, marty_draw_context::DrawCoord::value_type dy, int fontId, const ColorRef &clr, const wchar_t *text)
            {
                using marty_draw_context::DrawTextFlags;

                pDc->textOut(pos, fontId, clr, text);

                pos.y += dy;

                DrawCoord retPos = pos;

                #if 1


                
                auto textColorSaver = marty_draw_context::TextColorSaver(pDc, clr ); // Устанавливаем дефолтный цвет текста, одновременно сохраняя текущий для последующего восстановления

                DrawTextFlags dtFlags = DrawTextFlags::defMode;
                //dtFlags |= DrawTextFlags::fitWidthDisable;
                dtFlags |= DrawTextFlags::fitGlyphDefault;
                dtFlags |= DrawTextFlags::endEllipsis;
                //dtFlags |= DrawTextFlags::kerningDisable;
                //dtFlags |= DrawTextFlags::coloringWords;
                dtFlags |= DrawTextFlags::forceSpacesColoring;

                std::size_t nCharsProcessed = 0;
                std::size_t nSymbolsDrawn   = 0;
                pDc->drawTextColored( pos, /* pos.x+ */ 60
                                    , dtFlags
                                    , text
                                    , std::wstring() // 0 // stopChars
                                    , letterColors
                                    , letterBk
                                    , fontId
                                    , 0 // pNextPosX - не интересно
                                    , 0 // pOverhang - не интересно
                                    , 0 // lastCharProcessed
                                    , &nCharsProcessed
                                    , &nSymbolsDrawn
                                    );
                #endif

                return retPos;
            };

            auto drawSampleText = [&]( DrawCoord pos, marty_draw_context::DrawCoord::value_type dy, int fontId, const ColorRef &clr, const std::wstring &textStr)
            {
                return drawSampleTextImpl(pos, dy, fontId, clr, textStr.c_str());
            };

            auto dPos = DrawCoord(18.0, 7.8);
            //auto pos  = DrawCoord(76, 14);
            auto pos  = DrawCoord(76,  1)-dPos;

            // https://comp-security.net/%D1%83%D0%B4%D0%B0%D1%80%D0%B5%D0%BD%D0%B8%D0%B5-%D0%BD%D0%B0%D0%B4-%D0%B1%D1%83%D0%BA%D0%B2%D0%BE%D0%B9-%D0%BD%D0%B0-%D0%BA%D0%BB%D0%B0%D0%B2%D0%B8%D0%B0%D1%82%D1%83%D1%80%D0%B5/
            // Тут у нас строки с символом ударения U+02CA (а надо так-то U+0301), рисуется как говно, и это надо будет исправить
            // Вставлял из Word, там символ ударения задавал через Alt+ Numpad 769 (301 Alt+X)
            // Word отображает нормально, студия тоже, но в строке какие-то левые символы появились - 1052 (0x41C) M (Cyrillic Capital Letter Em) 
            // и 1027 (0x403) - Г с акцентом (Cyrillic Capital Letter Gje)
            // Какая-то хуйня
            // Поэтому ручками вставим конкретный символ
            std::wstring strTimes = L"Times T 11 AY F, F. FA P, P. PA";
            // Вставка после 2го символа, вставляем один символ
	        // strTimes.insert(2u, 1u, (wchar_t)0x2CA); // Тут вставляется не комбайнинг акцент
            strTimes.insert(2u, 1u, (wchar_t)0x301); // Работает гуд

            // Комбайнинг символы имеют нулевую отображаемую ширину, тут работает система и ничего дополнительно делать вообще не надо для нормального отображения
            // Тут возникает одно но - если система рисует, то она умеет снижать положение акцента в зависимости высоты буквы (прописная/строчная), если под 
            // ударением строчная i - то точка заменяется на символ ударения.
            // Если рисуем по буквам сами - то рисуется i с точкой, и над точкой ещё символ ударения
            // Не очень, но жить можно, думаю, никто и не заметит.

            // https://reactos.org/wiki/RC_File_Standards

            pos = drawSampleText(pos+dPos, 3*dPos.y/4, timesFontId    , ColorRef{128,128,  0}, strTimes          );
            pos = drawSampleText(pos+dPos, 3*dPos.y/4, arialFontId    , ColorRef{128,  0,  0}, L"Arial A"       );
            pos = drawSampleText(pos+dPos, 3*dPos.y/4, courierFontId  , ColorRef{0  ,128,128}, L"Courier C"      );
            pos = drawSampleText(pos+dPos, 3*dPos.y/4, shellDlgFontId , ColorRef{128,  0,128}, L"MS Shell Dlg"   );
            pos = drawSampleText(pos+dPos, 3*dPos.y/4, shellDlg2FontId, ColorRef{0  ,  0,128}, L"MS Shell Dlg 2\n222" );
            //pos = drawSampleText(pos+dPos, 3*dPos.y/4, lucidaFontId   , ColorRef{0  ,  0,128}, L"Lucida Console" );
            //pos = drawSampleText(pos+dPos, 3*dPos.y/4, fixedsysFontId , ColorRef{0  ,  0,128}, L"Fixedsys" );
            //drawSampleText(pos+dPos*DrawCoord(2,2), pos.y, );

        }
            // pDc->textOut( DrawCoord( 76, 14), arialFontId    , ColorRef{128,  0,  0}, L"Arial A"        );
            // pDc->textOut( DrawCoord( 94, 28), timesFontId    , ColorRef{128,128,  0}, L"Times T"        );
            // pDc->textOut( DrawCoord(112, 42), courierFontId  , ColorRef{0  ,128,128}, L"Courier C"      );
            // pDc->textOut( DrawCoord(130, 56), shellDlgFontId , ColorRef{128,  0,128}, L"MS Shell Dlg"   );
            // pDc->textOut( DrawCoord(148, 70), shellDlg2FontId, ColorRef{0  ,  0,128}, L"MS Shell Dlg 2" );

        #endif



        marty_draw_context::DrawCoord normalKeySize = { normalKeySize1, normalKeySize1 };


        auto spiderBasePos  = DrawCoord(14,85);
        auto spiderCellSize = 6;
        MARTY_ARG_USED(spiderBasePos );
        MARTY_ARG_USED(spiderCellSize);

        #ifdef TEST_DC_SPIDERS

        pDc->selectPen(penId);

        markersCollectMode = pDc->setCollectMarkers(true);


        // lout << "-------------\n";
        // lout << "!!! Spider #1\n";
        test_drawSpider(pDc, spiderBasePos+DrawCoord( 30*0, 0), spiderCellSize, penId, cosmeticPenId, underwood::FlagSpider1   , true, true);
        // lout << "-------------\n";
        // lout << "!!! Spider #2\n";
        test_drawSpider(pDc, spiderBasePos+DrawCoord( 30*1, 0), spiderCellSize, penId, cosmeticPenId, underwood::FlagSpider2   , true, true);
        // lout << "-------------\n";
        // lout << "!!! Spider #3\n";
        test_drawSpider(pDc, spiderBasePos+DrawCoord( 30*2, 0), spiderCellSize, penId, cosmeticPenId, underwood::FlagSpiderBoth, true, true);

        pDc->setCollectMarkers(markersCollectMode);

        pDc->markersDraw(markersPenId);

        #endif


        #ifdef TEST_DC_LINEJOINSTYLE
        //------------------------------
        // Test LineJoinStyle triangle
        pDc->beginPath();
        pDc->moveTo(DrawCoord{114, 75}); // 125
        pDc->lineTo(DrawCoord{124, 60}); // 105
        pDc->lineTo(DrawCoord{134, 75}); // 125
        //pDc->lineTo(DrawCoord{15,120});
        pDc->closeFigure();
        pDc->endPath( true, false );

        #endif // #ifdef TEST_DC_LINEJOINSTYLE


        //------------------------------

        DrawCoord::value_type iconHeight   = 10;
        DrawCoord::value_type iconInterval = 0.25;
        DrawCoord::value_type iconBottomCurrentPos = 0;

        MARTY_ARG_USED(iconHeight);

        //------------------------------

        DrawCoord::value_type gradientRectSizeX = (DrawCoord::value_type)50;
        DrawCoord::value_type gradientRectSizeY = (DrawCoord::value_type)4;
        MARTY_ARG_USED(gradientRectSizeX);
        MARTY_ARG_USED(gradientRectSizeY);

        GradientParams gradientParams;
        {
            DrawCoord::value_type gradientMidColorPos = (DrawCoord::value_type)0.8;

            ColorRef              gradientColorBegin = ColorRef::fromUnsigned(0x10EA14); // light green
            ColorRef              gradientColorMid   = ColorRef::fromUnsigned(0x193FE8); // semi blue
            ColorRef              gradientColorEnd   = ColorRef::fromUnsigned(0xE50909); // some of red

            gradientParams = GradientParams{ gradientColorBegin, gradientColorMid, gradientColorEnd, gradientMidColorPos };
        }


        DrawCoord
        gradientSamplePos = DrawCoord{ (DrawCoord::value_type)4, iconBottomCurrentPos };
        {
            //DC_LOG()<<"\n*** TEST_DC_GRADIENT_RECT\n\n";

            // iconBottomCurrentPos = 0;

            // DrawCoord
            // gradientSamplePos = DrawCoord{ (DrawCoord::value_type)4, iconBottomCurrentPos };

            #ifdef TEST_DC_GRADIENT_RECT
            {
                // DC_LOG()<<"\n*** TEST_DC_GRADIENT_RECT (1)\ngradientSamplePos.x: " << gradientSamplePos.x << "\ngradientSamplePos.y: " << gradientSamplePos.y << "\n";

                // DrawCoord
                // gradientSamplePos = DrawCoord{ (DrawCoord::value_type)2, iconBottomCurrentPos };
    
                pDc->fillGradientRect( gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY }
                                     , gradientParams, GradientType::vertical
                                     , true // false // excludeFrame
                                     );
    
    
                gradientSamplePos.y += 4 + 2*iconInterval;
                // DC_LOG()<<"\n*** TEST_DC_GRADIENT_RECT (2)\ngradientSamplePos.x: " << gradientSamplePos.x << "\ngradientSamplePos.y: " << gradientSamplePos.y << "\n";
    
                pDc->fillGradientRect( gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY/2 }
                                     , gradientParams, GradientType::horizontal
                                     , true // false // excludeFrame
                                     );

                //gradientSamplePos += DrawCoord{0, 8}; // { (DrawCoord::value_type)2, gradientSamplePos + 8 };
                gradientSamplePos.y += 2 + 2*iconInterval;
    
            }
            #endif // #ifdef TEST_DC_GRADIENT_RECT
    
    
    
            #ifdef TEST_DC_GRADIENT_ROUNDRECT
            {

                DrawCoord::value_type gradientDrawBreakPos  = (DrawCoord::value_type)0.5;
                
                DrawCoord::value_type gradientRectR    = gradientRectSizeY/(DrawCoord::value_type)2;
                if (gradientRectR<(DrawCoord::value_type)1)
                {
                    gradientRectR = (DrawCoord::value_type)1;
                }

                // DrawCoord
                // gradientSamplePos = DrawCoord{ (DrawCoord::value_type)2, iconBottomCurrentPos + 8 };

                //DC_LOG()<<"\n*** TEST_DC_GRADIENT_ROUNDRECT (1)\ngradientSamplePos.x: " << gradientSamplePos.x << "\ngradientSamplePos.y: " << gradientSamplePos.y << "\n";


                #if 1

                pDc->fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY }
                                     , gradientParams, GradientType::vertical
                                     , true // false // excludeFrame
                                     , gradientDrawBreakPos
                                     , GradientRoundRectFillFlags::roundBoth
                                     );
                gradientSamplePos.y += gradientRectSizeY + 2*iconInterval;

                #endif

                #if 1

                pDc->fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY }
                                     , gradientParams, GradientType::vertical
                                     , true // false // excludeFrame
                                     , gradientDrawBreakPos
                                     , GradientRoundRectFillFlags::squareBegin // underwood::GradientRoundRectFillFlags::squareEnd
                                     );
                gradientSamplePos.y += gradientRectSizeY + 2*iconInterval;

                #endif

                #if 1

                pDc->fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY }
                                     , gradientParams, GradientType::vertical
                                     , true // false // excludeFrame
                                     , gradientDrawBreakPos
                                     , GradientRoundRectFillFlags::squareEnd
                                     );
                gradientSamplePos.y += gradientRectSizeY + 2*iconInterval;

                #endif

                #if 1

                //DC_LOG()<<"\n*** TEST_DC_GRADIENT_ROUNDRECT (2)\ngradientSamplePos.x: " << gradientSamplePos.x << "\ngradientSamplePos.y: " << gradientSamplePos.y << "\n";

                pDc->fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY/2 }
                                     , gradientParams, GradientType::horizontal
                                     , true // false // excludeFrame
                                     , gradientDrawBreakPos
                                     , GradientRoundRectFillFlags::roundBoth
                                     );
                gradientSamplePos.y += gradientRectSizeY/2 + 2*iconInterval;

                #endif

                #if 1

                pDc->fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY/2 }
                                     , gradientParams, GradientType::horizontal
                                     , true // false // excludeFrame
                                     , gradientDrawBreakPos
                                     , GradientRoundRectFillFlags::roundBoth
                                     | GradientRoundRectFillFlags::noFillBegin
                                     );
                gradientSamplePos.y += gradientRectSizeY/2 + 2*iconInterval;

                #endif

                #if 1

                pDc->fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + DrawCoord{ gradientRectSizeX, gradientRectSizeY/2 }
                                     , gradientParams, GradientType::horizontal
                                     , true // false // excludeFrame
                                     , gradientDrawBreakPos
                                     , GradientRoundRectFillFlags::roundBoth
                                     | GradientRoundRectFillFlags::noFillEnd
                                     );
                gradientSamplePos.y += gradientRectSizeY/2 + 2*iconInterval;

                #endif

            }
            #endif // #ifdef TEST_DC_GRADIENT_ROUNDRECT

        }

        test_drawRects(pDc, gradientSamplePos);
        gradientSamplePos.y += 8;
        gradientSamplePos.y += gradientRectSizeY/2 + 2*iconInterval;

        {
            auto pos = gradientSamplePos;

            auto orgPen = pDc->selectPen(orangePenId);

            DrawCoord::value_type rrr = 1.2;
    
            //auto pos    = DrawCoord(4,35);
            DrawCoord::value_type sizeX  = 22;
    
            //auto rrr = 1.2;
    
            pDc->        roundRect (rrr, pos                                             , pos+DrawCoord( sizeX    , (DrawCoord::value_type)9.6));
            pDc->    fillRoundRect (rrr, pos+DrawCoord(sizeX+1, (DrawCoord::value_type)0), pos+DrawCoord( sizeX*2+1, (DrawCoord::value_type)9.6), true);
            pos.y += 11;
        
            pDc->        roundRect (rrr, pos                                             , pos+DrawCoord( sizeX    , (DrawCoord::value_type)4.8));
            pDc->    fillRoundRect (rrr, pos+DrawCoord(sizeX+1, (DrawCoord::value_type)0), pos+DrawCoord( sizeX*2+1, (DrawCoord::value_type)4.8), true);
            pos.y += 6;
        
            pDc->        roundRect (rrr, pos                                             , pos+DrawCoord( sizeX    , (DrawCoord::value_type)3.6));
            pDc->    fillRoundRect (rrr, pos+DrawCoord(sizeX+1, (DrawCoord::value_type)0), pos+DrawCoord( sizeX*2+1, (DrawCoord::value_type)3.6), true);
            pos.y += 5;
        
            pDc->        roundRect (rrr, pos                                             , pos+DrawCoord( sizeX    , (DrawCoord::value_type)2.4));
            pDc->    fillRoundRect (rrr, pos+DrawCoord(sizeX+1, (DrawCoord::value_type)0), pos+DrawCoord( sizeX*2+1, (DrawCoord::value_type)2.4), true);
            pos.y += 3;
        
            pDc->        roundRect (rrr, pos                                             , pos+DrawCoord( sizeX    , (DrawCoord::value_type)1.2));
            pDc->    fillRoundRect (rrr, pos+DrawCoord(sizeX+1, (DrawCoord::value_type)0), pos+DrawCoord( sizeX*2+1, (DrawCoord::value_type)1.2), true);
            pos.y += 2;
        
            pDc->        roundRect (rrr, pos                                             , pos+DrawCoord( sizeX    , (DrawCoord::value_type)1));
            pDc->    fillRoundRect (rrr, pos+DrawCoord(sizeX+1, (DrawCoord::value_type)0), pos+DrawCoord( sizeX*2+1, (DrawCoord::value_type)1), true);
            pos.y += 2;
        
            pDc->selectPen( orgPen );

            gradientSamplePos = pos;
        
        }


        #if defined(TEST_DC_GRADIENT_CIRCLE)

        {
            DrawCoord
            gradientCircleSamplePos = DrawCoord{ (DrawCoord::value_type)4 + gradientRectSizeX + 2*gradientRectSizeY, iconBottomCurrentPos+2*gradientRectSizeY };

            auto circleR = 1.6*gradientRectSizeY;

            // DC_LOG()<<"\n*** TEST_DC_GRADIENT_CIRCLE\ncircleR: " << circleR << "\n";
        
            pDc->fillGradientCircle(gradientCircleSamplePos, circleR, gradientParams, true);
        
        }
            

        #endif


        pDc->selectPen(penId);

        #if defined(TEST_DC_ROUND_SQUARE)
        //DrawCoord arcCenter = spiderBasePos + DrawCoord{ 172, -38 };
        DrawCoord arcCenter = fishTailPos + DrawCoord{ 15, 35 };
        #else
        DrawCoord arcCenter = DrawCoord{ 50, 50 };
        #endif



        #ifdef TEST_DC_ARCTO

        if (pDc->isPathStarted())
        {
            pDc->endPath(true,false);
        }

        markersCollectMode = pDc->setCollectMarkers(true);

        auto arcToByAngle = [&]( const DrawCoord startPointOffset, const DrawCoord::value_type angleGradus )
            {
                DrawCoord resEndPos;
                pDc->moveTo(arcCenter + startPointOffset);
                pDc->arcTo(arcCenter, angleGradus, &resEndPos );
                pDc->markerAdd(arcCenter);
                pDc->markerAdd(resEndPos);
                pDc->markerAdd(arcCenter + startPointOffset);
            };

        //lout << "-------------\n";
        //lout << "!!! Arc dAngle #1\n";
        arcToByAngle(DrawCoord{  2,  2 }, 40);
        //lout << "-------------\n";
        //lout << "!!! Arc dAngle #2\n";
        arcToByAngle(DrawCoord{  3,  3 }, 80);


        auto arcToByEnd = [&]( const DrawCoord startPointOffset, const DrawCoord endPointOffset, bool dir )
            {
                //lout << "CCW: " << dir << " \n";
                pDc->moveTo(arcCenter + startPointOffset);
                pDc->arcTo(arcCenter, arcCenter + endPointOffset, dir );
                pDc->markerAdd(arcCenter);
                pDc->markerAdd(arcCenter + startPointOffset);
                pDc->markerAdd(arcCenter + endPointOffset);
            };

        //lout << "-------------\n";
        //lout << "!!! Arc Vector #1\n";  // Первая ниже центра, вторая - выше, против часовой - малая дуга
        arcToByEnd(DrawCoord{  5,  5 }, DrawCoord{  5, -5 }, true ); // direction - CounterClockWise
        //lout << "-------------\n";
        //lout << "!!! Arc Vector #2\n";  // Первая ниже центра, вторая - выше, по часовой - большая дуга
        arcToByEnd(DrawCoord{  6,  6 }, DrawCoord{  6, -6 }, false); // direction - ClockWise

        // lout << "-------------\n";
        // lout << "!!! Arc Vector #3\n"; // Первая выше центра, вторая - ниже, против часовой - большая дуга
        arcToByEnd(DrawCoord{  7, -7 }, DrawCoord{  7, 7 }, true ); // direction - CounterClockWise
        // lout << "-------------\n";
        // lout << "!!! Arc Vector #4\n"; // Первая выше центра, вторая - ниже, по часовой - малая дуга
        arcToByEnd(DrawCoord{  8, -8 }, DrawCoord{  8, 8 }, false); // direction - ClockWise


        pDc->setCollectMarkers(markersCollectMode);

        pDc->markersDraw(markersPenId);


        if (pDc->isPathStarted())
        {
            pDc->endPath(true,false);
        }

        #endif // #ifdef TEST_DC_ARCTO



        #ifdef TEST_DC_DRAW_PARA
        {
            // https://ru.wikipedia.org/wiki/Lorem_ipsum

            std::wstring loremIpsumLong = L"Sed ut perspiciatis, unde omnis iste natus error sit voluptatem accusantium "
                                    L"doloremque laudantium, totam rem aperiam eaque ipsa, quae ab illo inventore veritatis "
                                    L"et quasi architecto beatae vitae dicta sunt, explicabo. Nemo enim ipsam voluptatem, "
                                    L"quia voluptas sit, aspernatur aut odit aut fugit, sed quia consequuntur magni dolores "
                                    L"eos, qui ratione voluptatem sequi nesciunt, neque porro quisquam est, qui dolorem ipsum, "
                                    L"quia dolor sit, amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora "
                                    L"incidunt, ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, "
                                    L"quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea "
                                    L"commodi consequatur? Quis autem vel eum iure reprehenderit, qui in ea voluptate velit "
                                    L"esse, quam nihil molestiae consequatur, vel illum, qui dolorem eum fugiat, quo voluptas "
                                    L"nulla pariatur? At vero eos et accusamus et iusto odio dignissimos ducimus, qui blanditiis "
                                    L"praesentium voluptatum deleniti atque corrupti, quos dolores et quas molestias excepturi "
                                    L"sint, obcaecati cupiditate non provident, similique sunt in culpa, qui officia deserunt "
                                    L"mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et "
                                    L"expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio, cumque nihil "
                                    L"impedit, quo minus id, quod maxime placeat, facere possimus, omnis voluptas assumenda est, "
                                    L"omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum "
                                    L"necessitatibus saepe eveniet, ut et voluptates repudiandae sint et molestiae non recusandae. "
                                    L"Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores "
                                    L"alias consequatur aut perferendis doloribus asperiores repellat.";

            std::wstring loremIpsumShort = L"Lorem\tipsum\tdolor sit amet, consectetur_adipiscing_elit,_sed_do eiusmod tempor "
                                    L"incididunt ut Labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud "
                                    L"exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure "
                                    L"dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
                                    L"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt "
                                    L"mollit anim id est laborum.";

            std::wstring loremIpsumTiny = L"Lo—r   em\tip–s\tum\tdolor — sit – amet, consectetur adipiscing elit, sed do eiusmod tempor";

            #include "hogwarts.h"

            //auto pos                       = DrawCoord(50, 42);
            //auto paraLimits                = DrawCoord(70, 200);
            //auto paraLimits                = DrawCoord(42, 20);
            auto deltaPos                  = DrawCoord(paraLimits.x+1, (DrawCoord::value_type)13);
            //DrawCoord::value_type nextPosY = 0;
            //bool                  verticalDone = false;
            //DrawCoord::value_type tabStopPositions[] = {30,50,60,110};
            std::vector<DrawCoord::value_type> tabStopPositions = {22,35,45,110};
            std::vector<DrawCoord::value_type> tabStopPositionsEmpty;

            DrawCoord::value_type frameBoxHeight = 30;

            // redFramePenId
            // pixelPen

            //DrawTextFlags dtfExtra = 0;
            DrawTextFlags dtFlags = DrawTextFlags::coloringWords;
            //DrawTextFlags dtfExtra = DrawTextFlags::coloringParas;

            dtFlags |= DrawTextFlags::fitHeightDisable;

            dtFlags |= DrawTextFlags::forceSpacesColoring;

            test_drawParaColored( pDc
                                , paraPos
                                , paraLimits
                                , tabStopPositions
                                , letterColors
                                , letterBk
                                , dtFlags
                                , HorAlign::left
                                , frameBoxHeight
                                , pixelPen
                                , timesSmallFont3_2Id
                                , L"Left " + loremIpsumShort // loremIpsumTiny
                                );

            test_drawParaColored( pDc
                                , paraPos + deltaPos
                                , paraLimits
                                , tabStopPositionsEmpty
                                , letterColors
                                , letterBk
                                , dtFlags
                                , HorAlign::center
                                , frameBoxHeight
                                , pixelPen
                                , timesSmallFont3_2Id
                                , L"Center " + loremIpsumShort // loremIpsumTiny
                                );

            test_drawParaColored( pDc
                                , paraPos + deltaPos + deltaPos
                                , paraLimits
                                , tabStopPositionsEmpty
                                , letterColors
                                , letterBk
                                , dtFlags
                                , HorAlign::right
                                , frameBoxHeight
                                , pixelPen
                                , timesSmallFont3_2Id
                                , L"Right " + loremIpsumShort // loremIpsumTiny
                                );

            test_drawParaColored( pDc
                                , paraPos + deltaPos + deltaPos + deltaPos
                                , paraLimits
                                , tabStopPositionsEmpty
                                , letterColors
                                , letterBk
                                , dtFlags
                                , HorAlign::width
                                , frameBoxHeight
                                , pixelPen
                                , timesSmallFont3_2Id
                                , L"Width " + loremIpsumShort // loremIpsumTiny
                                );

            #if 0
            test_drawTextBox(pDc, pixelPen, pos, paraLimits, 60);

            pDc->drawParaColoredEx( pos, paraLimits, &nextPosY, &verticalDone
                                  , (DrawCoord::value_type)0.2   // lineSpacing
                                  , (DrawCoord::value_type)3.5   // paraIndent
                                  , (DrawCoord::value_type)10.0  // tabSize
                                  , DrawTextFlags::fitGlyphDefault | DrawTextFlags::fitHeightDisable
                                  , HorAlign::width // тестируем выравнивание по ширине
                                  , VertAlign::top
                                  //, loremIpsumTiny.c_str(), loremIpsumTiny.size()
                                  , loremIpsumShort.c_str(), loremIpsumShort.size() // (std::size_t)-1
                                  , 0 // pCharsProcessed
                                  , &letterColors[0], sizeof(letterColors)/sizeof(letterColors[0])
                                  , 0, 0 // &tabStopPositions[0], sizeof(tabStopPositions)/sizeof(tabStopPositions[0])
                                  , timesSmallFont4Id
                                  );
            #endif
        }
        #endif




    }

};

#endif // __VIEW_H__
