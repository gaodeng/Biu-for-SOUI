#include "stdafx.h"
#include "souistd.h"
#include "MenuWndHook2.h"
#include "res.mgr/Sskinpool.h"
#include "core/Sskin.h"
#include <string>
#include "MySkin.h"
#include <sstream>
#include "WndShadow2.h"
#ifdef _WIN64
#define GWL_WNDPROC    GWLP_WNDPROC
#endif//_WIN64

namespace COOL {

	void BiuBiut() {

	}
}

namespace SOUI
{

const TCHAR CoolMenu_oldProc[] = _T("CoolMenu_oldProc");
const TCHAR CoolMenuParent_oldProc[] = _T("CoolMenuParent_oldProc");

#define SM_CXMENUBORDER    3        //默认菜单边框大小

void MakeMenuItemsOwnerDrawn(HMENU hmenu)
{

	int count = GetMenuItemCount(hmenu);
	//count = min(count, 8);
	//count/=2;
	for (int i = 0; i < count; i++)
	{
		MENUITEMINFO mii = { sizeof(mii) };
		mii.fMask = MIIM_FTYPE;
		if (GetMenuItemInfo(hmenu, i, TRUE, &mii))
		{
			if ((mii.fType & MFT_OWNERDRAW) == 0)
			{
				//MENUITEMDATA *pmid = (MENUITEMDATA *)new MENUITEMDATA(hmenu, i);

				//mii.dwItemData = reinterpret_cast<ULONG_PTR>(pmid);


				mii.fType |= MFT_OWNERDRAW;
				SetMenuItemInfo(hmenu, i, TRUE, &mii);

			}

		}
	}





}


BOOL hasSubMenu(HMENU hMenu, UINT id) {

	int count = GetMenuItemCount(hMenu);
	int i;
	for (i = 0; i < count; i++)
	{


		INT itemId = GetMenuItemID(hMenu, i);
		if (itemId == id) {
			return false;
		}
	}


	return true;
}

void ScreenToWndRect(
	__in HWND hWnd,
	__inout LPPOINT lpPoint) {

	CRect wndRect;
	GetWindowRect(hWnd, &wndRect);
	lpPoint->x -= wndRect.left;
	lpPoint->y -= wndRect.top;
}

bool isPopupMenu(HWND hWnd) {

	TCHAR strClassName[10];
	int Count = ::GetClassName(hWnd, strClassName, sizeof(strClassName) / sizeof(strClassName[0]));
	if (Count == 6 || _tcscmp(strClassName, _T("#32768")) == 0)
	{
		return true;
	}
	else {
		return false;
	}
}

void DrawItem1(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC dc(lpDrawItemStruct->hDC);
	CRect rcItem = lpDrawItemStruct->rcItem;
	int nSave = ::SaveDC(dc);
	BOOL bDisabled = lpDrawItemStruct->itemState & ODS_GRAYED;
	BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;
	BOOL bChecked = lpDrawItemStruct->itemState & ODS_CHECKED;
	BOOL bHotLight = lpDrawItemStruct->itemState&ODS_HOTLIGHT;
	BOOL bHover = false;
	BOOL bSep = lpDrawItemStruct->itemID == 0 ? true : false;

	HWND hWnd = WindowFromDC(dc);
	CPoint pt;
	GetCursorPos(&pt);

	////弹出式菜单 rcItem 是 clientRect Top Left 为原点
	////App menu 菜单 rcItem 是 WindowRect Top Left 为原点
	if (hWnd == NULL || isPopupMenu(hWnd)) {
		ScreenToClient(hWnd, &pt);
	}
	else {
		ScreenToWndRect(hWnd, &pt);
	}

	if (rcItem.PtInRect(pt)) {

		bHover = true;
		::OutputDebugString(L"hovvvvvvvvvvvvvvvvvvvv");
	}
	else {
		::OutputDebugString(L"outtttttttttttttttttt");
	}




	HMENU hMenu = (HMENU)lpDrawItemStruct->hwndItem;
	WCHAR    szString[256];
	int ret = GetMenuString((HMENU)lpDrawItemStruct->hwndItem, lpDrawItemStruct->itemID, szString, sizeof(szString), MF_BYCOMMAND);



	BOOL bhasSubMenu = hasSubMenu(hMenu, lpDrawItemStruct->itemID) ? true : false;



	::OutputDebugString(szString);
	::OutputDebugString(L"\n");

	ISkinObj *pSkin = bSep ? GETSKIN(L"menu_sep_red", 100) : GETSKIN(L"menu_item", 100);
	if (!pSkin) return;
	SSkinImgFrame *pBorderSkin = static_cast<SSkinImgFrame*>(pSkin);
	if (!pBorderSkin) return;

	CRect rect(lpDrawItemStruct->rcItem);
	rect.MoveToXY(0, 0);






	CAutoRefPtr<IRenderTarget> pRT;
	GETRENDERFACTORY->CreateRenderTarget(&pRT, rect.Width(), rect.Height());

	if (bSep) {

		ISkinObj *pSkin = GETSKIN(L"menu_item", 100);
		if (!pSkin) return;
		SSkinImgFrame *pBorderSkin = static_cast<SSkinImgFrame*>(pSkin);

		pBorderSkin->Draw(pRT, rect, (bSelected && !bDisabled) ? 1 : 0);

		pSkin = bSep ? GETSKIN(L"menu_sep", 100) : GETSKIN(L"menu_item", 100);
		if (!pSkin) return;
		pBorderSkin = static_cast<SSkinImgFrame*>(pSkin);
		CRect sepRect = rect;
		sepRect.top = rect.top + (rect.bottom - rect.top - 1) / 2;
		sepRect.bottom = sepRect.top + 1;
		pBorderSkin->Draw(pRT, sepRect, (bSelected && !bDisabled) ? 1 : 0);

	}
	else {
		if(hWnd==NULL||isPopupMenu(hWnd)||bHover)
		pBorderSkin->Draw(pRT, rect, ((bHover) && !bDisabled) ? 1 : 0);
		else pRT->FillSolidRect(&rect, RGBA(0,0,0,0));

		//pRT->FillSolidRect(&rect, (COLORREF)0xFF00FFFF);
		if (bhasSubMenu&&isPopupMenu(hWnd)) {
			ISkinObj *pSkin_menu_expand = GETSKIN(L"menu_expand", 150);

			MenuArrowSkin *pmenu_expandSkin = static_cast<MenuArrowSkin*>(pSkin_menu_expand);
			CSize szArrow = pmenu_expandSkin->GetSkinSize();
			CRect arrowRect;
			arrowRect.left = rect.right - szArrow.cx;
			arrowRect.right = rect.right;
			arrowRect.top = (rect.bottom - rect.top - szArrow.cy) / 2;
			arrowRect.bottom = arrowRect.top + szArrow.cy;
			//arrowRect.MoveToY(15);
			pmenu_expandSkin->Draw(pRT, arrowRect, (bSelected) ? 1 : 0);
		}
		CRect textRect = rect;
		
		if (hWnd==NULL||isPopupMenu(hWnd)) {
			textRect.left += 30;
			textRect.right -= 30;
		}
		else {
			//textRect.left += 10;
			//textRect.right -= 10;
		}
		
		std::wstring title = szString;
		COLORREF textColor = bHover ? RGBA(0x66,0x66,0x66,0xff): RGBA(0xff,0xff,0xff,0xff);
		if (hWnd == NULL || isPopupMenu(hWnd)) {
			 textColor = RGBA(0x66, 0x66, 0x66, 0xff);
		}

		if (bDisabled)textColor = 0xffaaaaaa;
		pRT->SetTextColor(textColor);
		CAutoRefPtr<IFont>  m_hFont = SFontPool::getSingleton().GetFont(FF_DEFAULTFONT, 100);
		CAutoRefPtr<IFont> oldFont;
		pRT->SelectObject(m_hFont, (IRenderObj**)&oldFont);
		if (hWnd == NULL || isPopupMenu(hWnd)) {
			pRT->DrawText(title.c_str(), title.length(), &textRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_HIDEPREFIX);

		}
		else {
			pRT->DrawText(title.c_str(), title.length(), &textRect, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS | DT_HIDEPREFIX);

		}


	}




		HDC hmemdc = pRT->GetDC(0);
		BitBlt(dc, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), hmemdc, 0, 0, SRCCOPY);
		pRT->ReleaseDC(hmemdc);

	


