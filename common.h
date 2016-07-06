// ver 1.0
// modified: 
//   2014-12-17,littledj: ȥ����common����
//   2015-01-23,littledj: �޸�SYS_LOGO

#pragma once
#pragma warning(disable:4996 4200)

#include "tstream.h"

using namespace std;

namespace gcommon
{

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#define IDLE_TIME				100
#define THREAD_START_TIMEOUT	2000			// �����̳߳�ʱʱ�䣨���룩
#define THREAD_STOP_TIMEOUT		2000			// ֹͣ�̳߳�ʱʱ��

	uint16_t htons(const uint16_t data);
	uint16_t ntohs(const uint16_t data);
	uint32_t htonl(const uint32_t data);
	uint32_t ntohl(const uint32_t data);
	int GetParaFromARG(int argc, tchar* argv[], tchar* prefix, tchar* &out, int pos = 1);
	tstring GetCurrentDirPath();
	tchar* inet_ltot(unsigned long ip);
	unsigned long inet_ttol(const tchar* strIP);
	uint32_t random(uint32_t start, uint32_t end);
	char* wtoa(const wchar_t* data, int len = 0);
	wchar_t* atow(const char* data, int len = 0);
	vector<string> SplitString(const string& str, const char ch);
	vector<wstring> SplitString(const wstring& str, const wchar_t ch);
	string& TrimString(string& str, const char ch = ' ');
	wstring& TrimString(wstring& str, const wchar_t ch = ' ');
	void RemoveAllChar(string& str, const char ch);
	void RemoveAllChar(wstring& str, const wchar_t ch);
	void DeleteEmptyItems(vector<string>& strs);
	wstring StringToWString(const string& str);
	string WStringToString(const wstring& str);
	tstring StringToTString(const string& str);
	tstring WStringToTString(const wstring& str);
	string TStringToString(const tstring& str);
	wstring TStringToWString(const tstring& str);
	string ReplaseAllSubString(string& str, const string& src, const string& dst);
	tstring GetConfigString(const tstring& filename, const tstring& key,
		const tstring& dft = TEXT(""), const tstring& title = TEXT("config"));
	int GetConfigInt(const tstring& filename, const tstring& key,
		const tstring& dft = TEXT("0"), const tstring& title = TEXT("config"));
}