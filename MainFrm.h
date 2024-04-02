// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "VolumeBar.h"

class CLeftView;
class CSoundServerView;
class CSoundServerDoc;
class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
	CLeftView* GetLeftPane() const;
	CSoundServerView* GetRightPane() const;
	CSoundServerDoc* GetDocument() const;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int GetVolumeSlider();
	void SetVolumeSlider(int nVolume);
	void RefreshMasterFolder();
	void DeleteMissingSounds();

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CVolumeBar      m_wndVolumeBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	BOOL BrowseForFolder(CString strTitle, CString& strFolder);
	CString AddGroupFromFolder(CString strPath);
	CString AddGroupsFromSubfolders(CString strPath);

	afx_msg LRESULT OnUpdateConnection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDataReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMmWomDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnAddGroup();
	afx_msg void OnAddSound();
	afx_msg void OnEditGroup();
	afx_msg void OnUpdateAddSound(CCmdUI *pCmdUI);
	afx_msg void OnEditSound();
	afx_msg void OnUpdateEditSound(CCmdUI *pCmdUI);
	afx_msg void OnDeleteGroup();
	afx_msg void OnUpdateDeleteGroup(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditGroup(CCmdUI *pCmdUI);
	afx_msg void OnDeleteSound();
	afx_msg void OnUpdateDeleteSound(CCmdUI *pCmdUI);
	afx_msg void OnPlaySound();
	afx_msg void OnUpdatePlaySound(CCmdUI *pCmdUI);
	afx_msg void OnPlayGroup();
	afx_msg void OnUpdatePlayGroup(CCmdUI *pCmdUI);
	afx_msg void OnConfigure();
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI *pCmdUI);
	afx_msg void OnDisconnect();
	afx_msg void OnUpdateDisconnect(CCmdUI *pCmdUI);
	afx_msg void OnAddSoundsFromFiles();
	afx_msg void OnUpdateAddSoundsFromFiles(CCmdUI *pCmdUI);
	afx_msg void OnAddGroupFromFolder();
	afx_msg void OnAddGroupsFromSubfolders();
	afx_msg void OnSoundAddFavorites();
	afx_msg void OnUpdateSoundAddFavorites(CCmdUI *pCmdUI);
	afx_msg void OnDeleteFavorites();
	afx_msg void OnUpdateDeleteFavorites(CCmdUI *pCmdUI);
	afx_msg void OnSoundAddSpeech();
	afx_msg void OnUpdateSoundAddSpeech(CCmdUI *pCmdUI);
	afx_msg void OnSoundEditSpeech();
	afx_msg void OnUpdateSoundEditSpeech(CCmdUI *pCmdUI);
	afx_msg void OnDeleteSpeech();
	afx_msg void OnUpdateDeleteSpeech(CCmdUI *pCmdUI);
	afx_msg void OnSoundTestSpeech();
	afx_msg void OnUpdateSoundTestSpeech(CCmdUI *pCmdUI);
	afx_msg void OnPlayStop();
	afx_msg void OnUpdatePlayStop(CCmdUI *pCmdUI);
	afx_msg void OnSetMasterFolder();
};


