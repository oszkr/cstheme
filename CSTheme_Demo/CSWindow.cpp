#include "stdafx.h"
#include "Resource.h"
#include "cstheme.h"
#include "CSWindow.h"

CSWindow::CSWindow(HWND hWnd, DWORD dwType)
: CSControl(hWnd, dwType), m_hBackgroundBrush(0), m_nTitleHeight(33), m_lpCSWindowReceiver(0), m_nTitlePadding(0)
{
	memset(&m_ButtonBitmap, 0, sizeof(m_ButtonBitmap));
	
}

CSWindow::~CSWindow()
{
}

CSWindowButton::CSWindowButton(wchar_t* szTag, UINT nImageIndex, BOOL bEnable)
	: m_szTag(szTag), m_bEnable(bEnable),  m_nImageIndex(nImageIndex), m_dwState(0), m_nPadding(0)
{

}

CSWindowButton::~CSWindowButton()
{
}

LRESULT CALLBACK CSWindow::SubProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CSWindow* pCSWindow = (CSWindow*)GetProp(hWnd, _T("CSWindow"));
	if(pCSWindow)
		return pCSWindow->SubProc2(hWnd, message, wParam, lParam);
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK CSWindow::SubProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{
	case WM_ENTERSIZEMOVE:
	case WM_NCMOUSEMOVE:
		RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
		break;

	case WM_SYSCOMMAND:
		OnSysCommand(hWnd, message, wParam, lParam);
		break;

	case WM_SETTEXT:
		CallWindowProc(m_OldProc, hWnd, message, wParam, lParam);
		OnSetText(hWnd, message, wParam, lParam);
		return TRUE;

	case WM_NCPAINT:
		OnNcPaint(hWnd, message, wParam, lParam);
		return 0;
		
	case WM_NCCALCSIZE:
		OnNcCalcSize(hWnd, message, wParam, lParam);
		break;
		
	case WM_NCHITTEST:
		return OnNcHitTest(hWnd, message, wParam, lParam);
		
	case WM_NCRBUTTONUP:
		OnNcPaint(hWnd, message, wParam, lParam);
		break;

	case WM_NCACTIVATE:
		OnNcActive(hWnd, message, wParam, lParam);;
		return TRUE;
		
	case WM_SIZE:
		OnSize(hWnd, message, wParam, lParam);
		break;
		
	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, message, wParam, lParam);
		break;
		
	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, message, wParam, lParam);
		break;

	case WM_LBUTTONUP:
		OnLButtonUp(hWnd, message, wParam, lParam);
		break;

	case WM_MOUSELEAVE:
		OnMouseLeave(hWnd, message, wParam, lParam);
		break;

	case WM_GETMINMAXINFO:
		OnGetMinMaxInfo(hWnd, message, wParam, lParam);
		break;

	case WM_QUIT:
		ShowWindow(hWnd, SW_HIDE);
		SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG)m_OldProc);

	default:
		return CallWindowProc(m_OldProc, hWnd, message, wParam, lParam);
	}
	return 0;
}

VOID CSWindow::AddCSWindowButton(CSWindowButton *lpCSWindowButton)
{
	CSWindowButtons.push_back(lpCSWindowButton);
}

