/******************************************************************************
 File     : MultiLanguage.h
 Author   : SDY
 Date     : 13.01.2006
 Comments : 
 
******************************************************************************/

#ifndef __MultiLanguage_h
#define __MultiLanguage_h

#ifndef WIN32
#define LANG_ENGLISH                     0x09
#define SUBLANG_ENGLISH_US               0x01    // English (USA)
#define SUBLANG_DEFAULT                  0x01    // user default
#define LANG_RUSSIAN                     0x19
#define MAKELANGID(p, s) 		((((WORD  )(s)) << 10) | (WORD  )(p))
#endif //! WIN32

#include "Array/CString.h"

namespace b_set
{

	enum ENU_ML{ML_RUS, ML_ENG};
	extern int g_iLanguage;

	inline BOOL IsEnglishMode()
	{
		return g_iLanguage == ML_ENG;
	}
	inline WORD GetResourceLangID()
	{
		if (g_iLanguage == ML_ENG)
			return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		else
			return MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
	}

	inline LPCTSTR ML_TEXT(LPCTSTR lpTextRus, LPCTSTR lpTextEng)
	{
		if (g_iLanguage == ML_ENG)
			return lpTextEng;
		else
			return lpTextRus;
	}

#ifdef UNICODE
	inline LPSTR ML_TEXT(LPSTR lpTextRus, LPSTR lpTextEng)
	{
		if (g_iLanguage == ML_ENG)
			return lpTextEng;
		else
			return lpTextRus;
	}
#else
	inline LPCWSTR ML_TEXT(LPCWSTR lpTextRus, LPCWSTR lpTextEng)
	{
		if (g_iLanguage == ML_ENG)
			return lpTextEng;
		else
			return lpTextRus;
	}
#endif

} //! namespace b_set

#endif // __MultiLanguage_h
