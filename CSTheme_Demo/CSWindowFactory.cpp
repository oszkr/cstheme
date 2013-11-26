#include "stdafx.h"
#include "CSTheme.h"
#include "CSControl.h"
#include "CSWindow.h"
#include "CSWindowFactory.h"


CSWindowFactory::CSWindowFactory(void)
{
}


CSWindowFactory::~CSWindowFactory(void)
{
}

CSControl* CSWindowFactory::Create(HWND hWnd)
{
	CSWindow* pCSWindow = new CSWindow(hWnd, CSTheme::CSType::Window);
	pCSWindow->m_OldProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pCSWindow->SubProc);
	SetProp(hWnd, _T("CSWindow"), (HANDLE)pCSWindow);
	pCSWindow->Init(4);
	
	return pCSWindow;
}