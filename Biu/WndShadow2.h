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


#pragma once
#include <map>
#include <gdiplus.h>

class CDrawImage;

class CWndShadow2
{
public:
	CWndShadow2(void);

public:
	virtual ~CWndShadow2(void);

protected:

	// Instance handle, used to register window class and create window 
	static HINSTANCE s_hInstance;

	// Parent HWND and CWndShadow object pares, in order to find CWndShadow in ParentProc()
	static std::map<HWND, CWndShadow2 *> s_Shadowmap;

	// 
	typedef BOOL (WINAPI *pfnUpdateLayeredWindow)(HWND hWnd, HDC hdcDst, POINT *pptDst,
		SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey,
		BLENDFUNCTION *pblend, DWORD dwFlags);
	static pfnUpdateLayeredWindow s_UpdateLayeredWindow;

	HWND m_hWnd;

	LONG m_OriParentProc;	// Original WndProc of parent window

	enum ShadowStatus
	{
		SS_ENABLED = 1,	// Shadow is enabled, if not, the following one is always false
		SS_VISABLE = 1 << 1,	// Shadow window is visible
		SS_PARENTVISIBLE = 1<< 2	// Parent window is visible, if not, the above one is always false
	};
	BYTE m_Status;

	int offset_left;
	int offset_top;
	int offset_right;
	int offset_bottom;

	// Restore last parent window size, used to determine the update strategy when parent window is resized
	LPARAM m_WndSize;

	// Set this to true if the shadow should not be update until next WM_PAINT is received
	bool m_bUpdate;

	COLORREF m_Color;	// Color of shadow

public:
	static bool Initialize(HINSTANCE hInstance);

	void Create(HWND hParentWnd, 
		LPCTSTR lpSkin, 	
		int offset_left,
		int offset_top, 
		int offset_right,
		int offset_bottom);

protected:
	//static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Redraw, resize and move the shadow
	// called when window resized or shadow properties changed, but not only moved without resizing
	void Update(HWND hParent);

	SOUI::ISkinObj*   m_pSkin;
};
