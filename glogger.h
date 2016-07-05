// ver 2.0
// modified: 
//   !!�벻Ҫ�Ը��ļ�ʹ��Ԥ����ͷ!!
//   2014-12-17,littledj: info��warning��error��debug����������Զ����ӻس�
//   2014-12-19,littledj: ?�޷��������
//   2015-11-21,littledj: ֧������
//	 2015-11-21,littledj: ���Ӳ�ɫ������ܣ���ͬ��Ϣ����һĿ��Ȼ
//   2016-07-06,littledj: ��linux�ϱ���ɹ���g++ -std=c++11 -D__LINUX__


#pragma once
#include <string>
#include "tstream.h"

using namespace std;

namespace gcommon
{
	enum class PRINT_COLOR
	{
		DARK_BLUE = 1,
		DARK_GREEN = 2,
		DARK_CYAN = 3,
		DARK_RED = 4,
		DARK_PURPLE = 5,
		DARK_YELLOW = 6,
		DARK_WHITE = 7,
		BRIGHT_BLUE = 9,	
		BRIGHT_GREEN = 10,
		BRIGHT_CYAN = 11,
		BRIGHT_RED = 12,
		BRIGHT_PURPLE = 13,
		BRIGHT_YELLOW = 14,
		BRIGHT_WHITE = 15,
	};

	static tstring LINUX_COLOR[] =
	{
		TEXT("\033[0m"),
		TEXT("\033[34m"),
		TEXT("\033[32m"),
		TEXT("\033[36m"),
		TEXT("\033[31m"),
		TEXT("\033[35m"),
		TEXT("\033[33m"),
		TEXT("\033[37m"),
		TEXT("\033[30m"),
		TEXT("\033[1m\033[34m"),
		TEXT("\033[1m\033[32m"),
		TEXT("\033[1m\033[36m"),
		TEXT("\033[1m\033[31m"),
		TEXT("\033[1m\033[35m"),
		TEXT("\033[1m\033[33m"),
		TEXT("\033[1m\033[37m"),
		TEXT("\033[1m\033[30m"),
	};
	
	enum class PRINT_TYPE
	{
		RAW = 0,		// ԭʼ��Ϣ�������κθ�����Ϣ��
		INFO = 1,		// ��ͨ��Ϣ
		WARNING = 2,	// ������Ϣ
		ERR = 3,		// ������Ϣ
		DEBUG1 = 4,		// ������Ϣ��level=1��
		DEBUG2 = 5,		// ������Ϣ��level=2��
		DEBUG3 = 6,		// ������Ϣ��level=3��
		UNKNOWN = 7,	// δ֪����
	};
	enum class PRINT_TARGET
	{
		BLANK = 0,
		SCREEN = 1,
		FILE = 2,
		BOTH = 3,
	};

	class GLogger
	{
	public:
		GLogger();
		~GLogger();

	public:
		// ��ɫ�������
		void enableColor(bool para = true);
		// ����Ĭ����ɫ
		void setDefaultColor(PRINT_COLOR color = PRINT_COLOR::DARK_WHITE);
		// �������Ŀ��:��Ļ
		void setOnlyScreen();
		// �������Ŀ�꣺��־
		void setOnlyLogfile();
		// �������Ŀ�꣺��Ļ����־
		void setTargetBoth();
		// �������
		void setTarget(PRINT_TARGET target);
		// ��ȡ��ǰ��Ϣ
		tstring getLastMsg();
		// ��ȡ������Ϣ
		tstring getLastError();
		// ��ȡ������Ϣ
		tstring getLastWarning();

		// ��������
		void setHeader(const tstring header = TEXT("##"));

		// ������־�ļ�·��
		void setLogFile(const tstring logFile = TEXT(""));

		// ���õ�����Ϣ�ȼ�
		void setDebugLevel(const int debugLevel = 0);
		// ������־�ĵ�����Ϣ�ȼ�
		void setLogDebugLevel(const int debugLevel = 0);

		// ���������Ϣ
		void error(const tstring format, ...);

		// ���������Ϣ
		void warning(const tstring format, ...);

		// �����ͨ��Ϣ
		void info(const tstring format, ...);

		// ֻ������Եȼ�1����Ϣ
		void debug1(const tstring format, ...);

		// ������Եȼ�1/2����Ϣ
		void debug2(const tstring format, ...);

		// ������Եȼ�1/2/3����Ϣ
		void debug3(const tstring format, ...);

		// ���ԭʼ��Ϣ����Ļ��������κ�ǰ���ַ���
		void screen(const tstring format, ...);

		// ���ԭʼ��Ϣ����־��������κ�ǰ���ַ���
		void logfile(const tstring format, ...);

		// ���뵱ǰʱ��
		void insertCurrentTime(tstring format = TEXT("** yyyy-MM-dd HH:mm:ss **\n"));

		// ����Ϣ���е���Ϣд����־
		void logMessagePool();
		// �����Ϣ��
		void clearMessagePool();

	private:
		// �̰߳�ȫ����Ϣ�������
		void output(const tstring& msg = TEXT(""), PRINT_COLOR color = PRINT_COLOR::DARK_WHITE);
		// ��ӡ
		void output_screen(const tstring& msg = TEXT(""), PRINT_COLOR color = PRINT_COLOR::DARK_WHITE);
		// д�ļ�
		void output_file(const tstring& msg = TEXT(""));
		// ��ʽ���ַ������浽m_msg
		void formatMsg_v(const PRINT_TYPE type, const tstring& format, va_list ap);
		// ��ʽ���ַ������浽m_msg
		void formatMsg(const PRINT_TYPE type, const tstring format, ...);
		// ���浱ǰ��Ϣ����Ϣ��
		void saveToMessagePool();
		// ��ʾ����ͼ��
		tstring PRINT_TYPE_ICON(const PRINT_TYPE type);

	private:
		//short m_wpTarget;		// ���Ŀ��
		const static int MUTEX_TIMEOUT = 2000; // ms
		const static int MAX_HEADER_LEN = 16;  // bytes
		const static int MAX_DEBUG_LEVEL = 3;
		const static int MIN_DEBUG_LEVEL = 0;
		const static int MAX_MESSAGE_LEN = 1000;   // bytes
		const static int MAX_POOL_SIZE = 100;

#ifndef MAX_PATH
		const static int MAX_PATH = 128;
#endif

		PRINT_TARGET m_wpTarget;// ���Ŀ��
		tstring m_msg;			// ��ǰ��Ϣ
		tstring m_errormsg;		// ������Ϣ
		tstring m_warningmsg;	// ������Ϣ
		tstring m_msgPool[MAX_POOL_SIZE];	// ��Ϣ��
		int m_msgPoolCount;		// ��Ϣ���е���Ϣ����
		int m_msgPoolPos;		// ��Ϣ�ص�ǰλ��
		tstring m_logFile;		// ��־�ļ�·��
		tstring m_header;		// ��Ϣͷ
		int m_debugLevel;		// ������Ϣ��ʾ�ȼ�
		int m_logDebugLevel;	// ������Ϣ��־�ȼ�
		bool m_enableColor;		// �����ɫ���
		PRINT_COLOR m_defaultColor; // Ĭ����ɫ
	};
}