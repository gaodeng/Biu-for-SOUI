#include "stdafx.h"
#include "MySkin.h"
namespace SOUI
{

	MenuArrowSkin::MenuArrowSkin()
	{
	}


	MenuArrowSkin::~MenuArrowSkin()
	{
	}
	SIZE MenuArrowSkin::GetSkinSize()
	{
		SIZE ret = { 0, 0 };
		if (m_pImg) ret = m_pImg->Size();
		ret.cx = MulDiv(20,GetScale(),100);
		return ret;
	}

	void MenuArrowSkin::_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha)
	{
		if (!m_pImg) return;
		
		SIZE sz = m_pImg->Size();
		RECT rcSrc = { 0,0,sz.cx,sz.cy };
		RECT dstRect = *rcDraw;
		dstRect.right = dstRect.left + sz.cx;
		//dstRect.bottom = dstRect.top + sz.cy;
		pRT->DrawBitmapEx(&dstRect, m_pImg, &rcSrc, GetExpandMode(), byAlpha);
	}
}