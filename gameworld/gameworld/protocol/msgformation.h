#ifndef __MSG_FORMATION_H__
#define __MSG_FORMATION_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/formationdef.hpp"
#include "servercommon/pethelperformationdef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class CSFormationDetailInfo		//!< 2952 �����ȡ������ϸ��Ϣ
	{
	public:
		CSFormationDetailInfo();
		MessageHeader header;

		int formation_seq;
	};

	class SCFormationDetailInfo		//!< 2953 ������ϸ��Ϣ����
	{
	public:
		SCFormationDetailInfo();
		MessageHeader header;

		struct PartnerPos
		{
			int partner_id;		//!< ���ID
			short partner_pos;	//!< ����վλ [0, 10)��2��7���⣬ �һ�黥�಻��վ��ͬһ��
			short reserve_sh;	//!< ������
		};

		enum REASON_TYPE
		{
			REASON_TYPE_INFO = 0, //!< �鿴��Ϣ
			REASON_TYPE_SAVE = 1, //!< �������
			REASON_TYPE_SAVE_NEW_FORMATION = 2, //!< ��������
		};

		char reason;			//!< �·�����  \see REASON_TYPE
		char is_valid;			//!< ����Ƿ���Ч��0�������ݣ�
		char role_row;			//!< ����վλ 0:���� 1:ǰ��
		char formation_seq;		//!< ����seq 	
		FormationAutoInfo auto_info;
		short pet_idx;			//!< �������� -1��ʾ���ų���
		short partner_num;		//!< ������� ��������д�����partner_list
		PartnerPos partner_list[SIDE_MAX_ROLE_NUM - 1];
	};

	class CSFormationSimpleInfo		//!< 2950 �������ݼ�����Ϣ���򿪽���ʱ����
	{
	public:
		CSFormationSimpleInfo();
		MessageHeader header;
	};

	class SCFormationSimpleInfo		//!< 2951 ���ݼ�����Ϣ����
	{
	public:
		SCFormationSimpleInfo();
		MessageHeader header;

		struct SimpleInfo
		{
			short seq;			//!< ���  [0, valid_formation_num)
			short reserve_sh;	//!< ������
			GameName formation_name; //!< ����
		};

		short valid_formation_num;	//!< ��Ч����������
		short active_formation_num;	//!< ���õ���������

		SimpleInfo formation_list[MAX_FORMATION_NUM]; //!< �����õ�����������ȡ
	};

	class CSFormationRename		//!< 2954 ���ݸ���
	{
	public:
		CSFormationRename();
		MessageHeader header;

		int formation_seq;		//!< �������
		GameName new_name;		//!< ������
	};

	class SCFormationRename		//!< 2955 ���ݸ����ذ�
	{
	public:
		SCFormationRename();
		MessageHeader header;

		int formation_seq;		//!< �������
		GameName new_name;		//!< ������
	};

	class CSFormationSave		//!< 2956 �������ݸĶ�
	{
	public:
		CSFormationSave();
		MessageHeader header;

		struct PartnerPos
		{
			int partner_id;		//!< ���ID
			short partner_pos;	//!< ����վλ [0, 10)��2��7���⣬ �һ�黥�಻��վ��ͬһ��
			short reserve_sh;	//!< ������
		};

		struct TeamMemberPos
		{
			int member_uid;		//!< ��Ա��uid
			short member_pos;	//!< ��Ավλ 0, 1, 3, 4
			short reserve_sh;	//!< ������
		};

		int is_save_new_format;	//!< 0:��ɲ���  1:����Ϊ������
		short formation_seq;	//!< ����seq 
		short role_row;			//!< ����վλ 0:���� 1:ǰ��
		short pet_idx;			//!< �������� -1��ʾ���ų���
		FormationAutoInfo auto_info;
								//short standby_idx1;		//!< ��������1��
								//short standby_idx2;		//!< ��������2��
								//short standby_idx3;		//!< ��������3��
								//short reserve_sh;		//!< ������

		short partner_num;		//!< ������� 
		PartnerPos partner_list[SIDE_MAX_ROLE_NUM - 1]; // ���ܻ�������ж��٣���Ҫ��4��PartnerPos

		int team_member_num;	//!< ��Ա���� ��������д�����member_list
		TeamMemberPos member_list[SIDE_MAX_ROLE_NUM - 1];
	};

	UNSTD_STATIC_CHECK(4 == ARRAY_ITEM_COUNT(CSFormationSave::partner_list));
	UNSTD_STATIC_CHECK(4 == ARRAY_ITEM_COUNT(CSFormationSave::member_list));

	class CSFormationDelete		//!< 2957 ɾ�����ݣ����ĳ�����ݣ�ע�⵱ǰ���ݲ�����ɾ����
	{
	public:
		CSFormationDelete();
		MessageHeader header;

		int formation_seq;		//!< ���
	};

	struct FormationAutoSmallerInfo
	{
		FormationAutoSmallerInfo()
		{
			this->Reset();
		}

		void Reset()
		{
			battle_auto_first_move_type = 0;
			battle_auto_first_skill_level = 0;
			battle_auto_first_skill_id = 0;
			battle_auto_second_move_type = 0;
			battle_auto_second_skill_level = 0;
			battle_auto_second_skill_id = 0;
		}

		char		battle_auto_first_move_type;
		char		battle_auto_first_skill_level;
		UInt16		battle_auto_first_skill_id;
		char		battle_auto_second_move_type;
		char		battle_auto_second_skill_level;
		UInt16		battle_auto_second_skill_id;
	};

	class CSSetPetHelperFormation	//!< 2970 ���ó�����ս����
	{
	public:
		CSSetPetHelperFormation();
		MessageHeader header;

		struct GridInfo
		{
			int id;					// ����uid �� ����id �� ���id 
			short type;				// 0Ϊ��  1Ϊ����  2Ϊ����  3Ϊ���
			unsigned char pet_exclusive_skill_disable_flag;
			unsigned char lian_yao_exclusive_skill_disable_flag;
			long long unique_id;    // Ϊ����ʱ�������ΨһID
		};

		struct PetStandbyInfo
		{
			int pet_id;				// ��������id ֵΪ0��û��
			long long unique_id;	// ��������ΨһID
			unsigned char pet_exclusive_skill_disable_flag;
			unsigned char lian_yao_exclusive_skill_disable_flag;
			unsigned short reserved;
		};

		bool CheckStanbyPetNum()
		{
			int standby_num = 0;
			for (int i = 0; i < ARRAY_ITEM_COUNT(standby_list); ++i)
			{
				if (standby_list[i].pet_id > 0) standby_num += 1;
			}

			if (standby_num >= FORMATION_STANDBY_PET_NUM)
			{
				if (grid_list[2].type == PET_HELPER_FORMATION_GRID_PET ||
					grid_list[7].type == PET_HELPER_FORMATION_GRID_PET)
				{
					return false;
				}
			}

			return true;
		}

		bool CheckDuplicate()
		{
			for (int i = 0; i < ARRAY_ITEM_COUNT(grid_list) - 1; ++i)
			{
				if (PET_HELPER_FORMATION_GRID_INVALID == grid_list[i].type)
				{
					continue;
				}

				for (int k = i + 1; k < ARRAY_ITEM_COUNT(grid_list); ++k)
				{
					if (grid_list[i].type == grid_list[k].type &&
						grid_list[i].id == grid_list[k].id)
					{
						if (PET_HELPER_FORMATION_GRID_PET == grid_list[i].type)
						{
							if (grid_list[i].unique_id == grid_list[k].unique_id)
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}
				}

				if (PET_HELPER_FORMATION_GRID_PET == grid_list[i].type)
				{
					for (int k = 0; k < ARRAY_ITEM_COUNT(standby_list); ++k)
					{
						if (grid_list[i].id == standby_list[k].pet_id &&
							grid_list[i].unique_id == standby_list[k].unique_id)
						{
							return false;
						}
					}
				}
			}

			for (int i = 0; i < ARRAY_ITEM_COUNT(standby_list) - 1; ++i)
			{
				if (0 == standby_list[i].pet_id)
				{
					continue;
				}

				for (int k = i + 1; k < ARRAY_ITEM_COUNT(standby_list); ++k)
				{
					if (standby_list[i].pet_id == standby_list[k].pet_id &&
						standby_list[i].unique_id == standby_list[k].unique_id)
					{
						return false;
					}
				}
			}

			return true;
		}

		int system_type; //!< ϵͳ���� \see PET_HELPER_FORMATION_TYPE
		GridInfo grid_list[SIDE_CHARACTER_NUM];
		PetStandbyInfo standby_list[FORMATION_STANDBY_PET_NUM];
		FormationAutoSmallerInfo auto_info;		// �Զ���������
	};

	class SCPetHelperFormationInfo	//!< 2971 �����·�
	{
	public:
		SCPetHelperFormationInfo();
		MessageHeader header;

		struct GridInfo
		{
			GridInfo() { this->Reset(); }

			void Reset()
			{
				id = 0;
				type = 0;
				pet_exclusive_skill_disable_flag = 0;
				lian_yao_exclusive_skill_disable_flag = 0;
				unique_id = 0;
			}

			int id;					// ����uid �� ����id �� ���id 
			short type;				// 0Ϊ��  1Ϊ����  2Ϊ����  3Ϊ���
			unsigned char pet_exclusive_skill_disable_flag;
			unsigned char lian_yao_exclusive_skill_disable_flag;
			long long unique_id;    // Ϊ����ʱ�������ΨһID
		};

		struct PetStandbyInfo
		{
			PetStandbyInfo() { this->Reset(); }
			void Reset()
			{
				pet_id = 0;
				unique_id = 0;
				pet_exclusive_skill_disable_flag = 0;
				lian_yao_exclusive_skill_disable_flag = 0;
			}

			int pet_id;				// ��������id ֵΪ0��û��
			long long unique_id;	// ��������ΨһID
			unsigned char pet_exclusive_skill_disable_flag;
			unsigned char lian_yao_exclusive_skill_disable_flag;
		};

		void Reset()
		{
			for (int i = 0; i < ARRAY_LENGTH(grid_list); ++i)
			{
				grid_list[i].Reset();
			}

			for (int i = 0; i < ARRAY_LENGTH(standby_list); ++i)
			{
				standby_list[i].Reset();
			}

			auto_info.Reset();
		}

		int system_type; //!< ϵͳ���� \see PET_HELPER_FORMATION_TYPE
		GridInfo grid_list[SIDE_CHARACTER_NUM];
		PetStandbyInfo standby_list[FORMATION_STANDBY_PET_NUM];
		FormationAutoSmallerInfo auto_info;		// �Զ���������
	};

	class CSPetHelperFormationQuery	//!< 2972 ���ݲ�ѯ
	{
	public:
		CSPetHelperFormationQuery();
		MessageHeader header;

		int system_type;	//!< ϵͳ���� \see PET_HELPER_FORMATION_TYPE
	};
}

#pragma pack(pop)
#endif