// BaseSocket.h: interface for the CBaseSocket class.
//
//////////////////////////////////////////////////////////////////////
/*!	\file
* \brief ������� �����.
*
* \author	Strokov K
*/

/*!	\defgroup Socket �����.
*	\brief ������ ��� ������ � ��������.
*
*	 ��������� ����������� �������� � ��������. �������� ��� Windows,Unix. 
*	@{
*/


#if !defined(AFX_BASESOCKET_H__5312B298_460B_4205_BC7A_C7E616C61C78__INCLUDED_)
#define AFX_BASESOCKET_H__5312B298_460B_4205_BC7A_C7E616C61C78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _ND_TRANSPORT
#define _BASE_TRANSPORT_DECL
#else
#include "..\ExportMacros.h"
#endif

#include <assert.h>

#ifdef WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#define socklen_t int

#ifndef SIO_RCVALL
#define SIO_RCVALL  0x98000001  // for sniff socket
#endif

#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../BaseTypes_x.h"
#define  SOCKET unsigned int

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET (SOCKET)(~0)
#endif
#ifndef SOCKET_ERROR
	#define SOCKET_ERROR (-1)

#endif
#define ioctlsocket ioctl
#define SIO_RCVALL  0x98000001  ///< for sniff socket 
#endif

typedef unsigned long IP; ///< ��� IP

#define ARM_DEFAULT_UDP_PORT	htons(20000) ///< ������������� UDP ���� �� �����
#define ARM_DEFAULT_TCP_PORT	htons(20004) ///< ������������� TCP ����

//#ifndef STR_ADDR_MC_LAZ
//	#define STR_ADDR_MC_LAZ   "234.0.1.75" ///< ��� ��������
//#endif

namespace Transport_x
{
	/*!	\class CBaseSocket
	*	\brief ������� ����� ������. ��������� ����������� �������� � ��������.
	*	\ingroup Socket
	*
	* �������� ��� Windows,Unix
	*
    *	������: 
	*	- ����� ������, 
	*	- ��������� ������� ��� ���������, 
	*	- �������� �� �� "�����" ������� (������ �� ������������),
	*/
	class _BASE_TRANSPORT_DECL CBaseSocket  
	{
		/*!	\name �����������/�����������
		//@{
		*/
	public:
		/*!	\brief �������� ���������..*/
		CBaseSocket();
		/*! \brief ����������. ���������� �������� ������.*/
		virtual ~CBaseSocket();
		//@}

		/*! \enum SocketDirection
		��� ���������� ������ �����
		*/
		enum SocketDirection
		{
			ssReceive,/*!< ���������� ������ */
			ssSend,/*!< �������� ������ */
			ssBoth,/*!< � ��� � ������ */
			ssAll
		};

		/*!	\name �������� �������� �� ������ � ��������. ( main socket fun�tion)
		*	@{
		*/

		/// \name �������� � �������� ������
		virtual bool Create(int iProtocol =IPPROTO_UDP); ///< �������� ������
        virtual bool IsCreate();///< check is socket create


		virtual bool Close(); ///< �������� ������
		virtual bool Shutdown(SocketDirection eHow);///< �������� ������ � �������, ��� ����� ��������� ������� ��� ���������


		/// \name �������� ������ (bind)
		virtual bool Bind(IP anoSourceAddress,unsigned short usnoPort); ///< ���������� ������ � �������. ���� ��������� � ������� �������
		virtual bool Bind(const char* lpSourceAddress,unsigned short iPort); ///< ���������� ������ � ������� 

		/// \name ��������� ������ (receive message)
		virtual int Receive(void *pbuf, int iBufLen, sockaddr* saFrom=NULL, socklen_t* pnFromSize=NULL);///< ��������� ������

		/// \name ������� ������ (Send dat�)
		virtual bool Send(const void* buf, int bufLength);///< ������� ������. ��� ������, ����� ��� ����������� ���������� (������ connect ��� ������� ������)
		virtual bool SendTo(IP anoDestinationAddress, unsigned short usnoDestinationPort,const void* buf,int bufLength);///< ������� ������. ��� ��������� � ������� �������
		virtual bool SendTo(const char* lpDestinationAddress, unsigned short usnoDestinationPort, const void* buf,int bufLength);///< ������� ������.
		virtual bool SendTo(const DWORD* pdwIPs, const size_t iCount, unsigned short usnoDestinationPort, const void* buf, int bufLength);///< ������� ������ �� ������� ����������� ������, ������� ��������. ��� ��������� � ������� �������
		virtual int SendToAll(const DWORD* pdwIPs, const size_t iCount, unsigned short usnoDestinationPort, const void* buf, int bufLength);///< ������� ������ �� ���� �������, ������� ��������. ��� ��������� � ������� �������

