/******************************************************************************
 File     : SocketInitializator.h
 Author   : SDY
 Date     : 15.03.2005
 Comments : 
 
******************************************************************************/

#ifndef __SocketInitializator_h
#define __SocketInitializator_h

#ifndef b_DLL
	#define b_DLL
#endif //! b_DLL

#include "SocketObj.h"

#include "../Array/Array.h"
#include "../Array/ArrayMT.h"
#include "../Thread/ThreadEx.h"
#include "../BaseObj/Subscription.h"

namespace b_set
{
///////////////////////////////////////////////////////////////////////////////
// CAddressListChangeSubscriber
class CAddressListChangeSubscriber : virtual public CBaseObj
{
public:
	virtual void OnAddressListChange(CDwordArray& arrAddr_n) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// CSocketInitializator
// Initializes Winsock. Makes cleanup on application closure.
// g_SocketInitializator initializes Winsock in the constructor. 
// Makes cleanup in destructor.
class b_DLL CSocketInitializator : protected CThreadEx
{
////////// Attributes:
public:
	#ifdef WIN32
		WSADATA		m_wsaData;
		INT		m_nStartupError;
		CString		m_strError;
	#endif //! WIN32

	CDwordArrayMT m_arrLocalAddresses_n;

protected:
	CSubscriberArray<CAddressListChangeSubscriber> m_arrSubscribers;
	CWaitObj m_woClose;

////////// Construction:
public:
	CSocketInitializator();
	virtual ~CSocketInitializator();

////////// Operations:
public:
	BOOL UpdateLocalAddresses();
	void GetLocalAddresses(CDwordArray& arrAddr_n);
	BOOL IsLocalAddress(ULONG ulAddr_n);
	BOOL Start();	// starts hook on local addresses list change 
	void Stop();
	BOOL Subscribe(CAddressListChangeSubscriber* pSubscriber);
	BOOL UnSubscribe(CAddressListChangeSubscriber* pSubscriber);
	void InformSubscribers();
protected:
	void OnAddressListChange();
	DWORD thrd_Proc();
};

///////////////////////////////
// CSocketInitializator inlines
inline BOOL CSocketInitializator::IsLocalAddress(ULONG ulAddr_n) 
{
	return m_arrLocalAddresses_n.Find(ulAddr_n) != -1;
}
inline BOOL CSocketInitializator::Start()
{
	m_woClose.ResetEvent();
	return StartThreadWait();
}

inline void CSocketInitializator::Stop()
{
	m_woClose.SetEvent();
	StopThread();
}

inline BOOL CSocketInitializator::Subscribe(CAddressListChangeSubscriber* pSubscriber)
{
	BOOL bRes = m_arrSubscribers.Add(pSubscriber);
	if(bRes)
	{
		CDwordArray arrAddr_n;
		GetLocalAddresses(arrAddr_n);
		pSubscriber->OnAddressListChange(arrAddr_n);
	}
	return bRes;
}

inline BOOL CSocketInitializator::UnSubscribe(CAddressListChangeSubscriber* pSubscriber)
{
	BOOL bRes = m_arrSubscribers.Remove(pSubscriber);
	return bRes;
}

inline void CSocketInitializator::InformSubscribers()
{
     OnAddressListChange();
};

extern CSocketInitializator b_DLL g_SocketInitializator;

}//! namespace b_set

#endif //! __SocketInitializator_h
