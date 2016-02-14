// GThread: 提供一个通用的线程父类
// 2015-07-20,littledj: create

#include "gthread.h"
#include "common.h"

using namespace gcommon;

GThread::GThread()
{
	m_state = THREAD_STATE::UNKNOWN;
	m_control = THREAD_CONTROL::UNKNOWN;
}

GThread::GThread(PTHREAD_PARA para)
{
	SetPara(para);
}

GThread::~GThread()
{
	if (m_state != THREAD_STATE::STOPPED)
	{
		Stop();
	}
}

void GThread::SetPara(PTHREAD_PARA para)
{
	
	m_para = para;
	if (m_para == NULL)
	{
		m_para = new THREAD_PARA(); // 从不释放
		m_para->threadName = TEXT("thread");
		m_para->debugLevel = 0;
		m_para->printTarget = PRINT_TARGET::BOTH;
		m_para->logfile = TEXT("thread.log");
	}
	else if (m_para->threadName.empty())
	{
		m_para->threadName = TEXT("");
	}
	m_state = THREAD_STATE::UNKNOWN;
	m_control = THREAD_CONTROL::UNKNOWN;
	m_para->current = this;

	m_hThread = NULL;
	m_glogger.setDebugLevel(m_para->debugLevel);
	m_glogger.setHeader(m_para->threadName);
	m_glogger.setLogFile(m_para->logfile);
	m_glogger.setTarget(m_para->printTarget);
	m_glogger.enableColor(m_para->enableColor);
}

BOOL GThread::Run()
{
	// 开始新线程前，必须为停止状态
	if (m_hThread != NULL)
	{
		m_glogger.warning(TEXT("thread already create, stop it first"));
		if (!Stop())
			return FALSE;
	}

	// 创建线程
	m_glogger.debug1(TEXT("create thread ..."));
	DWORD threadID;
	m_control = THREAD_CONTROL::START;
	m_hThread = CreateThread(NULL, 0, MainLoop, m_para, 0, &threadID);
	if (m_hThread == NULL)
	{
		m_glogger.debug1(TEXT("create thread failed"));
		return FALSE;
	}
	Sleep(IDLE_TIME);
	m_glogger.debug1(TEXT("create success"));

	// 线程正常启动判断过程
	m_glogger.debug1(TEXT("start thread, wait thread's response ..."));
	DWORD nTime = IDLE_TIME;
	while (m_state != THREAD_STATE::RUNNING && 
		m_state != THREAD_STATE::SUSPENDING)
	{
		if (nTime > THREAD_START_TIMEOUT)	// 超时判断
		{
			m_glogger.debug1(TEXT("thread response timeout"));
			Stop();
			return FALSE;
		}

		Sleep(IDLE_TIME);
		nTime += IDLE_TIME;
	}
	m_glogger.debug1(TEXT("start thread success"));
	return TRUE;
}

BOOL GThread::Stop()
{
	if (m_hThread == NULL)
	{
		m_glogger.debug1(TEXT("thread not exit"));
		return FALSE;
	}

	m_glogger.debug1(TEXT("stopping thread, wait thread's response ..."));
	m_control = THREAD_CONTROL::STOP;
	Sleep(IDLE_TIME);
	DWORD nTime = IDLE_TIME;
	while (m_state != THREAD_STATE::STOPPED)
	{
		if (nTime > THREAD_STOP_TIMEOUT)	// 超时判断
		{
			m_glogger.debug1(TEXT("thread response timeout"));
			m_glogger.debug1(TEXT("try to terminate thread ..."));
			if (TerminateThread(m_hThread, 0)) // 强制终止线程
			{
				m_glogger.debug1(TEXT("terminate thread success"));
				m_hThread = NULL;
				return TRUE;
			}
			else
			{
				m_glogger.debug1(TEXT("can not terminate thread"));
				return FALSE;
			}
		}
		Sleep(IDLE_TIME);
		nTime += IDLE_TIME;
	}
	m_glogger.debug1(TEXT("stop thread success"));
	m_hThread = NULL;
	return TRUE;
}

BOOL GThread::Suspend()
{
	if (m_hThread == NULL)
	{
		m_glogger.debug1(TEXT("thread not exit"));
		return FALSE;
	}

	m_glogger.debug1(TEXT("suspend thread, wait thread's response ..."));
	m_control = THREAD_CONTROL::SUSPEND;
	Sleep(IDLE_TIME);
	DWORD nTime = IDLE_TIME;
	while (m_state != THREAD_STATE::SUSPENDING)
	{
		if (nTime > THREAD_STOP_TIMEOUT)	// 超时判断
		{
			m_glogger.debug1(TEXT("thread response timeout"));
			m_control = THREAD_CONTROL::CONTINUE;
			return FALSE;
		}
		Sleep(IDLE_TIME);
		nTime += IDLE_TIME;
	}
	m_glogger.debug1(TEXT("suspend thread success"));
	return TRUE;
}

BOOL GThread::Continue()
{
	if (m_hThread == NULL)
	{
		m_glogger.debug1(TEXT("thread not exit"));
		return FALSE;
	}

	m_glogger.debug1(TEXT("continue thread, wait thread's response ..."));
	m_control = THREAD_CONTROL::CONTINUE;
	Sleep(IDLE_TIME);
	DWORD nTime = IDLE_TIME;
	while (m_state != THREAD_STATE::RUNNING)
	{
		if (nTime > THREAD_STOP_TIMEOUT)	// 超时判断
		{
			m_glogger.debug1(TEXT("thread response timeout"));
			m_control = THREAD_CONTROL::SUSPEND;
			return FALSE;
		}
		Sleep(IDLE_TIME);
		nTime += IDLE_TIME;
	}
	m_glogger.debug1(TEXT("continue thread success"));
	return TRUE;
}

tstring GThread::THREAD_STATE_NAME(THREAD_STATE state)
{
	switch (state)
	{
	case THREAD_STATE::UNKNOWN:
		return TEXT("<null>");
	case THREAD_STATE::CREATE:
		return TEXT("create");
	case THREAD_STATE::INIT:
		return TEXT("initialize");
	case THREAD_STATE::RUNNING:
		return TEXT("running");
	case THREAD_STATE::SUSPENDING:
		return TEXT("suspend");
	//case THREAD_STATE::WARNING:
	//	return TEXT("warning");
	//case THREAD_STATE::SUSPEND_WARNING:
	//	return TEXT("warning");
	//case THREAD_STATE::SUSPEND_ERROR:
	//	return TEXT("error");
	case THREAD_STATE::STOPPED:
		return TEXT("stop");
	case THREAD_STATE::SLEEPING:
		return TEXT("sleeping");
	default:
		return TEXT("unknown");
	}
}

tstring GThread::THREAD_STATE_IMG(THREAD_STATE state)
{
	switch (state)
	{
	case THREAD_STATE::UNKNOWN:
		return TEXT("??");
	case THREAD_STATE::CREATE:
		return TEXT("|>");
	case THREAD_STATE::INIT:
		return TEXT("*>");
	case THREAD_STATE::RUNNING:
		return TEXT(">>");
	case THREAD_STATE::SUSPENDING:
		return TEXT("||");
	//case THREAD_STATE::WARNING:
	//	return TEXT(">!");
	//case THREAD_STATE::SUSPEND_WARNING:
	//	return TEXT(">!");
	//case THREAD_STATE::SUSPEND_ERROR:
	//	return TEXT(">X");
	case THREAD_STATE::STOPPED:
		return TEXT(">|");
	case THREAD_STATE::SLEEPING:
		return TEXT("z~");
	default:
		return TEXT("??");
	}
}

DWORD WINAPI GThread::MainLoop(LPVOID para)
{
	PTHREAD_PARA threadpara = (PTHREAD_PARA)para;
	GThread* threadclass = (GThread*)threadpara->current;
	threadclass->m_glogger.debug1(TEXT("hello"));

	threadclass->m_glogger.debug1(TEXT("initializing ..."));
	threadclass->m_state = THREAD_STATE::INIT;
	threadclass->m_glogger.debug1(TEXT("initializing done"));
	
	threadclass->m_state = THREAD_STATE::RUNNING;
	while (threadclass->m_control != THREAD_CONTROL::STOP)
	{
		// 处理父线程指令
		switch (threadclass->m_control)
		{
		case THREAD_CONTROL::SUSPEND:			
			if (threadclass->m_state != THREAD_STATE::SUSPENDING)
			{		
				threadclass->m_glogger.info(TEXT("switch to suspend"));
				threadclass->m_state = THREAD_STATE::SUSPENDING;
			}
			break;
		case THREAD_CONTROL::CONTINUE:			
			if (threadclass->m_state == THREAD_STATE::SUSPENDING)
			{				
				threadclass->m_glogger.info(TEXT("continue to work"));
				threadclass->m_state = THREAD_STATE::RUNNING;
			}
			break;
		case THREAD_CONTROL::STOP:
			threadclass->m_state = THREAD_STATE::STOPPED;
			break;
		default:
			break;
		}

		if (threadclass->m_state != THREAD_STATE::RUNNING)
		{
			Sleep(IDLE_TIME);
			continue;
		}

		// 处理用户自定义的事务
		threadclass->ThreadMain();
		if (threadclass->m_state == THREAD_STATE::STOPPED)
			break;		
		//Sleep(1);
	}

	//threadclass->m_glogger.info(TEXT("bye"));
	threadclass->m_state = THREAD_STATE::STOPPED;
	return 0;
}