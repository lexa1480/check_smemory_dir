#ifndef WAIT_H
#define WAIT_H

#include <OSIndependent/GenericMutex.h>

#include "../BaseTypes_x.h"
#include "WaitObj.h"
#include "MultiWait.h"

namespace b_set
{
#ifndef WIN32
	typedef struct _SECURITY_ATTRIBUTES {
		DWORD nLength;
		LPVOID lpSecurityDescriptor;
		BOOL bInheritHandle;
	} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
#endif //! WIN32

	//////////////////////////////////////////////////////////////////////////
	//! CLocker
	template<class TYPE>
	class CLocker
	{
	public:
		CLocker(TYPE& obj)
			:m_obj(obj)
		{
			try
			{
				m_obj.Lock();
			}
			catch (...)
			{
			}
		}
		~CLocker()
		{
			try
			{
				m_obj.Unlock();
			}
			catch (...)
			{
			}
		}
	protected:
		TYPE& m_obj;
	};

	//////////////////////////////////////////////////////////////////////////
	//! CCriticalSectionObj
	class CCriticalSectionObj : public OsMng::CMutex
	{
	public:
		CCriticalSectionObj()
			:CMutex()
		{
		}
		~CCriticalSectionObj() {};

	public:
		void Enter(){lock();};
		void Leave(){unlock();};
		void Lock()
		{
			Enter();
		}
		void Unlock()
		{
			Leave();
		}
	};

#ifdef WIN32

	//////////////////////////////////////////////////////////////////////////
	// COverlapped
	class COverlapped : public OVERLAPPED
	{
	public:
		COverlapped()
			:m_bWaitObjCopy(false)
		{
			memset(this,0,sizeof(COverlapped));
			hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		COverlapped(CWaitObj& waitObj)
		{
			memset(this,0,sizeof(COverlapped));
			*this = waitObj;
		}
		~COverlapped()
		{
			if( hEvent && !m_bWaitObjCopy )
				::CloseHandle(hEvent);
		}

	public:
		operator HANDLE()
		{
			return hEvent;
		}
		void operator=(OVERLAPPED& over)
		{
			CopyFrom(over);
		}
		void operator=(CWaitObj& waitObj)
		{
			if ( this->hEvent )
				::CloseHandle(this->hEvent);
			this->hEvent = (EVENT_HANDLE)waitObj;
			this->m_bWaitObjCopy = true;
		}
		BOOL ResetEvent()
		{
			return ::ResetEvent(hEvent);
		}
		BOOL SetEvent()
		{
			return ::SetEvent(hEvent);
		}
		BOOL IsSet(DWORD dwTimeout_ms = 0) const
		{
			return ::WaitForSingleObject(hEvent, dwTimeout_ms) == WAIT_OBJECT_0;
		}
		BOOL IsReset(DWORD dwTimeout_ms = 0) const
		{
			return ::WaitForSingleObject(hEvent, dwTimeout_ms) == WAIT_TIMEOUT;
		}
	protected:
		void CopyFrom(OVERLAPPED& over)
		{
			HANDLE hTemp = hEvent;
			*this = over;
			over.hEvent = (hTemp) ? hTemp : ::CreateEvent(NULL, TRUE, FALSE, NULL);
		}

	protected:
		bool m_bWaitObjCopy;
	};
#endif //! WIN32

};


#endif //!WAIT_H
