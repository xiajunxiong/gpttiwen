#ifndef __TRADE_MARKET_PARAM_HPP__
#define __TRADE_MARKET_PARAM_HPP__

#include "servercommon/struct/itemlistparam.h"

enum SALE_ITEM_STATUS
{
	SALE_ITEM_STATUS_INVALID = 0,
	SALE_ITEM_STATUS_ON_SHELVE = 1,		//!< 1 上架
	SALE_ITEM_STATUS_ANNOUNCEMENT = 2,	//!< 2 公示
	SALE_ITEM_STATUS_ON_SALE = 3,		//!< 3 开售
	SALE_ITEM_STATUS_OFF_SHELVE = 4,	//!< 4 下架
	SALE_ITEM_STATUS_MAX = 5,
};

enum TRADEMARKET_ORDER_TYPE
{
	TRADEMARKET_ORDER_TYPE_INVALID = 0,
	TRADEMARKET_ORDER_TYPE_BUY = 1,
	TRADEMARKET_ORDER_TYPE_UPSHELVE = 2,
	TRADEMARKET_ORDER_TYPE_ADVERTISE = 3,
	TRADEMARKET_ORDER_TYPE_MAX
};

enum TRADEMARKET_ORDER_STATUS
{
	TRADEMARKET_ORDER_STATUS_BEGIN = 0,
	TRADEMARKET_ORDER_STATUS_QUERY = 1,
	TRADEMARKET_ORDER_STATUS_CONSUME = 2,
	TRADEMARKET_ORDER_STATUS_MAX
};

static const int USER_SALE_ITEM_MAX_NUM = 8;
static const int USER_TRADE_INFO_SAVE_DB_INTERVAL_S = 60;
static const int USER_TRADE_INFO_CHECK_STATUS_INTERVAL_S = 1;
static const int USER_TRADE_INFO_SAVE_RECORD_INTERVAL_S = 60;
static const int TRADE_ITEM_ANNOUNCE_TIME_S = 1 * 60 * 60;
static const int TRADE_ITEM_ON_SALE_TIME_S = 24 * 60 * 60;
static const int SEARCH_SALE_ITEM_PER_PAGE = 8;
static const int SEARCH_PET_QUICK_BUY_ITEM_NUM = 10;
static const int TRADE_MARKET_PERCENT_NUM = 1000;
static const int TRANSACTION_RECORD_MAX_NUM = 10;
static const int TRADE_MARKET_USER_FOCUS_MAX_NUM = 16;
static const int TRADE_MARKET_UPDATE_FOCUS_INTERVAL_S = 60;
static const int ROLE_TRANSACTION_RECORD_MAX_NUM = 10;
static const int SYSTEM_TRANSACTION_RECORD_SAVE_INTERVAL_S = 60;
static const int SYSTEM_TRANSACTION_RECORD_FASTER_SAVE_INTERVAL_S = 10;
static const int TRADE_ITEM_SEARCH_TYPE_MAX = 300;
static const int TRADE_ITEM_SEARCH_BIG_TYPE_MAX = 20;
static const int TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM = 23; // 从[0,23)的搜索类型可以按等级搜索
static const int TRADE_MEDAL_SEARCH_TYPE_MIN = 229;	// 特殊逻辑 搜索令牌时特殊处理
static const int TRADE_MEDAL_SEARCH_TYPE_MAX = 233; // 特殊逻辑 搜索令牌时特殊处理
static const int TRADE_ITEM_LEVEL_MAX = 13;
static const int TRADE_PET_ITEM_ID_BEGIN = 50000;
static const int TRADE_PET_ITEM_ID_END = 60000;
static const int SYSTEM_TRADE_UID = 1;

#pragma pack(push)
#pragma pack(4)

struct TransactionInfo
{
	TransactionInfo() : item_id(0), item_num(0), price_type(0), reserve_sh(0), price(0) 
	{
		this->Reset();
	}

	void Reset()
	{
		item_id = 0;
		item_num = 0;
		price_type = 0;
		reserve_sh = 0;
		price = 0;
		wrapper.Reset();
	}

	ItemID item_id;			//!< 物品ID
	short item_num;			//!< 交易数量
	short price_type;		//!< 价格类型
	short reserve_sh;		//!< 保留字段
	int price;				//!< 单价
	ItemDataWrapper wrapper;//!< 物品详细数据
};

struct RoleTransactionRecordParam
{
	RoleTransactionRecordParam() { this->Reset(); }

	void Reset()
	{
		sold_num_OBSOLETE = 0;
		buy_num_OBSOLETE = 0;
		sold_record_head_OBSOLETE = 0;
		sold_record_tail_OBSOLETE = 0;
		buy_record_head_OBSOLETE = 0;
		buy_record_tail_OBSOLETE = 0;
		total_sold_gold = 0LL;
		total_buy_gold = 0LL;
		memset(reserved_OBSOLETE, 0, sizeof(reserved_OBSOLETE));
	}

