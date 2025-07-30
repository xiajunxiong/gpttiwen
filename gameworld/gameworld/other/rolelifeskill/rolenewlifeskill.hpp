#ifndef __ROLE_NEW_LIFE_SKILL_HPP__
#define __ROLE_NEW_LIFE_SKILL_HPP__

#include "servercommon/rolenewlifeskilldef.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class RoleNewLifeSkill
{
public:
	RoleNewLifeSkill();
	~RoleNewLifeSkill();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const RoleNewLifeSkillParam & param);
	void GetInitParam(RoleNewLifeSkillParam* param);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	void MinusOriginFoodAttr(BattleCharacterData* out_data);
	static void AddSystemFoorAttr(BattleCharacterData* out_data);

	void Update(time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();
	void OnRoleLogout();

	void OnSkillLevelUp(int new_life_skill_type);
	void OnMakeItem(int sn, bool is_use_aux_tool, int count);
	bool OnUseProps(int seq);
	bool OnUseChangeCard(int seq, int use_num, bool is_change, const char * reason);
	void OnSetUseChangeCard(bool is_use_change_card);
	void OnStartSurface(int seq);							//幻化
	void OnRelieveSurface(int relieve_surface_type);		//解除幻化

	void OnRoleStartFight();		
	void OnRoleFinishFight();
	void OnRoleBattlePerformSkill(int change_card_seq);

	int GetNewLifeSkillByType(int new_life_skill_type);
	int GetNewLifeSkillPropsChangeCard() { return m_param.life_skill_props_list[NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD].seq; }
	char GetUseChangeCard() { return m_param.is_use_change_card; }
	int GetMultipleByItemSubType(int item_sub_type, int * new_life_skill_type = NULL, int * new_life_skill_level = NULL);
	void GetChangeCardSkill(int & skill_id, int & seq);
	int GetFightChangeCardSeq() const { return m_param.fight_change_card_seq; }
	bool IsCanPerformSkillByBattle();

	void SendAllInfo();
	void SendSingleInfo(short new_life_skill_type);

	void SendPropsAllInfo();
	void SendPropsSingleInfo(int new_life_skill_props_type, int param1 = 0, bool is_notice = true);
	void SendChangeCardSingleInfo(int seq, int surface_point);

private:
	void OnRecoveryChangeCard();	//回收已存在的幻化点

private:

	bool IsInvaild(short new_life_skill_type) { return new_life_skill_type < NEW_LIFE_SKILL_TYPE_MIN || new_life_skill_type >= NEW_LIFE_SKILL_TYPE_MAX; }
	bool IsPropsInvaild(int new_life_skill_props_type) { return new_life_skill_props_type < NEW_LIFE_SKILL_PROPS_TYPE_BEGIN || new_life_skill_props_type >= NEW_LIFE_SKILL_PROPS_TYPE_MAX; }
	int GetMultipleByType(int new_life_skill_type);
	bool IsNeedNotice(int relieve_surface_type);


	RoleModuleManager * m_module_mgr;
	RoleNewLifeSkillParam m_param;

	AttributeList m_attrs_add;
};

#endif