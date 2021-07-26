/******************************************************************************
 File     : ProxyOwner.h
 Author   : SDY
 Date     : 13.06.2006
 Comments :
 
******************************************************************************/

#ifndef __ProxyOwner_h
#define __ProxyOwner_h

#include "../Array/CString.h"
#include "../Array/ArrayMT.h"

namespace registry
{
class CXMLProxy;
class CXMLFileParser;
class CBasicProxy;
}

namespace b_set
{

namespace key_xml
{
	class CKeyEx;
}
///////////////////////////////////////////////////////////////////////////////
// CProxyOwner
class b_DLL CProxyOwner
{
////////////////////
// Attributes
protected:
public:
	registry::CXMLProxy*	m_pProxy;
#ifndef __ASTRA__
	CDwordArrayMT		m_arrUsers;
#else
	CPtLongArrayMT		m_arrUsers;
#endif
	CString 		m_strFileName;

////////////////////
// Construction
public:
	CProxyOwner();
	~CProxyOwner();

////////////////////
//Operations
public:
	void Lock();
	void Unlock();
	void AddUser(key_xml::CKeyEx* pKey);
	void DelUser(key_xml::CKeyEx* pKey);
	BOOL HasUsers() const;
	void Close();
	BOOL IsOpen() const;
	BOOL IsChanged() const;
	BOOL IsValid(LPCTSTR lpPath) const;
	BOOL Open(LPCTSTR lpFileName);
	BOOL Save(LPCTSTR lpFileName = NULL);
	registry::CXMLProxy* GetProxy() const;
	BOOL Reload( LPCTSTR lpFileName );
};

inline BOOL CProxyOwner::IsOpen() const
{
	return (this!= NULL) && (m_pProxy != NULL);
}
inline void CProxyOwner::Lock()
{
	m_arrUsers.Lock();
}
inline void CProxyOwner::Unlock()
{
	m_arrUsers.Unlock();
}
inline void CProxyOwner::AddUser(key_xml::CKeyEx* pKey)
{
	if( IsOpen() )
	{
	    #ifndef __ASTRA__
	    	m_arrUsers.FindAdd( (DWORD)pKey );
	    #else
	    	m_arrUsers.FindAdd( (PTLONG)pKey );
	    #endif
	}
}
inline void CProxyOwner::DelUser(key_xml::CKeyEx* pKey)
{
	if( IsOpen() )
	{
	    #ifndef __ASTRA__
		m_arrUsers.Remove( (DWORD)pKey );
	    #else
		m_arrUsers.Remove( (PTLONG)pKey );
	    #endif
	}
}
inline BOOL CProxyOwner::HasUsers() const
{
	return ( m_arrUsers.GetSize() > 0 );
}

}//! namespace b_set
#endif //! __ProxyOwner_h
