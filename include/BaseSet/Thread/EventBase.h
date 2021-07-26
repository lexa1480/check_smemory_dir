#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <limits.h>

#ifdef WIN32
	#include <winsock2.h>
#else //! WIN32 not define
	#include <pthread.h>
#endif //! WIN32

namespace b_set
{

#ifdef WIN32
	typedef void* EVENT_HANDLE;
#else
	typedef pthread_cond_t* EVENT_HANDLE;

	#define INFINITE 	0xffffffff
	#define WAIT_TIMEOUT	ETIMEDOUT
	#define WAIT_OBJECT_0	0
	#define WAIT_FAILED	0xffffffff
#endif //! WIN32

class CEventBase
{
public:
	
	CEventBase();
	CEventBase( bool bManualReset, bool bInitialState );

	virtual ~CEventBase();
public:
	bool Create( bool bManualReset = true, bool bInitialState = false );
	void Close();

	bool IsValidHandle();

	virtual bool IsSet( unsigned long msecs = 0L );
	virtual bool IsReset();
	virtual bool SetEvent();
	virtual bool PulseEvent();
	virtual bool ResetEvent();
	virtual bool WaitSingleObj( unsigned long msecs = 0xffffffff );

	operator EVENT_HANDLE() const;


	#ifndef WIN32
		virtual bool WakeAll();
		virtual bool IsSignaled();
		virtual void Subscribe(CEventBase*){};
		virtual void Unsubscribe(CEventBase*){};
	#endif

protected:

	EVENT_HANDLE		m_handle;

	#ifndef WIN32
		bool 			m_bManualReset;
		bool 			m_bSignal;
		mutable pthread_mutex_t m_mutex;
	#endif //! WIN32

};

//!////////////////////////////////////////////////////////////////////////
//! inlines
inline bool CEventBase::IsSet( unsigned long msecs /* = 0L  */ )
{
	return WaitSingleObj(msecs);
}
inline CEventBase::operator EVENT_HANDLE() const
{
	#ifdef WIN32
		return m_handle;
	#else
		EVENT_HANDLE handle;
		pthread_mutex_lock(&m_mutex);
		//m_mtx.lock();
		handle = m_handle;
		pthread_mutex_unlock(&m_mutex);
		//m_mtx.unlock();
		return handle;
	#endif //! WIN32
}
inline bool CEventBase::IsValidHandle()
{
	#ifdef WIN32
		return ((m_handle==NULL)?false:true);
	#else
		bool bRetVal = false;
		pthread_mutex_lock(&m_mutex);
		bRetVal = ((m_handle==NULL)?false:true);
		pthread_mutex_unlock(&m_mutex);
		return bRetVal;
	#endif //! WIN32
}


/*
#ifndef WIN32
	class CMutexLocker
	{
		public:
			CMutexLocker( pthread_mutex_t* pMutex );
			~CMutexLocker();
		private:
			pthread_mutex_t* m_pMutex;
	};

	inline CMutexLocker::CMutexLocker( pthread_mutex_t* pMutex )
	:m_pMutex(pMutex)
	{
		if ( pMutex ) {
			pthread_mutex_lock(m_pMutex);
		}
		else {
			printf( "CMutexLocker::CMutexLocker : Invalid mutex !!!\n" );
		}
	}
	inline CMutexLocker::~CMutexLocker()
	{
		if ( m_pMutex )
			pthread_mutex_unlock(m_pMutex);
	}
#endif //! WIN32
*/

} //! namespace b_set

#endif //! EVENTBASE_H
