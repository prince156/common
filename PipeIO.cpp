// pipeio.cpp
// �����ܵ��࣬���𴴽������ܵ������ܹ��ӹܵ��ж�ȡ����	
// 2013-11-28,littledj: CPipeIO�಻��������pipeInterface

#include "windows.h"
#include "PipeIO.h"
#include "common.h"

CPipeIO::CPipeIO()
{
	m_PipeName.clear();
	m_hPipe = INVALID_HANDLE_VALUE;
	m_bInit = false;
}

/********************************************************************/
/* ���캯��
/* ��ʼ����Ա����
/* �޸ļ�¼: 
/*   2013-11-17,littledj: create 
/*   2013-11-28,littledj: ����һ������pipeName
/********************************************************************/
CPipeIO::CPipeIO(const tstring& pipeName)
{
	m_PipeName = pipeName;
	m_hPipe = INVALID_HANDLE_VALUE;
	m_bInit = false;
}

/********************************************************************/
/* ��������
/********************************************************************/
CPipeIO::~CPipeIO()
{
	if (m_hPipe)
	{
		CloseHandle(m_hPipe);
		m_hPipe = NULL;
	}
		
}

/********************************************************************/
/* ������: CreatePipe
/* ����: ���������ܵ�����дmessage���ɷ���˳�ʼ�����ã�
/* ����: 
/*   pipeName: �ܵ�����
/* ���: 
/*   true: �����ɹ�
/*   false: ����ʧ��
/* �޸ļ�¼: 
/*   2013-12-03,littledj: create 
/********************************************************************/
bool CPipeIO::CreatePipe(const tstring& pipeName)
{
	// �������
	if(pipeName.empty() || 
		pipeName.length() > MAX_PIPENAME_LEN)
		return false;
	m_PipeName = pipeName;

	// ��������ܵ�
	SECURITY_ATTRIBUTES secAttrib;
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor( &sd, SECURITY_DESCRIPTOR_REVISION );
	SetSecurityDescriptorDacl( &sd, true, NULL, false );
	secAttrib.bInheritHandle= true;
	secAttrib.nLength= sizeof(SECURITY_ATTRIBUTES);
	secAttrib.lpSecurityDescriptor= &sd;	
	m_hPipe = CreateNamedPipe( m_PipeName.data(), 
		PIPE_ACCESS_DUPLEX,
		PIPE_READMODE_MESSAGE|PIPE_TYPE_MESSAGE|PIPE_WAIT,
		1,
		0, 
		0,
		5000,
		&secAttrib);
	if( INVALID_HANDLE_VALUE == m_hPipe )
		return false;

	m_bInit = true;
	return true;
}

/********************************************************************/
/* ������: OpenPipe
/* ����: �������ܵ�����дmessage���ɿͻ��˵��ã�
/* ����: 
/*   pipeName: �ܵ�����
/* ���: 
/*   true: �򿪳ɹ�
/*   false: ��ʧ��
/* �޸ļ�¼: 
/*   2013-12-03,littledj: create 
/********************************************************************/
bool CPipeIO::OpenPipe(const tstring& pipeName)
{
	// �������
	if (pipeName.empty() ||
		pipeName.length() > MAX_PIPENAME_LEN)
		return false;
	m_PipeName = pipeName;

	// �������ܵ�		
	m_hPipe = CreateFile( m_PipeName.data(),
		GENERIC_READ|GENERIC_WRITE,0,NULL,
		OPEN_EXISTING,0,NULL);
	if(m_hPipe == INVALID_HANDLE_VALUE)
		return false;

	m_bInit = true;
	return true;
}

/********************************************************************/
/* ������: ReadPipe
/* ����: ��ȡ�ܵ���Ϣ
/* ����: 
/*   buff: �����ߴ������Ļ���������Ŷ�ȡ����Ϣ
/*   len: �������Ĵ�С���ֽڣ�
/*   nRead: ��ȡ��Ϣ�Ĵ�С���ֽڣ�
/*   dwTimeout: ��ʱʱ��
/* ���: 
/*   true: ��ȡ�ɹ�
/*   false: ��ȡʧ��
/* �޸ļ�¼: 
/*   2013-11-17,littledj: create 
/*   2013-12-01,littledj: ���Ӳ������
/*   2013-12-04,littledj: ɾ��ConnectNamedPipe��DisconnectNamedPipe��ز���
/********************************************************************/
bool CPipeIO::ReadPipe(char* buff,uint32_t len,uint32_t& nRead,uint32_t dwTimeout)
{	
	// �������
	if (!m_bInit)
		return false;
	if(m_hPipe == NULL || buff == NULL || len == 0)
		return false;

	uint32_t t=0;
	while( !ReadFile( m_hPipe, buff, len, (LPDWORD)&nRead, NULL ) )
	{
		Sleep(IDLE_TIME);
		t += IDLE_TIME;
		if (t>dwTimeout)	// ��ȡ�ܵ���Ϣ��ʱ
		{
			return false;
		}
	}
	return true;
}

/********************************************************************/
/* ������: WritePipe
/* ����: ��ܵ�д����Ϣ
/* ����: 
/*   buff: ����ָ�룬���Ҫд�����Ϣ
/*   len: ���ݵĳ��ȣ��ֽڣ�
/*   nWrite: д�볤�ȣ��ֽڣ�
/*   dwTimeout: ��ʱʱ��
/* ���: 
/*   true: д��ɹ�
/*   false: д��ʧ��
/* �޸ļ�¼: 
/*   2013-11-28,littledj: create 
/********************************************************************/
bool CPipeIO::WritePipe( char* data, uint32_t len, uint32_t& nWrite, uint32_t dwTimeOut )
{
	// �������
	if (!m_bInit)
		return false;
	if(m_hPipe == NULL || data == NULL || len == 0)
		return false;

	uint32_t t=0;
	while (!WriteFile(m_hPipe, data, len, (LPDWORD)&nWrite, NULL))
	{
		Sleep(IDLE_TIME);
		t += IDLE_TIME;
		if (t>dwTimeOut)	// ��ȡ�ܵ���Ϣ��ʱ
		{
			return false;
		}
	}	
	return true;
}

/********************************************************************/
/* ������: Close
/* ����: �رչܵ����
/* �޸ļ�¼: 
/*   2013-11-17,littledj: create 
/********************************************************************/
void CPipeIO::Close()
{
	if (m_bInit)
	{
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
}

/********************************************************************/
/* ������: Disconnect
/* ����: �����ʹ�ã��Ͽ��ͻ���
/* �޸ļ�¼: 
/*   2013-12-04,littledj: create 
/********************************************************************/
void CPipeIO::Disconnect()
{
	if (m_bInit)
	{
		DisconnectNamedPipe(m_hPipe);
	}
}

/********************************************************************/
/* ������: Connect
/* ����: �����ʹ�ã��ȴ��ͻ�������
/* �޸ļ�¼: 
/*   2013-12-04,littledj: create 
/********************************************************************/
void CPipeIO::Connect()
{
	if (m_bInit)
	{
		ConnectNamedPipe(m_hPipe,NULL);
	}
}

/********************************************************************/
/* ������: Wait
/* ����: �ͻ���ʹ�ã��ȴ����ӹܵ�
/* �޸ļ�¼: 
/*   2013-12-04,littledj: create 
/********************************************************************/
void CPipeIO::Wait(uint32_t dwTimeout)
{
	if (m_bInit)
	{		
		WaitNamedPipe(m_PipeName.data(),dwTimeout);
	}
}

