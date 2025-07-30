#include "roleactivitytujiantreasure.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitytujiantreasureconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgratujiantreasure.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityTuJianTreasure::RoleActivityTuJianTreasure(int type)
	:RoleActivity(type)
{

}

RoleActivityTuJianTreasure::~RoleActivityTuJianTreasure()
{

}

void RoleActivityTuJianTreasure::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RATuJianTreasureParam));
}

void RoleActivityTuJianTreasure::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RATuJianTreasureParam));
}

void RoleActivityTuJianTreasure::OnLogin()
{
	if (0 == m_param.ra_tu_jian_treasure_end_timestamp)
	{
		this->CheckTuJianTreasureOpen(m_role_module_mgr->GetRole()->GetLevel());
	}
	this->OnSetLoginDay();
}

void RoleActivityTuJianTreasure::OnLoginSendInfo()
{
	this->OnSetLoginDay();
	this->SendRATuJianTreasureInfo();
}

void RoleActivityTuJianTreasure::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
	{
		if (this->IsRoleTuJianTreasureOpen())
		{
			this->OnSetLoginDay();
			this->SendRATuJianTreasureInfo();
			this->SetRoleDataChange();
		}
	}
}

void RoleActivityTuJianTreasure::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_tu_jian_treasure_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_tu_jian_treasure_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->CheckTuJianTreasureOpen(m_role_module_mgr->GetRole()->GetLevel());
		this->SetRoleDataChange();
	}
}

void RoleActivityTuJianTreasure::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE);

	switch (raor->opera_type)
	{
	case Protocol::RA_TU_JIAN_TREASURE_OPERA_TYPE_INFO:
		{
			this->SendRATuJianTreasureInfo();
		}
		break;
	case Protocol::RA_TU_JIAN_TREASURE_OPERA_TYPE_FETCH_DAY:
		{
			this->OnRATuJianTreasureFetchDay(raor->param_1);
		}
		break;
	case Protocol::RA_TU_JIAN_TREASURE_OPERA_TYPE_FETCH:
		{
			this->OnRATuJianTreasureFetch(raor->param_1);	
		}
		break;
	}
}

void RoleActivityTuJianTreasure::OnRoleLevelUp(int old_level, int to_level)
{
	this->CheckTuJianTreasureOpen(to_level);
}

void RoleActivityTuJianTreasure::OnTuJianTreasureCheckReward(int type, int param1, std::vector<ItemConfigData> item_list)
{
	this->SetTuJianTreasureCount(type, param1, 0);

	std::vector<ItemID> is_set_item;
	for (std::vector<ItemConfigData>::const_iterator it = item_list.begin(); it != item_list.end(); ++it)
	{
		const ItemBase* itembase = ITEMPOOL->GetItem(it->item_id);
		if (NULL == itembase) continue;

		if (itembase->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_COLLECTION) continue;

		//已经成功检测过的不再检测
		for (std::vector<ItemID>::const_iterator zit = is_set_item.begin(); zit != is_set_item.end(); ++zit)
		{
			if (it->item_id == *zit) continue;
		}

		if (this->SetTuJianTreasureCount(type, param1, it->item_id))
		{
			is_set_item.push_back(it->item_id);
		}
	}
}

void RoleActivityTuJianTreasure::OnTujianTreasureBuyCard(ItemID card_id, int trade_way)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
	{
		return;
	}
	if (!this->IsRoleTuJianTreasureOpen())
	{
		return;
	}

	const ItemBase* itembase = ITEMPOOL->GetItem(card_id);
	if (NULL == itembase) return;
	if (ItemNamespace::ITEM_SUB_TYPE_COLLECTION == itembase->GetItemSubType())
	{
		this->SetTuJianTreasureCount(TU_JIAN_TREASURE_TYPE_MARKET, card_id, trade_way);

		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityTuJianTreasure::OnTujianTreasureBuyCard user[%d, %s], level[%d], act_type[%d], card_id[%d] trade_way[%d]",
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE,
			card_id, trade_way);
	}
}

void RoleActivityTuJianTreasure::CheckTuJianTreasureOpen(int role_level)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTuJianTreasureConfig * config = static_cast<const RandActivityTuJianTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
	if (NULL == config)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
	{
		return;
	}

	if (0 < m_param.ra_tu_jian_treasure_end_timestamp)		//活动已激活
	{
		return;
	}

	const RATuJianTreasureOtherCfg & other_cfg = config->GetOtherCfg();

	unsigned int nowtime = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	if (role_level >= other_cfg.open_grade)
	{
		m_param.ra_tu_jian_treasure_end_timestamp = nowtime + other_cfg.open_time * SECOND_PER_HOUR;
		this->OnRATuJianTreasureSetData();

		this->SetRoleDataChange();
	}

	this->SendRATuJianTreasureInfo();
}

void RoleActivityTuJianTreasure::OnSetLoginDay()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE)) return;

	if (!this->IsRoleTuJianTreasureOpen()) return;

	unsigned int day_id = EngineAdapter::Instance().DayID();
	if (day_id == m_param.ra_tu_jian_treasure_day_id) return;

	m_param.ra_tu_jian_treasure_login_day += 1;
	m_param.ra_tu_jian_treasure_day_id = day_id;
	this->SendRATuJianTreasureInfo();
	this->SetRoleDataChange();
}

