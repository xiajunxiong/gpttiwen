#ifndef __MSG_HEART_SKILL_HPP__
#define __MSG_HEART_SKILL_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleheartskilldef.hpp"
#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push, 4) 

namespace Protocol
{

	class CSHeartSkillReq			//!< 2270 �ķ�����
	{
	public:
		CSHeartSkillReq();
		MessageHeader header;

		enum HEART_SKILL_REQ_TYPE
		{
			HEART_SKILL_REQ_TYPE_INFO = 0,				//!< �����·�������Ϣ
			HEART_SKILL_REQ_TYPE_LEARN_SKILL = 1,		//!< ����ѧϰ�ķ����� P1 heart_skill_seq	P2 HEART_SKILL_LEARN_TYPE
			HEART_SKILL_REQ_TYPE_EQUIP_SKILL = 2,		//!< ����װ���ķ�		P1 SKIll_STATUS(HEART_SKILL_STATUS_MAX) P2 INDEX(����2217�����б�)
			HEART_SKILL_REQ_TYPE_TAKE_OFF_SKILL = 3,	//!< �����ж�ķ�		P1 SKIll_STATUS(HEART_SKILL_STATUS_MAX)
			HEART_SKILL_REQ_TYPE_COMPOSE = 4,			//!< ����ϳ�		P1 compose_target
			HEART_SKILL_REQ_TYPE_CONVERT = 5,			//!< ����ת��		P1 column P2 index P3 target_item_id
			HEART_SKILL_REQ_TYPE_UN_LOCK = 6,			//!< ����������ܸ���	p1 SKIll_STATUS(HEART_SKILL_STATUS_MAX)
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};


	class SCHeartSkillAllInfo : public IMessageSerializer		//!< 2271 �ķ�������Ϣ�·�
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
		unsigned char equip_skill_unlock_flag;										//������ʶ --MAX_HEART_SKILL_STATUS_NUM 0:δ���� 1:�ѽ���(������)
		short equip_skill_list[MAX_HEART_SKILL_STATUS_NUM];							//��ʱװ���ļ����б�	�����б��index
		short initiative_skill_count;
		short passive_skill_count;
		HeartSkillInfo initiative_skill_list[MAX_HEART_SKILL_INITIATIVE_NUM];		//���������б�
		HeartSkillInfo passive_skill_list[MAX_HEART_SKILL_PASSIVE_NUM];				//���������б�
	};

	class SCHeartSkillSingleInfo		//!< 2272 �ķ�������Ϣ�·�
	{
	public:
		SCHeartSkillSingleInfo();
		MessageHeader header;

		int count;
		short skill_type;
		short skill_index;
		HeartSkillInfo skill_info;
	};

	UNSTD_STATIC_CHECK(HEART_SKILL_STATUS_MAX == 6);	//������޸�ʱ,��ע������Э�����
	class SCHeartSkillOtherInfo			//!< 2273 �ķ�������Ϣ�·�
	{
	public:
		SCHeartSkillOtherInfo();
		MessageHeader header;

		char reserve_ch;
		unsigned char equip_skill_unlock_flag;				//������ʶ --MAX_HEART_SKILL_STATUS_NUM 0:δ���� 1:�ѽ���(������)
		short resrve_sh;
		short equip_skill_list[HEART_SKILL_STATUS_MAX];		//��ʱװ���ļ����б�
	};

	class CSHeartSkillDisplaceReq		//!< 2274 �ķ��û��ϳ�����
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