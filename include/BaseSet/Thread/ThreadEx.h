/******************************************************************************
 File     : ThreadEx.h
 Author   : SDY
 Date     : 24.01.2002
 Comments : "Thread.txt"
 
******************************************************************************/

#ifndef __ThreadEx_h
#define __ThreadEx_h

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT assert
#endif

#ifdef WIN32
	#include <process.h>
#else
	#include <pthread.h>
	#include <sys/resource.h>
	#include <asm/unistd.h>
#endif //! WIN32

#include "Wait.h"
#include "../BaseObj/BaseObject.h"
#include "../Timer/TickCounter.h"

namespace b_set
{
	#ifdef WIN32
		typedef HANDLE THREAD_HANDLE;
	#else
		typedef pthread_t THREAD_HANDLE;
	#endif //! WIN32

	#define TIMEOUT_TERMINATE_THREAD_ms		60000

	enum Priority {
		IdlePriority,

		LowestPriority,
		LowPriority,
		NormalPriority,
		HighPriority,
		HighestPriority,
		TimeCriticalPriority,

		InheritPriority
	};

///////////////////////////////////////////////////////////////////////////////
// CThreadEx
class b_DLL CThreadEx : virtual public CBaseObj
{
//////////
public:
protected:
	THREAD_HANDLE		m_hThread;
	Priority			m_dwPriority;
    DWORD			m_dwThreadID;
    DWORD			m_dwThreadIndexId; // sets by g_mngThreadManager - through counter threads
    DWORD			m_dwBegTime_ticks;
    DWORD			m_dwLastActivationTime_ticks;
	CWaitObj		m_woCloseThread;
	CWaitObj		m_woThreadStarted;
	CWaitObj 		m_woThreadDone;
	// m_mwCloseEvents: to get an event use GetCloseEvents();
	CMultipleWait		m_mwCloseEvents;
	CCriticalSectionObj 	m_lockThreadHandle;

private:

//////////
public:
	CThreadEx();
	virtual ~CThreadEx();

//////////
public:
	CMultipleWait& GetCloseEvents();
	void SetThreadName(LPCTSTR lpThreadName);
	CString GetThreadName();
	THREAD_HANDLE GetThreadHandle() const;
    DWORD GetThreadID() const;
	BOOL IsActive();
	BOOL IsThreadOpen() const;
	virtual BOOL StartThread();
	BOOL StartThreadWait(DWORD dwWaitTime = INFINITE);
	BOOL InitiateStopThread();
	BOOL StopThread(DWORD dwTerminateThreadWaitTime_ms = TIMEOUT_TERMINATE_THREAD_ms);
	BOOL UnLinkThread(); // let the current work independently of the object, ie in the destructor does not stop the thread
	BOOL SetPriority ( Priority dwPriority ) ;
	int GetPriority();
	
	void AddCloseEvent( CWaitObj* wCloseEvents); ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	void AddCloseEvent( const CMultipleWait& mwCloseEvents );
	virtual BOOL WaitForCloseEvent(DWORD dwMilliseconds);
	BOOL WaitForThreadStarted(DWORD dwMilliseconds = INFINITE);
	BOOL WaitForThreadStopped(DWORD dwMilliseconds = INFINITE);
	void UpdateLastActivationTime(); // renew m_dwLastActivationTime_ticks
    DWORD GetBegTime_ticks() const;
    DWORD GetLastActivationTime_ticks() const;
    DWORD GetThreadIndexId() const; // transparent, non-recurring thread number

