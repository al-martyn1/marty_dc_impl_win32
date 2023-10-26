// Inline types. Not for include itself
    
    struct CharInfo
    {
        std::size_t                idx;
        std::size_t                len; // длина в тексте в wchar'ах
        bool                       bCombining;
        DrawCoord::value_type      width;
        DrawCoord::value_type      kerningValue; // в паре с предыдущим символом
    };

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


        void updateCharInfos( IDrawContext *pdc
                            , const std::unordered_set<KerningPair> &kerningPairs
                            //, const SimpleFontMetrics               &fontMetrics
                            , DrawTextFlags                         flags
                            )
        {
            // MARTY_IDC_ARG_USED(flags);
            // MARTY_IDC_ARG_USED(fontMetrics);

            const wchar_t *pText = text.data();
            std::size_t textSize = text.size();
            std::size_t idx = 0;
            charInfos.clear();
            charInfos.reserve(textSize);

            std::uint32_t prevCh32  = 0;
            std::uint32_t ch32      = 0;


            std::size_t curCharLen = pdc->getCharLen(pText, textSize);
            for( ; textSize && curCharLen!=0
                 ; curCharLen = pdc->getCharLen(pText, textSize)
               )
            {
                ATLASSERT(curCharLen<=textSize);
                if (curCharLen>textSize)
                {
                    break;
                }
      
                ch32 = pdc->getChar32(pText, textSize);

                DrawCoord::value_type charWidth = 0;
                if (!pdc->getCharWidth(ch32, charWidth))
                {
                    // ASSERTнуться или просто игнорим?
                }

                bool isCombining = charWidth<0.0001;

                DrawCoord::value_type kerningValue = 0;

                if (!isCombining && (flags&DrawTextFlags::kerningDisable)==0)
                {
                    kerningValue = pdc->getKerningValue(kerningPairs, prevCh32, ch32);
                }

                charInfos.emplace_back(CharInfo{idx, curCharLen, isCombining, charWidth, kerningValue});


                pText    += curCharLen;
                textSize -= curCharLen;
                idx      += curCharLen;
                prevCh32  = ch32      ;

            }

        } // updateCharInfos


        void updateWidthOnly(const SimpleFontMetrics &fontMetrics)
        {
            overhang = fontMetrics.overhang;

            width = 0;

            for(const auto &ci : charInfos)
            {
                width += ci.width;
                width += ci.kerningValue;
            }
        }


        void updateWidth( IDrawContext *pdc
                        , const std::unordered_set<KerningPair> &kerningPairs
                        , DrawTextFlags                         flags
                        , const SimpleFontMetrics               &fontMetrics
                        )
        {
            updateCharInfos(pdc, kerningPairs, flags);
            updateWidthOnly(fontMetrics);
        }


        // С учетом сурогатных пар
        std::size_t getNumberOfCharInfosFitWidth(DrawCoord::value_type lim) const
        {
            std::size_t            chIdx = 0;
            DrawCoord::value_type  w     = 0;

            lim -= overhang;

            for(; chIdx!=charInfos.size(); ++chIdx)
            {
                const CharInfo &ci = charInfos[chIdx];
                w += ci.kerningValue;
                w += ci.width;

                if (w>lim)
                {
                   break;
                }
            }

            // Могли остаться символы, которые не прибавляют длины выводимому тексту
            for(; chIdx!=charInfos.size(); ++chIdx)
            {
                if (!charInfos[chIdx].bCombining)
                {
                    break;
                }
            }

            return chIdx;

        } // getNumberOfCharsFitWidth

        std::size_t getNumberOfCharsByNumberCharInfos(std::size_t nCharInfos) const
        {
            if (nCharInfos>=charInfos.size())
            {
                if (charInfos.empty())
                {
                    return 0;
                }
                else
                {
                    return charInfos.back().idx + charInfos.back().len;
                    // можно и text.size() вернуть
                }
            }
            else
            {
                return charInfos[nCharInfos].idx; // 
            }

        }

    }; // struct TextPortionInfo
