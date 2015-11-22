#pragma once
#include <map>
#include <list>
#include <cstdint>
#include <algorithm>
#include <vector>
#include "tstream.h"

using namespace std;
using namespace gcommon;

enum class CMD_TYPE
{
	FULL = 0x00,			// ����ȫ��
	SHORT_FORM = 0x01,		// ������д
	SAME = 0x02,			// ͬ������
	EXPIRED = 0x03,			// ��������
	BAD = 0xff,				// ��������
};


// ����ص���������
typedef uint32_t (*CMD_CALLBACK)(const tstring&);		

// ������Ϣ�ṹ��
typedef struct _shell_handler
{
	tstring strCMD;				// ��������
	CMD_TYPE type;				// ��������
	CMD_CALLBACK handler;		// ����ص�����
	tstring strToolTip;			// �������
	tstring strUsage;
}SHELL_HANDLER,*PSHELL_HANDLER;


/********************************************************************/
/* ����: GCLI
/* ����: ͨ�õ�shell������,ͨ�����û��ܹ����ɴ����Լ���shell����
/*       ��Ҫ��ʵ���˰�������ѭ������֧���û�ע���Լ�������ص�������
/* �޸ļ�¼:
/*   2013-12-14,littledj: create
/********************************************************************/
class GCLI
{
public:
	GCLI(void);
	~GCLI(void);

public:
	static const uint32_t MAX_PROMPT_LEN = 16;			// ǰ���ַ�����󳤶�
	static const uint32_t MAX_CMD_LEN = 63;				// ������󳤶�
	static const uint32_t MAX_TOOLTIP_LEN = 255;		// ����������󳤶�
	static const uint32_t MAX_CMD_PARA_COUNT = 10;		// ��������������

private:	
	const tstring DEFAULT_PROMPT = TEXT("#shell> ");			// Ĭ��ǰ���ַ���
	const tstring DEFAULT_CMD_HELP = TEXT("help");				// Ĭ�ϰ�������
	const tstring DEFAULT_CMD_EXIT = TEXT("exit");				// Ĭ���˳�����

	map<tstring, SHELL_HANDLER> m_handlerMap;
	tstring m_prompt;
	bool m_shouldExit; // �����ж��Ƿ��˳�

public:
	bool Init(const tstring& prompt = TEXT(""));
	bool RegisterHandler( SHELL_HANDLER handler );
	bool RegisterHandlers( SHELL_HANDLER handler[] ,int count);
	void MainLoop();

private:
	void DispatchCommand(const tstring& cmd);
	PSHELL_HANDLER GetNextAssociat(const tstring& cmd);
	void ListAssociat(const tstring& cmd);
	void Exit();
	void ShowHelp();
	vector<tstring> split(const tstring& str); // �ŵ���������...
};

