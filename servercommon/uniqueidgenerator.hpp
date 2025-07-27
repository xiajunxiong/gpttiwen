#ifndef __UNIQUE_ID_GENERATOR__
#define __UNIQUE_ID_GENERATOR__

#include "servercommon/servercommon.h"
#include <set>

enum SERVER_TYPE
{
	SERVER_TYPE_INVALID = 0,
	LOGINSERVER,
	GATEWAY,
	GAMEWORLD,
	DATAACCESS,
	CROSSSERVER,
	BATTLESERVER,
	TELNETCLIENT,
	BIGCROSSSERVER,
	SERVER_TYPE_MAX,
};
UNSTD_STATIC_CHECK(SERVER_TYPE_MAX < 16);


//!< 开始时间戳 (2020-01-01)
static const long long ZERO_TIMESTAMP = 1577808000LL;

//!< 服务器类型位数，支持类型[0,16)
static const long long SERVER_TYPE_BIT = 4LL;

//!< 服务器ID位数, 支持服务器ID[0,4096)
static const long long SERVER_ID_BIT = 12LL;

//!< 实例index位数，支持index[0,16)
static const long long SERVER_INDEX_BIT = 4LL;

//!< 时间戳位数,以秒为单位，支持从[2020-01-01, 2037-01-04)  
static const long long TIMESTAMP_BIT = 29LL;

//!< 自增ID位数，支持自增ID范围[0, 8192) 即每秒最多16384个ID
static const long long INC_ID_BIT = 14LL;

static const long long SIGN_BIT = 1LL;

union UniqueIDUnion
{
	struct 
	{
		long long inc_id : INC_ID_BIT;
		long long timestamp_diff : TIMESTAMP_BIT;
		long long server_index : SERVER_INDEX_BIT;
		long long server_id : SERVER_ID_BIT;
		long long server_type : SERVER_TYPE_BIT;
		long long sign_bit : SIGN_BIT;
	};

	long long unique_id;
};
UNSTD_STATIC_CHECK(8 == sizeof(UniqueIDUnion));

// 必须保留1位作为符号位
UNSTD_STATIC_CHECK(64 == INC_ID_BIT + TIMESTAMP_BIT + SERVER_INDEX_BIT + SERVER_ID_BIT + SERVER_TYPE_BIT + SIGN_BIT);

class IClockModule;
class UniqueIDGenerator
{
public:
	//!< 自增ID左移0位
	static const long long INC_ID_SHIFT = 0LL;

	//!< 时间戳左移14位
	static const long long TIMESTAMP_SHIFT = INC_ID_SHIFT + INC_ID_BIT;

	//!< 实例index左移14+30位
	static const long long SERVER_INDEX_SHIFT = TIMESTAMP_SHIFT + TIMESTAMP_BIT;

	//!< 服务器ID左移14+30+4位
	static const long long SERVER_ID_SHIFT = SERVER_INDEX_SHIFT + SERVER_INDEX_BIT;

	//!< 服务器类型左移14+30+4+11位
	static const long long SERVER_TYPE_SHIFT = SERVER_ID_SHIFT + SERVER_ID_BIT;

	UNSTD_STATIC_CHECK(SERVER_TYPE_SHIFT + SERVER_TYPE_BIT == 63);
public:
	UniqueIDGenerator();
	~UniqueIDGenerator();

	static inline bool CheckUniqueIDValid(long long unique_id) { return 0 != unique_id; }
	static UniqueIDGenerator* Instance();
	bool Init(SERVER_TYPE server_type, int server_id, int server_index, IClockModule* clock);

	bool GenUniqueID(ARG_OUT long long* unique_id);
private:
	
	bool m_has_init;					//!< 是否已初始化
	SERVER_TYPE m_server_type;			//!< 服务器类型
	int m_server_id;					//!< 服务器id
	int m_server_index;					//!< 服务器index （同一个服务器id可能存在多个实例）
	long long m_last_gen_time;			//!< 上次生成uuid的时间戳
	int m_inc_id;						//!< 自增id
	IClockModule* m_clock;				//!< 用于获取时间戳

#ifdef _DEBUG
	std::set<long long> m_history_unique_id_set;
#endif
};

#endif