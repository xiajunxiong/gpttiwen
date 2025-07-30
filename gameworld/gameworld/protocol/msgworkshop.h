#ifndef __MSG_WORKSHOP_H__
#define __MSG_WORKSHOP_H__

#include "gamedef.h"
#include "servercommon/workshopdef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSWorkshopOperateRequire
	{
	public:
		CSWorkshopOperateRequire();			//!< 1730 - 工坊系统操作请求
		MessageHeader header;

		enum WORLSHOP_OP
		{
			WORKSHOP_OP_COMPOUND_REQ = 0,				//!< 请求合成物品
			WORKSHOP_OP_GEM_MOUNTING = 1,				//!< 装备宝石镶嵌
			WORKSHOP_OP_GEM_DISMOUNT = 2,				//!< 装备宝石拆卸
			WORKSHOP_OP_MAKE_JEWELTY = 3,				//!< 请求打造灵饰
			WORKSHOP_OP_JEWE_REFORGE = 4,				//!< 请求灵饰重锻
			WORKSHOP_OP_GAIN_REFORGE = 5,				//!< 选择重锻结果
			WORKSHOP_OP_GEM_UPGRADES = 6,				//!< 装备宝石升级
			WORKSHOP_OP_GEM_CONF_UGD = 7,				//!< 确认宝石升级
			WORKSHOP_OP_SYNTHESIS = 8,					//!< 物品兑换、合成
			WORKSHOP_OP_DISCOMPOUND_REQ = 9,			//!< 请求兑换物品
			WORKSHOP_OP_BATCH_COMP_REQ = 10,			//!< 请求批量合成装备
			WORKSHOP_OP_AVTIVE_EQUIP = 11,				//!< 请求激活孔位属性
			WORKSHOP_OP_REFRESH_EQUIP = 12,				//!< 请求洗练未锁定孔位属性
			WORKSHOP_OP_SAVE_EQUIP_REFRESH = 13,		//!< 请求保存洗练后的属性
			WORKSHOP_OP_ULTIMATE_WORK = 14,				//!< 请求精工
			WORKSHOP_OP_GOD_STRONGTHEN = 15,			//!< 神装强化请求
			WORKSHOP_OP_DIAN_HUA = 16,					//!< 装备点化 param1 是否穿戴中 param2 下标idx param3 点化石所在背包类 param4 点化石下标
			WORKSHOP_OP_SAVE_DIAN_HUA = 17,				//!< 保存点化 param1 是否穿戴中 param2 下标idx
			WORKSHOP_OP_FU_MO = 18,						//!< 装备附魔 param1 是否穿戴中 param2 下标idx
			WORKSHOP_OP_XI_FU = 19,						//!< 装备吸附 param1 是否穿戴中 param2 下标idx
			WORKSHOP_OP_DISCOMPOSE = 20,				//!< 分解点化石 param1 背包类型 param2 背包下表
			WORKSHOP_OP_UP_STAR = 21,					//!< 装备升星 param1 seq param2:是否灵饰	param3:装备所属位置 param4:装备所占格子 param5:消耗道具位置  param6 消耗道具2位置
			WORKSHOP_OP_UP_LEVEL_DIANHUA = 22,			//!< 点化石升级 param1 主点化石 param2 消耗点化石1 param2 消耗点化石2
		};

		short op_type;			//!< 操作类型
		short param1;			//!< 额外参数： 0 - 合成表 seq；	1 - 是否灵饰		2 - 是否灵饰		 3 - 列表id；	4 - 材料格子 1；	5 - 选项id；6 - 装备所属位置； 7 - null；			8 - 兑换Seq;		9 - 合成表序号 seq;		10:合成表seq	11:是否灵饰			12:是否灵饰			13:是否灵饰			14：1/2（精工/强化精工）	15:1/2（进阶/突破）
		short param2;			//!< 额外参数： 0 - 合成数量；		1 - 装备所属位置	2 - 装备所属位置					4 - 材料格子 2；				6 - 装备所占格子；						8 - 合成数量		9 - 合成目标物品id;		10:合成数量		11:装备所属位置		12:装备所属位置		13:装备所属位置		14：是否灵饰				15：是否灵饰
		short param3;			//!< 额外参数： 0 - 是否强化打造；	1 - 装备所占格子	2 - 装备所占格子													6 - 宝石镶嵌槽位； 7 - 所需材料物品id																11:装备所占格子		12:装备所占格子		13:装备所占格子		14：装备所属位置			15：装备所属位置
		short param4;			//!< 额外参数 : 0 - 0:不是化身打开  1 - 宝石所占格子	2 - 宝石镶嵌槽位																																						11:词条所属位置		12:锁定格子(二进)	13:是否化身			14：装备所占格子			15：装备所占格子
		short param5;			//!<																																																							11:是否化身					12:是否化身								14：是否化身				15：是否化身
		short param6;			//!<																																																												12:是否使用元宝替代
		short reserve;
	};

	class CSWorkshopDecomposeReq
	{
	public:
		const static int MAX_DECOMPOSE_NUM = 20;
		CSWorkshopDecomposeReq();		//!< 3600 - 装备分解请求
		MessageHeader header;

		int grid_num;
		int decompose_grid[MAX_DECOMPOSE_NUM];
	};

	class SCWorkshopAllInfoRespond
	{
	public:
		SCWorkshopAllInfoRespond();		//!< 1731 - 工坊所有数据信息
		MessageHeader header;

		int voc_refresh_times;								//!< 今天装备熔炼次数
		BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_active_flag;	//!< 所有可激活的配方的激活状态
		BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_created_flag;	//!< 所有配方的制造状态
	};

	class SCSingleRecipeActiveFlag
	{
	public:
		SCSingleRecipeActiveFlag();		//!< 1732 - 单个配个激活信息
		MessageHeader header;

		unsigned short active_id;		//!< 激活序列
		char is_active;					//!< 是否激活： 0 - 未激活； 1 - 激活
		char is_notice;					//!< 是否弹窗: 0:不弹 1:弹窗提示
	};

	class SCJewelryReforgeOutcomes
	{
	public:
		SCJewelryReforgeOutcomes();		//!< 1733 - 灵饰重锻结果通知
		MessageHeader header;

		JewelryProReserve reforge_list[JEWELTY_REFORGE_LIST_MAX];
	};

	class SCGemOneKeyUpgradesNeeds
	{
	public:
		SCGemOneKeyUpgradesNeeds();		//!< 1734 - 一键升级宝石消耗的材料
		MessageHeader header;

		GemUpgradedata mate_data;
	};

	class CSConvertPetSkillBookReq
	{
	public:
		CSConvertPetSkillBookReq();		//!< 1735 - 宠物技能书兑换
		MessageHeader header;

		UInt16 exc_seq;
		UInt16 list_num;
		ItemPairCon grid_list[PET_SKILL_BOOK_NEED_ITEM_TYPE_NUM];
	};

	class SCWorkshopComposeResults : public IMessageSerializer
	{
	public:
		SCWorkshopComposeResults();		//!< 1736 - 工坊合成结果回复
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

	class CSChangeEquipVocReq
	{
	public:
		enum VOC_CHANGE_TYPE
		{
			VOC_CHANGE_TYPE_EQUIP = 0,//装备
			VOC_CHANGE_TYPE_JEWELRY = 1,//灵饰
			VOC_CHANGE_TYPE_ULTMATE_EQUIP = 2,//淬炼 - 装备
			VOC_CHANGE_TYPE_ULTMATE_JEWELRY = 3,//淬炼 - 灵饰
		};
		CSChangeEquipVocReq();		//!< 1737 - 装备词条替换请求
		MessageHeader header;
		int change_voc_type;//更换词条类型
		int role_type;		//0:本尊  1:化身
		//获得词条装备
		int replacer_posi;
		int replacer_grid;
		int replacer_voc_index;
		//摘取词条装备
		int bereplacer_posi;
		int bereplacer_grid;
		int bereplacer_voc_index;
	};

	class CSWorkShopJewelryInfo
	{
	public:
		CSWorkShopJewelryInfo();		//!< 1738 - 灵饰合成失败次数
		MessageHeader header;
		int jewelry_times[INTJEWELRY_SUCC_MAX_GROUP];
	};

	class CSPetEquipMentReq
	{
	public:
		enum REQ_TYPR
		{
			REQ_TYPR_COMPOSE = 0,
			REQ_TYPR_REFRESH = 1,
			REQ_TYPR_SAVE_REFRESH = 2,
			REQ_TYPR_BREAK = 3,
		};
		CSPetEquipMentReq();
		MessageHeader header;
		int req_type;
		int param1;
		int param2;
		int param3;
	};
}

#pragma pack(pop)
#endif