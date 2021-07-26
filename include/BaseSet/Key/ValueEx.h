// value.h : header file
//

#ifndef _VALUE_EX_H
#define _VALUE_EX_H

#ifdef GEOPOINT
#include <GeoMath\GeoPoint.h>
#include <GeoMath\GeoFormater.h>
#endif

#include <vector>
#include <string>

#ifndef b_DLL
#define b_DLL
#endif //! b_DLL

#include "../BaseTypes_x.h"
#include "../Array/CString.h"
#include "../Array/StringArray.h"


namespace b_set
{

#ifndef WIN32

#include <errno.h>

#define REG_NONE 	0
#define REG_EXPAND_SZ 	2
#define REG_SZ		1
#define REG_DWORD 	4
#define REG_MULTI_SZ 	7

#define ERROR_SUCCESS 			0
#define ERROR_INVALID_DATA 		EINVAL
#define ERROR_BAD_FORMAT 		EBADF
#define ERROR_FILE_NOT_FOUND 		ENOENT
#define ERROR_INVALID_HANDLE 		EFAULT
#define ERROR_INVALID_PARAMETER 	EINVAL
#define ERROR_INVALID_FUNCTION 		EINVAL
#define ERROR_PATH_NOT_FOUND 		ENOENT
#define ERROR_ACCESS_DENIED 		ENOLCK
#define ERROR_HANDLE_DISK_FULL		ENOSPC

#endif //! WIN32

/////////////////////////////////////////////////////////////////////////////
// CValueEx
class b_DLL CValueEx
{
// Attributes
protected:
	CString m_strName;
	DWORD m_dwType;     // type REG_...
	LPBYTE m_lpbData;
	DWORD m_dwDataSize;

// Construction
public:
	CValueEx();
	CValueEx(CValueEx& value)
	{
		SetData(value);
	}

	CValueEx(	LPCTSTR lpValueName,
			DWORD dwType,
			BYTE const*lpbData,
			DWORD dwSize)
	{
		SetData(	lpValueName,
				dwType,
				lpbData,
				dwSize);
	}

	CValueEx(LPCTSTR lpValueName, LPCTSTR lpData, DWORD dwType = REG_SZ);
	CValueEx(LPCTSTR lpValueName, const std::string& strData, DWORD dwType = REG_SZ);
	CValueEx(LPCTSTR lpValueName, const INT& nData);
#ifndef __ASTRA__
	CValueEx(LPCTSTR lpValueName, const UINT& unData);
#endif
#if __BSWORDSIZE == 32
	CValueEx(LPCTSTR lpValueName, const INT64& n64Data);
	CValueEx(LPCTSTR lpValueName, const UINT64& un64Data);
#endif
#ifndef __ASTRA__
	CValueEx(LPCTSTR lpValueName, const LONG& lData);
#endif
	CValueEx(LPCTSTR lpValueName, const DWORD& dwData, BOOL bAsDWORD = FALSE);

	CValueEx(LPCTSTR lpValueName, const FLOAT& fData);
	CValueEx(LPCTSTR lpValueName, const double& dData);
	CValueEx(LPCTSTR lpValueName, const BYTE& byData);
	CValueEx(LPCTSTR lpValueName, const WORD& wData);
	CValueEx(LPCTSTR lpValueName, const CString& strData);
	CValueEx(LPCTSTR lpValueName, const CStringArray& arr);
//	CValueEx(LPCTSTR lpValueName, const CByteArray& arr);
//	CValueEx(LPCTSTR lpValueName, const POINT& pt);
//	CValueEx(LPCTSTR lpValueName, const RECT& rc);
	CValueEx(LPCTSTR lpValueName, const bool& bData);


	CValueEx(LPCTSTR lpValueName, const std::vector<std::string>& data);
#ifdef GEOPOINT
	CValueEx(LPCTSTR lpValueName, const CGeoPoint& gp);
#endif
	
