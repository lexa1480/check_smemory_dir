/******************************************************************************
 File     : .h
 Author   : SDY
 Date     : 25.10.2006
 Comments : 
 
******************************************************************************/

#ifndef __CmdLine_h
#define __CmdLine_h

#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>

namespace b_set
{
#ifndef WIN32
	#include <ctype.h>
	inline char* _strupr( char* pStr )
	{
		char* p = pStr;
		while( *p != 0 )
		{
			*p = toupper( *p );
			p++;
		}
		return pStr;
	}
#endif

///////////////////////////////////////////////////////////////////////////////
// CCmdLine
class CCmdLine
{
////////// Attributes:
public:
	std::string m_strCmdLine;
protected:
	const char* m_lpString;

////////// Construction:
public:
	CCmdLine( const char* lpCmdLine = 0 )
	{
		if( lpCmdLine )
			m_strCmdLine = lpCmdLine;
		else
		{
			if( m_strCmdLine.empty() )
			{

#ifdef  WIN32
				m_strCmdLine = GetCommandLineA();
#else
				char buf;
				std::ostringstream os;
				std::ifstream ifile("/proc/self/cmdline");
				if (ifile)
				{
					while(true)
					{
						ifile.get(buf);
						if (ifile)
						{
							if (buf=='\0')
								os << ' ';
							else
								os << buf;
						}
						else
							break;
					}
					os << std::ends;
					m_strCmdLine = os.str();
					ifile.close();
				}
#endif//WIN32
			}
		}
		m_lpString = NULL;
	}

////////// Operations:
public:
	std::string GetFirstPart();
	std::string GetNextPart();
	std::string FindByPrefix( const char* lpPrefix ) const;
	bool		FindByPrefix( const char* lpPrefix, std::string& str ) const;
	bool		FindByPrefix( const char* lpPrefix, int& iValue ) const;
	template<class T>
	bool		GetParam( const char* pParamName, T& param ) const;
	bool		FindCommand( const char* lpCommand );
	bool		AddPart( const char* lpPrefix, const char* lpValue = NULL );
	bool		ReplaceByPrefix( const char* lpPrefix, const char* lpReplacement );
	const char*	GetString() const;
	static void	CodeString( std::string& str );
	static void	DecodeString( std::string& str ) ;
protected:
	std::string GetNextAlNumPart( const char* &lpString ) const;

};

///////////////////////////////
// CCmdLine inlines

template<class T>
inline bool		CCmdLine::GetParam( const char* pParamName, T& param ) const
{
	std::string strParam;
	bool bRes = FindByPrefix( pParamName, strParam );
	if( bRes )
	{
		std::istringstream iss( strParam );
		iss >> param;
	}
	return bRes;
}

inline const char* CCmdLine::GetString() const
{
	return m_strCmdLine.c_str();
}

inline void CCmdLine::CodeString( std::string& str )
{
	std::stringstream ss;
	const char* lpPtr = str.c_str();
	while( *lpPtr!='\x0' )
	{
		if( *lpPtr == ' ' )
			ss << "%20";
		else
			ss << *lpPtr;
		lpPtr++;
	}
	str = ss.str();
}

inline void CCmdLine::DecodeString( std::string& str )
{
	std::stringstream ss;
	const char* lpPtr = str.c_str();
	while( *lpPtr!='\x0' )
	{
		if( *lpPtr == '%' )
		{
			if( *(lpPtr+1) == '2' && *(lpPtr+2) == '0' )
			{
				ss << " ";
				lpPtr += 2;
			}
		}
		else
		{
			ss << *lpPtr;
		}
		lpPtr++;
	}
	str = ss.str();
}

inline bool CCmdLine::AddPart( const char* lpPrefix, const char* lpValue )
{
	bool bRes = false;

	if( lpPrefix && lpValue )
		bRes = ReplaceByPrefix( lpPrefix, lpValue );

	if( !bRes )
	{
		bool bPrefixError = false;

		std::string strValue;
		if( lpValue )
			strValue = lpValue;
		std::stringstream ss;
		if( lpPrefix )
		{
			std::string strPrefix = lpPrefix;
			if( !strPrefix.empty() )
			{
				CodeString( strPrefix );
				if( m_strCmdLine.find( strPrefix ) == std::string::npos )
				{
					ss << m_strCmdLine << " " << strPrefix;
					if( !strValue.empty() )
					{
						if( strPrefix[strPrefix.length()-1] != ':' )
							ss << ':';
					}
					bRes = true;
				}
				else
					bPrefixError = true;
			}
		}

		if( !bPrefixError && !strValue.empty() )
		{
			CodeString( strValue );
			if( ss.str().empty() )
				ss << m_strCmdLine << " ";
			ss << strValue;
			bRes = true;
		}

		if( bRes )
			m_strCmdLine = ss.str();

	}

	return bRes;
}

inline bool CCmdLine::ReplaceByPrefix( const char* lpPrefix, const char* lpReplacement )
{
	bool bRes = false;

	std::string strReplacement;
	strReplacement = lpReplacement;
	CodeString( strReplacement );
	std::string strRes;
	if( lpPrefix )
	{
		std::string strPrefix = lpPrefix;
		std::string strCmdLine;
		strCmdLine.append( m_strCmdLine );
#ifdef  STRSAFE_CRT
		_strupr_s( (char*)strCmdLine.c_str(), strCmdLine.size()+1 );
		_strupr_s( (char*)strPrefix.c_str(), strPrefix.size()+1 );
#else //STRSAFE_CRT
#ifdef WIN32
		strupr( (char*)strCmdLine.c_str() );
		strupr( (char*)strPrefix.c_str() );
#else
		_strupr( (char*)strCmdLine.c_str() );
		_strupr( (char*)strPrefix.c_str() );
#endif
#endif//STRSAFE_CRT

#ifdef WINNT40
		unsigned uIdxPrefix = strCmdLine.find( strPrefix );
#else
		size_t uIdxPrefix = strCmdLine.find( strPrefix );
#endif
		if( uIdxPrefix != std::string::npos )
		{
			bRes = true;
			const char* pTmp = m_strCmdLine.c_str() + uIdxPrefix;
			uIdxPrefix += strPrefix.length();
			pTmp = m_strCmdLine.c_str() + uIdxPrefix;
			if( *pTmp == ':' )
				uIdxPrefix++;
			std::stringstream ss;
			ss << m_strCmdLine.substr( 0, uIdxPrefix );
			ss << strReplacement;
			const char* lpPtr = m_strCmdLine.c_str() + uIdxPrefix;
			while( (*lpPtr == ' ' || *lpPtr == ':') && *lpPtr!='\x0' )
				lpPtr++;
			while( (*lpPtr != ' ' ) && *lpPtr != '\x0' )
				lpPtr++;
			if( *lpPtr != '\x0' )
				ss << lpPtr;
			m_strCmdLine = ss.str();
		}
	}

	return bRes;
}

inline std::string CCmdLine::GetNextAlNumPart(const char* &lpString) const
{
	std::string str;
	if(lpString && *lpString!='\x0')
	{
		while( ( *lpString == ' ' || *lpString == ':') && *lpString != '\x0' )
			lpString++;
		int nCntr = 0;
		bool bQuotation = false;
		if( *lpString == '\"' )
		{
			bQuotation = true;
			lpString++;
		}
		str = lpString;
		if( bQuotation )
		{
			while( *lpString != '\"' && *lpString != '\x0' )
			{
				nCntr++;
				lpString++;
			}
			if( *lpString == '\"' )
				lpString++;
		}
		else
		{
			while( *lpString != ' ' && *lpString != '\x0' )
			{
				nCntr++;
				lpString++;
			}
		}
		str.resize( nCntr );
		while( *lpString == ' ' && *lpString != '\x0' )
			lpString++;
	}

	DecodeString( str );

	return str;
}

inline std::string CCmdLine::GetFirstPart()
{
	m_lpString = m_strCmdLine.c_str();
	return GetNextPart();
}

inline std::string CCmdLine::GetNextPart()
{
	const char* lpCmdLine = m_strCmdLine.c_str();
	if( m_lpString == NULL 
		|| ( m_lpString < lpCmdLine )
		|| ( m_lpString > ( lpCmdLine + m_strCmdLine.length() ) )
		)
	{
		m_lpString = lpCmdLine;
	}

	std::string str;
	if(m_lpString && *m_lpString!='\x0')
	{
		while( *m_lpString == ' ' && *m_lpString != '\x0' )
			m_lpString++;
		int nCntr = 0;
		str = m_lpString;
		while( *m_lpString != ' ' && *m_lpString != '\x0' )
		{
			nCntr++;
			m_lpString++;
		}
		str.resize( nCntr );
		while( *m_lpString == ' ' && *m_lpString != '\x0' )
			m_lpString++;
	}

	DecodeString( str );

	return str;
}

inline std::string CCmdLine::FindByPrefix( const char* lpPrefix ) const
{
	std::string strRes;
	if( lpPrefix )
	{
		std::string strPrefix = lpPrefix;
                std::string strCmdLine( m_strCmdLine.c_str() );
#ifdef  STRSAFE_CRT
		_strupr_s( (char*)strCmdLine.c_str(), strCmdLine.size()+1 );
		_strupr_s( (char*)strPrefix.c_str(), strPrefix.size()+1 );
#else //STRSAFE_CRT
#ifdef WIN32
		strupr( (char*)strCmdLine.c_str() );
		strupr( (char*)strPrefix.c_str() );
#else
		_strupr( (char*)strCmdLine.c_str() );
		_strupr( (char*)strPrefix.c_str() );
#endif
#endif//STRSAFE_CRT
		const char* lpPtr = strCmdLine.c_str();

		char* lpResult = (char*)strstr( lpPtr, strPrefix.c_str() );
		if( lpResult!=NULL )
		{
			const char* lpData = m_strCmdLine.c_str() + (lpResult-lpPtr) + strlen(lpPrefix);
			strRes = GetNextAlNumPart(lpData);
		}
	}
	DecodeString( strRes );
	return strRes;
}

inline bool CCmdLine::FindByPrefix( const char* lpPrefix, std::string& str ) const
{
	std::string strResByPrefix = FindByPrefix( lpPrefix );
	if( !strResByPrefix.empty() )
	{
		str = strResByPrefix;
		return true;
	}
	else
		return false;

}

inline bool CCmdLine::FindByPrefix( const char* lpPrefix, int& iValue ) const
{
	std::string strResByPrefix = FindByPrefix( lpPrefix );
	if( !strResByPrefix.empty() )
	{
		iValue = atoi( strResByPrefix.c_str() );
		return true;
	}
	else
		return false;

}

inline bool CCmdLine::FindCommand( const char* lpCommand )
{
	bool bRes = false;
	if(lpCommand)
	{
		std::string strCommand = lpCommand;
                std::string strCmdLine = m_strCmdLine.c_str();
#ifdef  STRSAFE_CRT
		_strupr_s( (char*)strCmdLine.c_str(), strCmdLine.size()+1 );
		_strupr_s( (char*)strCommand.c_str(), strCommand.size()+1 );
#else //STRSAFE_CRT
#ifdef WIN32
		strupr( (char*)strCmdLine.c_str() );
		strupr( (char*)strCommand.c_str() );
#else
		_strupr( (char*)strCmdLine.c_str() );
		_strupr( (char*)strCommand.c_str() );
#endif
#endif//STRSAFE_CRT

		const char* lpPtr = strCmdLine.c_str();
		char* lpResult = (char*)strstr( lpPtr, strCommand.c_str() );
		bRes = ( lpResult!=NULL );
	}
	return bRes;
}

} //! namespace b_set

#endif //! __CmdLine_h
