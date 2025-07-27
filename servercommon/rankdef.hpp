#ifndef __RANK_DEF_HPP__
#define __RANK_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/guilddef.hpp"
#include "attributedef.hpp"
#include "gameworld/gameworld/item/itembase.h"
#include "gameworld/gameworld/other/attributelist.hpp"

static const int RANK_DATA_SAVE_TO_DB_TIME = 60;			 // 排行榜数据保存到DB的时间间隔

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const int PERSON_RANK_LEVEL_LIMIT = 20;				 // 上榜等级限制
static const int PERSON_RANK_UPDATE_TO_GLOBAL_TIME = 1 * 60; // 同步到排行榜的时间间隔 
static const int PERSON_RANK_SNAPSHOT_INTERVAL = 2 * 60;	 // 个人排行榜快照时间间隔 秒

enum PROVISIONAL_RANK
{
	PROVISIONAL_RANK_TYPE_RELIC_LOOTING = 1,		// 遗迹夺宝
	PROVISIONAL_RANK_TYPE_MAX,
};

// 个人榜类型  
enum PERSON_RANK_TYPE
{
	PERSON_RANK_TYPE_LEVEL = 0,				// 等级榜

	REASON_RANK_TYPE_PROF_LEVEL_1 = 1,			// 职业等级榜格斗士
	REASON_RANK_TYPE_PROF_LEVEL_2 = 2,			// 职业等级榜剑士
	REASON_RANK_TYPE_PROF_LEVEL_3 = 3,			// 职业等级榜战斧斗士
	REASON_RANK_TYPE_PROF_LEVEL_4 = 4,			// 职业等级榜骑士
	REASON_RANK_TYPE_PROF_LEVEL_5 = 5,			// 职业等级榜弓箭手
	REASON_RANK_TYPE_PROF_LEVEL_6 = 6,			// 职业等级榜魔术师
	REASON_RANK_TYPE_PROF_LEVEL_7 = 7,			// 职业等级榜传教士
	REASON_RANK_TYPE_PROF_LEVEL_8 = 8,			// 职业等级榜巫师

	PERSON_RANK_TYPE_CAPABILITY = 9,			// 战力榜(此时修改为综合战力榜, 角色 + 出战伙伴 + 出战宠物)

	PERSON_RANK_TYPE_PROF_CAPABILITY_BEGIN,
	PERSON_RANK_TYPE_PROF_CAPABILITY_1 = 10,	// 职业战力榜格斗士
	PERSON_RANK_TYPE_PROF_CAPABILITY_2 = 11,	// 职业战力榜剑士
	PERSON_RANK_TYPE_PROF_CAPABILITY_3 = 12,	// 职业战力榜战斧斗士
	PERSON_RANK_TYPE_PROF_CAPABILITY_4 = 13,	// 职业战力榜骑士
	PERSON_RANK_TYPE_PROF_CAPABILITY_5 = 14,	// 职业战力榜弓箭手
	PERSON_RANK_TYPE_PROF_CAPABILITY_6 = 15,	// 职业战力榜魔术师
	PERSON_RANK_TYPE_PROF_CAPABILITY_7 = 16,	// 职业战力榜传教士
	PERSON_RANK_TYPE_PROF_CAPABILITY_8 = 17,	// 职业战力榜巫师
	PERSON_RANK_TYPE_PROF_CAPABILITY_END,

	PERSON_RANK_TYPE_HUASHEN_CAPABILITY = 18,	// 化身角色战力

	PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP = 19,	// 角色战力+宠物最高战力
	PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE = 20,	// 梦渊笔录
	PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK = 21,	//轮回忘川
	PERSON_RANK_TYPE_CHIVALROUS	= 22,			// 侠义值
	PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY = 23,// 幽冥炼狱

