#include "StdAfx.h"
#include "SCefBrowser.h"
#include "CefBrowserWrapper.h"

SCefBrowser::SCefBrowser(void)
{
	 m_pCefBrowserWrapper = NULL;
}

SCefBrowser::~SCefBrowser(void)
{
	if(m_pCefBrowserWrapper)
		delete m_pCefBrowserWrapper ;

	m_pCefBrowserWrapper = NULL;
}

int SCefBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	RECT rctCef={0,0,0,0};
	HWND hParent = GetParent(m_hWnd);
	::GetClientRect(hParent,&rctCef);
	m_pCefBrowserWrapper = new CefBrowserWrapper(m_hWnd);
	m_pCefBrowserWrapper->CreateCef(rctCef);

	return 0;
}

BOOL SCefBrowser::OnInitDialog(HWND wndFocus,LPARAM lInitParam)
{
	SetMsgHandled(FALSE);
	return 0;
}

BOOL SCefBrowser::OnEraseBkgnd(HDC dc)
{
    return TRUE;
}


void SCefBrowser::OnClose()
{
   SetMsgHandled(FALSE); 
}

void SCefBrowser::OnSize(UINT nType, CSize size)
{
	CRect rc;
	GetClientRect(&rc);
    m_pCefBrowserWrapper->AdjuestCef(rc);
}

HRESULT SCefBrowser::onBrowserCreatedMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled)
{

	CRect rc;
	GetClientRect(&rc);
	m_pCefBrowserWrapper->AdjuestCef(rc);
	return S_OK;
}



