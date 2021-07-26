/******************************************************************************
 File     : ByteStream.h
 Author   : SDY
 Date     : 11.08.2004
 Comments : 
 
******************************************************************************/

#ifndef __ByteStream_h
#define __ByteStream_h

#include <vector>
#include <string>

#include "../BaseTypes_x.h"

namespace b_set
{

class CByteStreamBase;

///////////////////////////////////////////////////////////////////////////////
// CSerializeBaseObj
class b_DLL CSerializeObjBase
{
public:
	CSerializeObjBase(){};
	virtual ~CSerializeObjBase(){};
public:
	virtual BOOL Serialize(CByteStreamBase* pBs) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// CByteStreamBase
class b_DLL CByteStreamBase
{
protected:
	enum{MODE_NONE, MODE_COLLECT, MODE_RESTORE}	m_enuMode;

public:
	CByteStreamBase(){m_enuMode = MODE_NONE;};
	virtual ~CByteStreamBase(){};

public:
	BOOL IsModeCollect() const
		{return m_enuMode == MODE_COLLECT;}
	BOOL IsModeRestore() const
		{return m_enuMode == MODE_RESTORE;}
	virtual DWORD  GetPtrSize() const = 0;
	virtual LPVOID GetPtr() = 0;
	virtual BOOL Serialize(LPVOID	lpParam, DWORD dwSize) = 0;
	virtual BOOL Serialize(BYTE&	param) = 0;
	virtual BOOL Serialize(WORD&	param) = 0;
	virtual BOOL Serialize(SHORT&	param) = 0;
#ifndef  __ASTRA__
	virtual BOOL Serialize(DWORD&	param) = 0;
#endif
	virtual BOOL Serialize(UINT&	param) = 0;
	virtual BOOL Serialize(INT&	param) = 0;
#if __BSWORDSIZE == 32
	virtual BOOL Serialize(UINT64&	param) = 0;
	virtual BOOL Serialize(INT64&	param) = 0;
#endif //! __BSWORDSIZE
	virtual BOOL Serialize(float&	param) = 0;
	virtual BOOL Serialize(double&	param) = 0;
	virtual BOOL Serialize(std::string& str) = 0;
	
	virtual BOOL Serialize(CSerializeObjBase* pSo)
	{
		return pSo->Serialize(this);
	}

