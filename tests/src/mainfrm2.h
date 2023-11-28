// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

#pragma once

#include <atlcrack.h>
#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/cmd_line.h"
#include "umba/filename.h"



class CMainFrame : public CFrameWindowImpl<CMainFrame>
                 , public CUpdateUI<CMainFrame>
                 , public CMessageFilter
                 , public CIdleHandler
                 //, public CPrintJobInfo
{
    unsigned m_showCounter   = 0;
    //bool     m_bFullScreenToggled = false;
    bool m_bFullScreen     = false;

    LONG m_savedWndStyle   = 0;
    LONG m_savedWndStyleEx = 0;
    RECT m_savedWndRect    = {0,0,0,0};

    UINT_PTR m_fullScreenStartupTimerId = 1;

public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    CMainFrame(const CMainFrame&) = delete;
    CMainFrame(CMainFrame&&) = delete;

    CBitmapView m_view;


    CMainFrame()
    {
        m_view.toggleFullScreen = [&]()
        {
            this->toggleFullScreen();
        };
    
    }

    ~CMainFrame()
    {
    }

    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
            return TRUE;

        return m_view.PreTranslateMessage(pMsg);
    }

    virtual BOOL OnIdle()
    {
        return FALSE;
    }

    #include "umba/warnings/push_disable_signed_unsigned_mismatch.h"
    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)

        //MSG_WM_TIMER(OnTimer)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)
        MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
        MSG_WM_EXITSIZEMOVE(OnExitSizeMove)

        // Вот тут F11 и обрабатываем
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_KEYUP(OnKeyUp)
        //

        //MSG_WM_CHAR(OnChar)
        //MSG_WM_DEADCHAR(OnDeadChar)
        // // MSG_WM_SYSKEYDOWN(OnSysKeyDown)
        // // MSG_WM_SYSKEYUP(OnSysKeyUp)
        // MSG_WM_SYSCHAR(OnSysChar)
        // MSG_WM_SYSDEADCHAR(OnSysDeadChar)

        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)

    END_MSG_MAP()
    #include "umba/warnings/pop.h"

    BEGIN_UPDATE_UI_MAP(CMainFrame)
    END_UPDATE_UI_MAP()

    int OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
    {
        SetMenu(NULL);
        CreateSimpleStatusBar();

        // create view window
        m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE );
        //m_view.SetBitmap(NULL);

        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
        pLoop->AddIdleHandler(this);

        // Иначе клавиатурные сообщения попадают в дочернее окно
        //::EnableWindow( m_hWndClient, FALSE );
        //m_view.EnableWindow(FALSE);

        return 0;
    }


    void OnShowWindow(BOOL bShow, UINT nStatus)
    {
        MARTY_ARG_USED(nStatus);
        // SYSINFO_LOG() << "OnShowWindow\n";

        SetMsgHandled(FALSE);

        if (bShow)
        {
            if (!m_showCounter) // Первый показ окна
            {
            }

            ++m_showCounter;
        }

    }

    void OnTimer(UINT_PTR nIDEvent)
    {
        if (nIDEvent==m_fullScreenStartupTimerId)
        {
            KillTimer(m_fullScreenStartupTimerId);
            goFullScreen(true); // Отложенный переход в полноэкранный режим
        }
    }

    bool isSysKeyRegularKey() const
    {
        return true;
    }

    bool getAsyncKeyState(int vKey, bool *pFlagPressed = 0) const
    {
        SHORT res = GetAsyncKeyState(vKey);

        if (pFlagPressed) // pressed after the previous call to GetAsyncKeyState
        {
            if (res&1)
                *pFlagPressed = true;
            else
                *pFlagPressed = false;
        }

        res &= ~(SHORT)1; // 0xFFFE; // Сбросили младший бит

        // Старший бит даёт текущее состояние кнопки

        return res ? true : false;
    }

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nRepCnt);

        if (nChar==VK_F11)
        {
            auto prevDown = CRACK_ON_KEY_UPDOWN_FLAGS_GET_PREV_DOWN_STATE_FLAG(nFlags);
            if (!prevDown)
            {
                SetMsgHandled(TRUE);
                toggleFullScreen();
            }
        }
        else
        {
            SetMsgHandled(FALSE);
            //SendMessage(m_hWnd, message, wParam, lParam);
        }
    }

    void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        //SetMsgHandled(TRUE);
        SetMsgHandled(FALSE);
    }

    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        SetMsgHandled(FALSE);
    }

    void OnDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        SetMsgHandled(FALSE);
    }


    void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        SetMsgHandled(FALSE);
    }

    void OnSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        MARTY_ARG_USED(nChar);
        MARTY_ARG_USED(nRepCnt);
        MARTY_ARG_USED(nFlags);

        SetMsgHandled(FALSE);
    }

    void saveWindowStyles()
    {
        m_savedWndStyle   = (LONG)::GetWindowLongW(m_hWnd, GWL_STYLE  );
        m_savedWndStyleEx = (LONG)::GetWindowLongW(m_hWnd, GWL_EXSTYLE);
    }

    void toggleFullScreen()
    {
        goFullScreen(!m_bFullScreen);
    }

    HWND findStatusBar() const
    {
        // HWND hwndParent = ::GetParent(m_hWnd);

        // ID status bar 0x0000E801
        // class is "msctls_statusbar32"

        //return ::FindWindowExA(hwndParent, 0, "msctls_statusbar32", 0);
        return ::FindWindowExA(m_hWnd, 0, "msctls_statusbar32", 0);
    }

    void goFullScreen( bool bFullscreen )
    {
        if (m_bFullScreen==bFullscreen)
        {
            // Уже в нужном режиме
            return;
        }

        HWND hwndStatusBar = findStatusBar();
        ::ShowWindow( hwndStatusBar, bFullscreen ? SW_HIDE : SW_SHOW );

        // https://stackoverflow.com/questions/2382464/win32-full-screen-and-hiding-taskbar
        if (bFullscreen)
        {
            // Переходим в полноэкранный режим
            saveWindowStyles();

            ::GetWindowRect(m_hWnd, &m_savedWndRect );

            // Заранее ставим признак полноэкранного режима, чтобы не было пересчета положения окна
            m_bFullScreen = true;

            // Set new window style and size.

            ::SetWindowLong(m_hWnd, GWL_STYLE  , m_savedWndStyle   & ~(WS_CAPTION | WS_THICKFRAME));
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, m_savedWndStyleEx & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE) );

            MONITORINFO monitorInfo;
            monitorInfo.cbSize = sizeof(monitorInfo);
            GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);

            const RECT &rcMonitor = monitorInfo.rcMonitor;

            ::SetWindowPos( m_hWnd, 0
                          , rcMonitor.left, rcMonitor.top
                          , rcMonitor.right-rcMonitor.left, rcMonitor.bottom-rcMonitor.top
                          , SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
                          );
        }
        else
        {
            m_bFullScreen = false;
            ::SetWindowLong(m_hWnd, GWL_STYLE  , m_savedWndStyle   );
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, m_savedWndStyleEx );

            auto wndWidth  = m_savedWndRect.right-m_savedWndRect.left + 1;
            auto wndHeight = m_savedWndRect.bottom-m_savedWndRect.top + 1;

            ::SetWindowPos( m_hWnd, 0
                          , m_savedWndRect.left, m_savedWndRect.top
                          , wndWidth, wndHeight
                          , SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
                          );

        }

        
        

    }

    void OnWindowPosChanging(LPWINDOWPOS lpWndPos)
    {
        MARTY_ARG_USED(lpWndPos);

        SetMsgHandled(FALSE);
        return;
    }

    void OnWindowPosChanged(LPWINDOWPOS lpWndPos)
    {
        MARTY_ARG_USED(lpWndPos);
        SetMsgHandled(FALSE);
    }

    void OnExitSizeMove()
    {
        SetMsgHandled(FALSE);

        //underwood::SysInfoImplWin32::init(m_hWnd);

        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-windowpos

        // underwood::SysInfoImplWin32::init(m_hWnd);
        // if (!m_bFullScreen)
        // {
        //     underwood::ISysInfo *pSysInfo = underwood::ISysInfo::getSysInfoPtr();
        //     underwood::tryAdjustAppMainWindowPosSize(pSysInfo);
        // }
    }

};

#endif // __MAINFRM_H__
