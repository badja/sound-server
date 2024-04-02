// SoundServerView.cpp : implementation of the CSoundServerView class
//

#include "stdafx.h"
#include "SoundServer.h"

#include "SoundServerDoc.h"
#include "SoundServerView.h"
#include ".\soundserverview.h"

#include "MainFrm.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define COLUMNHEADER_SOUNDNAME	_T("Sound name")
#define COLUMNHEADER_FILENAME	_T("File name")
#define COLUMNHEADER_GROUPNAME	_T("Group name")


// CSoundServerView

IMPLEMENT_DYNCREATE(CSoundServerView, CListView)

BEGIN_MESSAGE_MAP(CSoundServerView, CListView)
	ON_WM_STYLECHANGED()
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CSoundServerView construction/destruction

CSoundServerView::CSoundServerView()
{
	m_imageListNormal.Create(32, 32, ILC_COLOR4 | ILC_MASK, 1, 1);
	m_imageListNormal.Add(theApp.LoadIcon(IDR_MAINFRAME));

	m_imageListSmall.Create(16, 16, ILC_COLOR4 | ILC_MASK, 1, 1);
	m_imageListSmall.Add(theApp.LoadIcon(IDR_MAINFRAME));
}

CSoundServerView::~CSoundServerView()
{
}

BOOL CSoundServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (LVS_EDITLABELS | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS | LVS_SORTASCENDING);

	DWORD dwValue;
	CRegKey rkSettings;
	rkSettings.Open(theApp.GetAppRegistryKey(), REG_KEY_SETTINGS);

	if (rkSettings.QueryDWORDValue(REG_VALUENAME_VIEWSTYLE, dwValue) == ERROR_SUCCESS)
		cs.style |= dwValue;
	else
		cs.style |= (theApp.IsServer() ? LVS_REPORT : LVS_LIST);

	return CListView::PreCreateWindow(cs);
}

void CSoundServerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	GetListCtrl().SetImageList(&m_imageListNormal, LVSIL_NORMAL);
	GetListCtrl().SetImageList(&m_imageListSmall, LVSIL_SMALL);

	GetListCtrl().InsertColumn(0, COLUMNHEADER_SOUNDNAME, LVCFMT_LEFT, 200);
	GetListCtrl().InsertColumn(1, COLUMNHEADER_FILENAME, LVCFMT_LEFT, 400);
}


// CSoundServerView diagnostics

#ifdef _DEBUG
void CSoundServerView::AssertValid() const
{
	CListView::AssertValid();
}

void CSoundServerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSoundServerDoc* CSoundServerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoundServerDoc)));
	return (CSoundServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSoundServerView message handlers
CLeftView* CSoundServerView::GetLeftPane() const
{
	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	return pFrame->GetLeftPane();
}

CMainFrame* CSoundServerView::GetMainFrame() const
{
	return (CMainFrame*)AfxGetMainWnd();
}

void CSoundServerView::OnStyleChanged(int /*nStyleType*/, LPSTYLESTRUCT /*lpStyleStruct*/)
{
	//TODO: add code to react to the user changing the view style of your window
	
	Default();
}

void CSoundServerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CAction* pAction = (CAction*)pHint;

	int nItem;

	switch (lHint)
	{
	case eInitialise:
		GetListCtrl().DeleteAllItems();
		break;
	case eGroupPlay:
		GetDocument()->PlayGroup(pAction->m_strGroup);
		break;
	case eSoundAdd:
		if (GetLeftPane()->GetSelectedGroup() == pAction->m_strGroup)
			theApp.InsertListItem(GetListCtrl(), pAction->m_strSound, pAction->m_strFile);
		break;
	case eSoundEdit:
		if (GetLeftPane()->GetSelectedGroup() == pAction->m_strGroup)
		{
			nItem = FindSound(pAction->m_strSound);
			if (nItem != -1)
			{
				GetListCtrl().SetItemText(nItem, 0, pAction->m_pActionNew->m_strSound);
				GetListCtrl().SetItemText(nItem, 1, pAction->m_pActionNew->m_strFile);
			}
		}
		break;
	case eSoundDelete:
		if (GetLeftPane()->GetSelectedGroup() == pAction->m_strGroup)
		{
			nItem = FindSound(pAction->m_strSound);
			if (nItem != -1)
				GetListCtrl().DeleteItem(nItem);
		}
		break;
	case eSoundPlay:
		GetDocument()->PlaySound(pAction->m_strGroup, pAction->m_strSound);
		break;
	case eSoundStop:
		theApp.StopAll();
		break;
	case eFavoriteDelete:
		if (GetLeftPane()->IsFavoritesSelected())
		{
			CString strGroup, strSound;

			for (int i = 0; i < GetListCtrl().GetItemCount(); i++)
			{
				strSound = GetListCtrl().GetItemText(i, 0);
				strGroup = GetListCtrl().GetItemText(i, 1);

				if (strGroup == pAction->m_strGroup && strSound == pAction->m_strSound)
				{
					GetListCtrl().DeleteItem(i);
					break;
				}
			}
		}
		break;
	case eSpeechAdd:
		if (GetLeftPane()->IsSpeechSelected())
			theApp.InsertListItem(GetListCtrl(), pAction->m_strSound, GROUPNAME_SPEECH);
		break;
	case eSpeechEdit:
		if (GetLeftPane()->IsSpeechSelected())
		{
			nItem = FindSound(pAction->m_strSound);
			if (nItem != -1)
				GetListCtrl().SetItemText(nItem, 0, pAction->m_pActionNew->m_strSound);
		}
		break;
	case eSpeechDelete:
		if (GetLeftPane()->IsSpeechSelected())
		{
			CString strText;

			for (int i = 0; i < GetListCtrl().GetItemCount(); i++)
			{
				strText = GetListCtrl().GetItemText(i, 0);

				if (strText == pAction->m_strSound)
				{
					GetListCtrl().DeleteItem(i);
					break;
				}
			}
		}
		break;
	case eVolumeSet:
		GetMainFrame()->SetVolumeSlider((int)pAction->m_dwData);
		theApp.SetVolume((int)pAction->m_dwData);
		theApp.ServerSendData(eVolumeSet, CAction(NULL, NULL, NULL, NULL, pAction->m_dwData));
		break;
	case eRefreshMasterFolder:
		GetMainFrame()->RefreshMasterFolder();
		break;
	case eDeleteMissingSounds:
		GetMainFrame()->DeleteMissingSounds();
		break;
	}
}

BOOL CSoundServerView::UpdateSoundList(CString strGroup)
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = COLUMNHEADER_FILENAME;
	GetListCtrl().SetColumn(1, &lvc);

	return GetDocument()->UpdateSoundList(GetListCtrl(), strGroup);
}

BOOL CSoundServerView::UpdateFavoritesList()
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = COLUMNHEADER_GROUPNAME;
	GetListCtrl().SetColumn(1, &lvc);

	return GetDocument()->UpdateFavoritesList(GetListCtrl());
}

BOOL CSoundServerView::UpdateSpeechList()
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = COLUMNHEADER_GROUPNAME;
	GetListCtrl().SetColumn(1, &lvc);

	return GetDocument()->UpdateSpeechList(GetListCtrl());
}

CString CSoundServerView::GetSelectedSound()
{
	if (GetListCtrl().GetSelectedCount() != 1)
		return CString();

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	int nItem = GetListCtrl().GetNextSelectedItem(pos);

	return GetListCtrl().GetItemText(nItem, 0);
}

void CSoundServerView::GetSelectedSounds(CStringArray& saSounds)
{
	saSounds.RemoveAll();

	int nItem;

	for (POSITION pos = GetListCtrl().GetFirstSelectedItemPosition(); pos != NULL; )
	{
		nItem = GetListCtrl().GetNextSelectedItem(pos);
		saSounds.Add(GetListCtrl().GetItemText(nItem, 0));
	}
}

CString CSoundServerView::GetSelectedGroupOrFile()
{
	if (GetListCtrl().GetSelectedCount() != 1)
		return CString();

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	int nItem = GetListCtrl().GetNextSelectedItem(pos);

	return GetListCtrl().GetItemText(nItem, 1);
}

