#ifndef __CLOUD_DEF_HPP__
#define __CLOUD_DEF_HPP__

#include "servercommon/serverdef.h"
#include "gameworld/gameworld/gamedef.h"
#include "servercommon/servercommon.h"
#include "servercommon/protobuf/role_battle_data.pb.h"
#include "servercommon/struct/battle/battle_def.hpp"

static const int CLOUD_ARENA_ROBOT_SERVER_ID = 2047;						// �����˱��
static const int CLOUD_ARENA_OPPONENT_NUM = 4;
static const int CLOUD_ARENA_NEW_OPPONENT_NUM = 3;
static const int CLOUD_ARENA_SEASON_DURATION_S = 2 * 7 * 24 * 60 * 60;
static const int CLOUD_ARENA_ONCE_OPER_USER_COUNT = 64;
static const int CLOUD_ARENA_USER_FIGHT_RECORD_NUM = 20;
static const int CLOUD_ARENA_RANK_LIST_SIZE = 30000;
static const int CLOUD_ARENA_SETTLEMENT_DURATION_S = 30 * 60;	// ����ǰ30�����ǽ���ʱ�� �޷�������ս
static const int CLOUD_ARENA_SEASON_REWARD_TIME = 5 * 60;	// ����ǰ5������������������ʱ��
static const int CLOUD_ARENA_TODAY_CHALLENGE_LIST_SIZE = 12;
static const int CLOUD_ARENA_STANDBY_PET_MAX_NUM = 5;

enum CLOUD_ARENA_RANK_TYPE
{
	CLOUD_ARENA_RANK_TYPE_TOTAL = 0,		//!< �ܰ�
	CLOUD_ARENA_RANK_TYPE_LOCAL_SERVER = 1, //!< ������
	CLOUD_ARENA_RANK_TYPE_DAN = 2,			//!< ��λ��
};

enum CLOUD_ARENA_DAN_TYPE
{
	CLOUD_ARENA_DAN_TYPE_INVALID = 0,		
	CLOUD_ARENA_DAN_TYPE_NEWBIE = 1,		//!< ����
	CLOUD_ARENA_DAN_TYPE_ELITE = 2,			//!< ��Ӣ
	CLOUD_ARENA_DAN_TYPE_WARRIOR = 3,		//!< ս��
	CLOUD_ARENA_DAN_TYPE_COMMANDER = 4,		//!< ͳ˧
	CLOUD_ARENA_DAN_TYPE_LEGENDARY = 5,		//!< ����
	CLOUD_ARENA_DAN_TYPE_GOD = 6,			//!< ��ս����
	CLOUD_ARENA_DAN_TYPE_MAX
};

enum CLOUD_ARENA_ACHIEVEMENT
{
	CLOUD_ARENA_ACHIEVEMENT_RANK = 0,				//!< ����
	CLOUD_ARENA_ACHIEVEMENT_CHALLENGE_TIMES = 1,	//!< ��ս
	CLOUD_ARENA_ACHIEVEMENT_WIN_TIMES = 2,			//!< ��ս�ɹ�����
	CLOUD_ARENA_ACHIEVEMENT_CONTINUOUS_WIN = 3,		//!< ��ʤ����
	CLOUD_ARENA_ACHIEVEMENT_WIN_HIGHER_RANK = 4,	//!< ��ս���߶�λ����Ҳ���ʤ����
	CLOUD_ARENA_ACHIEVEMENT_TOTAL_DAMAGE = 5,		//!< �������˺�
	CLOUD_ARENA_ACHIEVEMENT_USE_MEDICINE = 6,		//!< ʹ��ҩƷ����
	CLOUD_ARENA_ACHIEVEMENT_REALIVE_TIMES = 7,		//!< �������
	CLOUD_ARENA_ACHIEVEMENT_WIN_ROUND_NO_LARGER_THAN_5 = 8, //!< ʤ���غϲ�����5�غ�
	CLOUD_ARENA_ACHIEVEMENT_MAX,
};

enum CLOUD_ARENA_ACHIEVE_STATUS
{
	CLOUD_ARENA_ACHIEVE_STATUS_NOT_FINISH,		//!< 0 δ���
	CLOUD_ARENA_ACHIEVE_STATUS_NOT_CLAIM,		//!< 1 �����δ��ȡ
	CLOUD_ARENA_ACHIEVE_STATUS_ALREADY_CLAIM,	//!< 2 �����
};

