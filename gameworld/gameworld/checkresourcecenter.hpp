/******************************************************************************************
FileName: checkresource.hpp
Author: dreamwang
Description: 资源检查
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreamwang   2011/01/31/ 09:04:05     1.0           创建
*****************************************************************************************/

#ifndef __CHECK_RESOURCE_CENTER_HPP__
#define __CHECK_RESOURCE_CENTER_HPP__

#include <map>
#include <set>
#include <vector>

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "global/bigcross/utaconfigchecker.hpp"

#include "gamedef.h"

class CheckResourceCenter
{
public:
	static CheckResourceCenter & Instance();

	bool Check(std::string *err);

private:
	CheckResourceCenter();
	~CheckResourceCenter();

	bool CheckHelper(std::string *err);
	void Clear();

	// 物品是否有效
	struct ItemValidCheckStruct
	{
		struct CheckItem
		{
			CheckItem(ItemID _item_owner, ItemID _item_id, int _num,const char* source_info)
			: item_owner(_item_owner), item_id(_item_id), num(_num) 
			{
				info = source_info;
			}

			ItemID item_owner;
			ItemID item_id;
			int num;
			std::string info;
		};

		void Add(ItemID item_owner, ItemID item_id, int num,const char* source_info);
		bool Check(std::string *err);
		void Clear();

		std::vector<CheckItem> item_vec;
	};
	
	ItemValidCheckStruct m_item_valid_check;

	// 场景是否存在
	struct SceneCheckStruct
	{
		void Add(int scene_id, const char* source_info);
		bool Check(std::string *err);
		void Clear();

		struct SceneCheckItem
		{
			SceneCheckItem() : scene_id(0) {}
			int scene_id;
			std::string info;
		};

		std::vector<SceneCheckItem> scene_vec;
	};

	SceneCheckStruct m_scene_check;

	// 主动使用类道具检查
	struct NorexItemCheckStruct
	{
		void Add(ItemID item_id);
		bool Check(std::string *err);
		void Clear();

		std::vector<ItemID> item_vec;
	};

	NorexItemCheckStruct m_norex_check;

	// 宠物是否有效
	struct PetValidCheckStruct
	{
		struct PetCheckItem
		{
			PetCheckItem() : pet_id(0) {}

			int pet_id;
			std::string info;
		};

		struct PetAttrCheckItem
		{
			PetAttrCheckItem() : pet_id(0), attr_point(0), attr_type(0){}

			int pet_id;
			int attr_point;
			int attr_type;
			std::string info;
		};

		struct PetBenedictionCheckItem
		{
			PetBenedictionCheckItem() : benediction_id(0) {}

			int benediction_id;
			std::string info;
		};

		void Add(int pet_id, const char* source_info);
		void AddAttrCheck(int pet_id, int attr_point, int attr_type, const char* source_info);
		void AddBenedictionCheck(int benediction_id, const char* source_info);
		bool Check(std::string *err);
		void Clear();

		std::vector<PetCheckItem> pet_vec;
		std::vector<PetAttrCheckItem> pet_attr_vec;
		std::vector<PetBenedictionCheckItem> pet_benediction_vec;
	};

	PetValidCheckStruct m_pet_check;

	struct SpecialEffectCheckStruct
	{
		struct EffectCheckItem
		{
			EffectCheckItem() : param1(0) {}

			int param1;
			std::string info;
		};

		void Add(int group_id, const char* source_info);
		void AddSpecial(int sp_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<EffectCheckItem> check_vec;
		std::vector<EffectCheckItem> special_check_vec;
	};

	SpecialEffectCheckStruct m_special_effect_check;

	struct MonsterCheckStruct
	{
		struct MonsterCheckItem
		{
			MonsterCheckItem() : monster_id(0) {}

			int monster_id;
			std::string info;
		};

		void Add(int monster_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<MonsterCheckItem> check_vec;
	};
	MonsterCheckStruct m_monster_check;

	struct MonsterGroupCheckStruct
	{
		struct MonsterGroupCheckItem
		{
			MonsterGroupCheckItem() : monster_group_id(0) {}

			int monster_group_id;
			std::string info;
		};

		void Add(int monster_group_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<MonsterGroupCheckItem> check_vec;
	};
	MonsterGroupCheckStruct m_monster_group_check;

	// 检测称号是否有效
	struct TitleCheckStruct
	{
		struct TitleCheckItem
		{
			TitleCheckItem(int id, const char* source_info) :title_id(id)
			{ 
				info = source_info;
			}

