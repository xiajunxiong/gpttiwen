#include "starchart.h"
#include "starchartconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/money.h"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "gamelog.h"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "skill/skillpool.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"


StarChart::StarChart():m_role_module_mgr(NULL)
{
	for (int i = 0; i < STAR_CHART_TYPE_MAX; i++)
	{
		m_star_chart_attrs_add[i].Reset();
		m_super_galaxies_attrs_add[i].Reset();
	}
}

StarChart::~StarChart()
{
}

void StarChart::Init(RoleModuleManager * mgr, const StarChartParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;
}

void StarChart::GetInitParam(StarChartParam * param)
{
	*param = m_param;
}

void StarChart::OnClientRequest(Protocol::CSStarChartRequest & req)
{
	switch (req.op_type)
	{
	case Protocol::CSStarChartRequest::OP_TYPE_STAR_CHART_LEVEL_UP:
		{
			this->StarChartLevelUp(req.param1, req.param2, req.param3);
		}
		break;
	case Protocol::CSStarChartRequest::OP_TYPE_SUPER_GALAXIES_LEVEL_UP:
		{
			this->SuperGalaxiesLevelUp(req.param1);
		}
		break;
// 	case Protocol::CSStarChartRequest::OP_TYPE_STAR_CHART_RESET:
// 		{
// 			this->ResetStarChart(req.param1);
// 		}
// 		break;
	case Protocol::CSStarChartRequest::OP_TYPE_SUPER_GALAXIES_RESET:
		{
			this->ResetSuperGalaxies();
		}
		break;
// 	case Protocol::CSStarChartRequest::OP_TYPE_STAR_CHART_UNLOCK_SKILL:
// 		{
// 			this->UnlockSkillHole(req.param1, req.param2);
// 		}
// 		break;
	case Protocol::CSStarChartRequest::OP_TYPE_STAR_CHART_INSTALL_SKILL:
		{
			this->InstallSkill(req.param1, req.param2, req.param3);
		}
		break;
	case Protocol::CSStarChartRequest::OP_TYPE_STAR_CHART_ONE_KEY_UPGRADE:
		{
			this->StarChartOneKeyLevelUp(req.param1, req.param2, req.param3);
		}
		break;
	case Protocol::CSStarChartRequest::OP_TYPE_SUPER_GALAXIES_ONE_KEY_UPGRADE:
		{
			this->SuperGalaxiesOneKeyLevelUp(req.param1);
		}
		break;
	default:
		break;
	}
}

void StarChart::OnRoleLogin()
{
	this->SendInfo();
}

