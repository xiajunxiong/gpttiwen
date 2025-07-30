#include "roleactivitytruntable.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitytruntableconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgratruntable.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

static const int RA_TRUN_TABLE_NEED_KILL_BOSS_NUM = 5;

RoleActivityTrunTable::RoleActivityTrunTable(int type)
	:RoleActivity(type)
{

}

RoleActivityTrunTable::~RoleActivityTrunTable()
{

}

void RoleActivityTrunTable::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RATrunTableParam));
}

void RoleActivityTrunTable::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RATrunTableParam));
}

void RoleActivityTrunTable::OnLoginSendInfo()
{
	this->SendRATurnTableInfo();
}

void RoleActivityTrunTable::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	unsigned int now = static_cast<unsigned int>(now_second);

	if (now > ramgr->GetRandActivityBeginTime(RAND_ACTIVITY_TYPE_TRUN_TABLE) && now < ramgr->GetRandActivityEndTime(RAND_ACTIVITY_TYPE_TRUN_TABLE) &&
		m_param.ra_turn_table_begin_timestamp == 0)
	{
		m_param.ra_turn_table_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}

	if ((now > ramgr->GetRandActivityEndTime(RAND_ACTIVITY_TYPE_TRUN_TABLE) || now < ramgr->GetRandActivityBeginTime(RAND_ACTIVITY_TYPE_TRUN_TABLE)) &&
		m_param.ra_turn_table_begin_timestamp != 0)
	{
		m_param.ra_turn_table_begin_timestamp = 0;
		this->SetRoleDataChange();
	}

	if (now > m_param.ra_turn_table_begin_timestamp && now > m_param.ra_free_turn_table_after_timestamp && m_param.ra_turn_table_begin_timestamp != 0 &&
		m_param.ra_free_turn_table_after_timestamp > 0 && now < ramgr->GetRandActivityEndTime(RAND_ACTIVITY_TYPE_TRUN_TABLE))
	{
		m_param.ra_has_free_turn_table_num++;
		m_param.ra_free_turn_table_after_timestamp = 0;
		this->SetRoleDataChange();
		this->SendRATurnTableInfo();
	}
	if (now > m_param.ra_turn_table_begin_timestamp && m_param.ra_has_free_turn_table_num == 0 && m_param.ra_turn_table_begin_timestamp != 0 &&
		m_param.ra_free_turn_table_after_timestamp == 0 && now < ramgr->GetRandActivityEndTime(RAND_ACTIVITY_TYPE_TRUN_TABLE))
	{
		const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();
		m_param.ra_free_turn_table_after_timestamp = now + other_cfg.extra_turntable_time;
		this->SetRoleDataChange();
		this->SendRATurnTableInfo();
	}
	if (m_param.ra_has_free_turn_table_num > 0)
	{
		m_param.ra_free_turn_table_after_timestamp = 0;
		this->SetRoleDataChange();
	}
}

void RoleActivityTrunTable::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_TRUN_TABLE != rand_act_type)
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


	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_turn_table_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_turn_table_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityTrunTable::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case Protocol::RA_TRUN_TABLE_OPERA_TYPE_GET_INFO:
		{
			this->SendRATurnTableInfo();
		}
		break;
	case Protocol::RA_TRUN_TABLE_OPERA_TYPE_FREE_TRUN_TABLE:
	case Protocol::RA_TRUN_TABLE_OPERA_TYPE_KILL_BOSS_TRUN_TABLE:
		{
			this->OnRAStartTurnTable(raor->opera_type);
		}
		break;
	case Protocol::RA_TRUN_TABLE_OPERA_TYPE_GET_REWARD:
		{
			this->OnRAGetTurnTableReward(raor->param_1);		//发送转盘奖励
		}
		break;
	}

}

void RoleActivityTrunTable::OnRaTurnTableAddKillBossNum()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TRUN_TABLE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TRUN_TABLE);
	m_param.kill_boss_num++;

	this->SetRoleDataChange();
}

void RoleActivityTrunTable::GMAddTimes(int free_num, int kill_boss_num)
{
	if (free_num > 0)
		m_param.ra_has_free_turn_table_num += free_num;
	if (kill_boss_num > 0)
		m_param.kill_boss_num += kill_boss_num;

	this->SendRATurnTableInfo();
	this->SetRoleDataChange();
}

