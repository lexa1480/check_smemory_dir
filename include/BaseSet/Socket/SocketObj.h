/******************************************************************************
 File     : SocketObj.h
 Author   : SDY
 Date     : 24.04.2002
 Comments : 
			CSocketObj
			CSockAddrIn
			CSocketInitializator
 
 Ws2_32.lib Mswsock.lib
******************************************************************************/

#ifndef __SocketObj_h
#define __SocketObj_h

///////////////////////////////////////////////////////////////////////////////
// You should link the next libraries:
// Ws2_32.lib Mswsock.lib
///////////////////////////////////////////////////////////////////////////////

#include "../BaseTypes_x.h"
#include "../Transport/BaseSocket.h"

#include "../Thread/MultiWait.h"
#include "../BaseObj/BaseObject.h"
#include "../Array/StringArray.h"
#include "../Timer/StreamRetarder.h"
#include "NitaSocket.h"



#ifdef WIN32
	//#include <string>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>

	#ifndef MAX_COMPUTERNAME_LENGTH
		#define MAX_COMPUTERNAME_LENGTH 15
	#endif //! MAX_COMPUTERNAME_LENGTH

	#ifndef MAX_PATH
		#define MAX_PATH 260
	#endif //! MAX_PATH

#endif //! WIN32

namespace b_set
{

/*
	Ports:
	    0 � 1023 are controlled by the IANA and are reserved for well-known services.
	 1024 �49151 are registered ports listed by the IANA and can be used by ordinary user processes or programs executed by ordinary users.
	49152 �65535 are dynamic and/or private ports. 

		IANA ports reservations are listed at %WINDOWS%\System32\Drivers\Etc\services.
	

	Special addresses:
		INADDR_ANY			0.0.0.0				"wildcard", meaning that it does not specify a particular address
		INADDR_LOOPBACK		127.0.0.1			loopback address, host bytes order
		INADDR_BROADCAST	255.255.255.255		broadcast address
	
	Multicast addresses: 
							224.0.0.0 through 239.255.255.255
							224.0.0.0 through 224.0.0.255	  A multicast router does not transmit on any subnetwork
										multicast datagrams with destination addresses 
										from 224.0.0.0 through 224.0.0.255, inclusive, 
										regardless of their TTL value. 
										This address range is reserved for routing protocols 

		Base address		224.0.0.0   Reserved 
		All hosts			224.0.0.1   Contains all systems on the same physical network 
		All routers			224.0.0.2   Contains all routers on the same physical network 
		Time Protocol		224.0.1.1   Distributes time-clock data used to synchronize time on a group of computers 
		RIP version 2		224.0.0.9   Distributes routing data between a group of routers that use a version 2 raster image processor (RIP) 
		WINS server			224.0.1.24  Supports autodiscovery and dynamic configuring of replication for WINS servers 
*/

#define STR_INADDR_ANY			_T("0.0.0.0")
#define STR_INADDR_LOOPBACK		_T("127.0.0.1")
#define STR_INADDR_BROADCAST	_T("255.255.255.255")

#define DEF_TIME_RDWR_ms	30000

///////////////////////////////////////////////////////////////////////////////
// CSockAddrIn
class b_DLL CSockAddrIn : public sockaddr_in
{
public:
	CSockAddrIn()
		{
			Clear();
			sin_family = AF_INET;
		}
	
	CSockAddrIn(const sockaddr_in& sai)
		{*(sockaddr_in*)this = sai;}
	
	CSockAddrIn(const sockaddr& sa)
		{*(sockaddr*)(sockaddr*)this = sa;}

	CSockAddrIn(UINT	unSocketPort_h,		// unSocketPort_h: host byte order
				const char* lpSocketAddress,
				INT		nFamily = AF_INET);
	
	CSockAddrIn(UINT	unSocketPort_h, 
				ULONG	ulSocketAddress_h,	// ulSocketAddress_h: host byte order
				INT		nFamily = AF_INET);

