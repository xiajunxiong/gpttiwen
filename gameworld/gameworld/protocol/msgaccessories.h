#ifndef MSGACCESSORIESMENT_H
#define MSGACCESSORIESMENT_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSAccessoriesMentOperaReq					//!< 4150 配饰操作请求
	{
	public:
		CSAccessoriesMentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 请求信息
			OPERA_TYPE_PUTON = 1,				//!< 1 穿戴装备 \param param1 背包栏id  \param param2 背包格子序号  \param param3 穿戴的部位 \see EQUIPMENT_INDEX
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 脱下装备 \param param1 要脱下的部位 \see EQUIPMENT_INDEX
			OPERA_TYPE_DISCOMPOSE = 3,			//!< 3 分解装备 param1 背包coloumn param2 背包index param3 数量默认 = 1
			OPERA_TYPE_ACCESSORIES = 4,			//!< 4 强化装备 param1 是否穿戴 param2 背包index
			OPERA_TYPE_CHANGE = 5,				//!< 5 置换装备 param1 是否穿戴 param2 背包coloumn param3 背包index(穿戴index) param4 新道具id
			OPERA_TYPE_SPECIAL = 6,				//!< 6 升级仙饰 param1 seq[1-2]
		};

		int msg_type;							//!< 消息类型  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class CSAccessoriesMentVocComposeReq// 4153 配饰词条继承
	{
	public:
		CSAccessoriesMentVocComposeReq();
		struct ComposeVoc
		{
			ComposeVoc() :voc_index(-1), replace_voc_index(-1) {}
			int voc_index;
			int replace_voc_index;
		};
		MessageHeader header;

		int is_wear;
		int index;
		int replace_index;
		ComposeVoc compose_voc_list[MAX_ACCESSORIES_VOC_NUM];
	};

	class SCAccessoriesMentInfo					//!< 4151 配饰信息
	{
	public:
		SCAccessoriesMentInfo();
		MessageHeader header;
		int capability;
		AccessoriesPro equipment_list[ItemNamespace::MAX_ACCESSORIES_INDEX_NUM];
	};

	class SCAccessoriesMentChange					//!< 4152 单个配饰改变通知
	{
	public:
		SCAccessoriesMentChange();
		MessageHeader header;
		int capability;
		AccessoriesPro equip_info;
	};

	class SCAccessoriesSpecialEquip					//!< 4154 仙饰 下发
	{
	public:
		SCAccessoriesSpecialEquip();
		MessageHeader header;
		int accessories_special_equip_list[ACCESSORIES_SPECIAL_EQUIP_LEVEL];
	};
}

#pragma pack(pop)

#endif


