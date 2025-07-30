#include "rolenewlifeskill.hpp"
#include "gamelog.h"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "config/otherconfig/newlifeskillconfig.hpp"

#include "item/money.h"
#include "obj/character/role.h"

#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/route/mailroute.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

#include "global/guild/guild.hpp"
#include "global/cross/crossusershadow.hpp"
#include "global/cross/crossuserregister.hpp"
#include "battle/battle_manager_local.hpp"

#include "protocol/msglifeskill.hpp"

RoleNewLifeSkill::RoleNewLifeSkill() : m_module_mgr(NULL)
{
}

RoleNewLifeSkill::~RoleNewLifeSkill()
{
}

void RoleNewLifeSkill::Init(RoleModuleManager * module_mgr, const RoleNewLifeSkillParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
	if (3 != m_param.is_reset_use_change_card_flag)
	{
		m_param.is_use_change_card = 1;
		m_param.is_reset_use_change_card_flag = 3;
	}
}

void RoleNewLifeSkill::GetInitParam(RoleNewLifeSkillParam * param)
{
	*param = m_param;
}

const AttributeList & RoleNewLifeSkill::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		for (int i = 0; i < NEW_LIFE_SKILL_PROPS_TYPE_MAX && i < ARRAY_LENGTH(m_param.life_skill_props_list); i++)
		{
			if (0 == m_param.life_skill_props_list[i].seq) continue;	//没有使用
			if (NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD != i && 0 >= m_param.life_skill_props_list[i].end_timestamp) continue;

			const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(m_param.life_skill_props_list[i].seq);
			if(NULL == props_cfg) continue;

			for (int k = 0; k < props_cfg->attr_count && k < ARRAY_LENGTH(props_cfg->attr_list); ++k)
			{
				if(props_cfg->attr_list[k].attr_type < 0 || props_cfg->attr_list[k].attr_type >= BATTLE_ATTR_MAX) continue;

				m_attrs_add.m_attrs[props_cfg->attr_list[k].attr_type] += props_cfg->attr_list[k].attr_value;
			}
		}
		//会出现五项基础属性
		AttributePlan tmp_ap;
		for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
		{
			tmp_ap.add_point_list[i] += m_attrs_add.m_attrs[k];
		}
		LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, m_attrs_add);
	}
	
	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void RoleNewLifeSkill::MinusOriginFoodAttr(BattleCharacterData* out_data)
{
	for (int i = 0; i < ARRAY_LENGTH(out_data->attr_list) && i < ARRAY_LENGTH(m_attrs_add.m_attrs); i++)
	{
		out_data->attr_list[i] -= m_attrs_add.m_attrs[i];
	}

	/*for (int i = 0; i < NEW_LIFE_SKILL_PROPS_TYPE_MAX && i < ARRAY_LENGTH(m_param.life_skill_props_list); i++)
	{
		if (0 == m_param.life_skill_props_list[i].seq) continue;	//没有使用
		if (NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD != i && 0 >= m_param.life_skill_props_list[i].end_timestamp) continue;

		const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(m_param.life_skill_props_list[i].seq);
		if (NULL == props_cfg) continue;

		for (int k = 0; k < props_cfg->attr_count && k < ARRAY_LENGTH(props_cfg->attr_list); ++k)
		{
			if (props_cfg->attr_list[k].attr_type < 0 || props_cfg->attr_list[k].attr_type >= BATTLE_ATTR_MAX) continue;

			out_data->attr_list[props_cfg->attr_list[k].attr_type] -= props_cfg->attr_list[k].attr_value;
		}
	}*/
}