VOID CSWindow::CheckStyle()
{
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);

	m_bSizable = static_cast<BOOL>((dwStyle & WS_THICKFRAME) || (dwStyle & WS_SIZEBOX));

	if(dwStyle & WS_SYSMENU)
	{
		m_bMinable = static_cast<BOOL>(dwStyle & WS_MINIMIZEBOX);
		m_bMaxable = static_cast<BOOL>(dwStyle & WS_MAXIMIZEBOX);
		m_bCloseable = TRUE;
	}
	else
	{
		m_bMinable = FALSE;
		m_bMaxable = FALSE;
		m_bCloseable = FALSE;
	}

	CSWindowButtons.clear();
	if(m_bMaxable && m_bMinable)
	{
		CSWindowButton* pCloseButton = new CSWindowButton(L"Close", 0);
		CSWindowButtons.push_back(pCloseButton);

		CSWindowButton* pMaxButton = new CSWindowButton(L"Max", m_bMaximize ? 2 : 1);
		CSWindowButtons.push_back(pMaxButton);

		CSWindowButton* pMinButton = new CSWindowButton(L"Min", 3);
		CSWindowButtons.push_back(pMinButton);

	}
	else if(!m_bMaxable && m_bMinable)
	{
		CSWindowButton* pCloseButton = new CSWindowButton(L"Close", 0);
		CSWindowButtons.push_back(pCloseButton);

		CSWindowButton* pMaxButton = new CSWindowButton(L"Max", m_bMaximize ? 2 : 1, FALSE);
		CSWindowButtons.push_back(pMaxButton);

		CSWindowButton* pMinButton = new CSWindowButton(L"Min", 3);
		CSWindowButtons.push_back(pMinButton);
	}
	else if(m_bMaxable && !m_bMinable)
	{
		CSWindowButton* pCloseButton = new CSWindowButton(L"Close", 0);
		CSWindowButtons.push_back(pCloseButton);

		CSWindowButton* pMaxButton = new CSWindowButton(L"Max", m_bMaximize ? 2 : 1);
		CSWindowButtons.push_back(pMaxButton);

		CSWindowButton* pMinButton = new CSWindowButton(L"Min", 3, FALSE);
		CSWindowButtons.push_back(pMinButton);
	}
	else if(!m_bMaxable && !m_bMinable && m_bCloseable)
	{
		CSWindowButton* pCloseButton = new CSWindowButton(L"Close", 0);
		CSWindowButtons.push_back(pCloseButton);
	}

	UINT nButtonPieceWidth = m_ButtonBitmap.bmWidth / m_nButtonCount;

	if(CSWindowButtons.size())
  		m_nTitlePadding = CSWindowButtons.size() * nButtonPieceWidth + m_nButtonOffsetX + m_nFrameSize + (CSWindowButtons.size() -1 * m_nButtonMarginSize);
	else
		m_nTitlePadding = m_nFrameSize;
}

RECT* CSWindow::GetButtonRect(UINT nIndex, UINT nWidth)
{
	RECT* rectButton = new RECT;
	UINT nButtonPieceWidth = m_ButtonBitmap.bmWidth / m_nButtonCount;
	UINT nButtonPieceHeight = m_ButtonBitmap.bmHeight / 7;

	rectButton->top = m_nButtonOffsetY;
	rectButton->left = nWidth - (nButtonPieceWidth * (nIndex + 1) + m_nButtonOffsetX + nIndex * m_nButtonMarginSize);
	rectButton->bottom = rectButton->top + nButtonPieceHeight;
	rectButton->right = rectButton->left + nButtonPieceWidth;

	return rectButton;
}

VOID CSWindow::Init(UINT nButtonCount)
{
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight			= 20;
	lf.lfWeight			= FW_NORMAL;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfQuality		= DEFAULT_QUALITY;
	lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s(lf.lfFaceName, _T("¸¼Àº °íµñ"));
	m_hFont = CreateFontIndirect(&lf);

	m_hMemDC = CreateCompatibleDC(NULL);

	m_SeletedBtn = -1;
	m_nTitlebarHeight = 33;

	m_nButtonWidth = 36;
	m_nButtonHeight = 20;

	m_nButtonOffsetX = 7;
	m_nButtonOffsetY = 9;

	m_nButtonMarginSize = 2;

	m_bActive = TRUE;
	m_bMaximize = FALSE;

	m_hMemDC2 = CreateCompatibleDC(m_hMemDC);
	HBITMAP hFrameBitmap = LoadBitmap((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_WINDOWFRAME));
	GetObject(hFrameBitmap, sizeof(BITMAP), &m_FrameBitmap);
	SelectObject(m_hMemDC2, hFrameBitmap);

	m_hMemDC3 = CreateCompatibleDC(m_hMemDC);
	HBITMAP hButtonBitmap = LoadBitmap((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_WINDOWBUTTONS));
	GetObject(hButtonBitmap, sizeof(BITMAP), &m_ButtonBitmap);
	SelectObject(m_hMemDC3, hButtonBitmap);

	m_nButtonCount = nButtonCount;
	m_nFrameSize = m_FrameBitmap.bmWidth / 6;

	CheckStyle();
}

