#pragma once


// CSpeechDialog dialog

class CSpeechDialog : public CDialog
{
	DECLARE_DYNAMIC(CSpeechDialog)

public:
	CSpeechDialog(CString strTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpeechDialog();

// Dialog Data
	enum { IDD = IDD_SPEECH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString m_strWindowTitle;

public:
	CString m_strText;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedTest();
	afx_msg void OnBnClickedStop();
};
