#ifndef __SERVERCOMMON_H__
#define __SERVERCOMMON_H__

// ������֮�乫�õ���ض��� 

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
	GATHER_TYPE_MINING = 0,			// �ڿ�
	GATHER_TYPE_LUMBER = 1,			// ��ľ
	GATHER_TYPE_FLOWER = 2,			// �ɻ�

	GATHER_TYPE_MAX
};

enum WEAPON_TYPE
{
	WEAPON_TYPE_NONE,	// 0 ����
	WEAPON_TYPE_BOW,	// 1 ��
	WEAPON_TYPE_SWORD,	// 2 ��
	WEAPON_TYPE_AXE,	// 3 ��
	WEAPON_TYPE_WAND,	// 4 ��
	WEAPON_TYPE_SPEAR,	// 5 ǹ
	WEAPON_TYPE_GLOVES,	// 6 ȭ��
	WEAPON_TYPE_LANTERN,// 7 ��յ
	WEAPON_TYPE_FEATHER_FAN, // 8 ����
	WEAPON_TYPE_MAX,
};

enum ROLE_TYPE
{
	ROLE_TYPE_BENZUN = 0,		//����
	ROLE_TYPE_HUASHEN = 1,		//����

	ROLE_TYPE_MAX,
};

enum EQUIP_TYPE
{
	EQUIP_TYPE_EQUIPMENT = 0,		//װ��
	EQUIP_TYPE_JEWELRY = 1,			//����

	EQUIP_TYPE_MAX,
};

enum ShopBuyLimitType
{
	SHOP_BUY_LIMIT_TYPE_INVALID = 0,
	SHOP_BUY_LIMIT_TYPE_LIFELONG,					// �����޹�
	SHOP_BUY_LIMIT_TYPE_WEEKLY,						// ÿ���޹�
	SHOP_BUY_LIMIT_TYPE_DAILY,						// ÿ���޹�
	SHOP_BUY_LIMIT_TYPE_MAX
};

enum CurrencyType
{
	CURRENCY_TYPE_GOLD = 0,			// Ԫ��
	CURRENCY_TYPE_IMMORTAL_COIN = 1,			// �ɱ�
	CURRENCY_TYPE_COIN_BIND = 2,			// ͭ��

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

// ���ɵ�½������������sessionkey
void GetSessionKey(const std::string &key_base, int scene_id, int scene_key, int last_scene_id, 
				   int role_id, GameName role_name, unsigned int time, char first_login, PlatName pname, SessionKey key);

// �����������������֤key
unsigned int GetChatLoginKey(int uid);

// �������id��Դ����
namespace chatdef
{
	static const int MAX_CHANNEL_TYPE_COUNT = 10;
	enum ChannelType
	{
		CHANNEL_TYPE_WORLD = 0,		//����
		CHANNEL_TYPE_SCENE,			//��ǰ
		CHANNEL_TYPE_TEAM,			//����
		CHANNEL_TYPE_GUILD,			//����
		CHANNEL_TYPE_PROF,			//ְҵ
		CHANNEL_TYPE_GROUP,			//Ⱥ������
		CHANNEL_TYPE_COMPLEX,		//�ۺ�����

		CHANNEL_TYPE_MAX,
	};

