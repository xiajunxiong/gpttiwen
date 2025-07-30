#include "roleactivityhappyup.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityhappyupconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgrahappyup.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityHappyUp::RoleActivityHappyUp(int type)
	:RoleActivity(type)
{

}

RoleActivityHappyUp::~RoleActivityHappyUp()
{

}

void RoleActivityHappyUp::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAHappyUpParam));
}

void RoleActivityHappyUp::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAHappyUpParam));
}

void RoleActivityHappyUp::OnLoginSendInfo()
{
	this->SendRAHappyUpInfo();
}

void RoleActivityHappyUp::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (old_dayid == now_dayid || !ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HAPPY_UP))
	{
		return;
	}

	const RandActivityHappyUpConfig * config = static_cast<const RandActivityHappyUpConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HAPPY_UP));
	if (NULL == config)
	{
		return;
	}

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_HAPPY_UP);

	const RAHappyUpSectionCfg * section_cfg = config->GetHappyUpSectionCfg(ramgr);
	if (NULL == section_cfg) return;

	for (int i = 0; i < (int)section_cfg->item.size() && i < MAX_HAPPY_UP_ITEM_SEQ_NUM; i++)
	{
		if (section_cfg->item[i].limit_type == HAPPY_UP_LIMIT_TYPE_TODAY)
		{
			m_param.ra_happy_up_exchange_count[i] = 0;
		}
	}

	this->SendRAHappyUpInfo();

	this->SetRoleDataChange();
}

void RoleActivityHappyUp::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_HAPPY_UP != rand_act_type)
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


	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_happy_up_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_happy_up_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		memset(m_param.ra_happy_up_exchange_count, 0, sizeof(m_param.ra_happy_up_exchange_count));

		this->SetRoleDataChange();
	}
}

void RoleActivityHappyUp::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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
	case Protocol::RA_HAPPY_UP_OPERA_TYPE_INFO:
		{
			this->SendRAHappyUpInfo();
		}
		break;
	case Protocol::RA_HAPPY_UP_OPERA_TYPE_EXCHANGE_ITEM:
		{
			this->OnRAHappyUpExchange(raor->param_1, raor->param_2);
		}
		break;
	}

}

void RoleActivityHappyUp::SendRAHappyUpInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityHappyUpConfig * config = static_cast<const RandActivityHappyUpConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HAPPY_UP));
	if (NULL == config)
	{
		return;
	}

	Protocol::SCRAHappyUpInfo info;
	info.count = config->GetHappyUpSectionItemCount(ramgr);
	if (info.count == 0) return;
	memcpy(info.happy_up_exchange_count, m_param.ra_happy_up_exchange_count, sizeof(info.happy_up_exchange_count));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityHappyUp::OnRAHappyUpExchange(int item_seq, int count)
{
	if (count <= 0) return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityHappyUpConfig * config = static_cast<const RandActivityHappyUpConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HAPPY_UP));
	if (NULL == config)
	{
		return;
	}

	const RAHappyUpItemCfg * cfg = config->GetHappyUpItemCfg(ramgr, item_seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	int item_count = m_role_module_mgr->GetKnapsack()->Count(cfg->consume_item_id);
	if (item_count < cfg->consume_item_count * count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CONVERT_SHOP_RESOURCE_NOT_ENOUGH);
		return;
	}

	int max_count = 0;
	switch (cfg->limit_type)
	{
	case HAPPY_UP_LIMIT_TYPE_BUY_COUNT:
		{
			max_count = cfg->buy_times;
		}
		break;
	case HAPPY_UP_LIMIT_TYPE_TODAY:
		{
			max_count = cfg->today_times;
		}
		break;
	default:
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (m_param.ra_happy_up_exchange_count[item_seq] + count > max_count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CONVERT_SHOP_BUY_TIMES_LIMIT);
		return;
	}

	ItemConfigData item = cfg->exchange_item;
	item.num *= count;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->consume_item_id, cfg->consume_item_count * count, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_HAPPY_UP_EXCHANGE))
	{
		//兑换失败,通过邮件返还道具
		m_role_module_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		return;
	}
	if (cfg->hearsay == 1 && !CrossConfig::Instance().IsHiddenServer())
	{
		const ItemBase * item_base = ItemPool::Instance()->GetItem(item.item_id);
		if (NULL != item_base)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_happy_up_hearsay_content,
				m_role_module_mgr->GetRole()->GetName(), cfg->consume_item_count * count, item_base->GetItemId(), RAND_ACTIVITY_TYPE_HAPPY_UP);
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
	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAHappyUpExchange user[%d, %s], level[%d], type[%d] item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_UP, item.item_id, item.num);

	m_param.ra_happy_up_exchange_count[item_seq] += count;
	this->SendRAHappyUpInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
