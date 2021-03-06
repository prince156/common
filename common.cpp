// common.cpp : Defines the entry point for the console application.
//
#ifdef __LINUX__
#include <unistd.h>
#else
#include <Windows.h>
#endif

#include <ctime>
#include <cstdint>
#include <random>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
using namespace gcommon;


namespace gcommon
{
	uint16_t g_htons(const uint16_t data)
	{
		uint16_t tmp = (data & 0xff00) >> 8;
		tmp |= (data & 0x00ff) << 8;
		return tmp;
	}

	uint16_t g_ntohs(const uint16_t data)
	{
		return g_htons(data);
	}

	uint32_t g_htonl(const uint32_t data)
	{
		uint32_t tmp = (data & 0xff000000) >> 24;
		tmp |= (data & 0x00ff0000) >> 8;
		tmp |= (data & 0x0000ff00) << 8;
		tmp |= (data & 0x000000ff) << 24;
		return tmp;
	}

	uint32_t g_ntohl(const uint32_t data)
	{
		return g_htonl(data);
	}

	/********************************************************************
	* [函数名]: GetParaFromARG
	* [描述]: 从参数列表中获取指定的参数
	* [输入]
	*   argc：参数数量
	*   argv：参数列表
	*   prefix：参数前导符，可以为NULL或者空字符串""
	*   pos：指定是第几个符合条件的参数，从1开始
	* [输出]
	*   out：获取的参数（无需释放）
	* [返回值]
	*   >0：成功，符合条件的参数数量
	*   0：失败，参数错误
	*   -1：失败，未找到prefix
	*   -2：失败，找到prefix，但后面无参数
	* [修改记录]
	*   2013-12-09,littledj: create
	*   2014-04-08,littledj: 单元测试，并修改
	*********************************************************************/
	int GetParaFromARG(int argc, tchar* argv[], tchar* prefix, tchar* &out, int pos)
	{
		int count = 0;
		bool bFind = false;
		bool bFindPrefix = false;

		// 参数检查
		if (argv == NULL || pos <= 0 ||
			argc <= 1 || argv[0] == NULL)
			return 0;
		if (prefix != NULL && prefix[0] != '-' && prefix[0] != 0)	//	前导符必须以'-'开始
			return 0;

		for (int i = 1; i < argc; i++)
		{
			// 参数为空
			if (argv[i] == NULL) return 0;

			// 去除起始空格
			while (argv[i][0] == ' ')
			{
				argv[i] += 1;
				if (argv[i][0] == 0) break;
			}
			if (argv[i][0] == 0) continue;

			// 寻找无前导参数
			if (prefix == NULL || prefix[0] == 0)
			{
				if (argv[i][0] != '-')	// 参数不应该以‘-’作为第一个字符
				{
					count++;
					if (count == pos)
					{
						out = argv[i];
						bFind = true;
					}
				}
				else
				{
					i++; // 判断为前导符，跳过一个参数
				}
			}
			else if (tcsncmp(argv[i], prefix, tcslen(prefix)) == 0 &&	// 寻找指定前导参数
				tcslen(argv[i]) == tcslen(prefix))
			{
				bFindPrefix = true;
				do
				{
					if (++i == argc)	// 判断后面是否还有参数
					{
						if (bFind) return count;
						else return -2;
					}
					if (argv[i] == NULL) return 0;	// 参数为空

													// 去除起始空格
					while (argv[i][0] == ' ')
					{
						argv[i] += 1;
						if (argv[i][0] == 0) break;
					}
				} while (argv[i][0] == 0);

				// 判断后面的参数是否还是前导符
				if (argv[i][0] != '-')
				{
					count++;
					if (pos == count)
					{
						bFind = true;
						out = argv[i];
					}
				}
				else
				{
					i++;
				}
			}
		}

		if (bFind)
			return count;	// 找到符合条件的参数
		else if (bFindPrefix)
			return -2;		// 只找到前导符
		else
			return -1;		// 什么也没找到
	}

