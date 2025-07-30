#ifndef __MSG_WILD_BOSS_HPP__
#define __MSG_WILD_BOSS_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/wildbossdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum WILD_BOSS_NOTICE
	{
		WILD_BOSS_NOTICE_MONSTER_BATTLE,			//��������ս����
		WILD_BOSS_NOTICE_NOT_ENOUGH_MEMBERS,	//������������
	};

	class SCWildBossNotice		//!<2470
	{
	public:
		SCWildBossNotice();
		MessageHeader header;
		
		int notice_type;
	};

	class SCWildBossInfo		//!<2472
	{
	public:
		SCWildBossInfo();
		MessageHeader header;

		int today_times;		//!< (������ս����) �ĳ�ÿ����ս����
		unsigned int next_refresh_time[MAX_WILD_BOSS_NUM];
		int wild_number_list[MAX_WILD_BOSS_NUM];	//boss����б�
	};

	class CSWildBossReq		//!<2473
	{
	public:
		CSWildBossReq();
		MessageHeader header;

		int req_type;		//!< ����,1���·�ˢ��ʱ�������ս���� 2:�����·�����seq��λ��
		int param;			//!< ����������seq
	};

	class SCWildBossPosiInfo		//!<2474
	{
	public:
		SCWildBossPosiInfo();
		MessageHeader header;

		int monster_pos_x;
		int monster_pos_y;
	};
	
	struct SCWildBossPokerInfo			// 2475	Ұ��boss���ƽ�����Ϣ
	{
		SCWildBossPokerInfo();
		MessageHeader header;

		int draw_group_id;
		unsigned int timeout;
		WildbossPoker wbpoker[SIDE_MAX_ROLE_NUM];
	};

	struct CSWildBossPokerReq			// 2476 Ұ��boss���ƽ�������  [���ã���Ϊ 2260]
	{
		CSWildBossPokerReq();
		MessageHeader header;

		int req_reason;		// ����ԭ�� 0 - �����������   1 - ���û�з��ƣ���ϵͳĬ�ϰ�æ����Ĭ�Ϸ���һ��
		int role_uid;
		int index;			//[1,5]
	};

	class SCWildBossSingleInfo			// 2477 Ұ��BOSS������Ϣ�·�
	{
	public:
		SCWildBossSingleInfo();
		MessageHeader header;

		int index;
		unsigned int next_refresh_time;
		int wild_number;
	};

	class SCWildBossOtherInfo		//!< 1147 Ұ������������Ϣ�·�
	{
	public:
		SCWildBossOtherInfo();
		MessageHeader header;

		int reward_times;				//�ѻ�ý�������
	};

}

#pragma pack(pop)

#endif