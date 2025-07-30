#ifndef __MSG_TRADE_MARKET_H__
#define __MSG_TRADE_MARKET_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/trademarketparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct TradeItemDetail
	{
		ItemID item_id;				//!< 物品ID 
		short item_num;				//!< 物品数量
		unsigned int invalid_time;	//!< 物品到期时间戳
		short param_length;			//!< param长度
		NetValueItemParamData data;	//!< param数据
	};

	class CSAddItemOnShelve			//!< 2301 上架货物
	{
	public:
		CSAddItemOnShelve();
		MessageHeader header;

		char is_from_pet;			//!< 是否宠物 [0:否  1:是]
		char reserve_ch;			//!< 保留字
		short index;				//!< 序号 （当为宠物时，是货物在宠物背包的序号，否则是货物在背包的序号）
		short column;				//!< 物品栏号 （仅当货物是道具时有效，表示货物在背包中的哪个物品栏）
		short item_num;				//!< 上架货物的数量
		int price;					//!< 单价
	};

	class CSRemoveItemOnShelve		//!< 2302 下架货物
	{
	public:
		CSRemoveItemOnShelve();
		MessageHeader header;

		int sale_index;				//!< 货物在货架上的序号 [0,8)
	};

	class CSTradeMarketCashOut		//!< 2303 提现
	{
	public:
		CSTradeMarketCashOut();
		MessageHeader header;

		int sale_index;				//!< 货物在货架上的序号 [0,8)
	};

	class SCTradeMarketUserShelveData : public IMessageSerializer	//!< 2351 玩家交易所个人数据
	{
	public:
		SCTradeMarketUserShelveData();
		MessageHeader header;

		enum REASON
		{
			REASON_NORMAL = 0,
			REASON_NOTIFY_CHANGE = 1,
			REASON_NOTIFY_ADD = 2,
			REASON_NOTIFY_REMOVE = 3,
		};

		struct SaleItemInfo
		{
			short sale_index;				//!< 格子序号 [0,8)
			short cur_status;				//!< 当前状态 \see SALE_ITEM_STATUS
			unsigned int next_status_time;	//!< 切换下个状态的时间戳

			short sold_num;					//!< 已卖出数 
			short price_type;				//!< 以何种货币交易 \see ITEM_TRADE_MONEY_TYPE
			int price;						//!< 单价

			TradeItemDetail item_detail;	//!< 物品详细数据
		};

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);
			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteShort(item_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].cur_status, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].next_status_time, buffer, buffer_size, out_length);

				result = result && WriteShort(item_list[i].sold_num, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(item_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].item_detail.data, item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
			}

			return result;
		}

		short reason;									//!< 原因,  0:整个货架信息下发  1:改变通知  2:新增  3：删除
		short item_count;								//!< 货架上货物的数量
		SaleItemInfo item_list[USER_SALE_ITEM_MAX_NUM];	//!< 货架上货物的信息，数组长度以item_count为准  \see SaleItemInfo
	};

	class CSTradeMarketUserShelveData		//!< 2304 玩家个人交易所数据请求
	{
	public:
		CSTradeMarketUserShelveData();
		MessageHeader header;
	};

	class CSTradeMarketSearchByItemID   //!< 2305 按物品ID搜索
	{
	public:
		CSTradeMarketSearchByItemID();
		MessageHeader header;

		ItemID item_id;					//!< 物品ID
		short req_page;					//!< 请求的页数

		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低
	};

	class CSTradeMarketSearchByItemSearchType	//!< 2306 按物品搜索类型检索
	{
	public:
		CSTradeMarketSearchByItemSearchType();
		MessageHeader header;

		short item_search_type;			//!< 物品搜索类型 \note 见 J-交易行.xls -> 筛选sheet
		short req_page;					//!< 请求的页数

		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低

		short item_big_search_type;		//!< 值为-1时，则按小类(item_search_type)搜索，否则则按大类搜索
		short param;					//!< 特殊参数（当搜索装备时param代表等级，当搜索令牌时param代表特效职业）(为-1时则按"全部"搜索)
	};

	class SCTradeMarketSearchResult : public IMessageSerializer	//!< 2352 搜索结果返回
	{
	public:
		SCTradeMarketSearchResult();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		struct SaleItemInfo
		{
			unsigned int next_status_time;	//!< 下个状态时间
			short cur_item_status;			//!< 当前商品状态	\see SALE_ITEM_STATUS
			short price_type;				//!< 以何种货币交易 \see ITEM_TRADE_MONEY_TYPE
			int price;						//!< 单价
			TradeItemDetail item_detail;	//!< 物品详细数据
			int seller_uid;					//!< 卖家uid					\note 用于购买请求
			short sale_index;				//!< 商品在卖家货架上的序号		\note 用于购买请求
			unsigned int upshelve_time;		//!< 物品上架时间				\note 用于购买请求
			UniqueKeyStr unique_key_str;	//!< 唯一标识字符串				\note 用于购买系统货架的物品
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(cursor_index, buffer, buffer_size, out_length);
			result = result && WriteShort(is_announce, buffer, buffer_size, out_length);
			result = result && WriteShort(cur_page, buffer, buffer_size, out_length);
			result = result && WriteShort(total_page, buffer, buffer_size, out_length);
			result = result && WriteShort(order_type, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUInt(item_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(item_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].item_detail.data, item_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(item_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].unique_key_str, sizeof(item_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			return result;
		}

		short cursor_index;					//!< 光标对准的货品序号
		short is_announce;					//!< 是否查找正在公示期的 0：否   1：是
		short cur_page;						//!< 当前页
		short total_page;					//!< 总页数
		short order_type;					//!< 排序类型  0:按价格低到高   1:按价格高到低
		short item_count;					//!< 该页有多少货品
		SaleItemInfo item_list[SEARCH_SALE_ITEM_PER_PAGE];	//!< 数组长度以item_count为准  \see SaleItemInfo

	};

	class CSTradeMarketBuyItem				//!< 2307 购买商品
	{
	public:
		CSTradeMarketBuyItem();
		MessageHeader header;

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short buy_num;						//!< 购买数量
		unsigned int upshelve_time;			//!< 商品上架时间 （用于核对物品）
		ItemID buy_item_id;					//!< 购买的物品ID （用于核对物品以及自动购买需求）
		short reserve_sh;					//!< 保留字
		int expect_price;					//!< 预期的价格（用于自动购买时，核对是否超过预期价格）
		UniqueKeyStr unique_key_str;		//!< 唯一标识字符串
	};

	class CSRecentTransactionRecords		//!< 2308 查询最近交易记录
	{
	public:
		CSRecentTransactionRecords();
		MessageHeader header;

		ItemID item_id;						//!< 物品ID
		short reserve_sh;					//!< 保留字
	};

	struct ProtocolTransactionItemInfo
	{
		ProtocolTransactionItemInfo() : item_id(0), item_num(0), price_type(0), data_len(0), price(0) {}

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

	class SCRecentTransactionRecords : public IMessageSerializer	//!< 2353 返回最近交易记录
	{
	public:
		SCRecentTransactionRecords();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = WRITE_BEGIN(header.msg_type);

			result = result && WRITE_INT(count);
			for (int i = 0; i < count; ++i)
			{
				result = result && WRITE_USHORT(record_list[i].item_id);
				result = result && WRITE_SHORT(record_list[i].item_num);
				result = result && WRITE_SHORT(record_list[i].price_type);
				result = result && WRITE_SHORT(record_list[i].data_len);
				result = result && WRITE_INT(record_list[i].price);
				result = result && WRITE_STRN(record_list[i].data, record_list[i].data_len);
			}

			return result;
		}

		int count;							//!< 记录数量
		ProtocolTransactionItemInfo record_list[TRANSACTION_RECORD_MAX_NUM];	//!< 记录列表
	};

	class CSTradeMarketGetFocusList			//!< 2309 查询关注列表
	{
	public:
		CSTradeMarketGetFocusList();
		MessageHeader header;
	};

	struct FocusItemInfo
	{
		unsigned int next_status_time;	//!< 下个状态时间
		short cur_item_status;			//!< 当前商品状态	\see SALE_ITEM_STATUS
		short price_type;				//!< 以何种货币交易 \see ITEM_TRADE_MONEY_TYPE
		int price;						//!< 单价
		TradeItemDetail item_detail;	//!< 物品详细数据
		int seller_uid;					//!< 卖家uid					\note 用于购买请求
		short sale_index;				//!< 商品在卖家货架上的序号		\note 用于购买请求
		short focus_index;				//!< 商品在我的关注列表中的序号 \note 用于取消关注时发的序号
		unsigned int upshelve_time;		//!< 物品上架时间				\note 用于购买请求
		UniqueKeyStr unique_key_str;	//!< 唯一标识key
	};

	class SCTradeMarketGetFocusList : public IMessageSerializer	//!< 2354 获取关注列表
	{
	public:
		SCTradeMarketGetFocusList();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(announce_count, buffer, buffer_size, out_length);
			result = result && WriteShort(sale_count, buffer, buffer_size, out_length);

			for (int i = 0; i < announce_count && i < SEARCH_SALE_ITEM_PER_PAGE; ++i)
			{
				result = result && WriteUInt(announce_focus_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(announce_focus_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(announce_focus_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(announce_focus_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(announce_focus_list[i].item_detail.data, announce_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(announce_focus_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].focus_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(announce_focus_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(announce_focus_list[i].unique_key_str, sizeof(announce_focus_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < sale_count && i < SEARCH_SALE_ITEM_PER_PAGE; ++i)
			{
				result = result && WriteUInt(sale_focus_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(sale_focus_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(sale_focus_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(sale_focus_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(sale_focus_list[i].item_detail.data, sale_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(sale_focus_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].focus_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(sale_focus_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(sale_focus_list[i].unique_key_str, sizeof(sale_focus_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			return true;
		}

		short announce_count;											//!< 公示期数量
		short sale_count;												//!< 发售期数量
		FocusItemInfo announce_focus_list[SEARCH_SALE_ITEM_PER_PAGE];	//!< 公示期关注列表
		FocusItemInfo sale_focus_list[SEARCH_SALE_ITEM_PER_PAGE];		//!< 关注列表
	};

	class CSTradeMarketAddFocus				//!< 2310 添加关注
	{
	public:
		CSTradeMarketAddFocus();
		MessageHeader header;

		int seller_uid;						//!< 卖家uid
		short sale_index;					//!< 在卖家货架上的序号
		short reserve_sh;					//!< 保留字
		unsigned int upshelve_time;			//!< 商品上架时间 （用于核对物品）
	};

	class CSTradeMarketRemoveFocus			//!< 2311 取消关注
	{
	public:
		CSTradeMarketRemoveFocus();
		MessageHeader header;

		int focus_index;					//!< 商品在我的关注列表中的序号  \note 详见SCTradeMarketGetFocusList中的focus_index
		short cur_page;						//!< 当前正在浏览哪一页
		short is_announce;					//!< 当前正在浏览的是公示期还是发售期的关注列表
	};

	class CSTradeMarketGetFocusItemInfo		//!< 2312 获取关注物品的最新数据
	{
	public:
		CSTradeMarketGetFocusItemInfo();
		MessageHeader header;

		int focus_index;					//!< 商品在我的关注列表中的序号  \note 详见SCTradeMarketGetFocusList中的focus_index
		short cur_page;						//!< 当前正在浏览哪一页
		short is_announce;					//!< 当前正在浏览的是公示期还是发售期的关注列表
	};

	class SCTradeMarketGetFocusItemInfo : public IMessageSerializer	//!< 2355 返回关注物品的最新数据
	{
	public:
		SCTradeMarketGetFocusItemInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteUInt(info.next_status_time, buffer, buffer_size, out_length);
			result = result && WriteShort(info.cur_item_status, buffer, buffer_size, out_length);
			result = result && WriteShort(info.price_type, buffer, buffer_size, out_length);
			result = result && WriteInt(info.price, buffer, buffer_size, out_length);

			result = result && WriteUShort(info.item_detail.item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(info.item_detail.item_num, buffer, buffer_size, out_length);
			result = result && WriteUInt(info.item_detail.invalid_time, buffer, buffer_size, out_length);
			result = result && WriteShort(info.item_detail.param_length, buffer, buffer_size, out_length);
			result = result && WriteStrN(info.item_detail.data, info.item_detail.param_length, buffer, buffer_size, out_length);

			result = result && WriteInt(info.seller_uid, buffer, buffer_size, out_length);
			result = result && WriteShort(info.sale_index, buffer, buffer_size, out_length);
			result = result && WriteShort(info.focus_index, buffer, buffer_size, out_length);
			result = result && WriteUInt(info.upshelve_time, buffer, buffer_size, out_length);
			result = result && WriteStrN(info.unique_key_str, sizeof(info.unique_key_str), buffer, buffer_size, out_length);

			return result;
		}

		FocusItemInfo info;					//!< 关注商品的最新数据
	};

	class CSTradeMarketAdvertise	//!< 2313 打广告
	{
	public:
		CSTradeMarketAdvertise();
		MessageHeader header;

		short sale_index;			//!< 需要打广告的货品的序号
		short reserve_sh;			//!< 保留字
	};

	class CSTradeMarketSearchAdvertise		//!< 2314 查找广告上的商品 \note 所需参数均在广告中有下发
	{
	public:
		CSTradeMarketSearchAdvertise();
		MessageHeader header;

		int seller_uid;						//!< 卖家uid
		unsigned int put_on_shelve_time;	//!< 上架时间
		int price;							//!< 价格
		short sale_index;					//!< 在货架上的序号
		ItemID item_id;						//!< 物品ID
		UniqueKeyStr unique_key_str;		//!< 唯一标识字符串
	};

	class CSTradeMarketGetSellingList		//!< 2315 查询正在售卖的某物品
	{
	public:
		CSTradeMarketGetSellingList();
		MessageHeader header;

		ItemID item_id;						//!< 物品ID
		short special_effect_type;			//!< 特效类型（搜令牌时使用）
	};

	struct MsgTransactionInfo
	{
		MsgTransactionInfo() : item_id(0), item_num(0), price_type(0), data_len(0), price(0)
		{
			memset(data, 0, sizeof(data));
		}

		void Reset()
		{
			item_id = 0;
			item_num = 0;
			price_type = 0;
			data_len = 0;
			price = 0;
		}

		ItemID item_id;			//!< 物品ID
		short item_num;			//!< 交易数量
		short price_type;		//!< 价格类型 
		short data_len;			//!< 净值数据长度
		int price;				//!< 单价
		NetValueItemParamData data;	//!< 净值数据
	};

	class SCTradeMarketGetSellingList : public IMessageSerializer	//!< 2356 返回正在售卖的某物品列表
	{
	public:
		SCTradeMarketGetSellingList();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = WRITE_BEGIN(header.msg_type);

			result = result && WRITE_INT(count);
			for (int i = 0; i < count; ++i)
			{
				result = result && WRITE_USHORT(selling_list[i].item_id);
				result = result && WRITE_SHORT(selling_list[i].item_num);
				result = result && WRITE_SHORT(selling_list[i].price_type);
				result = result && WRITE_SHORT(selling_list[i].data_len);
				result = result && WRITE_INT(selling_list[i].price);
				result = result && WRITE_STRN(selling_list[i].data, selling_list[i].data_len);
			}

			return result;
		}

		int count;							//!< 数量
		ProtocolTransactionItemInfo selling_list[TRANSACTION_RECORD_MAX_NUM];	//!< 列表
	};

	class CSTradeMarketGetRoleTransactionRecord	//!< 2316 请求获取玩家交易记录
	{
	public:
		CSTradeMarketGetRoleTransactionRecord();
		MessageHeader header;
	};

	class SCTradeMarketGetRoleTransactionRecord : public IMessageSerializer //!< 2357 获取玩家交易记录返回  \note decode请参考服务端Serialize函数
	{
	public:
		SCTradeMarketGetRoleTransactionRecord();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(sold_record_num, buffer, buffer_size, out_length);
			result = result && WriteShort(buy_record_num, buffer, buffer_size, out_length);

			for (int i = 0; i < sold_record_num; ++i)
			{
				result = result && WriteUShort(sold_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(sold_list[i].item_num, buffer, buffer_size, out_length);
				result = result && WriteShort(sold_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteShort(sold_list[i].data_len, buffer, buffer_size, out_length);
				result = result && WriteInt(sold_list[i].price, buffer, buffer_size, out_length);
				result = result && WRITE_STRN(sold_list[i].data, sold_list[i].data_len);
			}

			for (int i = 0; i < buy_record_num; ++i)
			{
				result = result && WriteUShort(buy_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(buy_list[i].item_num, buffer, buffer_size, out_length);
				result = result && WriteShort(buy_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteShort(buy_list[i].data_len, buffer, buffer_size, out_length);
				result = result && WriteInt(buy_list[i].price, buffer, buffer_size, out_length);
				result = result && WRITE_STRN(buy_list[i].data, buy_list[i].data_len);
			}

			result = WRITE_LL(total_sold_gold);
			result = WRITE_LL(total_buy_gold);

			return result;
		}

		short sold_record_num;
		short buy_record_num;
		ProtocolTransactionItemInfo sold_list[ROLE_TRANSACTION_RECORD_MAX_NUM];
		ProtocolTransactionItemInfo buy_list[ROLE_TRANSACTION_RECORD_MAX_NUM];

		long long total_sold_gold;
		long long total_buy_gold;
	};

	class CSTradeMarketRedoAddItemOnShelve		//!< 2317 重新上架
	{
	public:
		CSTradeMarketRedoAddItemOnShelve();
		MessageHeader header;

		short sale_index;						//!< 哪件商品需要重新上架 [0,8)	
		short reserve_sh;						//!< 保留字
		int price;								//!< 单价
	};

	class CSTradeMarketSearchPetForQuickBuy		//!< 2318 宠物快捷购买搜索请求
	{
	public:
		CSTradeMarketSearchPetForQuickBuy();
		MessageHeader header;

		short order_type;						//!< 筛选顺序 \see TRADE_PET_INDEX_ORDER
		ItemID item_id;							//!< 要搜的宠物物品ID

		int last_seller_uid;					//!< 上次搜到的最后一件商品的卖家UID （第一次搜索或更换筛选顺序时发0）
		int last_price;							//!< 上次搜到的最后一件商品的价格	 （第一次搜索或更换筛选顺序时发0）
		unsigned int last_put_on_shelve_time;	//!< 上次搜到的最后一件商品的上架时间（第一次搜索或更换筛选顺序时发0）
		short last_str_level;					//!< 上次搜到的最后一件商品的强化等级（第一次搜索或更换筛选顺序时发0）
		short quality;							//!< 宠物ID为0时使用这个参数，按品质查找宠物
	};

	class SCTradeMarketSearchPetForQuickBuy : public IMessageSerializer	//!< 2358 宠物快捷购买搜索返回
	{
	public:
		SCTradeMarketSearchPetForQuickBuy();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		struct SaleItemInfo
		{
			unsigned int next_status_time;	//!< 下个状态时间
			short cur_item_status;			//!< 当前商品状态	\see SALE_ITEM_STATUS
			short price_type;				//!< 以何种货币交易 \see ITEM_TRADE_MONEY_TYPE
			int price;						//!< 单价
			TradeItemDetail item_detail;	//!< 物品详细数据
			int seller_uid;					//!< 卖家uid					\note 用于购买请求
			short sale_index;				//!< 商品在卖家货架上的序号		\note 用于购买请求
			unsigned int upshelve_time;		//!< 物品上架时间				\note 用于购买请求
			UniqueKeyStr unique_key_str;	//!< 唯一标识字符串				\note 用于购买系统货架的物品	
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(order_type, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);
			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUInt(item_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(item_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].item_detail.data, item_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(item_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].unique_key_str, sizeof(item_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			return result;
		}

		short order_type;					//!< 筛选顺序 \see TRADE_PET_INDEX_ORDER
		short item_count;					//!< 有多少条数据
		SaleItemInfo item_list[SEARCH_PET_QUICK_BUY_ITEM_NUM];	//!< 数组长度以item_count为准  \see SaleItemInfo
	};

	class SCTradeMarketRefreshNotice	// 2359 通知客户端更新搜索列表
	{
	public:
		SCTradeMarketRefreshNotice();
		MessageHeader header;

	};

	class CSTradeMarketGetMerchandiseNum	//!< 2319 请求每个大类中所有小类目前有多少商品
	{
	public:
		CSTradeMarketGetMerchandiseNum();
		MessageHeader header;

		short is_announce;		//!< 0:发售期   1:公示期
		short big_search_type;	//!< 大类  1：武器	2：防具  3：宠物  10：灵饰
	};

	class SCTradeMarketGetMerchandiseNum	//!< 2360 返回每个大类中所有小类目前有多少商品
	{
	public:
		SCTradeMarketGetMerchandiseNum();
		MessageHeader header;

		struct MerchandiseInfo
		{
			int small_search_type;	// 小类
			int merchandise_num;	// 商品数量
		};

		short is_announce;			//!< 0:发售期  1:公示期
		short big_search_type;		//!< 大类  原样返回
		int num;					//!< MerchandiseInfo数量
		MerchandiseInfo info_list[TRADE_ITEM_SEARCH_TYPE_MAX];
	};

	class SCTradeMarketCreditInfo	//!< 2361 交易信用信息下发
	{
	public:
		SCTradeMarketCreditInfo();
		MessageHeader header;

		long long total_credit;		//!< 当前主角拥有的信用值
	};

	class CSTradeMarketSearchMedalByEffectTypeAndColor   //!< 2320 按特效类型和颜色搜令牌
	{
	public:
		CSTradeMarketSearchMedalByEffectTypeAndColor();
		MessageHeader header;

		short sp_type;					//!< 特效类型
		short color;					//!< 颜色
		short reserve_sh;				//!< 保留字

		short req_page;					//!< 请求的页数
		short is_announce;				//!< 是否查找正在公示期的 0：否   1：是
		short order_type;				//!< 排序类型  0:按价格低到高   1:按价格高到低
	};

	class CSQueryItemRecommendPrice		//!< 2321 查询物品推荐价格
	{
	public:
		CSQueryItemRecommendPrice();
		MessageHeader header;

		char is_from_pet;			//!< 是否宠物 [0:否  1:是]
		char reserve_ch;			//!< 保留字
		short reserve_sh;			//!< 保留字
		short index;				//!< 序号 （当为宠物时，是货物在宠物背包的序号，否则是货物在背包的序号）
		short column;				//!< 物品栏号 （仅当货物是道具时有效，表示货物在背包中的哪个物品栏）
	};

	class SCQueryItemRecommendPriceResp	//!< 2362 查询物品推荐价格返回
	{
	public:
		SCQueryItemRecommendPriceResp();
		MessageHeader header;

		int price;					//!< 推荐价格
		int min_price;				//!< 最低价格
		int max_price;				//!< 最高价格

		// 以下字段原样返回
		char is_from_pet;			//!< 是否宠物 [0:否  1:是]
		char reserve_ch;			//!< 保留字
		short reserve_sh;			//!< 保留字
		short index;				//!< 序号 （当为宠物时，是货物在宠物背包的序号，否则是货物在背包的序号）
		short column;				//!< 物品栏号 （仅当货物是道具时有效，表示货物在背包中的哪个物品栏）
	};
}

#pragma pack(pop)

#endif