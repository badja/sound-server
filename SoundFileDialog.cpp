// SoundFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SoundServer.h"
#include "SoundFileDialog.h"
#include ".\soundfiledialog.h"


// CSoundFileDialog

IMPLEMENT_DYNAMIC(CSoundFileDialog, CFileDialog)
CSoundFileDialog::CSoundFileDialog(LPCTSTR lpszFileName, CWnd* pParentWnd) :
		CFileDialog(TRUE, _T("wav"), lpszFileName,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_EXPLORER,
			_T("Sounds (*.wav)|*.wav|All Files (*.*)|*.*||"), pParentWnd)
	, m_lpszBuffer(NULL)
	, m_nLastLen(0)
{
}

CSoundFileDialog::~CSoundFileDialog()
{
	if (m_lpszBuffer)
		delete [] m_lpszBuffer;
}


BEGIN_MESSAGE_MAP(CSoundFileDialog, CFileDialog)
END_MESSAGE_MAP()



// CSoundFileDialog message handlers


void CSoundFileDialog::OnFileNameChange()
{
	int nLength;
	nLength = CommDlg_OpenSave_GetSpec(GetParent()->GetSafeHwnd(), NULL, 0);

	nLength += _MAX_PATH;

	// The OFN struct is stored in a property of dialog window

	if ((int)GetOFN().nMaxFile < nLength)  
	{
		// Free the previously allocated buffer.
		if (m_lpszBuffer)
			delete [] m_lpszBuffer;

		// Allocate a new buffer
		m_lpszBuffer = (LPTSTR) new TCHAR[nLength];
		memset(m_lpszBuffer, 0, sizeof(m_lpszBuffer));

		if (m_lpszBuffer)
		{
			GetOFN().lpstrFile = m_lpszBuffer;
			GetOFN().nMaxFile  = nLength;
		}
	}

	CFileDialog::OnFileNameChange();
}
