
#ifndef MSGEQUIPMENT_H
#define MSGEQUIPMENT_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSEquipmentOperaReq					//!< 1650 装备操作请求
	{
	public:
		CSEquipmentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 请求信息
			OPERA_TYPE_PUTON = 1,				//!< 1 穿戴装备 \param param1 背包栏id  \param param2 背包格子序号  \param param3 穿戴的部位 \see EQUIPMENT_INDEX
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 脱下装备 \param param1 要脱下的部位 \see EQUIPMENT_INDEX
			OPERA_TYPE_TAKEOFF_CRYSTAL = 3,		//!< 3 脱下元素水晶
			OPERA_TYPE_VOC_LOCK = 4,			//!< 4 词条锁 param1 装备类型（1，装备/2灵饰） param2 下标（装备[0-4]/灵饰[0-2]） param3 词条下标（[0-5]） param4 是否锁定 0/1
			OPERA_TYPE_UPGREADE = 5,			//!< 5 装备系统强化 param1 请求类型[0,下发，1强化]		param2 装备下标
			OPERA_TYPE_RESONACE = 6,			//!< 6 神装共鸣激活
		};

		int msg_type;							//!< 消息类型  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCEquipmentInfoAck					//!< 1600 装备信息
	{
	public:
		SCEquipmentInfoAck();
		MessageHeader header;

		ItemID element_crystal_id;				//!< 元素水晶的物品ID
		short used_times;						//!< 元素水晶已使用次数

		short reserve_sh;
		short count;
		EquipmentPro equipment_list[ItemNamespace::MAX_EQUIPMENT_GRID_NUM];
	};

	class SCEquipmentGridChange					//!< 1601 装备改变通知
	{
	public:
		SCEquipmentGridChange();
		MessageHeader header;

		EquipmentPro equip_info;
	};

	class SCElementCrystalChangeNotice			//!< 1602 元素水晶变更通知
	{
	public:
		SCElementCrystalChangeNotice();
		MessageHeader header;

		ItemID element_crystal_id;				//!< 元素水晶的物品ID
		short used_times;						//!< 元素水晶已使用次数
	};

	class SCEquipmentLockInfo					//!< 1603 词条锁信息
	{
	public:
		SCEquipmentLockInfo();
		MessageHeader header;

		EquipVocLock m_equipment_list[EQUIPMENT_MAX_INDEX];
		EquipVocLock m_jewelry_list[JEWELRY_MAX_INDEX];
	};

	class SCEquipmentJewerlyUpGreadeInfo					//!< 1604 装备强化系统
	{
	public:
		SCEquipmentJewerlyUpGreadeInfo();
		MessageHeader header;

		int equipment_jewelry_upgreade[EQUIP_JEWELRY_TYPE_MAX];//强化等级
		int equipment_jewelry_strengthen[EQUIP_JEWELRY_TYPE_MAX];//突破等级
	};

	class SCEquipmentJewerlyResonaceInfo					//!< 1605 神装共鸣
	{
	public:
		SCEquipmentJewerlyResonaceInfo();
		MessageHeader header;

		int resonace_flag;
	};

	class CSZaoHuaEquipmentOperaReq					//!< 1640 造化装备操作请求
	{
	public:
		CSZaoHuaEquipmentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 请求信息
			OPERA_TYPE_PUTON = 1,				//!< 1 穿戴装备 param1 背包格子序号
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 脱下装备 param1 要脱下的部位
			OPERA_TYPE_ORDER = 3,				//!< 3 整理背包
			OPERA_TYPE_COMPOSE = 4,				//!< 4 合成		param1 合成seq	param2 是否穿戴（0/1） param3 index
			OPERA_TYPE_DISCOMPOSE = 5,			//!< 5 分解		param1 背包idx
			OPERA_TYPE_ACTIVE_APP = 6,			//!< 6 激活外观 param1 外观id
			OPERA_TYPE_APP_WEAR = 7,			//!< 7 穿戴外观 param1 外观id
			OPERA_TYPE_APP_WEAROFF = 8			//!< 8 脱下外观
		};

		int msg_type;							//!< 消息类型  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
	};

	class SCZaoHuaBagListInfo : public IMessageSerializer			//!< 1641 造化装备背包下发
	{
	public:
		SCZaoHuaBagListInfo();
		virtual ~SCZaoHuaBagListInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].reserve_ch, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].reserve_sh, buffer, buffer_size, out_length);
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
		short				reason;														//!< 下发理由
		short				item_count;													//!< 物品数量

		InfoType2			item_list[ItemNamespace::MAX_ZAO_HUA_BAG_NUM];				//!< 物品列表
	};

	class SCZaoHuaBagItemChange : public IMessageSerializer //!< 1642 造化背包单个修改下发
	{
	public:
		SCZaoHuaBagItemChange();
		virtual ~SCZaoHuaBagItemChange() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(index, buffer, buffer_size, out_length);
			result = result && WriteChar(is_market_buy_flag, buffer, buffer_size, out_length);
			result = result && WriteChar(reserve_ch, buffer, buffer_size, out_length);
			result = result && WriteShort(reserve_sh, buffer, buffer_size, out_length);
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
		short				index;					//!< 物品在背包中的序号 
		char				is_market_buy_flag;		//!< 是否从市场购买标识 0:否 1:从市场购买
		char				reserve_ch;
		short				reserve_sh;
		int					num;					//!< 数量
		char				is_bind;				//!< 是否绑定
		char				has_param;				//!< 是否有净值参数
		short				param_length;			//!< 净值参数的数据长度 （当has_param为1时可读）
		UInt32				invalid_time;			//!< 过期时间
		long long           item_unique_id;			//!< 道具唯一ID
		NetValueItemParamData param_data;			//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
	};

	class SCZaoHuaEquipmentInfoAck					//!< 1643 造化装备信息
	{
	public:
		SCZaoHuaEquipmentInfoAck();
		MessageHeader header;

		int count;
		ZaoHuaPro equipment_list[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];
	};

	class SCZaoHuaEquipmentGridChange					//!< 1644 造化装备改变通知
	{
	public:
		SCZaoHuaEquipmentGridChange();
		MessageHeader header;

		ZaoHuaPro equip_info;
	};

	class SCZaoHuaEquipmentComposeResults : public IMessageSerializer
	{
	public:
		SCZaoHuaEquipmentComposeResults();		//!< 1645 - 造化装备打造结果回复
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteChar(com_result, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteUShort(number, buffer, buffer_size, out_length);

			if (0 != has_param)
			{
				result = result && WriteUShort(param_len, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_len, buffer, buffer_size, out_length);
			}

			return result;
		}

		char com_result;		//!< 合成结果: 0 - 失败；1 - 成功；
		char has_param;			//!< 是否带有净值
		ItemID item_id;			//!< 道具ID
		UInt16 number;			//!< 数量
		UInt16 param_len;		//!< 数据长度
		NetValueItemParamData param_data;	//!< 数据信息
	};

	class SCZaoHuaAppearance					//!< 1646 造化装备外观
	{
	public:
		SCZaoHuaAppearance();
		MessageHeader header;

		int wear_app_id;
		BitMap<ZAO_HUA_SUIT_APPEARANCE_MAX> flag;
	};

	class CSZaoHuaEquipmentDiscomposeReq					//!< 1647 造化装备分解列表请求
	{
	public:
		CSZaoHuaEquipmentDiscomposeReq();
		MessageHeader header;

		BitMap<512> discompose_list;
	};
	UNSTD_STATIC_CHECK(512 >= ItemNamespace::MAX_ZAO_HUA_BAG_NUM);

	class SCZaoHuaEquipmentCommonDataInfo					//!< 1648 造化装备通用信息下发
	{
	public:
		SCZaoHuaEquipmentCommonDataInfo();
		MessageHeader header;

		int show_mo_zhuang;
	};
}

#pragma pack(pop)

#endif


