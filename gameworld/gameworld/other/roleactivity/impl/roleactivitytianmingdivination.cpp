#include "roleactivitytianmingdivination.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitytianmingdivinationconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgratianmingdivination.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityTianMingDivination::RoleActivityTianMingDivination(int type)
	:RoleActivity(type)
{

}

RoleActivityTianMingDivination::~RoleActivityTianMingDivination()
{

}

void RoleActivityTianMingDivination::OnLogin()
{
	//天命老号问题，上线直接重置倍数
	for (int i = 0; i < RA_TIANMING_LOT_COUNT; ++i)
	{
		if (m_param.ra_tianming_add_lot_times_list[i] <= 0)
		{
			this->OnRATianMingDivinationResetAddLotTimes();
			break;
		}
	}
}

void RoleActivityTianMingDivination::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.ra_tianming_today_chou_tims = 0;
	this->SendRATianMingDivinationActivityInfo();
	this->SetRoleDataChange();
}

void RoleActivityTianMingDivination::OnLoginSendInfo()
{
	this->SendRATianMingDivinationActivityInfo();
}

void RoleActivityTianMingDivination::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RATianMingDivinationParam));
}

void RoleActivityTianMingDivination::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RATianMingDivinationParam));
}

void RoleActivityTianMingDivination::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_tianming_join_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_tianming_join_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityTianMingDivination::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);

	switch (raor->opera_type)
	{
	case Protocol::RA_TIANMING_DIVINATION_OPERA_TYPE_QUERY_INFO:
		{
			this->SendRATianMingDivinationActivityInfo();
		}
		break;

	case Protocol::RA_TIANMING_DIVINATION_OPERA_TYPE_ADD_LOT_TIMES:
		{
			this->OnRATianMingDivinationAddLotTimes(raor->param_1);	// param1是竹签下标
		}
		break;

	case Protocol::RA_TIANMING_DIVINATION_OPERA_TYPE_RESET_ADD_LOT_TIMES:
		{
			this->OnRATianMingDivinationResetAddLotTimes();
		}
		break;

	case Protocol::RA_TIANMING_DIVINATION_OPERA_TYPE_START_CHOU:
		{
			this->OnRATianMingDivinationStartChou();
		}
		break;
	case Protocol::RA_TIANMING_DIVINATION_OPERA_TYPE_REWARD_POOL_INFO:
		{
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
			if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION))
			{
				RandActivity * activity = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);
				if (NULL != activity)
				{
					RandActivityTianMingDivination * act_info = (RandActivityTianMingDivination *)activity;
					act_info->SendTianMingPoolInfo(m_role_module_mgr->GetRole());
				}
			}
		}
		break;
	}
}

void RoleActivityTianMingDivination::SendRATianMingDivinationActivityInfo()
{
	static Protocol::SCRATianMingDivinationInfo tmdi;
	tmdi.free_chou_times = m_param.ra_tianming_free_chou_times;
	for (int i = 0; i < RA_TIANMING_LOT_COUNT; ++i)
	{
		tmdi.add_lots_list[i] = m_param.ra_tianming_add_lot_times_list[i];
	}

	/*
	for (int i = 0; i < RA_TIANMING_REWARD_HISTORY_COUNT; ++i)
	{
	tmdi.reward_history_item_count_list[i].seq = m_activity_info.ra_tianming_reward_history_list[i].seq;
	tmdi.reward_history_item_count_list[i].add_lot = m_activity_info.ra_tianming_reward_history_list[i].add_lot;
	}

	tmdi.reward_history_list_cur_index = m_activity_info.ra_tianming_reward_history_list_cur_index;*/

	tmdi.ra_tianming_today_chou_tims = m_param.ra_tianming_today_chou_tims;
	unsigned int send_len = sizeof(tmdi);
	m_role_module_mgr->NetSend((const char *)&tmdi, send_len);
}

