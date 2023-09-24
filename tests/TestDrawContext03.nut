// TestDrawContext03.nut - OnPaint handler test with NultiDC


local drawingPrecise = Drawing.DrawingPrecise.DefPrecise;


function coordsSum(c1, c2)
{
    return c1+c2;
}

function test_drawSpider_drawGrid(dc, pos, sz, penId )
{
    local leftTop        = pos - Drawing.Coords(2*sz,2*sz);
    //local rightBottom  = pos + Drawing.Coords(2*sz,2*sz);

    local szFull = 4*sz;

    local  nLines = 5;

    dc.selectPen( penId );

    local x = leftTop.x;
    local y = leftTop.y;

    for(local i=0; i<nLines; i+=1)
    {
        dc.moveTo(Drawing.Coords(x,y));
        dc.lineTo(Drawing.Coords(x+szFull,y));
        y += sz;
    }

    y = leftTop.y;
    for(local i=0; i<nLines; i+=1)
    {
        dc.moveTo(Drawing.Coords(x,y));
        dc.lineTo(Drawing.Coords(x,y+szFull));
        x += sz;
    }

}

function test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, centerPos, startMul, endMul, ccw, drawPoints )
{
    local szCoord = Drawing.Coords(sz,sz);

    local leftTop       = centerPos - szCoord;
    local rightBottom   = centerPos + szCoord;

    local startPos      = centerPos + szCoord*startMul;
    local endPos        = centerPos + szCoord*endMul;

    dc.selectPen( penId );
    dc.moveTo(startPos);
    dc.ellipticArcTo( leftTop, rightBottom, startPos, endPos, ccw );

    if (drawPoints)
    {
        dc.selectPen(schPenId);
        dc.markerAdd(startPos);
        dc.markerAdd(endPos);
    }

}


const FlagSpider1    = 1;
const FlagSpider2    = 2;
const FlagSpiderBoth = 3;

function test_drawSpider(dc, pos, sz, penId, schPenId, spiderFlags, drawGrid, drawPoints)
{
    local z = 0;
    local szCoord = Drawing.Coords(sz,sz);

    local leftCenter        = pos - Drawing.Coords(sz,z);
    local leftLeftTop       = leftCenter - szCoord;
    local leftRightBottom   = leftCenter + szCoord;

    local rightCenter       = pos + Drawing.Coords(sz,z);
    local rightLeftTop      = rightCenter - szCoord;
    local rightRightBottom  = rightCenter + szCoord;

    local topCenter         = pos - Drawing.Coords(z,sz);
    local topLeftTop        = topCenter - szCoord;
    local topRightBottom    = topCenter + szCoord;

    local bottomCenter      = pos + Drawing.Coords(z,sz);
    local bottomLeftTop     = bottomCenter - szCoord;
    local bottomRightBottom = bottomCenter + szCoord;


    if (drawGrid)
    {
        test_drawSpider_drawGrid( dc, pos, sz, schPenId );
    }

    local ccw = true;
    local cw  = false;

    if (spiderFlags&FlagSpider1)
    {
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, rightCenter, Drawing.Coords(-1,0), Drawing.Coords(0,-1), cw , drawPoints ); //
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, leftCenter , Drawing.Coords( 1,0), Drawing.Coords(0,-1), ccw, drawPoints ); //
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, leftCenter , Drawing.Coords( 1,0), Drawing.Coords(0, 1), cw , drawPoints ); //
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, rightCenter, Drawing.Coords(-1,0), Drawing.Coords(0, 1), ccw, drawPoints ); //
    }

    if (spiderFlags&FlagSpider2)
    {
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, topCenter   , Drawing.Coords(0, 1), Drawing.Coords( 1,0), ccw, drawPoints ); //
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, topCenter   , Drawing.Coords(0, 1), Drawing.Coords(-1,0), cw , drawPoints ); //
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, bottomCenter, Drawing.Coords(0,-1), Drawing.Coords(-1,0), ccw, drawPoints ); //
        test_drawSpider_drawArcHelper(dc, penId, schPenId, sz, bottomCenter, Drawing.Coords(0,-1), Drawing.Coords( 1,0), cw , drawPoints ); //
    }
}


function test_drawRect_10_20(dc, offs)
{
    dc.moveTo(offs+Drawing.Coords(10,10));
    dc.lineTo(offs+Drawing.Coords(20,10));
    dc.lineTo(offs+Drawing.Coords(20,20));
    dc.lineTo(offs+Drawing.Coords(10,20));
    dc.lineTo(offs+Drawing.Coords(10,10));

    dc.closeFigure();
}


