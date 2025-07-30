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
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "protocol/randactivity/msgraxianshibaodai2.hpp"
#include "config/randactivityconfig/impl/randactivityxianshibaodai2config.hpp"
#include "roleactivityxianshibaodai2.hpp"


RoleActivityXianShiBaoDai2::RoleActivityXianShiBaoDai2(int type) :RoleActivity(type)
{
}

RoleActivityXianShiBaoDai2::~RoleActivityXianShiBaoDai2()
{
}

void RoleActivityXianShiBaoDai2::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityXianShiBaoDai2::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityXianShiBaoDai2::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityXianShiBaoDai2::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		m_param.ra_day_buy_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityXianShiBaoDai2::CheckInitRandActivity(int rand_act_type, bool is_force)
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
		this->OnCheckInitBonus();

		this->SetRoleDataChange();
	}
}

bool RoleActivityXianShiBaoDai2::IsFinishBonusCond(int type)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return false;
	}

	const RandActivityXianShiBaoDai2Config * config = static_cast<const RandActivityXianShiBaoDai2Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}

	int item_num = config->GetRAXianShiBaoDaiItemNum(ramgr);
	bool is_finish = true;
	for (int i = 0; i < item_num && i < RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM; i++)
	{
		if(!m_param.buy_flag.IsBitSet(i)) 
		{
			is_finish = false;
			break;
		}
	}

	return is_finish;
}

void RoleActivityXianShiBaoDai2::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
				int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);   //购买限时宝袋  param1 配置seq

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
			this->ReceiveFreeReward();
		}
		break;
	}
}

bool RoleActivityXianShiBaoDai2::OnBuyItemOld(int seq, int chong_zhi_gold)
{
	if (seq < 0 || seq >= RA_XIAN_SHI_BAO_DAI_MAX_ITEM)
	{
		return false;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return false;
	}

	const RandActivityXianShiBaoDai2Config * config = static_cast<const RandActivityXianShiBaoDai2Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}

	const RAXianShiBaoDai2ItemCfg * buy_cfg = config->GetRAXianShiBaoDaiItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return false;
	}

	if (buy_cfg->need_chong_zhi_gold != chong_zhi_gold)
	{
		return false;
	}

	if (seq != 0 && !(m_param.can_buy_seq & (1 << (seq - 1))))		//检测是不是购买过前一个
	{
		return false;
	}

	if (buy_cfg->limit_type == 1)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return false;
		}
	}

	if (buy_cfg->limit_type == 2)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return false;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		return false;
	}
	m_param.buy_flag.SetBit(seq);
	m_role_module_mgr->GetMoney()->AddGold(buy_cfg->add_gold, __FUNCTION__);

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0], PUT_REASON_XIAN_SHI_BAO_DAI);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, buy_cfg->rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	if (buy_cfg->limit_type == 1)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == 2)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	m_param.can_buy_seq |= 1 << seq;

	this->SendInfo();
	this->SetRoleDataChange();

	return true;
}

int RoleActivityXianShiBaoDai2::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= RA_XIAN_SHI_BAO_DAI_MAX_ITEM)
	{
		return 1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2))
	{
		return 1;
	}

	const RandActivityXianShiBaoDai2Config * config = static_cast<const RandActivityXianShiBaoDai2Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RAXianShiBaoDai2ItemCfg * buy_cfg = config->GetRAXianShiBaoDaiItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}

	if (seq != 0 && !(m_param.can_buy_seq & (1 << (seq - 1))))		//检测是不是购买过前一个
	{
		return 1;
	}

	if (buy_cfg->limit_type == 1)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return 2;
		}
	}

	if (buy_cfg->limit_type == 2)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return 2;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		return 1;
	}
	m_param.buy_flag.SetBit(seq);
	m_role_module_mgr->GetMoney()->AddGold(buy_cfg->add_gold, __FUNCTION__);

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0], PUT_REASON_XIAN_SHI_BAO_DAI);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, buy_cfg->rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	if (buy_cfg->limit_type == 1)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == 2)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	m_param.can_buy_seq |= 1 << seq;

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityXianShiBaoDai2::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= RA_XIAN_SHI_BAO_DAI_MAX_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityXianShiBaoDai2Config * config = static_cast<const RandActivityXianShiBaoDai2Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAXianShiBaoDai2ItemCfg * buy_cfg = config->GetRAXianShiBaoDaiItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (seq != 0 && !(m_param.can_buy_seq & (1 << (seq - 1))))		//检测是不是购买过前一个
	{
		return -__LINE__;
	}

	if (buy_cfg->limit_type == 1)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->limit_type == 2)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->rewards.empty())
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
	buy_item->need_gold = buy_cfg->need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = buy_cfg->limit_type;
	buy_item->param3 = buy_cfg->add_gold;
	::ItemConfigDataVecToBuyItem(buy_cfg->rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_xian_shi_bao_dai_2, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_xian_shi_bao_dai_2.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());
	
	return 0;
}

int RoleActivityXianShiBaoDai2::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_xian_shi_bao_dai_2.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					      m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return  -__LINE__;
	}

	int seq = buy_item->param1;
	int limit_type = buy_item->param2;
	int add_gold = buy_item->param3;


	m_role_module_mgr->GetMoney()->AddGold(add_gold, __FUNCTION__);

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_XIAN_SHI_BAO_DAI);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{
		if (0 <= seq && seq < RA_XIAN_SHI_BAO_DAI_MAX_ITEM)
		{
			if (limit_type == 1)
			{
				m_param.ra_buy_times[seq] += 1;
			}

			if (limit_type == 2)
			{
				m_param.ra_day_buy_times[seq] += 1;
			}
		}

		m_param.can_buy_seq |= 1 << seq;
		m_param.buy_flag.SetBit(seq);		

		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_xian_shi_bao_dai_2, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

void RoleActivityXianShiBaoDai2::SendInfo()
{
	Protocol::SCRAXianShiBaoDai2Info protocol;
	protocol.can_buy_seq = m_param.can_buy_seq;
	for (int i = 0; i < RA_XIAN_SHI_BAO_DAI_MAX_ITEM; ++i)
	{
		protocol.ra_buy_times[i] = m_param.ra_buy_times[i];
		protocol.ra_day_buy_times[i] = m_param.ra_day_buy_times[i];
	}
	protocol.buy_flag = m_param.buy_flag;
	protocol.has_free_reward = m_param.has_free_reward;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityXianShiBaoDai2::ReceiveFreeReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	if (m_param.has_free_reward != 0) return;

	const RandActivityXianShiBaoDai2Config * config = static_cast<const RandActivityXianShiBaoDai2Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const RAXianShiBaoDai2OtherCfg & other_cfg = config->GetOtherCfg();

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(other_cfg.free_reward))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(other_cfg.free_reward, PUT_REASON_XIAN_SHI_BAO_DAI))
	{
		return;
	}

	m_param.has_free_reward = 1;

	std::string gamelog_str;
	for (int i = 0; i < (int)other_cfg.free_reward.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", other_cfg.free_reward[i].item_id, other_cfg.free_reward[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "::%s user[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->SendInfo();
	this->SetRoleDataChange();
}
