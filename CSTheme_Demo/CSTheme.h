#pragma once

#include "CSControl.h"


class CSTheme
{
public:
	CSTheme(void);
	~CSTheme(void);

public:
	enum CSType {
		Window,
		Button,
		TextBox
	};

protected:
	typedef std::map<HWND, CSControl*> ControlContainer;
	typedef std::map<HWND, CSControl*>::iterator ControlIterator;

	ControlContainer m_Controls;

public:
	CSControl& GetCSControl(HWND hWnd);
	CSControl* AddControl(DWORD dwControlType, HWND hWnd);


};