void RoleActivityTrunTable::SendRATurnTableInfo()
{
	Protocol::SCRandActivityTurnTableInfo info;
	info.ra_has_free_turn_table_num = m_param.ra_has_free_turn_table_num;
	info.kill_boss_num = m_param.kill_boss_num;
	info.next_refresh_time = m_param.ra_free_turn_table_after_timestamp;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityTrunTable::OnRAStartTurnTable(int turn_type)
{
	if (( Protocol::RA_TRUN_TABLE_OPERA_TYPE_FREE_TRUN_TABLE ==  turn_type || Protocol::RA_TRUN_TABLE_OPERA_TYPE_KILL_BOSS_TRUN_TABLE == turn_type) &&
		(m_param.before_use_turn_table_type == Protocol::RA_TRUN_TABLE_OPERA_TYPE_FREE_TRUN_TABLE || m_param.before_use_turn_table_type == Protocol::RA_TRUN_TABLE_OPERA_TYPE_KILL_BOSS_TRUN_TABLE) &&
		m_param.turn_table_index != -1)
	{
		Protocol::SCRandActivityTurnTableReward info;
		info.index = m_param.turn_table_index;
		m_role_module_mgr->NetSend(&info, sizeof(info));
		m_param.before_use_turn_table_type = turn_type;
		return;
	}

	if (turn_type == Protocol::RA_TRUN_TABLE_OPERA_TYPE_FREE_TRUN_TABLE)
	{
		if (m_param.ra_has_free_turn_table_num <= 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
			return;
		}
	}
	else if (turn_type == Protocol::RA_TRUN_TABLE_OPERA_TYPE_KILL_BOSS_TRUN_TABLE)
	{
		if (m_param.kill_boss_num < RA_TRUN_TABLE_NEED_KILL_BOSS_NUM)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTrunTableConfig * config = static_cast<const RandActivityTrunTableConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TRUN_TABLE));
	if (NULL == config)
	{
		return;
	}

	const RATurnTableSectionCfg* cfg = config->GetTurnTableItemCfg(ramgr);
	if (cfg == NULL)
	{
		return;
	}
	const RATurnTableCfg *reward_cfg = config->GetTurnTableReward(cfg);
	if (reward_cfg == NULL)
	{
		return;
	}

	m_param.turn_table_index = reward_cfg->seq;
	m_param.before_use_turn_table_type = turn_type;

	Protocol::SCRandActivityTurnTableReward info;
	info.index = reward_cfg->seq;
	m_role_module_mgr->NetSend(&info, sizeof(info));

	this->SetRoleDataChange();
}

void RoleActivityTrunTable::OnRAGetTurnTableReward(int index)
{
	if (index != m_param.turn_table_index)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTrunTableConfig * config = static_cast<const RandActivityTrunTableConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TRUN_TABLE));
	if(NULL == config) return;

	const RATurnTableSectionCfg* cfg = config->GetTurnTableItemCfg(ramgr);
	if (cfg == NULL)
	{
		return;
	}

	const ItemConfigData & item = cfg->node_cfg[m_param.turn_table_index].item;

	bool is_true = true;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, item.num))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_RA_TURN_TABLE))
		{
			is_true = false;
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGetTurnTableReward user[%d, %s], level[%d], type[%d] item[id:%d num:%d]", m_role_module_mgr->GetRole()->GetUID(),
				m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_TRUN_TABLE, item.item_id, item.num);

		}
	}

	if (is_true)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::ONRAGetTurnTableReward by mail, role[%d,%s] item_id[%d] num[%d] reason[%s]",
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), item.item_id, item.num, __FUNCTION__);
	}

	const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();

	if (m_param.before_use_turn_table_type == Protocol::RA_TRUN_TABLE_OPERA_TYPE_FREE_TRUN_TABLE)
	{
		m_param.ra_has_free_turn_table_num--;
	}
	else if (m_param.before_use_turn_table_type == Protocol::RA_TRUN_TABLE_OPERA_TYPE_KILL_BOSS_TRUN_TABLE)
	{
		m_param.kill_boss_num -= other_cfg.kill_boss_num;
	}

	m_param.turn_table_index = -1;
	m_param.before_use_turn_table_type = 0;

	this->SendRATurnTableInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
