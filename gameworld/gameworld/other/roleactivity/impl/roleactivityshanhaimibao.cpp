#include "roleactivityshanhaimibao.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgracoolsummer.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaimibaoconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "roleactivityshanhaimeet.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"

RoleActivityShanHaiMiBao::RoleActivityShanHaiMiBao(int type)
	:RoleActivity(type)
{
}

RoleActivityShanHaiMiBao::~RoleActivityShanHaiMiBao()
{
}

void RoleActivityShanHaiMiBao::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALotusMiBao));
}

void RoleActivityShanHaiMiBao::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RALotusMiBao));
}

void RoleActivityShanHaiMiBao::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
	this->SendInfo();
}

void RoleActivityShanHaiMiBao::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityShanHaiMiBao::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
	switch (raor->opera_type)
	{
		case Protocol::RA_SHAN_HAI_MI_BAO_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_SHAN_HAI_MI_BAO_REQ_TYPE_ACTIVE_GRID:
		{	
			this->OnActiveGrid(raor->param_1, raor->param_2);
		}
		break;
		case Protocol::RA_SHAN_HAI_MI_BAO_REQ_TYPE_FETCH_ACTIVE_ALL_REWARD:
		{
			this->OnFetchAllActiveReward();
		}
		break;
	}
}

void RoleActivityShanHaiMiBao::SendInfo()
{
	Protocol::SCRAShanHaiMiBaoInfo info;
	info.reserve_sh = 0;
	info.reserve_ch = 0;
	info.is_fetch_all_reward_flag = m_param.cond_flag.IsBitSet(RA_LOUTS_MI_BAO_COND_NUM - 1);
	//info.cond_flag = m_param.cond_flag;
	info.grid_active_flag = m_param.grid_active_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityShanHaiMiBao::OnActiveGrid(int row, int column)
{
	if(row < 0 || row >= MAX_RA_LOTUS_MI_BAO_ROW_NUM || column < 0 || column >= MAX_RA_LOTUS_MI_BAO_COLUMN_NUM) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityShanHaiMiBaoConfig * config = static_cast<const RandActivityShanHaiMiBaoConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RAShanHaiMiBaoCfg * base_cfg = config->GetBaseCfg(ramgr, row, column);
	if (NULL == base_cfg)
	{
		return;
	}
	int active_grid_flag_pos = (row * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM) + column;
	if (m_param.grid_active_flag.IsBitSet(active_grid_flag_pos))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LOTUS_MI_BAO_LIGHT_UP);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(base_cfg->active_item_id, base_cfg->active_item_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	m_param.grid_active_flag.SetBit(active_grid_flag_pos);
	this->OnCheckCondFinish(row, column);
	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], row[%d], column[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO, row, column);
}

void RoleActivityShanHaiMiBao::OnFetchAllActiveReward()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityShanHaiMiBaoConfig * config = static_cast<const RandActivityShanHaiMiBaoConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (m_param.cond_flag.IsBitSet(RA_LOUTS_MI_BAO_COND_NUM - 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_HAS_GET_REWARD);
		return;
	}

	if (!this->IsFinishCondActive(RA_LOTUS_MI_BAO_CONDITION_TYPE_ALL))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_PARAM_NOT_ADV);
		return;
	}
	const RAShanMiBaoFinishCondRewardCfg * cond_reward_cfg = config->GetCondRewardCfg(RA_LOTUS_MI_BAO_CONDITION_TYPE_ALL, MAX_RA_LOTUS_MI_BAO_GRID_NUM);
	if (NULL == cond_reward_cfg)
	{
		return;
	}
	std::vector<ItemConfigData> reward_list;
	const RAShanHaiMiBaoRewardCfg * reward_cfg = config->GetRandRewardCfg(cond_reward_cfg->reward_group_id);
	if (NULL != reward_cfg)
	{
		reward_list.assign(reward_cfg->reward_list.begin(), reward_cfg->reward_list.end());
	}
	if (cond_reward_cfg->is_has_fixe_reward)
	{
		reward_list.push_back(cond_reward_cfg->fixe_reward);
	}
	if (!reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_list.size(), &reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_SHAN_HAI_MI_BAO_REWARD))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
		}
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list.size(), &reward_list[0], GET_REASON_ACTVITIY_REWARD, RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
	}
	
	m_param.cond_flag.SetBit(RA_LOUTS_MI_BAO_COND_NUM - 1);
	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);
	gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);

	{//山海初遇
		RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_LOTUS_MI_BAO_ACT_PAGE, 1);
		}
	}

	//活动-山海相逢
	RoleActivityShanHaiMeet * shan_hai_meet = static_cast<RoleActivityShanHaiMeet *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
	if (NULL != shan_hai_meet)
	{
		shan_hai_meet->OnFinishType(RA_SHAN_HAI_MEET_ACT_TYPE_8, 1);
	}
}

