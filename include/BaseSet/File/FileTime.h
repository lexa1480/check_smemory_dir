#ifndef BSET_FILE_TIME_H
#define BSET_FILE_TIME_H

#ifdef WIN32
	#include <time.h>
#else //! WIN32 not define
	#include <time.h>
	#include <sys/time.h>

    //for GetTickCount()
    #include <grn/GetTickCount.h>

#endif //! WIN32

#include "../Array/CString.h"

#ifndef CFileTime
namespace b_set
{

#ifdef _DEBUG
	#ifdef CFILETIME_ADD_ST
		#define DEBUG_TIME(x) x
	#else //! CFILETIME_ADD_ST not define
		#define DEBUG_TIME(x)
	#endif //! CFILETIME_ADD_ST
#else //! _DEBUG not define
	#define DEBUG_TIME(x)
#endif//! _DEBUG

#ifdef UNICODE
	#define _tcsftime wcsftime
#else
	#define _tcsftime strftime
#endif //! UNICODE

	#ifndef WIN32

		typedef struct _SYSTEMTIME {
			uint16_t wYear;
			uint16_t wMonth;
			uint16_t wDayOfWeek;
			uint16_t wDay;
			uint16_t wHour;
			uint16_t wMinute;
			uint16_t wSecond;
			uint16_t wMilliseconds;
		} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
	
		typedef struct _FILETIME {
			uint32_t dwLowDateTime;
			uint32_t dwHighDateTime;
		} FILETIME, *PFILETIME, *LPFILETIME;

		typedef union _LARGE_INTEGER{ 
			uint64_t QuadPart;
		} LARGE_INTEGER;

	#endif //! WIN32

	struct LOCALTIME : public SYSTEMTIME{};

#ifdef  WINNT40
	#define FILETIME_TO_UNIXTIME_100ns	0x19db1ded53e8000
#else //WINNT40
	#define FILETIME_TO_UNIXTIME_100ns	0x19db1ded53e8000LL
#endif//WINNT40

	#define SECOND_ms		1000
	#define MINUTE_ms		60*1000
	#define HOUR_ms			60*60*1000
	#define DAY_ms			24*60*60*1000

	#define mSECOND_100ns 		(1000 * 10)
	#define SECOND_100ns 		(SECOND_ms * mSECOND_100ns)
	#define MINUTE_100ns 		(MINUTE_ms * mSECOND_100ns)
	#define HOUR_100ns		((UINT64)HOUR_ms * mSECOND_100ns)
	#define DAY_100ns		((UINT64)DAY_ms * mSECOND_100ns)
	
	#define DAYS(x)			((x)*(UINT64)DAY_100ns)
	#define HOURS(x)		((x)*(UINT64)HOUR_100ns)
	#define MINUTES(x)		((x)*(UINT64)MINUTE_100ns)
	#define SECONDS(x)		((x)*(UINT64)SECOND_100ns)
	#define mSECONDS(x)		((x)*(UINT64)mSECOND_100ns)

	#define FT_DEF_YEAR 		1989
	#define FT_DEF_MONTH 		4
	#define FT_DEF_DAY 		12

	#ifndef WIN32
		bool SystemTimeToFileTime( const SYSTEMTIME* st, FILETIME* ft );
		bool FileTimeToSystemTime( const FILETIME* ft, SYSTEMTIME* st );
		bool FileTimeToLocalSystemTime( const FILETIME* ft, SYSTEMTIME* stLocal );

		bool FileTimeToLocalFileTime( const FILETIME* ftUTC, FILETIME* ftLocal );
		bool LocalFileTimeToFileTime( const FILETIME* ftLocal, FILETIME* ftUTC );
	#endif //! WIN32

	////////////////////////////////////////////////////////////////////////////////////
	class CFileTime : public FILETIME
	{
	public:
		DEBUG_TIME( 
			SYSTEMTIME m_st
			);