void RoleNewLifeSkill::AddSystemFoorAttr(BattleCharacterData* out_data)
{
	AttributeList attrs_add;
	const std::vector<int>& system_food_list = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetSystemFoodList();
	for (size_t i = 0; i < system_food_list.size(); ++i)
	{
		const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(system_food_list[i]);
		if (NULL == props_cfg) continue;

		for (int k = 0; k < props_cfg->attr_count && k < ARRAY_LENGTH(props_cfg->attr_list); ++k)
		{
			if (props_cfg->attr_list[k].attr_type < 0 || props_cfg->attr_list[k].attr_type >= BATTLE_ATTR_MAX) continue;

			attrs_add.m_attrs[props_cfg->attr_list[k].attr_type] += props_cfg->attr_list[k].attr_value;
		}
	}
	
	AttributePlan tmp_ap;
	for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
	{
		tmp_ap.add_point_list[i] += attrs_add.m_attrs[k];
	}
	LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, attrs_add);
	for (int i = 0; i < ARRAY_LENGTH(out_data->attr_list) && i < ARRAY_LENGTH(attrs_add.m_attrs); i++)
	{
		out_data->attr_list[i] += attrs_add.m_attrs[i];
	}
}

void RoleNewLifeSkill::Update(time_t now_second)
{
	for (int i = 0; i < NEW_LIFE_SKILL_PROPS_TYPE_MAX && i < ARRAY_LENGTH(m_param.life_skill_props_list); i++)
	{
		if(0 == m_param.life_skill_props_list[i].seq || 0 >= m_param.life_skill_props_list[i].end_timestamp) continue;	//没有使用
		if(m_param.life_skill_props_list[i].end_timestamp >= (unsigned int)now_second) continue;		//使用了,但还没到期
		
		m_param.life_skill_props_list[i].EndReset();
		this->SendPropsSingleInfo(i);
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_LIFE_SKILL_PROPS, __FUNCTION__);
		EventHandler::Instance().OnRoleLifeSkillPropsChange(m_module_mgr->GetRole(), i, false);
	}
}

void RoleNewLifeSkill::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	for (int i = 0; i < ARRAY_LENGTH(m_param.life_skill_list); i++)
	{
		if(!LOGIC_CONFIG->GetNewLifeSkillConfig()->IsNewLifeProf(i)) continue;

		m_param.life_skill_list[i].trigger_double_count = 0;
		this->SendSingleInfo(i);
	}
}

void RoleNewLifeSkill::OnRoleLogin()
{
	//上线自动解除幻化(战斗中则不解除)
	if (!BattleManagerLocal::GetInstance().IsRoleInBattle(m_module_mgr->GetUid()))
	{
		if ((unsigned int)EngineAdapter::Instance().Time() >= m_param.can_cancel_change_card_timestamp)
		{
			this->OnRelieveSurface(NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_LOGIN);
			m_param.fight_change_card_seq = 0;
		}
		/*if (CrossConfig::Instance().IsHiddenServer())
		{
			if (CrossUserRegister::Instance().IsHasCrossUser(m_module_mgr->GetUid()))
			{
				return;
			}
		}
		else
		{
			//返回本服,login前就已经被移除了,无法使用
			if (CrossUserShadow::Instance().IsUserInCross(m_module_mgr->GetUid()))
			{
				return;
			}
		}*/
	}
	//回收幻化点
	this->OnRecoveryChangeCard();
}

void RoleNewLifeSkill::OnRoleLogout()
{
	m_param.can_cancel_change_card_timestamp = (unsigned int)EngineAdapter::Instance().Time() + MAX_NEW_LIFE_SKILL_CHANGE_CARD_LOGOUT_TIME;
}