	static void Sleep( unsigned long dwMillisec );


protected:
	// OnStopThread: to stop the thread of runs before running the command Stop
	virtual BOOL OnStopThread() 
		{return TRUE;}
	// OnTerminateThread: runs in stopping the flow after a command Terminate
	virtual BOOL OnTerminateThread()
		{return TRUE;}
	virtual DWORD thrd_Proc()
		{ASSERT(FALSE); return 0;}
	void LockHandle()
		{m_lockThreadHandle.Enter();}
	void UnlockHandle()
		{m_lockThreadHandle.Leave();}
private:
	#ifdef WIN32
		static DWORD __stdcall thrd_ThreadStartRoutine( CThreadEx* pThread );
	#else
		static void* thrd_ThreadStartRoutine( void* pThread );
	#endif //! WIN32
};

///////////////////////////////
// CThreadEx inlines
inline BOOL CThreadEx::IsThreadOpen() const
{
	#ifdef WIN32
		return ( m_hThread != NULL );
	#else
		return ( m_hThread != 0 );
	#endif //! WIN32
}
inline DWORD CThreadEx::GetThreadIndexId() const
{
	return m_dwThreadIndexId;
}
inline DWORD CThreadEx::GetLastActivationTime_ticks() const
{
	return m_dwLastActivationTime_ticks;
}
inline DWORD CThreadEx::GetBegTime_ticks() const
{
	return m_dwBegTime_ticks;
}
inline void CThreadEx::UpdateLastActivationTime()
{
	m_dwLastActivationTime_ticks = CTickCounter().GetTickCount();
}
inline void CThreadEx::SetThreadName(LPCTSTR lpThreadName)
{
	SetObjectName(lpThreadName);
}
inline DWORD CThreadEx::GetThreadID() const
{
	return m_dwThreadID;
}
inline THREAD_HANDLE CThreadEx::GetThreadHandle() const
{
	return m_hThread;
}
inline BOOL CThreadEx::InitiateStopThread()
{
	return m_woCloseThread.SetEvent();
}
inline void CThreadEx::AddCloseEvent( const CMultipleWait& mwCloseEvents )
{
	m_mwCloseEvents.FindAdd(mwCloseEvents);
}
inline void CThreadEx::AddCloseEvent( CWaitObj* wCloseEvents )
{
	m_mwCloseEvents.FindAdd(wCloseEvents);
}
inline BOOL CThreadEx::WaitForCloseEvent(DWORD dwMilliseconds)
{
	UpdateLastActivationTime();

	#ifdef WIN32
		return (m_mwCloseEvents.WaitMultipleObj(false, dwMilliseconds) != WAIT_TIMEOUT);
	#else
		return (m_mwCloseEvents.WaitMultipleObj(false, dwMilliseconds) != ETIMEDOUT);
	#endif //! WIN32
}
inline BOOL CThreadEx::WaitForThreadStarted(DWORD dwMilliseconds)
{
	CMultipleWait mwThreadStarted;
	mwThreadStarted.Add(&m_woThreadStarted);
	mwThreadStarted.Add(m_mwCloseEvents);

	#ifdef WIN32
		return (mwThreadStarted.WaitMultipleObj(false, dwMilliseconds) == WAIT_OBJECT_0);
	#else
		return (mwThreadStarted.WaitMultipleObj(false, dwMilliseconds) == 0);
	#endif //! WIN32
}
inline BOOL CThreadEx::WaitForThreadStopped( DWORD dwMilliseconds )
{
	BOOL bRes = TRUE;
	#ifdef WIN32
		if ( m_hThread )
		{
			DWORD dwExitCode = 0;
			if( GetExitCodeThread(m_hThread, &dwExitCode) &&
			dwExitCode == STILL_ACTIVE )
			{
				if( ::WaitForSingleObject( m_hThread, dwMilliseconds ) != WAIT_OBJECT_0 )
				{
					bRes = FALSE;
				}
			}
		}
	#else
		if ( m_hThread ) {
			if ( m_woThreadStarted.IsSet(dwMilliseconds) )
				bRes = FALSE;
		}
	#endif //! WIN32
	return bRes;
}
inline BOOL CThreadEx::UnLinkThread()
{
	BOOL bRes = TRUE;
	
	#ifdef WIN32
		LockHandle();
		if(m_hThread)
		{
			bRes = CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		UnlockHandle();
	#else
		LockHandle();
		if ( m_hThread ){
			bRes = !pthread_detach(m_hThread);
			m_hThread = 0;
		}
		UnlockHandle();
	#endif //! WIN32
	return bRes;
}
inline BOOL CThreadEx::IsActive() 
{
	#ifdef WIN32
		DWORD dwExitCode = 0;
		return (
			m_hThread != NULL
			&& GetExitCodeThread(m_hThread, &dwExitCode) 
			&& dwExitCode == STILL_ACTIVE
			);
	#else
		return  (  m_hThread && m_woThreadStarted.IsSet() );
	#endif //! WIN32
}
inline void CThreadEx::Sleep(unsigned long dwMillisec)
{
#ifdef WIN32
	::Sleep(dwMillisec);
#else
	::usleep(dwMillisec*1000);
#endif
}

inline int CThreadEx::GetPriority()
{
#ifdef WIN32
	return ::GetThreadPriority( GetThreadHandle() );
#else
	struct sched_param param;
	int policy = SCHED_RR;
	if ( pthread_getschedparam( GetThreadHandle(), &policy, &param) == 0 )
		return  (param.sched_priority);
	else
		return 0xffffffff;
#endif //! WIN32
}
/*
inline bool CThreadEx::GetUsage( rusage* usage )
{
	if ( ::getrusage( RUSAGE_SELF, usage ) == 0 )
		return true;
	else
		return false;
}*/
#ifndef WINNT40
	///////////////////////////////////////////////////////////////////////////////
	// CThreadLink
	class b_DLL CThreadLink : public CThreadEx
	{
	//////////
	public:
	protected:

	//////////
	public:
		CThreadLink()
		{
		}
		~CThreadLink()
		{
			UnLinkThread();
		}

	//////////
	public:
		BOOL LinkThreadCurrent();
		BOOL LinkThread(DWORD dwThreadID, DWORD dwDesiredAccess); // API: OpenThread
	private:
		DWORD thrd_Proc(){return 0;}
		BOOL StartThread(){return FALSE;}
		BOOL StartThreadWait(DWORD /*dwWaitTime*/){return FALSE;}
		BOOL InitiateStopThread(){return FALSE;}
		BOOL StopThread(DWORD /*dwTerminateThreadWaitTime_ms*/){return FALSE;}
	};
#endif //! WINNT40

}//! namespace b_set

#endif //! __ThreadEx_h