	PERSON_RANK_TYPE_BIG_DIPPER_1 = 24,	// 星君1
	PERSON_RANK_TYPE_BIG_DIPPER_2 = 25,	// 星君2
	PERSON_RANK_TYPE_BIG_DIPPER_3 = 26,	// 星君3
	PERSON_RANK_TYPE_BIG_DIPPER_4 = 27,	// 星君4
	PERSON_RANK_TYPE_BIG_DIPPER_5 = 28,	// 星君5
	PERSON_RANK_TYPE_BIG_DIPPER_6 = 29,	// 星君6
	PERSON_RANK_TYPE_BIG_DIPPER_7 = 30,	// 星君7
	PERSON_RANK_TYPE_BIG_DIPPER_8 = 31,	// 帝君

	PERSON_RANK_TYPE_ROLE_CAPABILITY = 32,		// 角色战力榜
	PERSON_RANK_TYPE_FOUR_MAX_PARTNER = 33,		// 四个最高战力伙伴
	PERSON_RANK_TYPE_EQUIPMENT_CAP = 34,		// 穿戴装备评分比拼
	PERSON_RANK_TYPE_PET_MAX_CAPABILITY = 35,	// 宠物战力榜(只取玩家最高战力宠物)
	PERSON_RANK_TYPE_ENDLESS_TOWER = 36,		// 无尽之塔
	PERSON_RANK_TYPE_FALLEN_GOD = 37,	//殒神之地
	PERSON_RANK_TYPE_MEILI_QIXI = 38,	// 魅力榜
	PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE = 39,// 弑神之塔(总星数)

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

static const unsigned int CROSS_PERSON_RANK_MAX_SIZE = 10000;		// 上榜人数10000 
static const unsigned int PERSON_RANK_MAX_SIZE = 3000;				// 上榜人数3000 
static const unsigned int CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT = 100;		//单次协议同步数据最大元素数量

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

static const int PET_RANK_UPDATE_TO_GLOBAL_TIME = 1 * 60; // 同步到排行榜的时间间隔 
static const int PET_RANK_SNAPSHOT_INTERVAL = 60;		 // 个人排行榜快照时间间隔 秒
enum PET_RANK_TYPE
{
	PET_RANK_TYPE_CAPABILITY,								// 战力榜
	PET_RANK_TYPE_HUASHEN_CAPABILITY,						// 化身宠物战力

	PET_RANK_TYPE_MAX,
};
static const int CROSS_PET_RANK_MAX_SIZE = 10000;			// 跨服上榜宠物数10000
static const int PET_RANK_MAX_SIZE = 3000;					// 上榜宠物数3000
static const int CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT = 100;	// 单次协议同步数据最大元素数量

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum PARTNER_RANK_TYPE
{
	PARTNER_RANK_TYPE_CAPABILITY,							// 伙伴战力榜

	PARTNER_RANK_TYPE_MAX,
};
static const int PARTNER_RANK_MAX_SIZE = 10;				// 上榜伙伴数

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 军团榜类型  
enum GUILD_RANK_TYPE
{
	GUILD_RANK_TYPE_LEVEL = 0,								// 军团等级榜
	GUILD_RANK_TYPE_WEEK_ACTIVE = 1,						// 帮派周活跃度

	GUILD_RANK_TYPE_MAX,
};

static const int CROSS_GUILD_RANK_SYNC_PAGE_ELEMENT_COUNT = 100;	// 跨服帮派排行榜单次同步元素最大个数 100
static const int GUILD_RANK_MAX_SIZE = 100;							// 上榜军团 存多100个
static const int GUILD_RANK_SNAPSHOT_INTERVAL = 60;					// 军团排行榜快照时间间隔 秒
static const int CROSS_GUILD_ACTIVE_RANK_REWARD_COUNT = 10;			// 跨服帮派活跃度排行榜取前X名
static const int GUILD_ACTIVE_PERSON_RANK_REWARD_COUNT = 20;		// 帮派个人活跃度排行榜取前X名

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

	UniqueServerID server_id;		// 服务器ID
	int guild_id;					// 公会id

	GameName guild_name;			// 公会名
	GuildBanner guild_banner;		// 族徽

	UserID tuan_zhang_user_id;
	GameName tuan_zhang_name;

	short guild_level;
	short camp;
	short member_count;
	short max_member_count;
	int rank_type;

	long long rank_val;				// 参与排行榜的具体数值
	
