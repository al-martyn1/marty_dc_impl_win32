
local sysInfoStr = "Uninitialized";

local pos = Drawing.Coords(50,50);

local keyState = {}

local borderPenWidth  = 3;
local puckBorderWidth = 2;
local puckR = 3;

keyState[Vk.Code.Left ] <- 0;
keyState[Vk.Code.Right] <- 0;
keyState[Vk.Code.Up   ] <- 0;
keyState[Vk.Code.Down ] <- 0;


function formatCoord(c)
{
    return "(" + c.x.tostring() + ";" + c.y.tostring() + ")";
}

function getKeyStatesStr()
{
    return 
    keyState[Vk.Code.Left ].tostring() + ", "
    keyState[Vk.Code.Right].tostring() + ", "
    keyState[Vk.Code.Up   ].tostring() + ", "
    keyState[Vk.Code.Down ].tostring()
    ;
}

function keySpeed(vk)
{
    local nRepCnt = keyState[vk];
    if (nRepCnt==0)
    {
        return 0;
    }
    else if (nRepCnt<3)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}


function Game::onLoad(bFirstTime)
{
    /*
    local sys = appHost.sys;

    // local infoStr = 
    sysInfoStr = sys.info.kernel + "/" + sys.info.platform + ", " + sys.info.os + "-" + sys.info.osVer + ", " + sys.info.graphicsBackend.name;

    if (bFirstTime)
    {
        sys.smpprintln("Starting on " + sysInfoStr);
    }
    else
    {
        sys.smpprintln("Reloading on " + sysInfoStr);
    }

    return false;
    */
}

function Game::onUpdate(tickDelta)
{
    local bUpdate = false;

    local xSpeed = 0;
    local ySpeed = 0;

    xSpeed = - keySpeed(Vk.Code.Left);
    if (xSpeed==0)
    {
        xSpeed = keySpeed(Vk.Code.Right);
    }

    ySpeed = -keySpeed(Vk.Code.Up);
    //ySpeed = -keySpeed(Vk.Code.Key0);
    if (ySpeed==0)
    {
        ySpeed = keySpeed(Vk.Code.Down);
    }

    
    if (xSpeed==0 && ySpeed==0)
    {
        return false;
    }

    pos.x = pos.x + (xSpeed*tickDelta)/20.0;
    pos.y = pos.y + (ySpeed*tickDelta)/20.0;

    if (pos.x<(0.0+puckR))
    {
        pos.x = (0.0+puckR);
    }

    if (pos.x>(100.0-puckR))
    {
        pos.x = (100.0-puckR);
    }

    if (pos.y<(0.0+puckR))
    {
        pos.y = (0.0+puckR);
    }

    if (pos.y>(100.0-puckR))
    {
        pos.y = (100.0-puckR);
    }

    return true;
}

function Game::onKeyEvent(bDown, nChar, nRepCnt)
{
    if (nChar==Vk.Code.Left || nChar==Vk.Code.Right || nChar==Vk.Code.Up || nChar==Vk.Code.Down)
    {
        if (bDown)
        {
            //smpprint  ("Key down, code: ");
            //smpprint  (nChar)
            //smpprint  (", nRepCnt: ");
            //smpprintln(nRepCnt);
            keyState[nChar] = nRepCnt;
            return true;
        }
        else
        {
            //smpprint  ("Key up, code: ");
            //smpprintln(nChar);
            keyState[nChar] = 0;
        }
    }

    return false;

}


function Game::onPaint(dc)
{
    local D = Drawing;

    dc.setOffset(D.Coords(10,10));

    local scale = 5;
    dc.setScale(D.Scale(scale,scale));

    local px1 = dc.getPixelSize();


    //local drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.SmoothingPrecise);
    local drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.PixelPrecise);

    local framePen  = dc.createSolidPen(D.PenParams(borderPenWidth, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Blue);

    local pixelPen  = dc.createSolidPen(D.PenParams(0, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Purple);
    dc.selectPen(pixelPen);

    local rectLt = D.Coords(130,0);
    local rectSz = D.Coords(5,5);
    dc.rect(rectLt, rectLt+rectSz);

    local sz2px = dc.mapRawToLogicSize(D.Coords(2,2));

    local gradientParams = D.GradientParams(D.Colors.Green, D.Colors.Blue, D.Colors.Red, 0.5);
    dc.fillGradientRect(rectLt+sz2px, rectLt+rectSz-sz2px, gradientParams, D.GradientType.Vertical, false); // or true


    dc.selectPen(framePen);
    dc.roundRect(3, Drawing.Coords( 0,0), Drawing.Coords(100,100));

    local circlePen = dc.createSolidPen(D.PenParams(puckBorderWidth, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Green);
    dc.selectPen(circlePen);
    dc.circle(pos, puckR);

    local genericFontParamsH20 = D.FontParams(2  , D.FontWeight.Normal, D.FontStyleFlags.None, "Arial");
    local arialFontId          = dc.createFontWithFace( genericFontParamsH20 , "Arial"          );


    dc.textOutWithFontAndColor( D.Coords( 120,120), arialFontId, D.Color.fromRgb(0,0,0), formatCoord(pos) + "  " + getKeyStatesStr());

    dc.textOutWithFontAndColor( D.Coords(  10,120), arialFontId, D.Color.fromRgb(0,0,0), sysInfoStr);

}




