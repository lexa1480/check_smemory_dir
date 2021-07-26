 // NitaMisc.h: some inline funcs
//
//////////////////////////////////////////////////////////////////////

#if !defined	(__NitaMisc_H)
#define			 __NitaMisc_H

#include <string.h>

namespace b_set
{

inline int n_stricmp( const char* pStrFirst, const char* pStrSecond )
{
#ifdef  WIN32		
	return _stricmp( pStrFirst, pStrSecond );
#else //WIN32
	return strcasecmp( pStrFirst, pStrSecond );
#endif//WIN32
}

typedef std::vector <std::string>::iterator						V_STRING_ITER;

inline std::vector <std::string>::iterator n_find_no_case( std::vector <std::string>& vStrings, const char* pStrToFind ) 
{
	std::vector <std::string>::iterator iterRes = vStrings.end();
	std::string sStr = pStrToFind;

	for (size_t i = 0; i< vStrings.size(); i++)
	{
		std::string sItem = vStrings[i];
		if( 0 == n_stricmp( sItem.c_str(), sStr.c_str() ) )
		{
			iterRes = vStrings.begin() + i;
			break;
		}
	}
	
	return iterRes;
}

}//namespace b_set

#endif // !defined(__NitaMisc_H)
