#ifndef __COLOSSEUM_DEF_HPP__
#define __COLOSSEUM_DEF_HPP__

#include "servercommon/serverdef.h"
#include "gameworld/gameworld/gamedef.h"
#include "servercommon/servercommon.h"
#include "struct/battle/battle_def.hpp"
#include "servercommon/protobuf/role_battle_data.pb.h"

#pragma pack(push, 4)

static const int CHALLENGE_FIELD_MAX_USER = 50000;						// 最大玩家个数
static const int CHALLENGE_FIELD_OPPONENT_MAX = 4;						// 对手个数
static const int CHALLENGE_FIELD_TOP_NUM = 4;							// top 4
static const int CHALLENGE_FIELD_ONCE_OPER_USER_COUNT = 128;			// 竞技场一次操作玩家个数
static const int CHALLENGE_FIELD_INCREASE_STEP = 512;					// 竞技场玩家列表长度增长步长
static const int CHALLENGE_FIELD_RANK_AWARD_MAX_NUM = 5;				// 排名奖励列表长度

static const int CHALLENGE_FIELD_POOL_PLAYER_MAX_NUM = 1022;			// 古神宝库内的最大玩家人数
static const int CHALLENGE_FIELD_POOL_PLAYER_LIST_LEN = sizeof(int) + sizeof(int) * CHALLENGE_FIELD_POOL_PLAYER_MAX_NUM;

const static int COLOSSEUM_ROBOT_SERVER_ID = 2047;						// 机器人标记
static const int COLOSSEUM_IN_POOL_RANK_LIST_NUM = 50;

static const int COLOSSEUM_DEFENDER_BUFF_NUM = 3;

static const int INIT_ROBOT_NUM = 500;

enum InPoolRankRefreshReason
{
	REFRESH_TYPE_INIT = 0,		// 整张列表初始化
	REFRESH_TYPE_PUSH = 1,		// 列表尾部新增

	REFRESH_TYPE_MAX
};

enum COLOSSEUN_PARAM_TYPE
{
	COLOSSEUN_PARAM_TYPE_0 = 0,//最高历史排名
	COLOSSEUN_PARAM_TYPE_1 = 1,//累积挑战次数
	COLOSSEUN_PARAM_TYPE_2 = 2,//累积挑战胜利次数
	COLOSSEUN_PARAM_TYPE_3 = 3,//累积被挑战胜利次数
	COLOSSEUN_PARAM_TYPE_4 = 4,//当前连胜次数
	COLOSSEUN_PARAM_TYPE_5 = 5,//最高连胜次数
	COLOSSEUN_PARAM_TYPE_6 = 6,//单场最高伤害输出
	COLOSSEUN_PARAM_TYPE_7 = 7,//战胜比自己评分高的玩家
	COLOSSEUN_PARAM_TYPE_8 = 8,//累计战败次数
	COLOSSEUN_PARAM_TYPE_9 = 9,//累计复活次数
	COLOSSEUN_PARAM_TYPE_10 = 10,//累计使用药物次数
	COLOSSEUN_PARAM_TYPE_11 = 11,//战斗超过8回合并最终获胜
	COLOSSEUN_PARAM_TYPE_12 = 12,//不带女性伙伴胜利
	COLOSSEUN_PARAM_TYPE_13 = 13,//不带男性伙伴胜利
	COLOSSEUN_PARAM_TYPE_14 = 14,//玩家携带剑客伙伴获胜
	COLOSSEUN_PARAM_TYPE_15 = 15,//玩家携带星术师伙伴获胜
	COLOSSEUN_PARAM_TYPE_16 = 16,//玩家携带巫师伙伴获胜
	COLOSSEUN_PARAM_TYPE_17 = 17,//玩家携带狂战士伙伴获胜
	COLOSSEUN_PARAM_TYPE_18 = 18,//玩家携带神射手伙伴获胜
	COLOSSEUN_PARAM_TYPE_19 = 19,//玩家携带医师伙伴获胜
	COLOSSEUN_PARAM_TYPE_20 = 20,//玩家携带铁卫伙伴获胜
	COLOSSEUN_PARAM_TYPE_21 = 21,//玩家携带拳师伙伴获胜
	COLOSSEUN_PARAM_TYPE_MAX,
};

