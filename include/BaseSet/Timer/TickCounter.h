/******************************************************************************
 File     : TickCounter.h
 Author   : SDY
 Date     : 26.01.2002
 Comments : 
 
******************************************************************************/

#ifndef __TickCounter_h
#define __TickCounter_h

#include "../BaseTypes_x.h"

#ifndef WIN32
	#include <sys/time.h>
#endif
namespace b_set
{

///////////////////////////////////////////////////////////////////////////////
// CTickCounter
class CTickCounter
{
//////////
protected:
	DWORD m_dwDuration_ms;
	DWORD m_dwStartTickCount_ms;

//////////
public:
	CTickCounter()
	{
		Reset();
	}
	~CTickCounter()
	{
		Reset();
	}

//////////
public:
	bool StartCounter(DWORD dwDuration_ms);
	void StopCounter();
	bool IsComplete() const;
	bool IsActive() const;
	// GetTicks: returns time in ms from the inception (StartCounter)
	DWORD GetTicks() const;
	// GetLeftTicks: returns the time in ms remaining to complete (from start time StartCounter)
	DWORD GetLeftTicks() const;
	void Reset();

	static unsigned long GetTickCount();

protected:

};

///////////////////////////////
// CTickCounter inlines
inline unsigned long CTickCounter::GetTickCount()
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

inline void CTickCounter::Reset()
{
	m_dwDuration_ms = 0L;
	m_dwStartTickCount_ms = 0L;
}
inline bool CTickCounter::IsActive() const
{
	return (m_dwDuration_ms != 0L);
}
inline bool CTickCounter::StartCounter(DWORD dwDuration_ms)
{
	m_dwDuration_ms = dwDuration_ms;
	m_dwStartTickCount_ms = GetTickCount();
	return true;
}
inline void CTickCounter::StopCounter()
{
	Reset();
}
inline DWORD CTickCounter::GetTicks() const
{
	if ( m_dwDuration_ms == 0 )
		return (DWORD)-1;
	DWORD dwCurTickCount_ms = GetTickCount();
	if(dwCurTickCount_ms < m_dwStartTickCount_ms)
	{
		UINT64 un64Ticks = (UINT64)(((DWORD)-1 + dwCurTickCount_ms) - m_dwStartTickCount_ms);
		return (DWORD)un64Ticks;
	}
	else
	{
		DWORD dwTicks = dwCurTickCount_ms - m_dwStartTickCount_ms;
		return dwTicks;
	}
}
inline bool CTickCounter::IsComplete() const
{
	if ( m_dwDuration_ms == 0 )
		return true;
	else
		return (GetTicks() >= m_dwDuration_ms);
}
inline DWORD CTickCounter::GetLeftTicks() const
{
	if ( m_dwDuration_ms == 0 )
		return (DWORD)-1;
	DWORD dwTicks = GetTicks();
	return (m_dwDuration_ms > dwTicks) ? (m_dwDuration_ms - dwTicks) : 0L;
}

}//! namespace b_set
#endif //! __TickCounter_h
