#include "roleactivitycollectword.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycollectwordconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgracollectword.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityCollectWord::RoleActivityCollectWord(int type)
	:RoleActivity(type)
{

}

RoleActivityCollectWord::~RoleActivityCollectWord()
{

}

void RoleActivityCollectWord::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACollectWordParam));
}

void RoleActivityCollectWord::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RACollectWordParam));
}

void RoleActivityCollectWord::OnLoginSendInfo()
{
	this->RaCollectWordSendInfo();
}

void RoleActivityCollectWord::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//集字活动重置每日下限购次数
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_collect_word_exchange_num); ++i)
	{
		m_param.ra_collect_word_exchange_num[i] = 0;
	}
	m_param.ra_collect_word_day_gift_is_fetch = 0;
	this->RaCollectWordSendInfo();

	this->SetRoleDataChange();
}

void RoleActivityCollectWord::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_COLLECT_WORD != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_collect_word_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_collect_word_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		for (int i = 0; i < ARRAY_LENGTH(m_param.ra_collect_word_exchange_num); ++i)
		{
			m_param.ra_collect_word_exchange_num[i] = 0;
		}

		for (int i = 0; i < ARRAY_LENGTH(m_param.ra_collect_word_total_exchange_num); ++i)
		{
			m_param.ra_collect_word_total_exchange_num[i] = 0;
		}
		m_param.ra_collect_word_day_gift_is_fetch = 0;

		this->SetRoleDataChange();
	}
}

void RoleActivityCollectWord::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COLLECT_WORD))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COLLECT_WORD);

	switch (raor->opera_type)
	{
	case Protocol::RA_COLLECT_WORD_OPERA_TYPE_ROLE_INFO:
		{
			this->RaCollectWordSendInfo();
		}
		break;
	case Protocol::RA_COLLECT_WORD_OPERA_TYPE_EXCHANGE:
		{
			this->RaCollectWordExchange(raor->param_1);
		}
		break;
	case Protocol::RA_COLLECT_WORD_OPERA_TYPE_DAY_FREE:
		{
			this->RaCollectWordFetchDayGift();
		}
		break;
	}
}

void RoleActivityCollectWord::RaCollectWordSendInfo()
{
	Protocol::SCRaCollectWordInfo info;

	info.day_gift_is_fetch = m_param.ra_collect_word_day_gift_is_fetch;
	info.reserve_sh = 0;
	for (int i = 0; i < ARRAY_LENGTH(info.exchange_times) && i < ARRAY_LENGTH(m_param.ra_collect_word_exchange_num); ++i)
	{
		info.exchange_times[i] = m_param.ra_collect_word_exchange_num[i];
	}

	for (int i = 0; i < ARRAY_LENGTH(info.total_exchange_times) && i < ARRAY_LENGTH(m_param.ra_collect_word_total_exchange_num); ++i)
	{
		info.total_exchange_times[i] = m_param.ra_collect_word_total_exchange_num[i];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int RoleActivityCollectWord::RaCollectWordExchange(int seq)
{
	const RandActivityCollectWordConfig * config = static_cast<const RandActivityCollectWordConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLLECT_WORD));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RaCollectWordExchangeCfg * cfg = config->GetExchangeCfg(seq);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if ((cfg->limit_exchange_times != 0 && this->RaCollectWordGetExchangeTimes(seq) >= cfg->limit_exchange_times) || (cfg->limit_total_exchange_times != 0 && this->RaCollectWordGetTotalExchangeTimes(seq) >= cfg->limit_total_exchange_times))
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(cfg->reward))
	{
		return -__LINE__;
	}

	const std::vector<ItemExtern::ItemConsumeConfig>  & stuff_list = cfg->consume_item;
	if (stuff_list.empty())
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->ConsumeSuffList(stuff_list, __FUNCTION__))
	{
		return __LINE__;
	}

	m_role_module_mgr->GetKnapsack()->Put(cfg->reward, PUT_REASON_RA_COLLECT_WORD_EXCHANGE);

	if (cfg->is_broadcast != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_collect_word_broadcast,
			m_role_module_mgr->GetRole()->GetName(), cfg->reward.item_id, RAND_ACTIVITY_TYPE_COLLECT_WORD);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
		}
	}

	this->RaCollectWordAddExchangeTimes(seq);
	this->RaCollectWordSendInfo();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityCollectWord::RaCollectWordGetExchangeTimes(int seq)
{
	if (seq < 0 || seq >= ARRAY_LENGTH(m_param.ra_collect_word_exchange_num))
	{
		return 0;
	}

	return m_param.ra_collect_word_exchange_num[seq];
}

void RoleActivityCollectWord::RaCollectWordAddExchangeTimes(int seq)
{
	if (seq < 0 || seq >= ARRAY_LENGTH(m_param.ra_collect_word_exchange_num) || seq >= ARRAY_LENGTH(m_param.ra_collect_word_total_exchange_num))
	{
		return;
	}

	m_param.ra_collect_word_exchange_num[seq] += 1;
	m_param.ra_collect_word_total_exchange_num[seq] += 1;

	this->SetRoleDataChange();
}

void RoleActivityCollectWord::RaCollectWordFetchDayGift()
{
	if (m_param.ra_collect_word_day_gift_is_fetch != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COLLECT_WORD_NOT_FETCH);
		return;
	}

	const RandActivityCollectWordConfig * config = static_cast<const RandActivityCollectWordConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLLECT_WORD));
	if (NULL == config)
	{
		return;
	}

	const RACollectWordOtherCfg& other_cfg = config->GetOtherCfg();
	if (other_cfg.count > 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(other_cfg.count, &other_cfg.day_reward_list[0]))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutList(other_cfg.count, &other_cfg.day_reward_list[0], PUT_REASON_RA_COLLECT_WORD_EXCHANGE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
			return;
		}
	}
	m_param.ra_collect_word_day_gift_is_fetch = 1;
	this->RaCollectWordSendInfo();

	this->SetRoleDataChange();
}

int RoleActivityCollectWord::RaCollectWordGetTotalExchangeTimes(int seq)
{
	if (seq < 0 || seq >= ARRAY_LENGTH(m_param.ra_collect_word_total_exchange_num))
	{
		return 0;
	}

	return m_param.ra_collect_word_total_exchange_num[seq];
}
