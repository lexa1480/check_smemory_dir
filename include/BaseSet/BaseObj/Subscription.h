/******************************************************************************
 File     : SafePointer.h
 Author   : SDY
 Date     : 17.12.2001
 Comments : "Subscription.txt"
 
******************************************************************************/

#ifndef __SafePointer_h
#define __SafePointer_h

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT assert
#endif

#include "BaseObject.h"
#include "../Array/Array.h"
#include "../Thread/Wait.h"
#include "../Dbg/Tracer.h"

namespace b_set
{

///////////////////////////////////////////////////////////////////////////////
// CSafePointer
template<class TYPE>
class CSafePointer
{
//////////
public:
protected:
	TYPE* m_pObj;
	CBaseObj m_boCopy;

//////////
public:
	CSafePointer()
	{
		m_pObj = NULL;
	}
	virtual ~CSafePointer()
	{
	}

//////////
public:
	CString GetObjectName() const;
	void CopyFrom(const CSafePointer& sp);
	CSafePointer& operator=(const CSafePointer& sp);
	bool IsValidPointer();
	TYPE* GetObj();
	bool Attach(TYPE* pObj);
protected:

};

///////////////////////////////
// CSafePointer inlines
template<class TYPE>
inline void CSafePointer<TYPE>::CopyFrom(const CSafePointer& sp)
{
	m_pObj = sp.m_pObj;
	m_boCopy.CopyBaseObjectFrom(sp.m_boCopy);
}
template<class TYPE>
inline CString CSafePointer<TYPE>::GetObjectName() const
{
	return m_boCopy.GetObjectName();
}

template<class TYPE>
inline CSafePointer<TYPE>& CSafePointer<TYPE>::operator=(const CSafePointer<TYPE>& sp)
{
	CopyFrom(sp);
	return *this;
}
template<class TYPE>
inline bool CSafePointer<TYPE>::IsValidPointer()
{
	return m_pObj->IsValidObject();
}
template<class TYPE>
inline TYPE* CSafePointer<TYPE>::GetObj()
{
	if(IsValidPointer())
		return m_pObj;
	else
	{
		ASSERT(FALSE);
		return NULL;
	}
}
template<class TYPE>
inline bool CSafePointer<TYPE>::Attach(TYPE* pObj)
{
	bool bRes = true;
	if( pObj->IsValidObject() )
	{
		if(pObj->IsObjectNameEmpty())
		{
			DBGOBJ(TYPE_ERR, 
					_T("Subscription"),
					_T("CSafePointer::Attach.\r\n")
					_T("\t\tObject is not subscribed, \r\n")
					_T("\t\tcause it's name cannot be empty.\r\n")
					_T("\t\tUse SetObjectName().")
				);
			ASSERT(FALSE);
			bRes = false;
		}
		else
		{
			m_pObj = pObj;
			m_boCopy.CopyBaseObjectFrom(*pObj);
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	return bRes;
}

///////////////////////////////////////////////////////////////////////////////
// CSubscriberArray
template<class TYPE>
class CSubscriberArray : virtual public CBaseObj
{
/////////
public:
	CArray < CSafePointer<TYPE>, 
			 CSafePointer<TYPE>& > 
							m_arrSafePointers;
	CCriticalSectionObj		m_lockSafePointers;
protected:
	BOOL					m_bOpen;

/////////
public:
	CSubscriberArray(LPCTSTR lpArrayName = NULL)
	:CBaseObj(lpArrayName)
	{
		m_bOpen = FALSE;
	}
	~CSubscriberArray()
	{
		Close();
		RemoveAll();
	}

/////////
public:
	void Open();
	void Close();
	BOOL IsOpen();
	BOOL Add(TYPE* pObj);
	BOOL Remove(TYPE* pObj);
	BOOL RemoveAll();
	BOOL RemoveAt(int nIdx);
	TYPE* GetAt(int nIdx, CString* strObjName = NULL);
	int GetSize() const;
	BOOL Lock();
	BOOL Unlock();
protected:
};

////////////////////////////
// CSubscriberArray inlines
template<class TYPE>
inline void CSubscriberArray<TYPE>::Open()
{
	Lock();
	m_bOpen = true;
	Unlock();
}
template<class TYPE>
inline void CSubscriberArray<TYPE>::Close()
{
	Lock();
	m_bOpen = FALSE;
	if(m_arrSafePointers.GetSize())
	{
		DBGOBJ(TYPE_WRN, _T("Subscription"), FT( _T("%s> %u subscribers left subscribed on close")
			, (LPCSTR)GetObjectName(), m_arrSafePointers.GetSize()) ); 
		for(int i=0; i<m_arrSafePointers.GetSize(); i++)
		{
			DBGOBJ(TYPE_WRN, _T("Subscription"), FT( _T("%s> %u -> %s")
				, (LPCSTR)GetObjectName(), i, (LPCSTR)m_arrSafePointers[i].GetObjectName() ) ); 
		}
	}
	Unlock();
}
template<class TYPE>
inline BOOL CSubscriberArray<TYPE>::IsOpen()
{
	return m_bOpen;
}
template<class TYPE>
inline int CSubscriberArray<TYPE>::GetSize() const
{
	return m_arrSafePointers.GetSize();
}
template<class TYPE>
inline TYPE* CSubscriberArray<TYPE>::GetAt(int nIdx, CString* pstrObjName)
{
	TYPE* ptr = NULL;
	Lock();
	if(nIdx>=0 && nIdx<m_arrSafePointers.GetSize())
	{
		try
		{
			ptr = m_arrSafePointers[nIdx].GetObj();
			if(pstrObjName)
				*pstrObjName = m_arrSafePointers[nIdx].GetObjectName();
		}
		catch(...)
		{
			DBGOBJ(TYPE_EXCEPTION, _T("Subscription"), _T("GetAt") ); 
			throw;
		}
	}
	Unlock();
	return ptr;
}
template<class TYPE>
inline BOOL CSubscriberArray<TYPE>::Lock()
{
	m_lockSafePointers.Enter();
	return true;
}
template<class TYPE>
inline BOOL CSubscriberArray<TYPE>::Unlock()
{
	m_lockSafePointers.Leave();
	return true;
}
template<class TYPE>
inline BOOL CSubscriberArray<TYPE>::Add(TYPE* pObj)
{
	if( !pObj->IsValidObject() )
	{
		DBGOBJ(TYPE_ERR, _T("Subscription"),
			FT( _T("%s> Attempt to subscribe not valid object."), (LPCTSTR)GetObjectName() ) 
			);
		ASSERT(FALSE);
		return FALSE;
	}
	
	if(!m_bOpen)
	{
		CString strObj;
		if(pObj->IsValidObject())
			strObj = pObj->GetObjectName();
		
		DBGOBJ(TYPE_ERR, _T("Subscription"),
			FT( _T("%s> Attempt to add subscriber \"%s\" to closed subscription.")
			, (LPCSTR)GetObjectName(), (LPCSTR)strObj)
			);
		
		ASSERT(FALSE);
	}

	Lock();
	
	BOOL bAdded = FALSE;
	int nIdx = -1;
	for(int i=0; i<m_arrSafePointers.GetSize(); i++)
	{
		if(m_arrSafePointers[i].GetObj() == pObj)
		{
			nIdx = i;
			break;
		}
	}
	if(nIdx==-1)
	{
		CSafePointer<TYPE> spItem;
		if( spItem.Attach(pObj) )
		{
			nIdx = m_arrSafePointers.Add(spItem);
			bAdded = (nIdx>=0);
		}
		if(!bAdded)
		{
			DBGOBJ(TYPE_ERR, 
				_T("Subscription"), 
				FT( _T("%s> Failed to add in array \"%s\": not subscribed"),
				(LPCSTR)GetObjectName(),
				(LPCSTR)spItem.GetObjectName()));
		}
	}
	
	Unlock();
	
	return bAdded;
}

template<class TYPE>
inline BOOL CSubscriberArray<TYPE>::RemoveAt(int nIdx)
{
	if(nIdx<0 || nIdx>= m_arrSafePointers.GetSize())
	{
		ASSERT(FALSE);
		
		DBGOBJ( TYPE_WRN,
				_T("Subscription"),
				FT( _T("%s> Incorrect index in CSubscriberArray::RemoveAt(%i), due to GetSize()==%i"),
				(LPCSTR)GetObjectName(),
				nIdx,
				m_arrSafePointers.GetSize() )
		);
		return FALSE;
	}

	BOOL bRes = true;

	m_arrSafePointers.RemoveAt(nIdx);

	return bRes;
}

template<class TYPE>
inline BOOL CSubscriberArray<TYPE>::Remove(TYPE* pObj)
{
	if(!m_bOpen)
		return FALSE;

	if(!pObj)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	BOOL bRes = FALSE;

	Lock();
	
	for(int i=0; i<m_arrSafePointers.GetSize(); i++)
	{
		if(m_arrSafePointers[i].GetObj() == pObj)
		{
			bRes = RemoveAt(i);
			break;
		}
	}
	
	Unlock();
	
	return bRes;
}

template<class TYPE>
inline BOOL CSubscriberArray<TYPE>::RemoveAll()
{
	if(m_arrSafePointers.GetSize()==0)
		return true;

	BOOL bRes = true;

	Lock();

	for(int i=m_arrSafePointers.GetSize()-1; i>=0; i--)
	{
		bRes = RemoveAt(i)
			&& bRes;
	}
	
	Unlock();
	
	return bRes;
}

}//namespace b_set
#endif // __SafePointer_h