	CSockAddrIn(ULONG	ulSocketAddress_h)	// ulSocketAddress_h: host byte order
		{
			Clear();
			sin_family = AF_INET;
			SetAddr_h( ulSocketAddress_h );
		}


public:
	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}
	CString GetString() const;
	CString GetStringAddr() const;
	CString GetDisplayStringAddr() const;
	CString GetHostName() const;
	bool IsLocal() const;
	operator sockaddr*()
	{
		return (sockaddr*)this;
	}
	operator const sockaddr*() const
	{
		return (sockaddr*)this;
	}
	u_short GetPort_h() const					// returns host byte order
	{
		return ntohs(sin_port);
	}
	ULONG GetAddr_n() const			// _n - returns network byte order
	{
	    #ifdef WIN32
		return *(ULONG*)&sin_addr;
	    #else
		return sin_addr.s_addr;
	    #endif
	}
	ULONG GetAddr_h() const			// _h - returns host byte order
	{
	    #ifdef WIN32
		return ntohl(*(ULONG*)&sin_addr);
	    #else
		return ntohl(sin_addr.s_addr);
	    #endif
	}
	void SetPort_h(USHORT ushSocketPort_h)	// _h - host byte order
	{
		sin_port = htons((USHORT)ushSocketPort_h);
	}
	void SetAddr_h(ULONG ulAddr_h)			// _h - host byte order
	{
	    #ifdef WIN32
		*(ULONG*)&sin_addr = htonl(ulAddr_h);
	    #else
		sin_addr.s_addr = htonl(ulAddr_h);
	    #endif
	}
	void SetAddr_n(ULONG ulAddr_n)			// _n - network byte order
	{
	    #ifdef WIN32
		*(ULONG*)&sin_addr = ulAddr_n;
	    #else
		sin_addr.s_addr = ulAddr_n;
	    #endif
	}
	unsigned short GetLastAddrassByte()
	{
		return GetAddressByte(4);
	}
	unsigned short GetAddressByte( int nByte )
	{
		unsigned long retVal = 0;
		switch(nByte) {
		case 1:{
			#ifdef WIN32
				retVal = sin_addr.S_un.S_un_b.s_b1;
			#else
				retVal = sin_addr.s_addr << 24;
				retVal = retVal >> 24;
			#endif
		}break;
		case 2:{
			#ifdef WIN32
				retVal = sin_addr.S_un.S_un_b.s_b2;
			#else
				retVal = sin_addr.s_addr << 16;
				retVal = retVal >> 24;
			#endif
		}break;
		case 3:{
			#ifdef WIN32
				retVal = sin_addr.S_un.S_un_b.s_b3;
			#else
				retVal = sin_addr.s_addr << 8;
				retVal = retVal >> 24;
			#endif
		}break;
		case 4:{
			#ifdef WIN32
				retVal = sin_addr.S_un.S_un_b.s_b4;
			#else
				retVal = sin_addr.s_addr;
				retVal = retVal >> 24;
			#endif
		}break;
		default:
			retVal = 0;
		}
		return (unsigned short)(retVal);
	}
};


///////////////////////////////////////////////////////////////////////////////
// CSocketObj
class b_DLL CSocketObj : virtual public CBaseObj, public Transport_x::CBaseSocket
{
////////// Attributes:
public:
	CStreamRetarder		m_Retarder;

protected:

	CWaitObj			m_woClose;
	CMultipleWait		m_mwClose;
	CCriticalSectionObj	m_csLockWrite;

	DWORD				m_dwMaxPieceSize;		// If sending or receiving buffer is less
												// then m_dwMaxPieceSize, it is transferred
												// by pieces of m_dwMaxPieceSize.
	UINT64				m_u64CounterBytesRD;
	UINT64				m_u64CounterBytesWR;
	BOOL				m_bEnableCounterBytes;

////////// Construction:
public:
	CSocketObj();
	virtual ~CSocketObj();

////////// Operations:
public:
	bool CreateSocket(int nType, int nProtocol = IPPROTO_IP, int nFamily = AF_INET);
	virtual void CloseSocket();

