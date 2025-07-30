#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

#include "protocol/randactivity/msgchunridengfeng.hpp"
#include "config/randactivityconfig/impl/randactivitychunridengfengconfig.hpp"
#include "roleactivitychunridengfeng.hpp"

RoleActivityChunRiDengFeng::RoleActivityChunRiDengFeng(int type) : RoleActivity(type)
{
}

RoleActivityChunRiDengFeng::~RoleActivityChunRiDengFeng()
{
}

int RoleActivityChunRiDengFeng::Explore()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr || !ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return -__LINE__;
	}

	const RandActivityChunRiDengFengConfig * config = static_cast<const RandActivityChunRiDengFengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAChunRiDengFengOtherCfg & other_cfg = config->GetOtherCfg();
	const RAChunRiDengFengLayerCfg * layer_cfg = config->GetLayerCfg(ramgr, m_param.layer);
	if (layer_cfg == NULL)
	{
		return  -__LINE__;
	}

	bool is_last_layer = m_param.layer >= RA_CHUN_RI_DENG_FENG_MAX_LEVEL;
	int base_layer = m_param.explore_total_times + 1 >= other_cfg.change_base_layer_explore_times ? other_cfg.base_layer : 0;
	if (is_last_layer)
	{
		bool is_succ = false;
		const RAChunRiDengFengLayerCfg::ItemCfg * item_cfg = layer_cfg->GetRandItem(&is_succ, m_param.get_item_flag);
		if (!is_succ)
		{
			return  -__LINE__;
		}

		//消耗
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.explore_comsume_item_id, other_cfg.explore_comsume_num, __FUNCTION__))
		{
			return  -__LINE__;
		}

		if (item_cfg && item_cfg->reward.item_id)
		{
			if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &item_cfg->reward))
			{
				m_role_module_mgr->GetKnapsack()->Put(item_cfg->reward, PUT_REASON_CHUN_RI_DENG_FENG);
			}
			else
			{
				// 发邮件
				static MailContentParam contentparam; contentparam.Reset();
				FillMailContentParam(contentparam, item_cfg->reward);

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, m_role_activity_type);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, m_role_activity_type);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
				}
			}
			
			m_param.get_item_flag |= (1 << item_cfg->seq);

			// 传闻
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chun_ri_deng_feng_broadcast,
					      m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), item_cfg->reward.item_id);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
			}

			gamelog::g_log_chun_ri_deng_feng.printf(LL_INFO, "%s line:%d | user[%d, %s], ra_begin_timestamp[%ud], item_seq:%d [item_id:%d num:%d is_bind:%d]", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), 
								m_param.ra_begin_timestamp, item_cfg->seq, item_cfg->reward.item_id, item_cfg->reward.num, item_cfg->reward.is_bind);
		}

		m_param.explore_times = 0;
		m_param.layer = base_layer;
	}
	else
	{
		const RAChunRiDengFengLayerCfg::ItemCfg * item_cfg = layer_cfg->GetRandItem();
		if (item_cfg == NULL)
		{
			return -__LINE__;
		}

		//消耗
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.explore_comsume_item_id, other_cfg.explore_comsume_num, __FUNCTION__))
		{
			return  -__LINE__;
		}

		if (!item_cfg->is_into_next)
		{
			if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &item_cfg->reward))
			{
				m_role_module_mgr->GetKnapsack()->Put(item_cfg->reward, PUT_REASON_CHUN_RI_DENG_FENG);
			}
			else
			{
				// 发邮件
				static MailContentParam contentparam; contentparam.Reset();
				FillMailContentParam(contentparam, item_cfg->reward);

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, m_role_activity_type);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, m_role_activity_type);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
				}
			}

			gamelog::g_log_chun_ri_deng_feng.printf(LL_INFO, "%s line:%d | user[%d, %s], ra_begin_timestamp[%ud], item_seq:%d [item_id:%d num:%d is_bind:%d]", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
								m_param.ra_begin_timestamp, item_cfg->seq, item_cfg->reward.item_id, item_cfg->reward.num, item_cfg->reward.is_bind);

			m_param.layer = base_layer;
		}
		else
		{
			m_param.layer += 1;
		}

		m_param.explore_times += 1;
	}

	bool can_last_layer_item = config->CanGetLastLayerItem(ramgr, m_param.get_item_flag);
	if (can_last_layer_item && m_param.explore_times >= other_cfg.goto_last_layer_explore_times)
	{
		m_param.explore_times = 0;
		m_param.layer = RA_CHUN_RI_DENG_FENG_MAX_LEVEL;
	}

	m_param.explore_total_times += 1;
	this->SendInfo();

	gamelog::g_log_chun_ri_deng_feng.printf(LL_INFO, "%s line:%d | user[%d, %s], ra_begin_timestamp[%ud], layer[%d], explore_total_times[%ud], explore_times[%ud], get_item_flag[%ud]", __FUNCTION__, __LINE__,
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_param.ra_begin_timestamp, m_param.layer, m_param.explore_total_times,
						m_param.explore_times, m_param.get_item_flag);

	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

void RoleActivityChunRiDengFeng::SendInfo()
{
	Protocol::SCRAChunRiDengFengInfo info;
	info.layer = m_param.layer;
	info.explore_total_times = m_param.explore_total_times;
	info.explore_times = m_param.explore_times;
	info.get_item_flag = m_param.get_item_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityChunRiDengFeng::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityChunRiDengFeng::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityChunRiDengFeng::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityChunRiDengFeng::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
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

void RoleActivityChunRiDengFeng::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0://请求下发
			{
				this->SendInfo();
			}
			break;
		case 1:
			{
				int ret = this->Explore();
				if (ret)
				{
					gamelog::g_log_chun_ri_deng_feng.printf(LL_INFO, "%s line:%d | user[%d, %s], ret[%d], ra_begin_timestamp[%ud], layer[%d], explore_total_times[%ud], explore_times[%ud], get_item_flag[%ud]", __FUNCTION__, __LINE__,
										m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), ret, m_param.ra_begin_timestamp, m_param.layer, m_param.explore_total_times,
										m_param.explore_times, m_param.get_item_flag);
				}
			}
			break;
	}
}
