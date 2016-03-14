// common.cpp : Defines the entry point for the console application.
//

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
	int GetParaFromARG(int argc, TCHAR* argv[], TCHAR* prefix, TCHAR* &out, int pos)
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
	int GetCurrentDirPath(TCHAR* &out)
	{
		out = new TCHAR[MAX_PATH + 1];
		memset(out, 0, (MAX_PATH + 1)*sizeof(TCHAR));
		if (NULL == _tgetcwd(out, MAX_PATH))
			return -1;
		else
			return 0;
	}

	/********************************************************************
	* [������]: random
	* [����]: ��[start,end)��(����end, ���0xffffffff)��ȡһ���������,start>0
	* [����]:
	*   start: �������ʼλ��
	*   end: ���������λ�ã���������
	* [����ֵ]: unsigned int
	*   ���ɵ������
	* [�޸ļ�¼]:
	*   2013-11-17,littledj: create
	*   2016-01-20,littledj: �Ż�srand����
	*   2016-03-13,littledj: �����ͷ���ֵ����Ϊ�޷��ţ�����Χ
	********************************************************************/
	unsigned int random(unsigned int start, unsigned int end)
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
		double rate = (end - start)*1.0 / (unsigned int)0xFFFFFFFF;
		unsigned int delta = (unsigned int)(rate * (unsigned int)rr);
		return start + delta;
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
	TCHAR* inet_ltot(unsigned long ip)
	{
		unsigned char chIP[4];
		chIP[0] = (unsigned char)(ip);
		chIP[1] = (unsigned char)(ip >> 8);
		chIP[2] = (unsigned char)(ip >> 16);
		chIP[3] = (unsigned char)(ip >> 24);

		static TCHAR strIP[16];
		memset(strIP, 0, 16 * sizeof(TCHAR));

		_stprintf(strIP, TEXT("%u.%u.%u.%u"), chIP[0], chIP[1], chIP[2], chIP[3]);
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
	unsigned long inet_ttol(const TCHAR* strIP)
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
	char* wtoa(const wchar_t* data, int len)
	{
		if (data == NULL)
		{
			return NULL;
		}

		int data_len = wcslen(data);
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
	wchar_t* atow(const char* data, int len)
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

	vector<tstring> SplitString(const tstring& str, const TCHAR ch)
	{
		vector<tstring> strs;
		size_t start = 0;
		size_t pos = str.find(ch);
		while (pos != tstring::npos)
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

	tstring& TrimString(tstring& str, const TCHAR ch)
	{
		if (str.empty() || str.size() == 0)
			return str;

		size_t delch = tstring::npos;
		while ((delch = str.find(ch)) == 0)
			str.erase(delch, 1);

		if (str.empty() || str.size() == 0)
			return str;

		delch = str.rfind(ch);
		while ((delch = str.rfind(ch)) == str.length() - 1)
			str.erase(delch, 1);

		return str;
	}

	void RemoveAllChar(tstring& str, const TCHAR ch)
	{
		size_t delch = tstring::npos;
		while ((delch = str.find(ch)) != tstring::npos)
			str.erase(delch, 1);
	}

	wstring StringToWString(const string& str)
	{
		unsigned len = str.size() * 2;// Ԥ���ֽ���
		if (len == 0)
			return wstring(L"");
		setlocale(LC_CTYPE, "");     //������ô˺���
		wchar_t *p = new wchar_t[len];// ����һ���ڴ���ת������ַ���
		size_t count;
		mbstowcs_s(&count, p, len, str.c_str(), str.size());// ת��
		wstring str1(p);
		delete[] p;// �ͷ�������ڴ�
		return str1;
	}

	string WStringToString(const wstring& str)
	{
		unsigned len = str.size() * 4;
		if (len == 0)
			return string("");
		setlocale(LC_CTYPE, "");
		char *p = new char[len];
		size_t count;
		wcstombs_s(&count, p, len, str.c_str(), str.size());
		string str1(p);
		delete[] p;
		return str1;
	}

	/********************************************************************
	/* ������: GetConfigPara
	/* ����: ��ȡ�����ļ���ָ���Ĳ���ֵ
	/* ����:
	/*   strConfigFile�������ļ���
	/*   lpKey����������
	/*   lpDefault��Ĭ��ֵ
	/* ���:
	/*   CString: �������ַ�����ʽ
	/* �޸ļ�¼:
	/*   2013-12-24,xiaogu: create
	/*   2016-03-14,littledj: default����Ϊempty
	/********************************************************************/
	tstring GetConfigPara(tstring strConfigFilePath, tstring key, tstring default)
	{
		if (strConfigFilePath.empty() || key.empty())
			return tstring();

		TCHAR szConfigValue[MAX_PATH + 1];
		szConfigValue[0] = 0;
		GetPrivateProfileString(TEXT("config"), key.c_str(), default.c_str(),
			szConfigValue, MAX_PATH, strConfigFilePath.c_str());

		if (_tcslen(szConfigValue) > 0)
		{
			return tstring(szConfigValue);
		}
		else
			return default;
	}
}