void RoleNewLifeSkill::OnSkillLevelUp(int new_life_skill_type)
{
	if(this->IsInvaild(new_life_skill_type)) return;
	
	//在家族中才可以升级
	Guild * guild = m_module_mgr->GetRole()->GetGuild();
	if (NULL == guild)
	{
		m_module_mgr->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return;
	}

	{	//判断家族书房等级
		const GuildBaseConstructCfg * g_construct_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_STUDY, guild->GetBuildLevel(GUILD_BUILD_TYPE_STUDY));
		if (NULL == g_construct_cfg) return;

		if (m_param.life_skill_list[new_life_skill_type].skill_level >= g_construct_cfg->paramet_1)
		{
			m_module_mgr->NoticeNum(errornum::EN_GUILD_LACK_STUDY_LEVEL);
			return;
		}
	}

	const NewLifeSkillLevelCfg * old_life_skill_Level_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetNewLifeSkillLevelCfg(m_param.life_skill_list[new_life_skill_type].skill_level);
	const NewLifeSkillLevelCfg * new_life_skill_Level_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetNewLifeSkillLevelCfg(m_param.life_skill_list[new_life_skill_type].skill_level + 1);
	if (NULL == old_life_skill_Level_cfg || NULL == new_life_skill_Level_cfg)
	{
		return;
	}
	UNSTD_STATIC_CHECK(NEW_LIFE_SKILL_TYPE_MAX == 6);
	int consume_money = 0;
	switch (new_life_skill_type)
	{
		case NEW_LIFE_SKILL_TYPE_PHARMACIST:
		{
			consume_money = old_life_skill_Level_cfg->up_expend_medicine;
		}
		break;
		case NEW_LIFE_SKILL_TYPE_COOK:
		{
			consume_money = old_life_skill_Level_cfg->up_expend_cook;
		}
		break;
		default:
		consume_money = old_life_skill_Level_cfg->up_expend;
		break;
	}
	if(consume_money <= 0) return;

	if (!m_module_mgr->GetMoney()->UseOtherCoin(consume_money, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_GUILD_GONGXIAN_NOT_ENOUGH);
		return;
	}

	m_param.life_skill_list[new_life_skill_type].skill_level += 1;
	this->SendSingleInfo(new_life_skill_type);
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_life_skill_level_up_notice, consume_money,
		new_life_skill_type, m_param.life_skill_list[new_life_skill_type].skill_level);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
	gamelog::g_log_life_skill.printf(LL_INFO, "%s life_type[%d] skill_level[%d]", __FUNCTION__, new_life_skill_type, m_param.life_skill_list[new_life_skill_type].skill_level);
}

