#ifndef _ARRAY_H
#define _ARRAY_H

#include <new>
#include <string.h>
#include <assert.h>

namespace b_set
{

#ifndef WIN32
    #define __stdcall
#endif

template<class TYPE>
inline void _CopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	while (nCount--)
		*pDest++ = *pSrc++;
}

template<class TYPE>
inline void __stdcall _ConstructElements(TYPE* pElements, int nCount)
{
	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}

template<class TYPE>
inline void __stdcall _DestructElements(TYPE* pElements, int nCount)
{
	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

/////////////////////////////////////////////////////////
// CArray
template<class TYPE, class ARG_TYPE>
class CArray
{
public:
// Construction
	CArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);
	bool IsEmpty() const { return m_nSize == 0; };

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	TYPE GetAt(int nIndex) const;
	void SetAt(int nIndex, ARG_TYPE newElement);
	TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, ARG_TYPE newElement);
	int Add(ARG_TYPE newElement);
	int Append(const CArray& src);
	void Copy(const CArray& src);

	// overloaded operator helpers
	TYPE operator[](int nIndex) const;
	TYPE& operator[](int nIndex);
	CArray& operator=(const CArray& src);

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CArray* pNewArray);

	// Arrangement
	void SortUp();
	void SortDown();

// Implementation
public:
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:
	virtual ~CArray();
};

///////////////////////////////////////////////////////////////////////////////
// CPointerArray
template<class TYPE>
class CPointerArray : public CArray<TYPE*, TYPE*>
{	
public:
	int Find(TYPE* findElement);
	int RemoveAt(TYPE* removeElement,bool bDestructElement=false);
	void RemoveAt(int nIndex,bool bDestructElement=false);
	void RemoveAll(bool bDestructElement=false);
};

/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE>
inline int CArray<TYPE, ARG_TYPE>::GetSize() const
	{ return m_nSize; }
template<class TYPE, class ARG_TYPE>
inline int CArray<TYPE, ARG_TYPE>::GetUpperBound() const
	{ return m_nSize-1; }
template<class TYPE, class ARG_TYPE>
inline void CArray<TYPE, ARG_TYPE>::RemoveAll()
	{ SetSize(0, -1); }
