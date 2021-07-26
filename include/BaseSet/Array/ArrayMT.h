/******************************************************************************
 File     : ArrayMT.h
 Author   : SDY
 Date     : 12.02.2002
 Comments : 
 
******************************************************************************/

#ifndef _ARRAY_MT_H
#define _ARRAY_MT_H

#include "Array.h"
#include "../Thread/Wait.h"

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT assert
#endif

namespace b_set
{

#define ARRAY_ERROR -1
///////////////////////////////////////////////////////////////////////////////
// CArrayMT
template<class TYPE, class ARG_TYPE>
class CArrayMT : public CArray<TYPE, ARG_TYPE>
{
//////////
public:
protected:
	CCriticalSectionObj	m_csLock;

//////////
public:
	CArrayMT() {}
	virtual ~CArrayMT() {}

//////////
public:
	void SetSize(int nNewSize, int nGrowBy = -1);

	void Lock();
	void Unlock();
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	TYPE GetAt(int nIndex) const;
	void SetAt(int nIndex, ARG_TYPE newElement);
	TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, ARG_TYPE newElement);
	int Add(ARG_TYPE newElement);
	int Append(CArrayMT& src);
	void Copy(CArrayMT& src);

	// overloaded operator helpers
	TYPE& operator[](int nIndex);
	TYPE operator[](int nIndex) const;

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CArrayMT* pNewArray);

	void Swap(int nIndex1,int nIndex2);
	void Move(int nIndexFrom,int nIndexTo);