enum CLOUD_ARENA_DAY_REWARD
{
	CLOUD_ARENA_DAY_REWARD_INVALID,				//!< 0 ������ȡ
	CLOUD_ARENA_DAY_REWARD_NOT_CALIM,			//!< 1 ����ȡδ��ȡ
	CLOUD_ARENA_DAY_REWARD_ALREADY_CALIM,		//!< 2 ����ȡ
};

#pragma pack(push, 4)

struct CloudArenaSeasonInfo
{
	CloudArenaSeasonInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		season_timestamp = 0u;
		season_end_timestamp = 0u;
		hidden_server_idx = 0;
		has_release_season_reward = 0;
		reserve_ch = 0;
		season_idx = 0;
		world_level = 0;
	}

	unsigned int season_timestamp;		//!< ����ʱ��� Ӧ��Ϊ������ʼʱ��ĵ�����һ���
	unsigned int season_end_timestamp;	//!< ��������ʱ���
	short hidden_server_idx;			//!< cross.xml �е����ط�IDX�ֶΣ������������������������
	char has_release_season_reward;		//!< �Ƿ��ѷ�����������
	char reserve_ch;					//!< ������
	short season_idx;					//!< �������
	short world_level;					//!< ����ȼ�
};

struct CloudArenaOtherInfo
{
	CloudArenaOtherInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		defense_buff_id = 0;
	}

	int defense_buff_id;
};

struct CloudArenaAchievementItem
{
	CloudArenaAchievementItem() : process_num(0), cur_achievement_lv(0), reward_status(0)
	{
		this->Reset();
	}

	void Reset()
	{
		process_num = 0;
		cur_achievement_lv = 0;
		reward_status = 0;
	}

	void ResetOnNewSeasonStart()
	{
		process_num = 0;
		cur_achievement_lv = 0;
		reward_status = 0;
	}
	
	int process_num;
	short cur_achievement_lv;
	short reward_status;		// 0:������ȡ  1:����ȡ  2:����ȡ
};

struct CloudArenaAchievementParam
{
	CloudArenaAchievementParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		continuous_challenge_win_times = 0;
		for (int i = 0; i < CLOUD_ARENA_ACHIEVEMENT_MAX; ++i)
		{
			achieve_list[i].Reset();
		}
	}

	void ResetOnNewSeasonStart()
	{
		continuous_challenge_win_times = 0;
		for (int i = 0; i < CLOUD_ARENA_ACHIEVEMENT_MAX; ++i)
		{
			achieve_list[i].ResetOnNewSeasonStart();
		}
	}

	long long reserve_ll[4];
	int continuous_challenge_win_times;
	CloudArenaAchievementItem achieve_list[CLOUD_ARENA_ACHIEVEMENT_MAX];
};

typedef char CloudArenaAchievementParamHex[sizeof(CloudArenaAchievementParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CloudArenaAchievementParamHex) < 256);

struct CloudArenaParam
{
	CloudArenaParam()
	{
		this->Reset();
	}

	void Reset()
	{
		season_info.Reset();
	}

	bool Serialize(TLVSerializer& outstream) const;
	bool UnSerialize(TLVUnserializer& instream);

	CloudArenaSeasonInfo season_info;
	CloudArenaOtherInfo other_info;
};

class PB_CloudArenaFightRecordUserInfo;
struct CloudArenaFightRecordUserInfo
{
	CloudArenaFightRecordUserInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		uid = 0;
		level = 0;
		profession = 0;
		avatar = 0;
		headshot_id = 0;
		memset(name, 0, sizeof(name));
		change_score = 0;
		change_arena_coin = 0;
		new_rank = 0;
		old_score = 0;
		is_robot = 0;
		reserve_sh = 0;
	}

	bool GetPBData(ARG_OUT PB_CloudArenaFightRecordUserInfo* out_data) const;
	bool SetDataFromPB(const PB_CloudArenaFightRecordUserInfo& pb_data);

	int uid;
	int level;
	int profession;
	int avatar;
	int headshot_id;
	GameName name;
	int change_score;
	int change_arena_coin;
	int new_rank;
	int old_rank;
	int old_score;
	short is_robot;			// ����⣬������Э����
	short reserve_sh;		// ����⣬������Э����
};