void StarChart::StarChartLevelUp(int star_chart_type, int star_id, int star_stage)
{
	if (!this->ValidStarChartPoint(star_chart_type, star_stage, star_id)) return;

	if (star_stage != 0 && !this->IsStarChartAllLightUp(star_chart_type, star_stage - 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_NOT_ALL_LIGHT_UP);
		return;
	}

	int next_star_point_level = m_param.star_chart[star_chart_type][star_stage][star_id] + 1;

	const BaseStarChartCfg* star_chart_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetStarChartCfg(star_chart_type, star_stage, star_id, next_star_point_level);
	if (NULL == star_chart_cfg) return;

	if(m_param.star_chart[star_id][star_chart_type] <= 0)
	{
		const StarChartConnectCfg* connect_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetStarChartConnectCfg(star_chart_type, star_stage, star_id);
		if (NULL == connect_cfg) return;

		bool can_active = false;
		for (std::set<int>::const_iterator it = connect_cfg->adjoin.begin(); it != connect_cfg->adjoin.end(); it++)
		{
			if (m_param.star_chart[star_chart_type][star_stage][*it] >= connect_cfg->condition_level){
				can_active = true;
				break;
			}
		}

		if (!can_active){
			m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_POINT_CONDITION_NOT_MET);
			return;
		}
	}

	std::map<int, int> consume_item_map;
	{	// 判断消耗
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(star_chart_cfg->cost_coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	
		for (size_t i = 0; i < star_chart_cfg->cost_item_list.size(); i++)
		{
			std::map<int, int>::iterator it = consume_item_map.find(star_chart_cfg->cost_item_list[i].item_id);
			if (it != consume_item_map.end())
			{
				it->second += star_chart_cfg->cost_item_list[i].num;
			}
			else
			{
				consume_item_map.insert(std::make_pair(star_chart_cfg->cost_item_list[i].item_id, star_chart_cfg->cost_item_list[i].num));
			}
		}

		if (!m_role_module_mgr->GetKnapsack()->HasItemByMap(consume_item_map))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
			return;
		}

		// 判断发背包
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(star_chart_cfg->reward_item_list))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	{	
		// 消耗
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(star_chart_cfg->cost_coin, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByMap(consume_item_map, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		// 发道具
		if (!m_role_module_mgr->GetKnapsack()->PutVec(star_chart_cfg->reward_item_list, PUT_REASON_STAR_CHART_LEVEL_UP))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(star_chart_cfg->reward_item_list);
		}
	}

	m_param.star_chart[star_chart_type][star_stage][star_id] = next_star_point_level;

	this->SendInfo();
	if ((next_star_point_level - 1) == 0){
		this->UnlockSkillHole(star_chart_type, star_id);
	}
	this->NoticeStarAttrChange(star_chart_type);

	{
		std::string log_str;
		for (std::map<int, int>::const_iterator it = consume_item_map.begin(); it != consume_item_map.end(); it++)
		{
			log_str += STRING_SPRINTF("[%d:%d] ", it->first, it->second);
		}

		gamelog::g_log_xing_tu_system.printf(LL_INFO, "%s role[%d, %s] star_chart_type[%d] star_id[%d] cur_level[%d] consume_coin[%d] consume_item[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), star_chart_type, star_id, next_star_point_level, star_chart_cfg->cost_coin, log_str.c_str());
	}
}

void StarChart::StarChartOneKeyLevelUp(int star_chart_type, int star_id, int star_stage)
{
	if (!this->ValidStarChartPoint(star_chart_type, star_stage, star_id)) return;

	if (star_stage != 0 && !this->IsStarChartAllLightUp(star_chart_type, star_stage - 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_NOT_ALL_LIGHT_UP);
		return;
	}

	const BaseStarChartCfg* star_chart_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetStarChartCfg(star_chart_type, star_stage, star_id, m_param.star_chart[star_chart_type][star_stage][star_id] + 1);
	if (NULL == star_chart_cfg) return;

	if (m_param.star_chart[star_id][star_chart_type] <= 0)
	{
		const StarChartConnectCfg* connect_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetStarChartConnectCfg(star_chart_type, star_stage, star_id);
		if (NULL == connect_cfg) return;

		bool can_active = false;
		for (std::set<int>::const_iterator it = connect_cfg->adjoin.begin(); it != connect_cfg->adjoin.end(); it++)
		{
			if (m_param.star_chart[star_chart_type][star_stage][*it] >= connect_cfg->condition_level) {
				can_active = true;
				break;
			}
		}

		if (!can_active) {
			m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_POINT_CONDITION_NOT_MET);
			return;
		}
	}

	int old_level = (int)m_param.star_chart[star_chart_type][star_stage][star_id];
	std::string log_str;

	int count = 0;
	while (NULL != star_chart_cfg)
	{
		std::map<int, int> consume_item_map;
		{
			for (size_t i = 0; i < star_chart_cfg->cost_item_list.size(); i++)
			{
				std::map<int, int>::iterator it = consume_item_map.find(star_chart_cfg->cost_item_list[i].item_id);
				if (it != consume_item_map.end())
				{
					it->second += star_chart_cfg->cost_item_list[i].num;
				}
				else
				{
					consume_item_map.insert(std::make_pair(star_chart_cfg->cost_item_list[i].item_id, star_chart_cfg->cost_item_list[i].num));
				}
			}

			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(star_chart_cfg->cost_coin))
			{
				if (count <= 0) m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				break;
			}
			if (!m_role_module_mgr->GetKnapsack()->HasItemByMap(consume_item_map))
			{
				if (count <= 0) m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
				break;
			}
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(star_chart_cfg->reward_item_list))
			{
				if (count <= 0) m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				break;
			}
		}

		if (m_role_module_mgr->GetMoney()->UseCoinBind(star_chart_cfg->cost_coin, __FUNCTION__) &&
			m_role_module_mgr->GetKnapsack()->ConsumeItemByMap(consume_item_map, __FUNCTION__))
		{
			// 发道具
			if (!m_role_module_mgr->GetKnapsack()->PutVec(star_chart_cfg->reward_item_list, PUT_REASON_STAR_CHART_LEVEL_UP))
			{
				m_role_module_mgr->GetKnapsack()->SendMail(star_chart_cfg->reward_item_list);
			}

			m_param.star_chart[star_chart_type][star_stage][star_id] += 1;
			star_chart_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetStarChartCfg(star_chart_type, star_stage, star_id, m_param.star_chart[star_chart_type][star_stage][star_id] + 1);

			for (std::map<int, int>::iterator it = consume_item_map.begin(); it != consume_item_map.end(); it++)
			{
				log_str += STRING_SPRINTF("[%d : %d] ", it->first, it->second);
			}
		}
		else
		{
			gamelog::g_log_xing_tu_system.printf(LL_WARNING, "%s role[%d, %s] consume_failed star_id[%d] level[%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), star_id, (int)m_param.star_chart[star_chart_type][star_stage][star_id]);
			break;
		}

		count++;
	}

	this->SendInfo();
	if (old_level == 0 && m_param.star_chart[star_id][star_chart_type] != 0)
	{	// 自动解锁技能孔
		this->UnlockSkillHole(star_chart_type, star_id);
	}
	this->NoticeStarAttrChange(star_chart_type);
	gamelog::g_log_xing_tu_system.printf(LL_INFO, "%s role[%d, %s] star_chart_type[%d] star_id[%d] old_level[%d] cur_level[%d] while_count[%d] consume_item[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), star_chart_type, star_id, old_level, (int)m_param.star_chart[star_chart_type][star_stage][star_id], count, log_str.c_str());
}

