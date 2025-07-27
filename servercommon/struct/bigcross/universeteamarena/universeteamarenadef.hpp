#ifndef __UNIVERSE_TEAM_ARENA_DEF_HPP__
#define __UNIVERSE_TEAM_ARENA_DEF_HPP__

#include <cstring>
#include "servercommon/serverdef.h"
#include <vector>
#include "servercommon/servercommon.h"

enum UTA_STATUS
{
	UTA_STATUS_INVALID = 0,
	UTA_STATUS_PREPARE = 1,			//!< 1 准备阶段
	UTA_STATUS_QUALIFICATION = 2,	//!< 2 资格赛阶段
	UTA_STATUS_MAIN = 3,			//!< 3 正赛阶段
	UTA_STATUS_CALCULATE = 4,		//!< 4 结算阶段
	UTA_STATUS_REST = 5,			//!< 5 休赛阶段
	UTA_STATUS_MAX
};

enum UTA_FIGHT_RESULT
{
	UTA_FIGHT_RESULT_NONE = 0,
	UTA_FIGHT_RESULT_ATK_WIN = 1,
	UTA_FIGHT_RESULT_DEF_WIN = 2,
	UTA_FIGHT_RESULT_WAITING = 3,	//!< 已开始，等待结果中
	UTA_FIGHT_RESULT_DRAW = 4,		//!< 平局 （资格赛允许平局）
	UTA_FIGHT_RESULT_NUM
};

enum UTA_SCHEDULE_TYPE
{
	UTA_SCHEDULE_QUALIFICATION = 0,
	UTA_SCHEDULE_MAIN = 1,
};

enum UTA_ZHANJI_TYPE
{
	UTA_ZHANJI_QUALIFICATION = 0,
	UTA_ZHANJI_MAIN = 1,
};

enum UTA_MAIN_RANK_TYPE
{
	UTA_MAIN_RANK_NONE = 0,			//!< 没有正赛排名
	UTA_MAIN_RANK_CHAMPION = 1,		//!< 冠军 
	UTA_MAIN_RANK_RUNNER_UP = 2,	//!< 亚军
	UTA_MAIN_RANK_TOP_2 = 3,		//!< 二强
	UTA_MAIN_RANK_3RD = 4,			//!< 季军
	UTA_MAIN_RANK_4TH = 5,			//!< 殿军
	UTA_MAIN_RANK_TOP_4 = 6,		//!< 四强
	UTA_MAIN_RANK_5TH = 7,			//!< 第五名
	UTA_MAIN_RANK_6TH = 8,			//!< 第六名
	UTA_MAIN_RANK_7TH = 9,			//!< 第七名
	UTA_MAIN_RANK_8TH = 10,			//!< 第八名
	UTA_MAIN_RANK_TOP_8 = 11,		//!< 八强
	UTA_MAIN_RANK_9TH = 12,			//!< 第九名
	UTA_MAIN_RANK_10TH = 13,		//!< 第十名
	UTA_MAIN_RANK_11TH = 14,		//!< 第十一名
	UTA_MAIN_RANK_12TH = 15,		//!< 第十二名
	UTA_MAIN_RANK_13TH = 16,		//!< 第十三名
	UTA_MAIN_RANK_14TH = 17,		//!< 第十四名
	UTA_MAIN_RANK_15TH = 18,		//!< 第十五名
	UTA_MAIN_RANK_16TH = 19,		//!< 第十六名
	UTA_MAIN_RANK_TOP_16 = 20,		//!< 十六强
	UTA_MAIN_RANK_17TH = 21,		//!< 第十七名
	UTA_MAIN_RANK_18TH = 22,		//!< 第十八名
	UTA_MAIN_RANK_19TH = 23,		//!< 第十九名
	UTA_MAIN_RANK_20TH = 24,		//!< 第二十名
	UTA_MAIN_RANK_21TH = 25,		//!< 第二十一名
	UTA_MAIN_RANK_22TH = 26,		//!< 第二十二名
	UTA_MAIN_RANK_23TH = 27,		//!< 第二十三名
	UTA_MAIN_RANK_24TH = 28,		//!< 第二十四名
	UTA_MAIN_RANK_25TH = 29,		//!< 第二十五名
	UTA_MAIN_RANK_26TH = 30,		//!< 第二十六名
	UTA_MAIN_RANK_27TH = 31,		//!< 第二十七名
	UTA_MAIN_RANK_28TH = 32,		//!< 第二十八名
	UTA_MAIN_RANK_29TH = 33,		//!< 第二十九名
	UTA_MAIN_RANK_30TH = 34,		//!< 第三十名
	UTA_MAIN_RANK_31TH = 35,		//!< 第三十一名
	UTA_MAIN_RANK_32TH = 36,		//!< 第三十二名
	UTA_MAIN_RANK_TOP_32 = 37,		//!< 三十二强
	UTA_MAIN_RANK_MAX,
};

