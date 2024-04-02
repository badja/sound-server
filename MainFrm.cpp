// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SoundServer.h"

#include <cderr.h>

#include "MainFrm.h"
#include "LeftView.h"
#include "SoundServerView.h"
#include ".\mainfrm.h"

#include "SoundServerDoc.h"
#include "GroupDialog.h"
#include "SoundDialog.h"
#include "ConfigurationDialog.h"
#include "SoundFileDialog.h"
#include "SpeechDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
	ON_COMMAND(ID_ADD_SOUND, OnAddSound)
	ON_COMMAND(ID_ADD_GROUP, OnAddGroup)
	ON_COMMAND(ID_EDIT_GROUP, OnEditGroup)
	ON_UPDATE_COMMAND_UI(ID_ADD_SOUND, OnUpdateAddSound)
	ON_COMMAND(ID_EDIT_SOUND, OnEditSound)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SOUND, OnUpdateEditSound)
	ON_COMMAND(ID_DELETE_GROUP, OnDeleteGroup)
	ON_UPDATE_COMMAND_UI(ID_DELETE_GROUP, OnUpdateDeleteGroup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GROUP, OnUpdateEditGroup)
	ON_COMMAND(ID_DELETE_SOUND, OnDeleteSound)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SOUND, OnUpdateDeleteSound)
	ON_COMMAND(ID_PLAY_SOUND, OnPlaySound)
	ON_UPDATE_COMMAND_UI(ID_PLAY_SOUND, OnUpdatePlaySound)
	ON_MESSAGE(WM_UPDATE_CONNECTION, OnUpdateConnection)
	ON_MESSAGE(WM_DATA_RECEIVED, OnDataReceived)
	ON_COMMAND(ID_PLAY_GROUP, OnPlayGroup)
	ON_UPDATE_COMMAND_UI(ID_PLAY_GROUP, OnUpdatePlayGroup)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_ADD_SOUNDSFROMFILES, OnAddSoundsFromFiles)
	ON_UPDATE_COMMAND_UI(ID_ADD_SOUNDSFROMFILES, OnUpdateAddSoundsFromFiles)
	ON_COMMAND(ID_ADD_GROUPFROMFOLDER, OnAddGroupFromFolder)
	ON_COMMAND(ID_ADD_GROUPSFROMSUBFOLDERS, OnAddGroupsFromSubfolders)
	ON_COMMAND(ID_ADD_FAVORITES, OnSoundAddFavorites)
	ON_UPDATE_COMMAND_UI(ID_ADD_FAVORITES, OnUpdateSoundAddFavorites)
	ON_COMMAND(ID_DELETE_FAVORITES, OnDeleteFavorites)
	ON_UPDATE_COMMAND_UI(ID_DELETE_FAVORITES, OnUpdateDeleteFavorites)
	ON_COMMAND(ID_PLAY_STOP, OnPlayStop)
	ON_UPDATE_COMMAND_UI(ID_PLAY_STOP, OnUpdatePlayStop)
	ON_MESSAGE(MM_WOM_CLOSE, OnMmWomClose)
	ON_MESSAGE(MM_WOM_DONE, OnMmWomDone)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_COMMAND(ID_SET_MASTERFOLDER, OnSetMasterFolder)
	ON_COMMAND(ID_ADD_SPEECH, OnSoundAddSpeech)
	ON_UPDATE_COMMAND_UI(ID_ADD_SPEECH, OnUpdateSoundAddSpeech)
	ON_COMMAND(ID_EDIT_SPEECH, OnSoundEditSpeech)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPEECH, OnUpdateSoundEditSpeech)
	ON_COMMAND(ID_DELETE_SPEECH, OnDeleteSpeech)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SPEECH, OnUpdateDeleteSpeech)
	ON_COMMAND(ID_TEST_SPEECH, OnSoundTestSpeech)
	ON_UPDATE_COMMAND_UI(ID_TEST_SPEECH, OnUpdateSoundTestSpeech)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndVolumeBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndVolumeBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSoundServerView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	theApp.InitialiseSockets(this);

	if (!theApp.IsServer())
		GetMenu()->DeleteMenu(1, MF_BYPOSITION);	// delete the Edit menu

	RegisterHotKey(GetSafeHwnd(), 0, MOD_SHIFT | MOD_WIN, 'B');

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

