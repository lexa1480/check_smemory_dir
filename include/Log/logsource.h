#pragma once

#ifdef WIN32
#ifndef _WINSOCK2API_
#include <Winsock2.h>
#endif	
#else
#endif

#include <string>
#include <sstream>
#include <regtree/MessageParser.h> 
#include <OSIndependent/GenericMutex.h>
#include "ilogchannel.h"


#if !defined (WIN32)
#define NITALOGDEBUG
#else
	#if (_MSC_VER > 1200)
		#define NITALOGDEBUG
	#endif
#endif

#ifdef NITALOGDEBUG
#include "criticallog.h"
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#include <boost/date_time/posix_time/posix_time.hpp>
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic pop
#endif
#endif //NITALOGDEBUG



/** example
#include <log.h>

using namespace logger;

void f()
{
	CLogClient lc; 
	if (lc.LoadLibrary())				// подгрузка dll
	{
		lc.Init("c:/param.xml");		// инициализаци€ системы каналов
//		lc.InitByNita("param");			// инициализаци€ системы каналов

		CLogSource ls;					// создание канала
		ls.SetSource("My program", "");	// установка имени программы и имени категории (категори€ не используетс€)
		ls.SetUserName("√ость");		// установка имени пользовател€
		ls.SetCompName("Srv1");			// установка имени компьютера
//		ls.SetCompName();				// установка имени текущего компьютера с помощью gethostname()
		ls.Init(lc, "test channel");	// инициализаци€ канала

		// отсылка сообщени€
		ls.Put(MF_INFO | MF_NTRACE, "Some message."); 
		// Please, send message phrase (or phrases) starting from Capital latter and finish them with '.'
		// At this case:
		// 1. It will folow rules of humans languages.
		// 2. Messages with one sentence will looks like messages with several sentences.
		// 3. Logs from diferent sources will be at one style.
	}
}*/


namespace logger
{
	enum MsgFlags
	{
		// type
		MF_INFO			= 0x00000001,	//default 
		MF_WARNING		= 0x00000002,
		MF_ERROR		= 0x00000004,
		MF_TYPE_MASK	= 0x0000000F,
		// importance
		MF_NORMAL		= 0x00000000,	//default 
		MF_LOW			= 0x00000010,
		MF_HIGH			= 0x00000020,
		MF_LEVEL_MASK	= 0x000000F0,
		// trace
		MF_NTRACE		= 0x00000000,	//default 
		MF_TRACE		= 0x00000100,
		MF_TRACE_MASK	= 0x00000F00
	};

	class CLogSource
	{
	public:
		CLogSource();
		CLogSource(CLogClient&, const char* pChannelName = "StdLog");
		~CLogSource();

		bool Init(CLogClient&, const char* channelName = "StdLog");
		void Uninit();

		void Put(unsigned eFlag, const char* pBody, unsigned idMsg = 0, const char* pExtData = 0); 
		void Put(unsigned eFlag, const char* pBody, unsigned idMsg, std::map<std::string, std::string>& message);

		void SetSource(const char* pNameOfSource, const char* pNameOfCategory = "");
		void SetUserName(const char* pUserName);
		void SetCompName(const char* pCompName);
		void SetCompName();
		

	private:
		ILogChannel* m_pChannel;
		std::string m_compName, m_userName, m_nameOfSource, m_nameOfCategory;
		OsMng::CMutex m_Mutex;
		std::string m_channelName;

		void LogInitLog(const char* str);
		void LogIfLogNotFound(const char* str);
		void inject_time(std::ostream& ost_);
	};

//////////////////////////////////////////////////////////////////////////////////////

	inline CLogSource::CLogSource() : m_pChannel(0) {}
	inline CLogSource::~CLogSource() { Uninit(); }

	inline CLogSource::CLogSource(CLogClient& lc, const char* channelName) : m_pChannel(0)
	{
		Init(lc, channelName);
	}

	inline bool CLogSource::Init(CLogClient& lc, const char* channelName)
	{
		if (m_pChannel) Uninit();
		OsMng::CMutexLocker monitor(&m_Mutex);
		m_channelName = channelName;
        ILogMaker* pLMaker = lc.GetLogMaker();
		if (!pLMaker) return false;
		m_pChannel = pLMaker->OpenChannel(channelName);
		bool ret(true);
		if (m_pChannel)
		{
			m_pChannel->AddRef();
			m_pChannel->AddAppender("current_time");
#ifdef NITALOGDEBUG
			std::ostringstream osst;
			osst << "body: Open channel \"" << m_channelName << "\".|time ";
			inject_time(osst);
			LogInitLog(osst.str().c_str());
#endif //NITALOGDEBUG
		}
		else
		{
#ifdef NITALOGDEBUG
			std::ostringstream osst;
			osst << "body: Can't open channel \"" << m_channelName << "\".|time ";
			inject_time(osst);
			LogInitLog(osst.str().c_str());
			ret = false;
#endif //NITALOGDEBUG
		}
		return ret;
	}

