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

#include "protocol/randactivity/msglingpaitianci.hpp"
#include "config/randactivityconfig/impl/randactivitylingpaitianci2config.hpp"
#include "roleactivitylingpaitianci2.hpp"

RoleActivityLingPaiTianCi2::RoleActivityLingPaiTianCi2(int type) : RoleActivity(type)
{
}

RoleActivityLingPaiTianCi2::~RoleActivityLingPaiTianCi2()
{
}

void RoleActivityLingPaiTianCi2::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityLingPaiTianCi2::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityLingPaiTianCi2::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityLingPaiTianCi2::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		gamelog::g_log_ling_pai_tian_ci.printf(LL_INFO, "%s line:%d |  role[%d %s]   reset_time:%u", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
						       m_param.ra_begin_timestamp);

		this->SetRoleDataChange();
	}
}

void RoleActivityLingPaiTianCi2::OnAddChongZhi(long long chongzhi)
{
	m_param.chong_zhi_num += (unsigned int)chongzhi;
	this->SendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_ling_pai_tian_ci.printf(LL_INFO, "%s line:%d |  role[%d %s] m_param.chong_zhi_num:%u chongzhi:%u", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					   m_param.chong_zhi_num, (unsigned int)chongzhi);
}

void RoleActivityLingPaiTianCi2::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (raor->opera_type==1)
	{
		this->FetchRewards(raor->param_1);
	}
}

int RoleActivityLingPaiTianCi2::FetchRewards(int seq)
{
	if (seq < 0 || seq >= RA_LING_PAI_TIAN_CI_2_MAX_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityLingPaiTianCi2Config * config = static_cast<const RandActivityLingPaiTianCi2Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RALingPaiTianCi2Cfg * cfg = config->GetRALingPaiTianCi2CfgCfg(ramgr, seq);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if (m_param.chong_zhi_num < cfg->need_chong_zhi_gold)
	{
		return -__LINE__;
	}

	if (m_param.fetch_flag & (1u << seq))
	{
		return -__LINE__;
	}


	if (cfg->rewards.empty())
	{
		return -__LINE__;
	}

	m_param.fetch_flag |= (1u << seq);

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)cfg->rewards.size(), &cfg->rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)cfg->rewards.size(), &cfg->rewards[0], PUT_REASON_ZU_HE_HE_LI);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, cfg->rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	if (cfg->is_borcast)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_xin_fa_wu_liang_borcast,
				      m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_activity_type, cfg->borcast_item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_ling_pai_tian_ci.printf(LL_INFO, "%s line:%d |  role[%d %s]   m_param.fetch_flag:%u        fetch_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					       m_param.fetch_flag, seq);

	return 0;
}

void RoleActivityLingPaiTianCi2::SendInfo()
{
	Protocol::SCRALingPaiTianCi2Info protocol;
	protocol.chong_zhi_num = m_param.chong_zhi_num;
	protocol.fetch_flag = m_param.fetch_flag;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}
