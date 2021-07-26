/******************************************************************************
 File     : KeyXML.h
 Author   : SDY
 Date     : 13.06.2006
 Comments : 
 
******************************************************************************/
#ifndef _KEY_XML_H
#define _KEY_XML_H

#include "ValueEx.h"

#include "ProxyOwner.h"

namespace registry
{
	class CNode;
}//registry

namespace b_set
{

namespace key_xml
{
// limiting the maximum size of the key name (from the docks to RegEnumKey)
#ifndef MAX_KEY_NAME
#define MAX_KEY_NAME 		(MAX_PATH+1)
#endif
// no limit on the maximum size of name value, but the length of the 400 (apparently, when> 256) regedit produce an exception
#ifndef MAX_VALUE_NAME
#define MAX_VALUE_NAME 		(MAX_PATH+1)
#endif
// no limit on the maximum amount, recorded in the value of data, according to MSDN "Registry Storage Space"
// not recommend more than 1-2 Kb
#ifndef MAX_VALUE_DATA_SIZE
#define MAX_VALUE_DATA_SIZE 	(2048)
#endif


#ifndef KEY_WRITE
#define KEY_WRITE
#endif

#ifndef KEY_READ
#define KEY_READ
#endif

#ifndef KEY_RDWR_RIGHTS
#define KEY_RDWR_RIGHTS
#endif

#include "../Array/Array.h"

/////////////////////////////////////////////////////////////////////////////
// CKeyEx

class b_DLL CKeyEx
{
////////////////////
// Attributes
protected:
	mutable registry::CNode*	m_pNode;
	mutable CProxyOwner*		m_pProxyOwner;
	mutable BOOL			m_bOpen;

	CString				m_strName; 	// name by which the key was opened
	CString				m_strFullPath; 	// full path to the key;

////////////////////
// Construction
public:
	CKeyEx();
	CKeyEx(LPCTSTR lpPath, BOOL bCreate = true);
	CKeyEx(const CKeyEx& key);
	virtual ~CKeyEx();

////////////////////
//Operators
public:
	CKeyEx& operator=( const CKeyEx& key);

////////////////////
// Main Operations
public:
	BOOL IsOpen() const;
	BOOL Open(LPCTSTR lpPath = NULL,BOOL bCreate = true);
	BOOL Reload( LPCTSTR lpPath = NULL );
	BOOL Close();
	BOOL CopyTo( CKeyEx& keyDest, BOOL bClearDestKey = true );
	BOOL CopyTo(LPCTSTR lpPath);
	LPCTSTR GetFullPath() const;
	CString GetFullKeyPath() const;
	CString GetString() const;
	LPCTSTR GetName() const;
	BOOL FlushData();
	BOOL IsChanged() const; // is any value or key changed
	CString GetKeyErrorText(LONG lErrorCode, LPCTSTR lpText, ...);
	BOOL GetDuplicate(CKeyEx& keyRes);
	CKeyEx GetDuplicate();
	BOOL IsValid() const;
	registry::CXMLProxy* GetProxy() const;
	registry::CNode* GetNode() const;
protected:
	static BOOL GetNameParts(LPCTSTR lpPath, CString& strFileName, CString& strKeyPath);
	void InitializeBy(const CKeyEx& key);
   	void Reset();
	BOOL CopyToRecursive(CKeyEx& keyDest);
	void SetName(LPCTSTR lpPath, LPCTSTR lpName);

	void SetLastError(int nVal);
	
////////////////////
// SubKeys
protected:
public:
	DWORD GetSubKeysCount() const;
	CKeyEx GetSubKey(DWORD nNum);
	CKeyEx GetSubKey(LPCTSTR lpSubkeyName, BOOL bCreate = true);
	CString GetSubKeyName(DWORD dwIdx);
	BOOL IsSubKey(LPCTSTR lpSubkeyName);
	BOOL DeleteSubKey(LPCTSTR lpSubkeyName);
	BOOL DeleteAllSubKeys();
	static BOOL DeleteKey(LPCTSTR lpFullPathToKey);
	BOOL GetSubKey(LPCTSTR lpSubkeyName, CKeyEx& keySubkey, BOOL bCreate = true);
	BOOL GetSubKeyName(DWORD dwIdx, CString& str);

////////////////////
// Values
public:
	DWORD GetValuesCount() const;
	CString GetValueName(DWORD dwIdx);
	// IsValue: whether there is a parameter with this name
	BOOL IsValue(LPCTSTR lpValueName);
	BOOL DeleteValue(LPCTSTR lpValueName); 
	BOOL DeleteAllValues();
	BOOL CopyValuesTo( CKeyEx& key, BOOL bDeleteOldValues = true );
	BOOL AddValue(CValueEx& value);
protected:
	BOOL GetValueName(DWORD dwIdx, CString& str);

////////////////////
//GetValue & SetValue:
public:
	BOOL QueryValue(LPCTSTR lpValueName, CValueEx& value);
	BOOL GetValue(DWORD dwIdx, CValueEx& value);
	BOOL GetValue( CValueEx& value, BOOL bSetIfNotGet = FALSE );
#ifndef __ASTRA__
	BOOL GetValue(LPCTSTR lpName, DWORD& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, LONG& data, BOOL bSetIfNotGet = FALSE);
#endif