void RoleNewLifeSkill::OnMakeItem(int sn, bool is_use_aux_tool, int count)
{
	if (count <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	const NewLifeSkillRecipeCfg * recipe_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetRecipeCfg(sn);
	if (NULL == recipe_cfg || !LOGIC_CONFIG->GetNewLifeSkillConfig()->IsNewLifeProf(recipe_cfg->new_life_skill_type_limit) || 
		recipe_cfg->role_prof_set.end() == recipe_cfg->role_prof_set.find(m_module_mgr->GetRole()->GetAppearanceProf()))
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	NewLifeSkillInfo & skill_info = m_param.life_skill_list[recipe_cfg->new_life_skill_type_limit];
	if (skill_info.skill_level < recipe_cfg->new_life_skill_level_limit)
	{
		m_module_mgr->NoticeNum(errornum::EN_NEED_SKILL_LEVEL_NOT_ENOUGH);
		return;
	}
	int need_expend_active = recipe_cfg->expend_active * count;
	if (m_module_mgr->GetRoleGatherFb()->GetGatherPower() < need_expend_active)
	{
		m_module_mgr->NoticeNum(errornum::EN_GATHER_POWER_NOT_ENOUGH);
		return;
	}
	const NewLifeSkillLevelCfg * skill_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetNewLifeSkillLevelCfg(skill_info.skill_level);
	if (NULL == skill_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	const NewLifeSkillOtherCfg & other_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetOtherCfg();
	int real_get_item_count = count;
	int trigger_double_count = skill_info.trigger_double_count;

	/* 生活职业暂时屏蔽双倍
	if (is_use_aux_tool)
	{
		for (int i = 0; i < count; ++i)
		{
			if (trigger_double_count >= other_cfg.occupation_double_times) break;

			int rate_num = rand() % NewLifeSkillLevelCfg::MAX_DOUBLE_RATE_COUNT_NUM;
			if (rate_num < skill_cfg->prof_double_rate)
			{
				real_get_item_count += 1;
				trigger_double_count++;
			}
		}
	}*/

	if (!m_module_mgr->GetKnapsack()->CheckForPut(recipe_cfg->compose_item_id, real_get_item_count, false))
	{
		m_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	std::map<int, int> need_item_map;
	for (int i = 0; i < recipe_cfg->need_item_count && i < ARRAY_LENGTH(recipe_cfg->need_item_list); i++)
	{
		need_item_map[recipe_cfg->need_item_list[i].item_id] += recipe_cfg->need_item_list[i].num;
	}
	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (std::map<int, int>::const_iterator it = need_item_map.begin(); it != need_item_map.end() && stuff_count < ARRAY_LENGTH(stuff_list); ++it)
	{
		stuff_list[stuff_count].item_id = it->first;
		stuff_list[stuff_count].num = it->second * count;
		stuff_list[stuff_count].buyable = false;
		stuff_count++;
	}
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0)
		{
			if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}
		}
	}
	ItemConfigData item(recipe_cfg->compose_item_id, true, real_get_item_count);
	m_module_mgr->GetKnapsack()->Put(item, PUT_REASON_NEW_LIFE_MAKE, NULL, NULL, 0, 0, false);
	m_module_mgr->GetRoleGatherFb()->AddGatherPower(-need_expend_active);
	m_module_mgr->GetKnapsack()->NotifyGetItemList(1, &item, GET_REASON_LIFE_SKILL_MAKE, recipe_cfg->new_life_skill_type_limit);
	EventHandler::Instance().OnCompoundItem(m_module_mgr->GetRole(), item.item_id, item.num);		// 成就
	skill_info.trigger_double_count = trigger_double_count;
	if (real_get_item_count != count)
	{
		this->SendSingleInfo(recipe_cfg->new_life_skill_type_limit);
	}
	std::string consume_string;
	for (int i = 0; i < stuff_count && i < ARRAY_LENGTH(stuff_list); ++i)
	{
		consume_string += STRING_SPRINTF("[item_id:%d num:%d]", stuff_list[i].item_id, stuff_list[i].num);
	}
	gamelog::g_log_life_skill.printf(LL_INFO, "%s user[%d, %s] target[%d, %d, %d] need_expend_active[%d] consume_list[%s]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		recipe_cfg->compose_item_id, count, real_get_item_count, need_expend_active, consume_string.c_str());
}

bool RoleNewLifeSkill::OnUseProps(int seq)
{
	const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(seq);
	if (NULL == props_cfg || NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD == props_cfg->type)
	{
		return false;
	}
	const NewLifeSkillOtherCfg & other_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetOtherCfg();
	m_param.life_skill_props_list[props_cfg->type].seq = seq;
	m_param.life_skill_props_list[props_cfg->type].end_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + other_cfg.food_time);

	this->SendPropsSingleInfo(props_cfg->type);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_LIFE_SKILL_PROPS, __FUNCTION__);
	EventHandler::Instance().OnRoleLifeSkillPropsChange(m_module_mgr->GetRole(), props_cfg->type, true);
	gamelog::g_log_life_skill.printf(LL_INFO, "%s user[%d, %s] seq[%d] is_use_change_card[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), seq,
		(int)m_param.is_use_change_card);
	return true;
}

