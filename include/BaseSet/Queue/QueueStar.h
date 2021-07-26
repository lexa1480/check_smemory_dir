/******************************************************************************
 File     : QueueStar.h
 Author   : SDY
 Date     : 24.10.2002
 Comments : очередь: один писатель - много читателей
			"QueueStar.txt"
******************************************************************************/

#ifndef __QueueStar_h
#define __QueueStar_h

#include "../Thread/MultiWait.h"
#include "../Thread/EventBase.h"
#include "../Thread/WaitObj.h"
#include "../Array/ArrayMT.h"
#include "../Thread/ThreadEx.h"

#ifndef WIN32
	#include <unistd.h>
#endif


namespace b_set
{

///////////////////////////////////////////////////////////////////////////////
// CQueueItemIndex
class b_DLL CQueueItemIndex
{
////////// Attributes:
public:
#ifndef __ASTRA__
	CDwordArrayMT	m_arrUsage;	// src fills it on SetCurFull; dst removes its signature at SetUsed
	CDwordArrayMT	m_arrProcessing;	// dst adds signature on SetProcessingState, removes on SetUsed
#else
	CPtLongArrayMT	m_arrUsage;	// src fills it on SetCurFull; dst removes its signature at SetUsed
	CPtLongArrayMT	m_arrProcessing;	// dst adds signature on SetProcessingState, removes on SetUsed
#endif
	CWaitObj		m_woFull;		// src sets, dst waits
	CWaitObj		m_woEmpty;		// dst sets this event when this ItemIndex has been used by all subscribers
	int			m_nItemIdx;
	CQueueItemIndex* m_pNext;
protected:
	BOOL			m_bItemHasData;

////////// Construction:
public:
	CQueueItemIndex();
	virtual ~CQueueItemIndex();

////////// Operations:
public:
	int GetIdx();
	CQueueItemIndex* GetNext();
	BOOL IsUsedBy(DWORD dwID);
	BOOL HasData();
protected:

// friends
	friend class CQueueStar;
};

///////////////////////////////////////////////////////////////////////////////
// T_CQueueItemIndex
template<class DATA_TYPE>
class T_CQueueItemIndex : public CQueueItemIndex
{
public:
	typedef DATA_TYPE data_type;
protected:
	DATA_TYPE* m_pData;

public:
	T_CQueueItemIndex()
	{
		m_pData = new DATA_TYPE;
	}
	virtual ~T_CQueueItemIndex()
	{
		if(m_pData)
			delete m_pData;
	}

public:
	DATA_TYPE* GetData()
	{
		return m_pData;
	}
};

///////////////////////////////
// CQueueItemIndex inlines
inline BOOL CQueueItemIndex::HasData()
{
	return m_bItemHasData;
}

inline BOOL CQueueItemIndex::IsUsedBy(DWORD dwID)
{
	return (m_arrUsage.Find(dwID) == ARRAY_ERROR);
}
inline CQueueItemIndex* CQueueItemIndex::GetNext()
{
	return m_pNext;
}
inline int CQueueItemIndex::GetIdx()
{
	return m_nItemIdx;
}

///////////////////////////////////////////////////////////////////////////////
// CQueueStar
class b_DLL CQueueStar
{
////////// Attributes:
public:
	CPointerArrayMT<CQueueItemIndex> m_arrIndexes;
 	CQueueItemIndex*	m_pCurrentItemIndex;			// Current src ItemIndex
protected:
#ifndef __ASTRA__
	CDwordArrayMT		m_arrSignature;					// All users that is in use
#else
	CPtLongArrayMT         m_arrSignature;
#endif
	CWaitObj		m_woWakeUp;						// The Thread waits this event when nobody is connected
	CWaitObj		m_woQueueInitializationIsDone;
	CMultipleWait		m_mwCloseEvents;
	CCriticalSectionObj	m_lockItemChange;
	DWORD			m_dwTimeoutOnLostThread_ms;
	#ifdef WIN32
		LONG		m_lQueuedItemsCounter;
	#else
		OsMng::CMutex 	m_mtxCounter;
		long 		m_lQueuedItemsCounter;
	#endif //! WIN32;

////////// Construction:
public:
	CQueueStar();
	virtual ~CQueueStar();

////////// Client operations:
public:
	BOOL IsInitialized(DWORD dwWaitTime_ms = 0) ;
	CQueueItemIndex* Connect(CThreadEx* pThread);
	CQueueItemIndex* ConnectNoDelay(CThreadEx* pThread);
	void Disconnect(CThreadEx* pThread);
	CQueueItemIndex* GetNextFull(CQueueItemIndex* pItemIndex, CThreadEx* pThread); // if close event is on returns NULL
	