	enum ChannleIDBegin
	{
		CHANNEL_ID_WORLD_BEGIN = 0,				// ����Ƶ��idΪ0
		CHANNEL_ID_CAMP_BEGIN = 100,			// ��ӪƵ��idΪ100+��Ӫ���
		CHANNEL_ID_SCENE_BEGIN = 10000,			// ����Ƶ��idΪ10000 + ������id * 10000 + SceneIndex
		CHANNEL_ID_TEAM_BEGIN = 200000,			// �������idΪ20W + team_id
		CHANNEL_ID_GUILD_BEGIN = 1000000,		// ��������idΪ100W���� ��֤�������㹻��ID
	};
}


#define LL_ONE_BIT ((long long)0x0000000000000001)

static const int ROLE_RESET_MODULE_DATA_TM_HOUR = 6;

static const int MAX_ACCOUNT_ROLE_NUM = 5;
static const int MAX_ROLE_LEVEL = 120;	
static const int MAX_ROLE_LEVEL_VERSION_LIMIT = 500;
static const int MAX_ROLE_FLOOR_LEVEL = MAX_ROLE_LEVEL / 10;
static const int MAX_PET_LEVEL = MAX_ROLE_LEVEL;			// ����ȼ�

static const int MONSTER_MAX_LEVEL = 120;

static const int TWO_BILLION = 2000000000;
static const int MAX_XIANHUN_VAL = 2000000000;	// �ɻ����ֵ 20��
static const int MAX_YUANLI_VAL = 2000000000;	// Ԫ�����ֵ 20��
static const int MAX_CROSSHONOR_VAL = 2000000000;// ����������ֵ 20��

static const int SECOND_PER_MIN = 60;			// һ������60��
static const int SECOND_PER_HOUR = 3600;		// һСʱ��3600��
static const int SECOND_PER_DAY = 86400;		// һ����86400��
static const int SECOND_PER_WEEK = 7 * SECOND_PER_DAY; // һ������
static const int RAND_UNIVERSAL_RATIO_BASE_NUM = 10000;			// ��ֱȵ���

static const long long LONG_LONG_MAX_VALUE = 9223372036854775807;// long long ���ֵ 
static const int PET_COUNT_MAX = 256;

static const int REAL_TIME_COMMENT_LEN = 2048;
static const int MAX_ROLE_CROSS_GROUP = 20;			// ���Ⱥ��
static const int MAX_ROLE_LOCAL_GROUP = 0;			// �ǿ��Ⱥ��
static const int MAX_ROLE_GROUP = MAX_ROLE_CROSS_GROUP + MAX_ROLE_LOCAL_GROUP;			// ��Ҽ���Ⱥ������

enum ELEMENT_TYPE_NAME
{
	ELEMENT_TYPE_INVALID = 0,
	ELEMENT_TYPE_WATER,						// ˮ 1
	ELEMENT_TYPE_FIRE,						// �� 2 
	ELEMENT_TYPE_WIND,						// �� 3
	ELEMENT_TYPE_EARTH,						// �� 4
	ELEMENT_TYPE_CHAOS,						// ����Ԫ�� 5
	ELEMENT_TYPE_SELF,						// ȡ�������ԣ�������Щ��Ҫ��ȡ����Ԫ�����Եļ��ܣ�6
	ELEMENT_TYPE_MAX,
};

enum BATTLE_TYPE
{
	BATTLE_TYPE_NORMAL = 0,			//!< 0 ������ս��
	BATTLE_TYPE_JOY = 1,			//!< 1 ����Ѱ��
	BATTLE_TYPE_DRAGON_BOAT = 2,	//!< 2 ���۾���
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
time_t GetWeekZeroTime(time_t cur_time);									// ��ȡ��ǰʱ���Ӧ���ܵ���һ0��ʱ���
time_t GetMonthZeroTime(time_t cur_time);									// ��ȡ��ǰʱ���Ӧ���µ�һ��0��ʱ���
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
	SYS_MSG_FLOAT = 0,			 // Ʈ��	
	SYS_MSG_ROLLING_NOTICE = 1,  // �������棨����ƣ�
	SYS_MSG_SYSTEM_CHAT = 2,     // ϵͳ������
	SYS_MSG_RECRUIT = 3,		 // ��ļ��
	SYS_MSG_WORLD_CHAT = 4,		 // ����������
	SYS_MSG_PROF_CHAT = 5,		 // ְҵƵ��
};

enum FORBID_TALK_TYPE
{
	FORBID_TALK_TYPE_DEFAULT = 0,		//Ĭ��(��ͨ����ģʽ)(����ʾ)
	FORBID_TALK_TYPE_SILENCE = 1,		//��Ĭ����ģʽ(���Լ��ɿ���)

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

enum PROF_TYPE																	// ְҵ����
{
	PROF_TYPE_PROF_NO_LIMIT = -1,												// �����ڴ�������ְҵ�����ܽӵ�����ȣ�
	PROF_TYPE_INVALID = 0,														// ���� 
	
