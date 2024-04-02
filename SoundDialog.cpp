// SoundDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SoundServer.h"
#include "SoundDialog.h"
#include ".\sounddialog.h"

#include "SoundFileDialog.h"


// CSoundDialog dialog

IMPLEMENT_DYNAMIC(CSoundDialog, CDialog)
CSoundDialog::CSoundDialog(CString strTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CSoundDialog::IDD, pParent)
	, m_strWindowTitle(strTitle)
	, m_strSound(_T(""))
	, m_strFile(_T(""))
{
}

CSoundDialog::~CSoundDialog()
{
}

void CSoundDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_strSound);
	DDX_Text(pDX, IDC_FILE, m_strFile);
}


BEGIN_MESSAGE_MAP(CSoundDialog, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
END_MESSAGE_MAP()


// CSoundDialog message handlers

BOOL CSoundDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strWindowTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSoundDialog::OnBnClickedBrowse()
{
	UpdateData(TRUE);

	CSoundFileDialog dialog(m_strFile, this);

	if (dialog.DoModal() != IDOK)
		return;

	m_strFile = dialog.GetPathName();

	UpdateData(FALSE);
}
