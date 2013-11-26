#pragma once
#include "csfactory.h"

class CSWindowFactory :	public CSFactory
{
public:
	CSWindowFactory(void);
	~CSWindowFactory(void);

public:
	virtual CSControl* Create(HWND hWnd);
};

