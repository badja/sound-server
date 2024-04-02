#pragma once


// CSoundFileDialog

class CSoundFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CSoundFileDialog)

public:
	CSoundFileDialog(
		LPCTSTR lpszFileName = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CSoundFileDialog();

protected:
	DECLARE_MESSAGE_MAP()

	LPTSTR m_lpszBuffer;
	int m_nLastLen;

	virtual void OnFileNameChange();
};


