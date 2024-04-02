// GroupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SoundServer.h"
#include "GroupDialog.h"
#include ".\groupdialog.h"


// CGroupDialog dialog

IMPLEMENT_DYNAMIC(CGroupDialog, CDialog)
CGroupDialog::CGroupDialog(CString strTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CGroupDialog::IDD, pParent)
	, m_strWindowTitle(strTitle)
	, m_strGroup(_T(""))
{
}

CGroupDialog::~CGroupDialog()
{
}

void CGroupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_strGroup);
}


BEGIN_MESSAGE_MAP(CGroupDialog, CDialog)
END_MESSAGE_MAP()


// CGroupDialog message handlers

BOOL CGroupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strWindowTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
