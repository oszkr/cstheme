#pragma once
class CSShadow
{
public:
	CSShadow(void);
	~CSShadow(void);

public:
	void Create(HWND hWnd);
	VOID ParentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void SetActiveColor(BYTE r, BYTE g, BYTE b);
	void SetActiveColor(DWORD _Color);
	void SetInactiveColor(BYTE r, BYTE g, BYTE b);
	void SetInactiveColor(DWORD _Color);

	void Show() { ShowWindow(m_hWnd, SW_SHOW); OnSize(m_hWnd, 0, 0, 0);  }
	void Hide() { ShowWindow(m_hWnd, SW_HIDE); }

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnNcCalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	UINT OnNcHitTest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_hWnd; //Shadow window 1
	HWND m_hWnd_parent;
	HWND m_hWnd3;
	INT m_Status;

	BOOL m_bSizable;
	BOOL m_bMinable;
	BOOL m_bMaxable;

	INT BorderSize;
	INT m_WinState;
	bool bTemp;

	ULONG_PTR gdiplusToken;
	HDC m_MemDC;

	DWORD dwActiveColor;
	DWORD dwInactiveColor;

	BOOL m_bActive;

protected:
	void Draw();
	HBITMAP m_MemBitmap;
	HBITMAP m_OldMemBitmap;

	int m_nWidth, m_nHeight;
};

