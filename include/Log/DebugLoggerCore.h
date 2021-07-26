#ifndef DebugLoggerCore_h
#define DebugLoggerCore_h

/**
 * @author gda
 */

// prj
#include <Log/StreamLog.h>

// boost

#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/null.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic pop
#endif

namespace logger
{

//===========================================================================================================================================================
// fwd
class CDebugLoggerCore;
typedef boost::shared_ptr<CDebugLoggerCore>         ptr_logger_core;

ptr_logger_core                                     createNullLogger();
ptr_logger_core                                     createStreamLogger(const std::string&);

/**
 * @brief The CDebugLoggerCore class                abstract logger core (incapsulates output logic)
 */
class CDebugLoggerCore: public boost::noncopyable
{
// Typedefs
public:
    typedef std::ostream& (*ostream_manipulator)(std::ostream&);
// Dtor
public:
    virtual                                         ~CDebugLoggerCore()
    {
    }
protected:
    CDebugLoggerCore()
    {
    }
// Methods
public:
    template <typename T> CDebugLoggerCore&         operator<< (const T&);

    CDebugLoggerCore&                               operator<< (ostream_manipulator);
    virtual operator                                std::ostream&()                     = 0;
private:
    virtual void                                    _serialize(const std::string&)      = 0;
    virtual void                                    _serialize(ostream_manipulator)     = 0;
};


//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
inline CDebugLoggerCore&  CDebugLoggerCore::operator<< (const T& tValue)
{
    std::ostringstream oss;
    oss << tValue;

    _serialize(oss.str());
    return *this;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <>
inline CDebugLoggerCore& CDebugLoggerCore::operator<<  (const boost::posix_time::time_duration& td)
{
    std::ostringstream oss;
    oss << td.hours() << ":" << td.minutes() << ":" << td.seconds();

    _serialize(oss.str());
    return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline CDebugLoggerCore& CDebugLoggerCore::operator<< (ostream_manipulator manipulator)
{
    _serialize(manipulator);
    return *this;
}
//===========================================================================================================================================================
/**
 * @brief template CDebugLoggerImpl          - implementation of logger core
 */
template <typename tLogger>
class CDebugLoggerImpl: public CDebugLoggerCore
{
// Attributes
private:
    tLogger                             m_Logger;
// Ctor
private:
    explicit                            CDebugLoggerImpl(const b_path&, const size_t);
    explicit                            CDebugLoggerImpl();
// Methods
public:
    friend  ptr_logger_core             createNullLogger();
    friend  ptr_logger_core             createStreamLogger(const b_path&, const size_t);

    virtual operator                    std::ostream&();
private:
    virtual void                        _serialize(const std::string&);
    virtual void                        _serialize(ostream_manipulator);
};

// Typedefs
typedef CDebugLoggerImpl<boost::iostreams::stream<boost::iostreams::null_sink> > null_logger_core;
typedef CDebugLoggerImpl<stream_log>                                             stream_logger_core;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <>
inline stream_logger_core::CDebugLoggerImpl(const b_path& phLog, const size_t sLogSize)
    : m_Logger(phLog, sLogSize)
{
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <>
inline CDebugLoggerImpl<boost::iostreams::stream<boost::iostreams::null_sink> >::CDebugLoggerImpl()
    : m_Logger(boost::iostreams::null_sink())
{
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline ptr_logger_core                         createNullLogger()
{
    try
    {
        return ptr_logger_core(new null_logger_core);
    }
    catch(...)
    {
        return ptr_logger_core(static_cast<null_logger_core*>(0));
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
inline ptr_logger_core                         createStreamLogger(const b_path& phLog, const size_t sLogSize)
{
    try
    {
        return ptr_logger_core(new stream_logger_core(phLog, sLogSize));
    }
    catch(...)
    {
        return ptr_logger_core(static_cast<stream_logger_core*>(0));
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLog>
inline CDebugLoggerImpl<tLog>::operator std::ostream&()
{
    return m_Logger;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLog>
inline void CDebugLoggerImpl<tLog>::_serialize(const std::string& strText)
{
    m_Logger << strText;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
template <typename tLog>
inline void CDebugLoggerImpl<tLog>::_serialize(ostream_manipulator Manipulator)
{
    m_Logger << Manipulator;
}

}           // logger

#endif      // DebugLoggerCore_h
