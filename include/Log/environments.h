#pragma once

#include <map>
#include <string>
//#include <fstream>
#include <reg_12.h>

#ifdef WIN32
#include <winsock2.h>
#include <winbase.h>
#endif //WIN32

namespace modeinfo
{
	class CEnvironments
	{
	public:
		CEnvironments();

		void Expand(std::string&);
		//	void Compact(std::string&);

		void InitLocal(std::string fileName = "$(NITAROOT)/config/system.xml");

		std::map<std::string, std::string> environmentMap;
	};

	extern CEnvironments g_Environments;


	std::string get_NITAROOT();
	std::string get_NITADATA();
	std::string get_NITASTORAGE();
	std::string get_NITADATAPLAY();

	std::string get_NITAROOT();		//   	- (������ WINDOWS c:\nita, UNIX /soft) 
	std::string get_NITADATA();		//         - (������ WINDOWS c:\nitadata, UNIX /soft/nitadata )
	std::string get_NITASTORAGE();	//  - (������ WINDOWS c:\nitastorage, UNIX /soft/nitastorage )

	std::string get_NITADATAPLAY();	// - (������ WINDOWS c:\nitadataplay, UNIX /soft/nitadataplay )
		//	�����, ���� ������������ ������ � ������ ���������������

	std::string get_NITAETC();		//  - (������ WINDOWS %NITAROOT%\config, UNIX %NITAROOT%\etc)
		// �����, ��� �������� ���������������� �����
	std::string get_NITARES();		// - �����, ��� �������� ����� �������� ��� ��������
		//����� ����������, ������, svg, audio �����
	std::string get_NITA_Translation();// ����� ����������
	std::string get_NITA_Image();	// ����� ����������� ������ (������, svg�)
	std::string get_NITA_Sound();	// ����� ��������� ����������.


	///////////////////////////////////////////////////////////

	inline CEnvironments::CEnvironments()
	{
		char buf[64];
		unsigned long dwBufSize = 64;
		GetComputerName(buf, &dwBufSize);
		environmentMap["COMPNAME"] = buf;
	}

	inline void CEnvironments::Expand(std::string& str)
	{
		std::string out;
		size_t t = std::string::npos;

		bool isPreviouseUSD(false);
		int IsPreviousePercent(false);

		for (unsigned i(0); i < str.size(); ++i)
		{
			if (str[i] == '\\') str[i] = '/';
			if (isPreviouseUSD && str[i] == '(')
			{
				unsigned k(i+1);
				for (; k < str.size(); ++k)
				{
					if (str[k] == ')') break;
				}

				if (k < str.size())
				{
					std::string strKey(str.substr(i+1, k-i-1));

					if (const char* ch = getenv(strKey.c_str()))
					{
						std::string strVal(ch);
						str.replace(i-1, k-i+2, strVal);
						i += (int)strVal.size() - 2;
						continue;
					}
					else
					{
						std::map<std::string, std::string>::iterator it(environmentMap.find(strKey));

						if (it != environmentMap.end())
						{
							std::string strVal(it->second);
							str.replace(i-1, k-i+2, strVal);
							i += (int)strVal.size() - 2;
							continue;
						}
					}
				}
			}
			if (IsPreviousePercent && str[i] == '%')
			{
				unsigned k(i+1);
				for (; k+1 < str.size(); ++k)
				{
					if (str[k] == '%' && str[k+1] == '%') break;
				}

				if (k < str.size())
				{
					std::string strKey(str.substr(i+1, k-i-1));

					if (const char* ch = getenv(strKey.c_str()))
					{
						std::string strVal(ch);
						str.replace(i-1, k-i+3, strVal);
						i += (int)strVal.size() - 2;
						continue;
					}
				}
			}
			if (str[i] == '[')
			{
				unsigned k(i+1);
				for (; k < str.size(); ++k)
				{
					if (str[k] == ']') break;
				}
				if (k < str.size())
				{
					std::string strKey(str.substr(i+1, k-i-1));
					{
						std::map<std::string, std::string>::iterator it(environmentMap.find(strKey));

						if (it != environmentMap.end())
						{
							std::string strVal(it->second);
							str.replace(i, k-i+1, strVal);
							i += (int)strVal.size() - 1;
							continue;
						}
					}
				}
			}

			if (str[i] == '$') isPreviouseUSD = true;
			else isPreviouseUSD = false;		
			if (str[i] == '%' ) IsPreviousePercent = true;
			else IsPreviousePercent = false;
		}
	}
	
	inline void CEnvironments::InitLocal(std::string fileName)
	{
		Expand(fileName);

		registry::CXMLProxy proxy;
		if (proxy.load(fileName))
		{
			unsigned num = proxy.subnode_number(proxy.get_root());
			for (unsigned i(0); i < num; ++i)
			{
				registry::reg_marker marker = proxy.get_subnode(proxy.get_root(), i);
				std::string key, val;
				proxy.get_name(marker, key);
				proxy.get_attr(marker, "v", val);

				if (key.find("LoadFile") == 0)
				{
					InitLocal(val);
				}
				else environmentMap[key] = val;
			}
		}
	}

	inline bool test_CEnvironments()
	{
		CEnvironments env;
		{
			std::string str("$(NITAROOT)");
			env.Expand(str);
			assert(str == "C:\\Nita");

			std::string str1("1$(NITAROOT)");
			env.Expand(str1);
			assert(str1 == "1C:\\Nita");

			std::string str2("$(NITAROOT)1");
			env.Expand(str2);
			assert(str2 == "C:\\Nita1");

			std::string str3("$(NITAROOT)$(NITAROOT)");
			env.Expand(str3);
			assert(str3 == "C:\\NitaC:\\Nita");

			std::string str4("[COMPNAME]");
			env.Expand(str4);
			assert(str4 == "AL");

			std::string str5("[COMPNAME]/[COMPNAME]");
			env.Expand(str5);
			assert(str5 == "AL/AL");

		}
		std::string str("ghdsfjs\\hgfjdkbvg\\$(NITAROOT)\\$(NITADATA)\\$(xxx)\\$(COMPNAME)");
		env.Expand(str);
		std::string str1("ghdsfjs\\hgfjdkbvg\\%%NITAROOT%%\\%%NITADATA%%\\$(xxx)\\[COMPNAME]");
		env.Expand(str1);
		assert(str == str1);
		env.InitLocal();
		std::string str2("[WS]");
		env.Expand(str2);
		assert(str2 == "SRV");
		str = "$(xxx)";
		env.Expand(str);
		str = "zzz";
		return true;
	}

}

/*
inline void CEnvironments::ProcessFile(const char* fileName)
{
std::ifstream ifs(fileName);

if (!ifs) return;

std::string str;

while (!getline(ifs, str).fail())
{
std::istringstream isst(str);
std::string key, val;
getline(isst, key, '=');
getline(isst, val);

if (key == "LoadFile")
{
Expand(val);
ProcessFile(val.c_str());
}
else environmentMap[key] = val;
}
}

*/