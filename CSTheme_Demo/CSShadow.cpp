#include "stdafx.h"
#include "CSShadow.h"


CSShadow::CSShadow(void)
	: BorderSize(10)
{
	dwActiveColor = 0xFFFF0000;
	dwInactiveColor = 0xFF0000FF;
	m_bSizable = TRUE;
}

CSShadow::~CSShadow(void)
{
}

void CSShadow::SetActiveColor(BYTE r, BYTE g, BYTE b)
{
	dwActiveColor = (DWORD&)Color(r, g, b);
	RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
}

void CSShadow::SetActiveColor(DWORD _Color)
{
	dwActiveColor = _Color;
	RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	//Draw();
}

void CSShadow::SetInactiveColor(BYTE r, BYTE g, BYTE b)
{
	dwInactiveColor = (DWORD&)Color(r, g, b);
	RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	//Draw();
}

void CSShadow::SetInactiveColor(DWORD _Color)
{
	dwInactiveColor = _Color;
	RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
	//Draw();
}

void CSShadow::Create(HWND hWnd)
{
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	m_bSizable = dwStyle & WS_SIZEBOX;
	m_bMinable = dwStyle & WS_MINIMIZEBOX;
	m_bMaxable = dwStyle & WS_MAXIMIZEBOX;
	
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _T("CSThemeShadowWindow");
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);

	m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_TOOLWINDOW, _T("CSThemeShadowWindow"), _T(""),
		WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SIZEBOX | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_BORDER, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, 0, NULL);

	SetProp(m_hWnd, _T("CSThemeShadowWindow"), (HANDLE)this);

	m_hWnd_parent = hWnd;
	ShowWindow(m_hWnd, SW_SHOW);

	// Configure GDI+
	GdiplusStartupInput GdiplusStartup;
	if(::GdiplusStartup(&gdiplusToken, &GdiplusStartup, NULL) !=0)
	{
		AfxMessageBox(_T("ERROR : Fail to initialize GDI+ library!"));
		return;
	}

	SetInactiveColor(255, 0, 0);

	m_bActive = TRUE;
	//CloseHandle(CreateThread(NULL, 0, osuhhhMain, NULL, 0, NULL));
	RECT WindowRect;
	GetWindowRect(hWnd, &WindowRect);
	SetWindowPos(m_hWnd, hWnd,
		WindowRect.left - 10, WindowRect.top - 10,
		WindowRect.right - WindowRect.left + 20,
		WindowRect.bottom - WindowRect.top + 20, SWP_NOACTIVATE);

	m_nWidth = WindowRect.right - WindowRect.left + 20;
	m_nHeight = WindowRect.bottom - WindowRect.top + 20;
}

void CSShadow::Draw()
{
	RECT WinRect;
	GetWindowRect(m_hWnd, &WinRect);

	int nWidth = WinRect.right-WinRect.left;
	int nHeight = WinRect.bottom-WinRect.top;

	HDC hdcScreen = GetDC(NULL); 
	HDC hDC = CreateCompatibleDC(hdcScreen); 
	HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, nWidth+10, nHeight+10); 
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, hBmp);

	
	DWORD dwColor;
	if (m_bActive)
		dwColor = dwActiveColor;
	else
		dwColor = dwInactiveColor;

	Pen* _Pen = new Pen(dwColor, 1);
	Graphics _Graphics(hDC);

	Rect rect = { 9, 9, nWidth - 19, nHeight - 19 };
	_Graphics.DrawLine(_Pen, 9, 9, nWidth - 10, 9);
	_Graphics.DrawLine(_Pen, 9, nHeight - 10, nWidth - 10, nHeight - 10);

	_Graphics.DrawLine(_Pen, 9, 9, 9, nHeight - 10);
	_Graphics.DrawLine(_Pen, nWidth - 10, 9, nWidth - 10, nHeight - 10);

	_Graphics.ExcludeClip(rect);

	GraphicsPath graphicsPath;
	int diameter = 2 * 2;

	graphicsPath.AddArc(Rect(rect.X, rect.Y, diameter, diameter), 180.0f, 90.0f);
	graphicsPath.AddArc(Rect(rect.X + rect.Width - diameter, rect.Y, diameter, diameter), 270.0f, 90.0f);
	graphicsPath.AddArc(Rect(rect.X + rect.Width - diameter, rect.Y + rect.Height - diameter, diameter, diameter), 0.0f, 90.0f);
	graphicsPath.AddArc(Rect(rect.X, rect.Y + rect.Height - diameter, diameter, diameter), 90.0f, 90.0f);
	graphicsPath.CloseFigure();

	int num_steps = 10;
	float delta = 2;
	float alpha = delta;
	for (int thickness = 10; thickness >= 0; thickness--)
	{
		Color color = Color((int)alpha, (BYTE)(dwColor>>16), (BYTE)(dwColor >> 8), (BYTE)(dwColor));
		Pen *pen = new Pen(color, thickness);

		_Graphics.DrawPath(pen, &graphicsPath);
		
		alpha += delta;
	}
	Pen *pen = new Pen(*new Color(1, 255, 255, 255), 20);
	_Graphics.DrawPath(pen, &graphicsPath);
	//Rect rect2 = { 9, 9, nWidth-19, nHeight-19};
	
	//graphicsPath.AddRectangle(rect2);


	BLENDFUNCTION blend = {0}; 
	blend.BlendOp				= AC_SRC_OVER; 
	blend.SourceConstantAlpha	= 255; 
	blend.AlphaFormat			= AC_SRC_ALPHA; 
	blend.BlendFlags			= 0;

	POINT ptPos = {WinRect.left, WinRect.top}; 
	SIZE sizeWnd = {nWidth, nHeight}; 
	POINT ptSrc = {0, 0}; 

	UpdateLayeredWindow(m_hWnd, hdcScreen, &ptPos, &sizeWnd, hDC, &ptSrc, 0, &blend, ULW_ALPHA); 
	
	SelectObject(hDC, m_OldMemBitmap);
	DeleteObject(m_MemBitmap);
	DeleteObject(hBmp);
	ReleaseDC(m_hWnd, m_MemDC);
	ReleaseDC(m_hWnd, hdcScreen);
	DeleteDC(hDC);
}

LRESULT CALLBACK CSShadow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CSShadow* pCSShadow = (CSShadow*)GetProp(hWnd, _T("CSThemeShadowWindow"));
	if(pCSShadow)
		return pCSShadow->WndProc2(hWnd, message, wParam, lParam);
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
}

void Log2(const char* format, ...)
{
	char buffer[4096];
	va_list vaList;
	va_start(vaList, format);
	_vsnprintf_s(buffer, 4096, format, vaList);
	va_end(vaList);

	OutputDebugStringA(buffer);
}

LRESULT CALLBACK CSShadow::WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		OnPaint(hWnd, message, wParam, lParam);
		return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_SIZE:
		OnSize(hWnd, message, wParam, lParam);
		Draw();
		break;
	case WM_NCCALCSIZE:
		OnNcCalcSize(hWnd, message, wParam, lParam);
		break;

	case WM_NCHITTEST:
		return OnNcHitTest(hWnd, message, wParam, lParam);

	case WM_ACTIVATE:
		if (wParam)
		{
			if ((HWND)lParam != m_hWnd_parent)
			{
				//Log2("[Shadow] Activate! hWnd:%08X\n", lParam);
				m_bActive = TRUE;
				Draw();
				SendMessage(m_hWnd_parent, WM_USER + 0x11, 1, 0);
			}
				
		}
		else if(!wParam)
		{
			if ((HWND)lParam != m_hWnd_parent)
			{
			//	Log2("[Shadow] Inactivate! hWnd:%08X\n", lParam);
				m_bActive = FALSE;
				Draw();
				SendMessage(m_hWnd_parent, WM_USER + 0x11, 0, 0);
			}
			
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;	
}

VOID CSShadow::ParentWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	WINDOWPOS* pWinPos;
	switch(message)
	{

	case WM_MOVE:
		RECT WindowRect;
		GetWindowRect(hWnd, &WindowRect);
		SetWindowPos(m_hWnd, hWnd,
			WindowRect.left - 10, WindowRect.top - 10,
			WindowRect.right - WindowRect.left + 20,
			WindowRect.bottom - WindowRect.top + 20, SWP_NOACTIVATE);

		m_nWidth = WindowRect.right - WindowRect.left + 20;
		m_nHeight = WindowRect.bottom - WindowRect.top + 20;
		break;

	
	case WM_ACTIVATE:
		if (wParam)
		{
			if ((HWND)lParam != m_hWnd)
			{
				//Log2("[Parent] Activate! hWnd:%08X\n", lParam);
				m_bActive = TRUE;
				Draw();
				SendMessage(m_hWnd_parent, WM_USER + 0x11, 1, 0);
			}
				
		}
		else if (!wParam)
		{
			if ((HWND)lParam != m_hWnd)
			{
			//	Log2("[Parent] Inactivate! hWnd:%08X\n", lParam);
				m_bActive = FALSE;
				Draw();
				SendMessage(m_hWnd_parent, WM_USER + 0x11, 0, 0);
			}
				
		}
		SetWindowPos(m_hWnd, m_hWnd_parent, 300, 300, 500, 500, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOSENDCHANGING);
		break;

	case WM_CLOSE:
	case WM_DESTROY:
		RemoveProp(hWnd, _T("CSThemeShadowWindow"));
		break;
	}
}

