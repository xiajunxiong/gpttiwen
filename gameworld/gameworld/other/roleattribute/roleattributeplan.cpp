#include "roleattributeplan.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgrole.h"
#include "engineadapter.h"
#include "config/logicconfigmanager.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "equipment/equipmentmanager.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "other/zaohuaequip/zaohuaequipmanager.hpp"

RoleAttributePlan::RoleAttributePlan() : m_module_mgr(NULL)
{

}

RoleAttributePlan::~RoleAttributePlan()
{

}

void RoleAttributePlan::Init(RoleModuleManager* mgr, const AttributePlanParam& param)
{
	m_module_mgr = mgr;
	m_param = param;

	if (0 == m_param.plan_num)
	{
		// 新手属性分配
		m_param.plan_list[0].Reset();
		m_param.plan_num = 1;
		m_param.cur_plan_index = 0;

		const AttrPointCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetBasePointDistributeCfg(m_module_mgr->GetRole()->GetProfession());
		if (NULL != cfg)
		{
			m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_VITALITY]	= cfg->con;
			m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_STRENGTH]	= cfg->str;
			m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_POWER]		= cfg->def;
			m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_AGILITY]		= cfg->agi;
			m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_MAGIC]		= cfg->mag;
			m_param.plan_list[0].remain_point = 0;
		}
		else
		{
			// 这是一个没有找到配置的补救方法
			const RoleLevelCfg* level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_module_mgr->GetRole()->GetLevel());
			if (NULL != level_cfg)
			{
				m_param.plan_list[0].remain_point = level_cfg->total_attr_point;
			}
		}
	}
}

void RoleAttributePlan::GetOtherInitParam(AttributePlanParam *out_param)
{
	if (NULL == out_param) return;

	*out_param = m_param;
}

const AttributeList & RoleAttributePlan::ReCalcAttr(AttributeList &base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		if (m_param.cur_plan_index >= 0 && m_param.cur_plan_index < MAX_ATTR_PLAN_NUM)
		{
			LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(m_param.plan_list[m_param.cur_plan_index], m_attrs_add);

			int potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_MAX];
			memset(potential_add, 0, sizeof(potential_add));
			m_module_mgr->GetZaoHuaEquipManager()->GetPotentialAdd(potential_add);
			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_HP] += m_attrs_add.m_attrs[BATTLE_ATTR_MAX_HP] * potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_0] / 100;
			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_MP] += m_attrs_add.m_attrs[BATTLE_ATTR_MAX_MP] * potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_1] / 100;
			m_attrs_add.m_attrs[BATTLE_ATTR_ATTACK] += m_attrs_add.m_attrs[BATTLE_ATTR_ATTACK] * potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_2] / 100;
			m_attrs_add.m_attrs[BATTLE_ATTR_DEFENSE] += m_attrs_add.m_attrs[BATTLE_ATTR_DEFENSE] * potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_3] / 100;
			m_attrs_add.m_attrs[BATTLE_ATTR_AGILITY] += m_attrs_add.m_attrs[BATTLE_ATTR_AGILITY] * potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_4] / 100;
			m_attrs_add.m_attrs[BATTLE_ATTR_MENTAL] += m_attrs_add.m_attrs[BATTLE_ATTR_MENTAL] * potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_5] / 100;
			m_attrs_add.m_attrs[BATTLE_ATTR_RECOVERY] += m_attrs_add.m_attrs[BATTLE_ATTR_RECOVERY] * potential_add[ZaoHuaSuitCfg::ZAO_HUA_POTENTIAL_TYPE_6] / 100;
		}

	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void RoleAttributePlan::OnRoleLevelUp(int delta_level, int old_level)
{
	if (delta_level <= 0) return;

	for (int idx = 0; idx < m_param.plan_num; ++idx)
	{
		AttributePlan & plan = m_param.plan_list[idx];
		plan.remain_point += delta_level * UPLEVEL_ADD_ATTR_POINT;
	}

	this->CheckAutoAdd();
}