void RoleActivityTuJianTreasure::SendRATuJianTreasureInfo()
{
	const RandActivityTuJianTreasureConfig * config = static_cast<const RandActivityTuJianTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	Protocol::SCRATuJianTreasureInfo info;
	info.end_timestamp = m_param.ra_tu_jian_treasure_end_timestamp;
	info.login_day = m_param.ra_tu_jian_treasure_login_day;
	info.day_fetch_flag = m_param.ra_tu_jian_treasure_day_fetch_flag;
	info.count = 0;

	for (int i = 0; i < RA_TU_JIAN_TREASURE_MAXNUM; ++i)
	{
		const RATuJianTreasureRewardCfg * cfg = config->GetRewardCfg(i);
		if (NULL == cfg) continue;

		TuJianTreasureItem & item = m_param.ra_tu_jian_treasure_item[i];
		if (TU_JIAN_TREASURE_TYPE_BEGIN >= item.type || cfg->type != item.type || cfg->param1 != item.param1 || cfg->param2 != item.param2)
		{
			continue;
		}

		info.info[info.count].seq = i;
		info.info[info.count].count = item.count;
		if (m_param.ra_tu_jian_treasure_fetch_flag.IsBitSet(i))
		{
			info.info[info.count].is_fetch = 1;
		}
		else
		{
			info.info[info.count].is_fetch = 0;
		}
		info.count++;
	}

	int length = sizeof(info) - (sizeof(info.info[0]) * (RA_TU_JIAN_TREASURE_MAXNUM - info.count));
	m_role_module_mgr->NetSend(&info, length);
}

void RoleActivityTuJianTreasure::OnRATuJianTreasureFetchDay(int seq)
{
	if (0 > seq)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RandActivityTuJianTreasureConfig * config = static_cast<const RandActivityTuJianTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RATuJianTreasureLoginCfg * cfg = config->GetLoginCfg(seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.ra_tu_jian_treasure_day_fetch_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (cfg->sign_day > m_param.ra_tu_jian_treasure_login_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_TU_JIAN_TREASURE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_tu_jian_treasure_day_fetch_flag.SetBit(seq);
	this->SendRATuJianTreasureInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRATuJianTreasureFetchDayReward user[%d, %s], level[%d], act_type[%d], seq[%d] fetch_item[id:%d num:%d] fetch_day[%d] login_day[%d] end_timestamp[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE,
		seq, cfg->item.item_id, cfg->item.num, cfg->sign_day, m_param.ra_tu_jian_treasure_login_day, m_param.ra_tu_jian_treasure_end_timestamp);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityTuJianTreasure::OnRATuJianTreasureFetch(int seq)
{
	if (0 > seq || RA_TU_JIAN_TREASURE_MAXNUM <= seq)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RandActivityTuJianTreasureConfig * config = static_cast<const RandActivityTuJianTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RATuJianTreasureRewardCfg * cfg = config->GetRewardCfg(seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (0 >= cfg->type)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	TuJianTreasureItem & item = m_param.ra_tu_jian_treasure_item[seq];
	if ((char)cfg->type != item.type || cfg->param1 != item.param1 || cfg->param2 != item.param2)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.ra_tu_jian_treasure_fetch_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (cfg->param3 > item.count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_TU_JIAN_TREASURE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_tu_jian_treasure_fetch_flag.SetBit(seq);
	this->SendRATuJianTreasureInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRATuJianTreasureFetch user[%d, %s], level[%d], act_type[%d], seq[%d] fetch_item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE, seq, cfg->item.item_id, cfg->item.num);

	this->OnParticipateRA(__FUNCTION__);
}

bool RoleActivityTuJianTreasure::SetTuJianTreasureCount(int type, int param1, int param2, int count)
{
	if (TU_JIAN_TREASURE_TYPE_BEGIN >= type || TU_JIAN_TREASURE_TYPE_TYPE_MAX <= type)
	{
		return false;
	}

	bool is_add = false;
	for (int i = 0; i < RA_TU_JIAN_TREASURE_MAXNUM; ++i)
	{
		TuJianTreasureItem & item = m_param.ra_tu_jian_treasure_item[i];
		if (type != item.type) continue;

		switch (type)
		{
		case TU_JIAN_TREASURE_TYPE_WLID_BOSS:
		case TU_JIAN_TREASURE_TYPE_FB:
			{
				if (param1 == item.param1)
				{
					if (param2 == item.param2 || 0 == item.param2)
					{
						item.count += count;
						is_add = true;
					}
				}
			}
			break;

		case TU_JIAN_TREASURE_TYPE_MARKET:
			{
				if (param1 == item.param1 || 0 == item.param1)
				{
					if (param2 == item.param2 || 2 == item.param2)
					{
						item.count += count;
						is_add = true;
					}
				}
			}
			break;

		default:
			return false;
			break;
		}
	}

	if (is_add)
	{
		this->SendRATuJianTreasureInfo();

		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityTuJianTreasure::SetTuJianTreasureCount user[%d, %s], level[%d], act_type[%d], type[%d] param1[%d] param2[%d] count[%d]",
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE,
			type, param1, param2, count);
	}

	this->SetRoleDataChange();
	return is_add;
}

void RoleActivityTuJianTreasure::OnRATuJianTreasureSetData()
{
	const RandActivityTuJianTreasureConfig * config = static_cast<const RandActivityTuJianTreasureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
	if (NULL == config)
	{
		return;
	}

	for (int i = 0; i < RA_TU_JIAN_TREASURE_MAXNUM; ++i)
	{
		const RATuJianTreasureRewardCfg * cfg = config->GetRewardCfg(i);
		if (NULL == cfg) continue;

		TuJianTreasureItem & item = m_param.ra_tu_jian_treasure_item[i];
		item.param1 = cfg->param1;
		item.param2 = cfg->param2;
		item.type = (char)cfg->type;
	}

	this->SetRoleDataChange();
}
