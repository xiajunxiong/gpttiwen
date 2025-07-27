#ifndef __ROLE_ACTIVITY_DEF_HPP__
#define __ROLE_ACTIVITY_DEF_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/mysterioustrialdef.hpp"

#pragma pack(push, 4)

///////////////////////////////////////////////////////////////////////////
// new

struct RoleActivityCommonParam
{
	RoleActivityCommonParam() { this->Reset(); }

	void Reset()
	{
		////////////////////////////////��ֵ����(���ã���������/////////////////////////////
		history_chongzhi_num = 0;
		fanli_reward_fetch_flag = 0;
		chongzhi_reward_fetch_flag = 0;
		chongzhi_num = 0;
		level = 1;			//������Ϊ1��

		//////////////////////////////// �¶�Ͷ�� ///////////////////////////////////////
		ra_month_buy_time_timestamp = 0;
		is_buy_month_investment = 0;
		ra_month_investment_buy_day = 0;
		ra_month_investment_last_num = 0;
		month_investment_fetch_flag.Reset();

		////////////////////////////////// ���ֻ /////////////////////////////////////////
		capability = 0;
		score_reward_flag.Reset();

		// ----------------------------��ʱ�����--------------------------------
		sign_up_flag.Reset();
		sign_up_reward_flag.Reset();
		////////////////////////////////С�Ӿ���///////////////////////////////////////
		team_fight_score = -1;
		team_fight_victory_count = 0;
		team_fight_defeat_count = 0;
		team_fight_box_fetch_flag = 0;
		/////////////////////////////// ����BOSS /////////////////////////////////////
		world_boss_play_times = 0;
		world_boss_2_play_times = 0;

		////////////////////////////// ���⽱�� /////////////////////////////////////
		bonus_flag.Reset();

		//�׳�
		is_add_first_recharge_zhao_cai_day_flag = 0;
		first_cmd_buy_flag = 0;
		first_recharge_zhao_cai_progress = 0;
		first_recharge_grow_end_timestamp = 0;
		memset(first_recharge_day_flag, 0, sizeof(first_recharge_day_flag));
		first_recharge_exclusive_flag.Reset();

		//�¿�/�ܿ�
		is_fetch_card_day_reward_flag = 0;
		reserve_ch = 0;
		month_card_level = 0;
		week_card_end_timestamp = 0;								
		month_card_end_timestamp = 0;									
		memset(card_shop_buy_times, 0, sizeof(card_shop_buy_times));
		chong_zhi_flag = 0;
	}

	void ChongZhiFanLiDayReset()
	{
		fanli_reward_fetch_flag = 0;
		chongzhi_num = 0;
		level = 1;
	}

	void SignUpDataReset()
	{
		sign_up_flag.Reset();
		sign_up_reward_flag.Reset();
	}

	////////////////////////////  ��ȷ������   ////////////////////////
	// ͨ�� ���� û�л�ŵ�
	////////////////////////////////��ֵ����(���ã���������/////////////////////////////
	long long history_chongzhi_num;						// �ܼƳ�ֵ��
	int fanli_reward_fetch_flag;						// ����������ȡ���	(ÿ������)
	int chongzhi_reward_fetch_flag;						// ��ֵ������ȡ���
	long long chongzhi_num;								// ��ֵ����,ÿ������
	int level;											// ��¼���ÿ������ʱ�ĵȼ�
	////////////////////////////////�¶�Ͷ��///////////////////////////////////////
	time_t ra_month_buy_time_timestamp;									// �����¶�Ͷ�ʵ�ʱ��
	unsigned short is_buy_month_investment;								// �Ƿ������¶�Ͷ��  --0δ����   ---1�ѹ���
	unsigned short ra_month_investment_last_num;						// ��ǰʣ�ಹǩ����
	int ra_month_investment_buy_day;									// ��ǰ�ǹ���Ͷ�ʵĵڼ���
	BitMap<RA_MONTH_INVESTMENT_MAX_NUM> month_investment_fetch_flag;	// �¶�Ͷ����ȡ���

	//////////////////////////////////���ֻ/////////////////////////////////////////
	int capability;										//��������
	BitMap<SCORE_REWARD_MAX_NUM> score_reward_flag;		//���ֽ������  SCORE_REWARD_MAX_NUM-32 ��Сint

