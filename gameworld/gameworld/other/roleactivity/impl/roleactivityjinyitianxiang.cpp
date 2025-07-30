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
#include "protocol/randactivity/msgrajinyitianxiang.hpp"
#include "config/randactivityconfig/impl/randactivityjinyitianxiangconfig.hpp"
#include "roleactivityjinyitianxiang.hpp"


RoleActivityJinYiTianXiang::RoleActivityJinYiTianXiang(int type) :RoleActivity(type)
{
}

RoleActivityJinYiTianXiang::~RoleActivityJinYiTianXiang()
{
}

void RoleActivityJinYiTianXiang::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityJinYiTianXiang::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityJinYiTianXiang::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityJinYiTianXiang::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
}

void RoleActivityJinYiTianXiang::CheckInitRandActivity(int rand_act_type, bool is_force)
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
		m_param.ra_begin_timestamp = static_cast<unsigned int>(ramgr->GetRandActivityBeginTime(m_role_activity_type));

		const RandActivityJinYiTianXiangConfig * config = static_cast<const RandActivityJinYiTianXiangConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (config)
		{
			const RAJinYiTianXiangOtherCfg & other_cfg = config->GetOtherCfg();
			m_param.mark = other_cfg.mark;
		}

		this->SetRoleDataChange();
	}
}

void RoleActivityJinYiTianXiang::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityJinYiTianXiang::OnLoginCheck()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(this->GetRoleActivityType());
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(this->GetRoleActivityType(), m_param.ra_begin_timestamp);
	if (in_activity_time)
	{
		return;
	}

	this->GiveUnFetchReward();
}

void RoleActivityJinYiTianXiang::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
			long long unique_id = 0;
			int ret_num = this->OnBuyItemCheck(unique_id);	//购买定制宝箱	 param1 配置seq，param2 选择的宝箱按位来

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
			this->Draw();
		}
		break;
	case 3:
		{
			this->OnBuyItemByMoneyType();
		}
		break;
	}
}

int RoleActivityJinYiTianXiang::Draw()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityJinYiTianXiangConfig * config = static_cast<const RandActivityJinYiTianXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAJinYiTianXiangConsumeCfg * consume_cfg = config->GetDrawConsumeCfg(m_param.draw_times + 1);  //初始化是0，要加1
	if (consume_cfg == NULL)
	{
		return -__LINE__;
	}

	if (m_param.has_draw_value < consume_cfg->draw_consume_value)
	{
		return -__LINE__;
	}

	const RAJinYiTianXiangItemCfg * cfg = config->GetRandRewardCfg(ramgr, m_param.get_reward_flag);
	if (!cfg)
	{
		return -__LINE__;
	}

	const std::vector<ItemConfigData> & rewards = cfg->rewards;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_JIN_YI_TIAN_XIANG);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}


	m_param.has_draw_value -= consume_cfg->draw_consume_value;
	m_param.draw_times += 1;
	::SetBit(m_param.get_reward_flag, cfg->seq);
	this->SendInfo();

	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d |  role[%d %s] draw_times:%d get_reward_flag:%ud seq:%d has_draw_value:%d draw_consume_value:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.draw_times, m_param.get_reward_flag, cfg->seq, m_param.has_draw_value, consume_cfg->draw_consume_value);

	this->OnParticipateRA(__FUNCTION__);

	if (cfg->is_broadcast && !rewards.empty())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_get_item_broadcast_2, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
			m_role_activity_type, rewards[0].item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	return 0;
}

int RoleActivityJinYiTianXiang::OnBuyItemCheck(long long ARG_OUT & unique_key) const
{
	unique_key = 0;

	const RandActivityJinYiTianXiangConfig * config = static_cast<const RandActivityJinYiTianXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAJinYiTianXiangConsumeCfg * consume_cfg = config->GetDrawConsumeCfg(m_param.draw_times + 1);
	if (consume_cfg == NULL)
	{
		return -__LINE__;
	}

	if (m_param.has_draw_value >= consume_cfg->draw_consume_value)
	{
		return -__LINE__;
	}

	const RAJinYiTianXiangBuyCfg * buy_cfg = config->GetRAJinYiTianXiangBuyCfg(consume_cfg->buy_seq);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (buy_cfg->is_need_chong_zhi == 0)
	{
		return -__LINE__;
	}

	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
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
	buy_item->param1 = buy_cfg->add_draw_value;
	buy_item->param2 = buy_cfg->buy_seq;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_jin_yi_tian_xiang, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityJinYiTianXiang::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	int add_draw_value = buy_item->param1;
	int buy_seq = buy_item->param2;
	m_param.has_draw_value += add_draw_value;

	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u m_param.has_draw_value:%d add_draw_value:%d buy_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, m_param.has_draw_value, add_draw_value, buy_seq);

	this->SetRoleDataChange();
	this->SendInfo();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_jin_yi_tian_xiang, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityJinYiTianXiang::OnBuyItemByMoneyType()
{
	const RandActivityJinYiTianXiangConfig * config = static_cast<const RandActivityJinYiTianXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAJinYiTianXiangConsumeCfg * consume_cfg = config->GetDrawConsumeCfg(m_param.draw_times + 1);
	if (consume_cfg == NULL)
	{
		return -__LINE__;
	}

	if (m_param.has_draw_value >= consume_cfg->draw_consume_value)
	{
		return -__LINE__;
	}

	const RAJinYiTianXiangBuyCfg * buy_cfg = config->GetRAJinYiTianXiangBuyCfg(consume_cfg->buy_seq);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (buy_cfg->is_need_chong_zhi != 0)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetMoney()->UseMoney(buy_cfg->money_type, buy_cfg->money_value, __FUNCTION__))
	{
		return -__LINE__;
	}

	int add_draw_value = buy_cfg->add_draw_value;
	m_param.has_draw_value += add_draw_value;

	this->SetRoleDataChange();
	this->SendInfo();

	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u m_param.has_draw_value:%d add_draw_value:%d buy_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, m_param.has_draw_value, add_draw_value, buy_cfg->buy_seq);

	return 0;
}

