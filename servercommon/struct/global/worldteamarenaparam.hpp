#ifndef __WORLD_TEAM_ARENA_PARAM_HPP__
#define __WORLD_TEAM_ARENA_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/rankdef.hpp"
#include "common/tlvprotocol.h"

#pragma pack(push, 4)

static const int ACTIVITY_WORLD_TEAM_ARENA_RANK_TOP_NUM = 10;			//����ǰ10�ɲμ����µ�һ���

static const int ACTIVITY_WORLD_TEAM_ARENA_WEEK_NUM = 2;				//ÿ���µĵڶ���
static const int ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM = 5;			//�׼��ʱ�����ڼ�
static const int ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR = 19;			//�׼��ʱ�� 
static const int ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN = 50;				//�׼��ʱ�� 19:50
static const int ACTIVITY_WORLD_TEAM_ARENA_SIGN_UP_WEEK_DAY_NUM = 1;	//��ʼ����ʱ�����ڼ� (Ŀǰ��ʼΪ�����ͬ����һ0��)
static const int ACTIVITY_WORLD_TEAM_ARENA_OPEN_HOUR = 20;				//���ʼʱ�� 
static const int ACTIVITY_WORLD_TEAM_ARENA_OPEN_MIN = 0;				//���ʼʱ�� 20:00
static const int ACTIVITY_WORLD_TEAM_ARENA_STANDY_DUR_S = (ACTIVITY_WORLD_TEAM_ARENA_OPEN_HOUR - ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR) * SECOND_PER_HOUR +
	(ACTIVITY_WORLD_TEAM_ARENA_OPEN_MIN - ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN) * SECOND_PER_MIN;

static const int ACTIVITY_WORLD_TEAM_ARENA_RANK_NUM = 3;				//��Ҫ��ʾ��������Ϣ����
static const int ACTIVITY_WORLD_TEAM_ARENA_LOADING_FIGHT_TIME = 5;		//ս���ȴ�ʱ��
static const int ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM = 5;			//ս�ӳ�Ա����
static const int MAX_ACTIVITY_WORLD_TEAM_ARENA_DB_DATA_COUNT = 1000;	//���1000��(1000���Ѿ�5000��)
static const int ACTIVITY_WORLD_TEAM_ARENA_SAVE_DB_TIME = 60;			//ÿ60�뱣��һ��

static const int MAX_WORLD_TEAM_ARENA_SIGN_UP_TEAM_INFO_NUM = 100;		


UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_WEEK_NUM > 0 && ACTIVITY_WORLD_TEAM_ARENA_WEEK_NUM <= 4);
UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM > 0 && ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM <= 7);
UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR >= 0 && ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR < 24);
UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN >= 0 && ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN < 60);
UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_OPEN_HOUR >= 0 && ACTIVITY_WORLD_TEAM_ARENA_OPEN_HOUR < 24);
UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_OPEN_MIN >= 0 && ACTIVITY_WORLD_TEAM_ARENA_OPEN_MIN < 60);
UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_STANDY_DUR_S >= 0);

static const int ACTIVITY_WORLD_TEAM_ARENA_RANK_TYPE_LIST[ACTIVITY_WORLD_TEAM_ARENA_RANK_NUM] = 
{
	-1, PERSON_RANK_TYPE_LEVEL, PERSON_RANK_TYPE_CAPABILITY
};

enum MEMBER_DETERMINE_TYPE
{
	MEMBER_DETERMINE_TYPE_PENDING,
	MEMBER_DETERMINE_TYPE_YES,
};

struct PreTeamSignUpInfo
{
	PreTeamSignUpInfo() { this->Reset(); }

	void Reset()
	{
		memset(team_name, 0, sizeof(team_name));
		for (int i = 0; i < ARRAY_LENGTH(member_list); ++i)
		{
			member_list[i].Reset();
		}
	}
	struct MemberInfo
	{
		MemberInfo() {}

		void Reset()
		{
			role_id = 0;
			determine_flag = MEMBER_DETERMINE_TYPE_PENDING;
		}

		int role_id;
		int determine_flag;
	};

	GameName team_name;
	MemberInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
};

struct WorldTeamArenaQueryRankUserInfo
{
	WorldTeamArenaQueryRankUserInfo() : role_id(0), prof(0)
	{
		memset(rank_type_list, -1, sizeof(rank_type_list));
	}

	int role_id;
	int prof;
	int rank_type_list[ACTIVITY_WORLD_TEAM_ARENA_RANK_NUM];		// rank_type
};

struct WorldTeamArenaUserInfo
{
	WorldTeamArenaUserInfo() { this->Reset(); }

	void Reset()
	{
		role_uid = 0;
		profession = 0;
		level = 0;
		top_level = 0;
		avatar_type = 0;
		headshot_id = 0;
		fly_flag = 0;
		reserve_sh = 0;
		capablity = 0;

		memset(role_name, 0, sizeof(role_name));
	}

	int role_uid;
	int profession;
	short level;
	short top_level;				
	short avatar_type;				// ������������
	short headshot_id;				// ͷ��ID 
	short fly_flag;					// �����׶�
	short reserve_sh;
	int capablity;					// ��ɫ����

	GameName role_name;	
};

typedef char WorldTeamArenaUserInfoHex[sizeof(WorldTeamArenaUserInfo) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WorldTeamArenaUserInfoHex) < 256);

struct WorldTeamArenaRankItem
{
	WorldTeamArenaRankItem() { this->Reset(); }
	
	void Reset()
	{
		team_unique_id = 0;
		memset(team_name, 0, sizeof(team_name));
		total_score = 0;
		victory_times = 0;
		fight_times = 0;
		failures_times = 0;
		score_change_time = 0;
	}

	long long team_unique_id;			// ս��ID
	GameName team_name;					// ս����
	UInt32 total_score;					// �ܻ���
	UInt32 victory_times;				// ʤ������
	UInt32 fight_times;					// ս������
	UInt32 failures_times;				// ʧ�ܳ���
	UInt32 score_change_time;			// ��һ�λ��ֱ䶯��ʱ���
};

struct WorldTeamArenaTeamRankData;
struct WorldTeamArenaRankInfo
{
	WorldTeamArenaRankInfo() { this->Reset(); }
	WorldTeamArenaRankInfo(const WorldTeamArenaRankItem & data)
	{
		team_unique_id = data.team_unique_id;
		memcpy(team_name, data.team_name, sizeof(team_name));
		total_score = data.total_score;
		victory_times = data.victory_times;
		fight_times = data.fight_times;
	}

	void SetData(const WorldTeamArenaTeamRankData & data);
	void Reset()
	{
		team_unique_id = 0;
		memset(team_name, 0, sizeof(team_name));
		total_score = 0;
		victory_times = 0;
		fight_times = 0;
	}

	long long team_unique_id;			// ս��ID
	GameName team_name;					// ս����
	UInt32 total_score;					// �ܻ���
	UInt32 victory_times;				// ʤ������
	UInt32 fight_times;					// ս������
};

struct WorldTeamArenaUserSimpleInfo
{
	WorldTeamArenaUserSimpleInfo() {}

	void Reset()
	{
		role_id = 0;
		profession = 0;
	}

	int role_id;
	int profession;
};

struct WorldTeamArenaTeamSignUpData;
struct WorldTeamArenaTeamSignUpSimpleInfo
{
	WorldTeamArenaTeamSignUpSimpleInfo( ){ this->Reset(); }

	void SetData(const WorldTeamArenaTeamSignUpData & data);
	void Reset()
	{
		team_unique_id = 0;
		memset(team_name, 0, sizeof(team_name));
		total_capablity = 0;
		level = 0;
		top_level = 0;
		avatar_type = 0;
		headshot_id = 0;
		fly_flag = 0;
		reserve_sh = 0;
		for (int i = 0; i < ARRAY_ITEM_COUNT(member_list); i++)
		{
			member_list[i].Reset();
		}
	}

	long long team_unique_id;			//ս��ID
	GameName team_name;					//ս����
	long long total_capablity;			//������

	//�ӳ���Ҫ��ʾ����Ϣ��ȡ����
	short level;
	short top_level;
	short avatar_type;				// ������������
	short headshot_id;				// ͷ��ID 
	short fly_flag;					// �����׶�
	short reserve_sh;			

	WorldTeamArenaUserSimpleInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];	//ս�ӳ�Ա
};

struct WorldTeamArenaUserDetailedInfo
{
	WorldTeamArenaUserDetailedInfo() { this->Reset(); }

	void SetData(const WorldTeamArenaUserInfo & data);
	void Reset()
	{
		role_uid = 0;
		profession = 0;
		level = 0;
		top_level = 0;
		avatar_type = 0;
		headshot_id = 0;
		fly_flag = 0;
		reserve_sh = 0;
		capablity = 0;

		memset(role_name, 0, sizeof(role_name));
		memset(rank_list, -1, sizeof(rank_list));
	}

	int role_uid;
	int profession;
	short level;
	short top_level;
	short avatar_type;				// ������������
	short headshot_id;				// ͷ��ID 
	short fly_flag;					// �����׶�
	short reserve_sh;
	int capablity;					// ��ɫ����

	GameName role_name;
	int rank_list[ACTIVITY_WORLD_TEAM_ARENA_RANK_NUM];	
};

struct WorldTeamArenaTeamSignUpDetailedInfo
{
	WorldTeamArenaTeamSignUpDetailedInfo() { this->Reset(); }

	void Reset()
	{
		team_unique_id = 0;
		memset(team_name, 0, sizeof(team_name));

		for (int i = 0; i < ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM; i++)
		{
			member_list[i].Reset();
		}
	}

	long long team_unique_id;			//ս��ID
	GameName team_name;					//ս����
	WorldTeamArenaUserDetailedInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];		//ս�ӳ�Ա
};

struct WorldTeamArenaTeamSpecialInfo
{
	WorldTeamArenaTeamSpecialInfo() { this->Reset(); }

	void Reset()
	{
		role_uid = 0;
		memset(rank_list, -1, sizeof(rank_list));
	}

