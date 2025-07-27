#ifndef __SERVERCOMMON_H__
#define __SERVERCOMMON_H__

// 服务器之间公用的相关定义 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <limits.h>
#include <assert.h>
#include "basedef.h"
#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/logindef.h"
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <iostream>

#define ARG_OUT
#define ARG_IN
#define ARG_IN_AND_OUT
#define ret_line return -__LINE__;

enum BEHAVIOR_GATHER_TYPE
{
	GATHER_TYPE_MINING = 0,			// 挖矿
	GATHER_TYPE_LUMBER = 1,			// 伐木
	GATHER_TYPE_FLOWER = 2,			// 采花

	GATHER_TYPE_MAX
};

enum WEAPON_TYPE
{
	WEAPON_TYPE_NONE,	// 0 空手
	WEAPON_TYPE_BOW,	// 1 弓
	WEAPON_TYPE_SWORD,	// 2 剑
	WEAPON_TYPE_AXE,	// 3 斧
	WEAPON_TYPE_WAND,	// 4 杖
	WEAPON_TYPE_SPEAR,	// 5 枪
	WEAPON_TYPE_GLOVES,	// 6 拳套
	WEAPON_TYPE_LANTERN,// 7 灯盏
	WEAPON_TYPE_FEATHER_FAN, // 8 羽扇
	WEAPON_TYPE_MAX,
};

enum ROLE_TYPE
{
	ROLE_TYPE_BENZUN = 0,		//本尊
	ROLE_TYPE_HUASHEN = 1,		//化身

	ROLE_TYPE_MAX,
};

enum EQUIP_TYPE
{
	EQUIP_TYPE_EQUIPMENT = 0,		//装备
	EQUIP_TYPE_JEWELRY = 1,			//灵饰

	EQUIP_TYPE_MAX,
};

enum ShopBuyLimitType
{
	SHOP_BUY_LIMIT_TYPE_INVALID = 0,
	SHOP_BUY_LIMIT_TYPE_LIFELONG,					// 终身限购
	SHOP_BUY_LIMIT_TYPE_WEEKLY,						// 每周限购
	SHOP_BUY_LIMIT_TYPE_DAILY,						// 每日限购
	SHOP_BUY_LIMIT_TYPE_MAX
};

enum CurrencyType
{
	CURRENCY_TYPE_GOLD = 0,			// 元宝
	CURRENCY_TYPE_IMMORTAL_COIN = 1,			// 仙币
	CURRENCY_TYPE_COIN_BIND = 2,			// 铜币

	CURRENCY_TYPE_MAX
};

static const char *INTERNAL_NETWORK = "InternalNetwork";

namespace ggprotocal
{
	static const int MAX_GGPROTOCAL_LENGTH = 4096;
}

namespace crossgameprotocal
{
	static const int MAX_CROSSGAMEPROTOCAL_LENGTH = 4 * 1024 * 1024;
}

namespace bigchgameprotocol
{
	static const int MAX_BIG_CROSS_HGAME_PROTOCAL_LENGTH = 4 * 1024 * 1024;
}

// 生成登陆场景服务器的sessionkey
void GetSessionKey(const std::string &key_base, int scene_id, int scene_key, int last_scene_id, 
				   int role_id, GameName role_name, unsigned int time, char first_login, PlatName pname, SessionKey key);

// 生成聊天服务器的验证key
unsigned int GetChatLoginKey(int uid);

// 聊天相关id资源划分
namespace chatdef
{
	static const int MAX_CHANNEL_TYPE_COUNT = 10;
	enum ChannelType
	{
		CHANNEL_TYPE_WORLD = 0,		//世界
		CHANNEL_TYPE_SCENE,			//当前
		CHANNEL_TYPE_TEAM,			//队伍
		CHANNEL_TYPE_GUILD,			//家族
		CHANNEL_TYPE_PROF,			//职业
		CHANNEL_TYPE_GROUP,			//群组聊天
		CHANNEL_TYPE_COMPLEX,		//综合聊天

		CHANNEL_TYPE_MAX,
	};

	enum ChannleIDBegin
	{
		CHANNEL_ID_WORLD_BEGIN = 0,				// 世界频道id为0
		CHANNEL_ID_CAMP_BEGIN = 100,			// 阵营频道id为100+阵营编号
		CHANNEL_ID_SCENE_BEGIN = 10000,			// 场景频道id为10000 + 服务器id * 10000 + SceneIndex
		CHANNEL_ID_TEAM_BEGIN = 200000,			// 组队聊天id为20W + team_id
		CHANNEL_ID_GUILD_BEGIN = 1000000,		// 军团聊天id为100W以上 保证给队伍足够的ID
	};
}


#define LL_ONE_BIT ((long long)0x0000000000000001)

static const int ROLE_RESET_MODULE_DATA_TM_HOUR = 6;

static const int MAX_ACCOUNT_ROLE_NUM = 5;
static const int MAX_ROLE_LEVEL = 120;	
static const int MAX_ROLE_LEVEL_VERSION_LIMIT = 500;
static const int MAX_ROLE_FLOOR_LEVEL = MAX_ROLE_LEVEL / 10;
static const int MAX_PET_LEVEL = MAX_ROLE_LEVEL;			// 宠物等级

static const int MONSTER_MAX_LEVEL = 120;

static const int TWO_BILLION = 2000000000;
static const int MAX_XIANHUN_VAL = 2000000000;	// 仙魂最大值 20亿
static const int MAX_YUANLI_VAL = 2000000000;	// 元力最大值 20亿
static const int MAX_CROSSHONOR_VAL = 2000000000;// 跨服荣誉最大值 20亿

static const int SECOND_PER_MIN = 60;			// 一分钟有60秒
static const int SECOND_PER_HOUR = 3600;		// 一小时有3600秒
static const int SECOND_PER_DAY = 86400;		// 一天有86400秒
static const int SECOND_PER_WEEK = 7 * SECOND_PER_DAY; // 一周秒数
static const int RAND_UNIVERSAL_RATIO_BASE_NUM = 10000;			// 万分比底数

static const long long LONG_LONG_MAX_VALUE = 9223372036854775807;// long long 最大值 
static const int PET_COUNT_MAX = 256;

static const int REAL_TIME_COMMENT_LEN = 2048;
static const int MAX_ROLE_CROSS_GROUP = 20;			// 跨服群组
static const int MAX_ROLE_LOCAL_GROUP = 0;			// 非跨服群组
static const int MAX_ROLE_GROUP = MAX_ROLE_CROSS_GROUP + MAX_ROLE_LOCAL_GROUP;			// 玩家加入群组数量