	virtual bool Bind(IP anoSourceAddress, unsigned short usnoPort);
	bool Bind( UINT unSocketPort_h, const char* lpSocketAddress = NULL,	INT nFamily = AF_INET);
	bool Bind( const sockaddr_in& lpSockAddr );

	bool GetSockOpt(int nLevel, int nOptName, LPVOID lpOptVal, int nOptLen);
	bool SetSockOpt(int nLevel, int nOptName, LPVOID lpOptVal, int nOptLen);

	bool Send(const void* buf, int bufLength);

	bool RecvFrom(sockaddr_in* lpSA, LPVOID const lpBuffer, int dwDataSize, unsigned long* dwBytes = NULL, unsigned long dwWaitTime = INFINITE );
	bool SendTo(sockaddr_in* lpSA, LPVOID lpBuffer, int dwDataSize, unsigned long* dwBytes = NULL, unsigned long dwWaitTime = INFINITE );

	bool ShutDown();

	void InitiateSocketStop();
	virtual void AddCloseEvent(const CMultipleWait& mwCloseEvents);
	CMultipleWait GetCloseEvents();

	bool EnableReuseAddress();

public:

	bool IsValidSocket() const;

	operator SOCKET() const;

public:
	static BOOL StringToAddress(const char* lpHost, ULONG& ulAddr_n);		// lpHost - dotted address or comp name
	static BOOL GetHostInterfaceAddresses(const char* lpHost, CStringArray& arrAddr); // lpHost - dotted address or comp name
	static ULONG inet_addr(const char* lpAddr);							// returns address in network byte order
	static CString GetStringAddr(const sockaddr_in& saIn);
	static CString GetStringAddr(const struct in_addr* iaAddress);
	static CString GetStringAddr(ULONG ulAddress_n);
	static CString GetDisplayStringAddr(const sockaddr_in& saIn);
	static CString GetDisplayStringAddr(const struct in_addr* iaAddress);
	static CString GetDisplayStringAddr(ULONG ulAddress_n);
	static CString GetHostName(const sockaddr_in& saIn);
	static CString AddrToString(const sockaddr_in& saIn);
	static CString GetLocalComputerName();

	
};

//////////////////////////////////////////////////////////////////////////
inline CString CSocketObj::GetStringAddr(const sockaddr_in& saIn)
{
	return GetStringAddr(&saIn.sin_addr);
}
inline CString CSocketObj::GetStringAddr(ULONG ulAddress_n)
{
    #ifdef WIN32
	return GetStringAddr((const struct in_addr*) &ulAddress_n);
    #else
	struct in_addr addr1;
	addr1.s_addr = (int32_t)(ulAddress_n);
	return GetStringAddr(&addr1);
    #endif
}
inline CString CSocketObj::GetDisplayStringAddr(const sockaddr_in& saIn)
{
	return GetDisplayStringAddr(&saIn.sin_addr);
}
inline CString CSocketObj::GetDisplayStringAddr(ULONG ulAddress_n)
{
    #ifdef WIN32
	return GetDisplayStringAddr((const struct in_addr*) &ulAddress_n);
    #else
	struct in_addr addr1;
	addr1.s_addr = (int32_t)(ulAddress_n);
	return GetDisplayStringAddr(&addr1);
    #endif
}
inline void CSocketObj::AddCloseEvent(const CMultipleWait& mwCloseEvents)
{
	m_mwClose.FindAdd(mwCloseEvents);
}
inline CMultipleWait CSocketObj::GetCloseEvents()
{
	return m_mwClose;
}
inline void CSocketObj::InitiateSocketStop()
{
	m_woClose.SetEvent();
}
inline bool CSocketObj::IsValidSocket() const
{
	return m_Socket!=INVALID_SOCKET;
}
inline CSocketObj::operator SOCKET() const
{
	return m_Socket;
}
}//! namespace b_set

#endif //! __SocketObj_h