	int role_uid;
	int rank_list[ACTIVITY_WORLD_TEAM_ARENA_RANK_NUM];	
};

struct WorldTeamArenaRoleInfo
{
	WorldTeamArenaRoleInfo() { this->Reset(); }

	void Reset()
	{
		world_team_arena_rank = 0;
		title_id = 0;
		timestamp = 0;
	}

	int world_team_arena_rank;
	int title_id;
	unsigned int timestamp;
};

struct ActivityWorldTeamArenaTeamFightInfo
{
	ActivityWorldTeamArenaTeamFightInfo() { this->Reset(); }
	
	void Reset()
	{
		rank_posi = -1;
		memset(team_name, 0, sizeof(team_name));
		member_count = 0;
		for (int i = 0; i < ARRAY_ITEM_COUNT(member_list); ++i)
		{
			member_list[i].Reset();
		}
	}

	int rank_posi;
	GameName team_name;
	int member_count;
	WorldTeamArenaUserInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
};

struct ActivityWorldTeamArenaFightInfo
{
	ActivityWorldTeamArenaFightInfo() { this->Reset(); }

	void Reset()
	{
		start_fight_timestamp = 0;
		attacker_team.Reset();
		defender_team.Reset();
	}

	int start_fight_timestamp;
	ActivityWorldTeamArenaTeamFightInfo attacker_team;
	ActivityWorldTeamArenaTeamFightInfo defender_team;
};

struct WorldTeamArenaTopTeamInfo
{
	WorldTeamArenaTopTeamInfo() { this->Reset(); }
	
	void Reset()
	{
		team_unique_id = 0;
		memset(team_name, 0, sizeof(team_name));
		for (int i = 0; i < ARRAY_ITEM_COUNT(team_member_list); ++i)
		{
			team_member_list[i].Reset();
		}
	}

	long long team_unique_id;			//ս��ID
	GameName team_name;					//ս����
	WorldTeamArenaUserInfo team_member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
};

struct WorldTeamArenaServerTopTeamInfo
{
	WorldTeamArenaServerTopTeamInfo() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(team_list); ++i)
		{
			team_list[i].Reset();
		}
	}

	WorldTeamArenaTopTeamInfo team_list[ACTIVITY_WORLD_TEAM_ARENA_RANK_TOP_NUM];
};

#pragma pack(pop)

struct WorldTeamArenaTeamRankData;
struct WorldTeamArenaTeamSignUpData
{
	WorldTeamArenaTeamSignUpData() { this->Reset(); }
	void SetData(const WorldTeamArenaTeamRankData & info);

	bool IsVaild() const
	{
		return team_unique_id > 0;
	}

	void Reset()
	{
		change_state = 0;
		team_unique_id = 0;
		memset(team_name, 0, sizeof(team_name));

		for (int i = 0; i < ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM; i++)
		{
			member_list[i].Reset();
		}
	}

	char change_state;

	long long team_unique_id;			//ս��ID
	GameName team_name;					//ս����

	WorldTeamArenaUserInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];		//ս�ӳ�Ա
};

struct WorldTeamArenaTeamRankData
{
	WorldTeamArenaTeamRankData() { this->Reset(); }
	WorldTeamArenaTeamRankData(const WorldTeamArenaRankItem & data)
	{
		team_unique_id = data.team_unique_id;
		memcpy(team_name, data.team_name, sizeof(team_name));
		reserve_sh = 0;
		total_score = data.total_score;
		victory_times = data.victory_times;
		fight_times = data.fight_times;
		failures_times = data.failures_times;
		score_change_time = data.score_change_time;
	}
	bool IsVaild() const
	{
		return team_unique_id > 0;
	}

	void Reset()
	{
		change_state = 0;
		team_unique_id = 0;
		memset(team_name, 0, sizeof(team_name));

		hidden_server_idx = 0;
		reserve_sh = 0;
		total_score = 0;
		victory_times = 0;
		fight_times = 0;
		failures_times = 0;
		score_change_time = 0;
		for (int i = 0; i < ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM; i++)
		{
			member_list[i].Reset();
		}
	}

	char change_state;
	long long team_unique_id;			//ս��ID
	GameName team_name;					//ս����

	short hidden_server_idx;			//!< cross.xml �е����ط�IDX�ֶΣ��������ֿ������
	short reserve_sh;
	UInt32 total_score;					// �ܻ���
	UInt32 victory_times;				// ʤ������
	UInt32 fight_times;					// ս������
	UInt32 failures_times;				// ʧ�ܳ���
	UInt32 score_change_time;			// ��һ�λ��ֱ䶯��ʱ���
	WorldTeamArenaUserInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];		//ս�ӳ�Ա
};

struct WorldTeamArenaSignUpParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int count;
	WorldTeamArenaTeamSignUpData team_sign_up_list[MAX_ACTIVITY_WORLD_TEAM_ARENA_DB_DATA_COUNT];
};

struct WorldTeamArenaTeamRankParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int count;
	WorldTeamArenaTeamRankData team_rank_list[MAX_ACTIVITY_WORLD_TEAM_ARENA_DB_DATA_COUNT];
};

#endif