	// ְҵ��100Ϊ��λ��������λ��Ϊְҵ�����˵Ĵ���																			
	PROF_TYPE_GLADIATOR = 100,													// ְҵ1 ��ʿ
	PROF_TYPE_SWORDSMAN = 200,													// ְҵ2 ��ʿ
	PROF_TYPE_AXE_FIGHTER = 300,												// ְҵ3 ս����ʿ
	PROF_TYPE_KNIGHT = 400,														// ְҵ4 ��ʿ
	PROF_TYPE_ARCHER = 500,														// ְҵ5 ������
	PROF_TYPE_MAGICIAN = 600,													// ְҵ6 ħ��ʦ
	PROF_TYPE_MISSIONARY = 700,													// ְҵ7 ����ʿ
	PROF_TYPE_WIZZARD = 800,													// ְҵ8 ��ʦ

	PROF_TYPE_MAX = 900,																
};
static const int PROFESSION_BASE = 100;
static const int PROFESSION_TYPE_NUM = PROF_TYPE_MAX / PROFESSION_BASE;
static const int PROFESSION_ADVANCE_TIMES = 6;									// ְҵ��������

int GetBaseProf(int profession);

//������profession >= PROFESSION_BASE,����GetBaseProfrofession
//������profession = (0, PROFESSION_BASE) ����profession * PROFESSION_BASE;
int CompatibleGetProfType(int profession);

int GetAdvanceTimes(int profession);

enum AVATAR_TYPE					// ������������
{
	AVATAR_TYPE_INVALID,													
	AVATAR_TYPE_1,					// 1.���վ� (����)
	AVATAR_TYPE_2,					// 2.��ż   (����)
	AVATAR_TYPE_3,					// 3.è���� (С���)
	AVATAR_TYPE_4,					// 4.Ϳ��   (����)
	AVATAR_TYPE_5,					// 5.����   (����)
	AVATAR_TYPE_6,					// 6.����   (��С��)
	AVATAR_TYPE_MAX,	
};

static const int SEX_ARRAY[AVATAR_TYPE_MAX] = { FEMALE, FEMALE, FEMALE, FEMALE, MALE, MALE, MALE };
				
static const int APPEARANCE_BASE = 10000;
static const int INIT_APPREARANCE_COLOR_NUM = 4;

// ��ʼ���� init_apprearance = avatar_type * APPEARANCE_BASE + color

enum DISCONNECT_NOTICE_TYPE
{
	DISCONNECT_NOTICE_TYPE_INVALID,
	DISCONNECT_NOTICE_TYPE_LOGIN_OTHER_PLACE,									// ����ڱ𴦵�¼
	DISCONNECT_NOTICE_TYPE_CLIENT_REQ,											// �ͻ�������
};

// ȫ��ϵͳ����״̬ 
enum GLOBAL_SYSTEM_DATA_STATE
{
	GLOBAL_SYSTEM_DATA_STATE_INVALID = 0,						// ��ʼ״̬
	GLOBAL_SYSTEM_DATA_STATE_LOADING,							// ���ݼ����� 
	GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH,						// ���ݼ������ 
	GLOBAL_SYSTEM_DATA_STATE_MAX,
};

static const int ROLE_ENERGY_MAX = 1000;										// �������ֵ

static const int BIRTH_MAX_HP_BASE = 200;										// ��ʼѪ��
static const int BIRTH_MAX_MP_BASE = 50;										// ��ʼħ��
static const int BIRTH_BAOJI_BASE = 5;											// ��ʼ����
static const int BIRTH_JIANREN_BASE = 10;										// ��ʼ����
static const int BIRTH_MOVESPEED_BASE = 1200;									// ��ʼ�ٶ�

static const int BIRTH_FIRST_ATTR_LILIANG = 5;									// һ������ ����
static const int BIRTH_FIRST_ATTR_TIZHI = 5;									// һ������ ����
static const int BIRTH_FIRST_ATTR_JINGZHUN = 5;									// һ������ ��׼
static const int BIRTH_FIRST_ATTR_MINJIE = 5;									// һ������ ����

static const ItemID EXP_ITEM_ID = ItemID(-1);									// �������ƷID
static const ItemID GOLD_ITEM_ID = ItemID(-2);									// Ԫ������ƷID
static const ItemID IMMORTAL_COIN_ITEM_ID = ItemID(-3);							// �ǰ�ͭǮ��ƷID
static const ItemID COIN_ITEM_ID = ItemID(-4);									// ��ͭ����ƷID

static const int BIT_COUNT_OF_BYTE = 8 * sizeof(unsigned char);					// ���ֽڴ�С��λ��
static const int BIT_COUNT_OF_LONG_64 = 8 * BIT_COUNT_OF_BYTE;					// longlong �ʹ�С��λ��

const static int COMMON_SCENE_KEY = 0;											// ͨ�ó���key, Ұ�ⳡ�����Ǹ�����
const static int COMMON_FB_KEY = 1;												// ͨ�ø���key������������ҹ���һ�����������

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
typename MapType::iterator MapAdd(MapType & m, const MapKeyType & k, const MapValueType & v) //û�в����
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

// �ַ������� תlong long��Ϊ������LINUX�µľ���
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
// ������������ᴥ��valgrind��invalid read 
// �����ַ������鿽��
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

// �ַ�����ʽ����������
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

static const int MAX_TEAM_MEMBER_NUM = 5;						// ���������5����

enum TEAM_ASSIGN_MODE
{	
	TEAM_ASSIGN_MODE_INVALID = 0,
	TEAM_ASSIGN_MODE_RANDOM,									// �������ģʽ
	TEAM_ASSIGN_MODE_TAKE_ROUND,								// ��������ģʽ
	TEAM_ASSIGN_MODE_MAX
};

enum WEEKDAY
{
	SUNDAY = 0,													// ����
	MONDAY,														// ��һ
	TUESDAY,													// �ܶ�
	WEDNESDAY,													// ����
	THURSDAY,													// ����
	FRIDAY,														// ����
	SATURDAY,													// ����

	WEEKDAY_COUNT												// ÿ������
};

static const int CHANNEL_CD_TIME_WORLD = 10;					// ����Ƶ����ȴʱ�� s
static const int CHANNEL_CD_TIME_CAMP = 10;						// ����Ƶ����ȴʱ��
static const int CHANNEL_CD_TIME_SCENE = 10;					// ����Ƶ����ȴʱ��
static const int CHANNEL_CD_TIME_TEAM = 0;						// ���Ƶ����ȴʱ��
static const int CHANNEL_CD_TIME_GUILD = 0;						// ����Ƶ����ȴʱ��

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

	std::string ipstr;			// IP�ַ���
	unsigned short port;		// �˿�
};

static const int SCENE_MONSTER_GENERATER_MAX_NUM = 20;									// �����ڹ������ɵ��������

struct ClockReturnDayItem // AngineAdapter��m_clock->DayID()�ķ���ֵ����
{
	unsigned short year;
	unsigned char  mon;
	unsigned char  day;
};

int CalcDayIndexWithClockDayItem(ClockReturnDayItem *day1, ClockReturnDayItem *day2); // ����day1��day2������������0����ͬһ�죬����Ϊ����

int GetGameNameHash(const GameName str);
int GetStringHash(int max_len, const char *str);
int GetStringHash(const char* str);
int GetNumberOfOne(int n);

// ���ڼ򻯶�ȡ���ñ����
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

// ����Խ��ļӷ���Tֻ���ǣ�int��short��char��ȡֵ��Χ��[0, max_val]
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
	RETRIEVE_REASON_REFUND = 0,						// �˿�
	RETRIEVE_REASON_BUG = 1,						// ����ˢ��ԴBUG���쳣�������Ҫ������Դ
	RETRIEVE_REASON_IN_DEBT = 2,                    // ͸֧
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
void RandomShuffle(T* t_list, int t_num) // ��������Ԫ�����ϴ��
{
	for (int i = t_num - 1; i > 0; --i)
	{
		std::swap(t_list[i], t_list[rand() % (i + 1)]);
	}
}

template<typename T>
void RandomShuffle(T* t_list, int t_begin, int t_end) // ��������Ԫ�����ϴ�� [t_begin, t_end)
{
	for (int i = t_end - 1; i > t_begin; --i)
	{
		std::swap(t_list[i], t_list[t_begin + rand() % (i - t_begin + 1)]);
	}
}

template<typename T>
void RandomShuffle(T* t_list, int t_begin, int t_end, unsigned int rand_seed) //ָ��������� ��������Ԫ�����ϴ�� [t_begin, t_end)
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

// ��ֹʵ�����������ƣ����ⲿ����
#define INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(ClassName) \
	private:\
		ClassName();\
		virtual ~ClassName();\
		ClassName(const ClassName&);\
		ClassName& operator= (const ClassName&);

// ��ֹʵ������������ ֻ���������壬��������ʱ����
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
	short attr_type;					//!< ��������
	short attr_value;					//!< ������ֵ
};

