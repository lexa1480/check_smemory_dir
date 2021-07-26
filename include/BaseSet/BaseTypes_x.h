#ifndef _BASE_TYPES_X_H
#define _BASE_TYPES_X_H

#ifndef b_DLL
	#define b_DLL
#endif //!b_DLL

#ifdef WIN32
	#include <WinSock2.h>
	#include <wtypes.h>
	#include <tchar.h>

#ifndef __BSWORDSIZE
	#define __BSWORDSIZE 32
#endif //! __BSWORDSIZE

#else
	#include <stdint.h>

	#ifndef FALSE
		#define FALSE               0
	#endif

	#ifndef TRUE
		#define TRUE                1
	#endif

	typedef int			BOOL,*PBOOL,*LPBOOL;

	typedef short			INT16,SHORT;
	typedef unsigned short		UINT16,USHORT,WORD,*PWORD,*LPWORD;

	typedef int			INT32,INT,*PINT,*LPINT;
	typedef unsigned int 		UINT32,UINT,*PUINT,*LPUINT;

	typedef char 			CHAR;
	typedef unsigned char		BYTE,*PBYTE,*LPBYTE;

	typedef long			LONG,*PLONG,*LPLONG;

	#ifdef  __ASTRA__
		#define __BSWORDSIZE 32
		#include <stdint.h>
		typedef uint32_t DWORD,*PDWORD,*LPDWORD; //mbp
	#else
		#ifdef __WORDSIZE
		#define __BSWORDSIZE __WORDSIZE
		#endif
		typedef unsigned long		DWORD,*PDWORD,*LPDWORD;
	#endif

	typedef unsigned long		ULONG;

	typedef float 			FLOAT,*PFLOAT,*LPFLOAT;

	typedef void			VOID,*LPVOID;
	typedef const void		*LPCVOID;

	typedef int64_t			INT64;
	typedef uint64_t 		UINT64, *PUINT64;


	//! defines for CString
	#ifdef WIN32
		typedef unsigned short 	WCHAR; //! unreachable! Left for understanding
	#else
		typedef wchar_t 	WCHAR, *PWCHAR;
	#endif

	#define __readableTo(extent)
	#define __nullterminated                    __readableTo(sentinel(0))

	typedef __nullterminated CHAR 		*LPSTR, *PSTR;
	typedef __nullterminated const CHAR 	*LPCSTR, *PCSTR;

	typedef __nullterminated WCHAR 		*LPWSTR, *PWSTR;
	typedef __nullterminated const WCHAR 	*LPCWSTR, *PCWSTR;

	#ifdef UNICODE
		typedef LPWSTR 		LPTCH, PTCH;
		typedef LPWSTR 		PTSTR, LPTSTR;
		typedef LPCWSTR 	LPCTSTR;
		typedef wchar_t 	TCHAR, *PTCHAR;
		#define _tprintf	wprintf

		#define __T(x)      L ## x

	#else //! UNICODE not def
		typedef LPSTR 		LPTCH, PTCH;
		typedef LPSTR 		PTSTR, LPTSTR;
		typedef LPCSTR 		LPCTSTR;
		typedef char 		TCHAR, *PTCHAR;
		#define _tprintf	printf

		#define __T(x)      x

	#endif //!UNICODE

	#ifndef _T
	#define _T(x)       __T(x)
	#endif//_T

	#define _TEXT(x)    __T(x)

#endif //!WIN32

#endif //!_BASE_TYPES_X_H
