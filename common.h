// ver 1.0
// modified: 
//   2014-12-17,littledj: 去除非common内容
//   2015-01-23,littledj: 修改SYS_LOGO

#pragma once
#pragma warning(disable:4996 4200)
//#include <Windows.h>
#include <tchar.h>
#include <ctime>
#include <cstdint>
#include <random>
#include <direct.h>
#include <vector>
#include <string>
#include "tstream.h"

using namespace std;

namespace gcommon
{

#ifndef MAX_PATH
#define MAX_PATH 128
#endif

#define SYS_LOGO TEXT("\n")\
	TEXT("***********************************************************\n")\
	TEXT("                       %s                     \n")\
	TEXT("***********************************************************\n")\
	TEXT("\n")

#define IDLE_TIME				100
#define THREAD_START_TIMEOUT	2000			// 启动线程超时时间（毫秒）
#define THREAD_STOP_TIMEOUT		2000			// 停止线程超时时间

	static uint16_t htons(const uint16_t data)
	{
		uint16_t tmp = (data & 0xff00) >> 8;
		tmp |= (data & 0x00ff) << 8;
		return tmp;
	}

	static uint16_t ntohs(const uint16_t data)
	{
		return htons(data);
	}

	static uint32_t htonl(const uint32_t data)
	{
		uint32_t tmp = (data & 0xff000000) >> 24;
		tmp |= (data & 0x00ff0000) >> 8;
		tmp |= (data & 0x0000ff00) << 8;
		tmp |= (data & 0x000000ff) << 24;
		return tmp;
	}

