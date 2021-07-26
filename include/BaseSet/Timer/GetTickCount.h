/******************************************************************************
 File     : GetTickCount.h
 Author   : SDY
 Date     : 26.01.2002
 Comments : 
 
******************************************************************************/

#ifndef __GetTickCount_h
#define __GetTickCount_h

#ifdef WIN32
	#include <sys/time.h>

	inline unsigned int GetTickCount()
	{
		#ifndef WIN32
			struct timeval tvalbuf;
			if ( gettimeofday( &tvalbuf, 0 ) == 0L )
				return (unsigned int)( tvalbuf.tv_sec * 1000 + tvalbuf.tv_usec/1000 );
			else
				return 0;
		#endif //! WIN32
	}

#endif//WIN32

#endif //! __GetTickCount_h
