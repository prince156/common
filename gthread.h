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
		START,		// ��ʼִ��		
		STOP,		// ֹͣ
		SUSPEND,	// ��ͣ
		CONTINUE,	// ����ִ��
	};

	// �̻߳�ģ�������״̬����,��������
	enum class THREAD_STATE
	{
		NONE,				// ����
		CREATE,				// ����
		INIT,				// ��ʼ��
		RUNNING,			// ����ִ��
		SUSPENDING,			// ����
		SUSPEND_WARNING,	// ��澯����
		SUSPEND_ERROR,		// ��������
		STOPPED,			// ��ֹͣ
		SLEEPING			// ˯����
	};

#pragma pack(push,1)

	// �̲߳����ṹ��
	typedef struct _thread_para
	{
		tstring threadName;		// �߳�����
		uint32_t debugLevel;	// ������Ϣ��ʾ����
		PRINT_TARGET printTarget;	// �߳���Ϣ��ӡ���ѡ��
		tstring logfile;			// ��־�ļ�ȫ·��
		bool enableColor;		// �����ɫ���

		THREAD_CONTROL control; // �ɹ��캯�����ã����߳̽��п���
		THREAD_STATE state;		// �ɹ��캯�����ã��߳�״̬
		void* current;			// �ɹ��캯�����ã���ǰ�߳�������
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

		virtual void ThreadMain() = 0; // �û���������

		static tstring THREAD_STATE_NAME(THREAD_STATE state);
		static tstring THREAD_STATE_IMG(THREAD_STATE state);

	private:
		static DWORD WINAPI MainLoop(LPVOID para);
	};

}