void CSoundServerView::GetSelectedGroupsOrFiles(CStringArray& saGroupsOrSounds)
{
	saGroupsOrSounds.RemoveAll();

	int nItem;

	for (POSITION pos = GetListCtrl().GetFirstSelectedItemPosition(); pos != NULL; )
	{
		nItem = GetListCtrl().GetNextSelectedItem(pos);
		saGroupsOrSounds.Add(GetListCtrl().GetItemText(nItem, 1));
	}
}

int CSoundServerView::FindSound(CString strSound)
{
	LVFINDINFO lvfi;
	lvfi.flags = LVFI_STRING;
	lvfi.psz = strSound;

	return GetListCtrl().FindItem(&lvfi);
}

void CSoundServerView::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CAction action(NULL, GetListCtrl().GetItemText(pNMIA->iItem, 0));

	if (GetLeftPane()->IsFavoritesSelected())
		action.m_strGroup = GetSelectedGroupOrFile();
	else if (GetLeftPane()->IsSpeechSelected())
		action.m_strGroup = GROUPNAME_SPEECH;
	else
		action.m_strGroup = GetLeftPane()->GetSelectedGroup();

	if (theApp.IsServer())
		GetDocument()->UpdateAllViews(NULL, eSoundPlay, &action);
	else
		theApp.ClientSendData(eSoundPlay, action);

	*pResult = 0;
}

void CSoundServerView::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if (pDispInfo->item.pszText)
	{
		GetDocument()->EditSound(GetLeftPane()->GetSelectedGroup(), GetListCtrl().GetItemText(pDispInfo->item.iItem, 0), pDispInfo->item.pszText, GetListCtrl().GetItemText(pDispInfo->item.iItem, 1));
		*pResult = 1;
	}
	else
	{
		*pResult = 0;
	}
}

void CSoundServerView::OnRename()
{
	if (theApp.IsServer() && GetLeftPane()->IsNormalSelected())
		GetListCtrl().EditLabel(GetListCtrl().GetSelectionMark());
	else if (GetLeftPane()->IsSpeechSelected())
		GetListCtrl().EditLabel(GetListCtrl().GetSelectionMark());
}

void CSoundServerView::OnDelete()
{
	if (GetLeftPane()->IsFavoritesSelected())
	{
		GetMainFrame()->OnDeleteFavorites();
	}
	else if (GetLeftPane()->IsSpeechSelected())
	{
		GetMainFrame()->OnDeleteSpeech();
	}
	else
	{
		if (theApp.IsServer())
			GetMainFrame()->OnDeleteSound();
	}
}

void CSoundServerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!theApp.IsServer())
	{
		int nItem = GetListCtrl().HitTest(point);

		if (nItem > -1)
		{
			CAction action(NULL, GetListCtrl().GetItemText(nItem, 0));

			if (GetLeftPane()->IsFavoritesSelected())
				action.m_strGroup = GetListCtrl().GetItemText(nItem, 1);
			else if (GetLeftPane()->IsSpeechSelected())
				action.m_strGroup = GROUPNAME_SPEECH;
			else
				action.m_strGroup = GetLeftPane()->GetSelectedGroup();

			theApp.ClientSendData(eSoundPlay, action);
		}
	}

	CListView::OnLButtonDown(nFlags, point);
}

void CSoundServerView::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = !theApp.IsServer() || !GetLeftPane()->IsNormalSelected();
}

void CSoundServerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (point.x == -1 && point.y == -1)
	{
		CRect rect;
		if (GetListCtrl().GetSelectedCount() == 0)
		{
			GetListCtrl().GetWindowRect(&rect);
			point = rect.TopLeft();
		}
		else
		{
			GetListCtrl().GetItemRect(GetListCtrl().GetSelectionMark(), &rect, LVIR_ICON);
			GetListCtrl().ClientToScreen(rect);
			point = rect.CenterPoint();
		}
	}

	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);
	menu.GetSubMenu(theApp.IsServer() ? 2 : 3)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, GetMainFrame());
}
