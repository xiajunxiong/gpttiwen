#ifndef __MSG_ROLE_GUILD_TASK_HPP__
#define __MSG_ROLE_GUILD_TASK_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/roleguildtaskdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum ROLE_GUILD_TASK_REQ_TYPE
	{
		ROLE_GUILD_TASK_REQ_TYPE_ACCEPT,		//���� p1 5401Э��index
		ROLE_GUILD_TASK_REQ_TYPE_GIVE_UP,		//���� p1 5401Э��index		
		ROLE_GUILD_TASK_REQ_TYPE_SOLICIT,		//������� 
		ROLE_GUILD_TASK_REQ_TYPE_GIVE,		//���� p1 role_id
		ROLE_GUILD_TASK_REQ_TYPE_REFRESH,		//ˢ��
		ROLE_GUILD_TASK_REQ_TYPE_BATTLE,		//�������Ĵ�֣����������
		ROLE_GUILD_TASK_REQ_TYPE_SEND_MAIL,		//���������ʼ� p1 �Ƿ�һ�� p2 role_id
		ROLE_GUILD_TASK_REQ_TYPE_FETCH,		//��ȡ���轱��
	};

	class CSRoleGuildTaskReq					//!< 5400
	{
	public:
		CSRoleGuildTaskReq();
		MessageHeader		header;

		int req_type;
		int param1;
		int param2;
	};

	class SCRoleGuildTaskInfo				//!< 5401	
	{
	public:
		SCRoleGuildTaskInfo();
		MessageHeader		header;

		int finish_times;	//��ɴ���
		int task_index;	//��ǰ��������index
		RoleGuildTaskItem task_list[ROLE_GUILD_TASK_MAX_NUM];		//!< 6 �����б�
	};

	enum ROLE_GUILD_TASK_CHANGE_REASON
	{
		ROLE_GUILD_TASK_CHANGE_REASON_START,	//����ʼ
		ROLE_GUILD_TASK_CHANGE_REASON_FINISH,  //�������
		ROLE_GUILD_TASK_CHANGE_REASON_GIVE_UP,	//�������
		ROLE_GUILD_TASK_CHANGE_REASON_SOLICIT,	//����
	};

	class SCRoleGuildTaskChange			//!< 5402	�б�仯A
	{
	public:
		SCRoleGuildTaskChange();
		MessageHeader		header;

		int reason;
		int task_index;
		RoleGuildTaskItem task;		
	};
}

#pragma pack(pop)

#endif 

