// SoundServerDoc.h : interface of the CSoundServerDoc class
//


#pragma once

#define GROUPNAME_FAVORITES	_T("(Favorites)")
#define GROUPNAME_SPEECH	_T("(Speech)")
#define GROUPNAME_VOICE		_T("(Voice)")

class CSoundServerDoc : public CDocument
{
protected: // create from serialization only
	CSoundServerDoc();
	DECLARE_DYNCREATE(CSoundServerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CSoundServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	CMap<CString, LPCTSTR, CMapStringToString*, CMapStringToString*> m_mapGroupToSounds;
	CStringArray m_saFavoriteGroups;
	CStringArray m_saFavoriteSounds;
	CStringArray m_saSpeechText;

public:
	void ReadSoundsFromRegistry();
	void WriteSoundsToRegistry();
	void ReadFavoritesFromRegistry();
	void WriteFavoritesToRegistry();
	void ReadSpeechFromRegistry();
	void WriteSpeechToRegistry();

	BOOL AddGroup(CString strGroup, BOOL bWriteToRegistry = TRUE);
	BOOL AddSound(CString strGroup, CString strSound, CString strFile, BOOL bWriteToRegistry = TRUE);
	BOOL UpdateGroupList(CListCtrl& lc);
	BOOL UpdateSoundList(CListCtrl& lc, CString strGroup);
	BOOL UpdateFavoritesList(CListCtrl& lc);
	BOOL UpdateSpeechList(CListCtrl& lc);
	BOOL EditGroup(CString strGroup, CString strNewGroup);
	BOOL EditSound(CString strGroup, CString strSound, CString strNewSound, CString strNewFile);
	BOOL DeleteGroup(CString strGroup);
	BOOL DeleteSound(CString strGroup, CString strSound);
	BOOL PlaySound(CString strGroup, CString strSound);
	BOOL PlayGroup(CString strGroup);
	void ServerSendSounds(CSocketManager* pManager);
	BOOL UpdateDocument(EAction eAction, CAction& action);
	void UpdateAllViewsAndClient(EAction eAction, CAction& action);
	void ClearAllSounds();
	BOOL AddToFavorites(CString strGroup, CString strSound);
	BOOL DeleteFromFavorites(CString strGroup, CString strSound);
	BOOL AddToSpeech(CString strText);
	BOOL EditSpeech(CString strText, CString strNewText);
	BOOL DeleteFromSpeech(CString strText);
	void DeleteMissingSounds();

protected:
	void QueryMultiStringValue(CRegKey& rk, LPCTSTR pszValueName, CStringArray& saValue);
	void SetMultiStringValue(CRegKey& rk, LPCTSTR pszValueName, CStringArray& saValue);
};
