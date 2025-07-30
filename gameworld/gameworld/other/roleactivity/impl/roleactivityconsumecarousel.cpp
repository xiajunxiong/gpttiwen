#include "roleactivityconsumecarousel.hpp"
#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityconsumecarouselconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityconsumecarousel.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgraconsumecarousel.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

RoleActivityConsumeCarousel::RoleActivityConsumeCarousel(int type)
	:RoleActivity(type)
{

}

RoleActivityConsumeCarousel::~RoleActivityConsumeCarousel()
{

}

void RoleActivityConsumeCarousel::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy( &m_param , p.base_data ,sizeof(RAConsumerCarouselParam));

	if (0 == m_param.old_num_data_move_flag)
	{
		m_param.new_ra_consume_carousel_last_num = m_param.old_ra_consume_carousel_last_num;
		m_param.new_ra_consume_carousel_consume_num = m_param.old_ra_consume_carousel_consume_num;
		m_param.old_num_data_move_flag = 1;
		m_param.old_ra_consume_carousel_last_num = 0;
		m_param.old_ra_consume_carousel_consume_num = 0;
	}

	this->PrintLog(__FUNCTION__);
}

void RoleActivityConsumeCarousel::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAConsumerCarouselParam));

	this->PrintLog(__FUNCTION__);
}

void RoleActivityConsumeCarousel::OnLoginSendInfo()
{
	this->SendConsumeCarouselAllInfo();
}

void RoleActivityConsumeCarousel::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr && !ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL))
	{
		m_param.new_ra_consume_carousel_last_num = 0;
		m_param.ra_today_consume_num = 0;
		m_param.ra_consume_num_in_time = 0;
		this->SendConsumeCarouselInfo();
		this->SendConsumeCarouselShow();
		this->SetRoleDataChange();

		this->PrintLog("RoleActivityConsumeCarousel::OnDayChange___111");
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL);

	m_param.ra_today_consume_num = 0;
	this->SendConsumeCarouselInfo();

	this->SetRoleDataChange();

	this->PrintLog("RoleActivityConsumeCarousel::OnDayChange___222");
}

void RoleActivityConsumeCarousel::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_consume_carousel_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_consume_carousel_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();

		this->PrintLog(__FUNCTION__);
	}
}

void RoleActivityConsumeCarousel::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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
	case Protocol::RA_CONSUME_CAROUSEL_OPERA_TYPE_GET_INFO:
		{
			this->SendConsumeCarouselAllInfo();								// 发送消费转盘全部信息
		}
		break;
	case Protocol::RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ONE:
	case Protocol::RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ALL:
		{
			this->OnRAStartConsumeCarousel(raor->opera_type);				//消费转盘启动   --- 转动1次   type -- 1
		}
		break;
	case Protocol::RA_CONSUME_CAROUSEL_OPERA_TYPE_GET_REWARD:
		{
			this->OnRAGetConsumeCarouselReward(raor->param_1);				//消费转盘奖励下发 --- param1 -- seq
			this->SendConsumeCarouselInfo();
		}
		break;
	}
}

void RoleActivityConsumeCarousel::OnConsumeGold(long long consume_gold)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL);

	m_param.ra_today_consume_num += consume_gold;
	m_param.ra_consume_num_in_time += consume_gold;


	const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();
	int num = m_param.ra_consume_num_in_time / other_cfg.spend_ingots_once;
	if (num < m_param.new_ra_consume_carousel_consume_num)
	{
		m_param.new_ra_consume_carousel_consume_num = num;
	}

	m_param.new_ra_consume_carousel_last_num = num - m_param.new_ra_consume_carousel_consume_num;
	this->SendConsumeCarouselInfo();
	this->SetRoleDataChange();

	this->PrintLog(__FUNCTION__);
}

void RoleActivityConsumeCarousel::SendConsumeCarouselAllInfo()
{
	this->SendConsumeCarouselInfo();		//发送转盘信息
	this->SendConsumeCarouselShow();		//发送珍稀显示
}

void RoleActivityConsumeCarousel::SendConsumeCarouselInfo()
{
	Protocol::SCRAConsumeCarouselInfo info;
	info.last_num = m_param.new_ra_consume_carousel_last_num;
	info.consume_num_day = m_param.ra_today_consume_num;
	info.consume_count = m_param.ra_consume_num_in_time;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityConsumeCarousel::SendConsumeCarouselShow()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	Protocol::SCRAConsumeCarouselShow info;
	RandActivityConsumeCarousel *rand_activity = ramgr->GetRAconsumeCarousel();
	info.count = 0;
	if (NULL != rand_activity)
	{
		info.count = rand_activity->GetRAShowInfoCount();
		if (info.count <= 0) return;
		rand_activity->GetRAShowInfoData(info.consume_carousel_list, info.count);
	}
	else
	{
		return;
	}
	int length = sizeof(info) - sizeof(RAShowInfoParam::RAShowInfoDataParam) * (RA_SHOW_INFO_MAX_NUM - info.count);
	m_role_module_mgr->NetSend(&info, length);
}

