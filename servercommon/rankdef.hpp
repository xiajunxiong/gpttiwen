#ifndef __RANK_DEF_HPP__
#define __RANK_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/guilddef.hpp"
#include "attributedef.hpp"
#include "gameworld/gameworld/item/itembase.h"
#include "gameworld/gameworld/other/attributelist.hpp"

static const int RANK_DATA_SAVE_TO_DB_TIME = 60;			 // ���а����ݱ��浽DB��ʱ����

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const int PERSON_RANK_LEVEL_LIMIT = 20;				 // �ϰ�ȼ�����
static const int PERSON_RANK_UPDATE_TO_GLOBAL_TIME = 1 * 60; // ͬ�������а��ʱ���� 
static const int PERSON_RANK_SNAPSHOT_INTERVAL = 2 * 60;	 // �������а����ʱ���� ��

enum PROVISIONAL_RANK
{
	PROVISIONAL_RANK_TYPE_RELIC_LOOTING = 1,		// �ż��ᱦ
	PROVISIONAL_RANK_TYPE_MAX,
};

// ���˰�����  
enum PERSON_RANK_TYPE
{
	PERSON_RANK_TYPE_LEVEL = 0,				// �ȼ���

	REASON_RANK_TYPE_PROF_LEVEL_1 = 1,			// ְҵ�ȼ����ʿ
	REASON_RANK_TYPE_PROF_LEVEL_2 = 2,			// ְҵ�ȼ���ʿ
	REASON_RANK_TYPE_PROF_LEVEL_3 = 3,			// ְҵ�ȼ���ս����ʿ
	REASON_RANK_TYPE_PROF_LEVEL_4 = 4,			// ְҵ�ȼ�����ʿ
	REASON_RANK_TYPE_PROF_LEVEL_5 = 5,			// ְҵ�ȼ��񹭼���
	REASON_RANK_TYPE_PROF_LEVEL_6 = 6,			// ְҵ�ȼ���ħ��ʦ
	REASON_RANK_TYPE_PROF_LEVEL_7 = 7,			// ְҵ�ȼ��񴫽�ʿ
	REASON_RANK_TYPE_PROF_LEVEL_8 = 8,			// ְҵ�ȼ�����ʦ

	PERSON_RANK_TYPE_CAPABILITY = 9,			// ս����(��ʱ�޸�Ϊ�ۺ�ս����, ��ɫ + ��ս��� + ��ս����)

	PERSON_RANK_TYPE_PROF_CAPABILITY_BEGIN,
	PERSON_RANK_TYPE_PROF_CAPABILITY_1 = 10,	// ְҵս�����ʿ
	PERSON_RANK_TYPE_PROF_CAPABILITY_2 = 11,	// ְҵս����ʿ
	PERSON_RANK_TYPE_PROF_CAPABILITY_3 = 12,	// ְҵս����ս����ʿ
	PERSON_RANK_TYPE_PROF_CAPABILITY_4 = 13,	// ְҵս������ʿ
	PERSON_RANK_TYPE_PROF_CAPABILITY_5 = 14,	// ְҵս���񹭼���
	PERSON_RANK_TYPE_PROF_CAPABILITY_6 = 15,	// ְҵս����ħ��ʦ
	PERSON_RANK_TYPE_PROF_CAPABILITY_7 = 16,	// ְҵս���񴫽�ʿ
	PERSON_RANK_TYPE_PROF_CAPABILITY_8 = 17,	// ְҵս������ʦ
	PERSON_RANK_TYPE_PROF_CAPABILITY_END,

	PERSON_RANK_TYPE_HUASHEN_CAPABILITY = 18,	// �����ɫս��

	PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP = 19,	// ��ɫս��+�������ս��
	PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE = 20,	// ��Ԩ��¼
	PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK = 21,	//�ֻ�����
	PERSON_RANK_TYPE_CHIVALROUS	= 22,			// ����ֵ
	PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY = 23,// ��ڤ����

	PERSON_RANK_TYPE_BIG_DIPPER_1 = 24,	// �Ǿ�1
	PERSON_RANK_TYPE_BIG_DIPPER_2 = 25,	// �Ǿ�2
	PERSON_RANK_TYPE_BIG_DIPPER_3 = 26,	// �Ǿ�3
	PERSON_RANK_TYPE_BIG_DIPPER_4 = 27,	// �Ǿ�4
	PERSON_RANK_TYPE_BIG_DIPPER_5 = 28,	// �Ǿ�5
	PERSON_RANK_TYPE_BIG_DIPPER_6 = 29,	// �Ǿ�6
	PERSON_RANK_TYPE_BIG_DIPPER_7 = 30,	// �Ǿ�7
	PERSON_RANK_TYPE_BIG_DIPPER_8 = 31,	// �۾�

	PERSON_RANK_TYPE_ROLE_CAPABILITY = 32,		// ��ɫս����
	PERSON_RANK_TYPE_FOUR_MAX_PARTNER = 33,		// �ĸ����ս�����
	PERSON_RANK_TYPE_EQUIPMENT_CAP = 34,		// ����װ�����ֱ�ƴ
	PERSON_RANK_TYPE_PET_MAX_CAPABILITY = 35,	// ����ս����(ֻȡ������ս������)
	PERSON_RANK_TYPE_ENDLESS_TOWER = 36,		// �޾�֮��
	PERSON_RANK_TYPE_FALLEN_GOD = 37,	//����֮��
	PERSON_RANK_TYPE_MEILI_QIXI = 38,	// ������
	PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE = 39,// ߱��֮��(������)

	PERSON_RANK_TYPE_MAX
};
UNSTD_STATIC_CHECK(900 == PROF_TYPE_MAX);

static const char PERSON_RANK_TYPE_NAME[PERSON_RANK_TYPE_MAX][25] = { "RANK_LEVEL", "GLADIATOR_LEVEL", "SWORDSMAN_LEVEL", "OMAHAWK_LEVEL", "KNIGHT_LEVEL",
																	"ARCHER_LEVEL", "MAGICIAN_LEVEL", "MISSIONARY_LEVEL", "WIZARD_LEVEL", "CAPABILITY_RANK",
																	"GLADIATOR_CAPABILITY", "SWORDSMAN_CAPABILITY", "OMAHAWK_CAPABILITY", "KNIGHT_CAPABILITY",
																	"ARCHER_CAPABILITY","MAGICIAN_CAPABILITY", "MISSIONARY_CAPABILITY", "WIZARD_CAPABILITY",
																	"HUASHEN_CAPABILITY", "ROLE_AND_MAX_PET_CAP", "DREAM_NOTES_CHALLENGE","LUN_HUI_WANG_CHUAN_RANK",
																	"CHIVALROUS","NETHER_WORLD_PURGATORY","BIG_DIPPER_1","BIG_DIPPER_2" ,"BIG_DIPPER_3" ,"BIG_DIPPER_4"
																	,"BIG_DIPPER_5" ,"BIG_DIPPER_6" ,"BIG_DIPPER_7" ,"BIG_DIPPER_8", "ROLE_CAPABILITY", "FOUR_MAX_PARTNER",
																	"EQUIPMENT_CAPABILITY", "PET_MAX_CAPABILITY", "ENDLESS_TOWER", "FALLEN_GOD" ,"MEILI_QIXI"};

static const unsigned int CROSS_PERSON_RANK_MAX_SIZE = 10000;		// �ϰ�����10000 
static const unsigned int PERSON_RANK_MAX_SIZE = 3000;				// �ϰ�����3000 
static const unsigned int CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT = 100;		//����Э��ͬ���������Ԫ������

static const int PERSON_RANK_TYPE_SYNC_INTERVAL = 5;
//UNSTD_STATIC_CHECK(PERSON_RANK_TYPE_SYNC_INTERVAL * PERSON_RANK_TYPE_MAX < PERSON_RANK_SNAPSHOT_INTERVAL);
UNSTD_STATIC_CHECK(PERSON_RANK_TYPE_MAX > 0);