	inline void CLogSource::Uninit()
	{
		OsMng::CMutexLocker monitor(&m_Mutex);
		if (m_pChannel)
		{
			m_pChannel->Release();
#ifdef NITALOGDEBUG
			std::ostringstream osst;
			osst << "body: Close channel \"" << m_channelName << "\".|time ";
			inject_time(osst);
			LogInitLog(osst.str().c_str());
#endif //NITALOGDEBUG
		}
		m_pChannel = 0;
	}

	inline void CLogSource::LogInitLog(const char* str)
	{
#ifdef NITALOGDEBUG
		static CCriticalLog log;
		static bool isNeedInit(true);
		if(isNeedInit)
		{
			isNeedInit = false;
            log.Init("$(NITADATA)/log/LogSource.txt");
        }
		log.Put(str);
#endif //NITALOGDEBUG
	}	
	
	inline void CLogSource::LogIfLogNotFound(const char* str)
	{
#ifdef NITALOGDEBUG
		static CCriticalLog log;
		static bool isNeedInit(true);
		if(isNeedInit)
		{
			isNeedInit = false;
            log.Init("$(NITADATA)/log/LogIfNotFoundLog.txt", 50*1024);
		}
		log.Put(str);
#endif //NITALOGDEBUG
	}


	inline void CLogSource::SetSource(const char* nameOfSource, const char* nameOfCategory)
	{
		OsMng::CMutexLocker monitor(&m_Mutex);
		m_nameOfSource = nameOfSource;
		m_nameOfCategory = nameOfCategory;
	}
	inline void CLogSource::SetUserName(const char* userName)
	{
		OsMng::CMutexLocker monitor(&m_Mutex);		
		m_userName = userName;
	}
	inline void CLogSource::SetCompName(const char* compName)
	{
		OsMng::CMutexLocker monitor(&m_Mutex);
		m_compName = compName;
	}
	inline void CLogSource::SetCompName()
	{
		OsMng::CMutexLocker monitor(&m_Mutex);
		char lpCompName[64];
		gethostname(lpCompName, 64);
		m_compName = lpCompName;
	}

	inline void CLogSource::Put(unsigned eFlag, const char* pBody, unsigned idMsg, const char* pExtData)
	{
		std::map<std::string, std::string> message;

		if (pExtData!= 0) message["ext"] = pExtData;
		Put(eFlag, pBody, idMsg, message);
	}

	inline void CLogSource::Put(unsigned eFlag, const char* pBody, unsigned idMsg, std::map<std::string, std::string>& message_)
	{
		OsMng::CMutexLocker monitor(&m_Mutex);
		if (m_pChannel)
		{
			std::map<std::string, std::string> message(message_);
			if (m_compName.size() > 0) message["cmp"] = m_compName;
			if (m_userName.size() > 0) message["usr"] = m_userName;
			if (m_nameOfSource.size() > 0) message["src"] = m_nameOfSource;
			if (m_nameOfCategory.size() > 0) message["cat"] = m_nameOfCategory;
			message["log"] = m_pChannel->GetName();
			message["body"] = pBody;
			if (idMsg != 0)
			{
				std::ostringstream osst;
				osst << idMsg;
				message["idMsg"] = osst.str();
			}			std::ostringstream osst;
			osst << eFlag;
			message["flag"] = osst.str();

			std::string str;
			ToBuffer(message, str);
			assert(get_buf_size(str.data()) == str.size());

			m_pChannel->PutMessage(str.data());
		}
#ifdef NITALOGDEBUG
		else
		{
			std::ostringstream osst;
			osst << "log " << m_channelName;
			if (m_nameOfSource.size() > 0) osst << "|src " << m_nameOfSource;
			if (m_nameOfCategory.size() > 0) osst << "|cat " << m_nameOfCategory;
			if (m_compName.size() > 0) osst << "|cmp " << m_compName;
			if (m_userName.size() > 0) osst << "|usr " << m_userName;
			if (idMsg != 0) osst << "|idMsg " << idMsg;
		//	if (pExtData!= 0) osst << "|ext " << pExtData;
			osst << "|body " << pBody;
			osst << "|flag " << eFlag;

			for(std::map<std::string, std::string>::const_iterator it(message_.begin()); it != message_.end(); ++it)
			{
				osst << "|" << it->first << " " << it->second;
			}

			boost::posix_time::ptime curtime(boost::posix_time::microsec_clock::universal_time());
			osst << "|time ";
			inject_time(osst);

			LogIfLogNotFound(osst.str().c_str());
		}
#endif //NITALOGDEBUG
	}


    inline void CLogSource::inject_time(std::ostream& ost_)
	{
#ifdef NITALOGDEBUG
		boost::posix_time::ptime curtime(boost::posix_time::microsec_clock::universal_time());
		ost_.fill('0');
		ost_ 				
			<< std::setw(4) << curtime.date().year() << "-"
			<< std::setw(2) << (int)curtime.date().month() << "-"
			<< std::setw(2) << curtime.date().day() << " "
			<< std::setw(2) << curtime.time_of_day().hours() << ":"
			<< std::setw(2) << curtime.time_of_day().minutes() << ":"
			<< std::setw(2) << curtime.time_of_day().seconds() << "."
			<< std::setw(3) << (curtime.time_of_day().fractional_seconds()/1000);
#endif //NITALOGDEBUG
	}
}//namespace logger