		//@}

		
		/*!	\name ��������� � ��������� ���������� ������.
		*	@{
		*/
		/// \name ��������� ���������� (Ttl function)
		virtual void SetTTL(unsigned char ucTTL); ///< ��������� ������� ����� (����� �����)
		virtual unsigned char GetTTL()const; ///< ������ ������� �����
		virtual bool SetReuseAddr();///< ���������, ����� ���� ��������� ��� ������������ ������ �����
		virtual void SetReceivebuffer(int iSize);///< Setting internal buffer for this socket  (for example if receive socket in high speed )

		virtual SOCKET GetHandle() const;///< ��������� ������ ������ (Handle function)
		virtual void SetRaw(bool bRaw);///< ���������, ��� ��� ����� raw ����� ()
		virtual bool isRaw() const; // �������� �������� �� ����� "�����" 
		//@}


		//static function
		//select socket
		/*!	\name ����������� �������. ������� �������.
		*	@{
		*/

		static SOCKET FindMaxSock(const CBaseSocket*const*pvectBsk, size_t iSize);///< ��������������� ������� ��� �������. ���������� ������ � ��� �������
		static int FindReceiveSocket(CBaseSocket**pvectBsk, fd_set* fsSet, size_t iSize);///< ��������������� ������� ��� �������. ���������� ����������� ������
		static void FillSelectTimeout(timeval* Time);///< ��������������� ������� ��� �������. ��������� �������� .
		static void InitFdSetRead(const CBaseSocket*const* pvectBsk, fd_set* FdRead, size_t iSize);///< ��������������� ������� ��� �������
		static int Select(SOCKET iMaxSock,fd_set* readSckSet,fd_set* writeSckSet=NULL,fd_set* except=NULL,const timeval* time=NULL);///< ������� ������������ ��������� ������ �������. � ������ ������������� ������ ��������� ������ (time).
		static int Select(SOCKET iMaxSock, CBaseSocket** readSckSet, size_t iRead,CBaseSocket** writeSckSet=NULL, size_t iWrite=0, CBaseSocket** except=NULL, size_t iExcept=0, const timeval* time=NULL);///< ������� ������������ ��������� ������ �������. � ������ ������������� ������ ��������� ������ (time).
		
		//������� ��� ���������� ������
		static int SelectRead(CBaseSocket* pReadSckSet, const timeval* pTime=NULL);///< ������� ������������ ��������� ��������� ������. � ������ ������������� ������ ��������� ������ (time).
	        static int SelectWrite(CBaseSocket* pWriteSckSet, const timeval* pTime=NULL);///< ������� ������������ ��������� �������� ������. � ������ ������������� ������ ��������� ������ (time).
		

		//@}

		/*!	\name ��������� ��������� (Operators, for compare).
		*	@{
		*/

		bool operator==(const CBaseSocket& rSocket)const;///< �������� �� ����������
		bool operator<(const CBaseSocket& rSocket)const;///< �������� ��� ����� ������
		//@}

	protected: // attributes

		bool  m_bRaw;///< �������� �� ����� "�����" 

		IP m_aSourceAddress;///< ��� ����� (Our source address (interface)

		SOCKET m_Socket;///< �����  


	protected:// function

		/*!	\name ��������� ���������� ���������� ������.
		*	@{
		*/

	
		//Set and get source address
		virtual IP GetSourceAddress()const;///< ��������� ������
		virtual void SetSourceAddress(IP aSourceAddress);///< ��������� ������
		virtual void SetSourceAddress(const char* lpSourceAddress);///< ��������� ������
		//@}

	public:
		class _BASE_TRANSPORT_DECL SocketInit
		{
			static unsigned int nCount;
		public:
			SocketInit();
			~SocketInit();

		};		

	public:
		static IP* pIPs;
		static size_t nIPs;

	public:
		static size_t GetLocalInterfacesNumber()
		{
			return nIPs;
		}
		static IP GetLocalInterface(size_t nPos)
		{
			return (nPos < nIPs) ? pIPs[nPos] : 0;
		}
		
		static bool isLocalAddress(IP ipAddr)
		{
				for (unsigned int i=0; i<nIPs;i++)
				{
					if (pIPs[i] ==ipAddr)
						return true;
    			}
				return false;
		}


	};

	namespace
	{
		CBaseSocket::SocketInit socketInit;
	}

	inline void CBaseSocket::FillSelectTimeout(timeval* Time)
	{
		Time->tv_sec=1;
		Time->tv_usec=0;
	}
}

#endif // !defined(AFX_BASESOCKET_H__5312B298_460B_4205_BC7A_C7E616C61C78__INCLUDED_)

