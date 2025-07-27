#ifndef __RA_JING_JI_YUE_KA_PARAM_HPP__
#define __RA_JING_JI_YUE_KA_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_JING_JI_YUE_KA_DAY_NUM = 30;			//�����¿���������
static const int MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM = 20;		//�����¿�ÿ����������
static const int MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM = 20;		//�����¿�ÿ����������
static const int MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM = 20;	//�����¿�������������
static const int MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM = 75;		//�����¿�������������
static const int MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM = 3;	//�����¿���������������
static const int MAX_RA_JING_JI_YUE_KA_LEVEL_NUM = 64;			//�����¿��ȼ�
static const int MAX_RA_JING_JI_YUE_KA_DAY_TASK_REFRESH_NUM = 1;//�����¿�ÿ�������ˢ�´���
static const int MAX_RA_JING_JI_YUE_KA_TASK_COUNT = 20;		

UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_COUNT >= MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM);
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_COUNT >= MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM);
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_COUNT >= MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM);

enum RA_JING_JI_YUE_KA_TASK_GROUP_TYPE
{
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN = 0,
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON =  RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN,	//��������
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY = 1,		//ÿ������
	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK = 2,		//ÿ������

	RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX
};
UNSTD_STATIC_CHECK(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX == MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM);

enum RA_JING_JI_YUE_KA_TASK_TYPE
{
	RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN = 0,
	RA_JING_JI_YUE_KA_TASK_TYPE_CLOUD_ARENA_PAR = RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN,	// 񷶥֮�� P1:�������
	RA_JING_JI_YUE_KA_TASK_TYPE_CLOUD_ARENA_WIN = 1,			// 񷶥֮��	P1:ʤ������		//ʤ�������ǵ��Լ���������ս������
	RA_JING_JI_YUE_KA_TASK_TYPE_PLATFORM_BATTLE_PAR = 2,		// �巽��̨	P1:������� 
	RA_JING_JI_YUE_KA_TASK_TYPE_PLATFORM_BATTLE_WIN = 3,		// �巽��̨	P1:ʤ������
	RA_JING_JI_YUE_KA_TASK_TYPE_HUAN_JIE_ZHAN_CHANG_PAR = 4,	// �ý�ս��	P1:�������
	RA_JING_JI_YUE_KA_TASK_TYPE_HUAN_JIE_ZHAN_CHANG_WIN = 5,	// �ý�ս��	P1:ʤ������
	RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_PAR = 6,				// ������	P1:�������
	RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_WIN = 7,				// ������	P1:ʤ������
	RA_JING_JI_YUE_KA_TASK_TYPE_DREAM_NOTES_CHALLENGE = 8,		// ��Ԩ��¼	P1:ͨ�ش���
	RA_JING_JI_YUE_KA_TASK_TYPE_MATERIAL = 9,					// ���龳	P1:ͨ�ش���
	RA_JING_JI_YUE_KA_TASK_TYPE_PET_GOD_FB = 10,				// ��ӡ֮��	P1:ͨ�ش���
	RA_JING_JI_YUE_KA_TASK_TYPE_CRYSTAL = 11,					// �ر���	P1:ͨ�ش���
	RA_JING_JI_YUE_KA_TASK_TYPE_LOGIN = 12,						// ��½		P1:��½����
	RA_JING_JI_YUE_KA_TASK_TYPE_USE_CRYSTAL_FB_ENERGY = 13,		// ��������	P1:������ֵ
	RA_JING_JI_YUE_KA_TASK_TYPE_BIG_DIPPER = 14,				// ��������	P1:ͨ�ش���
	RA_JING_JI_YUE_KA_TASK_TYPE_LUN_HUI_WANG_CHUAN = 15,		// �ֻ�����	P1:ͨ�ش���
	RA_JING_JI_YUE_KA_TASK_TYPE_LEARN_PET_SKILL	= 16,			// ѧϰ���＼��	P1:ѧϰ���������� (������)
	RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_RANK = 17,			// ����������	P1:����������(ֱ��ȡ��ǰ,������ǰ�������)
	RA_JING_JI_YUE_KA_TASK_TYPE_COURAGE_CHALLENGE_STAR = 18,	// ��ħ֮��	P1:����(ֱ��ȡ���ģ���ۼ�)
	RA_JING_JI_YUE_KA_TASK_TYPE_ONLINE_TIME = 19,				// ����ʱ��	P1:����ʱ��(��)
	RA_JING_JI_YUE_KA_TASK_TYPE_PET_INTEN = 20,					// ����ǿ��	P1:ǿ������
	RA_JING_JI_YUE_KA_TASK_TYPE_KILL_WILD_BOSS = 21,			// ��ɱҰ��BOSS	P1:��ɱ����
	RA_JING_JI_YUE_KA_TASK_TYPE_BOUNTY_TASK = 22,				// ��������	P1:�������	(�ͽ�����)
	RA_JING_JI_YUE_KA_TASK_TYPE_BRAVE_GROUND = 23,				// �´��ع�	P1:ͨ����߲�(ȡ����ͨ�����)
	RA_JING_JI_YUE_KA_TASK_TYPE_ACTIVE = 24,					// �ճ���Ծ	P1:��Ծ��ֵ
	RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_COIN = 25,				// ����ͭ��	P1:��������
	RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_GOLD = 26,				// ����Ԫ��	P1:��������
	RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_SILVER_COIN = 27,		// ��������	P1:��������

	RA_JING_JI_YUE_KA_TASK_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM >= RA_JING_JI_YUE_KA_TASK_TYPE_MAX);

struct RAJingJiYueKaTaskInfo
{
	RAJingJiYueKaTaskInfo() { this->Reset(); }

	void Reset()
	{
		task_id = -1;
		finish_flag = 0;
		refresh_num = 0;
	}

	short task_id;				//��Ӧ��������������
	char finish_flag;			//0:δ��� 1:�����
	unsigned char refresh_num;	//����ˢ�´���
};
UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_DAY_TASK_REFRESH_NUM <= MAX_UINT8);

struct RAJingJiYueKaParam 
{
	RAJingJiYueKaParam () { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		next_add_login_day_timestamp = 0;
		reserve_ll_1 = 0; 
		active_role_level = 0;
		next_switch_timestamp = 0;
		last_active_role_level = 0;
		last_role_card_level = 0;
		last_switch_timestamp = 0;
		is_active_buy_flag = 0;
		reserve_ch = 0;
		role_card_level = 0;
		role_card_exp = 0;
		fetch_common_flag.Reset();
		reserve_ll_2 = 0;
		fetch_adv_flag.Reset();
		reserve_ll_3 = 0;
		day_task_count = 0;
		week_task_count = 0;
		season_task_count = 0;
		reserve_sh = 0;
		for (int i = 0; i < ARRAY_LENGTH(day_task_list); ++i)
		{
			day_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(week_task_list); ++i)
		{
			week_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(season_task_list); ++i)
		{
			season_task_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(progress_list, 0, sizeof(progress_list));
	}

	void SeasonReset()
	{
		reserve_ll_1 = 0;
		active_role_level = 0;
		next_switch_timestamp = 0;
		is_active_buy_flag = 0;
		reserve_ch = 0;
		role_card_level = 0;
		role_card_exp = 0;
		fetch_common_flag.Reset();
		reserve_ll_2 = 0;
		fetch_adv_flag.Reset();
		reserve_ll_3 = 0;
		day_task_count = 0;
		week_task_count = 0;
		season_task_count = 0;
		reserve_sh = 0;
		for (int i = 0; i < ARRAY_LENGTH(day_task_list); ++i)
		{
			day_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(week_task_list); ++i)
		{
			week_task_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(season_task_list); ++i)
		{
			season_task_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON], 0, sizeof(progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON]));
	}

	unsigned int ra_begin_timestamp;
	unsigned int next_add_login_day_timestamp;	//�´���ӵ�½����ʱ���

	long long reserve_ll_1;
	int active_role_level;					//���������ʱ��ɫ�ȼ�
	unsigned int next_switch_timestamp;		//�´��л�״̬ʱ���	

	int last_active_role_level;				//����������ʱ��ɫ�ȼ�
	int last_role_card_level;				//�������¿��ȼ�
	unsigned int last_switch_timestamp;		//�ϴ��л�״̬ʱ���
	
	char is_active_buy_flag;				//�Ƿ�ֱ�������˶��⽱��(������) 0:δ���� 1:�Ѽ���
	char reserve_ch;
	short role_card_level;					//�¿��ȼ�
	int role_card_exp;						//�¿�����

	BitMap<MAX_RA_JING_JI_YUE_KA_LEVEL_NUM> fetch_common_flag;				//��ͨ�ȼ�������ȡ��־λ
	long long reserve_ll_2;
	BitMap<MAX_RA_JING_JI_YUE_KA_LEVEL_NUM> fetch_adv_flag;					//�߼��ȼ�������ȡ��־λ
	long long reserve_ll_3;
			
	short day_task_count;
	short week_task_count;
	short season_task_count;
	short reserve_sh;									
	RAJingJiYueKaTaskInfo day_task_list[MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM];
	RAJingJiYueKaTaskInfo week_task_list[MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM];
	RAJingJiYueKaTaskInfo season_task_list[MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM];

	long long reserve_ll[3];
	
	unsigned int progress_list[MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM][MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM];
};
UNSTD_STATIC_CHECK(sizeof(RAJingJiYueKaParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)

#endif