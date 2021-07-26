#include <boost/scoped_ptr.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace b_set
{

class CInstanceHolder
{
protected:
	boost::scoped_ptr<boost::interprocess::file_lock>	m_pFLock;
	boost::filesystem::path								m_pathFile;
	bool												m_bLocked;

public:
	CInstanceHolder()
	{
		m_bLocked = false;
	}
	~CInstanceHolder()
	{
		Unlock();
	}
	bool CheckFileName( const char* pFileName );
	bool Lock();
	void Unlock();
};

inline bool CInstanceHolder::CheckFileName( const char* pFileName )
{
	bool bRes = true;
	m_pathFile = pFileName;
    using namespace boost::filesystem;
    try
    {
        if( !exists( m_pathFile ) )
        {
            create_directories( m_pathFile.parent_path() );
            std::ofstream( m_pathFile.string().c_str() );
        }
		m_pFLock.reset( new boost::interprocess::file_lock( pFileName ) );
    }
    catch( filesystem_error& err )
    {
		bRes = false;
		std::stringstream ss;
		ss << "ERR> " << "CInstanceHolder::CheckFileName> " << err.what();
		std::cout << ss.str() << std::endl;
    }
	return bRes;
}

inline bool CInstanceHolder::Lock()
{
	if( !m_pFLock )
		return false;

	if( m_bLocked )
		Unlock();

	bool bRes = false;
	using namespace boost::interprocess;
	try
	{
		m_bLocked = m_pFLock->try_lock();
	}
	catch( interprocess_exception& err )
	{
		std::stringstream ss;
		ss << "ERR> " << "CInstanceHolder::Lock> " << err.what();
		std::cout << ss.str() << std::endl;
	}
	return m_bLocked;
}

inline void CInstanceHolder::Unlock()
{
	if( !m_pFLock )
		return;

	if( m_bLocked )
	{
		using namespace boost::interprocess;
		try
		{
			m_pFLock->unlock();
		}
		catch( interprocess_exception& err )
		{
			std::stringstream ss;
			ss << "ERR> " << "CInstanceHolder::Unlock> " << err.what();
			std::cout << ss.str() << std::endl;
		}
		m_bLocked = false;
	}
}

}//namespace b_set