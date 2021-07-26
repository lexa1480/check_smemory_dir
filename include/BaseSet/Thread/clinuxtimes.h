#ifndef CLINUXTIMES_H
#define CLINUXTIMES_H

#ifndef WIN32

#include <pthread.h>

#define MAXTHREADS 100
#define MAX_PATH_LENGTH 	512
#define MAX_THREAD_INFO_LENGTH 	1024

enum TTI_FILED
{
	TTI_LC_STATE = 0,
	TTI_LD_PPID,
	TTI_LD_PGRP,
	TTI_LD_SESSION,
	TTI_LD_TTY_NR,
	TTI_LD_TPGID,
	TTI_LU_FLAGS,
	TTI_LU_MINFLT,
	TTI_LU_CMINFLT,
	TTI_LU_MAJFLT,
	TTI_LU_CMAJFLT,
	TTI_LD_UTIME,
	TTI_LD_STIME,
	TTI_LD_CUTIME,
	TTI_LD_CSTIME,
	TTI_LD_PRIORITY,
	TTI_LU_NICE,
	TTI_LU_ITREALVALUE = TTI_LU_NICE+2,
	TTI_LD_STARTTIME,
	TTI_LU_VSIZE,
	TTI_LU_RESIDENT_SET_SIZE,
	TTI_LU_RLIM,
	TTI_LU_STARTCODE,
	TTI_LU_ENDCODE,
	TTI_LU_STARTSTACK,
	TTI_LU_KSTK_ESP,
	TTI_LU_KSTK_EIP,
	TTI_LD_SIGNAL,
	TTI_LD_BLOCKED,
	TTI_LD_SIG_IGNORED,
	TTI_LD_SIG_CATCH,
	TTI_LU_WCHAN
};

struct ThreadStatInfo
{
	char 	chState;
	long 	ppid;
	long 	pgrp;
	long 	session;
	long 	tty_nr;
	long 	tpgid;
	unsigned long flags;
	unsigned long minflt;
	unsigned long cminflt;
	unsigned long majflt;
	unsigned long cmajflt;
	long 	utime;
	long 	stime;
	long 	cutime;
	long 	cstime;
	long 	pririty;
	unsigned long nice;
	long 	old_empty_fied;
	unsigned long itrealvalue;
	long 	starttime;
	unsigned long vsize;
	unsigned long rss;
	unsigned long rlim;
	unsigned long startcode;
	unsigned long endcode;
	unsigned long startstack;
	unsigned long kstkesp;
	unsigned long kstkeip;
	long 	signal;
	long 	blocked;
	long 	sigignored;
	long 	sigcatch;
	unsigned long wchan;

	ThreadStatInfo(){ Reset(); };
	~ThreadStatInfo(){};
	
	void Reset() {
		chState = '\0';
		ppid = 0l;
		pgrp = 0l;
		session = 0l;
		tty_nr = 0l;
		tpgid = 0l;
		flags = 0l;
		minflt = 0l;
		cminflt = 0l;
		majflt = 0l;
		cmajflt = 0l;
		utime = 0l;
		stime = 0l;
		cutime = 0l;
		cstime = 0l;
		pririty = 0l;
		nice = 0l;
		old_empty_fied = 0l;
		itrealvalue = 0l;
		starttime = 0l;
		vsize = 0l;
		rss = 0l;
		rlim = 0l;
		startcode = 0l;
		endcode = 0l;
		startstack = 0l;
		kstkesp = 0l;
		kstkeip = 0l;
		signal = 0l;
		blocked = 0l;
		sigignored = 0l;
		sigcatch = 0l;
		wchan = 0l;
	};
};

class CLinuxTimes 
{
	private:
		long m_lThreadId;
		long m_lParentId;
		int m_nFileDescroptor;
		char m_szFileName[MAX_PATH_LENGTH];
		char m_szScratch[MAX_THREAD_INFO_LENGTH];

//		pthread_mutex_t m_mutexLT;
	public:
		void Reset();
		int InitForThread();
		int InitForThread( long tid );
		int InitForThread( long tid, long pid );

		int GetThreadInfo( struct ThreadStatInfo* pInfo );

		int GetThreadParam( char* pData, TTI_FILED index );
		int GetThreadParam( long* pData, TTI_FILED index );
		int GetThreadParam( unsigned long* pData, TTI_FILED index );

		int GetThredTimes( timespec* start, timespec* kernel, timespec* user );
		long GetPriority();

	protected:
		int UpdateScratch();
 
	public: 
		CLinuxTimes();
		CLinuxTimes( long lTID );
		CLinuxTimes( long lTID, long lPID );
		~CLinuxTimes();

	private:
		const char* FillValue( const char* pStrSrc, char* pData, int nSkipSpace = 0 );
		const char* FillValue( const char* pStrSrc, long* pData, int nSkipSpace = 0);
		const char* FillValue( const char* pStrSrc, unsigned long* pData, int nSkipSpace = 0);
 };

#endif //! WIN32


#endif //! CLINUXTIMES_H
