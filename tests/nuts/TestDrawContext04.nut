

local pos = Drawing.Coords(50,50);

local keyState = {}

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
    return false;
}

function Game::onUpdate(tickDelta)
{
    local bUpdate = false;

    local xSpeed = 0;
    local ySpeed = 0;

    xSpeed = -keySpeed(Vk.Code.Left);
    if (xSpeed==0)
    {
        xSpeed = -keySpeed(Vk.Code.Right);
    }

    ySpeed = -keySpeed(Vk.Code.Up);
    if (ySpeed==0)
    {
        ySpeed = -keySpeed(Vk.Code.Down);
    }

    
    if (xSpeed==0 && ySpeed==0)
    {
        return false;
    }

    pos.x = pos.x + (xSpeed*tickDelta);///10.0;
    pos.y = pos.y + (ySpeed*tickDelta);///10.0;

    if (pos.x<0.0)
    {
        pos.x = 0.0;
    }

    if (pos.x>100.0)
    {
        pos.x = 100.0;
    }

    if (pos.y<0.0)
    {
        pos.y = 0.0;
    }

    if (pos.y>100.0)
    {
        pos.y = 100.0;
    }

    return true;
}

function Game::onKeyEvent(bDown, nChar, nRepCnt)
{
    if (nChar==Vk.Code.Left || nChar==Vk.Code.Right || nChar==Vk.Code.Up || nChar==Vk.Code.Down)
    {
        if (bDown)
        {
            keyState[nChar] = nRepCnt;
            return true;
        }
        else
        {
            keyState[nChar] = 0;
        }
    }

    return false;

}


function Game::onPaint(dc)
{
    local D = Drawing;

    local framePen  = dc.createSolidPen(D.PenParams(3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Blue);

    dc.setOffset(D.Coords(10,10));

    local scale = 5;
    dc.setScale(D.Scale(scale,scale));


    dc.selectPen(framePen);
    dc.roundRect(3, Drawing.Coords( 0,0), Drawing.Coords(100,100));

    local circlePen = dc.createSolidPen(D.PenParams(2, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Green);
    dc.selectPen(circlePen);
    dc.circle(pos, 3);

    local genericFontParamsH20 = D.FontParams(2  , D.FontWeight.Normal, D.FontStyleFlags.None, "Arial");
    local arialFontId          = dc.createFontWithFace( genericFontParamsH20 , "Arial"          );


    dc.textOutWithFontAndColor( D.Coords( 120,120), arialFontId, D.Color.fromRgb(0,0,0), formatCoord(pos) + "  " + getKeyStatesStr());


}