	BOOL GetValue(LPCTSTR lpName, INT& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, UINT& data, BOOL bSetIfNotGet = FALSE);
#if __BSWORDSIZE == 32
	BOOL GetValue(LPCTSTR lpName, INT64& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, UINT64& data, BOOL bSetIfNotGet = FALSE);
#endif
	BOOL GetValue(LPCTSTR lpName, FLOAT& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, double& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, BYTE& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, WORD& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, std::string& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, CString& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, std::vector<std::string>& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, CStringArray& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, CDwordArray& data, BOOL bSetIfNotGet = FALSE);
/*	BOOL GetValue(LPCTSTR lpName, POINT& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValue(LPCTSTR lpName, RECT& data, BOOL bSetIfNotGet = FALSE);*/
	BOOL GetValue(LPCTSTR lpName, bool& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetValuePort(LPCTSTR lpName, UINT& uiSockPort_h, BOOL bSetIfNotGet = FALSE);
#ifdef GEOPOINT
	BOOL GetValue(LPCTSTR lpName, CGeoPoint& data, BOOL bSetIfNotGet = FALSE);
#endif
	
	BOOL GetDWORD(LPCTSTR lpName, DWORD& data, BOOL bSetIfNotGet = FALSE);
//	BOOL GetBinary(LPCTSTR lpName, CByteArray& arr, BOOL bSetIfNotGet = FALSE);
	BOOL GetStringExpand(LPCTSTR lpName, CString& data, BOOL bSetIfNotGet = FALSE);
	BOOL GetStringExpand(LPCTSTR lpName, std::string& data, BOOL bSetIfNotGet = FALSE);
	static BOOL ExpandString(CString& str);
	static BOOL ExpandString(std::string& str);
//

#ifndef __ASTRA__
	BOOL SetValue(LPCTSTR lpName, DWORD data);
	BOOL SetValue(LPCTSTR lpName, LONG data);
#endif

	BOOL SetValue(LPCTSTR lpName, INT data);
	BOOL SetValue(LPCTSTR lpName, UINT data);
#if __BSWORDSIZE == 32
	BOOL SetValue(LPCTSTR lpName, INT64 data);
	BOOL SetValue(LPCTSTR lpName, UINT64 data);
#endif
	BOOL SetValue(LPCTSTR lpName, FLOAT data);
	BOOL SetValue(LPCTSTR lpName, double data);
	BOOL SetValue(LPCTSTR lpName, BYTE data);
	BOOL SetValue(LPCTSTR lpName, WORD data);
	BOOL SetValue(LPCTSTR lpName, LPCTSTR data);
	BOOL SetValue(LPCTSTR lpName, const std::string& data);
	BOOL SetValue(LPCTSTR lpName, std::vector<std::string>& arr);
	BOOL SetValue(LPCTSTR lpName, CStringArray& arr);
	BOOL SetValue(LPCTSTR lpName, CDwordArray& arr);
/*	BOOL SetValue(LPCTSTR lpName, POINT& data);
	BOOL SetValue(LPCTSTR lpName, RECT& data);*/
	BOOL SetValue(LPCTSTR lpName, bool data);
#ifdef GEOPOINT
	BOOL SetValue(LPCTSTR lpName, CGeoPoint& data);
#endif

	BOOL SetDWORD(LPCTSTR lpName, DWORD dwData);
//	BOOL SetBinary(LPCTSTR lpName, CByteArray& arr);
	BOOL SetStringExpand(LPCTSTR lpName, LPCTSTR data);

////////////////////
// Hives
public:
	BOOL Save(LPCTSTR lpFileName);
	BOOL Restore(LPCTSTR lpFileName);
public:
	void Lock();
	void Unlock();
};

/////////////////////////////////////////////////////////////////////////////
// CKeyEx inlines
inline void CKeyEx::Reset()
{
	m_pNode = NULL;
	m_pProxyOwner = NULL;
	m_bOpen = FALSE;
}
inline registry::CNode* CKeyEx::GetNode() const
{
	return m_pNode;
}
inline registry::CXMLProxy* CKeyEx::GetProxy() const
{
	registry::CXMLProxy* pProxy = (m_pProxyOwner) ? m_pProxyOwner->GetProxy() : NULL;
	return pProxy;
}
inline CKeyEx& CKeyEx::operator=(const  CKeyEx& key)
{
	if(IsOpen())
		Close();
	InitializeBy(key);
	return *this;
}
inline LPCTSTR CKeyEx::GetFullPath() const
{
	return m_strFullPath;
}
inline CString CKeyEx::GetString() const
{
	CString strRes;
	strRes.Format("Key \"%s\"", (LPCTSTR)m_strFullPath);
	return strRes;
}
inline LPCTSTR CKeyEx::GetName() const
{
	return m_strName;
}
inline void CKeyEx::SetLastError(int nVal)
{
	#ifdef WIN32
		::SetLastError(nVal);
	#else
		errno = nVal;
	#endif //! WIN32
}

#ifdef GEOPOINT
inline BOOL CKeyEx::GetValue(LPCTSTR lpName, CGeoPoint& data, BOOL bSetIfNotGet)
{
	CValueEx value(lpName, data);
	BOOL bRes = QueryValue(lpName, value);
	if( bRes )
	{
		bRes = value.GetFromValue(data);
	}
	else if(bSetIfNotGet)
	{
		bRes = AddValue(value);
	}
	return bRes;
}
inline BOOL CKeyEx::SetValue(LPCTSTR lpName, CGeoPoint& data)
{
	CValueEx value(lpName, data);
	return AddValue(value);
}
#endif


}//! namespace key_xml

}//! namespace b_set

#endif //_KEY_XML_H
