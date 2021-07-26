/******************************************************************************
 File     : DataLinker.h
 Author   : SDY
 Date     : 06.06.2006
 Comments : 
 
******************************************************************************/

#ifndef __DataLinker_h
#define __DataLinker_h

#include <string>

namespace b_set
{

///////////////////////////////////////////////////////////////////////////////
// link flags
#define LF_SHOW		0x0001		// show param on request
#define LF_DEBUG	0x0002		// show debug type param
#define LF_STREAM	0x0004		// param is data stream

class CDataStorageBase;

///////////////////////////////////////////////////////////////////////////////
// CLinkExchange
class b_DLL CLinkExchange
{
protected:
	enum ENU_MODE
	{
		MODE_NONE, 
		MODE_COLLECT, 
		MODE_RESTORE
	}m_enuMode;
	DWORD m_dwFlagsRequest; // LF_SHOW, ...

public:
	CLinkExchange(ENU_MODE enuMode = MODE_NONE, DWORD dwFlags=0)
	{
		m_enuMode = enuMode;
		m_dwFlagsRequest = dwFlags;
	}

public:
	BOOL IsModeCollect() const
		{return m_enuMode == MODE_COLLECT;}
	BOOL IsModeRestore() const
		{return m_enuMode == MODE_RESTORE;}
};

///////////////////////////////////////////////////////////////////////////////
// CDataLinkBaseObj
class b_DLL CDataLinkBaseObj
{
public:
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, CDataStorageBase* /*pDs*/, DWORD /*dwFlags*/=0 ){}
	virtual void UpdateLink( CLinkExchange* /*pLe*/, CDataStorageBase* /*pDs*/ ){}
	virtual LPCTSTR GetName() const
		{return NULL;}
	virtual int GetSubItemsCount() const
		{return 0;}
	virtual CDataLinkBaseObj* GetSubItem(int /*iIdx*/) const
		{return NULL;}
	virtual bool HasTableStyleItems() const
		{return false;}
};

///////////////////////////////////////////////////////////////////////////////
// CDataStorageBase
class b_DLL CDataStorageBase
{
public:
	// dwFlags: LF_SHOW, LF_DEBUG, ...
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, BYTE&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, WORD&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, SHORT&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, DWORD&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, UINT&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, INT& 	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, UINT64&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, INT64&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, float&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, double&	/*param*/, DWORD /*dwFlags*/=0 ){}
	virtual void Link( CLinkExchange* /*pLe*/, LPCTSTR /*lpName*/, std::string& /*str*/, DWORD /*dwFlags*/=0 ){}
	
	virtual void Link( CLinkExchange* pLe, LPCTSTR lpName, CDataLinkBaseObj* lpData, DWORD dwFlags=0 )
	{
		lpData->Link( pLe, lpName, this, dwFlags );
	}
};

} //!namespace b_set

#endif // __DataLinker_h