void StarChart::SuperGalaxiesLevelUp(int super_star_id)
{
	if (!this->ValidSuperGalaxiesPoint(super_star_id)) return;

	int next_super_galaxies_point_level = m_param.super_galaxies[super_star_id] + 1;

	const SuperGalaxiesCfg* super_galaxies_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, next_super_galaxies_point_level);
	if (NULL == super_galaxies_cfg) return;

	if(m_param.super_galaxies[super_star_id] <= 0)
	{
		const StarChartConnectCfg* connect_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesConnectCfg(super_star_id);
		if (NULL == connect_cfg) return;

		bool can_active = false;
		for (std::set<int>::const_iterator it = connect_cfg->adjoin.begin(); it != connect_cfg->adjoin.end(); it++)
		{
			if (m_param.super_galaxies[*it] >= connect_cfg->condition_level)
			{
				can_active = true;
				break;
			}
		}

		if (!can_active) {
			m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_POINT_CONDITION_NOT_MET);
			return;
		}
	}

	std::map<int, int> consume_item_map;
	{	// 判断消耗
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(super_galaxies_cfg->cost_coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}

		for (size_t i = 0; i < super_galaxies_cfg->cost_item_list.size(); i++)
		{
			std::map<int, int>::iterator it = consume_item_map.find(super_galaxies_cfg->cost_item_list[i].item_id);
			if (it != consume_item_map.end())
			{
				it->second += super_galaxies_cfg->cost_item_list[i].num;
			}
			else
			{
				consume_item_map.insert(std::make_pair(super_galaxies_cfg->cost_item_list[i].item_id, super_galaxies_cfg->cost_item_list[i].num));
			}
		}

		if (!m_role_module_mgr->GetKnapsack()->HasItemByMap(consume_item_map))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
			return;
		}
	}

	{	// 消耗
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(super_galaxies_cfg->cost_coin, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByMap(consume_item_map, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}

	{
		if (super_galaxies_cfg->star_type == SUPER_GALAXIESCFG_TYPE_PASSIVE && 0 == m_param.super_galaxies[super_star_id])
		{
			Protocol::SCStarChartInstallSkillNotice notice;
			notice.notice_type = Protocol::SCStarChartInstallSkillNotice::NOTICE_TYPE_ACTIVE;
			notice.skill_seq = super_star_id;
			m_role_module_mgr->NetSend(&notice, sizeof(notice));
		}
	}

	m_param.super_galaxies[super_star_id] = next_super_galaxies_point_level;

	this->SendInfo();
	this->NoticeStarAttrChange(super_galaxies_cfg->target);

	{
		std::string log_str;
		for (std::map<int, int>::const_iterator it = consume_item_map.begin(); it != consume_item_map.end(); it++)
		{
			log_str += STRING_SPRINTF("[%d:%d] ", it->first, it->second);
		}
		gamelog::g_log_xing_tu_system.printf(LL_INFO, "%s role[%d, %s] super_star_id[%d] cur_level[%d] consume_coin[%d] consume_item[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), super_star_id, next_super_galaxies_point_level, super_galaxies_cfg->cost_coin, log_str.c_str());
	}
}

void StarChart::SuperGalaxiesOneKeyLevelUp(int super_star_id)
{
	if (!this->ValidSuperGalaxiesPoint(super_star_id)) return;

	const SuperGalaxiesCfg* super_galaxies_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id] + 1);
	if (NULL == super_galaxies_cfg) return;

	if (m_param.super_galaxies[super_star_id] <= 0)
	{
		const StarChartConnectCfg* connect_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesConnectCfg(super_star_id);
		if (NULL == connect_cfg) return;

		bool can_active = false;
		for (std::set<int>::const_iterator it = connect_cfg->adjoin.begin(); it != connect_cfg->adjoin.end(); it++)
		{
			if (m_param.super_galaxies[*it] >= connect_cfg->condition_level)
			{
				can_active = true;
				break;
			}
		}

		if (!can_active) {
			m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_POINT_CONDITION_NOT_MET);
			return;
		}
	}

	int old_level = m_param.super_galaxies[super_star_id];
	std::string log_str;

	while (NULL != super_galaxies_cfg)
	{
		std::map<int, int> consume_item_map;
		{
			for (size_t i = 0; i < super_galaxies_cfg->cost_item_list.size(); i++)
			{
				std::map<int, int>::iterator it = consume_item_map.find(super_galaxies_cfg->cost_item_list[i].item_id);
				if (it != consume_item_map.end())
				{
					it->second += super_galaxies_cfg->cost_item_list[i].num;
				}
				else
				{
					consume_item_map.insert(std::make_pair(super_galaxies_cfg->cost_item_list[i].item_id, super_galaxies_cfg->cost_item_list[i].num));
				}
			}

			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(super_galaxies_cfg->cost_coin)) break;
			if (!m_role_module_mgr->GetKnapsack()->HasItemByMap(consume_item_map)) break;
		}

		if (m_role_module_mgr->GetMoney()->UseCoinBind(super_galaxies_cfg->cost_coin, __FUNCTION__) &&
			m_role_module_mgr->GetKnapsack()->ConsumeItemByMap(consume_item_map, __FUNCTION__))
		{
			m_param.super_galaxies[super_star_id] += 1;
			super_galaxies_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id] + 1);

			for (std::map<int, int>::iterator it = consume_item_map.begin(); it != consume_item_map.end(); it++)
			{
				log_str += STRING_SPRINTF("[%d : %d] ", it->first, it->second);
			}
		}
		else
		{
			gamelog::g_log_xing_tu_system.printf(LL_WARNING, "%s role[%d, %s] consume_failed star_id[%d] level[%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), super_star_id, m_param.super_galaxies[super_star_id]);
			break;
		}
	}

	this->SendInfo();
	super_galaxies_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id]);
	if (NULL != super_galaxies_cfg)
	{
		this->NoticeStarAttrChange(super_galaxies_cfg->target);
	}
	gamelog::g_log_xing_tu_system.printf(LL_INFO, "%s role[%d, %s] super_star_id[%d] old_level[%d] cur_level[%d] consume_item[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), super_star_id, old_level, m_param.super_galaxies[super_star_id], log_str.c_str());
}

