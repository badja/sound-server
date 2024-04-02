#pragma once
#include "afxcmn.h"


// CConfigurationDialog dialog

class CConfigurationDialog : public CDialog
{
	DECLARE_DYNAMIC(CConfigurationDialog)

public:
	CConfigurationDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigurationDialog();

// Dialog Data
	enum { IDD = IDD_CONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl	m_ctlPortInc;
	CEdit	m_ctlServer;
	CString	m_strPort;
	int	m_nSockType;
	CString m_strServer;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRefreshMasterFolder();
	afx_msg void OnBnClickedDeleteMissingSounds();
};
