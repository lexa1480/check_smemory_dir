#pragma once

#include <sstream>
#include <string>
#include <map>

#include "ilogchannel.h"
#include <regtree/MessageParser.h>

namespace logger
{
/** Example:

	CLogClient lc;
	lc.LoadLibrary();
	lc.GetLogMaker()->Init("param");

	CChannelStream (lc, "test")
		<< 4 << set_param("key1", "value1") << " Hello World! " << 23.34 << endl
		<< 4 << set_param("key1", "value1") << (int) 123.34 << endl
		<< "just test" << endl
		<< "just another test" << set_key("number") << 3 << set_key("rate") << 0.123 << endl
		<< set_param("key2", "value2") << "another string";
*/
/**
	inline CChannelStream& endl(CChannelStream& txt);
	inline tmanip set_param(const std::string& s1, const std::string& s2);
	inline tmanip set_appender(const std::string& s1, const std::string& s2);
	inline tmanip set_key(const std::string& s1);
*/
	class CChannelStream
	{
	public:
		CChannelStream() : m_pChannel(0) {}
		CChannelStream(CLogClient& lc, const char* pChannelName) : m_pChannel(0) {Init(lc, pChannelName);};
		~CChannelStream();

		bool Init(CLogClient&, const char* pChannelName);
		void Uninit();

		ILogChannel* m_pChannel;
		std::ostringstream m_osst;
		std::map<std::string, std::string> m_param_map;
		std::string m_currentKey;
	};

	template <typename T> 
	inline CChannelStream& operator << (CChannelStream& txt, const T& value)
	{
		txt.m_osst << value;
		return txt;
	}

	inline CChannelStream& operator << (CChannelStream& txt, CChannelStream& (*f) (CChannelStream&))
	{
		return f(txt);
	}

	inline CChannelStream& endl(CChannelStream& txt)
	{
		if (txt.m_pChannel)
		{
			txt.m_param_map[txt.m_currentKey] = txt.m_osst.str();
			txt.m_currentKey = "body";
			std::string str;
			ToBuffer(txt.m_param_map, str);
			txt.m_pChannel->PutMessage(str.data());;

			txt.m_param_map.clear();
			txt.m_osst.str("");
		}
		return txt;
	}

	struct tmanip
	{
		CChannelStream& (*f) (CChannelStream&, const std::string&, const std::string&);
		std::string s1, s2;

		tmanip(CChannelStream& (*ff)(CChannelStream&, const std::string&, const std::string&), const std::string& s_1, const std::string& s_2) 
			: f(ff), s1(s_1), s2(s_2) {};
	};	

	inline CChannelStream& operator << (CChannelStream& txt, const tmanip& m)
	{
		return m.f(txt, m.s1,  m.s2);
	}

	inline CChannelStream& _param(CChannelStream& txt, const std::string& s1, const std::string& s2)
	{
		assert(txt.m_pChannel);
		txt.m_param_map[s1] = s2;
		return txt;
	}

	inline tmanip set_param(const std::string& s1, const std::string& s2)
	{
		return tmanip(_param, s1, s2);
	}

	inline CChannelStream& _appender(CChannelStream& txt, const std::string& s1, const std::string& s2)
	{
		assert(txt.m_pChannel);
		if (txt.m_pChannel) 
		{
			txt.m_pChannel->AddAppender("pair", s1.c_str(), s2.c_str());
		}
		return txt;
	}

	inline tmanip set_appender(const std::string& s1, const std::string& s2)
	{
		return tmanip(_appender, s1, s2);
	}

	inline CChannelStream& _key(CChannelStream& txt, const std::string& s1, const std::string& /*s2*/)
	{
		assert(txt.m_pChannel);
		if (txt.m_pChannel) 
		{
			txt.m_param_map[txt.m_currentKey] = txt.m_osst.str();
			txt.m_currentKey = s1;
			txt.m_osst.str("");
		}
		return txt;
	}	
	
	inline tmanip set_key(const std::string& s1)
	{
		return tmanip(_key, s1, "");
	}
	/////////////////////////////////////////////////////////////////////////

	inline bool CChannelStream::Init(CLogClient& lc, const char* pChannelName)
	{
		if (m_pChannel) m_pChannel->Release();
        ILogMaker* pLMaker = lc.GetLogMaker();
		assert(pLMaker);
		m_currentKey = "body";
		m_pChannel = pLMaker->OpenChannel(pChannelName);
		if (m_pChannel) m_pChannel->AddRef();
		return m_pChannel != 0;
	}

	inline void CChannelStream::Uninit()
	{
		if (m_osst.str().size() > 0 || m_param_map.size() > 0) *this << endl;
		if (m_pChannel) m_pChannel->Release();
		m_pChannel = 0;
		m_param_map.clear();
	}

	inline CChannelStream::~CChannelStream()
	{
		Uninit();
	}

}
