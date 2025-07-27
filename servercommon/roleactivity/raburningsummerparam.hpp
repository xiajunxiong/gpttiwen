#ifndef __RA_BURNING_SUMMER_PARAM_HPP__
#define __RA_BURNING_SUMMER_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_SUMMER_LI_LIAN_BOSS_NUM = 31;				//��������BOSS���� (һ�����һ��)
static const int MAX_RA_SUMMER_LI_LIAN_MONSTER_NUM = 5;				//�����������������������
static const int MAX_RA_COLORFUL_LANTERN_TASK_NUM = 32;				//��ʻ�����������
static const int MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM = 10;		//��ʻ�������
static const int MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM = 100;			//�����̵��ڳ�����Ʒ�������ֵ
static const int MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM = 30;		//�����̵�׶��ڳ�����Ʒ�������ֵ


//��������
UNSTD_STATIC_CHECK(MAX_RA_SUMMER_LI_LIAN_BOSS_NUM <= (int)sizeof(int) * 8);
struct RASummerLiLianParam
{
	RASummerLiLianParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		open_day = 0;
		is_fetch_all_pass_reward_flag = 0;
		reserve_ch = 0;
		m_first_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	short open_day;												//���������
	char is_fetch_all_pass_reward_flag;							//�Ƿ���ȡȫͨ������ʶ 0:δ��ȡ 1:����ȡ
	char reserve_ch;
	BitMap<MAX_RA_SUMMER_LI_LIAN_BOSS_NUM> m_first_flag;		//�Ƿ���ɱ��ʶ  [1,31]
};
UNSTD_STATIC_CHECK(sizeof(RASummerLiLianParam) <= sizeof(RoleActivityRoleData));


//��ʻ���
struct RAColorfulLanternParam
{
	RAColorfulLanternParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		last_start_day_id = 0;
		game_end_timestamp = 0;
		single_score_count = 0;
		last_game_score_count = 0;
		day_single_score_count = 0;
		activity_single_score_count = 0;
		day_join_times = 0;
		activity_join_times = 0;
		day_score_count = 0;
		reserve_sh = 0;
		activity_score_count = 0;
		m_task_list_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned int last_start_day_id;									//��ʼ��Ϸʱ������ID
	unsigned int game_end_timestamp;								//��Ϸ����ʱ���
	short single_score_count;										//������Ϸ����
	short last_game_score_count;									//��һ����Ϸ��õĻ���
	short day_single_score_count;									//���쵥�λ������ֵ
	short activity_single_score_count;								//��ڵ��λ������ֵ
	short day_join_times;											//���ղ������
	short activity_join_times;										//��ڲ������
	short day_score_count;											//���ջ�õ��ܷ�
	short reserve_sh;
	int activity_score_count;										//��ڻ�õ��ܻ���
	BitMap<MAX_RA_COLORFUL_LANTERN_TASK_NUM> m_task_list_flag;		//�����б��ʶ 0:δ��� 1:�����
};
UNSTD_STATIC_CHECK(sizeof(RAColorfulLanternParam) <= sizeof(RoleActivityRoleData));

//�����̵�
struct RASummerStoreParam
{
	RASummerStoreParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_open_today_flag = 0;
		reserve_sh = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	unsigned int ra_begin_timestamp;
	short is_open_today_flag;													//�����Ƿ�򿪹����� 0:û�� 1���򿪹�
	short reserve_sh;															
	short today_buy_times[MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM];				//ÿ���޹�
	short activity_buy_times[MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM];				//��޹�
};
UNSTD_STATIC_CHECK(sizeof(RASummerStoreParam) <= sizeof(RoleActivityRoleData));

//���˻�ǩ
struct RALuckySignParam
{
	RALuckySignParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_base_show_flag = 0;
		is_fetch_reward_flag = 0;
		sign_id = 0;
		reward_type_seq = 0;
	}
	void DayReset()
	{
		is_base_show_flag = 0;
		is_fetch_reward_flag = 0;
		sign_id = 0;
		reward_type_seq = 0;
	}

	unsigned int ra_begin_timestamp;
	char is_base_show_flag;			//0:ǩ�Ľ��� 1:��ǩ����
	char is_fetch_reward_flag;		//0:δ��ȡ	1: ����ȡ
	short sign_id;
	int reward_type_seq;
};
UNSTD_STATIC_CHECK(sizeof(RALuckySignParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif