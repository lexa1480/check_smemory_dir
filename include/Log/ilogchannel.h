#pragma once

namespace logger
{

	struct ILogChannel
	{
		virtual unsigned AddRef() = 0;
		virtual unsigned Release() = 0;

		virtual bool Init(const char* ) = 0;
		virtual const char* GetType() const = 0;
		virtual void PutMessage(const char* pszBuffer) = 0;

		virtual const char* GetName() const = 0;
		virtual const char* GetParams() const = 0;

//		virtual void AddAppender(const char* pAppenderName) = 0;
		virtual void AddAppender(const char* appenderName, const char* val1 = 0, const char* val2 = 0) = 0;

		virtual unsigned AppenderNum() const = 0;
		virtual void DeleteAppender(unsigned uIndex) = 0;
		//virtual iAppender* getAppender(unsigned index) const = 0;

		virtual void  AddChannel(ILogChannel* pChannel) = 0;
		virtual unsigned ChannelNum() const = 0;
		virtual void DeleteChannel(unsigned uIndex) = 0;
		virtual ILogChannel* GetChannel(unsigned uIndex) = 0;
	};

	struct ILogMaker
	{
		/// Initialization by full qualification of file name.
		virtual bool Init(const char* pszFileName, const char* pszRegistryShift = "") = 0;
		virtual bool SaveParams(const char* pszFileName) = 0;

		virtual ILogChannel* OpenChannel(const char* pszName) = 0;
		virtual ILogChannel* CreateChannel(const char* pszChannelType) = 0;

		virtual void AddRef() = 0;
		virtual void Release() = 0;

		/// Set file path prefix for using by repositories
//		virtual void SetPathPrefix(const char* pszPathPrefix) = 0;

		/// Initialization by using NITAROOT environment variable.
		virtual bool InitByNita(const char* pszFileName, const char* pszRegistryShift = "") = 0;
		virtual bool InitDefault(void) = 0;
	};

}
