#ifndef __COLOSSEUM_DEF_HPP__
#define __COLOSSEUM_DEF_HPP__

#include "servercommon/serverdef.h"
#include "gameworld/gameworld/gamedef.h"
#include "servercommon/servercommon.h"
#include "struct/battle/battle_def.hpp"
#include "servercommon/protobuf/role_battle_data.pb.h"

#pragma pack(push, 4)

static const int CHALLENGE_FIELD_MAX_USER = 50000;						// �����Ҹ���
static const int CHALLENGE_FIELD_OPPONENT_MAX = 4;						// ���ָ���
static const int CHALLENGE_FIELD_TOP_NUM = 4;							// top 4
static const int CHALLENGE_FIELD_ONCE_OPER_USER_COUNT = 128;			// ������һ�β�����Ҹ���
static const int CHALLENGE_FIELD_INCREASE_STEP = 512;					// ����������б�����������
static const int CHALLENGE_FIELD_RANK_AWARD_MAX_NUM = 5;				// ���������б���

static const int CHALLENGE_FIELD_POOL_PLAYER_MAX_NUM = 1022;			// ���񱦿��ڵ�����������
static const int CHALLENGE_FIELD_POOL_PLAYER_LIST_LEN = sizeof(int) + sizeof(int) * CHALLENGE_FIELD_POOL_PLAYER_MAX_NUM;

const static int COLOSSEUM_ROBOT_SERVER_ID = 2047;						// �����˱��
static const int COLOSSEUM_IN_POOL_RANK_LIST_NUM = 50;

static const int COLOSSEUM_DEFENDER_BUFF_NUM = 3;

static const int INIT_ROBOT_NUM = 500;

enum InPoolRankRefreshReason
{
	REFRESH_TYPE_INIT = 0,		// �����б��ʼ��
	REFRESH_TYPE_PUSH = 1,		// �б�β������

	REFRESH_TYPE_MAX
};

enum COLOSSEUN_PARAM_TYPE
{
	COLOSSEUN_PARAM_TYPE_0 = 0,//�����ʷ����
	COLOSSEUN_PARAM_TYPE_1 = 1,//�ۻ���ս����
	COLOSSEUN_PARAM_TYPE_2 = 2,//�ۻ���սʤ������
	COLOSSEUN_PARAM_TYPE_3 = 3,//�ۻ�����սʤ������
	COLOSSEUN_PARAM_TYPE_4 = 4,//��ǰ��ʤ����
	COLOSSEUN_PARAM_TYPE_5 = 5,//�����ʤ����
	COLOSSEUN_PARAM_TYPE_6 = 6,//��������˺����
	COLOSSEUN_PARAM_TYPE_7 = 7,//սʤ���Լ����ָߵ����
	COLOSSEUN_PARAM_TYPE_8 = 8,//�ۼ�ս�ܴ���
	COLOSSEUN_PARAM_TYPE_9 = 9,//�ۼƸ������
	COLOSSEUN_PARAM_TYPE_10 = 10,//�ۼ�ʹ��ҩ�����
	COLOSSEUN_PARAM_TYPE_11 = 11,//ս������8�غϲ����ջ�ʤ
	COLOSSEUN_PARAM_TYPE_12 = 12,//����Ů�Ի��ʤ��
	COLOSSEUN_PARAM_TYPE_13 = 13,//�������Ի��ʤ��
	COLOSSEUN_PARAM_TYPE_14 = 14,//���Я�����ͻ���ʤ
	COLOSSEUN_PARAM_TYPE_15 = 15,//���Я������ʦ����ʤ
	COLOSSEUN_PARAM_TYPE_16 = 16,//���Я����ʦ����ʤ
	COLOSSEUN_PARAM_TYPE_17 = 17,//���Я����սʿ����ʤ
	COLOSSEUN_PARAM_TYPE_18 = 18,//���Я�������ֻ���ʤ
	COLOSSEUN_PARAM_TYPE_19 = 19,//���Я��ҽʦ����ʤ
	COLOSSEUN_PARAM_TYPE_20 = 20,//���Я����������ʤ
	COLOSSEUN_PARAM_TYPE_21 = 21,//���Я��ȭʦ����ʤ
	COLOSSEUN_PARAM_TYPE_MAX,
};

struct ColosseumFightStatistic // ս��ͳ����Ϣ �����
{
	ColosseumFightStatistic() : has_man_fight(0), has_woman_fight(0), has_start_fight(0),
		has_swordsman_fight(0), has_tiewei_fight(0), has_doctor_fight(0), has_lich_fight(0),
		has_berserk_fight(0), has_boxer_fight(0), has_arrower_fight(0), round_num(0),
		realive_times(0), use_medicine_times(0), total_damage_all(0LL)
	{}

	char has_man_fight;
	char has_woman_fight;
	char has_start_fight;//����ʦ
	char has_swordsman_fight;//����
	char has_tiewei_fight;//����
	char has_doctor_fight;//ҽ��
	char has_lich_fight;//��ҽ
	char has_berserk_fight;//��ս
	char has_boxer_fight;//ȭʦ
	char has_arrower_fight;//����
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
	short tips_times;		// ����ʾ����

	int history_highest_rank_for_reward;  // ��ʷ�������������������ʷ����������
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
	short avatar_type;		//!< ģ��
	short is_attacker;		//!< �����Ƿ�������
	short headshot_id;		//!< ͷ��ID
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
		ColosseumParam colosseum_param;//������
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