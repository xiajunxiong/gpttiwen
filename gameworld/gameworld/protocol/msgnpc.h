#ifndef __MSG_NPC_H__
#define __MSG_NPC_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSNPCHandleRequest
	{
	public:
		CSNPCHandleRequest();			//!< 1760: 请求 NPC 操作
		MessageHeader header;

		enum NPC_REQUEST_TYPE
		{
			NPC_REQUEST_STARTS_BATTLE = 0,		//!< 请求战斗
			NPC_REQUEST_PURCHASE_ITEM = 1,		//!< 购买物品（通过 NPC 商店）
			NPC_REQUEST_STARTS_GATHER = 2,		//!< 请求采集
			NPC_REQUEST_END_GATHER = 3,			//!< 完成采集
			NPC_REQUEST_CHOSED = 4,				//!< 对话NPC(只有特殊商人NPC，互动NPC才可以使用)
			NPC_REQUEST_START_BATTLE = 5,		//!< 发起战斗(不做判断)

			NPC_REQUEST_TYPE_MAX
		};

		short op_type;		//!< |	0			|	1		|	2		|	3		|	4		|	5 |
		ObjID npc_objid;	//!< 保留字->NPCobjid(现在只有对话NPC才用到)
		int npc_seq;		//!< |	   NPC 的 SeqID												|

		int param1;			//!< |0怪物(组)		|			|		采集物objid		|请求类型	|  怪物组ID
		int param2;			//!< | 0，挑战任务id|			|		是否自动采集	|请求内容	|
	};

	class SCNPCStartToGather
	{
	public:
		SCNPCStartToGather();			//!< 1761: 采集所需时间
		MessageHeader header;

		unsigned int need_time;			//!< 所需采集时间(秒)
	};

	class SCNPCStopGathering
	{
	public:
		enum CANCEL_REASONS
		{
			CANCEL_REASON_FAIL = 0,		//!< 采集失败
			CANCEL_REASON_SUCC = 1,		//!< 采集成功

			CANCEL_REASON_MAX
		};

		SCNPCStopGathering();		//!< 1762: 结束采集行为
		MessageHeader header;

		int role_obj_id;		//!< 采集玩家objid
		int gather_seq;			//!< 采集物的 SeqID
		int reasons;			//!< 结束理由
	};

	class SCNPCGathersNotify
	{
	public:
		SCNPCGathersNotify();		//!< 1763: 采集广播(通知周围玩家)
		MessageHeader header;

		int role_obj_id;		//!< 角色的 ObjID
		int gather_seq;			//!< 采集物的 SeqID
		int npc_id;				//!< 采集物的 NpcID
	};

	class SCNPCInfoNotify
	{
	public:
		SCNPCInfoNotify();		//!< 1765
		MessageHeader header;

		int obj_id;		//!< ObjID
		int param1;
		int param2;
		int param3;
	};

	class SCNPCCommitBack
	{
	public:
		SCNPCCommitBack();		//!< 1766  NPC交互返回
		MessageHeader header;
		struct option
		{
			option() : option_content(0), cost_coin(0), cost_item(0), cost_num(0)
			{}
			int option_content;	//!<选项内容语言seq
			int cost_coin;		//!<消耗铜币
			int cost_item;		//!<消耗物品id
			int cost_num;		//!<消耗物品数量
		};

		int obj_id;		//!< ObjID
		//主内容语言seq
		int main_content;
		//选项
		option option_list[2];
	};

	class SCNPCShopInfo
	{
	public:
		const static int MAX_SALE_ITEM_COUNT = 20;//最大销售种类
		SCNPCShopInfo();		//!< 1767  NPC商店返回
		MessageHeader header;
		struct NpcShopItem
		{
			struct NpcShopItem_data
			{
				NpcShopItem_data() :item_id(0), num(0), is_bind(0), money_type(0), price(0), discount(0){}
				int item_id;			//!< 物品id
				int num;				//!< 物品数量
				short is_bind;			//!< 是否绑定
				short money_type;		//!< 花费类型 0 元宝 1 仙币 2 铜币
				int price;				//!< 价格
				int discount;			//!< 折扣
			};
			NpcShopItem() :num(0) {}
			NpcShopItem_data item;		//!< 商品信息
			int num;					//!< 可购买数量
			int max_num;				//!< 限购数量
		};
		int obj_id;						//!< ObjID

		int item_count;					//!< 商品数量
		NpcShopItem item_list[MAX_SALE_ITEM_COUNT];
	};

	class CSNPCShopBuyReq
	{
	public:
		CSNPCShopBuyReq();		//!< 1768  向NPC购买请求
		MessageHeader header;
		int obj_id;				//!< ObjID

		int buy_index;			//!< 商品序号1开始 按照1767下发的序号
		int buy_num;			//!< 购买数量
	};
}

#pragma pack(pop)
#endif