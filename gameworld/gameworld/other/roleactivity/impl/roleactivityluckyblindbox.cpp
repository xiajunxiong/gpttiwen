#include "roleactivityluckyblindbox.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "config/randactivityconfig/impl/randactivityluckyblindboxconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

RoleActivityLuckyBlindBox::RoleActivityLuckyBlindBox(int type) :RoleActivity(type)
{
}

RoleActivityLuckyBlindBox::~RoleActivityLuckyBlindBox()
{
}

void RoleActivityLuckyBlindBox::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALuckyBlindBoxParam));
}

void RoleActivityLuckyBlindBox::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RALuckyBlindBoxParam));
}


void RoleActivityLuckyBlindBox::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	this->RASendInfo();
}

void RoleActivityLuckyBlindBox::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.OnDayChange();
	this->RASendInfo();
}

void RoleActivityLuckyBlindBox::Update(time_t now_second)
{
	RoleActivity::Update(now_second);

	if (m_param.bax_refresh_tamp > 0 && m_param.bax_refresh_tamp <= now_second)
	{
		memset(m_param.box_flag, -1, sizeof(m_param.box_flag));
		m_param.bax_refresh_tamp = 0;
		this->RASendInfo();
	}
}


void RoleActivityLuckyBlindBox::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityLuckyBlindBox::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:	// 开启盲盒
		{
			this->RandBox(raor->param_1, raor->param_2);
		}
		break;
	case 2://累计充值领取
		{
			this->OnAchiment(raor->param_1);
		}
		break;
	case 3://直购
		{
			this->BuyBox(raor);
		}
	break;
	}
}

void RoleActivityLuckyBlindBox::RandBox(int box_x, int box_y)
{
	if (box_x - 1 < 0 || box_x - 1 >= MAX_LUCKY_BLIND_BOX_X)return;
	if (box_y - 1 < 0 || box_y - 1 >= MAX_LUCKY_BLIND_BOX_Y)return;
	if (m_param.box_flag[box_x - 1][box_y - 1] >= 0)return;

	const RandActivityLuckyBlindBoxConfig * config = static_cast<const RandActivityLuckyBlindBoxConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX));
	if (NULL == config)return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	const LuckyBlindBoxBaseCfg * base_cfg = config->GetBaseCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX));
	if (NULL == base_cfg) return;
	bool floor_back = false;
	const LuckyBlindBoxRewardCfg * reward_cfg = config->RandRewardCfg(base_cfg->reward_group, m_param.box_rand_times + 1, &m_param.floor_bitmap, &floor_back);
	if (NULL == reward_cfg) return;
	const LuckyBlindBoxOtherCfg & other_cfg = config->GetOtherCfg();

	if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.cost_item.item_id,other_cfg.cost_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_RA_LUCKY_BLIND_BOX_REWARD))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.cost_item.item_id, other_cfg.cost_item.num, __FUNCTION__))
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d,%s] ConsumeItem err item_data[%d,%d]", __FUNCTION__, m_role_module_mgr->GetUid(),
			m_role_module_mgr->GetRoleName(), other_cfg.cost_item.item_id, other_cfg.cost_item.num);
		return;
	}

	m_param.box_flag[box_x - 1][box_y - 1] = (char)reward_cfg->seq;
	if (reward_cfg->guarantee_times > 0)
	{
		m_param.floor_bitmap.SetBit(reward_cfg->seq);//设置保底已出现标记
	}
	m_param.box_rand_times += 1;//保底次数
	if (floor_back)
	{
		//m_param.box_rand_times = 0;
	}
	if (reward_cfg->hearsay == 1)
	{
		//传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_reward_borcast, m_role_module_mgr->GetRole()->GetUID(),
			m_role_module_mgr->GetRole()->GetName(), m_role_activity_type, reward_cfg->reward.item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	this->CheckBox();
	this->RASendInfo();
}

