
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


local currentPressedMouseButton = Drawing.MouseButton.None;
local mouseButtonPressedPos     = Drawing.Coords(0,0);


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

function prepareDc(dc)
{
    dc.setOffset(Drawing.Coords(10,10));
    local scale = 5;
    dc.setScale(Drawing.Scale(scale,scale));
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
    smpprintln("Game::onPaint");

    local D = Drawing;

    prepareDc(dc);

    local px1 = dc.getPixelSize();


    //local drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.SmoothingPrecise);
    local drawingPrecise = dc.setDrawingPrecise(D.DrawingPrecise.PixelPrecise);

    local framePen  = dc.createSolidPen(D.PenParams(borderPenWidth, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Blue);

    local pixelPen  = dc.createSolidPen(D.PenParams(0, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), D.Colors.Purple);
    dc.selectPen(pixelPen);

    local rectLt = D.Coords(102,0);
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

    local genericFontParamsH20 = D.FontParams(2  , D.FontWeight.Normal, D.FontStyleFlags.Normal, "Arial");
    local genericFontParamsH30 = D.FontParams(3  , D.FontWeight.Normal, D.FontStyleFlags.Normal, "Arial");
    local arialFontId          = dc.createFontWithFace( genericFontParamsH20 , "Arial"          );
    local arialFont30Id        = dc.createFontWithFace( genericFontParamsH30 , "Arial"          );


    dc.textOutWithFontAndColor( D.Coords( 102,102), arialFontId, D.Color.fromRgb(0,0,0), formatCoord(pos) + "  " + getKeyStatesStr());

    dc.textOutWithFontAndColor( D.Coords(  10,102), arialFontId, D.Color.fromRgb(0,0,0), sysInfoStr);

    // D.Colors.Green, D.Colors.Blue, D.Colors.Red
    dc.drawTextColored( D.Coords(  10,106), 50, D.DrawTextFlags.EndEllipsis
                      , "Lorem ipsum dolor sit amet, consectetur_adipiscing_elit,_sed_do eiusmod tempor incididunt ut Labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
                      , ""
                      , [D.Colors.GreenYellow, D.Colors.Red, D.Colors.Gray, D.Colors.Blue, D.Colors.Aqua/*Cyan*/, D.Colors.DarkCyan, D.Colors.MediumCyan] 
                      , [D.Colors.Brown, D.Colors.DarkCyan, D.Colors.GreenYellow, D.Colors.Red, D.Colors.Gray, D.Colors.MediumCyan] 
                      , arialFont30Id
                      );

    dc.drawParaColored( D.Coords(  10,116)
                      , D.Coords(  70,10)
                      , 0.1 // lineSpacing
                      , 1.5 // paraIndent
                      , 2.0 // tabSize
                      , D.DrawTextFlags.FitHeightDisable | D.DrawTextFlags.ColoringWords // None
                      , D.HorAlign.Left
                      , D.VertAlign.Top
                      , "Lorem\tipsum\tdolor sit amet, consectetur_adipiscing_elit,_sed_do eiusmod tempor incididunt ut Labore et dolore magna aliqua."
                      , [D.Colors.GreenYellow, D.Colors.Red, D.Colors.Gray, D.Colors.Blue, D.Colors.Aqua/*Cyan*/, D.Colors.DarkCyan, D.Colors.MediumCyan] 
                      , [D.Colors.Brown, D.Colors.DarkCyan, D.Colors.GreenYellow, D.Colors.Red, D.Colors.Gray, D.Colors.MediumCyan] 
                      , [25, 45.0, 65.0]
                      , arialFont30Id
                      );

    dc.drawMultiParasColored( D.Coords(  110,10)
                            , D.Coords(  70 ,10)
                            , 0.1 // lineSpacing
                            , 1.5 // paraSpacing
                            , 2.5 // paraIndent
                            , 2.0 // tabSize
                            , D.DrawTextFlags.FitHeightDisable | D.DrawTextFlags.ColoringWords // None
                            , D.HorAlign.Left
                            , D.VertAlign.Top
                            , "Lorem\tipsum\tdolor sit amet, consectetur_adipiscing_elit,_sed_do eiusmod tempor incididunt ut Labore et dolore magna aliqua.\nUt enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\nDuis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
                            , [D.Colors.GreenYellow, D.Colors.Red, D.Colors.Gray, D.Colors.Blue, D.Colors.Aqua/*Cyan*/, D.Colors.DarkCyan, D.Colors.MediumCyan] 
                            , [D.Colors.Brown, D.Colors.DarkCyan, D.Colors.GreenYellow, D.Colors.Red, D.Colors.Gray, D.Colors.MediumCyan] 
                            , [25, 45.0, 65.0]
                            , [D.Colors.BlueViolet, D.Colors.DarkOliveGreen, D.Colors.RebeccaPurple]
                            , [D.Colors.GhostWhite, D.Colors.AntiqueWhite, D.Colors.FloralWhite]
                            , arialFont30Id
                            );

    dc.polyCubicBeziers([D.Coords(10,20), D.Coords(10,10), D.Coords(25,10), D.Coords(25,20), D.Coords(25,30), D.Coords(40,30), D.Coords(40,20)]);

}

// must return on of:
//   Drawing.CallbackResultFlags.None
//   Drawing.CallbackResultFlags.Repaint
//   Drawing.CallbackResultFlags.CaptureMouse
//   Drawing.CallbackResultFlags.ReleaseCapture
//   Drawing.CallbackResultFlags.DisableTimerUpdate
//   Drawing.CallbackResultFlags.EnableTimerUpdate

// Если moveEventType равно Leave, то mbStateFlags и point содержат мусор
//   Drawing.MouseMoveEventType.Move
//   Drawing.MouseMoveEventType.Hover
//   Drawing.MouseMoveEventType.Leave

// mbStateFlags
//   Drawing.MouseButtonStateFlags.None
//   Drawing.MouseButtonStateFlags.LeftButtonPressed
//   Drawing.MouseButtonStateFlags.RightButtonPressed
//   Drawing.MouseButtonStateFlags.ShiftPressed
//   Drawing.MouseButtonStateFlags.ControlPressed
//   Drawing.MouseButtonStateFlags.MiddleButtonPressed
//   Drawing.MouseButtonStateFlags.XButton1Pressed
//   Drawing.MouseButtonStateFlags.XButton2Pressed

// point - точка в клиентской части, если задаётся масштаб/офсет, то надо не забыть вызвать для точки dc.mapRawToLogicPos(point)

function Game::onMouseMoveEvents(dc, moveEventType, mbStateFlags, point)
{
    local D = Drawing;

    // // smpprintln("onMouseMoveEvents");

    prepareDc(dc);
    point = dc.mapRawToLogicPos(point);

    if (moveEventType==D.MouseMoveEventType.Move && (currentPressedMouseButton==D.MouseButton.LeftButton || currentPressedMouseButton==D.MouseButton.RightButton))
    {
        // smpprintln("onMouseMoveEvents while button pressed");

        local px = dc.getPixelSize();

        local clr = D.Colors.Green;
        if (currentPressedMouseButton==D.MouseButton.RightButton)
        {
            clr = D.Colors.Blue;
        }

        local pixel3Pen  = dc.createSolidPen(D.PenParams(px.x*3, D.LineEndcapStyle.Round, D.LineJoinStyle.Round), clr);
        dc.selectPen(pixel3Pen);

        if (currentPressedMouseButton==Drawing.MouseButton.LeftButton)
        {
            dc.rect(mouseButtonPressedPos, point);
        }

        if (currentPressedMouseButton==Drawing.MouseButton.RightButton)
        {
            local distance = dc.distanceBetween(mouseButtonPressedPos, point);
            dc.circle(mouseButtonPressedPos, distance);
        }

    }

    return D.CallbackResultFlags.None;
}

// mouseButton
//   Drawing.MouseButton.None
//   Drawing.MouseButton.LeftButton
//   Drawing.MouseButton.RightButton  
//   Drawing.MouseButton.MiddleButton
//   Drawing.MouseButton.XButton1
//   Drawing.MouseButton.XButton2

// buttonEvent
//   Drawing.MouseButtonEvent.Released
//   Drawing.MouseButtonEvent.Pressed
//   Drawing.MouseButtonEvent.DoubleClick

function Game::onMouseButtonEvents(dc, mouseButton, buttonEvent, mbStateFlags, point)
{
    local D = Drawing;

    // smpprint("onMouseButtonEvents ");

    prepareDc(dc);
    point = dc.mapRawToLogicPos(point);

    if (buttonEvent==Drawing.MouseButtonEvent.Pressed)
    {
        // smpprint(", pressed");

        if (currentPressedMouseButton!=Drawing.MouseButton.None)
        {
            // smpprintln(", something already pressed");
            return Drawing.CallbackResultFlags.None; // Какая-то кнопка уже нажата, на вторую не реагируем
        }

        if (mouseButton==Drawing.MouseButton.LeftButton || mouseButton==Drawing.MouseButton.RightButton)
        {
            // smpprintln(", pressed L/R button at {" + point.toString() + "}");
            currentPressedMouseButton = mouseButton;
            mouseButtonPressedPos     = point;
            return Drawing.CallbackResultFlags.CaptureMouse | Drawing.CallbackResultFlags.DisableTimerUpdate; // Нужно сделать захват мыши, чтобы события о перемещении мыши приходили в окно даже если мышь за пределами окна
        }
    }

    else if (buttonEvent==Drawing.MouseButtonEvent.Released)
    {
        // smpprint(", released");

        if (currentPressedMouseButton==mouseButton) // Отпущенная кнопка соответствует тому, что зафиксировано
        {
            // smpprintln(", same as pressed");
            currentPressedMouseButton = Drawing.MouseButton.None; // Очищаем зафиксированное нажатие
            return Drawing.CallbackResultFlags.ReleaseCapture | Drawing.CallbackResultFlags.EnableTimerUpdate;
        }
    }

    else
    {
    }

    // smpprintln("");

    return D.CallbackResultFlags.None;
}


function Game::onMouseWheel(dc, zDelta, mbStateFlags, point)
{
    local D = Drawing;

    //smpprintln("onMouseWheel");

    prepareDc(dc);
    point = dc.mapRawToLogicPos(point);

    return D.CallbackResultFlags.None;
}


