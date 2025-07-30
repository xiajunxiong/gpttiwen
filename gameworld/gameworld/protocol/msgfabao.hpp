#ifndef __MSG_FABAO_HPP__
#define __MSG_FABAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/fabaodef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCFabaoInfo				//!< 2450 法宝所有信息
	{
	public:
		SCFabaoInfo();
		MessageHeader		header;
		int week_use_coin_times;									//!< 本周金币使用次数
		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];			//!< 法宝装备栏-对应装备的法宝index
		FaBaoEquip m_fabao_list[MAX_FABAO_COUNT];					//!< 法宝信息
	};

	class CSFabaoReq				//!< 2451 请求法宝操作
	{
	public:
		enum FABAO_REQ_TYPE
		{
			FABAO_REQ_TYPE_INFO = 0,			//!< 0 信息
			FABAO_REQ_TYPE_ACTIVE = 1,			//!< 1 激活法宝 p1 fabao_index
			FABAO_REQ_TYPE_UPLEVEL = 2,			//!< 2 升级 p1 fabao_index
			FABAO_REQ_TYPE_ADVANCED = 3,		//!< 3 进阶 p1 fabao_index
			FABAO_REQ_TYPE_GUIDE_MONSTER = 9,		//!< 9 打法宝指引怪
			FABAO_REQ_TYPE_DRAW_MATERIAL = 10,		//!< 10 设置材料 p1 index p2~p6 物品id
			FABAO_REQ_TYPE_DRAW_SPEED_UP = 11,		//!< 11  加速  p1 index  p2 次数
			FABAO_REQ_TYPE_DRAW = 12,				//!< 12 获取抽奖奖励 p1 index
		};

		CSFabaoReq();
		MessageHeader		header;	

		int req_type;
		unsigned short param1;
		unsigned short param2;
		unsigned short param3;
		unsigned short param4;
		unsigned short param5;
		unsigned short param6;
	};

	class SCFabaoSignInfo			//!< 2452 单个法宝信息修改
	{
	public:
		enum SEND_INFO_TYPE								// !< 下发原因
		{
			SEND_INFO_TYPE_ACTIVE = 0,					// !< 法宝激活
			SEND_INFO_TYPE_UPLEVEL = 1,					// !< 法宝升级
			SEND_INFO_TYPE_ADVANCE = 2,					// !< 法宝进阶
			SEND_INFO_TYPE_DECOMPOSE = 3,				// !< 法宝分解
			SEND_INFO_TYPE_RANDATTR = 4,				// !< 法宝洗炼
			SEND_INFO_TYPE_TAKEUP_OR_OFF = 5,			// !< 法宝穿戴
		};
		SCFabaoSignInfo();
		MessageHeader		header;
		int reason;
		int week_use_coin_times;							//!< 本周金币使用次数
		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];	//!< 法宝装备栏-对应装备的法宝index
		FaBaoEquip m_fabao_info;							//!< 法宝信息
	};

	class SCFabaoEquipBarInfo			//!< 2453 法宝装备栏变更
	{
	public:
		SCFabaoEquipBarInfo();
		MessageHeader		header;

		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];	//!< 法宝装备栏-对应装备的法宝index
	};

	class SCFabaoDrawInfo				//!< 2454,		// 法宝抽奖信息
	{
	public:
		SCFabaoDrawInfo();
		MessageHeader		header;

		FaBaoDrawItemInfo item_info[FA_BAO_DRAW_ITEM_MAX_NUM];
	};

	class SCFabaoDrawRewardInfo			//!<  2455,		// 法宝抽奖奖励信息
	{
	public:
		SCFabaoDrawRewardInfo();
		MessageHeader		header;


		short fa_bao_id;
		short fa_bao_sui_pian_index;
		int item_id[FA_BAO_DRAW_MATERIAL_MAX_NUM];
		int item_num[FA_BAO_DRAW_MATERIAL_MAX_NUM];
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

