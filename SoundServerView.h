// SoundServerView.h : interface of the CSoundServerView class
//


#pragma once

class CLeftView;
class CMainFrame;

class CSoundServerView : public CListView
{
protected: // create from serialization only
	CSoundServerView();
	DECLARE_DYNCREATE(CSoundServerView)

// Attributes
public:
	CSoundServerDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CSoundServerView();
	CLeftView* GetLeftPane() const;
	CMainFrame* GetMainFrame() const;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()

	CImageList m_imageListNormal;
	CImageList m_imageListSmall;

	int FindSound(CString strSound);

	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

public:
	BOOL UpdateSoundList(CString strGroup);
	BOOL UpdateFavoritesList();
	BOOL UpdateSpeechList();
	CString GetSelectedSound();
	void GetSelectedSounds(CStringArray& saSounds);
	CString GetSelectedGroupOrFile();
	void GetSelectedGroupsOrFiles(CStringArray& saGroupsOrSounds);

	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRename();
	afx_msg void OnDelete();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

#ifndef _DEBUG  // debug version in SoundServerView.cpp
inline CSoundServerDoc* CSoundServerView::GetDocument() const
   { return reinterpret_cast<CSoundServerDoc*>(m_pDocument); }
#endif