enum ELEMENT_TYPE_NAME
{
	ELEMENT_TYPE_INVALID = 0,
	ELEMENT_TYPE_WATER,						// 水 1
	ELEMENT_TYPE_FIRE,						// 火 2 
	ELEMENT_TYPE_WIND,						// 风 3
	ELEMENT_TYPE_EARTH,						// 土 4
	ELEMENT_TYPE_CHAOS,						// 混沌元素 5
	ELEMENT_TYPE_SELF,						// 取自身属性（用于那些需要读取自身元素属性的技能）6
	ELEMENT_TYPE_MAX,
};

enum BATTLE_TYPE
{
	BATTLE_TYPE_NORMAL = 0,			//!< 0 正常的战斗
	BATTLE_TYPE_JOY = 1,			//!< 1 欢乐寻宠
	BATTLE_TYPE_DRAGON_BOAT = 2,	//!< 2 龙舟竞速
	BATTLE_TYPE_MAX
};

inline int UidToInt(const UserID &uid)
{
	int role_id = (uid.db_index << DB_INDEX_MARK_BIT)  + uid.role_id;
	return role_id;
}

inline UserID IntToUid(int role_id)
{
	int d = role_id >> DB_INDEX_MARK_BIT;
	int r = role_id - (d << DB_INDEX_MARK_BIT);
	return UserID(d, r);
}

inline long long TranslateInt64(int a, int b)
{
	return ((long long)(a) << 32) + b;
}

inline long long UniqueUserIDToLongLong(const UniqueUserID &unique_user_id)
{
	return TranslateInt64(unique_user_id.plat_type, UidToInt(unique_user_id.user_id));
}

inline UniqueUserID LongLongToUniqueUserID(long long uuid)
{
	UniqueUserID unique_user_id;
	unique_user_id.plat_type = static_cast<int>(uuid >> 32);
	unique_user_id.user_id = IntToUid(static_cast<int>(uuid & 0xffffffff));
	return unique_user_id;
}

inline int GetUidFromUniqueUserID(long long uuid)
{
	UniqueUserID unique_user_id = LongLongToUniqueUserID(uuid);
	return UidToInt(unique_user_id.user_id);
}

void LongLongToUInt(long long l, unsigned int(&out)[2]);
void LongLongToInt(long long l,int (&out)[2]);
long long ConvertParamToLongLong(int param_0, int param_1);
long long ConvertParamToLongLong(int param_0, short param_1, short param_2);
bool HasInvalidChar(char *str);
void InvalidCharFilte(char *str);
void RoleNameCatPostfix(GameName rolename, short server);
void PlatNameCatPostfix(PlatName pname, short server);
void GuildNameCatPostfix(GuildName guildname, short server);
char * GetCurVersion(unsigned int unVer);
void PlatNameReCatPostfix(PlatName pname);

struct ItemDataWrapper;
struct SaleItemParam;
struct MailParam;
struct MailVirtualItemList;
struct ItemConfigData;

static const int LOG_MAX_ITEM_NUM = 8;
static const int LOG_BASE_LENGTH = 1024;

const char * ItemConfigDataLog(const ItemConfigData *item_cfg, char log_str[LOG_BASE_LENGTH]);

bool GetTimeTM(const time_t &cur_time, struct tm &time_tm);
int GetHourIndex(time_t begin_time, time_t t);
int GetDayIndex(time_t begin_time, time_t t);
int GetDayIndexByEndTime(time_t end_time,time_t t);
int GetDayIndex_6(time_t begin_time, time_t);
int GetWeekIndex(time_t begin_time, time_t t);
time_t GetWeekZeroTime(time_t cur_time);									// 获取当前时间对应这周的周一0点时间戳
time_t GetMonthZeroTime(time_t cur_time);									// 获取当前时间对应本月第一天0点时间戳
int GetMonthIndex(time_t begin_time, time_t t);
time_t GetZeroTime(time_t time_stamp);
time_t GetZeroMinuteTime(time_t time_stamp);
time_t GetZeroSecondTime(time_t time_stamp);

bool ConvertTimeStringToUnixTime(const char *time_str, time_t *timestamp);
bool ConvertTimeStringToUnixTime2(const char *time_str, time_t *timestamp);

bool ConvertUniqueKeyStrToNumber(UniqueKeyStr unique_key_str, ARG_OUT long long* unique_key);
bool ConvertUniqueKeyNumberToStr(long long unique_key, ARG_OUT UniqueKeyStr unique_key_str);

int RandomNum(int max_num);
int RandomNum(int min_num, int max_num);

enum SYS_MSG_TYPE
{
	SYS_MSG_FLOAT = 0,			 // 飘字	
	SYS_MSG_ROLLING_NOTICE = 1,  // 滚动公告（跑马灯）
	SYS_MSG_SYSTEM_CHAT = 2,     // 系统聊天栏
	SYS_MSG_RECRUIT = 3,		 // 招募栏
	SYS_MSG_WORLD_CHAT = 4,		 // 世界聊天栏
	SYS_MSG_PROF_CHAT = 5,		 // 职业频道
};

enum FORBID_TALK_TYPE
{
	FORBID_TALK_TYPE_DEFAULT = 0,		//默认(普通禁言模式)(有提示)
	FORBID_TALK_TYPE_SILENCE = 1,		//静默禁言模式(仅自己可看到)

	FORBID_TALK_TYPE_MAX
};

class SysMsgType
{
public:
	SysMsgType() : m_pos_binary(0) {}

	int GetPosBinary() { return m_pos_binary; }
	void SetPosBinary(int pos_binary) { m_pos_binary = pos_binary; }

private:
	SysMsgType(int);

	int m_pos_binary;
};

SysMsgType GetMsgType(int t);
SysMsgType GetMsgType(int t1, int t2);
SysMsgType GetMsgType(int t1, int t2, int t3);

enum PROF_TYPE																	// 职业定义
{
	PROF_TYPE_PROF_NO_LIMIT = -1,												// 可用于代表所有职业都都能接的任务等！
	PROF_TYPE_INVALID = 0,														// 新手 
	