void RoleAttributePlan::CheckAutoAdd(bool is_send)
{
	int all_point = ATTR_BASE_POINT_MAX_NUM + UPLEVEL_ADD_ATTR_POINT * (m_module_mgr->GetRole()->GetLevel() - 1);

	for (int idx = 0; idx < m_param.plan_num; ++idx)
	{
		AttributePlan & plan = m_param.plan_list[idx];

		short(&custom_add_point)[ADD_POINT_TYPE_MAX] = m_param.custom_add_point_list[idx];

		int custom_add_point_sum = 0;
		for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
		{
			if (custom_add_point[k] > 0)
			{
				custom_add_point_sum += custom_add_point[k];
			}
		}

		if (idx == 0 && m_param.tui_jian_auto_add && custom_add_point_sum <= 0)
		{
			const AttrPointCfg * cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetRecommendAddPointCfg(m_module_mgr->GetRole()->GetProfession());
		if (NULL != cfg)
		{
				short cfg_add_point[ADD_POINT_TYPE_MAX] = { cfg->con ,cfg->str,cfg->def, cfg->agi, cfg->mag };

				int loop_count = 0;			//防止下面数据出错循环
				while (plan.remain_point > 0 && loop_count < 150)
			{
					loop_count += 1;
					for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
					{
						int min1 = std::min(all_point - plan.add_point_list[i], (int)cfg_add_point[i]);
						int real_add = std::min(min1, (int)plan.remain_point);

						if (real_add > 0)
						{
							plan.add_point_list[i] += real_add;
							plan.remain_point -= real_add;
						}
					}
				}

				m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
			}
		}

		if (custom_add_point_sum > 0)
		{
			int loop_count = 0;			//防止下面数据出错循环
			while (plan.remain_point > 0 && loop_count < 150)
			{
				loop_count += 1;
				for (int i = 0; i < ADD_POINT_TYPE_MAX; ++i)
				{
					int min1 = std::min(all_point - plan.add_point_list[i], (int)custom_add_point[i]);
					int real_add = std::min(min1, (int)plan.remain_point);

					if (real_add > 0)
					{
						plan.add_point_list[i] += real_add;
						plan.remain_point -= real_add;
	}
				}
			}

			m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
		}
	}

	if (is_send)
	{
	this->SendPlanInfo();
	}	
}

void RoleAttributePlan::SetCustomAddPoint(int plan_index, short(&custom_add_point)[ADD_POINT_TYPE_MAX])
{
	if (plan_index < 0 || plan_index >= m_param.plan_num || plan_index >= MAX_ATTR_PLAN_NUM) return;

	for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
	{
		if (custom_add_point[k] < 0)
		{
			return;
		}
	}

	if (plan_index == 0)
	{
		m_param.tui_jian_auto_add = 0;
	}

	for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
	{
		m_param.custom_add_point_list[plan_index][k] = custom_add_point[k];
	}

	this->CheckAutoAdd(false);

	this->SendPlanInfo();
}

void RoleAttributePlan::CreateAttributePlanReq(const char* plan_name)
{
	if (NULL == plan_name)
	{
		return;
	}

	if (m_param.plan_num >= MAX_ATTR_PLAN_NUM)
	{
		return;
	}

	const RoleExpOtherCfg& other_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetOtherCfg();
	if(NULL == &other_cfg) return;

	if (!m_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.wash_point_item, other_cfg.wash_point_num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_ITEM);
		return;
	}

	int add_to_index = m_param.plan_num;
	m_param.plan_num += 1;
	m_param.plan_list[add_to_index].Reset();
	F_STRNCPY(m_param.plan_list[add_to_index].plan_name, plan_name, sizeof(m_param.plan_list[add_to_index].plan_name));

	const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_module_mgr->GetRole()->GetLevel());
	if (NULL != cfg)
	{
		m_param.plan_list[add_to_index].remain_point = cfg->total_attr_point;
	}
	else
	{
		m_param.plan_list[add_to_index].remain_point = m_module_mgr->GetRole()->GetLevel() * 4 + 30; // 初始30，每级加4
	}
	m_param.cur_plan_index = add_to_index; // 自动切换至新的方案

	this->SendPlanInfo();

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
}

void RoleAttributePlan::ChangeAttributePlanNameReq(short plan_index, const char* plan_name)
{
	if (NULL == plan_name) return;

	if (plan_index < 0 || plan_index >= m_param.plan_num) return;

	F_STRNCPY(m_param.plan_list[plan_index].plan_name, plan_name, ATTRIBUTE_PLAN_NAME_LENGTH);

	this->SendPlanInfo();
}

