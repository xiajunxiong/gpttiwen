#ifndef __MSG_MI_YI_HPP__
#define __MSG_MI_YI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/miyidef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCMiYiInfo					//!< 3830 ��ҽ��ı��Ϣ
	{
	public:
		SCMiYiInfo();
		MessageHeader header;
		
		char pass_info[MI_YI_MAX_LEVEL][MI_YI_MAX_MONSTER_NUM];	//bit 1 �Ƿ��Ѿ���ɱ����bit2 ��������Ƿ��Ѿ�ͨ�ع�
		unsigned short fetch_flag[MI_YI_MAX_LEVEL];				//	ÿ����ɱ����������ȡ��־
	};

	struct SCMiYiPoker			//!< 3831 ��ҽ��ı�ܳ����������·� 
	{
		SCMiYiPoker();
		MessageHeader header;

		int draw_group_id;
		unsigned int timeout;
		MiYiPoker mypoker[SIDE_MAX_ROLE_NUM];
	};

	struct CSMiYiPokerReq				//!< 3832 ��ҽ��ı��������  [���ã���Ϊ 2260]
	{
		CSMiYiPokerReq();
		MessageHeader header;

		int role_uid;
		int index;			//[1,5]
	};

	class SCMiYiPokerInfo			//!< 3833 ��ҽ��ı��Ϣ�·�
	{
	public:
		SCMiYiPokerInfo();
		MessageHeader header;

		int weekly_pass_count;				//����ͨ�صĴ���
		int day_pass_count;
	};

	class SCMiYiSceneInfo			//!< 3834 ��ҽ��ı������Ϣ�·�
	{
	public:
		SCMiYiSceneInfo();
		MessageHeader header;

		unsigned int kill_flag;		//�����й����ɱ��ʶ(������) 0:δ��ɱ  1:�ѻ�ɱ
	};

	class SCMiYiKillPassInfo //!< 3835 ֪ͨ�ͻ��˴˴�ͨ���ǲ�����ͨ��Ϣ
	{
	public:
		SCMiYiKillPassInfo();
		MessageHeader header;

		unsigned char is_first_kill;
		unsigned char is_week_kill;
		unsigned short day_pass_count;
	};


	enum MI_YI_REQ_TYPE
	{
		MI_YI_REQ_TYPE_FETCH_WEEK_KILL_REWARD,		//��ȡ����ɱ����  p1 ����scene_id p2 ���� npc_id
		MI_YI_REQ_TYPE_SAO_DANG,		//ɨ��  p1 ����scene_id 
	};

	struct CSMiYiReq				//!< 3836 ��ҽ��ı����
	{
		CSMiYiReq();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
	};
}

#pragma pack(pop)

#endif

