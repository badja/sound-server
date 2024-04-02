#include "StdAfx.h"
#include ".\sendkeys.h"

CSendKeys::CSendKeys()
{
	m_byteShift = HIBYTE(GetKeyState(VK_SHIFT));
	m_byteControl = HIBYTE(GetKeyState(VK_CONTROL));
	m_byteMenu = HIBYTE(GetKeyState(VK_MENU));

	if (m_byteShift)	SendKeyUp(VK_SHIFT);
	if (m_byteControl)	SendKeyUp(VK_CONTROL);
	if (m_byteMenu)		SendKeyUp(VK_MENU);
}

CSendKeys::~CSendKeys()
{
	RevertKeyStates();
}

void CSendKeys::RevertKeyStates()
{
	if (m_byteShift)	SendKeyDown(VK_SHIFT);
	if (m_byteControl)	SendKeyDown(VK_CONTROL);
	if (m_byteMenu)		SendKeyDown(VK_MENU);

	m_byteShift = 0;
	m_byteControl = 0;
	m_byteMenu = 0;
}

void CSendKeys::SendKeyDown(BYTE vVk)
{
	Sleep(10);

	INPUT input;
	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vVk;

	SendInput(1, &input, sizeof(INPUT));
}

void CSendKeys::SendKeyUp(BYTE vVk)
{
	Sleep(10);

	INPUT input;
	ZeroMemory(&input, sizeof(input));

	input.type = INPUT_KEYBOARD;
	input.ki.wVk = vVk;
	input.ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(1, &input, sizeof(INPUT));
}

void CSendKeys::SendKeyPress(SHORT sVk)
{
	Sleep(10);

	WORD wVk = LOBYTE(sVk);
	BYTE bShiftState = HIBYTE(sVk);

	INPUT inputs[4];
	ZeroMemory(inputs, sizeof(inputs));

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_SHIFT;
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = wVk;
	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = wVk;
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_SHIFT;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	if (bShiftState & 0x1)
		SendInput(4, inputs, sizeof(INPUT));
	else
		SendInput(2, inputs + 1, sizeof(INPUT));
}

void CSendKeys::SendKeyChar(TCHAR c)
{
	SendKeyPress(VkKeyScan(c));
}

void CSendKeys::SendKeyString(LPCTSTR lpszString)
{
	LPTSTR lpChar = (LPTSTR)lpszString;
	while (*lpChar != NULL)
	{
		SendKeyChar(*lpChar);
		lpChar = CharNext(lpChar);
	}
}
