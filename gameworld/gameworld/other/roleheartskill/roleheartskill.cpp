#include "roleheartskill.hpp"
#include "engineadapter.h"
#include "gamelog.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "skill/skillpool.hpp"
#include "obj/character/role.h"
#include "protocol/msgrole.h"
#include "protocol/msgheartskill.hpp"

#include "other/funopen/funopen.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/capability/capability.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/roleheartskillconfig.hpp"

RoleHeartSkill::RoleHeartSkill() : m_module_mgr(NULL)
{
}

RoleHeartSkill::~RoleHeartSkill()
{
}

void RoleHeartSkill::Init(RoleModuleManager * module_mgr, const HeartSkillParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void RoleHeartSkill::GetInitParam(HeartSkillParam * param)
{
	*param = m_param;
}

void RoleHeartSkill::OnRoleLogin()
{
	if (0 == m_param.is_old_user_active_flag)
	{
		this->CheckHeartSkillCollection();

		m_param.is_old_user_active_flag = 1;
	}
}

const AttributeList & RoleHeartSkill::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		//共鸣
		std::map<int, HeartSkillReasonanceInfo> group_count_map;
		for(int i = 0; i < m_param.initiative_skill_count && i < MAX_HEART_SKILL_INITIATIVE_NUM; i++)
		{
			if(m_param.initiative_skill_list[i].skill_level <= 0) continue;

			int reasonance_group = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetReasonanceGroupBySeq(m_param.initiative_skill_list[i].heart_skill_seq);
			if(0 == reasonance_group) continue;

			group_count_map[reasonance_group].count++;
			group_count_map[reasonance_group].min_level = GetMin(group_count_map[reasonance_group].min_level, m_param.initiative_skill_list[i].skill_level);
		}

		for (int i = 0; i < m_param.passive_skill_count && i < MAX_HEART_SKILL_PASSIVE_NUM; i++)
		{
			if (m_param.passive_skill_list[i].skill_level <= 0) continue;

			int reasonance_group = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetReasonanceGroupBySeq(m_param.passive_skill_list[i].heart_skill_seq);
			if (0 == reasonance_group) continue;

			group_count_map[reasonance_group].count++;
			group_count_map[reasonance_group].min_level = GetMin(group_count_map[reasonance_group].min_level, m_param.passive_skill_list[i].skill_level);
		}

		for (std::map<int, HeartSkillReasonanceInfo>::iterator it = group_count_map.begin(); it != group_count_map.end(); ++it)
		{
			if(!LOGIC_CONFIG->GetRoleHeartSkillConfig()->IsMeetReasonance(it->first, it->second.count)) continue;

			const HeartSkillResonanceLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetReasonanceLevelCfg(it->first, it->second.min_level);
			if(NULL == level_cfg) continue;

			for(int i = 0; i < MAX_HEART_SKILL_RESONANCE_ATTR_NUM; i++)
			{
				if (level_cfg->attr_list[i].attr_value > 0 && level_cfg->attr_list->attr_type >= BATTLE_ATTR_MIN && level_cfg->attr_list[i].attr_type < BATTLE_ATTR_MAX)
				{
					m_attrs_add.m_attrs[level_cfg->attr_list[i].attr_type] += level_cfg->attr_list[i].attr_value;
				}
				else
				{
					break;
				}
			}
		}

		//心法属性只在穿戴时增加
		std::vector<int> sp_list;
		for (int i = HEART_SKILL_INITIATIVE_INDEX; i < HEART_SKILL_STATUS_MAX; i++)
		{
			if (-1 == m_param.equip_skill_list[i]) continue;

			int skill_type = RoleHeartSkillConfig::GetSkillTypeByStatus(i);
			HeartSkillInfo * skill_info = this->GetSkillInfo(skill_type, m_param.equip_skill_list[i]);
			if (NULL == skill_info || skill_info->skill_level <= 0) continue;

			const HeartSkillBaseCfg * base_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetBaseCfg(skill_info->heart_skill_seq);
			if (NULL == base_cfg) continue;

			const HeartSkillAttrCfg * attr_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetAttrCfg(skill_info->heart_skill_seq, skill_info->skill_level, 
				m_module_mgr->GetRole()->GetAppearanceProf());
			if (NULL != attr_cfg)
			{
				m_attrs_add.AddAttrInfo(attr_cfg->attr_list);
			}
			if (!this->IsPassiveSkill(i))
			{
				continue;
			}
			//被动技能可能有加面板
			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(base_cfg->heart_skill_id);
			if (NULL == passive_skill) continue;

			// 心法被动技能有等级
			int skill_level = skill_info->skill_level;
			int spid = passive_skill->sp_id;
			if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
			{
				spid = passive_skill->level_spid_list[skill_level];
			}
			sp_list.push_back(spid);
		}
		GetSpecialEffectAttr(sp_list, m_attrs_add, base_add, m_module_mgr->GetRole()->GetLevel(), m_module_mgr->GetRole()->GetLevel());
	}
	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void RoleHeartSkill::CheckHeartSkillCollection()
{
	for (int i = 0; i < m_param.initiative_skill_count && i < ARRAY_LENGTH(m_param.initiative_skill_list); ++i)
	{
		if (m_param.initiative_skill_list[i].skill_level <= 0) continue;

		EventHandler::Instance().OnRoleHeartSkillUnlock(m_module_mgr->GetRole(), m_param.initiative_skill_list[i].heart_skill_seq);
	}
	for (int i = 0; i < m_param.passive_skill_count && i < ARRAY_LENGTH(m_param.passive_skill_list); i++)
	{
		if (m_param.passive_skill_list[i].skill_level <= 0) continue;

		EventHandler::Instance().OnRoleHeartSkillUnlock(m_module_mgr->GetRole(), m_param.passive_skill_list[i].heart_skill_seq);
	}
}

