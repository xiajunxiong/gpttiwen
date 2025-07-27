#ifndef __TRADE_MARKET_PARAM_HPP__
#define __TRADE_MARKET_PARAM_HPP__

#include "servercommon/struct/itemlistparam.h"

enum SALE_ITEM_STATUS
{
	SALE_ITEM_STATUS_INVALID = 0,
	SALE_ITEM_STATUS_ON_SHELVE = 1,		//!< 1 �ϼ�
	SALE_ITEM_STATUS_ANNOUNCEMENT = 2,	//!< 2 ��ʾ
	SALE_ITEM_STATUS_ON_SALE = 3,		//!< 3 ����
	SALE_ITEM_STATUS_OFF_SHELVE = 4,	//!< 4 �¼�
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
static const int TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM = 23; // ��[0,23)���������Ϳ��԰��ȼ�����
static const int TRADE_MEDAL_SEARCH_TYPE_MIN = 229;	// �����߼� ��������ʱ���⴦��
static const int TRADE_MEDAL_SEARCH_TYPE_MAX = 233; // �����߼� ��������ʱ���⴦��
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

	ItemID item_id;			//!< ��ƷID
	short item_num;			//!< ��������
	short price_type;		//!< �۸�����
	short reserve_sh;		//!< �����ֶ�
	int price;				//!< ����
	ItemDataWrapper wrapper;//!< ��Ʒ��ϸ����
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

	ItemID item_id;					//!< �ϼ���ƷID
	short upshelve_num;				//!< �ϼ���Ʒ����
	int upshelve_price;				//!< �ϼܼ۸�
	int upshelve_interval_m;		//!< �ϼ�ʱ���������ӣ�
	int auto_offshelve_time_m;		//!< �Զ��¼�ʱ�䣨���ӣ�
	int auto_upshelve_limit_num;	//!< �Զ��������ޣ����������ڸ���Ʒ������������ʱ����������
	short price_float_percent;		//!< ÿ�μ۸񸡶��ٷֱȣ����������ۣ��������򽵼ۣ�
	short price_float_limit_percent;//!< �۸񸡶�������
	unsigned int begin_timestamp;	//!< ��ʼʱ���
	unsigned int end_timestamp;		//!< ����ʱ���
	long long unique_id;			//!< ����id
	int	stop_upshelve_world_level;	//!< ֹͣ�ϼܵ�����ȼ�
	int start_upshelve_world_level;	//!< ��ʼ�ϼܵ�����ȼ�
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

	bool is_need_announcement;		//!< �Ƿ���Ҫ��ʾ
	short item_search_type;			//!< ��Ʒ�������ͣ�������������
	int sale_index;					//!< ����Ʒ����������б��е����
	int immortal_coin_price;		//!< ����  �ɱ�
	int gold_price;					//!< ����  Ԫ��
	int num_per_group;				//!< ������ۣ�ÿ������
	int sold_num;					//!< ���۳�����
	int cur_status;					//!< ��ǰ��Ʒ״̬ \see SALE_ITEM_STATUS
	unsigned int next_status_time;	//!< �´�ת��״̬��ʱ���
	unsigned int put_on_shelve_time;//!< �ϼ�ʱ�䣨��������
	long long unique_key;			//!< һ��Ψһkey, ������־
	ItemDataWrapper item_wrapper;	//!< ��Ʒ����

	short system_buy_minute_num;		//!< ϵͳ�Թ�ʱ�������ӣ�
	short system_offshelve_minute_num;//!< ϵͳ�¼�ʱ�������ӣ�
	unsigned int system_buy_timestamp; //!< ϵͳ�Թ�ʱ���������ʱ����ã�
	unsigned int system_offshelve_timestamp; //!< ϵͳ���¼�ʱ���������ʱ����ã�
};

class TradeMarketData
{
public:
	bool Serialize(TLVSerializer& outsteam) const;
	bool UnSerialize(TLVUnserializer& instream);

	struct SaleItem
	{
		long long id_saleitem;		// ������ʼ����ʱ������
		char change_state;

		char is_focus;				// �Ƿ�Ϊ��ע�б������
		short index;				// ���б��е���� (�����������б���ע�б�
		int owner_uid;				// ���������ĸ����

		long long unique_key;		// Ψһkey
		
		// ��������ϸ����
		int seller_uid;				//!< ������uid
		GameName seller_name;		//!< ����������
		SaleItemParam salitem_param;//!< ��Ʒ����
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
		long long id; // ֻ�ڶ�����ʱ����
		int role_id;
		bool old_state; // ΪfalseʱInsert, ����Update
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

	ItemID item_id;			//!< ��ƷID
	short item_num;			//!< ��������
	short price_type;		//!< �۸����� 
	short data_len;			//!< ��ֵ���ݳ���
	int price;				//!< ����
	NetValueItemParamData data;	//!< ��ֵ����
};
class SystemTradeRecord
{
public:
	bool Serialize(TLVSerializer& outstream) const;
	bool UnSerialize(TLVUnserializer& instream);

	struct SystemRecord
	{
		long long id;						// ֻ�ڶ�����ʱ����
		bool old_state;						// ΪfalseʱInsert, ����Update
		short index;						// ���б��е����
		unsigned int transaction_time;		// ����ʱ��
		SystemTransactionInfo ti;			// ��ϸ������Ϣ
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
		long long id;						// ֻ�ڶ�����ʱ����
		char change_state;

		long long rule_id;					// Ψһ��ʶ
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
		long long id;						// ֻ�ڶ�����ʱ����
		char change_state;

		long long unique_id;
		AutoUpshelveParam param;
	};

	static const int MAX_AUTO_UPSHELVE_ITEM_NUM = 256;

	int count;
	AutoUpshelveData data_list[MAX_AUTO_UPSHELVE_ITEM_NUM];
};

#endif