#ifndef CWAITOBJ_H
#define CWAITOBJ_H

#include <set>
#include "EventBase.h"
#include <OSIndependent/GenericMutex.h>

namespace b_set
{

class CWaitObj : public CEventBase
{
public:
	CWaitObj();
	CWaitObj( bool bManualReset, bool bInitialState );
	virtual ~CWaitObj();

	#ifndef WIN32
		virtual bool SetEvent();
		virtual bool ResetEvent();
		virtual bool PulseEvent();
		virtual bool WakeAll();

//		virtual void Unlink();
		virtual void Subscribe( CEventBase* evSubscriber );
		virtual void Unsubscribe( CEventBase* evSubscriber );

		protected:
			OsMng::CMutex	m_mtxArray;
			std::set<CEventBase*> m_SubscriberArr;
	#endif //! WIN32
};

//!////////////////////////////////////////////////////////////////////////
//! inlines
#ifndef WIN32
	inline bool CWaitObj::SetEvent()
	{
		bool bRetVal = CEventBase::SetEvent();
		m_mtxArray.lock();
		std::set<CEventBase*>::iterator it = m_SubscriberArr.begin();
		while ( it != m_SubscriberArr.end() ) {
			(*it)->SetEvent();
			++it;
		}
		m_mtxArray.unlock();
		return bRetVal;
	}
	inline bool CWaitObj::ResetEvent()
	{
		return CEventBase::ResetEvent();
	}
	inline bool CWaitObj::PulseEvent()
	{
		bool bRetVal = CEventBase::PulseEvent();
		m_mtxArray.lock();
		std::set<CEventBase*>::iterator it = m_SubscriberArr.begin();
		while ( it != m_SubscriberArr.end() ) {
			(*it)->PulseEvent();
			++it;
		}
		m_mtxArray.unlock();
		return bRetVal;
	}
	inline bool CWaitObj::WakeAll()
	{
		bool bRetVal = CEventBase::WakeAll();
		m_mtxArray.lock();
		std::set<CEventBase*>::iterator it = m_SubscriberArr.begin();
		while ( it != m_SubscriberArr.end() ) {
			(*it)->WakeAll();
			++it;
		}
		m_mtxArray.unlock();
		return bRetVal;
	}
	inline void CWaitObj::Subscribe( CEventBase* evSubscriber )
	{
		m_mtxArray.lock();
		m_SubscriberArr.insert(evSubscriber);
		m_mtxArray.unlock();
		if (IsSignaled())
			evSubscriber->SetEvent();
	}
	inline void CWaitObj::Unsubscribe( CEventBase* evSubscriber )
	{
		m_mtxArray.lock();
		m_SubscriberArr.erase(evSubscriber);
		m_mtxArray.unlock();
	}
#endif //! WIN32
} //! namespace b_set

#endif //! CWAITOBJ_H