void RoleActivityTianMingDivination::OnRATianMingDivinationAddLotTimes(int lot_index)
{
	if (lot_index < 0 || lot_index >= RA_TIANMING_LOT_COUNT)
	{
		return;
	}

	if (m_param.ra_tianming_add_lot_times_list[lot_index] == RAND_ACTIVITY_TIANMING_DIVINATION_EVERY_LOT_MAX_ADD_TIMES + 1)	// 初始倍数为1
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_TIAN_MING_DIVINATION_MAX_ADD_LOT);
		return;
	}

	int total_add_lot = this->GetRATianMingDivinationTotalAddLotTimes();

	const RandActivityTianMingDivinationConfig * config = static_cast<const RandActivityTianMingDivinationConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION));
	if (NULL == config)
	{
		return;
	}

	const RATianMingDivinationGoldConsumeCfg * tianming_gold_consume_cfg = config->GetTianMingDivinationGoldConsumeCfg(total_add_lot + 1);
	if (NULL == tianming_gold_consume_cfg)
	{
		return;
	}
	int need_gold = tianming_gold_consume_cfg->add_consume_gold;
	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(need_gold))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	if (!m_role_module_mgr->GetMoney()->UseGold(need_gold, "RoleActivity::OnRATianMingDivinationAddLotTimes"))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	++m_param.ra_tianming_add_lot_times_list[lot_index];
	this->SendRATianMingDivinationActivityInfo();
	this->SetRoleDataChange();
}

int RoleActivityTianMingDivination::GetRATianMingDivinationTotalAddLotTimes()
{
	int total_times = 0;
	for (int i = 0; i < RA_TIANMING_LOT_COUNT; ++i)
	{
		total_times += (m_param.ra_tianming_add_lot_times_list[i] - 1);
	}

	return total_times;
}

void RoleActivityTianMingDivination::OnRATianMingDivinationResetAddLotTimes()
{
	for (int i = 0; i < RA_TIANMING_LOT_COUNT; ++i)
	{
		m_param.ra_tianming_add_lot_times_list[i] = 1;
	}

	this->SendRATianMingDivinationActivityInfo();
}

