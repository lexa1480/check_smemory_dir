#pragma once
#include <map>
#include <string>
#include <NitaDataTypes.h>
#include <reg_12.h>		// in linux must add LIBS += -lreg_12
#include <ModeInfo.h>	// for expand string

//for test 
static const char* const st_LogClassesClassName = "Class";	///< parameter name of class
static const char* const st_LogClassesTypeName  = "Type";		///< parameter name of message
static const char* const st_LogClassesEnum	  = "enum";		///< parameter name of enum
static const char* const st_LogClassesText	  = "text";		///< parameter name of text
static const char* const st_LogClassesObject	  = "object";	///< parameter name of object
static const char* const st_LogClassesBody	  = "body";	///< parameter name of body - in fact text

static const char st_cSepSymbol='%';					///< separator for description type of param

static const char* const st_PlaceForLogClasses="%%NITAETC%%/_System/log/"; ///< place where all params

//for test 
#define LOG_CLASSES_USER_TYPE_SHUTDOWN 100
#define LOG_CLASSES_USER_TYPE_REBOOT 101
#define LOG_CLASSES_USER_TYPE_SET_OFF_DIAG 200
#define LOG_CLASSES_USER_TYPE_SET_ON_DIAG 201
#define LOG_CLASSES_USER_TYPE_SET_RADAR_OFF 200
#define LOG_CLASSES_USER_TYPE_SET_RADAR_ON 201

enum CommonClasses
{
	lc_ClassesProgramm = 0,		///< prog_log.xml
	lc_DiagClasses     = 11,	///< diag_log.xml
	lc_DiagDevice      = 12,	//< device_diag_log.xml
	lc_Ksz			   = 13,    //< sphere message,
	lc_User            = 14		//user generated messages
};

struct CPacketLogFilterKey
{
	CPacketLogFilterKey(NShort uiClass, NShort uiId)
		: m_uiClass(uiClass), m_uiID(uiId)
	{
	}
	const unsigned int m_uiClass;
	const unsigned int m_uiID;
	bool operator < (const CPacketLogFilterKey& pack)const
	{
		if (m_uiClass!=pack.m_uiClass)
			return m_uiClass<pack.m_uiClass;
		return m_uiID<pack.m_uiID;
	}
};

class CLoggerInitMap
{
public:
	CLoggerInitMap()
	{
		Init();
	}
	void Init()
	{
		m_mapClasses[lc_ClassesProgramm]    = "prog_log.xml";
		m_mapClasses[lc_DiagClasses]		= "diag_log.xml";
		m_mapClasses[lc_DiagDevice]			= "device_diag_log.xml";
		m_mapClasses[lc_User]				= "user_log.xml";
		m_mapClasses[lc_Ksz]				= "ksz_log.xml";
	}
	std::map<NShort,const char*> m_mapClasses;

};
static CLoggerInitMap st_logCMap;

/*
namespace
{
	CLoggerInitMap loggerInitMap;
}
*/



struct CLogIdElem
{
	std::string strEnum;
	std::string strText;
};

class CLogIdReader
{
public:
	CLogIdReader() {};
	~CLogIdReader(){};
	bool ReadFile(const std::string& strPath);
	bool ReadFile(NShort nId);
	std::map<NShort,CLogIdElem> m_mapIdMsg;
};

inline bool CLogIdReader::ReadFile(NShort nId)
{
	if (st_logCMap.m_mapClasses.find(nId)!=st_logCMap.m_mapClasses.end())
	{
		std::ostringstream ossLogId;
		ossLogId<<"%%NITAETC%%/_System/log/"<<st_logCMap.m_mapClasses[nId];
		std::string strHelpConv = ossLogId.str();
		plug_key::g_ModeInfoPlug.Load();
		plug_key::g_ModeInfoPlug.ExpandString(strHelpConv);
		plug_key::g_ModeInfoPlug.Free();
		return  ReadFile(strHelpConv);
	}
	return false;
}

