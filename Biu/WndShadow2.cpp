// WndShadow.h : header file
//
// Version 0.1
//
// Copyright (c) 2006 Perry Zhu, All Rights Reserved.
//
// mailto:perry@live.com
//
//
// This source file may be redistributed unmodified by any means PROVIDING 
// it is NOT sold for profit without the authors expressed written 
// consent, and providing that this notice and the author's name and all 
// copyright notices remain intact. This software is by no means to be 
// included as part of any third party components library, or as part any
// development solution that offers MFC extensions that are sold for profit. 
// 
// If the source code is used in any commercial applications then a statement 
// along the lines of:
// 
// "Portions Copyright (c) 2006 Perry Zhu" must be included in the "Startup 
// Banner", "About Box" or "Printed Documentation". This software is provided 
// "as is" without express or implied warranty. Use it at your own risk! The 
// author accepts no liability for any damage/loss of business that this 
// product may cause.
//
/////////////////////////////////////////////////////////////////////////////
//****************************************************************************

#include "StdAfx.h"
#include "WndShadow2.h"

#include <math.h>
#include <crtdbg.h>

// Some extra work to make this work in VC++ 6.0

// walk around the for iterator scope bug of VC++6.0
#ifdef _MSC_VER
#if _MSC_VER == 1200
#define for if(false);else for
#endif
#endif

// Some definitions for VC++ 6.0 without newest SDK
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif

#ifndef ULW_ALPHA
#define ULW_ALPHA 0x00000002
#endif

CWndShadow2::pfnUpdateLayeredWindow CWndShadow2::s_UpdateLayeredWindow = NULL;

const TCHAR *strWndClassName = _T("PerryShadowWnd");

HINSTANCE CWndShadow2::s_hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;

std::map<HWND, CWndShadow2 *> CWndShadow2::s_Shadowmap;

CWndShadow2::CWndShadow2(void)
: m_hWnd((HWND)INVALID_HANDLE_VALUE)
, m_OriParentProc(NULL)
, offset_left(0)
, offset_top(0)
, offset_right(0)
, offset_bottom(0)
, m_WndSize(0)
, m_bUpdate(false)
, m_pSkin(NULL)
{
}

CWndShadow2::~CWndShadow2(void)
{
}

bool CWndShadow2::Initialize(HINSTANCE hInstance)
{
	// Should not initiate more than once
	if (NULL != s_UpdateLayeredWindow)
		return false;

	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	s_UpdateLayeredWindow = 
		(pfnUpdateLayeredWindow)GetProcAddress(hUser32, 
		"UpdateLayeredWindow");
	
	// If the import did not succeed, make sure your app can handle it!
	if (NULL == s_UpdateLayeredWindow)
		return false;

	// Store the instance handle
	s_hInstance = hInstance;

	//

	// Register window class for shadow window
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= DefWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= strWndClassName;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);

	return true;
}

void CWndShadow2::Create(HWND hParentWnd, 
						 LPCTSTR lpSkin,
						 int offset_left,
						 int offset_top, 
						 int offset_right,
						 int offset_bottom)
{
	this->offset_left = offset_left;
	this->offset_top = offset_top;
	this->offset_right = offset_right;
	this->offset_bottom = offset_bottom;

	// Do nothing if the system does not support layered windows
	if(NULL == s_UpdateLayeredWindow)
		return;
	
	using namespace SOUI;
	m_pSkin = GETSKIN(lpSkin,100);

	// m_pShadowDrawImage->InitDrawImage();
	// m_pShadowDrawImage->LoadResImage(_T("Shadow.PNG"), s_hInstance, _T("PIC"));

	// Already initialized
	_ASSERT(s_hInstance != INVALID_HANDLE_VALUE);

	// Add parent window - shadow pair to the map
	_ASSERT(s_Shadowmap.find(hParentWnd) == s_Shadowmap.end());	// Only one shadow for each window
	s_Shadowmap[hParentWnd] = this;

	// Create the shadow window
	m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, strWndClassName, NULL,
		/*WS_VISIBLE | WS_CAPTION |*/ WS_POPUPWINDOW,
		CW_USEDEFAULT, 0, 0, 0, hParentWnd, NULL, s_hInstance, NULL);
	::SetWindowPos(m_hWnd, hParentWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE| SWP_NOACTIVATE);
	
	// Determine the initial show state of shadow according to parent window's state
	LONG lParentStyle = GetWindowLong(hParentWnd, GWL_STYLE);
	if(!(WS_VISIBLE & lParentStyle))	// Parent invisible
		m_Status = SS_ENABLED;
	else if((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle)	// Parent visible but does not need shadow
		m_Status = SS_ENABLED | SS_PARENTVISIBLE;
	else	// Show the shadow
	{
		m_Status = SS_ENABLED | SS_VISABLE | SS_PARENTVISIBLE;
		::ShowWindow(m_hWnd, SW_SHOWNA);
		Update(hParentWnd);
	}
	
	// Replace the original WndProc of parent window to steal messages
	m_OriParentProc = GetWindowLong(hParentWnd, GWL_WNDPROC);

#pragma warning(disable: 4311)	// temporrarily disable the type_cast warning in Win32
	SetWindowLong(hParentWnd, GWL_WNDPROC, (LONG)ParentProc);
#pragma warning(default: 4311)

}

LRESULT CALLBACK CWndShadow2::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndShadow2 *pThis = s_Shadowmap[hwnd];
	LONG lParentStyle1 = GetWindowLong(hwnd, GWL_STYLE);
	switch(uMsg)
	{
	case WM_MOVE:
		::OutputDebugString(L"-------------WM_MOVE");
		
		if (WS_VISIBLE & lParentStyle1) {
			::OutputDebugString(L"-------------visible");
		}
		if (pThis != NULL)
		{
			if (pThis->m_Status & SS_VISABLE)
			{
				RECT WndRect;
				GetWindowRect(hwnd, &WndRect);
				
				SetWindowPos(pThis->m_hWnd, 0, WndRect.left - pThis->offset_left, WndRect.top - pThis->offset_top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
			}

			if (WS_VISIBLE & lParentStyle1)
				pThis->m_Status |= SS_PARENTVISIBLE;
			if (!(pThis->m_Status & SS_VISABLE)&&(WS_VISIBLE & lParentStyle1))
			{
				
				::SetWindowPos(pThis->m_hWnd, hwnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

				pThis->m_Status |= SS_VISABLE;
				// Update before show, because if not, restore from maximized will  
				// see a glance misplaced shadow  
				pThis->Update(hwnd);
				ShowWindow(pThis->m_hWnd, SW_SHOWNA);
				// If restore from minimized, the window region will not be updated until WM_PAINT:(  
				pThis->m_bUpdate = true;
			}
		}
		break;

	case WM_SIZE:
		::OutputDebugString(L"-------------WM_SIZE");
		
		if (WS_VISIBLE & lParentStyle1) {
			::OutputDebugString(L"-------------visible");
		}
		if (pThis != NULL)
		{
			::OutputDebugString(L"-------------tttttt2");
			if(pThis->m_Status & SS_ENABLED)
			{
				::OutputDebugString(L"-------------tttttt3");
				LONG lParentStyle = GetWindowLong(hwnd, GWL_STYLE);
				if(WS_VISIBLE & lParentStyle)
					pThis->m_Status |= SS_PARENTVISIBLE;
				if(SIZE_MAXIMIZED == wParam || SIZE_MINIMIZED == wParam)
				{
					::ShowWindow(pThis->m_hWnd, SW_HIDE);
					pThis->m_Status &= ~SS_VISABLE;
				}
				else if(pThis->m_Status & SS_PARENTVISIBLE)	// Parent maybe resized even if invisible
				{
					::OutputDebugString(L"-------------tttttt4");
					
					// Awful! It seems that if the window size was not decreased
					// the window region would never be updated until WM_PAINT was sent.
					// So do not Update() until next WM_PAINT is received in this case
					if(LOWORD(lParam) > LOWORD(pThis->m_WndSize) || HIWORD(lParam) > HIWORD(pThis->m_WndSize))
						pThis->m_bUpdate = true;
					else
						pThis->Update(hwnd);
					if(!(pThis->m_Status & SS_VISABLE))
					{
						pThis->m_Status |= SS_VISABLE;
						// Update before show, because if not, restore from maximized will  
						// see a glance misplaced shadow  
						pThis->Update(hwnd);
						ShowWindow(pThis->m_hWnd, SW_SHOWNA);
						// If restore from minimized, the window region will not be updated until WM_PAINT:(  
						pThis->m_bUpdate = true;
					}
				}
				pThis->m_WndSize = lParam;
			}
		}
		break;

	case WM_PAINT:
		{
			if (pThis != NULL)
			{
				if(pThis->m_bUpdate)
				{
					pThis->Update(hwnd);
					pThis->m_bUpdate = false;
				}
			}
			break;
		}

		// In some cases of sizing, the up-right corner of the parent window region would not be properly updated
		// Update() again when sizing is finished
	case WM_EXITSIZEMOVE:
		::OutputDebugString(L"-------------WM_EXITSIZEMOVE");
		
		if (WS_VISIBLE & lParentStyle1) {
			::OutputDebugString(L"-------------visible");
		}
		if (pThis != NULL)
		{
			if(pThis->m_Status & SS_VISABLE)
			{
				pThis->Update(hwnd);
			}
		}
		break;

	case WM_SHOWWINDOW:
		::OutputDebugString(L"-------------WM_SHOWWINDOW");

		if (WS_VISIBLE & lParentStyle1) {
			::OutputDebugString(L"-------------visible");
		}
		if (pThis != NULL)
		{
			if(pThis->m_Status & SS_ENABLED)
			{
				if(!wParam)	// the window is being hidden
				{
					::ShowWindow(pThis->m_hWnd, SW_HIDE);
					pThis->m_Status &= ~(SS_VISABLE | SS_PARENTVISIBLE);
				}
				else if(!(pThis->m_Status & SS_PARENTVISIBLE))
				{
					//pThis->Update(hwnd);
					pThis->m_bUpdate = true;
					::ShowWindow(pThis->m_hWnd, SW_SHOWNA);
					pThis->m_Status |= SS_VISABLE | SS_PARENTVISIBLE;
				}
			}
		}
		break;

	case WM_DESTROY:
		if (pThis != NULL)
		{
			DestroyWindow(pThis->m_hWnd);	// Destroy the shadow
		}
		break;
		
	case WM_NCDESTROY:
		if (pThis != NULL)
		{
			s_Shadowmap.erase(hwnd);	// Remove this window and shadow from the map
		}
		break;
	}

	//::OutputDebugString(L"\n");
#pragma warning(disable: 4312)	// temporrarily disable the type_cast warning in Win32
	// Call the default(original) window procedure for other messages or messages processed but not returned
	return ((WNDPROC)pThis->m_OriParentProc)(hwnd, uMsg, wParam, lParam);
#pragma warning(default: 4312)

}

void CWndShadow2::Update(HWND hParent)
{
	if (!m_pSkin) {	
		return ;
	}

	CRect WndRect;
	GetWindowRect(hParent, &WndRect);


	
	//
	int nParentWndWidth = WndRect.right - WndRect.left;
	int nParentWndHeight = WndRect.bottom - WndRect.top;

	int nShadWndWid = nParentWndWidth + offset_left + offset_right;
	int nShadWndHei = nParentWndHeight + offset_top + offset_bottom;

	SOUI::IRenderTarget* RenderTarget;
	GETRENDERFACTORY->CreateRenderTarget(&RenderTarget, nShadWndWid, nShadWndHei);

	RECT rcDraw = {0, 0, nShadWndWid, nShadWndHei};
	m_pSkin->Draw(RenderTarget, &rcDraw, 0);

	POINT ptDst = {WndRect.left- offset_left, WndRect.top - offset_top};
	POINT ptSrc = {0, 0};
	SIZE WndSize = {nShadWndWid, nShadWndHei};
	BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	SetWindowPos(m_hWnd, NULL, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);

	BOOL bRet= s_UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &WndSize, RenderTarget->GetDC(),
		&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);


	RenderTarget->Release();
}