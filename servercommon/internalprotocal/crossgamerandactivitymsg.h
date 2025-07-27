#ifndef __CROSS_GAME_RA_MSG_H__
#define __CROSS_GAME_RA_MSG_H__

#include "servercommon/serverdef.h"
#include "servercommon/rankdef.hpp"
#include "servercommon/crossdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace crossgameramsg
{
	// C - cross, G - gameworld
	enum _MsgType
	{
		MSG_INVALID = 0,

		MSG_COMMON_BEGIN = 1000,											// ����ʹ�õ�ͨѶ��

		MSG_COMMON_ACTIVITY_STATUS_CG,										// ͬ���״̬
		MSG_COMMON_FORCE_TO_NEXT_STATUS_GC,									// ǿ���л�����һ���״̬
		MSG_COMMON_FORCE_SET_ACTIVITY_TIME_GC,								// ǿ�Ƹ��Ļʱ��

		MSG_COMMON_END = 2000,

		MSG_ACT_BEGIN = 2001,												// ��Զ���ͨѶ��

		MSG_CHONGZHI_RANK_SYNC_DATA_GC,										// ��ֵ����-ͬ����������ֵ����
		MSG_CHONGZHI_RANK_CHECK_REWARD_CG,									// ��ֵ����-֪ͨԭ������

		MSG_CONSUME_RANK_SYNC_DATA_GC,										// ��������-ͬ����������������
		MSG_CONSUME_RANK_CHECK_REWARD_CG,									// ��������-֪ͨԭ������

		MSG_SYNC_LUCKY_ROLL_GC,												// ͬ��תת��Ϣ�����
		MSG_SYNC_LUCKY_ROLL_CG,												// ���ͬ��תת��¼��ԭ��

		MSG_ACT_END = 10000
 	};

	struct MessageHeader
	{
		MessageHeader(): msg_type(MSG_INVALID) {}
		MessageHeader(int _msg_type): msg_type(_msg_type) {}
		int	msg_type;
	};

	struct CGSyncActivityStatus
	{
		CGSyncActivityStatus() : header(MSG_COMMON_ACTIVITY_STATUS_CG) {}
		MessageHeader	header;

		short is_status_change;
		short activity_status;
		unsigned int begin_time;
		unsigned int end_time;
		unsigned int cross_cur_time;
	};

	struct GCForceToNextStatus
	{
		GCForceToNextStatus() : header(MSG_COMMON_FORCE_TO_NEXT_STATUS_GC) {}
		MessageHeader	header;
	};

	struct GCForceSetActivityTime
	{
		GCForceSetActivityTime() : header(MSG_COMMON_FORCE_SET_ACTIVITY_TIME_GC) {}
		MessageHeader	header;

		unsigned int begin_time;
		unsigned int end_time;
		unsigned int gameworld_cur_time;
	};


	// ������ÿ����Զ���ͨѶЭ�� =================================================================
	
	struct GCChongzhiRankSyncData
	{
		GCChongzhiRankSyncData() : header(MSG_CHONGZHI_RANK_SYNC_DATA_GC) {}
		MessageHeader	header;

		int plat_type;														// ƽ̨����
		int merge_server_id;												// ������id

		GameName mvp_name;													// �÷���ֵ��ߵ��������
		int mvp_server_id;													// ���ԭ������id
		long long total_chongzhi;											// ȫ���ܳ�ֵ
	};

	struct CGChongzhiRankCheckReward
	{
		CGChongzhiRankCheckReward() : header(MSG_CHONGZHI_RANK_CHECK_REWARD_CG) {}
		MessageHeader	header;

		int rank;															// ��������������
	};

	//����������а�
	struct GCConsumeRankSyncData
	{
		GCConsumeRankSyncData() : header(MSG_CONSUME_RANK_SYNC_DATA_GC) {}
		MessageHeader	header;

		int plat_type;														// ƽ̨����
		int merge_server_id;												// ������id

		GameName mvp_name;													// �÷���ֵ��ߵ��������
		int mvp_server_id;													// ���ԭ������id
		long long total_consume;											// ȫ���ܳ�ֵ
	};

	struct CGConsumeRankCheckReward
	{
		CGConsumeRankCheckReward() : header(MSG_CONSUME_RANK_CHECK_REWARD_CG) {}
		MessageHeader	header;

		int rank;															// ��������������
	};

	struct CALuckyRollUserData
	{
		int plat_type;
		int merge_server_id;
		int origin_server_id;
		int role_id;
		int role_camp;
		GameName role_name;
		int gold_add_pool;            // �����ؼӵ�Ԫ����
		short roll_idx;               // ��õĽ��������
		short reward_type;            // ��õĽ���������
		short gold_reward_percentage; // �������Ԫ�������ٷֱ�
		short is_broadcast;			  // �Ƿ����齱������·��㲥
		long long gold_reward_num;    // ����ʱ��ʾԤ�ƻ�õ�Ԫ������ ����ʱ��ʾʵ�ʻ�õ�Ԫ����
		unsigned int timestamp;		  // ����ʱ��ʾ
	};

	static const int CA_MAX_SYNC_LUCKY_ROLL_RECORD_NUM = 10;

	//ȫ��תתͬ����Ϣ�����
	struct GCLuckyRollSyncData
	{
		GCLuckyRollSyncData() : header(MSG_SYNC_LUCKY_ROLL_GC) {}
		MessageHeader header;

		int can_get_max_gold;
		long long server_total_gold_poll;
		int user_count;
		CALuckyRollUserData user_record_list[CA_MAX_SYNC_LUCKY_ROLL_RECORD_NUM];
	};

	struct CGLuckyRollSyncData
	{
		CGLuckyRollSyncData() : header(MSG_SYNC_LUCKY_ROLL_CG) {}
		MessageHeader header;

		long long all_server_total_gold_poll;
		int user_count;
		CALuckyRollUserData user_record_list[CA_MAX_SYNC_LUCKY_ROLL_RECORD_NUM];
	};
}

#pragma pack(pop)

#endif	// __CROSS_GAME_RA_MSG_H__