int RoleActivityLuckyBlindBox::OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver, int seq)
{
	unique_key = 0;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityLuckyBlindBoxConfig * config = static_cast<const RandActivityLuckyBlindBoxConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const LuckyBlindBoxShopCfg * buy_cfg = config->GetShopCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX), seq);
	if (NULL == buy_cfg)
	{
		return -__LINE__;
	}
	switch (buy_cfg->limit_type)
	{
	case LuckyBlindBoxShopCfg::LIMIT_TYPE_ENUM_1:
		if (m_param.recharge_buy_times[buy_cfg->seq] >= buy_cfg->limit_times)
		{
			return -__LINE__;
		}
		
		break;
	case LuckyBlindBoxShopCfg::LIMIT_TYPE_ENUM_2:
		if (m_param.today_recharge_buy_times[buy_cfg->seq] >= buy_cfg->limit_times)
		{
			return -__LINE__;
		}
		
		break;
	default:
		break;
	}

	const std::vector<ItemConfigData> & rewards = buy_cfg->reward_vec;
	if (rewards.empty())
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

	if (config->GetConfigVer() != cfg_ver)
	{
		return -__LINE__;
	}

	buy_item->param1 = seq;
	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = buy_cfg->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	::ItemConfigDataVecToBuyItem(rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_lucky_blind_box, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_lucky_blind_box.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityLuckyBlindBox::OnCmdBuy(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_lucky_blind_box.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
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

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_LUCKY_BLIND_EXTRA_REWARD);
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

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityLuckyBlindBoxConfig * config = static_cast<const RandActivityLuckyBlindBoxConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const LuckyBlindBoxShopCfg * buy_cfg = config->GetShopCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX), buy_item->param1);
	if (NULL == buy_cfg)
	{
		return -__LINE__;
	}

	switch (buy_cfg->limit_type)
	{
	case LuckyBlindBoxShopCfg::LIMIT_TYPE_ENUM_1:
		if (m_param.shop_buy_times[buy_cfg->seq] >= buy_cfg->limit_times)
		{
			return -__LINE__;
		}

		break;
	case LuckyBlindBoxShopCfg::LIMIT_TYPE_ENUM_2:
		if (m_param.today_shop_buy_times[buy_cfg->seq] >= buy_cfg->limit_times)
		{
			return -__LINE__;
		}

		break;
	default:
		break;
	}
	m_param.shop_buy_times[buy_cfg->seq] += 1;
	m_param.today_shop_buy_times[buy_cfg->seq] += 1;
	this->SetRoleDataChange();
	this->RASendInfo();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_lucky_blind_box, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityLuckyBlindBox::OnBuyGiftItem(int seq, int ARG_OUT * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityLuckyBlindBoxConfig * config = static_cast<const RandActivityLuckyBlindBoxConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const LuckyBlindBoxShopCfg * buy_cfg = config->GetShopCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX), seq);
	if (NULL == buy_cfg)
	{
		return -__LINE__;
	}

	switch (buy_cfg->limit_type)
	{
	case LuckyBlindBoxShopCfg::LIMIT_TYPE_ENUM_1:
		if (m_param.recharge_buy_times[buy_cfg->seq] >= buy_cfg->limit_times)
		{
			return -__LINE__;
		}

		break;
	case LuckyBlindBoxShopCfg::LIMIT_TYPE_ENUM_2:
		if (m_param.today_recharge_buy_times[buy_cfg->seq] >= buy_cfg->limit_times)
		{
			return -__LINE__;
		}

		break;
	default:
		break;
	}


	if (price)
	{
		*price = buy_cfg->price;
		return 0;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg->reward_vec.size(), &buy_cfg->reward_vec[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)buy_cfg->reward_vec.size(), &buy_cfg->reward_vec[0], PUT_REASON_RA_LUCKY_BLIND_EXTRA_REWARD);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, buy_cfg->reward_vec);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_param.shop_buy_times[buy_cfg->seq] += 1;
	m_param.today_shop_buy_times[buy_cfg->seq] += 1;
	this->SetRoleDataChange();
	this->RASendInfo();

	return 0;
}