	// 职业以100为单位递增，个位数为职业进阶了的次数																			
	PROF_TYPE_GLADIATOR = 100,													// 职业1 格斗士
	PROF_TYPE_SWORDSMAN = 200,													// 职业2 剑士
	PROF_TYPE_AXE_FIGHTER = 300,												// 职业3 战斧斗士
	PROF_TYPE_KNIGHT = 400,														// 职业4 骑士
	PROF_TYPE_ARCHER = 500,														// 职业5 弓箭手
	PROF_TYPE_MAGICIAN = 600,													// 职业6 魔术师
	PROF_TYPE_MISSIONARY = 700,													// 职业7 传教士
	PROF_TYPE_WIZZARD = 800,													// 职业8 巫师

	PROF_TYPE_MAX = 900,																
};
static const int PROFESSION_BASE = 100;
static const int PROFESSION_TYPE_NUM = PROF_TYPE_MAX / PROFESSION_BASE;
static const int PROFESSION_ADVANCE_TIMES = 6;									// 职业晋升次数

int GetBaseProf(int profession);

//当传入profession >= PROFESSION_BASE,返回GetBaseProfrofession
//当传入profession = (0, PROFESSION_BASE) 返回profession * PROFESSION_BASE;
int CompatibleGetProfType(int profession);

int GetAdvanceTimes(int profession);

enum AVATAR_TYPE					// 虚拟人物类型
{
	AVATAR_TYPE_INVALID,													
	AVATAR_TYPE_1,					// 1.月照君 (御姐)
	AVATAR_TYPE_2,					// 2.花偶   (萝莉)
	AVATAR_TYPE_3,					// 3.猫耳娘 (小姐姐)
	AVATAR_TYPE_4,					// 4.涂魂   (大叔)
	AVATAR_TYPE_5,					// 5.半月   (胖子)
	AVATAR_TYPE_6,					// 6.角男   (瘦小伙)
	AVATAR_TYPE_MAX,	
};

static const int SEX_ARRAY[AVATAR_TYPE_MAX] = { FEMALE, FEMALE, FEMALE, FEMALE, MALE, MALE, MALE };
				
static const int APPEARANCE_BASE = 10000;
static const int INIT_APPREARANCE_COLOR_NUM = 4;

// 初始形象 init_apprearance = avatar_type * APPEARANCE_BASE + color

enum DISCONNECT_NOTICE_TYPE
{
	DISCONNECT_NOTICE_TYPE_INVALID,
	DISCONNECT_NOTICE_TYPE_LOGIN_OTHER_PLACE,									// 玩家在别处登录
	DISCONNECT_NOTICE_TYPE_CLIENT_REQ,											// 客户端请求
};

// 全局系统数据状态 
enum GLOBAL_SYSTEM_DATA_STATE
{
	GLOBAL_SYSTEM_DATA_STATE_INVALID = 0,						// 初始状态
	GLOBAL_SYSTEM_DATA_STATE_LOADING,							// 数据加载中 
	GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH,						// 数据加载完成 
	GLOBAL_SYSTEM_DATA_STATE_MAX,
};

static const int ROLE_ENERGY_MAX = 1000;										// 体力最大值

static const int BIRTH_MAX_HP_BASE = 200;										// 初始血量
static const int BIRTH_MAX_MP_BASE = 50;										// 初始魔法
static const int BIRTH_BAOJI_BASE = 5;											// 初始暴击
static const int BIRTH_JIANREN_BASE = 10;										// 初始坚韧
static const int BIRTH_MOVESPEED_BASE = 1200;									// 初始速度

static const int BIRTH_FIRST_ATTR_LILIANG = 5;									// 一级属性 力量
static const int BIRTH_FIRST_ATTR_TIZHI = 5;									// 一级属性 体质
static const int BIRTH_FIRST_ATTR_JINGZHUN = 5;									// 一级属性 精准
static const int BIRTH_FIRST_ATTR_MINJIE = 5;									// 一级属性 敏捷

static const ItemID EXP_ITEM_ID = ItemID(-1);									// 经验的物品ID
static const ItemID GOLD_ITEM_ID = ItemID(-2);									// 元宝的物品ID
static const ItemID IMMORTAL_COIN_ITEM_ID = ItemID(-3);							// 非绑铜钱物品ID
static const ItemID COIN_ITEM_ID = ItemID(-4);									// 绑定铜币物品ID

static const int BIT_COUNT_OF_BYTE = 8 * sizeof(unsigned char);					// 单字节大小（位）
static const int BIT_COUNT_OF_LONG_64 = 8 * BIT_COUNT_OF_BYTE;					// longlong 型大小（位）

const static int COMMON_SCENE_KEY = 0;											// 通用场景key, 野外场景（非副本）
const static int COMMON_FB_KEY = 1;												// 通用副本key，用于所有玩家公用一个副本的情况

#define UNSTD_STATIC_CHECK(Expr)  				UNSTD_STATIC_CHECK1(Expr, __LINE__)
#define UNSTD_STATIC_CHECK1(Expr, Line)			UNSTD_STATIC_CHECK2(Expr, Line)
#define UNSTD_STATIC_CHECK2(Expr, Line)			typedef char UnName##Line[(Expr) ? 1 : -1];

#define UNSTD_OFFSETOFF(TYPE, MEMBER)			((size_t)((char *)&(((TYPE *)0x10)->MEMBER) - (char*)0x10))
 
#define ARRAY_ITEM_COUNT(a) (static_cast<int>(sizeof(a) / sizeof(*a)))

#ifdef _WIN32
template<class T, size_t size>
constexpr int ARRAY_LENGTH(T(&_Array)[size])
{
	return size;
}
#else
template<class T, size_t size>
inline int ARRAY_LENGTH(T(&_Array)[size])
{
	return size;
}
#endif

template<class T, size_t size>
inline void  ArrayCallReset(T(&arg)[size])
{
	for (size_t i = 0; i < size; ++i)
	{
		arg[i].Reset();
	}
}

template<typename T>
inline void  ArrayFillZero(T & src)
{
#ifdef _MSC_VER
	static_assert(std::is_pod<T>::value, "T  is not pod type in" __FUNCTION__);
	static_assert(std::is_array<T>::value, "T  is not array type in" __FUNCTION__);
#endif

	memset(&src, 0, sizeof(src));
}

template<typename T>
inline void  SameArrayTypeCopy(T & dst,const T & src)
{
#ifdef _MSC_VER
	static_assert(std::is_array<T>::value, "T  is not array type in" __FUNCTION__);
#endif

	if (&dst == &src)
	{
		return;
	}

	memcpy(&dst, &src, sizeof(T));
}

template<typename K, typename V>
V  MapValue(const std::map<K, V> & map,const K & key)
{
	typename std::map<K, V>::const_iterator iter = map.find(key);
	if (iter != map.end())
	{
		return iter->second;
	}
	else
	{
		return V();
	};
}