bool RoleNewLifeSkill::OnUseChangeCard(int seq, int use_num, bool is_change, const char * reason)
{
	//变身卡屏蔽
	return false;

	const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(seq);
	if (NULL == props_cfg || NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD != props_cfg->type)
	{
		return false;
	}
	NewLifeSkillChangeSurfaceInfo * surface_info = m_param.GetChangeSurface(seq);
	if (NULL == surface_info)
	{
		if (m_param.change_surface_count >= ARRAY_LENGTH(m_param.change_surface_list))
		{
			return false;
		}
		else
		{
			m_param.change_surface_list[m_param.change_surface_count].seq = seq;
			surface_info = &m_param.change_surface_list[m_param.change_surface_count];
			m_param.change_surface_count++;
		}
	}
	//变身卡没有时间概念
	surface_info->surface_point += props_cfg->increase_point * use_num;
	if (is_change)
	{
		//变身
		m_param.life_skill_props_list[props_cfg->type].seq = surface_info->seq;
		this->SendPropsSingleInfo(props_cfg->type, surface_info->surface_point);
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_life_skill_change_surface_noitce, seq);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
	}
	else
	{
		this->SendChangeCardSingleInfo(surface_info->seq, surface_info->surface_point);
	}
	
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_LIFE_SKILL_PROPS, __FUNCTION__);
	EventHandler::Instance().OnRoleLifeSkillPropsChange(m_module_mgr->GetRole(), props_cfg->type, true);
	gamelog::g_log_life_skill.printf(LL_INFO, "%s user[%d, %s] change_count[%d] seq[%d] suface_point[%d] is_use_change_card[%d] reason[%s]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(),
		m_module_mgr->GetRole()->GetName(), m_param.change_surface_count, seq, surface_info->surface_point, (int)m_param.is_use_change_card, reason);
	return true;
}

void RoleNewLifeSkill::OnSetUseChangeCard(bool is_use_change_card)
{
	m_param.is_use_change_card = is_use_change_card ? 1 : 0;
	EventHandler::Instance().OnRoleLifeSkillPropsChange(m_module_mgr->GetRole(), NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD, true);
}

void RoleNewLifeSkill::OnStartSurface(int seq)
{
	const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(seq);
	if (NULL == props_cfg || NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD != props_cfg->type)
	{
		return;
	}
	NewLifeSkillChangeSurfaceInfo * surface_info = m_param.GetChangeSurface(seq);
	if (NULL == surface_info)
	{
		return;
	}
	if (surface_info->surface_point < props_cfg->expend_point)
	{
		m_module_mgr->NoticeNum(errornum::EN_NEW_LIFE_SKILL_SURFACE_COUNT_NOT_ENOUGH);
		return;
	}

	m_param.life_skill_props_list[props_cfg->type].seq = seq;
	this->SendPropsSingleInfo(props_cfg->type, surface_info->surface_point);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_LIFE_SKILL_PROPS, __FUNCTION__);
	EventHandler::Instance().OnRoleLifeSkillPropsChange(m_module_mgr->GetRole(), props_cfg->type, true);
	
	if (BattleManagerLocal::GetInstance().IsRoleInBattle(m_module_mgr->GetUid()))
	{
		m_module_mgr->NoticeNum(noticenum::NT_NEW_LIFE_SKILL_FIGHT_CHANGE_SURFACE_SUCC);
	}
	else
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_life_skill_change_surface_noitce, seq);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
	}
}

void RoleNewLifeSkill::OnRelieveSurface(int relieve_surface_type)
{
	NewLifeSkillPropsInfo & props_info = m_param.life_skill_props_list[NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD];
	if (props_info.seq <= 0)
	{
		return;
	}
	if (NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_CLIENT == relieve_surface_type)
	{
		m_module_mgr->NoticeNum(noticenum::NT_NEW_LIFE_SKILL_RELIEVE_SURFACE_SUCC);
	}

	props_info.EndReset();
	this->SendPropsSingleInfo(NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD, 0, this->IsNeedNotice(relieve_surface_type));
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_LIFE_SKILL_PROPS, __FUNCTION__, false, Protocol::SCRoleAttrList::NOTIFY_REASON_SYNC_HPMP_IN_FB);
	EventHandler::Instance().OnRoleLifeSkillPropsChange(m_module_mgr->GetRole(), NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD, false);
}