int RoleActivityJinYiTianXiang::OnBuyGiftItem(ARG_OUT int * price)
{
	const RandActivityJinYiTianXiangConfig * config = static_cast<const RandActivityJinYiTianXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RAJinYiTianXiangConsumeCfg * consume_cfg = config->GetDrawConsumeCfg(m_param.draw_times + 1);
	if (consume_cfg == NULL)
	{
		return 1;
	}

	if (m_param.has_draw_value >= consume_cfg->draw_consume_value)
	{
		return 1;
	}

	const RAJinYiTianXiangBuyCfg * buy_cfg = config->GetRAJinYiTianXiangBuyCfg(consume_cfg->buy_seq);
	if (buy_cfg == NULL)
	{
		return 1;
	}

	if (buy_cfg->is_need_chong_zhi == 0)
	{
		return 1;
	}

	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}

	int add_draw_value = buy_cfg->add_draw_value;
	m_param.has_draw_value += add_draw_value;

	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u m_param.has_draw_value:%d add_draw_value:%d buy_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, m_param.has_draw_value, add_draw_value, buy_cfg->buy_seq);

	this->SetRoleDataChange();
	this->SendInfo();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

bool RoleActivityJinYiTianXiang::OnUseItem(int add_draw_value)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (!ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_OPEN_ITEM_NOT_USE);
		return false;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_OPEN_ITEM_NOT_USE);
		return false;
	}

	this->AddDrawValue(add_draw_value, __FUNCTION__);
	this->SendInfo();

	return true;
}

void RoleActivityJinYiTianXiang::SendInfo()
{
	Protocol::SCRAJinYiTianXiang protocol;

	protocol.draw_times = m_param.draw_times;
	protocol.get_reward_flag = m_param.get_reward_flag;
	protocol.has_draw_value = m_param.has_draw_value;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityJinYiTianXiang::GiveUnFetchReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityJinYiTianXiangConfig * config = static_cast<const RandActivityJinYiTianXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d |  role[%d %s] draw_times:%d get_reward_flag:%ud  has_draw_value:%d ra_begin_timestamp:%u mark:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.draw_times, m_param.get_reward_flag, m_param.has_draw_value, m_param.ra_begin_timestamp, m_param.mark);

	int max_times = 100;
	for (int i = 0; i < max_times && 0 < m_param.has_draw_value; ++i)
	{
		const RAJinYiTianXiangConsumeCfg * consume_cfg = config->GetDrawConsumeCfg(m_param.draw_times + 1);
		if (consume_cfg == NULL)
		{
			break;
		}

		if (m_param.has_draw_value < consume_cfg->draw_consume_value)
		{
			break;
		}

		const RAJinYiTianXiangItemCfg * cfg = config->GetRandRewardCfgByMarkAndTime(ramgr, m_param.mark, m_param.ra_begin_timestamp, m_param.get_reward_flag);
		if (!cfg)
		{
			break;
		}

		const std::vector<ItemConfigData> & rewards = cfg->rewards;

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_unfetch_reward_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_unfetch_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}

		m_param.has_draw_value -= consume_cfg->draw_consume_value;
		m_param.draw_times += 1;
		::SetBit(m_param.get_reward_flag, cfg->seq);

		gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d |  role[%d %s] draw_times:%d get_reward_flag:%ud seq:%d has_draw_value:%d draw_consume_value:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			m_param.draw_times, m_param.get_reward_flag, cfg->seq, m_param.has_draw_value, consume_cfg->draw_consume_value);
	}

	m_param.has_draw_value = 0;
	m_param.draw_times = 0;

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityJinYiTianXiang::AddDrawValue(int add_draw_value, const char * reason)
{
	if (!reason)
	{
		reason = "";
	}

	m_param.has_draw_value += add_draw_value;

	gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u m_param.has_draw_value:%d add_draw_value:%d %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, m_param.has_draw_value, add_draw_value, reason);
}
