/******************************************************************************
 File     : QueuePtrMT.h
 Author   : SDY
 Date     : 22.04.2003
 Comments : Queue
			Several vendors - one user
			"QueuePtrMT.txt"
 
******************************************************************************/

#ifndef __QueuePtrMT_h
#define __QueuePtrMT_h

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT assert
#endif

#include <deque>
#include "../Thread/MultiWait.h"
#include "../Thread/Wait.h"
#include "../Thread/ThreadEx.h"
#include "../File/FileTime.h"

namespace b_set
{

#define QUEUE_MAX_SIZE_DEF			(2048)
#define QUEUE_MAX_SIZE_UNLIMITED	(0x7FFFFFFF)

///////////////////////////////////////////////////////////////////////////////
// T_CQueuePtrMT
template<class TYPE>
class T_CQueuePtrMT
{
////////// Attributes:
public:
protected:
	CCriticalSectionObj m_lock;
	std::deque<TYPE*>	m_que;
	BOOL			m_bOpen;
	DWORD			m_dwMaxSize;
	BOOL			m_bEnableAdditionOnOverflow;
	CWaitObj		m_woWakeUp;
	CWaitObj		m_woQueIsNotFull; // used in PushWait(); Reset if que size >= then MaxSize
	CMultipleWait	m_mwWakeUp;
	CMultipleWait	m_mwQueIsNotFull;

////////// Construction:
public:
	T_CQueuePtrMT()
	{
		m_bOpen = FALSE;
		m_dwMaxSize = QUEUE_MAX_SIZE_DEF;
		m_bEnableAdditionOnOverflow = true;
		m_woQueIsNotFull.Create();
		m_woQueIsNotFull.SetEvent();
		m_mwQueIsNotFull.Add(&m_woQueIsNotFull);
		m_woWakeUp.Create();
		m_mwWakeUp.Add(&m_woWakeUp);
	}
	virtual ~T_CQueuePtrMT()
	{
		Close();
		DeleteAll();
	}

////////// Operations:
public:
	void Open();
	void Close();
	BOOL IsOpen() const;
	int Push(TYPE* pElement);
	int PushWithWait(TYPE* pElement); // waits if que is full
	TYPE* Pop();
	void PopAndDelete();
	TYPE* Front();
	BOOL IsEmpty();
	void Lock();
	void Unlock();
	void DeleteAll(BOOL bDestructElements = true);
	void DeleteElement(TYPE* pElement, BOOL bDestructElement = true);
	void DeleteElement(int nIdx, BOOL bDestructElement = true);
	int GetQueSize() const;
	TYPE* operator[](int nIdx);
	void AddCloseEvent(const CMultipleWait& mwCloseEvents);
	BOOL WaitForWakeUp(DWORD dwTimeout_ms);
	CWaitObj GetWakeUpEvent() const;
	
