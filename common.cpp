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
	uint16_t htons(const uint16_t data)
	{
		uint16_t tmp = (data & 0xff00) >> 8;
		tmp |= (data & 0x00ff) << 8;
		return tmp;
	}

	uint16_t ntohs(const uint16_t data)
	{
		return htons(data);
	}

	uint32_t htonl(const uint32_t data)
	{
		uint32_t tmp = (data & 0xff000000) >> 24;
		tmp |= (data & 0x00ff0000) >> 8;
		tmp |= (data & 0x0000ff00) << 8;
		tmp |= (data & 0x000000ff) << 24;
		return tmp;
	}

	uint32_t ntohl(const uint32_t data)
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
	int GetParaFromARG(int argc, tchar* argv[], tchar* prefix, tchar* &out, int pos)
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
			else if (tcsncmp(argv[i], prefix, tcslen(prefix)) == 0 &&	// Ѱ��ָ��ǰ������
				tcslen(argv[i]) == tcslen(prefix))
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
	tstring GetCurrentDirPath()
	{
		tstring strCurrentDir;
		tchar* pCurrentDir = new tchar[MAX_PATH + 1];
		pCurrentDir[0] = 0;
#ifdef __LINUX__
		ssize_t count = readlink("/proc/self/exe", pCurrentDir, MAX_PATH);
		if (count == 0)
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
	* [������]: random
	* [����]: ��[start,end)��(����end, ���0xffffffff)��ȡһ���������,start>0
	* [����]:
	*   start: �������ʼλ��
	*   end: ���������λ�ã���������
	* [����ֵ]: uint32_t
	*   ���ɵ������
	* [�޸ļ�¼]:
	*   2013-11-17,littledj: create
	*   2016-01-20,littledj: �Ż�srand����
	*   2016-03-13,littledj: �����ͷ���ֵ����Ϊ�޷��ţ�����Χ
	********************************************************************/
	uint32_t random(uint32_t start, uint32_t end)
	{
		// �������
		if (start < 0)
			start = 0;
		if (end <= start)
			return start;

		// ��һ�ε������ʼ�����������
		bool static bFirst = true;
		if (bFirst)
		{
			char* chr = new char[1];
			srand((int)chr);
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
	* [������]: inet_ltot
	* [����]: ��long���͵�ip��ַת�����ַ�����xxx.xxx.xxx.xx��
	* [����]:
	*   ip��ip��ַ
	* [����ֵ]: tchar
	*   ת�����ip��ַ�ַ���
	* [�޸ļ�¼]:
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
	* [������]: inet_ttol
	* [����]: ���ַ������͵�ip��ַת����long
	* [����]:
	*   strIP��ip�ַ���
	* [����ֵ]:
	*   ת�����ip��ַ
	* [�޸ļ�¼]:
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
		size_t len = str.size() * 2;// Ԥ���ֽ���
		if (len == 0)
			return wstring(L"");
		setlocale(LC_CTYPE, "");     //������ô˺���
		wchar_t *p = new wchar_t[len];// ����һ���ڴ���ת������ַ���
		mbstowcs(p, str.c_str(), str.size());// ת��
		wstring str1(p);
		delete[] p;// �ͷ�������ڴ�
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


	//��INI�ļ���ȡ�ַ�����������
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
							//ע����
							if ('#' == szLine[0])
							{
							}
							else if ('/' == szLine[0] && '/' == szLine[1])
							{
							}
							else
							{
								//�Ҵ�key��Ӧ����
								tcscpy(tmpstr, tmp + 1);
								fclose(fp);
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
						//ע����
						if ('#' == szLine[0])
						{
						}
						else if ('/' == szLine[0] && '/' == szLine[1])
						{

						}
						else
						{
							//�Ҵ�key��Ӧ����
							tcscpy(tmpstr, tmp + 1);
							fclose(fp);
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
						//�ҵ�title
						flag = 1;
					}
				}
			}
		}
		fclose(fp);
		return dft;
	}

	//��INI�ļ���ȡ����������
	int GetConfigInt(const tstring& filename, const tstring& key,
		const tstring& dft, const tstring& title)
	{
		return ttoi(GetConfigString(filename, key, dft, title).c_str());
	}

}