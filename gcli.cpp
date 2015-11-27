// CMDShell.cpp
// �ṩcommand shellͨ�ù���

#include "gcli.h"
#include "common.h"
#include <conio.h>

GCLI::GCLI(void)
{
	m_prompt.clear();
}

GCLI::~GCLI(void)
{
}

/********************************************************************/
/* ������: Init
/* ����: ��ʼ��GCLI��
/* ����:
/*   prompt: shell����ʾ��ǰ����������Ϊ�գ�Ĭ��ֵ��"analyze shell> "��
/* ���:
/*   true: ��ʼ���ɹ�
/*   false: ��ʼ��ʧ��
/* �޸ļ�¼:
/*   2013-12-03,littledj: create
/********************************************************************/
bool GCLI::Init(const tstring& prompt)
{
	m_handlerMap.clear();
	m_prompt = prompt;
	m_shouldExit = false;
	return true;
}

/********************************************************************/
/* ������: RegisterHandler
/* ����: ע��ָ������Ͷ�Ӧ�Ļص�������ÿ��ע��һ����
/* ����:
/*   handler: �ṹ�壬��������ص�����������
/* ���:
/*   true: ע��ɹ�
/*   false: ע��ʧ��
/* �޸ļ�¼:
/*   2013-12-03,littledj: create
/********************************************************************/
bool GCLI::RegisterHandler(SHELL_HANDLER handler)
{
	// �������
	if (handler.handler == NULL || handler.strCMD.empty())
	{
		return false;
	}
	if (handler.strCMD.length() > MAX_CMD_LEN ||
		handler.strToolTip.length() > MAX_TOOLTIP_LEN)
	{
		return false;
	}

	// ������Ͷ�Ӧ�Ļص���������map
	m_handlerMap.insert(pair<tstring, SHELL_HANDLER>(handler.strCMD, handler));
	return true;
}

/********************************************************************/
/* ������: RegisterHandlers
/* ����: ע��ָ������Ͷ�Ӧ�Ļص�����������ע�ᣩ
/* ����:
/*   handler: �ṹ�����飬��������ص�����������
/*   nCount: �ṹ������Ԫ������
/* ���:
/*   true: ע��ɹ�
/*   false: ע��ʧ��
/* �޸ļ�¼:
/*   2013-12-03,littledj: create
/********************************************************************/
bool GCLI::RegisterHandlers(SHELL_HANDLER handler[], int count)
{
	// �������
	if (handler == NULL || count == 0)
		return false;

	// ע����������
	for (int i = 0; i<count; i++)
	{
		if (handler[i].type != CMD_TYPE::BAD)
			if (!RegisterHandler(handler[i]))
				return false;
	}
	return true;
}

/********************************************************************/
/* ������: DispatchCommand
/* ����: ����ַ�����
/* ����:
/*   cmd: ���ַ�������(����Я���Ĳ���)
/* ���:
/*   ��
/* �޸ļ�¼:
/*   2013-12-02,littledj: create
/********************************************************************/
void GCLI::DispatchCommand(const tstring& command)
{
	// �������
	if (command.empty() || command.length() == 0 || command.length() > MAX_CMD_LEN)
	{
		_tprintf_s(TEXT("command is <null> or too long!\n"));
		return;
	}

	// ������Ϣ
	//_tprintf_s(TEXT("debug: "));
	//_tprintf_s(command.c_str());
	//_tprintf_s(TEXT("\n"));

	// ȷ������
	vector<tstring> cmdList = split(command);
	int argc = cmdList.size() - 1;
	if (argc > MAX_CMD_PARA_COUNT)
	{
		_tprintf_s(TEXT("to many arguments!\n"));
		return;
	}

	// ��map�в��Ҳ����ö�Ӧ�ص�����	
	SHELL_HANDLER shell_handler;
	auto element = m_handlerMap.find(cmdList[0]);
	if (element != m_handlerMap.end())
	{
		shell_handler = (*element).second;
		(*shell_handler.handler)(command);	// ���ûص�����

		// Ĭ���˳����������ö�ӦĬ�Ϻ�����
		if (cmdList[0] == DEFAULT_CMD_EXIT)
		{
			Exit();
		}
	}
	// Ĭ�ϰ������Ĭ�Ϻ������ᱻ�û�ע��ĺ������ǣ�
	else if (cmdList[0] == DEFAULT_CMD_HELP)
	{
		ShowHelp();
	}
	else
	{
		_tprintf_s(TEXT("<bad command>\n"));
	}
}