	RECT tmpR = lpDrawItemStruct->rcItem;
	::ExcludeClipRect(dc, tmpR.left, tmpR.top, tmpR.right,
		tmpR.bottom);


}

void MeasureItem1(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{


	lpMeasureItemStruct->itemWidth = 300;
	lpMeasureItemStruct->itemHeight = 32;
	if (lpMeasureItemStruct->itemID == 0) {
		lpMeasureItemStruct->itemHeight = 8;
	}
}



void SubClassMenuParent(HWND menu_hwnd) {

}


/*########################################################################
              ------------------------------------------------
                                class CMenuWndHook
              ------------------------------------------------
  ########################################################################*/
SMap <HWND,CMenuWndHook2*> CMenuWndHook2::m_WndMenuMap;

HHOOK CMenuWndHook2::m_hMenuHook = NULL;
SStringW CMenuWndHook2::m_strSkinName;

CMenuWndHook2::CMenuWndHook2 (HWND hWnd)
    : m_hWnd(hWnd)
{

}

CMenuWndHook2::~CMenuWndHook2 ()
{
    WNDPROC oldWndProc = (WNDPROC)::GetProp(m_hWnd, CoolMenu_oldProc);
    if (oldWndProc != NULL)
    {
        ::SetWindowLongPtr(m_hWnd, GWL_WNDPROC, (DWORD)(ULONG)oldWndProc);
        ::RemoveProp(m_hWnd, CoolMenu_oldProc);
    }
    m_WndMenuMap.RemoveKey(m_hWnd);
}

void CMenuWndHook2::InstallHook(HINSTANCE hInst,LPCWSTR pszSkinName)
{
    if (m_hMenuHook == NULL )
    {
        m_hMenuHook = ::SetWindowsHookEx(WH_CALLWNDPROC, WindowHook,
                                         hInst, ::GetCurrentThreadId());
        m_strSkinName=pszSkinName;
    }
}

void CMenuWndHook2::UnInstallHook()
{
    SPOSITION pos= m_WndMenuMap.GetStartPosition();
    while(pos)
    {
        SMap<HWND,CMenuWndHook2*>::CPair *p=m_WndMenuMap.GetNext(pos);
        delete p->m_value;
    }
    m_WndMenuMap.RemoveAll();

    if (m_hMenuHook != NULL)
    {
        ::UnhookWindowsHookEx(m_hMenuHook);
    }
}

CMenuWndHook2* CMenuWndHook2::GetWndHook(HWND hwnd)
{

    SMap<HWND,CMenuWndHook2*>::CPair *p=m_WndMenuMap.Lookup(hwnd);
    if(!p) return NULL;
    return p->m_value;
}

CMenuWndHook2* CMenuWndHook2::AddWndHook(HWND hwnd)
{
    CMenuWndHook2* pWnd = GetWndHook(hwnd);
    if(pWnd) return pWnd;

    pWnd = new CMenuWndHook2(hwnd);
    if (pWnd != NULL)
    {
        m_WndMenuMap[hwnd]=pWnd;
    }
    return pWnd;
}

/*########################################################################
              ------------------------------------------------
                                  消息过程
              ------------------------------------------------
  ########################################################################*/
LRESULT CALLBACK CMenuWndHook2::WindowHook(int code, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT* pStruct = (CWPSTRUCT*)lParam;
	BOOL handled = FALSE;
    while (code == HC_ACTION)
    {
        HWND hWnd = pStruct->hwnd;

		TCHAR strClassName[10];
		int Count = ::GetClassName(hWnd, strClassName, sizeof(strClassName) / sizeof(strClassName[0]));
		


		if (pStruct->message == WM_INITMENUPOPUP)
		{

			if ((Count != 9 || _tcscmp(strClassName, _T("DocHostUI")) != 0)&&( Count != 9 || _tcscmp(strClassName, _T("ViewsMenu")) != 0))
			{
				//break;
			}

			void MakeMenuItemsOwnerDrawn(HMENU hmenu);
			//handled = true;
			HMENU hMenu = (HMENU)pStruct->wParam;
			MakeMenuItemsOwnerDrawn(hMenu);
			HWND menu_parent_hWnd = (hWnd);

			if (menu_parent_hWnd == NULL) {
				break;
			}

			if (::GetProp(menu_parent_hWnd, CoolMenuParent_oldProc) != NULL)
			{
				break;
			}
			AddWndHook(menu_parent_hWnd);

			WNDPROC oldParentWndProc = (WNDPROC)(long)::GetWindowLongPtr(menu_parent_hWnd, GWL_WNDPROC);
			if (oldParentWndProc == NULL)
			{
				break;
			}

			SASSERT(oldParentWndProc != CoolMenuParentProc);
			// 保存到窗口的属性中 ----------------------------------
			if (!SetProp(menu_parent_hWnd, CoolMenuParent_oldProc, oldParentWndProc))
			{
				break;
			}

			// 子类化 ----------------------------------------------
			if (!SetWindowLongPtr(menu_parent_hWnd, GWL_WNDPROC, (DWORD)(ULONG)CoolMenuParentProc))
			{
				::RemoveProp(menu_parent_hWnd, CoolMenuParent_oldProc);
				break;
			}


			break;
		}
		else if (pStruct->message == WM_CREATE || pStruct->message == 0x01E2) {



			




			// 是否为菜单类 ----------------------------------------
			
			if (Count != 6 || _tcscmp(strClassName, _T("#32768")) != 0)
			{
				break;
			}

			// 是否已经被子类化 ------------------------------------
			if (::GetProp(hWnd, CoolMenu_oldProc) != NULL)
			{
				break;
			}

			AddWndHook(pStruct->hwnd);

			// 取得原来的窗口过程 ----------------------------------
			WNDPROC oldWndProc = (WNDPROC)(long)::GetWindowLongPtr(hWnd, GWL_WNDPROC);
			if (oldWndProc == NULL)
			{
				break;
			}

			SASSERT(oldWndProc != CoolMenuProc);
			// 保存到窗口的属性中 ----------------------------------
			if (!SetProp(hWnd, CoolMenu_oldProc, oldWndProc))
			{
				break;
			}

			// 子类化 ----------------------------------------------
			if (!SetWindowLongPtr(hWnd, GWL_WNDPROC, (DWORD)(ULONG)CoolMenuProc))
			{
				::RemoveProp(hWnd, CoolMenu_oldProc);
				break;
			}

			break;







		}


	


		break;

        
    }
    return CallNextHookEx (m_hMenuHook, code, wParam, lParam);
}



LRESULT CALLBACK CMenuWndHook2::CoolMenuParentProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	WNDPROC oldWndProc = (WNDPROC)::GetProp(hWnd, CoolMenuParent_oldProc);
	CMenuWndHook2* pWnd = NULL;
	switch (uMsg)
	{
		case WM_DRAWITEM:
		{
			
			//LRESULT lResult=0;// = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
			if ((pWnd = GetWndHook(hWnd)) != NULL)
			{
				pWnd->DrawItem((LPDRAWITEMSTRUCT)lParam);
				return 0;
			}
			
		}
		break;

		case WM_MEASUREITEM:
		{
			//LRESULT lResult=0;// = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
			if ((pWnd = GetWndHook(hWnd)) != NULL)
			{
				pWnd->MeasureItem((LPMEASUREITEMSTRUCT)lParam);
				return 0;
			}
			
		}
		break;
	}


	return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMenuWndHook2::CoolMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldWndProc = (WNDPROC)::GetProp(hWnd, CoolMenu_oldProc);
    CMenuWndHook2* pWnd = NULL;

    switch (uMsg)
    {
    case WM_CREATE:
        {
            LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
            if ((pWnd = GetWndHook(hWnd)) != NULL)
            {
                lResult = (LRESULT)pWnd->OnCreate((LPCREATESTRUCT)lParam);
				
            }
			
            return lResult;
        }
        break;
    case WM_NCCALCSIZE:
        {
            LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
            if ((pWnd = GetWndHook(hWnd)) != NULL)
            {
                pWnd->OnNcCalcsize((BOOL)wParam,(NCCALCSIZE_PARAMS*)lParam);
            }
            return lResult;
        }
        break;
    case WM_WINDOWPOSCHANGING:
        {
            if ((pWnd = GetWndHook(hWnd)) != NULL)
            {
                pWnd->OnWindowPosChanging((LPWINDOWPOS)lParam);
            }
        }
        break;
    case WM_WINDOWPOSCHANGED:
        {
            LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
            if ((pWnd = GetWndHook(hWnd)) != NULL)
            {
                pWnd->OnWindowPosChanged();
            }
            return lResult;
        }
        break;
    case WM_PRINT:
        {
            LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
            if ((pWnd = GetWndHook(hWnd)) != NULL)
            {
                pWnd->OnPrint((HDC)wParam);
            }
            return lResult;
        }
        break;
    case WM_NCPAINT:
        {
            if ((pWnd = GetWndHook(hWnd)) != NULL)
            {
                pWnd->OnNcPaint();
                return 0;
            }
        }
        break;
    case WM_NCDESTROY:
        {
            if ((pWnd = GetWndHook(hWnd)) != NULL)
            {
                pWnd->OnNcDestroy();
            }
        }
        break;

	case WM_APP:
		if (lParam == (LPARAM)999) // Just making sure it is our special message 
		{
			HMENU hMenu = (HMENU)SendMessage((HWND)hWnd, MN_GETHMENU, 0, 0);
			if (hMenu)
			{

				
				//InsertMenu(hMenu, 0, MF_STRING | MFT_OWNERDRAW, 666, L"Hello SO 中华人民共和国");
				MakeMenuItemsOwnerDrawn(hMenu);
			}
			////////////////////////////////////////////////menu parent ////////////////

			
			extern HWND g_main_hwnd;
			
			



		}
		break;

	
    }
    return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

/*########################################################################
              ------------------------------------------------
                                消息处理函数
              ------------------------------------------------
  ########################################################################*/
int CMenuWndHook2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetClassLong(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE) & ~CS_DROPSHADOW);
	//SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(m_hWnd, 255, 200, LWA_ALPHA);
	
	CWndShadow2 *shadow=new CWndShadow2();
	shadow->Create(m_hWnd, L"menu_shadow", 26, 26, 26, 26);

    return 0;
}

