// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "SoundServer.h"

#include "SoundServerDoc.h"
#include "LeftView.h"
#include ".\leftview.h"

#include "MainFrm.h"
#include "SoundServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CListView)

BEGIN_MESSAGE_MAP(CLeftView, CListView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CLeftView construction/destruction

CLeftView::CLeftView()
{
	// TODO: add construction code here
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (LVS_EDITLABELS | LVS_NOSORTHEADER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SORTASCENDING);

	return CListView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	GetListCtrl().InsertColumn(0, _T("Group Name"), LVCFMT_LEFT, 100);
}


// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CListView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSoundServerDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoundServerDoc)));
	return (CSoundServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView message handlers

CSoundServerView* CLeftView::GetRightPane() const
{
	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	return pFrame->GetRightPane();
}

CMainFrame* CLeftView::GetMainFrame() const
{
	return (CMainFrame*)AfxGetMainWnd();
}

void CLeftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CAction* pAction = (CAction*)pHint;

	int nItem;

	switch (lHint)
	{
	case eInitialise:
		GetDocument()->UpdateGroupList(GetListCtrl());
		break;
	case eGroupAdd:
		GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), pAction->m_strGroup);
		break;
	case eGroupEdit:
		nItem = FindGroup(pAction->m_strGroup);
		if (nItem != -1)
			GetListCtrl().SetItemText(nItem, 0, pAction->m_pActionNew->m_strGroup);
		break;
	case eGroupDelete:
		nItem = FindGroup(pAction->m_strGroup);
		if (nItem != -1)
		{
			GetListCtrl().DeleteItem(nItem);
			GetRightPane()->UpdateSoundList(pAction->m_strGroup);
		}
		break;
	}
}

CString CLeftView::GetSelectedGroup()
{
	if (GetListCtrl().GetSelectedCount() != 1)
		return CString();

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	int nItem = GetListCtrl().GetNextSelectedItem(pos);

	CString strGroup = GetListCtrl().GetItemText(nItem, 0);

	if (!theApp.IsNormalGroup(strGroup))
		strGroup.Empty();

	return strGroup;
}

void CLeftView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED) != (pNMLV->uOldState & LVIS_SELECTED))
	{
		if (IsFavoritesSelected())
			GetRightPane()->UpdateFavoritesList();
		else if (IsSpeechSelected())
			GetRightPane()->UpdateSpeechList();
		else
			GetRightPane()->UpdateSoundList(GetSelectedGroup());
	}

	*pResult = 0;
}

int CLeftView::FindGroup(CString strGroup)
{
	LVFINDINFO lvfi;
	lvfi.flags = LVFI_STRING;
	lvfi.psz = strGroup;

	return GetListCtrl().FindItem(&lvfi);
}

void CLeftView::GetSelectedGroups(CStringArray& saGroups)
{
	saGroups.RemoveAll();

	int nItem;
	CString strGroup;

	for (POSITION pos = GetListCtrl().GetFirstSelectedItemPosition(); pos != NULL; )
	{
		nItem = GetListCtrl().GetNextSelectedItem(pos);
		strGroup = GetListCtrl().GetItemText(nItem, 0);

		if (theApp.IsNormalGroup(strGroup))
			saGroups.Add(GetListCtrl().GetItemText(nItem, 0));
	}
}

void CLeftView::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if (pDispInfo->item.pszText)
	{
		GetDocument()->EditGroup(GetListCtrl().GetItemText(pDispInfo->item.iItem, 0), pDispInfo->item.pszText);
		*pResult = 1;
	}
	else
	{
		*pResult = 0;
	}
}

void CLeftView::OnRename()
{
	if (theApp.IsServer() && IsNormalSelected())
		GetListCtrl().EditLabel(GetListCtrl().GetSelectionMark());
}

void CLeftView::OnDelete()
{
	if (theApp.IsServer() && IsNormalSelected())
		GetMainFrame()->OnDeleteGroup();
}

void CLeftView::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CAction action(GetListCtrl().GetItemText(pNMIA->iItem, 0));

	if (theApp.IsServer())
		GetDocument()->UpdateAllViews(NULL, eGroupPlay, &action);
	else
		theApp.ClientSendData(eGroupPlay, action);

	*pResult = 0;
}

void CLeftView::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = !theApp.IsServer() || !IsNormalSelected();
}

BOOL CLeftView::IsNormalSelected()
{
	if (GetListCtrl().GetSelectedCount() != 1)
		return FALSE;

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	int nItem = GetListCtrl().GetNextSelectedItem(pos);

	return theApp.IsNormalGroup(GetListCtrl().GetItemText(nItem, 0));
}

BOOL CLeftView::IsFavoritesSelected()
{
	if (GetListCtrl().GetSelectedCount() != 1)
		return FALSE;

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	int nItem = GetListCtrl().GetNextSelectedItem(pos);

	return GetListCtrl().GetItemText(nItem, 0) == GROUPNAME_FAVORITES;
}

BOOL CLeftView::IsSpeechSelected()
{
	if (GetListCtrl().GetSelectedCount() != 1)
		return FALSE;

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	int nItem = GetListCtrl().GetNextSelectedItem(pos);

	return GetListCtrl().GetItemText(nItem, 0) == GROUPNAME_SPEECH;
}

BOOL CLeftView::IsSingleGroupSelected()
{
	return !GetSelectedGroup().IsEmpty();
}

BOOL CLeftView::AreMultipleGroupsSelected()
{
	CStringArray saGroups;
	GetSelectedGroups(saGroups);

	return !saGroups.IsEmpty();
}

void CLeftView::OnContextMenu(CWnd* pWnd, CPoint point)
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
	menu.GetSubMenu(theApp.IsServer() ? 0 : 1)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, GetMainFrame());
}

void CLeftView::SelectFavorites()
{
	int nItem = FindGroup(GROUPNAME_FAVORITES);

	for (int i = 0; i < GetListCtrl().GetItemCount(); i++)
		GetListCtrl().SetItemState(i, i == nItem ? LVIS_SELECTED : 0, LVIS_SELECTED);

	GetListCtrl().SetSelectionMark(nItem);
}