	// 只作为查找使用 , 只验证了部分数据
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

// 组队榜类型
enum TEAM_RANK_TYPE
{
	TEAM_RANK_TYPE_MIN = 0,
	TEAM_RANK_TYPE_FALLEN_GOD = 1,	//殒神之地
	TEAM_RANK_TYPE_MAX
};

static const int TEAM_RANK_MAX_SIZE = 120;					// 上榜记录数 存多20个

static const int TEAM_RANK_SNAPSHOT_INTERVAL = 60;			// 组队排行榜快照时间间隔 秒

static const int CHIVALROUS_RANK_LIST_NUM = 100;
static const int MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM = 100;	//跨服英雄录-排名奖励配置seq等于排名


static const int FALLEN_GOD_REWARD_MEMBER_LIST_NUM = 10;//殒神之地排行奖励Top10
static const int MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM = 100;//殒神之地排行奖励Max

enum CHIVALROUS_RANK_TYPE				// 侠义值排行榜
{
	CHIVALROUS_RANK_TYPE_SERVER = 0,	// 跨服榜
	CHIVALROUS_RANK_TYPE_GUILD = 1,		// 家族榜
	CHIVALROUS_RANK_TYPE_FRIEND = 2,	// 好友榜
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
	int week_chivalrous;		// 本周侠义值
};

#pragma pack(push, 4)

struct TeamRankMemberInfo									// 组队排行队员信息 会用在协议中 注意对齐
{
	TeamRankMemberInfo() : uid(0) { memset(user_name, 0, sizeof(user_name)); }

	void Reset() { uid = 0; memset(user_name, 0, sizeof(user_name)); }

	int uid;												// 用户ID
	GameName user_name;										// 用户名
};

// 玩家基础信息
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

struct PetSimpleInfo			//宠物排行列表简易数据
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
	short fei_sheng_times;			// 宠物飞升次数
	int lian_yao_pet_id;			// 炼妖宠物id
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
	int index;														// 排行榜排名 弃用,如需获取排名,请使用GetIndex

	RoleBaseInfo role_info;

	int rank_type;
	long long rank_value;
	int reserve_int;												// 弃用
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

	// 注意 , 修改判断条件时,必须保证判断条件可以保证数据的唯一性
	// 否则可能会出现丢失索引或者数据重复及其他未知问题
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
	// 排行榜特有参数
	int				pet_index;

	int				pet_id;
	GameName		name;
	// 强化等级
	unsigned short	str_level;
	// 等级&经验
	unsigned short	level;
	long long		exp;
	long long       add_exp;		// 战斗结束宠物增加的经验

									// 当前的血量与蓝量
	int				hp;
	int				mp;

	// 技能列表 技能等级*10000 + 技能id
	int	exclusive_skill;				//专属技能
	int	exclusive_passive_skill;		//专属被动

										// 天赋列表
	PetPassiveItemInfo	passive_list[PET_PASSIVE_MAX_COUNT];
	// 洗练出来天赋列表
	PetPassiveItemInfo	new_passive_list[PET_PASSIVE_MAX_COUNT];

	// 资质
	unsigned short	growth[ADD_POINT_TYPE_MAX];
	// 洗练出来资质
	unsigned short	new_growth[ADD_POINT_TYPE_MAX];

	int comprehend_gift_num;		//领悟技能的个数，每个宠物只能领悟2个技能
	int comprehend_passive_value;		//领悟技能的值

										//0为默认没有上锁  1 为上锁了
	unsigned char		lock;
	unsigned char		refined_flag;		//洗练了什么标志
											// 已分配属性点
	unsigned short	attr[ADD_POINT_TYPE_MAX];

	// 最大的血量与蓝量
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

	int str_buff_level;					// 共鸣等级
	RoleBaseInfo master_info;			// 主人信息
	PetPro pet_param;					// 完整的宠物信息
	GodPrintListPro pet_god_print;		// 神印套装

	//宠物属性
	AttributeList pet_attr;

	int rank_type;
	long long rank_value;
	int reserve_int;							// 弃用
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