void RoleActivityConsumeCarousel::SendConsumeCarouselReward()
{
	Protocol::SCRAConsumeCarouselReward info;
	info.index = m_param.ra_consume_carousel_last_index;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityConsumeCarousel::OnRAStartConsumeCarousel(int turn_type)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (m_param.new_ra_consume_carousel_last_num <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}

	const RandActivityConsumeCarouselConfig * config = static_cast<const RandActivityConsumeCarouselConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (turn_type == Protocol::RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ONE)
	{
		if (m_param.ra_consume_carousel_last_index != -1)
		{
			this->SendConsumeCarouselReward();
			return;
		}
	}
	else if (turn_type == Protocol::RA_CONSUME_CAROUSEL_OPERA_TYPE_CONSUME_ALL)
	{
		const RAConsumerCarouselCfg * cfg = NULL;
		while (m_param.new_ra_consume_carousel_last_num > 0)
		{
			if (m_param.ra_consume_carousel_last_index != -1)
			{
				 cfg = config->GetConsumerCarouselbyseq(ramgr, m_param.ra_consume_carousel_last_index);
				if (NULL == cfg) 
				{
					return;
				}
			}
			else
			{
				cfg = config->GetConsumerCarouselReward(ramgr);
				if (NULL == cfg) 
				{
					return;
				}
			}
			m_param.ra_consume_carousel_last_index = cfg->seq;
			this->SetRoleDataChange();
			if (!this->OnRAGetConsumeCarouselReward(cfg->seq, false))
			{
				return;
			}
		}
		this->SendConsumeCarouselInfo();
		return;
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	const RAConsumerCarouselCfg * cfg = config->GetConsumerCarouselReward(ramgr);
	if (NULL == cfg) return;

	m_param.ra_consume_carousel_last_index = cfg->seq;

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityConsumeCarousel::OnRAStartConsumeCarousel role[%d,%s] turn_type[%d] seq[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), turn_type, cfg->seq);

	this->SendConsumeCarouselReward();

	this->SetRoleDataChange();

	// 给奖励
	this->OnRAGetConsumeCarouselReward(m_param.ra_consume_carousel_last_index);
	this->SendConsumeCarouselInfo();

	this->PrintLog(__FUNCTION__);
}

bool RoleActivityConsumeCarousel::OnRAGetConsumeCarouselReward(int seq, bool is_single)
{
	if (m_param.new_ra_consume_carousel_last_num <= 0) return false;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return false;

	const RandActivityConsumeCarouselConfig * config = static_cast<const RandActivityConsumeCarouselConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	const RAConsumerCarouselCfg * cfg = config->GetConsumerCarouselbyseq(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	bool is_bool = true;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		int put_reason = is_single ? PUT_REASON_RA_CONSUME_CAROUSEL : PUT_REASON_RA_CONSUME_CAROUSEL_ALL;
		if (m_role_module_mgr->GetKnapsack()->Put(cfg->item, put_reason))
		{
			is_bool = false;
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGetConsumeCarouselReward user[%d, %s], level[%d], type[%d] item[id:%d num:%d]", m_role_module_mgr->GetRole()->GetUID(),
				m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL, cfg->item.item_id, cfg->item.num);
		}
	}
	if (is_bool)
	{
		static MailContentParam contentparam; contentparam.Reset();
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGetConsumeCarouselReward by mail, role[%d,%s] item_id[%d] num[%d] reason[%s]",
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), cfg->item.item_id, cfg->item.num, __FUNCTION__);
	}
	if (cfg->show)
	{
		RandActivityConsumeCarousel *rand_activity = ramgr->GetRAconsumeCarousel();
		if (NULL != rand_activity)
		{
			RAShowInfoParam::RAShowInfoDataParam data;
			data.item_id = cfg->item.item_id;
			data.num = cfg->item.num;
			F_STRNCPY(data.role_name, m_role_module_mgr->GetRole()->GetName(), sizeof(data.role_name));
			rand_activity->OnAddShowListInfo(&data, false);
		}

		this->SendConsumeCarouselShow();
	}

	m_param.ra_consume_carousel_last_index = -1;
	m_param.new_ra_consume_carousel_last_num--;
	m_param.new_ra_consume_carousel_consume_num++;

	this->SetRoleDataChange();

	this->PrintLog(__FUNCTION__);

	this->OnParticipateRA(__FUNCTION__);

	return true;
}

void RoleActivityConsumeCarousel::PrintLog(const char* func)
{
	if (NULL == func) return;

	gamelog::g_log_rand_activity.printf(LL_INFO, "func[%s] role[%d,%s] begin_timestamp[%u] \
		last_num[%d], consume_num[%d] today_consume_num[%d] consume_num_in_time[%d] last_index[%d]",
		func, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), 
		m_param.ra_consume_carousel_begin_timestamp, m_param.new_ra_consume_carousel_last_num, 
		m_param.new_ra_consume_carousel_consume_num, m_param.ra_today_consume_num,
		m_param.ra_consume_num_in_time, m_param.ra_consume_carousel_last_index);
}
