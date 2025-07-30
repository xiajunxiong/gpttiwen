#include "roleactivitytianyiyaoshi.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitytianyiyaoshiconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "engineadapter.h"
#include "protocol/randactivity/msgrahunqifanli.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "servercommon/string/globalstr.h"
#include "servercommon/string/gameworldstr.h"

RoleActivityTianYiYaoShi::RoleActivityTianYiYaoShi(int type) :RoleActivity(type)
{
}

RoleActivityTianYiYaoShi::~RoleActivityTianYiYaoShi()
{
}

void RoleActivityTianYiYaoShi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RATianYiYaoShiParam));
}

void RoleActivityTianYiYaoShi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RATianYiYaoShiParam));
}

void RoleActivityTianYiYaoShi::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityTianYiYaoShi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTianYiYaoShiConfig * config = static_cast<const RandActivityTianYiYaoShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	for (int i = 0; i < MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE; i++)
	{
		const RATianYiYaoShiShopCfg* shop_cfg = config->GetShopCfg(ramgr, i);
		if (NULL == shop_cfg) continue;

		if (shop_cfg->limit_type == RA_TIAN_YI_YAO_SHI_BUY_LIMIT_TODAY)
		{
			m_param.m_buy_count[i] = 0;
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTianYiYaoShi::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityTianYiYaoShi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor) return;

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
	case Protocol::RA_TIAN_YI_YAO_SHI_REQ_TYPE_INFO:
		{
			this->RASendInfo();
		}
		break;
	case Protocol::RA_TIAN_YI_YAO_SHI_REQ_TYPE_LOTTERY:
		{
			this->OnLottery(1);
		}
		break;
	case Protocol::RA_TIAN_YI_YAO_SHI_REQ_TYPE_TEN_LOTTERY:
		{
			this->OnLottery(10);
		}
		break;
	case Protocol::RA_TIAN_YI_YAO_SHI_REQ_TYPE_BUY:
		{
			this->BuyGiftBox(raor->param_1, raor->cfg_ver);
		}
		break;
	}
}