VOID CSWindow::OnNcPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(!m_ButtonBitmap.bmWidth)
		return;

	HDC hDC = GetWindowDC(hWnd);
	
	RECT WindowRect;
	GetWindowRect(&WindowRect);
	OffsetRect(&WindowRect, -WindowRect.left, -WindowRect.top);

	RECT ClientRect;
	GetClientRect(hWnd, &ClientRect);
	
	UINT nWidth, nHeight;
	
	if(m_bMaximize)
	{
		nWidth = WindowRect.right - GetSystemMetrics(SM_CXSIZEFRAME) * 2;
		nHeight = WindowRect.bottom - GetSystemMetrics(SM_CYSIZEFRAME) * 2;
	}
	else
	{
		nWidth = WindowRect.right;
		nHeight = WindowRect.bottom;
	}

	m_MemBitmap = CreateCompatibleBitmap(hDC, nWidth, nHeight);
	m_OldMemBitmap = (HBITMAP)SelectObject(m_hMemDC, m_MemBitmap);
	
	if (m_bMaximize)
		ExcludeClipRect(hDC, m_nFrameSize, m_nTitleHeight + GetSystemMetrics(SM_CYSIZEFRAME), nWidth, nHeight);
	else
		ExcludeClipRect(hDC, m_nFrameSize, m_nTitleHeight, nWidth - m_nFrameSize, nHeight - m_nFrameSize);

	UINT ActiveImageOffset = (m_bActive ? 0 : m_FrameBitmap.bmWidth / 2);

	if(m_bMaximize)
	{
		StretchBlt(m_hMemDC, 0, 0, nWidth, m_nTitlebarHeight,
		m_hMemDC2, m_nFrameSize + ActiveImageOffset, 0, m_nFrameSize, m_nTitlebarHeight, SRCCOPY);
	}
	else
	{
		BitBlt(m_hMemDC, 0, 0, m_nFrameSize, m_nTitlebarHeight, m_hMemDC2, ActiveImageOffset, 0, SRCCOPY);
		StretchBlt(m_hMemDC, m_nFrameSize, 0, nWidth - m_FrameBitmap.bmWidth / 3, m_nTitlebarHeight,
			m_hMemDC2, m_nFrameSize + ActiveImageOffset, 0, m_nFrameSize, m_nTitlebarHeight, SRCCOPY);
		BitBlt(m_hMemDC, nWidth - m_nFrameSize, 0, m_nFrameSize, m_nTitlebarHeight, m_hMemDC2, m_nFrameSize * 2 + ActiveImageOffset, 0, SRCCOPY);

		StretchBlt(m_hMemDC, 0, m_nTitlebarHeight, m_nFrameSize, nHeight - m_nTitlebarHeight - m_FrameBitmap.bmWidth/6,
			m_hMemDC2, ActiveImageOffset, m_nTitlebarHeight, m_nFrameSize, m_nFrameSize, SRCCOPY);
		StretchBlt(m_hMemDC, nWidth - m_nFrameSize, m_nTitlebarHeight, m_nFrameSize, nHeight - m_nTitlebarHeight - m_nFrameSize,
			m_hMemDC2, m_nFrameSize * 2 + ActiveImageOffset, m_nTitlebarHeight, m_nFrameSize, m_nFrameSize, SRCCOPY);

		BitBlt(m_hMemDC, 0, nHeight - m_nFrameSize, m_nFrameSize, m_nFrameSize,
			m_hMemDC2, 0 + ActiveImageOffset, m_nTitlebarHeight + m_nFrameSize, SRCCOPY);
		StretchBlt(m_hMemDC, m_nFrameSize, nHeight - m_nFrameSize, nWidth - m_nFrameSize * 2, m_nFrameSize,
			m_hMemDC2, m_nFrameSize + ActiveImageOffset, m_nTitlebarHeight + m_nFrameSize,
			m_nFrameSize, m_nFrameSize, SRCCOPY);
		BitBlt(m_hMemDC, nWidth - m_nFrameSize, nHeight - m_nFrameSize, m_nFrameSize, m_nFrameSize,
			m_hMemDC2, m_nFrameSize * 2 + ActiveImageOffset, m_nTitlebarHeight + m_nFrameSize, SRCCOPY);
	}

	UINT nSize = CSWindowButtons.size();
	UINT nButtonPieceWidth = m_ButtonBitmap.bmWidth / m_nButtonCount;
	UINT nButtonPieceHeight = m_ButtonBitmap.bmHeight / 7;

	for(int i=0; i<nSize; i++)
	{
		RECT* rectButton = GetButtonRect(i, nWidth);
		BitBlt(m_hMemDC, rectButton->left, rectButton->top, rectButton->right - rectButton->left, rectButton->bottom - rectButton->top,
			m_hMemDC3, CSWindowButtons[i]->GetImageIndex() * nButtonPieceWidth, CSWindowButtons[i]->GetState() * nButtonPieceHeight, SRCCOPY);
	}

	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	HFONT hOldFont;

	HICON hIcon;
	int cx = 24;
	int cy = 24;

	hIcon = GetIcon();
	DrawIconEx(m_hMemDC, 8, 5, hIcon, cx, cy, 0, 0, DI_NORMAL);


	hOldFont = (HFONT)SelectObject(m_hMemDC, m_hFont);

	TCHAR szWindowText[255];
	GetWindowText(hWnd, szWindowText, 255);
	RECT textRect;

	if(hIcon)
		textRect.left = 40;
	else
		textRect.left = 10;

	textRect.top = 5;
	textRect.right = nWidth - m_nTitlePadding;
	textRect.bottom = 30;

	SetBkMode(m_hMemDC, TRANSPARENT);
	SetTextColor(m_hMemDC, m_bActive ? RGB(0x1e, 0x39, 0x5b) : RGB(0x6a, 0x80, 0xa8));
	hOldFont = (HFONT)SelectObject(m_hMemDC, m_hFont);
	DrawText(m_hMemDC, szWindowText, _tcslen(szWindowText), &textRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_WORD_ELLIPSIS);
	SelectObject(m_hMemDC, hOldFont);

	if (m_bMaximize)
		BitBlt(hDC, GetSystemMetrics(SM_CXSIZEFRAME), GetSystemMetrics(SM_CYSIZEFRAME), nWidth, m_nTitlebarHeight, m_hMemDC, 0, 0, SRCCOPY);
	else
		BitBlt(hDC, 0, 0, nWidth, nHeight, m_hMemDC, 0, 0, SRCCOPY);

	SelectClipRgn(hDC, NULL);
	SelectObject(m_hMemDC, m_OldMemBitmap);
	DeleteObject(m_MemBitmap);
	ReleaseDC(hWnd, hDC);

}

