/******************************************************************************
 File     : Tracer.h
 Author   : SDY
 Date     : 
 Comments : "Tracer.txt"
 
******************************************************************************/

#ifndef _Dbg_Tracer_H
#define _Dbg_Tracer_H

#include "../Array/CString.h"
#include "../Thread/Wait.h"

#include <time.h>
#include <sys/timeb.h>

#ifdef WIN32
	#define FILETIME_TO_UNIXTIME	0x19db1ded53e8000
#else //!WIN32 not define
	#include <errno.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <stdarg.h>
	#define FILETIME_TO_UNIXTIME	0x19db1ded53e8000LL
#endif //!WIN32

namespace b_set
{

#ifndef b_DLL
	#define b_DLL
#endif //!b_DLL

#define DBG_OBJ_COMMON		_T("Common")

#define MASK_CRITIC_ERROR	0x00000001 // TYPE_EXCEPTION, TYPE_ERR_CRITIC, TYPE_MSG_CRITIC

///////////////////
// Types of message
#define TYPE_EXCEPTION		0x00000011 // 
#define TYPE_ERR_LOW		0x00000020 // lowlevel errors, (work with API)
#define TYPE_ERR 			0x00000040 // logic errors and etc.
#define TYPE_ERR_CRITIC		0x00000081 // critical and dangerous error
#define TYPE_WRN 			0x00000100 // warning
#define TYPE_MSG_LIFE 		0x00000200 // messages connected with objects (started, stopped, etc.)
#define TYPE_MSG 			0x00000400 // inessential message
#define TYPE_MSG_CRITIC 	0x00000801 // important message
#define TYPE_MSG_LIFE_THRD 	0x00001000 // thread end message

#define TYPE_MASK_ALL		0xFFFFFFFF // mask is used to allow all messages
#define TYPE_MASK_ERR		0x000000FF // mask is used to allow error messages
#define TYPE_MASK_CRITIC	0x00000001 // mask is used to allow critical message only

///////////////////
// куда обеспечивается вывод сообщений
#define OUT_MASK_ALL 		0xFFFFFFFF
#define OUT_DEBUGGER		0x00000001 // if debugger is on, use OutputDebugString
#define OUT_LOG				0x00000002 // use log



inline CString GetDbgMessageType(DWORD dwType)
{
	CString strRes;

	switch(dwType)
	{
	case TYPE_EXCEPTION:
		strRes = "EXC";
		break;
	case TYPE_ERR_LOW:
		strRes = "ERRL";
		break;
	case TYPE_ERR:
		strRes = "ERR";
		break;
	case TYPE_ERR_CRITIC:
		strRes = "ERRC";
		break;
	case TYPE_WRN:
		strRes = "WRN";
		break;
	case TYPE_MSG_LIFE:
		strRes = "MSGLF";
		break;
	case TYPE_MSG_LIFE_THRD:
		strRes = "MSGLF";
		break;
	case TYPE_MSG:
		strRes = "MSG";
		break;
	case TYPE_MSG_CRITIC:
		strRes = "MSGC";
		break;
	default:
		strRes.Format("$0x%04X", dwType);
	}
	return strRes;
}

class CDbgSubscriber;
class CDbgSubscriberArray;

///////////////////////////////////////////////////////////////////////////////////////////
// CDbgTracer
// Запись в журнал
class b_DLL CDbgTracer
{
public:
	DWORD m_dwMaskTypes;			// mask, types taking part in processing
	DWORD m_dwMaskOut;				// mask, where messages must be sent
	CString m_strAppName;			// application name
	CString m_strLogAppDirName;		// dir name for log files for current application
	CString m_strDefaultFileName;	// default log file name
	CString m_strCriticalFileName;	// log name in which duplicates critical errors
	DWORD m_dwMaxLogSize;
	BOOL m_bMultipleLogFiles;		// Separate messages on the types of objects in different files
	BOOL m_bDuplicateCriticalMsgs;	// Make copy of TYPE_EXCEPTION message in  в individual file (\Exceptions.log)

