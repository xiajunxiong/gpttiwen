#ifndef __MSG_HEART_SKILL_HPP__
#define __MSG_HEART_SKILL_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleheartskilldef.hpp"
#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push, 4) 

namespace Protocol
{

	class CSHeartSkillReq			//!< 2270 心法请求
	{
	public:
		CSHeartSkillReq();
		MessageHeader header;

		enum HEART_SKILL_REQ_TYPE
		{
			HEART_SKILL_REQ_TYPE_INFO = 0,				//!< 请求下发所有信息
			HEART_SKILL_REQ_TYPE_LEARN_SKILL = 1,		//!< 请求学习心法技能 P1 heart_skill_seq	P2 HEART_SKILL_LEARN_TYPE
			HEART_SKILL_REQ_TYPE_EQUIP_SKILL = 2,		//!< 请求装备心法		P1 SKIll_STATUS(HEART_SKILL_STATUS_MAX) P2 INDEX(对照2217技能列表)
			HEART_SKILL_REQ_TYPE_TAKE_OFF_SKILL = 3,	//!< 请求拆卸心法		P1 SKIll_STATUS(HEART_SKILL_STATUS_MAX)
			HEART_SKILL_REQ_TYPE_COMPOSE = 4,			//!< 请求合成		P1 compose_target
			HEART_SKILL_REQ_TYPE_CONVERT = 5,			//!< 请求转换		P1 column P2 index P3 target_item_id
			HEART_SKILL_REQ_TYPE_UN_LOCK = 6,			//!< 请求解锁技能格子	p1 SKIll_STATUS(HEART_SKILL_STATUS_MAX)
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};


	class SCHeartSkillAllInfo : public IMessageSerializer		//!< 2271 心法所有信息下发
	{
	public:
		SCHeartSkillAllInfo();
		virtual ~SCHeartSkillAllInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);	

			result = result && WriteUChar(equip_skill_unlock_flag, buffer, buffer_size, out_length);
			for (int i = 0; i < HEART_SKILL_STATUS_MAX; i++)
			{
				result = result && WriteShort(equip_skill_list[i], buffer, buffer_size, out_length);
			}
			result = result && WriteShort(initiative_skill_count, buffer, buffer_size, out_length);
			result = result && WriteShort(passive_skill_count, buffer, buffer_size, out_length);
			for (int i = 0; i < initiative_skill_count && i < MAX_HEART_SKILL_INITIATIVE_NUM; i++)
			{
				result = result && WriteShort(initiative_skill_list[i].heart_skill_seq, buffer, buffer_size, out_length);
				result = result && WriteShort(initiative_skill_list[i].skill_level, buffer, buffer_size, out_length);
			}
			for (int i = 0; i < passive_skill_count && i < MAX_HEART_SKILL_PASSIVE_NUM; i++)
			{
				result = result && WriteShort(passive_skill_list[i].heart_skill_seq, buffer, buffer_size, out_length);
				result = result && WriteShort(passive_skill_list[i].skill_level, buffer, buffer_size, out_length);
			}

			return result;
		}
		
		short reserve_sh;
		char reserve_ch;
		unsigned char equip_skill_unlock_flag;										//解锁标识 --MAX_HEART_SKILL_STATUS_NUM 0:未解锁 1:已解锁(二进制)
		short equip_skill_list[MAX_HEART_SKILL_STATUS_NUM];							//此时装备的技能列表	下列列表的index
		short initiative_skill_count;
		short passive_skill_count;
		HeartSkillInfo initiative_skill_list[MAX_HEART_SKILL_INITIATIVE_NUM];		//主动技能列表
		HeartSkillInfo passive_skill_list[MAX_HEART_SKILL_PASSIVE_NUM];				//被动技能列表
	};

	class SCHeartSkillSingleInfo		//!< 2272 心法单个信息下发
	{
	public:
		SCHeartSkillSingleInfo();
		MessageHeader header;

		int count;
		short skill_type;
		short skill_index;
		HeartSkillInfo skill_info;
	};

	UNSTD_STATIC_CHECK(HEART_SKILL_STATUS_MAX == 6);	//当这个修改时,请注意下面协议对齐
	class SCHeartSkillOtherInfo			//!< 2273 心法其它信息下发
	{
	public:
		SCHeartSkillOtherInfo();
		MessageHeader header;

		char reserve_ch;
		unsigned char equip_skill_unlock_flag;				//解锁标识 --MAX_HEART_SKILL_STATUS_NUM 0:未解锁 1:已解锁(二进制)
		short resrve_sh;
		short equip_skill_list[HEART_SKILL_STATUS_MAX];		//此时装备的技能列表
	};

	class CSHeartSkillDisplaceReq		//!< 2274 心法置换合成请求
	{
	public:
		CSHeartSkillDisplaceReq();
		MessageHeader header;

		int count;
		HeartSkillDisplaceInfo displace_list[MAX_HEART_SKILL_DISPLACE_NEED_NUM];
	};

}

#pragma pack(pop)


#endif //__MSG_HEART_SKILL_HPP__