	void SetUsed(CQueueItemIndex* pItemIndex, CThreadEx* pThread);
	CQueueItemIndex* SetUsedGetNext(CQueueItemIndex* pItemIndex, CThreadEx* pThread);
	DWORD WaitForFullState(CQueueItemIndex* pItemIndex, CThreadEx* pThread, DWORD dwTimeout_ms = INFINITE);
	int GetCurIdx()
	{
		return (m_pCurrentItemIndex) ? m_pCurrentItemIndex->GetIdx() : -1;
	}
	void LockItemChange();
	void UnlockItemChange();
	LONG GetQueuedItemsCounter();

////////// Server operations:
public:
	BOOL IsClientConnected() const;
	void ResetDataFlag();
	void SetProcessingState(CQueueItemIndex* pItemIndex, CThreadEx* pThread);
	void AddCloseEvent(const CMultipleWait& mwCloseEvents);
	CMultipleWait GetCloseEvents();
	CQueueItemIndex* GetNextEmpty();
	void SetCurFull();
	void SetTimeoutOnLostThread(DWORD dwTimeout_ms);
	void Sleep( DWORD dwMilliseconds );

protected:
	DWORD WaitForEndOfProcessing(
						DWORD dwBegin_tick, 
						CQueueItemIndex* pItemIndex, 
						CThreadEx* pThread, 
						DWORD dwTimeout_ms);
	DWORD WaitForNewUsage(
						DWORD dwBegin_tick, 
						CQueueItemIndex* pItemIndex, 
						CThreadEx* pThread, 
						DWORD dwTimeout_ms);
	void RemoveLostSignaturesFromUsage(CQueueItemIndex* pItemIndex);
	BOOL WaitForItemIndexEmpty(CQueueItemIndex* pItemIndex);
	void SetNext(CQueueItemIndex* pItemIndex, int nIdx, CQueueItemIndex* pNext);
	void IncrementCounter();
	void DecrementCounter();

