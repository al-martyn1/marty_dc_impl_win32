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
                               , const DrawCoord                       &limits       //!< Limits, vertical and horizontal, relative to start pos
                               , DrawCoord::value_type                 *pNextPosY    //!< OUT No line spacing added cause spacing between paras can be other then lineSpacing value
                               , const  DrawCoord::value_type          &lineSpacing  //!< Extra space between lines of text
                               , const  DrawCoord::value_type          &paraIndent   //!< Indent on the first line
                               , DrawCoord::value_type                 tabSize      //!< Size used for tabs if tabStops are over, >=0 - size in logical units, <0 - size in spaces
                               , DrawTextFlags                         flags
                               , HorAlign                              horAlign
                               , VertAlign                             vertAlign
                               , const wchar_t                         *text
                               , std::size_t                           textSize=(std::size_t)-1
                               , const std::uint32_t                   *pColors=0
                               , std::size_t                           nColors=0
                               , const DrawCoord::value_type           *pTabStopPositions=0        //!< Relative to start pos X coord
                               , std::size_t                           nTabStopPositions=0
                               , int                                   fontId=-1
                               )
    {
        MARTY_IDC_ARG_USED(kerningPairs     );
        MARTY_IDC_ARG_USED(fontMetrics      );
        MARTY_IDC_ARG_USED(textPortions     );
        MARTY_IDC_ARG_USED(startPos         );
        MARTY_IDC_ARG_USED(limits           );
        MARTY_IDC_ARG_USED(pNextPosY        );
        MARTY_IDC_ARG_USED(lineSpacing      );
        MARTY_IDC_ARG_USED(paraIndent       );
        MARTY_IDC_ARG_USED(tabSize          );
        MARTY_IDC_ARG_USED(flags            );
        MARTY_IDC_ARG_USED(horAlign         );
        MARTY_IDC_ARG_USED(vertAlign        );
        MARTY_IDC_ARG_USED(text             );
        MARTY_IDC_ARG_USED(textSize         );
        MARTY_IDC_ARG_USED(pColors          );
        MARTY_IDC_ARG_USED(nColors          );
        MARTY_IDC_ARG_USED(pTabStopPositions);
        MARTY_IDC_ARG_USED(nTabStopPositions);
        MARTY_IDC_ARG_USED(fontId           );

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
        pos.x                     += paraIndent; // Для первой строки сразу добавлям отступ параграфа

        const bool keepLtSpaces      = (flags&DrawTextFlags::keepLtSpaces)!=0;
        const bool noLastLineSpacing = (flags&DrawTextFlags::noLastLineSpacing)!=0;
        const auto ellipsisFlags     = DrawTextFlags::endEllipsis | DrawTextFlags::pathEllipsis | DrawTextFlags::wordEllipsis;
        const auto stopFlags         = DrawTextFlags::stopOnLineBreaks | DrawTextFlags::stopOnTabs;


        MARTY_IDC_ARG_USED(limX);
        MARTY_IDC_ARG_USED(limY);

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

        auto nextLine = [&]()
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
            
            skipSpaces();
        };

        auto checkGoNextLine = [&]()
        {
            if (pos.x>limX)
            {
                nextLine();
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



        bool globalStop = false;

        if (horAlign==HorAlign::left)
        {
            skipSpaces();

            //while(tpIdx!=textPortions.size() && !globalStop)
            for(; tpIdx!=textPortions.size() && !globalStop; ++tpIdx)
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
                            pos.x += tpi.width;
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

                        checkGoNextLine();

                    }
                    else if (tpi.tpType==TpType::tab)
                    {
                        pos.x = getTabStopPosX();
                        // Больше ничего не делаем, просто табуляция
                        checkGoNextLine();
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
                                    nextLine();
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
                                    nextLine();
                                }

                            }
                            else // !endLimReached - порцию текста рисуем целиком
                            {
                                if (!drawTextHelper(tpi.text.data(), tpi.text.size()))
                                    return false;
                                ++wordsDrawn;
                                wordSpacesOnly = false;
                                pos.x += tpi.width;

                                checkGoNextLine();
                            }
                        
                        }
                        else // wordsDrawn!=0
                        {
                            if (startLimReached)
                            {
                                nextLine(); // Начало не влезает, но это не первое слово в строке - просто переходим на следующую строку
                            }

                            if (endLimReached)
                            {
                                nextLine(); // Конец не влезает, но это не первое слово в строке - просто переходим на следующую строку
                            }

                            // порцию текста рисуем целиком
                            if (!drawTextHelper(tpi.text.data(), tpi.text.size()))
                                return false;
                            ++wordsDrawn;
                            wordSpacesOnly = false;
                            pos.x += tpi.width;

                            checkGoNextLine();
                        }

                    }
                    else
                    {
                        ATLASSERT(0);
                        throw std::runtime_error("Something goes wrong");
                    }
                }
            }


#if 0
    enum class TpType // TextPortionType
    {
        invalid,
        space,
        text,
        tab
    };

    struct TextPortionInfo
    {
        TpType                      tpType   ;
        std::wstring_view           text     ;
        std::vector<CharInfo>       charInfos;
        DrawCoord::value_type       width    ;
        DrawCoord::value_type       overhang ;
#endif


        }
        else if (horAlign==HorAlign::center)
        {
        }
        else if (horAlign==HorAlign::right)
        {
        }
        else if (horAlign==HorAlign::width)
        {
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

        return true; // 
        
    }


    bool drawParaColoredExImpl( const std::unordered_set<KerningPair> &kerningPairs
                              , const SimpleFontMetrics               &fontMetrics
                              , DrawCoord                             startPos
                              , DrawCoord                             limits       //!< Limits, vertical and horizontal, relative to start pos
                              , DrawCoord::value_type                 *pNextPosY    //!< OUT No line spacing added cause spacing between paras can be other then lineSpacing value
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
                                              , lineSpacing  //!< Extra space between lines of text
                                              , paraIndent   //!< Indent on the first line
                                              , tabSize      //!< Size used for tabs if tabStops are over, >=0 - size in logical units, <0 - size in spaces
                                              , flags | DrawTextFlags::fitHeightDisable | DrawTextFlags::calcOnly // установили флаг "нет лимита по высоте" и "только калькуляция", вроде ничего больше не надо
                                              , horAlign
                                              , VertAlign::top
                                              , text
                                              , textSize
                                              , 0 // const std::uint32_t                   *pColors=0
                                              , 0 // nColors=0
                                              , pTabStopPositions        //!< Relative to start pos X coord
                                              , nTabStopPositions
                                              , fontId
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
                                          , lineSpacing  //!< Extra space between lines of text
                                          , paraIndent   //!< Indent on the first line
                                          , tabSize      //!< Size used for tabs if tabStops are over, >=0 - size in logical units, <0 - size in spaces
                                          , flags
                                          , horAlign
                                          , VertAlign::top
                                          , text
                                          , textSize
                                          , pColors
                                          , nColors
                                          , pTabStopPositions        //!< Relative to start pos X coord
                                          , nTabStopPositions
                                          , fontId
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
                                    , startPos, limits, pNextPosY
                                    , lineSpacing, paraIndent, tabSize
                                    , flags, horAlign, vertAlign
                                    , text, textSize, pCharsProcessed
                                    , pColors, nColors
                                    , pTabStopPositions, nTabStopPositions
                                    , fontId
                                    );
    
    
    }





