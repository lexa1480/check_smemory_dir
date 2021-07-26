/******************************************************************************
 File     : BaseObject.cpp
 Author   : SDY
 Date     : 15.12.2001
 Comments : 
 
******************************************************************************/

#ifndef __BaseObject_h
#define __BaseObject_h

#include "../Array/CString.h"

#ifndef WIN32
	#include <errno.h>
	#include <unistd.h>
#endif //! WIN32

namespace b_set
{
///////////////////////////////////////////////////////////////////////////////
// CBaseObj
class b_DLL CBaseObj
{
protected:
	CString m_strObjectName;

public:
	CBaseObj(LPCTSTR lpObjectName = NULL){
		if (lpObjectName)
			SetObjectName(lpObjectName);
	}
	virtual ~CBaseObj(){};

public:
	void SetObjectName(LPCTSTR lpObjectName);
	CString GetObjectName() const;

	bool IsObjectNameEmpty() const;
	bool IsValidObject() const;

	void CopyBaseObjectFrom(const CBaseObj& bo);

	unsigned long GetLastError() const;
	void Sleep( unsigned long dwMillisec ) const;
};

//////////////////////
// CBaseObj inlines
inline void CBaseObj::SetObjectName(LPCTSTR lpObjectName)
{
	m_strObjectName = lpObjectName;
}

inline CString CBaseObj::GetObjectName() const
{
	return m_strObjectName;
}

inline void CBaseObj::CopyBaseObjectFrom(const CBaseObj& bo)
{
	m_strObjectName = bo.m_strObjectName;
}

inline bool CBaseObj::IsObjectNameEmpty() const
{
	return m_strObjectName.IsEmpty();
}

inline bool CBaseObj::IsValidObject() const
{
	bool bValidObject = false;
	try
	{
		if ( this != NULL )
		{
			bValidObject = true;
		}
	}
	catch(...)
	{
	}
	return bValidObject;
}

inline unsigned long CBaseObj::GetLastError() const
{
#ifdef WIN32
	return ::GetLastError();
#else
	return errno;
#endif //! WIN32
}

inline void CBaseObj::Sleep(unsigned long dwMillsec) const
{
#ifdef WIN32
	::Sleep(dwMillsec);
#else
	::usleep(dwMillsec*1000);
#endif //! WIN32
}
}//! namespace b_set
#endif //! __BaseObject_h

