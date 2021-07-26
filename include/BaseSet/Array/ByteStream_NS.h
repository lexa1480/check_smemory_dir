/******************************************************************************
 File     : ByteStream_NS.h
 Author   : SDY
 Date     : 11.08.2006
 Comments : 
 
******************************************************************************/

#ifndef __ByteStreamNS_h
#define __ByteStreamNS_h

#include <string>
#include <vector>
#include <string.h>

#include <NitaDataTypes.h>
#include <assert.h>

namespace nita_set
{

class CByteStreamBase;

///////////////////////////////////////////////////////////////////////////////
// CSerializeBaseObj
class CSerializeObjBase
{
public:
	CSerializeObjBase(){};
	virtual ~CSerializeObjBase(){};
public:
	virtual NBool Serialize(CByteStreamBase* pBs) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// CByteStreamBase
class CByteStreamBase
{
protected:
	enum{MODE_NONE, MODE_COLLECT, MODE_RESTORE}	m_enuMode;

public:
	CByteStreamBase()
		{m_enuMode = MODE_NONE;}
	virtual ~CByteStreamBase(){};

public:
	NBool IsModeCollect() const
		{return m_enuMode == MODE_COLLECT;}
	NBool IsModeRestore() const
		{return m_enuMode == MODE_RESTORE;}
	virtual NDword  GetPtrSize() const = 0;
	virtual NLPVoid GetPtr() = 0;
	virtual NBool Serialize(NLPVoid	lpParam, NDword dwSize) = 0;
	virtual NBool Serialize(NByte&	param) = 0;
	virtual NBool Serialize(NWord&	param) = 0;
	virtual NBool Serialize(NShort&	param) = 0;
	virtual NBool Serialize(NDword&	param) = 0;
	virtual NBool Serialize(NInt&	param) = 0;
	virtual NBool Serialize(NUInt64&param) = 0;
	virtual NBool Serialize(NInt64&	param) = 0;
	virtual NBool Serialize(NFloat&	param) = 0;
	virtual NBool Serialize(NDouble&param) = 0;
	virtual NBool Serialize(std::string& str) = 0;
	
	virtual NBool Serialize(CSerializeObjBase* pSo)
	{
		return pSo->Serialize(this);
	}

