// TestDrawContext02.nut - OnPaint handler test

function coordsSum(c1, c2)
{
    return Drawing.Coords(c1.x+c2.x, c1.y+c2.y);
}

function test_drawRect_10_20(dc, offs)
{
    dc.moveTo(coordsSum(offs,Drawing.Coords(10,10)));
    dc.lineTo(coordsSum(offs,Drawing.Coords(20,10)));
    dc.lineTo(coordsSum(offs,Drawing.Coords(20,20)));
    dc.lineTo(coordsSum(offs,Drawing.Coords(10,20)));
    dc.lineTo(coordsSum(offs,Drawing.Coords(10,10)));
    dc.closeFigure();
}

function test_drawFishTail_5_10(dc, offs )
{
    dc.moveTo(coordsSum(offs,Drawing.Coords( 5,10)));
    dc.lineTo(coordsSum(offs,Drawing.Coords(10,10)));
    dc.lineTo(coordsSum(offs,Drawing.Coords(10, 5)));
    dc.closeFigure();
}

function test_drawRoundSquare(dc, offs, sz, cornersR )
{
    dc.roundRect( cornersR.tofloat()
                , offs //+DrawCoord(10,10) // leftTop
                , coordsSum(offs,sz) //+DrawCoord(20,20) // rightBottom
                );
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

    local genericFontParamsH8  = D.FontParams(8, D.FontWeight.Normal, D.FontStyleFlags.None, "Arial");
    //local genericFontParamsH20 = clone genericFontParamsH8;
    //genericFontParamsH20.height = 20; // Type error bad cast expected: FLOAT got: INTEGER
    //genericFontParamsH20.height = 20.0; // : Access violation writing location 0x0000000000000000.

    local genericFontParamsH20 = D.FontParams(20, D.FontWeight.Normal, D.FontStyleFlags.None, "Arial");
    genericFontParamsH20.height = 20.0;

    local arialFontId    = dc.createFontWithFace( genericFontParamsH8 , "Arial"          );
    local timesFontId    = dc.createFontWithFace( genericFontParamsH8 , "Times New Roman");
    local courierFontId  = dc.createFontWithFace( genericFontParamsH8 , "Courier New"    );
    local labelsFontId   = dc.createFontWithFace( genericFontParamsH20, "Courier New"    );

    
    local penId          = dc.selectNewSolidPen( D.PenParams(0.5, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromRgb(0, 168, 0) );
    local brushId        = dc.selectNewSolidBrush( D.Color.fromRgb(0, 0, 168) );
    local yellowBrushId  = dc.selectNewSolidBrush( D.Color.fromRgb(255, 242, 0) );
    local pinkBrushId    = dc.selectNewSolidBrush( D.Color.fromRgb(255, 160, 128) );

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
   
    // test_drawRoundSquare(dc, offs, sz, cornersR )
    test_drawRoundSquare(dc, Drawing.Coords(90,10), Drawing.Coords(30, 20), 4 );
   
    //test_drawSnake_10_60(pDc, Drawing.Coords(115,0), 4 /* 0.8 */  /* 0.5 */ );
   
    dc.endPath( true, true );


}