	virtual BOOL PushFront(LPVOID /*lpParam*/, DWORD /*dwSize*/)
		{return true;}
	virtual BOOL PushFront(DWORD /*param*/)
		{return true;}
};

///////////////////////////////////////////////////////////////////////////////
// CByteStreamCollect
class b_DLL CByteStreamCollect : public CByteStreamBase
{
////////// Attributes:
public:
protected:
	std::vector<BYTE> m_vBytes;

////////// Construction:
public:
	CByteStreamCollect(){m_enuMode = MODE_COLLECT;};
	virtual ~CByteStreamCollect(){};

////////// Operations:
public:
	BOOL PushFront(LPVOID	lpParam, DWORD dwSize);
	BOOL PushFront(DWORD	param);
	void Clear();
	DWORD  GetPtrSize() const;
	LPVOID GetPtr();
	// overwritten
	BOOL Serialize( LPVOID	lpParam, DWORD dwSize );
	BOOL Serialize( BYTE&	param );
	BOOL Serialize( WORD&	param );
	BOOL Serialize( SHORT&	param );
#ifndef  __ASTRA__
	BOOL Serialize( DWORD&	param );
#endif
	BOOL Serialize( UINT&	param );
	BOOL Serialize( INT&	param );
#if __BSWORDSIZE == 32
	BOOL Serialize( UINT64&	param );
	BOOL Serialize( INT64&	param );
#endif //! __BSWORDSIZE
	BOOL Serialize( float&	param );
	BOOL Serialize( double&	param );
	BOOL Serialize( std::string& str );
	BOOL Serialize( CSerializeObjBase* pSo );
protected:

};

///////////////////////////////
// CByteStreamCollect inlines

inline void CByteStreamCollect::Clear()
{
	m_vBytes.clear();
}
inline DWORD CByteStreamCollect::GetPtrSize() const
{
	return (DWORD)m_vBytes.size();
}
inline LPVOID CByteStreamCollect::GetPtr()
{
	return &m_vBytes.front();
}
inline BOOL CByteStreamCollect::PushFront( LPVOID lpParam, DWORD dwSize )
{
	std::vector<BYTE> vRes;
	DWORD i;
	for( i=0; i<dwSize; i++ )
	{
		BYTE by = *(((const LPBYTE)lpParam)+i);
		vRes.push_back(by);
	}
	m_vBytes.insert( m_vBytes.begin(), vRes.begin(), vRes.end() );

	return true;
}
inline BOOL CByteStreamCollect::PushFront(DWORD param)
{
	return PushFront( &param, sizeof(param) );
}
inline BOOL CByteStreamCollect::Serialize(CSerializeObjBase*	pSo)
{
	return CByteStreamBase::Serialize(pSo);
}
inline BOOL CByteStreamCollect::Serialize(LPVOID lpParam, DWORD dwSize)
{
	if(lpParam == NULL)
		return FALSE;
	if(dwSize)
	{
		DWORD i=0;
		for(; i<dwSize; i++)
			m_vBytes.push_back( ((LPBYTE)lpParam)[i] );
	}
	return true;
}
inline BOOL CByteStreamCollect::Serialize(BYTE& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamCollect::Serialize(WORD& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamCollect::Serialize(SHORT& param)
{
	return Serialize( &param, sizeof(param) );
}
#ifndef  __ASTRA__
inline BOOL CByteStreamCollect::Serialize(DWORD& param)
{
	return Serialize( &param, sizeof(param) );
}
#endif
inline BOOL CByteStreamCollect::Serialize(UINT& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamCollect::Serialize(INT& param)
{
	return Serialize( &param, sizeof(param) );
}
#if __BSWORDSIZE == 32
inline BOOL CByteStreamCollect::Serialize(UINT64& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamCollect::Serialize(INT64& param)
{
	return Serialize( &param, sizeof(param) );
}
#endif //! __BSWORDSIZE
inline BOOL CByteStreamCollect::Serialize(float& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamCollect::Serialize(double& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamCollect::Serialize(std::string& str)
{
	BOOL bRes = true;
	WORD wSize = (WORD)str.size();
	Serialize(wSize);
	if(str.size())
		bRes = Serialize( &str.at(0), str.size() );
	return bRes;
}

///////////////////////////////////////////////////////////////////////////////
// CByteStreamRestore
class b_DLL CByteStreamRestore : public CByteStreamBase
{
////////// Attributes:
public:
protected:
	DWORD	m_dwPtrSize;
	LPBYTE	m_lpPtr;

////////// Construction:
public:
	CByteStreamRestore()
	{
		m_enuMode = MODE_RESTORE;
		m_dwPtrSize = 0;
		m_lpPtr = NULL;
	}
	virtual ~CByteStreamRestore(){}

////////// Operations:
public:
	void	SetPtr( LPVOID	lpPtr, DWORD dwPtrSize);
	LPVOID	GetPtr();
	DWORD	GetPtrSize() const;

	BOOL	Serialize( LPVOID	lpParam, DWORD dwParamSize);
	BOOL	Serialize( BYTE&	param);
	BOOL	Serialize( WORD&	param);
	BOOL	Serialize( SHORT&	param);
#ifndef  __ASTRA__
	BOOL	Serialize( DWORD&	param);
#endif
	BOOL	Serialize( UINT&	param);
	BOOL	Serialize( INT&		param);
#if __BSWORDSIZE == 32
	BOOL	Serialize( UINT64&	param);
	BOOL	Serialize( INT64&	param);
#endif //! __BSWORDSIZE
	BOOL	Serialize( float&	param);
	BOOL	Serialize( double&	param);
	BOOL	Serialize( std::string&	str);
	BOOL	Serialize( CSerializeObjBase* pSo);
protected:

};

///////////////////////////////
// CByteStreamRestore inlines
inline void CByteStreamRestore::SetPtr(LPVOID lpPtr, DWORD dwPtrSize)
{
	m_dwPtrSize = dwPtrSize;
	m_lpPtr = (LPBYTE)lpPtr;
}
inline BOOL	CByteStreamRestore::Serialize( LPVOID lpParam, DWORD dwParamSize)
{
	BOOL bRes = FALSE;
	if(dwParamSize <= m_dwPtrSize)
	{
		memcpy( lpParam, m_lpPtr, dwParamSize );
		m_dwPtrSize -= dwParamSize;
		m_lpPtr += dwParamSize;
		bRes = true;
	}
	else
		m_dwPtrSize = 0;
	return bRes;
}
inline BOOL CByteStreamRestore::Serialize( BYTE& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamRestore::Serialize( WORD& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamRestore::Serialize( SHORT& param)
{
	return Serialize( &param, sizeof(param) );
}
#ifndef  __ASTRA__
inline BOOL CByteStreamRestore::Serialize( DWORD& param)
{
	return Serialize( &param, sizeof(param) );
}
#endif
inline BOOL CByteStreamRestore::Serialize( UINT& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamRestore::Serialize( INT& param)
{
	return Serialize( &param, sizeof(param) );
}
#if __BSWORDSIZE == 32
inline BOOL CByteStreamRestore::Serialize( UINT64& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamRestore::Serialize( INT64& param)
{
	return Serialize( &param, sizeof(param) );
}
#endif //!__BSWORDSIZE
inline BOOL CByteStreamRestore::Serialize( float& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamRestore::Serialize( double& param)
{
	return Serialize( &param, sizeof(param) );
}
inline BOOL CByteStreamRestore::Serialize( std::string& str)
{
	WORD wStringSize = 0;
	if( !Serialize( &wStringSize, sizeof(wStringSize) ) )
		return FALSE;
	if(wStringSize)
	{
		str.resize(wStringSize, '\x0');
		return Serialize( &str.at(0), str.size() );
	}
	else
	{
		str.erase();
		return true;
	}
}
inline BOOL CByteStreamRestore::Serialize(CSerializeObjBase* pSo)
{
	return CByteStreamBase::Serialize(pSo);
}
inline LPVOID CByteStreamRestore::GetPtr()
{
	return m_lpPtr;
}
inline DWORD CByteStreamRestore::GetPtrSize() const
{
	return m_dwPtrSize;
}

///////////////////////////////
// helpers
template <class T_DATA_TYPE>
inline BOOL T_SerializeVector(CByteStreamBase* pBsb, std::vector<T_DATA_TYPE>& t_vect)
{
	int dwSize = int(t_vect.size());
	if( !pBsb->Serialize( dwSize ) )
		return FALSE;

	BOOL bRes = true;
	int i;
	for(i=0; i < dwSize; i++)
	{
		if( pBsb->IsModeCollect() )
		{
			if( !pBsb->Serialize( t_vect[i] ) )
			{
				bRes = FALSE;
				break;
			}
		}
		else
		{
			T_DATA_TYPE t_data;
			if( !pBsb->Serialize( t_data ) )
			{
				bRes = FALSE;
				break;
			}
			else
				t_vect.push_back( t_data );
		}
	}

	return bRes;
}

template <class T_DATA_TYPE>
inline BOOL T_SerializeVectorPtr(CByteStreamBase* pBsb, std::vector<T_DATA_TYPE*>& t_vect)
{
	int dwSize = int(t_vect.size());
	if( !pBsb->Serialize( dwSize ) )
		return FALSE;

	BOOL bRes = true;
	int i;
	for(i=0; i < dwSize; i++)
	{
		if( pBsb->IsModeCollect() )
		{
			if( !pBsb->Serialize( t_vect[i] ) )
			{
				bRes = FALSE;
				break;
			}
		}
		else
		{
			T_DATA_TYPE* t_pData = new T_DATA_TYPE;
			if( t_pData )
			{
				if( !pBsb->Serialize( t_pData ) )
				{
					bRes = FALSE;
					delete t_pData;
					break;
				}
				else
					t_vect.push_back( t_pData );
			}
		}
	}

	return bRes;
}

template <class T_DATA_TYPE>
inline BOOL T_SerializeVector_NS(CByteStreamBase* pBsb, std::vector<T_DATA_TYPE>& t_vect)
{
	int dwSize = int(t_vect.size());
	if( !pBsb->Serialize( dwSize ) )
		return FALSE;

	BOOL bRes = true;
	int i;
	for(i=0; i < dwSize; i++)
	{
		if( pBsb->IsModeCollect() )
		{
			if( !pBsb->Serialize( (&t_vect[i]) ) )
			{
				bRes = FALSE;
				break;
			}
		}
		else
		{
			T_DATA_TYPE t_data;
			if( !pBsb->Serialize( &t_data ) )
			{
				bRes = FALSE;
				break;
			}
			else
				t_vect.push_back( t_data );
		}
	}

	return bRes;
}


} //!namespace b_set

#endif // __ByteStream_h
