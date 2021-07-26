#ifndef DebugLogManager_h
#define DebugLogManager_h

/**
 * @author gda
 */

// Log
#include <Log/DebugLogger.h>

// boost
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#include <boost/noncopyable.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic pop
#endif

// std
#include <set>

namespace logger
{

class CDebugLogManager: public boost::noncopyable
{
// Typedefs
private:
    typedef boost::shared_mutex                         shared_mutex;
    typedef boost::shared_lock<shared_mutex>            shared_locker;
    typedef boost::unique_lock<shared_mutex>            unique_locker;
    ///< @todo ptr_map ->std::map
    typedef boost::ptr_map<std::string, debug_logger>   map_log;                                                ///< key - log path, value - log
    typedef map_log::iterator                           iter_log;
    typedef std::set<std::string>                       set_str;
// Attributes
private:
    mutable shared_mutex                                m_LogsMutex;                                            ///< Logs protection mutex
    map_log                                             m_mLoggers;                                             ///< Map of Magnit logs
    debug_logger                                        m_EmptyLogger;                                          ///< Empty logger
    set_str                                             m_sEnabled;                                             ///< Set of enabled sources
// Methods
public:
    void                                                addLogger(std::string);                                 ///< Add logger with path
    ref_debug_logger                                    getLogger(const std::string&, const std::string&);      ///< Get logger by path. If logger non exists in internal map, return Null logger
private:
    iter_log                                            _findLog(const std::string&);
    iter_log                                            _insertLog(const std::string&, std::string);
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline void CDebugLogManager::addLogger(std::string strName)
{
    m_sEnabled.insert(strName);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline ref_debug_logger CDebugLogManager::getLogger(const std::string& strLogDir, const std::string& strName)
{
    if(m_sEnabled.find(strName) == m_sEnabled.end())
        return m_EmptyLogger;

    iter_log it = _findLog(strName);
    if(it == m_mLoggers.end())
        it = _insertLog(strLogDir, strName);

    return *it->second;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLogManager::iter_log CDebugLogManager::_findLog(const std::string& strName)
{
    shared_locker Locker(m_LogsMutex);
    return m_mLoggers.find(strName);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLogManager::iter_log CDebugLogManager::_insertLog(const std::string& strLogDir, std::string strName)
{
    unique_locker Locker(m_LogsMutex);
    // generating client's log name
    std::ostringstream ossLogPh;

    std::string strAppName = logger::get_app_name();

    std::string strLogName = strName;
#ifdef WIN32
    std::replace(strLogName.begin(), strLogName.end(), ':', '_');
#endif			// WIN32

    ossLogPh << strLogDir << "/" << strLogName << "_" << strAppName << ".log";

    // creating new logger
    std::pair<iter_log, bool> prRes = m_mLoggers.insert(strName, new debug_logger(ossLogPh.str()));
    // new log must be created anyway
    assert(prRes.second);

    return prRes.first;
}

}               // logger

#endif          // DebugLogManager_h
