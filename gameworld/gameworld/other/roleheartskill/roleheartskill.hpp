#ifndef __ROLE_HEART_SKILL_HPP__
#define __ROLE_HEART_SKILL_HPP__

#include "servercommon/roleheartskilldef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class RoleHeartSkill
{
public:
	RoleHeartSkill();
	~RoleHeartSkill();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const HeartSkillParam & param);
	void GetInitParam(HeartSkillParam * param);

	void OnRoleLogin();
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);	

	void CheckHeartSkillCollection();

	int GetEquipSkillScore();

	void OnLearnHeartSkill(int heart_skill_seq, int heart_skill_learn_type);
	void OnEquipHeartSkill(int skill_status, int skill_index);
	void OnTakeOffHeartSkill(int skill_status);
	void OnCompose(int compose_target);
	void OnConvert(int column, int index, int target_item_id);
	void OnUnLock(int skill_status);
	void OnDisplace(std::vector<HeartSkillDisplaceInfo> displace_vec);

	void SendAllInfo();
	void SendSingleInfo(int skill_type, int index);
	void SendOtherInfo();
	void SendCapInfo();

	//收集战斗数据
	void GetEquipInitativeSkill(int max_heart_skill_num, char* out_heart_skill_num, BattleHeartSkill * battle_heart_skill_list);
	void GetEquipPassiveSkill(int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);

private:

	bool IsValid(int skill_type) { return skill_type > HEART_SKILL_SKILL_TYPE_BEGIN && skill_type < HEART_SKILL_SKILL_TYPE_MAX; }
	bool IsValid(int skill_type, int skill_index);
	bool IsSkillStatusValid(int skill_status) { return skill_status >= HEART_SKILL_INITIATIVE_INDEX && skill_status < HEART_SKILL_STATUS_MAX; }
	bool CheckIsEquip(int skill_type, int skill_index);
	bool IsPassiveSkill(int skill_status) { return skill_status >= HEART_SKILL_PASSIVE_INDEX && skill_status < HEART_SKILL_PASSIVE_END;}
	HeartSkillInfo * GetSkillInfo(int skill_type, int skill_index, int * count = NULL);
	HeartSkillInfo * GetSkillInfoBySeq(int skill_type, short heart_skill_seq, int * index = NULL);
	void SendDisplaceNotice(int need_skill_num);
	
	RoleModuleManager * m_module_mgr;
	HeartSkillParam m_param;
	AttributeList m_attrs_add;
};



#endif