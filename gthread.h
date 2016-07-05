#pragma once
#include <string>
#include <cstdint>
#include "windows.h"
#include "glogger.h"

using namespace std;
namespace gcommon
{
	enum class THREAD_CONTROL
	{
		UNKNOWN,
		START,		// ��ʼִ��		
		STOP,		// ֹͣ
		SUSPEND,	// ��ͣ
		CONTINUE,	// ����ִ��
	};

	// �̻߳�ģ�������״̬����,��������
	enum class THREAD_STATE
	{
		UNKNOWN,			// ����
		CREATE,				// ����
		INIT,				// ��ʼ��
		RUNNING,			// ����ִ��
		SUSPENDING,			// ����
		STOPPED,			// ��ֹͣ
		SLEEPING			// ˯����
	};

#pragma pack(push,1)

	// �̲߳����ṹ��
	typedef struct _thread_para
	{
		tstring threadName;		// �߳�����
		uint32_t debugLevel = 0;	// ������Ϣ��ʾ����
		PRINT_TARGET printTarget = PRINT_TARGET::SCREEN;	// �߳���Ϣ��ӡ���ѡ��
		tstring logfile;			// ��־�ļ�ȫ·��
		bool enableColor = true;	// �����ɫ���

		//THREAD_CONTROL control = THREAD_CONTROL::START; // �ɹ��캯�����ã����߳̽��п���
		//THREAD_STATE state = THREAD_STATE::CREATE;		// �ɹ��캯�����ã��߳�״̬
		void* current = NULL;			// �ɹ��캯�����ã���ǰ�߳�������
	}THREAD_PARA, *PTHREAD_PARA;

#pragma pack(pop)

	class GThread
	{
	public:
		GThread();
		GThread(PTHREAD_PARA para);
		virtual ~GThread();

	private:
		PTHREAD_PARA m_para;
		HANDLE m_hThread = NULL;

	public:
		GLogger m_glogger;
		THREAD_CONTROL m_control = THREAD_CONTROL::UNKNOWN;
		THREAD_STATE m_state = THREAD_STATE::UNKNOWN;

	public:
		virtual void SetPara(PTHREAD_PARA para);

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