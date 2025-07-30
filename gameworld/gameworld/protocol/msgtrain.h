#ifndef __MSG_TRAIN_H__
#define __MSG_TRAIN_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/traindef.hpp"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCTrainAllInfo :public IMessageSerializer2				//!< 1920 修炼 信息
	{
	public:
		SCTrainAllInfo();
		MessageHeader		header;

		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool ret = true;

			TLVSerializer2 & out = *outstream;
			ret = ret && header.Serialize(outstream);
			ret = ret && out.Push(count);
			ret = ret && pet_train.Serialize(outstream);
			for (int i = 0; ret && i < count; ++i)
			{
				const TrainData & curr = train_list[i];

				ret = ret && curr.Serialize(outstream);
			}

			return ret;
		}

		struct TrainData
		{
			bool Serialize(TLVSerializer2 * outstream) const 
			{
				return outstream->Push(type) && outstream->Push(level) && outstream->Push(exp);
			}

			short type;
			unsigned short level;
			int exp;
		};

		int count;
		TrainData pet_train;
		TrainData train_list[50];
	};

	enum CS_TRAIN_REQ_TYPE
	{
		CS_TRAIN_REQ_TYPE_TRAIN,		//!<修炼技能，param1 train_type ,param2  修炼的次数
		CS_TRAIN_REQ_TYPE_LIU_LI_DING,	//!<琉璃鼎  ，param1 要合成物品的配置seq
		CS_TRAIN_REQ_TYPE_INFO_PROMOTED,//!<请求已提升信息下发
		CS_TRAIN_REQ_TYPE_SET_RED_DOT,	//!<设置红点提示 param1:train_type
		CS_TRAIN_REQ_TYPE_PET_TRAIN_2,			//!<宠物修炼技能，param1 train_type ,param2  修炼的次数
		CS_TRAIN_REQ_TYPE_PARTNER_TRAIN,		//!<伙伴修炼技能，param1 train_type ,param2  修炼的次数
	};

	class CSTrainReq						//!< 1921 修炼 
	{
	public:
		CSTrainReq();
		MessageHeader		header;

		int req_type;
		int param1;	
		int param2;					
	};

	class CSRolePetTrainSkillReq						//!< 1922 修炼 宠物属性技能
	{
	public:
		CSRolePetTrainSkillReq();
		MessageHeader		header;

		int train_num;					//!< 修炼的次数
	};

	class SCTrainInfoPromoted				//!< 1923 修炼技能已提升信息下发
	{
	public:
		SCTrainInfoPromoted();
		MessageHeader header;

		int physical_dec_damage;			// 已提升物伤减免  (千分比)
		int magic_dec_damage;				// 已提升法伤减免	(千分比)
	};

	class SCTrainRedDotFlagInfo				//!< 1924 修炼技能红点标识下发
	{
	public:
		SCTrainRedDotFlagInfo();
		MessageHeader  header;

		short pet_train_red_dot_flag;						//0 不提示 1：需要提示
		unsigned short train_red_dot_flag;					//修炼红点标识 （位运算）
	};
}

#pragma pack(pop)

#endif