/********************************************************************/
/* ������: CMD_Loop
/* ����: �������ѭ��
/* �޸ļ�¼:
/*   2013-12-02,littledj: create
/*   2013-12-03,littledj: ����[tab]��[?]�����빦��
/********************************************************************/
void GCLI::MainLoop()
{
	// �������
	if (m_prompt.empty())
		m_prompt = DEFAULT_PROMPT;
	if (m_handlerMap.empty())
		return;
	if (m_prompt.length() > MAX_PROMPT_LEN ||
		m_handlerMap.size() == 0)
		return;

	unsigned int i = 0;
	list<tstring> cmdList;
	auto posList = cmdList.cend();
	//bool bDirect = 0;	//  ��ѯlist�ķ����������·�����ʷ�����
	TCHAR *cmd = new TCHAR[MAX_CMD_LEN + 1];
	TCHAR *cmd_tmp = new TCHAR[MAX_CMD_LEN + 1];
	PSHELL_HANDLER cmdAss = NULL;
	unsigned int pos = 0;
	bool bInit = true;
	while (1)
	{
		// �ж��Ƿ��˳�
		if (m_shouldExit)
			break;

		// �ж��Ƿ��ʼ��
		if (bInit)
		{
			_tprintf_s(m_prompt.c_str());
			pos = 0;
			memset(cmd, 0, MAX_CMD_LEN + 1);
			bInit = false;
		}

		// �û�����һ���ַ�
		unsigned char ch = getch();

		// [Tab]���Ĵ���������������
		if (ch == '\t')
		{
			if (pos != 0)
			{
				if (cmdAss)
					delete cmdAss;
				cmdAss = GetNextAssociat(cmd);
				if (cmdAss == NULL)
					continue;

				while (pos--)
					_tprintf_s(TEXT("\b \b"));
				_tprintf_s(cmdAss->strCMD.c_str());
				pos = cmdAss->strCMD.length();
			}
			continue;
		}

		// �������������cmd���������
		if (cmdAss)
		{
			memcpy(cmd, cmdAss->strCMD.c_str(), pos);
			delete cmdAss;
			cmdAss = NULL;
		}

		// '?'�Ĵ���
		// ����û�ֱ������'?'�����ӡ������Ϣ��ͬ"help"��;
		// ����û������뼸���ַ����'?'���޶�Ӧ��������ʾ���룬
		// �ж�Ӧ��������ʾ������÷�
		if (ch == '?')
		{
			if (pos == 0)
			{
				_tprintf_s(TEXT("\n"));
				DispatchCommand(DEFAULT_CMD_HELP);
				bInit = true;
			}
			else
			{
				ListAssociat(cmd);
			}
			continue;
		}

		// [Backspace]���Ĵ���
		if (ch == '\b')
		{
			if (pos > 0)
			{
				pos--;
				_tprintf_s(TEXT("\b"));
				_tcscpy_s(cmd + pos, _tcslen(cmd + pos + 1)+1, cmd + pos + 1);
				_tprintf_s(TEXT("%s "), cmd + pos);
				for (i = 0; i < _tcslen(cmd + pos) + 1; i++)
				{
					_tprintf_s(TEXT("\b"));
				}
			}
			continue;
		}

		// �س����Ĵ���
		if (ch == '\r' || ch == '\n')
		{
			_tprintf_s(TEXT("\n"));
			DispatchCommand(cmd);
			if (pos != 0)
			{
				if (!cmdList.empty())
				{
					cmdList.remove(cmd);
				}
				cmdList.push_back(cmd);
				posList = cmdList.cend();
			}
			bInit = true;
			continue;
		}

		// ������[delete]���ϡ��¡�����(0xe0, S,H,P,K,M)
		if (ch == 0xe0)
		{
			tstring strTmp;
			ch = getch();
			switch (ch)
			{
			case 'S':	// [delete]��
				if (pos < _tcslen(cmd))
				{
					_tcscpy_s(cmd + pos, _tcslen(cmd+pos+1)+1, cmd + pos + 1);
					_tprintf_s(TEXT("%s "), cmd + pos);
					for (i = 0; i < _tcslen(cmd + pos) + 1; i++)
					{
						_tprintf_s(TEXT("\b"));
					}
				}
				break;
			case 'H':	// ��
				if (cmdList.empty())
					break;

				if (posList != cmdList.cbegin())
				{										
					strTmp = *(--posList);
				}
				else
				{
					strTmp = *posList;
				}

				break;
			case 'P':	// ��
				if (cmdList.empty())
					break;

				if (posList == cmdList.cend())
				{
					while (pos--)
						_tprintf_s(TEXT("\b \b"));
					memset(cmd, 0, MAX_CMD_LEN + 1);
					pos = 0;
				}
				else if (++posList != cmdList.cend())
				{
					strTmp = *posList;
				}
				else if(pos)
				{
					while (pos--)
						_tprintf_s(TEXT("\b \b"));
					memset(cmd, 0, MAX_CMD_LEN + 1);
					pos = 0;
				}
					
				break;
			case 'K':	// ��
				if (pos > 0)
				{
					_tprintf_s(TEXT("\b"));
					pos--;
				}
				break;
			case 'M':	// ��
				if (pos < _tcslen(cmd))
				{
					_tprintf_s(TEXT("%c"), cmd[pos]);
					pos++;
				}
				break;
			default:
				break;
			}
			if (!strTmp.empty())
			{
				while (pos--)
					_tprintf_s(TEXT("\b \b"));
				memset(cmd, 0, MAX_CMD_LEN + 1);
				_tcscpy_s(cmd, strTmp.length()+1, strTmp.c_str());
				_tprintf_s(cmd);
				pos = strTmp.length();
			}
			continue;
		}

		// ��Ч�ַ�
		if (ch == 0x1b)
		{
			//GetAsyncKeyState(0x1b); // ΪʲôҪ���������
			continue;
		}

		// �����ַ��Ĵ���
		else
		{
			// ��������������󳤶�
			if (pos >= MAX_CMD_LEN)
			{
				_tprintf_s(TEXT("\ncommand too long !\n"));
				bInit = true;
				continue;
			}
			if (_tcslen(cmd) > pos)
			{
				_tcscpy_s(cmd_tmp, _tcslen(cmd+pos)+1, cmd + pos);
				_tcscpy_s(cmd + pos + 1, _tcslen(cmd_tmp)+1, cmd_tmp);
			}
			cmd[pos] = ch;
			_tprintf_s(TEXT("%s"), cmd + pos);
			for (unsigned int ii = 0; ii < _tcslen(cmd + pos) - 1; ii++)
			{
				_tprintf_s(TEXT("\b"));
			}
			pos++;
		}
	}
}

