#ifndef __UNIVERSE_TEAM_ARENA_DEF_HPP__
#define __UNIVERSE_TEAM_ARENA_DEF_HPP__

#include <cstring>
#include "servercommon/serverdef.h"
#include <vector>
#include "servercommon/servercommon.h"

enum UTA_STATUS
{
	UTA_STATUS_INVALID = 0,
	UTA_STATUS_PREPARE = 1,			//!< 1 ׼���׶�
	UTA_STATUS_QUALIFICATION = 2,	//!< 2 �ʸ����׶�
	UTA_STATUS_MAIN = 3,			//!< 3 �����׶�
	UTA_STATUS_CALCULATE = 4,		//!< 4 ����׶�
	UTA_STATUS_REST = 5,			//!< 5 �����׶�
	UTA_STATUS_MAX
};

enum UTA_FIGHT_RESULT
{
	UTA_FIGHT_RESULT_NONE = 0,
	UTA_FIGHT_RESULT_ATK_WIN = 1,
	UTA_FIGHT_RESULT_DEF_WIN = 2,
	UTA_FIGHT_RESULT_WAITING = 3,	//!< �ѿ�ʼ���ȴ������
	UTA_FIGHT_RESULT_DRAW = 4,		//!< ƽ�� ���ʸ�������ƽ�֣�
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
	UTA_MAIN_RANK_NONE = 0,			//!< û����������
	UTA_MAIN_RANK_CHAMPION = 1,		//!< �ھ� 
	UTA_MAIN_RANK_RUNNER_UP = 2,	//!< �Ǿ�
	UTA_MAIN_RANK_TOP_2 = 3,		//!< ��ǿ
	UTA_MAIN_RANK_3RD = 4,			//!< ����
	UTA_MAIN_RANK_4TH = 5,			//!< ���
	UTA_MAIN_RANK_TOP_4 = 6,		//!< ��ǿ
	UTA_MAIN_RANK_5TH = 7,			//!< ������
	UTA_MAIN_RANK_6TH = 8,			//!< ������
	UTA_MAIN_RANK_7TH = 9,			//!< ������
	UTA_MAIN_RANK_8TH = 10,			//!< �ڰ���
	UTA_MAIN_RANK_TOP_8 = 11,		//!< ��ǿ
	UTA_MAIN_RANK_9TH = 12,			//!< �ھ���
	UTA_MAIN_RANK_10TH = 13,		//!< ��ʮ��
	UTA_MAIN_RANK_11TH = 14,		//!< ��ʮһ��
	UTA_MAIN_RANK_12TH = 15,		//!< ��ʮ����
	UTA_MAIN_RANK_13TH = 16,		//!< ��ʮ����
	UTA_MAIN_RANK_14TH = 17,		//!< ��ʮ����
	UTA_MAIN_RANK_15TH = 18,		//!< ��ʮ����
	UTA_MAIN_RANK_16TH = 19,		//!< ��ʮ����
	UTA_MAIN_RANK_TOP_16 = 20,		//!< ʮ��ǿ
	UTA_MAIN_RANK_17TH = 21,		//!< ��ʮ����
	UTA_MAIN_RANK_18TH = 22,		//!< ��ʮ����
	UTA_MAIN_RANK_19TH = 23,		//!< ��ʮ����
	UTA_MAIN_RANK_20TH = 24,		//!< �ڶ�ʮ��
	UTA_MAIN_RANK_21TH = 25,		//!< �ڶ�ʮһ��
	UTA_MAIN_RANK_22TH = 26,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_23TH = 27,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_24TH = 28,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_25TH = 29,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_26TH = 30,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_27TH = 31,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_28TH = 32,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_29TH = 33,		//!< �ڶ�ʮ����
	UTA_MAIN_RANK_30TH = 34,		//!< ����ʮ��
	UTA_MAIN_RANK_31TH = 35,		//!< ����ʮһ��
	UTA_MAIN_RANK_32TH = 36,		//!< ����ʮ����
	UTA_MAIN_RANK_TOP_32 = 37,		//!< ��ʮ��ǿ
	UTA_MAIN_RANK_MAX,
};

enum UTA_PUT_REASON_TYPE
{
	UTA_PUT_REASON_TYPE_NONE = 0,
	UTA_PUT_REASON_TYPE_WIN_BOX_REWARD = 1,
	UTA_PUT_REASON_TYPE_WIN_BOX_REISSUE = 2,   // ʤ�����䲹��
	UTA_PUT_REASON_TYPE_QUALIFICATION_JOIN_REWARD = 3,	// �ʸ���ÿ�ղ��뽱��
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
	short avatar_type;				// ������������
	short headshot_id;				// ͷ��ID 
	short fly_flag;					// �����׶�
	short today_get_reward_num;		// ���콱����ô���
	int capablity;					// ��ɫ����
	GameName role_name;
	Appearance appearance;
	int win_box_fetch_flag;			// ʤ��������ȡ��ǣ������ƣ�
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
	int season_idx;						// ������
	long long team_id_in_hidden_server;	// �ڿ���Ķ���ID
	long long team_unique_id;		// �ڴ�������Ķ���ΨһID
	GameName team_name;

	// [-1,-1)����û�������ʸ�, [0,32) ��ʾ32ǿ, [16,32)��ʾ32ǿ�����飬[0,16)��ʾ16ǿ����32ǿʤ����)			
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

	int season_idx;				//!< ������
	char type;					//!< ��������  0:�ʸ���  1:����
	char fight_result;			//!< ������� 0:δ��  1:����ʤ   2:�ط�ʤ ��û��ƽ�֣�����ֳ�ʤ����
	char round_idx;				//!< �ִ� ����typeһ��ȷ����ʼʱ�䣩
	char match_idx;				//!< �������Σ��ʸ���û�еģ� 
	long long schedule_id;		//!< ����ID
	long long atk_team_unique_id;	//!< ��������ΨһID
	long long def_team_unique_id;	//!< �ط�����ΨһID
	char is_win_group;			//!< �Ƿ�ʤ���飨������ʱ��Ч��
	long long video_file_id;	//!< ����¼��
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