void RoleActivityTianMingDivination::OnRATianMingDivinationStartChou()
{
	int total_add_lot = this->GetRATianMingDivinationTotalAddLotTimes();

	const RandActivityTianMingDivinationConfig * config = static_cast<const RandActivityTianMingDivinationConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION));
	if (NULL == config)
	{
		return;
	}

	const RATianMingDivinationGoldConsumeCfg * tianming_gold_consume_cfg = config->GetTianMingDivinationGoldConsumeCfg(total_add_lot);
	if (NULL == tianming_gold_consume_cfg)
	{
		return;
	}

	int need_gold = tianming_gold_consume_cfg->chou_consume_gold;
	bool use_free_chou = false;
	if (0 == total_add_lot && m_param.ra_tianming_free_chou_times < config->GetTianMingDivinationFreeChouTimes())
	{
		use_free_chou = true;
		need_gold = 0;
	}

	if (m_param.ra_tianming_today_chou_tims >= config->GetOtherCfg()->day_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
		return;
	}

	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(need_gold))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());

	int total_weight = config->GetTianMingDivinationTotalWeight(ramgr);
	int max_seq = config->GetTianMingDivinationRewardMaxSeq(ramgr);

	int reward_seq = 0;
	const RATianMingDivinationRewardCfg * tianming_reward_cfg = NULL;

	if (total_weight <= 0) return;

	int rand_value = rand() % total_weight;
	for (int seq = 1; seq <= max_seq; ++seq)
	{
		tianming_reward_cfg = config->GetTianMingDivinationRewardCfg(ramgr, seq);
		if (NULL == tianming_reward_cfg)
		{
			return;
		}

		if (rand_value < tianming_reward_cfg->weight)
		{
			reward_seq = seq;
			break;
		}

		rand_value -= tianming_reward_cfg->weight;
	}

	if (reward_seq <= 0)
	{
		return;
	}

	int reward_lot = 1; // 奖励倍数，默认1是未加注
	if (total_add_lot > 0)
	{
		int lot_index = config->GetTianMingDivinationLotIndexWithSeq(ramgr, reward_seq);
		if (-1 == lot_index)
		{
			reward_lot = 1;	// -1 表示非加注奖励
		}
		else
		{
			reward_lot = m_param.ra_tianming_add_lot_times_list[lot_index];
		}
	}

	int reward_count = tianming_reward_cfg->reward_item.num * reward_lot;

	const ItemBase *reward_item_base = ITEMPOOL->GetItem(tianming_reward_cfg->reward_item.item_id);
	if (NULL == reward_item_base)
	{
		return;
	}

	int need_empty_slot_num = 1;
	need_empty_slot_num = reward_count / reward_item_base->GetPileLimit();
	if (reward_count % reward_item_base->GetPileLimit() > 0)
	{
		++need_empty_slot_num;
	}

	/*
	if (!m_role->GetKnapsack()->CheckEmptyGridNoLessThan(need_empty_slot_num))
	{
	m_role->NoticeNum(errornum::EN_KNAPSACK_NO_SAPCE);
	return;
	}

	if (use_free_chou)	// 消耗免费次数
	{
	++ m_activity_info.ra_tianming_free_chou_times;
	}
	else
	{
	if (!m_role->GetKnapsack()->GetMoney()->UseGold(need_gold, "RoleActivity::OnRATianMingDivinationStartChou"))
	{
	m_role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
	return;
	}
	}

	for (int i = 0; i < reward_lot; ++ i)
	{
	m_role->GetKnapsack()->Put(tianming_reward_cfg->reward_item, PUT_REASON_TIAN_MING_DIVINATION_REWARD);
	}
	*/

	// 检查背包是否足够
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(tianming_reward_cfg->reward_item.item_id, reward_count))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	if (use_free_chou)	// 消耗免费次数
	{
		++m_param.ra_tianming_free_chou_times;
	}
	else
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(need_gold, "RoleActivity::OnRATianMingDivinationStartChou"))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}

	static ItemConfigData chest_shop_reward_item;
	chest_shop_reward_item.item_id = tianming_reward_cfg->reward_item.item_id;
	chest_shop_reward_item.num = reward_count;
	chest_shop_reward_item.is_bind = tianming_reward_cfg->reward_item.is_bind;
	if (use_free_chou) chest_shop_reward_item.is_bind = true;

	// 发放奖励到寻宝仓库
	/*m_role->GetRoleChestShop()->OnBuyItemList(1, &chest_shop_reward_item, "OnRATianMingDivinationStartChou");*/
	if (!m_role_module_mgr->GetKnapsack()->Put(chest_shop_reward_item, PUT_REASON_NO_NOTICE))
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRATianMingDivinationStartChou user[uid:%d name:%s] err_put_item[id:%d num:%d is_bind:%d]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), chest_shop_reward_item.item_id, chest_shop_reward_item.num, chest_shop_reward_item.is_bind ? 1 : 0);
		return;
	}
	if (tianming_reward_cfg->is_rare == 1)
	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
		if (NULL != ramgr)
		{
			RandActivity * tianming_activity = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);
			if (NULL != tianming_activity)
			{
				RandActivityTianMingDivination *tianming = (RandActivityTianMingDivination *)tianming_activity;
				tianming->OnRoleGetRateItem(m_role_module_mgr->GetRole(), &chest_shop_reward_item);
			}
		}

		if (reward_lot > RAND_ACTIVITY_TIANMING_DIVINATION_EVERY_LOT_MAX_ADD_TIMES)// 10倍加注传闻
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_rand_activity_tian_ming_divination_reward_content,
				m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION, tianming_reward_cfg->reward_item.item_id, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
		else
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_rand_activity_tianming_chou_content,
				m_role_module_mgr->GetRole()->GetName(), reward_lot, (int)tianming_reward_cfg->reward_item.item_id, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
	}

	// 更新卜卦奖励历史记录
	{
		m_param.ra_tianming_reward_history_list[(int)m_param.ra_tianming_reward_history_list_cur_index].seq = reward_seq;
		m_param.ra_tianming_reward_history_list[(int)m_param.ra_tianming_reward_history_list_cur_index].add_lot = reward_lot;
		++m_param.ra_tianming_reward_history_list_cur_index;
		m_param.ra_tianming_reward_history_list_cur_index %= RA_TIANMING_REWARD_HISTORY_COUNT;
	}

	m_param.ra_tianming_today_chou_tims += 1;
	this->SendRATianMingDivinationStartChouResult(reward_seq);
	this->SendRATianMingDivinationActivityInfo();

	this->SetRoleDataChange();

	// 活动参与日志
	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRATianMingDivinationStartChou user[%d, %s], reward_seq[%d], add_lot[%d] reward_item[%s] free_chou_times[%d]",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), reward_seq, reward_lot, ItemConfigDataLog(&chest_shop_reward_item, NULL), m_param.ra_tianming_free_chou_times);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityTianMingDivination::SendRATianMingDivinationStartChouResult(int reward_index)
{
	static Protocol::SCTianMingDivinationActivityStartChouResult tmdascr;
	tmdascr.reward_index = reward_index;
	tmdascr.reserve_sh = 0;

	unsigned int send_len = sizeof(tmdascr);
	m_role_module_mgr->NetSend((const char *)&tmdascr, send_len);
}