UINT CSShadow::OnNcHitTest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = GET_X_LPARAM(lParam);
	point.y = GET_Y_LPARAM(lParam);

	RECT WinRect, rect;
	GetWindowRect(hWnd, &WinRect);

	point.x -= WinRect.left;
	point.y -= WinRect.top;

	int nWidth = WinRect.right - WinRect.left;
	int nHeight = WinRect.bottom - WinRect.top;

	int BorderLeftWidth = 10, BorderLeftHeight = 10, BorderBottomHeight = 10, BorderRightWidth = 10, BorderTopHeight = 10;
	SetRect(&rect, 0, 0, BorderLeftWidth, BorderTopHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTTOPLEFT;

	SetRect(&rect, nWidth - BorderLeftWidth, 0, nWidth, BorderTopHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTTOPRIGHT;

	SetRect(&rect, 0, nHeight - BorderBottomHeight, BorderLeftWidth, nHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTBOTTOMLEFT;

	SetRect(&rect, nWidth - BorderRightWidth, nHeight - BorderBottomHeight, nWidth, nHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTBOTTOMRIGHT;

	SetRect(&rect, 0, BorderTopHeight, BorderLeftWidth, nHeight - BorderBottomHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTLEFT;

	SetRect(&rect, nWidth - BorderRightWidth, BorderTopHeight, nWidth, nHeight - BorderBottomHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTRIGHT;

	SetRect(&rect, BorderLeftWidth, nHeight - BorderBottomHeight, nWidth - BorderRightWidth, nHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTBOTTOM;

	SetRect(&rect, BorderLeftWidth, 0, nWidth - BorderRightWidth, BorderBottomHeight);
	if (PtInRect(&rect, point) && m_bSizable)  //!IsZoomed(m_hWnd) )
		return HTTOP;

	return HTTOP;
}

VOID CSShadow::OnNcCalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	NCCALCSIZE_PARAMS* pParams = NULL;
	RECT* pRect = NULL;

	BOOL bValue = static_cast<BOOL>(wParam);
	if(bValue) pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
	else pRect = reinterpret_cast<RECT*>(lParam);

	if(bValue)
		pRect = &pParams->rgrc[0];

	if(bValue)
	{
		pRect->left = pRect->left + 10;
		pRect->top = pRect->top + 10;
		pRect->right = pRect->right + 10;
		pRect->bottom = pRect->bottom + 10;
		
		pParams->rgrc[1] = pParams->rgrc[0];
	}
	else DefWindowProc(hWnd, message, wParam, lParam);
}

VOID CSShadow::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);

	RECT WinRect, rect;
	GetWindowRect(hWnd, &WinRect);

	int width = WinRect.right-WinRect.left;
	int height = WinRect.bottom-WinRect.top;

	//point.x -= WinRect.left;
	//point.y -= WinRect.top;

	SetRect( &rect, 0, 0, BorderSize, BorderSize );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTTOPLEFT;
		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		return;
	}
	SetRect( &rect, width - BorderSize, 0,  width, BorderSize  );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTTOPRIGHT;
		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		return;
	}
	SetRect( &rect, 0, height - BorderSize, BorderSize, height );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTBOTTOMLEFT;
		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		return;
	}
	SetRect( &rect, width-BorderSize,height-BorderSize,  width, height );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTBOTTOMRIGHT;
		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		return;
	}
	SetRect( &rect, 0, BorderSize,  BorderSize, height-BorderSize  );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTLEFT;
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		return;
	}
	SetRect( &rect, width-BorderSize, BorderSize,  width, height-BorderSize  );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTR;
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		return;
	}
	SetRect( &rect, BorderSize, height-BorderSize, width-BorderSize, height );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTBOTTOM;
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return;
	}
	SetRect( &rect, BorderSize, 0,  width-BorderSize, BorderSize );
	if ( PtInRect( &rect, point ) && m_bSizable )  //!IsZoomed(m_hWnd) )
	{
		//return HTTOP;
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		return;
	}
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

VOID CSShadow::OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT WindowRect;
	GetWindowRect(hWnd, &WindowRect);
	OffsetRect(&WindowRect, -WindowRect.left, -WindowRect.top);

	int nWidth, nHeight;
	nWidth = WindowRect.right;
	nHeight = WindowRect.bottom;

	HRGN hRgn;
	hRgn = CreateRectRgn(0, 0, nWidth, nHeight);
	SetWindowRgn(hWnd, hRgn, false);

	GetWindowRect(hWnd, &WindowRect);
	SetWindowPos(m_hWnd_parent, 0, WindowRect.left + 10, WindowRect.top + 10, WindowRect.right - WindowRect.left - 20, WindowRect.bottom - WindowRect.top - 20, SWP_NOZORDER | SWP_NOACTIVATE);
}

VOID CSShadow::OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*
	HDC hDC = GetWindowDC(hWnd);

	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	HFONT hOldFont;
	hBrush = CreateSolidBrush(0xff0000);
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	hPen = CreatePen(PS_SOLID, 1, 0xff0000);
	hOldPen = (HPEN)SelectObject(hDC, hPen);

	SetBkMode(hDC, OPAQUE);
	Rectangle(hDC, 0, 0, 1000, 1000);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
	*/
	Draw();
}