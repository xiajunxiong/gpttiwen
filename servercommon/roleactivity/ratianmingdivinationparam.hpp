#ifndef __RA_TIAN_MING_DIVINATION_PARAM_HPP__
#define __RA_TIAN_MING_DIVINATION_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2077 ����� - ��������  //////////////////////////////////////////

static const int RAND_ACTIVITY_TIANMING_DIVINATION_REWARD_MAX_SEQ = 22;						// �������Խ�����󽱳��±�
static const int RAND_ACTIVITY_TIANMING_DIVINATION_TOTAL_MAX_ADD_LOT_TIMES = 54;			// ������������ע����
static const int RAND_ACTIVITY_TIANMING_DIVINATION_EVERY_LOT_MAX_ADD_TIMES = 9;				// ��������ÿ��ǩͲ����ע����
static const int RAND_ACTIVITY_SECTION_MAX_COUNT = 20;

static const int RA_TIANMING_LOT_COUNT = 6;								// �ɼ�ע��ǩ����
static const int RA_TIANMING_REWARD_HISTORY_COUNT = 20;					// ������ʷ��¼����

struct RATianMingRewardHistoryItem
{
	RATianMingRewardHistoryItem() { this->Reset(); }

	void Reset()
	{
		seq = 0;
		add_lot = 0;
		reserve_sh = 0;
	}

	char seq;															// �����±�
	char add_lot;														// �񽱼�ע����
	short reserve_sh;
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RATianMingDivinationParam
{
	RATianMingDivinationParam() { this->Reset(); }
	void Reset()
	{
		ra_tianming_join_timestamp = 0;

		for (int i = 0; i < RA_TIANMING_LOT_COUNT; ++i)
		{
			ra_tianming_add_lot_times_list[i] = 1;
		}
		reserve_ch = 0;
		ra_tianming_reward_history_list_cur_index = 0;
		ra_tianming_free_chou_times = 0;
		for (int i = 0; i < RA_TIANMING_REWARD_HISTORY_COUNT; ++i)
		{
			ra_tianming_reward_history_list[i].Reset();
		}
		ra_tianming_today_chou_tims = 0;
	}

	unsigned int ra_tianming_join_timestamp;									// ��Ҳ���ʱ��
	char ra_tianming_add_lot_times_list[RA_TIANMING_LOT_COUNT];					// ��ǩ��ע�����б�
	char ra_tianming_reward_history_list_cur_index;								// ������ʷ��¼�б�����ɽ�����¼���±�
	char reserve_ch;
	short reserve_sh;
	short ra_tianming_free_chou_times;											// �������Ի - ������ѳ齱����
	RATianMingRewardHistoryItem ra_tianming_reward_history_list[RA_TIANMING_REWARD_HISTORY_COUNT];	// ������ʷ��¼
	unsigned int ra_tianming_today_chou_tims;
};

UNSTD_STATIC_CHECK(sizeof(RATianMingDivinationParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_TIAN_MING_DIVINATION_PARAM_HPP__