int RoleHeartSkill::GetEquipSkillScore()
{
	int skill_score = 0;
	for (int i = HEART_SKILL_INITIATIVE_INDEX; i < HEART_SKILL_STATUS_MAX; i++)
	{
		if(-1 == m_param.equip_skill_list[i]) continue;

		int skill_type = RoleHeartSkillConfig::GetSkillTypeByStatus(i);
		HeartSkillInfo * skill_info = this->GetSkillInfo(skill_type, m_param.equip_skill_list[i]);
		if(NULL == skill_info) continue;

		skill_score += LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetSkillScore(skill_info->heart_skill_seq, skill_info->skill_level);
	}

	return skill_score;
}

void RoleHeartSkill::OnLearnHeartSkill(int heart_skill_seq, int heart_skill_learn_type)
{
	const HeartSkillBaseCfg * base_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetBaseCfg(heart_skill_seq);
	if(NULL == base_cfg) return;

	int index = 0;
	HeartSkillInfo * skill_info = this->GetSkillInfoBySeq(base_cfg->heart_skill_type, heart_skill_seq, &index);
	const HeartSkillLevelCfg * skill_cfg = NULL;
	bool is_new_skill = false;
	if (NULL == skill_info)
	{
		if (base_cfg->heart_skill_type == HEART_SKILL_SKILL_TYPE_INITATIVE)
		{
			if(m_param.initiative_skill_count < 0 || m_param.initiative_skill_count >= MAX_HEART_SKILL_INITIATIVE_NUM) return;

			skill_info = &m_param.initiative_skill_list[m_param.initiative_skill_count];
			index = m_param.initiative_skill_count;
		}
		else if(base_cfg->heart_skill_type == HEART_SKILL_SKILL_TYPE_PASSIVE)
		{
			if (m_param.passive_skill_count < 0 || m_param.passive_skill_count >= MAX_HEART_SKILL_PASSIVE_NUM) return;

			skill_info = &m_param.passive_skill_list[m_param.passive_skill_count];
			index = m_param.passive_skill_count;
		}
		else
		{
			return;
		}
		is_new_skill = true;
		skill_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetLevelCfg(heart_skill_seq, 1);	
	}
	else
	{
		int max_skill_level = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetMaxLevelBySeq(heart_skill_seq);
		if(max_skill_level <= 0) return;

		if (skill_info->skill_level >= max_skill_level)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_FULL);
			return;
		}
		skill_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetLevelCfg(heart_skill_seq, skill_info->skill_level + 1);
	}
	if (NULL == skill_cfg || NULL == skill_info)
	{
		return;
	}
	switch (heart_skill_learn_type)
	{
		case HEART_SKILL_LEARN_TYPE_COIN:
		{
			if(skill_cfg->coin_consume <= 0) return;

			if (!m_module_mgr->GetMoney()->UseCoinBind(skill_cfg->coin_consume, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		case HEART_SKILL_LEARN_TYPE_GOLD:
		{
			if (skill_cfg->gold_consume <= 0) return;

			if (!m_module_mgr->GetMoney()->UseGold(skill_cfg->gold_consume, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
		case HEART_SKILL_LEARN_TYPE_ITEM:
		{
			if(skill_cfg->item_consume_list.empty()) return;

			int stuff_count = 0;
			ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
			for (int i = 0; i < (int)skill_cfg->item_consume_list.size() && stuff_count < ARRAY_LENGTH(stuff_list); ++i)
			{
				stuff_list[stuff_count].item_id = skill_cfg->item_consume_list[i].item_id;
				stuff_list[stuff_count].num = skill_cfg->item_consume_list[i].num;
				stuff_list[stuff_count].buyable = false;
				stuff_count += 1;
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
		}
		break;
	default:
		return;
		break;
	}

	if (is_new_skill)
	{
		skill_info->heart_skill_seq = heart_skill_seq;
		skill_info->skill_level = skill_cfg->skill_level;
		if (base_cfg->heart_skill_type == HEART_SKILL_SKILL_TYPE_INITATIVE)
		{
			m_param.initiative_skill_count++;
		}
		else
		{
			m_param.passive_skill_count++;
		}
		EventHandler::Instance().OnRoleHeartSkillUnlock(m_module_mgr->GetRole(), heart_skill_seq);
	}
	else
	{
		skill_info->skill_level = skill_cfg->skill_level;
		EventHandler::Instance().OnRoleHeartSkillLevelUp(m_module_mgr->GetRole(), heart_skill_seq, skill_info->skill_level);
		////检测是否是佩戴的,是的话需要算战力
		//if (this->CheckIsEquip(base_cfg->heart_skill_type, index))
		//{
		//	m_module_mgr->GetCapability()->CalcRoleCapability();
		//}
	}
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HEART_SKILL, __FUNCTION__);	//先默认重算
	this->SendSingleInfo(base_cfg->heart_skill_type, index);
	gamelog::g_log_role_heart_skill.printf(LL_INFO, "%s user[%d, %s] heart_skill[%d, %d,%d, %d, %d] learn_type[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), heart_skill_seq, 
		base_cfg->heart_skill_type, base_cfg->heart_skill_id, skill_info->skill_level, index, heart_skill_learn_type);
}

void RoleHeartSkill::OnEquipHeartSkill(int skill_status, int skill_index)
{
	if(!this->IsSkillStatusValid(skill_status)) return;

	const HeartSkillHoleCfg* hole_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetUnlockLevel(skill_status);
	if(NULL == hole_cfg || !this->IsValid(hole_cfg->heart_skill_type, skill_index)) return;

	//if (m_module_mgr->GetRole()->GetLevel() < hole_cfg->unlock_level)
	//{	
	//	m_module_mgr->NoticeNum(errornum::EN_ROLE_HEART_SKILL_LEVEL_NOT_ENOUGH_UNLOCK);
	//	return;
	//}
	if (!(m_param.equip_skill_unlock_flag & (1 << skill_status)))
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_HEART_SKILL_GRID_NOT_UNLOCK);
		return;
	}

	if (m_param.equip_skill_list[skill_status] == skill_index)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_HEART_SKILL_NOT_REPEAT_WEAR);
		return;
	}
	UNSTD_STATIC_CHECK(HEART_SKILL_STATUS_MAX == 6);
	//如果此时有相同技能类型的其他技能栏装备了该技能,那么需要把该技能栏的转移过来
	switch (hole_cfg->heart_skill_type)
	{
	case HEART_SKILL_SKILL_TYPE_INITATIVE:	
	{
		if (skill_index == m_param.equip_skill_list[HEART_SKILL_INITIATIVE_INDEX])
		{
			m_param.equip_skill_list[HEART_SKILL_INITIATIVE_INDEX] = -1;
		}
		else if(skill_index == m_param.equip_skill_list[HEART_SKILL_INITIATIVE_INDEX1])
		{
			m_param.equip_skill_list[HEART_SKILL_INITIATIVE_INDEX1] = -1;
		}
	}
	break;
	case HEART_SKILL_SKILL_TYPE_PASSIVE:
	{
		for (int i = HEART_SKILL_PASSIVE_INDEX; i < HEART_SKILL_PASSIVE_END; i++)
		{
			if (m_param.equip_skill_list[i] == skill_index)
			{
				m_param.equip_skill_list[i] = -1;
			}
		}
	}
	break;
	default:
	return;
	}
	
	m_param.equip_skill_list[skill_status] = skill_index;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HEART_SKILL, __FUNCTION__);
	this->SendOtherInfo();
}