struct AttrVocInfo						//!< װ������
{
	short un_lock;						//!< �Ƿ����
	short reserve_sh;
	short attr_type;					//!< ��������
	short attr_value;					//!< ������ֵ
};

const char * ItemDataWrapperLog(const ItemDataWrapper *item_wrapper, int index = 0);		// ����ʹ����Ϊ�������� Ҫ������ С��LOG_MAX_ITEM_NUM
const char * MailVirtualItemLog(const MailVirtualItemList *param);
const char * MailParamLog(const MailParam *param);
const char * MailParamSimpleLog(const MailParam *param);
void MergeItemConfigData(std::vector<ItemConfigData> ARG_IN_AND_OUT  & list);
void MergeItemDataWrapper(std::vector<ItemDataWrapper> ARG_IN_AND_OUT & list);


enum RELOAD_CONFIG_TYPE
{
	RELOAD_CONFIG_TYPE_INVALID = 0,
	RELOAD_CONFIG_TYPE_GLOBAL_CONFIG,								//!< 1 ȫ��
	RELOAD_CONFIG_TYPE_SKILL,										//!< 2 ����
	RELOAD_CONFIG_TYPE_TASK,										//!< 3 ����
	RELOAD_CONFIG_TYPE_MONSTER,										//!< 4 ����
	RELOAD_CONFIG_TYPE_ITEM,										//!< 5 ��Ʒ
	RELOAD_CONFIG_TYPE_LOGIC_CONFIG,								//!< 6 �߼�
	RELOAD_CONFIG_TYPE_DROP,										//!< 7 ���� ����ʱ���Σ�
	RELOAD_CONFIG_TYPE_SHIZHUANG,									//!< 8 ʱװ ��û�����ã�
	RELOAD_CONFIG_TYPE_SCENE,										//!< 9 ���� 
	RELOAD_CONFIG_TYPE_SHARED,										//!< 10 ���� ����ʱ���Σ�
	RELOAD_CONFIG_TYPE_ONE_ITEM,									//!< 11 ������Ʒ ����ʱ���Σ�
	RELOAD_CONFIG_TYPE_ACTIVITY,									//!< 12 �
	RELOAD_CONFIG_TYPE_BATTLE,										//!< 13 ս��
	RELOAD_CONFIG_TYPE_SPID_CONFIG,									//!< 14 �������� (ͬʱ�ȸ���¼����SpidConfig����Ϸ����GlobalConfig)
	RELOAD_CONFIG_TYPE_TRADE_MARKET,								//!< 15 ����������
	RELOAD_CONFIG_TYPE_NPC,											//!< 16 NPC
	RELOAD_CONFIG_TYPE_NOTICE_CONFIG,								//!< 17 ����
	RELOAD_CONFIG_TYPE_APPEARANCE_CONFIG,							//!< 18 ���
	RELOAD_CONFIG_TYPE_LOGIC_CONFIG_ASYNC,							//!< 19 �߼����첽��
	RELOAD_CONFIG_TYPE_SPECIAL_EFFECT,								//!< 20 ��Ч
	RELOAD_CONFIG_TYPE_ONE_MONSTER,									//!< 21 �ض���������
	RELOAD_CONFIG_TYPE_SPECIAL_MONSTER,								//!< 22 �ض��������
	RELOAD_CONFIG_TYPE_ROBOT,										//!< 23 ������
	RELOAD_CONFIG_TYPE_GOD_EQUIP,									//!< 24 ��װ
	RELOAD_CONFIG_TYPE_BATTLE_HALLOW,								//!< 25 ս��ʥ��
	RELOAD_CONFIG_TYPE_ASYNC_BATTLE_MONSTER,						//!< 26 ����첽��
	RELOAD_CONFIG_TYPE_BATTLE_OTHER,								//!< 27 ս����������
	RELOAD_CONFIG_TYPE_MAX,
};

