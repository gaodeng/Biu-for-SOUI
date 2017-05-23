#include "StdAfx.h"
#include "CefRealWndHandler.h"
#include "SCefBrowser.h"



HMENU hRoot;
#define IDM_OPT1     301  
#define IDM_OPT2     302  
void CreateMyMenu()
{
	hRoot = CreateMenu();
	if (!hRoot)
		return;
	HMENU pop1 = CreatePopupMenu();
	AppendMenu(hRoot,
		MF_POPUP,
		(UINT_PTR)pop1,
		L"操作");
	// 一种方法是使用AppendMenu函数  
	AppendMenu(pop1,
		MF_STRING,
		IDM_OPT1,
		L"飞机");

	// 另一种方法是使用InsertMenuItem函数  
	MENUITEMINFO mif;
	mif.cbSize = sizeof(MENUITEMINFO);
	mif.cch = 100;
	mif.dwItemData = NULL;
	mif.dwTypeData = L"机关枪";
	mif.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE;
	mif.fState = MFS_ENABLED;
	mif.fType = MIIM_STRING;
	mif.wID = IDM_OPT2;

	InsertMenuItem(pop1, IDM_OPT2, FALSE, &mif);

}



CCefRealWndHandler::CCefRealWndHandler(void)
{
}

CCefRealWndHandler::~CCefRealWndHandler(void)
{
}

HWND g_appMenuBar_hWnd;
HWND CCefRealWndHandler::OnRealWndCreate(SRealWnd *pRealWnd)
{
       const SRealWndParam&  param = pRealWnd->GetRealWndParam();
	   if(param.m_strClassName == _T("cef_cls"))
	   {
		   HWND HostWnd = pRealWnd->GetContainer()->GetHostHwnd();
		   SCefBrowser* pSCefBrowser = new SCefBrowser;    
		   HWND hCefWnd = pSCefBrowser->Create(L"cef", WS_CHILD |WS_VISIBLE,NULL,0,0,0,0,HostWnd,NULL);
		   return pSCefBrowser->m_hWnd;
	   }
	   
	   return 0;
}

void CCefRealWndHandler::OnRealWndDestroy(SRealWnd *pRealWnd)
{
	const SRealWndParam &param=pRealWnd->GetRealWndParam();
	if(param.m_strClassName==_T("cef_cls"))
	{
		SCefBrowser *pSCefBrowser=(SCefBrowser*) pRealWnd->GetData();
		if(pSCefBrowser)
		{
			pSCefBrowser->DestroyWindow();
			delete pSCefBrowser;
		}
	}
	
}

BOOL CCefRealWndHandler::OnRealWndInit(SRealWnd *pRealWnd)
{
     return FALSE;
}

BOOL CCefRealWndHandler::OnRealWndSize(SRealWnd *pRealWnd)
{
	if(!pRealWnd)
		return FALSE;


	
	return FALSE;


	


}

BOOL CCefRealWndHandler::OnRealWndPosition(SRealWnd *pRealWnd, const CRect &rcWnd) {

	
	return FALSE;



}