void RoleHeartSkill::OnTakeOffHeartSkill(int skill_status)
{
	if (!this->IsSkillStatusValid(skill_status)) return;

	m_param.equip_skill_list[skill_status] = -1;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HEART_SKILL, __FUNCTION__);
	this->SendOtherInfo();
}

void RoleHeartSkill::OnCompose(int compose_target)
{
	const ItemConfigData* consume_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetComposeCfg(compose_target);
	if(NULL == consume_cfg)
	{
		return;
	}
	if (!m_module_mgr->GetKnapsack()->CheckForPut(compose_target, 1, false))
	{
		m_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItem(consume_cfg->item_id, consume_cfg->num, __FUNCTION__, 0, true))
	{
		return;
	}

	m_module_mgr->GetKnapsack()->Put(ItemConfigData(compose_target, 1, 1), PUT_REASON_HEART_SKILL_COMPOSE, NULL, NULL, 0, 0, false);
	gamelog::g_log_role_heart_skill.printf(LL_INFO, "%s user[%d, %s] compose[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), compose_target);
}

void RoleHeartSkill::OnConvert(int column, int index, int target_item_id)
{	
	const ItemGridData *item_grid_data = m_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	if (NULL == item_grid_data)
	{
		return;
	}
	const HeartSkillConvertCfg * convert_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetConvertCfg(item_grid_data->item_id);
	if(NULL == convert_cfg)
	{
		return;
	}
	if(NULL == ITEMPOOL->GetItem(target_item_id))
	{
		return;
	}
	if (convert_cfg->convert_list.end() == convert_cfg->convert_list.find(target_item_id) || item_grid_data->item_id == target_item_id)
	{
		return;
	}
	
	if (!m_module_mgr->GetKnapsack()->CheckForPut(target_item_id, 1, false))
	{
		m_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	int old_item_id = item_grid_data->item_id;
	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	{
		stuff_list[stuff_count].item_id = convert_cfg->convert_consume_item_id;
		stuff_list[stuff_count].num = 1;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
	}

	{
		stuff_list[stuff_count].item_id = item_grid_data->item_id;
		stuff_list[stuff_count].num = 1;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
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
	m_module_mgr->GetKnapsack()->Put(ItemConfigData(target_item_id, true, 1), PUT_REASON_HEART_SKILL_CONVERT, NULL, NULL, 0, 0, false);
	gamelog::g_log_role_heart_skill.printf(LL_INFO, "%s user[%d, %s] grid[%d, %d] convert[%d, %d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), column, index,
		old_item_id, target_item_id);
}

void RoleHeartSkill::OnUnLock(int skill_status)
{
	if (!this->IsSkillStatusValid(skill_status)) return;

	if(m_param.equip_skill_unlock_flag & (1 << skill_status))
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_HEART_SKILL_UNLOCK);
		return;
	}

	const HeartSkillHoleCfg* hole_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetUnlockLevel(skill_status);
	if (NULL == hole_cfg) return;

	if (m_module_mgr->GetRole()->GetLevel() < hole_cfg->unlock_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_HEART_SKILL_LEVEL_NOT_ENOUGH_UNLOCK);
		return;
	}
	m_param.equip_skill_unlock_flag |= (1 << skill_status);
	this->SendOtherInfo();
	gamelog::g_log_role_heart_skill.printf(LL_INFO, "%s user[%d, %s] skill_status[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), skill_status);
}

void RoleHeartSkill::OnDisplace(std::vector<HeartSkillDisplaceInfo> displace_vec)
{
	if(displace_vec.empty()) return;
	UNSTD_STATIC_CHECK(ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM >= MAX_HEART_SKILL_DISPLACE_NEED_NUM);

	int heart_skill_color = 0;
	const HeartSkillDisplaceCfg * displace_cfg = NULL;
	int need_skill_num = MAX_HEART_SKILL_DISPLACE_NEED_NUM;
	std::map<int, int> stuff_map;
	for(int i = 0; i < (int)displace_vec.size(); ++i)
	{
		const ItemGridData *item_grid_data = m_module_mgr->GetKnapsack()->GetItemGridData2(displace_vec[i].column, displace_vec[i].index);
		if (NULL == item_grid_data)
		{
			return;
		}
		stuff_map[item_grid_data->item_id] += 1;
		int cur_heart_skill_color = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetBaseColorById(item_grid_data->item_id);
		if (0 == i)
		{
			heart_skill_color = cur_heart_skill_color;
			displace_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetDisplaceCfg(heart_skill_color);
			if(NULL != displace_cfg)
			{
				need_skill_num = displace_cfg->need_heart_skill_num;
				stuff_map[displace_cfg->need_consume_item_id] += 1;
			}
			continue;
		}
		if (heart_skill_color != cur_heart_skill_color)
		{
			this->SendDisplaceNotice(need_skill_num);
			return;
		}
	}
	if(heart_skill_color <= 0 || NULL == displace_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_NOT_ELIGIBLE);
		return;
	}
	int stuff_count = 0;
	int item_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for(std::map<int, int>::iterator it = stuff_map.begin(); it != stuff_map.end() && stuff_count < ARRAY_LENGTH(stuff_list); it++)
	{
		stuff_list[stuff_count].item_id = it->first;
		stuff_list[stuff_count].num = it->second;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
		if (displace_cfg->need_consume_item_id == it->first)	//额外消耗道具(不是技能书)
		{
			continue;
		}
		item_count += it->second;
	}

	if (item_count != need_skill_num)
	{
		this->SendDisplaceNotice(need_skill_num);
		return;
	}
	if (!m_module_mgr->GetKnapsack()->CheckForPut(displace_cfg->displace_target_item_id, displace_cfg->target_num, false))
	{
		m_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
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

	m_module_mgr->GetKnapsack()->Put(ItemConfigData(displace_cfg->displace_target_item_id, true, displace_cfg->target_num), PUT_REASON_HEART_SKILL_CONVERT, NULL, NULL, 0, 0, false);
	std::string consume_string;
	for(int i = 0; i < stuff_count && i < ARRAY_LENGTH(stuff_list); ++i)
	{	
		consume_string += STRING_SPRINTF("[item_id:%d num:%d]", stuff_list[i].item_id, stuff_list[i].num);
	}

	gamelog::g_log_role_heart_skill.printf(LL_INFO, "%s user[%d, %s] target[%d, %d] consume_list[%s]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		displace_cfg->displace_target_item_id, displace_cfg->target_num, consume_string.c_str());
}

void RoleHeartSkill::SendAllInfo()
{
	Protocol::SCHeartSkillAllInfo info;
	info.reserve_sh = 0;
	info.reserve_ch = 0;
	info.equip_skill_unlock_flag = m_param.equip_skill_unlock_flag;
	memcpy(info.equip_skill_list, m_param.equip_skill_list, sizeof(info.equip_skill_list));
	info.initiative_skill_count = m_param.initiative_skill_count;
	info.passive_skill_count = m_param.passive_skill_count;
	for (int i = 0; i < info.initiative_skill_count && i < MAX_HEART_SKILL_INITIATIVE_NUM; i++)
	{
		info.initiative_skill_list[i] = m_param.initiative_skill_list[i];
	}
	for (int i = 0; i < info.passive_skill_count && i < MAX_HEART_SKILL_PASSIVE_NUM; i++)
	{
		info.passive_skill_list[i] = m_param.passive_skill_list[i];
	}
	
	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleHeartSkill::SendSingleInfo(int skill_type, int skill_index)
{
	int count = 0;
	HeartSkillInfo * skill_info = this->GetSkillInfo(skill_type, skill_index, &count);
	if (NULL == skill_info)
	{
		return;
	}

	Protocol::SCHeartSkillSingleInfo info;
	info.count = count;
	info.skill_type = skill_type;
	info.skill_index = skill_index;
	info.skill_info = *skill_info;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleHeartSkill::SendOtherInfo()
{
	Protocol::SCHeartSkillOtherInfo info;
	info.reserve_ch = 0;
	info.equip_skill_unlock_flag = m_param.equip_skill_unlock_flag;
	memcpy(info.equip_skill_list, m_param.equip_skill_list, sizeof(info.equip_skill_list));

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleHeartSkill::SendCapInfo()
{
	Protocol::SCRoleAttrList info;
	info.attr_count = 0;
	info.notify_reason = Protocol::SCRoleAttrList::NOTIFY_REASON_HEART_SKILL_EQUIP_CHANGE;
	info.capability = m_module_mgr->GetCapability()->GetRoleCapability();

	int length = sizeof(info) - sizeof(info.attr_list);
	m_module_mgr->NetSend(&info, length);
}

void RoleHeartSkill::GetEquipInitativeSkill(int max_heart_skill_num, char* out_heart_skill_num, BattleHeartSkill * battle_heart_skill_list)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_HEART_SKILL) || NULL == out_heart_skill_num || NULL == battle_heart_skill_list) return;

	for (int i = HEART_SKILL_INITIATIVE_INDEX; i < HEART_SKILL_STATUS_MAX; i++)
	{
		if(this->IsPassiveSkill(i) || -1 == m_param.equip_skill_list[i]) continue;
		
		HeartSkillInfo * skill_info = this->GetSkillInfo(HEART_SKILL_SKILL_TYPE_INITATIVE, m_param.equip_skill_list[i]);
		if (NULL == skill_info || skill_info->skill_level <= 0) continue;

		const HeartSkillBaseCfg * base_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetBaseCfg(skill_info->heart_skill_seq);
		if (NULL == base_cfg) continue;

		if(*out_heart_skill_num < max_heart_skill_num)
		{
			battle_heart_skill_list[(*out_heart_skill_num)].skill_id = base_cfg->heart_skill_id;
			battle_heart_skill_list[(*out_heart_skill_num)].level = skill_info->skill_level;
			battle_heart_skill_list[(*out_heart_skill_num)].remain_times = 1;			//目前写死每场战斗一次
			(*out_heart_skill_num) += 1;
		}
		else
		{
#ifdef _DEBUG
			assert(false);
#else
			gamelog::g_log_battle.printf(LL_INFO, "%s out_heart_skill_num >= max_heart_skill_num[%d] ", __FUNCTION__, max_heart_skill_num);
#endif
			return;
		}
	}
}

void RoleHeartSkill::GetEquipPassiveSkill(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_HEART_SKILL) || NULL == out_other_sp_num || NULL == out_other_sp_list) return;
	
	for (int i = HEART_SKILL_PASSIVE_INDEX; i < HEART_SKILL_PASSIVE_END; i++)
	{
		if(-1 == m_param.equip_skill_list[i]) continue;

		HeartSkillInfo * skill_info = this->GetSkillInfo(HEART_SKILL_SKILL_TYPE_PASSIVE, m_param.equip_skill_list[i]);
		if (NULL == skill_info || skill_info->skill_level <= 0) continue;;

		const HeartSkillBaseCfg * base_cfg = LOGIC_CONFIG->GetRoleHeartSkillConfig()->GetBaseCfg(skill_info->heart_skill_seq);
		if (NULL == base_cfg) continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(base_cfg->heart_skill_id);
		if (NULL == passive_skill) continue;

		// 心法被动技能有等级
		int skill_level = skill_info->skill_level;
		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
		{
			spid = passive_skill->level_spid_list[skill_level];
		}
		if (*out_other_sp_num < max_other_sp_num)
		{
			out_other_sp_list[(*out_other_sp_num)++] = spid;
		}
		else
		{
#ifdef _DEBUG
			assert(false);
#else
			gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, max_other_sp_num);
#endif
			return;
		}

	}

}

