#ifndef __AFXCOLL_H__
#define __AFXCOLL_H__

#include "CString.h"

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT assert
#endif

namespace b_set
{
////////////////////////////////////////////////////////////////////////////
// CStringArray

class CStringArray
{
// Implementation
protected:
	CString* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

// Construction
public:
	CStringArray();
	~CStringArray();

// Attributes
public:
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
public:
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	CString GetAt(int nIndex) const;
	void SetAt(int nIndex, LPCTSTR newElement);

	void SetAt(int nIndex, const CString& newElement);

	CString& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const CString* GetData() const;
	CString* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, LPCTSTR newElement);

	void SetAtGrow(int nIndex, const CString& newElement);

	int Add(LPCTSTR newElement);

	int Add(const CString& newElement);

	int Append(const CStringArray& src);
	void Copy(const CStringArray& src);

	// overloaded operator helpers
	CString operator[](int nIndex) const;
	CString& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, LPCTSTR newElement, int nCount = 1);

	void InsertAt(int nIndex, const CString& newElement, int nCount = 1);

	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CStringArray* pNewArray);
	void InsertEmpty(int nIndex, int nCount);

public:
	static void ConstructElements(CString* pNewData, int nCount);
	static void DestructElements(CString* pOldData, int nCount);
	static void CopyElements(CString* pDest, CString* pSrc, int nCount);
};

inline int CStringArray::GetSize() const
	{ return m_nSize; }
inline int CStringArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CStringArray::RemoveAll()
	{ SetSize(0); }
inline CString CStringArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline void CStringArray::SetAt(int nIndex, LPCTSTR newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }
inline void CStringArray::SetAt(int nIndex, const CString& newElement)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }
inline CString& CStringArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
inline const CString* CStringArray::GetData() const
	{ return (const CString*)m_pData; }
inline CString* CStringArray::GetData()
	{ return (CString*)m_pData; }
inline int CStringArray::Add(LPCTSTR newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
inline int CStringArray::Add(const CString& newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
inline CString CStringArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline CString& CStringArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }
static inline void ConstructElement(CString* pNewData)
	{ CString afxEmptyString;
		memcpy(pNewData, &afxEmptyString, sizeof(CString)); }
static inline void DestructElement(CString* pOldData)
	{ pOldData->~CString(); }
static inline void CopyElement(CString* pSrc, CString* pDest)
	{ *pSrc = *pDest; }

}; //!namespace b_set

#endif //!__AFXCOLL_H__

/////////////////////////////////////////////////////////////////////////////
