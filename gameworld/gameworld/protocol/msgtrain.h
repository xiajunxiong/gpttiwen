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
	class SCTrainAllInfo :public IMessageSerializer2				//!< 1920 ���� ��Ϣ
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
		CS_TRAIN_REQ_TYPE_TRAIN,		//!<�������ܣ�param1 train_type ,param2  �����Ĵ���
		CS_TRAIN_REQ_TYPE_LIU_LI_DING,	//!<������  ��param1 Ҫ�ϳ���Ʒ������seq
		CS_TRAIN_REQ_TYPE_INFO_PROMOTED,//!<������������Ϣ�·�
		CS_TRAIN_REQ_TYPE_SET_RED_DOT,	//!<���ú����ʾ param1:train_type
		CS_TRAIN_REQ_TYPE_PET_TRAIN_2,			//!<�����������ܣ�param1 train_type ,param2  �����Ĵ���
		CS_TRAIN_REQ_TYPE_PARTNER_TRAIN,		//!<����������ܣ�param1 train_type ,param2  �����Ĵ���
	};

	class CSTrainReq						//!< 1921 ���� 
	{
	public:
		CSTrainReq();
		MessageHeader		header;

		int req_type;
		int param1;	
		int param2;					
	};

	class CSRolePetTrainSkillReq						//!< 1922 ���� �������Լ���
	{
	public:
		CSRolePetTrainSkillReq();
		MessageHeader		header;

		int train_num;					//!< �����Ĵ���
	};

	class SCTrainInfoPromoted				//!< 1923 ����������������Ϣ�·�
	{
	public:
		SCTrainInfoPromoted();
		MessageHeader header;

		int physical_dec_damage;			// ���������˼���  (ǧ�ֱ�)
		int magic_dec_damage;				// ���������˼���	(ǧ�ֱ�)
	};

	class SCTrainRedDotFlagInfo				//!< 1924 �������ܺ���ʶ�·�
	{
	public:
		SCTrainRedDotFlagInfo();
		MessageHeader  header;

		short pet_train_red_dot_flag;						//0 ����ʾ 1����Ҫ��ʾ
		unsigned short train_red_dot_flag;					//��������ʶ ��λ���㣩
	};
}

#pragma pack(pop)

#endif

