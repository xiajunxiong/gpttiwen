#ifndef __MSG_LIFE_SKILL_HPP__
#define __MSG_LIFE_SKILL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolenewlifeskilldef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class CSNewLifeSkillReq		//!< 1310 生活技能请求
	{
	public:
		CSNewLifeSkillReq();
		MessageHeader header;

		enum NEW_LIFE_SKILL_OP_TYPE
		{
			NEW_LIFE_SKILL_OP_TYPE_ALL_INFO = 0,			//!< 请求生活技能信息
			NEW_LIFE_SKILL_OP_TYPE_LEVEL_UP = 1,			//!< 请求升级生活技能 P1:new_life_skill_type
			NEW_LIFE_SKILL_OP_TYPE_MAKE_ITEM = 2,			//!< 请求制作生活道具	 P1:sn P2:is_use_aux_tool(0:没有使用 1:使用了辅助工具) P3:count
			NEW_LIFE_SKILL_OP_TYPE_PROPS_INFO = 3,			//!< 请求生活食物类型信息
			NEW_LIFE_SKILL_OP_TYPE_USE_CARD = 4,			//!< 请求是否使用变身卡形象 P1:is_use_change_card(0:不使用 1:使用变身卡形象)
			NEW_LIFE_SKILL_OP_TYPE_START_SURFACE = 5,		//!< 请求幻化 P1:seq
			NEW_LIFE_SKILL_OP_TYPE_RELIEVE_SURFACE = 6,		//!< 请求解除幻化 
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};

	class SCNewLifeSkillAllInfo				//!< 1311 生活技能信息下发
	{
	public:
		SCNewLifeSkillAllInfo();
		MessageHeader header;

		int count;
		NewLifeSkillInfo new_life_skill_list[MAX_NEW_LIFT_SKILL_TYPE];
	};

	class SCNewLifeSkillSingleInfo			//!< 1312 生活技能单个信息下发
	{
	public:
		SCNewLifeSkillSingleInfo();
		MessageHeader header;

		int new_life_skill_type;
		NewLifeSkillInfo skill_info;
	};

	class SCNewLifeSkillPropsAllInfo : public IMessageSerializer		//!< 1313 生活食物类型信息下发
	{
	public:
		SCNewLifeSkillPropsAllInfo();
		virtual ~SCNewLifeSkillPropsAllInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(life_skill_props_list); i++)
			{
				const NewLifeSkillPropsInfo & props_info = life_skill_props_list[i];
				result = result && WriteShort(props_info.seq, buffer, buffer_size, out_length);
				result = result && WriteUInt(props_info.end_timestamp, buffer, buffer_size, out_length);
			}
			result = result && WriteInt(change_surface_count, buffer, buffer_size, out_length);
			for (int i = 0; i < change_surface_count && i < ARRAY_LENGTH(change_surface_list); i++)
			{
				const NewLifeSkillChangeSurfaceInfo & surface_info = change_surface_list[i];
				result = result && WriteShort(surface_info.seq, buffer, buffer_size, out_length);
				result = result && WriteUInt(surface_info.surface_point, buffer, buffer_size, out_length);
			}

			return result;
		}

		int count;
		int change_surface_count;			
		NewLifeSkillPropsInfo life_skill_props_list[MAX_NEW_LIFE_SKILL_PROPS_NUM];	//食物道具类型列表
		NewLifeSkillChangeSurfaceInfo change_surface_list[MAX_NEW_LIFE_SKILL_CHANGE_SURFACE_NUM]; //变身卡信息列表(幻化)
	};

	class SCNewLifeSkillPropsSingleInfo		//!< 1314 生活食物类型单条下发
	{
	public:
		SCNewLifeSkillPropsSingleInfo();
		MessageHeader header;

		int is_noitce;						//0:不通知
		int new_life_skill_props_type;		// type->NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD P1:幻化点数
		int param1;							
		NewLifeSkillPropsInfo props_info;
	};

	class SCNewLifeSkillChangeCardSingleInfo	//!< 1318 变身卡单条信息下发
	{
	public:
		SCNewLifeSkillChangeCardSingleInfo();
		MessageHeader header;

		int seq;					//变身卡索引
		int surface_point;			//该变身卡幻化点
	};

}

#pragma pack(pop)

#endif	// __MSG_LIFE_SKILL_HPP__