#pragma once
namespace SOUI
{
	class MenuArrowSkin : public SSkinImgList
	{
	public:

		SOUI_CLASS_NAME(MenuArrowSkin, L"menuarrow")
		MenuArrowSkin();
		~MenuArrowSkin();


		virtual SIZE GetSkinSize();
		virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha);
	};

}