void CMenuWndHook2::OnWindowPosChanging(WINDOWPOS *pWindowPos)
{
	
    if(m_strSkinName.IsEmpty()) return;
    ISkinObj *pSkin=GETSKIN(m_strSkinName,100);
    if(!pSkin || !pSkin->IsClass(SSkinImgFrame::GetClassName())) return;
    SSkinImgFrame *pBorderSkin=static_cast<SSkinImgFrame*>(pSkin);
    if(!pBorderSkin) return;
    pWindowPos->cx += pBorderSkin->GetMargin().left+pBorderSkin->GetMargin().right-SM_CXMENUBORDER*2;
    pWindowPos->cy += pBorderSkin->GetMargin().top+pBorderSkin->GetMargin().bottom-SM_CXMENUBORDER*2;
}

void CMenuWndHook2::OnNcCalcsize(BOOL bValidCalc,NCCALCSIZE_PARAMS* lpncsp)
{
	
    if(m_strSkinName.IsEmpty()) return;
    ISkinObj *pSkin=GETSKIN(m_strSkinName,100);
    if(!pSkin || !pSkin->IsClass(SSkinImgFrame::GetClassName())) return;
    SSkinImgFrame *pBorderSkin=static_cast<SSkinImgFrame*>(pSkin);
    if(!pBorderSkin) return;

    lpncsp->rgrc[0].left=lpncsp->lppos->x+pBorderSkin->GetMargin().left;
    lpncsp->rgrc[0].top=lpncsp->lppos->y+pBorderSkin->GetMargin().top;
    lpncsp->rgrc[0].right=lpncsp->lppos->x+lpncsp->lppos->cx-pBorderSkin->GetMargin().right;
    lpncsp->rgrc[0].bottom=lpncsp->lppos->y+lpncsp->lppos->cy-pBorderSkin->GetMargin().bottom;
}