enum UTA_PUT_REASON_TYPE
{
	UTA_PUT_REASON_TYPE_NONE = 0,
	UTA_PUT_REASON_TYPE_WIN_BOX_REWARD = 1,
	UTA_PUT_REASON_TYPE_WIN_BOX_REISSUE = 2,   // 胜场宝箱补发
	UTA_PUT_REASON_TYPE_QUALIFICATION_JOIN_REWARD = 3,	// 资格赛每日参与奖励
	UTA_PUT_REASON_TYPE_MAX
};

static const int UTA_QUALIFICATION_ROUND_NUM = 10;
static const int UTA_MAIN_ROUND_NUM = 5;
static const int UTA_TEAM_MEMBER_NUM = 5;
static const int MAX_UTA_TEAM_ONCE_COUNT = 32;
static const int MAX_UTA_SCHEDULE_ONCE_COUNT = 512;
static const int UTA_MAIN_TEAM_NUM = 32;
static const int UTA_MAIN_MATCH_NUM_PER_ROUND = UTA_MAIN_TEAM_NUM / 2;
static const int UTA_QUALIFICATION_ROUND_DURATION_S = 30 * 60;
static const int UTA_BATTLE_STANDBY_TIME_S = 10 * 60;
static const int UTA_BATTLE_DURATION_S = 30 * 60;
static const int UTA_PREPARE_ENTER_SCENE_TIME_S = 10 * 60;
static const int UTA_LAST_SEASON_OVERVIEW_BUFF_SIZE = 16384;

#pragma pack(push)
#pragma pack(4)

class TLVSerializer;
class TLVUnserializer;
struct UTABaseParam
{
	UTABaseParam() { this->Reset(); }

	void Reset()
	{
		cur_status = 0u;
		qualification_round_idx = -1;
		main_round_idx = -1;
		season_idx = 0;
		next_status_timestamp = 0u;
		prepare_status_timestamp = 0u;
		qualification_status_timestamp = 0u;
		main_status_timestamp = 0u;
		calculate_status_timestamp = 0u;
		rest_status_timestamp = 0u;
		qualification_round_timestamp = 0u;
		main_round_timestamp = 0u;
		next_make_main_schedule_timestamp = 0u;

		memset(qualification_timestamp_list, 0, sizeof(qualification_timestamp_list));
		memset(main_timestamp_list, 0, sizeof(main_timestamp_list));

		last_season_overview_buffer_length = 0;
		memset(last_season_overview_buffer, 0, sizeof(last_season_overview_buffer));

		active_reward_begin_timestamp = 0u;
		active_reward_end_timestamp = 0u;
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	short cur_status;
	short qualification_round_idx;
	short main_round_idx;
	short season_idx;
	unsigned int next_status_timestamp;
	unsigned int prepare_status_timestamp;
	unsigned int qualification_status_timestamp;
	unsigned int main_status_timestamp;
	unsigned int calculate_status_timestamp;
	unsigned int rest_status_timestamp;
	unsigned int qualification_round_timestamp;
	unsigned int main_round_timestamp;
	unsigned int qualification_timestamp_list[UTA_QUALIFICATION_ROUND_NUM];
	unsigned int main_timestamp_list[UTA_MAIN_ROUND_NUM];
	unsigned int next_make_main_schedule_timestamp;
	unsigned int active_reward_begin_timestamp;
	unsigned int active_reward_end_timestamp;
	
	int last_season_overview_buffer_length;
	char last_season_overview_buffer[UTA_LAST_SEASON_OVERVIEW_BUFF_SIZE];
};


class PB_UTATeamMemberData;
struct UTATeamMemberInfo
{
	UTATeamMemberInfo() { this->Reset(); }

	void Reset()
	{
		uuid = 0LL;
		uid = 0;
		profession = 0;
		level = 0;
		top_level = 0;
		avatar_type = 0;
		headshot_id = 0;
		fly_flag = 0;
		today_get_reward_num = 0;
		capablity = 0;
		memset(role_name, 0, sizeof(role_name));
		win_box_fetch_flag = 0;
		appearance.Reset();
	}

	bool CanGetTodayReward() const
	{
		return today_get_reward_num < 2;
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	bool GetPBData(PB_UTATeamMemberData* out_data) const;
	bool SetDataFromPB(const PB_UTATeamMemberData& pb_data);

	bool Serialize(char* buffer, int buffer_size, int* out_length);
	bool Deserialize(const char* data, int data_length, int* out_offset);

	long long uuid;
	int uid;
	int profession;
	short level;
	short top_level;
	short avatar_type;				// 虚拟人物类型
	short headshot_id;				// 头像ID 
	short fly_flag;					// 飞升阶段
	short today_get_reward_num;		// 今天奖励获得次数
	int capablity;					// 角色评分
	GameName role_name;
	Appearance appearance;
	int win_box_fetch_flag;			// 胜场宝箱领取标记（二进制）
};

struct UTARankKey
{
	UTARankKey() : rank_left(-1), rank_right(-1) {}
	UTARankKey(short left, short right) : rank_left(left), rank_right(right) {}

