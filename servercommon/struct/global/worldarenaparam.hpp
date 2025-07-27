#ifndef __WORLD_ARENA_PARAM_HPP__
#define __WORLD_ARENA_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

enum WORLD_ARENA_STATUS
{
	WORLD_ARENA_STATUS_INVALID = 0,
	WORLD_ARENA_STATUS_SIGNUP,		//!< 1.�����׶� 1��-5��
	WORLD_ARENA_STATUS_SCHEDULE,	//!< 2.����׶� 6��0��
	WORLD_ARENA_STATUS_BATTLE,		//!< 3.�����׶� 6��-24��
	WORLD_ARENA_STATUS_CALCULATE,	//!< 4.����׶� 25��0��
	WORLD_ARENA_STATUS_REST,		//!< 5.�����׶� 25��-��ĩ
};

enum WORLD_ARENA_RESULT
{
	WORLD_ARENA_RESULT_INVALID = 0,
	WORLD_ARENA_RESULT_ATTACKER_WIN,	//!< 1. ����ʤ
	WORLD_ARENA_RESULT_DEFENDER_WIN,	//!< 2. �ط�ʤ
	WORLD_ARENA_RESULT_DRAW,			//!< 3. ƽ��
	WORLD_ARENA_RESULT_BOTH_GIVEUP,		//!< 4. ˫������Ȩ
	WORLD_ARENA_RESULT_MAX,
};

enum WORLD_ARENA_DAILY_REWARD_FLAG
{
	WORLD_ARENA_DAILY_REWARD_FLAG_UNFETCHABLE = 0,		//!< 0 ������ȡ
	WORLD_ARENA_DAILY_REWARD_FLAG_FETCHABLE = 1,		//!< 1 ����ȡ
	WORLD_ARENA_DAILY_REWARD_FLAG_ALREADY_FETCHED = 2,	//!< 2 ����ȡ
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

	short cur_status;							//!< ��ǰ״̬
	short season_index;							//!< ������
	unsigned int next_status_timestamp;			//!< �´�״̬�仯ʱ���
	unsigned int next_signup_status_timestamp;	//!< �´α����׶�ʱ���
	unsigned int next_schedule_status_timestamp;//!< �´η���׶�ʱ���
	unsigned int next_battle_status_timestamp;	//!< �´ο�ս�׶�ʱ���
	unsigned int next_calculate_status_timestamp;//!<�´ν���׶�ʱ���
	unsigned int next_rest_status_timestamp;	//!< �´������׶�ʱ���
	int robot_index_generator;					//!< ���ڻ�����UUID����

	unsigned int round_battle_start_timestamp;	//!< ����ս����ʼʱ���
	unsigned int next_round_battle_start_timestamp;	//!< ����ս����ʼʱ���
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
UNSTD_STATIC_CHECK(sizeof(WorldArenaSignUpInfo) * 2.5 < 512);  // ���ݱ������Protobuf���л�����BinToHex����ַ�����������2.5

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

	long long group_id;			//!< ���id
	long long schedule_id;		//!< ����ΨһID
	long long attacker_uuid;	//!< ������Ϣ
	long long defender_uuid;	//!< �ط���Ϣ
	short round_index;			//!< �������[0, 19)
	short fight_result;			//!< ��ս���
	unsigned int check_result_timestamp; //!< �����ʱ��� ��ʱ��ͼ���Ƿ���˽����δ�н������Ϊƽ��
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

	short win_num;		//!< ʤ����
	short lose_num;		//!< ʧ����
	short draw_num;		//!< ƽ����
	short giveup_num;	//!< ��Ȩ��
	int total_score;	//!< �ܻ���
	int win_box_reward_flag;	//!< ʤ��������ȡ���
	short today_giveup_notice;	//!< �Ƿ�Ʈ�ֽ������֪ͨ
	short total_battle_round;	//!< �ܻغ���
	char has_fetch_signup_reward;	//!< �Ƿ�����ȡ��������
	char daily_reward_fetch_flag;	//!< ��ȡÿ�ս������
	short reserve_sh;
};

UNSTD_STATIC_CHECK(sizeof(WorldArenaScoreInfo) * 2.5 < 256); // ���ݱ������Protobuf���л�����BinToHex����ַ�����������2.5

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
UNSTD_STATIC_CHECK(sizeof(WorldArenaUserScheduleInfo) * 2.5 < 512); // ���ݱ������Protobuf���л�����BinToHex����ַ�����������2.5

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

	WorldArenaSignUpInfo signup_info;	//!< �����ߵı�����Ϣ
	WorldArenaScoreInfo score_info;		//!< �����ߵĻ�����Ϣ	
	WorldArenaUserScheduleInfo schedule_info;

	long long reserve_ll[8];			//!< ������
	RoleBattleData battle_data;			//!< ���ڻ�����ս���������а�չʾ
};

UNSTD_STATIC_CHECK(sizeof(RoleBattleData) * 2.5 < 24576); // ���ݱ������Protobuf���л�����BinToHex����ַ�����������2.5

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