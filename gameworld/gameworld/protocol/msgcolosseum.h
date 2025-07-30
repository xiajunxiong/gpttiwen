#ifndef MSGCOLOSSEUM_H
#define MSGCOLOSSEUM_H

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"
#include "servercommon/colosseumdef.hpp"
#include "servercommon/struct/global/offlineregisterparam.hpp"
#include "servercommon/commondata.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSColosseumReq					//!< ����2600
	{
	public:
		CSColosseumReq();
		MessageHeader	header;

		enum REQ_TYPE
		{
			REQ_TYPE_FIHGT = 0,				//!< ս������	param ��[0,3]
			REQ_TYPE_REFRESH_TARGET = 1,	//!< ˢ�¶���
			REQ_TYPE_ROLE_INFO = 2,			//!< ������Ϣ
			REQ_TYPE_TARGET_INFO = 3,		//!< ������Ϣ
			REQ_TYPE_TOP_INFO = 4,			//!< ǰ4��Ϣ
			REQ_TYPE_RANK_INFO = 5,			//!< ���а���Ϣ  param:rank_count
			REQ_TYPE_REPORT_INFO = 6,		//!< ս����Ϣ
			REQ_TYPE_FETCH_EXP = 7,			//!< ��ȡ����	param��[0,3]
			REQ_TYPE_GET_AWARD = 8,			//!< ��ȡÿ����������
			REQ_TYPE_ACHIEVEMENT_INFO = 9,	//!< �ɾ���Ϣ�·�
			REQ_TYPE_GET_ACHIEVEMENT = 10,	//!< ��ȡ�ɾͽ��� param����Ӧsn
			REQ_TYPE_GET_RANK_TOP_INFO = 11,//!< ��һ����Ϣ�·�
			REQ_TYPE_SWEEP = 12,			//!< ɨ�� param ��[0-3]
		};

		int req_type;
		int param;
	};

	class SCColosseumRoleInfo					//!< ������Ϣ2601
	{
	public:
		SCColosseumRoleInfo();
		MessageHeader header;

		int is_out_of_rank_list;		//!< �Ƿ������������
		int rank;
		int today_fight_count;
		int fetch_exp_flag;
		int player_num_in_pool;
		int can_get_coin;
		int get_colosseum_credits;
		int capability;
		int highest_rank;
	};

	class SCColosseumTopInfo					//!< ǰ4��Ϣ2602
	{
	public:
		SCColosseumTopInfo();
		MessageHeader	header;

		struct Top
		{
			GameName name;
			int profession;
			short avatar_type;
			short level;
			int worship_count;
			int top_level;
		};

		int count;
		Top top_list[CHALLENGE_FIELD_TOP_NUM];
	};

	class SCColosseumTargetInfo					//!< ������Ϣ2603
	{
	public:
		SCColosseumTargetInfo();
		MessageHeader	header;

		struct Target
		{
			int buff_id_list[COLOSSEUM_DEFENDER_BUFF_NUM];
			GameName name;
			short rank;
			short level;
			Appearance appearance;
			int capability;
			int top_level;
			short is_can_sweep;		// �Ƿ��ɨ��
			short reserve_sh;
		};

		int count;
		Target target_list[CHALLENGE_FIELD_OPPONENT_MAX];
	};

	static const int COLOSSEUM_RANK_NUM = 50;
	class SCColosseumRankInfo					//!< ���а���Ϣ2604
	{
	public:
		SCColosseumRankInfo();
		MessageHeader	header;

		struct Top
		{
			GameName name;
			short profession;
			short level;
			int top_level;
			int capability;
			short headshot_id;
			short avatar_type;
		};

		int count;
		Top top_list[COLOSSEUM_RANK_NUM];
	};

	static const int COLOSSEUM_REPORT_NUM = 20;
	class SCColosseumReportInfo					//!< ս����Ϣ2605
	{
	public:
		SCColosseumReportInfo();
		MessageHeader	header;
		
		int count;
		Report report_list[COLOSSEUM_REPORT_NUM];
	};

	class SCColosseumRankRewardInfo
	{
	public:
		SCColosseumRankRewardInfo();		//!< 2606 - ����������ȡ�ſ�
		MessageHeader header;

		int history_highest_rank;			//!< ��ʷ�������
		long long rank_reward_fetch_flag;	//!< ����������ȡ���
	};

	class SCColosseumPvPsResultInfo
	{
	public:
		SCColosseumPvPsResultInfo();		//!< 2607 - ������ʤ������·�
		MessageHeader header;

		int up_rank;			//!< ����������ֵ
		int new_rank;			//!< ������
		int add_score;			//!< ���˶��ٻ���
		int score;
		int is_sweep;			//!< �Ƿ���ɨ��,0--��, 1--��
	};

	class SCColosseumOpenStatusInfo
	{
	public:
		SCColosseumOpenStatusInfo();		//!< 2608 - ����������״̬���֪ͨ
		MessageHeader header;

		int open_status;					//!< ���񱦿� ����״̬��0 - �رգ�1 - ������
		long long next_switch_timestamp;	//!< ���񱦿� �´ν���ʱ���
	};

	class SCColosseumInPoolListInfo					//!< 2609 - ���������а�
	{
	public:
		SCColosseumInPoolListInfo();
		MessageHeader	header;

		struct Top
		{
			GameName name;
			short profession;
			short level;
			int top_level;
		};

		UInt16 reason;		// ����ԭ�� \see PtcSendReason
		UInt16 count;		// �б��ܳ�
		Top top_list[COLOSSEUM_IN_POOL_RANK_LIST_NUM];
	};

	class SCColosseumNewReport		//!< 2610 ��ս��֪ͨ
	{
	public:
		SCColosseumNewReport();
		MessageHeader header;

		Report report;
	};

	class SCColosseumEveryDayRewardInfo		//!< 2611 ��ÿ�ս���֪ͨ
	{
	public:
		SCColosseumEveryDayRewardInfo();
		MessageHeader header;
		int count;
		int rank_pos_list[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST];  //!< ��ҵĽ��㵱ʱ���� OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST = 3
	};

	class SCColosseumNewReportDetail	//!< 2612 ��ս������
	{
	public:
		SCColosseumNewReportDetail();
		MessageHeader header;

		Report report;
	};

	class SCColosseumClientRefreshTargetNotice	//!< 2613 ֪ͨ�ͻ����������¶�����Ϣ
	{
	public:
		SCColosseumClientRefreshTargetNotice();
		MessageHeader header;
	};

	class SCColosseumAchievementInfo	//!< 2614 �������ɾ��·�
	{
	public:
		SCColosseumAchievementInfo();
		MessageHeader header;

		unsigned int next_refresh_tamp;
		BitMap<MAX_COLOSSEUM_TASK_NUM> colosseum_task_flag;
	};

	class SCColosseumDetalInfo	//!< 2616 ��������ϸ��¼�·�
	{
	public:
		SCColosseumDetalInfo();
		MessageHeader header;

		ColosseumParam colosseum_task_flag;
	};

	class SCHistoryHighestRankChangeNotice	//!< 2617 ��������ʷ�����仯֪ͨ
	{
	public:
		SCHistoryHighestRankChangeNotice();
		MessageHeader header;

		int cur_rank;						//!< ��ǰ����
		int change_rank_num;				//!< ��ʷ�����仯��ֵ
	};

	class SCSweepResultNotice	//!< 2618 ������ɨ�����֪ͨ
	{
	public:
		SCSweepResultNotice();
		MessageHeader header;

		int is_sweep_success;	//1-�ɹ�,0-ʧ��
	};

	class SCBattleRewardNotice	//2619 ��������ս������֪ͨ
	{
	public:
		SCBattleRewardNotice();
		MessageHeader header;

		int reward_group;
	};
}

#pragma pack(pop)
#endif