inline int GetRankTypeByProfType(int prof_type)
{
	int base_prof = GetBaseProfession(prof_type);
	int prof_rank_type = -1;
	switch (base_prof)
	{
	case PROF_TYPE_GLADIATOR:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_1;
	}
	break;
	case PROF_TYPE_SWORDSMAN:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_2;
	}
	break;
	case PROF_TYPE_AXE_FIGHTER:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_3;
	}
	break;
	case PROF_TYPE_KNIGHT:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_4;
	}
	break;
	case PROF_TYPE_ARCHER:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_5;
	}
	break;
	case PROF_TYPE_MAGICIAN:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_6;
	}
	break;
	case PROF_TYPE_MISSIONARY:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_7;
	}
	break;
	case PROF_TYPE_WIZZARD:
	{
		prof_rank_type = PERSON_RANK_TYPE_PROF_CAPABILITY_8;
	}
	break;
	default:
		break;
	}
	return prof_rank_type;
}

inline bool IsLevelRankType(int rank_type)
{
	switch (rank_type)
	{
	case	PERSON_RANK_TYPE_LEVEL:
	case 	REASON_RANK_TYPE_PROF_LEVEL_1:
	case 	REASON_RANK_TYPE_PROF_LEVEL_2:
	case 	REASON_RANK_TYPE_PROF_LEVEL_3:
	case 	REASON_RANK_TYPE_PROF_LEVEL_4:
	case 	REASON_RANK_TYPE_PROF_LEVEL_5:
	case 	REASON_RANK_TYPE_PROF_LEVEL_6:
	case 	REASON_RANK_TYPE_PROF_LEVEL_7:
	case 	REASON_RANK_TYPE_PROF_LEVEL_8:
	{
		return true;
	}
	break;
	default:
		break;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const int PET_RANK_UPDATE_TO_GLOBAL_TIME = 1 * 60; // ͬ�������а��ʱ���� 
static const int PET_RANK_SNAPSHOT_INTERVAL = 60;		 // �������а����ʱ���� ��
enum PET_RANK_TYPE
{
	PET_RANK_TYPE_CAPABILITY,								// ս����
	PET_RANK_TYPE_HUASHEN_CAPABILITY,						// �������ս��

	PET_RANK_TYPE_MAX,
};
static const int CROSS_PET_RANK_MAX_SIZE = 10000;			// ����ϰ������10000
static const int PET_RANK_MAX_SIZE = 3000;					// �ϰ������3000
static const int CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT = 100;	// ����Э��ͬ���������Ԫ������

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum PARTNER_RANK_TYPE
{
	PARTNER_RANK_TYPE_CAPABILITY,							// ���ս����

	PARTNER_RANK_TYPE_MAX,
};
static const int PARTNER_RANK_MAX_SIZE = 10;				// �ϰ�����

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���Ű�����  
enum GUILD_RANK_TYPE
{
	GUILD_RANK_TYPE_LEVEL = 0,								// ���ŵȼ���
	GUILD_RANK_TYPE_WEEK_ACTIVE = 1,						// �����ܻ�Ծ��

	GUILD_RANK_TYPE_MAX,
};

static const int CROSS_GUILD_RANK_SYNC_PAGE_ELEMENT_COUNT = 100;	// ����������а񵥴�ͬ��Ԫ�������� 100
static const int GUILD_RANK_MAX_SIZE = 100;							// �ϰ���� ���100��
static const int GUILD_RANK_SNAPSHOT_INTERVAL = 60;					// �������а����ʱ���� ��
static const int CROSS_GUILD_ACTIVE_RANK_REWARD_COUNT = 10;			// ������ɻ�Ծ�����а�ȡǰX��
static const int GUILD_ACTIVE_PERSON_RANK_REWARD_COUNT = 20;		// ���ɸ��˻�Ծ�����а�ȡǰX��

struct GuildRankItem
{
	GuildRankItem()
	{
		this->Reset();
	}

	void* operator new(size_t c);
	void operator delete(void* m);

	bool Invalid() const { return INVALID_GUILD_ID == guild_id; }
	void Reset()
	{
		guild_id = INVALID_GUILD_ID;
		memset(guild_name, 0, sizeof(guild_name));
		memset(guild_banner, 0, sizeof(guild_banner));

		memset(tuan_zhang_name, 0, sizeof(tuan_zhang_name));
		guild_level = 0;
		camp = 0;
		member_count = 0;
		max_member_count = 0;
		rank_type = 0;

		rank_val = 0;
	}

	UniqueServerID server_id;		// ������ID
	int guild_id;					// ����id

	GameName guild_name;			// ������
	GuildBanner guild_banner;		// ���

	UserID tuan_zhang_user_id;
	GameName tuan_zhang_name;

	short guild_level;
	short camp;
	short member_count;
	short max_member_count;
	int rank_type;

	long long rank_val;				// �������а�ľ�����ֵ
	
	// ֻ��Ϊ����ʹ�� , ֻ��֤�˲�������
	bool operator==(const GuildRankItem & _a) const
	{
		if (server_id == _a.server_id && guild_id == _a.guild_id && rank_val == _a.rank_val )
			return true;

		return false;
	}

	bool operator< (const GuildRankItem & _a) const
	{
		if (rank_val == _a.rank_val)
		{
			return guild_id < _a.guild_id;
		}
		return rank_val > _a.rank_val;
	}

	bool operator> (const GuildRankItem & _a) const
	{
		return !(*this < _a) && ! (*this == _a);
	}

	bool operator<= (const GuildRankItem & _a) const
	{
		return *this < _a || *this == _a;
	}

	static bool RankCompare(const GuildRankItem & _a, const GuildRankItem & _b)
	{
		if (_a == _b)
		{
			return true;
		}

		return _a < _b;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ��Ӱ�����
enum TEAM_RANK_TYPE
{
	TEAM_RANK_TYPE_MIN = 0,
	TEAM_RANK_TYPE_FALLEN_GOD = 1,	//����֮��
	TEAM_RANK_TYPE_MAX
};

static const int TEAM_RANK_MAX_SIZE = 120;					// �ϰ��¼�� ���20��

static const int TEAM_RANK_SNAPSHOT_INTERVAL = 60;			// ������а����ʱ���� ��

static const int CHIVALROUS_RANK_LIST_NUM = 100;
static const int MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM = 100;	//���Ӣ��¼-������������seq��������


static const int FALLEN_GOD_REWARD_MEMBER_LIST_NUM = 10;//����֮�����н���Top10
static const int MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM = 100;//����֮�����н���Max

enum CHIVALROUS_RANK_TYPE				// ����ֵ���а�
{
	CHIVALROUS_RANK_TYPE_SERVER = 0,	// �����
	CHIVALROUS_RANK_TYPE_GUILD = 1,		// �����
	CHIVALROUS_RANK_TYPE_FRIEND = 2,	// ���Ѱ�
	CHIVALROUS_RANK_TYPE_MAX
};

struct ChivalrousRankInfo
{
	ChivalrousRankInfo() : uid(0), avatar_type(0), headshot_id(0), week_chivalrous(0)
	{
		memset(user_name, 0, sizeof(user_name));
	}

	int uid;
	GameName user_name;
	short avatar_type;
	short headshot_id;
	int prof;
	int week_chivalrous;		// ��������ֵ
};

#pragma pack(push, 4)

struct TeamRankMemberInfo									// ������ж�Ա��Ϣ ������Э���� ע�����
{
	TeamRankMemberInfo() : uid(0) { memset(user_name, 0, sizeof(user_name)); }

	void Reset() { uid = 0; memset(user_name, 0, sizeof(user_name)); }

	int uid;												// �û�ID
	GameName user_name;										// �û���
};

// ��һ�����Ϣ
struct RoleBaseInfo
{
	RoleBaseInfo()
	{
		this->Reset();
	}

	bool IsVaild() const
	{
		return uid > 0;
	}

	void Reset() 
	{
		uid = -1;
		prof = 0;
		role_level = 0;
		avatar_type = AVATAR_TYPE_INVALID;
		exp = 0;
		capability = 0;
		headshot_id = 0;
		top_level = 0;
		reserve_sh = 0;
		role_fly_flag = 0;
		memset(user_name, 0, sizeof(user_name));
	}

	int uid;
	GameName user_name;
	int prof;
	int role_level;
	int avatar_type;
	long long exp;
	int capability;
	int headshot_id;
	int top_level;
	short reserve_sh;
	short role_fly_flag;
};
typedef char RoleBaseInfoHex[sizeof(RoleBaseInfo) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleBaseInfoHex) < 256);

struct RoleSimapleInfo
{
	RoleSimapleInfo() { this->Reset(); }
	RoleSimapleInfo(const RoleBaseInfo & _role_base_info)
	{
		role_id = _role_base_info.uid;
		prof = _role_base_info.prof;
		avatar_type = _role_base_info.avatar_type;
		headshot_id = _role_base_info.headshot_id;
		F_STRNCPY(role_name, _role_base_info.user_name, sizeof(role_name));
		top_level = _role_base_info.top_level;
		reserve_sh = _role_base_info.reserve_sh;
		role_fly_flag = _role_base_info.role_fly_flag;
	}

	void Reset()
	{
		role_id = 0;
		memset(role_name, 0, sizeof(role_name));
		prof = 0;
		avatar_type = 0;
		headshot_id = 0;
		top_level = 0;
		reserve_sh = 0;
		role_fly_flag = 0;
	}

	int role_id;
	GameName role_name;
	int prof;
	short avatar_type;
	short headshot_id;
	int top_level;
	short reserve_sh;
	short role_fly_flag;
};

struct PetSimpleInfo			//���������б��������
{
	PetSimpleInfo() { this->Reset(); }

	void Reset()
	{
		pet_id = 0;
		str_level = 0;
		pet_level = 0;
		skin_id = 0;
		pet_unique_item_id = 0;
		reserve_sh = 0;
		fei_sheng_times = 0;
		lian_yao_pet_id = 0;
		memset(pet_name, 0, sizeof(pet_name));
	}

	int pet_id;
	unsigned short str_level;
	unsigned short pet_level;
	int skin_id;
	unsigned long long pet_unique_item_id;
	short reserve_sh;
	short fei_sheng_times;			// �����������
	int lian_yao_pet_id;			// ��������id
	GameName pet_name;
};

struct PersonRankItem
{
	PersonRankItem()
	{
		this->Reset();
	}

	void* operator new(size_t c);
	void operator delete(void* m);

	bool Invalid() const { return -1 == rank_type; }
	void Reset() 
	{
		change_state = -1;
		index = 0;
		role_info.Reset();

		rank_type = -1;
		rank_value = 0;
		reserve_int = 0;
		flexible_int = 0;
		flexible_ll = 0;
	}

	char change_state;
	int index;														// ���а����� ����,�����ȡ����,��ʹ��GetIndex

	RoleBaseInfo role_info;

	int rank_type;
	long long rank_value;
	int reserve_int;												// ����
	int flexible_int;
	long long flexible_ll;

	bool operator==(const PersonRankItem & _a) const
	{
		if (rank_type == _a.rank_type)
		{
			if (IsLevelRankType(rank_type))
			{
				if (rank_value == _a.rank_value && role_info.uid == _a.role_info.uid &&
					flexible_ll == _a.flexible_ll && role_info.top_level == _a.role_info.top_level)
				{
					return true;
				}
			}
			else
			{
				if (rank_value == _a.rank_value && role_info.uid == _a.role_info.uid &&
					flexible_ll == _a.flexible_ll)
				{
					return true;
				}
			}
		}

		/*if (rank_type == _a.rank_type && rank_value == _a.rank_value && role_info.uid == _a.role_info.uid &&
			flexible_ll == _a.flexible_ll)
			return true;*/
		return false;
	}
	bool operator< (const PersonRankItem & _a) const
	{
		if (rank_value == _a.rank_value)
		{
			if (IsLevelRankType(rank_type))
			{
				if (role_info.top_level != _a.role_info.top_level)
				{
					return role_info.top_level < _a.role_info.top_level;
				}
			}
			return flexible_ll < _a.flexible_ll;
		}
		return rank_value < _a.rank_value;
	}

	bool operator<= (const PersonRankItem & _a) const
	{
		return *this < _a || *this == _a;
	}

	bool operator> (const PersonRankItem & _a) const
	{
		if (rank_value == _a.rank_value)
		{
			if (IsLevelRankType(rank_type))
			{
				if (role_info.top_level != _a.role_info.top_level)
				{
					return role_info.top_level > _a.role_info.top_level;
				}
			}
			return flexible_ll < _a.flexible_ll;
		}
		return rank_value > _a.rank_value;
	}

	bool operator>= (const PersonRankItem & _a) const
	{
		return *this > _a || *this == _a;
	}

	// ע�� , �޸��ж�����ʱ,���뱣֤�ж��������Ա�֤���ݵ�Ψһ��
	// ������ܻ���ֶ�ʧ�������������ظ�������δ֪����
	static bool RankCompare(const PersonRankItem & _a, const PersonRankItem & _b)
	{
		if (_a == _b)
		{
			return true;
		}

		if (_a.rank_value != _b.rank_value)
		{
			return _a.rank_value >= _b.rank_value;
		}
		else if (_a.rank_type == _b.rank_type)
		{
			if (IsLevelRankType(_a.rank_type))
			{
				if (_a.role_info.top_level != _b.role_info.top_level)
				{
					return _a.role_info.top_level >= _b.role_info.top_level;
				}
				else if (_a.flexible_ll != _b.flexible_ll)
				{
					return _a.flexible_ll > _b.flexible_ll;
				}
			}
			else if(_a.flexible_int == _b.flexible_int)
			{
				if (_a.flexible_ll != _b.flexible_ll)
				{
					switch (_a.rank_type)
					{
					case	PERSON_RANK_TYPE_CAPABILITY:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_1:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_2:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_3:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_4:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_5:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_6:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_7:
					case 	PERSON_RANK_TYPE_PROF_CAPABILITY_8:
					case 	PERSON_RANK_TYPE_HUASHEN_CAPABILITY:
					case 	PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP:
					case	PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE:
					case	PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK:
					case    PERSON_RANK_TYPE_CHIVALROUS:
					case	PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY:
					case	PERSON_RANK_TYPE_BIG_DIPPER_1:
					case	PERSON_RANK_TYPE_BIG_DIPPER_2:
					case	PERSON_RANK_TYPE_BIG_DIPPER_3:
					case	PERSON_RANK_TYPE_BIG_DIPPER_4:
					case	PERSON_RANK_TYPE_BIG_DIPPER_5:
					case	PERSON_RANK_TYPE_BIG_DIPPER_6:
					case	PERSON_RANK_TYPE_BIG_DIPPER_7:
					case	PERSON_RANK_TYPE_BIG_DIPPER_8:
					case	PERSON_RANK_TYPE_ROLE_CAPABILITY:
					case	PERSON_RANK_TYPE_FOUR_MAX_PARTNER:
					case	PERSON_RANK_TYPE_EQUIPMENT_CAP:
					case	PERSON_RANK_TYPE_PET_MAX_CAPABILITY:
					case	PERSON_RANK_TYPE_ENDLESS_TOWER:
					case	PERSON_RANK_TYPE_FALLEN_GOD:
					case	PERSON_RANK_TYPE_MEILI_QIXI:
					case	PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE:
						{
							return _a.flexible_ll < _b.flexible_ll;
						}
					default:
						break;
					}
				}
			}
			else
			{
				return _a.flexible_int > _b.flexible_int;
			}
		}

		/*else if (_a.flexible_ll != _b.flexible_ll)
		{
			if (_a.rank_type == _b.rank_type)
			{
				switch (_a.rank_type)
				{
				case	PERSON_RANK_TYPE_CAPABILITY:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_1:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_2:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_3:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_4:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_5:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_6:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_7:
				case 	PERSON_RANK_TYPE_PROF_CAPABILITY_8:
				case 	PERSON_RANK_TYPE_HUASHEN_CAPABILITY:
				case 	PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP:
				case	PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE:
				case	PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK:
				case	PERSON_RANK_TYPE_CHIVALROUS:
					{
						return _a.flexible_ll <= _b.flexible_ll;
					}
					break;
				case	PERSON_RANK_TYPE_LEVEL:
				case 	REASON_RANK_TYPE_PROF_LEVEL_1:
				case 	REASON_RANK_TYPE_PROF_LEVEL_2:
				case 	REASON_RANK_TYPE_PROF_LEVEL_3:
				case 	REASON_RANK_TYPE_PROF_LEVEL_4:
				case 	REASON_RANK_TYPE_PROF_LEVEL_5:
				case 	REASON_RANK_TYPE_PROF_LEVEL_6:
				case 	REASON_RANK_TYPE_PROF_LEVEL_7:
				case 	REASON_RANK_TYPE_PROF_LEVEL_8:
					{
						return _a.flexible_ll >= _b.flexible_ll;
					}
				default:
					break;
				}
			}
		}*/

		return _a.role_info.uid < _b.role_info.uid;
	}
};

/*
struct PetDetailInfo
{
	// ���а����в���
	int				pet_index;

	int				pet_id;
	GameName		name;
	// ǿ���ȼ�
	unsigned short	str_level;
	// �ȼ�&����
	unsigned short	level;
	long long		exp;
	long long       add_exp;		// ս�������������ӵľ���

									// ��ǰ��Ѫ��������
	int				hp;
	int				mp;

	// �����б� ���ܵȼ�*10000 + ����id
	int	exclusive_skill;				//ר������
	int	exclusive_passive_skill;		//ר������

										// �츳�б�
	PetPassiveItemInfo	passive_list[PET_PASSIVE_MAX_COUNT];
	// ϴ�������츳�б�
	PetPassiveItemInfo	new_passive_list[PET_PASSIVE_MAX_COUNT];

	// ����
	unsigned short	growth[ADD_POINT_TYPE_MAX];
	// ϴ����������
	unsigned short	new_growth[ADD_POINT_TYPE_MAX];

	int comprehend_gift_num;		//�����ܵĸ�����ÿ������ֻ������2������
	int comprehend_passive_value;		//�����ܵ�ֵ

										//0ΪĬ��û������  1 Ϊ������
	unsigned char		lock;
	unsigned char		refined_flag;		//ϴ����ʲô��־
											// �ѷ������Ե�
	unsigned short	attr[ADD_POINT_TYPE_MAX];

	// ����Ѫ��������
	int				max_hp;
	int				max_mp;

	short pet_auto_move_type;
	short pet_auto_skill_id;
	short pet_auto_skill_level;
	char guide_pet_flag;
	char reserved_ch;
};*/
UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5)
UNSTD_STATIC_CHECK(PET_PASSIVE_MAX_COUNT == 20);

typedef char PetDetailInfoHex[sizeof(PetPro) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetDetailInfoHex) < 2048);

struct PetRankKey
{
	PetRankKey() : role_id(0), pet_unique_item_id(0) {}
	PetRankKey(int _role_id, unsigned long long  _pet_unique_item_id) : role_id(_role_id), pet_unique_item_id(_pet_unique_item_id) {}

	int role_id;
	unsigned long long pet_unique_item_id;

	bool operator<(const PetRankKey& right) const
	{
		if (role_id == right.role_id)
		{
			return pet_unique_item_id < right.pet_unique_item_id;
		}

		return role_id < right.role_id;
	}
};


struct PetRankItem
{
	PetRankItem()
		: str_buff_level(0), rank_type(-1), rank_value(0), reserve_int(0), flexible_int(0), flexible_ll(0) {}

	void* operator new(size_t c);
	void operator delete(void* m);

	bool Invalid() const { return -1 == rank_type; }
	void Reset() { rank_type = -1; }

	char change_state;
	int index;

	int str_buff_level;					// �����ȼ�
	RoleBaseInfo master_info;			// ������Ϣ
	PetPro pet_param;					// �����ĳ�����Ϣ
	GodPrintListPro pet_god_print;		// ��ӡ��װ

	//��������
	AttributeList pet_attr;

	int rank_type;
	long long rank_value;
	int reserve_int;							// ����
	int flexible_int;
	long long flexible_ll;

	bool operator==(const PetRankItem & _a) const
	{
		if (rank_type == _a.rank_type &&rank_value == _a.rank_value &&  master_info.uid == _a.master_info.uid &&
			pet_param.pet_unique_id == _a.pet_param.pet_unique_id &&
			flexible_ll == _a.flexible_ll)
			return true;
		return false;
	}
	bool operator< (const PetRankItem & _a) const
	{
		if (rank_value == _a.rank_value)
		{
			return flexible_ll > _a.flexible_ll;
		}
		return rank_value < _a.rank_value;
	}

	bool operator<= (const PetRankItem & _a) const
	{
		return *this < _a || *this == _a ;
	}

	bool operator> (const PetRankItem & _a) const
	{
		if (rank_value == _a.rank_value)
		{
			return flexible_ll < _a.flexible_ll;
		}
		return rank_value > _a.rank_value;
	}

	bool operator>= (const PetRankItem & _a) const
	{
		return *this > _a || *this == _a;
	}

	static bool RankCompare(const PetRankItem & _a, const PetRankItem & _b)
	{
		if (_a == _b)
		{
			return true;
		}

		if (_a.rank_value != _b.rank_value)
		{
			return _a.rank_value > _b.rank_value;
		}
		else if (_a.flexible_ll != _b.flexible_ll)
		{
			return _a.flexible_ll < _b.flexible_ll;
		}

		PetRankKey a_key(_a.master_info.uid, _a.pet_param.pet_unique_id);
		PetRankKey b_key(_b.master_info.uid, _b.pet_param.pet_unique_id);
		return a_key < b_key;
	}
};

typedef char PetAttrInfoHex[sizeof(AttributeList) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetAttrInfoHex) < 512);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PartnerRankKey
{
	PartnerRankKey() : role_id(0), partner_id(0) {}
	PartnerRankKey(int _role_id, int  _partner_id) : role_id(_role_id), partner_id(_partner_id) {}

	int role_id;
	int partner_id;

	bool operator<(const PartnerRankKey& right) const
	{
		if (role_id == right.role_id)
		{
			return partner_id < right.partner_id;
		}

		return role_id < right.role_id;
	}
};

