/******************************************************************************
 File     : UdpSocket.h
 Author   : SDY
 Date     : 10.06.2002
 Comments : "UdpSocket.txt"
 
******************************************************************************/

#ifndef __UdpSocket_h
#define __UdpSocket_h

#include "SocketObj.h"
#include "../Timer/ElapsedTimeCounter.h"
#include "../Thread/ThreadEx.h"

namespace b_set
{

//#define STR_DEF_MCAST_ADDR	"234.0.0.0"		// default multicast address ( reserved for NITA :)

///////////////////////////////////////////////////////////////////////////////
// CUdpSocket
class b_DLL CUdpSocket : virtual public CSocketObj, public CThreadEx
{
////////// Attributes:
public:
protected:
	CSockAddrIn m_saSockAddrSendTo;

////////// Construction:
public:
	CUdpSocket();
	virtual ~CUdpSocket();

////////// Operations:
public:
	virtual void AddCloseEvent(const CMultipleWait& mwCloseEvents);
	CMultipleWait GetCloseEvents();
	BOOL CreateSocket();
	void SetObjectName(LPCTSTR lpObjectName);
	int SetAsyncMode();

	CString GetObjectName();
//	UINT GetMaxMsgSize(); // UDP ONLY: for message-oriented protocol only
	BOOL EnableBroadcastMessages();
	BOOL Read(LPVOID const lpBuffer, DWORD dwDataSize,LPDWORD lpdwBytesProcessed = NULL, DWORD dwWaitTime = INFINITE);

// Sender specific:
public:
	// Use CSockAddrIn
	void SetHost(const sockaddr_in& saSockAddrSendTo);
	// SendTo uses address set by SetHost
	BOOL SendTo(LPVOID lpBuffer, DWORD dwDataSize,LPDWORD lpdwBytesProcessed = NULL, DWORD dwWaitTime = INFINITE);
	bool SendTo(const char* lpDestinationAddress,unsigned short usnoDestinationPort,const void* buf,int bufLength);
	CSockAddrIn GetSockAddrSend() const;

// Receiver specific:
public:
	BOOL StartReceiver();
	void StopReceiver();

// Multicast sender specific:
public:
	// Each multicast transmission is sent from a SINGLE network interface!!!
	// Use SetMulticastOutgoingInterface to override default interface 
	// Attention!!! For NT40 BIND socket to local interface before SetMulticastOutgoingInterface.
	BOOL SetMulticastOutgoingInterface(UINT ulLocalAddress_n); 
	BOOL SetMulticastOutgoingInterface(const char* lpLocalAddress); 
	
