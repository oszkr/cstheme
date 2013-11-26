
// CSTheme_DemoDlg.h : header file
//

#pragma once

#include "CSTheme.h"
#include "CSWindow.h"

// CCSTheme_DemoDlg dialog
class CCSTheme_DemoDlg : public CDialogEx, private CSWindowReceiver
{
// Construction
public:
	CCSTheme_DemoDlg(CWnd* pParent = NULL);	// standard constructor
	virtual UINT CSWindowReceive(UINT nEvent);
// Dialog Data
	enum { IDD = IDD_CSTHEME_DEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CSTheme m_CSTheme;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
