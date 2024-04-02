// ConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SoundServer.h"
#include "ConfigurationDialog.h"
#include ".\configurationdialog.h"

#include "MainFrm.h"


// CConfigurationDialog dialog

IMPLEMENT_DYNAMIC(CConfigurationDialog, CDialog)
CConfigurationDialog::CConfigurationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigurationDialog::IDD, pParent)
	, m_strPort(_T("2000"))
	, m_nSockType(SOCK_TCP)
{
}

CConfigurationDialog::~CConfigurationDialog()
{
}

void CConfigurationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SVR_PORTINC, m_ctlPortInc);
	DDX_Control(pDX, IDC_SERVER, m_ctlServer);
	DDX_Text(pDX, IDC_SVR_PORT, m_strPort);
	DDX_Radio(pDX, IDC_TCP, m_nSockType);
	DDX_Text(pDX, IDC_SERVER, m_strServer);
}


BEGIN_MESSAGE_MAP(CConfigurationDialog, CDialog)
	ON_BN_CLICKED(IDC_REFRESH_MASTERFOLDER, OnBnClickedRefreshMasterFolder)
	ON_BN_CLICKED(IDC_DELETE_MISSINGSOUNDS, OnBnClickedDeleteMissingSounds)
END_MESSAGE_MAP()


// CConfigurationDialog message handlers

BOOL CConfigurationDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlPortInc.SetRange32( 2000, 4500);
	m_ctlServer.EnableWindow(!theApp.IsServer());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigurationDialog::OnBnClickedRefreshMasterFolder()
{
	((CMainFrame*)AfxGetMainWnd())->RefreshMasterFolder();
}

void CConfigurationDialog::OnBnClickedDeleteMissingSounds()
{
	((CMainFrame*)AfxGetMainWnd())->DeleteMissingSounds();
}