inline bool CLogIdReader::ReadFile(const std::string& strPath)
{
	registry::CXMLProxy proxy;
	if ( proxy.load(strPath) )
	{
		registry::CNode nodeRoot(&proxy);
		for (int i=0; i<nodeRoot.getSubNodeCount(); i++)
		{
			registry::CNode nodeChild = nodeRoot.getSubNode(i);
			std::string strRes = nodeRoot.getSubNodeName(i);
			std::istringstream issOut(strRes);
			NShort nId=-1; 
			issOut>>nId;
			if (nId!=-1)
			{
				CLogIdElem elem;
				nodeChild.getValue(st_LogClassesEnum,elem.strEnum);
				nodeChild.getValue(st_LogClassesText,elem.strText);
				m_mapIdMsg[nId] = elem;
			}
			else
				return false;
		}
		return true;
	}
	else
		return false; 
	return false;
}

class CLogIdSystem
{
public:
	std::map<NShort,CLogIdReader> m_mapReader;
	std::map<std::string,std::pair<NShort,NShort> >		  m_mapHelpEnum;

	bool GetIdMessage(const std::string& strEnumMsg,NShort& idClass, NShort& idMsg);
	bool GetStringId(NShort idClass,NShort idMsg,std::string& strEnumMsg);		///< receive string by class and id
	
	//text function
	std::string GetText(const NShort idClass,const NShort idMsg,const char* lpBuffer=NULL,unsigned int uiSize=0);	///<may be another variant lpBuffer zero ended string with param (uiSize full length size)	
	std::string GetText(const NShort idClass,const NShort idMsg,std::map<std::string,std::string>& mapStr);			///<take text by map(id pole,value)

	bool GetParamMap(const NShort idClass,const NShort idMsg,const char* lpBuffer,unsigned int uiSize,std::map<std::string,std::string>* mapData);	///<function to get specific param to map(uiSize full length size)	

	CLogIdSystem(){};
	bool Init();
};

inline bool CLogIdSystem::Init()
{
	for(std::map<NShort,const char*>::iterator itCur = st_logCMap.m_mapClasses.begin();itCur!=st_logCMap.m_mapClasses.end();itCur++)
	{
		CLogIdReader reader;
		m_mapReader[itCur->first] = reader;
		m_mapReader[itCur->first].ReadFile(itCur->first);
		///mbp not optimal 
		for (std::map<NShort,CLogIdElem>::iterator itHlp = m_mapReader[itCur->first].m_mapIdMsg.begin();itHlp!=m_mapReader[itCur->first].m_mapIdMsg.end();++itHlp)
		{
			std::pair<NShort,NShort> pairIdMsg;
			pairIdMsg.first = itCur->first;
			pairIdMsg.second= itHlp->first;
			m_mapHelpEnum[itHlp->second.strEnum] = pairIdMsg;
		}

	}
	return true;
}

inline bool CLogIdSystem::GetStringId(NShort idClass, NShort idMsg,std::string& strEnumMsg)
{
	std::map<NShort,CLogIdReader>::iterator itClass = m_mapReader.find(idClass);
	if (itClass!=m_mapReader.end())
	{
		std::map<NShort,CLogIdElem>::iterator itElem = itClass->second.m_mapIdMsg.find(idMsg);
		if (itElem!=itClass->second.m_mapIdMsg.end())
		{
			strEnumMsg = itElem->second.strEnum;
			return true;
		}
	}
	return false;
}
		///< receive string by class and id


inline bool CLogIdSystem::GetIdMessage(const std::string& strEnumMsg,NShort& idClass, NShort& idMsg)
{
	std::map<std::string,std::pair<NShort,NShort> >::iterator itFind = m_mapHelpEnum.find(strEnumMsg);
	if ( itFind!=m_mapHelpEnum.end() )
	{
		std::pair<NShort,NShort> pairIdMsg;
		pairIdMsg = itFind->second;
		idClass   = pairIdMsg.first;
		idMsg     = pairIdMsg.second;
		return true;
	}
	else
	{
		idClass   = -1;
		idMsg     = -1;
		return false;
	}
	return false;
}


