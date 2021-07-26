#ifndef Loggable_h
#define Loggable_h

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
#include <boost/type_traits/remove_reference.hpp>
#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#pragma GCC diagnostic pop
#endif

// fwd
namespace logger
{

/**
 * @brief The CLoggable template    - wrapper for inheritance and logging data
 */

template <typename tLogObj>
class CLoggable
{
// Attributes
private:
    mutable tLogObj                 m_LogObj;
// Ctor
protected:
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
    explicit                        CLoggable(tLogObj rLogObj)
        : m_LogObj(rLogObj)
    {
    }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
    virtual                         ~CLoggable()
    {
    }
protected:
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual typename boost::remove_reference<tLogObj>::type&                _log() const
    {
        return m_LogObj;
    }
};

template <typename tLogObj>
class CLoggable<tLogObj&>
{
// Attributes
private:
    tLogObj&                         m_LogObj;
// Ctor
protected:
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
    explicit                        CLoggable(tLogObj& rLogObj)
        : m_LogObj(rLogObj)
    {
    }
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
    virtual                         ~CLoggable()
    {
    }
protected:
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
        virtual tLogObj&            _log() const
    {
        return m_LogObj;
    }
};

//===========================================================================================================================================================
class CLoggableObj: public CLoggable<debug_logger>
{
// Ctor
protected:
    explicit                            CLoggableObj(const b_path& phLog)
        : CLoggable<debug_logger>(debug_logger(phLog))
    {
    }
    virtual                             ~CLoggableObj()
    {

    }
};


typedef CLoggable<ref_debug_logger>     loggable_ref_obj;
typedef CLoggableObj                    loggable_obj;

}           // logger

#endif      // Loggable_h