void RoleActivityLuckyBlindBox::OnAchiment(int seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)return;

	const RandActivityLuckyBlindBoxConfig * config = static_cast<const RandActivityLuckyBlindBoxConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)return;

	const LuckyBlindBoxRechargeCfg * recharge_cfg = config->GetRechargeCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX), seq);

	if (NULL == recharge_cfg)return;

	if (recharge_cfg->add_recharge > m_param.day_chongzhi)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_CHONGZHI_NOT_ENOUGH);
		return;
	}
	switch (recharge_cfg->get_type)
	{
	case LuckyBlindBoxRechargeCfg::LIMIT_TYPE_ENUM_1:
		if (m_param.recharge_buy_times[recharge_cfg->seq] >= recharge_cfg->get_times)
		{
			return;
		}
		break;
	case LuckyBlindBoxRechargeCfg::LIMIT_TYPE_ENUM_2:
		if (m_param.today_recharge_buy_times[recharge_cfg->seq] >= recharge_cfg->get_times)
		{
			return;
		}
		break;
	default:
		break;
	}
	m_param.recharge_buy_times[recharge_cfg->seq] += 1;
	m_param.today_recharge_buy_times[recharge_cfg->seq] += 1;
	if (!m_role_module_mgr->GetKnapsack()->PutList((short)recharge_cfg->reward_vec.size(), &recharge_cfg->reward_vec[0], PUT_REASON_RA_LUCKY_BLIND_EXTRA_REWARD))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	this->RASendInfo();
}

void RoleActivityLuckyBlindBox::BuyBox(Protocol::CSRandActivityOperaReq * raor)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityLuckyBlindBoxConfig * config = static_cast<const RandActivityLuckyBlindBoxConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const LuckyBlindBoxShopCfg * buy_cfg = config->GetShopCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX), raor->param_1);
	if (NULL == buy_cfg)
	{
		return;
	}

	if (buy_cfg->cfg_or == 1)
	{
		long long unique_id = 0;
		int ret_num = this->OnBuyItemCheck(unique_id, raor->cfg_ver, raor->param_1);	// param1 配置seq

		if (ret_num == 0)
		{
			Protocol::SCCmdBuyItemCheckRet protocol;
			protocol.unique_id = unique_id;
			protocol.cfg_ver = raor->cfg_ver;
			protocol.buy_type = BUY_TYPE;

			m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		}
		return;
	}

	if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(buy_cfg->price_type, buy_cfg->price))
		return;

	if (!m_role_module_mgr->GetMoney()->UseMoneyType(buy_cfg->price_type, buy_cfg->price, __FUNCTION__))
		return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg->reward_vec.size(), &buy_cfg->reward_vec[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)buy_cfg->reward_vec.size(), &buy_cfg->reward_vec[0], PUT_REASON_RA_LUCKY_BLIND_EXTRA_REWARD);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, buy_cfg->reward_vec);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_param.shop_buy_times[buy_cfg->seq] += 1;
	m_param.today_shop_buy_times[buy_cfg->seq] += 1;
	this->SetRoleDataChange();
	this->RASendInfo();
}

void RoleActivityLuckyBlindBox::OnAddChongZhi(long long chongzhi)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX))
	{
		return;
	}

	m_param.day_chongzhi += chongzhi;
	this->RASendInfo();
}

void RoleActivityLuckyBlindBox::RASendInfo()
{
	Protocol::SCRALuckyBlindBoxInfo msg;
	msg.chongzhi = m_param.day_chongzhi;
	memcpy(msg.box_flag, m_param.box_flag, sizeof(m_param.box_flag));
	memcpy(msg.today_shop_buy_times, m_param.today_shop_buy_times, sizeof(m_param.today_shop_buy_times));
	memcpy(msg.shop_buy_times, m_param.shop_buy_times, sizeof(m_param.shop_buy_times));
	memcpy(msg.today_recharge_buy_times, m_param.today_recharge_buy_times, sizeof(m_param.today_recharge_buy_times));
	memcpy(msg.recharge_buy_times, m_param.recharge_buy_times, sizeof(m_param.recharge_buy_times));
	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityLuckyBlindBox::CheckBox()
{
	for (int x = 0; x < MAX_LUCKY_BLIND_BOX_X; x++)
	{
		for (int y = 0; y < MAX_LUCKY_BLIND_BOX_Y; y++)
		{
			if (m_param.box_flag[x][y] < 0)return;
		}
	}

	m_param.bax_refresh_tamp = (unsigned int)EngineAdapter::Instance().Time() + 5;
}
