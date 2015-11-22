// ver 1.0
// modified: 
//   2014-12-17,littledj: ȥ����common����
//   2015-01-23,littledj: �޸�SYS_LOGO

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
#define THREAD_START_TIMEOUT	2000			// �����̳߳�ʱʱ�䣨���룩
#define THREAD_STOP_TIMEOUT		2000			// ֹͣ�̳߳�ʱʱ��

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
	* [������]: GetParaFromARG
	* [����]: �Ӳ����б��л�ȡָ���Ĳ���
	* [����]
	*   argc����������
	*   argv�������б�
	*   prefix������ǰ����������ΪNULL���߿��ַ���""
	*   pos��ָ���ǵڼ������������Ĳ�������1��ʼ
	* [���]
	*   out����ȡ�Ĳ����������ͷţ�
	* [����ֵ]
	*   >0���ɹ������������Ĳ�������
	*   0��ʧ�ܣ���������
	*   -1��ʧ�ܣ�δ�ҵ�prefix
	*   -2��ʧ�ܣ��ҵ�prefix���������޲���
	* [�޸ļ�¼]
	*   2013-12-09,littledj: create
	*   2014-04-08,littledj: ��Ԫ���ԣ����޸�
	*********************************************************************/
	static int GetParaFromARG(int argc, TCHAR* argv[], TCHAR* prefix, TCHAR* &out, int pos = 1)
	{
		int count = 0;
		bool bFind = false;
		bool bFindPrefix = false;

		// �������
		if (argv == NULL || pos <= 0 ||
			argc <= 1 || argv[0] == NULL)
			return 0;
		if (prefix != NULL && prefix[0] != '-' && prefix[0] != 0)	//	ǰ����������'-'��ʼ
			return 0;

		for (int i = 1; i < argc; i++)
		{
			// ����Ϊ��
			if (argv[i] == NULL) return 0;

			// ȥ����ʼ�ո�
			while (argv[i][0] == ' ')
			{
				argv[i] += 1;
				if (argv[i][0] == 0) break;
			}
			if (argv[i][0] == 0) continue;

			// Ѱ����ǰ������
			if (prefix == NULL || prefix[0] == 0)
			{
				if (argv[i][0] != '-')	// ������Ӧ���ԡ�-����Ϊ��һ���ַ�
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
					i++; // �ж�Ϊǰ����������һ������
				}
			}
			else if (_tcsncmp(argv[i], prefix, _tcslen(prefix)) == 0 &&	// Ѱ��ָ��ǰ������
				_tcslen(argv[i]) == _tcslen(prefix))
			{
				bFindPrefix = true;
				do
				{
					if (++i == argc)	// �жϺ����Ƿ��в���
					{
						if (bFind) return count;
						else return -2;
					}
					if (argv[i] == NULL) return 0;	// ����Ϊ��

					// ȥ����ʼ�ո�
					while (argv[i][0] == ' ')
					{
						argv[i] += 1;
						if (argv[i][0] == 0) break;
					}
				} while (argv[i][0] == 0);

				// �жϺ���Ĳ����Ƿ���ǰ����
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
			return count;	// �ҵ����������Ĳ���
		else if (bFindPrefix)
			return -2;		// ֻ�ҵ�ǰ����
		else
			return -1;		// ʲôҲû�ҵ�
	}

	/********************************************************************
	* [������]: GetCurrentDirPath
	* [����]: ��ȡ��ǰ��ִ�г��������ļ��е�ȫ·��
	* [����]:
	*   ��
	* [���]:
	*   out: ��ǰ�ļ���·���������ڲ����룬�ɵ������ͷţ�
	* [����ֵ]: int
	*   0���ɹ�
	*   -1��ʧ��
	* [�޸ļ�¼]:
	*   2013-11-17,littledj: create
	*   2013-12-07,littledj: ����ֵ��ΪCString������ʹ��ȫ�ֱ���
	*   2014-10-17,littledj: ����ֵ��Ϊint, ���ͨ������outʵ��
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
	* [������]: random
	* [����]: ��[start,end)��(����end, ���0x7fff)��ȡһ���������,start>0
	* [����]:
	*   start: �������ʼλ��
	*   end: ���������λ�ã���������
	* [����ֵ]: int
	*   ���ɵ������
	* [�޸ļ�¼]:
	*   2013-11-17,littledj: create
	********************************************************************/
	static int random(int start, int end)
	{
		// �������
		if (start <0)
			start = 0;
		if (end <= start)
			return start;

		// ��һ�ε������ʼ�����������
		bool static bFirst = true;
		if (bFirst)
		{
			srand((int)((int)time(0) * 1000.0 * rand() / (RAND_MAX + 1.0)));
			bFirst = false;
		}

		return (int)(start + (end - start)*rand()*1.0 / (RAND_MAX + 1.0));
	}

	/********************************************************************
	* [������]: inet_ltot
	* [����]: ��long���͵�ip��ַת�����ַ�����xxx.xxx.xxx.xx��
	* [����]:
	*   ip��ip��ַ
	* [����ֵ]: TCHAR
	*   ת�����ip��ַ�ַ���
	* [�޸ļ�¼]:
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
	* [������]: inet_ttol
	* [����]: ���ַ������͵�ip��ַת����long
	* [����]:
	*   strIP��ip�ַ���
	* [����ֵ]:
	*   ת�����ip��ַ
	* [�޸ļ�¼]:
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
	* [������]: wtoa
	* [����]: �����ֽ��ַ���ת���ɵ��ֽ��ַ��������ֽڵĸ�λ��Ϣ����ʧ
	* [����]:
	*   data�����ֽ��ַ���
	*   len: ָ���ַ������ȣ����Ϊ0��ʹ��_tcslen(data)
	* [����ֵ]:
	*   ת����ĵ��ֽ��ַ����������ڲ����룬�ɵ������ͷţ�
	* [�޸ļ�¼]:
	*   2014-12-17,littledj: create
	*   2015-04-23,littledj: ��char��wchar�����ݣ����Ӳ���len
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
	* [������]: atow
	* [����]: �����ֽ��ַ���ת���ɿ��ֽ��ַ���
	* [����]:
	*   data�����ֽ��ַ���
	*   len: ָ���ַ������ȣ����Ϊ0��ʹ��_tcslen(data)
	* [����ֵ]:
	*   ת����Ŀ��ֽ��ַ����������ڲ����룬�ɵ������ͷţ�
	* [�޸ļ�¼]:
	*   2014-12-17,littledj: create
	*   2015-04-23,littledj: ���Ӳ���len
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
		unsigned len = str.size() * 2;// Ԥ���ֽ���
		setlocale(LC_CTYPE, "");     //������ô˺���
		wchar_t *p = new wchar_t[len];// ����һ���ڴ���ת������ַ���
		size_t count;
		mbstowcs_s(&count, p, len, str.c_str(), str.size());// ת��
		wstring str1(p);
		delete[] p;// �ͷ�������ڴ�
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