	short sold_num_OBSOLETE;
	short buy_num_OBSOLETE;
	short sold_record_head_OBSOLETE;
	short sold_record_tail_OBSOLETE;
	short buy_record_head_OBSOLETE;
	short buy_record_tail_OBSOLETE;
	long long total_sold_gold;
	long long total_buy_gold;
	char reserved_OBSOLETE[240];
};

typedef char RoleTransactionRecordParamHex[sizeof(RoleTransactionRecordParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleTransactionRecordParamHex) < 1024);

struct NewUserTradeRecordParam
{
	NewUserTradeRecordParam() : next_slot_idx(0), record_num(0)
	{
		this->Reset();
	}

	void Reset()
	{
		next_slot_idx = 0;
		record_num = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));

		for (int i = 0; i < ARRAY_ITEM_COUNT(record_list); ++i)
		{
			record_list[i].Reset();
		}
	}

	short next_slot_idx;
	short record_num;
	long long reserve_ll[4];
	TransactionInfo record_list[ROLE_TRANSACTION_RECORD_MAX_NUM];
};

typedef char NewUserTradeRecordParamHex[sizeof(NewUserTradeRecordParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NewUserTradeRecordParamHex) < 22000);

struct RecycleRuleParam
{
	RecycleRuleParam() : rule_id(0), begin_timestamp(0u), end_timestamp(0u),
		recycle_num_limit(0), recycle_price_limit(0), item_id(0), reserve_sh(0),
		cur_recycle_num(0)
	{
		memset(data, 0, sizeof(data));
	}

	void Reset()
	{
		rule_id = 0;
		begin_timestamp = 0;
		end_timestamp = 0;
		recycle_price_limit = 0;
		recycle_num_limit = 0;
		item_id = 0;
		reserve_sh = 0;
		cur_recycle_num = 0;
		memset(data, 0, sizeof(data));
	}

	bool IsValid()
	{
		return rule_id > 0;
	}

	long long rule_id;
	unsigned int begin_timestamp;
	unsigned int end_timestamp;
	int recycle_num_limit;
	int recycle_price_limit;
	ItemID item_id;
	short reserve_sh;
	int cur_recycle_num;
	RecycleRuleDetailData data;
};

typedef char RecycleRuleDetailDataHex[sizeof(RecycleRuleDetailData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RecycleRuleDetailDataHex) < 1024);


struct AutoUpshelveParam
{
	AutoUpshelveParam() : item_id(0), upshelve_num(0),
		upshelve_price(0), upshelve_interval_m(0), auto_offshelve_time_m(0),
		auto_upshelve_limit_num(0), price_float_percent(0), price_float_limit_percent(0),
		begin_timestamp(0u), end_timestamp(0u), unique_id(0u)
	{
		this->Reset();
	}

	void Reset()
	{
		item_id = 0;
		upshelve_num = 0;
		upshelve_price = 0;
		upshelve_interval_m = 0;
		auto_offshelve_time_m = 0;
		auto_upshelve_limit_num = 0;
		price_float_percent = 0;
		price_float_limit_percent = 0;
		begin_timestamp = 0u;
		end_timestamp = 0u;
		unique_id = 0u;
		stop_upshelve_world_level = 0;
		start_upshelve_world_level = 0;
	}

	ItemID item_id;					//!< 上架物品ID
	short upshelve_num;				//!< 上架物品数量
	int upshelve_price;				//!< 上架价格
	int upshelve_interval_m;		//!< 上架时间间隔（分钟）
	int auto_offshelve_time_m;		//!< 自动下架时间（分钟）
	int auto_upshelve_limit_num;	//!< 自动补货上限（当交易行内该物品数量超过该数时，不补货）
	short price_float_percent;		//!< 每次价格浮动百分比（卖出则升价，无人买则降价）
	short price_float_limit_percent;//!< 价格浮动上下限
	unsigned int begin_timestamp;	//!< 开始时间戳
	unsigned int end_timestamp;		//!< 结束时间戳
	long long unique_id;			//!< 规则id
	int	stop_upshelve_world_level;	//!< 停止上架的世界等级
	int start_upshelve_world_level;	//!< 开始上架的世界等级
};

#pragma pack(pop)

struct SaleItemParam
{
	SaleItemParam()
	{
		this->Reset();
	}

	void Reset()
	{
		is_need_announcement = false;
		item_search_type = 0;
		sale_index = 0;
		immortal_coin_price = 0;
		gold_price = 0;
		sold_num = 0;
		cur_status = SALE_ITEM_STATUS_INVALID;
		next_status_time = 0u;
		put_on_shelve_time = 0u;
		num_per_group = 1;
		item_wrapper.Reset();

		system_buy_minute_num = 0;
		system_offshelve_minute_num = 0;
		system_buy_timestamp = 0u;
		system_offshelve_timestamp = 0u;
	}