VOID CSWindow::GetWindowRect(RECT* lpRect, DWORD dwFlag)
{
	::GetWindowRect(m_hWnd, lpRect);

	if (m_bMaximize && dwFlag)
	{
		lpRect->bottom -= GetSystemMetrics(SM_CYSIZEFRAME);
		lpRect->top += GetSystemMetrics(SM_CYSIZEFRAME);
		lpRect->left += GetSystemMetrics(SM_CXSIZEFRAME);
		lpRect->right -= GetSystemMetrics(SM_CXSIZEFRAME);
	}
}

VOID CSWindow::OnNcCalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	NCCALCSIZE_PARAMS* pParams = NULL;
	RECT* pRect = NULL;

	BOOL bValue = static_cast<BOOL>(wParam);
	if(bValue) pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
	else pRect = reinterpret_cast<RECT*>(lParam);

	if(bValue)
	{
		pRect = &pParams->rgrc[0];
		if (m_bMaximize)
		{
			pRect->left = 0;
			pRect->top = pRect->top + m_nTitlebarHeight + GetSystemMetrics(SM_CYSIZEFRAME);
			pRect->right = pRect->right - 0;
			pRect->bottom = pRect->bottom - 0;
		}
		else
		{
			pRect->left = pRect->left + m_FrameBitmap.bmWidth / 6;
			pRect->top = pRect->top + m_nTitlebarHeight;
			pRect->right = pRect->right - m_FrameBitmap.bmWidth / 6;
			pRect->bottom = pRect->bottom - m_FrameBitmap.bmWidth / 6;
		}
		
	}
	else DefWindowProc(hWnd, message, wParam, lParam);
}

