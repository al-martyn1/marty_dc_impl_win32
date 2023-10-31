// Inline methods. Not for include itself

    bool drawTextColoredExImpl( const std::unordered_set<KerningPair> &kerningPairs
                              , const SimpleFontMetrics       &fontMetrics
                              , const DrawCoord               &startPos
                              , const DrawCoord::value_type   &widthLim
                              , DrawCoord::value_type         *pNextPosX //!< OUT, Положение вывода для символа, следующего за последним выведенным
                              , DrawCoord::value_type         *pOverhang //!< OUT, Вынос элементов символа за пределы NextPosX - актуально, как минимум, для iatalic стиля шрифта
                              , DrawTextFlags                 flags
                              , const wchar_t                 *text
                              , std::size_t                   textSize=(std::size_t)-1
                              , std::uint32_t                 *pLastCharProcessed = 0 //!< IN/OUT last drawn char, for kerning calculation
                              , std::size_t                   *pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                              , const std::uint32_t           *pColors=0
                              , std::size_t                   nColors=0
                              , std::size_t                   *pSymbolsDrawn=0
                              , const wchar_t                 *stopChars=0
                              , int                           fontId=-1
                              )
    {
        //MARTY_IDC_ARG_USED(kerningPairs);

        // https://www.compart.com/en/unicode/U+2026
        static wchar_t ellipsisStr[2] = { 0x2026u, 0 };

        bool drawEllipsis = (flags&DrawTextFlags::endEllipsis)!=0;
        float_t ellipsisWidth = (float_t)0;

        if ((flags&DrawTextFlags::fitWidthDisable)!=0) // ограничение под длине запрещено
        {
            drawEllipsis = false; // если нет лимита по длине, значит и элипсис не рисуется
        }

        if (drawEllipsis)
        {
            std::uint32_t elCh32 = getChar32(&ellipsisStr[0], 1u);
            getCharWidth(elCh32, ellipsisWidth);
        }

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

        std::basic_string<std::uint32_t> wstrStopChars32 = makeStopCharsString32(flags, stopChars);

        size_t nCharsProcessed  = 0;
        size_t nSymbolsDrawn    = 0;
        size_t nColorIndex      = 0;
        std::uint32_t prevCh32  = 0;
        std::uint32_t ch32      = 0;
        if (pLastCharProcessed)
        {
            prevCh32 = *pLastCharProcessed;
        }

        DrawCoord pos = startPos;
        DrawCoord::value_type xPosMax = pos.x + widthLim;

        bool breakOnLimit = false;

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

            ch32 = getChar32(text, textSize);
            std::string::size_type chPos32 = wstrStopChars32.find(ch32,0);
            if (chPos32!=std::string::npos)
            {
                break; // found stop char
            }

            if ((flags&DrawTextFlags::stopOnLineBreaks)!=0 && isAnyLineBreakChar(ch32))
            {
                break; // stop on line break
            }

            if ((flags&DrawTextFlags::stopOnTabs)!=0 && isAnyTabChar(ch32))
            {
                break; // stop on tab char
            }


            const auto &curCharWidth = *wit;
            bool isCombining = curCharWidth<0.0001;

            if (pLastCharProcessed && !isCombining)
            {
                *pLastCharProcessed = ch32;
            }

            if (!isCombining && (flags&DrawTextFlags::kerningDisable)==0)
            {
                pos.x += getKerningValue(kerningPairs, prevCh32, ch32);
            }

            if ((flags&DrawTextFlags::fitWidthDisable)==0) // Флаг не установлен, значит, ограничение по длине действует
            {
                auto testPosX = pos.x;
                
    
                if (isCombining)
                {
                    if ((flags&DrawTextFlags::combiningAsSeparateGlyph)==0)
                    {
                        if (nSymbolsDrawn)
                            --nSymbolsDrawn;
                    }

                    if (nColorIndex)
                        --nColorIndex;

                }

                if (drawEllipsis)
                {
                    testPosX += ellipsisWidth;
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
                    breakOnLimit = true;
                    break;
                }

            }

            if ((flags&DrawTextFlags::calcOnly)==0)
            {
                // Не только считаем, но и рисуем

                std::uint32_t curUintTextColor = (std::uint32_t)-1;
                if (pColors && nColorIndex<nColors)
                {
                    curUintTextColor = pColors[nColorIndex];
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

            const bool forceSpacesColoring = (flags&DrawTextFlags::forceSpacesColoring)!=0;
            if (!isAnyWhiteSpaceChar(ch32) || forceSpacesColoring)
            {
                ++nColorIndex;
            }

            if (isCombining)
            {
                prevCh32     = 0;
            }
            else
            {
                prevCh32     = ch32;
            }
            
        }

        if (breakOnLimit && drawEllipsis && (flags&DrawTextFlags::calcOnly)==0)
        {
            std::uint32_t curUintTextColor = (std::uint32_t)-1;
            if (pColors && nColorIndex<nColors)
            {
                curUintTextColor = pColors[nColorIndex];
            }

            auto textColorSaver = (curUintTextColor==(std::uint32_t)-1)
                                ? TextColorSaver(this)
                                : TextColorSaver(this, ColorRef::fromUnsigned(curUintTextColor) )
                                ;

            textOut(pos, ellipsisStr, 1);
        }

        // if (drawEllipsis) // ellipsisWidth



        if (pNextPosX)
        {
            *pNextPosX = pos.x;
        }

        if (pOverhang)
        {
            *pOverhang = fontMetrics.overhang;
        }

        // if (pLastCharProcessed)
        // {
        //     *pLastCharProcessed = ch32;
        // }

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


    virtual bool drawTextColoredEx( const DrawCoord               &startPos
                                  , const DrawCoord::value_type   &widthLim
                                  , DrawCoord::value_type         *pNextPosX //!< OUT, Положение вывода для символа, следующего за последним выведенным
                                  , DrawCoord::value_type         *pOverhang //!< OUT, Вынос элементов символа за пределы NextPosX - актуально, как минимум, для iatalic стиля шрифта
                                  , DrawTextFlags                 flags
                                  , const wchar_t                 *text
                                  , std::size_t                   textSize=(std::size_t)-1
                                  , std::uint32_t                 *pLastCharProcessed = 0 //!< IN/OUT last drawn char, for kerning calculation
                                  , std::size_t                   *pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                                  , const std::uint32_t           *pColors=0
                                  , std::size_t                   nColors=0
                                  , std::size_t                   *pSymbolsDrawn=0
                                  , const wchar_t                 *stopChars=0
                                  , int                           fontId=-1
                                  ) override
    {
        std::unordered_set<KerningPair> kerningPairs;
        getKerningPairsSet(kerningPairs, fontId);

        SimpleFontMetrics fontMetrics;
        if (!getSimpleFontMetrics(fontMetrics, fontId))
        {
            return false;
        }

        return drawTextColoredExImpl( kerningPairs, fontMetrics
                                    , startPos, widthLim, pNextPosX, pOverhang, flags
                                    , text, textSize, pLastCharProcessed, pCharsProcessed, pColors, nColors
                                    , pSymbolsDrawn, stopChars, fontId
                                    );
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
                                , 0 // pLastCharProcessed
                                , 0 // pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                                , pColors
                                , nColors
                                , 0 // pSymbolsDrawn=0
                                , stopChars
                                , fontId
                                );
    }


    bool drawParaColoredExImpl2( const std::unordered_set<KerningPair> &kerningPairs
                               , const SimpleFontMetrics               &fontMetrics
                               , std::vector<TextPortionInfo>          textPortions
                               , const DrawCoord                       &startPos
                               , const DrawCoord                       &limits        //!< Limits, vertical and horizontal, relative to start pos
                               , DrawCoord::value_type                 *pNextPosY     //!< OUT No line spacing added cause spacing between paras can be other then lineSpacing value
                               , bool                                  *pVerticalDone //!< OUT All/not all lines drawn, 
                               , const  DrawCoord::value_type          &lineSpacing   //!< Extra space between lines of text
                               , const  DrawCoord::value_type          &paraIndent    //!< Indent on the first line
                               , DrawCoord::value_type                 tabSize        //!< Size used for tabs if tabStops are over, >=0 - size in logical units, <0 - size in spaces
                               , DrawTextFlags                         flags
                               , HorAlign                              horAlign
                               // , VertAlign                             vertAlign
                               // , const wchar_t                         *text
                               // , std::size_t                           textSize=(std::size_t)-1
                               , const std::uint32_t                   *pColors=0
                               , std::size_t                           nColors=0
                               , const DrawCoord::value_type           *pTabStopPositions=0        //!< Relative to start pos X coord
                               , std::size_t                           nTabStopPositions=0
                               // , int                                   fontId=-1 // шрифт уже выбран, параметр не нужен
                               )
    {

        DrawCoord::value_type spaceWidth = 0;
        getCharWidth((std::uint32_t)' ', spaceWidth);
        if (tabSize<0)
        {
            tabSize = -tabSize * spaceWidth;
        }

        // Для каждого фрагмента у нас есть его ширина
        // Теперь надо выводить текст
        // Замечания
        //   Пробелы выводим сколько есть, но пропускаем в начале строки и не выводим в конце
        //   Если слово не первое в строке, и не влезает в остаток строки, то переносим его на следующую строку
        //   Если слово - первое в строке, и не влезает в строку, то обрезаем его сколько влезло, остаток переносим на следующую строку
        //   Выравнивание по левому краю можно рисовать сразу
        //   Выравнивание по центру, по правому краю и по ширине нужно рисовать только после того, 
        //     как посчитаны все слова в строке, и известно, сколько осталось пустого места
        //

        std::size_t lineNumber     = 0;
        std::size_t wordsDrawn     = 0;
        bool        wordSpacesOnly = true;
        std::size_t tpIdx          = 0;
        std::size_t tabNumber      = 0;
        DrawCoord::value_type limX = startPos.x + limits.x;
        DrawCoord::value_type limY = startPos.y + limits.y;
        DrawCoord   pos            = startPos;
        pos.x                     += (horAlign==HorAlign::center) ? (DrawCoord::value_type)0 : paraIndent; // Для первой строки сразу добавлям отступ параграфа


        const bool keepLtSpaces      = (horAlign==HorAlign::left) && ((flags&DrawTextFlags::keepLtSpaces)!=0);
        const bool noLastLineSpacing = (flags&DrawTextFlags::noLastLineSpacing)!=0;
        const auto ellipsisFlags     = DrawTextFlags::endEllipsis | DrawTextFlags::pathEllipsis | DrawTextFlags::wordEllipsis;
        const auto stopFlags         = DrawTextFlags::stopOnLineBreaks | DrawTextFlags::stopOnTabs;
        const auto fitHeightDisable  = (flags&DrawTextFlags::calcOnly)!=0 || (flags&DrawTextFlags::fitHeightDisable)!=0;

        bool bStoppedByLimY = false;

        auto checLimY = [&]()
        {
            if (fitHeightDisable)
            {
                auto testY = pos.y + fontMetrics.height;
                if (testY>limY)
                {
                    bStoppedByLimY = true;
                }
            }
        };


        auto skipSpaces = [&]()
        {
            if (keepLtSpaces)
            {
                return;
            }

            while(tpIdx!=textPortions.size())
            {
                if (textPortions[tpIdx].tpType!=TpType::space)
                    break;
                ++tpIdx;
            }
        };

        auto nextLine = [&](bool bSkipSpaces)
        {
            if (!wordSpacesOnly)
            {
                ++lineNumber;
                pos.y     += fontMetrics.height + lineSpacing; // Потом в конце надо отнять lineSpacing, если не первая строка
            }
            
            wordsDrawn     = 0;
            wordSpacesOnly = true;
            tabNumber      = 0;
            pos.x          = startPos.x;
            
            if (!bSkipSpaces)
            {
                skipSpaces();
            }

            checLimY();
        };

        auto checkGoNextLine = [&](bool bSkipSpaces)
        {
            if (pos.x>limX)
            {
                nextLine(bSkipSpaces);
            }
        };

        auto getTabStopPosX = [&]()
        {
            if (pTabStopPositions && tabNumber<nTabStopPositions)
            {
                // Если заданы таб позиции и не вылезли за их количество, то берём относительную позицию из массива и прибавляем к координате левого края
                return startPos.x + pTabStopPositions[tabNumber++];
            }
            else
            {
                // Если не заданы таб позиции или 
                tabNumber++;
                return pos.x + tabSize;
            }
        };

        //DrawCoord::value_type

        auto drawTextHelper = [&](const wchar_t *pText, std::size_t textSize /* , std::size_t *pSymbolsDrawn */ )
        {
            std::size_t nSymbolsDrawn = 0;
            bool bRes = drawTextColoredEx( pos, 0 // widthLim
                                         , 0 // pNextPosX - не нужен
                                         , 0 // pOverhang - не нужен
                                         , (flags | DrawTextFlags::fitWidthDisable) & ~(ellipsisFlags|stopFlags) // в лимит укладываться не нужно, и элипсисы не рисуем
                                         , pText, textSize // tpi.text.data(), numWcharsFit
                                         , 0 // pLastCharProcessed = 0 //!< IN/OUT last drawn char, for kerning calculation
                                         , 0 // pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                                         , pColors, nColors
                                         , &nSymbolsDrawn
                                         , 0 // stopChars
                                         , -1 // fontId
                                         );
            if (bRes && pColors)
            {
                if (nSymbolsDrawn>nColors)
                {
                    pColors += nColors;
                    nColors  = 0;
                }
                else
                {
                    pColors += nSymbolsDrawn;
                    nColors -= nSymbolsDrawn;
                }
            }

            return bRes;
        };


        auto splitTextPortionInfo = [&]( std::size_t tpIdx, std::size_t numWcharsFit)
        {
            const TextPortionInfo &tpi = textPortions[tpIdx];

            TextPortionInfo newTpi;
            newTpi.tpType = tpi.tpType; // TpType::space;
            newTpi.text = std::wstring_view( tpi.text.data()+numWcharsFit, tpi.text.size()-numWcharsFit );
            // https://www.nextptr.com/tutorial/ta1430524603/capture-this-in-lambda-expression-timeline-of-change
            newTpi.updateWidth(this, kerningPairs, flags, fontMetrics);
            textPortions.insert(textPortions.begin()+(std::ptrdiff_t)tpIdx, newTpi);
        };


        //if (startIdx==tpIdx)
        auto expandTpSpacesToFitWidth = [&](std::size_t startIdx, std::size_t endIdx, DrawCoord::value_type fitToWidth)
        {
            std::size_t spaceWords = 0;
            DrawCoord::value_type calulatedWidth = 0;

            for(std::size_t tpIdx=startIdx; tpIdx!=endIdx; ++tpIdx)
            {
                const TextPortionInfo &tpi = textPortions[tpIdx];
                calulatedWidth += tpi.width;
                if (tpi.tpType!=TpType::text)
                {
                    ++spaceWords;
                }
            }

            if (!spaceWords)
            {
                return;
            }

            if (endIdx==textPortions.size())
            {
                return;
            }

            MARTY_IDC_ARG_USED(fitToWidth);
            MARTY_IDC_ARG_USED(calulatedWidth);
            MARTY_IDC_ARG_USED(spaceWords);

            // #if 0
            DrawCoord::value_type addW = (fitToWidth - calulatedWidth) / (DrawCoord::value_type)spaceWords;

            for(std::size_t tpIdx=startIdx; tpIdx!=endIdx; ++tpIdx)
            {
                TextPortionInfo &tpi = textPortions[tpIdx];
                if (tpi.tpType!=TpType::text)
                {
                    tpi.width += addW;
                }
            }
            // #endif
        };



        bool globalStop = false;

        if (horAlign==HorAlign::left)
        {
            skipSpaces();

            //while(tpIdx!=textPortions.size() && !globalStop)
            for(; tpIdx!=textPortions.size() && !globalStop && !bStoppedByLimY; ++tpIdx)
            {
                // Готовы рисовать
                // Рисуем строку

                //for(; tpIdx!=textPortions.size() && !globalStop; ++tpIdx) // Пока строка не закончена
                {
                    const TextPortionInfo &tpi = textPortions[tpIdx];
                    if (tpi.tpType==TpType::space)
                    {
                        if (!keepLtSpaces)
                        {
                            if (wordsDrawn)
                            {
                                pos.x += tpi.width;
                            }
                            // Больше ничего не делаем, пробел же
                        }
                        else // Режим сохранения leading & trailing spaces
                        {
                            // тут надо проверить лимит и нарисовать/пропустить нужное число пробелов
                            // и при необходимости перенести строку

                            auto remainingWidth = limX - pos.x;
                            auto nCharsToDraw   = tpi.getNumberOfCharInfosFitWidth(remainingWidth);

                            if (nCharsToDraw>=tpi.charInfos.size())
                            {
                                // рисуем целиком, всё нормально
                                // опс, у нас пробелы, ничего делать и не надо
                                pos.x += tpi.width;
                            }
                            else if (!nCharsToDraw)
                            {
                                return false; // ничего не влезает - это ошибка с лимитами, слишком маленький
                            }
                            else
                            {
                                // Нужно проверить, это первое слово в текущей строке
                                // Если не первое, то все равно - пробелы можно разбивать всегда

                                auto numWcharsFit = tpi.getNumberOfCharsByNumberCharInfos(nCharsToDraw);
                                splitTextPortionInfo(tpIdx, numWcharsFit);
                            }
                        }

                        ++wordsDrawn; // Пробелы тоже считаем за отрисованые слова

                        checkGoNextLine(false /* no skipSpaces */);

                    }
                    else if (tpi.tpType==TpType::tab)
                    {
                        pos.x = getTabStopPosX();
                        // Больше ничего не делаем, просто табуляция
                        checkGoNextLine(false /* no skipSpaces */);
                    }
                    else if (tpi.tpType==TpType::text)
                    {
                        auto startX = pos.x;
                        auto endX   = startX + tpi.width + tpi.overhang;

                        // Какие у нас варианты
                        // 1) Начальная точка вышла за пределы - тут мы вообще ничего сделать не можем, надо переносить на следующую строку

                        bool startLimReached = false;
                        bool endLimReached   = false;

                        if (startX>=limX)
                            startLimReached = true;

                        if (endX>=limX) // 
                            endLimReached = true;

                        if (wordsDrawn==0)
                        {
                            if (startLimReached)
                            {
                                return false; // Слово ещё не нарисовано, но уже вышли за диапазон - явно что-то с диапазоном - возвращаем ошибку
                            }

                            if (endLimReached)
                            {
                                // Слов ещё не нарисовано, а текущее - уже не влезает, значит, рисуем то, что есть
                                auto remainingWidth = limX - pos.x;
                                auto nCharsToDraw   = tpi.getNumberOfCharInfosFitWidth(remainingWidth);
                                auto numWcharsFit   = tpi.getNumberOfCharsByNumberCharInfos(nCharsToDraw);

                                if (nCharsToDraw>=tpi.charInfos.size())
                                {
                                    // рисуем целиком, всё нормально, но вообще - странная ситуация, такого не должно быть
                                    if (!drawTextHelper(tpi.text.data(), numWcharsFit))
                                        return false;
                                    ++wordsDrawn;
                                    wordSpacesOnly = false;
                                    nextLine(false /* no skipSpaces */);
                                }
                                else if (!nCharsToDraw)
                                {
                                    return false; // ничего не влезает - это ошибка с лимитами, слишком маленький
                                }
                                else
                                {
                                    if (!drawTextHelper(tpi.text.data(), numWcharsFit))
                                        return false;
                                    ++wordsDrawn;
                                    wordSpacesOnly = false;
                                    splitTextPortionInfo(tpIdx, numWcharsFit);
                                    nextLine(false /* no skipSpaces */);
                                }

                            }
                            else // !endLimReached - порцию текста рисуем целиком
                            {
                                if (!drawTextHelper(tpi.text.data(), tpi.text.size()))
                                    return false;
                                ++wordsDrawn;
                                wordSpacesOnly = false;
                                pos.x += tpi.width;

                                checkGoNextLine(false /* no skipSpaces */);
                            }
                        
                        }
                        else // wordsDrawn!=0
                        {
                            if (startLimReached || endLimReached)
                            {
                                nextLine(false /* no skipSpaces */); // Начало или конец не влезает, но это не первое слово в строке - просто переходим на следующую строку
                                if (tpIdx)
                                {
                                    --tpIdx; // Но надо откатить индекс, иначе пропустим слово
                                }
                            }
                            else
                            {
                                // порцию текста рисуем целиком
                                if (!drawTextHelper(tpi.text.data(), tpi.text.size()))
                                    return false;
                                ++wordsDrawn;
                                wordSpacesOnly = false;
                                pos.x += tpi.width;
    
                                checkGoNextLine(false /* no skipSpaces */);
                            }

                        }

                    }
                    else
                    {
                        ATLASSERT(0);
                        throw std::runtime_error("Something goes wrong");
                    }
                }
            }

        }
        else if (horAlign==HorAlign::center || horAlign==HorAlign::right || horAlign==HorAlign::width)
        {
            skipSpaces();

            for(; tpIdx!=textPortions.size() && !bStoppedByLimY; )
            {
                skipSpaces();

                auto startIdx = tpIdx;
                //auto nextIdx  = startIdx;
                std::size_t spaceWords  = 0;
                bool breakOnLimit = false;

                DrawCoord::value_type curX = pos.x; // тут идёт учет отступа

                DrawCoord::value_type sumWidth = 0;

                for(; tpIdx!=textPortions.size() && (curX<limX); ++tpIdx)
                {
                    const TextPortionInfo &tpi = textPortions[tpIdx];

                    if ((curX+tpi.width)>limX)
                    {
                        breakOnLimit = true;
                        // if (tpIdx!=startIdx)
                        // {
                        //     --tpIdx;
                        // }
                        break;
                    }

                    ++wordsDrawn;

                    if (tpi.tpType==TpType::space)
                    {
                        ++spaceWords;
                    }
                    else if (tpi.tpType==TpType::tab)
                    {
                        ++spaceWords;
                    }
                    else if (tpi.tpType==TpType::text)
                    {
                        wordSpacesOnly = false;
                    }

                    sumWidth += tpi.width;
                    curX     += tpi.width;
                }

                // Удаляем пробелы в конце - в следющей строки они будут пропущены при вызове skipSpaces();
                if (tpIdx!=startIdx)
                {
                    for(--tpIdx; tpIdx!=startIdx; --tpIdx)
                    {
                        const TextPortionInfo &tpi = textPortions[tpIdx];
                        if (tpi.tpType==TpType::space || tpi.tpType==TpType::tab)
                        {
                            --spaceWords;
                            --wordsDrawn;
                            sumWidth -= tpi.width;
                        }
                        else
                        {
                            break;
                        }
                    }

                    ++tpIdx; // На следующее за последним словом слово
                }

                // !!!

                // Какие у нас тут варианты
                // I  Вышли по лимиту
                //    а) ни одного слова не добавили в текущую строку - значит, первое слово очень длинное 
                //       и не влезает в строку, значит, нужно его принудительно переносить
                //    б) слова в строке есть - добавляем нужное количество пробелов и рисуем без особых исключений
                // II Вышли по окончанию текста.
                //    а) Если нет слов для рисования, то ничего не делаем
                //    б) Если слова есть, но их длина меньше от какого-то процента - то не расстягиваем по ширине. 
                //       Если строка почти полностью заполнена, то растягиваем её. Применимо только при форматировании по ширине
                //       По умолчанию - наверно ничего не делаем, и надо бы добавить флаг, включающий такое поведение.
                //    в) Если слова есть и длина достаточна для растягивания - работаем по обычному алгоритму

                // Надо найти стартовую позицию
                // if (horAlign==HorAlign::center)
                // {
                //     pos.x += (limX-sumWidth)/2;
                // }
                // else if (horAlign==HorAlign::right)
                // {
                //     pos.x += (limX-sumWidth);
                // }
                // else if(horAlign==HorAlign::width)
                // {
                //     // ничего не надо, начинаем с pos.x, но надо расшить пробелы
                // }

                auto fitToWidth = limX - pos.x;


                if (breakOnLimit) // I
                {
                    if (startIdx==tpIdx)
                    {
                        // I.a
                        // Слов ещё не нарисовано, а текущее - уже не влезает, значит, рисуем то, что есть
                        const TextPortionInfo &tpi = textPortions[tpIdx];

                        auto remainingWidth = limX - pos.x;
                        auto nCharsToDraw   = tpi.getNumberOfCharInfosFitWidth(remainingWidth);
                        auto numWcharsFit   = tpi.getNumberOfCharsByNumberCharInfos(nCharsToDraw);

                        auto actualWidth = tpi.getWidthByNumberCharInfos(nCharsToDraw);
                        if (horAlign==HorAlign::center)
                        {
                            pos.x += (fitToWidth-actualWidth)/2;
                        }
                        else if (horAlign==HorAlign::right)
                        {
                            pos.x += (fitToWidth-actualWidth);
                        }


                        if (nCharsToDraw>=tpi.charInfos.size())
                        {
                            // рисуем целиком, всё нормально, но вообще - странная ситуация, такого не должно быть
                            if (!drawTextHelper(tpi.text.data(), numWcharsFit))
                                return false;
                            nextLine(true /* skipSpaces */);
                        }
                        else if (!nCharsToDraw)
                        {
                            return false; // ничего не влезает - это ошибка с лимитами, слишком маленький
                        }
                        else
                        {
                            if (!drawTextHelper(tpi.text.data(), numWcharsFit))
                                return false;
                            splitTextPortionInfo(tpIdx, numWcharsFit);
                            nextLine(true /* skipSpaces */);
                        }

                    }
                    else
                    {
                        // I.б

                        if (horAlign==HorAlign::center)
                        {
                            pos.x += (fitToWidth-sumWidth)/2;
                        }
                        else if (horAlign==HorAlign::right)
                        {
                            pos.x += (fitToWidth-sumWidth);
                        }
                        else if (horAlign==HorAlign::width)
                        {
                            expandTpSpacesToFitWidth(startIdx, tpIdx, fitToWidth);
                        }


                        for(; startIdx!=tpIdx && startIdx!=textPortions.size(); ++startIdx)
                        {
                            const TextPortionInfo &tpi = textPortions[startIdx];
                            if (!drawTextHelper(tpi.text.data(), tpi.text.size()))
                                return false;
                            pos.x += tpi.width;
                        }

                        nextLine(true /* skipSpaces */);
                    }
                }
                else // (!breakOnLimit) // II
                {
                    if (startIdx==tpIdx)
                    {
                        // II.а
                        // Ничего не делаем
                    }
                    else
                    {
                        // II.в
                        // работаем по обычному алгоритму
                        if (horAlign==HorAlign::center)
                        {
                            pos.x += (fitToWidth-sumWidth)/2;
                        }
                        else if (horAlign==HorAlign::right)
                        {
                            pos.x += (fitToWidth-sumWidth);
                        }
                        else if (horAlign==HorAlign::width)
                        {
                            expandTpSpacesToFitWidth(startIdx, tpIdx, fitToWidth);
                        }


                        for(; startIdx!=tpIdx && startIdx!=textPortions.size(); ++startIdx)
                        {
                            const TextPortionInfo &tpi = textPortions[startIdx];
                            if (!drawTextHelper(tpi.text.data(), tpi.text.size()))
                                return false;
                            pos.x += tpi.width;
                        }

                    }

                    nextLine(true /* skipSpaces */);
                    
                }


                if (tpIdx!=textPortions.size())
                {
                    //++tpIdx;
                }

            }

        }
        else
        {
            ATLASSERT(0);
            throw std::runtime_error("Invalid HorAlign value");
        }


        if (wordsDrawn && !wordSpacesOnly && !noLastLineSpacing)
        {
            pos.x += lineSpacing;
        }

        if (pNextPosY)
        {
            *pNextPosY = pos.x;
        }

        if (pVerticalDone)
        {
            *pVerticalDone = !bStoppedByLimY;
        }



        MARTY_IDC_ARG_USED(pVerticalDone);

        return true; // 
        
    }


    bool drawParaColoredExImpl( const std::unordered_set<KerningPair> &kerningPairs
                              , const SimpleFontMetrics               &fontMetrics
                              , DrawCoord                             startPos
                              , DrawCoord                             limits       //!< Limits, vertical and horizontal, relative to start pos
                              , DrawCoord::value_type                 *pNextPosY    //!< OUT No line spacing added cause spacing between paras can be other then lineSpacing value
                              , bool                                  *pVerticalDone
                              , const DrawCoord::value_type           &lineSpacing  //!< Extra space between lines of text
                              , const DrawCoord::value_type           &paraIndent   //!< Indent on the first line
                              , const DrawCoord::value_type           &tabSize      //!< Size used for tabs if tabStops are over, >=0 - size in logical units, <0 - size in spaces
                              , DrawTextFlags                         flags
                              , HorAlign                              horAlign
                              , VertAlign                             vertAlign
                              , const wchar_t                         *text
                              , std::size_t                           textSize=(std::size_t)-1
                              , std::size_t                           *pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                              , const std::uint32_t                   *pColors=0
                              , std::size_t                           nColors=0
                              , const DrawCoord::value_type           *pTabStopPositions=0        //!< Relative to start pos X coord
                              , std::size_t                           nTabStopPositions=0
                              , int                                   fontId=-1
                              )
    {
        textSize = checkCalcStringSize(text, textSize);

        // Нужно разбить на слова

        std::vector<TextPortionInfo>  textPortions;

        bool         stopParse = false;
        //ReadState           st = stReadSpaces;
        const wchar_t *tpStart = text; // сохранили на будущее

        auto putCurTextPortion = [&](TpType curTpType) // std::uint32_t ch32, std::size_t charLen
        {
            std::size_t sz = (std::size_t)(text-tpStart);
            if (sz)
            {
                TextPortionInfo tpi;
                tpi.tpType = curTpType;
                tpi.text   = std::wstring_view(tpStart, sz);
                textPortions.emplace_back(tpi);
            }

            tpStart = text;
        };

        auto incTextPtr = [&](std::size_t charLen)
        {
            if (!textSize)
                return;

            if (charLen<=textSize)
            {
                text      += charLen;
                textSize  -= charLen;
            }
            else
            {
                text      += textSize;
                textSize   = 0;
            }
        };

        TpType       curTpType = TpType::space;


        while(!stopParse)
        {
            std::size_t charLen = getCharLen(text, textSize);
            if (!charLen)
            {
                stopParse = true;
                break;
            }

            std::uint32_t ch32 = getChar32(text, textSize);
            if (isAnyLineBreakChar(ch32))
            {
                // Параграф рисуем до первого символа перевода строки (любого)
                stopParse = true;
                break;
            }

            //TODO: !!! Не забыть последний фрагмент поместить в вектор textPortions

            switch(curTpType)
            {
                case TpType::space:
                {
                    if (isAnyBreakingSpaceChar(ch32))
                    {
                        incTextPtr(charLen); // двигаем указатель текущего текста и всё - режим не сменился, ничего не надо делать
                    }
                    else if (isAnyBreakingDashChar(ch32))
                    {
                        putCurTextPortion(curTpType);
                        // curTpType не меняем
                        incTextPtr(charLen); // двигаем указатель текущего текста
                        putCurTextPortion(TpType::text); // дефисы по одному добавляем как текст
                    }
                    else if (isAnyTabChar(ch32))
                    {
                        putCurTextPortion(curTpType);
                        curTpType = TpType::tab;
                        incTextPtr(charLen); // двигаем указатель текущего текста
                        putCurTextPortion(curTpType); // табы по одному добавляем
                    }
                    else // какой-то текст
                    {
                        putCurTextPortion(curTpType);
                        curTpType = TpType::text;
                        incTextPtr(charLen); // двигаем указатель текущего текста
                    }
                }
                break;

                case TpType::text :
                {
                    if (isAnyBreakingSpaceChar(ch32))
                    {
                        putCurTextPortion(curTpType);
                        curTpType = TpType::space;
                        incTextPtr(charLen); // двигаем указатель текущего текста
                    }
                    else if (isAnyBreakingDashChar(ch32))
                    {
                        incTextPtr(charLen); // двигаем указатель текущего текста
                        putCurTextPortion(TpType::text); // добавляем дефис вместе с предыдущим текстом, разбивая слово с дефисом на части
                    }
                    else if (isAnyTabChar(ch32))
                    {
                        putCurTextPortion(curTpType);
                        curTpType = TpType::tab;
                        incTextPtr(charLen); // двигаем указатель текущего текста
                        putCurTextPortion(curTpType); // табы по одному добавляем
                    }
                    else // какой-то текст
                    {
                        incTextPtr(charLen); // двигаем указатель текущего текста и всё - режим не сменился, ничего не надо делать
                    }
                }
                break;

                case TpType::tab  :
                {
                    if (isAnyBreakingSpaceChar(ch32))
                    {
                        putCurTextPortion(curTpType);
                        curTpType = TpType::space;
                        incTextPtr(charLen); // двигаем указатель текущего текста
                    }
                    else if (isAnyBreakingDashChar(ch32))
                    {
                        putCurTextPortion(curTpType);
                        // curTpType не меняем
                        incTextPtr(charLen); // двигаем указатель текущего текста
                        putCurTextPortion(TpType::text); // дефисы по одному добавляем как текст
                    }
                    else if (isAnyTabChar(ch32))
                    {
                        putCurTextPortion(curTpType);
                        incTextPtr(charLen); // двигаем указатель текущего текста
                        putCurTextPortion(curTpType); // табы по одному добавляем
                    }
                    else // какой-то текст
                    {
                        putCurTextPortion(curTpType);
                        curTpType = TpType::text;
                        incTextPtr(charLen); // двигаем указатель текущего текста
                    }
                }
                break;

                case TpType::invalid: [[fallthrough]];

                default: 
                {
                    ATLASSERT(0);
                }
            }
        
        }

        putCurTextPortion(curTpType);

        //-------

        // У нас есть строка, разбитая по словам
        // Нам надо вычислить отображаемую длину (ширину) каждого слова.
        // Может возникать ситуация, когда надо будет слово дополнительно разбить на части, так как оно всё равно не влезает.
        // Для каждого символа надо вычислить ширину, кернинг с предыдущим символом (для первого - 0)
        // Также надо запомнить индекс символа в строке и его длину в исходном тексте в wchar'ах

        if (fontId<0)
        {
            fontId = getCurFont();
        }

        auto fontSaver = FontSaver(this, fontId);

        // DrawCoord::value_type spaceWidth = 0;
        // pdc->getCharWidth((std::uint32_t)' ', spaceWidth);
        // if (tabSize<0)
        // {
        //     tabSize = -tabSize * spaceWidth;
        // }

        //std::vector<TextPortionInfo>  textPortions;
        for(auto &tp : textPortions)
        {
            // Также обновляет charInfos
            tp.updateWidth(this, kerningPairs, flags, fontMetrics);
        }

        //-------

        if (pTabStopPositions && nTabStopPositions)
        {
            // Мы не умеем делать tab стопы, если выравнивание идёт не по левому краю
            horAlign = HorAlign::left;
        }


        if ((flags&DrawTextFlags::fitHeightDisable)!=0)
        {
            // По вертикали вписываться не надо, нет лимита
            // Раз нет лимита по высоте - то нет и выравнивания по вертикали
        
            vertAlign = VertAlign::top;
        }

        // Отсекаем некорректные значения
        if (vertAlign!=VertAlign::center && vertAlign!=VertAlign::bottom)
        {
            vertAlign = VertAlign::top;
        }


        //const DrawCoord pos = startPos;

        if (vertAlign==VertAlign::center || vertAlign==VertAlign::bottom)
        {
            DrawCoord::value_type nextPosY = startPos.y;
            // Нам нужно узнать реальную высоту, которую занимает параграф
            bool bRes = drawParaColoredExImpl2( kerningPairs, fontMetrics, textPortions
                                              , startPos, limits
                                              , &nextPosY
                                              , pVerticalDone
                                              , lineSpacing  //!< Extra space between lines of text
                                              , paraIndent   //!< Indent on the first line
                                              , tabSize      //!< Size used for tabs if tabStops are over, >=0 - size in logical units, <0 - size in spaces
                                              , flags | DrawTextFlags::fitHeightDisable | DrawTextFlags::calcOnly // установили флаг "нет лимита по высоте" и "только калькуляция", вроде ничего больше не надо
                                              , horAlign
                                              //, VertAlign::top
                                              //, text
                                              //, textSize
                                              , 0 // const std::uint32_t                   *pColors=0
                                              , 0 // nColors=0
                                              , pTabStopPositions        //!< Relative to start pos X coord
                                              , nTabStopPositions
                                              // , fontId
                                              );

            if (!bRes)
            {
                return bRes;
            }

            auto actualParaHeight = nextPosY - startPos.y;
            auto limDelta = limits.y - actualParaHeight; // предплагаем, что лимит (куда надо вписать параграф) больше, чем реальная высота параграфа

            if (vertAlign==VertAlign::center)
            {
                limDelta /= 2; // Если надо выровнять посередине, то сдвигаем вниз только на половину дельты
            }

            if (limDelta<0)
            {
                limDelta = 0; // смещение не может быть меньше нуля
            }

            startPos.y += limDelta; // сместили вниз

            // Лимиты относительные, относительно стартовой позиции. Если позицию сместили вниз, то лимит надо уменьшить, 
            // чтобы абсолютное значение осталось тем же
            limits  .y -= limDelta;

        }


        bool bRes = drawParaColoredExImpl2( kerningPairs, fontMetrics, textPortions
                                          , startPos, limits
                                          , pNextPosY
                                          , pVerticalDone
                                          , lineSpacing  //!< Extra space between lines of text
                                          , paraIndent   //!< Indent on the first line
                                          , tabSize      //!< Size used for tabs if tabStops are over, >=0 - size in logical units, <0 - size in spaces
                                          , flags
                                          , horAlign
                                          //, VertAlign::top
                                          //, text
                                          //, textSize
                                          , pColors
                                          , nColors
                                          , pTabStopPositions        //!< Relative to start pos X coord
                                          , nTabStopPositions
                                          // , fontId
                                          );

        if (!bRes)
        {
            return bRes;
        }

        if (pCharsProcessed)
        {
            *pCharsProcessed = (std::size_t)(text - tpStart);
        }

        return bRes;
    }


    virtual bool drawParaColoredEx( const DrawCoord                       &startPos
                                  , const DrawCoord                       &limits       //!< Limits, vertical and horizontal, relative to start pos
                                  , DrawCoord::value_type                 *pNextPosY    //!< OUT No line spacing added cause spacing between paras can be other then lineSpacing value
                                  , bool                                  *pVerticalDone
                                  , const DrawCoord::value_type           &lineSpacing  //!< Extra space between lines of text
                                  , const DrawCoord::value_type           &paraIndent   //!< Indent on the first line
                                  , const DrawCoord::value_type           &tabSize      //!< Size used for tabs if tabStops are over
                                  , DrawTextFlags                         flags
                                  , HorAlign                              horAlign
                                  , VertAlign                             vertAlign
                                  , const wchar_t                         *text
                                  , std::size_t                           textSize=(std::size_t)-1
                                  , std::size_t                           *pCharsProcessed=0 //!< OUT Num chars, not symbols/glyphs
                                  , const std::uint32_t                   *pColors=0
                                  , std::size_t                           nColors=0
                                  , const DrawCoord::value_type           *pTabStopPositions=0        //!< Relative to start pos X coord
                                  , std::size_t                           nTabStopPositions=0
                                  , int                                   fontId=-1
                                  ) override
    {
        std::unordered_set<KerningPair> kerningPairs;
        getKerningPairsSet(kerningPairs, fontId);

        SimpleFontMetrics fontMetrics;
        if (!getSimpleFontMetrics(fontMetrics, fontId))
        {
            return false;
        }

        return drawParaColoredExImpl( kerningPairs, fontMetrics
                                    , startPos, limits, pNextPosY, pVerticalDone
                                    , lineSpacing, paraIndent, tabSize
                                    , flags, horAlign, vertAlign
                                    , text, textSize, pCharsProcessed
                                    , pColors, nColors
                                    , pTabStopPositions, nTabStopPositions
                                    , fontId
                                    );
    
    
    }