	// Messages with TTL (Time-To-Live) > 1 may be delivered to more than one subnetwork.
	// A multicast router does not forward multicast datagrams with destination addresses 
	// from 224.0.0.0 through 224.0.0.255, inclusive, regardless of their TTL value. 
	// This address range is reserved for routing protocols 
	BOOL SetMulticastTTL(INT nTTL = 64);

// Multicast receiver specific:
public:
	BOOL JoinMulticastGroup(const char* lpMulticastAddr, const char* lpInterfaceAddr = NULL);
	BOOL JoinMulticastGroup(ULONG ulMulticastAddr_n, ULONG ulInterfaceAddr_n);
	BOOL JoinMulticastGroup(ULONG ulMulticastAddr_n); // for all interfaces
	BOOL DropMulticastGroup(const char* lpMulticastAddr, const char* lpInterfaceAddr = NULL);
	BOOL DropMulticastGroup(ULONG ulMulticastAddr_n, ULONG ulInterfaceAddr_n);
	BOOL DropMulticastGroup(ULONG ulMulticastAddr_n); // for all interfaces

protected:
	virtual DWORD thrd_Proc();
	virtual void DataExchangeReceiver();
	BOOL IsObjectNameEmpty() const;
};

///////////////////////////////
// CUdpSocket inlines
inline BOOL CUdpSocket::Read(LPVOID const lpBuffer, DWORD dwDataSize, LPDWORD lpdwBytesProcessed, DWORD dwWaitTime)
{
	BOOL nRetVal = FALSE;

	CElapsedTimeCounter TimeCounter;
	TimeCounter.Begin();

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000;

	while( !WaitForCloseEvent(0) )
	{
		nRetVal = SelectRead(this,&tv);
		if ( nRetVal > 0 )
		{
			if ( lpdwBytesProcessed )
				*lpdwBytesProcessed = (unsigned long)CBaseSocket::Receive(lpBuffer,dwDataSize);
			if( *lpdwBytesProcessed > 0 )
				break;
		}
		else if ( nRetVal == -1 )
		{
			///! some error recived
			break;
		}

		//! if nRetVal=0 try read again till we have time

		if ( dwWaitTime != INFINITE && TimeCounter.GetActiveTime_ms() >= dwWaitTime  ) {
			break;
		}
	}
	return nRetVal;
}
inline CSockAddrIn CUdpSocket::GetSockAddrSend() const
{
	return m_saSockAddrSendTo;
}
/*
inline void CUdpSocket::AddCloseEvent( const CMultipleWait& mwCloseEvents )
{
	CSocketObj::AddCloseEvent(mwCloseEvents);
}
*/
inline BOOL CUdpSocket::SendTo(LPVOID lpBuffer, DWORD dwDataSize, LPDWORD lpdwBytesProcessed, DWORD dwWaitTime)
{
	BOOL nRetVal = FALSE;
	CElapsedTimeCounter TimeCounter;
	TimeCounter.Begin();

	if ( !CSocketObj::SendTo(&m_saSockAddrSendTo,lpBuffer,dwDataSize) ) {
		return FALSE; //! some error received
	}

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000;

	while( !WaitForCloseEvent(0) )
	{
		nRetVal = SelectWrite(this,&tv);
		if ( nRetVal > 0 )
		{
			if ( lpdwBytesProcessed )
				*lpdwBytesProcessed = dwDataSize;
			break;
		}
		else if ( nRetVal == -1 )
		{
			///! some error received
			break;
		}

		//! if nRetVal=0 try send again till we have time
		if ( dwWaitTime != INFINITE && TimeCounter.GetActiveTime_ms() >= dwWaitTime  ) {
			break;
		}
	}

	return nRetVal;
//!	return CSocketObj::SendTo(&m_saSockAddrSendTo,lpBuffer,dwDataSize,lpdwBytesProcessed,dwWaitTime);
}
inline BOOL CUdpSocket::CreateSocket()
{
	BOOL bRes = CSocketObj::CreateSocket(SOCK_DGRAM, IPPROTO_UDP, AF_INET);
	if(bRes)
	{
		bRes = EnableBroadcastMessages();
		if(!bRes)
			CloseSocket();
	}
	return bRes;
}
inline BOOL CUdpSocket::SetMulticastOutgoingInterface(UINT ulLocalAddress_n)
{
	return SetSockOpt(IPPROTO_IP, IP_MULTICAST_IF, (char*)&ulLocalAddress_n, sizeof(ulLocalAddress_n));
}
inline BOOL CUdpSocket::SetMulticastOutgoingInterface(const char* lpLocalAddress)
{
	return SetMulticastOutgoingInterface(inet_addr(lpLocalAddress));
}
inline BOOL CUdpSocket::SetMulticastTTL(INT nTTL)
{
	return SetSockOpt(IPPROTO_IP, IP_MULTICAST_TTL, (char*)&nTTL, sizeof (nTTL));
}
inline CString CUdpSocket::GetObjectName()
{
	return CSocketObj::GetObjectName();
}
inline BOOL CUdpSocket::IsObjectNameEmpty() const
{
	return CSocketObj::IsObjectNameEmpty();
}
inline void CUdpSocket::SetObjectName(LPCTSTR lpObjectName)
{
	CSocketObj::SetObjectName(lpObjectName);
	CThreadEx::SetObjectName(lpObjectName);
}
inline void CUdpSocket::StopReceiver()
{
	if ( CThreadEx::IsActive() ){
		m_woClose.SetEvent();
		CThreadEx::StopThread();
	}
}
inline BOOL CUdpSocket::JoinMulticastGroup(const char* lpMulticastAddr, const char* lpInterfaceAddr)
{
	ULONG ulAddrIf_n = (lpInterfaceAddr != NULL) ? inet_addr(lpInterfaceAddr) : INADDR_ANY;
	if(ulAddrIf_n == INADDR_ANY)
		return JoinMulticastGroup(inet_addr(lpMulticastAddr) );
	else
		return JoinMulticastGroup(inet_addr(lpMulticastAddr), ulAddrIf_n );
}
inline BOOL CUdpSocket::DropMulticastGroup(const char* lpMulticastAddr, const char* lpInterfaceAddr)
{
	ULONG ulAddrIf_n = (lpInterfaceAddr != NULL) ? inet_addr(lpInterfaceAddr) : INADDR_ANY;
	if(ulAddrIf_n == INADDR_ANY)
		return DropMulticastGroup(inet_addr(lpMulticastAddr) );
	else
		return DropMulticastGroup(inet_addr(lpMulticastAddr), ulAddrIf_n );
}/*
inline UINT CUdpSocket::GetMaxMsgSize()
{
	UINT unMaxMsgSize = 0;
	GetSockOpt(SOL_SOCKET, SO_MAX_MSG_SIZE, &unMaxMsgSize, sizeof(unMaxMsgSize));
	return unMaxMsgSize;
}*/
inline BOOL CUdpSocket::EnableBroadcastMessages()
{
	int nBroadcast = 1;
	return SetSockOpt(SOL_SOCKET, SO_BROADCAST, &nBroadcast, sizeof(nBroadcast) );
}
inline CMultipleWait CUdpSocket::GetCloseEvents()
{
	return CThreadEx::GetCloseEvents();
}

inline bool CUdpSocket::SendTo(const char* lpDestinationAddress,unsigned short usnoDestinationPort,const void* buf,int bufLength)
{
	return CBaseSocket::SendTo(lpDestinationAddress,usnoDestinationPort,buf,bufLength);
}

} //! namespace b_set

#endif //! __UdpSocket_h
