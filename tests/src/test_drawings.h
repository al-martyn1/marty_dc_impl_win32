#pragma once

// #include "keyboard_geometry/keyboard_drawing.h"

#include "marty_draw_context/emf.h"

#include <pugixml/pugixml.hpp>


namespace underwood {


inline
void test_drawSquareMarker(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord pos
                          , int penId
                          )
{
    drawSquareMarker(pDc, pos, penId);
    // pDc->selectPen( penId );
    // auto sz = DrawCoord(1,1);
    // pDc->rect( pos-sz, pos+sz );
}

inline
void  test_drawRects(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord offs)
{
    using marty_draw_context::DrawCoord;
    pDc->roundRect    (1, offs+DrawCoord( 0,0), offs+DrawCoord( 8, 8));
    pDc->fillRoundRect(1, offs+DrawCoord(10,0), offs+DrawCoord(18, 8), false);
    pDc->fillRoundRect(1, offs+DrawCoord(20,0), offs+DrawCoord(28, 8), true );

    pDc->rect         (   offs+DrawCoord(30,0), offs+DrawCoord(38, 8));
    pDc->fillRect     (   offs+DrawCoord(40,0), offs+DrawCoord(48, 8), false);
    pDc->fillRect     (   offs+DrawCoord(50,0), offs+DrawCoord(58, 8), true );
}


inline
void test_drawSpider_calcPoints( marty_draw_context::DrawCoord pos, marty_draw_context::DrawCoord::value_type sz
                               , marty_draw_context::DrawCoord &leftCenter
                               , marty_draw_context::DrawCoord &rightCenter
                               , marty_draw_context::DrawCoord &topCenter
                               , marty_draw_context::DrawCoord &bottomCenter
                               , marty_draw_context::DrawCoord &leftLeftTop
                               , marty_draw_context::DrawCoord &leftRightBottom
                               , marty_draw_context::DrawCoord &rightLeftTop
                               , marty_draw_context::DrawCoord &rightRightBottom
                               , marty_draw_context::DrawCoord &topLeftTop
                               , marty_draw_context::DrawCoord &topRightBottom
                               , marty_draw_context::DrawCoord &bottomLeftTop
                               , marty_draw_context::DrawCoord &bottomRightBottom
                               )
{
    const marty_draw_context::DrawCoord::value_type z = 0;
    marty_draw_context::DrawCoord szCoord = marty_draw_context::DrawCoord(sz,sz);

    leftCenter        = pos - marty_draw_context::DrawCoord(sz,z);
    leftLeftTop       = leftCenter - szCoord;
    leftRightBottom   = leftCenter + szCoord;

    rightCenter       = pos + marty_draw_context::DrawCoord(sz,z);
    rightLeftTop      = rightCenter - szCoord;
    rightRightBottom  = rightCenter + szCoord;

    topCenter         = pos - marty_draw_context::DrawCoord(z,sz);
    topLeftTop        = topCenter - szCoord;
    topRightBottom    = topCenter + szCoord;

    bottomCenter      = pos + marty_draw_context::DrawCoord(z,sz);
    bottomLeftTop     = bottomCenter - szCoord;
    bottomRightBottom = bottomCenter + szCoord;

}

inline
void test_drawSpider_drawGrid(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord pos, marty_draw_context::DrawCoord::value_type sz, int penId )
{
    marty_draw_context::DrawCoord leftTop        = pos - marty_draw_context::DrawCoord(2*sz,2*sz);
    //DrawCoord rightBottom  = pos + DrawCoord(2*sz,2*sz);

    marty_draw_context::DrawCoord::value_type szFull = 4*sz;

    const auto nLines = 5u;

    pDc->selectPen( penId );

    auto x = leftTop.x;
    auto y = leftTop.y;

    for(auto i=0u; i!=nLines; ++i)
    {
        pDc->moveTo(marty_draw_context::DrawCoord(x,y));
        pDc->lineTo(marty_draw_context::DrawCoord(x+szFull,y));
        y += sz;
    }

    y = leftTop.y;
    for(auto i=0u; i!=nLines; ++i)
    {
        pDc->moveTo(marty_draw_context::DrawCoord(x,y));
        pDc->lineTo(marty_draw_context::DrawCoord(x,y+szFull));
        x += sz;
    }

}


inline
auto test_drawSpider_drawArcHelper(marty_draw_context::IDrawContext *pDc, int penId, int schPenId, marty_draw_context::DrawCoord::value_type sz, marty_draw_context::DrawCoord centerPos, marty_draw_context::DrawCoord startMul, marty_draw_context::DrawCoord endMul, bool ccw, bool drawPoints )
{
    marty_draw_context::DrawCoord szCoord = marty_draw_context::DrawCoord(sz,sz);

    marty_draw_context::DrawCoord leftTop       = centerPos - szCoord;
    marty_draw_context::DrawCoord rightBottom   = centerPos + szCoord;

    marty_draw_context::DrawCoord startPos      = centerPos + szCoord*startMul;
    marty_draw_context::DrawCoord endPos        = centerPos + szCoord*endMul;

    pDc->selectPen( penId );
    pDc->moveTo(startPos);
    pDc->ellipticArcTo( leftTop, rightBottom, startPos, endPos, ccw );

    if (drawPoints)
    {
        pDc->selectPen( schPenId );
        pDc->markerAdd(startPos);
        pDc->markerAdd(endPos);
    }

}

/* Trigonometric quarters order

   II     I

   III   IV

 */


/* Trigonometric quarters order

   CCW   CW

   CW    CCW

 */

const unsigned FlagSpider1 = 1;
const unsigned FlagSpider2 = 2;
const unsigned FlagSpiderBoth = 3;

inline
void test_drawSpider(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord pos, marty_draw_context::DrawCoord::value_type sz
                    , int penId, int schPenId
                    , unsigned spiderFlags
                    , bool drawGrid = false
                    , bool drawPoints = false
                    )
{
    marty_draw_context::DrawCoord leftCenter , rightCenter    , topCenter    , bottomCenter     ;
    marty_draw_context::DrawCoord leftLeftTop, leftRightBottom, rightLeftTop , rightRightBottom ;
    marty_draw_context::DrawCoord topLeftTop , topRightBottom , bottomLeftTop, bottomRightBottom;

    test_drawSpider_calcPoints( pos, sz
                              , leftCenter , rightCenter    , topCenter    , bottomCenter
                              , leftLeftTop, leftRightBottom, rightLeftTop , rightRightBottom
                              , topLeftTop , topRightBottom , bottomLeftTop, bottomRightBottom
                              );


    if (drawGrid)
        test_drawSpider_drawGrid( pDc, pos, sz, schPenId );

    auto drawArcHelper = [&](marty_draw_context::DrawCoord centerPos, marty_draw_context::DrawCoord startMul, marty_draw_context::DrawCoord endMul, bool ccw, bool drawPoints)
    {
        test_drawSpider_drawArcHelper(pDc, penId, schPenId, sz, centerPos, startMul, endMul, ccw, drawPoints );
    };

    //drawArcHelper(rightCenter, DrawCoord(-1,0), DrawCoord(0,-1), false, true );
    //bool drawPoints = true;

    bool ccw = true;
    bool cw  = false;

    if (spiderFlags&FlagSpider1)
    {
        drawArcHelper(rightCenter, marty_draw_context::DrawCoord(-1,0), marty_draw_context::DrawCoord(0,-1), cw , drawPoints ); //
        drawArcHelper(leftCenter , marty_draw_context::DrawCoord( 1,0), marty_draw_context::DrawCoord(0,-1), ccw, drawPoints ); //
        drawArcHelper(leftCenter , marty_draw_context::DrawCoord( 1,0), marty_draw_context::DrawCoord(0, 1), cw , drawPoints ); //
        drawArcHelper(rightCenter, marty_draw_context::DrawCoord(-1,0), marty_draw_context::DrawCoord(0, 1), ccw, drawPoints ); //
    }

    if (spiderFlags&FlagSpider2)
    {
        drawArcHelper(topCenter   , marty_draw_context::DrawCoord(0, 1), marty_draw_context::DrawCoord( 1,0), ccw, drawPoints ); //
        drawArcHelper(topCenter   , marty_draw_context::DrawCoord(0, 1), marty_draw_context::DrawCoord(-1,0), cw , drawPoints ); //
        drawArcHelper(bottomCenter, marty_draw_context::DrawCoord(0,-1), marty_draw_context::DrawCoord(-1,0), ccw, drawPoints ); //
        drawArcHelper(bottomCenter, marty_draw_context::DrawCoord(0,-1), marty_draw_context::DrawCoord( 1,0), cw , drawPoints ); //
    }
}




inline
void test_drawRect_10_20(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord offs )
{
    pDc->moveTo(offs+ marty_draw_context::DrawCoord(10,10));
    pDc->lineTo(offs+ marty_draw_context::DrawCoord(20,10));
    pDc->lineTo(offs+ marty_draw_context::DrawCoord(20,20));
    pDc->lineTo(offs+ marty_draw_context::DrawCoord(10,20));
    pDc->lineTo(offs+ marty_draw_context::DrawCoord(10,10));
    pDc->closeFigure();
}

inline
void test_drawFishTail_5_10(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord offs )
{
    pDc->moveTo(offs+ marty_draw_context::DrawCoord( 5,10));
    pDc->lineTo(offs+ marty_draw_context::DrawCoord(10,10));
    pDc->lineTo(offs+ marty_draw_context::DrawCoord(10, 5));
    pDc->closeFigure();
}

inline
void test_drawRoundSquare(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord offs, marty_draw_context::DrawCoord sz, marty_draw_context::DrawCoord::value_type cornersR = 1 )
{
    pDc->roundRect( cornersR
                  , offs//+DrawCoord(10,10) // leftTop
                  , offs+sz//+DrawCoord(20,20) // rightBottom
                  );
}

inline
void test_drawSnake_10_60(marty_draw_context::IDrawContext *pDc, marty_draw_context::DrawCoord offs, marty_draw_context::DrawCoord::value_type cornersR = 1 )
{
    std::vector<marty_draw_context::DrawCoord> snakePoints = { { 30, 60 }
                              , { 40, 60 }
                              , { 50, 60 }
                              , { 50, 50 }
                              , { 60, 50 }
                              , { 60, 40 }
                              , { 60, 30 }
                              , { 60, 20 }
                              , { 50, 20 }
                              , { 50, 10 }
                              , { 40, 10 }
                              , { 30, 10 }
                              , { 20, 10 }
                              , { 10, 10 }
                              , { 10, 20 }
                              , { 10, 30 }
                              , { 10, 40 }
                              , { 20, 40 }
                              , { 30, 40 }
                              , { 30, 30 }
                              , { 20, 30 }
                              , { 20, 20 }
                              , { 30, 20 }
                              , { 40, 20 }
                              , { 40, 30 }
                              , { 50, 30 }
                              , { 50, 40 }
                              , { 40, 40 }
                              , { 40, 50 }
                              , { 30, 50 }
                              //, { ,  }
                              };

    for(auto &p : snakePoints )
        p += offs;

    pDc->roundRectFigure( cornersR
                        , snakePoints.size()
                        , &snakePoints[0]
                        );

}


inline
bool isWordLetter(wchar_t ch)
{
    return (ch>=L'a' && ch<=L'z')
        || (ch>=L'A' && ch<=L'Z');
        ;
}

inline
std::wstring simple_text_compress( const std::wstring &str, std::vector<std::wstring> &words
                                 , std::map<char, std::size_t> &charCounts
                                 , std::size_t &nWords, std::size_t &nChars, std::size_t &nPackedChars
                                 )
{
    std::unordered_map<std::wstring, std::size_t>    wordMap;

    auto getWordIndexFromDict = [&](std::size_t startPos, std::size_t curPos) -> std::size_t
    {
        if (startPos!=curPos)
        {
            auto word = std::wstring(str, startPos, curPos - startPos);
            std::unordered_map<std::wstring, std::size_t>::const_iterator wit = wordMap.find(word);
            if (wit==wordMap.end())
            {
                auto res = words.size();
                words.emplace_back(word);
                wordMap[word] = res;
                return res;
            }
            else
            {
                return wit->second;
            }
        }

        throw std::runtime_error("Something goes wrong");
    };

    nWords  = 0;
    nChars  = 0;
    nPackedChars = 0;
    std::size_t startPos = 0;
    std::size_t curPos   = 0;
    std::wstring compressedText;
    //bool collect

    auto addWordCompressed = [&]()
    {
        if (startPos!=curPos)
        {
            // что-то было накоплено
            std::size_t idx = getWordIndexFromDict(startPos, curPos);
            
            // Добавляем индекс слова в словаре, флаг 0x8000u указывает, что данный элемент - слово из словаря, а не отдельный символ
            compressedText.append(1, (wchar_t)(0x0800u | idx));

            ++nWords;

            startPos = curPos;
        }
    };

    auto addCharsCompressed = [&]()
    {
        // Пары: ", ", ". ", "? ", "! " - итого - 4
        // Одиночные символы: " " (пробел), ",", ".", "?", "!", "(", ")", "[", "]", "{", "}" 

        // Собрали статистику
        // 37615 байт текста
        // ' ' (32) - 6133
        // ',' (44) - 395
        // '.' (46) - 295
        // Если код программы, то наверное ещё будут регулярно встречаться символы: ":", ";"
        // Остальные символы на уровне погрешности

        // у нас 12 бит на символ, 
        // 8 бит - код символ, осталось 4 бита
        // 1 бит - признак того, что у нас слово, а не отдельный символ
        // Итого у нас есть три бита, чтобы закодировать наиболее частотные символы
        // 0b000 - признак того, что следом идёт байт с символом
        // 0b001 - пробел, второго байта нет
        // 0b010 - ',', второго байта нет
        // 0b011 - '.', второго байта нет
        // 0b100 - ":", второго байта нет
        // 0b101 - ";", второго байта нет
        // 0b110 - резерв
        // 0b111 - резерв

        for(auto i=startPos; i!=curPos; ++i)
        {
            auto wch = str[i];
            // compressedText.append(1, wch); // старая логика посимвольного добавления обычных карактеров
            char ch = (char)wch;
            charCounts[ch]++;
            ++nPackedChars; // считаем заранее, что упакованый символ
            switch(wch)
            {
                case L' ': compressedText.append(1, (wchar_t)(0x0100u | wch)); break;
                case L',': compressedText.append(1, (wchar_t)(0x0200u | wch)); break;
                case L'.': compressedText.append(1, (wchar_t)(0x0300u | wch)); break;
                case L':': compressedText.append(1, (wchar_t)(0x0400u | wch)); break;
                case L';': compressedText.append(1, (wchar_t)(0x0500u | wch)); break;
                default:
                    ++nPackedChars; // Откатили
                    ++nChars;
                    compressedText.append(1, wch);
            }
        }

        startPos = curPos;

    };


    bool collectWord = false;

    while(curPos!=str.size())
    {
        auto wch = str[curPos];

        if (collectWord)
        {
            if (isWordLetter(wch))
            {
                // Ничего не делаем
            }
            else // попался небуквенный символ 
            {
                addWordCompressed(); // добавляем слово, если было что добавить
                collectWord = false;
            }
        }
        else // collect chars mode
        {
            if (isWordLetter(wch))
            {
                addCharsCompressed(); // добавляем буквы, если было что добавить
                collectWord = true;
            }
            else // попался небуквенный символ 
            {
                // Ничего не делаем
            }
        }

        ++curPos; // накапливаем

    }

    if (startPos!=curPos)
    {
        if (collectWord)
        {
            addWordCompressed();
        }
        else
        {
            addCharsCompressed();
        }
    }

    return compressedText;

}

// Считаем размер wchar_t как 1 байт, потом что на самом деле идея для однобайтных кодировок
inline
std::size_t calcWordsBytes(const std::vector<std::wstring> &words, marty::Decimal &avgWordLen)
{
    std::size_t res = 0;

    std::size_t sumLen = 0;

    for( const auto & w : words)
    {
        res    += w.size() + 1;
        sumLen += w.size();
    }

    if (words.size())
    {
        avgWordLen = (marty::Decimal)sumLen / (marty::Decimal)words.size();
    }
    else
    {
        avgWordLen = 0;
    }

    return res;
}


inline
void test_drawTextBox(marty_draw_context::IDrawContext *pDc, int penId, const marty_draw_context::DrawCoord &pos, const marty_draw_context::DrawCoord &lim, marty_draw_context::DrawCoord::value_type heigh)
{
    auto prevPen  = pDc->selectPen(penId);

    using marty_draw_context::DrawCoord;

    auto startPos = pos+DrawCoord((DrawCoord::value_type)0,heigh);
    auto leftPos  = pos;
    auto rightPos = DrawCoord(pos.x+lim.x,pos.y);
    auto endPos   = DrawCoord(rightPos.x ,pos.y+heigh);

    pDc->beginPath();
    pDc->moveTo(startPos);
    pDc->lineTo(leftPos);
    pDc->lineTo(rightPos);
    pDc->lineTo(endPos);
    pDc->endPath( true, false ); // stroke, no fill

    pDc->selectPen(prevPen);
}

inline 
void test_drawParaColored( marty_draw_context::IDrawContext *pDc
                         , const marty_draw_context::DrawCoord &pos
                         , const marty_draw_context::DrawCoord &lim
                         , const std::vector<marty_draw_context::DrawCoord::value_type> &tabStopPositions
                         , const std::vector<std::uint32_t> &letterColors
                         , const std::vector<std::uint32_t> &letterBkColors
                         , marty_draw_context::DrawTextFlags flags
                         , marty_draw_context::HorAlign horAlign
                         , const marty_draw_context::DrawCoord::value_type &frameBoxHeight
                         , int pixelPenId
                         , int fontId
                         , const std::wstring &text
                         )
{
    using marty_draw_context::DrawCoord;
    using marty_draw_context::DrawTextFlags;
    using marty_draw_context::VertAlign;

    DrawCoord::value_type nextPosY = 0;
    bool                  verticalDone = false;

    test_drawTextBox(pDc, pixelPenId, pos, lim, frameBoxHeight);

    pDc->drawParaColored( pos, lim
                        , (DrawCoord::value_type)0.2   // lineSpacing
                        , (DrawCoord::value_type)3.5   // paraIndent
                        , (DrawCoord::value_type)10.0  // tabSize
                        , flags | DrawTextFlags::fitGlyphDefault // | DrawTextFlags::fitHeightDisable
                        , horAlign // HorAlign::width // тестируем выравнивание по ширине
                        , VertAlign::top
                        , text
                        , letterColors
                        , letterBkColors
                        , tabStopPositions
                        , fontId
                        , &nextPosY, &verticalDone
                        , 0 // pCharsProcessed
                        );

}

inline
bool hasLineBreaks(marty_draw_context::IDrawContext *pDc, const std::wstring &str)
{
    const wchar_t *text     = str.data();
    std::size_t    textSize = str.size();

    std::size_t curCharLen  = pDc->getCharLen(str.data(), textSize);
    for( ; textSize && curCharLen!=0; curCharLen = pDc->getCharLen(text, textSize) )
    {
        //ATLASSERT(curCharLen<=textSize);
        if (curCharLen>textSize)
        {
            break;
        }

        std::uint32_t ch32 = pDc->getChar32(text, textSize);

        if (pDc->isAnyLineBreakChar(ch32))
        {
            return true;
        }
        else
        {
            //curPara.append(text, curCharLen);
            text     += curCharLen;
            textSize -= curCharLen;
        }
    }

    return false;
}

inline
std::wstring splitSentencesToParas(const std::wstring &str)
{
    std::wstring resStr; resStr.reserve(str.size());
    bool prevDot = false;

    for(const auto &ch : str)
    {
        if (prevDot && ch==(wchar_t)' ')
        {
            resStr.append(1, (wchar_t)'\n');
            prevDot = false;
        }
        else if (ch==(wchar_t)'.')
        {
            resStr.append(1, ch);
            prevDot = true;
        }
        else
        {
            resStr.append(1, ch);
            prevDot = false;
        }
    }

    return resStr;
}


inline 
void test_drawMultiParasColored( marty_draw_context::IDrawContext *pDc
                         , const marty_draw_context::DrawCoord &pos
                         , const marty_draw_context::DrawCoord &lim
                         , const std::vector<marty_draw_context::DrawCoord::value_type> &tabStopPositions
                         , const std::vector<std::uint32_t> &letterColors
                         , const std::vector<std::uint32_t> &letterBkColors
                         , const std::vector<std::uint32_t> &paraColors
                         , const std::vector<std::uint32_t> &paraBkColors
                         , marty_draw_context::DrawTextFlags flags
                         , marty_draw_context::HorAlign horAlign
                         , const marty_draw_context::DrawCoord::value_type &frameBoxHeight
                         , int pixelPenId
                         , int fontId
                         , std::wstring text
                         )
{
    using marty_draw_context::DrawCoord;
    using marty_draw_context::DrawTextFlags;
    using marty_draw_context::VertAlign;

    DrawCoord::value_type nextPosY = 0;
    bool                  verticalDone = false;

    test_drawTextBox(pDc, pixelPenId, pos, lim, frameBoxHeight);

    if (!hasLineBreaks(pDc, text))
    {
        text = splitSentencesToParas(text);
    }
    

    pDc->drawMultiParasColored( pos, lim
                              , (DrawCoord::value_type)0.2   // lineSpacing
                              , (DrawCoord::value_type)2 // 0.3   // paraSpacing
                              , (DrawCoord::value_type)3.5   // paraIndent
                              , (DrawCoord::value_type)10.0  // tabSize
                              , flags | DrawTextFlags::fitGlyphDefault // | DrawTextFlags::fitHeightDisable
                              , horAlign // HorAlign::width // тестируем выравнивание по ширине
                              , VertAlign::top
                              , text
                              , letterColors
                              , letterBkColors
                              , tabStopPositions
                              , paraColors
                              , paraBkColors
                              , fontId
                              , &nextPosY, &verticalDone
                              );

}


inline 
void test_drawEmf( marty_draw_context::IDrawContext *pDc
                 , const std::uint8_t *pData
                 , std::size_t          size
                 )
{
    MARTY_ARG_USED(pDc);
    MARTY_ARG_USED(pData);
    MARTY_ARG_USED(size);

    using namespace marty_draw_context;

    if (!emf::MetafileHeader::canExtractFromRawBytes(pData, size))
    {
        return;
    }

    auto metafileHeader = emf::MetafileHeader::extractFromRawBytes(pData, size);

}

inline 
void test_drawSvg( marty_draw_context::IDrawContext *pDc
                 , const std::uint8_t *pData
                 , std::size_t          size
                 )
{
    MARTY_ARG_USED(pDc);
    MARTY_ARG_USED(pData);
    MARTY_ARG_USED(size);

    using namespace marty_draw_context;

    #if 0
    	// Load document from buffer. Copies/converts the buffer, so it may be deleted or changed after the function returns.
		xml_parse_result load_buffer(const void* contents, size_t size, unsigned int options = parse_default, xml_encoding encoding = encoding_auto);

		// Load document from buffer, using the buffer for in-place parsing (the buffer is modified and used for storage of document data).
		// You should ensure that buffer data will persist throughout the document's lifetime, and free the buffer memory manually once document is destroyed.
		xml_parse_result load_buffer_inplace(void* contents, size_t size, unsigned int options = parse_default, xml_encoding encoding = encoding_auto);
    #endif

    // http://www.w3.org/2000/svg
    // xmlns:svg="http://www.w3.org/2000/svg"
    // xmlns="http://www.w3.org/2000/svg"
    pugi::xml_document svgDoc;
    pugi::xml_parse_result result = svgDoc.load_buffer((const void*)pData, size);

    if (result)
    {
        // std::cerr << "XML [" << genresConverterXmlData << "] parsed without errors, attr value: [" << genresConverterXmlDoc.child("node").attribute("attr").value() << "]\n\n";
    }
    else
    {
        //std::cerr << "XML parsed with errors, attr value: [" + svgDoc.child("node").attribute("attr").value() << "]\n";
        throw std::runtime_error(
        result.description() // << std::string("\n") + 
        //std::string("Error offset: ") + result.offset  //  + " (error at [..." << (genresConverterXmlData.c_str() + result.offset) << "]\n\n";
        );
    }


}




} // namespace underwood



