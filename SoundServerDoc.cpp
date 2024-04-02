// SoundServerDoc.cpp : implementation of the CSoundServerDoc class
//

#include "stdafx.h"
#include "SoundServer.h"

#include "SoundServerDoc.h"
#include ".\soundserverdoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSoundServerDoc

IMPLEMENT_DYNCREATE(CSoundServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSoundServerDoc, CDocument)
END_MESSAGE_MAP()


// CSoundServerDoc construction/destruction

CSoundServerDoc::CSoundServerDoc()
{
	// TODO: add one-time construction code here

}

CSoundServerDoc::~CSoundServerDoc()
{
	ClearAllSounds();
}

BOOL CSoundServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if (theApp.IsServer())
		ReadSoundsFromRegistry();

	ReadFavoritesFromRegistry();
	ReadSpeechFromRegistry();

	UpdateAllViews(NULL, eInitialise);

	// (SDI documents will reuse this document)

	return TRUE;
}




// CSoundServerDoc serialization

void CSoundServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CSoundServerDoc diagnostics

#ifdef _DEBUG
void CSoundServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSoundServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSoundServerDoc commands

BOOL CSoundServerDoc::AddGroup(CString strGroup, BOOL bWriteToRegistry /*= TRUE*/)
{
	CMapStringToString* pmapSoundToFile;

	if (m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	m_mapGroupToSounds.SetAt(strGroup, new CMapStringToString);

	if (bWriteToRegistry)
		WriteSoundsToRegistry();

	UpdateAllViewsAndClient(eGroupAdd, CAction(strGroup));

	return TRUE;
}

BOOL CSoundServerDoc::AddSound(CString strGroup, CString strSound, CString strFile, BOOL bWriteToRegistry /*= TRUE*/)
{
	CMapStringToString* pmapSoundToFile;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	CString strDummy;

	if (pmapSoundToFile->Lookup(strSound, strDummy))
		return FALSE;

	pmapSoundToFile->SetAt(strSound, strFile);

	if (bWriteToRegistry)
		WriteSoundsToRegistry();

	UpdateAllViewsAndClient(eSoundAdd, CAction(strGroup, strSound, strFile));

	return TRUE;
}

BOOL CSoundServerDoc::UpdateGroupList(CListCtrl& lc)
{
	if (!lc.DeleteAllItems())
		return FALSE;

	lc.InsertItem(0, GROUPNAME_FAVORITES);
	lc.InsertItem(0, GROUPNAME_SPEECH);

	CString strGroup;
	CMapStringToString* pmapSoundToFile;

	for (POSITION pos = m_mapGroupToSounds.GetStartPosition(); pos != NULL; )
	{
		m_mapGroupToSounds.GetNextAssoc(pos, strGroup, pmapSoundToFile);
		lc.InsertItem(lc.GetItemCount(), strGroup);
	}

	return TRUE;
}

BOOL CSoundServerDoc::UpdateSoundList(CListCtrl& lc, CString strGroup)
{
	if (!lc.DeleteAllItems())
		return FALSE;

	CMapStringToString* pmapSoundToFile;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	CString strSound, strFile;

	for (POSITION pos = pmapSoundToFile->GetStartPosition(); pos != NULL; )
	{
		pmapSoundToFile->GetNextAssoc(pos, strSound, strFile);
		theApp.InsertListItem(lc, strSound, strFile);
	}

	return TRUE;
}

BOOL CSoundServerDoc::UpdateFavoritesList(CListCtrl& lc)
{
	if (!lc.DeleteAllItems())
		return FALSE;

	for (INT_PTR i = 0; i < m_saFavoriteGroups.GetCount(); i++)
		theApp.InsertListItem(lc, m_saFavoriteSounds[i], m_saFavoriteGroups[i]);

	return TRUE;
}

BOOL CSoundServerDoc::UpdateSpeechList(CListCtrl& lc)
{
	if (!lc.DeleteAllItems())
		return FALSE;

	theApp.InsertListItem(lc, _T("01 - Male"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("02 - Female"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("03 - Large Male"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("04 - Child"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("05 - Low Male"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("06 - Mellow Female"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("07 - Mellow Male"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("08 - Crisp Male"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("09 - The Fly"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("10 - Robotoid"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("11 - Martian"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("12 - Colossus"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("13 - Fast Fred"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("14 - Old Woman"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("15 - Munchkin"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("16 - Troll"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("17 - Nerd"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("18 - Milktoast"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("19 - Tipsy"), GROUPNAME_VOICE);
	theApp.InsertListItem(lc, _T("20 - Choirboy"), GROUPNAME_VOICE);

	for (INT_PTR i = 0; i < m_saSpeechText.GetCount(); i++)
		theApp.InsertListItem(lc, m_saSpeechText[i], GROUPNAME_SPEECH);

	return TRUE;
}

BOOL CSoundServerDoc::EditGroup(CString strGroup, CString strNewGroup)
{
	CMapStringToString* pmapSoundToFile;

	if (strGroup != strNewGroup && m_mapGroupToSounds.Lookup(strNewGroup, pmapSoundToFile))
		return FALSE;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	if (!m_mapGroupToSounds.RemoveKey(strGroup))
		return FALSE;

	m_mapGroupToSounds.SetAt(strNewGroup, pmapSoundToFile);

	WriteSoundsToRegistry();
	UpdateAllViewsAndClient(eGroupEdit, CAction(strGroup, NULL, NULL, &CAction(strNewGroup)));

	return TRUE;
}

BOOL CSoundServerDoc::EditSound(CString strGroup, CString strSound, CString strNewSound, CString strNewFile)
{
	CMapStringToString* pmapSoundToFile;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	CString strFile;

	if (strSound != strNewSound && pmapSoundToFile->Lookup(strNewSound, strFile))
		return FALSE;

	if (!pmapSoundToFile->RemoveKey(strSound))
		return FALSE;

	pmapSoundToFile->SetAt(strNewSound, strNewFile);

	WriteSoundsToRegistry();
	UpdateAllViewsAndClient(eSoundEdit, CAction(strGroup, strSound, NULL, &CAction(NULL, strNewSound, strNewFile)));

	return TRUE;
}

BOOL CSoundServerDoc::DeleteGroup(CString strGroup)
{
	CMapStringToString* pmapSoundToFile;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	if (!m_mapGroupToSounds.RemoveKey(strGroup))
		return FALSE;

	delete pmapSoundToFile;

	WriteSoundsToRegistry();
	UpdateAllViewsAndClient(eGroupDelete, CAction(strGroup));

	return TRUE;
}

BOOL CSoundServerDoc::DeleteSound(CString strGroup, CString strSound)
{
	CMapStringToString* pmapSoundToFile;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	if (!pmapSoundToFile->RemoveKey(strSound))
		return FALSE;

	WriteSoundsToRegistry();
	UpdateAllViewsAndClient(eSoundDelete, CAction(strGroup, strSound));

	return TRUE;
}

void CSoundServerDoc::ReadSoundsFromRegistry()
{
	CRegKey rkSounds;
	if (rkSounds.Open(theApp.GetAppRegistryKey(), REG_KEY_SOUNDS, KEY_READ) != ERROR_SUCCESS)
		return;

	DWORD dwGroupIndex = 0, dwGroupNameLength = REG_LIMIT_KEYNAME + 1;
	CString strGroupName, strSoundName, strFileName;
	CMapStringToString* pmapSoundToFile;

	while (rkSounds.EnumKey(dwGroupIndex++, strGroupName.GetBuffer(REG_LIMIT_KEYNAME), &dwGroupNameLength) == ERROR_SUCCESS)
	{
		strGroupName.ReleaseBuffer(dwGroupNameLength);
		dwGroupNameLength = REG_LIMIT_KEYNAME + 1;

		pmapSoundToFile = new CMapStringToString;
		m_mapGroupToSounds.SetAt(strGroupName, pmapSoundToFile);

		CRegKey rkGroup;
		rkGroup.Open(rkSounds, strGroupName, KEY_READ);

		DWORD dwSoundIndex = 0, dwSoundNameLength = REG_LIMIT_VALUENAME + 1, dwFileNameLength = REG_LIMIT_VALUE + 1;
		while (::RegEnumValue(rkGroup, dwSoundIndex++, strSoundName.GetBuffer(REG_LIMIT_VALUENAME), &dwSoundNameLength, NULL, NULL, (LPBYTE)strFileName.GetBuffer(REG_LIMIT_VALUE), &dwFileNameLength) == ERROR_SUCCESS)
		{
			strSoundName.ReleaseBuffer(dwSoundNameLength);
			dwSoundNameLength = REG_LIMIT_VALUENAME + 1;

			strFileName.ReleaseBuffer(dwFileNameLength);
			dwFileNameLength = REG_LIMIT_VALUE + 1;

			pmapSoundToFile->SetAt(strSoundName, strFileName);
		}
	}
}

void CSoundServerDoc::WriteSoundsToRegistry()
{
	if (!theApp.IsServer())
		return;

	CRegKey rk;
	rk.Attach(theApp.GetAppRegistryKey());
	rk.RecurseDeleteKey(REG_KEY_SOUNDS);

	CRegKey rkSounds;
	rkSounds.Create(rk, REG_KEY_SOUNDS);

	rk.Detach();

	CString strGroup, strSound, strFile;
	CMapStringToString* pmapSoundToFile;

	for (POSITION posGroup = m_mapGroupToSounds.GetStartPosition(); posGroup != NULL; )
	{
		m_mapGroupToSounds.GetNextAssoc(posGroup, strGroup, pmapSoundToFile);

		CRegKey rkGroup;
		rkGroup.Create(rkSounds, strGroup);

		for (POSITION posSound = pmapSoundToFile->GetStartPosition(); posSound != NULL; )
		{
			pmapSoundToFile->GetNextAssoc(posSound, strSound, strFile);
			rkGroup.SetStringValue(strSound, strFile);
		}
	}
}

BOOL CSoundServerDoc::PlaySound(CString strGroup, CString strSound)
{
	if (strGroup == GROUPNAME_SPEECH)
	{
		return theApp.SpeakText(strSound);
	}
	else
	{
		CMapStringToString* pmapSoundToFile;

		if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
			return FALSE;

		CString strFile;

		if (!pmapSoundToFile->Lookup(strSound, strFile))
			return FALSE;

		return theApp.PlaySound(strFile);
	}
}

BOOL CSoundServerDoc::PlayGroup(CString strGroup)
{
	CMapStringToString* pmapSoundToFile;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	CString strSound, strFile;
	INT_PTR nSoundCount = pmapSoundToFile->GetCount();

	if (nSoundCount > 0)
	{
		POSITION pos;
		int i;
		for (pos = pmapSoundToFile->GetStartPosition(), i = ::rand() % (int)nSoundCount; pos != NULL, i >= 0; i--)
			pmapSoundToFile->GetNextAssoc(pos, strSound, strFile);

		return theApp.PlaySound(strFile);
	}
	
	return TRUE;
}

void CSoundServerDoc::ServerSendSounds(CSocketManager* pManager)
{
	CString strGroup, strSound, strFile;
	CMapStringToString* pmapSoundToFile;

	for (POSITION posGroup = m_mapGroupToSounds.GetStartPosition(); posGroup != NULL; )
	{
		m_mapGroupToSounds.GetNextAssoc(posGroup, strGroup, pmapSoundToFile);
		theApp.ServerSendData(eGroupAdd, CAction(strGroup), pManager);

		for (POSITION posSound = pmapSoundToFile->GetStartPosition(); posSound != NULL; )
		{
			pmapSoundToFile->GetNextAssoc(posSound, strSound, strFile);
			theApp.ServerSendData(eSoundAdd, CAction(strGroup, strSound), pManager);
		}
	}
}

BOOL CSoundServerDoc::UpdateDocument(EAction eAction, CAction& action)
{
	switch (eAction)
	{
	case eGroupAdd:
		return AddGroup(action.m_strGroup);
	case eGroupEdit:
		return EditGroup(action.m_strGroup, action.m_pActionNew->m_strGroup);
	case eGroupDelete:
		return DeleteGroup(action.m_strGroup);
	case eSoundAdd:
		return AddSound(action.m_strGroup, action.m_strSound, action.m_strFile);
	case eSoundEdit:
		return EditSound(action.m_strGroup, action.m_strSound, action.m_pActionNew->m_strSound, action.m_pActionNew->m_strSound);
	case eSoundDelete:
		return DeleteSound(action.m_strGroup, action.m_strSound);
	case eVolumeSet:
		((CMainFrame*)AfxGetMainWnd())->SetVolumeSlider((int)action.m_dwData);
		return TRUE;
	default:
		return FALSE;
	}
}

void CSoundServerDoc::UpdateAllViewsAndClient(EAction eAction, CAction& action)
{
	UpdateAllViews(NULL, eAction, &action);

	if (theApp.IsServer())
	{
		action.m_strFile.Empty();
		theApp.ServerSendData(eAction, action);
	}
}

void CSoundServerDoc::ClearAllSounds()
{
	CString strGroup;
	CMapStringToString* pmapSoundToFile;

	for (POSITION pos = m_mapGroupToSounds.GetStartPosition(); pos != NULL; )
	{
		m_mapGroupToSounds.GetNextAssoc(pos, strGroup, pmapSoundToFile);
		delete pmapSoundToFile;
	}

	m_mapGroupToSounds.RemoveAll();

	UpdateAllViews(NULL, eInitialise);
}

BOOL CSoundServerDoc::AddToFavorites(CString strGroup, CString strSound)
{
	CMapStringToString* pmapSoundToFile;

	if (!m_mapGroupToSounds.Lookup(strGroup, pmapSoundToFile))
		return FALSE;

	CString strFile;

	if (!pmapSoundToFile->Lookup(strSound, strFile))
		return FALSE;

	m_saFavoriteGroups.Add(strGroup);
	m_saFavoriteSounds.Add(strSound);

	WriteFavoritesToRegistry();
	UpdateAllViewsAndClient(eFavoriteAdd, CAction(strGroup, strSound));

	return TRUE;
}

BOOL CSoundServerDoc::DeleteFromFavorites(CString strGroup, CString strSound)
{
	for (INT_PTR i = 0; i < m_saFavoriteGroups.GetCount(); i++)
	{
		if (m_saFavoriteGroups[i] == strGroup && m_saFavoriteSounds[i] == strSound)
		{
			m_saFavoriteGroups.RemoveAt(i);
			m_saFavoriteSounds.RemoveAt(i);
			break;
		}
	}

	WriteFavoritesToRegistry();
	UpdateAllViewsAndClient(eFavoriteDelete, CAction(strGroup, strSound));

	return TRUE;
}

BOOL CSoundServerDoc::AddToSpeech(CString strText)
{
	for (INT_PTR i = 0; i < m_saSpeechText.GetCount(); i++)
	{
		if (m_saSpeechText[i] == strText)
			return FALSE;
	}

	m_saSpeechText.Add(strText);

	WriteSpeechToRegistry();
	UpdateAllViewsAndClient(eSpeechAdd, CAction(GROUPNAME_SPEECH, strText));

	return TRUE;
}

BOOL CSoundServerDoc::EditSpeech(CString strText, CString strNewText)
{
	if (strText == strNewText)
		return TRUE;

	for (INT_PTR i = 0; i < m_saSpeechText.GetCount(); i++)
	{
		if (m_saSpeechText[i] == strNewText)
			return FALSE;
	}

	for (INT_PTR i = 0; i < m_saSpeechText.GetCount(); i++)
	{
		if (m_saSpeechText[i] == strText)
		{
			m_saSpeechText[i] = strNewText;
			break;
		}
	}

	WriteSpeechToRegistry();
	UpdateAllViewsAndClient(eSpeechEdit, CAction(GROUPNAME_SPEECH, strText, NULL, &CAction(GROUPNAME_SPEECH, strNewText)));

	return TRUE;
}

BOOL CSoundServerDoc::DeleteFromSpeech(CString strText)
{
	for (INT_PTR i = 0; i < m_saSpeechText.GetCount(); i++)
	{
		if (m_saSpeechText[i] == strText)
		{
			m_saSpeechText.RemoveAt(i);
			WriteSpeechToRegistry();
			UpdateAllViewsAndClient(eSpeechDelete, CAction(GROUPNAME_SPEECH, strText));
		}
	}

	return TRUE;
}

void CSoundServerDoc::ReadFavoritesFromRegistry()
{
	CRegKey rkFavorites;
	rkFavorites.Open(theApp.GetAppRegistryKey(), REG_KEY_FAVORITES, KEY_READ);

	QueryMultiStringValue(rkFavorites, REG_VALUENAME_GROUPS, m_saFavoriteGroups);
	QueryMultiStringValue(rkFavorites, REG_VALUENAME_SOUNDS, m_saFavoriteSounds);
}

void CSoundServerDoc::WriteFavoritesToRegistry()
{
	CRegKey rkFavorites;
	rkFavorites.Create(theApp.GetAppRegistryKey(), REG_KEY_FAVORITES);

	SetMultiStringValue(rkFavorites, REG_VALUENAME_GROUPS, m_saFavoriteGroups);
	SetMultiStringValue(rkFavorites, REG_VALUENAME_SOUNDS, m_saFavoriteSounds);
}

void CSoundServerDoc::ReadSpeechFromRegistry()
{
	CRegKey rkSpeech;
	rkSpeech.Open(theApp.GetAppRegistryKey(), REG_KEY_SPEECH, KEY_READ);

	QueryMultiStringValue(rkSpeech, REG_VALUENAME_TEXT, m_saSpeechText);
}

void CSoundServerDoc::WriteSpeechToRegistry()
{
	CRegKey rkSpeech;
	rkSpeech.Create(theApp.GetAppRegistryKey(), REG_KEY_SPEECH);

	SetMultiStringValue(rkSpeech, REG_VALUENAME_TEXT, m_saSpeechText);
}

void CSoundServerDoc::QueryMultiStringValue(CRegKey& rk, LPCTSTR pszValueName, CStringArray& saValue)
{
	if (rk == NULL)
		return;

	ULONG nChars;
	CString strValue;

	rk.QueryMultiStringValue(pszValueName, NULL, &nChars);

	if (nChars > 0)
	{
		rk.QueryMultiStringValue(pszValueName, strValue.GetBuffer(nChars - 1), &nChars);
		strValue.ReleaseBuffer(nChars - 1);
	}

	saValue.RemoveAll();

	int nPos = 0;

	while (nPos < strValue.GetLength())
	{
		saValue.Add(strValue.GetString() + nPos);
		nPos = strValue.Find(_T('\0'), nPos) + 1;
	}
}

void CSoundServerDoc::SetMultiStringValue(CRegKey& rk, LPCTSTR pszValueName, CStringArray& saValue)
{
	CString strValue;

	for (INT_PTR i = 0; i < saValue.GetCount(); i++)
		strValue += saValue[i] + _T('\1');

	strValue.Replace(_T('\1'), _T('\0'));

	rk.SetMultiStringValue(pszValueName, strValue);
}

void CSoundServerDoc::DeleteMissingSounds()
{
	CString strGroup, strSound, strFile;
	CMapStringToString* pmapSoundToFile;
	CStringArray saDeleteSoundGroup, saDeleteSoundName, saDeleteGroupName;

	// Delete missing sounds

	for (POSITION posGroup = m_mapGroupToSounds.GetStartPosition(); posGroup != NULL; )
	{
		m_mapGroupToSounds.GetNextAssoc(posGroup, strGroup, pmapSoundToFile);

		for (POSITION posSound = pmapSoundToFile->GetStartPosition(); posSound != NULL; )
		{
			pmapSoundToFile->GetNextAssoc(posSound, strSound, strFile);

			if (GetFileAttributes(strFile) == INVALID_FILE_ATTRIBUTES)
			{
				saDeleteSoundGroup.Add(strGroup);
				saDeleteSoundName.Add(strSound);
			}
		}
	}

	INT_PTR nSoundsToDelete = saDeleteSoundGroup.GetCount();

	for (INT_PTR i = 0; i < nSoundsToDelete; i++)
		DeleteSound(saDeleteSoundGroup[i], saDeleteSoundName[i]);


	// Delete empty groups

	for (POSITION posGroup = m_mapGroupToSounds.GetStartPosition(); posGroup != NULL; )
	{
		m_mapGroupToSounds.GetNextAssoc(posGroup, strGroup, pmapSoundToFile);

		if (pmapSoundToFile->IsEmpty())
			saDeleteGroupName.Add(strGroup);
	}

	INT_PTR nGroupsToDelete = saDeleteGroupName.GetCount();

	for (INT_PTR i = 0; i < nGroupsToDelete; i++)
		DeleteGroup(saDeleteGroupName[i]);
}
