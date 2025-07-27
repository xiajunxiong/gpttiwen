#ifndef __MSGSYSTEM_H__
#define __MSGSYSTEM_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"
#include "servercommon/userprotocal/systemmsgcode.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	/*
		ϵͳ���
	*/

	#define PROTOCAL_MAX_SYSTEM_MSG_LENGTH 2048
	#define SPID_MSG_LENGTH 64
	class SCSystemMsg				//!< 701 ϵͳ��Ϣ
	{
	public:
		SCSystemMsg() : header(MT_SYSTEM_MSG_SC) {}
		MessageHeader header;

		unsigned int send_time;
		short reserve_sh;
		short min_level;				// ����0ʱ����û������	
		short max_level;				// ����0ʱ����û������
		short msg_length;
		int	display_pos;
		int color;
		int msg_type;	
		char spid_id[SPID_MSG_LENGTH];
		char msg[PROTOCAL_MAX_SYSTEM_MSG_LENGTH];
	};

	class SCNoticeNum
	{
	public:
		SCNoticeNum() : header(MT_SYSTEM_NOTICE_CODE_SC) {}
		MessageHeader header;

		int notice_num;
	};

	class SCZeroHour
	{
	public:
		SCZeroHour();
		MessageHeader header;
	};

	class CSHeartbeatReq //!< 1053 ��������
	{
	public:
		CSHeartbeatReq();
		MessageHeader header;
	};

	class SCHeartbeatResp //!< 1003 ��������
	{
	public:
		SCHeartbeatResp();
		MessageHeader header;
	};

	class SCClientHeartbeatCheckModify	// 704 ���ƿͻ��˵����������Զ������߼�����
	{
	public:
		SCClientHeartbeatCheckModify();
		MessageHeader header;

		int is_open;		// �Ƿ�����������Զ����ߵ��߼�   0:�ر�  1:����  

		//!< \note  ��ʼ�ȸ�ǰ���·�����Э�飬�ͻ����յ�����ʱ�رա���⵽û������ʱ��Ͽ����ӡ����߼���
		//!<        ����Ҫ�ر������߼��������ճ����ͺ��ˡ���һ���Ӻ���ٴ��·�����Э�飬���¿���
		//!<        ����⵽û������ʱ��Ͽ����ӡ����߼�
	};

	class SCForbidReason //!< 705 ������ɷ���
	{
	public:
		SCForbidReason();
		MessageHeader header;

		int forbid_time_log;//��ų���ʱ��
	};
}

#pragma pack(pop)

#endif

