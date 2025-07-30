#ifndef __MSG_CHIEF_ELECTION_HPP__
#define __MSG_CHIEF_ELECTION_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum CHIEF_ELECTION_REQ_TYPE
	{
		CHIEF_ELECTION_REQ_TYPE_ROLE_INFO,		//��ɫ��Ϣ	��û�ã�
		CHIEF_ELECTION_REQ_TYPE_MATCH_INFO,		//������Ϣ
		CHIEF_ELECTION_REQ_TYPE_JOIN,		//���� ��û�ã�
		CHIEF_ELECTION_REQ_TYPE_READY,		//׼��
		CHIEF_ELECTION_REQ_JOIN_TOP_INFO,		//������ǰ������Ϣ ��û�ã�
		CHIEF_ELECTION_REQ_TYPE_GOTO_SEA,		//�����ս��ѡ����
		CHIEF_ELECTION_REQ_TYPE_OP_LOG,		//�ͻ���log�������Ų�����
		
		CHIEF_ELECTION_REQ_GET_INFO = 1000,		//��ȡ�غ���Ϣ�� ������Զ���ȥ���ط� (�������)
	};

	class CSChiefElectionReq			//!< 2650
	{
	public:
		CSChiefElectionReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCChiefElectionNotice		//!< 		2651
	{
	public:
		SCChiefElectionNotice();
		MessageHeader header;

		int curr_round;				//��ǰ�غ�
		int round_state;				//0�ǻغ�׼��״̬��1�ǿ�ʼ
		unsigned int end_time;		//�غϽ���ʱ��
	};

	class SCChiefElectionMatchInfo		//!< 		2652
	{
	public:
		SCChiefElectionMatchInfo();
		MessageHeader header;

		struct InfoItem
		{
			InfoItem()
			{
				prof = 0;
				role_id = 0;
				level = 0;
				avatar_type = 0;
				headshot_id = 0;
				memset(role_name, 0, sizeof(role_name));
				match_round = 0;
				state = 0;
			}

			int prof;
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
		int is_finish;				//�Ƿ����
		int  match_info_count;
		InfoItem match_info[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM];	
	};

	class SCChiefElectionRoleInfo		//!< 		2653
	{
	public:
		SCChiefElectionRoleInfo();
		MessageHeader header;

		int is_join;			//�Ƿ�����
	};

	class SCChiefElectionWinRoleInfo		//!< 		2654  ʤ���˵�roleid�����Բμ��۷����
	{
	public:
		SCChiefElectionWinRoleInfo();
		MessageHeader header;
		
		ChiefElectionWinnerInfo winner_info[ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM];
	};

	class SCChiefElectionJoinTopRoleInfo	//!< 		2655
	{
	public:
		SCChiefElectionJoinTopRoleInfo();
		MessageHeader header;

		struct InfoItem
		{
			InfoItem()
			{
				role_id = 0;
				capability = 0;
				avatar_type = 0;
				level = 0;
				headshot_id = 0;
				memset(role_name, 0, sizeof(role_name));
			}

			int role_id;					//0˵��������Ч
			int capability;	
			short avatar_type;				
			short headshot_id;				//ͷ��ID
			int level;
			GameName role_name;
		};

		InfoItem match_info[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM];	//��СΪ
	};

	class SCChiefElectionFighterFinishNotice		//!< 		2656  ֪ͨ�ĸ��˴�����,���˷ֳ�ʤ��
	{
	public:
		SCChiefElectionFighterFinishNotice();
		MessageHeader header;

		int role_id;								
		int is_win;
	};

	class SCChiefElectionChampion		//!< 		2657  ˭��Ϊ�ھ�
	{
	public:
		SCChiefElectionChampion();
		MessageHeader header;
	};

	class SCChiefElectionSeaRoleInfo	//!< 		2658 ��ս��ɫ��Ϣ
	{
	public:
		SCChiefElectionSeaRoleInfo();
		MessageHeader header;

		int socre;
		int win_times;
		int lose_times;
		int total_times;
	};

	class SCChiefElectionSeaSceneInfo	//!< 		2659 ��ս������Ϣ
	{
	public:
		SCChiefElectionSeaSceneInfo();
		MessageHeader header;

		int role_num;					//��ǰ��������
	};

	class SCChiefElectionStateInfo	//!< 		2660 ��ϯ�״̬����¼���߸ı��˷�
	{
	public:
		SCChiefElectionStateInfo();
		MessageHeader header;
			
		int which_prof_match;
		int match_state;				//�״̬���յ� ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FINISH ����ս����
		unsigned int next_match_state_time;		//�л��״̬ʱ���
	};

	class SCChiefElectionSeaRank	//!< 		2661 ��ս���а�
	{
	public:
		SCChiefElectionSeaRank();			//ˢ�º��Զ��·�
		MessageHeader header;

		struct Item
		{
			int role_id;
			GameName role_name;
			int socre;
			int win_times;
			int lose_times;
			int total_times;
		};

		int count;
		Item rank_list[100];
	};
}

#pragma pack(pop)

#endif