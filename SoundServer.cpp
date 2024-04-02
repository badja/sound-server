// SoundServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SoundServer.h"
#include "MainFrm.h"

#include "SoundServerDoc.h"
#include "LeftView.h"
#include "SoundServerView.h"
#include "SendKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WSA_VERSION  MAKEWORD(2,0)

#define DOCTALKER_DEFAULT_FILE_NAME	_T("Z:\\Temp\\agrucza\\DOSBoxPortable\\DOSBoxPortable.exe")
#define DOCTALKER_ARGUMENTS			_T("-noconsole")
#define DOCTALKER_CLASS_NAME		_T("SDL_app")

// CSoundServerApp

BEGIN_MESSAGE_MAP(CSoundServerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CSoundServerApp construction

CSoundServerApp::CSoundServerApp()
{
	m_nLinkMode = 1; // client
	m_bHideOnStartup = FALSE;
	m_strPort = _T("2000");
	m_nSockType = SOCK_TCP;	// default TCP
	m_fPlaying = FALSE;
	m_fFileOpen = FALSE;
	m_startPos = 0;
	m_rc = 0;
	m_hmmioIn = NULL;
	m_dataOffset = 0;
	m_audioLength = 0;
	m_bufferSize = 0;
	m_hWaveOut = NULL;
	m_bPlayOnClose = FALSE;
	m_pVoice = NULL;
	m_bUseDocTalker = FALSE;
	m_nVoice = 1;

    for (int i = 0; i < NUM_BUFFERS; i++)
		m_pmem[i] = NULL;
}


// The one and only CSoundServerApp object

CSoundServerApp theApp;

// CSoundServerApp initialization

BOOL CSoundServerApp::InitInstance()
{
	WSADATA		WSAData = { 0 };
	if ( 0 != WSAStartup( WSA_VERSION, &WSAData ) )
	{
		// Tell the user that we could not find a usable
		// WinSock DLL.
		if ( LOBYTE( WSAData.wVersion ) != LOBYTE(WSA_VERSION) ||
			 HIBYTE( WSAData.wVersion ) != HIBYTE(WSA_VERSION) )
			 ::MessageBox(NULL, _T("Incorrect version of Winsock.dll found"), _T("Error"), MB_OK);

		WSACleanup( );
		return FALSE;
	}

	CoInitialize(NULL);
	CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&m_pVoice);

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	ParseCommandLineArgs();
	::srand((unsigned)time(NULL));
	m_ClientSocketManager.GetLocalAddress(m_strServer.GetBuffer(256), 256);
	m_strServer.ReleaseBuffer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("Adrian Grucza"));
	ReadRegistrySettings();
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSoundServerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLeftView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	if (IsServer())
		SetVolume(DEFAULT_VOLUME);
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(m_bHideOnStartup ? SW_HIDE : SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	m_bUseDocTalker = (GetFileAttributes(GetDocTalkerFileName()) != INVALID_FILE_ATTRIBUTES);
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CSoundServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CSoundServerApp message handlers


void CSoundServerApp::ParseCommandLineArgs()
{
	CString strCmdLine = (LPCTSTR) GetCommandLine();

	if (!strCmdLine.IsEmpty())
	{
		strCmdLine.MakeUpper();

		int nPos = 0;
		do {
			nPos = strCmdLine.Find(TCHAR(' '));
			if (nPos>0)
			{
				strCmdLine.Delete( 0, nPos+1);
				CString strCurrent = strCmdLine;
				int nNextPos = strCmdLine.Find(TCHAR(' '));
				if (nNextPos > 0)
					strCurrent = strCmdLine.Left( nNextPos );

				if (strCurrent == _T("/SERVER") || strCurrent == _T("/S"))
					m_nLinkMode = 0;
				else if (strCurrent == _T("/CLIENT") || strCurrent == _T("/C"))
					m_nLinkMode = 1;
				else if (strCurrent == _T("/HIDE") || strCurrent == _T("/H"))
					m_bHideOnStartup = TRUE;
			}
		} while( nPos != -1);
	}
}

void CSoundServerApp::CloseFile()
{
    mmioClose(m_hmmioIn, 0);
    m_fFileOpen = FALSE;
}

BOOL CSoundServerApp::OpenFile(CString soundfile)
{
	MMCKINFO mmckinfoParentIn = {0};
    MMCKINFO mmckinfoSubchunkIn = {0};
    MMIOINFO mmioinf = {0};
    

    // close previously open file (if any)
    CloseFile();
    
    if (soundfile.IsEmpty())
		return TRUE;
        
    // Open the input file
    m_hmmioIn = mmioOpen(soundfile.GetBuffer(), &mmioinf, MMIO_READ);
    if (m_hmmioIn == NULL)
	{
        TRACE(_T("Error opening input file, rc = %d\n"), mmioinf.wErrorRet);
		return FALSE;
	}

    // Check if this is a wave file
    mmckinfoParentIn.fccType = mmioStringToFOURCC(_T("WAVE"), 0);
    m_rc = mmioDescend(m_hmmioIn, &mmckinfoParentIn, NULL, MMIO_FINDRIFF);
    if (m_rc != MMSYSERR_NOERROR)
	{
        CloseFile();
        TRACE(_T("Not a wave file\n"));
		return FALSE;
	}

    // Get format info
    mmckinfoSubchunkIn.ckid = mmioStringToFOURCC(_T("fmt"), 0);
    m_rc = mmioDescend(m_hmmioIn, &mmckinfoSubchunkIn, &mmckinfoParentIn, MMIO_FINDCHUNK);
    if (m_rc != MMSYSERR_NOERROR)
	{
        CloseFile();
        TRACE(_T("Couldn't get format chunk\n"));
		return FALSE;
	}
    m_rc = mmioRead(m_hmmioIn, (HPSTR)&m_format, min(mmckinfoSubchunkIn.cksize, sizeof(m_format)));
    if (m_rc == -1)
	{
        CloseFile();
        TRACE(_T("Error reading format\n"));
		return FALSE;
	}
    m_rc = mmioAscend(m_hmmioIn, &mmckinfoSubchunkIn, 0);
    
    // Find the data subchunk
    mmckinfoSubchunkIn.ckid = mmioStringToFOURCC(_T("data"), 0);
    m_rc = mmioDescend(m_hmmioIn, &mmckinfoSubchunkIn, &mmckinfoParentIn, MMIO_FINDCHUNK);
    if (m_rc != MMSYSERR_NOERROR)
	{
        CloseFile();
        TRACE(_T("Couldn't get data chunk\n"));
		return FALSE;
	}
    m_dataOffset = mmioSeek(m_hmmioIn, 0, SEEK_CUR);
    
    // Get the length of the audio
    m_audioLength = mmckinfoSubchunkIn.cksize;
    
    // Allocate audio buffers
    m_bufferSize = m_format.nSamplesPerSec * m_format.nBlockAlign * m_format.nChannels / BUFFER_SECONDS_INV;
    m_bufferSize -= m_bufferSize % m_format.nBlockAlign;
    for (int i = 0; i < NUM_BUFFERS; i++)
	{
        delete m_pmem[i];
		m_pmem[i] = new char[m_bufferSize];
	}
    
    m_fFileOpen = TRUE;

	return TRUE;
}

BOOL CSoundServerApp::PlaySound(CString strFile)
{
	m_strSoundFile = strFile;

	StopAll();

	if (Playing())
	{
		m_bPlayOnClose = TRUE;
		return TRUE;
	}

	if (!OpenFile(strFile))
		return FALSE;

	m_rc = waveOutOpen(&m_hWaveOut, 0, &m_format, (DWORD_PTR)AfxGetMainWnd()->GetSafeHwnd(), 0, CALLBACK_WINDOW);

	if (m_rc != MMSYSERR_NOERROR)
	{
		waveOutGetErrorText(m_rc, m_msg.GetBuffer(MAXERRORLENGTH), MAXERRORLENGTH);
		m_msg.ReleaseBuffer();
		TRACE("%s\n", m_msg);
		return FALSE;
	}

	for (int i = 0; i < NUM_BUFFERS; i++)
	{
		m_hdr[i].lpData = m_pmem[i];
		m_hdr[i].dwBufferLength = m_bufferSize;
		m_hdr[i].dwFlags = 0;
		m_hdr[i].dwLoops = 0;
		m_rc = waveOutPrepareHeader(m_hWaveOut, &m_hdr[i], sizeof(WAVEHDR));
		if (m_rc != MMSYSERR_NOERROR)
		{
			waveOutGetErrorText(m_rc, m_msg.GetBuffer(MAXERRORLENGTH), MAXERRORLENGTH);
			m_msg.ReleaseBuffer();
			TRACE("%s\n", m_msg);
		}
	}

	m_fPlaying = TRUE;

	m_startPos = mmioSeek(m_hmmioIn, 0, SEEK_CUR) - m_dataOffset;

	for (int i = 0; i < NUM_BUFFERS; i++)
		AfxGetMainWnd()->PostMessage(MM_WOM_DONE, 0, (LPARAM)&m_hdr[i]);

	return TRUE;
}

void CSoundServerApp::StopPlay()
{
    m_fPlaying = FALSE;
    FileSeek(Position());
    waveOutReset(m_hWaveOut);
}

BOOL CSoundServerApp::FileSeek(LONG lPosition)
{
    LONG bytepos;
    bytepos = lPosition * m_format.nBlockAlign;
    if (!m_fFileOpen || bytepos < 0 || bytepos >= (LONG)m_audioLength)
		return FALSE;
    m_rc = mmioSeek(m_hmmioIn, bytepos + m_dataOffset, SEEK_SET);
    m_startPos = m_rc;
    return m_rc == MMSYSERR_NOERROR;
}

LONG CSoundServerApp::Position()
{
	if (m_hWaveOut == NULL)
		return 0;

	MMTIME tm = {0};
    tm.wType = TIME_BYTES;
    m_rc = waveOutGetPosition(m_hWaveOut, &tm, sizeof(MMTIME));
    if (m_rc == MMSYSERR_NOERROR)
        return (m_startPos + tm.u.cb) / m_format.nBlockAlign;
	else
        return (mmioSeek(m_hmmioIn, 0, SEEK_CUR) - m_dataOffset + m_bufferSize * NUM_BUFFERS) / m_format.nBlockAlign;
}

BOOL CSoundServerApp::Playing()
{
    MMTIME tm = {0};
    tm.wType = TIME_BYTES;
    m_rc = waveOutGetPosition(m_hWaveOut, &tm, sizeof(MMTIME));
    return m_rc == MMSYSERR_NOERROR;
}

void CSoundServerApp::OnMmWomClose(HWAVEOUT hOutputDev)
{
	if (m_bPlayOnClose)
	{
		m_bPlayOnClose = FALSE;
		PlaySound(m_strSoundFile);
	}
	else
	{
		CloseFile();
	}
}

void CSoundServerApp::OnMmWomDone(HWAVEOUT hOutputDev, WAVEHDR* lpwvhdr)
{
	static LONG dataRemaining;
	if (m_fPlaying)
	{
		dataRemaining = (m_dataOffset + m_audioLength - mmioSeek(m_hmmioIn, 0, SEEK_CUR));
		if ((LONG)m_bufferSize < dataRemaining)
		{
			m_rc = mmioRead(m_hmmioIn, lpwvhdr->lpData, m_bufferSize);
		}
		else
		{
			m_rc = mmioRead(m_hmmioIn, lpwvhdr->lpData, dataRemaining);
			m_fPlaying = FALSE;
		}
		lpwvhdr->dwBufferLength = m_rc;
		m_rc = waveOutWrite(m_hWaveOut, lpwvhdr, sizeof(WAVEHDR));
	}
	else
	{
		for (int i = 0; i < NUM_BUFFERS; i++)
			waveOutUnprepareHeader(m_hWaveOut, &m_hdr[i], sizeof(WAVEHDR));
		waveOutClose(m_hWaveOut);
	}
}

int CSoundServerApp::ExitInstance()
{
	StopAll();

	CWnd* pWnd = GetDocTalkerWindow();
	if (pWnd)
		pWnd->PostMessage(WM_CLOSE);

    for (int i = 0; i < NUM_BUFFERS; i++)
        delete m_pmem[i];

	if (m_nLinkMode == 0)
	{
		// Disconnect all clients
		for(int i=0; i<MAX_CONNECTION; i++)
			m_SocketManager[i].StopComm();
	}
	else
	{
		// Disconnect socket
		m_ClientSocketManager.StopComm();
	}

	// Terminate use of the WS2_32.DLL
	WSACleanup();

	m_pVoice->Release();
	m_pVoice = NULL;

	CoUninitialize();

	return CWinApp::ExitInstance();
}

///////////////////////////////////////////////////////////////////////////////
// PickNextAvailable : this is useful only for TCP socket
void CSoundServerApp::PickNextAvailable()
{
	m_pCurServer = NULL;
	for(int i=0; i<MAX_CONNECTION; i++)
	{
		if (!m_SocketManager[i].IsOpen())
		{
			m_pCurServer = &m_SocketManager[i];
			break;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// StartServer : Start the server
bool CSoundServerApp::StartServer()
{
	bool bSuccess = false;
	if (m_pCurServer != NULL)
	{
		if (m_nSockType == SOCK_TCP)
		{
			// no smart addressing - we use connection oriented
			m_pCurServer->SetSmartAddressing( false );
			bSuccess = m_pCurServer->CreateSocket( m_strPort, AF_INET, SOCK_STREAM, 0); // TCP
		}
		else
		{
			m_pCurServer->SetSmartAddressing( true );
			bSuccess = m_pCurServer->CreateSocket( m_strPort, AF_INET, SOCK_DGRAM, SO_BROADCAST); // UDP
		}

		if (bSuccess)
			bSuccess = m_pCurServer->WatchComm();
	}

	return bSuccess;
}

BOOL CSoundServerApp::ClientConnect() 
{
	ASSERT(!IsServer());

	bool bSuccess;
	CWaitCursor wc;
	if (m_nSockType == SOCK_TCP)
	{
		bSuccess = m_ClientSocketManager.ConnectTo( m_strServer, m_strPort, AF_INET, SOCK_STREAM); // TCP
	}
	else
	{
		bSuccess = m_ClientSocketManager.ConnectTo( m_strServer, m_strPort, AF_INET, SOCK_DGRAM); // UDP
	}

	if (bSuccess)
		bSuccess = m_ClientSocketManager.WatchComm();

	return bSuccess;
}

void CSoundServerApp::ClientDisconnect() 
{
	ASSERT(!IsServer());

	m_ClientSocketManager.StopComm();
}

void CSoundServerApp::ClientSend(CString strText) 
{
	if (!m_ClientSocketManager.IsOpen())
		return;

	BYTE byBuffer[BUFFER_SIZE] = { 0 };
	strText = strText.Left(BUFFER_SIZE - 1);

	int nLen = strText.GetLength();

	if (nLen > 0)
	{
		nLen++;
		USES_CONVERSION;
		strcpy_s((LPSTR)byBuffer, BUFFER_SIZE, T2CA(strText));
		m_ClientSocketManager.WriteComm( byBuffer, nLen, INFINITE);
	}
}

BOOL CSoundServerApp::ServerUpdateConnection(UINT uEvent, CSocketManager* pManager)
{
	ASSERT(IsServer());

	// We need to do this only for TCP socket
	if (m_nSockType != SOCK_TCP)
		return FALSE;

	if ( pManager != NULL)
	{
		// Server socket is now connected, we need to pick a new one
		if (uEvent == EVT_CONSUCCESS)
		{
			PickNextAvailable();
			StartServer();
		}
		else if (uEvent == EVT_CONFAILURE || uEvent == EVT_CONDROP)
		{
			pManager->StopComm();
			if (m_pCurServer == NULL)
			{
				PickNextAvailable();
				StartServer();
			}
		}
	}

	return TRUE;
}

void CSoundServerApp::ClientSendData(EAction eAction, const CAction& action)
{
	ASSERT(!IsServer());

	ClientSend(CreateCommandString(eAction, action));
}

CString CSoundServerApp::CreateCommandString(EAction eAction, const CAction& action)
{
	CString strData;
	strData.Format(_T("Action=%d"), eAction);

	AddParameter(strData, _T("Group"), action.m_strGroup);
	AddParameter(strData, _T("Sound"), action.m_strSound);
	AddParameter(strData, _T("File"), action.m_strFile);
	AddParameter(strData, _T("Data"), action.m_dwData);

	if (action.m_pActionNew != NULL)
	{
		AddParameter(strData, _T("Group2"), action.m_pActionNew->m_strGroup);
		AddParameter(strData, _T("Sound2"), action.m_pActionNew->m_strSound);
		AddParameter(strData, _T("File2"), action.m_pActionNew->m_strFile);
		AddParameter(strData, _T("Data2"), action.m_pActionNew->m_dwData);
	}

	return strData;
}

void CSoundServerApp::AddParameter(CString& strData, CString strName, CString strValue)
{
	if (!strValue.IsEmpty())
		strData.AppendFormat(_T("&%s=%s"), strName, strValue);
}

void CSoundServerApp::AddParameter(CString& strData, CString strName, DWORD_PTR dwValue)
{
	if (dwValue != (DWORD_PTR)-1)
		strData.AppendFormat(_T("&%s=%ld"), strName, dwValue);
}

EAction CSoundServerApp::ParseData(CString strData, CAction& action)
{
	EAction eAction = eInvalid;
	CString strToken, strName, strValue;
	int nCurPos = 0;

	strToken = strData.Tokenize(_T("&"), nCurPos);
	while (!strToken.IsEmpty())
	{
		int nParamPos = 0;
		strName = strToken.Tokenize(_T("="), nParamPos);
		strValue = strToken.Tokenize(_T("="), nParamPos);

		ProcessParameter(strName, strValue, eAction, action);

		strToken = strData.Tokenize(_T("&"), nCurPos);
	}

	return eAction;
}

void CSoundServerApp::ProcessParameter(CString strName, CString strValue, EAction& eAction, CAction& action)
{
	if (strName.CompareNoCase(_T("Action")) == 0)
		eAction = (EAction)_ttoi(strValue);
	else if (strName.CompareNoCase(_T("Group")) == 0)
		action.m_strGroup = strValue;
	else if (strName.CompareNoCase(_T("Sound")) == 0)
		action.m_strSound = strValue;
	else if (strName.CompareNoCase(_T("File")) == 0)
		action.m_strFile = strValue;
	else if (strName.CompareNoCase(_T("Data")) == 0)
		action.m_dwData = _ttoi(strValue);
	else if (strName.CompareNoCase(_T("Group2")) == 0)
		action.m_pActionNew->m_strGroup = strValue;
	else if (strName.CompareNoCase(_T("Sound2")) == 0)
		action.m_pActionNew->m_strSound = strValue;
	else if (strName.CompareNoCase(_T("File2")) == 0)
		action.m_pActionNew->m_strFile = strValue;
	else if (strName.CompareNoCase(_T("Data2")) == 0)
		action.m_pActionNew->m_dwData = _ttoi(strValue);
}

void CSoundServerApp::InitialiseSockets(CWnd* pMsgWnd)
{
	if (m_nLinkMode == 0)
	{
		for(int i=0; i<MAX_CONNECTION; i++)
		{
			m_SocketManager[i].SetMessageWindow( pMsgWnd );
			m_SocketManager[i].SetServerState( true );	// run as server
		}

		PickNextAvailable();
		StartServer(); // Server
	}
	else
	{
		// Initialize socket manager
		m_ClientSocketManager.SetMessageWindow( pMsgWnd );
		m_ClientSocketManager.SetServerState( false );	// run as client
		m_ClientSocketManager.SetSmartAddressing( false );	// always send to server
		ClientConnect();
	}
}

void CSoundServerApp::ServerSendData(EAction eAction, const CAction& action, CSocketManager* pClient /*= NULL*/)
{
	ASSERT(IsServer());

	ServerSend(CreateCommandString(eAction, action), pClient);
}

void CSoundServerApp::ServerSend(CString strText, CSocketManager* pClient /*= NULL*/) 
{
	char szBuffer[BUFFER_SIZE] = { 0 };
	strText = strText.Left(BUFFER_SIZE - 1);
	int nLen = strText.GetLength();

	if (nLen > 0)
	{
		USES_CONVERSION;
		if (m_nSockType == SOCK_UDP)
		{
			SockAddrIn sin;
			// send broadcast...
			sin.CreateFrom(_T("255.255.255.255"), m_strPort);
			memcpy(szBuffer, (LPSOCKADDR) sin, sin.Size());
			// memory overflow check...(BUFFER_SIZE bytes max)
			nLen = __min(BUFFER_SIZE-(int)sin.Size(), nLen);
			strncpy_s((LPSTR)&szBuffer[sin.Size()], BUFFER_SIZE, T2CA(strText), nLen);
			nLen += (int)sin.Size();
		}
		else
		{
			nLen = __min(BUFFER_SIZE, nLen);
			strncpy_s((LPSTR)szBuffer, BUFFER_SIZE, T2CA(strText), nLen);
			nLen++;
		}

		// Send data to peer...
		if (m_nSockType == SOCK_UDP)
			m_pCurServer->WriteComm((BYTE*)szBuffer, nLen, INFINITE);
		else
		{
			if (pClient)
			{
				// Send to single client
				if (pClient->IsOpen())
					pClient->WriteComm((BYTE*)szBuffer, nLen, INFINITE);
			}
			else
			{
				// Send to all clients
				for(int i=0; i<MAX_CONNECTION; i++)
				{
					if (m_SocketManager[i].IsOpen() && m_pCurServer != &m_SocketManager[i])
						m_SocketManager[i].WriteComm((BYTE*)szBuffer, nLen, INFINITE);
				}
			}
		}
	}	
}

void CSoundServerApp::WriteRegistrySettings()
{
	CRegKey rkSettings;
	rkSettings.Open(GetAppRegistryKey(), REG_KEY_SETTINGS);
	rkSettings.SetStringValue(REG_VALUENAME_PORT, m_strPort);
	rkSettings.SetDWORDValue(REG_VALUENAME_SOCKTYPE, m_nSockType);
	if (!IsServer())
		rkSettings.SetStringValue(REG_VALUENAME_SERVER, m_strServer);
	if (IsServer())
		rkSettings.SetStringValue(REG_VALUENAME_MASTERFOLDER, m_strMasterFolder);
}

void CSoundServerApp::ReadRegistrySettings()
{
	CRegKey rkSettings;
	LONG lReturn;
	ULONG nChars;
	CString strValue;
	DWORD dwValue;

	if (rkSettings.Open(GetAppRegistryKey(), REG_KEY_SETTINGS, KEY_READ) != ERROR_SUCCESS)
		return;

	nChars = REG_LIMIT_VALUE + 1;
	lReturn = rkSettings.QueryStringValue(REG_VALUENAME_PORT, strValue.GetBuffer(REG_LIMIT_VALUE), &nChars);
	strValue.ReleaseBuffer(nChars - 1);
	if (lReturn == ERROR_SUCCESS)
		m_strPort = strValue;

	if (rkSettings.QueryDWORDValue(REG_VALUENAME_SOCKTYPE, dwValue) == ERROR_SUCCESS)
		m_nSockType = dwValue;

	if (!IsServer())
	{
		nChars = REG_LIMIT_VALUE + 1;
		lReturn = rkSettings.QueryStringValue(REG_VALUENAME_SERVER, strValue.GetBuffer(REG_LIMIT_VALUE), &nChars);
		strValue.ReleaseBuffer(nChars - 1);
		if (lReturn == ERROR_SUCCESS)
			m_strServer = strValue;
	}

	if (IsServer())
	{
		nChars = REG_LIMIT_VALUE + 1;
		lReturn = rkSettings.QueryStringValue(REG_VALUENAME_MASTERFOLDER, strValue.GetBuffer(REG_LIMIT_VALUE), &nChars);
		strValue.ReleaseBuffer(nChars - 1);
		if (lReturn == ERROR_SUCCESS)
			m_strMasterFolder = strValue;
	}
}

BOOL CSoundServerApp::ClientIsConnected()
{
	ASSERT(!IsServer());

	return m_ClientSocketManager.IsOpen();
}

void CSoundServerApp::SetVolume(int nVolume)
{
	WORD wChannelVolume = nVolume * 0xffff / 100;
	waveOutSetVolume(0, MAKELONG(wChannelVolume, wChannelVolume));
}

BOOL CSoundServerApp::IsNormalGroup(CString strGroup)
{
	return strGroup != GROUPNAME_FAVORITES && strGroup != GROUPNAME_SPEECH;
}

BOOL CSoundServerApp::SpeakText(CString strText)
{
	CAtlRegExp<> re;
	CAtlREMatchContext<> mc;
	re.Parse(_T("^{\\d\\d} - .*$"));

	if (re.Match(strText, &mc))
	{
		const CAtlREMatchContext<>::RECHAR* szStart;
		const CAtlREMatchContext<>::RECHAR* szEnd;

		mc.GetMatch(0, &szStart, &szEnd);
		m_nVoice = _ttoi(CString(*szStart) + *CharNext(szStart));

		return TRUE;
	}
	else
	{
		StopPlay();

		if (m_bUseDocTalker)
		{
			CWnd* pWndActive = CWnd::GetActiveWindow();
			CWnd* pWnd = GetDocTalkerWindow();

			if (!pWnd)
			{
				ExecDocTalker();
				return FALSE;
			}

			pWnd->SetForegroundWindow();

			CSendKeys sk;

			sk.SendKeyPress(VK_ESCAPE);
			sk.SendKeyPress(VK_ESCAPE);
			sk.SendKeyPress(VK_RETURN);

			sk.SendKeyDown(VK_MENU);
			sk.SendKeyChar(_T('m'));
			sk.SendKeyUp(VK_MENU);
			sk.SendKeyPress(VK_HOME);

			for (int i = 1; i < m_nVoice; i++)
				sk.SendKeyPress(VK_DOWN);

			sk.SendKeyDown(VK_MENU);
			sk.SendKeyChar(_T('x'));
			sk.SendKeyUp(VK_MENU);
			sk.SendKeyString(strText);
			sk.SendKeyDown(VK_MENU);
			sk.SendKeyChar(_T('t'));
			sk.SendKeyUp(VK_MENU);

			if (pWndActive)
				pWndActive->SetForegroundWindow();

			return TRUE;
		}
		else
		{
			StopSpeak();
			return SUCCEEDED(m_pVoice->Speak(strText.AllocSysString(), SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL));
		}
	}
}

BOOL CSoundServerApp::StopSpeak()
{
	if (m_bUseDocTalker)
	{
		CWnd* pWndActive = CWnd::GetActiveWindow();
		CWnd* pWnd = GetDocTalkerWindow();

		if (pWnd)
		{
			pWnd->SetForegroundWindow();

			CSendKeys sk;
			sk.SendKeyPress(VK_ESCAPE);
			sk.SendKeyPress(VK_ESCAPE);
			sk.SendKeyPress(VK_RETURN);

			if (pWndActive)
				pWndActive->SetForegroundWindow();
		}

		return TRUE;
	}
	else
	{
		return SUCCEEDED(m_pVoice->Speak(NULL, SPF_PURGEBEFORESPEAK, NULL));
	}
}

BOOL CSoundServerApp::Speaking()
{
	SPVOICESTATUS status;

	if (SUCCEEDED(m_pVoice->GetStatus(&status, NULL)))
		return status.dwRunningState == SPRS_IS_SPEAKING;
	else
		return FALSE;
}

void CSoundServerApp::StopAll()
{
	StopPlay();
	StopSpeak();
}

BOOL CSoundServerApp::ExecDocTalker()
{
	CString strCommandLine(GetDocTalkerFileName());
	strCommandLine.Insert(0, _T('"'));
	strCommandLine.AppendChar(_T('"'));
	strCommandLine.AppendChar(_T(' '));
	strCommandLine.Append(DOCTALKER_ARGUMENTS);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	if (!CreateProcess(NULL, strCommandLine.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return FALSE;

	/*CWnd* pWnd = WaitForDocTalkerWindow(TRUE);

	if (pWnd)
	{
		CRect rect;
		pWnd->GetClientRect(rect);
		pWnd->MoveWindow(-rect.Width(), -rect.Height(), 0, 0);
		pWnd->SetForegroundWindow();
	}*/

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return TRUE;
}

CString CSoundServerApp::GetDocTalkerFileName()
{
	CString strFileName(DOCTALKER_DEFAULT_FILE_NAME);

	CRegKey rkSpeech;
	rkSpeech.Create(theApp.GetAppRegistryKey(), REG_KEY_SPEECH);

	ULONG nChars = REG_LIMIT_VALUE + 1;
	LONG lReturn = rkSpeech.QueryStringValue(REG_VALUENAME_DOCTALKER, strFileName.GetBuffer(REG_LIMIT_VALUE), &nChars);
	strFileName.ReleaseBuffer(nChars - 1);

	if (lReturn != ERROR_SUCCESS)
		rkSpeech.SetStringValue(REG_VALUENAME_DOCTALKER, strFileName);

	return strFileName;
}

CWnd* CSoundServerApp::GetDocTalkerWindow()
{
	return CWnd::FindWindow(DOCTALKER_CLASS_NAME, NULL);
}

CWnd* CSoundServerApp::WaitForDocTalkerWindow(BOOL bOpen)
{
	CWnd* pWnd = GetDocTalkerWindow();
	int nIterations = 100;

	while (((bOpen && pWnd == NULL) || (!bOpen && pWnd != NULL)) && nIterations > 0)
	{
		Sleep(10);
		pWnd = GetDocTalkerWindow();
		nIterations--;
	}

	return pWnd;
}

void CSoundServerApp::InsertListItem(CListCtrl& lc, CString strItem, CString strItemText)
{
	int nItem = lc.InsertItem(lc.GetItemCount(), strItem);
	lc.SetItemText(nItem, 1, strItemText);
}
