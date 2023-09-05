// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

#pragma once

#define FILE_MENU_POSITION    0
#define RECENT_MENU_POSITION    6
#define POPUP_MENU_POSITION    0

LPCTSTR g_lpcstrMRURegKey = _T("Software\\Microsoft\\WTL Samples\\TestDrawContext");
LPCTSTR g_lpcstrApp = _T("TestDrawContext");


class CMainFrame : public CFrameWindowImpl<CMainFrame>
                 , public CUpdateUI<CMainFrame>
                 , public CMessageFilter
                 , public CIdleHandler
                 //, public CPrintJobInfo
{
public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    CMainFrame(const CMainFrame&) = delete;
    CMainFrame(CMainFrame&&) = delete;

    CBitmapView m_view;


    CMainFrame() // : m_bPrintPreview(false)
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

    virtual bool IsValidPage(UINT nPage)
    {
        return (nPage == 0);    // we have only one page
    }

    BEGIN_MSG_MAP_EX(CMainFrame)
        MSG_WM_CREATE(OnCreate)
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

    BEGIN_UPDATE_UI_MAP(CMainFrame)
    END_UPDATE_UI_MAP()

    int OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
    {
        SetMenu(NULL);
        CreateSimpleStatusBar();

        // create view window
        m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
        m_view.SetBitmap(NULL);

        CMessageLoop* pLoop = _Module.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
        pLoop->AddIdleHandler(this);

        return 0;
    }

};

#endif // __MAINFRM_H__