int RoleActivityTianYiYaoShi::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE)
	{
		return -1;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return -2;

	const RandActivityTianYiYaoShiConfig * config = static_cast<const RandActivityTianYiYaoShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return -3;

	const RATianYiYaoShiShopCfg* shop_cfg = config->GetShopCfg(ramgr, seq);
	if (NULL == shop_cfg)
	{
		return -4;
	}

	if (!shop_cfg->cfg_or)
		return -5;

	if (m_param.m_buy_count[seq] >= shop_cfg->buy_times)
		return -6;

	if (NULL == ITEMPOOL->GetItem(shop_cfg->reward.item_id) || shop_cfg->reward.num <= 0)
		return -7;

	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -8;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -9;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = shop_cfg->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;

	{
		std::vector<ItemConfigData> t_vec;
		t_vec.push_back(shop_cfg->reward);
		::ItemConfigDataVecToBuyItem(t_vec, buy_item->rewards_item, &buy_item->rewards_num);
	}
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_tian_yi_yao_shi, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_tian_yi_yao_shi.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityTianYiYaoShi::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_tian_yi_yao_shi.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
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

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_TIAN_YI_YAO_SHI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(rewards, SEND_MAIL_TYPE_ACTIVITY_BUY_BAG_FULL, false, RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);
	}

	if (seq >= 0 && seq < MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE)
	{
		m_param.m_buy_count[seq] += 1;

		this->RASendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_tian_yi_yao_shi, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	std::string log_str;
	for (size_t i = 0; i < rewards.size(); i++)
	{
		log_str += STRING_SPRINTF("item[%d, %d] ", rewards[i].item_id, rewards[i].num);
	}
	gamelog::g_log_tian_yi_yao_shi.printf(LL_INFO, "%s role[%d, %s] seq[%d] m_buy_count:seq[%d] reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, m_param.m_buy_count[seq], log_str.c_str());
	return 0;
}

int RoleActivityTianYiYaoShi::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityTianYiYaoShiConfig * config = static_cast<const RandActivityTianYiYaoShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RATianYiYaoShiShopCfg* shop_cfg = config->GetShopCfg(ramgr, seq);
	if (shop_cfg == NULL)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (NULL != price)
	{
		*price = shop_cfg->price;
		return 0;
	}

	if (m_param.m_buy_count[seq] >= shop_cfg->buy_times)
		return BUY_GIFT_ITEM_RET_TYPE_2;


	if (shop_cfg->reward.num <= 0)
	{
		return 1;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(shop_cfg->reward))
	{
		m_role_module_mgr->GetKnapsack()->Put(shop_cfg->reward, PUT_REASON_RA_TIAN_YI_YAO_SHI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&shop_cfg->reward, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_BAG_FULL, false, RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);
	}

	m_param.m_buy_count[seq] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_tian_yi_yao_shi.printf(LL_INFO, "%s role[%d, %s] seq[%d] m_buy_count:seq[%d] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), seq, m_param.m_buy_count[seq], shop_cfg->reward.item_id, shop_cfg->reward.num);
	return 0;
}

void RoleActivityTianYiYaoShi::OnLottery(int lottery_times)
{
	if (lottery_times != 1 && lottery_times != 10) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTianYiYaoShiConfig * config = static_cast<const RandActivityTianYiYaoShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(config->GetOtherCfg().consume_item, lottery_times))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	std::vector<bool> is_hearsays;
	std::vector<ItemConfigData> reward_list;
	for (int i = 0; i < lottery_times; i++)
	{
		const RATianYiYaoShiBasicCfg* reward_cfg = config->RandRewardCfg(ramgr);
		if (NULL == reward_cfg) break;

		is_hearsays.push_back(reward_cfg->is_hearsay);
		reward_list.push_back(reward_cfg->reward);
	}

	if ((int)is_hearsays.size() != lottery_times || (int)reward_list.size() != lottery_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_list))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg().consume_item, lottery_times, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	std::string log_str;
	for (size_t i = 0; i < reward_list.size(); i++)
	{
		log_str += STRING_SPRINTF("[%d, %d] ", reward_list[i].item_id, reward_list[i].num);
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(reward_list, PUT_REASON_NO_NOTICE))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);
	}
	m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(reward_list, GET_REASON_TIAN_YI_YAO_SHI);

	for (size_t i = 0; i < is_hearsays.size(); i++)
	{
		if(!is_hearsays[i]) continue;

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_reward_borcast2, m_role_module_mgr->GetRole()->GetUID(),
			m_role_module_mgr->GetRole()->GetName(), m_role_activity_type, reward_list[i].item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	for (int vec_index = 0; vec_index < (int)reward_list.size(); vec_index++)
	{
		RATianYiYaoShiLOG lottery_log;
		lottery_log.lottery_timestamp = (uint32_t)EngineAdapter::Instance().Time();
		lottery_log.item_id = reward_list[vec_index].item_id;
		lottery_log.num = reward_list[vec_index].num;

		m_param.m_lottery_log[m_param.cur_log_tail++] = lottery_log;	

		if (m_param.cur_log_tail >= MAX_RA_TIAN_YI_YAO_SHI_LOG)
		{
			m_param.cur_log_tail = 0;
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_tian_yi_yao_shi.printf(LL_INFO, "%s role[%d, %s] lottery_times[%d] reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), lottery_times, log_str.c_str());
}

void RoleActivityTianYiYaoShi::BuyGiftBox(int seq, int cfg_ver)
{
	if (seq < 0 || seq >= MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTianYiYaoShiConfig * config = static_cast<const RandActivityTianYiYaoShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const RATianYiYaoShiShopCfg* shop_cfg = config->GetShopCfg(ramgr, seq);
	if (NULL == shop_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.m_buy_count[seq] >= shop_cfg->buy_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
		return;
	}

	if (shop_cfg->cfg_or)
	{
		long long unique_id = 0;
		int ret_num = this->OnBuyItemCheck(seq, unique_id);

		if (ret_num == 0)
		{
			Protocol::SCCmdBuyItemCheckRet protocol;
			protocol.unique_id = unique_id;
			protocol.cfg_ver = cfg_ver;
			protocol.buy_type = BUY_TYPE;

			m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		}
		return;
	}

	if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(shop_cfg->price_type, shop_cfg->price))
		return;

	if (!m_role_module_mgr->GetMoney()->UseMoneyType(shop_cfg->price_type, shop_cfg->price, __FUNCTION__))
		return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(shop_cfg->reward))
	{
		m_role_module_mgr->GetKnapsack()->Put(shop_cfg->reward, PUT_REASON_RA_TIAN_YI_YAO_SHI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&shop_cfg->reward, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_BAG_FULL, false, RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);
	}

	m_param.m_buy_count[seq]++;
	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_tian_yi_yao_shi.printf(LL_INFO, "%s role[%d, %s] price_type[%d] price[%d] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), shop_cfg->price_type, shop_cfg->price, shop_cfg->reward.item_id, shop_cfg->reward.num);
}

void RoleActivityTianYiYaoShi::RASendInfo()
{
	static Protocol::SCRATianYiYaoShiInfo msg;
	msg.cur_log_tail = m_param.cur_log_tail;
	memcpy(msg.m_buy_count, m_param.m_buy_count, sizeof(msg.m_buy_count));
	memcpy(msg.m_lottery_log, m_param.m_lottery_log, sizeof(msg.m_lottery_log));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}