	// ----------------------------��ʱ�����--------------------------------
	BitMap<MAX_SIGN_UP_NUM> sign_up_flag;							//�Ƿ���
	BitMap<MAX_SIGN_UP_NUM> sign_up_reward_flag;					//���������Ƿ���ȡ
	////////////////////////////////С�Ӿ���///////////////////////////////////////
	int team_fight_score;				// С�Ӿ���-����
	int team_fight_victory_count;		// ʤ��
	int team_fight_defeat_count;		// �ܳ�
	int team_fight_box_fetch_flag;		// ������ȡ��¼(�����Ʊ��)
	/////////////////////////////// ����BOSS /////////////////////////////////////
	short world_boss_play_times;		// ����BOSS�������
	short world_boss_2_play_times;		// ����BOSS�������
	////////////////////////////// ��Ӫ-���⽱��//////////////////////////////////
	BitMap<MAX_BONUS_COUNT_NUM> bonus_flag;	// 0:δ��ȡ 1:����ȡ

	char is_add_first_recharge_zhao_cai_day_flag;							//�����в������Ƿ����ӹ����� 1:���ӹ�
	unsigned char first_cmd_buy_flag;										//�׳�ֱ����ʶ���� ������
	short first_recharge_zhao_cai_progress;									//�׳�ר��-�в������ۼƽ���
	unsigned int first_recharge_grow_end_timestamp;							//�׳�ר��-�ɳ�����δ�������ʱ���
	unsigned char first_recharge_day_flag[MAX_FIRST_RECHARGE_TYPE_NUM];		//�׳��������� ������
	BitMap<MAX_FIRST_RECHARGE_EXCLUSIVE_NUM> first_recharge_exclusive_flag;	//�׳�ר��������ȡ��ʶ

	//�¿�,�ܿ�
	unsigned char is_fetch_card_day_reward_flag;							//�¿�/�ܿ�ÿ�ս�����ȡ��ʶ ������(card_type)
	char reserve_ch;
	short month_card_level;
	unsigned int week_card_end_timestamp;									//�ܿ�����ʱ���
	unsigned int month_card_end_timestamp;									//�¿�����ʱ���
	int card_shop_buy_times[MAX_MONTH_CARD_SHOP_ITEM_NUM];					//�¿��ܿ��̳ǹ����¼
	unsigned int chong_zhi_flag;
};

typedef char RoleActivityCommonParamHex[sizeof(RoleActivityCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleActivityCommonParamHex) < 1024);

struct YuanQiSpeedSection
{
	short section_start;
	short section_end;
};

union YuanQiKey
{
	YuanQiSpeedSection section;
	int key;
};

union RoleActivityRoleData
{
	RoleActivityRoleData() // union�ɴ��й��캯���������Ա�������Զ���Ĺ��캯��
	{
		this->Reset();
	}

	~RoleActivityRoleData() {}

	void Reset()
	{
		memset(base_data, 0, sizeof(base_data));
	}

	char base_data[2000];
};

typedef char RoleActivityRoleDataHex[sizeof(RoleActivityRoleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleActivityRoleDataHex) < 4096);


// �ӻ����Ҳ��Ҫ�޸���������ڵ�switch
unsigned int GetRandActivityRoleDataLengthByType(int rand_activity_type);

static const int MAX_RAND_ACTIVITY_ROLE_PARAM_COUNT = 200;
UNSTD_STATIC_CHECK(MAX_RAND_ACTIVITY_ROLE_PARAM_COUNT >= RAND_ACTIVITY_TYPE_MAX);

class RandActivityRoleParamList
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream) ;

	struct DBRandActivityRoleDataItem
	{
		DBRandActivityRoleDataItem()
		{
			this->Reset(0);
		}

		void Reset(int act_type)
		{
			change_state = 0;
			activity_type = RAND_ACTIVITY_TYPE_BEGIN;

			memset(&data, 0, sizeof(data));

		}

		char change_state;

		int activity_type;
		RoleActivityRoleData data;
	};

	int count;
	DBRandActivityRoleDataItem data_list[MAX_RAND_ACTIVITY_ROLE_PARAM_COUNT];
};

const static int BAO_DI_INFO_ITEM_MAX_NUM = 20;	//Ҫ���׽�������
struct BaoDiInfoItem
{
	BaoDiInfoItem()
	{
		this->Reset();
	}

	void Reset()
	{
		is_get = 0;
		reserved_ch = 0;
		reserved_sh = 0;
		times = 0;
	}

	unsigned char is_get;
	unsigned char reserved_ch;
	unsigned short reserved_sh;
	int times;
};

#pragma pack(pop)

#endif 

