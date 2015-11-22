// ver 1.0
// modified: 
//   2014-12-17,littledj: ȥ����common����
//   2015-05-23,littledj: �޸��ж�Խ������

#pragma once
//#include <windows.h>
#include "common.h"
#pragma warning(disable:4200)

#define MAX_PACKET_LEN 2000

/********************************************************************
* DNS
********************************************************************/
#define DNS_QTYPE_A			0x0001
#define DNS_QTYPE_SOA		0x0006
#define DNS_QTYPE_SRV		0x0021

#define DNS_FLAG_IS_RESPONSE		0x8000
#define DNS_FLAG_OPCODE_MASK		0x7800
#define DNS_FLAG_IS_AUTHORITY		0x0400
#define DNS_FLAG_IS_TRUNCATED		0x0200
#define DNS_FLAG_IS_RECURSION_D		0x0100
#define DNS_FLAG_IS_RECURSION_A		0x0080
#define DNS_FLAG_RESERVED			0x0040
#define DNS_FLAG_IS_ANSWER_AUTH		0x0020
#define DNS_FLAG_IS_NONAUTH_ACCEPT	0x0010
#define DNS_FLAG_REPLYCODE_MASK		0x000f

#pragma pack(push,1)

typedef struct _dns_head
{
	unsigned short trans_id;
	unsigned short flags;
	unsigned short questions;	// question ����
	unsigned short answer;		// answer ����
	unsigned short authority;	// authority ����
	unsigned short additional;	// additional ����
	unsigned char data[0];
}DNS_HEAD, *PDNS_HEAD;

typedef struct _dns_answer_head
{
	unsigned short name;
	unsigned short type;
	unsigned short _class;
	unsigned long ttl;
	unsigned short datalen;
}DNS_ANSWER_HEAD;

typedef struct _dns_answer_A
{
	unsigned long addr[0];
}DNS_ANSWER_A;

typedef struct _dns_answer_SRV
{
	unsigned short priority;
	unsigned short weight;
	unsigned short port;
	unsigned char target[0];
}DNS_ANSWER_SRV;

typedef struct _iphdr //����IP�ײ� 
{
	unsigned char h_lenver; //4λ�ײ�����+4λIP�汾�� 
	unsigned char tos; //8λ��������TOS 
	unsigned short total_len; //16λ�ܳ��ȣ��ֽڣ� 
	unsigned short ident; //16λ��ʶ 
	unsigned short frag_and_flags; //3λ��־λ 
	unsigned char ttl; //8λ����ʱ�� TTL 
	unsigned char proto; //8λЭ�� (TCP, UDP ������) 
	unsigned short checksum; //16λIP�ײ�У��� 
	unsigned int sourceIP; //32λԴIP��ַ 
	unsigned int destIP; //32λĿ��IP��ַ 
}IP_HEADER;

typedef struct psd_hdr //����TCPα�ײ� 
{
	unsigned long saddr; //Դ��ַ 
	unsigned long daddr; //Ŀ�ĵ�ַ 
	char mbz;
	char ptcl; //Э������ 
	unsigned short tcpl; //TCP���� 
}PSD_HEADER;

typedef struct _tcphdr //����TCP�ײ� 
{
	unsigned short th_sport; //16λԴ�˿� 
	unsigned short th_dport; //16λĿ�Ķ˿� 
	unsigned int th_seq; //32λ���к� 
	unsigned int th_ack; //32λȷ�Ϻ� 
	unsigned char th_lenres; //4λ�ײ�����/6λ������ 
	unsigned char th_flag; //6λ��־λ 
	unsigned short th_win; //16λ���ڴ�С 
	unsigned short th_sum; //16λУ��� 
	unsigned short th_urp; //16λ��������ƫ���� 
}TCP_HEADER;

#pragma pack(pop)

unsigned short PKT_CheckSum(unsigned short *buf, int bufSize)
{
	unsigned long cksum = 0;
	while (bufSize > 1)
	{
		cksum += *buf++;
		bufSize -= sizeof(unsigned short);
	}
	if (bufSize)
	{
		cksum += *(unsigned char*)buf;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (unsigned short)(~cksum);
}

/********************************************************************
* [������]: PKT_AddByte
* [����]: �����绺�������ָ���������ֽ�
* [����]:
*   buf: ���绺����ָ��
*   bufSize: ��������С
*   pos: ���λ��
*   data: ����ӵ��ֽ�
*   count: �ֽ�����
* [���]:
*   pos: ������ݺ��β��λ��
* [����ֵ]: int
*   1���ɹ�
*   0: δ����κ�����
*   -1��ʧ��
* [�޸ļ�¼]:
*   2014-12-17,littledj: create
********************************************************************/
int static PKT_AddByte(char* buf, int bufSize, int &pos, unsigned char data, int count = 1)
{
	if (buf == NULL || pos + count >= bufSize)
	{
		return -1;
	}

	memset(buf + pos, data, count);
	pos += count;
	return count;
}

int static PKT_SetWORD(char* buf, int bufSize, int pos, unsigned short data)
{
	int data_size = sizeof(short);
	if (buf == NULL || pos + data_size >= bufSize)
	{
		return -1;
	}

	unsigned short data_net = gcommon::htons(data);
	memcpy(buf + pos, (char*)&data_net, data_size);
	return data_size;
}

/********************************************************************
* [������]: PKT_AddWORD
* [����]: �����绺�������WORD����
* [����]:
*   buf: ���绺����ָ��
*   bufSize: ��������С
*   pos: ���λ��
*   data: ����ӵ�����
* [���]:
*   pos: ������ݺ��β��λ��
* [����ֵ]: int
*   >0���ɹ�,��ӵ��ֽ���
*   0: δ����κ�����
*   -1��ʧ��
* [�޸ļ�¼]:
*   2014-10-21,littledj: create
********************************************************************/
int static PKT_AddWORD(char* buf, int bufSize, int &pos, unsigned short data)
{
	int data_size = sizeof(short);
	PKT_SetWORD(buf, bufSize, pos, data);
	pos += data_size;
	return data_size;
}

/********************************************************************
* [������]: PKT_AddDWORD
* [����]: �����绺�������DWORD����
* [����]:
*   buf: ���绺����ָ��
*   bufSize: ��������С
*   pos: ���λ��
*   data: ����ӵ�����
* [���]:
*   pos: ������ݺ��β��λ��
* [����ֵ]: int
*   >0���ɹ�,��ӵ��ֽ���
*   0: δ����κ�����
*   -1��ʧ��
* [�޸ļ�¼]:
*   2014-10-21,littledj: create
********************************************************************/
int static PKT_AddDWORD(char* buf, int bufSize, int &pos, unsigned long data)
{
	int data_size = sizeof(long);
	if (buf == NULL || pos + data_size >= bufSize)
	{
		return -1;
	}

	unsigned long data_net = gcommon::htonl(data);
	memcpy(buf + pos, (char*)&data_net, data_size);
	pos += data_size;
	return data_size;
}

/********************************************************************
* [������]: PKT_AddBytes
* [����]: �����绺��������ַ���
* [����]:
*   buf: ���绺����ָ��
*   bufSize: ��������С
*   pos: ���λ��
*   data: ����ӵ��ַ���
*   data_size: �ַ�������
* [���]:
*   pos: ������ݺ��β��λ��
* [����ֵ]: int
*   >0���ɹ�,��ӵ��ֽ���
*   0: δ����κ�����
*   -1��ʧ��
* [�޸ļ�¼]:
*   2014-12-17,littledj: create
********************************************************************/
int static PKT_AddBytes(char* buf, int bufSize, int &pos, char* data, int data_size)
{
	if (buf == NULL || pos + data_size >= bufSize || data == NULL)
	{
		return -1;
	}

	memcpy(buf + pos, data, data_size);
	pos += data_size;
	return data_size;
}

/********************************************************************
* [������]: PKT_AddNop
* [����]: �����绺������ӡ�\0�����
* [����]:
*   buf: ���绺����ָ��
*   bufSize: ��������С
*   pos: ���λ��
*   count: ��ӡ�\0��������
* [���]:
*   pos: ������ݺ��β��λ��
* [����ֵ]: int
*   1���ɹ�,��ӵ��ֽ���
*   0: δ����κ�����
*   -1��ʧ��
* [�޸ļ�¼]:
*   2014-12-17,littledj: create
********************************************************************/
int static PKT_AddNop(char* buf, int bufSize, int &pos, int count = 1)
{
	if (buf == NULL || pos + count >= bufSize)
	{
		return -1;
	}

	memset(buf + pos, 0, count);
	pos += count;
	return count;
}