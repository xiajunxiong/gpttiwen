#ifndef __WORLD_ARENA_PARAM_HPP__
#define __WORLD_ARENA_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

enum WORLD_ARENA_STATUS
{
	WORLD_ARENA_STATUS_INVALID = 0,
	WORLD_ARENA_STATUS_SIGNUP,		//!< 1.报名阶段 1号-5号
	WORLD_ARENA_STATUS_SCHEDULE,	//!< 2.分组阶段 6号0点
	WORLD_ARENA_STATUS_BATTLE,		//!< 3.正赛阶段 6号-24号
	WORLD_ARENA_STATUS_CALCULATE,	//!< 4.结算阶段 25号0点
	WORLD_ARENA_STATUS_REST,		//!< 5.休赛阶段 25号-月末
};

enum WORLD_ARENA_RESULT
{
	WORLD_ARENA_RESULT_INVALID = 0,
	WORLD_ARENA_RESULT_ATTACKER_WIN,	//!< 1. 攻方胜
	WORLD_ARENA_RESULT_DEFENDER_WIN,	//!< 2. 守方胜
	WORLD_ARENA_RESULT_DRAW,			//!< 3. 平局
	WORLD_ARENA_RESULT_BOTH_GIVEUP,		//!< 4. 双方都弃权
	WORLD_ARENA_RESULT_MAX,
};

enum WORLD_ARENA_DAILY_REWARD_FLAG
{
	WORLD_ARENA_DAILY_REWARD_FLAG_UNFETCHABLE = 0,		//!< 0 不可领取
	WORLD_ARENA_DAILY_REWARD_FLAG_FETCHABLE = 1,		//!< 1 可领取
	WORLD_ARENA_DAILY_REWARD_FLAG_ALREADY_FETCHED = 2,	//!< 2 已领取
	WORLD_ARENA_DAILY_REWARD_FLAG_MAX
};

static const int WORLD_ARENA_FIGHTER_NUM_PER_GROUP = 20;
static const int WORLD_ARENA_ROUND_NUM = WORLD_ARENA_FIGHTER_NUM_PER_GROUP - 1;
static const int WORLD_ARENA_FIGHT_NUM_PER_ROUND = WORLD_ARENA_FIGHTER_NUM_PER_GROUP / 2;
static const int WORLD_ARENA_BATTLE_START_DATE = 6;
static const int WORLD_ARENA_ROUND_BATTLE_STANDBY_TIME_S = 10 * 60;
static const int WORLD_ARENA_ROUND_BATTLE_DURATION_S = 30 * 60;
static const int WORLD_ARENA_ROUND_BATTLE_START_HOUR = 18;
static const int WORLD_ARENA_ROUND_BATTLE_START_MINUTE = 30;
static const int WORLD_ARENA_ONCE_OPER_USER_COUNT = 64;
static const int WORLD_ARENA_ONCE_OPER_SCHEDULE_COUNT = 512;
static const int WORLD_ARENA_WIN_BOX_REWARD_MAX_NUM = 30;
static const int WORLD_ARENA_COUNT_DOWN_S = 3;

class PB_WorldArenaBaseData;
class PB_WorldArenaUserSignupData;
class PB_WorldArenaUserScheduleData;
class PB_WorldArenaUserScoreData;
class PB_WorldArenaUserData;
class PB_WorldArenaScheduleData;

#pragma pack(push)
#pragma pack(4)

struct WorldArenaBaseParam
{
	WorldArenaBaseParam() 
	{
		this->Reset();
	}

	void Reset()
	{
		cur_status = 0;
		season_index = 0;
		next_status_timestamp = 0u;
		next_signup_status_timestamp = 0u;
		next_schedule_status_timestamp = 0u;
		next_battle_status_timestamp = 0u;
		next_calculate_status_timestamp = 0u;
		next_rest_status_timestamp = 0u;
		robot_index_generator = 1;

		round_battle_start_timestamp = 0u;
		next_round_battle_start_timestamp = 0u;
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	bool GetPBData(ARG_OUT PB_WorldArenaBaseData* out_data) const;
	bool SetDataFromPB(const PB_WorldArenaBaseData& pb_data);

	short cur_status;							//!< 当前状态
	short season_index;							//!< 赛季号
	unsigned int next_status_timestamp;			//!< 下次状态变化时间戳
	unsigned int next_signup_status_timestamp;	//!< 下次报名阶段时间戳
	unsigned int next_schedule_status_timestamp;//!< 下次分组阶段时间戳
	unsigned int next_battle_status_timestamp;	//!< 下次开战阶段时间戳
	unsigned int next_calculate_status_timestamp;//!<下次结算阶段时间戳
	unsigned int next_rest_status_timestamp;	//!< 下次休赛阶段时间戳
	int robot_index_generator;					//!< 用于机器人UUID生成

	unsigned int round_battle_start_timestamp;	//!< 本轮战斗开始时间戳
	unsigned int next_round_battle_start_timestamp;	//!< 下轮战斗开始时间戳
};

struct WorldArenaSignUpInfo
{
	WorldArenaSignUpInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		uid = 0;
		plat_type = 0;
		uuid = 0;
		profession = 0;
		avatar_type = 0;
		headshot_id = 0;
		level = 0;
		top_level = 0;
		season_index = 0;
		memset(role_name, 0, sizeof(role_name));
		memset(guild_name, 0, sizeof(guild_name));
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	bool Serialize(char* buffer, int buffer_size, int* out_length);
	bool Deserialize(const char* data, int data_length, int* out_offset);

