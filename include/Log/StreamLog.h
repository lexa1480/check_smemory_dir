#ifndef TestLog_h
#define TestLog_h

/**
 * @author gda
 */

#ifdef WIN32
#include <Winsock2.h>
#include <shellapi.h>
#endif		// WIN32

// boost
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/any.hpp>
#include <boost/thread/mutex.hpp>
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic pop
#endif
// std
#include <fstream>

namespace logger
{

// Typedefs
typedef boost::filesystem::path         b_path;

std::string  get_app_name();

template <typename tLocker>
class CStreamLogger: public boost::noncopyable
{
// Params
public:
    enum
    {
        c_eMaxSize                      = 1048576                                                                                                                           ///< Default max file size
    };
private:
    enum
    {
        c_eBakupCheck                   = 10                                                                                                                                ///< Check for bakup every c_eBakupCheck
    };
// Typedefs
private:
    typedef std::ostream& (*ostream_manipulator)(std::ostream&);
// Attributes
private:
    mutable boost::mutex                m_Mutex;                                                                                                                            ///< Synchronization mutex

    b_path                              m_phLog;                                                                                                                            ///< Log full path
    std::ofstream                       m_oStream;                                                                                                                          ///< Out stream
    size_t                              m_sMaxSize;                                                                                                                         ///< Max log size, after passing - bakup
    size_t                              m_sSerialCnt;                                                                                                                       ///< Serialization counter
    bool                                m_bNewLine;                                                                                                                         ///< Write on a new line if true
// Ctor
public:
    explicit                            CStreamLogger(const b_path& phLog, const size_t sMaxSize = c_eMaxSize, std::ios_base::openmode mode = std::ios_base::app);
                                        CStreamLogger();
                                        ~CStreamLogger();
public:
    void                                init(const b_path& phLog, const size_t sMaxSize = c_eMaxSize,  std::ios_base::openmode mode = std::ios_base::app);          ///< Initialize log by path and size in bytes (default = 1Mb)
    bool                                isValid() const;                                                                                                                    ///< true if log initialzed
    CStreamLogger&                      operator<< (ostream_manipulator);                                                                                                   ///< Overloaded version for manipulators (like std::endl)
    template <typename T>
    CStreamLogger&                      operator<< (const T&);                                                                                                              ///< Overloaded serialization
    operator                            std::ostream& ();                                                                                                                   ///< Type convertion operator (for using with iterators)

private:
    void                                _checkBakup();
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLocker>
inline CStreamLogger<tLocker>::CStreamLogger()
    : m_sMaxSize(0),
      m_sSerialCnt(0),
      m_bNewLine(false)
{
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLocker>
inline CStreamLogger<tLocker>::CStreamLogger(const b_path& phLog, const size_t sMaxSize, std::ios_base::openmode OpenMode)
{
    init(phLog, sMaxSize, OpenMode);
    _checkBakup();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLocker>
inline CStreamLogger<tLocker>::~CStreamLogger()
{
    tLocker Locker(m_Mutex);

    if(m_oStream.is_open())
        m_oStream.flush();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief                                   Initialize log by path and max size
 * @param phLog                             Full path to log file
 * @param sMaxSize                          Max log size in bytes
 */
template <typename tLocker>
inline void CStreamLogger<tLocker>::init(const b_path& phLog, const size_t sMaxSize, std::ios_base::openmode OpenMode)
{
    tLocker Locker(m_Mutex);

    if(m_oStream.is_open())
    {
        m_oStream.close();
        m_oStream.clear();
    }

    m_phLog         = phLog;
    m_sMaxSize      = sMaxSize;
    m_sSerialCnt    = 0;
    m_bNewLine      = true;

    try
    {
        using namespace boost::filesystem;
        if(!exists(m_phLog.branch_path()))
            create_directories(m_phLog.branch_path());
    }
    catch(...)
    {
        return;
    }

    m_oStream.open(m_phLog.string().c_str(), OpenMode);


#ifdef MACRO_SHA_ONLY
    std::ostringstream oss;
    oss << "=================================================================" << std::endl;
    oss << "APPLICATION: " << get_app_name() << ", COMMIT: " << MACRO_SHA_ONLY;
    m_oStream << oss.str() << std::endl;
#else
    std::ostringstream oss;
    oss << "=================================================================" << std::endl;
    oss << "APPLICATION: " << get_app_name();
    m_oStream << oss.str() << std::endl;

#endif          // MACRO_SHA_ONLY

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief                                   Check is log valid (stream created)
 * @return                                  true if valid
 */
template <typename tLocker>
inline bool CStreamLogger<tLocker>::isValid() const
{
    tLocker Locker(m_Mutex);
    return m_oStream.is_open();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLocker>
inline CStreamLogger<tLocker>& CStreamLogger<tLocker>::operator<< (ostream_manipulator manipulator)
{
    tLocker Locker(m_Mutex);
    m_oStream << manipulator;

    m_bNewLine = true;
    return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLocker>
template <typename T>
inline CStreamLogger<tLocker>& CStreamLogger<tLocker>::operator<< (const T& tObject)
{
    tLocker Locker(m_Mutex);

    if(m_bNewLine)
    {
        if(m_sSerialCnt >= c_eBakupCheck)
        {
            _checkBakup();
            m_sSerialCnt = 0;
        }

        // writing current time
        m_oStream << boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::universal_time()) << ' ';
        m_bNewLine = false;
    }

    m_oStream << tObject;
    ++m_sSerialCnt;

    return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief                                   Check is it neccessary to bakup file (max size check). If true, move log to .bak and reopen existed stream
 */
template <typename tLocker>
inline void CStreamLogger<tLocker>::_checkBakup()
{
    try
    {
        using namespace boost::filesystem;

        // checking max size
        if(file_size(m_phLog) >= m_sMaxSize)
        {
            m_oStream.close();
            m_oStream.clear();

            b_path phLogBakup = std::string(m_phLog.string()).append(".bak");
            // remove previous bakup
            remove(phLogBakup);
            rename(m_phLog, phLogBakup);

            m_oStream.open(m_phLog.string().c_str(), std::ios_base::app);
        }
    }
    catch(...)
    {
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief                                   Type convertion operator (for ostream_iterator for example)
 */
template <typename tLocker>
inline CStreamLogger<tLocker>::operator std::ostream& ()
{
    tLocker Locker(m_Mutex);
	assert(m_oStream.is_open());
    return m_oStream;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief                                   Get current application name
 * @return
 */
inline std::string  get_app_name()
{
#ifdef  WIN32
	int argc;

	LPWSTR* lpArgv = CommandLineToArgvW( GetCommandLineW(), &argc );

	std::vector< std::string > vArgv(argc);

	for(size_t i = 0 ; i < vArgv.size(); ++i)
	{
		vArgv[i].resize(wcslen(lpArgv[i]));
		wcstombs(&vArgv[i][0], lpArgv[i], vArgv[i].size());
	}
	LocalFree( lpArgv );

	std::string strCmdLine = vArgv[0];
#else
	std::string strCmdLine = "";

	std::ostringstream os;
	std::ifstream ifile("/proc/self/cmdline");
	if (ifile)
	{
		while(true)
		{
			char buf;
			ifile.get(buf);
			if (ifile)
			{
				if (buf=='\0' || buf == ' ' || buf == '\n' || buf == '\r' || buf =='\t')
					break;
				else
					os << buf;
			}
			else
				break;
		}
		strCmdLine = os.str();
		ifile.close();
	}

	if(strCmdLine.empty())
		return strCmdLine;

#endif      //WIN32
	size_t iStart = 0;
	// removing quotes
	strCmdLine.erase(std::remove(strCmdLine.begin(), strCmdLine.end(), '\"'), strCmdLine.end());
	// removing full path
	if(strCmdLine.find_last_of('/') != std::string::npos)
		iStart = strCmdLine.find_last_of('/') + 1;
	else if(strCmdLine.find_last_of('\\') != std::string::npos)
		iStart = strCmdLine.find_last_of('\\') + 1;

	return strCmdLine.substr(iStart, strCmdLine.size() - iStart);
}

}           // logger

typedef logger::CStreamLogger<boost::mutex::scoped_lock>        CStreamLog;
typedef logger::CStreamLogger<boost::mutex::scoped_lock>        stream_log;
typedef stream_log&                                             ref_stream_log;

#endif // TestLog_h