struct PartnerRankItem
{
	PartnerRankItem() { this->Reset(); }

	void* operator new(size_t c);
	void operator delete(void* m);

	bool Invalid() const { return -1 == rank_type; }
	void Reset()
	{ 
		change_state = -1;

		user_uid = 0;
		partner_id = 0;

		rank_type = -1;
		rank_value = 0;
		flexible_int = 0;
		flexible_ll = 0;
	}

	char change_state;
	int index;

	int user_uid;
	int partner_id;

	int rank_type;
	long long rank_value;
	int flexible_int;
	long long flexible_ll;


	bool operator==(const PartnerRankItem & _a) const
	{
		if (rank_type == _a.rank_type &&rank_value == _a.rank_value &&  user_uid == _a.user_uid &&
			partner_id == _a.partner_id &&
			flexible_ll == _a.flexible_ll)
			return true;
		return false;
	}
	bool operator< (const PartnerRankItem & _a) const
	{
		if (rank_value == _a.rank_value)
		{
			return flexible_ll > _a.flexible_ll;
		}
		return rank_value < _a.rank_value;
	}

	bool operator<= (const PartnerRankItem & _a) const
	{
		return *this < _a || *this == _a;
	}

	bool operator> (const PartnerRankItem & _a) const
	{
		if (rank_value == _a.rank_value)
		{
			return flexible_ll < _a.flexible_ll;
		}
		return rank_value > _a.rank_value;
	}

	bool operator>= (const PartnerRankItem & _a) const
	{
		return *this > _a || *this == _a;
	}

	static bool RankCompare(const PartnerRankItem & _a, const PartnerRankItem & _b)
	{
		if (_a == _b)
		{
			return true;
		}

		if (_a.rank_value != _b.rank_value)
		{
			return _a.rank_value > _b.rank_value;
		}
		else if (_a.flexible_ll != _b.flexible_ll)
		{
			return _a.flexible_ll < _b.flexible_ll;
		}

		PartnerRankKey a_key(_a.user_uid, _a.partner_id);
		PartnerRankKey b_key(_b.user_uid, _b.partner_id);
		return a_key < b_key;
	}
};

#pragma pack(pop)

#endif  // __RANK_DEF_HPP__