void StarChart::ResetSuperGalaxies()
{
	Int64 all_recover_coin = 0LL;
	std::map<ItemID, int> all_recover_item;

	for (int index = 0; index < MAX_SUPER_GALAXIES_POINT; index++)
	{
		for (int level = m_param.super_galaxies[index]; level > 0; level--)
		{
			const SuperGalaxiesCfg* super_galaxier_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(index, level);
			if (NULL == super_galaxier_cfg)
			{
				gamelog::g_log_xing_tu_system.printf(LL_WARNING, "%s role[%d, %s] index[%d] level[%d] Recover Failed!", __FUNCTION__,
					m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), index, level);
				continue;
			}

			all_recover_coin += super_galaxier_cfg->cost_coin;

			for (size_t i = 0; i < super_galaxier_cfg->cost_item_list.size(); i++)
			{
				std::map<ItemID, int>::iterator it = all_recover_item.find(super_galaxier_cfg->cost_item_list[i].item_id);
				if (it == all_recover_item.end())
				{
					all_recover_item.insert(std::make_pair(super_galaxier_cfg->cost_item_list[i].item_id, super_galaxier_cfg->cost_item_list[i].num));
				}
				else
				{
					it->second += super_galaxier_cfg->cost_item_list[i].num;
				}
			}
		}
	}

	std::vector<ItemConfigData> recover_list;
	for (std::map<ItemID, int>::iterator it = all_recover_item.begin(); it != all_recover_item.end(); it++)
	{
		recover_list.push_back(ItemConfigData(it->first, true, it->second));
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(recover_list))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(LOGIC_CONFIG->GetStarChartConfig()->GetOtherCfg().reset_gold2))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	if (!m_role_module_mgr->GetMoney()->UseGold(LOGIC_CONFIG->GetStarChartConfig()->GetOtherCfg().reset_gold2, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	all_recover_coin = all_recover_coin * LOGIC_CONFIG->GetStarChartConfig()->GetOtherCfg().reset_back / STAR_CHART_RESET_POINT_RATIO;

	memset(m_param.super_galaxies, STAR_CHART_STATE_LOCK, sizeof(m_param.super_galaxies));

	for (int i = 0; i < MAX_TAR_CHART_TYPE; i++)
	{
		for (int j = 0; j < MAX_STAR_CHART_SKILL; j++)
		{
			if (m_param.star_chart_skill[j][i] != STAR_CHART_STATE_SKILL_LOCK)
			{
				m_param.star_chart_skill[j][i] = STAR_CHART_STATE_SKILL_UNLOCK;
			}
		}
	}

	if (!m_role_module_mgr->GetMoney()->AddCoinBind(all_recover_coin, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(recover_list, PUT_REASON_STAR_CHART_RESET))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
	}

	this->SendInfo();
	this->NoticeStarAttrChange();

	{
		std::string log_str;
		for (size_t i = 0; i < recover_list.size(); i++)
		{
			log_str += STRING_SPRINTF("item_%d[%d:%d] ", i + 1, recover_list[i].item_id, recover_list[i].num);
		}

		gamelog::g_log_xing_tu_system.printf(LL_INFO, "%s role[%d, %s] all_recover_coin[%lld] recover_list[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), all_recover_coin, log_str.c_str());
	}
}

void StarChart::UnlockSkillHole(int star_chart_type, int star_id)
{
	if (star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return;
	if (!this->ValidStarChartPoint(star_chart_type, 0, star_id)) return;
	
	for (int skill_seq = 0; skill_seq < MAX_STAR_CHART_SKILL; skill_seq++)
	{
		const std::set<int>* need_hole_list = LOGIC_CONFIG->GetStarChartConfig()->GetSkillHoleOpenCfg(star_chart_type, skill_seq);
		if (NULL == need_hole_list) continue;

		std::set<int>::const_iterator it = need_hole_list->find(star_id);
		if (it != need_hole_list->end())
		{
			if (m_param.star_chart_skill[skill_seq][star_chart_type] != STAR_CHART_STATE_SKILL_LOCK) continue;

			bool can_unlock = true;
			for (std::set<int>::iterator it = need_hole_list->begin(); it != need_hole_list->end(); it++)
			{
				if (m_param.star_chart[star_chart_type][0][*it] <= STAR_CHART_STATE_LOCK)
				{
					can_unlock = false;
					break;
				}
			}

			if (can_unlock)
			{
				m_param.star_chart_skill[skill_seq][star_chart_type] = STAR_CHART_STATE_SKILL_UNLOCK;

				this->SendInfo();
				m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_SKILL_HOLE_UNLOCKED);
				gamelog::g_log_xing_tu_system.printf(LL_INFO, "%s role[%d, %s] star_chart_type[%d] skill_seq[%d] cur_state[%d]", __FUNCTION__,
					m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), star_chart_type, skill_seq, m_param.star_chart_skill[skill_seq][star_chart_type]);
			}
		}
	}
}

void StarChart::InstallSkill(int star_chart_type, int skill_hole_seq, int super_star_id)
{
	if (star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return;
	if (skill_hole_seq < 0 || skill_hole_seq >= MAX_STAR_CHART_SKILL) return;
	if (!this->ValidSuperGalaxiesPoint(super_star_id)) return;

	if (m_param.star_chart_skill[skill_hole_seq][star_chart_type] == STAR_CHART_STATE_SKILL_LOCK)
	{
		return;
	}

	// 判断重复星图技能
	for (int i = 0; i < MAX_STAR_CHART_SKILL; i++)
	{
		if (m_param.star_chart_skill[i][star_chart_type] == super_star_id)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_SKILL_REPEAT);
			return;
		}
	}

	if (m_param.super_galaxies[super_star_id] == STAR_CHART_STATE_LOCK)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_SKILL_LOCK);
		return;
	}

	const SuperGalaxiesCfg* sup_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id]);
	if (NULL == sup_cfg) return;

	if (sup_cfg->target != star_chart_type)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STAR_CHART_SKILL_TYPE_MISMATCH);
		return;
	}

	if (sup_cfg->star_type != SUPER_GALAXIESCFG_TYPE_PASSIVE)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int old_skill_id = m_param.star_chart_skill[skill_hole_seq][star_chart_type];
	m_param.star_chart_skill[skill_hole_seq][star_chart_type] = super_star_id;

	this->SendInfo();
	this->NoticeStarAttrChange(star_chart_type);
	{
		Protocol::SCStarChartInstallSkillNotice notice;
		notice.notice_type = Protocol::SCStarChartInstallSkillNotice::NOTICE_TYPE_USED;
		notice.skill_seq = super_star_id;
		m_role_module_mgr->NetSend(&notice, sizeof(notice));
	}

	gamelog::g_log_xing_tu_system.printf(LL_INFO, "%s role[%d, %s] star_chart_type[%d] skill_hole_seq[%d] super_star_id[%d] old_skill_id[%d] cur_skill_id[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), star_chart_type, skill_hole_seq, super_star_id, old_skill_id, m_param.star_chart_skill[skill_hole_seq][star_chart_type]);
}