/********************************************************************/
/* ������: GetNextAssociat
/* ����: ��ȡ��cmdƥ����������ѭ����ȡ��ֻ��ʾ����ȫ�ƣ�
/* ����:
/*   cmd: ����ʸ�
/* ���:
/*   NULL: δ�ҵ�ƥ�����������
/*   char*: ƥ�������ָ��
/* �޸ļ�¼:
/*   2013-12-03,littledj: create
/********************************************************************/
PSHELL_HANDLER GCLI::GetNextAssociat(const tstring& cmd)
{
	if (m_handlerMap.empty() || cmd.empty())
	{
		return NULL;
	}

	PSHELL_HANDLER sh = new SHELL_HANDLER();
	static auto pos = m_handlerMap.cbegin(); // use static
	if (pos == m_handlerMap.cend() || ++pos == m_handlerMap.cend())
	{
		pos = m_handlerMap.cbegin();
	}	
	else
	{
		pos++;
	}

	for (size_t i = 0; i < m_handlerMap.size(); i++)
	{
		tstring fullCommand = (*pos).first;
		if (fullCommand.find(cmd) == 0)
		{
			*sh = (*pos).second;
			if (sh->type == CMD_TYPE::FULL)	//ֻ��ʾ����ȫ��
			{
				return sh;
			}
		}	
		if (++pos == m_handlerMap.cend())
		{
			pos = m_handlerMap.cbegin();
		}
	}	
	delete sh;
	return NULL;
}

/********************************************************************/
/* ������: ListAssociat
/* ����: ��ʾ��ȡ��cmdƥ�����������(���ڴ���'?'����)
/* ����:
/*   cmd: ����ʸ�
/* ���:
/*   ��
/* �޸ļ�¼:
/*   2013-12-03,littledj: create
/********************************************************************/
void GCLI::ListAssociat(const tstring& cmd)
{
	if (cmd.empty())
	{
		return;
	}

	PSHELL_HANDLER psh = NULL;
	tstring strFirst;
	int assCount = 0;	 // 
	while (1)
	{
		psh = GetNextAssociat(cmd);
		if (psh == NULL)
			break;
		if (++assCount == 1)
		{
			_tprintf_s(TEXT("\n"));
			strFirst = psh->strCMD;
		}
		if (assCount>1 && strFirst == psh->strCMD)
			break;

		_tprintf_s(TEXT("  %-10s  %s\n"), psh->strCMD.c_str(), psh->strToolTip.c_str());
	}
	if (assCount>0)
	{
		_tprintf_s(m_prompt.c_str());
		_tprintf_s(cmd.c_str());
	}
}

/********************************************************************/
/* ������: Exit
/* ����: ����û�û��ע�����exit�������������ô�Ĭ�Ϻ���
/* �޸ļ�¼:
/*   2013-12-03,littledj: create
/********************************************************************/
void GCLI::Exit()
{
	m_shouldExit = true;
}

/********************************************************************/
/* ������: ShowHelp
/* ����: ����û�û��ע�����help�������������ô�Ĭ�Ϻ���
/* �޸ļ�¼:
/*   2013-12-03,littledj: create
/********************************************************************/
void GCLI::ShowHelp()
{
	_tprintf_s(TEXT("This is DEFAULT help info:\n"));
	for each (auto var in m_handlerMap)
	{
		_tprintf_s(TEXT("  %-10s  %s\n"), var.first.c_str(), var.second.strToolTip.c_str());
	}
}

vector<tstring> GCLI::split(const tstring& str)
{
	vector<tstring> strList;
	if (str.empty())
	{
		return strList;
	}

	int comma_n = 0;
	tstring strIn = str;
	do
	{
		tstring tmp_s = TEXT("");
		comma_n = strIn.find(TEXT(" "));
		if (-1 == comma_n)
		{
			tmp_s = strIn.substr(0, strIn.length());
			strList.push_back(tmp_s);
			break;
		}
		tmp_s = strIn.substr(0, comma_n);
		strIn.erase(0, comma_n + 1);
		strList.push_back(tmp_s);
	} while (true);

	return strList;
}