protected:

};
///////////////////////////////////////////////////////////////////////////////
// CPointerArrayMT
template<class TYPE>
class CPointerArrayMT : public CArrayMT<TYPE*, TYPE*>
{
public:
	int Find(TYPE* findElement);
	int RemoveAt(TYPE* removeElement,BOOL bDestructElement);
	void RemoveAt(int nIndex,BOOL bDestructElement);
	void RemoveAll(BOOL bDestructElement);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CDwordArrayMT
class CDwordArrayMT : public CArrayMT<DWORD, DWORD>
{
public:
	CDwordArrayMT(){}
	virtual ~CDwordArrayMT(){}

public:
	void Clear();
	int Find(DWORD dwID);
	int FindAdd(DWORD dwID);
	int FindMinimumValueIndex();
	int FindMaximumValueIndex();
	BOOL Remove(DWORD dwID);
	void AddIntersection(CDwordArrayMT& arrTry, CDwordArrayMT& arrBase); // this += Try & Base
	void AddDifference(CDwordArrayMT& arrTry, CDwordArrayMT& arrBase);   // this += Try - Base
	void Add(CDwordArrayMT& arrToAdd);
	int Add(DWORD dwToAdd)
		{return CArrayMT<DWORD, DWORD>::Add(dwToAdd);}
	void Minus(CDwordArrayMT& arrToMinus);

public:
};
/////
/// for astra
#ifdef __ASTRA__
// CPtLontArrayMT (mbp)
typedef unsigned long PTLONG;
class CPtLongArrayMT : public CArrayMT<PTLONG, PTLONG>
{
public:
	CPtLongArrayMT(){}
	virtual ~CPtLongArrayMT(){}

public:
	void Clear();
	int Find(PTLONG dwID);
	int FindAdd(PTLONG dwID);
	int FindMinimumValueIndex();
	int FindMaximumValueIndex();
	BOOL Remove(PTLONG dwID);
	void AddIntersection(CPtLongArrayMT& arrTry, CPtLongArrayMT& arrBase); // this += Try & Base
	void AddDifference(CPtLongArrayMT& arrTry, CPtLongArrayMT& arrBase);   // this += Try - Base
	void Add(CPtLongArrayMT& arrToAdd);
	int Add(PTLONG dwToAdd)
	{return CArrayMT<PTLONG, PTLONG>::Add(dwToAdd);}
	void Minus(CPtLongArrayMT& arrToMinus);

public:
};
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// inlines

/////////////////////
// CArrayMT inlines
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::Lock()
{
	m_csLock.Lock();
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::Unlock()
{
	m_csLock.Unlock();
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::RemoveAll()
{
	Lock();
	CArray<TYPE, ARG_TYPE>::RemoveAll();
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline TYPE CArrayMT<TYPE, ARG_TYPE>::GetAt(int nIndex) const
{
	ASSERT( nIndex >= 0 && nIndex < (CArray<TYPE,ARG_TYPE>::m_nSize) );
	return CArray<TYPE,ARG_TYPE>::m_pData[nIndex];
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
{
	Lock();
	CArray<TYPE, ARG_TYPE>::SetAt(nIndex, newElement);
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline TYPE& CArrayMT<TYPE, ARG_TYPE>::ElementAt(int nIndex)
{
	Lock();
	TYPE& typeElement = CArray<TYPE, ARG_TYPE>::ElementAt(nIndex);
	Unlock();
	return typeElement;
}
template<class TYPE, class ARG_TYPE>
inline TYPE* CArrayMT<TYPE, ARG_TYPE>::GetData()
{
	Lock();
	TYPE* pElement = CArray<TYPE, ARG_TYPE>::GetData();
	Unlock();
	return pElement;
}
template<class TYPE, class ARG_TYPE>
inline int CArrayMT<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
{
	Lock();
	int nIdx = int(CArray<TYPE, ARG_TYPE>::Add(newElement));
	Unlock();
	return nIdx;
}
template<class TYPE, class ARG_TYPE>
inline TYPE& CArrayMT<TYPE, ARG_TYPE>::operator[](int nIndex)
{
	return ElementAt(nIndex);
}
template<class TYPE, class ARG_TYPE>
inline TYPE CArrayMT<TYPE, ARG_TYPE>::operator[](int nIndex) const
{
	return GetAt(nIndex);
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	Lock();
	CArray<TYPE, ARG_TYPE>::SetSize(nNewSize, nGrowBy);
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline int CArrayMT<TYPE, ARG_TYPE>::Append(CArrayMT& src)
{
	Lock();
	src.Lock();
	int nIdx = CArray<TYPE, ARG_TYPE>::Append(src);
	src.Unlock();
	Unlock();
	return nIdx;
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::Copy(CArrayMT& src)
{
	Lock();
	src.Lock();
	CArray<TYPE, ARG_TYPE>::Copy(src);
	src.Unlock();
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::FreeExtra()
{
	Lock();
	CArray<TYPE, ARG_TYPE>::FreeExtra();
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
	Lock();
	CArray<TYPE, ARG_TYPE>::SetAtGrow(nIndex, newElement);
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount /*=1*/)
{
	Lock();
	CArray<TYPE, ARG_TYPE>::InsertAt(nIndex, newElement, nCount);
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
	Lock();
	CArray<TYPE, ARG_TYPE>::RemoveAt(nIndex, nCount);
	Unlock();
}
template<class TYPE, class ARG_TYPE>
inline void CArrayMT<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, CArrayMT* pNewArray)
{
	if(pNewArray==NULL)
		return;
	Lock();
	pNewArray->Lock();
	CArray<TYPE, ARG_TYPE>::InsertAt(nStartIndex, pNewArray);
	pNewArray->Unlock();
	Unlock();
}

template<class TYPE, class ARG_TYPE>
void CArrayMT<TYPE, ARG_TYPE>::Swap(int nIndex1,int nIndex2)
{
	if (nIndex1 == nIndex2 || nIndex1 < 0 || nIndex1 >= (CArray<TYPE, ARG_TYPE>::m_nSize) || nIndex2 < 0 || nIndex2 >= (CArray<TYPE, ARG_TYPE>::m_nSize) )
		return;
	Lock();
	void* lpTemp = new char [sizeof(TYPE)];
	memcpy(lpTemp,&(CArray<TYPE, ARG_TYPE>::m_pData[nIndex1]),sizeof(TYPE));
	memcpy(&(CArray<TYPE, ARG_TYPE>::m_pData[nIndex1]),&(CArray<TYPE, ARG_TYPE>::m_pData[nIndex2]),sizeof(TYPE));
	memcpy(&(CArray<TYPE, ARG_TYPE>::m_pData[nIndex2]),lpTemp,sizeof(TYPE));
	delete [] lpTemp;
	Unlock();
}

template<class TYPE, class ARG_TYPE>
void CArrayMT<TYPE, ARG_TYPE>::Move(int nIndexFrom,int nIndexTo)
{
	if (nIndexFrom == nIndexTo || nIndexFrom < 0 || nIndexFrom >= (CArray<TYPE, ARG_TYPE>::m_nSize) || nIndexTo < 0 || nIndexTo >= (CArray<TYPE, ARG_TYPE>::m_nSize) )
		return;
	Lock();
	void* lpTemp = new char [sizeof(TYPE)];
	memcpy(lpTemp,&(CArray<TYPE, ARG_TYPE>::m_pData[nIndexFrom]),sizeof(TYPE));
	if (nIndexTo > nIndexFrom)
		memmove(&(CArray<TYPE, ARG_TYPE>::m_pData[nIndexFrom]),&(CArray<TYPE, ARG_TYPE>::m_pData[nIndexFrom + 1]), (nIndexTo - nIndexFrom) * sizeof(TYPE));
	else
		memmove(&(CArray<TYPE, ARG_TYPE>::m_pData[nIndexTo + 1]),&(CArray<TYPE, ARG_TYPE>::m_pData[nIndexTo]), (nIndexFrom - nIndexTo) * sizeof(TYPE));
	memcpy(&(CArray<TYPE, ARG_TYPE>::m_pData[nIndexTo]),lpTemp,sizeof(TYPE));
	delete [] lpTemp;
	Unlock();
}

///////////////////////////////
// CPointerArrayMT inlines
template<class TYPE>
int CPointerArrayMT<TYPE>::Find(TYPE* findElement)
{
	int nResult = -1;
	CArrayMT<TYPE*, TYPE*>::Lock();
	for (int i = 0; i < (CArray<TYPE*, TYPE*>::m_nSize); i++)
		if ((CArray<TYPE*, TYPE*>::m_pData[i]) == findElement)
		{
			nResult = i;
			break;
		}
	CArrayMT<TYPE*, TYPE*>::Unlock();
	return nResult;
}

template<class TYPE>
inline int CPointerArrayMT<TYPE>::RemoveAt(TYPE* removeElement,BOOL bDestructElement)
{
	CArrayMT<TYPE*,TYPE*>::Lock();
	int nIndex = Find(removeElement);
	if (nIndex != -1)
		CArray<TYPE*, TYPE*>::RemoveAt(nIndex);
	CArrayMT<TYPE*, TYPE*>::Unlock();
	if (bDestructElement && nIndex != -1)
		delete removeElement;
	return nIndex;
}

template<class TYPE>
inline void CPointerArrayMT<TYPE>::RemoveAt(int nIndex,BOOL bDestructElement)
{
	CArrayMT<TYPE*, TYPE*>::Lock();
	TYPE* lpElement = CArray<TYPE*, TYPE*>::ElementAt(nIndex);
	CArray<TYPE*, TYPE*>::RemoveAt(nIndex,1);
	CArrayMT<TYPE*, TYPE*>::Unlock();
	if (bDestructElement)
		delete lpElement;
}

template<class TYPE>
inline void CPointerArrayMT<TYPE>::RemoveAll(BOOL bDestructElement)
{
	if (bDestructElement)
	{
		while ((CArrayMT<TYPE*, TYPE*>::GetSize()))
			CPointerArrayMT<TYPE>::RemoveAt( CArrayMT<TYPE*, TYPE*>::GetSize()-1, TRUE );
	}
	else
		CArrayMT<TYPE*, TYPE*>::RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// inlines for CDwordArrayMT 
inline void CDwordArrayMT::Clear()
{
	Lock();
	RemoveAt(0, int (GetSize()) );
	Unlock();
}

inline int CDwordArrayMT::Find(DWORD dwID)
{
	int nRes = ARRAY_ERROR;
	Lock();
	for(int i=0; i<GetSize(); i++)
		if(GetAt(i) == dwID)
		{
			nRes = i;
			break;
		}
	Unlock();
	return nRes;
}

inline int CDwordArrayMT::FindMinimumValueIndex()
{
	int nRes = ARRAY_ERROR;
	Lock();
	if(GetSize())
	{
		nRes = 0;
		DWORD dwID = GetAt(0);
		for(int i=1; i<GetSize(); i++)
			if(GetAt(i) < dwID)
			{
				dwID = GetAt(i);
				nRes = i;
			}
	}
	Unlock();
	return nRes;
}

inline int CDwordArrayMT::FindMaximumValueIndex()
{
	int nRes = ARRAY_ERROR;
	Lock();
	if(GetSize())
	{
		nRes = 0;
		DWORD dwID = GetAt(0);
		for(int i=1; i<GetSize(); i++)
			if(GetAt(i) > dwID)
			{
				dwID = GetAt(i);
				nRes = i;
			}
	}
	Unlock();
	return nRes;
}

inline int CDwordArrayMT::FindAdd(DWORD dwID)
{
	int nIdx = ARRAY_ERROR;
	Lock();
	nIdx = Find(dwID);
	if(nIdx == ARRAY_ERROR)
		nIdx = Add(dwID);
	Unlock();
	return nIdx;
}

inline BOOL CDwordArrayMT::Remove(DWORD dwID)
{
	int nIdx = ARRAY_ERROR;
	Lock();
	nIdx = Find(dwID);
	if(nIdx != ARRAY_ERROR)
		RemoveAt(nIdx);
	Unlock();
	return (nIdx != ARRAY_ERROR);
}

inline void CDwordArrayMT::AddIntersection(CDwordArrayMT& arrTry, CDwordArrayMT& arrBase) // this += Try & Base; Try -= this;
{
	for(int i=0; i<arrTry.GetSize(); i++)
		if(arrBase.Find( arrTry[i] ) != ARRAY_ERROR)
		{
			if( Find( arrTry[i] ) == ARRAY_ERROR)
			{
				Add( arrTry[i] );
				i--;
			}
		}
}

inline void CDwordArrayMT::AddDifference(CDwordArrayMT& arrTry, CDwordArrayMT& arrBase) // this = 0; this += Try - Base
{
	for(int i=0; i<arrTry.GetSize(); i++)
		if(arrBase.Find( arrTry[i] ) == ARRAY_ERROR)
			FindAdd( arrTry[i] );
}

inline void CDwordArrayMT::Add(CDwordArrayMT& arrToAdd)
{
	Lock();
	for(int i=0; i<arrToAdd.GetSize(); i++)
		FindAdd( arrToAdd[i] );
	Unlock();
}

inline void CDwordArrayMT::Minus(CDwordArrayMT& arrToMinus)
{
	Lock();
	for(int i=0; i<arrToMinus.GetSize(); i++)
		if( Find( arrToMinus[i] ) != ARRAY_ERROR)
			Remove(arrToMinus[i]);
	Unlock();
}

//atsra
//////////////////////////////////////////////////////////////////////////////////////////////////////
// inlines for CDwordArrayMT 
#ifdef __ASTRA__
inline void CPtLongArrayMT::Clear()
{
	Lock();
	RemoveAt(0, int (GetSize()) );
	Unlock();
}

inline int CPtLongArrayMT::Find(PTLONG dwID)
{
	int nRes = ARRAY_ERROR;
	Lock();
	for(int i=0; i<GetSize(); i++)
		if(GetAt(i) == dwID)
		{
			nRes = i;
			break;
		}
		Unlock();
		return nRes;
}

inline int CPtLongArrayMT::FindMinimumValueIndex()
{
	int nRes = ARRAY_ERROR;
	Lock();
	if(GetSize())
	{
		nRes = 0;
		PTLONG dwID = GetAt(0);
		for(int i=1; i<GetSize(); i++)
			if(GetAt(i) < dwID)
			{
				dwID = GetAt(i);
				nRes = i;
			}
	}
	Unlock();
	return nRes;
}

inline int CPtLongArrayMT::FindMaximumValueIndex()
{
	int nRes = ARRAY_ERROR;
	Lock();
	if(GetSize())
	{
		nRes = 0;
		PTLONG dwID = GetAt(0);
		for(int i=1; i<GetSize(); i++)
			if(GetAt(i) > dwID)
			{
				dwID = GetAt(i);
				nRes = i;
			}
	}
	Unlock();
	return nRes;
}

inline int CPtLongArrayMT::FindAdd(PTLONG dwID)
{
	int nIdx = ARRAY_ERROR;
	Lock();
	nIdx = Find(dwID);
	if(nIdx == ARRAY_ERROR)
		nIdx = Add(dwID);
	Unlock();
	return nIdx;
}

inline BOOL CPtLongArrayMT::Remove(PTLONG dwID)
{
	int nIdx = ARRAY_ERROR;
	Lock();
	nIdx = Find(dwID);
	if(nIdx != ARRAY_ERROR)
		RemoveAt(nIdx);
	Unlock();
	return (nIdx != ARRAY_ERROR);
}

inline void CPtLongArrayMT::AddIntersection(CPtLongArrayMT& arrTry, CPtLongArrayMT& arrBase) // this += Try & Base; Try -= this;
{
	for(int i=0; i<arrTry.GetSize(); i++)
		if(arrBase.Find( arrTry[i] ) != ARRAY_ERROR)
		{
			if( Find( arrTry[i] ) == ARRAY_ERROR)
			{
				Add( arrTry[i] );
				i--;
			}
		}
}

inline void CPtLongArrayMT::AddDifference(CPtLongArrayMT& arrTry, CPtLongArrayMT& arrBase) // this = 0; this += Try - Base
{
	for(int i=0; i<arrTry.GetSize(); i++)
		if(arrBase.Find( arrTry[i] ) == ARRAY_ERROR)
			FindAdd( arrTry[i] );
}

inline void CPtLongArrayMT::Add(CPtLongArrayMT& arrToAdd)
{
	Lock();
	for(int i=0; i<arrToAdd.GetSize(); i++)
		FindAdd( arrToAdd[i] );
	Unlock();
}

inline void CPtLongArrayMT::Minus(CPtLongArrayMT& arrToMinus)
{
	Lock();
	for(int i=0; i<arrToMinus.GetSize(); i++)
		if( Find( arrToMinus[i] ) != ARRAY_ERROR)
			Remove(arrToMinus[i]);
	Unlock();
}
#endif
}//namespace b_set

#endif // _ARRAY_MT_H
