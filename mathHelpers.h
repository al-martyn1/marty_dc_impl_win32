#pragma once

#include <cmath>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace math_helpers {



//----------------------------------------------------------------------------
//static const double mathPi = 3.1415926;
static const double mathPi = 3.14159265359;

//----------------------------------------------------------------------------
inline
double angleToRadians( double graduses )
{
    return graduses*mathPi/180.0;
}

//----------------------------------------------------------------------------
inline
double angleToDegrees( double radians )
{
    return radians*180.0/mathPi;
}

//----------------------------------------------------------------------------
inline
bool calcVectorAngle(double x, double y, double &angle, double *pVectorLen = 0)
{
    // double x = (double)vec.x;
    // double y = (double)vec.y;

    // if (x<0.000001 && x>-0.000001)
    // {
    // }

    double r = std::sqrt(x*x + y*y);

    if (r<0.000001)
        return false; // очень маленький радиус,ничего не произойдёт (не будет нарисовано). А чтобы не делить на 0, просто вернём false

    if (pVectorLen)
    {
        *pVectorLen = r;
    }

    double startCos = x / r;

    // Косинус - это проекция единичного вектора на ось X

    // Четверти/квадранты
    //               90 (pi/2)
    //           2              1
    // 180 (pi)
    //           3              4
    //              270 (3*pi/2)

    // Вычисляем для первой четверти
    if (startCos<0)
    {
        startCos = -startCos;
    }


    double startAngle1 = std::acos(startCos);

    if (x>=0.0)
    {
        // Первая или четвертая четверть

        if (y>=0.0)
        {
            // Первая четверть
            angle = startAngle1;
        }
        else
        {
            // Четвертая четверть
            angle = 2*mathPi-startAngle1;
        }

    }
    else
    {
        // Вторая или третья четверть

        if (y>=0.0)
        {
            // Вторая четверть
            angle = mathPi-startAngle1;
        }
        else
        {
            // Третья четверть
            angle = mathPi+startAngle1;
        }

    }

    return true;

}

//----------------------------------------------------------------------------
//! Приводим в диапазон +360 - -360
inline
double normalizeAngleDegreesMod360( double degrees )
{
    while(degrees >= 360.0)
    {
        degrees -= 360.0;
    }

    while(degrees <= -360.0)
    {
        degrees += 360.0;
    }

    return degrees;

}

//----------------------------------------------------------------------------
//! Приводим в диапазон 0 - +360
inline
double normalizeAngleDegrees360( double degrees )
{
    while(degrees >= 360.0)
    {
        degrees -= 360.0;
    }

    while(degrees < 0.0)
    {
        degrees += 360.0;
    }

    return degrees;

}

//----------------------------------------------------------------------------
inline
double normalizeAngleDegreesMod180( double degrees )
{
    degrees = normalizeAngleDegreesMod360(degrees);

    // Бывает так: 180.00000000000003
    // 180.000 000 000 000 030 - в 14ом знаке проблема
    while(degrees > 180.000001) // до 6 знака делаем
    {
        degrees = degrees - 360.0;
    }

    while(degrees <= -180.0)
    {
        degrees = degrees + 360.0;
    }

    return degrees;

}

//----------------------------------------------------------------------------
inline
double normalizeAngleDegreesComplement360( double degrees )
{
    // degrees = normalizeAngleDegreesMod360(degrees);
    //
    // if (degrees<0)
    // {
    //     return 360+degrees;
    // }
    // else // >= 0
    // {
    //     return 360-degrees;
    // }

    degrees = normalizeAngleDegreesMod360(degrees);

    return 360-degrees;

}




} // namespace math_helpers

