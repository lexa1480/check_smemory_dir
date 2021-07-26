#ifndef CSTRING_H
#define CSTRING_H

#include <string>
#include <stdio.h>
#include <algorithm>
#include <string.h>

#include "../BaseTypes_x.h"

#ifdef WIN32
	#include "../stdafx.h"
	#include <wtypes.h>
	#include <tchar.h>
#else //!WIN32 not define

	#include <wchar.h>
	#include <wctype.h>
	#include <ctype.h>

	#ifdef UNICODE

		#define _tcslen 	wcslen
		#define _tcscmp 	wcscmp
		#define _tcsicmp 	wcscasecmp
		#define _tcscoll 	wcscoll
		#define _tcsicoll 	wcscasecmp
		#define _totupper 	towupper
		#define _totlower 	towlower
		#define _tcsinc 	_wcsinc
		#define _tcsdec 	_wcsdec		
		#define _tcschr 	wcschr
		#define _tcscspn 	wcscspn
		#define _ttoi 		_wcstol
		#define _tstof		_wcstof
		#define _tstoi64	_wcstoll
		#define _istdigit 	iswdigit
		#define _tcspbrk 	wcspbrk
		#define _tcsstr 	wcsstr
		#define _tcsspn 	wcsspn
		#define _tcsrchr	wcsrchr
		#define _stscanf 	swscanf

		#define _vsntprintf 	vswprintf

	#else //!UNICODE not define

// In the "POSIX" strcoll() equivalent to strcmp() (see man pages) //

		#define _tcslen		strlen
		#define _tcscmp 	strcmp
		#define _tcsicmp 	strcasecmp
		#define _tcscoll 	strcoll
		#define _tcsicoll 	strcasecmp
		#define _totupper 	toupper
		#define _totlower 	tolower
		#define _tcsinc 	_wcsinc
		#define _tcsdec 	_mbsdec
		#define _tcschr 	strchr
		#define _tcscspn 	strcspn
		#define _ttoi 		atoi
		#define _tstof		atof
		#define _tstoi64	atoll
		#define _istdigit 	isdigit
		#define _tcspbrk 	strpbrk
		#define _tcsstr 	strstr
		#define _tcsspn 	strspn
		#define _tcsrchr 	strrchr
		#define _stscanf 	sscanf

		#define _vsntprintf	vsnprintf
	#endif //!UNICODE

	

	#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
	#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
	#define LOWORD(l)           ((WORD)((DWORD)(l) & 0xffff))
	#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))
	#define LOBYTE(w)           ((BYTE)((DWORD)(w) & 0xff))
	#define HIBYTE(w)           ((BYTE)((DWORD)(w) >> 8))

/*	#ifndef b_max
		#define b_max(a,b)            (((a) > (b)) ? (a) : (b))
	#endif //!max

	#ifndef b_min
		#define b_min(a,b)            (((a) < (b)) ? (a) : (b))
	#endif //!min
*/
	//!FOR FORMAT FUNC
/*        #define _ADDRESSOF(v)   ( &reinterpret_cast<const char &>(v) )
	#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

	#define _crt_va_start(ap,v)  ( ap = (va_list)_ADDRESSOF(v) + _INTSIZEOF(v) )
	#define _crt_va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
	#define _crt_va_end(ap)      ( ap = (va_list)0 )

	#define va_arg 		_crt_va_arg
	#define va_start 	_crt_va_start
	#define va_end 		_crt_va_end
*/

	__inline  TCHAR * _wcsspnp
	( 
	 const TCHAR * _Cpc1, 
	 const TCHAR * _Cpc2
	);

	#define __cdecl
	__inline TCHAR* __cdecl _wcsdec( const TCHAR * _Cpc1, const TCHAR * _Cpc2) 
	{ 
		return (TCHAR *)((_Cpc1)>=(_Cpc2) ? NULL : ((_Cpc2)-1)); 
	}
	
	__inline TCHAR* __cdecl _wcsinc( const TCHAR * _Pc) 
	{ 
		return (TCHAR *)(_Pc+1); 
	}

	__inline  size_t  _tclen( const TCHAR * /*_Cpc*/ )
	{ 
		/* avoid compiler warning */
//		(void *)_Cpc;
		return sizeof(TCHAR);
		//! mblen (const char* s, size_t n); for multibyte
	}

	__inline int __cdecl _wcstol ( LPCTSTR lpsz )
	{
		return wcstol((wchar_t*)lpsz,0,10);
	}

	__inline float __cdecl _wcstof ( LPCTSTR lpsz )
	{
		return wcstof((wchar_t*)lpsz,0);
	}

	__inline long long int __cdecl _wcstoll ( LPCTSTR lpsz )
	{
		return wcstoll((wchar_t*)lpsz,0,10);
	}

