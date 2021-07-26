#ifndef AFX_STDAFX_H__1234577_18FA32F9_C11C_4E69_8C1E_E0EB9719200A__INCLUDED_
#define AFX_STDAFX_H__1234577_18FA32F9_C11C_4E69_8C1E_E0EB9719200A__INCLUDED_

#ifdef WIN32
	#ifdef WINNT40
		#define _WIN32_WINNT	0x0401
		#define WINVER			0x0401
		#pragma warning(disable: 4786 4355)
		#define REGISTRYPROXY_IGNORE_DLL
		#define REGISTRYPROXY_SOURCE_IMPORT
	#endif//WINNT40

	#include <winsock2.h>
#else
	#include "BaseTypes_x.h"
#endif //! WIN32

#include <string>
#include <sstream>

#define BETA_SET_OBJ_OVERWRITTEN

#ifdef  WINNT40
	#define _tstof		atof
	#define _tstoi64	_atoi64
#else //WINNT40
#endif//WINNT40


#endif //! AFX_STDAFX_H__1234577_18FA32F9_C11C_4E69_8C1E_E0EB9719200A__INCLUDED_
