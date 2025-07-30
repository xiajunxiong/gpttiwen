#include "roleactivityintegraldraw.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityintegraldrawconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityintegraldraw.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "servercommon/string/gameworldstr.h"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimeet.hpp"

RoleActivityIntegralDraw::RoleActivityIntegralDraw(int type) :RoleActivity(type)
{
}

RoleActivityIntegralDraw::~RoleActivityIntegralDraw()
{
}

void RoleActivityIntegralDraw::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAIntegralDrawParam));
}

void RoleActivityIntegralDraw::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAIntegralDrawParam));
}

void RoleActivityIntegralDraw::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW))
		return;

	this->RASendInfo();
}

void RoleActivityIntegralDraw::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_INTEGRAL_DRAW != rand_act_type)
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

void RoleActivityIntegralDraw::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->RADraw();
		}
		break;
	case 2:
		{
			this->RAFetchReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityIntegralDraw::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW))
	{
		return;
	}

	m_param.draw_times = 0;

	this->RASendInfo();
}

void RoleActivityIntegralDraw::RASendInfo()
{
	Protocol::SCRAIntegralDrawRoleInfo info;
	info.draw_times = m_param.draw_times;
	memcpy(info.reward_times, m_param.reward_list, sizeof(info.reward_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityIntegralDraw::RADraw()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityIntegralDraw * act = (RandActivityIntegralDraw *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
	if (NULL == act) return;

	if (!act->IsGameOpen())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_IS_END);
		return;
	}

	const RandActivityIntegralDrawConfig * config = static_cast<const RandActivityIntegralDrawConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.draw_times >= config->GetOtherCfg().day_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}

	char line_color[MAX_RA_INTEGRAL_DRAW_LINE_NUM];				// 每一行抽中的花色
	memset(line_color, 0, sizeof(line_color));

	char color_num[MAX_RA_INTEGRAL_DRAW_COLOR_NUM];				// 每种花色的数量
	memset(color_num, 0, sizeof(color_num));

	for (int i = 0; i < MAX_RA_INTEGRAL_DRAW_LINE_NUM; i++)
	{
		int color = rand() % MAX_RA_INTEGRAL_DRAW_COLOR_NUM;
		line_color[i] = color;
		color_num[color] += 1;
	}

	int max_color = 0;
	for (int i = 0; i < MAX_RA_INTEGRAL_DRAW_COLOR_NUM; i++)
	{
		if (color_num[i] > max_color) max_color = color_num[i];
	}

	const RAIntegralDrawRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, max_color - 1);
	if (NULL == reward_cfg)
		return;
	
	m_param.draw_times += 1;
	m_param.reward_list[reward_cfg->seq] += 1;

	{
		Protocol::SCRAIntegralDrawResult info;
		memcpy(info.color, line_color, sizeof(info.color));
				 
		m_role_module_mgr->NetSend(&info, sizeof(info));
	}

	if (reward_cfg->is_borcast)
	{
		std::string broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_integral_draw_broadcast,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName());

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", broadcast_str.c_str());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] draw color_num[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_INTEGRAL_DRAW,
		max_color);

	{//山海初遇
		RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_INTEGRAL_DRAW, 1);
		}
	}

	//活动-山海相逢
	RoleActivityShanHaiMeet * shan_hai_meet = static_cast<RoleActivityShanHaiMeet *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
	if (NULL != shan_hai_meet)
	{
		shan_hai_meet->OnFinishType(RA_SHAN_HAI_MEET_ACT_TYPE_4, 1);
	}
}

void RoleActivityIntegralDraw::RAFetchReward(int index)
{
	if (index < 0 || index >= MAX_RA_INTEGRAL_DRAW_LINE_NUM)
		return;

	if (m_param.reward_list[index] <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_GRID_NO_REWARD);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityIntegralDrawConfig * config = static_cast<const RandActivityIntegralDrawConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAIntegralDrawRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, index);
	if (NULL == reward_cfg)
		return;

	int time = m_param.reward_list[index];
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->reward_item.size(), &reward_cfg->reward_item[0], false))
	{
		for (int i = 0; i < time; i++)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_item[0], reward_cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_INTEGRAL_DRAW);
		}
	}
	else
	{
		std::vector<ItemConfigData> item_list;
		for (int i = 0; i <  reward_cfg->reward_item.size(); i++)
		{
			ItemConfigData item(reward_cfg->reward_item[i].item_id, reward_cfg->reward_item[i].is_bind, reward_cfg->reward_item[i].num * time);

			item_list.push_back(item);
		}

		m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_RA_INTEGRAL_DRAW);

		m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(item_list, GET_REASON_ACTVITIY_REWARD, RAND_ACTIVITY_TYPE_INTEGRAL_DRAW);
	}

	m_param.reward_list[index] = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] index[%d] fetch_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_INTEGRAL_DRAW,
		index, time);
}