#endif // !WIN32

namespace b_set
{
#ifdef UNICODE 
	__inline CHAR* __cdecl _strinc( const CHAR * _Pc) 
	{ 
		return (CHAR *)(_Pc+1); 
	}
#endif
class CString
{
public:
	CString();
	CString(const CString& stringSrc);

	#ifdef UNICODE
		CString(LPCSTR lpsz);
	#else
		CString(LPCWSTR lpsz);
	#endif

	CString(LPCTSTR lpsz);
	CString(LPCTSTR lpch, int nLength);
	CString(const unsigned char* psz);
	~CString();

	// Attributes & Operations
	// as an array of characters
	int GetLength() const;
	bool IsEmpty(void) const;
	void Empty();                       // free up the data

	TCHAR GetAt(int nIndex) const;      // 0 based
	TCHAR operator[](int nIndex) const; // same as GetAt
	void SetAt(int nIndex, TCHAR ch);
	operator LPCTSTR() const;           // as a C string
	
	void Convert();

	#ifdef UNICODE
		operator LPCSTR() const;
	#else
		operator LPCWSTR() const;
	#endif //! UNICODE
		
	// overloaded assignment
	const CString& operator=(const CString& stringSrc);
	const CString& operator=(TCHAR ch);
	const CString& operator=(LPCSTR lpsz);
	const CString& operator=(LPCWSTR lpsz);
	const CString& operator=(const unsigned char* psz);

	// string concatenation
	const CString& operator+=(const CString& stringSrc);
	const CString& operator+=(TCHAR ch);
	const CString& operator+=(LPCTSTR lpsz);

	// string comparison
	int Compare(LPCTSTR lpsz) const;         // straight character
	int CompareNoCase(LPCTSTR lpsz) const;   // ignore case
	int Collate(LPCTSTR lpsz) const;         // NLS aware
	int CollateNoCase(LPCTSTR lpsz) const;

	// simple sub-string extraction
	CString Mid(int nFirst, int nCount) const;
	CString Mid(int nFirst) const;
	CString Left(int nCount) const;
	CString Right(int nCount) const;

	CString SpanIncluding(LPCTSTR lpszCharSet) const;
	CString SpanExcluding(LPCTSTR lpszCharSet) const;

	// upper/lower/reverse conversion
	void MakeUpper();
	void MakeLower();
	void MakeReverse();

	// searching (return starting index, or -1 if not found)
	// look for a single character match
	int Find(TCHAR ch) const;               // like "C" strchr
	int ReverseFind(TCHAR ch) const;
	int FindOneOf(LPCTSTR lpszCharSet) const;

	// look for a specific sub-string
	int Find(LPCTSTR lpszSub) const;        // like "C" strstr

	// simple formatting
	void Format(LPCTSTR lpszFormat, ...);
        void FormatV(LPCTSTR lpszFormat, va_list argList );

	#ifdef UNICODE
                void Format(LPCSTR lpszFormat, ...);
                void FormatV(LPCSTR lpszFormat, va_list argList );
        #endif

	// Windows support
	BOOL LoadString(UINT nID);          // load from string resource

	int Delete(int nIndex, int nCount /* = 1 */);

	
	LPTSTR GetBufferSetLength(int nLength);
protected:
	#ifdef UNICODE
		std::wstring m_data;
		std::string  m_strConverted; //for convert from wchar_t to char
	#else
		std::string m_data;
		std::wstring m_strConverted; //for convert from char to wchar_t
	#endif


        int GetFormattedLenght( LPCTSTR lpszFormat, va_list argList );