void RoleNewLifeSkill::OnRoleStartFight()
{
	NewLifeSkillPropsInfo & props_info = m_param.life_skill_props_list[NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD];
	if (props_info.seq <= 0)
	{
		return;
	}
	const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(props_info.seq);
	if (NULL == props_cfg)
	{
		return;
	}
	NewLifeSkillChangeSurfaceInfo * surface_info = m_param.GetChangeSurface(props_info.seq);
	if (NULL == surface_info)
	{
		return;
	}

	surface_info->surface_point -= props_cfg->expend_point;
	if (surface_info->surface_point < 0)
	{
		surface_info->surface_point = 0;
	}
	m_param.fight_change_card_seq = props_info.seq;
	this->SendChangeCardSingleInfo(surface_info->seq, surface_info->surface_point);
	gamelog::g_log_life_skill.printf(LL_INFO, "%s user[%d, %s] seq[%d] suface_point[%d] is_use_change_card[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(),
		m_module_mgr->GetRole()->GetName(), props_info.seq, surface_info->surface_point, (int)m_param.is_use_change_card);
}

void RoleNewLifeSkill::OnRoleFinishFight()
{
	NewLifeSkillPropsInfo & props_info = m_param.life_skill_props_list[NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD];
	const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(props_info.seq);
	if (NULL == props_cfg)
	{
		return;
	}
	NewLifeSkillChangeSurfaceInfo * surface_info = m_param.GetChangeSurface(props_info.seq);
	if (NULL == surface_info || surface_info->surface_point >= props_cfg->expend_point)
	{
		return;
	}
	
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_life_skill_surface_point_not_enough, props_info.seq);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
	//解除幻化放后面,防止飘字seq错误
	this->OnRelieveSurface(NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_FINISH_FIGHT);
	props_info.seq = 0;
}

void RoleNewLifeSkill::OnRoleBattlePerformSkill(int change_card_seq)
{
	if(change_card_seq <= 0 && m_param.fight_change_card_seq <= 0) return;
	NewLifeSkillChangeSurfaceInfo * surface_info = NULL;
	surface_info = m_param.GetChangeSurface(change_card_seq); 
	if (NULL == surface_info)
	{
		surface_info = m_param.GetChangeSurface(m_param.fight_change_card_seq);
	}
	if (NULL == surface_info || surface_info->surface_point <= 0)
	{
		return;
	}

	const NewLifeSkillPropsCfg * props_cfg = NULL;
	props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(change_card_seq);
	if (NULL == props_cfg || props_cfg->type != NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD)
	{
		props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(m_param.fight_change_card_seq);
	}
	if (NULL == props_cfg || props_cfg->type != NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD)
	{
		return;
	}
	surface_info->surface_point -= props_cfg->use_skill_need_point;
	if (surface_info->surface_point < 0)
	{
		surface_info->surface_point = 0;
	}

	this->SendChangeCardSingleInfo(surface_info->seq, surface_info->surface_point);
	gamelog::g_log_life_skill.printf(LL_INFO, "%s user[%d, %s] seq[%d] suface_point[%d] is_use_change_card[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(),
		m_module_mgr->GetRole()->GetName(), surface_info->seq, surface_info->surface_point, (int)m_param.is_use_change_card);
}

int RoleNewLifeSkill::GetNewLifeSkillByType(int new_life_skill_type)
{
	if(this->IsInvaild(new_life_skill_type)) return 0;

	return m_param.life_skill_list[new_life_skill_type].skill_level;
}

