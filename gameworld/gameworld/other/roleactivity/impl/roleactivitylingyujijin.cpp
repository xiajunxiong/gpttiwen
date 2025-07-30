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
#include "servercommon/uniqueidgenerator.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "protocol/randactivity/msglingyujijin.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "config/randactivityconfig/impl/randactivitylingyujijinconfig.hpp"
#include "roleactivitylingyujijin.hpp"

int RoleActivityLingYuJiJin::OnBuyItemCheck(int phase, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (phase < 0 || phase >= RA_LING_YU_JI_JIN_MAX_PHASE)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return -__LINE__;
	}

	const RandActivityLingYuJiJinConfig * config = static_cast<const RandActivityLingYuJiJinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RALingYuJiJinPhareCfg * buy_cfg = config->GetRALingYuJiJinPhareCfg(phase);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (buy_cfg->need_chong_zhi <= 0)
	{
		return -__LINE__;
	}

	if (m_param.active_flag & (1 << phase))
	{
		return -__LINE__;
	}


	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = buy_cfg->need_chong_zhi;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = phase;
	buy_item->param2 = buy_cfg->reward_ling_yu_num;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_ling_yu_ji_jin, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_ling_yu_ji_jin.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityLingYuJiJin::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_ling_yu_ji_jin.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					   m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	int reward_ling_yu_num = buy_item->param2;
	m_role_module_mgr->GetMoney()->AddOtherCoin(reward_ling_yu_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__);

	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{
		int phase = buy_item->param1;
		m_param.active_flag |= 1 << phase;
		gamelog::g_log_ling_yu_ji_jin.printf(LL_INFO, "%s line:%d | role[%d %s] phase:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), phase);
		
		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_ling_yu_ji_jin, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityLingYuJiJin::OnFetchReward(int type)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return -__LINE__;
	}

	const RandActivityLingYuJiJinConfig * config = static_cast<const RandActivityLingYuJiJinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RALingYuJiJinCfg * cfg = config->GetRALingYuJiJinCfg(type);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < cfg->need_role_level)
	{
		return -__LINE__;
	}

	int phase = cfg->need_active_phase;
	int seq = cfg->seq;
	if (!(m_param.active_flag & (1 << phase)))
	{
		return -__LINE__;
	}

	if (phase < 0 || phase >= ARRAY_LENGTH(m_param.fetch_flag))
	{
		return -__LINE__;
	}

	if (seq < 0 || seq >= RA_LING_YU_JI_JIN_MAX_ITEM)
	{
		return -__LINE__;
	}

	if (m_param.fetch_flag[phase] & (1 << seq))
	{
		return -__LINE__;
	}

	if (cfg->reward_ling_yu_num <= 0)
	{
		return -__LINE__;
	}

	m_role_module_mgr->GetMoney()->AddOtherCoin(cfg->reward_ling_yu_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
	m_param.fetch_flag[phase] |= 1 << cfg->seq;

	gamelog::g_log_ling_yu_ji_jin.printf(LL_INFO, "%s line:%d | role[%d %s] type:%d phase:%d seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), type, phase, seq);

	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityLingYuJiJin::OnBuyGiftItem(int phase, ARG_OUT int * price)
{
	if (phase < 0 || phase >= RA_LING_YU_JI_JIN_MAX_PHASE)
	{
		return 1;
	}
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}

	const RandActivityLingYuJiJinConfig * config = static_cast<const RandActivityLingYuJiJinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RALingYuJiJinPhareCfg * buy_cfg = config->GetRALingYuJiJinPhareCfg(phase);
	if (buy_cfg == NULL)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi;
		return 0;
	}

	if (m_param.active_flag & (1 << phase))
	{
		return 2;
	}

	m_role_module_mgr->GetMoney()->AddOtherCoin(buy_cfg->reward_ling_yu_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
	m_param.active_flag |= 1 << phase;
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityLingYuJiJin::SendInfo()
{
	Protocol::SCRALingYuJiJinInfo protocol;

	protocol.active_flag = m_param.active_flag;

	for (int i = 0; i < RA_LING_YU_JI_JIN_MAX_PHASE; ++i)
	{
		protocol.fetch_flag[i] = m_param.fetch_flag[i];
	}

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

RoleActivityLingYuJiJin::RoleActivityLingYuJiJin(int type) :RoleActivity(type)
{
}

RoleActivityLingYuJiJin::~RoleActivityLingYuJiJin()
{
}

void RoleActivityLingYuJiJin::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityLingYuJiJin::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityLingYuJiJin::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityLingYuJiJin::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityLingYuJiJin::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0:
			{
				this->SendInfo();
			}
			break;
		case 1:
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);	// p1 要激活phase，充值购买

				if (ret_num == 0)
				{
					Protocol::SCCmdBuyItemCheckRet protocol;
					protocol.unique_id = unique_id;
					protocol.cfg_ver = raor->cfg_ver;
					protocol.buy_type = BUY_TYPE;

					m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
				}
			}
			break;
		case 2:
			{
				this->OnFetchReward(raor->param_1);					// p1 领取激活奖励 配置type
			}
			break;
	}
}