void StarChart::ChangeProfession(int old_prof, int new_prof)
{
	this->ReCalStarTypeAttr(STAR_CHART_TYPE_ROLE);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_STAR_CHART, __FUNCTION__);
}

const AttributeList & StarChart::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		this->ReCalStarTypeAttr(STAR_CHART_TYPE_ROLE);
		this->ReCalSuperGalaxiesAttr();
	}

	// 因为函数返回引用, 声明一个静态变量用于返回加值
	static AttributeList total_attrs_add;
	total_attrs_add.Reset();
	total_attrs_add.Add(m_star_chart_attrs_add[STAR_CHART_TYPE_ROLE].m_attrs);
	total_attrs_add.Add(m_super_galaxies_attrs_add[STAR_CHART_TYPE_ROLE].m_attrs);

	base_add.Add(total_attrs_add.m_attrs);
	return total_attrs_add;
}

const AttributeList & StarChart::ReCalcPetAttr(AttributeList & base_add)
{
	this->ReCalStarTypeAttr(STAR_CHART_TYPE_PET);
	this->ReCalSuperGalaxiesAttr();

	// 因为函数返回引用, 声明一个静态变量用于返回加值
	static AttributeList total_attrs_add;
	total_attrs_add.Reset();
	total_attrs_add.Add(m_star_chart_attrs_add[STAR_CHART_TYPE_PET].m_attrs);
	total_attrs_add.Add(m_super_galaxies_attrs_add[STAR_CHART_TYPE_PET].m_attrs);

	base_add.Add(total_attrs_add.m_attrs);
	return total_attrs_add;
}

const AttributeList & StarChart::ReCalcPartnerAttr(AttributeList & base_add)
{
	this->ReCalStarTypeAttr(STAR_CHART_TYPE_PARTNER);
	this->ReCalSuperGalaxiesAttr();

	// 因为函数返回引用, 声明一个静态变量用于返回加值
	static AttributeList total_attrs_add;
	total_attrs_add.Reset();
	total_attrs_add.Add(m_star_chart_attrs_add[STAR_CHART_TYPE_PARTNER].m_attrs);
	total_attrs_add.Add(m_super_galaxies_attrs_add[STAR_CHART_TYPE_PARTNER].m_attrs);

	base_add.Add(total_attrs_add.m_attrs);
	return total_attrs_add;
}

void StarChart::OnGetRoleBattleEffect(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	for (int index = 0; index < MAX_STAR_CHART_SKILL; index++)
	{
		int super_star_id = m_param.star_chart_skill[index][STAR_CHART_TYPE_ROLE];

		if (m_param.star_chart_skill[index][STAR_CHART_TYPE_ROLE] == STAR_CHART_STATE_SKILL_LOCK)
			continue;

		if (!this->ValidSuperGalaxiesPoint(super_star_id))
			continue;

		const SuperGalaxiesCfg* sup_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id]);
		if (NULL == sup_cfg) continue;

		if (sup_cfg->target != STAR_CHART_TYPE_ROLE)
			continue;

		if (sup_cfg->star_type != SUPER_GALAXIESCFG_TYPE_PASSIVE)
			continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(sup_cfg->param.first);
		if (NULL == passive_skill) continue;

		if(m_param.super_galaxies[super_star_id] < 0 || m_param.super_galaxies[super_star_id] > MAX_BATTLE_SKILL_LEVEL)
			continue;

		// 获取特效id
		int sp_id = passive_skill->level_spid_list[(int)m_param.super_galaxies[super_star_id]];

		const SpecialEffectCfg* effect_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (NULL == effect_cfg) continue;

		for (int effect_index = 0; effect_index < EFFECT_DETAIL_NUM; effect_index++)
		{
			if (effect_cfg->effect_detail_list[effect_index].effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR)
			{
				if (*out_other_sp_num < max_other_sp_num)
				{
					out_other_sp_list[(*out_other_sp_num)++] = sp_id;
					break;
				}
				else
				{
#ifdef _DEBUG
					assert(0);
#else
					gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num[%d] >= max_other_sp_num[%d] ", __FUNCTION__, *out_other_sp_num, max_other_sp_num);
#endif
					return;
				}
			}
		}
	}
}

void StarChart::GetPetEffectList(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	for (int index = 0; index < MAX_STAR_CHART_SKILL; index++)
	{
		int super_star_id = m_param.star_chart_skill[index][STAR_CHART_TYPE_PET];

		if (m_param.star_chart_skill[index][STAR_CHART_TYPE_PET] == STAR_CHART_STATE_SKILL_LOCK)
			continue;

		if (!this->ValidSuperGalaxiesPoint(super_star_id))
			continue;

		const SuperGalaxiesCfg* sup_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id]);
		if (NULL == sup_cfg) continue;

		if (sup_cfg->target != STAR_CHART_TYPE_PET)
			continue;

		if (sup_cfg->star_type != SUPER_GALAXIESCFG_TYPE_PASSIVE)
			continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(sup_cfg->param.first);
		if (NULL == passive_skill) continue;

		if (m_param.super_galaxies[super_star_id] < 0 || m_param.super_galaxies[super_star_id] > MAX_BATTLE_SKILL_LEVEL)
			continue;

		// 获取特效id
		int sp_id = passive_skill->level_spid_list[(int)m_param.super_galaxies[super_star_id]];

		const SpecialEffectCfg* effect_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (NULL == effect_cfg) continue;

		for (int effect_index = 0; effect_index < EFFECT_DETAIL_NUM; effect_index++)
		{
			if (effect_cfg->effect_detail_list[effect_index].effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR)
			{
				if (*out_other_sp_num < max_other_sp_num)
				{
					out_other_sp_list[(*out_other_sp_num)++] = sp_id;
					break;
				}
				else
				{
#ifdef _DEBUG
					assert(0);
#else
					gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num[%d] >= max_other_sp_num[%d] ", __FUNCTION__, *out_other_sp_num, max_other_sp_num);
#endif
					return;
				}
			}
		}
	}
}

