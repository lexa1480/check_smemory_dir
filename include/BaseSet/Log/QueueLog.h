/******************************************************************************
 File     : QueueLog.h
 Author   : SDY
 Date     : 04.07.2005
 Comments : 
 
******************************************************************************/

#ifndef __QueueLog_h
#define __QueueLog_h

#include "QueueLogItem.h"
#include "LogFile.h"
#include "../Queue/QueueStar.h"

namespace b_set
{
	typedef CPointerArray<CQueueLogItem> CPointerArrayLogItems;
	///////////////////////////////////////////////////////////////////////////////
	// CQueueLog
	class b_DLL CQueueLog : public T_CQueueStar<CQueueLogItem>
	{
	////////// Attributes:
	public:
	protected:
		CLogFileObj 		m_lfFile;
		BOOL 			m_bLogFileIsEnabled;
		CFileTime 		m_ftLastRecordTime;
		CCriticalSectionObj 	m_csLog;

	////////// Construction:
	public:
		CQueueLog()
		{
			SetSize(50);
			m_bLogFileIsEnabled = TRUE;
		}
		~CQueueLog()
		{
		}

	////////// Operations:
	public:
		void EnableLogFile(BOOL bEnable);
		void LogRecord(CQueueLogItem* pRecord);
		void LogText(LPCTSTR lpText, LPCTSTR lpObjName = NULL);
		void GetItems(CPointerArrayLogItems& arr, UINT64 u64Time);
		void SetFileName(const char* lpFullFileName);
		void SetFileName(const char* lpPath, const char* lpFileName);
		void SetFileName(const char* lpPath, const char* lpFileName, const char* plExtension);
	protected:

	};

	///////////////////////////////
	// CQueueLog in lines
	inline void CQueueLog::SetFileName(const char* lpFullFileName)
	{
		m_lfFile.SetFileName(lpFullFileName);
	}
	inline void CQueueLog::SetFileName(const char* lpPath, const char* lpFileName)
	{
		m_lfFile.SetFileName(lpPath, lpFileName);
	}
	inline void CQueueLog::SetFileName(const char* lpPath, const char* lpFileName, const char* plExtension)
	{
		m_lfFile.SetFileName(lpPath, lpFileName, plExtension);
	}
	inline void CQueueLog::EnableLogFile(BOOL bEnable)
	{
		m_bLogFileIsEnabled = bEnable;
	}
	inline void CQueueLog::LogRecord(CQueueLogItem* pRecord)
	{
		if(pRecord == NULL)
			return;

		m_csLog.Lock();								//{
		CQueueLogItem* pItem = GetNextEmpty();
		if(pItem)
		{
			pItem->CopyFrom(*pRecord);
			if(m_ftLastRecordTime && m_ftLastRecordTime >= pItem->m_ftTimeMark)
			{
				m_ftLastRecordTime = m_ftLastRecordTime + 1;
				pItem->m_ftTimeMark = m_ftLastRecordTime;
			}
			else
			{
				m_ftLastRecordTime = pItem->m_ftTimeMark;
			}

			SetCurFull();
		}
		
		if(pItem && m_bLogFileIsEnabled)
			m_lfFile.LogText( pItem->GetText().c_str() );

		m_csLog.Unlock();							//}
	}
	
	inline void CQueueLog::LogText(LPCTSTR lpText, LPCTSTR lpObjName)
	{
		if( lpText == NULL )
			return;

		m_csLog.Lock();								//{
		CQueueLogItem* pItem = GetNextEmpty();
		if(pItem)
		{
			pItem->m_ftTimeMark.SystemTime();
			pItem->m_strText = lpText;
			if(lpObjName)
				pItem->m_strObjName = lpObjName;
			else
				pItem->m_strObjName = _T("");
			if(m_ftLastRecordTime && m_ftLastRecordTime >= pItem->m_ftTimeMark)
			{
				m_ftLastRecordTime = m_ftLastRecordTime + 1;
				pItem->m_ftTimeMark = m_ftLastRecordTime;
			}
			else
			{
				m_ftLastRecordTime = pItem->m_ftTimeMark;
			}

			SetCurFull();
		}

		if(pItem && m_bLogFileIsEnabled)
			m_lfFile.LogText( pItem->GetText().c_str() );

		m_csLog.Unlock();							//}
	}

	inline void CQueueLog::GetItems(CPointerArrayLogItems& arr, UINT64 u64Time)
	{
		m_csLog.Lock();								// {
		LockItemChange();							// {
		
		CQueueLog::item_type* pItemLast = (CQueueLog::item_type*)m_pCurrentItemIndex;
		if(pItemLast)
		{
			CQueueLog::item_type* pItem = (CQueueLog::item_type*)pItemLast->GetNext();
			
			for(int i=0; i<m_arrIndexes.GetSize(); i++)
			{
				if( pItem->HasData() )
				{
					CQueueLogItem* pData = pItem->GetData();
					if( pData && ( pData->m_ftTimeMark > u64Time ) )
					{
						CQueueLogItem* pResItem = new CQueueLogItem();
						pResItem->CopyFrom( *pData );
						if( arr.Add(pResItem) == -1)
							delete pResItem;
					}
				}
				pItem = (CQueueLog::item_type*)pItem->GetNext();
				if(pItem == pItemLast)
					break;
			}
		}
		
		UnlockItemChange();							// }
		m_csLog.Unlock();							// }
	}

} //! namespace b_set

#endif //! __QueueLog_h
