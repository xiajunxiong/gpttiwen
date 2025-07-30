#include "roleactivityyuanqispeed.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqispeedconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"

RoleActivityYuanQiSpeed::RoleActivityYuanQiSpeed(int type)
	:RoleActivity(type)
{

}

RoleActivityYuanQiSpeed::~RoleActivityYuanQiSpeed()
{

}

void RoleActivityYuanQiSpeed::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYuanQiSpeed::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYuanQiSpeed::Update(time_t now_second)
{
	if (m_param.play_end_tamp > 0 && m_param.play_end_tamp <= now_second)
	{
		this->EndPlay(Protocol::SCRAYuanQiSpeedPlayInfo::PLAY_TYPE_LOSE);
		m_param.play_end_tamp = 0;
	}
}

void RoleActivityYuanQiSpeed::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_SPEED))
	{
		return;
	}
	this->RASendInfo();
}

void RoleActivityYuanQiSpeed::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUAN_QI_SPEED != rand_act_type)
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

void RoleActivityYuanQiSpeed::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_SPEED))
	{
		return;
	}

	m_param.DayReset();
	this->OnLoginSendInfo();
}

void RoleActivityYuanQiSpeed::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 0://请求下发信息
		{
			this->RASendInfo();
		}
		break;
	case 1://开始游戏，param1 = seq 游戏难度
		{
			this->StarPlay(raor->param_1);
		}
		break;
	case 2://是否胜利 param1 （0失败/1胜利）
		{
			this->Achiment(raor->param_1 == 1);
		}
		break;
	case 3://领取首通奖励 param1 seq
		{
			this->GetFirstReward(raor->param_1);
		}
		break;
	default:
		break;
	}
}

void RoleActivityYuanQiSpeed::StarPlay(int seq)
{
	const RandActivityYuanQiSpeedConfig * config = static_cast<const RandActivityYuanQiSpeedConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUAN_QI_SPEED));
	if (NULL == config)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const YuanQiSpeedOtherCfg & other_cfg = config->GetOtherCfg();
	if (m_param.times >= other_cfg.frequency)
	{
		return;
	}

	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);

	const YuanQiSpeedReward * cfg = config->GetRewardCfg(open_day, seq);
	if (NULL != cfg)
	{
		m_param.play_end_tamp = (unsigned int)EngineAdapter::Instance().Time() + other_cfg.time;
		m_param.play_seq = seq;
		this->RASendInfo();
		this->RASendPlayInfo(Protocol::SCRAYuanQiSpeedPlayInfo::PLAY_TYPE_PLAYING);
	}
}

void RoleActivityYuanQiSpeed::Achiment(bool is_true)
{
	this->EndPlay(is_true ? Protocol::SCRAYuanQiSpeedPlayInfo::PLAY_TYPE_WIN : Protocol::SCRAYuanQiSpeedPlayInfo::PLAY_TYPE_LOSE);
	m_param.play_end_tamp = 0;
}

void RoleActivityYuanQiSpeed::GetFirstReward(int seq)
{
	const RandActivityYuanQiSpeedConfig * config = static_cast<const RandActivityYuanQiSpeedConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUAN_QI_SPEED));
	if (NULL == config)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);

	const YuanQiSpeedReward * cfg = config->GetRewardCfg(open_day, seq);
	if (NULL != cfg)
	{
		if (m_param.pass_flag.IsBitSet(seq + 1) && !m_param.reward_flag.IsBitSet(seq + 1))
		{
			if ((int)cfg->m_fisrt_reward.size() > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->PutList((int)cfg->m_fisrt_reward.size(), &cfg->m_fisrt_reward[0], PUT_REASON_RA_YUAN_QI_SPEED))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&cfg->m_fisrt_reward[0], (int)cfg->m_fisrt_reward.size(), SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);
				}
			}
			m_param.reward_flag.SetBit(seq + 1);
		}
	}
}

void RoleActivityYuanQiSpeed::EndPlay(bool is_win)
{
	const RandActivityYuanQiSpeedConfig * config = static_cast<const RandActivityYuanQiSpeedConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUAN_QI_SPEED));
	if (NULL == config)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);

	const YuanQiSpeedReward * cfg = config->GetRewardCfg(open_day, m_param.play_seq);
	if (NULL != cfg)
	{
		if (is_win)
		{
			if ((int)cfg->m_pass_reward.size() > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->PutList((int)cfg->m_pass_reward.size(), &cfg->m_pass_reward[0], PUT_REASON_RA_YUAN_QI_SPEED))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&cfg->m_pass_reward[0], (int)cfg->m_pass_reward.size(), SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);
				}
			}
			m_param.pass_flag.SetBit(m_param.play_seq + 1);
		}
		else
		{
			if ((int)cfg->m_fail_reward.size() > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->PutList((int)cfg->m_fail_reward.size(), &cfg->m_fail_reward[0], PUT_REASON_RA_YUAN_QI_SPEED))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&cfg->m_fail_reward[0], (int)cfg->m_fail_reward.size(), SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);
				}
			}
		}
		m_param.PlayEnd();
		m_param.times += 1;
		this->RASendPlayInfo(is_win ? Protocol::SCRAYuanQiSpeedPlayInfo::PLAY_TYPE_WIN : Protocol::SCRAYuanQiSpeedPlayInfo::PLAY_TYPE_LOSE);
		this->RASendInfo();
		this->OnJoinActivity();
	}
}

void RoleActivityYuanQiSpeed::RASendInfo()
{
	Protocol::SCRAYuanQiSpeedInfo msg;
	
	msg.pass_flag = m_param.pass_flag;
	msg.reward_flag = m_param.reward_flag;
	msg.today_play_times = m_param.times;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityYuanQiSpeed::RASendPlayInfo(int game_type)
{
	Protocol::SCRAYuanQiSpeedPlayInfo msg;

	msg.play_end_tamp = m_param.play_end_tamp;
	msg.play_seq = m_param.play_seq;
	msg.game_type = game_type;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityYuanQiSpeed::OnJoinActivity()
{
	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_YUANQI_JINGSU);
		}
	}
}