CLeftView* CMainFrame::GetLeftPane() const
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	CLeftView* pView = DYNAMIC_DOWNCAST(CLeftView, pWnd);
	return pView;
}

CSoundServerView* CMainFrame::GetRightPane() const
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CSoundServerView* pView = DYNAMIC_DOWNCAST(CSoundServerView, pWnd);
	return pView;
}

CSoundServerDoc* CMainFrame::GetDocument() const
{
	return GetRightPane()->GetDocument();
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	if (!pCmdUI)
		return;

	// TODO: customize or extend this code to handle choices on the View menu

	CSoundServerView* pView = GetRightPane(); 

	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		// if the command is ID_VIEW_LINEUP, only enable command
		// when we're in LVS_ICON or LVS_SMALLICON mode

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// otherwise, use dots to reflect the style of the view
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the View menu
	CSoundServerView* pView = GetRightPane();

	// if the right-hand pane has been created and is a CSoundServerView,
	// process the menu commands...
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// ask the list control to snap to grid
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		// other commands change the style on the list control
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		// change the style; window will repaint automatically
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);

		CRegKey rkSettings;
		rkSettings.Open(theApp.GetAppRegistryKey(), REG_KEY_SETTINGS);
		rkSettings.SetDWORDValue(REG_VALUENAME_VIEWSTYLE, pView->GetStyle() & LVS_TYPEMASK);
	}
}

void CMainFrame::OnAddGroup()
{
	CGroupDialog dialog(_T("Add Group"), this);

	if (dialog.DoModal() != IDOK)
		return;

	if (!GetDocument()->AddGroup(dialog.m_strGroup))
		AfxMessageBox(_T("Cannot add the group. The group may already exist."));
}

void CMainFrame::OnAddSound()
{
	CString strGroup = GetLeftPane()->GetSelectedGroup();

	if (strGroup.IsEmpty())
		return;

	CSoundDialog dialog(_T("Add Sound"), this);

	if (dialog.DoModal() != IDOK)
		return;

	if (!GetDocument()->AddSound(strGroup, dialog.m_strSound, dialog.m_strFile))
		AfxMessageBox(_T("Cannot add the sound. The sound may already exist."));
}

void CMainFrame::OnEditGroup()
{
	CString strGroup = GetLeftPane()->GetSelectedGroup();

	if (strGroup.IsEmpty())
		return;

	CGroupDialog dialog(_T("Edit Group"), this);
	dialog.m_strGroup = strGroup;

	if (dialog.DoModal() != IDOK)
		return;

	if (!GetDocument()->EditGroup(strGroup, dialog.m_strGroup))
		AfxMessageBox(_T("Cannot edit the group. The new group name may already exist."));
}

void CMainFrame::OnUpdateAddSound(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsSingleGroupSelected());
}

void CMainFrame::OnEditSound()
{
	CString strGroup = GetLeftPane()->GetSelectedGroup();
	CString strSound = GetRightPane()->GetSelectedSound();
	CString strFile = GetRightPane()->GetSelectedGroupOrFile();

	if (strSound.IsEmpty())
		return;

	CSoundDialog dialog(_T("Edit Sound"), this);
	dialog.m_strSound = strSound;
	dialog.m_strFile = strFile;

	if (dialog.DoModal() != IDOK)
		return;

	if (!GetDocument()->EditSound(strGroup, strSound, dialog.m_strSound, dialog.m_strFile))
		AfxMessageBox(_T("Cannot edit the sound. The new sound name may already exist."));
}

void CMainFrame::OnUpdateEditSound(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsNormalSelected() && GetRightPane()->GetListCtrl().GetSelectedCount() == 1);
}

