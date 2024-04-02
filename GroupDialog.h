#pragma once


// CGroupDialog dialog

class CGroupDialog : public CDialog
{
	DECLARE_DYNAMIC(CGroupDialog)

public:
	CGroupDialog(CString strTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CGroupDialog();

// Dialog Data
	enum { IDD = IDD_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString m_strWindowTitle;

public:
	CString m_strGroup;

	virtual BOOL OnInitDialog();
};
