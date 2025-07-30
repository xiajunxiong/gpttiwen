#ifndef __MSG_SKILL_H__
#define __MSG_SKILL_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/skilllistparam.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	struct SkillItemInfo
	{
		short	index;			//!< ��� (���������˽�������ʶĳ�����ܣ�
		UInt16	skill_id;		//!< ����ID
		short	skill_level;	//!< ���ܵȼ�
		short	reserve_sh;		//!< ������
		int		skill_exp;		//!< ��ǰ����
	};

	class SCSkillList : public IMessageSerializer	//!< 1300 �����б�Э��
	{
	public:
		SCSkillList();
		virtual ~SCSkillList() {}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteInt(skill_count, buffer, buffer_size, out_length);

			for (int i = 0; i < skill_count && i < MAX_SKILL_NUM; ++i)
			{
				result = result && WriteShort(skill_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteUShort(skill_list[i].skill_id, buffer, buffer_size, out_length);
				result = result && WriteShort(skill_list[i].skill_level, buffer, buffer_size, out_length);
				result = result && WriteShort(skill_list[i].reserve_sh, buffer, buffer_size, out_length); // ����ͻ�������ͬ�����SkillItemInfo
				result = result && WriteInt(skill_list[i].skill_exp, buffer, buffer_size, out_length);
			}

			result = result && WriteInt(common_use_skill_num, buffer, buffer_size, out_length);
			for (int i = 0; i < common_use_skill_num && i < MAX_SKILL_NUM; ++i)
			{
				result = result && WriteShort(common_use_skill_index_list[i], buffer, buffer_size, out_length);
			}

			return result;
		}

		MessageHeader	header;

		int				skill_count;				//!< ��ѧϰ�ļ�������
		SkillItemInfo	skill_list[MAX_SKILL_NUM];	//!< �����б� 

		int				common_use_skill_num;		//!< ���ü�����
		short			common_use_skill_index_list[MAX_SKILL_NUM]; //!< ���ü����б�����Ԫ��Ϊ������� \see SkillItemInfo::index��
	};

	class SCSkillChangeNotice						//!< 1301 ������Ϣ���Э��
	{
	public:
		SCSkillChangeNotice();
		MessageHeader header;

		SkillItemInfo	skill;						//!< ����ļ���		\see SkillItemInfo
	};

	class CSSkillOperateReq							//!< 1350 ���ܲ�������
	{
	public:
		CSSkillOperateReq();
		MessageHeader header;

		enum OPERATE_TYPE
		{
			OPERATE_TYPE_GET_LIST = 0,				//!< ��ȡ�����б� 0
			OPERATE_TYPE_LEARN_SKILL = 1,			//!< ѧϰ���� 1    \param param1 ����id
			OPERATE_TYPE_USE_SKILL_GRASS = 2,		//!< ʹ�ü��ܲ� 2  \param param1 ����id  \param param2 ���ܲ�����  \param param3 �Ƿ�����[0�� 1��]
			OPERATE_TYPE_SET_COMMON_USE_SKILL = 3,	//!< ���ó��ü��� 3 \param param1 ������ţ�index�� \param param2 [0:��ӳ���  1:ɾ������]
			OPERATE_TYPE_SKILL_FAST_UP = 4,			//!< ѧϰ����(һ��) 4	\param param1 ����id	param2:Ŀ��ȼ�
			OPERATE_TYPE_SKILL_UPLEVEL = 5,			//!< �������� 5	   \param param1 ����id  param2 �Ƿ�ѡ���м��ܶ�����
		};

		int operate_type;							//!< ��������  \see OPERATE_TYPE
		int param1;
		int param2;
		int param3;
	};

	struct SkillContainer
	{
		SkillContainer() : skill_id(0) {}

		int skill_id;
		LifeSkillInfo skill_data;
	};

	class SCLifeSkillInfo						//!< 1302 �������Ϣ�·�
	{
	public:
		SCLifeSkillInfo();
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_NORMAL = 0,				//!< 0 ��������·�		
			REASON_TYPE_CHANGE_NOTICE,			//!< 1 ���֪ͨ ��������£�ֻ���·�����������Ϣ���ͻ����滻�������ܵ���ʾ���ɣ�
		};

		short reason_type;						//!< �·�Э���ԭ�� \see SCLifeSkillInfo
		short count;							//!< �·������б��Ԫ������
		SkillContainer life_skill_list[LIFE_SKILL_TYPE_MAX];  //!<  ������б�   \see LIFE_SKILL_TYPE_MAX
	};

	class SCCommonUseSkillChangeNotice			//!< 1303 ���ü��ܱ��֪ͨ
	{
	public:
		SCCommonUseSkillChangeNotice();
		MessageHeader header;

		int common_use_skill_num;						//!< ���ü�������
		short common_use_skill_idx_list[MAX_SKILL_NUM];	//!< ���ü�������б� ������common_use_skill_num��ֵ����ȡ�б���)
	};

	class SCSkillExpDeltaChangeNotify
	{
	public:
		SCSkillExpDeltaChangeNotify();			//!< 1304 ���ܾ���ı�֪ͨ
		MessageHeader header;

		int skill_id;				//!< ����ID���п�����ս�����ܣ�Ҳ�п����������
		int exp_change;				//!< ����ֵ�ı���������Ϊ���ӣ�����Ϊ����
	};

	class CSSkillPositionModify		//!< 1351
	{
	public:
		CSSkillPositionModify();
		MessageHeader header;

		int skill_num;				//!< �����б����ж��ٸ��������
		short common_use_skill_idx_list[MAX_SKILL_NUM]; //!< ��������б�
	};
}

#pragma pack(pop)
#endif