void CMenuWndHook2::OnNcPaint()
{
	
    HDC dc=GetWindowDC(m_hWnd);
    OnPrint(dc);
    ReleaseDC(m_hWnd,dc);
}

void CMenuWndHook2::OnPrint(HDC dc)
{

	
    if(m_strSkinName.IsEmpty()) return;
    ISkinObj *pSkin=GETSKIN(m_strSkinName,100);
    if(!pSkin) return;
    SSkinImgFrame *pBorderSkin=static_cast<SSkinImgFrame*>(pSkin);
    if(!pBorderSkin) return;

    CRect rcClient;
    GetClientRect(m_hWnd,&rcClient);
    ClientToScreen(m_hWnd,(LPPOINT)&rcClient);
    ClientToScreen(m_hWnd,((LPPOINT)&rcClient)+1);
    CRect rcWnd;
    GetWindowRect(m_hWnd, &rcWnd);
    rcClient.OffsetRect(-rcWnd.TopLeft());
    
    int nSave = ::SaveDC(dc);
    ::ExcludeClipRect(dc,rcClient.left,rcClient.top,rcClient.right,rcClient.bottom);
    rcWnd.MoveToXY(0,0);
    
    CAutoRefPtr<IRenderTarget> pRT;
    GETRENDERFACTORY->CreateRenderTarget(&pRT,rcWnd.Width(),rcWnd.Height());
    pBorderSkin->Draw(pRT,rcWnd,0);
    HDC hmemdc=pRT->GetDC(0);
    ::BitBlt(dc,0,0,rcWnd.Width(),rcWnd.Height(),hmemdc,0,0,SRCCOPY);
    pRT->ReleaseDC(hmemdc);
    ::RestoreDC(dc,nSave);


}

void CMenuWndHook2::OnNcDestroy()
{
    delete this;
}

//不能设计窗口半透明，设置区域后，非客户区位置发生改变，不明白原因。
void CMenuWndHook2::OnWindowPosChanged()
{

	/*
    CRect rcWnd;
    GetWindowRect(m_hWnd,&rcWnd);
	
    rcWnd.MoveToXY(0,0);
	rcWnd.InflateRect(-0, -3);
    HRGN hRgn = CreateRoundRectRgn(rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,0,0);
    SetWindowRgn(m_hWnd,hRgn,FALSE);
    DeleteObject(hRgn);*/
    
}



void CMenuWndHook2::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
	DrawItem1(0, lpDrawItemStruct);
}

void CMenuWndHook2::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	MeasureItem1(0, lpMeasureItemStruct);
}

}//namespace SOUI