	virtual ~CValueEx();

public:
	LPCTSTR	GetName() const;
	int	GetNameLength() const;
	DWORD	GetType() const;
	DWORD	GetDataSize() const;
	LPBYTE	GetData();

	void Reset();
	void SetData(LPCTSTR lpValueName, const std::vector<std::string>& arr );
	void SetData(LPCTSTR lpValueName, const CStringArray& arr);
	void SetData(LPCTSTR lpValueName, LPCTSTR lpData, DWORD dwType = REG_SZ);
	void SetData(CValueEx const& value);
	void SetData(
				LPCTSTR lpValueName,
				DWORD dwType,
				BYTE const* lpbData,
				DWORD dwSize
				);

#ifndef __ASTRA__
	BOOL GetFromValue(LONG& lData);
	BOOL GetFromValue(DWORD& dwData);
#endif

	BOOL GetFromValue(INT& nData);
	BOOL GetFromValue(UINT& unData);
#if __BSWORDSIZE == 32
	BOOL GetFromValue(INT64& n64Data);
	BOOL GetFromValue(UINT64& un64Data);
#endif
	BOOL GetFromValue(FLOAT& fData);
	BOOL GetFromValue(double& dData);
	BOOL GetFromValue(BYTE& byData);
	BOOL GetFromValue(WORD& wData);
	BOOL GetFromValue(std::string& strData);
	BOOL GetFromValue(CString& strData);
	BOOL GetFromValue(CStringArray& arr);
	BOOL GetFromValue(std::vector<std::string>& arr);
//	BOOL GetFromValue(CByteArray& arr);
//	BOOL GetFromValue(POINT& pt);
//	BOOL GetFromValue(RECT& rc);
	BOOL GetFromValue(bool& bData);
#ifdef GEOPOINT
	BOOL GetFromValue(CGeoPoint& gp);
#endif

protected:
	// SetDataDirect: assign a pointer m_lpbData = lpbData
	// SetDataDirect: operator new for m_lpbData and making copy from lpbData are not doing
	// SetDataDirect(CValueEx& value): After assigning a value m_lpbData value.m_lpbData is reset
	void SetDataDirect(CValueEx& value);
	void SetDataDirect( 
				LPCTSTR lpValueName,
				DWORD dwType,
				BYTE* lpbData,
				DWORD dwSize
				);

	void SetLastError( int nVal );
// Operators
public:
	CValueEx& operator=(CValueEx const& value);
};

/////////////////////////////////////////////////////////////////////////////
// inlines 
inline CValueEx& CValueEx::operator=(CValueEx const& value)
{
	SetData(value);
	return *this;
}
inline int CValueEx::GetNameLength() const
{
	return int(m_strName.GetLength());
}
inline LPCTSTR CValueEx::GetName() const
{
	return (LPCTSTR)m_strName;
}
inline DWORD CValueEx::GetType() const
{
	return m_dwType;
}
inline DWORD CValueEx::GetDataSize() const
{
	return m_dwDataSize;
}
inline LPBYTE CValueEx::GetData()
{
	return m_lpbData;
}
inline void CValueEx::SetLastError(int nVal)
{
	#ifdef WIN32
		::SetLastError(nVal);
	#else
		errno = nVal;
	#endif
}

#ifdef GEOPOINT
inline CValueEx::CValueEx(LPCTSTR lpValueName, const CGeoPoint& gp)
{
	m_lpbData = NULL;
	Reset();
	CString str = CGeoFormater().Geo2String(gp).c_str();
	SetData(lpValueName, str);
}
inline BOOL CValueEx::GetFromValue(CGeoPoint& gp)
{
	CString str;
	GetFromValue(str);
	gp = CGeoFormater().String2Geo(std::string(str));
	return gp.Valid();
}
#endif

}//! namespace b_set

/////////////////////////////////////////////////////////////////////////////
#endif //! _VALUE_EX_H
