/*! \file
    \brief тесты биндинга сквирела
*/

#define TESTWINDRAWCONTEXT 2

#include "TestDrawContextCommon.cpp"


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
{
    return mainImpl(hInstance, hPrevInstance, lpstrCmdLine, nCmdShow);
}

