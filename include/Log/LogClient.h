#pragma once

#include <BaseSet/LibraryBase.h>
#include <string>
#include "ilogchannel.h"

#ifndef CALLBACK
#define CALLBACK 
#endif

namespace logger
{

	class CLogClient : protected b_set::CLibraryBase
	{
	public:
		CLogClient() : m_pInterface(0) {};
		virtual ~CLogClient() {FreeLibrary();};

		bool IsLibraryLoaded();
		bool LoadLibrary();
		void FreeLibrary();

		bool Init(const char* pszFileName, const char* pszRegistryShift = "");
		bool SaveParams(const char* pszFileName);

		ILogChannel* OpenChannel(const char* pszName);
		ILogChannel* CreateChannel(const char* pszChannelType);

	//	void AddRef();
	//	void Release();

		/// Initialization by using NITAROOT environment variable.
		bool InitByNita(const char* pszFileName, const char* pszRegistryShift = "");
        bool InitDefault(void);

        ILogMaker* GetLogMaker();//Internal function. Please do not use

	private:
		ILogMaker* m_pInterface;
		typedef void* (CALLBACK* LPFUNC_GetInterface)();
	};

	inline bool CLogClient::IsLibraryLoaded()
	{
		return CLibraryBase::IsLibraryLoaded();
	}
	inline bool CLogClient::LoadLibrary()
	{
	    #ifdef WIN32
            #ifdef  _DEBUG
            m_strLibraryFileName = "Log_d.dll";
            #else
            m_strLibraryFileName = "Log.dll";
            #endif //_DEBUG
		#else  //WIN32
            #ifdef  _DEBUG
            m_strLibraryFileName = "liblog_d.so";
            #else
            m_strLibraryFileName = "liblog.so";
            #endif //_DEBUG
		#endif //WIN32

		bool bRes = CLibraryBase::LoadLibrary();

		if( m_hLibrary )
		{
			if( !m_pInterface )
			{
				LPFUNC_GetInterface func 
					= (LPFUNC_GetInterface)GetProcAddress("getLogMaker");
				if (func)
				{
					m_pInterface = (ILogMaker*)func();
					if (m_pInterface)
					{
						m_pInterface->AddRef();
						bRes = true;
					}
				}
				else return false;
			}
		}
		return bRes;
	}

	inline void CLogClient::FreeLibrary()
	{
		if (m_pInterface)
		{
			m_pInterface->Release();
			CLibraryBase::FreeLibrary();
			m_pInterface = 0;
		}
	}

    inline ILogMaker* CLogClient::GetLogMaker()
	{
		return m_pInterface;
	}


	inline bool CLogClient::Init(const char* pszFileName, const char* pszRegistryShift)
	{
		if (m_pInterface) return m_pInterface->Init(pszFileName, pszRegistryShift);
		else return false;
	}
	inline bool CLogClient::SaveParams(const char* pszFileName)
	{
		if (m_pInterface) return m_pInterface->SaveParams(pszFileName);
		else return false;
	}

	inline ILogChannel* CLogClient::OpenChannel(const char* pszName)
	{
		if (m_pInterface) return m_pInterface->OpenChannel(pszName);
		else return 0;
	}
	inline ILogChannel* CLogClient::CreateChannel(const char* pszChannelType)
	{
		if (m_pInterface) return m_pInterface->CreateChannel(pszChannelType);
		else return 0;
	}

	/// Initialization by using NITAROOT environment variable.
	inline bool CLogClient::InitByNita(const char* pszFileName, const char* pszRegistryShift)
	{
		if (m_pInterface) return m_pInterface->InitByNita(pszFileName, pszRegistryShift);
		else return false;
	}
	inline bool CLogClient::InitDefault(void)
	{
		if (m_pInterface) return m_pInterface->InitDefault();
		else return false;
	}


/*	class CLogClient
	{
	public:
		////////// Construction:
		CLogClient();
		~CLogClient();
		////////// Operations:
	public:
		bool Load();		// Load library //& initialization from fileconst char* pszFileName
		void Free();		// Free library
		bool IsOpen() const { return (m_pInterface != NULL); }

		ILogMaker* GetLogMaker() { return m_pInterface; }

		////////// Attributes:
	protected:
		HINSTANCE m_hLibrary;
		ILogMaker* m_pInterface;
		std::string m_strLibraryFileName;
		typedef void*	(CALLBACK* LPFUNC_GetInterface)();
	};


	inline CLogClient::CLogClient()
	{
		m_hLibrary = NULL;
		m_pInterface = NULL;
		#ifdef  _DEBUG
		m_strLibraryFileName = "Log_d.dll";
//		m_strLibraryFileName = "log-vc80-mt-gd-1_00.dll";
		#else
		m_strLibraryFileName = "Log.dll";
		#endif//_DEBUG
	}
	inline CLogClient::~CLogClient()
	{
		Free();
	}
	inline bool CLogClient::Load()
	{
		if( m_pInterface ) return true;

		bool bRes(false);

		if( !m_hLibrary )
			m_hLibrary = ::LoadLibrary( m_strLibraryFileName.c_str() );

		if( m_hLibrary )
		{
			if( !m_pInterface )
			{
				LPFUNC_GetInterface func 
					= (LPFUNC_GetInterface)(FARPROC) ::GetProcAddress( m_hLibrary, "getLogMaker" );
				if( func )
				{
					m_pInterface = (ILogMaker*)func();
					if (m_pInterface) 
					{
						m_pInterface->AddRef();
						bRes = true;
					}
				}
			}
		}
		return bRes;
	}

	inline void CLogClient::Free()
	{
		if( m_hLibrary )
		{
			if (m_pInterface) m_pInterface->Release();
			::FreeLibrary( m_hLibrary );
			m_pInterface = NULL;
			m_hLibrary = NULL;
		}
	}*/

}//namespace logger

