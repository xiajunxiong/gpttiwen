#ifndef __MSG_FORMATION_H__
#define __MSG_FORMATION_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/formationdef.hpp"
#include "servercommon/pethelperformationdef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class CSFormationDetailInfo		//!< 2952 请求获取阵容详细信息
	{
	public:
		CSFormationDetailInfo();
		MessageHeader header;

		int formation_seq;
	};

	class SCFormationDetailInfo		//!< 2953 阵容详细信息返回
	{
	public:
		SCFormationDetailInfo();
		MessageHeader header;

		struct PartnerPos
		{
			int partner_id;		//!< 伙伴ID
			short partner_pos;	//!< 伙伴的站位 [0, 10)，2和7除外， 且伙伴互相不能站在同一列
			short reserve_sh;	//!< 保留字
		};

		enum REASON_TYPE
		{
			REASON_TYPE_INFO = 0, //!< 查看信息
			REASON_TYPE_SAVE = 1, //!< 布阵完成
			REASON_TYPE_SAVE_NEW_FORMATION = 2, //!< 保存阵容
		};

		char reason;			//!< 下发理由  \see REASON_TYPE
		char is_valid;			//!< 陣容是否有效（0代表空陣容）
		char role_row;			//!< 主角站位 0:后排 1:前排
		char formation_seq;		//!< 阵容seq 	
		FormationAutoInfo auto_info;
		short pet_idx;			//!< 宠物的序号 -1表示不放宠物
		short partner_num;		//!< 伙伴数量 根据数量写下面的partner_list
		PartnerPos partner_list[SIDE_MAX_ROLE_NUM - 1];
	};

	class CSFormationSimpleInfo		//!< 2950 请求阵容简易信息（打开界面时请求）
	{
	public:
		CSFormationSimpleInfo();
		MessageHeader header;
	};

	class SCFormationSimpleInfo		//!< 2951 阵容简易信息返回
	{
	public:
		SCFormationSimpleInfo();
		MessageHeader header;

		struct SimpleInfo
		{
			short seq;			//!< 序号  [0, valid_formation_num)
			short reserve_sh;	//!< 保留字
			GameName formation_name; //!< 名字
		};

		short valid_formation_num;	//!< 有效的阵容数量
		short active_formation_num;	//!< 启用的阵容数量

		SimpleInfo formation_list[MAX_FORMATION_NUM]; //!< 按启用的阵容数量读取
	};

	class CSFormationRename		//!< 2954 阵容改名
	{
	public:
		CSFormationRename();
		MessageHeader header;

		int formation_seq;		//!< 阵容序号
		GameName new_name;		//!< 新名字
	};

	class SCFormationRename		//!< 2955 阵容改名回包
	{
	public:
		SCFormationRename();
		MessageHeader header;

		int formation_seq;		//!< 阵容序号
		GameName new_name;		//!< 新名字
	};

	class CSFormationSave		//!< 2956 保存阵容改动
	{
	public:
		CSFormationSave();
		MessageHeader header;

		struct PartnerPos
		{
			int partner_id;		//!< 伙伴ID
			short partner_pos;	//!< 伙伴的站位 [0, 10)，2和7除外， 且伙伴互相不能站在同一列
			short reserve_sh;	//!< 保留字
		};

		struct TeamMemberPos
		{
			int member_uid;		//!< 队员的uid
			short member_pos;	//!< 队员站位 0, 1, 3, 4
			short reserve_sh;	//!< 保留字
		};

		int is_save_new_format;	//!< 0:完成布阵  1:保存为新阵容
		short formation_seq;	//!< 阵容seq 
		short role_row;			//!< 主角站位 0:后排 1:前排
		short pet_idx;			//!< 宠物的序号 -1表示不放宠物
		FormationAutoInfo auto_info;
								//short standby_idx1;		//!< 待机宠物1号
								//short standby_idx2;		//!< 待机宠物2号
								//short standby_idx3;		//!< 待机宠物3号
								//short reserve_sh;		//!< 保留字

		short partner_num;		//!< 伙伴数量 
		PartnerPos partner_list[SIDE_MAX_ROLE_NUM - 1]; // 不管伙伴数量有多少，都要发4个PartnerPos

		int team_member_num;	//!< 队员数量 根据数量写下面的member_list
		TeamMemberPos member_list[SIDE_MAX_ROLE_NUM - 1];
	};

	UNSTD_STATIC_CHECK(4 == ARRAY_ITEM_COUNT(CSFormationSave::partner_list));
	UNSTD_STATIC_CHECK(4 == ARRAY_ITEM_COUNT(CSFormationSave::member_list));

	class CSFormationDelete		//!< 2957 删除阵容（清空某个阵容，注意当前阵容不可以删除）
	{
	public:
		CSFormationDelete();
		MessageHeader header;

		int formation_seq;		//!< 序号
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

	class CSSetPetHelperFormation	//!< 2970 设置宠物助战阵容
	{
	public:
		CSSetPetHelperFormation();
		MessageHeader header;

		struct GridInfo
		{
			int id;					// 主角uid 或 宠物id 或 伙伴id 
			short type;				// 0为空  1为主角  2为宠物  3为伙伴
			unsigned char pet_exclusive_skill_disable_flag;
			unsigned char lian_yao_exclusive_skill_disable_flag;
			long long unique_id;    // 为宠物时，宠物的唯一ID
		};

		struct PetStandbyInfo
		{
			int pet_id;				// 待机宠物id 值为0则没有
			long long unique_id;	// 待机宠物唯一ID
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

		int system_type; //!< 系统类型 \see PET_HELPER_FORMATION_TYPE
		GridInfo grid_list[SIDE_CHARACTER_NUM];
		PetStandbyInfo standby_list[FORMATION_STANDBY_PET_NUM];
		FormationAutoSmallerInfo auto_info;		// 自动技能设置
	};

	class SCPetHelperFormationInfo	//!< 2971 阵容下发
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

			int id;					// 主角uid 或 宠物id 或 伙伴id 
			short type;				// 0为空  1为主角  2为宠物  3为伙伴
			unsigned char pet_exclusive_skill_disable_flag;
			unsigned char lian_yao_exclusive_skill_disable_flag;
			long long unique_id;    // 为宠物时，宠物的唯一ID
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

			int pet_id;				// 待机宠物id 值为0则没有
			long long unique_id;	// 待机宠物唯一ID
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

		int system_type; //!< 系统类型 \see PET_HELPER_FORMATION_TYPE
		GridInfo grid_list[SIDE_CHARACTER_NUM];
		PetStandbyInfo standby_list[FORMATION_STANDBY_PET_NUM];
		FormationAutoSmallerInfo auto_info;		// 自动技能设置
	};

	class CSPetHelperFormationQuery	//!< 2972 阵容查询
	{
	public:
		CSPetHelperFormationQuery();
		MessageHeader header;

		int system_type;	//!< 系统类型 \see PET_HELPER_FORMATION_TYPE
	};
}

#pragma pack(pop)
#endif