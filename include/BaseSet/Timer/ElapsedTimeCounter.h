/******************************************************************************
 File     : ElapsedTimeCounter.h
 Author   : SDY
 Date     : 15.11.2005
 Comments : 
 
******************************************************************************/

#ifndef __ElapsedTimeCounter_h
#define __ElapsedTimeCounter_h

#ifdef WIN32
	#include <winsock2.h>
#else
	typedef long long __int64;
	#include <sys/time.h>
#endif

namespace b_set
{

///////////////////////////////////////////////////////////////////////////////
// CElapsedTimeCounter
class CElapsedTimeCounter
{
////////// Attributes:
public:
	unsigned long	m_dwStartTickCount_ms;
	__int64		m_i64ElapsedTime_ms;

////////// Construction:
public:
	CElapsedTimeCounter()
	{
		Reset();
	}

////////// Operations:
public:
	bool IsActive() const;
	void Reset();
	__int64 GetTime_ms() const;
	unsigned long GetActiveTime_ms() const;
	operator __int64() const;
	void Begin( bool bReset );
	void Begin();
	void End();
	void CopyFrom(const CElapsedTimeCounter& etc);
	void operator=(const CElapsedTimeCounter& etc);
	unsigned long GetTickCount() const;
	
};

///////////////////////////////
// CElapsedTimeCounter inlines

inline void CElapsedTimeCounter::operator=(const CElapsedTimeCounter& etc)
{
	CopyFrom(etc);
}
inline void CElapsedTimeCounter::CopyFrom(const CElapsedTimeCounter& etc)
{
	m_dwStartTickCount_ms = etc.m_dwStartTickCount_ms;
	m_i64ElapsedTime_ms = etc.m_i64ElapsedTime_ms;
}
inline bool CElapsedTimeCounter::IsActive() const
{
	return (m_dwStartTickCount_ms != 0L);
}
inline unsigned long CElapsedTimeCounter::GetActiveTime_ms() const
{
	if( !IsActive() )
		return 0L;
	return (GetTickCount() - m_dwStartTickCount_ms);
}
inline void CElapsedTimeCounter::Reset()
{
	m_dwStartTickCount_ms = 0L;
	m_i64ElapsedTime_ms = 0L;
}
inline void CElapsedTimeCounter::Begin( bool bReset )
{
	if( bReset )
		Reset();
	Begin();
}
inline void CElapsedTimeCounter::Begin()
{
	m_dwStartTickCount_ms = GetTickCount();
}
inline void CElapsedTimeCounter::End()
{
	if(m_dwStartTickCount_ms)
		m_i64ElapsedTime_ms += (GetTickCount() - m_dwStartTickCount_ms);
}
inline __int64 CElapsedTimeCounter::GetTime_ms() const
{
	return m_i64ElapsedTime_ms;
}
inline CElapsedTimeCounter::operator __int64() const
{
	return GetTime_ms();
}

inline unsigned long CElapsedTimeCounter::GetTickCount() const
{
	#ifdef WIN32
		return ::GetTickCount();
	#else
		struct timeval tvalbuf;
		if ( gettimeofday( &tvalbuf, 0 ) == 0L )
			return (unsigned long)(tvalbuf.tv_sec*1000+tvalbuf.tv_usec/1000);
		else
			return 0L;
	#endif //! WIN32
}

}//! namespace b_set
#endif //! __ElapsedTimeCounter_h
