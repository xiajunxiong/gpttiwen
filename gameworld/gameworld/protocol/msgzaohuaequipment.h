
#ifndef MSG_ZAO_HUA_EQUIPMENT_H
#define MSG_ZAO_HUA_EQUIPMENT_H

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/userprotocal/msgheader.h"
#include "item/itembase.h"
#include "servercommon/zaohuaequipdef.hpp"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum ZAO_HUA_EQUIPMENT_REQ_TYPE
	{
		ZAO_HUA_EQUIPMENT_REQ_TYPE_REFINE = 0,			//!< 造化装备请求精炼 p1:1600协议数组index
		ZAO_HUA_EQUIPMENT_REQ_TYPE_HUN_LI_UP_LEVEL = 1,		//!< 魂力升级  p1 item_src,见YUAN_HUN_ZHU_ITEM_SRC  p2 index,  p3 column   
		ZAO_HUA_LONG_ZHU_DRAW = 2,		//!< 珑铸抽奖 p1 equip_index p2 slot_index p3 次数
		ZAO_HUA_LONG_ZHU_PUT_ON = 3,		//!< 珑铸穿上 p1 equip_index p2 slot_index p3 词条index
		ZAO_HUA_LONG_ZHU_CLEAR_1 = 4,		//!< 珑铸清除1个 p1 equip_index p2 slot_index p3 词条index
		ZAO_HUA_LONG_ZHU_CLEAR_ALL = 5,	//!< 珑铸清除全部 p1 equip_index p2 slot_index
		ZAO_HUA_LONG_ZHU_UNLOCK = 6,	//!< 珑铸解锁 p1 equip_index p2 slot_index
		ZAO_HUA_RU_MO = 7,	//!< 入魔  p1 item_src,见YUAN_HUN_ZHU_ITEM_SRC  p2 index,
		ZAO_HUA_DECOMPOSE = 8,	//!< 分解元魂珠 p1 index,  p2 column   
		ZAO_HUA_COMPOSE = 9,	//!< 合成元魂珠 p1 item_id
	};

	class CSZaoHuaEquipmentReq					//!< 4800 装备操作请求
	{
	public:
		CSZaoHuaEquipmentReq();
		MessageHeader header;

		int req_type;							
		int param1;
		int param2;
		int param3;
	};

	class SCZaoHuaReFineInfo					//!< 4801 装备信息
	{
	public:
		SCZaoHuaReFineInfo();
		MessageHeader header;

		int refine_info[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];		//8 精炼等级
	};

	enum ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE
	{
		ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE_PUT_ON,		//穿上 p1 item_src p2 index,  p3 column,  p4 slot_index  |  消耗 p5  item_src  p6 index, p7  column   p8 slot_index
		ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE_TAKE_OFF,	//脱下 p1 item_src p2 index,  p3 column,  p4 slot_index  |  消耗 p5  item_src  p6 index, p7  column   p8 slot_index
	};

	class CSZaoHuaYuanHunZhuReq					//!< 4802
	{
	public:
		CSZaoHuaYuanHunZhuReq();
		MessageHeader		header;

		int req_type;
		short param1;
		short param2;
		short param3;
		short param4;
		short param5;
		short param6;
		short param7;
		short param8;
	};

	class CSYuanHunZhuUpLevelReq				//!< 4803			元魂珠升级请求
	{
	public:
		CSYuanHunZhuUpLevelReq();
		MessageHeader		header;

		short item_src;
		short index;
		short  column;
		short slot_index;
	};

	class SCZaoHuaLongZhuInfo					//!< 4804 珑铸信息
	{
	public:
		SCZaoHuaLongZhuInfo();
		MessageHeader header;

		ZaoHuaLongZhuPartParam long_zhu_info[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];  //8个部位	
	};

	class SCZaoHuaLongZhuSlotInfo //!< 4805 单个珑铸孔位信息
	{
	public:
		SCZaoHuaLongZhuSlotInfo();
		MessageHeader header;

		int equip_index;
		int slot_index;
		ZaoHuaLongZhuSlotParam long_zhu_slot_info;  
	};
}

#pragma pack(pop)

#endif


