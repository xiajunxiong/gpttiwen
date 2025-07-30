#ifndef __MSG_LIFE_SKILL_HPP__
#define __MSG_LIFE_SKILL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolenewlifeskilldef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class CSNewLifeSkillReq		//!< 1310 ���������
	{
	public:
		CSNewLifeSkillReq();
		MessageHeader header;

		enum NEW_LIFE_SKILL_OP_TYPE
		{
			NEW_LIFE_SKILL_OP_TYPE_ALL_INFO = 0,			//!< �����������Ϣ
			NEW_LIFE_SKILL_OP_TYPE_LEVEL_UP = 1,			//!< ������������� P1:new_life_skill_type
			NEW_LIFE_SKILL_OP_TYPE_MAKE_ITEM = 2,			//!< ���������������	 P1:sn P2:is_use_aux_tool(0:û��ʹ�� 1:ʹ���˸�������) P3:count
			NEW_LIFE_SKILL_OP_TYPE_PROPS_INFO = 3,			//!< ��������ʳ��������Ϣ
			NEW_LIFE_SKILL_OP_TYPE_USE_CARD = 4,			//!< �����Ƿ�ʹ�ñ������� P1:is_use_change_card(0:��ʹ�� 1:ʹ�ñ�������)
			NEW_LIFE_SKILL_OP_TYPE_START_SURFACE = 5,		//!< ����û� P1:seq
			NEW_LIFE_SKILL_OP_TYPE_RELIEVE_SURFACE = 6,		//!< �������û� 
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};

	class SCNewLifeSkillAllInfo				//!< 1311 �������Ϣ�·�
	{
	public:
		SCNewLifeSkillAllInfo();
		MessageHeader header;

		int count;
		NewLifeSkillInfo new_life_skill_list[MAX_NEW_LIFT_SKILL_TYPE];
	};

	class SCNewLifeSkillSingleInfo			//!< 1312 ����ܵ�����Ϣ�·�
	{
	public:
		SCNewLifeSkillSingleInfo();
		MessageHeader header;

		int new_life_skill_type;
		NewLifeSkillInfo skill_info;
	};

	class SCNewLifeSkillPropsAllInfo : public IMessageSerializer		//!< 1313 ����ʳ��������Ϣ�·�
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
		NewLifeSkillPropsInfo life_skill_props_list[MAX_NEW_LIFE_SKILL_PROPS_NUM];	//ʳ����������б�
		NewLifeSkillChangeSurfaceInfo change_surface_list[MAX_NEW_LIFE_SKILL_CHANGE_SURFACE_NUM]; //������Ϣ�б�(�û�)
	};

	class SCNewLifeSkillPropsSingleInfo		//!< 1314 ����ʳ�����͵����·�
	{
	public:
		SCNewLifeSkillPropsSingleInfo();
		MessageHeader header;

		int is_noitce;						//0:��֪ͨ
		int new_life_skill_props_type;		// type->NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD P1:�û�����
		int param1;							
		NewLifeSkillPropsInfo props_info;
	};

	class SCNewLifeSkillChangeCardSingleInfo	//!< 1318 ����������Ϣ�·�
	{
	public:
		SCNewLifeSkillChangeCardSingleInfo();
		MessageHeader header;

		int seq;					//��������
		int surface_point;			//�ñ����û���
	};

}

#pragma pack(pop)

#endif	// __MSG_LIFE_SKILL_HPP__