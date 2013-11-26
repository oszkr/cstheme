#include "stdafx.h"
#include "CSFactory.h"
#include "CSWindowFactory.h"
#include "CSTheme.h"
#include "CSWindow.h"

CSTheme::CSTheme(void)
{
}


CSTheme::~CSTheme(void)
{
}

CSControl* CSTheme::AddControl(DWORD dwControlType, HWND hWnd)
{
	CSFactory* pFactory = NULL;
	CSControl* pControl = NULL;

	if(&GetCSControl(hWnd))
		return 0;

	if(dwControlType == CSType::Window)
		pFactory = new CSWindowFactory;


	pControl = pFactory->Create(hWnd);

	m_Controls.insert(std::make_pair(hWnd, pControl));

	return pControl;
}

CSControl& CSTheme::GetCSControl(HWND hWnd)
{
	ControlIterator ControlIter = m_Controls.find(hWnd);
	if(ControlIter != m_Controls.end())
		return *ControlIter->second;
	else
		return *(CSControl*)0;
	
}