#ifndef __KUA_FU_YING_XIONG_LU_DEF_HPP__
#define __KUA_FU_YING_XIONG_LU_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int MAX_KUA_FU_YING_XIONG_LU_RANK_NUM = 10;
static const int MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM = 10;	//��������ÿ���񵥵������������
static const int MAX_KUA_FU_YING_XIONG_LU_RANK_CHECK_INTERVAL_TIME = 15;	//�񵥼����ʱ��

enum KUA_FU_YING_XIONG_LU_RANK_TYPE
{
	KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN = 0,

	KUA_FU_YING_XIONG_LU_RANK_TYPE_ROLE_CAP = KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN,			// ��ɫ����
	KUA_FU_YING_XIONG_LU_RANK_TYPE_EQUIP_CAP = 1,			// װ������
	KUA_FU_YING_XIONG_LU_RANK_TYPE_PARTNER_CAP = 2,			// �������
	KUA_FU_YING_XIONG_LU_RANK_TYPE_PET_CAP = 3,				// ��������
	KUA_FU_YING_XIONG_LU_RANK_TYPE_COMPLEX_CAP = 4,			// �ۺ�����

	KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_KUA_FU_YING_XIONG_LU_RANK_NUM >= KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX);

enum KUA_FU_YING_XIONG_LU_TASK_TYPE
{
	KUA_FU_YING_XIONG_LU_TASK_TYPE_INVALID = 0,				
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_MEDAL = 1,			// ��������		param1:����ǿ���ȼ�
	KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE = 2,			// ��Ƕ��ʯ		param1:��ʯ�ȼ�
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_EQUIP = 3,			// ����װ��		param1:����Ʒ��(0:������)\see I_COLOR_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_PET_EQUIP = 4,		// �������ﻤ��	param1:����Ʒ��(0:������)
	KUA_FU_YING_XIONG_LU_TASK_TYPE_XUN_BAO = 5,				// Ѱ��			param1:��������(0:������)\see XUN_BAO_TREASURE_TYPE_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_HAS_PET = 6,				// ӵ�г���		param1:����Ʒ��(0:������)\see PET_QUALITY_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_SOUL_EQUIP = 7,		// ��������		param1:����Ʒ��(0:������)
	KUA_FU_YING_XIONG_LU_TASK_TYPE_LEARN_PET_SKILL = 8,		// ѧϰ���＼��	param1:������Ʒ��(-1:������)\see GAME_PASSIVE_SKILL_PRIORITY_TYPE_MAX
	KUA_FU_YING_XIONG_LU_TASK_TYPE_PARTNER_STR = 9,			// ���ǿ��		param1:���Ʒ��(0:������)\see PQT_MAX

	KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX
};

struct KuaFuYingXiongLuRankItem
{
	KuaFuYingXiongLuRankItem() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		reserve_sh = 0;
		reserve_ch = 0;
		is_give_rand_activity_reward_flag = 0;
		rank_end_timestamp = 0;
	}
	bool IsInvalid() const
	{
		return rank_end_timestamp <= 0;
	}

	long long reserve_ll[2];

	short reserve_sh;
	char reserve_ch;
	char is_give_rand_activity_reward_flag;			// ���������ͻ�����������㷢�ű�ʶ 0:δ���� 1:�ѷ���
	unsigned int rank_end_timestamp;
	unsigned int next_check_timestamp;				// �´μ���ʱ���
};

