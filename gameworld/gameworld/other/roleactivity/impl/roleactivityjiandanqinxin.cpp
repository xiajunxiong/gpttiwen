#include "roleactivityjiandanqinxin.hpp"

#include "engineadapter.h"

#include "battle/battle_manager_local.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivitybonusconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityjiandanqinxinconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "protocol/randactivity/msgrajiandanqinxin.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityJianDanQinXin::RoleActivityJianDanQinXin(int type)
	:RoleActivity(type)
{

}

RoleActivityJianDanQinXin::~RoleActivityJianDanQinXin()
{

}

void RoleActivityJianDanQinXin::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAJianDanQinXinParam));
}

void RoleActivityJianDanQinXin::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAJianDanQinXinParam));
}

void RoleActivityJianDanQinXin::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	this->SendRAJianDanQinXinInfo();
}

void RoleActivityJianDanQinXin::OnLoginSendInfo()
{
	this->SendRAJianDanQinXinInfo();
}

void RoleActivityJianDanQinXin::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_jiandanqinxin_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_jiandanqinxin_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_jiandanqinxin_flag.Reset();
		this->OnCheckInitBonus();

		this->SetRoleDataChange();
	}
}

bool RoleActivityJianDanQinXin::IsFinishBonusCond(int layer_type)
{
	if (layer_type < JIAN_DAN_QIN_XIN_TYPE_COMMON || layer_type >= JIAN_DAN_QIN_XIN_TYPE_MAX) return false;
	bool if_all_finish = true;
	for (int i = layer_type * MAX_RA_JIANDNAQINXIN_DAY_NUM; i < (layer_type + 1) * MAX_RA_JIANDNAQINXIN_DAY_NUM; i++)
	{
		if (!m_param.ra_jiandanqinxin_flag.IsBitSet(i)) 
		{
			if_all_finish = false;
			break;
		}
	}

	return if_all_finish;
}

void RoleActivityJianDanQinXin::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN);

	switch (raor->opera_type)
	{
	case Protocol::RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_INFO:
		{
			this->SendRAJianDanQinXinInfo();
		}
		break;
	case Protocol::RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_BATTLE_REQ:
		{
			this->OnRAJianDanQinXinBattle(raor->param_1);			// param1 --- monster_group_id
		}
		break;
	case Protocol::RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_BATTLE_RESULT:
		{
			this->OnRAJianDanQinXinReward(raor->param_1);			// param1 -- monster_group_id
		}
		break;
	}
}

void RoleActivityJianDanQinXin::SendRAJianDanQinXinInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	Protocol::SCRAJianDanQinxinInfo info;
	info.open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN);
	info.flag = m_param.ra_jiandanqinxin_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityJianDanQinXin::OnRAJianDanQinXinBattle(int monster_group_id)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityJianDanQinXinConfig * config = static_cast<const RandActivityJianDanQinXinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAJianDanQinXinRewardCfg * cfg = config->GetLayerCfgByMonsterId(ramgr, monster_group_id);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role) return;

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN);
	if (cfg->checkpoint_layer > open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LAYER_NOT_OPEN);
		return;
	}

	Team * team = role->GetMyTeam();
	if (team == NULL)
	{
		BattleManagerLocal::GetInstance().StartMonsterFightReq(role, cfg->monster_group_id, BATTLE_MODE_JIAN_DAN_QIN_XIN, 0);
	}
	else
	{
		BattleManagerLocal::GetInstance().StartTeamMonsterFightReq(role, cfg->monster_group_id, BATTLE_MODE_JIAN_DAN_QIN_XIN, 0);
	}
}

void RoleActivityJianDanQinXin::OnRAJianDanQinXinReward(int monster_group_id)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN);

	const RandActivityJianDanQinXinConfig * config = static_cast<const RandActivityJianDanQinXinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAJianDanQinXinRewardCfg * cfg = config->GetLayerCfgByMonsterId(ramgr, monster_group_id);
	if (NULL == cfg)
	{
		return;
	}
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN);
	if (cfg->checkpoint_layer > open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LAYER_NOT_OPEN);
		return;
	}

	if (m_param.ra_jiandanqinxin_flag.IsBitSet(cfg->seq))
	{
		return;
	}
	if (cfg->reward_count > 0)
	{
		bool is_notice = true;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count, &cfg->item_reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_count, &cfg->item_reward_list[0], PUT_REASON_JIAN_DAN_QIN_XIN))
			{
				is_notice = false;
			}
		}

		if (is_notice)
		{
			//玩家背包空间不足,改由邮件发送奖励
			m_role_module_mgr->GetKnapsack()->SendMail(&cfg->item_reward_list[0], (int)cfg->reward_count, SNED_MAIL_TYPE_DEFAULT, true);
		}
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], type[%d] item_list_num[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN, (int)cfg->reward_count);
	m_param.ra_jiandanqinxin_flag.SetBit(cfg->seq);

/*	const RABonusCfg * bonus_cfg = LOGIC_CONFIG->GetRandActivityBonusConfig()->GetBonusCfg(m_role_activity_type, cfg->type);
	if (NULL != bonus_cfg && 0 != bonus_cfg->type_item)
	{
		if(this->IsPassLayerByLayerType(cfg->type))
		{
			m_role_module_mgr->GetRoleActivityManager()->SetBonusFlag(bonus_cfg->seq, 1);
		}
	}
*/
	this->SendRAJianDanQinXinInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
