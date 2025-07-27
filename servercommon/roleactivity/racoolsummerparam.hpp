#ifndef __RA_COOL_SUMMER_PARAM_HPP__
#define __RA_COOL_SUMMER_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivitydef.hpp"
#include <set>

#pragma pack(push, 4)

static const int MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM = 27;			//�����ۿ��̳ǽ׶����������ܵ��������
static const int MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM = 5;				//�����ۿ��̳ǽ׶����ۿۻ��ܵ��������
static const int MAX_RA_SUPER_OFF_SHOP_PHASE_ITEM_NUM = MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM + MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM;
static const int MAX_RA_SUPER_OFF_SHOP_PHASE_NUM = 5;						//�����ۿ��̳����׶���
static const int MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM = MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM * MAX_RA_SUPER_OFF_SHOP_PHASE_NUM;
static const int MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM = MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM * MAX_RA_SUPER_OFF_SHOP_PHASE_NUM;
static const int MAX_RA_SUPER_OFF_SHOP_iTEM_NUM = MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM + MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM;

static const int MAX_RA_COOL_BREEZE_FIXE_TASK_NUM = 3;						//������ˬ�̶��������ͬʱ���ڵ�����
static const int MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM = 2;					//������ˬ����������ͬʱ���ڵ�����

static const int MAX_RA_LOTUS_MI_BAO_ROW_NUM = 4;							//�����ٱ�һ�й̶�������
static const int MAX_RA_LOTUS_MI_BAO_COLUMN_NUM = 4;						//�����ٱ�һ�й̶�������
static const int MAX_RA_LOTUS_MI_BAO_GRID_NUM = MAX_RA_LOTUS_MI_BAO_ROW_NUM * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM;
static const int RA_LOTUS_MI_BAO_COND_BASE_NUM = 10;
static const int RA_LOUTS_MI_BAO_COND_NUM = 32;
UNSTD_STATIC_CHECK(MAX_RA_LOTUS_MI_BAO_GRID_NUM < (int)(sizeof(int) * BIT_COUNT_OF_BYTE));
UNSTD_STATIC_CHECK(MAX_RA_LOTUS_MI_BAO_ROW_NUM < RA_LOTUS_MI_BAO_COND_BASE_NUM && MAX_RA_LOTUS_MI_BAO_COLUMN_NUM < RA_LOTUS_MI_BAO_COND_BASE_NUM);

//�����ۿ��̳�
UNSTD_STATIC_CHECK(MAX_RA_SUPER_OFF_SHOP_PHASE_ITEM_NUM % sizeof(int) == 0);
UNSTD_STATIC_CHECK(MAX_RA_SUPER_OFF_SHOP_iTEM_NUM % sizeof(int) == 0);
struct RASuperOffShopParam
{
	RASuperOffShopParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		reserve_sh = 0;
		memset(phase_rand_disc_item_list, 0, sizeof(phase_rand_disc_item_list));
		memset(today_normal_buy_times, 0, sizeof(today_normal_buy_times));
		memset(today_disc_buy_times, 0, sizeof(today_disc_buy_times));
		memset(activity_normal_buy_times, 0, sizeof(activity_normal_buy_times));
		memset(activity_disc_buy_times, 0, sizeof(activity_disc_buy_times));
	}

	unsigned int ra_begin_timestamp;							
	short reserve_sh;													
	short phase_rand_disc_item_list[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];		//ˢ�����ۿ���Ʒ�б�
	short today_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM];	//��������ÿ���޹�
	short today_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM];		//�ۿۻ���ÿ���޹�
	short activity_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM];		//�������ܻ�޹�
	short activity_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];			//�ۿۻ��ܻ�޹�
};
UNSTD_STATIC_CHECK(sizeof(RASuperOffShopParam) <= sizeof(RoleActivityRoleData));

//������ˬ
enum RA_COOL_BREEZE_TASK_GROUP_TYPE
{
	RA_COOL_BREEZE_TASK_GROUP_TYPE_BEGIN = 0,
	RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE = 1,			//�̶�����
	RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM = 2,			//�������

	RA_COOL_BREEZE_TASK_GROUP_TYPE_MAX
};

enum RA_COOL_BREEZE_TASK_TYPE
{
	RA_COOL_BREEZE_TASK_TYPE_BEGIN = 0,					
	RA_COOL_BREEZE_TASK_TYPE_PASS_CRYSTAL = 1,			   // ͨ���ر���		param1:����
	RA_COOL_BREEZE_TASK_TYPE_ACTIVE = 2,				   // ��û�Ծ��		param1:��Ծ����ֵ
	RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_NUM = 3,		   // ǿ���������   param1:����
	RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_LEVEL = 4,		   // ǿ������ȼ�   param1:�ȼ�
	RA_COOL_BREEZE_TASK_TYPE_CONSUME_COIN = 5,			   // ����ͭ��		param1:����
	RA_COOL_BREEZE_TASK_TYPE_CONSUME_GOLD = 6,			   // ����Ԫ��		param1:����
	RA_COOL_BREEZE_TASK_TYPE_CONSUME_ENERGY = 7,		   // ��������		param1:����
	RA_COOL_BREEZE_TASK_TYPE_XUNBAO_NUM = 8,			   // ����		  	param1:����
	RA_COOL_BREEZE_TASK_TYPE_SEAL_PET = 9,				   // ��ӡ����		param1:ֻ
	RA_COOL_BREEZE_TASK_TYPE_PET_SKILL_LEARN = 10,		   // ���＼��ѧϰ   param1:����

	RA_COOL_BREEZE_TASK_TYPE_MAX
};

struct RACoolBreezeTaskInfo
{
	RACoolBreezeTaskInfo() { this->Reset(); }
	
	void Reset()
	{
		type = 0;
		task_type = 0;
		schedule = 0;
		open_day = 0;
		refresh_num = 0;
		reserve_ch = 0;
	}

	short type;				//����type
	short task_type;		//��������
	int schedule;			//����
	short open_day;			//�ڼ���ˢ�µ�����
	char refresh_num;		//ˢ���������(���̶�����)
	char reserve_ch;
};

struct RACoolBreezeParam
{
	RACoolBreezeParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		next_fixe_task_refresh_timestamp = 0;
		next_random_task_refresh_timestamp = 0;
		last_refresh_fixe_open_day = 0;
		last_refresh_random_open_day = 0;
		for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; i++)
		{
			fixe_task_list[i].Reset();
		}
		for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
		{
			random_task_list[i].Reset();
		}
	}

	RACoolBreezeTaskInfo & GetSuitRefreshPosInfo(bool is_fixe = true)	//���ȿ�λ
	{
		short suit_refresh_index = 0;
		short min_open_day = MAX_INT16;
		if (is_fixe)
		{
			for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; i++)
			{
				if (fixe_task_list[i].type <= 0)
				{
					return fixe_task_list[i];
				}
				if (fixe_task_list[i].open_day < min_open_day)
				{
					suit_refresh_index = i;
					min_open_day = fixe_task_list[i].open_day;
				}
			}
			return fixe_task_list[suit_refresh_index];
		}

		for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
		{
			if (random_task_list[i].type <= 0)
			{
				return random_task_list[i];
			}
			if (random_task_list[i].open_day < min_open_day)
			{
				suit_refresh_index = i;
				min_open_day = random_task_list[i].open_day;
			}
		}
		return random_task_list[suit_refresh_index];
	}

	void GetTypeList(std::vector<int> * has_type_list, bool is_fixe = true)
	{
		if(NULL == has_type_list) return;

		if (is_fixe)
		{
			for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; i++)
			{
				if(fixe_task_list[i].type <= 0) continue;

				has_type_list->push_back(fixe_task_list[i].type);
			}
			return;
		}
		for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
		{
			if(random_task_list[i].type <= 0) continue;

			has_type_list->push_back(random_task_list[i].type);
		}
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_fixe_task_refresh_timestamp;									//�´ι̶�����ˢ��ʱ���			
	unsigned int next_random_task_refresh_timestamp;								//�´��������ˢ��ʱ���
	short last_refresh_fixe_open_day;												//�ϴ�ˢ�¹̶�����ʱ��Ӧ�Ļ��������
	short last_refresh_random_open_day;												//�ϴ�ˢ���������ʱ��Ӧ���������
	RACoolBreezeTaskInfo fixe_task_list[MAX_RA_COOL_BREEZE_FIXE_TASK_NUM];			//�̶������б�
	RACoolBreezeTaskInfo random_task_list[MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM];		//��������б�
};
UNSTD_STATIC_CHECK(sizeof(RACoolBreezeParam) <= sizeof(RoleActivityRoleData));

enum RA_LOTUS_MI_BAO_CONDITION_TYPE
{
	RA_LOTUS_MI_BAO_CONDITION_TYPE_BEGIN = 0,
	RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW = 1,			//����һ��	
	RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN = 2,		//����һ��	
	RA_LOTUS_MI_BAO_CONDITION_TYPE_ALL = 3,			//����ȫ��

	RA_LOTUS_MI_BAO_CONDITION_TYPE_MAX
};

//�����ٱ�
struct RALotusMiBao
{
	RALotusMiBao() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cond_flag.Reset();
		grid_active_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	BitMap<RA_LOUTS_MI_BAO_COND_NUM> cond_flag;					//�ճ�����������ʶ  һ�л�һ��:(cond - 1) * 10 + (param - 1)  ����ȫ��(3)RA_LOUTS_MI_BAO_COND_NUM - 1 
	BitMap<MAX_RA_LOTUS_MI_BAO_GRID_NUM> grid_active_flag;		//ÿ�����ӵļ����ʶ

};
UNSTD_STATIC_CHECK(sizeof(RALotusMiBao) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif