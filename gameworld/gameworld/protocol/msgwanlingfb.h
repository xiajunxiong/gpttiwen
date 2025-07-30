#ifndef __MSG_WAN_LING_FN_H__
#define __MSG_WAN_LING_FN_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#include "servercommon/wanlingfbdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{//���黰��
	//�û�����
	class CSWanLingFBReq
	{
	public:
		CSWanLingFBReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_TASK_FINISH = 0,	//FB-�������(�ͻ���α����)
			REQ_TYPE_FB_FETCH_SET,//�����콱��� p_1:����_index p_2:0���� 1������

			REQ_TYPE_TASK_FIGHT,		//FB-���뵱ǰ�����ս��
		};

		int req_type;		//��������
		int param_1;
		int param_2;
	};

	//��ɫ��Ϣ
	class SCWanLingFBRoleInfo
	{
	public:
		SCWanLingFBRoleInfo();
		MessageHeader		header;

		int day_fetch_battle_reward;	//������ȡս����������
		int day_fetch_pass_reward;		//������ȡͨ�ؽ�������
		int fb_fetch_flag;				//�����콱���[����_index]
	};

	//������Ϣ
	class SCWanLingFBInfo
	{
	public:
		SCWanLingFBInfo();
		MessageHeader		header;

		int m_fb_index;		//����index
		int m_is_finish;	//������ɱ��

		int m_task_seq;		//��ǰ����
	};
};

#pragma pack(pop)

#endif