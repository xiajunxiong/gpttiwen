#include "roleactivitysports.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysportsconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "protocol/randactivity/msgrasports.hpp"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivitySports::RoleActivitySports(int type)
	:RoleActivity(type)
{

}

RoleActivitySports::~RoleActivitySports()
{

}

void RoleActivitySports::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASportsParam));
}

void RoleActivitySports::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RASportsParam));
}

void RoleActivitySports::OnLoginSendInfo()
{
	this->SendRASportsHistroyInfo();
}

void RoleActivitySports::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_SPORTS != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_sports_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_sports_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivitySports::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SPORTS))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SPORTS);

	switch (raor->opera_type)
	{
	case Protocol::RA_SPORTS_OPERA_TYPE_INFO:
		{
			this->SendRASportsHistroyInfo();
		}
		break;
	case Protocol::RA_SPORTS_OPERA_TYPE_GET_REWARD:
		{
			this->GetRaSportHistroyReward(raor->param_1);
		}
		break;
	}
}

void RoleActivitySports::OnRaSportsHistroyEnd()
{
	const RandActivitySportsConfig * config = static_cast<const RandActivitySportsConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPORTS));
	if (NULL == config)
	{
		return;
	}

	const std::map<int, RASportsHistoryCfg>* cfg_list = config->GetSportsHistoryList();
	int user_pos = m_param.ra_sports_histroy_top;
	if (user_pos == -1)
	{
		//玩家有可能是活动开启之后一次没打过的
		//检查玩家排名
		user_pos = ColosseumManager::Instance().GetRoleRank(m_role_module_mgr->GetUid());
	}
	if (cfg_list != NULL)
	{
		for (std::map<int, RASportsHistoryCfg>::const_iterator history_it = cfg_list->begin(); history_it != cfg_list->end(); history_it++)
		{
			if ((m_param.ra_sports_histroy_reward & (unsigned short)(1 << history_it->first)) > 0)continue;

			if (history_it->second.historical_ranking < user_pos + 1)
			{
				continue;
			}

			static MailContentParam contentparam; contentparam.Reset();
			if (history_it->second.itemlist.size() > 0)
			{
				int real_item = 0;
				for (std::vector<ItemConfigData>::const_iterator item_it = history_it->second.itemlist.begin(); item_it != history_it->second.itemlist.end(); item_it++)
				{
					contentparam.item_list[real_item].item_id = item_it->item_id;
					contentparam.item_list[real_item].num = item_it->num;
					contentparam.item_list[real_item].is_bind = (item_it->is_bind) ? 1 : 0;
					const ItemBase *item_base = ITEMPOOL->GetItem(item_it->item_id);
					if (item_base != NULL)contentparam.item_list[real_item].invalid_time = item_base->CalInvalidTime();
					real_item++;
				}

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_sports_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_sports_histroy_content, RAND_ACTIVITY_TYPE_SPORTS, user_pos);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
					for (int i = 0; i < real_item; i++)
					{
						gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRaSportsHistroyEnd user[%s %d] item[item_id:%d  num:%d is_bind:%d]",
							m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), contentparam.item_list[i].item_id, contentparam.item_list[i].num, contentparam.item_list[i].is_bind);
					}
				}
			}
			else
			{
				gamelog::g_log_rand_activity.printf(LL_INFO, "err RoleActivity::GetRaSportHistroyReward user[%s %d] seq[%d]", m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), history_it->first);
				return;
			}

			m_param.ra_sports_histroy_reward |= (unsigned short)1 << history_it->first;
		}

		this->SetRoleDataChange();
	}
}

void RoleActivitySports::SendRASportsHistroyInfo()
{
	static Protocol::SCSportsHistroyInfo msg;

	msg.ra_sports_histroy_top = m_param.ra_sports_histroy_top + 1;
	msg.ra_sports_histroy_reward = m_param.ra_sports_histroy_reward;

	m_role_module_mgr->NetSend((const char *)&msg, sizeof(msg));
}

void RoleActivitySports::GetRaSportHistroyReward(int seq)
{
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SPORTS);
	if ((m_param.ra_sports_histroy_reward & (unsigned short)(1 << seq)) > 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_REWARD_CLAIMED);
		return;
	}

	const RandActivitySportsConfig * config = static_cast<const RandActivitySportsConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPORTS));
	if (NULL == config)
	{
		return;
	}

	const RASportsHistoryCfg *cfg = config->GetSportsHistoryCfg(seq);
	if (cfg == NULL)return;

	int user_pos = m_param.ra_sports_histroy_top;
	if (user_pos == -1)
	{
		//玩家有可能是活动开启之后一次没打过的
		//检查玩家排名
		user_pos = ColosseumManager::Instance().GetRoleRank(m_role_module_mgr->GetUid());
	}

	if (cfg->historical_ranking < user_pos + 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPORTS_HISTORY_ERR);
		return;
	}

	static MailContentParam contentparam; contentparam.Reset();
	if (cfg->itemlist.size() > 0)
	{
		/*int real_item = 0;
		for (std::vector<ItemConfigData>::const_iterator it = cfg->itemlist.begin(); it != cfg->itemlist.end(); it++)
		{
		contentparam.item_list[real_item].item_id = it->item_id;
		contentparam.item_list[real_item].num = it->num;
		contentparam.item_list[real_item].is_bind = (it->is_bind) ? 1 : 0;
		const ItemBase *item_base = ITEMPOOL->GetItem(it->item_id);
		if (item_base != NULL)contentparam.item_list[real_item].invalid_time = item_base->CalInvalidTime();
		real_item++;
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_sports_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_sports_settlement_content, RAND_ACTIVITY_TYPE_SPORTS, user_pos);
		if (lensub > 0 && length > 0)
		{
		MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
		}*/
		if (m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->itemlist.size(), &cfg->itemlist[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)cfg->itemlist.size(), &cfg->itemlist[0], PUT_REASON_RA_SPROTS_REWARD);
		}
	}
	else
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "err RoleActivity::GetRaSportHistroyReward user[%d] seq[%d]", m_role_module_mgr->GetUid(), seq);
		return;
	}

	m_param.ra_sports_histroy_reward |= (unsigned short)1 << seq;
	this->SendRASportsHistroyInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
