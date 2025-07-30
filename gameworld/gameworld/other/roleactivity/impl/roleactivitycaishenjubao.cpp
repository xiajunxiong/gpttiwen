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
#include "servercommon/string/gameworldstr.h"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "protocol/randactivity/msgracaishenjubao.hpp"
#include "config/randactivityconfig/impl/randactivitycaishenjubaoconfig.hpp"
#include "roleactivitycaishenjubao.hpp"

RoleActivityCaiShenJuBao::RoleActivityCaiShenJuBao(int type) :RoleActivity(type)
{
}

RoleActivityCaiShenJuBao::~RoleActivityCaiShenJuBao()
{
}

void RoleActivityCaiShenJuBao::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityCaiShenJuBao::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityCaiShenJuBao::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityCaiShenJuBao::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_cai_shen_ju_bao_buy_day_times); ++i)
	{
		m_param.ra_cai_shen_ju_bao_buy_day_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityCaiShenJuBao::CheckInitRandActivity(int rand_act_type, bool is_force)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_cai_shen_ju_bao_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_cai_shen_ju_bao_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->OnCheckInitBonus();

		this->SetRoleDataChange();
	}
}

bool RoleActivityCaiShenJuBao::IsFinishBonusCond(int type)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return false;
	}
	const RandActivityCaiShenJuBaoConfig * config = static_cast<const RandActivityCaiShenJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}
	int item_num = config->GetRACaiShenJuBaoItemNum(ramgr);
	bool is_finish = true;
	for (int i = 0; i < item_num && i < RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM; i++)
	{
		if(!m_param.buy_flag.IsBitSet(i)) 
		{
			is_finish = false;
			break;
		}
	}

	return is_finish;
}

void RoleActivityCaiShenJuBao::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
				int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);  //购买财神聚宝		param1 配置seq

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
	}
}

bool RoleActivityCaiShenJuBao::OnBuyItem(int seq, int chong_zhi_gold)
{
	if (seq < 0 || seq >= RA_CAI_SHEN_JU_BAO_MAX_ITEM)
	{
		return false;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return false;
	}

	const RandActivityCaiShenJuBaoConfig * config = static_cast<const RandActivityCaiShenJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}

	const RACaiShenJuBaoItemCfg * cfg = config->GetRACaiShenJuBaoItemCfg(ramgr, seq);
	if (cfg == NULL)
	{
		return false;
	}

	if (cfg->need_chong_zhi_gold != chong_zhi_gold)
	{
		return false;
	}

	if (cfg->limit_type == 1)
	{
		if (m_param.ra_cai_shen_ju_bao_buy_times[seq] >= cfg->limit_buy_times)
		{
			return false;
		}
	}

	if (cfg->limit_type == 2)
	{
		if (m_param.ra_cai_shen_ju_bao_buy_day_times[seq] >= cfg->limit_buy_times_day)
		{
			return false;
		}
	}
	m_param.buy_flag.SetBit(seq);
	m_role_module_mgr->GetMoney()->AddGold(cfg->add_gold + cfg->extra_add_gold, __FUNCTION__);

	if (cfg->limit_type == 1)
	{
		m_param.ra_cai_shen_ju_bao_buy_times[seq] += 1;
	}

	if (cfg->limit_type == 2)
	{
		m_param.ra_cai_shen_ju_bao_buy_day_times[seq] += 1;
	}


	this->SendInfo();

	this->SetRoleDataChange();

	return true;
}

int RoleActivityCaiShenJuBao::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= RA_CAI_SHEN_JU_BAO_MAX_ITEM)
	{
		return 1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO))
	{
		return 1;
	}

	const RandActivityCaiShenJuBaoConfig * config = static_cast<const RandActivityCaiShenJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RACaiShenJuBaoItemCfg * cfg = config->GetRACaiShenJuBaoItemCfg(ramgr, seq);
	if (cfg == NULL)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = cfg->need_chong_zhi_gold;
		return 0;
	}

	if (cfg->limit_type == 1)
	{
		if (m_param.ra_cai_shen_ju_bao_buy_times[seq] >= cfg->limit_buy_times)
		{
			return 2;
		}
	}
	else if (cfg->limit_type == 2)
	{
		if (m_param.ra_cai_shen_ju_bao_buy_day_times[seq] >= cfg->limit_buy_times_day)
		{
			return 2;
		}
	}
	m_param.buy_flag.SetBit(seq);
	m_role_module_mgr->GetMoney()->AddGold(cfg->add_gold + cfg->extra_add_gold, __FUNCTION__);

	if (cfg->limit_type == 1)
	{
		m_param.ra_cai_shen_ju_bao_buy_times[seq] += 1;
	}

	if (cfg->limit_type == 2)
	{
		m_param.ra_cai_shen_ju_bao_buy_day_times[seq] += 1;
	}
	
	this->SendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityCaiShenJuBao::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= RA_CAI_SHEN_JU_BAO_MAX_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityCaiShenJuBaoConfig * config = static_cast<const RandActivityCaiShenJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RACaiShenJuBaoItemCfg * cfg = config->GetRACaiShenJuBaoItemCfg(ramgr, seq);
	if (cfg == NULL)
	{
		return -__LINE__;
	}


	if (cfg->limit_type == 1)
	{
		if (m_param.ra_cai_shen_ju_bao_buy_times[seq] >= cfg->limit_buy_times)
		{
			return -__LINE__;
		}
	}

	if (cfg->limit_type == 2)
	{
		if (m_param.ra_cai_shen_ju_bao_buy_day_times[seq] >= cfg->limit_buy_times_day)
		{
			return -__LINE__;
		}
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
	buy_item->need_gold = cfg->need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_cai_shen_ju_bao_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = cfg->limit_type;
	buy_item->param3 = cfg->add_gold + cfg->extra_add_gold;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_cai_shen_ju_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_cai_shen_ju_bao.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityCaiShenJuBao::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_cai_shen_ju_bao.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					m_param.ra_cai_shen_ju_bao_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	int seq = buy_item->param1;
	int limit_type = buy_item->param2;
	int add_gold = buy_item->param3;
	m_role_module_mgr->GetMoney()->AddGold(add_gold, __FUNCTION__);

	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_cai_shen_ju_bao_begin_timestamp)
	{
		if (0 <= seq && seq < RA_CAI_SHEN_JU_BAO_MAX_ITEM)
		{
			if (limit_type == 1)
			{
				m_param.ra_cai_shen_ju_bao_buy_times[seq] += 1;
			}

			if (limit_type == 2)
			{
				m_param.ra_cai_shen_ju_bao_buy_day_times[seq] += 1;
			}
		}

		m_param.buy_flag.SetBit(seq);
		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_cai_shen_ju_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

void RoleActivityCaiShenJuBao::SendInfo()
{
	Protocol::SCRACaiShenJuBaoInfo protocol;
	for (int i = 0; i < RA_CAI_SHEN_JU_BAO_MAX_ITEM; ++i)
	{
		protocol.ra_cai_shen_ju_bao_buy_day_times[i] = m_param.ra_cai_shen_ju_bao_buy_day_times[i];
		protocol.ra_cai_shen_ju_bao_buy_times[i] = m_param.ra_cai_shen_ju_bao_buy_times[i];
	}
	protocol.buy_flag = m_param.buy_flag;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}