	virtual NBool PushFront(NLPVoid /*lpParam*/, NDword /*dwSize*/)
		{return true;}
	virtual NBool PushFront(NDword /*param*/)
		{return true;}
};

///////////////////////////////////////////////////////////////////////////////
// CByteStreamCollect
class CByteStreamCollect : public CByteStreamBase
{
////////// Attributes:
public:
protected:
	std::vector<NByte> m_vBytes;

////////// Construction:
public:
	CByteStreamCollect()
		{m_enuMode = MODE_COLLECT;}
	virtual ~CByteStreamCollect(){}

////////// Operations:
public:
	NBool PushFront(NLPVoid	lpParam, NDword dwSize);
	NBool PushFront(NDword	param);
	void Clear();
	NDword  GetPtrSize() const;
	NLPVoid GetPtr();
	// overwritten
	NBool Serialize( NLPVoid	lpParam, NDword dwSize );
	NBool Serialize( NByte&		param );
	NBool Serialize( NWord&		param );
	NBool Serialize( NShort&	param );
	NBool Serialize( NDword&	param );
	NBool Serialize( NInt&		param );
	NBool Serialize( NUInt64&	param );
	NBool Serialize( NInt64&	param );
	NBool Serialize( NFloat&	param );
	NBool Serialize( NDouble&	param );
	NBool Serialize( std::string& str );
	NBool Serialize( CSerializeObjBase*	pSo );
protected:

};

///////////////////////////////
// CByteStreamCollect inlines

inline void CByteStreamCollect::Clear()
{
	m_vBytes.clear();
}
inline NDword CByteStreamCollect::GetPtrSize() const
{
	return (NDword)m_vBytes.size();
}
inline NLPVoid CByteStreamCollect::GetPtr()
{
	return &m_vBytes.front();
}
inline NBool CByteStreamCollect::PushFront( NLPVoid	lpParam, NDword dwSize )
{
	std::vector<NByte> vRes;
	NDword i;
	for( i=0; i<dwSize; i++ )
	{
		NByte by = *(((const LPNByte)lpParam)+i);
		vRes.push_back(by);
	}
	m_vBytes.insert( m_vBytes.begin(), vRes.begin(), vRes.end() );

	return true;
}
inline NBool CByteStreamCollect::PushFront(NDword param)
{
	return PushFront( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(CSerializeObjBase*	pSo)
{
	return CByteStreamBase::Serialize(pSo);
}
inline NBool CByteStreamCollect::Serialize(NLPVoid lpParam, NDword dwSize)
{
	if(lpParam == NULL)
		return false;
	if(dwSize)
	{
		NDword i=0;
		for(; i<dwSize; i++)
			m_vBytes.push_back( ((LPNByte)lpParam)[i] );
	}
	return true;
}
inline NBool CByteStreamCollect::Serialize(NByte& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NWord& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NShort& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NDword& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NInt& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NUInt64& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NInt64& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NFloat& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(NDouble& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamCollect::Serialize(std::string& str)
{
	NBool bRes = true;
	NWord wSize = (NWord)str.size();
	Serialize(wSize);
	if(str.size())
		bRes = Serialize( &str.at(0), str.size() );
	return bRes;
}

///////////////////////////////////////////////////////////////////////////////
// CByteStreamRestore
class CByteStreamRestore : public CByteStreamBase
{
////////// Attributes:
public:
protected:
	NDword	m_dwPtrSize;
	LPNByte	m_lpPtr;

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
	void	SetPtr( NLPVoid	lpPtr, NDword dwPtrSize);
	NLPVoid	GetPtr();
	NDword	GetPtrSize() const;

	NBool	Serialize( NLPVoid	lpParam, NDword dwParamSize);
	NBool	Serialize( NByte&	param);
	NBool	Serialize( NWord&	param);
	NBool	Serialize( NShort&	param);
	NBool	Serialize( NDword&	param);
	NBool	Serialize( NInt&	param);
	NBool	Serialize( NUInt64&	param);
	NBool	Serialize( NInt64&	param);
	NBool	Serialize( NFloat&	param);
	NBool	Serialize( NDouble&	param);
	NBool	Serialize( std::string&	str);
	NBool	Serialize( CSerializeObjBase* pSo);
protected:

};

///////////////////////////////
// CByteStreamRestore inlines
inline void CByteStreamRestore::SetPtr(NLPVoid lpPtr, NDword dwPtrSize)
{
	m_dwPtrSize = dwPtrSize;
	m_lpPtr = (LPNByte)lpPtr;
}
inline NBool CByteStreamRestore::Serialize( NLPVoid lpParam, NDword dwParamSize)
{
	NBool bRes = false;
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
inline NBool CByteStreamRestore::Serialize( NByte& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NWord& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NShort&	param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NDword&	param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NInt& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NUInt64& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NInt64&	param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NFloat&	param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( NDouble& param)
{
	return Serialize( &param, sizeof(param) );
}
inline NBool CByteStreamRestore::Serialize( std::string& str)
{
	NWord wStringSize = 0;
	if( !Serialize( &wStringSize, sizeof(wStringSize) ) )
		return false;
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
inline NBool CByteStreamRestore::Serialize(CSerializeObjBase* pSo)
{
	return CByteStreamBase::Serialize(pSo);
}
inline NLPVoid CByteStreamRestore::GetPtr()
{
	return m_lpPtr;
}
inline NDword CByteStreamRestore::GetPtrSize() const
{
	return m_dwPtrSize;
}

///////////////////////////////
// helpers
template <class T_DATA_TYPE>
inline NBool T_SerializeVector(CByteStreamBase* pBsb, std::vector<T_DATA_TYPE>& t_vect)
{
	int dwSize = int(t_vect.size());
	if( !pBsb->Serialize( dwSize ) )
		return false;

	NBool bRes = true;
	int i;
	for(i=0; i < dwSize; i++)
	{
		if( pBsb->IsModeCollect() )
		{
			if( !pBsb->Serialize(t_vect[i]))
			{
				bRes = false;
				break;
			}
		}
		else
		{
			T_DATA_TYPE t_data;
			if( !pBsb->Serialize( t_data ) )
			{
				bRes = false;
				break;
			}
			else
				t_vect.push_back( t_data );
		}
	}

	return bRes;
}

template <class T_DATA_TYPE>
inline NBool T_SerializeVectorPtr(CByteStreamBase* pBsb, std::vector<T_DATA_TYPE*>& t_vect)
{
	int dwSize = int(t_vect.size());
	if( !pBsb->Serialize( dwSize ) )
		return false;

	NBool bRes = true;
	int i;
	for(i=0; i < dwSize; i++)
	{
		if( pBsb->IsModeCollect() )
		{
			if( !pBsb->Serialize( t_vect[i] ) )
			{
				bRes = false;
				break;
			}
		}
		else
		{
			T_DATA_TYPE* t_pData = 0;
			try{t_pData = new T_DATA_TYPE;}catch(...){assert(false);}
			if( t_pData )
			{
				if( !pBsb->Serialize( t_pData ) )
				{
					bRes = false;
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
inline NBool T_SerializeVector_NS(CByteStreamBase* pBsb, std::vector<T_DATA_TYPE>& t_vect)
{
	int dwSize = int(t_vect.size());
	if( !pBsb->Serialize( dwSize ) )
		return false;

	NBool bRes = true;
	int i;
	for(i=0; i < dwSize; i++)
	{
		if( pBsb->IsModeCollect() )
		{
			if( !pBsb->Serialize(&t_vect[i]))
			{
				bRes = false;
				break;
			}
		}
		else
		{
			T_DATA_TYPE t_data;
			if( !pBsb->Serialize( &t_data ) )
			{
				bRes = false;
				break;
			}
			else
				t_vect.push_back( t_data );
		}
	}

	return bRes;
}


} //!namespace nita_set

#endif // __ByteStreamNS_h
