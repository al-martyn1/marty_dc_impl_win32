// TestDrawContext02.nut - OnPaint handler test

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
    local genericFontParamsH20 = clone genericFontParamsH8;
    genericFontParamsH20.height = 20; // Type error bad cast expected: FLOAT got: INTEGER
    //genericFontParamsH20.height = 20.0; // : Access violation writing location 0x0000000000000000.





}


