#pragma once

class CSControl
{
protected:
	CSControl(HWND hWnd, DWORD dwType);

	DWORD m_dwType;
	HWND m_hWnd;

	static WNDPROC m_OldProc;

	//virtual LRESULT CALLBACK SubProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) = 0;

	friend class CSTheme;
	friend class CSWindowFactory;
};