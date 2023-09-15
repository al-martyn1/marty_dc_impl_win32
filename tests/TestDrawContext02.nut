// TestDrawContext02.nut - OnPaint handler test

function onPaint(drawingContext)
{
    local D  = Drawing;        // short alias for namespace Drawing
    local dc = drawingContext; // short alias for Drawing.Context object

    local greenPen = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Color.fromString("green"));
    dc.selectPen(greenPen);

    dc.moveTo(Drawing.Coords(10,10));
    dc.lineTo(Drawing.Coords(30,30));

}