template<class TYPE, class ARG_TYPE>
inline TYPE CArray<TYPE, ARG_TYPE>::GetAt(int nIndex) const
	{ return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CArray<TYPE, ARG_TYPE>::ElementAt(int nIndex)
	{ return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
inline const TYPE* CArray<TYPE, ARG_TYPE>::GetData() const
	{ return (const TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
inline TYPE* CArray<TYPE, ARG_TYPE>::GetData()
	{ return (TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
inline int CArray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
template<class TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE>& CArray<TYPE, ARG_TYPE>::operator=(const CArray& src)
	{ Copy(src); return *this;}
template<class TYPE, class ARG_TYPE>
inline TYPE CArray<TYPE, ARG_TYPE>::operator[](int nIndex) const
	{ return GetAt(nIndex); }
template<class TYPE, class ARG_TYPE>
inline TYPE& CArray<TYPE, ARG_TYPE>::operator[](int nIndex)
	{ return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////
// CArray<TYPE, ARG_TYPE> out-of-line functions

template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::CArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
CArray<TYPE, ARG_TYPE>::~CArray()
{
	if (m_pData != NULL)
	{
		_DestructElements(m_pData, m_nSize);
		delete[] (unsigned char*)m_pData;
		m_pData = NULL;
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
{
	try
	{
		m_pData[nIndex] = newElement; 
	}
	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	try
	{
		if (nGrowBy != -1)
			m_nGrowBy = nGrowBy;  // set new size

		if (nNewSize == 0)
		{
			// shrink to nothing
			if (m_pData != NULL)
			{
				_DestructElements(m_pData, m_nSize);
				delete[] (unsigned char*)m_pData;
				m_pData = NULL;
			}
			m_nSize = m_nMaxSize = 0;
		}
		else if (m_pData == NULL)
		{
			// create one with exact size
			m_pData = (TYPE*) new unsigned char[nNewSize * sizeof(TYPE)];
			_ConstructElements(m_pData, nNewSize);
			m_nSize = m_nMaxSize = nNewSize;
		}
		else if (nNewSize <= m_nMaxSize)
		{
			// it fits
			if (nNewSize > m_nSize)
			{
				// initialize the new elements
				_ConstructElements(&m_pData[m_nSize], nNewSize-m_nSize);
			}
			else
			{
				// destroy the old elements
				_DestructElements(&m_pData[nNewSize], m_nSize-nNewSize);
			}
			m_nSize = nNewSize;
		}
		else
		{
			// otherwise, grow array
			int nGrowBy = m_nGrowBy;
			if (nGrowBy == 0)
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = m_nSize / 8;
				nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
			}
			int nNewMax;
			if (nNewSize < m_nMaxSize + nGrowBy)
				nNewMax = m_nMaxSize + nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush

			TYPE* pNewData = (TYPE*) new unsigned char[nNewMax * sizeof(TYPE)];

			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

			// construct remaining elements
			_ConstructElements(&pNewData[m_nSize], nNewSize-m_nSize);

			// get rid of old stuff (note: no destructor called)
			delete[] (unsigned char*)m_pData;
			m_pData = pNewData;
			m_nSize = nNewSize;
			m_nMaxSize = nNewMax;
		}
	}
	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::Append(const CArray& src)
{
	int nOldSize = 0;
	try
	{
		nOldSize = m_nSize;
		SetSize(m_nSize + src.m_nSize);
		_CopyElements(m_pData + nOldSize, src.m_pData, src.m_nSize);
	}
	catch(...)
	{
	}
	return nOldSize;
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Copy(const CArray& src)
{
	try
	{
		SetSize(src.m_nSize);
		_CopyElements(m_pData, src.m_pData, src.m_nSize);
	}

	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::FreeExtra()
{
	try
	{
		if (m_nSize != m_nMaxSize)
		{
			TYPE* pNewData = NULL;
			if (m_nSize != 0)
			{
				pNewData = (TYPE*) new unsigned char[m_nSize * sizeof(TYPE)];
				// copy new data from old
				memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
			}

			// get rid of old stuff (note: no destructors called)
			delete[] (unsigned char*)m_pData;
			m_pData = pNewData;
			m_nMaxSize = m_nSize;
		}
	}

	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
	try
	{
		if (nIndex >= m_nSize)
			SetSize(nIndex+1, -1);
		assert( m_pData != NULL );
		if( m_pData )
		{
			m_pData[nIndex] = newElement;
		}
	}

	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount /*=1*/)
{
	try
	{
		if (nIndex >= m_nSize)
		{
			// adding after the end of the array
			SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
		}
		else
		{
			// inserting in the middle of the array
			int nOldSize = m_nSize;
			SetSize(m_nSize + nCount, -1);  // grow it to new size
			// shift old data up to fill gap
			memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
				(nOldSize-nIndex) * sizeof(TYPE));

			// re-init slots we copied from
			_ConstructElements(&m_pData[nIndex], nCount);
		}

		// insert new value in the gap
		while (nCount--)
			m_pData[nIndex++] = newElement;
	}

	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
	try
	{
		// just remove a range
		int nMoveCount = m_nSize - (nIndex + nCount);
		_DestructElements(&m_pData[nIndex], nCount);
		if (nMoveCount)
			memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
				nMoveCount * sizeof(TYPE));
		m_nSize -= nCount;
	}

	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, CArray* pNewArray)
{
	try
	{
		if (pNewArray->GetSize() > 0)
		{
			InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
			for (int i = 0; i < pNewArray->GetSize(); i++)
				SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}

	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SortUp()
{
	try
	{
		bool bChange = false;
		do
		{
			bChange = false;
			for(int i=0; i<GetSize()-1; i++)
			{
				if(Compare(&m_pData[i], &m_pData[i+1]) > 0)
				{
					bChange = true;
					unsigned char pBuff[sizeof(TYPE)];
					memcpy(pBuff, &m_pData[i], sizeof(TYPE));
					memcpy(&m_pData[i], &m_pData[i+1], sizeof(TYPE));
					memcpy(&m_pData[i+1], pBuff, sizeof(TYPE));
				}
			}
		}
		while(bChange);
	}
	catch(...)
	{
	}
}

template<class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SortDown()
{
	try
	{
		bool bChange = false;
		do
		{
			bChange = false;
			for(int i=0; i<GetSize()-1; i++)
			{
				if(Compare(&m_pData[i], &m_pData[i+1]) < 0)
				{
					bChange = true;
					unsigned char pBuff[sizeof(TYPE)];
					memcpy(pBuff, &m_pData[i], sizeof(TYPE));
					memcpy(&m_pData[i], &m_pData[i+1], sizeof(TYPE));
					memcpy(&m_pData[i+1], pBuff, sizeof(TYPE));
				}
			}
		}
		while(bChange);
	}
	catch(...)
	{
	}
}

///////////////////////////////
// CPointerArray inlines
template<class TYPE>
inline int CPointerArray<TYPE>::Find(TYPE* findElement)
{
	int nResult = -1;
	for (int i = 0; i < CArray<TYPE*,TYPE*>::m_nSize; i++)
		if (CArray<TYPE*,TYPE*>::m_pData[i] == findElement)
		{
			nResult = i;
			break;
		}
	return nResult;
}

template<class TYPE>
inline int CPointerArray<TYPE>::RemoveAt(TYPE* removeElement,bool bDestructElement)
{
	int nIndex = Find(removeElement);
	if (nIndex != -1)
		CArray<TYPE*, TYPE*>::RemoveAt(nIndex);
	if (bDestructElement && nIndex != -1)
		delete removeElement;
	return nIndex;
}

template<class TYPE>
inline void CPointerArray<TYPE>::RemoveAt(int nIndex,bool bDestructElement)
{
	TYPE* lpElement = CArray<TYPE*, TYPE*>::ElementAt(nIndex);
	CArray<TYPE*, TYPE*>::RemoveAt(nIndex,1);
	if (bDestructElement)
		delete lpElement;
}

template<class TYPE>
inline void CPointerArray<TYPE>::RemoveAll(bool bDestructElement)
{
	if (bDestructElement)
	{
		while (CArray<TYPE*, TYPE*>::GetSize())
			RemoveAt(int(CArray<TYPE*, TYPE*>::GetSize())-1,true);
	}
	else
		CArray<TYPE*, TYPE*>::RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CDwordArray
class CDwordArray : public CArray<unsigned long, unsigned long>
{
public:
	CDwordArray(){}
	~CDwordArray(){}

public:
	void Clear();
	int Find(unsigned long dwID);
	int FindAdd(unsigned long dwID);
	bool Remove(unsigned long dwID);
	void AddCommon(CDwordArray& arrTry, CDwordArray& arrBase); // this += Try & Base; Try -= this;
	void SetDifference(CDwordArray& arrTry, CDwordArray& arrBase); // this = 0; this += Try - Base
	void Add(CDwordArray& arrToAdd, bool bClearArrToAdd = false);
	int Add(unsigned long dwToAdd)
		{return CArray<unsigned long, unsigned long>::Add(dwToAdd);}
	void Minus(CDwordArray& arrToMinus);

public:
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// inlines for CDwordArray 
inline void CDwordArray::Clear()
	{
		try
		{
			RemoveAt(0, GetSize() );
		}
		catch(...)
		{
		}
	}
inline int CDwordArray::Find(unsigned long dwID)
	{
		int iRes = -1;
		try
		{
			for(int i=0; i<GetSize(); i++)
				if(GetAt(i) == dwID)
				{
					iRes = i;
					break;
				}
		}
		catch(...)
		{
		}
		return iRes;
	}
inline int CDwordArray::FindAdd(unsigned long dwID)
	{
		int nIdx = -1;
		try
		{
			nIdx = Find(dwID);
			if(nIdx == -1)
				nIdx = Add(dwID);
		}
		catch(...)
		{
		}
		return nIdx;
	}
inline bool CDwordArray::Remove(unsigned long dwID)
	{
		int nIdx = -1;
		try
		{
			nIdx = Find(dwID);
			if(nIdx != -1)
				RemoveAt(nIdx);
		}
		catch(...)
		{
		}
		return (nIdx != -1);
	}
inline void CDwordArray::AddCommon(CDwordArray& arrTry, CDwordArray& arrBase) // this += Try & Base; Try -= this;
	{
		for(int i=0; i<arrTry.GetSize(); i++)
			if(arrBase.Find( arrTry[i] ) != -1)
			{
				if( Find( arrTry[i] ) == -1)
				{
					Add( arrTry[i] );
					arrTry.Remove( arrTry[i] );
					i--;
				}
			}
	}
inline void CDwordArray::SetDifference(CDwordArray& arrTry, CDwordArray& arrBase) // this = 0; this += Try - Base
	{
		Clear();
		for(int i=0; i<arrTry.GetSize(); i++)
			if(arrBase.Find( arrTry[i] ) == -1)
				Add( arrTry[i] );
	}
inline void CDwordArray::Add(CDwordArray& arrToAdd, bool bClearArrToAdd /*= false*/)
	{
		try
		{
			for(int i=0; i<arrToAdd.GetSize(); i++)
				FindAdd( arrToAdd[i] );
			if(bClearArrToAdd)
				arrToAdd.Clear();
		}
		catch(...)
		{
		}
	}
inline void CDwordArray::Minus(CDwordArray& arrToMinus)
	{
		try
		{
			for(int i=0; i<arrToMinus.GetSize(); i++)
				if( Find( arrToMinus[i] ) != -1)
					Remove(arrToMinus[i]);
		}
		catch(...)
		{
		}
	}

inline int Compare( CDwordArray& arrFirst, CDwordArray& arrSecond )
	{return arrFirst.GetSize() - arrSecond.GetSize();}

}; //! namespace b_set

#endif //! _ARRAY_H