	public:
		CFileTime(const CFileTime& ftTime)
			{ memcpy(this, &ftTime, sizeof(CFileTime)); DEBUG_TIME(m_st=ftTime); }
		CFileTime(const SYSTEMTIME& stNewTime)
			{ SystemTimeToFileTime(&stNewTime, this); DEBUG_TIME(m_st=*this); }
		CFileTime(FILETIME ftTime)
			{ memcpy(this, &ftTime, sizeof(CFileTime)); DEBUG_TIME(m_st=*this); }
		CFileTime(UINT64 nTime)
			{ memcpy(this, &nTime, sizeof(nTime)); DEBUG_TIME(m_st=*this); }
		CFileTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
			{ SetDateTime( nYear, nMonth, nDay, nHour, nMin, nSec); DEBUG_TIME(m_st=*this); }
		CFileTime()
			{ memset(this, 0, sizeof(CFileTime)); DEBUG_TIME(m_st=*this); }
		~CFileTime(){}

	public:
		static CFileTime GetSystemTime();
		static CFileTime GetTickTime(DWORD dwTime_Ticks = 0);
		static DWORD GetTickCount();
		BOOL SetCurrent();
		BOOL SystemTime();
		BOOL LocalTime();
		BOOL SystemToLocalTime();
		BOOL LocalToSystemTime();
		SYSTEMTIME Time(BOOL bLocalTime = FALSE) const;
		CString GetString(BOOL bLocalTime = FALSE) const;
		CString GetStringTime(BOOL bLocalTime = FALSE) const;
		CString GetStringFullTime(BOOL bLocalTime = FALSE) const;
		CString GetStringDate(BOOL bLocalTime = FALSE) const;
		BOOL IsNull() const;
		CFileTime& Increment(DWORD dwHours, DWORD dwMinutes = 0, DWORD dwSeconds = 0, DWORD dwMSeconds = 0);
		CFileTime& Decrement(DWORD dwHours, DWORD dwMinutes = 0, DWORD dwSeconds = 0, DWORD dwMSeconds = 0);
		DWORD GetTotalHours() const;
		DWORD GetTotalMinutes() const;
		UINT64 GetTotalSeconds() const;
		DWORD GetTDWORDSeconds() const;
		UINT64 GetTotalMSeconds() const;
		DWORD GetDayMSeconds() const;
		BOOL SetDateTime(const SYSTEMTIME& stDate, const SYSTEMTIME& stTime);
		BOOL SetDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec);
		BOOL SetDate(int nYear, int nMonth, int nDay);
		void ChangeHourMin(WORD wNewHour, WORD wNewMinute); // modified time varies + - 12 hours min. deviation from the current
		CFileTime GetDate(); // returns date only; h, min, sec, ms - reset
		BOOL SetTime(int nHour, int nMin, int nSec);
//#ifdef _INC_TIME // if time.h is included
		BOOL SetTimeByUnix(const time_t tt);
		time_t GetUnixTime() const;
		CString Format(LPCTSTR pFormat, BOOL bLocalTime = FALSE) const; // similar strftime
//#endif