	/********************************************************************
	/* 函数名: GetCurrentDirPath
	/* 描述: 获取当前可执行程序所在文件夹的全路径
	/* 输入:
	/*   无
	/* 输出:
	/*   tstring: 当前文件夹路径
	/* 修改记录:
	/*   2013-11-17,littledj: create
	/*   2013-12-07,littledj: 返回值改为CString，不再使用全局变量
	/*   2015-07-21,littledj; 返回值改为tstring,如果失败返回".\"
	/*   2016-07-08,littledj: 增加对linux的支持
	/********************************************************************/
	tstring GetCurrentDirPath()
	{
		tstring strCurrentDir;
		tchar* pCurrentDir = new tchar[MAX_PATH + 1];
		pCurrentDir[0] = 0;
#ifdef __LINUX__
		ssize_t count = readlink("/proc/self/exe", pCurrentDir, MAX_PATH);
		if (count <= 0)
		{
			delete[] pCurrentDir;
			return TEXT("./");
		}

		pCurrentDir[count] = 0;
		(strrchr(pCurrentDir, '/'))[1] = 0;
		strCurrentDir = pCurrentDir;
		strCurrentDir += "/";
		delete[] pCurrentDir;
#else
		uint32_t nRet = GetModuleFileName(NULL, pCurrentDir, MAX_PATH);
		if (nRet == 0)
		{
			delete[] pCurrentDir;
			return TEXT(".\\");
		}

		(tcsrchr(pCurrentDir, '\\'))[1] = 0;
		strCurrentDir = pCurrentDir;
		delete[] pCurrentDir;
#endif
		return strCurrentDir;
	}

	/********************************************************************
	* [函数名]: random
	* [描述]: 在[start,end)内(不包end, 最大0xffffffff)获取一个随机整数,start>0
	* [输入]:
	*   start: 随机数开始位置
	*   end: 随机数结束位置（不包括）
	* [返回值]: uint32_t
	*   生成的随机数
	* [修改记录]:
	*   2013-11-17,littledj: create
	*   2016-01-20,littledj: 优化srand参数
	*   2016-03-13,littledj: 参数和返回值都改为无符号，增大范围
	********************************************************************/
	uint32_t random(uint32_t start, uint32_t end)
	{
		// 参数检查
		if (end <= start)
			return start;

		// 第一次调用则初始化随机数种子
		bool static bFirst = true;
		if (bFirst)
		{
			char* chr = new char[1];
			srand((long)chr); // osx 上需要long
			bFirst = false;
		}

		int r1 = rand();
		int r2 = rand();
		int r3 = rand();
		int r4 = rand();
		int rr = (r1 & 0x000000ff) |
			(r2 & 0x000000ff) << 8 |
			(r3 & 0x000000ff) << 16 |
			(r4 & 0x000000ff) << 24;
		double rate = (end - start)*1.0 / (uint32_t)0xFFFFFFFF;
		uint32_t delta = (uint32_t)(rate * (uint32_t)rr);
		return start + delta;
	}

	/********************************************************************
	* [函数名]: inet_ltot
	* [描述]: 将long类型的ip地址转换成字符串（xxx.xxx.xxx.xx）
	* [输入]:
	*   ip：ip地址
	* [返回值]: tchar
	*   转换后的ip地址字符串
	* [修改记录]:
	*   2014-12-17,littledj: create
	********************************************************************/
	tchar* inet_ltot(unsigned long ip)
	{
		unsigned char chIP[4];
		chIP[0] = (unsigned char)(ip);
		chIP[1] = (unsigned char)(ip >> 8);
		chIP[2] = (unsigned char)(ip >> 16);
		chIP[3] = (unsigned char)(ip >> 24);

		static tchar strIP[16];
		memset(strIP, 0, 16 * sizeof(tchar));

		stprintf(strIP, TEXT("%u.%u.%u.%u"), chIP[0], chIP[1], chIP[2], chIP[3]);
		return strIP;
	}

	/********************************************************************
	* [函数名]: inet_ttol
	* [描述]: 将字符串类型的ip地址转换成long
	* [输入]:
	*   strIP：ip字符串
	* [返回值]:
	*   转换后的ip地址
	* [修改记录]:
	*   2014-12-17,littledj: create
	********************************************************************/
	unsigned long inet_ttol(const tchar* strIP)
	{
		if (strIP == NULL)
		{
			return 0;
		}

		unsigned long ip = 0;
		size_t ip_len = tcslen(strIP);
		size_t data_len = 0;
		size_t pos = 0;
		tchar* strIPTmp = new tchar[ip_len + 1];
		tcscpy(strIPTmp, strIP);
		for (size_t i = 0; i < ip_len; i++)
		{
			if (strIPTmp[i] == '.')
			{
				strIPTmp[i] = 0;
			}
		}

		data_len = tcslen(strIPTmp + pos);
		ip |= (unsigned char)ttoi(strIPTmp + pos);
		ip <<= 8;
		pos += data_len; pos++;	// skip '\0'
		if (pos >= ip_len)
			goto convert_end;

		data_len = tcslen(strIPTmp + pos);
		ip |= (unsigned char)ttoi(strIPTmp + pos);
		ip <<= 8;
		pos += data_len; pos++;	// skip '\0'
		if (pos >= ip_len)
			goto convert_end;

		data_len = tcslen(strIPTmp + pos);
		ip |= (unsigned char)ttoi(strIPTmp + pos);
		ip <<= 8;
		pos += data_len; pos++;	// skip '\0'
		if (pos >= ip_len)
			goto convert_end;

		data_len = tcslen(strIPTmp + pos);
		ip |= (unsigned char)ttoi(strIPTmp + pos);
		pos += data_len;
		if (pos == ip_len)
		{
			delete[] strIPTmp;
			return g_ntohl(ip);
		}

	convert_end:
		delete[] strIPTmp;
		return 0;
	}

