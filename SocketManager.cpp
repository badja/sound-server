// SocketManager.cpp: implementation of the CSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlconv.h>
#include "SocketManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
const UINT EVT_CONSUCCESS = 0x0000;	// Connection established
const UINT EVT_CONFAILURE = 0x0001;	// General failure - Wait Connection failed
const UINT EVT_CONDROP	  = 0x0002;	// Connection dropped
const UINT EVT_ZEROLENGTH = 0x0003;	// Zero length message
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketManager::CSocketManager()
: m_pMsgWnd(NULL)
{

}

CSocketManager::~CSocketManager()
{

}


void CSocketManager::DisplayData(const LPBYTE lpData, DWORD dwCount, const SockAddrIn& sfrom)
{
	USES_CONVERSION;

	CString strData;
	memcpy(strData.GetBuffer(dwCount), A2CT((LPSTR)lpData), dwCount * sizeof(TCHAR));
	strData.ReleaseBuffer(dwCount);
	if (!sfrom.IsNull())
	{
		LONG  uAddr = sfrom.GetIPAddr();
		BYTE* sAddr = (BYTE*) &uAddr;
		short nPort = ntohs( sfrom.GetPort() );	// show port in host format...
		CString strAddr;
		// Address is stored in network format...
		strAddr.Format(_T("%u.%u.%u.%u (%d)>"),
					(UINT)(sAddr[0]), (UINT)(sAddr[1]),
					(UINT)(sAddr[2]), (UINT)(sAddr[3]), nPort);

		strData = strAddr + strData;
	}

	AppendMessage( strData );
	CString* pData = new CString(strData);
	m_pMsgWnd->PostMessage( WM_DATA_RECEIVED, (WPARAM) pData, (LPARAM) this);
}


void CSocketManager::AppendMessage(LPCTSTR strText )
{
/*	if (NULL == m_pMsgCtrl)
		return;*/
/*
	if (::IsWindow( m_pMsgCtrl->GetSafeHwnd() ))
	{
		int nLen = m_pMsgCtrl->GetWindowTextLength();
		m_pMsgCtrl->SetSel(nLen, nLen);
		m_pMsgCtrl->ReplaceSel( strText );
	}
*/
/*	HWND hWnd = m_pMsgCtrl->GetSafeHwnd();
	DWORD dwResult = 0;
	if (SendMessageTimeout(hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_NORMAL, 1000L, &dwResult) != 0)
	{
		int nLen = (int) dwResult;
		if (SendMessageTimeout(hWnd, EM_SETSEL, nLen, nLen, SMTO_NORMAL, 1000L, &dwResult) != 0)
		{
			if (SendMessageTimeout(hWnd, EM_REPLACESEL, FALSE, (LPARAM)strText, SMTO_NORMAL, 1000L, &dwResult) != 0)
			{
			}
		}

	}*/
}


void CSocketManager::SetMessageWindow(CWnd* pMsgWnd)
{
	m_pMsgWnd = pMsgWnd;
}


void CSocketManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	SockAddrIn saddr_in;
	LPBYTE lpData = lpBuffer;
	if (IsSmartAddressing())
	{
		saddr_in.SetAddr((SOCKADDR_IN*) lpBuffer);
		lpData = &lpData[sizeof(SOCKADDR_IN)];
		if (IsServer())
		{
			// broadcast message to all
			SockAddrIn sdest_in;
			sdest_in.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			memcpy(lpBuffer, (LPSOCKADDR)sdest_in, sdest_in.Size());
			WriteComm(lpBuffer, dwCount, 0L);
		}
		dwCount -= sizeof(SOCKADDR_IN);
	}

	// Display data to message list
	DWORD dwLast = 0;
	for (DWORD dwPos = 0; dwPos < dwCount; dwPos++)
	{
		if (*(lpData + dwPos) == '\0')
		{
			DisplayData( lpData + dwLast, dwPos - dwLast, saddr_in );
			dwLast = dwPos + 1;
		}
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////
// OnEvent
// Send message to parent window to indicate connection status
void CSocketManager::OnEvent(UINT uEvent)
{
	if (NULL == m_pMsgWnd)
		return;

/*	CWnd* pParent = m_pMsgCtrl->GetParent();
	if (!::IsWindow( pParent->GetSafeHwnd()))
		return;

	switch( uEvent )
	{
		case EVT_CONSUCCESS:
			AppendMessage( _T("Connection Established\r\n") );
			break;
		case EVT_CONFAILURE:
			AppendMessage( _T("Connection Failed\r\n") );
			break;
		case EVT_CONDROP:
			AppendMessage( _T("Connection Abandonned\r\n") );
			break;
		case EVT_ZEROLENGTH:
			AppendMessage( _T("Zero Length Message\r\n") );
			break;
		default:
			TRACE("Unknown Socket event\n");
			break;
	}
*/
	if (::IsWindow(m_pMsgWnd->m_hWnd))
		m_pMsgWnd->PostMessage( WM_UPDATE_CONNECTION, uEvent, (LPARAM) this);
}