	// m_strObjShow: If name is set, debug info will be filtered by it's name only (log for this object only)
	// если m_strObjShow - if empty all objects are valid
	CString m_strObjShow; 
protected:
	CCriticalSectionObj m_csShared;
	CDbgSubscriberArray* m_pArrDbgSubscribers;

public:
	CDbgTracer();
	~CDbgTracer();

public:
	void SetAppName(LPCTSTR lpAppName);
	void SetMaskTypes(DWORD dwMaskTypes) // TYPE_MASK_ALL; which types of messages to log; the rest are ignored
	{
		m_dwMaskTypes = dwMaskTypes;
	}
	void Output(LPCTSTR lpObj, DWORD dwMsgType, LPCTSTR lpText,	UINT nLine = NULL, LPCSTR lpFile = NULL, LPCSTR lpDate = NULL);
	BOOL IsLogged(LPCTSTR lpObj, DWORD dwMsgType);
	BOOL IsAppNameSet() const;
	BOOL Subscribe( CDbgSubscriber* pSubscriber );
	void UnSubscribe( CDbgSubscriber* pSubscriber );
protected:
	void InformSubscribers( LPCTSTR lpObjName, DWORD dwMsgType, LPCTSTR lpText );
	void LogMessage(LPCTSTR lpObjName, DWORD dwMsgType, const CString& strMsg);
	CString CombineMessage(LPCTSTR lpObj, DWORD dwMsgType, DWORD dwOut, LPCTSTR lpText) const;
	CString CombineDbgInfo(UINT nLine, LPCSTR lpFile, LPCSTR lpDate) const;
	UINT64 GetLogFileSize(LPCTSTR lpFileName);
	CString GetTimeString() const;
	void WriteFile(const CString& strFileName, const CString& strMsg);
	BOOL PrepareDirectory();
	void Lock();
	void Unlock();
};

extern CDbgTracer b_DLL g_mngDbgTracer;

///////////////////////////////////////////////////////////////////////////////////////////
// Get message text by error code
// GetErrText, GetLastErrText
inline CString GetErrText(DWORD dwError)
{
#ifdef WIN32
	DWORD dwLastError = GetLastError();
	CString strText;

	LPTSTR lpBuffer;
	if( FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpBuffer,
		0,
		NULL)
		)
	{
		strText.Format(_T("[%d] %s"), dwError, lpBuffer);
		int nRes = strText.FindOneOf(_T("\r\n"));
		while(nRes!=-1)
		{
			strText.SetAt(nRes, _T(' '));
			nRes = strText.FindOneOf(_T("\r\n"));
		}

		LocalFree(lpBuffer);
	}
	else
	{
		strText.Format(_T("[%d]"),dwError);
	}
	SetLastError(dwLastError);
	return strText;
#else //!WIN32 not define
	int nLastError = errno;
	CString strText;

	strText.Format(_T("[%d] %hs"), dwError, strerror(dwError));
	int nRes = strText.FindOneOf(_T("\r\n"));
	while(nRes!=-1)
	{
		strText.SetAt(nRes, _T(' '));
		nRes = strText.FindOneOf(_T("\r\n"));
	}
	
	errno = nLastError;
	return strText;
#endif //! WIN32
}

inline CString GetLastErrText()
{
	DWORD dwError = 0;

	#ifdef WIN32
	 dwError = GetLastError();
	#else
	 dwError = errno;
	#endif //! WIN32

	CString strRes;
	#ifdef UNICODE
		strRes.Format("%ls", (LPCTSTR)GetErrText(dwError));
	#else
		strRes.Format("%hs", (LPCTSTR)GetErrText(dwError));
	#endif //!UNICODE

	#ifdef WIN32
	 SetLastError(dwError);
	#else
	 errno = dwError;
	#endif //! WIN32

	return strRes;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Debugging output
// Usage:
// DBG( TYPE_MSG, "Message" );
// DBGLE( TYPE_ERR, FT("Object #%i", nIdx) )
// DBGOBJ( TYPE_MSG, "Object name", "message" ); // use in *.h files only

#define DBG(dwType, lpTxt) DbgOutput(DBG_OBJ_NAME, dwType, __LINE__, __FILE__, __DATE__, lpTxt)
#define DBGLE(dwType, lpTxt) DbgLastError(DBG_OBJ_NAME, dwType, __LINE__, __FILE__, __DATE__, lpTxt)
#define DBGOBJ(dwType, lpObjName, lpTxt) DbgOutput(lpObjName, dwType, __LINE__, __FILE__, __DATE__, lpTxt)

inline void DbgOutput(LPCTSTR lpObj, DWORD dwMsgType, UINT nLine, LPCSTR lpFile, LPCSTR lpDate, LPCTSTR lpText)
//inline void DbgOutput(const char* lpObj, DWORD dwMsgType, UINT nLine, const char* lpFile, const char* lpDate, const char* lpText)
{
	DWORD dwLastError = 0;
	#ifdef WIN32
	 dwLastError = GetLastError();
	 g_mngDbgTracer.Output(lpObj, dwMsgType, lpText, nLine, lpFile, lpDate);
	 SetLastError(dwLastError); // to ignore possible changes LastError
	#else
	 dwLastError = errno;
	 g_mngDbgTracer.Output(lpObj, dwMsgType, lpText, nLine, lpFile, lpDate);
	 errno = dwLastError;
	#endif //! WIN32
}

inline void DbgLastError(LPCTSTR lpObj, DWORD dwMsgType, UINT nLine, LPCSTR lpFile, LPCSTR lpDate, LPCTSTR lpText)
{
	DWORD dwLastError = 0;
	#ifdef WIN32
	 dwLastError = GetLastError();
	#else
	 dwLastError = errno;
	#endif

	CString strErr;
	strErr = lpText;
	strErr += _T("\r\n\t");
	strErr += GetLastErrText();
	strErr += _T("\r\n");
	strErr.Convert();
	g_mngDbgTracer.Output(lpObj, dwMsgType, (LPCTSTR)strErr, nLine, lpFile, lpDate);
	
	#ifdef WIN32
	 SetLastError(dwLastError); // ��� ������������� ���������� ��������� LastError'a
	#else
	 errno = dwLastError;
	#endif
}

///////////////////////////////////////////////////////////////////////////////////////////
// Text formatting
// FT: Text formatting like CString::Format()
inline CString FormatTextV(LPCTSTR lpText, va_list argList)
{
	CString strRes;
	try
	{
		strRes.FormatV(lpText, argList);
	}
	catch(...)
	{
		DbgOutput(DBG_OBJ_COMMON, TYPE_EXCEPTION, __LINE__, __FILE__, __DATE__, _T("FormatTextV")); 
		throw;
	}
	return strRes;
}

inline CString FT(LPCTSTR lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);
	CString ss;
	ss.FormatV(lpText, argList);
	va_end(argList);
	return ss;
}

