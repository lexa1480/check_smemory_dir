#ifndef DebugLogger_h
#define DebugLogger_h

/**
 * @author gda
 */

// prj
#include <Log/DebugLoggerCore.h>
#include <Log/LogLevel.h>
// Modeinfo
#include <ModeInfo.h>

// boost
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic pop
#endif

namespace logger
{

class CDebugLogger
{
// Typedefs
private:
    typedef CDebugLoggerCore::ostream_manipulator   ostream_manipulator;
// Attributes
private:
    ptr_logger_core                                 m_pLoggerCore;
    enu_log_level                                   m_eLogLvl;
    enu_log_level                                   m_eMsgLvl;
// Ctor
public:
                                                    CDebugLogger(enu_log_level eLvl = normal_lvl);
    explicit                                        CDebugLogger(const b_path&, const size_t sLogSize = stream_log::c_eMaxSize);
// Methods
public:
    bool                                            isValid() const;
    void                                            init(const b_path&, const size_t sLogSize = stream_log::c_eMaxSize);                                        ///< initialize logger by path
    void                                            setLevel(enu_log_level);

    template <typename T>
    CDebugLogger&                                   operator << (const T&);                                                                                      ///< default serializing operator
    CDebugLogger&                                   operator << (const level_wrapper&);
    CDebugLogger&                                   operator << (ostream_manipulator);                                                                           ///< manipulator version (std::endl for example)
    operator                                        std::ostream& ();                                                                                            ///< ostream ptr (for ostream_iterator)
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLogger::CDebugLogger(enu_log_level eLvl)
    : m_pLoggerCore(createNullLogger()),
      m_eLogLvl(eLvl),
      m_eMsgLvl(m_eLogLvl)
{

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLogger::CDebugLogger(const b_path& phLog, const size_t sLogSize)
    : m_pLoggerCore(createNullLogger()),
      m_eLogLvl(normal_lvl),
      m_eMsgLvl(m_eLogLvl)
{
    init(phLog, sLogSize);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline bool CDebugLogger::isValid() const
{
    return boost::dynamic_pointer_cast<stream_logger_core>(m_pLoggerCore) != NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline void CDebugLogger::init(const b_path& phLog, const size_t sLogSize)
{
    if(phLog.empty())
        return;

    // resolving modeinfo paths
    plug_key::CModeInfoPlug ModeInfo;
    if(!ModeInfo.Load())
        return;

    std::string strExpandedPh(phLog.string());
    ModeInfo.ExpandString(strExpandedPh);
    ModeInfo.Free();

    ptr_logger_core pStreamLogger = createStreamLogger(strExpandedPh, sLogSize);
    if(pStreamLogger)
        m_pLoggerCore.swap(pStreamLogger);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline void CDebugLogger::setLevel(enu_log_level eLevel)
{
    m_eLogLvl = eLevel;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
inline CDebugLogger& CDebugLogger::operator<< (const T& tValue)
{
    if(m_eMsgLvl >= m_eLogLvl)
        *m_pLoggerCore << tValue;

    return *this;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLogger& CDebugLogger::operator<< (const level_wrapper& lvlWrapper)
{
    m_eMsgLvl = lvlWrapper.m_eLvl;
    return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLogger& CDebugLogger::operator<< (ostream_manipulator manipulator)
{
    if(m_eMsgLvl >= m_eLogLvl)
        *m_pLoggerCore << manipulator;

    m_eMsgLvl = m_eLogLvl;
    return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLogger::operator std::ostream&()
{
    return *m_pLoggerCore;
}

}           // logger

// Typedefs
typedef logger::CDebugLogger                        debug_logger;
typedef debug_logger&                               ref_debug_logger;

#endif      // DebugLogger_h
