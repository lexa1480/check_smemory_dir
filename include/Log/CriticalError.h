#pragma once
#ifndef CRITICALERROR_H
#define CRITICALERROR_H

#include "criticallog.h"
#include <sstream>
#include <iomanip>
#include <time.h>

#ifdef  WIN32
	#include <process.h>	//_getpid
#else //WIN32
#endif//WIN32

namespace logger
{

#ifdef  WIN32
#pragma warning( push )
#pragma warning(disable : 4996)
#endif//WIN32

inline std::string GetTimeString( time_t ltime = 0, const char* pFormat = "%Y-%m-%d %H:%M:%S" )
{
	if( 0 == ltime )
		time( &ltime );
	struct tm *ptmToday = gmtime( &ltime );
	char szTime[128] = "";
	strftime( szTime, sizeof( szTime ), pFormat, ptmToday );
	return std::string( szTime );
}

#ifdef  WIN32
#pragma warning( pop )
#endif//WIN32

inline std::string GetThreadIdText( const char* pText = 0 )
{
	std::ostringstream ss; 
#ifdef  WIN32
	ss	<< "PID" << std::setw(5) << std::setfill('0') << ::_getpid()
		<< "TRD" << std::setw(5) << std::setfill('0') << ::GetCurrentThreadId() << "> ";
#else //WIN32
#endif//WIN32
	if( pText )
		ss << pText;
	return ss.str();
}

inline std::string GetCriticalLogPath()
{
	std::string sLogFile;
#ifdef  WIN32
			sLogFile = "c:/CriticalErrors.log";
#else //WIN32
			sLogFile = "/var/log/CriticalErrors.log";
#endif//WIN32
	return sLogFile;
}

inline void LogError( const char* pText, const char* pLogFile = 0, unsigned uLine = 0, const char* pFile = 0, const char* pDate = 0 )
{
	std::ostringstream ss; 
	ss	<< GetTimeString() 
		<< "> " << pText;
	
	if( 0 != pFile )
	{
		ss << "\n" << "\t\tline:" << uLine << " file:" << pFile;
		if( 0 != pDate )
		{
			ss << " " << pDate;
		}
		ss << "\n";
	}

	try
	{
		std::string sLogFile;
		if( 0 != pLogFile )
			sLogFile = pLogFile;
		if( sLogFile.empty() )
		{
			sLogFile = GetCriticalLogPath();
		}
		logger::CCriticalLog().Init( sLogFile ).Put( ss.str() ); 
	}
	catch(...)
	{
	}
}

#define LOG_ERR(x) logger::LogError( logger::GetThreadIdText( x ).c_str(), 0, __LINE__, __FILE__)
//#define LOG_ERR(x) logger::LogError( logger::GetThreadIdText( x ).c_str(), 0, __LINE__, __FILE__, __DATE__ )

///////////////////////////////////////////
// LOG_EXC_TRY
#ifdef  _DEBUG
	#define LOG_EXC_TRY(x)		\
		try						\
		{						\
			x;					\
		}						\
		catch(...)				\
		{						\
			LOG_ERR( "SEH" );	\
			throw;				\
		}						\

#else //_DEBUG
	#define LOG_EXC_TRY(x)		\
		try						\
		{						\
			x;					\
		}						\
		catch(...)				\
		{						\
			LOG_ERR( "SEH" );	\
		}						\

#endif//_DEBUG

}//namespace logger

#endif //!CRITICALERROR_H
