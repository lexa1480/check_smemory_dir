/******************************************************************************
 File     : LogFile.h
 Author   : SDY
 Date     : 19.05.2003
 Comments : 
 
******************************************************************************/

#ifndef __LogFileObj_h
#define __LogFileObj_h

#include <fstream>

#ifndef WIN32
 #include<sys/stat.h>
 #include<errno.h>
#endif

#include "../Thread/Wait.h"

namespace b_set
{
	///////////////////////////////////////////////////////////////////////////////
	// CLogFileObj
	class b_DLL CLogFileObj
	{
	////////// Attributes:
	public:
	protected:
		CCriticalSectionObj			m_csShared;
		std::fstream				m_foFile;
		std::string				m_strPath;
		unsigned long				m_dwMaxLogSize;
		static bool				m_bEnableLogging;

	////////// Construction:
	public:
		CLogFileObj(const char* lpFullFileName = NULL);
		virtual ~CLogFileObj();

	////////// Operations:
	public:
		void LogTime( const char* lpMessage);
		void LogText( const char* lpMessage);
		void SetFileName( const char* lpFullFileName);
		void SetFileName( const char* lpPath, const char* lpFileName);
		void SetFileName( const char* lpPath, const char* lpFileName, const char* plExtension );
		void SetMaxLogSize( unsigned long dwMaxLogSize);

		std::string ChangeExtension( std::string strFileName, std::string strExtension );
		bool CreateDirectories( std::string strFilePath );
//		int GetFileSize(std::string strFilePath);
		bool CheckDir(std::string strPath);
		bool MakeDir(const char* lpPath);

		static void EnableLoggingForAllLogs( bool bEnable );
		static bool IsLoggingEnabled();
		static std::string GetTimeString();
	protected:
		void Lock();
		void Unlock();
		bool Write( const char* lpMessage);
	};

	///////////////////////////////
	// CLogFileObj inlines
	inline void CLogFileObj::SetFileName( const char* lpFullFileName )
	{
		if ( lpFullFileName )
		{
			m_strPath = lpFullFileName;
			int i = m_strPath.find("\\");
			while (i >= 0) {
				m_strPath.replace(i,1,"/");
				i = m_strPath.find("\\");
			}
		}
		else
			m_strPath = "";
	}
	inline void CLogFileObj::SetMaxLogSize( unsigned long dwMaxLogSize)
	{
		m_dwMaxLogSize = dwMaxLogSize;
	}

	inline std::string CLogFileObj::ChangeExtension( std::string strFileName, std::string strExtension )
	{
		if ( !strExtension.empty() )
		{
			int nPosPoint = strFileName.find(".");
			if ( nPosPoint > 0 )
			{
				strFileName[nPosPoint] = 0;
				strFileName.insert(nPosPoint,strExtension.c_str(),strExtension.length());
			}	
			else
				strFileName += strExtension;
			return strFileName;
		}
		else
			return strFileName;
	}

/*
	inline bool CLogFileObj::CreateDirectories( std::string strFilePath )
	{
		if (!strFilePath.empty())
		{
			int i = strFilePath.find("\\");
			while (i >= 0) {
				strFilePath.replace(i,1,"/");
				i = strFilePath.find("\\");
			}
			//! remove leaf
			i = strFilePath.rfind("/");
			strFilePath[i] = 0;
			//! Command line
			strFilePath.insert(0,"mkdir -p ", 9 );
	
			return system(strFilePath.c_str()) >=0;
		}
		else
			return false;
	}

*/
	inline bool CLogFileObj::CreateDirectories(std::string strPath)
	{
		int nPos = strPath.rfind(".");
		if ( nPos >= 0 )
		{
			nPos = strPath.rfind("/");
			strPath[nPos] = 0;
		}

		std::string strBuff;
		nPos = strPath.find("/",1);
		while (nPos >= 0)
		{
			strBuff = strPath;
			strBuff[nPos+1] = 0;
			MakeDir(strBuff.c_str());
			nPos = strPath.find("/",nPos+1);
		}
		return MakeDir(strPath.c_str());
	}

	inline bool CLogFileObj::MakeDir( const char* lpPath )
	{
#ifdef WIN32
		return CreateDirectoryA(lpPath,NULL) > 0;
#else
		return mkdir(lpPath,509) >= 0;
#endif //! WIN32
		
	}

	inline bool CLogFileObj::CheckDir( std::string strPath )
	{
		int nError = 0;
		std::fstream file;
#ifdef WIN32
		file.open(strPath.c_str(),std::fstream::app);
		if (!file.is_open())
		{
			if (::GetLastError() == ERROR_PATH_NOT_FOUND)
			    return false;
			else
			    return true;
		}
		else
			return true;
#else
		int nPos = strPath.find(".");
		if ( nPos >= 0 )
		    strPath[strPath.rfind("/")] = 0;

		file.open(strPath.c_str());
		nError = errno;
		if (nError == EISDIR)
		    return true;
		else
		    return false;
#endif
	}
/*
	inline int CLogFileObj::GetFileSize( std::string strFilePath )
	{
		std::fstream fFile;
		fFile.open(strFilePath.c_str(),std::fstream::in|std::fstream::ate);
		if (fFile.is_open()) 
			return fFile.tellg();
		else
			return -1;
	}
*/
} //! namespace b_set

#endif //! __LogFileObj_h