	/********************************************************************
	* [函数名]: wtoa
	* [描述]: 将宽字节字符串转换成单字节字符串，宽字节的高位信息将丢失
	* [输入]:
	*   data：宽字节字符串
	*   len: 指定字符串长度，如果为0则使用_tcslen(data)
	* [返回值]:
	*   转换后的单字节字符串（函数内部申请，由调用者释放）
	* [修改记录]:
	*   2014-12-17,littledj: create
	*   2015-04-23,littledj: 对char和wchar都兼容，增加参数len
	********************************************************************/
	char* wtoa(const wchar_t* data, int len)
	{
		if (data == NULL)
		{
			return NULL;
		}

		size_t data_len = wcslen(data);
		if (len) data_len = len;
		char* retData = new char[data_len + 1];
		for (size_t i = 0; i < data_len; i++)
		{
			retData[i] = (char)data[i];
		}
		retData[data_len] = 0;

		return retData;
	}

	/********************************************************************
	* [函数名]: atow
	* [描述]: 将单字节字符串转换成宽字节字符串
	* [输入]:
	*   data：单字节字符串
	*   len: 指定字符串长度，如果为0则使用_tcslen(data)
	* [返回值]:
	*   转换后的宽字节字符串（函数内部申请，由调用者释放）
	* [修改记录]:
	*   2014-12-17,littledj: create
	*   2015-04-23,littledj: 增加参数len
	********************************************************************/
	wchar_t* atow(const char* data, int len)
	{
		if (data == NULL)
		{
			return NULL;
		}

		size_t data_len = strlen(data);
		if (len) data_len = len;
		wchar_t* retData = new wchar_t[data_len + 1];
		for (size_t i = 0; i < data_len; i++)
		{
			retData[i] = data[i];
		}
		retData[data_len] = 0;

		return retData;
	}

	vector<wstring> SplitString(const wstring& str, const wchar_t ch)
	{
		vector<wstring> strs;
		size_t start = 0;
		size_t pos = str.find(ch);
		while (pos != wstring::npos)
		{
			strs.push_back(str.substr(start, pos - start));
			start = pos + 1;
			pos = str.find(ch, start);
		}
		strs.push_back(str.substr(start, str.length() - start));
		return strs;
	}

	vector<string> SplitString(const string& str, const char ch)
	{
		if (str.empty())
		{
			return vector<string>();
		}

		vector<string> strs;
		size_t start = 0;
		size_t pos = str.find(ch);
		while (pos != string::npos)
		{
			strs.push_back(str.substr(start, pos - start));
			start = pos + 1;
			pos = str.find(ch, start);
		}
		strs.push_back(str.substr(start, str.length() - start));
		return strs;
	}

	string& TrimString(string& str, const char ch)
	{
		if (str.empty() || str.size() == 0)
			return str;

		size_t delch = string::npos;
		while ((delch = str.find(ch)) == 0)
			str.erase(delch, 1);

		if (str.empty() || str.size() == 0)
			return str;

		delch = str.rfind(ch);
		while ((delch = str.rfind(ch)) == str.length() - 1)
			str.erase(delch, 1);

		return str;
	}

	wstring& TrimString(wstring& str, const wchar_t ch)
	{
		if (str.empty() || str.size() == 0)
			return str;

		size_t delch = wstring::npos;
		while ((delch = str.find(ch)) == 0)
			str.erase(delch, 1);

		if (str.empty() || str.size() == 0)
			return str;

		delch = str.rfind(ch);
		while ((delch = str.rfind(ch)) == str.length() - 1)
			str.erase(delch, 1);

		return str;
	}

	void RemoveAllChar(string& str, const char ch)
	{
		size_t delch = string::npos;
		while ((delch = str.find(ch)) != string::npos)
			str.erase(delch, 1);
	}

