#ifndef __MSG_HOLY_EQUIP_H__
#define __MSG_HOLY_EQUIP_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

static const int MAX_HOLY_LEVEL_UP_ITEM_TYPE = 4;		//!< 圣器升级材料类型
static const int MAX_HOLY_LEVEL_UP_INDEX_NUM = 30;		//!< 单次升级请求最多可放入的圣器数量

namespace Protocol
{

	class CSHolyEquipOpReq				//!<4830 圣器客户端操作请求
	{
	public:
		CSHolyEquipOpReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_INFO_REQ = 0,		//!< 请求信息
			OP_TYPE_PUT_ON = 1,			//!< 更改圣器状态 param1 背包格子序号 param2 修改到的状态 HOLY_EQUIP_STATUS
			OP_TYPE_UNLOCK_HOLE = 2,	//!< 解锁主动技能孔位 param1 要解锁的圣器index, param2 要解锁的孔位
			OP_TYPE_STUDY_NEW_SKILL = 3,//!< 领悟新的主动技能 param1 要领悟的圣器格子序号, param2 使用的领悟道具id
			OP_TYPE_SAVE_NEW_SKILL = 4,	//!< 保存洗练出来的新技能 param1 要保存的圣器格子序号, param2 保存或放弃(1--保存, 0--放弃)

			OP_TYPE_INLAY_FU_SHI = 5,	//!< 镶嵌符石 param1 要镶嵌的圣器格子序号, param2 要镶嵌的圣器的孔位, param3 要镶嵌的符石的格子序号
			OP_TYPE_TAKE_OFF_FU_SHI = 6,//!< 取下符石 param1 要取下符石的圣器格子序号, param2 要取下符石的圣器的符石孔位
			OP_TYPE_FU_SHI_FUSION = 7,	//!< 合成符石 param1 目标等级 param2 符石index1, param3 符石index2, param4 符石index3, param5 符石index4, param6 符石index5
			OP_TYPE_FU_SHI_RECAST = 8,	//!< 重铸符石 param1 要重铸的符石index, param2 重铸消耗的符石index
			OP_TYPE_UNLOCK_FUSHI_HOLE = 9,//!< 解锁符石孔 param1 圣器index param2 符石孔位0-4
			OP_TYPE_FU_SHI_RECAST_ON_HOLY_EQUIP = 10,	//!< 重铸圣器上的符石, param1 圣器index, param2 要重铸的符石孔位, param3 重铸消耗的符石index
			OP_TYPE_HOLY_BLESS_PET = 11,//!< 宠物圣佑 param1 圣器index param2 宠物index(-1表示取消圣佑)
			OP_TYPE_HOLY_FOLLOW	= 12,	//!< 圣器跟随 param1 圣器index
			OP_TYPE_HOLY_SKILL_CONVERT = 13,	//!< 圣器技能书置换 param1 技能书物品id_1, param2 技能书物品id_2
		};

		int op_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		int param6;
	};


	class CSHolyEquipLevelUpReq				//!<4831 圣器升级请求
	{
	public:
		CSHolyEquipLevelUpReq();
		MessageHeader header;

		int holy_equip;		// 要升级的圣器idx

		short material_type_num;				// 材料种类数量
		short holy_index_num;					// 放入圣器数量
		unsigned short material_type[MAX_HOLY_LEVEL_UP_ITEM_TYPE];		// 材料id, MAX_HOLY_LEVEL_UP_ITEM_TYPE = 4
		short material_num[MAX_HOLY_LEVEL_UP_ITEM_TYPE];		// 材料数量
		short holy_index[MAX_HOLY_LEVEL_UP_INDEX_NUM];		// 以圣器作为材料,最大可放入30个
	};


	class SCHolyBagListInfo : public IMessageSerializer			//!<4832 圣器背包全部信息下发
	{
	public:
		SCHolyBagListInfo();
		virtual ~SCHolyBagListInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(item_count, buffer, buffer_size, out_length);

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
				result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);
			
				result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);			
			}

			return result;
		}

		int	item_count;		//!< 物品数量
		InfoType2 item_list[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];	//!< 物品列表
	};



	class SCFuShiBagListInfo : public IMessageSerializer			//!<4833 符石背包全部信息下发
	{
	public:
		SCFuShiBagListInfo();
		virtual ~SCFuShiBagListInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(item_count, buffer, buffer_size, out_length);

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
				result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);
				
				result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);			
			}

			return result;
		}

		int	item_count;		//!< 物品数量
		InfoType2 item_list[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];	//!< 物品列表
	};



	class SCHolyBagItemChange : public IMessageSerializer		//!<4834 圣器/符石背包单个信息下发
	{
	public:
		enum BAG_TYPE
		{
			BAG_TYPE_HOLY = 1,
			BAG_TYPE_FUSHI = 2,
		};

	public:
		SCHolyBagItemChange();
		virtual ~SCHolyBagItemChange() {}
		MessageHeader header;

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
			result = result && WriteChar(bag_type, buffer, buffer_size, out_length);
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

		short				reason;					//!< 改变的原因 \see HOLY_EQUIP_SEND_REASON
		short				put_reason;				//!< 获得物品的来源  \see PUT_REASON_TYPE
		ItemID				item_id;				//!< 物品id
		short				index;					//!< 物品在背包中的序号 
		char				is_market_buy_flag;		//!< 是否从市场购买标识 0:否 1:从市场购买
		char				bag_type;				//!< 背包类型,1-圣器背包,2-符石背包 BAG_TYPE
		short				reserve_sh;
		int					num;					//!< 数量
		char				is_bind;				//!< 是否绑定
		char				has_param;				//!< 是否有净值参数
		short				param_length;			//!< 净值参数的数据长度 （当has_param为1时可读）
		UInt32				invalid_time;			//!< 过期时间
		long long           item_unique_id;			//!< 道具唯一ID
		NetValueItemParamData param_data;			//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
	};

	class SCSCHolyEquipLevelUpConsumeInfo		//!<4835 圣器 圣器圣器/符石合成消耗列表下发
	{
	public:
		SCSCHolyEquipLevelUpConsumeInfo();
		MessageHeader header;

		enum BAG_TYPE
		{
			BAG_TYPE_HOLY = 1,
			BAG_TYPE_FUSHI = 2,
		};

		int bag_type;
		int count;
		int index[MAX_HOLY_LEVEL_UP_INDEX_NUM];
	};

	class SCHolyEquipFirstGainInfo		//4836 圣器 第一次获得时特殊指引
	{
	public:
		SCHolyEquipFirstGainInfo();
		MessageHeader header;

		int first_gain;		// 0-未触发,1-已触发过
	};

	class SCHolyEquipFollowInfo		//4837 圣器 跟随信息
	{
	public:
		SCHolyEquipFollowInfo();
		MessageHeader header;

		int follow_index;
	};

};

#pragma pack(pop)

#endif
