/******************************************************************************
 File     : ThreadManager.h
 Author   : SDY
 Date     : 24.01.2002
 Comments : "Thread.txt"
 
CThreadManager g_mngThreadManager: global facility for thread control.

 
******************************************************************************/

#ifndef __ThreadManager_h
#define __ThreadManager_h

#include "../BaseObj/Subscription.h"
#include "WaitObj.h"


namespace b_set
{

class CThreadEx;

///////////////////////////////////////////////////////////////////////////////
// CThreadManager
class b_DLL CThreadManager
{
//////////
public:
protected:
	CSubscriberArray<CThreadEx> 
				m_arrThreads;
	CWaitObj	m_woStopApp;
	DWORD		m_dwNextThreadIndexId;

//////////
public:
	CThreadManager();
	virtual ~CThreadManager();

//////////
public:
	void Open();
	void Close();
	void InitiateStopApp();
	BOOL StopAllThreads(DWORD dwMaxWaitTime_ms = 10000);
	CWaitObj* GetStopEvent();
	bool Sleep(DWORD dwMillisectonds);
	BOOL Add(CThreadEx* pObj);
	BOOL Remove(CThreadEx* pObj);
	DWORD GetThreadIndexId() const;
	DWORD GetThreadIndexId_Increment();
	void Lock();
	void Unlock();
	int GetThreadsCount() const;
	bool IsOpen();
	CThreadEx* GetAt(int nIdx);

protected:

};

///////////////////////////////
// CThreadManager inlines
inline int CThreadManager::GetThreadsCount() const
{
	return m_arrThreads.GetSize();
}
inline CThreadEx* CThreadManager::GetAt(int nIdx)
{
	return m_arrThreads.GetAt(nIdx);
}
inline void CThreadManager::Lock()
{
	m_arrThreads.Lock();
}
inline void CThreadManager::Unlock()
{
	m_arrThreads.Unlock();
}
inline DWORD CThreadManager::GetThreadIndexId() const
{
	return m_dwNextThreadIndexId;
}
inline DWORD CThreadManager::GetThreadIndexId_Increment()
{
	DWORD dwRes = 0;
	Lock();
	dwRes = m_dwNextThreadIndexId;
	m_dwNextThreadIndexId++;
	Unlock();
	return dwRes;
}
inline void CThreadManager::Open()
{
	m_arrThreads.Open();
}
inline bool CThreadManager::IsOpen()
{
	return m_arrThreads.IsOpen() ? true : false;
}
inline CWaitObj* CThreadManager::GetStopEvent() 
{
	return &m_woStopApp;
}
inline bool CThreadManager::Sleep(DWORD dwMilliseconds)
{
	return m_woStopApp.WaitSingleObj(dwMilliseconds);
}
inline void CThreadManager::InitiateStopApp()
{
	m_woStopApp.SetEvent();
}
inline BOOL CThreadManager::Remove(CThreadEx* pObj)
{
	return m_arrThreads.Remove(pObj);
}


extern CThreadManager b_DLL g_mngThreadManager;

}//! namespace b_set

#endif //! __ThreadManager_h