function test_drawRects(dc, offs)
{
    dc.roundRect    (1, offs+Drawing.Coords( 0,0), offs+Drawing.Coords( 8, 8));
    dc.fillRoundRect(1, offs+Drawing.Coords(10,0), offs+Drawing.Coords(18, 8));
    dc.rect         (   offs+Drawing.Coords(20,0), offs+Drawing.Coords(28, 8));
    dc.fillRect     (   offs+Drawing.Coords(30,0), offs+Drawing.Coords(38, 8));
}


function test_drawFishTail_5_10(dc, offs )
{
    dc.moveTo(offs+Drawing.Coords( 5,10));
    dc.lineTo(offs+Drawing.Coords(10,10));
    dc.lineTo(offs+Drawing.Coords(10, 5));

    dc.closeFigure();
}

function test_drawRoundSquare(dc, offs, sz, cornersR )
{
    dc.roundRect( cornersR.tofloat()
                , offs //+DrawCoord(10,10) // leftTop
                , coordsSum(offs,sz) //+DrawCoord(20,20) // rightBottom
                );
}

function test_drawSnake_10_60(dc, offs, cornersR )
{
    local D  = Drawing;        // short alias for namespace Drawing

    local snakePoints = [ D.Coords( 30, 60 )
                        , D.Coords( 40, 60 )
                        , D.Coords( 50, 60 )
                        , D.Coords( 50, 50 )
                        , D.Coords( 60, 50 )
                        , D.Coords( 60, 40 )
                        , D.Coords( 60, 30 )
                        , D.Coords( 60, 20 )
                        , D.Coords( 50, 20 )
                        , D.Coords( 50, 10 )
                        , D.Coords( 40, 10 )
                        , D.Coords( 30, 10 )
                        , D.Coords( 20, 10 )
                        , D.Coords( 10, 10 )
                        , D.Coords( 10, 20 )
                        , D.Coords( 10, 30 )
                        , D.Coords( 10, 40 )
                        , D.Coords( 20, 40 )
                        , D.Coords( 30, 40 )
                        , D.Coords( 30, 30 )
                        , D.Coords( 20, 30 )
                        , D.Coords( 20, 20 )
                        , D.Coords( 30, 20 )
                        , D.Coords( 40, 20 )
                        , D.Coords( 40, 30 )
                        , D.Coords( 50, 30 )
                        , D.Coords( 50, 40 )
                        , D.Coords( 40, 40 )
                        , D.Coords( 40, 50 )
                        , D.Coords( 30, 50 )
                        //, D.Coords( ,  }
                        ];

    //local snakePointsShifted = array(0);
    foreach(coord in snakePoints)
    {
        //snakePointsShifted.append(D.Coords(coord.x+offs.x, coord.y+offs.y))
        coord.x = coord.x + offs.x;
        coord.y = coord.y + offs.y;
    }
    //for(auto &p : snakePoints )
    //    p += offs;

    // http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#id1
    //dc.roundRectFigure( cornersR, snakePoints.len(), snakePoints );
    dc.roundRectFigure( cornersR, snakePoints );

}

function arcToByAngle(dc, arcCenter, startPointOffset, angleGradus)
{
    //DrawCoord resEndPos;
    dc.moveTo(arcCenter + startPointOffset);
    dc.arcByAngleDeg(arcCenter, angleGradus);
    dc.markerAdd(arcCenter);
    dc.markerAdd(dc.getLastArcEndPos());
    dc.markerAdd(arcCenter + startPointOffset);
}

function arcToByEnd(dc, arcCenter, startPointOffset, endPointOffset, dir)
{
    //lout << "CCW: " << dir << " \n";
    dc.moveTo(arcCenter + startPointOffset);
    dc.arcToPos(arcCenter, arcCenter + endPointOffset, dir );
    dc.markerAdd(arcCenter);
    dc.markerAdd(arcCenter + startPointOffset);
    dc.markerAdd(arcCenter + endPointOffset);
}