bool RoleHeartSkill::IsValid(int skill_type, int skill_index)
{
	switch (skill_type)
	{
		case HEART_SKILL_SKILL_TYPE_INITATIVE:
		{
			if(skill_index < 0 || skill_index >= m_param.initiative_skill_count || skill_index >= MAX_HEART_SKILL_INITIATIVE_NUM) return false;
		}
		break;
		case HEART_SKILL_SKILL_TYPE_PASSIVE:
		{
			if (skill_index < 0 || skill_index >= m_param.passive_skill_count || skill_index >= MAX_HEART_SKILL_PASSIVE_NUM) return false;
		}
		break;
	default:
		return false;
		break;
	}

	return true;
}


bool RoleHeartSkill::CheckIsEquip(int skill_type, int skill_index)
{	
	if(-1 == skill_index) return false;

	switch (skill_type)
	{
		case HEART_SKILL_SKILL_TYPE_INITATIVE:
		{
			if(m_param.equip_skill_list[HEART_SKILL_INITIATIVE_INDEX] == skill_index) 
			{
				return true;
			}
			else if (m_param.equip_skill_list[HEART_SKILL_INITIATIVE_INDEX1] == skill_index)
			{
				return true;
			}
		}
		break;
		case HEART_SKILL_SKILL_TYPE_PASSIVE:
		{
			for (int i = HEART_SKILL_PASSIVE_INDEX; i < HEART_SKILL_PASSIVE_END; i++)
			{
				if (m_param.equip_skill_list[i] == skill_index)
				{
					return true;
				}
			}
		}
		break;
		default:
		break;
	}

	return false;
}

