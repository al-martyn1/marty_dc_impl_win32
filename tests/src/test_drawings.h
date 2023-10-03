#pragma once

// #include "keyboard_geometry/keyboard_drawing.h"

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





} // namespace underwood

