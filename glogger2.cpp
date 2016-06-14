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
		m_wpTarget = PRINT_TARGET::BOTH;		// ��ӡ���Ŀ��
		m_msg = TEXT("");
		m_msgPoolCount = 0;
		m_msgPoolPos = 0;
		m_poolMutex = CreateMutex(NULL, FALSE, TEXT("Mutex_MsgPool")); // ����Ѿ����ڣ��򽫵õ����е�mutex
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
	* [������]: enableColor
	* [����]: �����Ƿ������ɫ���
	* [�޸ļ�¼]:
	*   2015-11-21,littledj: create
	********************************************************************/
	void GLogger2::enableColor(bool para)
	{
		m_enableColor = para;
	}

	/********************************************************************
	* [������]: setDefaultColor
	* [����]: ����Ĭ�ϲ�ɫ
	* [�޸ļ�¼]:
	*   2015-11-21,littledj: create
	********************************************************************/
	void GLogger2::setDefaultColor(PRINT_COLOR color)
	{
		m_defaultColor = color;
	}

	/********************************************************************
	* [������]: setOnlyScreen
	* [����]: ����ֻ����Ļ���
	* [�޸ļ�¼]:
	*   2015-05-26,littledj: create
	********************************************************************/
	void GLogger2::setOnlyScreen()
	{
		m_wpTarget = PRINT_TARGET::SCREEN;
	}

	/********************************************************************
	* [������]: setOnlyLogfile
	* [����]: ����ֻ����־���
	* [�޸ļ�¼]:
	*   2015-05-26,littledj: create
	********************************************************************/
	void GLogger2::setOnlyLogfile()
	{
		m_wpTarget = PRINT_TARGET::FILE;
	}

	/********************************************************************
	* [������]: setTargetBoth
	* [����]: ������Ļ����־�������
	* [�޸ļ�¼]:
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
	* [������]: setHeader
	* [����]: ���ô�ӡʱ��Ҫ��ʾ������
	* [����]:
	*   header: ����
	* [�޸ļ�¼]:
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
	* [������]: setLogFile
	* [����]: ������־�ļ�·��
	* [����]:
	*   logFile: ��־�ļ�����·��
	* [�޸ļ�¼]:
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
	* [������]: setDebugLevel
	* [����]: ���õ�����Ϣ�ȼ�
	* [����]:
	*   debugLevel: ���Եȼ�
	* [�޸ļ�¼]:
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
	* [������]: setLogDebugLevel
	* [����]: ������־������Ϣ�ȼ�
	* [����]:
	*   logDebugLevel: ���Եȼ�
	* [�޸ļ�¼]:
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
	* [������]: getLastMsg
	* [����]: ��ȡ��ǰ��Ϣm_msg
	* [�޸ļ�¼]:
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
	* [������]: output
	* [����]: ��ӡ��Ϣ
	* [�޸ļ�¼]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::output(const tstring& msg, PRINT_COLOR color)
	{
		tstring out_str;
		if (!msg.empty())
			out_str = msg;
		else
			out_str = m_msg;

		// �ж��Ƿ��������Ļ
		if (m_wpTarget == PRINT_TARGET::SCREEN)
		{
			output_screen(out_str, color);
		}

		// �ж��Ƿ��������־�ļ�
		else if (m_wpTarget == PRINT_TARGET::FILE)
		{
			output_file(out_str);
		}	

		// �����
		else if (m_wpTarget == PRINT_TARGET::BOTH)
		{
			output_screen(out_str, color);
			output_file(out_str);
		}
	}

	/********************************************************************
	* [������]: output_screen
	* [����]: ����Ϣ��ӡ����Ļ
	* [�޸ļ�¼]:
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
			tcout << out_str ; // ����ʹ�� << ends�����������ж�һ���ո�
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)PRINT_COLOR::DARK_WHITE); // Ĭ�ϰ�ɫ
		}
		else
		{
			// �ȴ���ӡ�ź�������ֹ���߳�ͬʱ��ӡʱ���໥����Ϣ��λ��
			if (WaitForSingleObject(m_printMutex, MUTEX_TIMEOUT) == 0)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
				tcout << out_str ;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)PRINT_COLOR::DARK_WHITE);
			}
			ReleaseMutex(m_printMutex);// �ͷŴ�ӡ�ź���
		}
	}

	/********************************************************************
	* [������]: output_file
	* [����]: ����Ϣд����־�ļ�
	* [�޸ļ�¼]:
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
			// �ȴ���־�ź���
			if (WaitForSingleObject(m_logMutex, MUTEX_TIMEOUT) == 0)
			{
				tofstream logfile(m_logFile, ios_base::out | ios_base::app);
				if (!logfile.bad())
				{
					logfile << out_str;
					logfile.close();
				}
			}
			ReleaseMutex(m_logMutex);// �ͷ���־�ź���
		}
	}

	/********************************************************************
	* [������]: formatMsg
	* [����]: ��ʽ����ǰ��Ϣm_msg
	* [����]:
	*   type����Ϣ���ͣ�INFO��WARNING��...��
	*   format: �����Ϣ��ʽ���ַ���
	*   ap: �����б�
	* [�޸ļ�¼]:
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

		// Ϊ�˷�ֹ���������Ϣ�ض�ΪMAX_MSG_LEN
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
	* [������]: formatMsg
	* [����]: ��ʽ����ǰ��Ϣm_msg���ɱ����
	* [����]:
	*   type����Ϣ���ͣ�INFO��WARNING��...��
	*   format: �����Ϣ��ʽ���ַ���
	* [�޸ļ�¼]:
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
	* [������]: saveToMessagePool
	* [����]: ���浱ǰ��Ϣ����Ϣ��
	* [�޸ļ�¼]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::saveToMessagePool()
	{
		// ��������Ϣ������
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
	* [������]: error
	* [����]: ��ӡ������Ϣ
	* [�޸ļ�¼]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::error(const tstring format, ...)
	{
		this->m_msg.clear();
		va_list ap;
		va_start(ap, format);
		formatMsg_v(PRINT_TYPE::ERR, format, ap);
		va_end(ap);

		// ���
		if (m_enableColor)	// ��ɫ
			output(m_msg, PRINT_COLOR::BRIGHT_RED);
		else
			output(m_msg);

		// ���浱ǰ��Ϣ
		saveToMessagePool();
		m_errormsg = m_msg;
	}	

	/********************************************************************
	* [������]: warning
	* [����]: ��ӡ������Ϣ
	* [�޸ļ�¼]:
	*   2015-05-20,littledj: create
	********************************************************************/
	void GLogger2::warning(const tstring format, ...)
	{
		this->m_msg.clear();
		va_list ap;
		va_start(ap, format);
		formatMsg_v(PRINT_TYPE::WARNING, format, ap);
		va_end(ap);

		// ���
		if (m_enableColor)	// ��ɫ
			output(m_msg, PRINT_COLOR::DARK_YELLOW);
		else
			output(m_msg);

		// ���浱ǰ��Ϣ
		saveToMessagePool();
		m_warningmsg = m_msg;
		m_errormsg = m_msg;
	}

	/********************************************************************
	* [������]: info
	* [����]: ��ӡ��ͨ��Ϣ
	* [�޸ļ�¼]:
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

		// ���浱ǰ��Ϣ
		saveToMessagePool();
	}

	/********************************************************************
	* [������]: debug
	* [����]: ��ӡ������Ϣ
	* [�޸ļ�¼]:
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
			
			// ���
			if (m_enableColor)	// ��ɫ
				output(m_msg, PRINT_COLOR::DARK_GREEN);
			else
				output(m_msg);

			// ���浱ǰ��Ϣ
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

			// ���
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

			// ���
			if (m_enableColor)	
				output(m_msg, PRINT_COLOR::DARK_GREEN);
				output(m_msg);

			// ���浱ǰ��Ϣ
			saveToMessagePool();
		}
	}

	/********************************************************************
	* [������]: screen
	* [����]: �����Ϣ����Ļ
	* [�޸ļ�¼]:
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

		// ���浱ǰ��Ϣ
		saveToMessagePool();
	}

	/********************************************************************
	* [������]: logfile
	* [����]: ԭʼ��־��Ϣ
	* [�޸ļ�¼]:
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

		// ���浱ǰ��Ϣ
		saveToMessagePool();
	}

	/********************************************************************
	* [������]: insertCurrentTime
	* [����]: ��־��ǰʱ�� (eg: YYYY-MM-DD hh:mm:ss)
	*         y: year    M: month    d: day
	*         h: hour    m: minute   s: second
	* [�޸ļ�¼]:
	*   2015-05-20,littledj: create
	*   2015-05-20,littledj: �����Զ����ʽ
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

		// ���浱ǰ��Ϣ
		saveToMessagePool();
	}

	/********************************************************************
	* [������]: logMessagePool
	* [����]: ����Ϣ���е���Ϣд����־
	* [�޸ļ�¼]:
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
	* [������]: clearMessagePool
	* [����]: ����Ϣ�����
	* [�޸ļ�¼]:
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
	* [������]: PRINT_TYPE_ICON
	* [����]: �������͵�ͼ��
	* [�޸ļ�¼]:
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