struct ColosseumFightStatistic // 战斗统计信息 不存库
{
	ColosseumFightStatistic() : has_man_fight(0), has_woman_fight(0), has_start_fight(0),
		has_swordsman_fight(0), has_tiewei_fight(0), has_doctor_fight(0), has_lich_fight(0),
		has_berserk_fight(0), has_boxer_fight(0), has_arrower_fight(0), round_num(0),
		realive_times(0), use_medicine_times(0), total_damage_all(0LL)
	{}

	char has_man_fight;
	char has_woman_fight;
	char has_start_fight;//星术师
	char has_swordsman_fight;//剑客
	char has_tiewei_fight;//铁卫
	char has_doctor_fight;//医生
	char has_lich_fight;//巫医
	char has_berserk_fight;//狂战
	char has_boxer_fight;//拳师
	char has_arrower_fight;//神射
	short round_num;
	short realive_times;
	short use_medicine_times;
	long long total_damage_all;
};

struct ColosseumParam
{
	ColosseumParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(colosseum_param, 0, sizeof(colosseum_param));
		memset(reserve_int, 0, sizeof(reserve_int));
		history_highest_rank_for_reward = -1;
		reserve_sh = 0;
		tips_times = 0;
	}
	int colosseum_param[COLOSSEUN_PARAM_TYPE_MAX];
	int reserve_int[3];
	
	short reserve_sh;
	short tips_times;		// 已提示次数

	int history_highest_rank_for_reward;  // 历史最高排名（但仅用于历史排名奖励）
};

typedef char ColosseumParamHex[sizeof(ColosseumParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ColosseumParamHex) < 256);

struct Report
{
	Report()
	{
		memset(name, 0, sizeof(name));
		rank = 0;
		profession = 0;
		level = 0;
		is_win = 0;
		avatar_type = 0;
		is_attacker = 0;
		headshot_id = 0;
		reserve_sh = 0;
	}

	GameName name;
	int rank;
	short profession;
	char level;
	char is_win;
	short avatar_type;		//!< 模型
	short is_attacker;		//!< 这人是否来打你
	short headshot_id;		//!< 头像ID
	short reserve_sh;
};

class ChallengeUserListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ChallengeUserAttr
	{
		ChallengeUserAttr() : change_state(0), rankpos(0), role_id(0), today_fight_count(0),
			fetch_exp_flag(0), worship_count(0), rank_award_fetch_flag(0), level(0),
			profession(0), avatar_type(0), highest_rank(-1), total_capability(0), colosseum_clear_check(0), battle_data_length(0)
		{
			memset(name, 0, sizeof(name));
			memset(battle_data_buff, 0, sizeof(battle_data_buff));
		}

		char change_state;
		int rankpos;
		int role_id;
		int today_fight_count;
		int fetch_exp_flag;
		int worship_count;
		long long rank_award_fetch_flag;
		int level;
		int profession;
		int avatar_type;
		int highest_rank;
		int total_capability;
		GameName name;
		Appearance appearance;
		ColosseumParam colosseum_param;//竞技场
		int colosseum_clear_check;
		int battle_data_length;
		char battle_data_buff[sizeof(RoleBattleData) * 2];
	};

	ChallengeUserListParam() : next_id_from(0), count(0) {}

	long long next_id_from;
	int count;
	ChallengeUserAttr user_list[CHALLENGE_FIELD_ONCE_OPER_USER_COUNT];
};

typedef char RoleBattleDataHex[sizeof(RoleBattleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleBattleDataHex) < 20000);

#pragma pack(pop)
#endif // !__COLOSSEUM_DEF_HPP__