	bool GetPBData(ARG_OUT PB_WorldArenaUserSignupData* out_data) const;
	bool SetDataFromPB(const PB_WorldArenaUserSignupData& pb_data);

	int uid;
	int plat_type;
	long long uuid;
	int profession;
	short avatar_type;
	short headshot_id;
	short level;
	short season_index;
	int top_level;
	GameName role_name;
	GuildName guild_name;
	long long reserve_ll[4];
};
UNSTD_STATIC_CHECK(sizeof(WorldArenaSignUpInfo) * 2.5 < 512);  // 数据保存的是Protobuf序列化后再BinToHex后的字符串，所以用2.5

struct WorldArenaScheduleInfo
{
	WorldArenaScheduleInfo() { this->Reset(); }
	
	void Reset()
	{
		group_id = 0;
		schedule_id = 0;
		attacker_uuid = 0;
		defender_uuid = 0;
		round_index = 0;
		fight_result = 0;
		check_result_timestamp = 0;
	}

	bool GetPBData(ARG_OUT PB_WorldArenaScheduleData* out_data) const;
	bool SetDataFromPB(const PB_WorldArenaScheduleData& pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long group_id;			//!< 组别id
	long long schedule_id;		//!< 赛程唯一ID
	long long attacker_uuid;	//!< 攻方信息
	long long defender_uuid;	//!< 守方信息
	short round_index;			//!< 场次序号[0, 19)
	short fight_result;			//!< 对战结果
	unsigned int check_result_timestamp; //!< 检查结果时间戳 到时间就检查是否出了结果，未有结果就设为平局
};

struct WorldArenaScoreInfo
{
	WorldArenaScoreInfo() { this->Reset(); }

	void Reset()
	{
		win_num = 0;
		lose_num = 0;
		draw_num = 0;
		giveup_num = 0;
		total_score = 0;
		win_box_reward_flag = 0;
		today_giveup_notice = 0;
		total_battle_round = 0;
		has_fetch_signup_reward = 0;
		daily_reward_fetch_flag = WORLD_ARENA_DAILY_REWARD_FLAG_UNFETCHABLE;
		reserve_sh = 0;
	}

	bool GetPBData(ARG_OUT PB_WorldArenaUserScoreData* out_data) const;
	bool SetDataFromPB(const PB_WorldArenaUserScoreData& pb_data);

	short win_num;		//!< 胜利数
	short lose_num;		//!< 失败数
	short draw_num;		//!< 平局数
	short giveup_num;	//!< 弃权数
	int total_score;	//!< 总积分
	int win_box_reward_flag;	//!< 胜场宝箱领取标记
	short today_giveup_notice;	//!< 是否飘字今天放弃通知
	short total_battle_round;	//!< 总回合数
	char has_fetch_signup_reward;	//!< 是否已领取报名奖励
	char daily_reward_fetch_flag;	//!< 领取每日奖励标记
	short reserve_sh;
};

UNSTD_STATIC_CHECK(sizeof(WorldArenaScoreInfo) * 2.5 < 256); // 数据保存的是Protobuf序列化后再BinToHex后的字符串，所以用2.5

struct WorldArenaUserScheduleInfo
{
	WorldArenaUserScheduleInfo() { this->Reset(); }

	void Reset()
	{
		group_id = 0;
		memset(schedule_list, 0, sizeof(schedule_list));
	}

	bool GetPBData(ARG_OUT PB_WorldArenaUserScheduleData* out_data) const;
	bool SetDataFromPB(const PB_WorldArenaUserScheduleData& pb_data);

	long long group_id;
	long long schedule_list[WORLD_ARENA_ROUND_NUM];
};
UNSTD_STATIC_CHECK(sizeof(WorldArenaUserScheduleInfo) * 2.5 < 512); // 数据保存的是Protobuf序列化后再BinToHex后的字符串，所以用2.5

struct WorldArenaUserInfo
{
	WorldArenaUserInfo() { this->Reset(); }

	void Reset()
	{
		signup_info.Reset();
		score_info.Reset();
		schedule_info.Reset();
		battle_data.Reset();
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	bool GetPBData(ARG_OUT PB_WorldArenaUserData* out_data) const;
	bool SetDataFromPB(const PB_WorldArenaUserData& pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	WorldArenaSignUpInfo signup_info;	//!< 参赛者的报名信息
	WorldArenaScoreInfo score_info;		//!< 参赛者的积分信息	
	WorldArenaUserScheduleInfo schedule_info;

	long long reserve_ll[8];			//!< 保留字
	RoleBattleData battle_data;			//!< 用于机器人战斗，与排行榜展示
};

UNSTD_STATIC_CHECK(sizeof(RoleBattleData) * 2.5 < 24576); // 数据保存的是Protobuf序列化后再BinToHex后的字符串，所以用2.5

#pragma pack(pop)


class WorldArenaUserListParamForInit
{
public:
	WorldArenaUserListParamForInit() : next_id_from(0LL) {}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long next_id_from;
	std::vector<WorldArenaUserInfo> user_list;
};

class WorldArenaUserListParamForSave
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	std::vector<WorldArenaUserInfo> save_user_list;
	std::vector<long long> delete_user_list;
};

class WorldArenaScheduleListParamForInit
{
public:
	WorldArenaScheduleListParamForInit() : next_id_from(0) {}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long next_id_from;
	std::vector<WorldArenaScheduleInfo> schedule_list;
};

class WorldArenaScheduleListParamForSave
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	std::vector<WorldArenaScheduleInfo> save_schedule_list;
	std::vector<long long> delete_schedule_list;
};


#endif