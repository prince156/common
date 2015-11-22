#pragma once
#include <string>
#include <cstdint>
#include "windows.h"
#include "glogger2.h"

using namespace std;
namespace gcommon
{
	enum class THREAD_CONTROL
	{
		START,		// 开始执行		
		STOP,		// 停止
		SUSPEND,	// 暂停
		CONTINUE,	// 继续执行
	};

	// 线程或模块的运行状态常量,及其名称
	enum class THREAD_STATE
	{
		NONE,				// 其他
		CREATE,				// 创建
		INIT,				// 初始化
		RUNNING,			// 正常执行
		SUSPENDING,			// 挂起
		SUSPEND_WARNING,	// 因告警挂起
		SUSPEND_ERROR,		// 因错误挂起
		STOPPED,			// 已停止
		SLEEPING			// 睡眠中
	};

#pragma pack(push,1)

	// 线程参数结构体
	typedef struct _thread_para
	{
		tstring threadName;		// 线程名称
		uint32_t debugLevel;	// 调试信息显示级别
		PRINT_TARGET printTarget;	// 线程消息打印输出选择
		tstring logfile;			// 日志文件全路径
		bool enableColor;		// 允许彩色输出

		THREAD_CONTROL control; // 由构造函数设置，对线程进行控制
		THREAD_STATE state;		// 由构造函数设置，线程状态
		void* current;			// 由构造函数设置，当前线程所在类
	}THREAD_PARA, *PTHREAD_PARA;

#pragma pack(pop)

	class GThread
	{
	public:
		GThread(PTHREAD_PARA para);
		virtual ~GThread();

	private:
		PTHREAD_PARA m_para;
		HANDLE m_hThread;

	public:
		GLogger2 m_glogger;

	public:
		BOOL Run();
		BOOL Stop();
		BOOL Suspend();
		BOOL Continue();

		virtual void ThreadMain() = 0; // 用户事务处理函数

		static tstring THREAD_STATE_NAME(THREAD_STATE state);
		static tstring THREAD_STATE_IMG(THREAD_STATE state);

	private:
		static DWORD WINAPI MainLoop(LPVOID para);
	};

}