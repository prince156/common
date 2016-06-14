#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <fstream>

#include "glogger2.h"

namespace gcommon
{
	GLogger2::GLogger2()
	{
		m_wpTarget = PRINT_TARGET::BOTH;		// 打印输出目标
		m_msg = TEXT("");
		m_msgPoolCount = 0;
		m_msgPoolPos = 0;
		m_poolMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_MsgPool")); // 如果已经存在，则将得到已有的mutex
		m_logMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_MsgLog"));
		m_printMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_MsgPrint"));
		m_logFile = TEXT("default.log");
		m_header = TEXT("##");
		m_debugLevel = 0;
		m_logDebugLevel = 0;
		m_enableColor = false;
		m_defaultColor = PRINT_COLOR::DARK_WHITE;
	}

	GLogger2::~GLogger2()
	{
		if (m_logMutex) ReleaseMutex(m_logMutex);
		if (m_printMutex) ReleaseMutex(m_printMutex);
		if (m_poolMutex) ReleaseMutex(m_poolMutex);
	}

	/********************************************************************
	* [函数名]: enableColor
	* [描述]: 设置是否允许彩色输出
	* [修改记录]:
	*   2015-11-21,littledj: create
	********************************************************************/
	void GLogger2::enableColor(bool para)
	{
		m_enableColor = para;
	}

	/********************************************************************
	* [函数名]: setDefaultColor
	* [描述]: 设置默认彩色
	* [修改记录]:
	*   2015-11-21,littledj: create
	********************************************************************/
	void GLogger2::setDefaultColor(PRINT_COLOR color)
	{
		m_defaultColor = color;
	}

	/********************************************************************
	* [函数名]: setOnlyScreen
	* [描述]: 设置只有屏幕输出
	* [修改记录]:
	*   2015-05-26,littledj: create
	********************************************************************/
	void GLogger2::setOnlyScreen()
	{
		m_wpTarget = PRINT_TARGET::SCREEN;
	}

	/********************************************************************
	* [函数名]: setOnlyLogfile
	* [描述]: 设置只有日志输出
	* [修改记录]:
	*   2015-05-26,littledj: create
	********************************************************************/
	void GLogger2::setOnlyLogfile()
	{
		m_wpTarget = PRINT_TARGET::FILE;
	}

	/********************************************************************
	* [函数名]: setTargetBoth
	* [描述]: 设置屏幕和日志都有输出
	* [修改记录]:
	*   2015-05-26,littledj: create
	********************************************************************/
	void GLogger2::setTargetBoth()
	{
		m_wpTarget = PRINT_TARGET::BOTH;
	}

	void GLogger2::setTarget(PRINT_TARGET target)
	{
		m_wpTarget = target;
	}