void StarChart::GetPartnerBattleEffect(int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list)
{
	for (int index = 0; index < MAX_STAR_CHART_SKILL; index++)
	{
		int super_star_id = m_param.star_chart_skill[index][STAR_CHART_TYPE_PARTNER];

		if (m_param.star_chart_skill[index][STAR_CHART_TYPE_PARTNER] == STAR_CHART_STATE_SKILL_LOCK)
			continue;

		if (!this->ValidSuperGalaxiesPoint(super_star_id))
			continue;

		const SuperGalaxiesCfg* sup_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id]);
		if (NULL == sup_cfg) continue;

		if (sup_cfg->target != STAR_CHART_TYPE_PARTNER)
			continue;

		if (sup_cfg->star_type != SUPER_GALAXIESCFG_TYPE_PASSIVE)
			continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(sup_cfg->param.first);
		if (NULL == passive_skill) continue;

		if (m_param.super_galaxies[super_star_id] < 0 || m_param.super_galaxies[super_star_id] > MAX_BATTLE_SKILL_LEVEL)
			continue;

		// 获取特效id
		int sp_id = passive_skill->level_spid_list[(int)m_param.super_galaxies[super_star_id]];

		const SpecialEffectCfg* effect_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (NULL == effect_cfg) continue;

		for (int effect_index = 0; effect_index < EFFECT_DETAIL_NUM; effect_index++)
		{
			if (effect_cfg->effect_detail_list[effect_index].effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR)
			{
				if (*out_other_sp_num < max_other_sp_num)
				{
					out_other_sp_list[(*out_other_sp_num)++] = sp_id;
					break;
				}
				else
				{
#ifdef _DEBUG
					assert(0);
#else
					gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num[%d] >= max_other_sp_num[%d] ", __FUNCTION__, *out_other_sp_num, max_other_sp_num);
#endif
					return;
				}
			}
		}
	}
}


bool StarChart::ValidStarChartPoint(int star_chart_type, int stage, int star_id) const
{
	return LOGIC_CONFIG->GetStarChartConfig()->ValidStarChartPoint(star_chart_type, stage, star_id);
}

bool StarChart::ValidSuperGalaxiesPoint(int star_id) const
{
	return LOGIC_CONFIG->GetStarChartConfig()->ValidSuperGalaxiesPoint(star_id);
}

void StarChart::GmAllLigthUp()
{
	for (int i = 0; i < MAX_SUPER_GALAXIES_POINT; i++)
	{
		if (!this->ValidSuperGalaxiesPoint(i)) continue;
		m_param.super_galaxies[i] = 10;
	}

	for (int i = 0; i < MAX_TAR_CHART_TYPE; i++)
	{
		for (int j = 0; j < MAX_STAR_CHART_STAGE; j++)
		{
			for (int z = 0; z < MAX_STAR_CHART_POINT; z++)
			{
				if (!this->ValidStarChartPoint(i, j, z)) continue;
				m_param.star_chart[i][j][z] = 10;
			}
		}
	}

	for (int i = 0; i < MAX_STAR_CHART_SKILL; i++)
	{
		for (int j = 0; j < MAX_TAR_CHART_TYPE; j++)
		{
			m_param.star_chart_skill[i][j] = STAR_CHART_STATE_SKILL_UNLOCK;
		}
	}

	this->SendInfo();
	this->NoticeStarAttrChange();
}

void StarChart::GmResetStarChart()
{
	memset(m_param.star_chart, STAR_CHART_STATE_LOCK, sizeof(m_param.star_chart));
	memset(m_param.star_chart_skill, STAR_CHART_STATE_SKILL_LOCK, sizeof(m_param.star_chart_skill));

	this->SendInfo();
	this->NoticeStarAttrChange();
}