        #ifdef UNICODE                
                int GetFormattedLenght( LPCSTR lpszFormat, va_list argList );
        #endif
};
	//////////////////////////////////////////////////////////////////////////
	//
	inline bool CString::IsEmpty() const
	{ return m_data.empty(); }
	inline int CString::GetLength() const
	{ return m_data.length(); }
	inline TCHAR CString::GetAt(int nIndex) const
	{ return m_data[nIndex];	}
	inline TCHAR CString::operator[](int nIndex) const
	{ return m_data[nIndex];	}
	inline void CString::SetAt(int nIndex, TCHAR ch)
	{ m_data[nIndex] = ch; }
	inline CString::operator LPCTSTR() const
	{ return (LPCTSTR)m_data.c_str(); }
	inline const CString& CString::operator=(const unsigned char* lpsz)
	{ *this = (LPCSTR)lpsz; return *this; }

	// CString support (windows specific)
	inline int CString::Compare(LPCTSTR lpsz) const
	{ return _tcscmp(m_data.c_str(), lpsz); }    // MBCS/Unicode aware
	inline int CString::CompareNoCase(LPCTSTR lpsz) const
	{ return _tcsicmp(m_data.c_str(), lpsz); }   // MBCS/Unicode aware
	// CString::Collate is often slower than Compare but is MBSC/Unicode
	//  aware as well as locale-sensitive with respect to sort order.
	inline int CString::Collate(LPCTSTR lpsz) const
	{ return _tcscoll(m_data.c_str(), lpsz); }   // locale sensitive
	inline int CString::CollateNoCase(LPCTSTR lpsz)const
	{ return _tcsicoll(m_data.c_str(), lpsz); }	//! ???

	//////////////////////////////////////////////////////////////////////////
	inline void CString::MakeReverse()
	{
		#ifdef WIN32
			_tcsrev((TCHAR*)&m_data[0]);
		#else
			size_t nCount = m_data.length();
			for( size_t i = 0; i < nCount / 2; ++i )
				std::swap( m_data[ i ], m_data[ nCount-i-1 ] );
		#endif
		Convert();
	}
	//////////////////////////////////////////////////////////////////////////
	inline LPTSTR CString::GetBufferSetLength(int nLength)
	{
	    m_data.resize(nLength+2);
	    return &m_data[0];
	}
	//////////////////////////////////////////////////////////////////////////
	inline void CString::MakeUpper() 
	{
		std::transform(m_data.begin(), m_data.end(), m_data.begin(), ::_totupper);
	}
	//////////////////////////////////////////////////////////////////////////
	inline void CString::MakeLower()
	{
		std::transform(m_data.begin(), m_data.end(), m_data.begin(), ::_totlower);
	}
	//////////////////////////////////////////////////////////////////////////
	inline void CString::Empty()
	{
	   m_data.erase();
	   m_strConverted.erase();
	}
	//////////////////////////////////////////////////////////////////////////

	#ifdef UNICODE
		inline CString::operator LPCSTR() const
		{return m_strConverted.c_str();}
	#else
		inline CString::operator LPCWSTR() const
		{return m_strConverted.c_str();}
	#endif //!UNICODE


	inline const CString operator+( const CString &s1, const CString &s2 )
	{
		CString tmp(s1);
		tmp += s2;
		return tmp;
	}

	inline const CString operator+( const CString &s1, LPCTSTR s2 )
	{
		CString tmp( s1 );
		tmp += s2;
		return tmp;
	}

	inline bool operator!=(const CString& str1, const LPCTSTR& s2 )
	{
		return( str1.Compare( s2 ) != 0 );
	}

	inline bool operator!=(const LPCTSTR& s1, const CString& str2 )
	{
		return( str2.Compare( s1 ) != 0 );
	}

	inline bool operator!=(const CString& str1, const CString& str2 )
	{
		return( str1.Compare( str2 ) != 0 );
	}
	inline bool operator==(const CString& str1, const CString& str2 )
	{
		return( str1.Compare( str2 ) == 0 );
	}
	
	inline bool operator==(const LPCTSTR& s1, const CString& str2 )
	{
		return( str2.Compare( s1 ) == 0 );
	}

	inline bool operator==(const CString& str1, const LPCTSTR& s2 )
	{
		return( str1.Compare( s2 ) == 0 );
	}

}; //! b_set


#endif //! CSTRING_H