	/********************************************************************
	* [函数名]: setHeader
	* [描述]: 设置打印时需要显示的名称
	* [输入]:
	*   header: 名称
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::setHeader(const tstring header)
	{
		if (header.length() > MAX_HEADER_LEN)
		{
			m_header = TEXT("##");
			return;
		}

		if (header.empty())
		{
			m_header = TEXT("");
			return;
		}

		m_header = header;
	}

	/********************************************************************
	* [函数名]: setLogFile
	* [描述]: 设置日志文件路径
	* [输入]:
	*   logFile: 日志文件名或路径
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::setLogFile(const tstring logFile)
	{
		if (logFile.empty() || logFile.length() > MAX_PATH)
		{
			m_logFile = TEXT("");
			return;
		}

		m_logFile = logFile;
	}

	/********************************************************************
	* [函数名]: setDebugLevel
	* [描述]: 设置调试信息等级
	* [输入]:
	*   debugLevel: 调试等级
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::setDebugLevel(const int debugLevel)
	{
		m_debugLevel = 
			(debugLevel > MAX_DEBUG_LEVEL) ? MAX_DEBUG_LEVEL : debugLevel;
		m_debugLevel = 
			(debugLevel < MIN_DEBUG_LEVEL) ? MIN_DEBUG_LEVEL : debugLevel;
	}

	/********************************************************************
	* [函数名]: setLogDebugLevel
	* [描述]: 设置日志调试信息等级
	* [输入]:
	*   logDebugLevel: 调试等级
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::setLogDebugLevel(const int logDebugLevel)
	{
		m_logDebugLevel = 
			(logDebugLevel > MAX_DEBUG_LEVEL) ? MAX_DEBUG_LEVEL : logDebugLevel;
		m_logDebugLevel = 
			(logDebugLevel < MIN_DEBUG_LEVEL) ? MIN_DEBUG_LEVEL : logDebugLevel;
	}

	/********************************************************************
	* [函数名]: getLastMsg
	* [描述]: 获取当前消息m_msg
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	tstring GLogger2::getLastMsg()
	{
		return m_msg;
	}
	tstring GLogger2::getLastError()
	{
		return m_errormsg;
	}
	tstring GLogger2::getLastWarning()
	{
		if (!m_errormsg.empty())
			return m_errormsg;
		return m_warningmsg;
	}

	/********************************************************************
	* [函数名]: output
	* [描述]: 打印信息
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::output(const tstring& msg, PRINT_COLOR color)
	{
		tstring out_str;
		if (!msg.empty())
			out_str = msg;
		else
			out_str = m_msg;

		// 判断是否输出到屏幕
		if (m_wpTarget == PRINT_TARGET::SCREEN)
		{
			output_screen(out_str, color);
		}

		// 判断是否输出到日志文件
		else if (m_wpTarget == PRINT_TARGET::FILE)
		{
			output_file(out_str);
		}	

		// 都输出
		else if (m_wpTarget == PRINT_TARGET::BOTH)
		{
			output_screen(out_str, color);
			output_file(out_str);
		}
	}

	/********************************************************************
	* [函数名]: output_screen
	* [描述]: 将消息打印至屏幕
	* [修改记录]:
	*   2015-05-27,littledj: create
	********************************************************************/
	void GLogger2::output_screen(const tstring& msg, PRINT_COLOR color)
	{
		tstring out_str;
		if (!msg.empty())
			out_str = msg;
		else
			out_str = m_msg;

		if (m_printMutex == NULL)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
			tcout << out_str ; // 请勿使用 << ends，否则导致下行多一个空格
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)PRINT_COLOR::DARK_WHITE); // 默认白色
		}
		else
		{
			// 等待打印信号量（防止多线程同时打印时，相互间信息错位）
			if (WaitForSingleObject(m_printMutex, MUTEX_TIMEOUT) == 0)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
				tcout << out_str ;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)PRINT_COLOR::DARK_WHITE);
			}
			ReleaseMutex(m_printMutex);// 释放打印信号量
		}
	}

	/********************************************************************
	* [函数名]: output_file
	* [描述]: 将消息写入日志文件
	* [修改记录]:
	*   2015-05-27,littledj: create
	********************************************************************/
	void GLogger2::output_file(const tstring& msg)
	{
		tstring out_str;
		if (!msg.empty())
			out_str = msg;
		else
			out_str = m_msg;

		if (m_logMutex == NULL)
		{
			tofstream logfile(m_logFile, ios_base::out | ios_base::app);
			if (!logfile.bad())
			{
				logfile << out_str;
				logfile.close();
			}
		}
		else
		{
			// 等待日志信号量
			if (WaitForSingleObject(m_logMutex, MUTEX_TIMEOUT) == 0)
			{
				tofstream logfile(m_logFile, ios_base::out | ios_base::app);
				if (!logfile.bad())
				{
					logfile << out_str;
					logfile.close();
				}
			}
			ReleaseMutex(m_logMutex);// 释放日志信号量
		}
	}

	/********************************************************************
	* [函数名]: formatMsg
	* [描述]: 格式化当前消息m_msg
	* [输入]:
	*   type：信息类型（INFO、WARNING、...）
	*   format: 输出信息格式化字符串
	*   ap: 参数列表
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::formatMsg_v(const PRINT_TYPE type, const tstring& format, va_list ap)
	{
		if (format.find('%') != tstring::npos)
		{
			TCHAR* msg_tmp = new TCHAR[format.length() + 1024];
			_vstprintf_s(msg_tmp, format.length() + 1024, format.c_str(), ap);
			m_msg.assign(msg_tmp);
			delete[] msg_tmp;
		}
		else
		{
			m_msg = format;
		}

		if (type != PRINT_TYPE::RAW)
		{
			m_msg = PRINT_TYPE_ICON(type) + TEXT("[") + m_header + TEXT("] ") + m_msg + TEXT("\n");
		}

		// 为了防止溢出，将信息截断为MAX_MSG_LEN
		if (m_msg.length() > MAX_MSG_LEN)
		{			
			if (*(--m_msg.cend()) == '\n')
			{
				m_msg.erase(m_msg.cbegin() + MAX_MSG_LEN - 3, m_msg.cend());
				m_msg.append(TEXT("..\n"));
			}
			else
			{
				m_msg.erase(m_msg.cbegin() + MAX_MSG_LEN - 3, m_msg.cend());
				m_msg.append(TEXT("..."));
			}
		}			
	}

	/********************************************************************
	* [函数名]: formatMsg
	* [描述]: 格式化当前消息m_msg，可变参数
	* [输入]:
	*   type：信息类型（INFO、WARNING、...）
	*   format: 输出信息格式化字符串
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::formatMsg(const PRINT_TYPE type, const tstring format, ...)
	{
		va_list ap;
		va_start(ap, format);
		formatMsg_v(type, format, ap);
		va_end(ap);
	}

	/********************************************************************
	* [函数名]: saveToMessagePool
	* [描述]: 保存当前消息至消息池
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::saveToMessagePool()
	{
		// 保存至消息缓冲区
		if (m_poolMutex)
		{
			if (WaitForSingleObject(m_poolMutex, MUTEX_TIMEOUT) == 0)
			{
				m_msgPool[m_msgPoolPos] = m_msg;
				m_msgPoolPos = (m_msgPoolPos + 1) % MAX_POOL_SIZE;

				if (m_msgPoolCount < MAX_POOL_SIZE)
				{
					m_msgPoolCount++;
				}
			}
			ReleaseMutex(m_poolMutex);
		}
	}

	/********************************************************************
	* [函数名]: error
	* [描述]: 打印错误信息
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::error(const tstring format, ...)
	{
		this->m_msg.clear();
		va_list ap;
		va_start(ap, format);
		formatMsg_v(PRINT_TYPE::ERR, format, ap);
		va_end(ap);

		// 输出
		if (m_enableColor)	// 红色
			output(m_msg, PRINT_COLOR::BRIGHT_RED);
		else
			output(m_msg);

		// 保存当前消息
		saveToMessagePool();
		m_errormsg = m_msg;
	}	

	/********************************************************************
	* [函数名]: warning
	* [描述]: 打印警告信息
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::warning(const tstring format, ...)
	{
		this->m_msg.clear();
		va_list ap;
		va_start(ap, format);
		formatMsg_v(PRINT_TYPE::WARNING, format, ap);
		va_end(ap);

		// 输出
		if (m_enableColor)	// 黄色
			output(m_msg, PRINT_COLOR::DARK_YELLOW);
		else
			output(m_msg);

		// 保存当前消息
		saveToMessagePool();
		m_warningmsg = m_msg;
		m_errormsg = m_msg;
	}

	/********************************************************************
	* [函数名]: info
	* [描述]: 打印普通信息
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::info(const tstring format, ...)
	{
		this->m_msg.clear();
		va_list ap;
		va_start(ap, format);
		formatMsg_v(PRINT_TYPE::INFO, format, ap);
		va_end(ap);	

		if (m_enableColor)
			output(m_msg, m_defaultColor);
		else
			output(m_msg);

		// 保存当前消息
		saveToMessagePool();
	}

	/********************************************************************
	* [函数名]: debug
	* [描述]: 打印调试信息
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::debug1(const tstring format, ...)
	{
		this->m_msg.clear();
		if (m_debugLevel>0)
		{
			va_list ap;
			va_start(ap, format);
			formatMsg_v(PRINT_TYPE::DEBUG1, format, ap);
			va_end(ap);
			
			// 输出
			if (m_enableColor)	// 绿色
				output(m_msg, PRINT_COLOR::DARK_GREEN);
			else
				output(m_msg);

			// 保存当前消息
			saveToMessagePool();
		}
	}
	void GLogger2::debug2(const tstring format, ...)
	{
		this->m_msg.clear();
		if (m_debugLevel>1)
		{
			va_list ap;
			va_start(ap, format);
			formatMsg_v(PRINT_TYPE::DEBUG2, format, ap);
			va_end(ap);

			// 输出
			if (m_enableColor)	
				output(m_msg, PRINT_COLOR::DARK_GREEN);
			else
				output(m_msg);
		}
	}
	void GLogger2::debug3(const tstring format, ...)
	{
		this->m_msg.clear();
		if (m_debugLevel>2)
		{
			va_list ap;
			va_start(ap, format);
			formatMsg_v(PRINT_TYPE::DEBUG3, format, ap);
			va_end(ap);

			// 输出
			if (m_enableColor)	
				output(m_msg, PRINT_COLOR::DARK_GREEN);
				output(m_msg);

			// 保存当前消息
			saveToMessagePool();
		}
	}

	/********************************************************************
	* [函数名]: screen
	* [描述]: 输出信息至屏幕
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::screen(const tstring format, ...)
	{
		this->m_msg.clear();
		va_list ap;
		va_start(ap, format);
		formatMsg_v(PRINT_TYPE::RAW, format, ap);
		va_end(ap);
		
		if (m_enableColor)
			output_screen(m_msg, m_defaultColor);
		else
			output_screen(m_msg);

		// 保存当前消息
		saveToMessagePool();
	}

	/********************************************************************
	* [函数名]: logfile
	* [描述]: 原始日志信息
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::logfile(const tstring format, ...)
	{
		this->m_msg.clear();
		va_list ap;
		va_start(ap, format);
		formatMsg_v(PRINT_TYPE::RAW, format, ap);
		va_end(ap);

		output_file();

		// 保存当前消息
		saveToMessagePool();
	}

	/********************************************************************
	* [函数名]: insertCurrentTime
	* [描述]: 日志当前时间 (eg: YYYY-MM-DD hh:mm:ss)
	*         y: year    M: month    d: day
	*         h: hour    m: minute   s: second
	* [修改记录]:
	*   2015-05-20,littledj: create
	*   2015-05-20,littledj: 增加自定义格式
	********************************************************************/
	void GLogger2::insertCurrentTime(tstring format)
	{
		this->m_msg.clear();
		if (format.empty())
		{
			format = TEXT("** yyyy-MM-dd hh:mm:ss **\n");
		}

		TCHAR* ct = new TCHAR[format.length() + 4];
		
		_tcscpy_s(ct, format.length() + 4, format.c_str());
		GetDateFormat(0, 0, NULL, format.c_str(), ct, (uint32_t)format.length() + 4);
		GetTimeFormat(0, 0, NULL, ct, ct, (uint32_t)format.length() + 4);

		formatMsg(PRINT_TYPE::RAW, TEXT("%s"), ct);
		delete[] ct;

		if (m_enableColor)
			output(m_msg, m_defaultColor);
		else
			output(m_msg);

		// 保存当前消息
		saveToMessagePool();
	}

	/********************************************************************
	* [函数名]: logMessagePool
	* [描述]: 将消息池中的信息写入日志
	* [修改记录]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::logMessagePool()
	{
		if (m_poolMutex)
		{
			if (WaitForSingleObject(m_poolMutex, MUTEX_TIMEOUT) == 0)
			{
				int msgStart = (m_msgPoolPos + MAX_POOL_SIZE - m_msgPoolCount) % MAX_POOL_SIZE;
				for (int i = 0; i < m_msgPoolCount; i++)
				{
					int msgReadPos = (msgStart + i) % MAX_POOL_SIZE;
					output_file(m_msgPool[msgReadPos]);					
				}
			}
			ReleaseMutex(m_poolMutex);
		}
	}

	/********************************************************************
	* [函数名]: clearMessagePool
	* [描述]: 将消息池清空
	* [修改记录]:
	*   2015-05-27,littledj: create
	********************************************************************/
	void GLogger2::clearMessagePool()
	{
		if (m_poolMutex)
		{
			if (WaitForSingleObject(m_poolMutex, MUTEX_TIMEOUT) == 0)
			{
				this->m_msgPoolCount = 0;
				this->m_msgPoolPos = 0;
			}
			ReleaseMutex(m_poolMutex);
		}
	}

	/********************************************************************
	* [函数名]: PRINT_TYPE_ICON
	* [描述]: 返回类型的图标
	* [修改记录]:
	*   2015-05-25,littledj: create
	********************************************************************/	
	tstring GLogger2::PRINT_TYPE_ICON(const PRINT_TYPE type)
	{
		switch (type)
		{
		case PRINT_TYPE::RAW:
			return TEXT("");		// raw
		case PRINT_TYPE::INFO:
			return TEXT("   ");		// info
		case PRINT_TYPE::WARNING:
			return TEXT(" ! ");		// warning
		case PRINT_TYPE::ERR:
			return TEXT(" x ");		// error
		case PRINT_TYPE::DEBUG1:
			return TEXT("+1 ");		// debug1
		case PRINT_TYPE::DEBUG2:
			return TEXT("+2 ");		// debug2
		case PRINT_TYPE::DEBUG3:
			return TEXT("+3 ");	// debug3
		case PRINT_TYPE::UNKNOWN:
			return TEXT(" ? ");		// unknown
		default:
			return TEXT(" ? ");
		}
	}
}