inline LPTSTR FormatTextSz(LPTSTR lpBuf, DWORD dwBufSize, LPTSTR lpFormat, ...)
{
	va_list argList;
	va_start(argList, lpFormat);
	try
	{
		lpBuf[0] = '\x0';
		#ifdef WIN32
		 #ifdef STRSAFE_CRT
		  _vsntprintf_s(lpBuf, dwBufSize, _TRUNCATE, lpFormat, argList);
		 #else
		  _vsntprintf(lpBuf, dwBufSize, lpFormat, argList);
		 #endif //! STRSAFE_CRT
		#else //! WIN32 not define
		 #ifdef UNICODE
		  vswprintf(lpBuf, dwBufSize, lpFormat, argList);
		 #else //! UNICODE not define
		  vsnprintf(lpBuf, dwBufSize, lpFormat, argList);
		 #endif //! UNICODE
		#endif //! WIN32
	}
	catch(...)
	{
		DbgOutput(DBG_OBJ_COMMON, TYPE_EXCEPTION, __LINE__, __FILE__, __DATE__, _T("FormatTextSz")); 
		throw;
	}
	va_end(argList);
	return lpBuf;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Exception Handling
// EXC_TRY, EXC_IGNORE, EXC_IGNORE_C log the exception
// EXC_IGNORE_C not compile if used in function
// automatically created object with the constructor;
// EXC_IGNORE_C lets see what kind of exception.
#define EXC_TRY(x)	\
{					\
	try{x;}		\
	catch(...){DbgOutput(DBG_OBJ_NAME, TYPE_EXCEPTION, __LINE__, __FILE__, __DATE__, _T("EXC_TRY")); throw;} \
}

#ifdef _DEBUG
	#define EXC_IGNORE(x) EXC_TRY(x)
#else //_DEBUG
	#define EXC_IGNORE(x)	\
	{					\
		try{x;}		\
		catch(...){DbgOutput(DBG_OBJ_NAME, TYPE_EXCEPTION, __LINE__, __FILE__, __DATE__, _T("EXC_IGNORE"));} \
	}
#endif //_DEBUG

#define EXC_IGNORE_C(x)	\
{					\
	TCHAR lpszMsgTMP[MAX_PATH]; \
	__try{x;}		\
	__except( DbgOutput(DBG_OBJ_NAME, TYPE_EXCEPTION, __LINE__, __FILE__, __DATE__, FT( _T("EXC_IGNORE_C code: %u"), GetExceptionCode() ) ), EXCEPTION_EXECUTE_HANDLER){} \
}

///////////////////////////////////////////////////////////////////////////////////////////
// DEBUG_CODE - code compiled only in Debug mode
#ifndef DEBUG_CODE
	#ifdef _DEBUG
		#define DEBUG_CODE(x) x
	#else //_DEBUG
		#define DEBUG_CODE(x)
	#endif//_DEBUG
#endif //DEBUG_CODE


}//!namespace b_set

#endif //! _Dbg_Tracer_H