void CMainFrame::OnDeleteGroup()
{
	CStringArray saGroups;
	GetLeftPane()->GetSelectedGroups(saGroups);

	if (saGroups.IsEmpty())
		return;

	if (AfxMessageBox(_T("Are you sure you want to delete the selected group(s) and all the sounds inside?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	CWaitCursor wc;
	GetRightPane()->GetListCtrl().SetRedraw(FALSE);

	for (INT_PTR i = 0; i < saGroups.GetCount(); i++)
		GetDocument()->DeleteGroup(saGroups[i]);

	GetRightPane()->GetListCtrl().SetRedraw(TRUE);
}

void CMainFrame::OnUpdateDeleteGroup(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->AreMultipleGroupsSelected());
}

void CMainFrame::OnUpdateEditGroup(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsSingleGroupSelected());
}

void CMainFrame::OnDeleteSound()
{
	CString strGroup = GetLeftPane()->GetSelectedGroup();

	if (strGroup.IsEmpty())
		return;

	CStringArray saSounds;
	GetRightPane()->GetSelectedSounds(saSounds);

	if (saSounds.IsEmpty())
		return;

	if (AfxMessageBox(_T("Are you sure you want to delete the selected sound(s)?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	CWaitCursor wc;
	GetRightPane()->GetListCtrl().SetRedraw(FALSE);

	for (INT_PTR i = 0; i < saSounds.GetCount(); i++)
		GetDocument()->DeleteSound(strGroup, saSounds[i]);

	GetRightPane()->GetListCtrl().SetRedraw(TRUE);
}

void CMainFrame::OnUpdateDeleteSound(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsNormalSelected() && GetRightPane()->GetListCtrl().GetSelectedCount() > 0);
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	SetWindowText(theApp.IsServer() ? _T("Sound Server") : _T("Sound Client"));
}

void CMainFrame::OnPlaySound()
{
	CAction action(NULL, GetRightPane()->GetSelectedSound());

	if (GetLeftPane()->IsFavoritesSelected())
		action.m_strGroup = GetRightPane()->GetSelectedGroupOrFile();
	else if (GetLeftPane()->IsSpeechSelected())
		action.m_strGroup = GROUPNAME_SPEECH;
	else
		action.m_strGroup = GetLeftPane()->GetSelectedGroup();

	if (theApp.IsServer())
		GetDocument()->UpdateAllViews(NULL, eSoundPlay, &action);
	else
		theApp.ClientSendData(eSoundPlay, action);
}

void CMainFrame::OnUpdatePlaySound(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetRightPane()->GetListCtrl().GetSelectedCount() == 1);
}

LRESULT CMainFrame::OnUpdateConnection(WPARAM wParam, LPARAM lParam)
{
	UINT uEvent = (UINT) wParam;
	CSocketManager* pManager = reinterpret_cast<CSocketManager*>( lParam );

	if (theApp.IsServer())
		theApp.ServerUpdateConnection(uEvent, pManager);

	if (theApp.IsServer())
	{
		theApp.ServerSendData(eVolumeSet, CAction(NULL, NULL, NULL, NULL, GetVolumeSlider()));
		if (uEvent == EVT_CONSUCCESS)
			GetDocument()->ServerSendSounds(pManager);
	}
	else
	{
		if (uEvent == EVT_CONDROP)
		{
			theApp.ClientDisconnect();
			GetDocument()->ClearAllSounds();
		}
	}

	return 1L;
}

LRESULT CMainFrame::OnDataReceived(WPARAM wParam, LPARAM lParam)
{
	CString* pData = (CString*) wParam;
	CSocketManager* pManager = reinterpret_cast<CSocketManager*>( lParam );

	CAction action;
	CAction actionNew;
	action.m_pActionNew = &actionNew;
	EAction eAction = theApp.ParseData(*pData, action);
	delete pData;

	if (theApp.IsServer())
		GetDocument()->UpdateAllViews(NULL, eAction, &action);
	else
		GetDocument()->UpdateDocument(eAction, action);

	return 1L;
}

void CMainFrame::OnPlayGroup()
{
	CAction action(GetLeftPane()->GetSelectedGroup());

	if (theApp.IsServer())
		GetDocument()->UpdateAllViews(NULL, eGroupPlay, &action);
	else
		theApp.ClientSendData(eGroupPlay, action);
}

void CMainFrame::OnUpdatePlayGroup(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsSingleGroupSelected());
}

void CMainFrame::OnConfigure()
{
	CConfigurationDialog dialog(this);
	dialog.m_strPort = theApp.m_strPort;
	dialog.m_nSockType = theApp.m_nSockType;
	dialog.m_strServer = theApp.m_strServer;

	if (dialog.DoModal() != IDOK)
		return;

	theApp.m_strPort = dialog.m_strPort;
	theApp.m_nSockType = dialog.m_nSockType;
	theApp.m_strServer = dialog.m_strServer;

	theApp.WriteRegistrySettings();
}

void CMainFrame::OnConnect()
{
	if (!theApp.ClientConnect())
		AfxMessageBox(_T("Cannot connect to the Sound Server."));
}

void CMainFrame::OnUpdateConnect(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!theApp.IsServer() && !theApp.ClientIsConnected());
}

