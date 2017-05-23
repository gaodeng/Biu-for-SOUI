#include "StdAfx.h"
#include "CefBrowserWrapper.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "appshell/cefclient.h"
#include "appshell/client_handler.h"

// external
extern HINSTANCE                hInst;
extern CefRefPtr<ClientHandler> g_handler;




void CefBrowserWrapper::CreateCef(RECT rc)
{
	if (g_handler == NULL) {
		g_handler = new ClientHandler();
		g_handler->SetMainHwnd(parent_window_);
	}
	if (g_handler->GetBrowser() != NULL) {
		HWND brower_wnd = g_handler->GetBrowser()->GetHost()->GetWindowHandle();
		::SetWindowPos(brower_wnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
		return;
	}


	
	


	CefWindowInfo window_info;

	window_info.SetAsChild(parent_window_, rc);
	//window_info.SetAsPopup(parent_window_, "test");

	// SimpleHandler implements browser-level callbacks.
	CefRefPtr<ClientHandler> handler(g_handler);

	// Specify CEF browser settings here.
	CefBrowserSettings settings;

	settings.web_security = STATE_DISABLED;

	// Necessary to enable document.executeCommand("paste")
	settings.javascript_access_clipboard = STATE_ENABLED;
	settings.javascript_dom_paste = STATE_ENABLED;

	std::string url;

	// Check if a "--url=" value was provided via the command-line. If so, use
	// that instead of the default URL.
	CefRefPtr<CefCommandLine> command_line =
		CefCommandLine::GetGlobalCommandLine();
	url = command_line->GetSwitchValue("url");
	if (url.empty())
		//url = "lightyear://messageHistory/handler.html";
		url = "http://localhost:8081/";

	// Create the first browser window.
	CefBrowserHost::CreateBrowser(window_info, handler.get(), url, settings, NULL);
}

void CefBrowserWrapper::AdjuestCef(CRect rc)
{
	if (g_handler->GetBrowser() != NULL) {
		HWND brower_wnd = g_handler->GetBrowser()->GetHost()->GetWindowHandle();
		::SetWindowPos(brower_wnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
		return;
	}
}
