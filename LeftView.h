// LeftView.h : interface of the CLeftView class
//


#pragma once

class CSoundServerDoc;
class CSoundServerView;
class CMainFrame;

class CLeftView : public CListView
{
protected: // create from serialization only
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CSoundServerDoc* GetDocument();

// Operations
public:

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CLeftView();
	CSoundServerView* GetRightPane() const;
	CMainFrame* GetMainFrame() const;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	int FindGroup(CString strGroup);

public:
	BOOL IsSingleGroupSelected();
	BOOL AreMultipleGroupsSelected();
	CString GetSelectedGroup();
	void GetSelectedGroups(CStringArray& saGroups);
	BOOL IsNormalSelected();
	BOOL IsFavoritesSelected();
	BOOL IsSpeechSelected();
	void SelectFavorites();

	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRename();
	afx_msg void OnDelete();
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CSoundServerDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CSoundServerDoc*>(m_pDocument); }
#endif