void CMainFrame::OnDisconnect()
{
	theApp.ClientDisconnect();
}

void CMainFrame::OnUpdateDisconnect(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!theApp.IsServer() && theApp.ClientIsConnected());
}

void CMainFrame::OnAddSoundsFromFiles()
{
	CString strGroup = GetLeftPane()->GetSelectedGroup();

	if (strGroup.IsEmpty())
		return;

	CSoundFileDialog dialog(NULL, this);

	if (dialog.DoModal() != IDOK)
	{
		DWORD dwError = CommDlgExtendedError();

		if (dwError == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox(_T("Too many files selected. No sounds were added."));
		}
		else if (dwError != 0)
		{
			CString strMessage;
			strMessage.Format(_T("Common dialog box error code %ld. No sounds were added."), dwError);
			AfxMessageBox(strMessage);
		}

		return;
	}

	CString strPathName, strFileTitle;
	CWaitCursor wc;
	GetRightPane()->GetListCtrl().SetRedraw(FALSE);

	for (POSITION pos = dialog.GetStartPosition(); pos != NULL; )
	{
		strPathName = dialog.GetNextPathName(pos);

		strFileTitle = strPathName;
		LPTSTR pszBuffer = strFileTitle.GetBuffer();
		::PathStripPath(pszBuffer);
		::PathRemoveExtension(pszBuffer);
		strFileTitle.ReleaseBuffer();

		GetDocument()->AddSound(strGroup, strFileTitle, strPathName, FALSE);
	}

	GetDocument()->WriteSoundsToRegistry();
	GetRightPane()->GetListCtrl().SetRedraw(TRUE);
}

void CMainFrame::OnUpdateAddSoundsFromFiles(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsSingleGroupSelected());
}

void CMainFrame::OnAddGroupFromFolder()
{
	CString strPath;

	if (!BrowseForFolder(_T("For the selected folder, a group will be added and sounds from it added to the group."), strPath))
	{
		AfxMessageBox(_T("Error adding group from folder."));
		return;
	}

	if (!strPath.IsEmpty())
	{
		CWaitCursor wc;
		CString strError = AddGroupFromFolder(strPath);
		GetDocument()->WriteSoundsToRegistry();
		wc.Restore();

		if (!strError.IsEmpty())
			AfxMessageBox(strError);
	}
}

INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) 
{
	TCHAR szDir[MAX_PATH];

	switch (uMsg) 
	{
	case BFFM_INITIALIZED:
		if (!theApp.m_strMasterFolder.IsEmpty())
		{
			// WParam is TRUE since you are passing a path.
			// It would be FALSE if you were passing a pidl.
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)theApp.m_strMasterFolder.GetString());
		}
		break;

	case BFFM_SELCHANGED: 
		// Set the status window to the currently selected path.
		if (SHGetPathFromIDList((LPITEMIDLIST)lp, szDir))
		{
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
		}
		break;
	}
	return 0;
}

BOOL CMainFrame::BrowseForFolder(CString strTitle, CString& strPath)
{
	strPath.Empty();

	BOOL bSuccess = FALSE;
	LPMALLOC pMalloc;

	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		BROWSEINFO bi = {0};
		bi.hwndOwner = GetSafeHwnd();
		bi.lpszTitle = strTitle;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (pidl)
		{
			TCHAR pszPath[MAX_PATH];
			bSuccess = SHGetPathFromIDList(pidl, pszPath);
			pMalloc->Free(pidl);

			if (bSuccess)
				strPath = pszPath;
		}
		else
		{
			bSuccess = TRUE;
		}

		pMalloc->Release();
	}

	return bSuccess;
}

