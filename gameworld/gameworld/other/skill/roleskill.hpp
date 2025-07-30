#ifndef __ROLE_SKILL_HPP__
#define __ROLE_SKILL_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/skilllistparam.hpp"
#include "other/attributelist.hpp"

struct SkillCfg;
class RoleModuleManager;
class RoleSkill
{
public:
	struct SkillItem
	{
		SkillItem() { this->Reset(); }

		bool Invalid() const { return (UInt16)-1 == skill_id; }
		void Reset() 
		{ 
			index = -1; 
			skill_id = -1; 
			skill_level = 0; 
			skill_show_level = 0; 
			skill_exp = 0; 
		}

		short index;
		UInt16 skill_id;
		short skill_level;
		short skill_show_level;	// RoleSkill创建时检测一次
		int skill_exp;
	};

public:
	RoleSkill();
	~RoleSkill();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const SkillListParam& skill_list, const SkillOtherParam& other_param);
	void OnInitEnd();
	void GetSkillOtherParam(SkillOtherParam* out_other_param);
	void GetSkillList(SkillListParam* p);
	void GetChangeSkillList(SkillListParam* p);
	SkillItem* GetSkill(short skill_index);
	SkillItem* GetSkillBySkillId(UInt16 skill_id);
	SkillItem* GetRandSkill();
	void ClearDirtyMark();
	void OnRoleLogin();

	void OnRoleOperateReq(int oper_type, int param1, int param2, int param3);
	void OnRoleModifySkillPositionReq(int skill_num, short* skill_idx_list);

	int GetMaxSkillLevel();		//获取此时学习的最高技能等级

	void SendSkillList();
	void SendSingleSkillInfo(short skill_index);
	void SendLifeSkillInfo(short life_skill_type = -1, short reason_type = 0);
	void SendCommonUseSkillChangeNotice();

	bool LearnSkill(UInt16 skill_id);
	bool LearnSkillFast(UInt16 skill_id, int target_level);
	bool UplevelSkill(UInt16 skill_id, bool * is_coin_not_enough = NULL,  bool is_uplevel_all = false);
	bool UplevelAllSkill();

	bool SetCommonUseSkill(short skill_idx, bool is_unset);
	void ResetCommonUseSkill();
	
	void OnChangeProfession(int old_prof, int new_prof);
	const LifeSkillInfo * GetLifeSkillInfoByLifeSkillType(int life_skill_type);

	//////////////////////////////// 生活技能触发接口(屏蔽) ///////////////////////////////////////
	void OnDig();			// 挖掘
	void OnLumbering();		// 伐木
	void OnPickFlower();	// 采花
	void OnMakeEquipment(ItemID equip_id, short num); // 制作装备
	static int GetLifeSkillTypeByEquipClass(int equip_class);
	void GiveInitialSkill(int skill_id);
	int GetGatherMultiple(int life_skill_type);
	int CheckIsGather(int item_sub_type, int reason_type);	//返回触发采集技能倍数

	int TheNumOfLevelSkill(int level);

	//把本尊技能信息同步到化身(不包括生活技能)
	void GetSkillPosition();

	void AddLifeSkillExp(int life_skill_type, int add_exp);
	bool AddSkillExp(SkillItem* skill_item, int add_exp, const SkillCfg* skill_cfg = NULL, int* prof_max_skill_level = NULL);
	void SetDirty(short skill_index, bool is_notice_client = true);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	void OnRoleAdvanceCareer();
private:
	bool AddSkill(UInt16 skill_id, int* out_index);
	void SendSkillExpChangeNotify(int skill_id, int exp_change);
	void CheckAnyInitialSkillMissing();
	void CheckOldSkillReturn();
	void CheckSkillUplevelConsumeCoinReturn();
	void CheckSkillLevel();
	void CheckSkillConformProf();
	int GetSkillLevel();
	void GiveInitialSkillOnRoleCreate();
	void RemoveSkillByProfession(int profession);
	void RemoveSkillById(int skill_id);

	RoleModuleManager* m_module_mgr;

	SkillItem m_skill_list[MAX_SKILL_NUM];

	AttributeList m_attrs_add;
	SkillOtherParam m_other_param;
	
	bool m_dirty_mark[MAX_SKILL_NUM];
	bool m_old_state[MAX_SKILL_NUM];
	bool m_is_change;
};

#endif