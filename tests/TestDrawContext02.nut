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

    local greenPen = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromString("green"));
    //local greenPen = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Green);
    dc.selectPen(greenPen);

    dc.moveTo(Drawing.Coords(10,10));
    dc.lineTo(Drawing.Coords(30,30));

}


