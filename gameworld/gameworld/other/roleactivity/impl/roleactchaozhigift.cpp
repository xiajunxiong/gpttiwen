#include "roleactchaozhigift.hpp"
#include "config/randactivityconfig/impl/rachaozhigiftconfig.hpp"

#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "obj/character/role.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleACTChaoZhiGift::RoleACTChaoZhiGift(int type)
	: RoleActivity(type)
{
}

RoleACTChaoZhiGift::~RoleACTChaoZhiGift()
{
}

void RoleACTChaoZhiGift::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RoleRAChaoZhiGiftParam));
}

void RoleACTChaoZhiGift::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RoleRAChaoZhiGiftParam));
}

void RoleACTChaoZhiGift::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleACTChaoZhiGift::OnLoginCheck()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{//跨服短时间离线会直接登录开服 屏蔽掉跨服  
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	this->CheckGive(m_param.ra_begin_timestamp);
}

void RoleACTChaoZhiGift::CheckInitRandActivity(int rand_act_type, bool is_force)
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
		this->CheckGive(m_param.ra_begin_timestamp);

		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(ramgr->GetRandActivityBeginTime(m_role_activity_type));

		this->SetRoleDataChange();
	}
}

void RoleACTChaoZhiGift::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
	case RA_CHAO_ZHI_GIFT_REQ_TYPE_CMD_BUY:
		{
			long long unique_id = 0;
			int ret_num = this->BuyItemCheck(raor->cfg_ver, unique_id);
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
	case RA_CHAO_ZHI_GIFT_REQ_TYPE_FETCH:
		{
			this->FetchGift(raor->param_1);
		}
		break;
	default:
		return;
	}

}

void RoleACTChaoZhiGift::OnRAClose()
{
	this->CheckGive(m_param.ra_begin_timestamp);
}

int RoleACTChaoZhiGift::BuyItemCheck(int cfg_ver, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (0 != m_param.gift_act_flag)
	{
		return -1;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -2;
	}

	const RAChaoZhiGiftConfig * config = static_cast<const RAChaoZhiGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -3;
	}

	const RAChaoZhiGift::OtherCfg &other_cfg = config->GetOtherCfg();

	if (other_cfg.cfg_ver != cfg_ver)
	{
		return -5;
	}

	//允许购买 活动开启天数限制
	int act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (other_cfg.can_buy_day < act_open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_IS_END);
		return -101;
	}


	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return -6;
	}

	const RAChaoZhiGift::CmdBuyCfg *cmd_buy_cfg = config->GetCmdBuyCfg(ramgr);
	if (NULL == cmd_buy_cfg)
	{
		return -7;
	}

	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -8;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = cmd_buy_cfg->buy_money;
	buy_item->create_time = (unsigned int)EngineAdapter::Instance().Time();
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;

	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleACTChaoZhiGift::BuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -1;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u  %s",
		__FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -2;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -3;
	}

	if (0 != m_param.gift_act_flag)
	{
		return -4;
	}

	//记录
	m_param.gift_act_flag = 1;
	this->SetRoleDataChange();
	this->SendInfo();

	//日志
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s succ |  role[%d %s] gift_act_flag[%d].",
		__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.gift_act_flag);

	//注意 buy_item 野指针警告 Save必须放最后
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleACTChaoZhiGift::OnBuyGiftItem(ARG_OUT int * price)
{
	if (0 != m_param.gift_act_flag)
	{
		return 2;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}
	const RAChaoZhiGiftConfig * config = static_cast<const RAChaoZhiGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RAChaoZhiGift::CmdBuyCfg *cmd_buy_cfg = config->GetCmdBuyCfg(ramgr);
	if (NULL == cmd_buy_cfg)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = cmd_buy_cfg->buy_money;
		return 0;
	}
	m_param.gift_act_flag = 1;
	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s succ |  role[%d %s] gift_act_flag[%d].",
		__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.gift_act_flag);

	return 0;
}

void RoleACTChaoZhiGift::SendInfo()
{
	Protocol::SCRAChaoZhiGiftInfo info;
	info.gift_act_flag = m_param.gift_act_flag;
	info.count = RAChaoZhiGift::GIFT_MAX;

	for (int i = 0; i < RAChaoZhiGift::GIFT_MAX; i++)
	{
		info.gift_num_list[i] = m_param.gift_info_list[i].fetch_num;
	}

	m_role_module_mgr->NetSend(&info, sizeof(SCRAChaoZhiGiftInfo));
}

void RoleACTChaoZhiGift::FetchGift(int gift_index)
{
	if (1 != m_param.gift_act_flag)
	{//未激活
		m_role_module_mgr->NoticeNum(errornum::EN_RA_CHAO_ZHI_GIFT);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAChaoZhiGiftConfig * config = static_cast<const RAChaoZhiGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAChaoZhiGift::GiftCfg *gift_cfg = config->GetGiftCfg(ramgr, m_param.ra_begin_timestamp, gift_index);
	if (NULL == gift_cfg)
	{
		return;
	}

	//已领
	if (0 != m_param.gift_info_list[gift_index].fetch_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	//判断活动天数
	if (gift_cfg->can_fetch_act_day > config->GetActOpenDay(ramgr, m_role_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_DAY_REWARD_DAY_FOOD_FETCH_NOT_TIME);
		return;
	}

	ItemConfigData reward = gift_cfg->RandRewardItem();
	if (0 == reward.item_id)
	{//未取到奖励
		return;
	}

	//记录
	m_param.gift_info_list[gift_index].fetch_num = reward.num;
	this->SetRoleDataChange();
	this->SendInfo();

	//奖励
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)1, &reward))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)1, &reward, PUT_REASON_RA_CHAO_ZHI_GIFT);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward, (short)1, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, m_role_activity_type);
	}
	
	//日志
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s succ |  role[%d %s] gift_index[%d] reward_num[%d].",
		__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		gift_index, reward.num);

}

void RoleACTChaoZhiGift::CheckGive(unsigned int act_begin_itme)
{
	//未激活
	if (1 != m_param.gift_act_flag)
	{
		return;
	}
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAChaoZhiGiftConfig * config = static_cast<const RAChaoZhiGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}


	for (int i = 0; i < RAChaoZhiGift::GIFT_MAX; i++)
	{
		//已领
		if (0 != m_param.gift_info_list[i].fetch_num)
		{
			continue;
		}

		const RAChaoZhiGift::GiftCfg *gift_cfg = config->GetGiftCfg(ramgr, act_begin_itme, i);
		if (NULL == gift_cfg)
		{
			continue;
		}

		ItemConfigData reward = gift_cfg->RandRewardItem();
		if (0 == reward.item_id)
		{//未取到奖励
			continue;
		}

		//记录
		m_param.gift_info_list[i].fetch_num = reward.num;

		m_role_module_mgr->GetKnapsack()->SendMail(&reward, (short)1, SEND_MAIL_TYPE_ACTIVITY_END, true, m_role_activity_type);

		//日志
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s succ |  role[%d %s] gift_index[%d] reward_num[%d].",
			__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			i, reward.num);

	}

	m_param.gift_act_flag = 2;
	this->SetRoleDataChange();
	//log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] end gift_act_flag:[%d].",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.gift_act_flag);

}