void RoleAttributePlan::SwitchAttributePlanReq(short switch_to_index)
{
	if (switch_to_index < 0 || switch_to_index >= m_param.plan_num || switch_to_index == m_param.cur_plan_index)
	{
		return;
	}

	m_param.cur_plan_index = switch_to_index;

	this->SendPlanInfo();

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);

	gamelog::g_log_role_data.printf(LL_INFO, "SwitchAttributePlanReq role[%d,%s] attr_list[%d,%d,%d,%d,%d]", m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(),
		m_param.plan_list[switch_to_index].add_point_list[ADD_POINT_TYPE_VITALITY], m_param.plan_list[switch_to_index].add_point_list[ADD_POINT_TYPE_STRENGTH],
		m_param.plan_list[switch_to_index].add_point_list[ADD_POINT_TYPE_POWER], m_param.plan_list[switch_to_index].add_point_list[ADD_POINT_TYPE_AGILITY],
		m_param.plan_list[switch_to_index].add_point_list[ADD_POINT_TYPE_MAGIC]);
}

void RoleAttributePlan::AddAttributePointReq(short plan_index, short vitality, short strength, short power, short speed, short magic)
{
	if (plan_index < 0 || plan_index >= m_param.plan_num)
	{
		return;
	}

	if (vitality < 0 || strength < 0 || power < 0 || speed < 0 || magic < 0)
	{
		gamelog::g_log_role_data.printf(LL_WARNING, "AddAttributePointReq role[%d,%s] attr below 0, vit[%d] str[%d] pow[%d] spd[%d] mag[%d]", 
			m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(), vitality, strength, power, speed, magic);
		return;
	}

	int total_change_point = vitality + strength + power + speed + magic;
	if (total_change_point > m_param.plan_list[plan_index].remain_point)
	{
		return; // 点数不够
	}

	AttributePlan& plan = m_param.plan_list[plan_index];
	short* point_list = m_param.plan_list[plan_index].add_point_list;

	int total_vit = point_list[ADD_POINT_TYPE_VITALITY] + vitality;
	int total_str = point_list[ADD_POINT_TYPE_STRENGTH] + strength;
	int total_pow = point_list[ADD_POINT_TYPE_POWER] + power;
	int total_agi = point_list[ADD_POINT_TYPE_AGILITY] + speed;
	int total_mag = point_list[ADD_POINT_TYPE_MAGIC] + magic;
	
	int total_point = plan.remain_point +
		point_list[ADD_POINT_TYPE_VITALITY] +
		point_list[ADD_POINT_TYPE_STRENGTH] +
		point_list[ADD_POINT_TYPE_POWER] +
		point_list[ADD_POINT_TYPE_AGILITY] +
		point_list[ADD_POINT_TYPE_MAGIC];

	plan.add_point_list[ADD_POINT_TYPE_VITALITY]	+= vitality;
	plan.add_point_list[ADD_POINT_TYPE_STRENGTH]	+= strength;
	plan.add_point_list[ADD_POINT_TYPE_POWER]		+= power;
	plan.add_point_list[ADD_POINT_TYPE_AGILITY]		+= speed;
	plan.add_point_list[ADD_POINT_TYPE_MAGIC]		+= magic;

	m_param.plan_list[plan_index].remain_point -= total_change_point;

	if (plan_index == m_param.cur_plan_index)
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
	}

	gamelog::g_log_role_data.printf(LL_INFO, "AddAttributePointReq role[%d,%s] vitality[%d] strength[%d] power[%d] speed[%d] magic[%d] point_vit[%d] point_str[%d] point_pow[%d] point_spd[%d] point_mag[%d] remain_point[%d]",
		m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(),
		vitality, strength, power, speed, magic, plan.add_point_list[ADD_POINT_TYPE_VITALITY], plan.add_point_list[ADD_POINT_TYPE_STRENGTH], plan.add_point_list[ADD_POINT_TYPE_POWER],
		plan.add_point_list[ADD_POINT_TYPE_AGILITY], plan.add_point_list[ADD_POINT_TYPE_MAGIC], m_param.plan_list[plan_index].remain_point);
	
	this->SendPlanInfo();
}

void RoleAttributePlan::ClearAttributePointReq(short plan_index, bool is_change_prof)
{
	if (plan_index < 0 || plan_index >= m_param.plan_num)
	{
		return;
	}
	if (!is_change_prof)
	{
		if (m_param.reset_plan_times > 0 && !m_module_mgr->GetKnapsack()->ConsumeItem(LOGIC_CONFIG->GetRoleExpConfig()->GetOtherCfg().wash_point_item, LOGIC_CONFIG->GetRoleExpConfig()->GetOtherCfg().wash_point_num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, true))
		{
			return;
		}
	}

	int total_change_point =
		m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_VITALITY] +
		m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_STRENGTH] +
		m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_POWER] +
		m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_AGILITY] +
		m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_MAGIC];

	m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_VITALITY] = 0;
	m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_STRENGTH] = 0;
	m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_POWER] = 0;
	m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_AGILITY] = 0;
	m_param.plan_list[plan_index].add_point_list[ADD_POINT_TYPE_MAGIC] = 0;

	m_param.plan_list[plan_index].remain_point += total_change_point;

	const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_module_mgr->GetRole()->GetLevel());
	if (NULL != cfg)
	{
		m_param.plan_list[plan_index].remain_point = cfg->total_attr_point;
	}
	else
	{
		m_param.plan_list[plan_index].remain_point = m_module_mgr->GetRole()->GetLevel() * 4 + 30; // 初始30，每级加4
	}

	if (!is_change_prof)
	{
		m_param.reset_plan_times += 1;
		this->SendPlanInfo();
	}

	if (plan_index == m_param.cur_plan_index)
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
	}
}

