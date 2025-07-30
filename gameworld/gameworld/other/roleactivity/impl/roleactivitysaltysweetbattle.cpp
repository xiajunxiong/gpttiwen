#include "roleactivitysaltysweetbattle.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/randactivity/randactivityimpl/randactivitysaltysweetbattle.hpp"
#include "config/randactivityconfig/impl/randactivitysaltysweetbattleconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"

RoleActivitySaltySweetBattle::RoleActivitySaltySweetBattle(int type) :RoleActivity(type)
{
}

RoleActivitySaltySweetBattle::~RoleActivitySaltySweetBattle()
{
}

void RoleActivitySaltySweetBattle::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASaltySweetBattleParam));
}

void RoleActivitySaltySweetBattle::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RASaltySweetBattleParam));
}

void RoleActivitySaltySweetBattle::OnRAClose()
{
	this->GetReward();
}

void RoleActivitySaltySweetBattle::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivitySaltySweetBattle::OnLoginCheck()
{
	this->GetReward();
}

void RoleActivitySaltySweetBattle::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	this->RASendInfo();
}


void RoleActivitySaltySweetBattle::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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
		this->GetReward();
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivitySaltySweetBattle::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 0://请求下发
		{
			this->RASendInfo();
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
			if (NULL == ramgr) return;

			RandActivitySaltySweetBattle * activity = (RandActivitySaltySweetBattle *)ramgr->ForceGetRandActivity(m_role_activity_type);
			if (NULL == activity) return;

			activity->RASendGameInfo(m_role_module_mgr->GetRole());
		}
		break;
	case 1:	// 投票 param1 投票类型， param2 投票值
		{
			const RandActivitySaltySweetBattleConfig * config = static_cast<const RandActivitySaltySweetBattleConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE));
			if (NULL == config)return;

			{//投票消耗，发放奖励，累计
				const SaltySweetOtherCfg & other_cfg = config->GetOtherCfg();
				if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.consume_item, raor->param_2, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
					return;
				}
				if (m_param.vote_type == 0)
				{
					m_param.vote_type = raor->param_1;
				}
				else
				{
					if (m_param.vote_type != raor->param_1)return;
				}
				int real_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
				const SaltySweetBaseCfg * base_cfg = config->GetBaseCfg(real_open_day);
				if (NULL == base_cfg)return;
				const SaltySweetVoteCfg * vote_cfg = config->GetVoteCfg(base_cfg->vote_reward_group);
				if (NULL == vote_cfg)return;
				if (!m_role_module_mgr->GetKnapsack()->Put(vote_cfg->reward, PUT_REASON_RA_SALTY_SWEET_BATTLE))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&vote_cfg->reward, 1, SNED_MAIL_TYPE_DEFAULT);
				}
			}

			if (CrossConfig::Instance().IsHiddenServer())
			{
				//投票回原服
				crossgameprotocal::HiddenGameSaltySweetBattleVoteReq req;
				req.role_uid = m_role_module_mgr->GetUid();
				req.add_type = raor->param_1;
				req.add_value = raor->param_2;
				InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(m_role_module_mgr->GetUid()).db_index, (const char*)&req, sizeof(req));
			}
			else
			{
				RandActivitySaltySweetBattle * activity = (RandActivitySaltySweetBattle *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
				if (NULL != activity)
				{
					activity->OnRoleVote(raor->param_1, raor->param_2, m_role_module_mgr->GetRole());
				}
			}
			this->RASendInfo();
		}
		break;
	case 2://领取奖励
		this->GetReward();
		break;
	}
}

void RoleActivitySaltySweetBattle::GetReward()
{
	if (m_param.reward_flag == 1 || m_param.vote_type == SALTY_SWEET_TYPE_NO)return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	RandActivitySaltySweetBattle * activity = (RandActivitySaltySweetBattle *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
	if (NULL == activity)return;
	int win_type = 0;
	int play_status = 0;
	activity->GetWinType(&win_type, &play_status);

	if (play_status != SALTY_SWEET_MOD_RESULT)
	{
		return;
	}

	const RandActivitySaltySweetBattleConfig * config = static_cast<const RandActivitySaltySweetBattleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE));
	if (NULL != config)
	{
		int real_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
		const SaltySweetBaseCfg * base_cfg = config->GetBaseCfg(real_open_day);
		if (NULL != base_cfg)
		{
			static MailContentParam contentparam; contentparam.Reset();
			if (win_type == m_param.vote_type)
			{
				SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_salty_sweet_battle_subject);
				SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_salty_sweet_battle_content);
				if ((short)base_cfg->win_reward_vec.size() > 0)
				{
					int real_count = 0;
					for (std::vector<ItemConfigData>::const_iterator it = base_cfg->win_reward_vec.begin(); it != base_cfg->win_reward_vec.end(); it++)
					{
						const ItemBase * i_b = ITEMPOOL->GetItem(it->item_id);
						if (NULL == i_b)continue;
						contentparam.item_list[real_count].is_bind = it->is_bind ? 1 : 0;
						contentparam.item_list[real_count].num = it->num;
						contentparam.item_list[real_count].invalid_time = i_b->CalInvalidTime();
						contentparam.item_list[real_count++].item_id = it->item_id;
					}
				}
			}
			else
			{
				SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_salty_sweet_battle_subject, RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
				SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_salty_sweet_battle_lose_content, RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
				if ((short)base_cfg->lose_reward_vec.size() > 0)
				{
					int real_count = 0;
					for (std::vector<ItemConfigData>::const_iterator it = base_cfg->lose_reward_vec.begin(); it != base_cfg->lose_reward_vec.end(); it++)
					{
						const ItemBase * i_b = ITEMPOOL->GetItem(it->item_id);
						if (NULL == i_b)continue;
						contentparam.item_list[real_count].is_bind = it->is_bind ? 1 : 0;
						contentparam.item_list[real_count].num = it->num;
						contentparam.item_list[real_count].invalid_time = i_b->CalInvalidTime();
						contentparam.item_list[real_count++].item_id = it->item_id;
					}
				}
			}
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			m_param.reward_flag = 1;
		}
	}
	this->RASendInfo();
}

void RoleActivitySaltySweetBattle::RASendInfo()
{
	Protocol::SCRASaltySweetBattleRoleInfo msg;
	msg.vote_type = m_param.vote_type;
	msg.reward_flag = m_param.reward_flag;
	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}