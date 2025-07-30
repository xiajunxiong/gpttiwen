#ifndef __MSG_SMART_MOUNTS_SYSTEM_HPP__
#define __MSG_SMART_MOUNTS_SYSTEM_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/smartmountssystemdef.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSSmartMountsSystemReq			//!< 2100 灵骑系统请求
	{
	public:
		CSSmartMountsSystemReq();
		MessageHeader header;

		enum SMART_MOUNTS_SYSTEM_OP_TYPE
		{
			SMART_MOUNTS_SYSTEM_OP_TYPE_ALL_INFO = 0,				//!< 请求灵骑信息
			SMART_MOUNTS_SYSTEM_OP_TYPE_SINGLE_INFO = 1,			//!< 请求灵骑单个信息 param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE = 2,					//!< 请求激活灵骑		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_LEVEL_UP = 3,				//!< 请求升级灵骑		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_BREAK_LEVEL = 4,			//!< 请求突破等级		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_REFINE_NATURE = 5,			//!< 请求洗练灵性		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_SKILL_UP = 6,				//!< 请求升级技能		param1 : index param2 : base_or_exclusive[0, 1] param3 : skill_index(技能格子) (屏蔽)
			SMART_MOUNTS_SYSTEM_OP_TYPE_REFINE_SKILL = 7,			//!< 请求洗练技能		param1 : index param2 : 上锁 [0-2bit]二进制 置0:没上锁, 置1:上锁 param3 : 0.不自动购买 (屏蔽)
			SMART_MOUNTS_SYSTEM_OP_TYPE_SAVE_REFINE_SKILL = 8,		//!< 请求保存洗练		param1 : index (屏蔽)
			SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_REFINE_SKILL = 9,	//!< 请求取消洗练		param1 : index (屏蔽)
			SMART_MOUNTS_SYSTEM_OP_TYPE_WEAR_ORNAMENTS = 10,		//!< 请求穿戴饰品		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_TAKE_OFF = 11,				//!< 请求脱下饰品		param1 : index 
			SMART_MOUNTS_SYSTEM_OP_TYPE_ADD_ENERGY = 12,			//!< 请求补充精力		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_RESET_SKILL = 13,			//!< 请求技能重置		param1 : index param2 : base_or_exclusive[0, 1] param3 : skill_index(技能格子) (屏蔽)
			SMART_MOUNTS_SYSTEM_OP_TYPE_DOMINANCE_PET = 14,			//!< 请求统御宠物		param1 : index param2 : pet_index param3 : dominance_index[0, 1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_DOMINANCE = 15,		//!< 请求取消统御		param1 :　index param2 : dominance_index[0, 1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_ATTR_REQ = 16,				//!< 请求单个灵骑属性 param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_FINISH_FUN_GUIDE = 17,		//!< 请求完成指引	
			SMART_MOUNTS_SYSTEM_OP_TYPE_RIDE = 18,					//!< 请求骑乘		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_RIDE = 19,			//!< 请求取消骑乘		
			SMART_MOUNTS_SYSTEM_OP_TYPE_SET_IMAGE = 20,				//!< 请求设置外观		param1 : index param2 : 0:普通形象 1:进阶形象
			SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE_EQUIP = 21,			//!< 请求激活装备    param1 : index param2 : equip_index[0,1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_EQUIP_LEVEL_UP = 22,		//!< 请求装备升级		param1 : index param2 : equip_index[0,1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_JEWELRY_STRENGTH = 23,		//!< 请求灵饰强化		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE_HOLY = 24,			//!< 请求圣物激活		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_HOLY_LEVEL_UP = 25,			//!< 请求圣物强化		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_REVEAL = 26,				//!< 请求灵骑点化		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_CONVERT_ITEM = 27,			//!< 请求转换道具		param1 : need_convert_item param2 : target_item_id
			SMART_MOUNTS_SYSTEM_OP_TYPE_USE_ACTIVE_CONVERT_ITEM = 28,//!< 请求使用激活灵骑道具转换 param1:bag_column param2:bag_index
			SMART_MOUNTS_SYSTEM_OP_TYPE_SKILL_ITEM_COMPOSE = 29,	//!< 请求灵技合成		param1 : main_item_id param2:stuff_item_id param3:target_item_id
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};

	struct SmartMountsSystemSinglePro
	{
		SmartMountsSystemSinglePro() { this->Reset(); }
		SmartMountsSystemSinglePro(const SmartMountsSystemSingleParam & info)
		{
			bicycle_id = info.bicycle_id;
			level = info.level;
			//remain_skill_point = info.remain_skill_point;
			energy_value = info.energy_value;
			reveal_level = info.reveal_level;
			is_breach_flag = info.is_breach_flag;
			//is_refine_flag = info.is_refine_flag;
			is_use_advanced_flag = info.is_use_advanced_flag;
			holy_strength_level = info.holy_strength_level;
			capability = info.capability;
			jewelry_data = info.jewelry_data;
			for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM; i++)
			{
				equip_list[i] = info.equip_list[i];
			}
			/*for(int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM; i++)
			{
				exclusive_level_list[i] = info.exclusive_level_list[i];
			}*/
			for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; i++)
			{
				base_skill_list[i] = info.base_skill_list[i];
				//new_base_skill_list[i] = info.new_base_skill_list[i];
			}
			for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
			{
				dominance_pet_index_list[i] = info.dominance_pet_index_list[i];
			}
		}
		void Reset()
		{
			bicycle_id = 0;
			level = 0;
			//remain_skill_point = 0;
			energy_value = 0;
			reveal_level = 0;
			is_breach_flag = 0;
			//is_refine_flag = 0;
			is_use_advanced_flag = 0;
			holy_strength_level = 0;
			reserve_ch = 0;
			capability = 0;
			jewelry_data.Reset();
			for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM; i++)
			{
				equip_list[i].Reset();
			}
			//memset(exclusive_level_list, 0, sizeof(exclusive_level_list));
			for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; i++)
			{
				base_skill_list[i].Reset();
			}
			//memset(new_base_skill_list, 0, sizeof(new_base_skill_list));
			for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
			{
				dominance_pet_index_list[i] = -1;
			}
		}
		
		short bicycle_id;
		UInt16 level;																					// 灵骑等级
		//short remain_skill_point;																		// 剩余技能点
		short energy_value;																				// 精力值
		UInt16 reveal_level;																			// 点化等级
		char is_breach_flag;																			// 是否经过突破 1:完成突破			
		//char is_refine_flag;																			// 是否存在洗练 1:有洗练内容
		char is_use_advanced_flag;																		// 形象使用标识 0:未使用进阶形象 1:使用进阶形象
		char holy_strength_level;																		// 圣物强化等级
		char reserve_ch;
		int capability;																					// 评分
		//short exclusive_level_list[MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM];					// 专属技能等级
		SmartMountsSystemItemData jewelry_data;															// 灵饰数据
		SmartMountsSystemItemData equip_list[MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM];					// 装备数据
		SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];	// 基础技能
		//UInt16 new_base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];						// 洗练基础技能ID
		char dominance_pet_index_list[MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM];					// 统御宠物index
	};


	class SCSmartMountsSystemAllInfo					//!< 2101 灵骑系统信息下发
	{
	public:
		SCSmartMountsSystemAllInfo();
		MessageHeader header;

		int count;
		SmartMountsSystemSinglePro smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];
	};

	class SCSmartMountsSystemSingleInfo					//!< 2102 灵骑系统单个信息下发
	{
	public:
		SCSmartMountsSystemSingleInfo();
		MessageHeader header;

		int index;
		SmartMountsSystemSinglePro smart_mounts;
	};

	class SCSmartMountsSystemSingleAttr					//!< 2103  灵骑基础属性列表下发
	{
	public:
		SCSmartMountsSystemSingleAttr();
		MessageHeader header;

		short index;
		short reserve_sh;
		AttrInfo attr_list[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];
	};

	class SCSmartMountsSystemDominanceInfo				//!< 2104 灵骑统御单个信息下发
	{
	public:
		SCSmartMountsSystemDominanceInfo();
		MessageHeader header;

		short index;
		char dominance_pet_index_list[MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM];
	};

	class SCSmartMountsSystemShowInfo					//!< 2105 灵骑弹窗界面展示信息下发
	{
	public:
		SCSmartMountsSystemShowInfo();
		MessageHeader header;

		enum SHOW_NOTICE_TYPE
		{
			SHOW_NOTICE_TYPE_ACTIVE = 0,						// 激活灵骑		param1:smart_mounts_level 
			SHOW_NOTICE_TYPE_LEVEL_UP = 1,						// 等级提升		param1:smart_mounts_level param2:remain_skill_point 
			SHOW_NOTICE_TYPE_SKILL_UP_BASE = 2,					// 基础技能提升	param1:skill_level param2:skill_index 
			SHOW_NOTICE_TYPE_SKILL_UP_EXCLUSIVE = 3,			// 专属技能提升	param1:skill_level param2:skill_index 
			SHOW_NOTICE_TYPE_BREAK_SUCC = 4,					// 突破成功		
			SHOW_NOTICE_TYPE_ENERGY_VALUE = 5,					// 灵骑精力不足	param1:energy_value
			SHOW_NOTICE_TYPE_ACTIVE_EQUIP = 6,					// 激活装备		param1:equip_index param2:strength_level
			SHOW_NOTICE_TYPE_EQUIP_LEVEL_UP = 7,				// 升级装备		param1:equip_index param2:strength_level
			SHOW_NOTICE_TYPE_ACTIVE_HOLY = 8,					// 激活圣物		param1:strength_level
			SHOW_NOTICE_TYPE_HOLY_LEVEL_UP = 9,					// 圣物升级		param1:strength_level
			SHOW_NOTICE_TYPE_REVEAL_UP = 10,					// 点化升级		param1:reveal_level
			SHOW_NOTICE_TYPE_LEARN_SKILL = 11,					// 学习技能		param1:skill_index param2:skill_id param3:skill_level
			SHOW_NOTICE_TYPE_REPLACE_SKILL = 12,				// 置换技能		param1:skill_index param2:skill_id param3:skill_level
		};

		short noitce_type;
		short index;
		short param1;
		short param2;
		short param3;
		short param4;
	};

	class SCSmartMountsSystemRefineSkillInfo			//!< 2106 灵骑洗练技能结果下发
	{
	public:
		SCSmartMountsSystemRefineSkillInfo();
		MessageHeader header;

		char index;
		char is_refine_flag;							// 是否存在洗练 1:有洗练内容
		UInt16 new_base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];
		SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];		//MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM = 3
	};

	class SCSmartMountsSystemOtherInfo					//!< 2107 灵骑其它信息下发
	{
	public:
		SCSmartMountsSystemOtherInfo();
		MessageHeader header;

		char is_finish_fun_guide_flag;			// 是否完成功能指引 0:未完成 1:已完成
		char ride_index;						// 玩家此时骑乘的灵骑 -1:未骑乘 
		char is_use_advanced_flag;				// 形象使用标识 0:未使用进阶形象 1:使用进阶形象
		char reserve_ch;
	};

	class SCSmartMountsSystemRelatedInfo					//!< 2108 灵骑相关信息下发
	{
	public:
		SCSmartMountsSystemRelatedInfo();
		MessageHeader header;

		enum INFO_TYPE
		{
			INFO_TYPE_ENERGY_CHANGE = 0,			//精力值变化		param1:精力值
			INFO_TYPE_NATURE_CHANGE = 1,			//灵性值变化		param1:灵性值
			INFO_TYPE_WEAR_JEWELRY = 2,				//穿戴饰品		param1:饰品强化等级
			INFO_TYPE_JEWELRY_LEVEL_UP = 3,			//升级饰品		param1:饰品强化等级
			INFO_TYPE_SET_IMAGE = 4,				//外观变化		param1:形象使用标识 0:未使用进阶形象 1:使用进阶形象
		};

		short op_type;	
		short index;
		int param1;
	};

	class CSSmartMountsSystemSkillReq			//!< 2109 灵骑系统技能请求
	{
	public:
		CSSmartMountsSystemSkillReq();
		MessageHeader header;

		enum SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE
		{
			SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_LEARN_SKILL = 0,		//!< 请求学习灵骑技能		param1 : index param2:skill_index param3:column param4:bag_index
			SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_SKILL_UP = 1,			//!< 请求升级灵骑技能		param1 : index param2:skill_index param3:column param4:bag_index
			SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_SKILL_REPLACE = 2,	//!< 请求置换灵骑技能		param1 : index param2:skill_index param3:column param4:bag_index
		};

		int op_type;
		int param1;
		int param2;
		int param3;
		int param4;
	};

}

#pragma pack(pop)

#endif