/**
* @brief �ж�flag�ĵ�bit_indexλ��1����0
* @param flag һ��[0,MAX_INT)��ֵ
* @param bit_index ��Ҫ���flag����һλ
* @return ����true����flag�ĵ�bit_indexλ��1��������0
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

////////////////////////////////////ƥ���߼�/////////////////////////////////////////////////////

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
	SURFACE_TYPE_BODY = 1,			// ����
	SURFACE_TYPE_ARM,				// ����
	SURFACE_TYPE_WING,				// ����
	SURFACE_TYPE_MOUNT,				// ����
	SURFACE_TYPE_RESERVE,			// ����
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

	char prof_base;			//!< ְҵ[0,9)
	char advance_times;		//!< ���״���[0,6]
	char avatar_type;		//!< ��ɫģ��[1,6]
	char color;				//!< ��ɫ[0,4)
	ItemID weapon_id;		//!< ����ID
	ItemID sub_weapon_id;	//!< ������ID

	short headshot_id;		//!< ͷ��ID 
	short special_appearance_type;	//!< ������������
	int special_appearance_param;	//!< �����������
	
	short wear_surface[SURFACE_TYPE_MAX];		//!< ��װ���ĵĻû���Դid �����±�Ϊ���ñ��type

	short smart_mounts_index;			//��ǰ��˵����� -1:δ���
	char is_use_advanced_flag;			//��������ʹ�ñ�ʶ 0:δʹ�ý������� 1:ʹ�ý�������
	char is_use_change_card;			//�Ƿ�ʹ�ñ������� 0:��ʹ�� 1:ʹ�ñ�������
	short change_card;					//����(seq)
	short fly_flag;						//�����׶�

	int zaohua_app_id;			//!< �컯װ�����
	char reserve_ch;
	char is_use_huanshou;		//�Ƿ�ʹ�û������� 0:��ʹ�� 1:ʹ�û�������
	short huanshou_fushen_id;	//���޸���ID (-1:δ����)
	long long reserve_ll;		//!< ������  ������չ
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

int IntAddOk(int  a, int  b, int & out_sum); //int ����ж�
void IntHelpAdd(int ARG_IN_AND_OUT & curr_value, int max_value, int add_value, int min_value = 0);

/**
* \brief ͬ��д�ļ�
* \param path Ŀ¼ ����Ҫȷ��Ŀ¼�Ѵ��ڣ�
* \param data Ҫд�������
* \param length Ҫд�����ݵĳ���
* \param is_append �Ƿ�׷��д������
* \param is_binary �Ƿ�д�����������
* \return �����Ƿ�ɹ�д������
* \warning ��Ҫȷ��Ŀ¼�Ѵ���
* \remarks IFileWriteModule�ṩ��write�ӿ����첽д�ı��ļ�����windows������ÿ��0x0A������ǰ���һ��0x0D,
           ����д���������ݽ��ļ�ʱ�����������ƫ������⣬������ʱ��windows��ʹ��ͬ��д�ļ�
*/
bool WriteFile(const char *path, const char *data, int length, bool is_append, bool is_binary);

/**
* \brief ����Ŀ¼
* \param folder Ҫ������Ŀ¼
* \return �Ƿ�ɹ�����
* \note �ú������Դ����༶Ŀ¼
*/
bool CreateDirectory(const std::string& folder);

std::string PrintStackBackTrace(bool is_dump_core);

bool CheckNameHasServerSuffix(const std::string& name);
/**
* \brief ����ȫ�����е�һ���������
* \param start_number ��ʼ��
* \param end_number �ս��� ��ǰ�պ󿪣�
* \return ����һ��������У������е�����Χ��[start_number,end_number)
*	����ֵΨһ��������ΪN=end_number - start_number
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


// ������Ŀ¼·�������ҵ��汾�Ŵ�
bool GetVersionStrFromPathStr(std::string* version_str, const std::string& path);

bool GetVersionStr(std::string* version_str);

std::string GetCurrentWorkingDirectory();

bool IsStringAllNumber(const std::string& str);
bool IsStringMatchVersionStr(const std::string& version_str);

#endif // __SERVERCOMMON_H__