void RoleAttributePlan::OnRoleChangeProf()
{
	for (int i = 0; i < m_param.plan_num && i < MAX_ATTR_PLAN_NUM; i++)
	{
		this->ClearAttributePointReq(i, true);
	}
	this->SendPlanInfo();
}

void RoleAttributePlan::OnRoleFirstTransfer()
{
	int total_change_point =
		m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_VITALITY] +
		m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_STRENGTH] +
		m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_POWER] +
		m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_AGILITY] +
		m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_MAGIC];

	m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_VITALITY] = 0;
	m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_STRENGTH] = 0;
	m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_POWER] = 0;
	m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_AGILITY] = 0;
	m_param.plan_list[0].add_point_list[ADD_POINT_TYPE_MAGIC] = 0;

	m_param.plan_list[0].remain_point += total_change_point;
	const AttrPointCfg* cfg_base = LOGIC_CONFIG->GetRoleExpConfig()->GetBasePointDistributeCfg(m_module_mgr->GetRole()->GetProfession());
	if(NULL != cfg_base)
	{ 
		const AttrPointCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetRecommendAddPointCfg(m_module_mgr->GetRole()->GetProfession());
		if (NULL != cfg)
		{
			AttributePlan& plan = m_param.plan_list[0];
			int born_point = (cfg_base->con + cfg_base->str + cfg_base->def + cfg_base->mag + cfg_base->agi);
			int remain_point = plan.remain_point - born_point;
			int level = remain_point / UPLEVEL_ADD_ATTR_POINT;
			if (remain_point >= level * UPLEVEL_ADD_ATTR_POINT)
			{
				plan.add_point_list[ADD_POINT_TYPE_VITALITY] = level * cfg->con + cfg_base->con;
				plan.add_point_list[ADD_POINT_TYPE_STRENGTH] = level * cfg->str + cfg_base->str;
				plan.add_point_list[ADD_POINT_TYPE_POWER] = level * cfg->def + cfg_base->def;
				plan.add_point_list[ADD_POINT_TYPE_MAGIC] = level * cfg->mag + cfg_base->mag;
				plan.add_point_list[ADD_POINT_TYPE_AGILITY] = level * cfg->agi + cfg_base->agi;
				plan.remain_point -= (level * UPLEVEL_ADD_ATTR_POINT + born_point);

				m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTRIBUTE_PLAN, __FUNCTION__);
			}
		}
	}
	this->SendPlanInfo();
}

void RoleAttributePlan::SendPlanInfo()
{
	static Protocol::SCRoleAttributePlanInfo info;
	info.plan_num = m_param.plan_num;
	info.cur_plan_index = m_param.cur_plan_index;
	for (int i = 0; i < MAX_ATTR_PLAN_NUM; ++i)
	{
		F_STRNCPY(info.plan_list[i].plan_name, m_param.plan_list[i].plan_name, sizeof(info.plan_list[i].plan_name));
		info.plan_list[i].remain_point = m_param.plan_list[i].remain_point;
		for (int k = 0; k < ADD_POINT_TYPE_MAX; k++)
		{
			info.plan_list[i].add_point_list[k] = m_param.plan_list[i].add_point_list[k];
			info.plan_list[i].custom_add_point_list[k] = m_param.custom_add_point_list[i][k];
		}
	}

	info.reset_plan_times = m_param.reset_plan_times;
	info.tui_jian_auto_add = m_param.tui_jian_auto_add;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void RoleAttributePlan::GetAttributePlan(AttributePlan & plan)const
{
	plan = m_param.plan_list[m_param.cur_plan_index];
}

bool RoleAttributePlan::GetAttributeList(AttributeList* out_attrs_list)
{
	if (NULL == out_attrs_list) return false;

	*out_attrs_list = m_attrs_add;

	return true;
}