int RoleNewLifeSkill::GetMultipleByItemSubType(int item_sub_type, int * new_life_skill_type, int * new_life_skill_level)
{
	int multiple = 1;
	int life_skill_type = -1;
	switch (item_sub_type)
	{
		case ItemNamespace::ITEM_SUB_TYPE_STONE:
		multiple = this->GetMultipleByType(NEW_LIFE_SKILL_TYPE_DIG);
		life_skill_type = NEW_LIFE_SKILL_TYPE_DIG;
		break;
		case ItemNamespace::ITEM_SUB_TYPE_WOOD:
		multiple = this->GetMultipleByType(NEW_LIFE_SKILL_TYPE_LUMBERING);
		life_skill_type = NEW_LIFE_SKILL_TYPE_LUMBERING;
		break;
		case ItemNamespace::ITEM_SUB_TYPE_HERB:
		multiple = this->GetMultipleByType(NEW_LIFE_SKILL_TYPE_PICK_FLOWER);
		life_skill_type = NEW_LIFE_SKILL_TYPE_PICK_FLOWER;
		break;
		case ItemNamespace::ITEM_SUB_TYPE_FISH:
		multiple = this->GetMultipleByType(NEW_LIFE_SKILL_TYPE_FISHING);
		life_skill_type = NEW_LIFE_SKILL_TYPE_FISHING;
		break;
		default:
		break;
	}
	if (NULL != new_life_skill_type)
	{
		*new_life_skill_type = life_skill_type;
	}
	if (!this->IsInvaild(life_skill_type) && NULL != new_life_skill_level)
	{
		*new_life_skill_level = m_param.life_skill_list[life_skill_type].skill_level;
	}

	return multiple;
}

void RoleNewLifeSkill::GetChangeCardSkill(int & skill_id, int & seq)
{
	if (m_param.life_skill_props_list[NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD].seq <= 0) return;

	const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(m_param.life_skill_props_list[NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD].seq);
	if (NULL == props_cfg)
	{
		return;
	}
	skill_id = props_cfg->skill_id;
	seq = m_param.life_skill_props_list[NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD].seq;
}

bool RoleNewLifeSkill::IsCanPerformSkillByBattle()
{
	const NewLifeSkillPropsCfg * props_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetPropsCfg(m_param.fight_change_card_seq);
	if (NULL == props_cfg || m_param.fight_change_card_seq <= 0 || props_cfg->type != NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD)
	{
		m_module_mgr->NoticeNum(errornum::EN_NEW_LIFE_SKILL_NOT_HAS_CHANGE_CARD);
		return false;
	}
	NewLifeSkillChangeSurfaceInfo * surface_info = m_param.GetChangeSurface(m_param.fight_change_card_seq);
	if (NULL == surface_info)
	{
		m_module_mgr->NoticeNum(errornum::EN_NEW_LIFE_SKILL_NOT_HAS_CHANGE_CARD);
		return false;
	}
	if (surface_info->surface_point < props_cfg->use_skill_need_point)
	{
		m_module_mgr->NoticeNum(errornum::EN_NEW_LIFE_SKILL_PERFORM_SKILL_ERROR);
		return false;
	}

	return true;
}

void RoleNewLifeSkill::SendAllInfo()
{
	Protocol::SCNewLifeSkillAllInfo info;
	info.count = 0;
	for (int i = 0; i < NEW_LIFE_SKILL_TYPE_MAX && info.count < ARRAY_LENGTH(info.new_life_skill_list) && i < ARRAY_LENGTH(m_param.life_skill_list); i++)
	{
		info.new_life_skill_list[info.count++] = m_param.life_skill_list[i];
	}

	int length = sizeof(info) - (ARRAY_LENGTH(info.new_life_skill_list) - info.count) * sizeof(info.new_life_skill_list[0]);
	m_module_mgr->NetSend(&info, length);
}