template<typename K, typename V>
V  * MapValuesPtr(std::map<K, V> & map, const K & key)
{
	typename std::map<K, V>::iterator iter = map.find(key);
	if (iter != map.end())
	{
		return &iter->second;
	}
	else
	{
		return NULL;
	};
}

template<typename K, typename V>
const V * MapValuesConstPtr(const std::map<K, V> & map, const K & key)
{
	typename std::map<K, V>::const_iterator iter = map.find(key);
	if (iter != map.end())
	{
		return &iter->second;
	}
	else
	{
		return NULL;
	};
}

template<typename K, typename V>
std::vector<V>  MapValuesAsVector(const std::map<K, V> & map)
{
	std::vector<V> vec;
	vec.reserve(map.size());

	for (typename std::map<K, V>::const_iterator iter = map.begin(); iter != map.end(); ++iter)
	{
		vec.push_back(iter->second);
	}

	return vec;
}

template<typename K, typename V>
std::vector<const V *>  MapValuesConstPtrAsVector(const std::map<K, V> & map)
{
	std::vector<const V *> vec;
	vec.reserve(map.size());

	for (typename std::map<K, V>::const_iterator iter = map.begin(); iter != map.end(); ++iter)
	{
		vec.push_back(&iter->second);
	}

	return vec;
}

template<typename MapType, typename MapKeyType, typename MapValueType>
typename MapType::iterator MapAddOrUpdate(MapType & m, const MapKeyType & k, const MapValueType & v)
{
	typename MapType::iterator iter = m.lower_bound(k);
	if (iter != m.end() && !(m.key_comp()(k, iter->first)))
	{
		iter->second = v;
		return iter;
	}
	else
	{
		return m.insert(iter, typename MapType::value_type(k, v));
	}
}

template<typename MapType, typename MapKeyType, typename MapValueType>
typename MapType::iterator MapAdd(MapType & m, const MapKeyType & k, const MapValueType & v) //没有才添加
{
	typename MapType::iterator iter = m.lower_bound(k);
	if (iter != m.end() && !(m.key_comp()(k, iter->first)))
	{
		return iter;
	}
	else
	{
		return m.insert(iter, typename MapType::value_type(k, v));
	}
}

template<typename T>
int BitCount(T k) 
{
	return sizeof(T) * 8;
}

template<typename T>
int BitCount()
{
	return sizeof(T) * 8;
}

int GetBaseProfession(int profession);
std::string STRING_SPRINTF(const char * format, ...);

// 字符串拷贝 转long long是为了屏蔽LINUX下的警告
#define STRNCPY(dst_str, src_str, dst_len)                     \
{                                                              \
	if (NULL != (dst_str) && (int)(dst_len) > 0)               \
	{                                                          \
		if (NULL != (src_str) && ((long long)dst_str) != ((long long)src_str))       \
		{                                                      \
			strncpy((dst_str), (src_str), (dst_len) - 1);      \
			(dst_str)[(dst_len) - 1] = '\0';                   \
		}										               \
		else                                                   \
		{                                                      \
			(dst_str)[0] = '\0';                               \
		}                                                      \
	}                                                          \
}

#define F_STRNCPY(dst_str, src_str, dst_len)                     \
{                                                              \
	if (NULL != (dst_str) && (int)(dst_len) > 0)               \
	{                                                          \
		if (NULL != (src_str) && ((long long)dst_str) != ((long long)src_str))       \
		{                                                      \
			strncpy((dst_str), (src_str), (dst_len) - 1);      \
			(dst_str)[(dst_len) - 1] = '\0';                   \
		}										               \
		else                                                   \
		{                                                      \
			(dst_str)[0] = '\0';                               \
		}                                                      \
	}                                                          \
}

/*
// 下面这个做法会触发valgrind的invalid read 
// 定长字符串数组拷贝
#define F_STRNCPY(dst_str, src_str, len)                       \
{                                                              \
	if (NULL != (dst_str) && (int)(len) > 0)                   \
	{                                                          \
		if (NULL != (src_str) && (dst_str) != (src_str))       \
		{                                                      \
			memcpy((dst_str), (src_str), (len) - 1);           \
			(dst_str)[(len) - 1] = '\0';                       \
		}                                                      \
		else                                                   \
		{                                                      \
			(dst_str)[0] = '\0';                               \
		}                                                      \
	}                                                          \
}
*/

// 字符串格式化操作函数
inline int SNPRINTF(char *dst, int len, const char *format, ...)
{
	if (NULL == dst || len <= 0 || NULL == format) return -1;
	
	va_list va;
	va_start(va, format);

	int ret = vsnprintf(dst, len, format, va);
	dst[len - 1] = '\0';
	if (ret >= len) ret = -1;

	va_end(va);

	dst[len - 1] = '\0';

	return ret;
}

#ifndef _WIN32
#define USE_VAR(x)    ((void) &(x)) 
#else 
#define USE_VAR(x) 
#endif 

#define USE_DATAACCESS_VAR()						\
inline void UseDataaccessVar()						\
{													\
	USE_VAR(dataaccessrmi::LOGIN);					\
	USE_VAR(dataaccessrmi::ROLE);					\
	USE_VAR(dataaccessrmi::SYSTEM);					\
	USE_VAR(dataaccessrmi::GLOBAL);					\
}

#define USE_INTERNAL_NETWORK()						\
inline void UseInternalNetwork()					\
{													\
	USE_VAR(INTERNAL_NETWORK);						\
}		

#define OLD_USE_INTERNAL_NETWORK()

#define USE_DATAACCESS_ROLE_VAR()					\
	inline void UseDataaccessRoleVar()				\
{													\
	USE_VAR(dataaccessrmi::role::GET_ROLE_INFO);	\
	USE_VAR(dataaccessrmi::role::ROLE_INIT);		\
	USE_VAR(dataaccessrmi::role::ROLE_SAVE);		\
	USE_VAR(dataaccessrmi::role::CREATE_ROLE);		\
	USE_VAR(dataaccessrmi::role::DESTROY_ROLE);		\
	USE_VAR(dataaccessrmi::role::ROLE_SAVE_STATUS);	\
	USE_VAR(dataaccessrmi::role::ROLE_QUERY);		\
	USE_VAR(dataaccessrmi::role::ROLE_READ_DATA);	\
	USE_VAR(dataaccessrmi::role::ROLE_SAVE_DATA);	\
}

