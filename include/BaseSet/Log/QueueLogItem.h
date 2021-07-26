/******************************************************************************
 File     : QueueLogItem.h
 Author   : SDY
 Date     : 26.07.2005
 Comments : 
 
******************************************************************************/

#ifndef __QueueLogItem_h
#define __QueueLogItem_h

#include <sstream>

#include "../File/FileTime.h"

#define QLI_FLAG_EOL		0x0001
#define QLI_FLAG_OBJ		0x0002
#define QLI_FLAG_DATE		0x0004
#define QLI_FLAG_TIME		0x0008
#define QLI_FLAG_TIME_MS	0x0010
#define QLI_FLAG_ALL		0xFFFF

namespace b_set
{
	///////////////////////////////////////////////////////////////////////////////
	// CQueueLogItem
	class b_DLL CQueueLogItem
	{
	public:
		CFileTime	m_ftTimeMark;
		CString 	m_strObjName;
		CString 	m_strText;

	public:
		std::string GetText(DWORD dwFlags = QLI_FLAG_ALL) const
		{
			std::stringstream ss;
			
			if(dwFlags & QLI_FLAG_DATE)
			{
				ss << (LPCSTR)m_ftTimeMark.GetStringDate(TRUE) << " ";
			}

			if(dwFlags & QLI_FLAG_TIME)
			{
				if(dwFlags & QLI_FLAG_TIME_MS)
					ss << (LPCSTR)m_ftTimeMark.GetStringFullTime(TRUE);
				else
					ss << (LPCSTR)m_ftTimeMark.GetStringTime(TRUE);
			}
	
			if(dwFlags & (QLI_FLAG_DATE|QLI_FLAG_TIME) )
				ss << "> ";

			if(dwFlags & QLI_FLAG_OBJ)
			{
				if( !m_strObjName.IsEmpty() )
				{
					ss << (LPCSTR)m_strObjName << "> ";
				}
			}

			ss << (LPCSTR)m_strText;

			if(dwFlags & QLI_FLAG_EOL)
			{
				ss << "\r\n";
			}

			return ss.str();
		}

		void CopyFrom(CQueueLogItem& item)
		{
			m_ftTimeMark = 	item.m_ftTimeMark;
			m_strObjName = 	item.m_strObjName;
			m_strText = 	item.m_strText;
		}
	};

} //! namespace b_set

#endif //! __QueueLogItem_h
