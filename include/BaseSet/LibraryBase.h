/******************************************************************************
 File     : LibraryBase.h
 Author   : Petrov Dima
 Date     : 27.06.2007
 Comments : Windows and Linux
 
******************************************************************************/
#ifndef __CLIBRARYBASE_H__
#define __CLIBRARYBASE_H__

#ifdef  WIN32
	#include <winsock2.h>
#else //WIN32
	#include <dlfcn.h>
	#define CALLBACK
#endif//WIN32

#include <stdio.h>
#include <string>
#include <sstream>
#include <Log/CriticalError.h>

namespace b_set
{

//////////////////////////////////////////////////////////////////////////
// CLibraryBase
//////////////////////////////////////////////////////////////////////////
class CLibraryBase
{
	typedef void* (CALLBACK* LPFUNC_GetInterfaceProc)();
//////////////////////////
	#ifndef LIB_HANDLE
		#ifdef  WIN32
			typedef HINSTANCE	LIB_HANDLE;
		#else //!WIN32
			typedef void*		LIB_HANDLE;	
		#endif //!WIN32
	#endif //!LIB_HANDLE

//////////////////////////
protected:
	LIB_HANDLE 	m_hLibrary;
	std::string m_strLibraryFileName;
	std::string m_strLastError;

/////////////////////////
public:
	CLibraryBase()
	{
		m_hLibrary = 0L;
		m_strLibraryFileName.erase();
		m_strLastError.erase();
	}
	~CLibraryBase()
	{
		FreeLibrary();
	}

/////////////////////////
public:
	bool		IsLibraryLoaded() const;
	bool		LoadLibrary();
	bool		FreeLibrary();
	bool		DetachLibrary();
	bool		CheckProcAddress( const char* pProcName ) const; 	//! checks: does proc name exist?
	void*		GetProcAddress( const char* pProcName ); 	//! proc address
	void*		GetInterface( const char* pProcName ); 		//! pointer to interface
	LIB_HANDLE	GetLibHandle(); 							//! library handle
	void		SetLibraryName( const char* pLibraryID ); 	//! sets library file name by library ID
	const char* GetErrorText();
	const char* GetLibraryFileName() const;
        bool		LoadLibraryWithoutLogging();
protected:
	void		TraceLastError( const char* pFuncName );
private:
	void		GetLastErrorText( std::string& strRes );
};

//////////////////////////////////////////////////////////////////////////
// CLibraryBase in lines
//////////////////////////////////////////////////////////////////////////
inline void* CLibraryBase::GetInterface( const char* pProcName )
{
	void* pInterface = NULL;
	
	if( IsLibraryLoaded() )
	{
		LPFUNC_GetInterfaceProc func = (LPFUNC_GetInterfaceProc)GetProcAddress( pProcName );
		if( func )
		{
			pInterface = func();
		}
	}
	
	return pInterface;
}

inline bool	CLibraryBase::CheckProcAddress( const char* pProcName ) const
{
	if ( !m_hLibrary )
		return 0L;

	void* ptrFunction = 0L;

	if ( pProcName != 0 )
	{
		#ifdef  WIN32
			{
				ptrFunction = ::GetProcAddress( m_hLibrary, pProcName );
			}
		#else //!WIN32
			{
				ptrFunction = dlsym( m_hLibrary, pProcName );
			}
		#endif //!WIN32
	}

	return 0L != ptrFunction;
}

inline bool CLibraryBase::IsLibraryLoaded() const
{
	return ( m_hLibrary != 0L );
}

inline bool CLibraryBase::LoadLibrary()
{
	if ( m_hLibrary )
		return true;

        if ( m_strLibraryFileName.empty() ) 
			return false;
		
	#ifdef  WIN32
		{
			m_hLibrary = ::LoadLibraryA( m_strLibraryFileName.c_str() );
		}
	#else //!WIN32
		{
        	m_hLibrary = dlopen( m_strLibraryFileName.c_str(), RTLD_NOW );
		}
	#endif //!WIN32

	if ( !m_hLibrary )
		TraceLastError( "LoadLibrary" );
	else
		m_strLastError.erase();

	return ( m_hLibrary != 0 );
}


inline bool CLibraryBase::LoadLibraryWithoutLogging()
{
	if ( m_hLibrary )
		return true;

        if ( m_strLibraryFileName.empty() ) 
			return false;
		
	#ifdef  WIN32
		{
			m_hLibrary = ::LoadLibraryA( m_strLibraryFileName.c_str() );
		}
	#else //!WIN32
		{
        	m_hLibrary = dlopen( m_strLibraryFileName.c_str(), RTLD_NOW );
		}
	#endif //!WIN32

	if ( m_hLibrary )
		m_strLastError.erase();

	return ( m_hLibrary != 0 );
}



inline bool CLibraryBase::FreeLibrary()
{
	if ( !m_hLibrary )
		return true;
	
	bool bRes = true;

	#ifdef  WIN32 
		{
			if ( !::FreeLibrary( m_hLibrary ) )
				bRes = false;
		}
	#else //!WIN32
		{
			int nError = dlclose( m_hLibrary );
			if ( nError )
				bRes = false;
		}
	#endif //!WIN32
	
	if( !bRes )
		TraceLastError( "FreeLibrary" );
	else
		m_strLastError.erase();

	m_hLibrary = 0L;
	
	return bRes;
}

inline bool CLibraryBase::DetachLibrary()
{
	if ( !m_hLibrary )
		return true;
	
//	bool bRes = true;

	m_strLastError.erase();

	m_hLibrary = 0L;
	
	return true;
}

inline void* CLibraryBase::GetProcAddress( const char* pProcName )
{
	if ( !m_hLibrary )
		return 0L;

	void* ptrFunction = 0L;

	if ( pProcName != 0 )
	{
		#ifdef  WIN32
			{
				ptrFunction = ::GetProcAddress( m_hLibrary, pProcName );
			}
		#else //!WIN32
			{
				ptrFunction = dlsym( m_hLibrary, pProcName );
			}
		#endif //!WIN32
		if ( !ptrFunction )
			TraceLastError( "GetProcAddress" );
	}

	return ptrFunction;
}

inline void CLibraryBase::TraceLastError( const char* pFuncName )
{
	GetLastErrorText( m_strLastError );
	printf( "%s> ERROR: \"%s\"\n\t%s\n", m_strLibraryFileName.c_str(), pFuncName, m_strLastError.c_str() );

	std::stringstream ss;
	ss << logger::GetThreadIdText() << "ERR> " << pFuncName << ">"  << m_strLibraryFileName << "> " << m_strLastError;
	logger::LogError( ss.str().c_str() );
}

inline void CLibraryBase::GetLastErrorText( std::string& strRes )
{
	#ifdef WIN32
		{
			unsigned long dwLastError = ::GetLastError();
			LPTSTR lpBuffer;

			if( ::FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
				NULL, 
				dwLastError, 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR)&lpBuffer, 
				0, 
				NULL)
				)
			{
				std::stringstream ss;
				ss << "[" << dwLastError << "] " << lpBuffer;
				strRes = ss.str();
				LocalFree(lpBuffer);
			}
			else
			{
				std::stringstream ss;
				ss << "[" << dwLastError << "] ";
				strRes = ss.str();
			}

			::SetLastError( dwLastError );
		}
	#else  //!WIN32
		{
			strRes = dlerror();
		}	
	#endif //!WIN32
}

inline CLibraryBase::LIB_HANDLE CLibraryBase::GetLibHandle()
{
	return m_hLibrary ? m_hLibrary : NULL;
}

inline void	CLibraryBase::SetLibraryName( const char* pLibraryID )
{
	std::string strFileName = pLibraryID;
#ifdef  _DEBUG
	strFileName += "_d";
#endif//_DEBUG

#ifdef  WIN32
	strFileName += ".dll";
#else //!WIN32
	if (strFileName.find("lib")!=0)
	{
		std::stringstream ss;
		ss<<"lib"<<strFileName<<".so";
		strFileName = ss.str();
	}
	else
		strFileName += ".so";
#endif //!WIN32
	m_strLibraryFileName = strFileName;
}

inline const char* CLibraryBase::GetErrorText()
{
	return m_strLastError.c_str();
}

inline const char* CLibraryBase::GetLibraryFileName() const
{
	return m_strLibraryFileName.c_str();
}

}; //!namespace b_set

#endif //! __CLIBRARYBASE_H__


