/******************************************************************************
 File     : StreamRetarder.h
 Author   : SDY
 Date     : 27.05.2006
 Comments : 
 
******************************************************************************/

#ifndef __StreamRetarder_h
#define __StreamRetarder_h

#include "ElapsedTimeCounter.h"
#include "../Thread/Wait.h"

namespace b_set
{

///////////////////////////////////////////////////////////////////////////////
// CStreamRetarder
class CStreamRetarder
{
////////// Attributes:
public:
	CElapsedTimeCounter	m_etcTimeCounter;
	DWORD			m_dwStatisticsAverageSpeed_bps;
	DWORD			m_dwStatisticsSleepTimePerSecond_ms;
	DWORD			m_dwStatisticsSleepTimePerIteration_mks;
	DWORD			m_dwMaxStreamSpeed_bps;
	DWORD			m_dwTimeInterval_ms;
	DWORD			m_dwSleepTimePerInterval_ms;
	INT64			m_i64StreamCounter_bytes;
	DWORD			m_dwSleepTimeMin_ms;
	DWORD			m_dwSleepTimeMax_ms;
	int			m_iIteration;
	DWORD			m_dwActiveTime_ms;
	DWORD			m_dwLastActivationTime_ms;
protected:
	CCriticalSectionObj	m_csLock;

////////// Construction:
public:
	CStreamRetarder();

////////// Operations:
public:
	void Reset();
	void SetMaxStreamSpeed(DWORD dwMaxStreamSpeed_bps);
	void SetTimeInterval(DWORD dwTimeInterval_ms);
	void SetMaxSleepTime(DWORD dwMaxSleepTime_ms);
	void SetMinSleepTime(DWORD dwMinSleepTime_ms);
	DWORD GetSleepTime_ms(DWORD dwBytes);
	DWORD GetAverageSpeed_bps() const;
	DWORD GetSleepTimePerSecond_ms() const;
	void CalculateStatistics();
	bool IsActive() const;
protected:
};

///////////////////////////////
// CStreamRetarder inlines
inline bool CStreamRetarder::IsActive() const
{
	return m_dwMaxStreamSpeed_bps > 0;
}
inline void CStreamRetarder::SetMaxSleepTime(DWORD dwMaxSleepTime_ms)
{
	m_dwSleepTimeMax_ms = dwMaxSleepTime_ms;
}

inline void CStreamRetarder::SetMinSleepTime(DWORD dwMinSleepTime_ms)
{
	m_dwSleepTimeMin_ms = dwMinSleepTime_ms;
}

inline void CStreamRetarder::SetMaxStreamSpeed(DWORD dwMaxStreamSpeed_bps)
{
	m_dwMaxStreamSpeed_bps = dwMaxStreamSpeed_bps;
}

inline void CStreamRetarder::SetTimeInterval(DWORD dwTimeInterval_ms)
{
	if(dwTimeInterval_ms)
		m_dwTimeInterval_ms = dwTimeInterval_ms;
}

inline DWORD CStreamRetarder::GetAverageSpeed_bps() const
{
	return m_dwStatisticsAverageSpeed_bps;
}

inline DWORD CStreamRetarder::GetSleepTimePerSecond_ms() const
{
	return m_dwStatisticsSleepTimePerSecond_ms;
}

};//namespace b_set

#endif // __StreamRetarder_h
