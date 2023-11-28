// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MAINFRM_H__
#define __MAINFRM_H__

#pragma once


#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/cmd_line.h"
#include "umba/filename.h"


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


    CMainFrame()
    {
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

        std::string exeFullName = umba::program_location::getExeName<std::string>();
        std::string exeName     = umba::filename::getName(exeFullName);
        ::SetWindowTextA(m_hWnd, exeName.c_str());

        return 0;
    }

};

#endif // __MAINFRM_H__