CString CMainFrame::AddGroupFromFolder(CString strPath)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch; 

	BOOL bFinished = FALSE;

	CString strGroup = ::PathFindFileName(strPath);

	if (strPath.Right(1) != _T("\\"))
		strPath += _T('\\');

	hSearch = FindFirstFile(strPath + _T("*.wav"), &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		CString strError;
		strError.Format(_T("No .WAV files found in %s."), strPath);
		return strError;
	}

	GetDocument()->AddGroup(strGroup, FALSE);

	CString strSound;

	while (!bFinished)
	{
		strSound = FileData.cFileName;
		::PathRemoveExtension(strSound.GetBuffer());
		strSound.ReleaseBuffer();

		GetDocument()->AddSound(strGroup, strSound, strPath + FileData.cFileName, FALSE);

		if (!FindNextFile(hSearch, &FileData))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				bFinished = TRUE;
			}
			else
			{
				FindClose(hSearch);
				return _T("Error reading files.");
			}
		}
	}

	FindClose(hSearch);

	return CString();
}

void CMainFrame::OnAddGroupsFromSubfolders()
{
	CString strPath;

	if (!BrowseForFolder(_T("For each subfolder beneath the selected folder, a group will be added and sounds from the subfolder added to the group."), strPath))
	{
		AfxMessageBox(_T("Error adding groups from subfolders."));
		return;
	}

	if (!strPath.IsEmpty())
	{
		CWaitCursor wc;
		CString strError = AddGroupsFromSubfolders(strPath);
		wc.Restore();

		if (!strError.IsEmpty())
			AfxMessageBox(strError);
	}
}

CString CMainFrame::AddGroupsFromSubfolders(CString strPath)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch; 

	BOOL bFinished = FALSE;

	if (strPath.Right(1) != _T("\\"))
		strPath += _T('\\');

	hSearch = FindFirstFile(strPath + _T("*.*"), &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
		return CString();

	CString strFileName;

	while (!bFinished)
	{
		strFileName = FileData.cFileName;

		if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strFileName.SpanIncluding(_T(".")) != strFileName)
			AddGroupFromFolder(strPath + strFileName);

		if (!FindNextFile(hSearch, &FileData))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				bFinished = TRUE;
			}
			else
			{
				FindClose(hSearch);
				return _T("Error reading files.");
			}
		}
	}

	FindClose(hSearch);
	GetDocument()->WriteSoundsToRegistry();

	return CString();
}

void CMainFrame::OnSoundAddFavorites()
{
	CString strGroup = GetLeftPane()->GetSelectedGroup();

	if (strGroup.IsEmpty())
		return;

	CStringArray saSounds;
	GetRightPane()->GetSelectedSounds(saSounds);

	if (saSounds.IsEmpty())
		return;

	for (INT_PTR i = 0; i < saSounds.GetCount(); i++)
		GetDocument()->AddToFavorites(strGroup, saSounds[i]);
}

void CMainFrame::OnUpdateSoundAddFavorites(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsNormalSelected() && GetRightPane()->GetListCtrl().GetSelectedCount() > 0);
}

void CMainFrame::OnDeleteFavorites()
{
	CStringArray saGroups, saSounds;
	GetRightPane()->GetSelectedGroupsOrFiles(saGroups);
	GetRightPane()->GetSelectedSounds(saSounds);

	if (saGroups.IsEmpty() || saSounds.IsEmpty())
		return;

	for (INT_PTR i = 0; i < saSounds.GetCount(); i++)
		GetDocument()->DeleteFromFavorites(saGroups[i], saSounds[i]);
}

void CMainFrame::OnUpdateDeleteFavorites(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsFavoritesSelected() && GetRightPane()->GetListCtrl().GetSelectedCount() > 0);
}

void CMainFrame::OnSoundAddSpeech()
{
	CSpeechDialog dialog(_T("Add Speech"), this);

	if (dialog.DoModal() != IDOK)
		return;

	if (!GetDocument()->AddToSpeech(dialog.m_strText))
		AfxMessageBox(_T("Cannot add the speech text. The speech text may already exist."));
}

void CMainFrame::OnUpdateSoundAddSpeech(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetLeftPane()->IsSpeechSelected());
}

