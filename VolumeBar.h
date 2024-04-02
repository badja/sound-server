#pragma once


// CVolumeBar

class CVolumeBar : public CDialogBar
{
	DECLARE_DYNAMIC(CVolumeBar)

public:
	CVolumeBar();
	virtual ~CVolumeBar();

	int GetVolumeSlider();
	void SetVolumeSlider(int nVolume);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


