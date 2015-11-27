// CMDShell.cpp
// 提供command shell通用功能

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
/* 函数名: Init
/* 描述: 初始化GCLI类
/* 输入:
/*   prompt: shell中显示的前导符（可以为空，默认值："analyze shell> "）
/* 输出:
/*   true: 初始化成功
/*   false: 初始化失败
/* 修改记录:
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
/* 函数名: RegisterHandler
/* 描述: 注册指定命令和对应的回调函数（每次注册一个）
/* 输入:
/*   handler: 结构体，包括命令、回调函数、描述
/* 输出:
/*   true: 注册成功
/*   false: 注册失败
/* 修改记录:
/*   2013-12-03,littledj: create
/********************************************************************/
bool GCLI::RegisterHandler(SHELL_HANDLER handler)
{
	// 参数检查
	if (handler.handler == NULL || handler.strCMD.empty())
	{
		return false;
	}
	if (handler.strCMD.length() > MAX_CMD_LEN ||
		handler.strToolTip.length() > MAX_TOOLTIP_LEN)
	{
		return false;
	}

	// 将命令和对应的回调函数加入map
	m_handlerMap.insert(pair<tstring, SHELL_HANDLER>(handler.strCMD, handler));
	return true;
}

/********************************************************************/
/* 函数名: RegisterHandlers
/* 描述: 注册指定命令和对应的回调函数（批量注册）
/* 输入:
/*   handler: 结构体数组，包括命令、回调函数、描述
/*   nCount: 结构体数组元素数量
/* 输出:
/*   true: 注册成功
/*   false: 注册失败
/* 修改记录:
/*   2013-12-03,littledj: create
/********************************************************************/
bool GCLI::RegisterHandlers(SHELL_HANDLER handler[], int count)
{
	// 参数检查
	if (handler == NULL || count == 0)
		return false;

	// 注册所有命令
	for (int i = 0; i<count; i++)
	{
		if (handler[i].type != CMD_TYPE::BAD)
			if (!RegisterHandler(handler[i]))
				return false;
	}
	return true;
}

/********************************************************************/
/* 函数名: DispatchCommand
/* 描述: 命令分发函数
/* 输入:
/*   cmd: 待分发的命令(包括携带的参数)
/* 输出:
/*   无
/* 修改记录:
/*   2013-12-02,littledj: create
/********************************************************************/
void GCLI::DispatchCommand(const tstring& command)
{
	// 参数检查
	if (command.empty() || command.length() == 0 || command.length() > MAX_CMD_LEN)
	{
		_tprintf_s(TEXT("command is <null> or too long!\n"));
		return;
	}

	// 调试信息
	//_tprintf_s(TEXT("debug: "));
	//_tprintf_s(command.c_str());
	//_tprintf_s(TEXT("\n"));

	// 确定命令
	vector<tstring> cmdList = split(command);
	int argc = cmdList.size() - 1;
	if (argc > MAX_CMD_PARA_COUNT)
	{
		_tprintf_s(TEXT("to many arguments!\n"));
		return;
	}

	// 从map中查找并调用对应回调函数	
	SHELL_HANDLER shell_handler;
	auto element = m_handlerMap.find(cmdList[0]);
	if (element != m_handlerMap.end())
	{
		shell_handler = (*element).second;
		(*shell_handler.handler)(command);	// 调用回调函数

		// 默认退出命令（必须调用对应默认函数）
		if (cmdList[0] == DEFAULT_CMD_EXIT)
		{
			Exit();
		}
	}
	// 默认帮助命令（默认函数将会被用户注册的函数覆盖）
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
/* 函数名: CMD_Loop
/* 描述: 命令处理主循环
/* 修改记录:
/*   2013-12-02,littledj: create
/*   2013-12-03,littledj: 增加[tab]和[?]的联想功能
/********************************************************************/
void GCLI::MainLoop()
{
	// 参数检查
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
	//bool bDirect = 0;	//  查询list的方向（用于上下翻看历史命令；）
	TCHAR *cmd = new TCHAR[MAX_CMD_LEN + 1];
	TCHAR *cmd_tmp = new TCHAR[MAX_CMD_LEN + 1];
	PSHELL_HANDLER cmdAss = NULL;
	unsigned int pos = 0;
	bool bInit = true;
	while (1)
	{
		// 判断是否退出
		if (m_shouldExit)
			break;

		// 判断是否初始化
		if (bInit)
		{
			_tprintf_s(m_prompt.c_str());
			pos = 0;
			memset(cmd, 0, MAX_CMD_LEN + 1);
			bInit = false;
		}

		// 用户输入一个字符
		unsigned char ch = getch();

		// [Tab]键的处理，补齐联想命令
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

		// 将联想命令拷贝到cmd，清空联想
		if (cmdAss)
		{
			memcpy(cmd, cmdAss->strCMD.c_str(), pos);
			delete cmdAss;
			cmdAss = NULL;
		}

		// '?'的处理
		// 如果用户直接输入'?'，则打印帮助信息（同"help"）;
		// 如果用户在输入几个字符后加'?'，无对应命令则显示联想，
		// 有对应命令则显示命令的用法
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

		// [Backspace]键的处理
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

		// 回车键的处理
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

		// 处理按键[delete]、上、下、左、右(0xe0, S,H,P,K,M)
		if (ch == 0xe0)
		{
			tstring strTmp;
			ch = getch();
			switch (ch)
			{
			case 'S':	// [delete]键
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
			case 'H':	// 上
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
			case 'P':	// 下
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
			case 'K':	// 左
				if (pos > 0)
				{
					_tprintf_s(TEXT("\b"));
					pos--;
				}
				break;
			case 'M':	// 右
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

		// 无效字符
		if (ch == 0x1b)
		{
			//GetAsyncKeyState(0x1b); // 为什么要这个函数？
			continue;
		}

		// 其他字符的处理
		else
		{
			// 命令超过了允许的最大长度
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
/* 函数名: GetNextAssociat
/* 描述: 获取与cmd匹配的联想命令（循环获取，只显示命令全称）
/* 输入:
/*   cmd: 联想词根
/* 输出:
/*   NULL: 未找到匹配的联想命令
/*   char*: 匹配的命令指针
/* 修改记录:
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
			if (sh->type == CMD_TYPE::FULL)	//只显示命令全称
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
/* 函数名: ListAssociat
/* 描述: 显示获取与cmd匹配的联想命令(用于处理'?'命令)
/* 输入:
/*   cmd: 联想词根
/* 输出:
/*   无
/* 修改记录:
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
/* 函数名: Exit
/* 描述: 如果用户没有注册命令“exit”，则该命令将调用此默认函数
/* 修改记录:
/*   2013-12-03,littledj: create
/********************************************************************/
void GCLI::Exit()
{
	m_shouldExit = true;
}

/********************************************************************/
/* 函数名: ShowHelp
/* 描述: 如果用户没有注册命令“help”，则该命令将调用此默认函数
/* 修改记录:
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