HeartSkillInfo * RoleHeartSkill::GetSkillInfo(int skill_type, int skill_index, int * count)
{
	switch (skill_type)
	{
	case HEART_SKILL_SKILL_TYPE_INITATIVE:
	{
		if (skill_index < 0 || skill_index >= m_param.initiative_skill_count || skill_index >= MAX_HEART_SKILL_INITIATIVE_NUM) return NULL;

		if (NULL != count)
		{
			*count = m_param.initiative_skill_count;
		}
		return &m_param.initiative_skill_list[skill_index];
	}
	break;
	case HEART_SKILL_SKILL_TYPE_PASSIVE:
	{
		if (skill_index < 0 || skill_index >= m_param.passive_skill_count || skill_index >= MAX_HEART_SKILL_PASSIVE_NUM) return NULL;

		if (NULL != count)
		{
			*count = m_param.passive_skill_count;
		}
		return &m_param.passive_skill_list[skill_index];
	}
	break;
	default:
		return NULL;
		break;
	}

	return NULL;
}

HeartSkillInfo * RoleHeartSkill::GetSkillInfoBySeq(int skill_type, short heart_skill_seq, int *index)
{
	switch (skill_type)
	{
	case HEART_SKILL_SKILL_TYPE_INITATIVE:
	{
		for (int i = 0; i < m_param.initiative_skill_count && i < MAX_HEART_SKILL_INITIATIVE_NUM; i++)
		{
			if (heart_skill_seq == m_param.initiative_skill_list[i].heart_skill_seq)
			{
				if (NULL != index)
				{
					*index = i;
				}
				return &m_param.initiative_skill_list[i];
			}
		}
	}
	break;
	case HEART_SKILL_SKILL_TYPE_PASSIVE:
	{
		for (int i = 0; i < m_param.passive_skill_count && i < MAX_HEART_SKILL_PASSIVE_NUM; i++)
		{
			if (heart_skill_seq == m_param.passive_skill_list[i].heart_skill_seq)
			{
				if (NULL != index)
				{
					*index = i;
				}
				return &m_param.passive_skill_list[i];
			}
		}
	}
	break;
	default:
		return NULL;
		break;
	}

	return NULL;
}

void RoleHeartSkill::SendDisplaceNotice(int need_skill_num)
{
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_heart_skill_displace_failed_notice, need_skill_num);
	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			m_module_mgr->NetSend((const char*)&sm, sendlen);
		}
	}
}