	static uint32_t ntohl(const uint32_t data)
	{
		return htonl(data);
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
	static int GetParaFromARG(int argc, TCHAR* argv[], TCHAR* prefix, TCHAR* &out, int pos = 1)
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
			else if (_tcsncmp(argv[i], prefix, _tcslen(prefix)) == 0 &&	// 寻找指定前导参数
				_tcslen(argv[i]) == _tcslen(prefix))
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
	* [函数名]: GetCurrentDirPath
	* [描述]: 获取当前可执行程序所在文件夹的全路径
	* [输入]:
	*   无
	* [输出]:
	*   out: 当前文件夹路径（函数内部申请，由调用者释放）
	* [返回值]: int
	*   0：成功
	*   -1：失败
	* [修改记录]:
	*   2013-11-17,littledj: create
	*   2013-12-07,littledj: 返回值改为CString，不再使用全局变量
	*   2014-10-17,littledj: 返回值改为int, 输出通过参数out实现
	********************************************************************/
	static int GetCurrentDirPath(TCHAR* &out)
	{
		out = new TCHAR[MAX_PATH + 1];
		memset(out, 0, (MAX_PATH + 1)*sizeof(TCHAR));
		//uint32_t nRet = GetModuleFileName(NULL, out, MAX_PATH);
		//if (nRet == 0)
		//{
		//	return -1;
		//}
		//(_tcsrchr(out, TEXT('\\')))[1] = 0;

		_tgetcwd(out, MAX_PATH);
		return 0;
	}

	/********************************************************************
	* [函数名]: random
	* [描述]: 在[start,end)内(不包end, 最大0x7fff)获取一个随机整数,start>0
	* [输入]:
	*   start: 随机数开始位置
	*   end: 随机数结束位置（不包括）
	* [返回值]: int
	*   生成的随机数
	* [修改记录]:
	*   2013-11-17,littledj: create
	********************************************************************/
	static int random(int start, int end)
	{
		// 参数检查
		if (start <0)
			start = 0;
		if (end <= start)
			return start;

		// 第一次调用则初始化随机数种子
		bool static bFirst = true;
		if (bFirst)
		{
			srand((int)((int)time(0) * 1000.0 * rand() / (RAND_MAX + 1.0)));
			bFirst = false;
		}

		return (int)(start + (end - start)*rand()*1.0 / (RAND_MAX + 1.0));
	}

	/********************************************************************
	* [函数名]: inet_ltot
	* [描述]: 将long类型的ip地址转换成字符串（xxx.xxx.xxx.xx）
	* [输入]:
	*   ip：ip地址
	* [返回值]: TCHAR
	*   转换后的ip地址字符串
	* [修改记录]:
	*   2014-12-17,littledj: create
	********************************************************************/
	static TCHAR* inet_ltot(unsigned long ip)
	{
		unsigned char chIP[4];
		chIP[0] = (unsigned char)(ip);
		chIP[1] = (unsigned char)(ip >> 8);
		chIP[2] = (unsigned char)(ip >> 16);
		chIP[3] = (unsigned char)(ip >> 24);

		static TCHAR strIP[16];
		memset(strIP, 0, 16 * sizeof(TCHAR));

		_stprintf(strIP, TEXT("%d.%d.%d.%d"), chIP[0], chIP[1], chIP[2], chIP[3]);
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
	static unsigned long inet_ttol(const TCHAR* strIP)
	{
		if (strIP == NULL)
		{
			return 0;
		}

		unsigned long ip = 0;
		int ip_len = _tcslen(strIP);
		int data_len = 0;
		int pos = 0;
		TCHAR* strIPTmp = new TCHAR[ip_len + 1];
		_tcscpy(strIPTmp, strIP);
		for (int i = 0; i < ip_len; i++)
		{
			if (strIPTmp[i] == '.')
			{
				strIPTmp[i] = 0;
			}
		}

		data_len = _tcslen(strIPTmp + pos);
		ip |= (unsigned char)_ttoi(strIPTmp + pos);
		ip <<= 8;
		pos += data_len; pos++;	// skip '\0'
		if (pos >= ip_len)
			goto convert_end;

		data_len = _tcslen(strIPTmp + pos);
		ip |= (unsigned char)_ttoi(strIPTmp + pos);
		ip <<= 8;
		pos += data_len; pos++;	// skip '\0'
		if (pos >= ip_len)
			goto convert_end;

		data_len = _tcslen(strIPTmp + pos);
		ip |= (unsigned char)_ttoi(strIPTmp + pos);
		ip <<= 8;
		pos += data_len; pos++;	// skip '\0'
		if (pos >= ip_len)
			goto convert_end;

		data_len = _tcslen(strIPTmp + pos);
		ip |= (unsigned char)_ttoi(strIPTmp + pos);
		pos += data_len;
		if (pos == ip_len)
		{
			delete[] strIPTmp;
			return ntohl(ip);
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
	static char* wtoa(const TCHAR* data, int len = 0)
	{
		if (data == NULL)
		{
			return NULL;
		}

		int data_len = _tcslen(data);
		if (len) data_len = len;
		char* retData = new char[data_len + 1];
		for (int i = 0; i < data_len; i++)
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
	static wchar_t* atow(const char* data, int len = 0)
	{
		if (data == NULL)
		{
			return NULL;
		}

		int data_len = strlen(data);
		if (len) data_len = len;
		wchar_t* retData = new wchar_t[data_len + 1];
		for (int i = 0; i < data_len; i++)
		{
			retData[i] = data[i];
		}
		retData[data_len] = 0;

		return retData;
	}

	static vector<string> SplitString(const string& str, const char ch)
	{
		vector<string> strs;
		int start = 0;
		int pos = str.find(ch);
		while (pos != string::npos)
		{
			strs.push_back(str.substr(start, pos - start));
			start = pos + 1;
			pos = str.find(ch, start);
		}
		strs.push_back(str.substr(start, str.length() - start));
		return strs;
	}

	static wstring StringToWString(const string& str)
	{
		unsigned len = str.size() * 2;// 预留字节数
		setlocale(LC_CTYPE, "");     //必须调用此函数
		wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
		size_t count;
		mbstowcs_s(&count, p, len, str.c_str(), str.size());// 转换
		wstring str1(p);
		delete[] p;// 释放申请的内存
		return str1;
	}

	static string WStringToString(const wstring& str)
	{
		unsigned len = str.size() * 4;
		setlocale(LC_CTYPE, "");
		char *p = new char[len];
		size_t count;
		wcstombs_s(&count, p, len, str.c_str(), str.size());
		string str1(p);
		delete[] p;
		return str1;
	}

}