	void RemoveAllChar(wstring& str, const wchar_t ch)
	{
		size_t delch = wstring::npos;
		while ((delch = str.find(ch)) != wstring::npos)
			str.erase(delch, 1);
	}

	void DeleteEmptyItems(vector<string>& strs)
	{
		for (auto i = strs.begin(); i < strs.end();)
		{
			if ((*i).empty())
				strs.erase(i++);
			else
				i++;
		}
	}

	wstring StringToWString(const string& str)
	{
		size_t len = str.size() * 2;// 预留字节数
		if (len == 0)
			return wstring(L"");
		setlocale(LC_CTYPE, "");     //必须调用此函数
		wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
		mbstowcs(p, str.c_str(), str.size());// 转换
		p[str.size()] = 0;
		wstring str1(p);
		delete[] p;// 释放申请的内存
		return str1;
	}

	string WStringToString(const wstring& str)
	{
		size_t len = str.size() * 4;
		if (len == 0)
			return string("");
		setlocale(LC_CTYPE, "");
		char *p = new char[len];
		wcstombs(p, str.c_str(), str.size());
		p[str.size()] = 0;
		string str1(p);
		delete[] p;
		return str1;
	}

	tstring StringToTString(const string & str)
	{
#ifdef UNICODE	
		return StringToWString(str);
#else
		return str;
#endif
	}

	tstring WStringToTString(const wstring & str)
	{
#ifdef UNICODE	
		return str;
#else
		return WStringToString(str);
#endif
	}

	string TStringToString(const tstring & str)
	{
#ifdef UNICODE	
		return WStringToString(str);
#else
		return str;
#endif
	}

	wstring TStringToWString(const tstring & str)
	{
#ifdef UNICODE	
		return str;
#else
		return StringToWString(str);
#endif
	}

	string ReplaseAllSubString(string & str, const string & src, const string & dst)
	{
		size_t fd = string::npos;
		while ((fd = str.find(src)) != string::npos)
		{
			str.replace(fd, src.size(), dst);
		}
		return string();
	}


	//从INI文件读取字符串类型数据
	tstring GetConfigString(const tstring& filename, const tstring& key,
		const tstring& dft, const tstring& title)
	{
		FILE *fp;
		tchar szLine[1024];
		szLine[0] = 0;
		static tchar tmpstr[1024];
		int rtnval;
		int i = 0;
		int flag = 0;
		tchar *tmp;

		if ((fp = tfopen(filename.c_str(), TEXT("r"))) == NULL)
		{
			return dft;
		}
		while (!feof(fp))
		{
			rtnval = fgetc(fp);
			if (rtnval == EOF)
			{
				if (tcslen(szLine) > 0)
				{
					tmp = tcschr(szLine, '=');
					if ((tmp != NULL) && (flag == 1))
					{
						if (tcsstr(szLine, key.c_str()) != NULL)
						{
							//注释行
							if ('#' == szLine[0])
							{
							}
							else if ('/' == szLine[0] && '/' == szLine[1])
							{
							}
							else
							{
								fclose(fp);

								if (tcslen(tmp) == 1)
									return dft;

								//找key对应变量
								tcscpy(tmpstr, tmp + 1);
								return tmpstr;
							}
						}
					}
				}

				break;
			}
			else
			{
				szLine[i++] = rtnval;
				szLine[i] = 0;
			}
			if (rtnval == '\n' || rtnval == '\r')
			{
				szLine[i - 1] = 0;
				i = 0;
				tmp = tcschr(szLine, '=');
				if ((tmp != NULL) && (flag == 1))
				{
					if (tcsstr(szLine, key.c_str()) != NULL)
					{
						//注释行
						if ('#' == szLine[0])
						{
						}
						else if ('/' == szLine[0] && '/' == szLine[1])
						{

						}
						else
						{
							fclose(fp);

							if (tcslen(tmp) == 1)
								return dft;

							//找key对应变量
							tcscpy(tmpstr, tmp + 1);							
							return tmpstr;
						}
					}
				}
				else
				{
					tcscpy(tmpstr, TEXT("["));
					tcscat(tmpstr, title.c_str());
					tcscat(tmpstr, TEXT("]"));
					if (tcsncmp(tmpstr, szLine, tcslen(tmpstr)) == 0)
					{
						//找到title
						flag = 1;
					}
				}
			}
		}
		fclose(fp);
		return dft;
	}

	//从INI文件读取整类型数据
	int GetConfigInt(const tstring& filename, const tstring& key,
		const tstring& dft, const tstring& title)
	{
		return ttoi(GetConfigString(filename, key, dft, title).c_str());
	}

}