#define USE_DATAACCESS_LOGIN_VAR()							\
	inline void UseDataaccessLoginVar()						\
{															\
	USE_VAR(dataaccessrmi::login::GET_PROF_NUM);			\
	USE_VAR(dataaccessrmi::login::GET_ACCOUNT_INFO);		\
	USE_VAR(dataaccessrmi::login::UPDATE_LAST_LOGIN_TIME);	\
	USE_VAR(dataaccessrmi::login::ADD_ROLE);				\
	USE_VAR(dataaccessrmi::login::REMOVE_ROLE);				\
	USE_VAR(dataaccessrmi::login::FORBID);					\
	USE_VAR(dataaccessrmi::login::GET_GOLD);				\
	USE_VAR(dataaccessrmi::login::CHANGE_GOLD);				\
	USE_VAR(dataaccessrmi::login::ADD_NAME_INFO);			\
	USE_VAR(dataaccessrmi::login::UPDATE_NAME_INFO);		\
	USE_VAR(dataaccessrmi::login::DELETE_NAME_INFO);		\
	USE_VAR(dataaccessrmi::login::DELETE_NAME_INFO_BY_NAME);\
	USE_VAR(dataaccessrmi::login::GET_NAME_CACHE);			\
	USE_VAR(dataaccessrmi::login::GET_GOLD_HISTORY);		\
}

USE_INTERNAL_NETWORK();

// ---------------------------------------------------------------------------------------------------------------

inline bool ItemCanMerge(ItemID item_id, int num, bool has_param, UInt32 invalid_time)
{
	return (0 != item_id) && (num > 0) && (num < 99) && (!has_param) && (0 == invalid_time);
}

static const int MAX_TEAM_MEMBER_NUM = 5;						// 队伍最大有5个人

enum TEAM_ASSIGN_MODE
{	
	TEAM_ASSIGN_MODE_INVALID = 0,
	TEAM_ASSIGN_MODE_RANDOM,									// 随机分配模式
	TEAM_ASSIGN_MODE_TAKE_ROUND,								// 轮流分配模式
	TEAM_ASSIGN_MODE_MAX
};

enum WEEKDAY
{
	SUNDAY = 0,													// 周日
	MONDAY,														// 周一
	TUESDAY,													// 周二
	WEDNESDAY,													// 周三
	THURSDAY,													// 周四
	FRIDAY,														// 周五
	SATURDAY,													// 周六

	WEEKDAY_COUNT												// 每周天数
};

static const int CHANNEL_CD_TIME_WORLD = 10;					// 世界频道冷却时间 s
static const int CHANNEL_CD_TIME_CAMP = 10;						// 国家频道冷却时间
static const int CHANNEL_CD_TIME_SCENE = 10;					// 场景频道冷却时间
static const int CHANNEL_CD_TIME_TEAM = 0;						// 组队频道冷却时间
static const int CHANNEL_CD_TIME_GUILD = 0;						// 军团频道冷却时间

// ---------------------------------------------------------------------------------------------------------------

namespace gamecommon
{
	const Posi GetDisRandPos(const Posi &curr_pos, int rand_dis);
	const Posi GetDisRandRangePos(const Posi &curr_pos, int min_rand_dis, int max_rand_dis);
}

// ---------------------------------------------------------------------------------------------------------------

struct NetAddress
{
	NetAddress(const char *_ipstr = "", unsigned short _port = 0) : ipstr(_ipstr), port(_port) {}

	bool Invalid() const { return (0 == port || ipstr.empty()); }
	bool operator==(const NetAddress &r) const { return (port == r.port && ipstr == r.ipstr); }
	bool operator!=(const NetAddress &r) const { return (port != r.port || ipstr != r.ipstr); }

	std::string ipstr;			// IP字符串
	unsigned short port;		// 端口
};

static const int SCENE_MONSTER_GENERATER_MAX_NUM = 20;									// 场景内怪物生成点最大数量

struct ClockReturnDayItem // AngineAdapter里m_clock->DayID()的返回值类型
{
	unsigned short year;
	unsigned char  mon;
	unsigned char  day;
};

int CalcDayIndexWithClockDayItem(ClockReturnDayItem *day1, ClockReturnDayItem *day2); // 计算day1到day2经过的天数，0代表同一天，可以为负数

int GetGameNameHash(const GameName str);
int GetStringHash(int max_len, const char *str);
int GetStringHash(const char* str);
int GetNumberOfOne(int n);

// 用于简化读取配置表代码
#define INIT_CFG_START()						\
	char errinfo[1024] = {0};					\
	int iRet = 0;								\
	TiXmlDocument document;						\
	\
	if (configname == "" || !document.LoadFile(configname))	\
{											\
	sprintf(errinfo,"%s: Load %s Error,\n %s.", configname.c_str(), configname.c_str(), document.ErrorDesc());	\
	*err = errinfo;							\
	return false;							\
}											\
	TiXmlElement *RootElement = document.RootElement();		\
	if (NULL == RootElement)					\
{											\
	*err = configname + ": xml root node error.";		\
	return false;							\
}											\