	bool operator < (const UTARankKey& other) const
	{
		if (rank_left != other.rank_left) return rank_left < other.rank_left;
		else return rank_right < other.rank_right;
	}

	bool Invalid() const 
	{
		return -1 == rank_left || -1 == rank_right || rank_left > rank_right;
	}

	bool IsWinGroup() const
	{
		return 0 == rank_left;
	}

	short rank_left;
	short rank_right;
};

struct UTATeamInfo
{
	UTATeamInfo() { this->Reset(); }

	void Reset()
	{
		plat_type = 0;
		hidden_idx = 0;
		season_idx = 0;
		team_unique_id = 0LL;
		team_id_in_hidden_server = 0LL;
		memset(team_name, 0, sizeof(team_name));
		for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
		{
			member_info_list[i].Reset();
		}

		rank_left = -1;
		rank_right = -1;

		memset(qualification_schedules, 0, sizeof(qualification_schedules));
		memset(main_schedules, 0, sizeof(main_schedules));

		qualification_score = 0;
		world_team_arena_score = 0;

		last_modify_fight_result_timestamp = 0u;

		qualification_win_num = 0;
		qualification_lose_num = 0;
		qualification_giveup_num = 0;
		qualification_draw_num = 0;
		main_win_num = 0;
		main_lose_num = 0;

		cur_continuous_win_num = 0;
		max_continuous_win_num = 0;
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int plat_type;
	int hidden_idx;
	int season_idx;						// 赛季号
	long long team_id_in_hidden_server;	// 在跨服的队伍ID
	long long team_unique_id;		// 在大跨服分配的队伍唯一ID
	GameName team_name;

	// [-1,-1)代表没有正赛资格, [0,32) 表示32强, [16,32)表示32强败者组，[0,16)表示16强（或32强胜者组)			
	short rank_left;				
	short rank_right;

	UTATeamMemberInfo member_info_list[UTA_TEAM_MEMBER_NUM];

	long long qualification_schedules[UTA_QUALIFICATION_ROUND_NUM];
	long long main_schedules[UTA_MAIN_ROUND_NUM];

	int qualification_score;
	int world_team_arena_score;
	unsigned int last_modify_fight_result_timestamp;

	short qualification_win_num;
	short qualification_lose_num;
	short qualification_giveup_num;
	short qualification_draw_num;
	short main_win_num;
	short main_lose_num;

	short cur_continuous_win_num;
	short max_continuous_win_num;
};

struct UTAScheduleInfo
{
	UTAScheduleInfo() { this->Reset(); }

	void Reset()
	{
		type = 0;
		fight_result = 0;
		round_idx = 0;
		match_idx = 0;
		schedule_id = 0LL;
		atk_team_unique_id = 0LL;
		def_team_unique_id = 0LL;
		is_win_group = false;
		video_file_id = 0LL;
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int season_idx;				//!< 赛季号
	char type;					//!< 比赛类型  0:资格赛  1:正赛
	char fight_result;			//!< 比赛结果 0:未赛  1:攻方胜   2:守方胜 （没有平局，必须分出胜负）
	char round_idx;				//!< 轮次 （与type一起确定开始时间）
	char match_idx;				//!< 正赛场次（资格赛没有的） 
	long long schedule_id;		//!< 赛程ID
	long long atk_team_unique_id;	//!< 攻方队伍唯一ID
	long long def_team_unique_id;	//!< 守方队伍唯一ID
	char is_win_group;			//!< 是否胜者组（当正赛时有效）
	long long video_file_id;	//!< 赛事录像
};

#pragma pack(pop)

std::string GetScheduleListString(int num, const long long* schedule_list);
bool SetScheduleListFromString(const std::string& str, int num, long long* schedule_list);

class UTATeamListParamForInit
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long next_id_from;
	std::vector<UTATeamInfo> team_list;
};

class UTATeamListParamForSave
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	std::vector<UTATeamInfo> save_team_list;
	std::vector<long long> delete_team_list;
};

class UTAScheduleListParamForInit
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long next_id_from;
	std::vector<UTAScheduleInfo> schedule_list;
};

class UTAScheduleListParamForSave
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	std::vector<UTAScheduleInfo> save_schedule_list;
	std::vector<long long> delete_schedule_list;
};

#endif