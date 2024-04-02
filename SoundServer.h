// SoundServer.h : main header file for the SoundServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "SocketManager.h"

#include "sapi.h"

#define MAX_CONNECTION		10
#define NUM_BUFFERS			5
#define BUFFER_SECONDS_INV	10
#define DEFAULT_VOLUME		50

#define REG_KEY_SETTINGS			_T("Settings")
#define REG_KEY_SOUNDS				_T("Sounds")
#define REG_KEY_FAVORITES			_T("Favorites")
#define REG_KEY_SPEECH				_T("Speech")
#define REG_VALUENAME_PORT			_T("Port")
#define REG_VALUENAME_SOCKTYPE		_T("Socket Type")
#define REG_VALUENAME_SERVER		_T("Server")
#define REG_VALUENAME_GROUPS		_T("Groups")
#define REG_VALUENAME_SOUNDS		_T("Sounds")
#define REG_VALUENAME_TEXT			_T("Text")
#define REG_VALUENAME_MASTERFOLDER	_T("Master Sound Folder")
#define REG_VALUENAME_VIEWSTYLE		_T("View Style")
#define REG_VALUENAME_DOCTALKER		_T("DocTalker Path")
#define REG_LIMIT_KEYNAME			255
#define REG_LIMIT_VALUENAME			260
#define REG_LIMIT_VALUE				2048

enum EAction
{
	eInvalid,
	eInitialise,
	eGroupAdd,
	eGroupEdit,
	eGroupDelete,
	eGroupPlay,
	eSoundAdd,
	eSoundEdit,
	eSoundDelete,
	eSoundPlay,
	eSoundStop,
	eFavoriteAdd,
	eFavoriteDelete,
	eVolumeSet,
	eDeleteMissingSounds,
	eRefreshMasterFolder,
	eSpeechAdd,
	eSpeechEdit,
	eSpeechDelete
};


class CAction : public CObject
{
public:
	CAction(LPCTSTR lpszGroup = NULL, LPCTSTR lpszSound = NULL, LPCTSTR lpszFile = NULL, CAction* pActionNew = NULL, DWORD_PTR dwData = -1) :
	  m_strGroup(lpszGroup), m_strSound(lpszSound), m_strFile(lpszFile), m_pActionNew(pActionNew), m_dwData(dwData) {}

	CString m_strGroup;
	CString m_strSound;
	CString m_strFile;
	CAction* m_pActionNew;
	DWORD_PTR m_dwData;
};

const int SOCK_TCP	= 0;
const int SOCK_UDP  = 1;

// CSoundServerApp:
// See SoundServer.cpp for the implementation of this class
//

class CSoundServerApp : public CWinApp
{
public:
	CSoundServerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	CString	m_strPort;
	int		m_nSockType;
	CString m_strServer;
	CString	m_strMasterFolder;

	BOOL PlaySound(CString strFile);
	BOOL ServerUpdateConnection(UINT uEvent, CSocketManager* pManager);
	BOOL IsServer() { return m_nLinkMode == 0; }
	void ClientSendData(EAction eAction, const CAction& action);
	void ServerSendData(EAction eAction, const CAction& action, CSocketManager* pClient = NULL);
	EAction ParseData(CString strData, CAction& action);
	void InitialiseSockets(CWnd* pMsgWnd);
	void WriteRegistrySettings();
	void ReadRegistrySettings();
	BOOL ClientConnect();
	void ClientDisconnect();
	BOOL ClientIsConnected();
	void StopPlay();
	BOOL Playing();
	void OnMmWomClose(HWAVEOUT hOutputDev);
	void OnMmWomDone(HWAVEOUT hOutputDev, WAVEHDR* lpwvhdr);
	void SetVolume(int nVolume);
	BOOL IsNormalGroup(CString strGroup);
	BOOL SpeakText(CString strText);
	BOOL StopSpeak();
	BOOL Speaking();
	void StopAll();
	void InsertListItem(CListCtrl& lc, CString strItem, CString strItemText);

	virtual int ExitInstance();

protected:
	int  m_nLinkMode;
	BOOL m_bHideOnStartup;
	CSocketManager m_SocketManager[MAX_CONNECTION];
	CSocketManager* m_pCurServer;
	CSocketManager m_ClientSocketManager;
	MMRESULT m_rc;					// Return code
	HMMIO m_hmmioIn;				// file handle
	LONG m_dataOffset;				// start of audio data in wave file
	DWORD m_audioLength;			// number of bytes in audio data
	LONG m_startPos;				// sample where we started playback from
	WAVEFORMATEX m_format;			// waveformat structure
	LPSTR m_pmem[NUM_BUFFERS];		// memory pointers
	WAVEHDR m_hdr[NUM_BUFFERS];		// wave headers
	DWORD m_bufferSize;				// size of output buffers
	BOOL m_fPlaying;				// is file currently playing
	BOOL m_fFileOpen;				// is file currently open
	HWAVEOUT m_hWaveOut;			// waveout handle
	CString m_msg;					// message buffer
	CString m_strSoundFile;
	BOOL m_bPlayOnClose;
	ISpVoice* m_pVoice;
	BOOL m_bUseDocTalker;
	int m_nVoice;

	void ParseCommandLineArgs();
	void PickNextAvailable();
	bool StartServer();
	void ServerSend(CString strText, CSocketManager* pClient = NULL);
	void ClientSend(CString strText);
	CString CreateCommandString(EAction eAction, const CAction& action);
	void AddParameter(CString& strData, CString strName, CString strValue);
	void AddParameter(CString& strData, CString strName, DWORD_PTR dwValue);
	void ProcessParameter(CString strName, CString strValue, EAction& eAction, CAction& action);
	void CloseFile();
	BOOL OpenFile(CString soundfile);
	BOOL FileSeek(LONG lPosition);
	LONG Position();
	BOOL ExecDocTalker();
	CString GetDocTalkerFileName();
	CWnd* GetDocTalkerWindow();
	CWnd* WaitForDocTalkerWindow(BOOL bOpen);
};

extern CSoundServerApp theApp;