void CMainFrame::OnSoundEditSpeech()
{
	CString strText = GetRightPane()->GetSelectedSound();

	CSpeechDialog dialog(_T("Edit Speech"), this);
	dialog.m_strText = strText;

	if (dialog.DoModal() != IDOK)
		return;

	if (!GetDocument()->EditSpeech(strText, dialog.m_strText))
		AfxMessageBox(_T("Cannot edit the speech text. The new speech text may already exist."));
}

void CMainFrame::OnUpdateSoundEditSpeech(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetRightPane()->GetListCtrl().GetSelectedCount() == 1 && GetRightPane()->GetSelectedGroupOrFile() == GROUPNAME_SPEECH);
}

void CMainFrame::OnSoundTestSpeech()
{
	theApp.SpeakText(GetRightPane()->GetSelectedSound());
}

void CMainFrame::OnUpdateSoundTestSpeech(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!theApp.IsServer() && GetRightPane()->GetListCtrl().GetSelectedCount() == 1 && GetLeftPane()->IsSpeechSelected());
}

void CMainFrame::OnDeleteSpeech()
{
	CStringArray saText;
	GetRightPane()->GetSelectedSounds(saText);

	if (saText.IsEmpty())
		return;

	for (INT_PTR i = 0; i < saText.GetCount(); i++)
		GetDocument()->DeleteFromSpeech(saText[i]);
}

void CMainFrame::OnUpdateDeleteSpeech(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((GetRightPane()->GetListCtrl().GetSelectedCount() == 1 && GetRightPane()->GetSelectedGroupOrFile() == GROUPNAME_SPEECH) ||
		(GetRightPane()->GetListCtrl().GetSelectedCount() > 1 && GetLeftPane()->IsSpeechSelected()));
}

void CMainFrame::OnPlayStop()
{
	if (theApp.IsServer())
		theApp.StopAll();
	else
		theApp.ClientSendData(eSoundStop, CAction());
}

void CMainFrame::OnUpdatePlayStop(CCmdUI *pCmdUI)
{
	//pCmdUI->Enable(!theApp.IsServer() || theApp.Playing() || theApp.Speaking());
}

LRESULT CMainFrame::OnMmWomClose(WPARAM wParam, LPARAM lParam)
{
	theApp.OnMmWomClose((HWAVEOUT)wParam);

	return 1L;
}

LRESULT CMainFrame::OnMmWomDone(WPARAM wParam, LPARAM lParam)
{
	theApp.OnMmWomDone((HWAVEOUT)wParam, (WAVEHDR*)lParam);

	return 1L;
}

int CMainFrame::GetVolumeSlider()
{
	return m_wndVolumeBar.GetVolumeSlider();
}

void CMainFrame::SetVolumeSlider(int nVolume)
{
	m_wndVolumeBar.SetVolumeSlider(nVolume);
}

LRESULT CMainFrame::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (IsWindowVisible())
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
//		GetLeftPane()->SelectFavorites();
		ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);
		SetForegroundWindow();
	}

	return 1L;
}

void CMainFrame::OnSetMasterFolder()
{
	CString strPath;

	if (!BrowseForFolder(_T("Use Connection -> Configuration -> Refresh Sounds to update the sounds from the contents of the selected folder."), strPath))
	{
		AfxMessageBox(_T("Error setting master sound folder."));
		return;
	}

	if (!strPath.IsEmpty())
	{
		theApp.m_strMasterFolder = strPath;
		theApp.WriteRegistrySettings();
	}
}

void CMainFrame::RefreshMasterFolder()
{
	if (theApp.IsServer())
	{
		if (!theApp.m_strMasterFolder.IsEmpty())
		{
			CWaitCursor wc;
			AddGroupsFromSubfolders(theApp.m_strMasterFolder);
			wc.Restore();
		}
	}
	else
	{
		theApp.ClientSendData(eRefreshMasterFolder, CAction());
	}
}

void CMainFrame::DeleteMissingSounds()
{
	if (theApp.IsServer())
	{
		CWaitCursor wc;
		GetDocument()->DeleteMissingSounds();
		wc.Restore();
	}
	else
	{
		theApp.ClientSendData(eDeleteMissingSounds, CAction());
	}
}