	#ifndef WIN32
public:
	DWORD GetTickCount() const
	{
		struct timeval tvalbuf;
		if ( gettimeofday( &tvalbuf, NULL ) == 0 )
			return (DWORD)(tvalbuf.tv_sec*1000+tvalbuf.tv_usec/1000);
		else
			return 0;
	}
	#endif //! WIN32
};

///////////////////////
// CQueueStar inlines
inline BOOL CQueueStar::IsClientConnected() const
{
	return m_arrSignature.GetSize() > 0;
}
inline BOOL CQueueStar::IsInitialized(DWORD dwWaitTime_ms) 
{
	return m_woQueueInitializationIsDone.IsSet(dwWaitTime_ms);
}
inline void CQueueStar::SetNext(CQueueItemIndex* pItemIndex, int nIdx, CQueueItemIndex* pNext)
{
	pItemIndex->m_pNext = pNext;
	pItemIndex->m_nItemIdx = nIdx;
}
inline void CQueueStar::Sleep( DWORD dwMilliseconds )
{
	#ifdef WIN32
		::Sleep(dwMilliseconds);
	#else
		usleep(dwMilliseconds*1000);
	#endif
}
///////////////////////////////////////////////////////////////////////////////
// T_CQueueStar
template<class DATA_TYPE>
class T_CQueueStar : public CQueueStar
{
public:
	typedef DATA_TYPE data_type;
	typedef T_CQueueItemIndex<DATA_TYPE> item_type;
protected:

public:
	T_CQueueStar()
	{
	}
	~T_CQueueStar()
	{
	}

public:
	void SetSize(int nNewSize)
	{
		if( nNewSize != m_arrIndexes.GetSize() )
		{
			LockItemChange();			// {

			if(nNewSize<2)
				nNewSize = 2;

			if(m_arrIndexes.GetSize() )
			{
				m_woQueueInitializationIsDone.ResetEvent();
				Sleep(1);
				m_arrIndexes.RemoveAll(true);
				m_pCurrentItemIndex = NULL;
			}

			m_arrIndexes.SetSize(nNewSize);
			
			int i=0;
			for(; i<nNewSize; i++)
				m_arrIndexes[i] = new T_CQueueItemIndex<DATA_TYPE>;

			for(i=0; i<nNewSize-1; i++)
			{
				SetNext(m_arrIndexes[i], i, m_arrIndexes[i+1]);
			}
			SetNext(m_arrIndexes[nNewSize-1], nNewSize-1, m_arrIndexes[0]);
			
			m_pCurrentItemIndex = m_arrIndexes[0];
			UnlockItemChange();			// }

			m_woQueueInitializationIsDone.SetEvent();
			Sleep(1);	// pass execution to the threads that wait for finish of initialization
		}
	}
	DATA_TYPE* GetNextEmpty()
	{
		T_CQueueItemIndex<DATA_TYPE>* pItem = (T_CQueueItemIndex<DATA_TYPE>*)CQueueStar::GetNextEmpty();
		return (pItem) ? pItem->GetData() : NULL;
	}
	T_CQueueItemIndex<DATA_TYPE>* Connect(CThreadEx* pThread)
	{
		return (T_CQueueItemIndex<DATA_TYPE>*)CQueueStar::Connect(pThread);
	}
	T_CQueueItemIndex<DATA_TYPE>* ConnectNoDelay(CThreadEx* pThread)
	{
		return (T_CQueueItemIndex<DATA_TYPE>*)CQueueStar::ConnectNoDelay(pThread);
	}
	T_CQueueItemIndex<DATA_TYPE>* GetNextFull(CQueueItemIndex* pItemIndex, CThreadEx* pThread)
	{
		return (T_CQueueItemIndex<DATA_TYPE>*)CQueueStar::GetNextFull(pItemIndex, pThread);
	}
	T_CQueueItemIndex<DATA_TYPE>* SetUsedGetNext(CQueueItemIndex* pItemIndex, CThreadEx* pThread)
	{
		return (T_CQueueItemIndex<DATA_TYPE>*)CQueueStar::SetUsedGetNext(pItemIndex, pThread);
	}
	int GetSize()
	{
		return m_arrIndexes.GetSize();
	}
	DATA_TYPE* operator[](int nIdx) // 
	{
		if( nIdx<0 || nIdx >= m_arrIndexes.GetSize() )
			return NULL;
		T_CQueueItemIndex<DATA_TYPE>* pItem = (T_CQueueItemIndex<DATA_TYPE>*)m_arrIndexes[nIdx];
		return (pItem) ? pItem->GetData() : NULL;
	}
};

///////////////////////////////
// CQueueStar inlines
inline LONG CQueueStar::GetQueuedItemsCounter()
{
	#ifdef WIN32
		return m_lQueuedItemsCounter;
	#else
		long lRetValue = 0;
		m_mtxCounter.lock();
		lRetValue = m_lQueuedItemsCounter;
		m_mtxCounter.unlock();
		return lRetValue;
	#endif //! WIN32
}
inline void CQueueStar::AddCloseEvent(const CMultipleWait& mwCloseEvents)
{
	m_mwCloseEvents.FindAdd(mwCloseEvents);
}

inline CMultipleWait CQueueStar::GetCloseEvents()
{
	return m_mwCloseEvents;
}

inline void CQueueStar::SetTimeoutOnLostThread(DWORD dwTimeout_ms)
{
	m_dwTimeoutOnLostThread_ms = dwTimeout_ms;
}

inline void CQueueStar::LockItemChange()
{
	m_lockItemChange.Enter();
}
inline void CQueueStar::UnlockItemChange()
{
	m_lockItemChange.Leave();
}


///////////////////////////////////////////////////////////////////////////////
// T_CQueueClient
template<class DATA_TYPE>
class T_CQueueClient
{
protected:
	CThreadEx* m_pThread;
	T_CQueueStar<DATA_TYPE>& m_qs;
	T_CQueueItemIndex<DATA_TYPE>* m_pCurrentItem;
	BOOL m_bConnected;

public:
	T_CQueueClient(T_CQueueStar<DATA_TYPE>& qs, CThreadEx* pThread)
	:m_pThread(pThread)
	,m_qs(qs)
	{
		m_pCurrentItem = NULL;
		m_bConnected = FALSE;
	}
	~T_CQueueClient()
	{
		Disconnect();
	}

public:
	BOOL IsInitialized(DWORD dwWaitTime_ms = 0) const
	{
		return m_qs.IsInitialized(dwWaitTime_ms);
	}
	BOOL ConnectNoDelay()
	{
		if(m_bConnected)
			Disconnect();
		m_pCurrentItem = m_qs.ConnectNoDelay(m_pThread);
		if(m_pCurrentItem)
			m_bConnected = true;
		return m_bConnected;
	}
	DATA_TYPE* Connect()
	{
		if(m_bConnected)
			Disconnect();
		m_pCurrentItem = m_qs.Connect(m_pThread);
		if(m_pCurrentItem)
			m_bConnected = true;
		return (m_pCurrentItem) ? m_pCurrentItem->GetData() : NULL;
	}
	void Disconnect()
	{
		if(m_bConnected)
		{
			m_qs.Disconnect(m_pThread);
			m_pCurrentItem = NULL;
			m_bConnected = FALSE;
		}
	}
	BOOL IsConnected()
	{
		return m_bConnected && IsInitialized();
	}
	DATA_TYPE* GetNext()
	{
		m_pCurrentItem = m_qs.GetNextFull(m_pCurrentItem, m_pThread);
		return (m_pCurrentItem) ? m_pCurrentItem->GetData() : NULL;
	}
	void Next()
	{
		m_pCurrentItem = (T_CQueueItemIndex<DATA_TYPE>*)m_pCurrentItem->GetNext();
	}
	void SetUsed()
	{
		m_pCurrentItem = m_qs.SetUsedGetNext(m_pCurrentItem, m_pThread);
	}
	BOOL IsItemFull(DWORD dwTimeout_ms = 0)
	{
		return WaitForFullState(dwTimeout_ms) == WAIT_OBJECT_0;
	}
	DWORD WaitForFullState(DWORD dwTimeout_ms = INFINITE)
	{
		if(m_pCurrentItem == NULL)
			return WAIT_FAILED;

		DWORD dwRes = m_qs.WaitForFullState(m_pCurrentItem, m_pThread, dwTimeout_ms);
		if(dwRes == WAIT_TIMEOUT)
		{
			if( m_qs.GetCloseEvents().IsSet() || m_pThread->GetCloseEvents().IsSet() )
				Disconnect();
		}
		return dwRes;
	}
	DATA_TYPE* GetData()
	{
		m_qs.SetProcessingState(m_pCurrentItem, m_pThread);
		return (m_pCurrentItem) ? m_pCurrentItem->GetData() : NULL;
	}
	int GetSize()
	{
		return m_qs.m_arrIndexes.GetSize();
	}
	DATA_TYPE* operator[](int nIdx) // 
	{
		return m_qs[nIdx];
	}
	int GetCurIdx()
	{
		return (m_pCurrentItem) ? m_pCurrentItem->GetIdx() : -1;
	}
};

}//namespace a_set
#endif // __QueueStar_h
