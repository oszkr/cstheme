#pragma once
#include "CSControl.h"

class CSFactory
{
public:
	CSFactory(void);
	~CSFactory(void);

public:
	virtual CSControl* Create(HWND hWnd) = 0;
};