void StarChart::ReCalStarTypeAttr(int star_chart_type)
{
	if (star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return;

	AttributeList & attr_list = m_star_chart_attrs_add[star_chart_type];
	attr_list.Reset();

	for (int stage = 0; stage < MAX_STAR_CHART_STAGE; stage++)
	{
		for (int star_id = 0; star_id < MAX_STAR_CHART_POINT; star_id++)
		{
			if (m_param.star_chart[star_chart_type][stage][star_id] <= STAR_CHART_STATE_LOCK) continue;

			const BaseStarChartCfg* star_chart_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetStarChartCfg(star_chart_type, stage, star_id, m_param.star_chart[star_chart_type][stage][star_id]);
			if (NULL == star_chart_cfg) continue;

			switch (star_chart_type)	// 根据星图类型不同需要转为不同的子类
			{
			case STAR_CHART_TYPE_ROLE:
			{
				const RoleStarChartCfg* role_star_chart_cfg = static_cast<const RoleStarChartCfg*>(star_chart_cfg);

				const AttrInfo* temp_attr = role_star_chart_cfg->GetAttr(m_role_module_mgr->GetRole()->GetBaseProfession());
				if (NULL == temp_attr) continue;

				attr_list.m_attrs[temp_attr->attr_type] += temp_attr->attr_value;
			}
			break;
			case STAR_CHART_TYPE_PET:
			case STAR_CHART_TYPE_PARTNER:
			{
				const PetOrPartnerStarChartCfg* pet_or_partner_star_chart_cfg = static_cast<const PetOrPartnerStarChartCfg*>(star_chart_cfg);

				attr_list.m_attrs[pet_or_partner_star_chart_cfg->attr.attr_type] += pet_or_partner_star_chart_cfg->attr.attr_value;
			}
			break;
			default:
				continue;
			}
		}
	}

	for (int index = 0; index < MAX_STAR_CHART_SKILL; index++)
	{
		int super_star_id = m_param.star_chart_skill[index][star_chart_type];

		if (m_param.star_chart_skill[index][star_chart_type] == STAR_CHART_STATE_SKILL_LOCK)
			continue;

		if (!this->ValidSuperGalaxiesPoint(super_star_id))
			continue;

		const SuperGalaxiesCfg* sup_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(super_star_id, m_param.super_galaxies[super_star_id]);
		if (NULL == sup_cfg) continue;

		if (sup_cfg->target != star_chart_type)
			continue;

		if (sup_cfg->star_type != SUPER_GALAXIESCFG_TYPE_PASSIVE)
			continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(sup_cfg->param.first);
		if (NULL == passive_skill) continue;

		if (m_param.super_galaxies[super_star_id] < 0 || m_param.super_galaxies[super_star_id] > MAX_BATTLE_SKILL_LEVEL)
			continue;

		// 获取特效id
		int sp_id = passive_skill->level_spid_list[(int)m_param.super_galaxies[super_star_id]];

		const SpecialEffectCfg* effect_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (NULL == effect_cfg) continue;

		for (int effect_index = 0; effect_index < EFFECT_DETAIL_NUM; effect_index++)
		{
			if (SPECIAL_EFFECT_TYPE_ADD_ATTR != effect_cfg->effect_detail_list[effect_index].effect_type) continue;

			const EffectDetailCfg* detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(effect_cfg->effect_detail_list[effect_index]);
			if (NULL == detail_cfg) continue;

			// 判断加属性类型
			if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				attr_list.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
			}
		}
	}

}

void StarChart::ReCalSuperGalaxiesAttr()
{
	for (int i = 0; i < STAR_CHART_TYPE_MAX; i++)
	{
		m_super_galaxies_attrs_add[i].Reset();
	}

	for (int star_id = 0; star_id < MAX_SUPER_GALAXIES_POINT; star_id++)
	{
		if (m_param.super_galaxies[star_id] <= STAR_CHART_STATE_LOCK) continue;

		const SuperGalaxiesCfg* star_chart_cfg = LOGIC_CONFIG->GetStarChartConfig()->GetSuperGalaxiesCfg(star_id, m_param.super_galaxies[star_id]);
		if (NULL == star_chart_cfg) continue;
		
		if(star_chart_cfg->star_type != SUPER_GALAXIESCFG_TYPE_ATTR) continue;

		AttributeList & attr_list = m_super_galaxies_attrs_add[star_chart_cfg->target];

		attr_list.m_attrs[star_chart_cfg->param.first] += star_chart_cfg->param.second;
	}
}

bool StarChart::IsStarChartAllLightUp(int star_chart_type, int star_chart_stage)
{
	for (int star_id = 0; star_id < MAX_STAR_CHART_POINT; star_id++)
	{
		if (!this->ValidStarChartPoint(star_chart_type, star_chart_stage, star_id)) continue;
		if (m_param.star_chart[star_chart_type][star_chart_stage][star_id] <= 0) return false;
	}

	return true;
}

void StarChart::NoticeStarAttrChange(int star_chart_type)
{
	if(star_chart_type == STAR_CHART_TYPE_MAX || star_chart_type == STAR_CHART_TYPE_ROLE) m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_STAR_CHART, __FUNCTION__);
	if(star_chart_type == STAR_CHART_TYPE_MAX || star_chart_type == STAR_CHART_TYPE_PET) m_role_module_mgr->GetPet()->OnStarChartPetAttrChange();
	if(star_chart_type == STAR_CHART_TYPE_MAX || star_chart_type == STAR_CHART_TYPE_PARTNER) m_role_module_mgr->GetPartner()->ReCalAllPartnerAttr(Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_STAR_CHART);
	UNSTD_STATIC_CHECK(STAR_CHART_TYPE_MAX == 3)
}

void StarChart::SendInfo()
{
	Protocol::SCStarChartAllInfo msg;

	memcpy(msg.super_galaxies, m_param.super_galaxies, sizeof(msg.super_galaxies));
	memcpy(msg.star_chart, m_param.star_chart, sizeof(msg.star_chart));
	memcpy(msg.star_chart_skill, m_param.star_chart_skill, sizeof(msg.star_chart_skill));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

