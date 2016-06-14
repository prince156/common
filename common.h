// ver 1.0
// modified: 
//   2014-12-17,littledj: 去除非common内容
//   2015-01-23,littledj: 修改SYS_LOGO

#pragma once
#pragma warning(disable:4996 4200)
#include <Windows.h>
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

#define IDLE_TIME				100
#define THREAD_START_TIMEOUT	2000			// 启动线程超时时间（毫秒）
#define THREAD_STOP_TIMEOUT		2000			// 停止线程超时时间

	uint16_t htons(const uint16_t data);
	uint16_t ntohs(const uint16_t data);
	uint32_t htonl(const uint32_t data);
	uint32_t ntohl(const uint32_t data);
	int GetParaFromARG(int argc, TCHAR* argv[], TCHAR* prefix, TCHAR* &out, int pos = 1);
	int GetCurrentDirPath(TCHAR* &out);
	TCHAR* inet_ltot(unsigned long ip);
	unsigned long inet_ttol(const TCHAR* strIP);
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
	string GetConfigPara(string strConfigFilePath, string key, string dft);
	wstring GetConfigPara(wstring strConfigFilePath, wstring key, wstring dft);
}