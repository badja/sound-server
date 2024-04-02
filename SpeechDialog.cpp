// SpeechDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SoundServer.h"
#include "SpeechDialog.h"
#include ".\speechdialog.h"


// CSpeechDialog dialog

IMPLEMENT_DYNAMIC(CSpeechDialog, CDialog)
CSpeechDialog::CSpeechDialog(CString strTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CSpeechDialog::IDD, pParent)
	, m_strWindowTitle(strTitle)
	, m_strText(_T(""))
{
}

CSpeechDialog::~CSpeechDialog()
{
}

void CSpeechDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT, m_strText);
}


BEGIN_MESSAGE_MAP(CSpeechDialog, CDialog)
	ON_BN_CLICKED(IDC_TEST, OnBnClickedTest)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
END_MESSAGE_MAP()


// CSpeechDialog message handlers

BOOL CSpeechDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strWindowTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeechDialog::OnBnClickedTest()
{
	UpdateData();

	theApp.SpeakText(m_strText);
}

void CSpeechDialog::OnBnClickedStop()
{
	theApp.StopSpeak();
}