	bool is_need_announcement;		//!< 是否需要公示
	short item_search_type;			//!< 物品搜索类型（用于做索引）
	int sale_index;					//!< 该商品在玩家售卖列表中的序号
	int immortal_coin_price;		//!< 单价  仙币
	int gold_price;					//!< 单价  元宝
	int num_per_group;				//!< 按组出售，每组数量
	int sold_num;					//!< 已售出数量
	int cur_status;					//!< 当前商品状态 \see SALE_ITEM_STATUS
	unsigned int next_status_time;	//!< 下次转换状态的时间戳
	unsigned int put_on_shelve_time;//!< 上架时间（用于排序）
	long long unique_key;			//!< 一个唯一key, 用于日志
	ItemDataWrapper item_wrapper;	//!< 物品数据

	short system_buy_minute_num;		//!< 系统自购时长（分钟）
	short system_offshelve_minute_num;//!< 系统下架时长（分钟）
	unsigned int system_buy_timestamp; //!< 系统自购时间戳（发售时计算好）
	unsigned int system_offshelve_timestamp; //!< 系统自下架时间戳（发售时计算好）
};

class TradeMarketData
{
public:
	bool Serialize(TLVSerializer& outsteam) const;
	bool UnSerialize(TLVUnserializer& instream);

	struct SaleItem
	{
		long long id_saleitem;		// 仅仅初始化的时候有用
		char change_state;

		char is_focus;				// 是否为关注列表的数据
		short index;				// 在列表中的序号 (可能是售卖列表或关注列表）
		int owner_uid;				// 数据属于哪个玩家

		long long unique_key;		// 唯一key
		
		// 以下是详细数据
		int seller_uid;				//!< 售卖者uid
		GameName seller_name;		//!< 售卖者名称
		SaleItemParam salitem_param;//!< 货品数据
	};

	static const int MAX_SALE_ITEM_COUNT = 256;

	bool is_system_shelve;
	int count;
	SaleItem sale_list[MAX_SALE_ITEM_COUNT];
};

class UserTradeRecordData
{
public:
	bool Serialize(TLVSerializer& outstream) const;
	bool UnSerialize(TLVUnserializer& instream);

	struct UserRecord
	{
		long long id; // 只在读数据时有用
		int role_id;
		bool old_state; // 为false时Insert, 否则Update
		RoleTransactionRecordParam other_param;
		NewUserTradeRecordParam sold_record;
		NewUserTradeRecordParam buy_record;
	};

	static const int MAX_USER_RECORD_NUM = 64;

	int count;
	UserRecord record_list[MAX_USER_RECORD_NUM];
};



struct SystemTransactionInfo
{
	SystemTransactionInfo() : item_id(0), item_num(0), price_type(0), data_len(0), price(0) {}

	void Reset()
	{
		item_id = 0;
		item_num = 0;
		price_type = 0;
		data_len = 0;
		price = 0;
		memset(data, 0, sizeof(data));
	}

	ItemID item_id;			//!< 物品ID
	short item_num;			//!< 交易数量
	short price_type;		//!< 价格类型 
	short data_len;			//!< 净值数据长度
	int price;				//!< 单价
	NetValueItemParamData data;	//!< 净值数据
};
class SystemTradeRecord
{
public:
	bool Serialize(TLVSerializer& outstream) const;
	bool UnSerialize(TLVUnserializer& instream);

	struct SystemRecord
	{
		long long id;						// 只在读数据时有用
		bool old_state;						// 为false时Insert, 否则Update
		short index;						// 在列表中的序号
		unsigned int transaction_time;		// 交易时间
		SystemTransactionInfo ti;			// 详细交易信息
	};

	static const int MAX_SYSTEM_RECORD_NUM = 256;

	int count;
	SystemRecord record_list[MAX_SYSTEM_RECORD_NUM];
};

class SystemRecycleRuleList
{
public:
	bool Serialize(TLVSerializer& outstream) const;
	bool UnSerialize(TLVUnserializer& instream);

	struct RecycleRuleData
	{
		long long id;						// 只在读数据时有用
		char change_state;

		long long rule_id;					// 唯一标识
		RecycleRuleParam param;				
	};

	static const int MAX_RULE_NUM = 256;

	int count;
	RecycleRuleData rule_list[MAX_RULE_NUM];
};

class AutoUpshelveDataList
{
public:
	bool Serialize(TLVSerializer& outstream) const;
	bool UnSerialize(TLVUnserializer& instream);

	struct AutoUpshelveData
	{
		long long id;						// 只在读数据时有用
		char change_state;

		long long unique_id;
		AutoUpshelveParam param;
	};

	static const int MAX_AUTO_UPSHELVE_ITEM_NUM = 256;

	int count;
	AutoUpshelveData data_list[MAX_AUTO_UPSHELVE_ITEM_NUM];
};

#endif