void RoleActivityShanHaiMiBao::OnCheckCondFinish(int row, int column)
{
	if (row < 0 || row >= MAX_RA_LOTUS_MI_BAO_ROW_NUM || column < 0 || column >= MAX_RA_LOTUS_MI_BAO_COLUMN_NUM) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityShanHaiMiBaoConfig * config = static_cast<const RandActivityShanHaiMiBaoConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO));
	if (NULL == config)
	{
		return;
	}
	std::vector<ItemConfigData> notice_list;
	const RAShanMiBaoFinishCondRewardCfg * row_cond_reward_cfg = config->GetCondRewardCfg(RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW, row + 1);
	if (NULL != row_cond_reward_cfg)
	{
		int row_cond_pos = (RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW - 1) * RA_LOTUS_MI_BAO_COND_BASE_NUM + row;
		if (row_cond_pos >= 0 && row_cond_pos < RA_LOUTS_MI_BAO_COND_NUM && !m_param.cond_flag.IsBitSet(row_cond_pos) && this->IsFinishCondActive(RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW, row))
		{
			const RAShanHaiMiBaoRewardCfg * reward_cfg = config->GetRandRewardCfg(row_cond_reward_cfg->reward_group_id);
			std::vector<ItemConfigData> reward_list;
			if (NULL != reward_cfg && !reward_cfg->reward_list.empty())
			{
				reward_list.assign(reward_cfg->reward_list.begin(), reward_cfg->reward_list.end());
			}
			if (NULL != ITEMPOOL->GetItem(row_cond_reward_cfg->fixe_reward.item_id))
			{
				reward_list.push_back(row_cond_reward_cfg->fixe_reward);
			}
			if (!reward_list.empty())
			{
				bool is_put_succ = false;
				if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_list.size(), &reward_list[0]))
				{
					if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_SHAN_HAI_MI_BAO_REWARD))
					{
						is_put_succ = true;
					}
				}
				if (!is_put_succ)
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
				}
				notice_list.assign(reward_list.begin(), reward_list.end());
			}
			m_param.cond_flag.SetBit(row_cond_pos);
			this->SetRoleDataChange();

			//活动-山海相逢
			RoleActivityShanHaiMeet * shan_hai_meet = static_cast<RoleActivityShanHaiMeet *>
				(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
			if (NULL != shan_hai_meet)
			{
				shan_hai_meet->OnFinishType(RA_SHAN_HAI_MEET_ACT_TYPE_7, 1);
			}

			{//山海初遇
				RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
					(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_LOTUS_MI_BAO_ACT_ROW, 1);
				}
			}
			gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] cond[%d] row[%d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
				RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO, RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW, row);
		}
	}
	const RAShanMiBaoFinishCondRewardCfg * column_cond_reward_cfg = config->GetCondRewardCfg(RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN, column + 1);
	if (NULL != column_cond_reward_cfg)
	{
		int column_cond_pos = (RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN - 1) * RA_LOTUS_MI_BAO_COND_BASE_NUM + column;
		if (column_cond_pos >= 0 && column_cond_pos < RA_LOUTS_MI_BAO_COND_NUM && !m_param.cond_flag.IsBitSet(column_cond_pos) && this->IsFinishCondActive(RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN, column))
		{
			const RAShanHaiMiBaoRewardCfg * reward_cfg = config->GetRandRewardCfg(column_cond_reward_cfg->reward_group_id);
			std::vector<ItemConfigData> reward_list;
			if (NULL != reward_cfg && !reward_cfg->reward_list.empty())
			{
				reward_list.assign(reward_cfg->reward_list.begin(), reward_cfg->reward_list.end());
			}
			if (NULL != ITEMPOOL->GetItem(column_cond_reward_cfg->fixe_reward.item_id))
			{
				reward_list.push_back(column_cond_reward_cfg->fixe_reward);
			}
			if (!reward_list.empty())
			{
				bool is_put_succ = false;
				if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_list.size(), &reward_list[0]))
				{
					if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_SHAN_HAI_MI_BAO_REWARD))
					{
						is_put_succ = true;
					}
				}
				if (!is_put_succ)
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
				}
				notice_list.insert(notice_list.end(), reward_list.begin(), reward_list.end());
			}
			m_param.cond_flag.SetBit(column_cond_pos);
			this->SetRoleDataChange();
			gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] cond[%d] column[%d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
				RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO, RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN, column);
		}
	}
	if (!notice_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_ACTVITIY_REWARD, RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
	}
}

bool RoleActivityShanHaiMiBao::IsFinishCondActive(int cond_type, int param1)
{
	switch (cond_type)
	{
		case RA_LOTUS_MI_BAO_CONDITION_TYPE_ROW:
		{
			for (int i = 0; i < MAX_RA_LOTUS_MI_BAO_COLUMN_NUM; i++)
			{
				int active_grid_flag_pos = (param1 * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM) + i;
				if (!m_param.grid_active_flag.IsBitSet(active_grid_flag_pos))
				{
					return false;
				}
			}
		}
		break;
		case RA_LOTUS_MI_BAO_CONDITION_TYPE_COLUMN:
		{
			for (int i = 0; i < MAX_RA_LOTUS_MI_BAO_ROW_NUM; i++)
			{
				int active_grid_flag_pos = (i * MAX_RA_LOTUS_MI_BAO_ROW_NUM) + param1;
				if (!m_param.grid_active_flag.IsBitSet(active_grid_flag_pos))
				{
					return false;
				}
			}
		}
		break;
		case RA_LOTUS_MI_BAO_CONDITION_TYPE_ALL:
		{
			for (int i = 0; i < MAX_RA_LOTUS_MI_BAO_GRID_NUM; i++)
			{
				if (!m_param.grid_active_flag.IsBitSet(i))
				{
					return false;
				}
			}
		}
		break;
	default:
		return false;
	}
	
	return true;
}