inline std::string CLogIdSystem::GetText(const NShort idClass,const NShort idMsg,const char* lpszParam,unsigned int uiSize)
{
	std::map<NShort,CLogIdReader>::iterator itClass = m_mapReader.find(idClass);
	std::string strText= "";
	if (itClass!=m_mapReader.end())
	{
		std::map<NShort,CLogIdElem>::iterator itId = itClass->second.m_mapIdMsg.find(idMsg);
		if (itId != itClass->second.m_mapIdMsg.end())
		{
			strText = itId->second.strText;
			if (lpszParam)
			{
				std::string strFullText;
				size_t tShift=0;
				size_t tBPos =strText.find(st_cSepSymbol);
				size_t tEPos =strText.find(st_cSepSymbol,tBPos+1);
				size_t tFnd=0;
				while ((tBPos!=std::string::npos)&&(tEPos!=std::string::npos))
				{
					strFullText += strText.substr(tFnd,tBPos-tFnd);
					unsigned int tRes = (strlen(lpszParam+tShift)+1);
					std::string strParam;
					strParam.assign(lpszParam+tShift);
					tShift +=tRes;
					strFullText+=strParam;
					tFnd=tEPos+1;
					if (tShift>=uiSize)
					{
						// all data for param readed
						break;
					}
					tBPos =strText.find(st_cSepSymbol,tEPos+1);
					tEPos =strText.find(st_cSepSymbol,tBPos+1);
				}
				if (tFnd!=strText.size())
				{
					strFullText += strText.substr(tFnd,strText.size()-tFnd);
					strText = strFullText;
				}
				else
					strText = strFullText;
			}
		}
	}
	return strText;
}

inline std::string CLogIdSystem::GetText(const NShort idClass,const NShort idMsg,std::map<std::string,std::string>& mapStr)
{
	std::map<NShort,CLogIdReader>::iterator itClass = m_mapReader.find(idClass);
	std::string strText= "";
	if (itClass!=m_mapReader.end())
	{
		std::map<NShort,CLogIdElem>::iterator itId = itClass->second.m_mapIdMsg.find(idMsg);
		if (itId != itClass->second.m_mapIdMsg.end())
		{
			strText = itId->second.strText;
			if (mapStr.size())
			{
				std::string strFullText;
				size_t tBPos =strText.find(st_cSepSymbol);
				size_t tEPos =strText.find(st_cSepSymbol,tBPos+1);
				size_t tFnd=0;
				while ((tBPos!=std::string::npos)&&(tEPos!=std::string::npos))
				{
					strFullText += strText.substr(tFnd,tBPos-tFnd);

					std::string strParam;
					std::string strValue;
					strParam = strText.substr(tBPos+1,tEPos-tBPos-1);
					std::map<std::string,std::string>::iterator itFnd = mapStr.find(strParam);
					if (itFnd!=mapStr.end())		
						strFullText += itFnd->second;
					tFnd=tEPos+1;
					tBPos =strText.find(st_cSepSymbol,tEPos+1);
					if (tFnd==strText.size())
					{
						break;
					}
					else
					{
						tEPos =strText.find(st_cSepSymbol,tBPos+1);
					}
				}
				if (tFnd!=strText.size())
				{
					strFullText += strText.substr(tFnd,strText.size()-tFnd);
					strText = strFullText;
				}
				else
				{
					strText =strFullText;
				}
			}
		}
	}
	return strText;
}



inline bool CLogIdSystem::GetParamMap(const NShort idClass,const NShort idMsg,const char* lpszParam,unsigned int uiSize,std::map<std::string,std::string>* mapData)
{
	if (uiSize==0)
		return true;
	std::map<NShort,CLogIdReader>::iterator itClass = m_mapReader.find(idClass);
	std::string strText= "";
	if (itClass!=m_mapReader.end())
	{
		std::map<NShort,CLogIdElem>::iterator itId = itClass->second.m_mapIdMsg.find(idMsg);
		if (itId != itClass->second.m_mapIdMsg.end())
		{
			strText = itId->second.strText;
			if (lpszParam)
			{
				std::string strValue;
				size_t tShift=0;
				size_t tBPos =strText.find(st_cSepSymbol);
				size_t tEPos =strText.find(st_cSepSymbol,tBPos+1);
				//size_t tFnd=0;
				while ((tBPos!=std::string::npos)&&(tEPos!=std::string::npos))
				{
					std::string strParam;
					std::string strValue;
					strParam = strText.substr(tBPos+1,tEPos-tBPos-1);
					strValue.assign(lpszParam+tShift);
					(*mapData)[strParam]=strValue;
					unsigned int tRes = (strlen(lpszParam+tShift)+1);
					tShift +=tRes;
					//tFnd=tEPos+1;
					if (tShift>=uiSize)
					{
						// all data for param readed
						break;
					}
					tBPos =strText.find(st_cSepSymbol,tEPos+1);
					tEPos =strText.find(st_cSepSymbol,tBPos+1);
				}
				return true;
			}
		}
	}
	return false;

}

