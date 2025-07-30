#ifndef __MSG_CHIEF_PEAK_BATTLE_HPP__
#define __MSG_CHIEF_PEAK_BATTLE_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum CHIEF_PEAK_BATTLE_REQ_TYPE
	{
		CHIEF_PEAK_BATTLE_REQ_TYPE_ROLE_INFO,		//��ɫ��Ϣ
		CHIEF_PEAK_BATTLE_REQ_TYPE_MATCH_INFO,		//������Ϣ
		CHIEF_PEAK_BATTLE_REQ_TYPE_BET,			//��ע p1 seq��p2 figher_index
		CHIEF_PEAK_BATTLE_REQ_TYPE_READY,		//׼��

		CHIEF_PEAK_BATTLE_REQ_GET_NOTICE = 1000,		//��ȡ�غ���Ϣ�� ������Զ���ȥ���ط�
	};

	class CSChiefPeakBattleReq			//!< 2700
	{
	public:
		CSChiefPeakBattleReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCChiefPeakBattleNotice		//!< 		2701
	{
	public:
		SCChiefPeakBattleNotice();
		MessageHeader header;

		int curr_round;				//��ǰ�غ�
		int round_state;				//0�ǻغ�׼��״̬��1�ǿ�ʼ
		unsigned int end_time;		//�غϽ���ʱ��
	};

	class SCChiefPeakBattleMatchInfo		//!< 		2702
	{
	public:
		SCChiefPeakBattleMatchInfo();
		MessageHeader header;

		struct InfoItem
		{
			InfoItem()
			{
				role_id = 0;
				level = 0;
				avatar_type = 0;
				memset(role_name, 0, sizeof(role_name));
				match_round = 0;
				state = 0;
				headshot_id = 0;
			}

			int role_id;					//0˵��������Ч
			int level;
			short avatar_type;
			short headshot_id;
			GameName role_name;

			int match_round;		//����Ļغ�
			int state;				//1�����ˣ�0��	match_round>curr_round Ӯ�� �� match_round==curr_round  δ�ֳ�ʤ��
		};

		int curr_round;				//��ǰ�غ�	
		int round_state;				//0�ǻغ�׼��״̬��1�ǿ�ʼ
		unsigned int end_time;		//�غϽ���ʱ��
		int is_finish;
		InfoItem match_info[ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM];	//��СΪ8
	};

	class SCChiefPeakBattleRoleInfo		//!< 		2703 ��ɫ��Ϣ����
	{
	public:
		SCChiefPeakBattleRoleInfo();
		MessageHeader header;

		int bet_type;			//��ע������
		int role_id;
	};

	class SCChiefPeakBattleInfo		//!< 		2704 
	{
	public:
		SCChiefPeakBattleInfo();
		MessageHeader header;

		int champion_role_id;			//�ϴιھ�
	};

	class SCChiefPeakBattleFighterFinishNotice		//!< 		2705  ֪ͨ�ĸ��˴�����,���˷ֳ�ʤ��
	{
	public:
		SCChiefPeakBattleFighterFinishNotice();
		MessageHeader header;

		int role_id;
		int is_win;
	};

	class SCChiefPeakBattleChampion		//!< 		2706  ˭��Ϊ�ھ�
	{
	public:
		SCChiefPeakBattleChampion();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif