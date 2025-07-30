#include "capability.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "equipment/equipmentmanager.h"

#include "protocol/msgrole.h"

#include "other/medal/medal.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/roleheartskill/roleheartskill.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "other/train/train.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "config/otherconfig/professionconfig.hpp"

Capability::Capability():m_module_mgr(NULL), m_capability(0), m_max_capability(0), m_extra_capability(0), m_huashen_capability(0), m_huashen_max_capability(0)
{
}

Capability::~Capability()
{
}

void Capability::Init(RoleModuleManager * module_mgr, int capability, int max_capability, int huashen_capability, int huashen_max_capability)
{
	m_module_mgr = module_mgr;

	m_capability = capability;
	m_max_capability = max_capability;
	m_huashen_capability = huashen_capability;
	m_huashen_max_capability = huashen_max_capability;
	gamelog::g_log_capability.printf(LL_INFO, "Capability::Init role[%d,%s] capability[%d] max_capability[%d] huashen_capability[%d] huashen_max_capability[%d]",
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_capability, m_max_capability, m_huashen_capability, m_huashen_max_capability);
}

void Capability::GetCapabilityInitParam(int * capability, int * max_capability, int *huashen_capability, int * huashen_max_capability)
{
	*capability = m_capability;
	*max_capability = m_max_capability;
	*huashen_capability = m_huashen_capability;
	*huashen_max_capability = m_huashen_max_capability;
}

void Capability::GetAttrListParam(AttributeList & attr_List)
{
	m_attr_list = attr_List;
	this->CalcRoleCapability();
}

void Capability::CalcRoleCapability()
{
	int old_capability = m_capability;
	m_capability = this->CalcCapablity();
	m_extra_capability = this->CalcExtraCapability();
	m_capability += m_extra_capability;
	m_module_mgr->GetRole()->UpdateCacheNode();
	if (m_capability > m_max_capability)
	{
		m_max_capability = m_capability;
		this->OnSendMaxChangeInfo();
	}
	
	EventHandler::Instance().OnRoleCapabilityChange(m_module_mgr->GetRole(), old_capability, m_capability);
}

void Capability::GetHuaShenAttrListParam(AttributeList & attr_List)
{
	m_huashen_attr_list = attr_List;
	this->CalcHuaShenCapability();
}

void Capability::CalcHuaShenCapability()
{
	m_huashen_capability = this->CalcCapablity(ROLE_TYPE_HUASHEN);
	m_huashen_capability += m_module_mgr->GetHuaShenManager()->GetMedalSpeScore();
	m_huashen_capability += m_module_mgr->GetSmartMountsSystem()->GetSkillOrSpCap();
	m_huashen_capability += m_module_mgr->GetTrain()->GetTrainSkillCapability();
	m_huashen_capability += m_module_mgr->GetRoleGuildInborn()->GetSkillScroe(GUILD_INBORN_ADD_TARGET_TYPE_ROLE);
	m_huashen_capability += m_module_mgr->GetRoleHeartSkill()->GetEquipSkillScore();
	if (m_huashen_capability > m_huashen_max_capability)
	{
		m_huashen_max_capability = m_huashen_capability;
	}
}

AttributeList & Capability::GetAttributeList(int role_type)
{
	if (role_type == ROLE_TYPE_BENZUN)
	{
		return m_attr_list;
	}
	return m_huashen_attr_list;
}

int Capability::CalcCapabityByAttrList(AttributeList & attr_list)
{
	int app_prof = m_module_mgr->GetRole()->GetAppearanceProf();
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if (NULL == prof_cap_cfg) return 0;

	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(attr_list.m_attrs); i++)
	{
		if (prof_cap_cfg->cap_base[i] == 0) continue;

		if (LOGIC_CONFIG->GetScoreSystemConfig()->IsSpecialAttrType(i))
		{
			special_capability += attr_list.m_attrs[i] * prof_cap_cfg->cap_base[i];
		}
		else
		{
			capability += attr_list.m_attrs[i] * prof_cap_cfg->cap_base[i];
		}
	}

	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + special_capability;
}

int Capability::GetCalcCapByAttrList(AttributeList & attr_list)
{
	int app_prof = m_module_mgr->GetRole()->GetAppearanceProf();
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if (NULL == prof_cap_cfg) return 0;

	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(attr_list.m_attrs); i++)
	{
		if (prof_cap_cfg->cap_base[i] == 0) continue;

		if (LOGIC_CONFIG->GetScoreSystemConfig()->IsSpecialAttrType(i))
		{
			special_capability += attr_list.m_attrs[i] * prof_cap_cfg->cap_base[i];
		}
		else
		{
			capability += attr_list.m_attrs[i] * prof_cap_cfg->cap_base[i];
		}
	}
	
	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + special_capability;
}

void Capability::OnSendMaxChangeInfo()
{
	Protocol::SCRoleHistoryMaxCapabilityChange info;
	info.max_role_capability = m_max_capability;
	
	m_module_mgr->NetSend(&info, sizeof(info));
}

int Capability::CalcCapablity(int role_type)
{
	int app_prof = m_module_mgr->GetRole()->GetAppearanceProf();
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if(NULL == prof_cap_cfg) return 0;

	AttributeList &  attr_list = this->GetAttributeList(role_type);
	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(attr_list.m_attrs) ; i++)
	{
		if(prof_cap_cfg->cap_base[i] == 0) continue;

		if (LOGIC_CONFIG->GetScoreSystemConfig()->IsSpecialAttrType(i))
		{
			special_capability += attr_list.m_attrs[i] * prof_cap_cfg->cap_base[i];
		}
		else
		{
			capability += attr_list.m_attrs[i] * prof_cap_cfg->cap_base[i];
		}
	}
	int skill_capability = 0;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		RoleSkill::SkillItem* skill_item = NULL;
		if (ROLE_TYPE_BENZUN == role_type)
		{
			skill_item = m_module_mgr->GetRoleSkill()->GetSkill(i);
		}
		else
		{
			skill_item = m_module_mgr->GetHuaShenManager()->GetSkill(i);
		}
		if (NULL == skill_item || skill_item->Invalid()) continue;

		//检测是否是该职业技能
		const ProfSkillCfg* prof_skill_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetProfSkillCfgByHashKey(ProfessionConfig::GetHashKey(app_prof, skill_item->skill_id));
		if (NULL == prof_skill_cfg) continue;

		int add_skill_score = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfSkillScore(ProfSkillScoreKey(app_prof, skill_item->skill_level));
		if (add_skill_score > 0)
		{
			skill_capability += add_skill_score;
		}
	}

	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + skill_capability + special_capability;
}

int Capability::CalcExtraCapability()
{
	int extra_capability = 0;
	extra_capability += m_module_mgr->GetMedal()->GetSpeScore();
	extra_capability += m_module_mgr->GetSmartMountsSystem()->GetSkillOrSpCap();
	extra_capability += m_module_mgr->GetTrain()->GetTrainSkillCapability();
	extra_capability += m_module_mgr->GetRoleGuildInborn()->GetSkillScroe(GUILD_INBORN_ADD_TARGET_TYPE_ROLE);
	extra_capability += m_module_mgr->GetRoleHeartSkill()->GetEquipSkillScore();
	extra_capability += m_module_mgr->GetEquipmentManager()->GetEquipDianHuaShiCap();
	return extra_capability;
}