class PB_CloudArenaFightRecord;
struct CloudArenaFightRecord
{
	CloudArenaFightRecord()
	{
		this->Reset();
	}

	void Reset()
	{
		record_id = 0;
		record_timestamp = 0;
		attacker_info.Reset();
		defender_info.Reset();
	}

	bool GetPBData(ARG_OUT PB_CloudArenaFightRecord* out_data) const;
	bool SetDataFromPB(const PB_CloudArenaFightRecord& pb_data);

	long long record_id;			//!< ս��ΨһID
	unsigned int record_timestamp;	//!< ս������ʱ���
	int is_attacker_win;			//!< ʤ�����  1:ʤ  0:��  2:ƽ
	CloudArenaFightRecordUserInfo attacker_info;	//!< ս��������Ϣ
	CloudArenaFightRecordUserInfo defender_info;	//!< ս���ط���Ϣ
};

struct CloudArenaUserOtherParam
{
	CloudArenaUserOtherParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reserve_ll = 0;
		last_season_final_score = 0u;
		new_season_first_time_update_last_day_score = 0;
		season_highest_score_flag = 0;
		manual_refresh_times = 0;
		season_highest_score = 0;
	}

	long long reserve_ll;
	int last_season_final_score;
	char new_season_first_time_update_last_day_score;
	char season_highest_score_flag;
	short manual_refresh_times;
	int season_highest_score;
};

typedef char CloudArenaUserOtherParamHex[sizeof(CloudArenaUserOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CloudArenaUserOtherParamHex) < 128);

class CloudArenaUserListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct CloudArenaUserData
	{
		CloudArenaUserData() : change_state(0), uid(0), level(0), profession(0),
			avatar(0), capability(0), score(0), remain_fight_times(0), today_buy_fighttimes_times(0),
			next_recover_fighttimes_timtstamp(0), battle_data_length(0), fight_record_head(0),
			fight_record_tail(0), fight_record_data_length(0), reach_score_timestamp(0u),
			last_season_rank(-1), reach_ranking_flag(0), fetch_ranking_reward_flag(0),
			season_final_rank(-1), season_initial_score(0), day_reward_status(0),
			season_timestamp(0u), hidden_server_idx(0), season_highest_rank(-1), 
			season_highest_dan_id(0), last_day_score(0)
		{
			memset(name, 0, sizeof(name));
			memset(battle_data_buff, 0, sizeof(battle_data_buff));
			memset(fight_record_data_buff, 0, sizeof(fight_record_data_buff));
			memset(today_challenge_list, 0, sizeof(today_challenge_list));
		}

		char change_state;
		int uid;
		int level;
		int profession;
		int avatar;
		GameName name;
		Appearance appearance;
		int capability;
		int score;
		int remain_fight_times;
		int target_uid_list[CLOUD_ARENA_OPPONENT_NUM];
		int today_buy_fighttimes_times;
		unsigned int next_recover_fighttimes_timtstamp;

		int battle_data_length;
		char battle_data_buff[sizeof(RoleBattleData) * 2];

		int fight_record_head;
		int fight_record_tail;
		
		int fight_record_data_length;
		char fight_record_data_buff[sizeof(CloudArenaFightRecord) * CLOUD_ARENA_USER_FIGHT_RECORD_NUM * 2];
	
		unsigned int reach_score_timestamp;
		int last_season_rank;

		int reach_ranking_flag;
		int fetch_ranking_reward_flag;

		int season_final_rank;
		int season_initial_score;
		int day_reward_status;

		CloudArenaAchievementParam achievement_param;

		int today_challenge_list[CLOUD_ARENA_TODAY_CHALLENGE_LIST_SIZE];

		unsigned int season_timestamp;
		int hidden_server_idx;

		int season_highest_rank;
		int season_highest_dan_id;
		int last_day_score;

		CloudArenaUserOtherParam other_param;
	};

	CloudArenaUserListParam() : next_id_from(0), count(0) {}

	long long next_id_from;
	int count;
	CloudArenaUserData user_list[CLOUD_ARENA_ONCE_OPER_USER_COUNT];
};

typedef char RoleBattleDataHex[sizeof(RoleBattleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleBattleDataHex) < 20000);

#pragma pack(pop)

#endif
