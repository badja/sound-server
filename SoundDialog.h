#pragma once


// CSoundDialog dialog

class CSoundDialog : public CDialog
{
	DECLARE_DYNAMIC(CSoundDialog)

public:
	CSoundDialog(CString strTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSoundDialog();

// Dialog Data
	enum { IDD = IDD_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString m_strWindowTitle;

public:
	CString m_strSound;
	CString m_strFile;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBrowse();
};