HICON CSWindow::GetIcon()
{
	HICON hIcon = (HICON)SendMessage(m_hWnd, WM_GETICON, ICON_BIG, 0);
	if(!hIcon) hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDR_MAINFRAME));

	return hIcon;
}

UINT CSWindow::OnNcHitTest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_SeletedBtn != 255)
	{
		CSWindowButtons[m_SeletedBtn]->SetState(1 + (m_bActive ? 0 : 4));
		return HTCLIENT;
	}

	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);

	RECT rectWindow, rect;
	GetWindowRect(&rectWindow, 1);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	point.x -= rectWindow.left;
	point.y -= rectWindow.top;

	HICON hIcon = GetIcon();;

	if(hIcon)
	{
		int cx = GetSystemMetrics(SM_CXICON);
		int cy = GetSystemMetrics(SM_CYICON);
		SetRect(&rect, 5, 2, 5+cx, 2+cy);
		if(PtInRect( &rect, point))
			return HTSYSMENU;
	}

	RECT* rectButton;
	BOOL bChanged = FALSE;
	UINT nSize = CSWindowButtons.size();

	for(int i=0; i<nSize; i++)
	{
		rectButton = GetButtonRect(i, nWidth);
		if (PtInRect(rectButton, point))
		{
			OnNcPaint(hWnd, message, wParam, lParam);
			return HTCLIENT;
		}
		else if(CSWindowButtons[i]->GetEnable())
		{
			CSWindowButtons[i]->SetState(0  + (m_bActive ? 0 : 4));
			bChanged = TRUE;
		}		
	}


	if (!m_bMaximize)
	{

		int BorderLeftWidth = 8, BorderLeftHeight = 8, BorderBottomHeight = 8, BorderRightWidth = 8, BorderTopHeight = 8;
		SetRect(&rect, 0, 0, BorderLeftWidth, BorderTopHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTTOPLEFT;

		SetRect(&rect, nWidth - BorderLeftWidth, 0, nWidth, BorderTopHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTTOPRIGHT;

		SetRect(&rect, 0, nHeight - BorderBottomHeight, BorderLeftWidth, nHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTBOTTOMLEFT;

		SetRect(&rect, nWidth - BorderRightWidth, nHeight - BorderBottomHeight, nWidth, nHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTBOTTOMRIGHT;

		SetRect(&rect, 0, BorderTopHeight, BorderLeftWidth, nHeight - BorderBottomHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTLEFT;

		SetRect(&rect, nWidth - BorderRightWidth, BorderTopHeight, nWidth, nHeight - BorderBottomHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTRIGHT;

		SetRect(&rect, BorderLeftWidth, nHeight - BorderBottomHeight, nWidth - BorderRightWidth, nHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTBOTTOM;

		SetRect(&rect, BorderLeftWidth, 0, nWidth - BorderRightWidth, BorderBottomHeight);
		if (PtInRect(&rect, point) && m_bSizable)
			return HTTOP;
	}

	SetRect(&rect, 0, 0, nWidth, m_nTitleHeight);
	if (PtInRect(&rect, point))
		return HTCAPTION;
	
	return HTCLIENT;
}

VOID CSWindow::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(hWnd, NULL, TRUE);
	OnNcPaint(hWnd, message, wParam, lParam);

	CSWindowButton* pCSWindowButton;
	RECT rectWindow;

	switch(wParam)
	{ 
	case SIZE_RESTORED:
		m_bMaximize = FALSE;
		pCSWindowButton = GetCSWindowButton(L"Max");
		if(pCSWindowButton)
			pCSWindowButton->SetImageIndex(1);
		break;

	case SIZE_MAXIMIZED:
		m_bMaximize = TRUE;
		pCSWindowButton = GetCSWindowButton(L"Max");
		if(pCSWindowButton)
			pCSWindowButton->SetImageIndex(2);
		break;
	} 

	GetWindowRect(&rectWindow);
	OffsetRect(&rectWindow, -rectWindow.left, -rectWindow.top);

	int nWidth, nHeight;
	nWidth = rectWindow.right;
	nHeight = rectWindow.bottom;

	HRGN hRgn;
	if (m_bMaximize)
		hRgn = CreateRectRgn(GetSystemMetrics(SM_CXSIZEFRAME), GetSystemMetrics(SM_CYSIZEFRAME),
							nWidth - GetSystemMetrics(SM_CXSIZEFRAME), nHeight - GetSystemMetrics(SM_CYSIZEFRAME));
	else
		hRgn = CreateRectRgn(0, 0, nWidth, nHeight);

	SetWindowRgn(hWnd, hRgn, TRUE);
	RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}

VOID CSWindow::OnNcActive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_bActive = (BOOL)wParam; 
	UINT nSize = CSWindowButtons.size();
	
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_hWnd, &point);

	for(int i=0; i<nSize; i++)
	{
		if(m_bActive)
		{
			if(CSWindowButtons[i]->GetEnable()) 
				OnMouseMove(hWnd, message, wParam, MAKELPARAM(point.x, point.y));
			else
				CSWindowButtons[i]->SetState(3);
		}
		else
		{
			if(CSWindowButtons[i]->GetEnable())
				OnMouseMove(hWnd, message, wParam, MAKELPARAM(point.x, point.y));
			else
				CSWindowButtons[i]->SetState(6);
		}
	}
	OnNcPaint(hWnd, message, wParam, lParam);
}

VOID CSWindow::OnGetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MINMAXINFO *lpMMI = (MINMAXINFO*)lParam;
	lpMMI->ptMinTrackSize.x = 200;
	lpMMI->ptMinTrackSize.y = m_nTitlebarHeight + m_nFrameSize;
}

VOID CSWindow::OnNcLButtonDblClk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OnNcPaint(hWnd, message, wParam, lParam);
}

VOID CSWindow::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = GET_X_LPARAM(lParam) + m_nFrameSize;
	point.y = GET_Y_LPARAM(lParam) + m_nTitlebarHeight;

	RECT rectWindow;
	GetWindowRect(&rectWindow, 1);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	ResetButtonState();

	if(m_SeletedBtn != 255)
	{
		RECT* rectButton = GetButtonRect(m_SeletedBtn, nWidth);
		if(PtInRect(rectButton, point))
			CSWindowButtons[m_SeletedBtn]->SetState(2 + (m_bActive ? 0 : 4));
		else
			CSWindowButtons[m_SeletedBtn]->SetState(0 + (m_bActive ? 0 : 4));

		RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
		return;
	}

	if (point.y < m_nTitlebarHeight)
	{
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
		TrackMouseEvent.dwFlags = TME_LEAVE;
		TrackMouseEvent.hwndTrack = hWnd;
		TrackMouseEvent.dwHoverTime = 0;
		_TrackMouseEvent(&TrackMouseEvent);


		UINT nSize = CSWindowButtons.size();
		for(int i=0; i<nSize; i++)
		{
			RECT* rectButton = GetButtonRect(i, nWidth);
			if(PtInRect(rectButton, point) && CSWindowButtons[i]->GetEnable())
			{
				if(m_SeletedBtn == 255)
					CSWindowButtons[i]->SetState(1 + (m_bActive ? 0 : 4));
				if(GetAsyncKeyState(VK_LBUTTON) && m_SeletedBtn == i)
					CSWindowButtons[i]->SetState(2 + (m_bActive ? 0 : 4));

				break;
			}
		}
		
	}
	if(m_SeletedBtn != 255)
		RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);

}

