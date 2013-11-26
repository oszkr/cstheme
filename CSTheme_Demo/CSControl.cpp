#include "stdafx.h"
#include "CSControl.h"

WNDPROC CSControl::m_OldProc = 0;

CSControl::CSControl(HWND hWnd, DWORD dwType)
	: m_hWnd(hWnd), m_dwType(dwType)
{
}