function onPaint(drawingContext)
{
    local D  = Drawing;        // short alias for namespace Drawing
    local dc = drawingContext; // short alias for Drawing.Context object

    local redClr   = D.Color(D.Colors.Red);
    local redClr2  = D.Color("red");
    local redCopy  = D.Color(redClr);
    local redClone = clone redClr;
    local clrEmpty = D.Color(D.Color(0));
    local clrBlue  = D.Colors.Blue;

    local greenPen = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromString("green"));
    local bluePen  = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), clrBlue);
    local blueThickPen  = dc.createSolidPen(D.PenParams(25, D.LineEndcapStyle.Square/*Round/Square/Flat*/, D.LineJoinStyle.Mitter/*Bevel/Mitter/Round*/), clrBlue);
    //local greenPen = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Green);
    //dc.selectPen(greenPen);
    //dc.selectPen(bluePen);

    // D.Colors.Red   = D.Color.fromString("green"); // Runtime error at (onPaint) F:\_github\underwood-trainer\_libs\marty_dc_impl_win32\tests\TestDrawContext02.nut:21: trying to set 'class'
    //D.Colors.Red.r = 127;
    //D.Colors.Red.g = 127;
    //D.Colors.Red.b = 127;

    //local redPen   = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Red);
    //dc.selectPen(redPen);

    //dc.moveTo(Drawing.Coords(10,10));
    //dc.lineTo(Drawing.Coords(30,30));


    dc.setSmoothingMode( D.SmoothingMode.AntiAlias );
    dc.setBkMode( D.BkMode.Transparent );

    dc.markerSetDefSize( 0.5 );

    local scale = 6;

    dc.setOffset(D.Coords(1.4,1.4));
    dc.setScale(D.Scale(scale,scale));
    dc.setPenScale(scale);

    local genericFontParamsH4  = D.FontParams(4, D.FontWeight.Normal, D.FontStyleFlags.None, "Arial");
    local genericFontParamsH8  = D.FontParams(8, D.FontWeight.Normal, D.FontStyleFlags.None, "Arial");
    //local genericFontParamsH20 = clone genericFontParamsH8;
    //genericFontParamsH20.height = 20; // Type error bad cast expected: FLOAT got: INTEGER
    //genericFontParamsH20.height = 20.0; // : Access violation writing location 0x0000000000000000.

    local genericFontParamsH20 = D.FontParams(20, D.FontWeight.Normal, D.FontStyleFlags.None, "Arial");
    genericFontParamsH20.height = 20.0;

    local infoFontId     = dc.createFontWithFace( genericFontParamsH4 , "Arial"          );
    local arialFontId    = dc.createFontWithFace( genericFontParamsH8 , "Arial"          );
    local timesFontId    = dc.createFontWithFace( genericFontParamsH8 , "Times New Roman");
    local courierFontId  = dc.createFontWithFace( genericFontParamsH8 , "Courier New"    );
    local labelsFontId   = dc.createFontWithFace( genericFontParamsH20, "Courier New"    );

    
    local penId          = dc.selectNewSolidPen( D.PenParams(0.5, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(0, 168, 0) );
    local brushId        = dc.selectNewSolidBrush( D.Color.fromRgb(0, 0, 168) );
    local yellowBrushId  = dc.createSolidBrush( D.Color.fromRgb(255, 242, 0) );
    local pinkBrushId    = dc.createSolidBrush( D.Color.fromRgb(255, 160, 128) );

    local normalKeySize1 = 12.0; // 50; // 40; // 80; // 120, 140, 30
    local keyFrameWidth  = normalKeySize1/30.0; // 1  /20
    
    local lgrayPenId     = dc.createSolidPen( D.PenParams(keyFrameWidth, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(160, 160, 160) );
    local llgrayPenId    = dc.createSolidPen( D.PenParams(keyFrameWidth, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(192, 192, 192) );
    local dgrayPenId     = dc.createSolidPen( D.PenParams(keyFrameWidth, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(128, 128, 128) );

    local cosmeticPenId  = dc.createSolidPen( D.PenParams(0, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(0, 0, 0) );
    local cosmeticPenId2 = dc.createSolidPen( D.PenParams(0, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(168, 168, 168) );
    local markersPenId   = dc.createSolidPen( D.PenParams(0, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(0, 0, 0) );

    local prevCosmeticPenId = dc.setDefaultCosmeticPen(cosmeticPenId);

    // Fishtail

    local fishTailPos = Drawing.Coords(65,0);

    dc.beginPath();
    
        test_drawRect_10_20(dc, fishTailPos);
       
        test_drawFishTail_5_10(dc, fishTailPos);
       
        test_drawSnake_10_60(dc, D.Coords(115,0), 4 /* 0.8 */  /* 0.5 */ );
    dc.endPath( true, true );

    // test_drawRoundSquare(dc, offs, sz, cornersR )
    drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.SmoothingPrecise);
    test_drawRoundSquare(dc, D.Coords(90,10), Drawing.Coords(30, 20), 4 );
    drawingPrecise = dc.setDrawingPrecise(drawingPrecise);

    test_drawRects(dc, D.Coords(80,1));

    // Text output
    drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.TextPrecise);
    // Меняем обратно на то, что было - проверка, будет ли работать - оказалось - не будет, и всё потому, что я просто забыл забиндить set/getDrawingPrecise
    //drawingPrecise = dc.setDrawingPrecise(drawingPrecise);

    dc.textOutWithFontAndColor( D.Coords( 76,14), arialFontId  , D.Color.fromRgb(128,0,0)  , "Arial A"   );
    dc.textOutWithFontAndColor( D.Coords( 94,28), timesFontId  , D.Color.fromRgb(128,128,0), "Times T"   );
    //dc.textOutWithFontAndColor( D.Coords(112,42), courierFontId, D.Color.fromRgb(0,128,128), "Courier C" );
    dc.textOutWithFontAndColor( D.Coords(112,42), courierFontId, D.Colors.Magenta, "Courier C" );

    drawingPrecise = dc.setDrawingPrecise(drawingPrecise);

    local normalKeySize = D.Coords( normalKeySize1, normalKeySize1 );

    local spiderBasePos  = D.Coords(14,85);
    local spiderCellSize = 6;

    dc.selectPen(penId);

    local markersCollectMode = dc.setCollectMarkers(true);

    test_drawSpider(dc, spiderBasePos+Drawing.Coords( 30*0, 0), spiderCellSize, penId, cosmeticPenId, FlagSpider1   , true, true);
    test_drawSpider(dc, spiderBasePos+Drawing.Coords( 30*1, 0), spiderCellSize, penId, cosmeticPenId, FlagSpider2   , true, true);
    test_drawSpider(dc, spiderBasePos+Drawing.Coords( 30*2, 0), spiderCellSize, penId, cosmeticPenId, FlagSpiderBoth, true, true);

    dc.setCollectMarkers(markersCollectMode);

    dc.markersDrawEx(markersPenId);


    // Test LineJoinStyle triangle
    local orgPen = dc.selectPen( blueThickPen );
    dc.beginPath();
    dc.moveTo(D.Coords(114, 75)); // 125
    dc.lineTo(D.Coords(124, 60)); // 105
    dc.lineTo(D.Coords(134, 75)); // 125
    //pDc->lineTo(Drawing.Coords(15,120));
    dc.closeFigure();
    dc.endPath( true, false );

    // Rect with pixel precise
    drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.PixelPrecise);
    dc.rect(D.Coords(5,32), D.Coords(50,45));
    drawingPrecise = dc.setDrawingPrecise(drawingPrecise);

    dc.selectPen( orgPen );


    const iconHeight   = 10.0;
    const iconInterval = 0.25;
    local iconBottomCurrentPos = 0;

    const gradientRectSizeX = 50.0;
    const gradientRectSizeY = 4.0;

    local gradientParams = D.GradientParams(D.Color(0x10EA14), D.Color(0x193FE8), D.Color(0xE50909), 0.8);


    local gradientSamplePos = D.Coords(4, iconBottomCurrentPos);



    // TEST_DC_GRADIENT_RECT
    dc.fillGradientRect( gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY)
                       , gradientParams, D.GradientType.Vertical
                       , true // false // excludeFrame
                       );


    gradientSamplePos.y += 4 + 2*iconInterval;

    dc.fillGradientRect( gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY/2)
                       , gradientParams, D.GradientType.Horizontal
                       , true // false // excludeFrame
                       );

    gradientSamplePos.y += 2 + 2*iconInterval;


    
    // TEST_DC_GRADIENT_ROUNDRECT

    local gradientDrawBreakPos  = 0.5;
    
    local gradientRectR    = gradientRectSizeY/2;
    if (gradientRectR<1)
    {
        gradientRectR = 1;
    }


    dc.fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY)
                            , gradientParams, D.GradientType.Vertical
                            , true // false // excludeFrame
                            , gradientDrawBreakPos
                            , D.GradientRoundRectFillFlags.None
                            );
    gradientSamplePos.y += gradientRectSizeY + 2*iconInterval;


    dc.fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY)
                            , gradientParams, D.GradientType.Vertical
                            , true // false // excludeFrame
                            , gradientDrawBreakPos
                            , D.GradientRoundRectFillFlags.SquareBegin // underwood::GradientRoundRectFillFlags::squareEnd
                            );
    gradientSamplePos.y += gradientRectSizeY + 2*iconInterval;


    dc.fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY)
                            , gradientParams, D.GradientType.Vertical
                            , true // false // excludeFrame
                            , gradientDrawBreakPos
                            , D.GradientRoundRectFillFlags.SquareEnd
                            );
    gradientSamplePos.y += gradientRectSizeY + 2*iconInterval;


    dc.fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY/2)
                            , gradientParams, D.GradientType.Horizontal
                            , true // false // excludeFrame
                            , gradientDrawBreakPos
                            , D.GradientRoundRectFillFlags.None
                            );
    gradientSamplePos.y += gradientRectSizeY/2 + 2*iconInterval;


    dc.fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY/2)
                            , gradientParams, D.GradientType.Horizontal
                            , true // false // excludeFrame
                            , gradientDrawBreakPos
                            , D.GradientRoundRectFillFlags.None
                            | D.GradientRoundRectFillFlags.NoFillBegin
                            );
    gradientSamplePos.y += gradientRectSizeY/2 + 2*iconInterval;


    dc.fillGradientRoundRect( gradientRectR, gradientSamplePos  , gradientSamplePos + D.Coords(gradientRectSizeX, gradientRectSizeY/2)
                            , gradientParams, D.GradientType.Horizontal
                            , true // false // excludeFrame
                            , gradientDrawBreakPos
                            , D.GradientRoundRectFillFlags.None
                            | D.GradientRoundRectFillFlags.NoFillEnd
                            );
    gradientSamplePos.y += gradientRectSizeY/2 + 2*iconInterval;


    // TEST_DC_GRADIENT_CIRCLE

    local gradientCircleSamplePos = D.Coords( 4 + gradientRectSizeX + 2*gradientRectSizeY, iconBottomCurrentPos+2*gradientRectSizeY );

    local circleR = 1.6*gradientRectSizeY;

    dc.fillGradientCircle(gradientCircleSamplePos, circleR, gradientParams, true);
    


    dc.selectPen(penId);

    // TEST_DC_ROUND_SQUARE
    local arcCenter = fishTailPos + D.Coords(15, 35);
    //local  arcCenter = D.Coords(50, 50);
    


    // TEST_DC_ARCTO

    if (dc.isPathStarted())
    {
        dc.endPath(true,false);
    }

    markersCollectMode = dc.setCollectMarkers(true);

    //arcToByAngle( dc, arcCenter, startPointOffset, angleGradus )
    arcToByAngle(dc, arcCenter, D.Coords(2, 2), 40);
    arcToByAngle(dc, arcCenter, D.Coords(3, 3), 80);

    // function arcToByEnd(dc, arcCenter, startPointOffset, endPointOffset, dir)
    arcToByEnd(dc, arcCenter, D.Coords(5, 5), D.Coords(5, -5), true ); // direction - CounterClockWise
    // Первая ниже центра, вторая - выше, по часовой - большая дуга
    arcToByEnd(dc, arcCenter, D.Coords(6, 6), D.Coords(6, -6), false); // direction - ClockWise

    // Первая выше центра, вторая - ниже, против часовой - большая дуга
    arcToByEnd(dc, arcCenter, D.Coords(7, -7), D.Coords(7, 7), true ); // direction - CounterClockWise
    // Первая выше центра, вторая - ниже, по часовой - малая дуга
    arcToByEnd(dc, arcCenter, D.Coords(8, -8), D.Coords(8, 8), false); // direction - ClockWise


    dc.setCollectMarkers(markersCollectMode);

    dc.markersDrawEx(markersPenId);


    if (dc.isPathStarted())
    {
        dc.endPath(true,false);
    }


    // dc.textOutWithFontAndColor( D.Coords( 76,14), arialFontId  , D.Color.fromRgb(128,0,0)  , "Arial A"   );
    // dc.textOutWithFontAndColor( D.Coords( 94,28), timesFontId  , D.Color.fromRgb(128,128,0), "Times T"   );
    // //dc.textOutWithFontAndColor( D.Coords(112,42), courierFontId, D.Color.fromRgb(0,128,128), "Courier C" );
    // dc.textOutWithFontAndColor( D.Coords(112,42), courierFontId, D.Colors.Magenta, "Courier C" );

    // infoFontId

    // print draw context size on left top and right bottom positions

    drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.TextPrecise);

    local ltLtPos = D.Coords(1,1);

    local dcSize  = dc.getSize();

    local dcSizeStr = "X: " + dcSize.x.tostring() + ",  Y: " + dcSize.y.tostring();

    dc.textOutWithFontAndColor( ltLtPos, infoFontId, D.Color.fromRgb(0,0,0), dcSizeStr );

    local ltRbPos = dcSize - D.Coords(50,10);
    // local ltRbPos = D.Coords(15,10);

    dc.textOutWithFontAndColor( ltRbPos, infoFontId, D.Color.fromRgb(0,0,0), dcSizeStr );

    drawingPrecise = dc.setDrawingPrecise(drawingPrecise);

}


