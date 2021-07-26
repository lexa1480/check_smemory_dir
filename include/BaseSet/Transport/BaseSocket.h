// BaseSocket.h: interface for the CBaseSocket class.
//
//////////////////////////////////////////////////////////////////////
/*!	\file
* \brief Базовый сокет.
*
* \author	Strokov K
*/

/*!	\defgroup Socket Сокет.
*	\brief Классы для работы с сокетами.
*
*	 Выполняет стандартные операции с сокетами. Работают под Windows,Unix. 
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

typedef unsigned long IP; ///< Тип IP

#define ARM_DEFAULT_UDP_PORT	htons(20000) ///< Умолчательный UDP порт на прием
#define ARM_DEFAULT_TCP_PORT	htons(20004) ///< Умолчательный TCP порт

//#ifndef STR_ADDR_MC_LAZ
//	#define STR_ADDR_MC_LAZ   "234.0.1.75" ///< АРМ инженера
//#endif

namespace Transport_x
{
	/*!	\class CBaseSocket
	*	\brief Базовый класс сокета. Выполняет стандартные операции с сокетами.
	*	\ingroup Socket
	*
	* Работает под Windows,Unix
	*
    *	Хранит: 
	*	- хендл сокета, 
	*	- интерфейс посылки или получения, 
	*	- является ли он "сырым" сокетом (сейчас не используется),
	*/
	class _BASE_TRANSPORT_DECL CBaseSocket  
	{
		/*!	\name Конструктор/Декструктор
		//@{
		*/
	public:
		/*!	\brief Обнуляет параметры..*/
		CBaseSocket();
		/*! \brief Деструктор. Производит закрытие сокета.*/
		virtual ~CBaseSocket();
		//@}

		/*! \enum SocketDirection
		Чем занимается данный сокет
		*/
		enum SocketDirection
		{
			ssReceive,/*!< Получением данных */
			ssSend,/*!< Посылкой данных */
			ssBoth,/*!< И тем и другим */
			ssAll
		};

		/*!	\name Основные операции по работе с сокетами. ( main socket funсtion)
		*	@{
		*/

		/// \name Создание и закрытие сокета
		virtual bool Create(int iProtocol =IPPROTO_UDP); ///< Создание сокета
        virtual bool IsCreate();///< check is socket create


		virtual bool Close(); ///< Закрытие сокета
		virtual bool Shutdown(SocketDirection eHow);///< Закрытие сокета с выбором, что будет запрещено посылка или получение


		/// \name Привязка сокета (bind)
		virtual bool Bind(IP anoSourceAddress,unsigned short usnoPort); ///< Связывание сокета с адресом. Всем параметры в сетевом порядке
		virtual bool Bind(const char* lpSourceAddress,unsigned short iPort); ///< Связывание сокета с адресом 

		/// \name Получение пакета (receive message)
		virtual int Receive(void *pbuf, int iBufLen, sockaddr* saFrom=NULL, socklen_t* pnFromSize=NULL);///< Получение пакета

		/// \name Посылка пакета (Send datа)
		virtual bool Send(const void* buf, int bufLength);///< Посылка пакета. Для случая, когда был произведено соединение (вызван connect для данного сокета)
		virtual bool SendTo(IP anoDestinationAddress, unsigned short usnoDestinationPort,const void* buf,int bufLength);///< Посылка пакета. Все параметры в сетевом порядке
		virtual bool SendTo(const char* lpDestinationAddress, unsigned short usnoDestinationPort, const void* buf,int bufLength);///< Посылка пакета.
		virtual bool SendTo(const DWORD* pdwIPs, const size_t iCount, unsigned short usnoDestinationPort, const void* buf, int bufLength);///< Посылка пакета по первому доступеному адресу, который доступен. Все параметры в сетевом порядке
		virtual int SendToAll(const DWORD* pdwIPs, const size_t iCount, unsigned short usnoDestinationPort, const void* buf, int bufLength);///< Посылка пакета по всем адресам, которые доступны. Все параметры в сетевом порядке

		//@}

		
		/*!	\name Установка и получение параметров сокета.
		*	@{
		*/
		/// \name Установка параметров (Ttl function)
		virtual void SetTTL(unsigned char ucTTL); ///< Установка времени жизни (число хопов)
		virtual unsigned char GetTTL()const; ///< Взятие времени жизни
		virtual bool SetReuseAddr();///< Позволяет, чтобы этот интерфейс мог использовать другой сокет
		virtual void SetReceivebuffer(int iSize);///< Setting internal buffer for this socket  (for example if receive socket in high speed )

		virtual SOCKET GetHandle() const;///< Получения хендла сокета (Handle function)
		virtual void SetRaw(bool bRaw);///< Установка, что это будет raw сокет ()
		virtual bool isRaw() const; // Проверка является ли сокет "сырым" 
		//@}


		//static function
		//select socket
		/*!	\name Статические функции. Функция селекта.
		*	@{
		*/

		static SOCKET FindMaxSock(const CBaseSocket*const*pvectBsk, size_t iSize);///< Вспомогательная функция для селекта. Нахождение сокета с мах хендлом
		static int FindReceiveSocket(CBaseSocket**pvectBsk, fd_set* fsSet, size_t iSize);///< Вспомогательная функция для селекта. Нахождение получившего сокета
		static void FillSelectTimeout(timeval* Time);///< Вспомогательная функция для селекта. Установка таймаута .
		static void InitFdSetRead(const CBaseSocket*const* pvectBsk, fd_set* FdRead, size_t iSize);///< Вспомогательная функция для селекта
		static int Select(SOCKET iMaxSock,fd_set* readSckSet,fd_set* writeSckSet=NULL,fd_set* except=NULL,const timeval* time=NULL);///< Функция определяющая состояния набора сокетов. В случае необходимости ждущее требуемый период (time).
		static int Select(SOCKET iMaxSock, CBaseSocket** readSckSet, size_t iRead,CBaseSocket** writeSckSet=NULL, size_t iWrite=0, CBaseSocket** except=NULL, size_t iExcept=0, const timeval* time=NULL);///< Функция определяющая состояния набора сокетов. В случае необходимости ждущее требуемый период (time).
		
		//функция для одиночного сокета
		static int SelectRead(CBaseSocket* pReadSckSet, const timeval* pTime=NULL);///< Функция определяющая состояния читающего сокета. В случае необходимости ждущее требуемый период (time).
	        static int SelectWrite(CBaseSocket* pWriteSckSet, const timeval* pTime=NULL);///< Функция определяющая состояния пишущего сокета. В случае необходимости ждущее требуемый период (time).
		

		//@}

		/*!	\name Операторы сравнения (Operators, for compare).
		*	@{
		*/

		bool operator==(const CBaseSocket& rSocket)const;///< Проверка на совпадение
		bool operator<(const CBaseSocket& rSocket)const;///< Проверка чей хендл больше
		//@}

	protected: // attributes

		bool  m_bRaw;///< Является ли сокет "Сырым" 

		IP m_aSourceAddress;///< Наш адрес (Our source address (interface)

		SOCKET m_Socket;///< хендл  


	protected:// function

		/*!	\name Установка внутренних параметров сокета.
		*	@{
		*/

	
		//Set and get source address
		virtual IP GetSourceAddress()const;///< Получение адреса
		virtual void SetSourceAddress(IP aSourceAddress);///< Установка адреса
		virtual void SetSourceAddress(const char* lpSourceAddress);///< Установка адреса
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