VOID CSWindow::OnMouseLeave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ResetButtonState();

	if(m_SeletedBtn != 255)
		CSWindowButtons[m_SeletedBtn]->SetState(1);

	RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}

VOID CSWindow::OnLButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SetCapture(hWnd);
	POINT point;
	point.x = GET_X_LPARAM(lParam) + m_nFrameSize;
	point.y = GET_Y_LPARAM(lParam) + m_nTitlebarHeight;

	RECT rectWindow;
	GetWindowRect(&rectWindow, 1);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	m_SeletedBtn = 255;

	UINT nSize = CSWindowButtons.size();
	for(int i=0; i<nSize; i++)
	{
		RECT* rectButton = GetButtonRect(i, nWidth);
		if(PtInRect(rectButton, point) && CSWindowButtons[i]->GetEnable())
		{
			CSWindowButtons[i]->SetState(2);
			m_SeletedBtn = i;
			break;
		}
	}

	RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}

VOID CSWindow::OnLButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = GET_X_LPARAM(lParam) + m_nFrameSize;
	point.y = GET_Y_LPARAM(lParam) + m_nTitlebarHeight;

	RECT rectWindow;
	GetWindowRect(&rectWindow, 1);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	RECT* rectButton = GetButtonRect(m_SeletedBtn, nWidth);

	if(PtInRect(rectButton, point) && CSWindowButtons[m_SeletedBtn]->GetEnable())
	{
		UINT bRet = TRUE;

		if(m_lpCSWindowReceiver)
			bRet = m_lpCSWindowReceiver->CSWindowReceive(m_SeletedBtn);

		if(bRet == 1)
		{
			if(wcscmp(CSWindowButtons[m_SeletedBtn]->GetTag(), L"Close") == 0)
			{
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
			else if(wcscmp(CSWindowButtons[m_SeletedBtn]->GetTag(), L"Max") == 0)
			{
				if(m_bMaximize)
					PostMessage(hWnd, WM_SYSCOMMAND, (WPARAM)SC_RESTORE, 0);
				else
					PostMessage(hWnd, WM_SYSCOMMAND, (WPARAM)SC_MAXIMIZE, 0);
			}
			else if(wcscmp(CSWindowButtons[m_SeletedBtn]->GetTag(), L"Min") == 0)
			{
				PostMessage(hWnd, WM_SYSCOMMAND, (WPARAM)SC_MINIMIZE, 0);
			}
		}
	}

	m_SeletedBtn = 255;
	OnMouseMove(hWnd, message, wParam, lParam);
	RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	ReleaseCapture();
}

VOID CSWindow::OnSysCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	DefWindowProc(hWnd, message, wParam, lParam);
 		RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	
}

VOID CSWindow::OnSetText(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OnNcPaint(hWnd, message, wParam, lParam);
}

UINT CSWindow::OnNcRButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return OnNcHitTest(hWnd, message, wParam, lParam);
}