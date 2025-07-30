#ifndef MSGITEM_H
#define MSGITEM_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	/*
	物品相关操作，包括使用物品，装备等
	*/

	class CSKnapsackOperaReq			//!< 1550 背包操作请求
	{
	public:
		enum OPERA_TYPE					//!< 操作请求类型枚举
		{
			OPERA_TYPE_REQUEST_INFO,	//!< 请求背包信息 0  
			OPERA_TYPE_EXTEND,			//!< 扩展背包 1  \param param1 背包栏ID  \param param2 扩展格子数
			OPERA_TYPE_DEPOSIT,			//!< 存入仓库 2  \param param1 背包栏ID  \param param2 物品所在序号 \param param3 仓库页号
			OPERA_TYPE_WITHDRAW,		//!< 取出仓库或临时背包 3  \param param1 背包栏ID(仓库或临时背包) \param param2 所在的序号
			OPERA_TYPE_WITHDRAW_ALL,	//!< 取出所有仓库或临时背包中的物品 4  \param param1 背包栏ID(仓库或临时背包)
			OPERA_TYPE_INORDER,			//!< 整理背包 5  \param param1 背包栏ID
			OPERA_TYPE_DISCARD,			//!< 丢弃(出售)物品 6 \param param1 背包栏ID \param param2 物品所在序号  \param param3 丢弃物品数量
		};

	public:
		CSKnapsackOperaReq();		
		MessageHeader		header;

		int					opera_type;	//!< 操作类型 \see OPERA_TYPE
		int					param1;
		int					param2;
		int					param3;
		int					param4;
	};

	class SCKnapsackInfoAck : public IMessageSerializer  //!< 1500 背包信息返回
	{
	public:
		enum SEND_REASON
		{
			SEND_REASON_NORMAL = 0,				//!< 0 正常下发（上线或客户端请求）			
			SEND_REASON_IN_ORDER = 1,			//!< 1 整理
		};

	public:
		SCKnapsackInfoAck();
		virtual ~SCKnapsackInfoAck() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			for (int i = ItemNamespace::ITEM_COLUMN_TYPE_MIN; i < ItemNamespace::ITEM_COLUMN_TYPE_MAX; ++i)
			{
				result = result && WriteShort(valid_grid_num_list[i], buffer, buffer_size, out_length);
			}

			for (int i = ItemNamespace::ITEM_COLUMN_TYPE_MIN; i < ItemNamespace::ITEM_COLUMN_TYPE_MAX; ++i)
			{
				result = result && WriteShort(extend_times_list[i], buffer, buffer_size, out_length);
			}

			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].column, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].num, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);

				if (1 == item_list[i].has_param)
				{
					result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);
				}
			}

			return result;
		}
		
		short				valid_grid_num_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	//!< 栏位有效格子数列表 \see ITEM_COLUMN_TYPE_MAX
		short				extend_times_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];		//!< 栏位扩展次数列表 \see ITEM_COLUMN_TYPE_MAX
		
		short				reason;														//!< 下发理由
		short				item_count;													//!< 物品数量

		InfoType			item_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];			//!< 物品列表  \see InfoType \see KNAPSACK_MAX_GRID_NUM
	};
	UNSTD_STATIC_CHECK(9 == ItemNamespace::ITEM_COLUMN_TYPE_MAX);

	class SCKnapsackItemChange : public IMessageSerializer //!< 1501 单个物品改变通知
	{
	public:
		SCKnapsackItemChange();	
		virtual ~SCKnapsackItemChange() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(column, buffer, buffer_size, out_length);
			result = result && WriteShort(index, buffer, buffer_size, out_length);
			result = result && WriteChar(is_market_buy_flag, buffer, buffer_size, out_length);
			result = result && WriteInt(num, buffer, buffer_size, out_length);
			result = result && WriteChar(is_bind, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUInt(invalid_time, buffer, buffer_size, out_length);
			result = result && WriteLL(item_unique_id, buffer, buffer_size, out_length);

			if (1 == has_param)
			{
				result = result && WriteShort(param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_length, buffer, buffer_size, out_length);
			}

			return result;
		}

		short				reason;					//!< 改变的原因
		short				put_reason;				//!< 获得物品的来源  \see PUT_REASON_TYPE
		ItemID				item_id;				//!< 物品id
		short				column;					//!< 物品在哪个背包栏中 [0,9)
		short				index;					//!< 物品在背包中的序号 
		char				is_market_buy_flag;		//!< 是否从市场购买标识 0:否 1:从市场购买
		char				reserve_ch;
		short				num;					//!< 数量
		char				is_bind;				//!< 是否绑定
		char				has_param;				//!< 是否有净值参数
		UInt32				invalid_time;			//!< 过期时间
		long long           item_unique_id;			//!< 道具唯一ID
		short				param_length;			//!< 净值参数的数据长度 （当has_param为1时可读）
		NetValueItemParamData param_data;			//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
	};

	class SCUseItemSuc						//!< 1504 通知使用某物品成功
	{
	public:
		SCUseItemSuc();
		MessageHeader		header;

		UInt16				item_id;		//!< 物品ID
		short num;
	};

	class SCKnapsackExtend					//!< 1505 背包扩充返回
	{
	public:
		SCKnapsackExtend();
		MessageHeader		header;

		short				column;			//!< 背包栏 \see ITEM_COLUMN_TYPE_MAX
		short				grid_num;		//!< 新的格子数
		short				extend_times;	//!< 当前的扩充次数
		short				reserve_sh;		//!< 保留字
	};

	class SCLackItem						//!< 1506缺乏物品  \note 消耗物品不足时会下发
	{
	public:
		SCLackItem();
		MessageHeader		header;

		int					item_id;		//!< 物品id
		int					item_count;		//!< 数量
	};

	class CSUseItem							//!< 1551 使用物品
	{
	public:
		CSUseItem();				
		MessageHeader		header;

		short				column;
		short				index;
		short				num;
		short				param1;
		short				param2;
		short				param3;

	};

	class SCItemIntoKnpackSpecialProtocal :public IMessageSerializer2		//！<1507 获得物品特殊通知
	{
	public:
		SCItemIntoKnpackSpecialProtocal();
		MessageHeader		header;

		virtual bool Serialize(TLVSerializer2 * outstream) const;

		static const int MAX_FETCH_ITEM_NUM = 120;

		struct ItemInfo
		{
			void Reset()
			{
				item_id = 0;
				is_bind = 0;
				count = 0;
				has_param = 0;
				param_length = 0;
				memset(param_data, 0, sizeof(param_data));
			}

			ItemID			item_id;
			short			is_bind;
			int				count;
			short			reserve_sh;
			short			has_param;
			unsigned int	param_length;		//!< 净值参数的数据长度 （当has_param不为0时可读）
			NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
		};
		
		short get_reson;
		short count;
		int param1;				//get_reason = 8, param1为道具ID
		int param2;				//get_reason = 8, param2为使用数量
		ItemInfo item_list[MAX_FETCH_ITEM_NUM];
	};

	class CSUseOptionalGift		//1552
	{

	public:
		CSUseOptionalGift();
		MessageHeader		header;

		static const int MAX_SELECT_ITEM = 64;

		struct SelectInfo
		{
			short index;
			short num;
		};

		short				column;
		short				index;
		int					num;
		SelectInfo select_info[MAX_SELECT_ITEM];
	};

	class SCTaskItemCommitNotice	//!< 1554 提交任务物品通知
	{
	public:
		SCTaskItemCommitNotice();
		MessageHeader header;

		struct ItemInfo
		{
			ItemID item_id;
			short num;
		};

		int count;		//!< 物品数量 根据数量来读
		ItemInfo item_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	};

	class SCTaskItemLackForCommitNotice		//!< 1553 缺少用于提交任务物品的通知
	{
	public:
		SCTaskItemLackForCommitNotice();
		MessageHeader header;

		struct ItemInfo
		{
			ItemID item_id;
			short num;
		};

		int count;		//!< 物品数量 根据数量来读
		ItemInfo item_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	};

	class CSUseOptionalGift2		//1555
	{

	public:
		CSUseOptionalGift2();
		MessageHeader		header;

		short				column;
		short				index;
		long long      select_index_flag;
	};

	class CSUseGoldRecovery		//!<1556 使用元宝恢复
	{

	public:
		CSUseGoldRecovery();
		MessageHeader		header;

		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCUseRecoveryNotice		//!<1557 使用恢复通知
	{

	public:
		SCUseRecoveryNotice();
		MessageHeader		header;

		unsigned short item_id;					
		unsigned short use_gold_type;
		int param1;
		int param2;
		int param3;
	};

	class SCKnapsackGetItemNotice		//!< 1558 特殊获得道具飘字通知	
	{
	public:
		SCKnapsackGetItemNotice();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_BATCH_COMPOUND = 0,		//工坊批量打造
			NOTICE_TYPE_LIFE_SKILL = 1,			//生活技能双倍 param1:NEW_LIFE_SKILL_TYPE
		};

		int notice_type;
		ItemID item_id;
		short num;	
		int param1;
	};
		
	class CSUseSpecialLoigcItem			//!<  1559  使用需要特殊逻辑道具
	{
	public:
		CSUseSpecialLoigcItem();
		MessageHeader header;

		enum USE_SPECIAL_LOGIC_ITEM_TYPE
		{
			USE_SPECIAL_LOGIC_ITEM_TYPE_CRYSTAL_FB = 0,				// 使用秘境药剂 
			USE_SPECIAL_LOGIC_ITEM_TYPE_BREAK_ITEM = 1,				// 使用灵力丹
		};

		int notice_type;
		ItemID item_id;
		short num;
	};

	class CSActiveSpecialLogicItem			//!< 1560 激活特殊逻辑道具
	{
	public:
		CSActiveSpecialLogicItem();
		MessageHeader header;
		
		enum ACTIVE_SPECIAL_LOGIC_ITEM_TYPE
		{
			ACTIVE_SPECIAL_LOGIC_ITEM_TYPE_STAR_TREASURE_MAP = 0,		//激活星辰藏宝图	param1:column param2:index
			ACTIVE_SPECIAL_LOGIC_ITEM_RESET_TREASURE_MAP_POS = 1,		//客户端检测藏宝图坐标不对,重新随机坐标  param1:column param2:index
		};

		int notice_type;
		short param1;
		short param2;
	};

	class CSHearsayInfoReq					//!< 1561 请求下发传闻
	{
	public:
		CSHearsayInfoReq();
		MessageHeader header;

		enum HEARSAY_INFO_TYPE
		{
			HEARSAY_INFO_TYPE_TREASURE_MAP = 0,				//宝图传闻 param1:monster_id/reward_id	param2:use_item_id	param3:公告类型(is_notice)
		};

		int notice_type;
		int param1;
		int param2;
		int param3;
	};

	class CSBuyCmdGiftReq				//!< 1562 请求购买直购激活(礼包)
	{
	public:
		CSBuyCmdGiftReq();
		MessageHeader header;

		int param1;			// BUY_GIFT_ITEM_TYPE_MAX
		int param2;			
		int param3;
	};

	class SCHongBaoUseRet 		//！<1563 红包使用奖励返回
	{
	public:
		SCHongBaoUseRet();
		MessageHeader		header;

		struct Item
		{
			int money_type;
			int num;
		};

		int count;
		Item item_list[100];
	};
}

#pragma pack(pop)

#endif





