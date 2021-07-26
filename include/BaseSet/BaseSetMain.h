/******************************************************************************
 File     : BaseSetMain.h
 Author   : SDY
 Date     : 28.04.2005
 Comments : 
 
******************************************************************************/

#ifndef __BaseSetMain_h
#define __BaseSetMain_h

#include "Thread/Wait.h"
#include <string>

#ifndef WIN32
	typedef int* HWND;
	typedef void* HMODULE;
#endif

namespace b_set
{

///////////////////////////////////////////////////////////////////////////////
// CBaseSetMain
class b_DLL CBaseSetMain
{
////////// Attributes:
public:
protected:
	static CBaseSetMain*		m_pBaseSetMain;
	static CCriticalSectionObj 	m_csLock;
	static std::string 		m_strModules;
	static bool 			m_bOpen;

////////// Construction:
public:
	CBaseSetMain();
	virtual ~CBaseSetMain();

////////// Operations:
public:
	static CBaseSetMain* GetObj();
	bool IsAppNameSet() const;
	void SetAppName( LPCTSTR lpAppName);
	void SetLanguage(const char* pLanguage); 	//! ENU_ML
	void SetLanguage(int iLanguage); 		//! ENU_ML
	int& GetLanguage(); 				//! ENU_ML

	static std::string GetAppName();
	virtual bool Open();
	virtual void Close();
	
	virtual void StartDlgThreads( HWND /*hWndParent*/ ) {};
	virtual void StartDlgAppTimer( HWND /*hWndParent*/ ) {};

	static void AddModule( HMODULE hModule );
	static void AddModuleName( const char* lpModuleName );
	

	static const char* GetModules();

	static bool IsOpen()
	{
		return m_bOpen;
	}

};

}//! namespace b_set

#endif //! __BaseSetMain_h