void RoleNewLifeSkill::SendSingleInfo(short new_life_skill_type)
{
	if(this->IsInvaild(new_life_skill_type)) return;

	Protocol::SCNewLifeSkillSingleInfo info;
	info.new_life_skill_type = new_life_skill_type;
	info.skill_info = m_param.life_skill_list[new_life_skill_type];

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleNewLifeSkill::SendPropsAllInfo()
{
	Protocol::SCNewLifeSkillPropsAllInfo info;
	info.count = 0;
	for (int i = 0; i < NEW_LIFE_SKILL_PROPS_TYPE_MAX && info.count < ARRAY_LENGTH(info.life_skill_props_list) && i < ARRAY_LENGTH(m_param.life_skill_props_list); i++)
	{
		info.life_skill_props_list[info.count++] = m_param.life_skill_props_list[i];
	}
	info.change_surface_count = 0;
	for (int i = 0; i < m_param.change_surface_count && info.change_surface_count < ARRAY_LENGTH(info.change_surface_list) && i < ARRAY_LENGTH(m_param.change_surface_list); i++)
	{
		info.change_surface_list[info.change_surface_count++] = m_param.change_surface_list[i];
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleNewLifeSkill::SendPropsSingleInfo(int new_life_skill_props_type, int param1, bool is_notice)
{
	if(this->IsPropsInvaild(new_life_skill_props_type)) return;

	Protocol::SCNewLifeSkillPropsSingleInfo info;
	info.is_noitce = is_notice ? 1 : 0;
	info.new_life_skill_props_type = new_life_skill_props_type;
	info.param1 = param1;
	info.props_info = m_param.life_skill_props_list[new_life_skill_props_type];

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleNewLifeSkill::SendChangeCardSingleInfo(int seq, int surface_point)
{
	Protocol::SCNewLifeSkillChangeCardSingleInfo info;
	info.seq = seq;
	info.surface_point = surface_point;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleNewLifeSkill::OnRecoveryChangeCard()
{
	if(m_param.change_surface_count <= 0) return;

	for (int i = 0; i < m_param.change_surface_count && i < ARRAY_LENGTH(m_param.change_surface_list); i++)
	{
		if(m_param.change_surface_list[i].seq <= 0) continue;

		NewLifeSkillChangeSurfaceInfo & info = m_param.change_surface_list[i];
		if (info.surface_point > 0)
		{
			const NewLifeSkillRecoveryCfg * recovery_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetRecoveryCfg(info.seq);
			if (NULL == recovery_cfg)
			{
				continue;
			}
			static MailContentParam contentparam; contentparam.Reset();

			contentparam.virtual_item_list.virtual_item[recovery_cfg->mail_virtual_type] = recovery_cfg->price * info.surface_point;

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_change_card_point_recovery_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_change_card_point_recovery_content, info.seq, info.surface_point, recovery_cfg->price * info.surface_point);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}

		info.Reset();
	}
	m_param.change_surface_count = 0;
	m_param.is_use_change_card = 0;
	m_param.fight_change_card_seq = 0;
	m_param.can_cancel_change_card_timestamp = 0;
	this->OnRelieveSurface(NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_LOGIN);
}

int RoleNewLifeSkill::GetMultipleByType(int new_life_skill_type)
{
	if(this->IsInvaild(new_life_skill_type)) return 1;

	const NewLifeSkillLevelCfg * level_cfg = LOGIC_CONFIG->GetNewLifeSkillConfig()->GetNewLifeSkillLevelCfg(m_param.life_skill_list[new_life_skill_type].skill_level);
	if(NULL == level_cfg) 
	{
		return 1;
	}
	int rate_count = LOGIC_CONFIG->GetNewLifeSkillConfig()->IsNewLifeProf(new_life_skill_type) ? level_cfg->prof_double_rate : level_cfg->double_rate;
	int rand_num = rand() % NewLifeSkillLevelCfg::MAX_DOUBLE_RATE_COUNT_NUM;
	if (rand_num < rate_count)
	{
		return 2;
	}

	return 1;
}

bool RoleNewLifeSkill::IsNeedNotice(int relieve_surface_type)
{
	bool is_notice = false;
	switch (relieve_surface_type)
	{
		case NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_FINISH_FIGHT:
		case NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_CLIENT:
		{
			is_notice = true;
		}
		break;
		default:
		break;
	}

	return is_notice;
}
