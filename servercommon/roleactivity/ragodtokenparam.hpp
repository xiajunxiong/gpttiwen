#ifndef __RA_GOD_TOKEN_PARAM_HPP__
#define __RA_GOD_TOKEN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2081 ����� - ����ս��  //////////////////////////////////////////

enum RA_GOD_TOKEN_DAY_TYPE								//����ս��ÿ����������
{
	RA_GOD_TOKEN_DAY_TYPE_BEGIN = 0,
	RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB,					//���ˮ������(�ؾ�)
	RA_GOD_TOKEN_DAY_TYPE_COLOSSEUM,					//���뾺����
	RA_GOD_TOKEN_DAY_TYPE_WST_QUESTION,					//���������
	RA_GOD_TOKEN_DAY_TYPE_TRANSACTION_TASK,				//���Զ��ó��(˿��֮·)
	RA_GOD_TOKEN_DAY_TYPE_RESEARCH_TASK,				//���������ħ
	RA_GOD_TOKEN_DAY_TYPE_MAKE_EQUIP,					//����װ��
	RA_GOD_TOKEN_DAY_TYPE_LIMIT_ACTIVITY,				//������ʱ�
	RA_GOD_TOKEN_DAY_TYPE_GUILD_STORE,					//�����̵�����
	RA_GOD_TOKEN_DAY_TYPE_PRESTIGE,						//��������
	RA_GOD_TOKEN_DAY_TYPE_SEAL_PET,						//��ӡ����
	RA_GOD_TOKEN_DAY_TYPE_CONSUME_COIN,					//����ͭ��
	RA_GOD_TOKEN_DAY_TYPE_SHOP_BUY_GEM,					//�̻Ṻ��ʯ
	RA_GOD_TOKEN_DAY_TYPE_TOWER,						//��ս��ħ֮��
	RA_GOD_TOKEN_DAY_TYPE_EQUIP_VOC_CHANGE,				//װ������
	RA_GOD_TOKEN_DAY_TYPE_PLATFORM_BATTLE,				//������̨ս
	RA_GOD_TOKEN_DAY_TYPE_MAKE_JEWELTY,					//��������
	RA_GOD_TOKEN_DAY_TYPE_ESCORT,						//�������
	RA_GOD_TOKEN_DAY_TYPE_WILD_BOSS,					//����Ұ��boss
	RA_GOD_TOKEN_DAY_TYPE_LOGIN,						//��½
	RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB_TI_LI,				//�ؾ�����

	RA_GOD_TOKEN_DAY_TYPE_MAX
};
enum RA_GOD_TOKEN_WEEK_TYPE								//����ս��ÿ����������
{
	RA_GOD_TOKEN_TYPE_BEGIN = 0,
	RA_GOD_TOKEN_TYPE_WEEK_PET_LEARN_SKILL,				//����ѧϰ������
	RA_GOD_TOKEN_TYPE_WEEK_LIMIT_ACTIVITY,				//������ʱ�
	RA_GOD_TOKEN_TYPE_WEEK_COMSUME_COIN,				//����ͭ��
	RA_GOD_TOKEN_TYPE_WEEK_SEAL_PET,					//��ӡ����
	RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB,					//���ˮ������(�ؾ�)
	RA_GOD_TOKEN_TYPE_WEEK_MARKET,						//��ɽ���������
	RA_GOD_TOKEN_TYPE_WEEK_GET_PRESTIGE,				//�������
	RA_GOD_TOKEN_TYPE_WEEK_XUN_BAO,						//����Ѱ��
	RA_GOD_TOKEN_TYPE_WEEK_WILD_BOSS,					//����Ұ��boss
	RA_GOD_TOKEN_TYPE_WEEK_PET_BREAKTHROUGHS,			//����ǿ��
	RA_GOD_TOKEN_TYPE_WEEK_MI_YI,						//����Զ��ħڣ����(��ҽ��ı)
	RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB_TI_LI,			//�ؾ�����
	RA_GOD_TOKEN_TYPE_WEEK_LOGIN,						//��½
	RA_GOD_TOKEN_TYPE_WEEK_SHOP_BUY_GEM,				//�̻Ṻ��ʯ

	RA_GOD_TOKEN_WEEK_TYPE_MAX
};
enum RA_GOD_TOKEN_TASK_TYPE
{
	RA_GOD_TOKEN_TASK_TYPE_DAILY = 0,
	RA_GOD_TOKEN_TASK_TYPE_WEEKLY
};
static const int RA_GOD_TOKEN_TYPE_MAXNUM = 32;					//����ս���������
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TYPE_MAXNUM >= RA_GOD_TOKEN_DAY_TYPE_MAX);
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TYPE_MAXNUM >= RA_GOD_TOKEN_WEEK_TYPE_MAX);

static const int RA_GOD_TOKEN_EXCHANGE_MAXNUM = 32;				//����ս��һ����seq
static const int RA_GOD_TOKEN_EXCHANGE_BUY_EXP_ADD = 10;		//����ս���һ�����Ӿ���

static const int RA_GOD_TOKEN_TASK_NUM = 10;					//����ս��������(���)
static const int RA_GOD_TOKEN_DAILY_TASK_NUM = 6;				//����ս��ÿ��������
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TASK_NUM >= RA_GOD_TOKEN_DAILY_TASK_NUM);
static const int RA_GOD_TOKEN_WEEKLY_TASK_NUM = 10;				//����ս��ÿ��������
UNSTD_STATIC_CHECK(RA_GOD_TOKEN_TASK_NUM >= RA_GOD_TOKEN_WEEKLY_TASK_NUM);

static const int RA_GOD_TOKEN_MAX_LEVEL = 300;					//����ս�����ȼ�
static const int RA_GOD_TOKEN_LEVEL_BIT_INFO = 64;				//�ȼ������·���Ϣ

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAGodTokenParam
{
	RAGodTokenParam() { this->Reset(); }
	void Reset()
	{
		ra_god_token_begin_timestamp = 0;
		ra_god_token_is_open = 0;
		ra_god_token_time_seq = -1;
		ra_god_token_is_active_token = 0;
		weekly_task_add_mark = 0;

		ra_god_token_level = 0;
		ra_god_token_exp = 0;

		ra_god_token_fetch_common_flag.Reset();
		ra_god_token_fetch_senior_flag.Reset();

		memset(ra_god_token_exchange_count, 0, sizeof(ra_god_token_exchange_count));

		ra_god_token_daily_task_level = 0;
		memset(ra_god_token_daily_task_type, 0, sizeof(ra_god_token_daily_task_type));
		memset(ra_god_token_daily_type_count, 0, sizeof(ra_god_token_daily_type_count));
		ra_god_token_daily_reward_flag.Reset();

		ra_god_token_weekly_task_level = 0;
		memset(ra_god_token_weekly_task_type, 0, sizeof(ra_god_token_weekly_task_type));
		memset(ra_god_token_weekly_type_count, 0, sizeof(ra_god_token_weekly_type_count));
		ra_god_token_week_reward_flag.Reset();

		surplus_exp = 0;
		surplus_receive_count = 0;
		surplus_senior_count = 0;
		ch_reserver = 0;

		ra_god_token_week_task_limit_act_flag.Reset();

		time_seq_timestamp = 0;
		time_level = 0;
		is_change_level_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	unsigned int	ra_god_token_begin_timestamp;
	char			ra_god_token_is_open;											//�Ƿ�ﵽҪ��ȼ������
	char			ra_god_token_time_seq;											//ʱ�������
	char			ra_god_token_is_active_token;									//�Ƿ����˸߼�ս��
	char			weekly_task_add_mark;

	short			ra_god_token_level;												//ս��ȼ�
	short			ra_god_token_exp;												//ս���

	BitMap<RA_GOD_TOKEN_MAX_LEVEL> ra_god_token_fetch_common_flag;					//��ͨ�ȼ�������ȡ��־λ
	BitMap<RA_GOD_TOKEN_MAX_LEVEL> ra_god_token_fetch_senior_flag;					//�߼��ȼ�������ȡ��־λ

	unsigned char	ra_god_token_exchange_count[RA_GOD_TOKEN_EXCHANGE_MAXNUM];		//�һ�����

	short			ra_god_token_daily_task_level;
	char			ra_god_token_daily_task_type[RA_GOD_TOKEN_TASK_NUM];			//ÿ���������ͣ��ȸ����10����Ŀǰ6����
	unsigned int	ra_god_token_daily_type_count[RA_GOD_TOKEN_TYPE_MAXNUM];		//ÿ������������ɴ���
	BitMap<RA_GOD_TOKEN_TYPE_MAXNUM> ra_god_token_daily_reward_flag;				//ÿ���������ͽ�����־

	short			ra_god_token_weekly_task_level;
	char			ra_god_token_weekly_task_type[RA_GOD_TOKEN_TASK_NUM];			//ÿ���������ͣ��ȸ����10����Ŀǰ10����
	unsigned int	ra_god_token_weekly_type_count[RA_GOD_TOKEN_TYPE_MAXNUM];		//ÿ������������ɴ���
	BitMap<RA_GOD_TOKEN_TYPE_MAXNUM> ra_god_token_week_reward_flag;					//ÿ���������ͽ�����־

	short			surplus_exp;													//������飨�������ü�һ�ο���ȡ������
	short			surplus_receive_count;											//�����������ȡ����
	short			surplus_senior_count;											//��������߼�����������û����ս��ǰ��ȡ�����������������ϸ߼�����
	short			ch_reserver;

	BitMap<INT_TYPE_BIT> ra_god_token_week_task_limit_act_flag;						//ÿ���������� - ��ʱ� ��¼ÿ��������ĸ���ʱ���ÿ������

	unsigned int	time_seq_timestamp;
	int				time_level;														//������õȼ�
	char			is_change_level_flag;											//����������ȼ��ֶ����ݴ��� 1:�Ѵ���
	char			reserve_ch;
	short			reserve_sh;										
};

UNSTD_STATIC_CHECK(sizeof(RAGodTokenParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_GOD_TOKEN_PARAM_HPP__
