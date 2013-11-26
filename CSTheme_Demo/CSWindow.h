#pragma once

class CSWindowReceiver
{
public:
	virtual UINT CSWindowReceive(UINT nEvent) = 0;
};

class CSWindowButton
{
	friend class CSWindow;

	CSWindowButton(wchar_t* szTag, UINT nImageIndex, BOOL bEnable = TRUE);
	~CSWindowButton();

private:
	BOOL	m_bEnable;
	DWORD	m_dwState;
	UINT	m_nPadding;
	UINT	m_nImageIndex;
	std::wstring m_szTag;

public:
	VOID SetEnable(CONST BOOL bEnable)
	{
		m_bEnable = bEnable;
	}
	VOID SetState(DWORD dwState)
	{
		m_dwState = dwState;
	}
	VOID SetPadding(UINT nPadding)
	{
		m_nPadding = nPadding;
	}
	VOID SetImageIndex(UINT nImageIndex)
	{
		m_nImageIndex = nImageIndex;
	}
	VOID SetTag(wchar_t szTag)
	{
		m_szTag = szTag;
	}

	BOOL GetEnable() const
	{
		return m_bEnable;
	}
	DWORD GetState() const
	{
		return m_dwState;
	}
	UINT GetPadding() const
	{
		return m_nPadding;
	}
	UINT GetImageIndex() const
	{
		return m_nImageIndex;
	}
	wchar_t* GetTag() const
	{
		return (wchar_t*)m_szTag.c_str();
	}
};

class CSWindow : public CSControl
{
public:
	CSWindow(HWND hWnd, DWORD dwType);
	~CSWindow();

public:
	static LRESULT CALLBACK SubProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK SubProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID Init(UINT nButtonCount = 4);
	VOID SetReceiver(CSWindowReceiver* lpCSWindowReceiver);
	VOID AddCSWindowButton(CSWindowButton *lpCSWindowButton);

	CSWindowButton* GetCSWindowButton(wchar_t* szTag)
	{
		UINT nSize = CSWindowButtons.size();
		for(int i=0; i<nSize; i++)
		{
			if(wcscmp(CSWindowButtons[i]->GetTag(), szTag) == 0)
				return CSWindowButtons[i];
		}
		return NULL;
	}

protected:
	VOID OnNcPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnNcCalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	UINT OnNcHitTest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnNcActive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnNcLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnNcLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnNcLButtonDblClk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnNcMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnMouseLeave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnUserMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnSysCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnGetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	UINT OnRButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	UINT OnNcRButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnSetText(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	RECT* GetButtonRect(UINT nIndex, UINT nWidth);

	VOID ResetButtonState()
	{
		UINT nSize = CSWindowButtons.size();
		for(int i=0; i<nSize; i++)
		{
			if(CSWindowButtons[i]->GetEnable())
				CSWindowButtons[i]->SetState(0 + (m_bActive ? 0 : 4));
			else
				CSWindowButtons[i]->SetState(m_bActive ? 3 : 6);
		}
	}
	
protected:
	HBITMAP m_MemBitmap;
	HBITMAP m_OldMemBitmap;
	HDC m_hMemDC;
	HDC m_hMemDC2;
	HDC m_hMemDC3;
	HFONT m_hFont;

	BOOL m_bSizable;
	BOOL m_bMinable;
	BOOL m_bMaxable;
	BOOL m_bCloseable;
	BOOL m_bActive;
	
	UINT m_nTitlebarHeight;
	ULONG_PTR m_gdiplusToken;

	UINT m_nTitleHeight;
	UINT m_nFrameSize;
	UINT m_nTitlebarWidth;

	UINT m_nButtonWidth;
	UINT m_nButtonHeight;
	UINT m_nButtonOffsetX;
	UINT m_nButtonOffsetY;
	UINT m_nButtonMarginSize;

	BITMAP m_FrameBitmap;
	BITMAP m_ButtonBitmap;

	BYTE m_SeletedBtn;

	HBRUSH m_hBackgroundBrush;

	RECT m_rectNC;

	RECT m_RestoreWinRect;

	BOOL m_bMoving;
	BOOL m_bMaximize;
	UINT m_nButtonCount;

	UINT m_nTitlePadding;
	CSWindowReceiver* m_lpCSWindowReceiver;

	std::vector<CSWindowButton*> CSWindowButtons;

protected:
	VOID CheckStyle();
	VOID DrawBtn(HDC hDC, int nType, int nIndex, int nState, int nWindowWidth, bool bEnable = true);
	VOID GetWindowRect(RECT* lpRect, DWORD dwFlag = 0);
	HICON GetIcon();
};