	// EnableAdditionOnOverflow: enables addition (Push) of a new element  
	// on queue overflow (if size is more then SetMaxSize).
	// On overflow we remove the first element, then add the new.
	// or refuse to add a new element.
	void EnableAdditionOnOverflow(BOOL bEnable);
	void SetMaxSize(DWORD dwMaxSize);

};

///////////////////////////////
// T_CQueuePtrMT inlines
template<class TYPE>
inline CWaitObj T_CQueuePtrMT<TYPE>::GetWakeUpEvent() const
{
	return m_woWakeUp;
}

template<class TYPE>
inline BOOL T_CQueuePtrMT<TYPE>::IsEmpty()
{
	return m_que.empty();
}

template<class TYPE>
inline BOOL T_CQueuePtrMT<TYPE>::IsOpen() const
{
	return m_bOpen;
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::AddCloseEvent(const CMultipleWait& mwCloseEvents)
{
	m_mwWakeUp.FindAdd(mwCloseEvents);
	m_mwQueIsNotFull.FindAdd(mwCloseEvents);
}

template<class TYPE>
inline BOOL T_CQueuePtrMT<TYPE>::WaitForWakeUp(DWORD dwTimeout_ms)
{
	return m_mwWakeUp.WaitMultipleObj(dwTimeout_ms) == WAIT_OBJECT_0;
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::EnableAdditionOnOverflow(BOOL bEnable)
{
	m_bEnableAdditionOnOverflow = bEnable;
}

template<class TYPE>
inline TYPE* T_CQueuePtrMT<TYPE>::Front()
{
	TYPE* pRes = NULL;
	Lock();
	try
	{
		pRes = (m_que.size()) ? m_que.front() : NULL;
	}
	catch(...)
	{
		pRes = NULL;
	}
	Unlock();

	return pRes;
}
template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::SetMaxSize(DWORD dwMaxSize)
{
	m_dwMaxSize = dwMaxSize;
}
template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::Lock()
{
	m_lock.Enter();
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::Unlock()
{
	m_lock.Leave();
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::Open()
{
	Lock();
	m_bOpen = true;
	Unlock();
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::Close()
{
	Lock();
	m_bOpen = FALSE;
	Unlock();
}

template<class TYPE>
inline TYPE* T_CQueuePtrMT<TYPE>::Pop()
{
	Lock();
	
	TYPE* pItem = NULL;
	if( !m_que.empty() )
	{
		pItem = m_que.front();
		m_que.pop_front();
	}

	if(m_que.empty())
		m_woWakeUp.ResetEvent();
	if(m_que.size() < m_dwMaxSize)
		m_woQueIsNotFull.SetEvent();

	Unlock();
	return pItem;
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::PopAndDelete()
{
	TYPE* pItem = Pop();
	if(pItem)
		delete pItem;
}

template<class TYPE>
inline int T_CQueuePtrMT<TYPE>::PushWithWait(TYPE* pElement)
{
	if(!m_bOpen)
	{
//		TRACE("ERR> An attempt to add the item to not open QueuePtrMT\r\n");
		ASSERT(FALSE);
		return -1;
	}
	else
	{

		BOOL bAdded = FALSE;
		int nIdx = -1;

		do
		{
			if(m_que.size() >= m_dwMaxSize)
			{
				if(m_mwQueIsNotFull.WaitMultipleObj(INFINITE) != WAIT_OBJECT_0)
					break;
			}

			Lock();

			if(m_que.size() >= m_dwMaxSize)
			{
				m_woQueIsNotFull.ResetEvent();
			}
			else
			{
				bAdded = true;
				m_que.push_back(pElement);
				nIdx = m_que.size()-1;
				m_woWakeUp.SetEvent();
			}

			Unlock();
		
		}while(!bAdded);

		return nIdx;
	}
}

template<class TYPE>
inline int T_CQueuePtrMT<TYPE>::Push(TYPE* pElement)
{
	if(!m_bOpen)
	{
//!		TRACE("ERR> An attempt to add the item to not open QueuePtrMT\r\n");
		ASSERT(FALSE);
		return -1;
	}

	Lock();

	int nIdx = -1;
	if(m_bOpen)
	{
		BOOL bEnableAddition = true;
		
		if(m_que.size() >= m_dwMaxSize)
		{
			bEnableAddition = m_bEnableAdditionOnOverflow;
			if(bEnableAddition)
				PopAndDelete();	// delete first element
		}
		
		if(bEnableAddition)
		{
			m_que.push_back(pElement);
			nIdx = m_que.size()-1;
			m_woWakeUp.SetEvent();
		}
	}

	Unlock();

	return nIdx;
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::DeleteAll(BOOL bDestructElements)
{
	Lock();
	
	while( !m_que.empty() )
	{
		if(bDestructElements)
		{
			TYPE* pItem = m_que.front();
			delete pItem;
		}
		m_que.pop_front();
	}
	m_woWakeUp.ResetEvent();
	m_woQueIsNotFull.SetEvent();

	Unlock();
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::DeleteElement(TYPE* pElement, BOOL bDestructElement)
{
	Lock();
	
	int i=0;
	for( ; i < (int)m_que.size(); i++ )
	{
		if( m_que.at(i) && m_que.at(i) == pElement )
		{
			m_que.erase(m_que.begin()+i);
			if(bDestructElement && pElement)
				delete pElement;
			if( m_que.empty() )
				m_woWakeUp.ResetEvent();
			if(m_que.size() < m_dwMaxSize)
				m_woQueIsNotFull.SetEvent();
			break;
		}
	}

	Unlock();
}

template<class TYPE>
inline void T_CQueuePtrMT<TYPE>::DeleteElement(int nIdx, BOOL bDestructElement)
{
	Lock();
	
	if( nIdx>=0 && nIdx < (int)m_que.size() )
	{
		TYPE* pElement = m_que.at(nIdx);
		m_que.erase(m_que.begin()+nIdx);
		if(bDestructElement && pElement)
			delete pElement;
		if( m_que.empty() )
			m_woWakeUp.ResetEvent();
		if(m_que.size() < m_dwMaxSize)
			m_woQueIsNotFull.SetEvent();
	}

	Unlock();
}

template<class TYPE>
inline int T_CQueuePtrMT<TYPE>::GetQueSize() const
{
	int iSize = 0;
	try
	{
		iSize = (int)m_que.size();
	}
	catch(...)
	{
		iSize = 0;
	}
	return iSize;
}

template<class TYPE>
inline TYPE* T_CQueuePtrMT<TYPE>::operator[](int nIdx)
{
	Lock();
	TYPE* pItem = NULL;
	if( nIdx>=0 && nIdx< (int)m_que.size() )
		pItem = m_que[nIdx];
	Unlock();
	return pItem;
}

}//namespace a_set
#endif // __QueuePtrMT_h