	public:
		operator SYSTEMTIME() const;
		operator LOCALTIME() const;
		operator UINT64() const;
		operator LARGE_INTEGER() const;
		const CFileTime& operator=(UINT64 nNewTime);
		const CFileTime& operator=(const SYSTEMTIME& stNewTime);
		const CFileTime& operator+=(UINT64 nNewTime);
		const CFileTime& operator-=(UINT64 nNewTime);
	};
	
	////////////////////////////////////////////////////////////////////////////////////
	#ifndef WIN32


	inline bool SystemTimeToFileTime( const SYSTEMTIME* st, FILETIME* ft )
	{
		struct tm tim;
		memset (&tim,0,sizeof(tm));

		memset (ft,0,sizeof(FILETIME));

		tim.tm_sec = st->wSecond;
		tim.tm_min = st->wMinute;
		tim.tm_hour = st->wHour;
		tim.tm_mday = st->wDay;
		tim.tm_mon = st->wMonth-1;
		tim.tm_year = st->wYear-1900;
		tim.tm_isdst = 0;
		
#ifndef WIN32
        uint64_t tt = timegm(&tim);
#else
		uint64_t tt = mktime(&tim);
#endif //WIN32

		if ( tt == ((uint64_t)-1) )
		 return false;

		tt *= 1000;
		tt += st->wMilliseconds;
		tt *= 10000;
		tt += FILETIME_TO_UNIXTIME_100ns;
	
		ft->dwHighDateTime = (DWORD)(tt >> 32);
		ft->dwLowDateTime = (DWORD)(tt);
		
		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////
	inline bool FileTimeToSystemTime( const FILETIME* ft, SYSTEMTIME* st )
	{

		memset(st,0,sizeof(SYSTEMTIME));

		uint64_t ftime = ft->dwHighDateTime;
		ftime = ftime << 32;
		ftime += ft->dwLowDateTime;

		if ( ftime > (FILETIME_TO_UNIXTIME_100ns) )
			ftime	-= FILETIME_TO_UNIXTIME_100ns;	//! Adjust to unix time in 100 nanosecond interval
		ftime	/= 10000;			//! Adjust to milliseconds
		st->wMilliseconds = ftime%1000;		//! Get milliseconds
		ftime	/= 1000;			//! Adjust to seconds

		struct  tm ttm;
		struct  tm * tim = gmtime_r((time_t*)&ftime, &ttm);

		if ( tim == NULL )
		 return false;

		st->wSecond = tim->tm_sec;
		st->wMinute = tim->tm_min;
		st->wHour = tim->tm_hour;
		st->wDay = tim->tm_mday;
		st->wMonth = tim->tm_mon + 1;
		st->wYear = tim->tm_year + 1900;

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////
	inline bool FileTimeToLocalFileTime( const FILETIME* ftUTC, FILETIME* ftLocal )
	{
		memset(ftLocal,0,sizeof(FILETIME));

		uint64_t ftime = ftUTC->dwHighDateTime;
		ftime = ftime << 32;
		ftime += ftUTC->dwLowDateTime;

		if ( ftime > (FILETIME_TO_UNIXTIME_100ns) )
			ftime	-= FILETIME_TO_UNIXTIME_100ns;	//! Adjust to unix time in 100 nanosecond interval
		ftime	/= 10000;			//! Adjust to milliseconds
		int msec = ftime%1000;			//! Get milliseconds
		ftime	/= 1000;			//! Adjust to seconds

		struct  tm ttm;
		struct tm * tmbuf = localtime_r((time_t*)&ftime, &ttm);
		if (tmbuf)
			ftime = mktime(tmbuf);
		else
			return false;

		if ( ftime == ((uint64_t)-1) )
		 return false;

		ftime *= 1000;
		ftime += msec;
		ftime *= 10000;
		ftime += FILETIME_TO_UNIXTIME_100ns;

		ftLocal->dwHighDateTime = (DWORD)(ftime>>32);
		ftLocal->dwLowDateTime = (DWORD)(ftime);

		return true;
	}

	inline bool LocalFileTimeToFileTime( const FILETIME* ftLocal, FILETIME* ftUTC )
	{
		memset( ftUTC,0,sizeof(FILETIME) );

		uint64_t ftime = ftLocal->dwHighDateTime;
		ftime = ftime << 32;
		ftime += ftLocal->dwLowDateTime;

		ftime	-= FILETIME_TO_UNIXTIME_100ns;	//! Adjust to unix time in 100 nanosecond interval
		ftime	/= 10000;			//! Adjust to milliseconds
		int msec = ftime%1000;			//! Get milliseconds
		ftime	/= 1000;			//! Adjust to seconds

		struct  tm ttm;
		struct tm * tmbuf = gmtime_r((time_t*)&ftime, &ttm);
		if (tmbuf)
			ftime = mktime(tmbuf);
		else
			return false;

		if ( ftime == ((uint64_t)-1) )
		 return false;

		ftime *= 1000;
		ftime += msec;
		ftime *= 10000;
		ftime += FILETIME_TO_UNIXTIME_100ns;
	
		ftUTC->dwHighDateTime = (DWORD)(ftime>>32);
		ftUTC->dwLowDateTime = (DWORD)(ftime);
	
		return true;
	}
	
	inline bool FileTimeToLocalSystemTime( const FILETIME* ft, SYSTEMTIME* stLocal )
	{
		memset( stLocal,0,sizeof(SYSTEMTIME) );

		uint64_t ftime = ft->dwHighDateTime;
		ftime = ftime << 32;
		ftime += ft->dwLowDateTime;

		ftime	-= FILETIME_TO_UNIXTIME_100ns;	//! Adjust to unix time in 100 nanosecond interval
		ftime	/= 10000;			//! Adjust to milliseconds
		stLocal->wMilliseconds = ftime%1000;	//! Get milliseconds
		ftime	/= 1000;			//! Adjust to seconds

		struct  tm ttm;
		struct  tm * tim = localtime_r((time_t*)&ftime,&ttm);

		if ( tim == NULL )
		 return false;

		stLocal->wSecond = tim->tm_sec;
		stLocal->wMinute = tim->tm_min;
		stLocal->wHour 	= tim->tm_hour;
		stLocal->wDay 	= tim->tm_mday;
		stLocal->wMonth = tim->tm_mon + 1;
		stLocal->wYear 	= tim->tm_year + 1900;

		return true;
	}
	
	#endif //! WIN32
	////////////////////////////////////////////////////////////////////////////////////
	inline CFileTime CFileTime::GetSystemTime()
		{
			CFileTime ft;
			ft.SystemTime();
			return ft;
		}
	inline CFileTime CFileTime::GetTickTime(DWORD dwTime_Ticks /* = 0 */)
		{
			CFileTime ft;
			ft.SystemTime();
			DWORD dwCur_Ticks = GetTickCount();
			ft = ft - (dwCur_Ticks - dwTime_Ticks) * INT64(mSECOND_100ns);
			return ft;
		}
	inline DWORD CFileTime::GetTickCount()
		{
            return ::GetTickCount();
		}
	inline BOOL CFileTime::SetCurrent()
		{
			#ifdef WIN32
				SYSTEMTIME stTime;
				::GetSystemTime(&stTime);
				DEBUG_TIME(m_st=stTime);
				return SystemTimeToFileTime(&stTime, this);
			#else //! WIN32 not define
				struct timeval tvalbuf;
				struct timezone tzbuf;

				if ( gettimeofday( &tvalbuf, &tzbuf ) != 0 )
					return false;

				uint64_t ftime = tvalbuf.tv_sec;
				ftime *= 10000*1000;
				ftime += tvalbuf.tv_usec*10;
				ftime += FILETIME_TO_UNIXTIME_100ns;

				dwHighDateTime = (DWORD)(ftime>>32);
				dwLowDateTime = (DWORD)(ftime);

				return true;
			#endif //! WIN32
		}
	inline BOOL CFileTime::SystemTime()
		{
			#ifdef WIN32
				SYSTEMTIME stTime;
				::GetSystemTime(&stTime);
				DEBUG_TIME(m_st=stTime);
				return SystemTimeToFileTime(&stTime, this);
			#else //! WIN32 not define
				struct timeval tvalbuf;
				struct timezone tzbuf;

				if ( gettimeofday( &tvalbuf, &tzbuf ) != 0 )
					return false;

				uint64_t ftime = tvalbuf.tv_sec;
				ftime *= 10000*1000;
				ftime += tvalbuf.tv_usec * 10; //! adjust microsec to 100 nanosec
				ftime += FILETIME_TO_UNIXTIME_100ns;

				dwHighDateTime = (DWORD)(ftime>>32);
				dwLowDateTime = (DWORD)(ftime);

				return true;
			#endif //! WIN32
		}
	inline BOOL CFileTime::LocalTime()
		{
			#ifdef WIN32
				SYSTEMTIME stTime;
				::GetLocalTime(&stTime);
				DEBUG_TIME(m_st=stTime);
				return SystemTimeToFileTime(&stTime, this);
			#else //! WIN32 not define
				struct timeval tvalbuf;
				struct timezone tzbuf;

				if ( gettimeofday( &tvalbuf, &tzbuf ) != 0 )
					return false;

				uint64_t ftime = tvalbuf.tv_sec - tzbuf.tz_minuteswest * 60;
				ftime *= 10000*1000;
				ftime += tvalbuf.tv_usec * 10; //! udjust microsec to 100 nanosec
				ftime += FILETIME_TO_UNIXTIME_100ns;

				dwHighDateTime = (DWORD)(ftime>>32);
				dwLowDateTime = (DWORD)(ftime);

				return true;
			#endif
		}
	inline BOOL CFileTime::SystemToLocalTime()
		{
			FILETIME ftTmp;
			if( FileTimeToLocalFileTime(this, &ftTmp) )
			{
				*this = ftTmp;
				return true;
			}
			else
				return FALSE;
		}
	inline BOOL CFileTime::LocalToSystemTime()
		{
			FILETIME ftTmp;
			if( LocalFileTimeToFileTime(this, &ftTmp) )
			{
				*this = ftTmp;
				return true;
			}
			else
				return FALSE;
		}
	inline BOOL CFileTime::IsNull() const
		{
			return this->operator UINT64() == 0;
		}
	inline CFileTime& CFileTime::Increment(DWORD dwHours, DWORD dwMinutes, DWORD dwSeconds, DWORD dwMSeconds)
		{
			this->operator+=(HOURS(dwHours)+MINUTES(dwMinutes)+SECONDS(dwSeconds)+mSECONDS(dwMSeconds));
			return *this;
		}
	inline CFileTime& CFileTime::Decrement(DWORD dwHours, DWORD dwMinutes, DWORD dwSeconds, DWORD dwMSeconds)
		{
			this->operator-=(HOURS(dwHours)+MINUTES(dwMinutes)+SECONDS(dwSeconds)+mSECONDS(dwMSeconds));
			return *this;
		}
	inline DWORD CFileTime::GetTotalHours() const
		{
			return DWORD(this->operator UINT64() / HOUR_100ns);
		}
	inline DWORD CFileTime::GetTotalMinutes() const
		{
			return DWORD(this->operator UINT64() / MINUTE_100ns);
		}
	inline UINT64 CFileTime::GetTotalSeconds() const
		{
			return this->operator UINT64() / SECOND_100ns;
		}
	inline DWORD CFileTime::GetTDWORDSeconds() const
		{
			UINT64 n64Temp = this->operator UINT64()/SECOND_100ns;
			return DWORD(n64Temp);
		}
	inline UINT64 CFileTime::GetTotalMSeconds() const
		{
			return this->operator UINT64() / mSECOND_100ns;
		}
	inline DWORD CFileTime::GetDayMSeconds() const
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(this, &st);
			return 
				  (DWORD)st.wHour * 60 * 60 * 1000
				+ (DWORD)st.wMinute * 60 * 1000
				+ (DWORD)st.wSecond * 1000
				+ (DWORD)st.wMilliseconds;
		}
	inline CFileTime::operator SYSTEMTIME() const
		{
			SYSTEMTIME st;
			FileTimeToSystemTime(this, &st);
			return st;
		}
	inline CFileTime::operator LOCALTIME() const
		{
			#ifdef WIN32
				LOCALTIME st;
				CFileTime ft = *this;
				FileTimeToLocalFileTime(this, &ft);
				FileTimeToSystemTime(&ft, &st);
				return st;
			#else //! WIN32 not define
				LOCALTIME st;
				FileTimeToLocalSystemTime(this,&st);
				return st;
			#endif //! WIN32
		}
	inline CFileTime::operator LARGE_INTEGER() const
		{
			LARGE_INTEGER li;
			li.QuadPart = this->operator UINT64();
			return li;
		}
	inline CString CFileTime::GetString(BOOL bLocalTime) const
		{
			if(IsNull())
				return CString(_T("00/00/00 00:00:00"));

			SYSTEMTIME st;
			if(bLocalTime)
				st = (LOCALTIME)*this;
			else
				FileTimeToSystemTime(this, &st);
			CString str;
			str.Format(_T("%02d/%02d/%02d %02d:%02d:%02d"),
				st.wDay, st.wMonth, st.wYear%100, 
				st.wHour, st.wMinute, st.wSecond);
			return str;
		}
	inline CString CFileTime::GetStringTime(BOOL bLocalTime) const
		{
			if(IsNull())
				return CString(_T("00:00:00"));
			SYSTEMTIME st;
			if(bLocalTime)
				st = (LOCALTIME)*this;
			else
				FileTimeToSystemTime(this, &st);
			CString str;
			str.Format(_T("%02d:%02d:%02d"),
				(int)st.wHour, (int)st.wMinute, (int)st.wSecond);
			return str;
		}
	inline CString CFileTime::GetStringFullTime(BOOL bLocalTime) const
		{
			if(IsNull())
				return CString(_T("00:00:00.000"));
			SYSTEMTIME st;
			if(bLocalTime)
				st = (LOCALTIME)*this;
			else
				FileTimeToSystemTime(this, &st);
			CString str;
			str.Format(_T("%02d:%02d:%02d.%03d"),
				(int)st.wHour, (int)st.wMinute, (int)st.wSecond, (int)st.wMilliseconds);
			return str;
		}
	inline CString CFileTime::GetStringDate(BOOL bLocalTime) const
		{
			if(IsNull())
				return CString(_T("00/00/00"));
			SYSTEMTIME st;
			if(bLocalTime)
				st = (LOCALTIME)*this;
			else
				FileTimeToSystemTime(this, &st);
			CString str;
			str.Format(_T("%02d/%02d/%02d"),
				(int)st.wDay, (int)st.wMonth, (int)st.wYear%100);
			return str;
		}
	inline SYSTEMTIME CFileTime::Time(BOOL bLocalTime) const
		{
			if(bLocalTime)	
				return this->operator LOCALTIME();
			else
				return this->operator SYSTEMTIME();
		}
	inline CFileTime::operator UINT64() const
		{
			return *(UINT64*)this;
		}
	inline const CFileTime& CFileTime::operator=(UINT64 nNewTime)
		{
			UINT64* pn64this = (UINT64*)this;
			#ifdef WIN32
				*pn64this = nNewTime;
			#else //WIN32
				memcpy( (char*)pn64this, (char*)&nNewTime, sizeof( *this ) );
			#endif//WIN32
			DEBUG_TIME(m_st=*this);
			return *this;
		}
	inline const CFileTime& CFileTime::operator=(const SYSTEMTIME& stNewTime)
		{
			SystemTimeToFileTime(&stNewTime, this);
			DEBUG_TIME(m_st=*this);
			return *this;
		}
	inline const CFileTime& CFileTime::operator+=(UINT64 nNewTime)
		{
			UINT64* pn64this = (UINT64*)this;
#ifdef WIN32
			*pn64this += nNewTime;
#else //WIN32
			UINT64 ui64New = (*pn64this)+nNewTime;
			memcpy( (char*)pn64this, (char*)&ui64New, sizeof( *this ) );
#endif//WIN32
			DEBUG_TIME(m_st=*this);
			return *this;
		}
	inline const CFileTime& CFileTime::operator-=(UINT64 nNewTime)
		{
			UINT64* pn64this = (UINT64*)this;
			*pn64this -= nNewTime;
			DEBUG_TIME(m_st=*this);
			return *this;
		}

	// inline helpers
	inline UINT64 AbsDifference(UINT64 nFirst, UINT64 nSecond)
		{
			return (nFirst > nSecond) ? (nFirst - nSecond) : (nSecond - nFirst);
		}
	inline BOOL CFileTime::SetDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
		{
			SYSTEMTIME st;
			memset( &st, 0, sizeof(SYSTEMTIME));
			st.wYear = nYear; st.wMonth = nMonth; st.wDay = nDay;
			st.wHour = nHour; st.wMinute = nMin; st.wSecond = nSec;
			st.wMilliseconds = 0;
			if ( SystemTimeToFileTime(&st, this) == 0 )
			{
	  			UINT64* pn64this = (UINT64*)this;
				*pn64this = 0L;
				DEBUG_TIME(m_st=*this);
				return FALSE;
			}
			DEBUG_TIME(m_st=*this);
			return true;
		}
	inline BOOL CFileTime::SetDateTime(const SYSTEMTIME& stDate, const SYSTEMTIME& stTime)
		{
			SYSTEMTIME st; 
			memset(&st,0,sizeof(st));
			st.wYear = stDate.wYear; st.wMonth = stDate.wMonth; st.wDay = stDate.wDay;
			st.wHour = stTime.wHour; st.wMinute = stTime.wMinute; st.wSecond = stTime.wSecond;
			st.wMilliseconds = stTime.wMilliseconds;
			if ( SystemTimeToFileTime(&st, this) == 0 )
			{
	  			UINT64* pn64this = (UINT64*)this;
				*pn64this = 0L;
				DEBUG_TIME(m_st=*this);
				return FALSE;
			}
			DEBUG_TIME(m_st=*this);
			return true;
		}
	inline BOOL CFileTime::SetDate(int nYear, int nMonth, int nDay)
		{
			SYSTEMTIME st;
			memset( &st, 0, sizeof(SYSTEMTIME));
			st.wYear = nYear; st.wMonth = nMonth; st.wDay = nDay;
			st.wHour = st.wMinute = st.wSecond = st.wMilliseconds = 0;
			if ( SystemTimeToFileTime(&st, this) == 0 )
			{
	  			UINT64* pn64this = (UINT64*)this;
				*pn64this = 0L;
				DEBUG_TIME(m_st=*this);
				return FALSE;
			}
			DEBUG_TIME(m_st=*this);
			return true;
 		}
	inline BOOL CFileTime::SetTime(int nHour, int nMin, int nSec)
		{
			SYSTEMTIME st;
			memset( &st, 0, sizeof(SYSTEMTIME));
			st.wYear = FT_DEF_YEAR; st.wMonth = FT_DEF_MONTH; st.wDay = FT_DEF_DAY;
			st.wHour = nHour; st.wMinute = nMin; st.wSecond = nSec;
			st.wMilliseconds = 0;
			if ( SystemTimeToFileTime(&st, this) == 0 )
			{
	  			UINT64* pn64this = (UINT64*)this;
				*pn64this = 0L;
				DEBUG_TIME(m_st=*this);
				return FALSE;
			}
			DEBUG_TIME(m_st=*this);
			return true;
		}
	inline void CFileTime::ChangeHourMin(WORD wNewHour, WORD wNewMinute)
	{
		CFileTime ftOld = *this;
		SetTime(wNewHour, wNewMinute,0);
		if (ftOld > *this && (ftOld - *this) > HOURS(12))
			*this += HOURS(24);
		else
			if(*this > ftOld && (*this - ftOld) > HOURS(12))
				*this -= HOURS(24);
	}
//#ifdef _INC_TIME //! если подключен time.h
	inline BOOL CFileTime::SetTimeByUnix(const time_t tt)
		{
			UINT64 u64Time = UINT64(tt)*SECOND_100ns + FILETIME_TO_UNIXTIME_100ns;
			*this = u64Time;

			DEBUG_TIME(m_st=*this);
			return true;
		}
	inline time_t CFileTime::GetUnixTime() const
		{
			UINT64 u64Time = this->operator UINT64();
			time_t tt = (time_t)((u64Time - FILETIME_TO_UNIXTIME_100ns + SECOND_100ns/2) / SECOND_100ns);
			return tt;
		}
	inline CString CFileTime::Format(LPCTSTR pFormat, BOOL bLocalTime) const
		{
			SYSTEMTIME st = Time(bLocalTime);
			struct tm tmTime;
			tmTime.tm_sec = st.wSecond;
			tmTime.tm_min = st.wMinute;
			tmTime.tm_hour = st.wHour;
			tmTime.tm_mday = st.wDay;
			tmTime.tm_mon = st.wMonth - 1;        // tm_mon is 0 based
			if(st.wYear < 1900)
				tmTime.tm_year = 0;
			else
				tmTime.tm_year = st.wYear - 1900;     // tm_year is 1900 based
			tmTime.tm_isdst = 0;

			const int nMaxCount = 128;
			TCHAR szBuffer[nMaxCount] = _T("");
			_tcsftime(szBuffer, nMaxCount, pFormat, &tmTime);
			return szBuffer;
		}
//#endif //! _INC_TIME, если подключен time.h
	inline CFileTime CFileTime::GetDate()
		{
			SYSTEMTIME st = *this;
			st.wHour = 0;
			st.wMinute = 0;
			st.wSecond = 0;
			st.wMilliseconds = 0;
			return st;
		}

	inline CString UINT64ToString(UINT64 u64)
		{
			CString str; str.Format(_T("%I64i"), u64);
			return str;
		}

} //! namespace b_set

#ifndef NDEF_A_FileTime
	#define CFileTime		b_set::CFileTime

#endif//!NDEF_A_FileTime
#ifndef WIN32
#define SYSTEMTIME	b_set::SYSTEMTIME
#define FILETIME	b_set::FILETIME
#endif//WIN32

#endif //!CFileTime

#endif //!BSET_FILE_TIME_H
