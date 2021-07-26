#pragma once
#ifndef CRITICALLOG_H
#define CRITICALLOG_H

#ifdef WIN32
#include <winsock2.h>
#else
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#endif

#include <string>
#include <fstream>
#include <ModeInfo/nitaenv.h>

namespace logger
{
	class CCriticalLog
	{
	public:
		CCriticalLog();
		CCriticalLog& Init(const std::string fileName, unsigned logSizeBytes = 4*1024*1024);
		void Put(const std::string& str);

	private:
		void ApplySizePolicy();

		unsigned file_size();
		bool move_file();
		bool delete_bak();
                void create_directories_();

		std::string m_FileName;
		std::string m_backFileName;

		unsigned m_counter;
		unsigned m_logMaxSize;
	};


/////////////////////////////////////////////////////////////////

	inline CCriticalLog::CCriticalLog()
		:m_counter(0),
		m_logMaxSize(0)
	{
	};

	inline void CCriticalLog::Put(const std::string& str)
	{
		std::ofstream ofsLog(m_FileName.c_str(), std::ios::out | std::ios::app);
		if (ofsLog)
		{
			ofsLog << str << "\n";
			ofsLog.close();
                        if (m_counter%10 == 9) ApplySizePolicy();
			++m_counter;
		}
//		else std::cerr << "CCriticalLog::Put(). Log not open. File name " << m_FileName << std::endl;
	};

        inline void CCriticalLog::create_directories_()
        {
			unsigned i(0);
                for (; i < m_FileName.size(); ++i)
                {
                        if (m_FileName.at(i) == '\\') m_FileName.at(i) = '/';
                }

                std::string path(m_FileName);
                path = path.substr(0, path.rfind('/'));
                path += "/";

                i = 1;
                while(i < path.size())
                {
                    if (path[i] == '/')
                    {
#ifdef WIN32
						std::string str = path.substr(0, i);
                        CreateDirectory(str.c_str(), 0);
#else
                        mkdir(path.substr(0, i).c_str(), 0777);
#endif // WIN32
                    }
                    ++i;
                }
//                        size_t parentEnd = m_FileName.rfind('/');
//                        std::string parent(m_FileName.substr(0, parentEnd));
        };

#ifdef WIN32

	inline unsigned CCriticalLog::file_size()
	{
		HANDLE hFile;
		hFile = CreateFileA(m_FileName.c_str(),
			GENERIC_READ,             // open for reading
			0,                        // do not share
			NULL,                     // no security
			OPEN_EXISTING,            // existing file only
			FILE_ATTRIBUTE_NORMAL,    // normal file
			NULL);                    // no attr. template

		if (hFile == INVALID_HANDLE_VALUE)
		{
	//		std::cerr << "CCriticalLog::file_size Handle open - " << m_FileName << "." << std::endl;
			return 0;
		}
		 unsigned ret = GetFileSize(hFile, 0);

		if (CloseHandle(hFile)==0)
		{
//			std::cerr << "CCriticalLog::file_size Handle close - " << m_FileName << "." << std::endl;
		}

		return ret;

	}

	inline bool CCriticalLog::delete_bak()
	{
		return (0 != DeleteFileA(m_backFileName.c_str() ));
	}
	inline bool CCriticalLog::move_file()
	{
		return (0 != MoveFileA( m_FileName.c_str(), m_backFileName.c_str() ));
	}

#else

	inline unsigned CCriticalLog::file_size()
	{
                struct stat st;
                int ret = 0;
                ret = stat( m_FileName.c_str(), &st );

                if ( ret == -1 )
                        return 0;
                return st.st_size;
	}

	inline bool CCriticalLog::delete_bak()
	{
		return unlink( m_backFileName.c_str() ) == 0;
	}

	inline bool CCriticalLog::move_file()
	{
		return ::rename( m_FileName.c_str() ,
			m_backFileName.c_str() ) == 0; 
	}
#endif //WIN32


        inline void CCriticalLog::ApplySizePolicy()
	{
		unsigned fileSize(file_size());

		if (m_logMaxSize <= fileSize)
		{
			if (!delete_bak())
			{
//				std::cerr << "CCriticalLog::ApplySizePolicy Delete error - " << m_backFileName << "." << std::endl;
//				return;
			}
			if (!move_file())
			{
//				std::cerr << "CCriticalLog::ApplySizePolicy Rename error - " << m_backFileName << "." << std::endl;
			}
		}
	}

        inline CCriticalLog& CCriticalLog::Init(std::string fileName, unsigned logSizeBytes)
        {
                m_logMaxSize = logSizeBytes;
                modeinfo::CEnvironments env;
                env.Expand(fileName);
                m_FileName = fileName;
                // find last '.' and insert "_bak"
                m_backFileName = m_FileName;
                size_t pos = m_backFileName.rfind('.');
                if (pos == 0) pos = m_backFileName.size()-1;
                m_backFileName.insert(pos, "_bak");
                pos = fileName.rfind('/'); // name of dir

                create_directories_();
                m_counter = 0;
                ApplySizePolicy();
                return *this;
        };
}

#endif //!CRITICALLOG_H
