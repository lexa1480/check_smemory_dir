#ifndef CMULTIWAIT_H
#define CMULTIWAIT_H

#include "WaitObj.h"

#ifndef MAXIMUM_WAIT_OBJECTS
	#define MAXIMUM_WAIT_OBJECTS 64
#endif


namespace b_set
{

class CMultipleWait //: private CEventBase
{
public:
	CMultipleWait();
	CMultipleWait( CWaitObj* hWaitObject );
	CMultipleWait( CMultipleWait& mw );

	virtual ~CMultipleWait();

	int Add( CWaitObj* hWaitObject );
	int FindAdd( CWaitObj* hWaitObject );

	void Add( const CMultipleWait& mwAdditional );
	void FindAdd( const CMultipleWait& mwAdditional );

	int Remove( CWaitObj* hWaitObject );
	void Remove( CMultipleWait& mwRemove );
	void RemoveSignaled();
	void RemoveAll();

	unsigned int GetCount() const;
	int Find( CWaitObj* hWaitObject );

	bool Set ( unsigned int dwIndex );
	bool Reset( unsigned int dwIndex );

	bool ResetAll();

	EVENT_HANDLE operator[] ( unsigned int dwIndex );
//!	virtual CMultipleWait& operator = ( const CMultipleWait& mw );

	#ifndef WIN32
		unsigned int CheckEvents( bool bAllSignaled );
		bool HaveSignaledEvents();
		unsigned long GetTickCount();
	#endif //! WIN32

	//virtual bool SetEvent();
	//virtual bool ResetEvent();

	virtual bool IsSet( unsigned long msecs = 0L );
	virtual unsigned int WaitMultipleObj( unsigned long msecs );
	virtual unsigned int WaitMultipleObj( bool bWaitAll , unsigned long );

private:
	EVENT_HANDLE 		m_lpHandles[MAXIMUM_WAIT_OBJECTS];
	CEventBase*		m_WaitObjArray[MAXIMUM_WAIT_OBJECTS];
	unsigned long 		m_dwCount;
	mutable OsMng::CMutex 	m_mtxArray;
};

///////////////////////////////////////////////////////////////////
//inlines
inline unsigned int CMultipleWait::WaitMultipleObj( unsigned long msecs )
{
	return WaitMultipleObj(false,msecs);
}
/*
inline bool CMultipleWait::SetEvent()
{
	return CEventBase::SetEvent();
}
inline bool CMultipleWait::ResetEvent()
{
	#ifdef WIN32
		return CEventBase::ResetEvent();
	#else
		return ( HaveSignaledEvents() ? true : CEventBase::ResetEvent() );
	#endif //! WIN32
}
*/
inline unsigned int CMultipleWait::GetCount() const
{
	#ifdef WIN32
		return m_dwCount;
	#else
		m_mtxArray.lock();
		unsigned int nRetVal = m_dwCount;
		m_mtxArray.unlock();
		return nRetVal;
	#endif //! WIN32
}

/*
inline CMultipleWait& CMultipleWait::operator=( const CMultipleWait& mw )
{
	RemoveAll();
	Add(mw);
	return *this;
}*/

} //! namespace b_set

#endif //! CMULTIWAIT_H

