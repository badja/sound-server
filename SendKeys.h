#pragma once

class CSendKeys
{
public:
	CSendKeys();
	virtual ~CSendKeys();

	void RevertKeyStates();

	void SendKeyDown(BYTE vVk);
	void SendKeyUp(BYTE vVk);
	void SendKeyPress(SHORT sVk);
	void SendKeyChar(TCHAR c);
	void SendKeyString(LPCTSTR lpszString);

protected:
	BYTE m_byteShift;
	BYTE m_byteControl;
	BYTE m_byteMenu;
};
