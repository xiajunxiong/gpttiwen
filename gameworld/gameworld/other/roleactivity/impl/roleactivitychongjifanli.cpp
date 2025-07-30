#include "roleactivitychongjifanli.hpp"
#include "gamelog.h"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgrachongjifanli.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/rachongjifanliconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

RoleActivityChongJiFanLi::RoleActivityChongJiFanLi(int type)
	:RoleActivity(type)
{
}

RoleActivityChongJiFanLi::~RoleActivityChongJiFanLi()
{
}

void RoleActivityChongJiFanLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityChongJiFanLi::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityChongJiFanLi::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityChongJiFanLi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityChongJiFanLi::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityChongJiFanLi::OnLoginCheck()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(this->GetRoleActivityType());
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(this->GetRoleActivityType(), m_param.ra_begin_timestamp);
	if (in_activity_time)
	{
		return;
	}

	this->GiveUnFetchReward();
}

void RoleActivityChongJiFanLi::OnAddChongZhi(long long chongzhi)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	m_param.day_chongzhi_num += chongzhi;
	if (0 == m_param.is_day_meet_flag)
	{
		const RandActivityChongJiFanLiConfig * config = static_cast<const RandActivityChongJiFanLiConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL != config)
		{
			const RAChongJiFanLiRewardCfg * reward_cfg = config->GetRewardCfgByDay(ramgr, m_param.meet_cond_day_num + 1);
			if (NULL != reward_cfg && m_param.day_chongzhi_num >= reward_cfg->need_chong_num)
			{
				m_param.meet_cond_day_num += 1;
				m_param.is_day_meet_flag = 1;
			}
		}
	}
	gamelog::g_log_ra_chong_ji_fan_li.printf(LL_INFO, "%s role[%d,%s] day_chongzhi[%d] meet_day[%d] day_flag[%d]", __FUNCTION__, 
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.day_chongzhi_num, m_param.meet_cond_day_num, (int)m_param.is_day_meet_flag);
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityChongJiFanLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	m_param.OnDayChange();
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityChongJiFanLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
		case Protocol::RA_CHONG_JI_FAN_LI_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_CHONG_JI_FAN_LI_REQ_TYPE_FETCH:
		{
			this->OnFetchReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityChongJiFanLi::OnFetchReward(int seq)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityChongJiFanLiConfig * config = static_cast<const RandActivityChongJiFanLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RAChongJiFanLiRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, seq);
	if (NULL == reward_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.reward_fetch_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (m_param.meet_cond_day_num < reward_cfg->day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	std::vector<ItemConfigData> reward_list(reward_cfg->common_reward_list);
	if (NULL != ITEMPOOL->GetItem(reward_cfg->grand_prize.item_id))
	{
		reward_list.push_back(reward_cfg->grand_prize);
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_list))
	{
		if (m_role_module_mgr->GetKnapsack()->PutVec(reward_list, PUT_REASON_CHONG_JI_FAN_LI))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, m_role_activity_type);
	}

	m_param.reward_fetch_flag.SetBit(seq);
	this->SetRoleDataChange();
	this->SendInfo();

	gamelog::g_log_ra_chong_ji_fan_li.printf(LL_INFO, "%s role[%d,%s] seq[%d] day_chongzhi[%d] meet_day[%d] day_flag[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, m_param.day_chongzhi_num, m_param.meet_cond_day_num, (int)m_param.is_day_meet_flag);
}

void RoleActivityChongJiFanLi::SendInfo()
{
	Protocol::SCRAChongJiFanLiInfo info;
	info.is_day_meet_flag = m_param.is_day_meet_flag;
	info.reserve_ch = 0;
	info.reserve_sh = 0;
	info.meet_cond_day_num = m_param.meet_cond_day_num;
	info.day_chongzhi_num = m_param.day_chongzhi_num;
	info.reward_fetch_flag = m_param.reward_fetch_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityChongJiFanLi::GiveUnFetchReward()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityChongJiFanLiConfig * config = static_cast<const RandActivityChongJiFanLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAChongJiFanLiSectionRewardCfg * section_cfg = config->GetRewardSectionCfg(ramgr);
	if (NULL == section_cfg)
	{
		return;
	}

	std::vector<ItemConfigData> reward_list;
	for (int i = 0; i < (int)section_cfg->reward_list.size() && i < MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM; ++i)
	{
		if (m_param.reward_fetch_flag.IsBitSet(i))
		{
			continue;
		}

		if (m_param.meet_cond_day_num < section_cfg->reward_list[i].day)
		{
			break;
		}
		
		reward_list.insert(reward_list.end(), section_cfg->reward_list[i].common_reward_list.begin(), section_cfg->reward_list[i].common_reward_list.end());
		if (NULL != ITEMPOOL->GetItem(section_cfg->reward_list[i].grand_prize.item_id))
		{
			reward_list.push_back(section_cfg->reward_list[i].grand_prize);
		}
		m_param.reward_fetch_flag.SetBit(i);
		gamelog::g_log_ra_chong_ji_fan_li.printf(LL_INFO, "%s role[%d,%s] seq[%d] day_chongzhi[%d] meet_day[%d] day_flag[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), i, m_param.day_chongzhi_num, m_param.meet_cond_day_num, (int)m_param.is_day_meet_flag);
	}
	this->SetRoleDataChange();

	if (reward_list.empty())
	{
		return;
	}

	m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_ACTIVITY_END_REWARD_REISSUE, false, m_role_activity_type);

	gamelog::g_log_ra_chong_ji_fan_li.printf(LL_INFO, "%s role[%d,%s] succ", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());
}
