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
		////////////////////////////////充值返利(永久，非随机活动）/////////////////////////////
		history_chongzhi_num = 0;
		fanli_reward_fetch_flag = 0;
		chongzhi_reward_fetch_flag = 0;
		chongzhi_num = 0;
		level = 1;			//玩家最低为1级

		//////////////////////////////// 月度投资 ///////////////////////////////////////
		ra_month_buy_time_timestamp = 0;
		is_buy_month_investment = 0;
		ra_month_investment_buy_day = 0;
		ra_month_investment_last_num = 0;
		month_investment_fetch_flag.Reset();

		////////////////////////////////// 评分活动 /////////////////////////////////////////
		capability = 0;
		score_reward_flag.Reset();

		// ----------------------------限时活动报名--------------------------------
		sign_up_flag.Reset();
		sign_up_reward_flag.Reset();
		////////////////////////////////小队竞技///////////////////////////////////////
		team_fight_score = -1;
		team_fight_victory_count = 0;
		team_fight_defeat_count = 0;
		team_fight_box_fetch_flag = 0;
		/////////////////////////////// 世界BOSS /////////////////////////////////////
		world_boss_play_times = 0;
		world_boss_2_play_times = 0;

		////////////////////////////// 额外奖励 /////////////////////////////////////
		bonus_flag.Reset();

		//首充
		is_add_first_recharge_zhao_cai_day_flag = 0;
		first_cmd_buy_flag = 0;
		first_recharge_zhao_cai_progress = 0;
		first_recharge_grow_end_timestamp = 0;
		memset(first_recharge_day_flag, 0, sizeof(first_recharge_day_flag));
		first_recharge_exclusive_flag.Reset();

		//月卡/周卡
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

	////////////////////////////  已确定区域   ////////////////////////
	// 通用 或者 没有活动号的
	////////////////////////////////充值返利(永久，非随机活动）/////////////////////////////
	long long history_chongzhi_num;						// 总计充值数
	int fanli_reward_fetch_flag;						// 返利奖励领取标记	(每日重置)
	int chongzhi_reward_fetch_flag;						// 充值奖励领取标记
	long long chongzhi_num;								// 充值数量,每日重置
	int level;											// 记录玩家每日重置时的等级
	////////////////////////////////月度投资///////////////////////////////////////
	time_t ra_month_buy_time_timestamp;									// 购买月度投资的时间
	unsigned short is_buy_month_investment;								// 是否购买了月度投资  --0未购买   ---1已购买
	unsigned short ra_month_investment_last_num;						// 当前剩余补签次数
	int ra_month_investment_buy_day;									// 当前是购买投资的第几天
	BitMap<RA_MONTH_INVESTMENT_MAX_NUM> month_investment_fetch_flag;	// 月度投资领取标记

	//////////////////////////////////评分活动/////////////////////////////////////////
	int capability;										//人物评分
	BitMap<SCORE_REWARD_MAX_NUM> score_reward_flag;		//评分奖励标记  SCORE_REWARD_MAX_NUM-32 大小int

	// ----------------------------限时活动报名--------------------------------
	BitMap<MAX_SIGN_UP_NUM> sign_up_flag;							//是否报名
	BitMap<MAX_SIGN_UP_NUM> sign_up_reward_flag;					//报名奖励是否领取
	////////////////////////////////小队竞技///////////////////////////////////////
	int team_fight_score;				// 小队竞技-积分
	int team_fight_victory_count;		// 胜场
	int team_fight_defeat_count;		// 败场
	int team_fight_box_fetch_flag;		// 宝箱领取记录(二进制标记)
	/////////////////////////////// 世界BOSS /////////////////////////////////////
	short world_boss_play_times;		// 世界BOSS参与次数
	short world_boss_2_play_times;		// 世界BOSS参与次数
	////////////////////////////// 运营-额外奖励//////////////////////////////////
	BitMap<MAX_BONUS_COUNT_NUM> bonus_flag;	// 0:未领取 1:已领取

	char is_add_first_recharge_zhao_cai_day_flag;							//当天招财仙子是否增加过进度 1:增加过
	unsigned char first_cmd_buy_flag;										//首充直购标识设置 二进制
	short first_recharge_zhao_cai_progress;									//首充专属-招财仙子累计进度
	unsigned int first_recharge_grow_end_timestamp;							//首充专属-成长助力未购买结束时间戳
	unsigned char first_recharge_day_flag[MAX_FIRST_RECHARGE_TYPE_NUM];		//首充天数奖励 二进制
	BitMap<MAX_FIRST_RECHARGE_EXCLUSIVE_NUM> first_recharge_exclusive_flag;	//首充专属奖励领取标识

	//月卡,周卡
	unsigned char is_fetch_card_day_reward_flag;							//月卡/周卡每日奖励领取标识 二进制(card_type)
	char reserve_ch;
	short month_card_level;
	unsigned int week_card_end_timestamp;									//周卡结束时间戳
	unsigned int month_card_end_timestamp;									//月卡结束时间戳
	int card_shop_buy_times[MAX_MONTH_CARD_SHOP_ITEM_NUM];					//月卡周卡商城购买记录
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
	RoleActivityRoleData() // union可带有构造函数，但其成员不能有自定义的构造函数
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


// 加活动数据也需要修改这个函数内的switch
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

const static int BAO_DI_INFO_ITEM_MAX_NUM = 20;	//要保底奖励个数
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

