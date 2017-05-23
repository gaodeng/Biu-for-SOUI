#pragma once

class CefBrowserWrapper
{
public:
	CefBrowserWrapper(HWND parent) :parent_window_(parent) {}
	void CreateCef(RECT rc);
	void AdjuestCef(CRect rc);
private:
	HWND parent_window_;
};