			int title_id;
			std::string info;
		};
		void Add(int title_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<TitleCheckItem> check_vec;
	};

	TitleCheckStruct m_title_check;

	// 检测任务是否有效
	struct TaskCheckStruct
	{
		struct TaskCheckItem
		{
			TaskCheckItem() : task_id(0) {}

			int task_id;
			std::string info;
		};
		void Add(int task_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<TaskCheckItem> check_vec;
	};
	TaskCheckStruct m_task_check;

	// 检测任务条件物品是否有效
	struct TaskConditionItemStruct
	{
		struct TaskConditionItem
		{
			TaskConditionItem() : item_id(0) {}

			ItemID item_id;
			std::string info;
		};

		void Add(ItemID item_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<TaskConditionItem> check_vec;
	};
	TaskConditionItemStruct m_task_condition_item_check;

	// 检测伙伴是否有效
	struct PartnerCheckStruct
	{
		struct PartnerCheckItem
		{
			PartnerCheckItem(int id, const char* source_info) : partner_id(id)
			{ 
				info = source_info;
			}

			int partner_id;
			std::string info;
		};
		void Add(int partner_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<PartnerCheckItem> check_vec;
	};

	PartnerCheckStruct m_partner_check;

	// 检测成就完成类型33 宠物修炼等级
	struct CourseOfGrowthStruct
	{
		struct CourseOfGrowthCheckParam1
		{
			int param1;
			std::string info;
		};
		void Add(int param1, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<CourseOfGrowthCheckParam1> check_vec;
	};

	CourseOfGrowthStruct m_growth_check;

	// 技能检测
	struct SkillCheckStruct
	{
		struct SkillCheckParam
		{
			int skill_id;
			std::string info;
		};

		void Add(int skill_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<SkillCheckParam> check_vec;
	};
	SkillCheckStruct m_skill_check;

	// 技能检测
	struct PassiveSkillCheckStruct
	{
		struct SkillCheckParam
		{
			int skill_id;
			std::string info;
		};

		void Add(int skill_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<SkillCheckParam> check_vec;
	};
	PassiveSkillCheckStruct m_passive_skill_check;

	//伙伴-元神契约 检测储存位置
	struct PartnerPactCheckStruct
	{
		struct PactCheckParam
		{
			ItemID pact_item_id;
			std::string info;
		};

		void Add(ItemID pact_item_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<PactCheckParam> check_vec;
	};
	PartnerPactCheckStruct m_partner_pact_check;

	//天下至尊令 奖励组检测
	struct PeakTokenCheckStruct
	{
		struct PeakTokenCheckParam
		{
			ItemID item_id;
			int reward_group_id;
			std::string info;
		};

		void Add(ItemID item_id, int reward_group_id, const char* source_info);
		bool Check(std::string* err);
		void Clear();

		std::vector<PeakTokenCheckParam> check_vec;
	};
	PeakTokenCheckStruct m_peak_token_check;
public:
	ItemValidCheckStruct * GetItemValidCheck() { return &m_item_valid_check; }
	SceneCheckStruct * GetSceneCheck() { return &m_scene_check; }
	NorexItemCheckStruct * GetNorexCheck() { return &m_norex_check; }
	PetValidCheckStruct * GetPetCheck() { return &m_pet_check; }
	SpecialEffectCheckStruct * GetSpecialEffectCheck() { return &m_special_effect_check; }
	MonsterCheckStruct* GetMonsterCheck() { return &m_monster_check; }
	TitleCheckStruct* GetTitleCheck() { return &m_title_check; }
	TaskCheckStruct* GetTaskCheck() { return &m_task_check; }
	PartnerCheckStruct* GetPartnerCheck() { return &m_partner_check; }
	MonsterGroupCheckStruct* GetMonsterGroupCheck() { return &m_monster_group_check; }
	CourseOfGrowthStruct* GetCourseOfGrowthCheck() { return &m_growth_check; } 
	SkillCheckStruct* GetSkillCheckStruct() { return &m_skill_check; }
	PassiveSkillCheckStruct* GetPassiveSkillCheckStruct() { return &m_passive_skill_check; }
	TaskConditionItemStruct* GetTaskConditionItemCheck() { return &m_task_condition_item_check; }
	PartnerPactCheckStruct* GetPartnerPactCheckStruct() { return &m_partner_pact_check; }
	PeakTokenCheckStruct* GetPeakTokenCheckStruct() { return &m_peak_token_check; }

private:
	UTAConfigChecker m_uta_cfg_checker;
};

#endif // __CHECK_RESOURCE_CENTER_HPP__
