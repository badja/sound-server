// VolumeBar.cpp : implementation file
//

#include "stdafx.h"
#include "SoundServer.h"
#include "VolumeBar.h"


// CVolumeBar

IMPLEMENT_DYNAMIC(CVolumeBar, CDialogBar)
CVolumeBar::CVolumeBar()
{
}

CVolumeBar::~CVolumeBar()
{
}


BEGIN_MESSAGE_MAP(CVolumeBar, CDialogBar)
	ON_WM_HSCROLL()
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()



// CVolumeBar message handlers

LONG CVolumeBar::OnInitDialog(UINT wParam, LONG lParam)
{
	BOOL bRet = (BOOL)HandleInitDialog(wParam, lParam);

	SetVolumeSlider(DEFAULT_VOLUME);

	return bRet; 
}

void CVolumeBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (theApp.IsServer() && (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK))
	{
		theApp.SetVolume(nPos);
	}
	else if (nSBCode == SB_ENDSCROLL)
	{
		int nVolume = ((CSliderCtrl*)pScrollBar)->GetPos();
		CAction action(NULL, NULL, NULL, NULL, nVolume);

		if (theApp.IsServer())
		{
			theApp.SetVolume(nVolume);
			theApp.ServerSendData(eVolumeSet, action);
		}
		else
		{
			theApp.ClientSendData(eVolumeSet, action);
		}
	}

	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}

int CVolumeBar::GetVolumeSlider()
{
	return ((CSliderCtrl*)GetDlgItem(IDC_VOLUME))->GetPos();
}

void CVolumeBar::SetVolumeSlider(int nVolume)
{
	((CSliderCtrl*)GetDlgItem(IDC_VOLUME))->SetPos(nVolume);
}
