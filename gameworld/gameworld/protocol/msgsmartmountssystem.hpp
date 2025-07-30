#ifndef __MSG_SMART_MOUNTS_SYSTEM_HPP__
#define __MSG_SMART_MOUNTS_SYSTEM_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/smartmountssystemdef.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSSmartMountsSystemReq			//!< 2100 ����ϵͳ����
	{
	public:
		CSSmartMountsSystemReq();
		MessageHeader header;

		enum SMART_MOUNTS_SYSTEM_OP_TYPE
		{
			SMART_MOUNTS_SYSTEM_OP_TYPE_ALL_INFO = 0,				//!< ����������Ϣ
			SMART_MOUNTS_SYSTEM_OP_TYPE_SINGLE_INFO = 1,			//!< �������ﵥ����Ϣ param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE = 2,					//!< ���󼤻�����		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_LEVEL_UP = 3,				//!< ������������		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_BREAK_LEVEL = 4,			//!< ����ͻ�Ƶȼ�		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_REFINE_NATURE = 5,			//!< ����ϴ������		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_SKILL_UP = 6,				//!< ������������		param1 : index param2 : base_or_exclusive[0, 1] param3 : skill_index(���ܸ���) (����)
			SMART_MOUNTS_SYSTEM_OP_TYPE_REFINE_SKILL = 7,			//!< ����ϴ������		param1 : index param2 : ���� [0-2bit]������ ��0:û����, ��1:���� param3 : 0.���Զ����� (����)
			SMART_MOUNTS_SYSTEM_OP_TYPE_SAVE_REFINE_SKILL = 8,		//!< ���󱣴�ϴ��		param1 : index (����)
			SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_REFINE_SKILL = 9,	//!< ����ȡ��ϴ��		param1 : index (����)
			SMART_MOUNTS_SYSTEM_OP_TYPE_WEAR_ORNAMENTS = 10,		//!< ���󴩴���Ʒ		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_TAKE_OFF = 11,				//!< ����������Ʒ		param1 : index 
			SMART_MOUNTS_SYSTEM_OP_TYPE_ADD_ENERGY = 12,			//!< ���󲹳侫��		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_RESET_SKILL = 13,			//!< ����������		param1 : index param2 : base_or_exclusive[0, 1] param3 : skill_index(���ܸ���) (����)
			SMART_MOUNTS_SYSTEM_OP_TYPE_DOMINANCE_PET = 14,			//!< ����ͳ������		param1 : index param2 : pet_index param3 : dominance_index[0, 1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_DOMINANCE = 15,		//!< ����ȡ��ͳ��		param1 :��index param2 : dominance_index[0, 1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_ATTR_REQ = 16,				//!< ���󵥸��������� param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_FINISH_FUN_GUIDE = 17,		//!< �������ָ��	
			SMART_MOUNTS_SYSTEM_OP_TYPE_RIDE = 18,					//!< �������		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_RIDE = 19,			//!< ����ȡ�����		
			SMART_MOUNTS_SYSTEM_OP_TYPE_SET_IMAGE = 20,				//!< �����������		param1 : index param2 : 0:��ͨ���� 1:��������
			SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE_EQUIP = 21,			//!< ���󼤻�װ��    param1 : index param2 : equip_index[0,1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_EQUIP_LEVEL_UP = 22,		//!< ����װ������		param1 : index param2 : equip_index[0,1]
			SMART_MOUNTS_SYSTEM_OP_TYPE_JEWELRY_STRENGTH = 23,		//!< ��������ǿ��		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE_HOLY = 24,			//!< ����ʥ�Ｄ��		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_HOLY_LEVEL_UP = 25,			//!< ����ʥ��ǿ��		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_REVEAL = 26,				//!< ��������㻯		param1 : index
			SMART_MOUNTS_SYSTEM_OP_TYPE_CONVERT_ITEM = 27,			//!< ����ת������		param1 : need_convert_item param2 : target_item_id
			SMART_MOUNTS_SYSTEM_OP_TYPE_USE_ACTIVE_CONVERT_ITEM = 28,//!< ����ʹ�ü����������ת�� param1:bag_column param2:bag_index
			SMART_MOUNTS_SYSTEM_OP_TYPE_SKILL_ITEM_COMPOSE = 29,	//!< �����鼼�ϳ�		param1 : main_item_id param2:stuff_item_id param3:target_item_id
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
		UInt16 level;																					// ����ȼ�
		//short remain_skill_point;																		// ʣ�༼�ܵ�
		short energy_value;																				// ����ֵ
		UInt16 reveal_level;																			// �㻯�ȼ�
		char is_breach_flag;																			// �Ƿ񾭹�ͻ�� 1:���ͻ��			
		//char is_refine_flag;																			// �Ƿ����ϴ�� 1:��ϴ������
		char is_use_advanced_flag;																		// ����ʹ�ñ�ʶ 0:δʹ�ý������� 1:ʹ�ý�������
		char holy_strength_level;																		// ʥ��ǿ���ȼ�
		char reserve_ch;
		int capability;																					// ����
		//short exclusive_level_list[MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM];					// ר�����ܵȼ�
		SmartMountsSystemItemData jewelry_data;															// ��������
		SmartMountsSystemItemData equip_list[MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM];					// װ������
		SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];	// ��������
		//UInt16 new_base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];						// ϴ����������ID
		char dominance_pet_index_list[MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM];					// ͳ������index
	};


	class SCSmartMountsSystemAllInfo					//!< 2101 ����ϵͳ��Ϣ�·�
	{
	public:
		SCSmartMountsSystemAllInfo();
		MessageHeader header;

		int count;
		SmartMountsSystemSinglePro smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];
	};

	class SCSmartMountsSystemSingleInfo					//!< 2102 ����ϵͳ������Ϣ�·�
	{
	public:
		SCSmartMountsSystemSingleInfo();
		MessageHeader header;

		int index;
		SmartMountsSystemSinglePro smart_mounts;
	};

	class SCSmartMountsSystemSingleAttr					//!< 2103  ������������б��·�
	{
	public:
		SCSmartMountsSystemSingleAttr();
		MessageHeader header;

		short index;
		short reserve_sh;
		AttrInfo attr_list[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];
	};

	class SCSmartMountsSystemDominanceInfo				//!< 2104 ����ͳ��������Ϣ�·�
	{
	public:
		SCSmartMountsSystemDominanceInfo();
		MessageHeader header;

		short index;
		char dominance_pet_index_list[MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM];
	};

	class SCSmartMountsSystemShowInfo					//!< 2105 ���ﵯ������չʾ��Ϣ�·�
	{
	public:
		SCSmartMountsSystemShowInfo();
		MessageHeader header;

		enum SHOW_NOTICE_TYPE
		{
			SHOW_NOTICE_TYPE_ACTIVE = 0,						// ��������		param1:smart_mounts_level 
			SHOW_NOTICE_TYPE_LEVEL_UP = 1,						// �ȼ�����		param1:smart_mounts_level param2:remain_skill_point 
			SHOW_NOTICE_TYPE_SKILL_UP_BASE = 2,					// ������������	param1:skill_level param2:skill_index 
			SHOW_NOTICE_TYPE_SKILL_UP_EXCLUSIVE = 3,			// ר����������	param1:skill_level param2:skill_index 
			SHOW_NOTICE_TYPE_BREAK_SUCC = 4,					// ͻ�Ƴɹ�		
			SHOW_NOTICE_TYPE_ENERGY_VALUE = 5,					// ���ﾫ������	param1:energy_value
			SHOW_NOTICE_TYPE_ACTIVE_EQUIP = 6,					// ����װ��		param1:equip_index param2:strength_level
			SHOW_NOTICE_TYPE_EQUIP_LEVEL_UP = 7,				// ����װ��		param1:equip_index param2:strength_level
			SHOW_NOTICE_TYPE_ACTIVE_HOLY = 8,					// ����ʥ��		param1:strength_level
			SHOW_NOTICE_TYPE_HOLY_LEVEL_UP = 9,					// ʥ������		param1:strength_level
			SHOW_NOTICE_TYPE_REVEAL_UP = 10,					// �㻯����		param1:reveal_level
			SHOW_NOTICE_TYPE_LEARN_SKILL = 11,					// ѧϰ����		param1:skill_index param2:skill_id param3:skill_level
			SHOW_NOTICE_TYPE_REPLACE_SKILL = 12,				// �û�����		param1:skill_index param2:skill_id param3:skill_level
		};

		short noitce_type;
		short index;
		short param1;
		short param2;
		short param3;
		short param4;
	};

	class SCSmartMountsSystemRefineSkillInfo			//!< 2106 ����ϴ�����ܽ���·�
	{
	public:
		SCSmartMountsSystemRefineSkillInfo();
		MessageHeader header;

		char index;
		char is_refine_flag;							// �Ƿ����ϴ�� 1:��ϴ������
		UInt16 new_base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];
		SmartMountsSystemBaseSkillParam base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];		//MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM = 3
	};

	class SCSmartMountsSystemOtherInfo					//!< 2107 ����������Ϣ�·�
	{
	public:
		SCSmartMountsSystemOtherInfo();
		MessageHeader header;

		char is_finish_fun_guide_flag;			// �Ƿ���ɹ���ָ�� 0:δ��� 1:�����
		char ride_index;						// ��Ҵ�ʱ��˵����� -1:δ��� 
		char is_use_advanced_flag;				// ����ʹ�ñ�ʶ 0:δʹ�ý������� 1:ʹ�ý�������
		char reserve_ch;
	};

	class SCSmartMountsSystemRelatedInfo					//!< 2108 ���������Ϣ�·�
	{
	public:
		SCSmartMountsSystemRelatedInfo();
		MessageHeader header;

		enum INFO_TYPE
		{
			INFO_TYPE_ENERGY_CHANGE = 0,			//����ֵ�仯		param1:����ֵ
			INFO_TYPE_NATURE_CHANGE = 1,			//����ֵ�仯		param1:����ֵ
			INFO_TYPE_WEAR_JEWELRY = 2,				//������Ʒ		param1:��Ʒǿ���ȼ�
			INFO_TYPE_JEWELRY_LEVEL_UP = 3,			//������Ʒ		param1:��Ʒǿ���ȼ�
			INFO_TYPE_SET_IMAGE = 4,				//��۱仯		param1:����ʹ�ñ�ʶ 0:δʹ�ý������� 1:ʹ�ý�������
		};

		short op_type;	
		short index;
		int param1;
	};

	class CSSmartMountsSystemSkillReq			//!< 2109 ����ϵͳ��������
	{
	public:
		CSSmartMountsSystemSkillReq();
		MessageHeader header;

		enum SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE
		{
			SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_LEARN_SKILL = 0,		//!< ����ѧϰ���＼��		param1 : index param2:skill_index param3:column param4:bag_index
			SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_SKILL_UP = 1,			//!< �����������＼��		param1 : index param2:skill_index param3:column param4:bag_index
			SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_SKILL_REPLACE = 2,	//!< �����û����＼��		param1 : index param2:skill_index param3:column param4:bag_index
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