struct KuaFuYingXiongLuActivityParam
{
	KuaFuYingXiongLuActivityParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		is_has_ready_open = 0;
		is_lock_flag = 0;
		reserve_sh = 0;
		activity_open_cross_day = 0;
		activity_begin_timestamp = 0;
		activty_end_timestamp = 0;
		activity_unique_id = 0;
		for (int i = 0; i < ARRAY_LENGTH(rank_list); i++)
		{
			rank_list[i].Reset();
		}
	}
	void ReopenReset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		is_has_ready_open = 0;
		reserve_sh = 0;
		activity_open_cross_day = 0;
		activity_begin_timestamp = 0;
		activty_end_timestamp = 0;
		activity_unique_id = 0;
		for (int i = 0; i < ARRAY_LENGTH(rank_list); i++)
		{
			rank_list[i].Reset();
		}
	}
	
	long long reserve_ll[3];

	char is_has_ready_open;								//�Ƿ���׼��Ҫ����(0:û�� 1:��(��ʱ��ȴ�ԭ�Ƚ���������ϲ��ܿ����µ�)) 
	char is_lock_flag;									//�Ƿ����� 0:δ���� 1:������(�����������Ч)
	short reserve_sh;
	int activity_open_cross_day;						//�����ʱ�Ŀ������
	unsigned int activity_begin_timestamp;				//���ʼ
	unsigned int activty_end_timestamp;					//�����
	long long activity_unique_id;						//�Ψһ��ʶ
	KuaFuYingXiongLuRankItem rank_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];		//�������а����ʱ���
};

typedef char KuaFuYingXiongLuActivityParamHex[sizeof(KuaFuYingXiongLuActivityParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(KuaFuYingXiongLuActivityParamHex) < 1024);

struct KuaFuYingXiongLuTaskInfo
{
	KuaFuYingXiongLuTaskInfo() { this->Reset(); }

	void Reset()
	{
		task_type = 0;
		is_fetch_flag = 0;
		reserve_sh = 0;
		schedule = 0;
	}

	bool IsInvalid() const
	{
		return task_type <= KUA_FU_YING_XIONG_LU_TASK_TYPE_INVALID || task_type >= KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX;
	}

	unsigned char task_type;
	char is_fetch_flag;		//������ȡ��ʶ 0:δ��ȡ
	short reserve_sh;
	int schedule;			//�������
};
UNSTD_STATIC_CHECK(KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX < MAX_UINT8);

struct KFYXLRoleRankTypeItem
{
	KFYXLRoleRankTypeItem() { this->Reset(); }

	void Reset()
	{
		reserve_sh = 0;
		reserve_ch = 0;
		is_fetch_rank_end_reward_flag = 0;
		for (int i = 0; i < ARRAY_LENGTH(task_list); ++i)
		{
			task_list[i].Reset();
		}
	}

	short reserve_sh;
	char reserve_ch;
	char is_fetch_rank_end_reward_flag;											// �Ƿ񾭹������а������ʶ 0:δ������ 1:������
	KuaFuYingXiongLuTaskInfo task_list[MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM];// �����б�
};
UNSTD_STATIC_CHECK(MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM == 10);

//���Ӣ��¼
struct RoleKuaFuYingXiongParam
{
	RoleKuaFuYingXiongParam() { this->Reset(); }

	void Reset()
	{
		memset(reserev_ll, 0, sizeof(reserev_ll));

		user_begin_participate_timestamp = 0;
		activity_begin_timestamp = 0;
		activity_open_cross_day = 0;
		activity_unique_id = 0;

		for (int i = 0; i < ARRAY_LENGTH(rank_info_list); i++)
		{
			rank_info_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(reserve_rank_list); i++)
		{
			reserve_rank_list[i].Reset();
		}
	}

	long long reserev_ll[3];
	unsigned int user_begin_participate_timestamp;			//��ɫ��ʼ�����ʱ���
	unsigned int activity_begin_timestamp;					//�����ʱ���
	int activity_open_cross_day;							//�����ʱ�Ŀ������
	long long activity_unique_id;							//�Ψһ��ʶ
	unsigned int user_recently_participate_timestamp;		//�����������ʱ��(������ڰ񵥹��������߹����ɵ�������)

	KFYXLRoleRankTypeItem rank_info_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];	//��ɫ����Ϣ�б�
	KFYXLRoleRankTypeItem reserve_rank_list[5];	//����
};
UNSTD_STATIC_CHECK(MAX_KUA_FU_YING_XIONG_LU_RANK_NUM == 10);
typedef char RoleKuaFuYingXiongParamHex[sizeof(RoleKuaFuYingXiongParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleKuaFuYingXiongParamHex) < 4096);

#pragma pack(pop)

#endif	// __KUA_FU_YING_XIONG_LU_DEF_HPP__