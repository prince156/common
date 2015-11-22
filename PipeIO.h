#pragma once
#include <cstdint>
#include "tstream.h"

using namespace std;
using namespace gcommon;

/********************************************************************/
/* ����: CPipeIO
/* ����: ʵ�ַ���˺Ϳͻ��˵Ĺܵ�ͨ��
/* �޸ļ�¼:
/*   2013-11-17,littledj: create
/********************************************************************/
class CPipeIO
{
public:
	CPipeIO();
	CPipeIO(const tstring& pipeName);
	~CPipeIO();
	
public:
	static const uint32_t MAX_PIPENAME_LEN = 128;
	static const uint32_t MAX_PIPEMSG_LEN = 255 * 100;

private:
	bool m_bInit;
	tstring m_PipeName;
	void* m_hPipe;

public:
	// ����˺���
	bool CreatePipe(const tstring& pipeName);
	void Connect();
	void Disconnect();

	// �ͻ��˺���
	bool OpenPipe(const tstring& pipeName);
	void Wait(uint32_t dwTimeout);

	// ���ú���
	bool ReadPipe(char* buff, uint32_t len, uint32_t& nRead, uint32_t timeout);//��ȡ����
	bool WritePipe(char* data, uint32_t len, uint32_t& nWrite, uint32_t timeout);//д������
	void Close();	
};