#define INIT_CFG_LIST(element_val, init_func)	\
{												\
	TiXmlElement *root_element = RootElement->FirstChildElement(#element_val);	\
	if (NULL == root_element)					\
{											\
	*err = configname + ": no ["#element_val"]";		\
	return false;							\
}											\
	\
	iRet = this->init_func(root_element);		\
	if (iRet)									\
{											\
	sprintf(errinfo,"%s: "#init_func" failed %d", configname.c_str(), iRet);	\
	*err = errinfo;							\
	return false;							\
}											\
}												\

// 避免越界的加法，T只能是：int，short，char，取值范围是[0, max_val]
template <typename T>
inline void AddNum(T *num, int add_num)
{
	if (NULL == num) return;

	int max_val = 0;
	switch (sizeof(T))
	{
	case 1:
		max_val = CHAR_MAX;
		break;

	case 2:
		max_val = SHRT_MAX;
		break;

	case 4:
		max_val = INT_MAX;
		break;

	default:
		return;
		break;
	}

	long long tmp_num = *num + (long long)add_num;
	if (tmp_num > max_val)
	{
		tmp_num = max_val;
	}

	if (tmp_num < 0)
	{
		tmp_num = 0;
	}

	*num = (T)tmp_num;
}

enum RETRIEVE_REASON
{
	RETRIEVE_REASON_REFUND = 0,						// 退款
	RETRIEVE_REASON_BUG = 1,						// 出现刷资源BUG等异常情况下需要回收资源
	RETRIEVE_REASON_IN_DEBT = 2,                    // 透支
	RETRIEVE_REASON_MAX,
};

struct RetrieveReasonItem
{
	int reason;
	const char *str;
};

const static RetrieveReasonItem gs_retrieve_reason_str_list[] =
{
	{ RETRIEVE_REASON_REFUND, "retrieve_reason_refund" },
	{ RETRIEVE_REASON_BUG, "retrieve_reason_bug" },
	{ RETRIEVE_REASON_IN_DEBT, "retrieve_reason_in_debt" },
	//{RETRIEVE_REASON_MAX, "MAX"}
};

const char* g_GetRetrieveReasonTypeStr(int reason);

UNSTD_STATIC_CHECK(3 == RETRIEVE_REASON_MAX);

UNSTD_STATIC_CHECK(sizeof(gs_retrieve_reason_str_list) / sizeof(gs_retrieve_reason_str_list[0]) == RETRIEVE_REASON_MAX);

template<typename T, size_t init_size = 10>
class DynamicArray
{
public:
	DynamicArray()
	{
		UNSTD_STATIC_CHECK(init_size > 0);
		data_p = (T *)malloc(init_size * sizeof(T));
		lenth = 0;
		size = init_size;
	}

	~DynamicArray()
	{
		free(data_p);
	}

	DynamicArray(const DynamicArray & _src)
	{
		size = _src.size;
		data_p = (T *)malloc(size * sizeof(T));
		memcpy(data_p, _src.data_p, size * sizeof(T));
		lenth = _src.lenth;
	}

	void PushBack(const T &data)
	{
		if (lenth >= size)
		{
			size += init_size;
			void* new_p = (T *)realloc(data_p, size * sizeof(T));
			if (NULL == new_p)
			{
				return;
			}
			data_p = (T *)new_p;
		}

		data_p[lenth++] = data;
	}

	T &operator[](int index) const
	{
		if (index < 0 || index >= size)
		{
			assert(false);
		}

		return data_p[index];
	}

	DynamicArray& operator=(const DynamicArray& _src)
	{
		if (this == &_src) return *this;

		size = _src.size;
		void* new_p = (T *)realloc(data_p, size * sizeof(T));
		if (NULL == new_p)
		{
			return *this;
		}
		data_p = (T *)new_p;
		memcpy(data_p, _src.data_p, size * sizeof(T));
		lenth = _src.lenth;

		return *this;
	}

	inline size_t Size() const
	{
		return (size_t)size;
	}

	inline int Count() const
	{
		return lenth;
	}

private:
	T *data_p;
	int lenth;
	int size;
};

template<typename T>
void RandomShuffle(T* t_list, int t_num) // 对数组里元素随机洗牌
{
	for (int i = t_num - 1; i > 0; --i)
	{
		std::swap(t_list[i], t_list[rand() % (i + 1)]);
	}
}

template<typename T>
void RandomShuffle(T* t_list, int t_begin, int t_end) // 对数组里元素随机洗牌 [t_begin, t_end)
{
	for (int i = t_end - 1; i > t_begin; --i)
	{
		std::swap(t_list[i], t_list[t_begin + rand() % (i - t_begin + 1)]);
	}
}

template<typename T>
void RandomShuffle(T* t_list, int t_begin, int t_end, unsigned int rand_seed) //指定随机种子 对数组里元素随机洗牌 [t_begin, t_end)
{
	srand(rand_seed);
	for (int i = t_end - 1; i > t_begin; --i)
	{
		std::swap(t_list[i], t_list[t_begin + rand() % (i - t_begin + 1)]);
	}
}

inline void SplitPlatName(PlatName pname)
{
	static PlatName tmp_name;
	int index = -1;
	memcpy(tmp_name, pname, sizeof(PlatName));
	for (int i = 0; i < 64; ++i)
	{
		if (tmp_name[i] == '|')
		{
			index = i + 1;
			break;
		}
	}

	if (index < 0 || index >= 64)
	{
		return;
	}

	int t_index = index; int p_index = 0;
	for (; p_index < 64 && t_index < 64; ++p_index, ++t_index)
	{
		pname[p_index] = tmp_name[t_index];

		if (tmp_name[t_index] == '\0')
		{
			break;
		}
	}

	if (64 == t_index)
	{
		pname[63] = '\0';
	}
}

typedef int	Attribute;

// 禁止实例被拷贝复制，或被外部创建
#define INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(ClassName) \
	private:\
		ClassName();\
		virtual ~ClassName();\
		ClassName(const ClassName&);\
		ClassName& operator= (const ClassName&);

// 禁止实例被拷贝复制 只声明不定义，会在链接时报错
#define NONCOPYABLE(T) T(const T&); T& operator=(const T&);


template <typename T>
const T& GetMin(const T& t1, const T& t2)
{
	return t1 < t2 ? t1 : t2;
}

const char* GetMin(const char* t1, const char* t2);

template <typename T>
const T& GetMax(const T& t1, const T& t2)
{
	return t1 < t2 ? t2 : t1;
}

const char* GetMax(const char* t1, const char* t2);

template <typename T>
const T& GetRand(const T& t1, const T& t2)
{
	int rand_num = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	return (RAND_UNIVERSAL_RATIO_BASE_NUM >> 1 > rand_num) ? t1 : t2;
}

struct AttrSegment
{
	AttrSegment() : min_val(0), max_val(0) {}

	void Reset()
	{
		min_val = 0;
		max_val = 0;
	}

	Attribute min_val;
	Attribute max_val;
};

struct AttrInfo
{
	short attr_type;					//!< 属性类型
	short attr_value;					//!< 属性数值
};

struct AttrVocInfo						//!< 装备词条
{
	short un_lock;						//!< 是否解锁
	short reserve_sh;
	short attr_type;					//!< 属性类型
	short attr_value;					//!< 属性数值
};

const char * ItemDataWrapperLog(const ItemDataWrapper *item_wrapper, int index = 0);		// 连续使用作为函数参数 要传索引 小于LOG_MAX_ITEM_NUM
const char * MailVirtualItemLog(const MailVirtualItemList *param);
const char * MailParamLog(const MailParam *param);
const char * MailParamSimpleLog(const MailParam *param);
void MergeItemConfigData(std::vector<ItemConfigData> ARG_IN_AND_OUT  & list);
void MergeItemDataWrapper(std::vector<ItemDataWrapper> ARG_IN_AND_OUT & list);


enum RELOAD_CONFIG_TYPE
{
	RELOAD_CONFIG_TYPE_INVALID = 0,
	RELOAD_CONFIG_TYPE_GLOBAL_CONFIG,								//!< 1 全局
	RELOAD_CONFIG_TYPE_SKILL,										//!< 2 技能
	RELOAD_CONFIG_TYPE_TASK,										//!< 3 任务
	RELOAD_CONFIG_TYPE_MONSTER,										//!< 4 怪物
	RELOAD_CONFIG_TYPE_ITEM,										//!< 5 物品
	RELOAD_CONFIG_TYPE_LOGIC_CONFIG,								//!< 6 逻辑
	RELOAD_CONFIG_TYPE_DROP,										//!< 7 掉落 （暂时屏蔽）
	RELOAD_CONFIG_TYPE_SHIZHUANG,									//!< 8 时装 （没有引用）
	RELOAD_CONFIG_TYPE_SCENE,										//!< 9 场景 
	RELOAD_CONFIG_TYPE_SHARED,										//!< 10 共享 （暂时屏蔽）
	RELOAD_CONFIG_TYPE_ONE_ITEM,									//!< 11 单个物品 （暂时屏蔽）
	RELOAD_CONFIG_TYPE_ACTIVITY,									//!< 12 活动
	RELOAD_CONFIG_TYPE_BATTLE,										//!< 13 战斗
	RELOAD_CONFIG_TYPE_SPID_CONFIG,									//!< 14 渠道配置 (同时热更登录服的SpidConfig和游戏服的GlobalConfig)
	RELOAD_CONFIG_TYPE_TRADE_MARKET,								//!< 15 交易行配置
	RELOAD_CONFIG_TYPE_NPC,											//!< 16 NPC
	RELOAD_CONFIG_TYPE_NOTICE_CONFIG,								//!< 17 公告
	RELOAD_CONFIG_TYPE_APPEARANCE_CONFIG,							//!< 18 外观
	RELOAD_CONFIG_TYPE_LOGIC_CONFIG_ASYNC,							//!< 19 逻辑（异步）
	RELOAD_CONFIG_TYPE_SPECIAL_EFFECT,								//!< 20 特效
	RELOAD_CONFIG_TYPE_ONE_MONSTER,									//!< 21 重读单个怪物
	RELOAD_CONFIG_TYPE_SPECIAL_MONSTER,								//!< 22 重读特殊怪物
	RELOAD_CONFIG_TYPE_ROBOT,										//!< 23 机器人
	RELOAD_CONFIG_TYPE_GOD_EQUIP,									//!< 24 神装
	RELOAD_CONFIG_TYPE_BATTLE_HALLOW,								//!< 25 战斗圣器
	RELOAD_CONFIG_TYPE_ASYNC_BATTLE_MONSTER,						//!< 26 怪物（异步）
	RELOAD_CONFIG_TYPE_BATTLE_OTHER,								//!< 27 战斗其他配置
	RELOAD_CONFIG_TYPE_MAX,
};

/**
* @brief 判断flag的第bit_index位是1还是0
* @param flag 一个[0,MAX_INT)的值
* @param bit_index 需要检查flag的哪一位
* @return 返回true，则flag的第bit_index位是1，否则是0
*/
template <typename T>
bool IsSetBit(const T & flag, int bit_index)
{
	return (flag & (1LL << bit_index)) != 0;
}

template <typename T>
void SetBit(T & out_flag, int bit_index)
{
	(out_flag) |= (1LL << bit_index);
}

template <typename T>
void WipeBit(T & out_flag, int bit_index)
{
	(out_flag) &= ~(1LL << bit_index);
}

////////////////////////////////////匹配逻辑/////////////////////////////////////////////////////

struct MatchTeamData;
struct MatchUserData
{
	MatchUserData(){this->Reset();}

	void Reset()
	{
		uid = 0;
		level = 0;
	}

	int uid;
	int level;
	MatchTeamData* team;
};

struct MatchTeamData
{
	MatchTeamData() { this->Reset(); }

	void Reset()
	{
		team_key = 0;
		count = 0;
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
		{
			user_list[i] = NULL;
		}
	}

	bool AddMember(MatchUserData* user)
	{
		if (NULL == user) return false;
		if (count >= MAX_TEAM_MEMBER_NUM) return false;
		user_list[count] = user;
		user->team = this;
		count++;
		return true;
	}

	bool DecMember(MatchUserData* user)
	{
		if (NULL == user) return false;
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
		{
			if (NULL != user_list[i] && user_list[i]->uid == user->uid)
			{
				count--;
				delete user_list[i];
				user_list[i] = user_list[count];
				user_list[count] = NULL;
				break;
			}
		}
		return true;
	}

	int GetTotalLevel()
	{
		int ret = 0;
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
		{
			if (NULL != user_list[i])
			{
				ret += user_list[i]->level;
			}
		}
		return ret;
	}

	int team_key;
	int count;
	MatchUserData* user_list[MAX_TEAM_MEMBER_NUM];
};

struct ActFightGroup
{
	ActFightGroup() { this->Reset(); }
	void Reset()
	{
		memset(user_list, 0, sizeof(user_list));
	}

	int user_list[2][MAX_TEAM_MEMBER_NUM];
};

struct FightGroup
{
	FightGroup()
	{
		count = 0;
	}

	int count;
	MatchTeamData* team[2];
};

class MatchLogic
{
public:
	MatchLogic();
	virtual ~MatchLogic();

	void Reset();

	virtual int OnMatchReq(int count, int uid_list[MAX_TEAM_MEMBER_NUM], int level[MAX_TEAM_MEMBER_NUM]);
	virtual int OnCancelReq(int count, int uid_list[MAX_TEAM_MEMBER_NUM]);

	int Logic(std::vector<FightGroup>& fight_group);
	int Match(std::vector<ActFightGroup>& ret_group);

	void SetFunc(bool(*func)(MatchTeamData* a, MatchTeamData* b)) { m_func = func; }

	void PutTeamMap(MatchTeamData* team);
	int team_key;
	std::map<int, MatchTeamData*> m_team_list[MAX_TEAM_MEMBER_NUM + 1];
	std::map<int, MatchUserData*> m_user_map;

	bool(*m_func)(MatchTeamData* a, MatchTeamData* b);
};

const char* GetIPStr(unsigned int ip);
unsigned int GetIPFromStr(std::string ip_str);

enum SURFACE_TYPE
{
	SURFACE_TYPE_BODY = 1,			// 身体
	SURFACE_TYPE_ARM,				// 武器
	SURFACE_TYPE_WING,				// 羽翼
	SURFACE_TYPE_MOUNT,				// 坐骑
	SURFACE_TYPE_RESERVE,			// 保留
	SURFACE_TYPE_MAX,
};

UNSTD_STATIC_CHECK(6 == SURFACE_TYPE_MAX);

#pragma pack(push, 4)

struct Appearance
{
	Appearance() : prof_base(0), advance_times(0), avatar_type(0), color(0), weapon_id(0), sub_weapon_id(0),
		headshot_id(0), special_appearance_type(0),
		special_appearance_param(0), smart_mounts_index(-1), is_use_advanced_flag(0), is_use_change_card(0), change_card(0), fly_flag(0),
		is_use_huanshou(0), huanshou_fushen_id(-1)
	{
		memset(wear_surface, 0, sizeof(wear_surface));
		zaohua_app_id = 0;
		reserve_ch = 0;
		reserve_ll = 0;
	}

	bool Invalid() const
	{
		return 0 == prof_base || 0 == advance_times || 0 == avatar_type;
	}

	void Reset()
	{
		prof_base = 0;
		advance_times = 0;
		avatar_type = 0;
		color = 0;
		weapon_id = 0;
		sub_weapon_id = 0;
		
		memset(wear_surface, 0, sizeof(wear_surface));

		special_appearance_type = 0;
		special_appearance_param = 0;
		headshot_id = 0;

		smart_mounts_index = -1;
		is_use_advanced_flag = 0;
		is_use_change_card = 0;
		change_card = 0;
		fly_flag = 0;

		zaohua_app_id = 0;
		reserve_ch = 0;
		is_use_huanshou = 0;
		huanshou_fushen_id = -1;
		reserve_ll = 0;
	}

	char prof_base;			//!< 职业[0,9)
	char advance_times;		//!< 进阶次数[0,6]
	char avatar_type;		//!< 角色模型[1,6]
	char color;				//!< 颜色[0,4)
	ItemID weapon_id;		//!< 武器ID
	ItemID sub_weapon_id;	//!< 副武器ID

	short headshot_id;		//!< 头像ID 
	short special_appearance_type;	//!< 特殊形象类型
	int special_appearance_param;	//!< 特殊形象参数
	
	short wear_surface[SURFACE_TYPE_MAX];		//!< 已装备的的幻化资源id 数组下标为配置表的type

	short smart_mounts_index;			//当前骑乘的灵骑 -1:未骑乘
	char is_use_advanced_flag;			//灵骑形象使用标识 0:未使用进阶形象 1:使用进阶形象
	char is_use_change_card;			//是否使用变身卡形象 0:不使用 1:使用变身卡形象
	short change_card;					//变身卡(seq)
	short fly_flag;						//飞升阶段

	int zaohua_app_id;			//!< 造化装备外观
	char reserve_ch;
	char is_use_huanshou;		//是否使用幻兽形象 0:不使用 1:使用幻兽形象
	short huanshou_fushen_id;	//幻兽附身ID (-1:未附身)
	long long reserve_ll;		//!< 保留字  留作扩展
};
typedef char AppearanceParamHex[sizeof(Appearance) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AppearanceParamHex) < 256);


#pragma pack(pop)

enum ADD_OK_TYPE
{
	ADD_OK_TYPE_RIGHT,
	ADD_OK_TYPE_POS_OVER ,
	ADD_OK_TYPE_NEG_OVER,
};

int IntAddOk(int  a, int  b, int & out_sum); //int 相加判断
void IntHelpAdd(int ARG_IN_AND_OUT & curr_value, int max_value, int add_value, int min_value = 0);

/**
* \brief 同步写文件
* \param path 目录 （需要确保目录已存在）
* \param data 要写入的数据
* \param length 要写入数据的长度
* \param is_append 是否追加写入数据
* \param is_binary 是否写入二进制数据
* \return 返回是否成功写入数据
* \warning 需要确保目录已存在
* \remarks IFileWriteModule提供的write接口是异步写文本文件，在windows下遇到每个0x0A都会在前面加一个0x0D,
           导致写二进制数据进文件时会出现数据有偏差的问题，所以暂时在windows下使用同步写文件
*/
bool WriteFile(const char *path, const char *data, int length, bool is_append, bool is_binary);

/**
* \brief 创建目录
* \param folder 要创建的目录
* \return 是否成功创建
* \note 该函数可以创建多级目录
*/
bool CreateDirectory(const std::string& folder);

std::string PrintStackBackTrace(bool is_dump_core);

bool CheckNameHasServerSuffix(const std::string& name);
/**
* \brief 产生全排列中的一个随机序列
* \param start_number 起始数
* \param end_number 终结数 （前闭后开）
* \return 返回一个随机序列，序列中的数范围在[start_number,end_number)
*	且数值唯一，并数量为N=end_number - start_number
*/
std::vector<int> RandomPermutation(int start_number, int end_number);

enum SYNC_LOG_TYPE
{
	SYNC_LOG_TYPE_MONEY_GOLD = 0,
	SYNC_LOG_TYPE_MONEY_COIN = 1,
	SYNC_LOG_TYPE_TASK_STAT = 2,
	SYNC_LOG_TYPE_TASK = 4,
	SYNC_LOG_TYPE_SCORE_TASK = 5,
	SYNC_LOG_TYPE_MAX,
};

static const std::string HOTFIX_CROSS_PATH = "./hotfix/cross/";
static const std::string HOTFIX_GAMEWORLD_PATH = "./hotfix/gameworld/";
static const std::string HOTFIX_BATTLE_PATH = "./hotfix/battle/";
static const std::string HOTFIX_FILE_NAME = "hotfix_file";
static const std::string HOTFIX_BIGC_PATH = "./hotfix/bigcross/";

std::vector<std::string> ParseStringByDelimiter(const char* str, const char* delimiter);
bool ReadHotfixFile(const std::string& path);

unsigned long long GetNowTime_MS();

std::string GetRoleNameWithServerId(const char* name, int server_id);


// 尝试在目录路径串中找到版本号串
bool GetVersionStrFromPathStr(std::string* version_str, const std::string& path);

bool GetVersionStr(std::string* version_str);

std::string GetCurrentWorkingDirectory();

bool IsStringAllNumber(const std::string& str);
bool IsStringMatchVersionStr(const std::string& version_str);

#endif // __SERVERCOMMON_H__