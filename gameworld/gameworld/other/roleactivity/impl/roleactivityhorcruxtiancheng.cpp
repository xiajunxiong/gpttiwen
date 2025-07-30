#include "roleactivityhorcruxtiancheng.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityhorcruxtianchengconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityhorcruxtiancheng.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"


RoleActivityHorcruxTianCheng::RoleActivityHorcruxTianCheng(int type) :RoleActivity(type)
{
}

RoleActivityHorcruxTianCheng::~RoleActivityHorcruxTianCheng()
{
}

void RoleActivityHorcruxTianCheng::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAHorcruxTianChengParam));
}

void RoleActivityHorcruxTianCheng::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAHorcruxTianChengParam));
}

void RoleActivityHorcruxTianCheng::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG))
		return;

	this->RASendInfo();
}

void RoleActivityHorcruxTianCheng::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG != rand_act_type)
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

void RoleActivityHorcruxTianCheng::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			RandActivityHorcruxTianCheng * act = (RandActivityHorcruxTianCheng *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
			if (NULL == act) return;

			act->Draw(m_role_module_mgr->GetRole());
		}
		break;
	case 2:
		{
			this->RAExchange(raor->param_1);
		}
		break;
	case 3:
		{
			this->RAShop(raor->param_1);
		}
		break;
	case 4:
		{
			long long unique_id = 0;
			int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);
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
	case 5:
		{
			this->RAFetchConsumeReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityHorcruxTianCheng::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG))
	{
		return;
	}

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
	if (NULL == config)
		return;

	const std::vector<RAHorcruxTianChengShopCfg> * shop_map_cfg = config->GetShopMapCfg(ramgr);
	const std::vector<RAHorcruxTianChengExchangeCfg> * exchange_map_cfg = config->GetExchangeMapCfg(ramgr);
	if (NULL == shop_map_cfg || NULL == exchange_map_cfg)
		return;

	for (int i = 0; i < MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX && i < (int)shop_map_cfg->size(); i++)
	{
		if ((*shop_map_cfg)[i].limit_type == 2)
			m_param.shop_list[i] = 0;
	}

	if (config->GetOtherCfg().is_open_exchange)
	{
		for (int i = 0; i < MAX_RA_HORCRUX_TIANCHENG_EXCHANGE_INDEX && i < (int)exchange_map_cfg->size(); i++)
		{
			if ((*exchange_map_cfg)[i].buy_type == 2)
				m_param.exchange_list[i] = 0;
		}
	}

	this->CheckConsumeReward();
	m_param.consume_gold = 0;
	m_param.consume_fetch_mark = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityHorcruxTianCheng::OnConsumeGold(long long consume_gold)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	m_param.consume_gold += consume_gold;

	this->RASendInfo();
	this->SetRoleDataChange();
}

int RoleActivityHorcruxTianCheng::OnBuyGiftItem(int shop_index, ARG_OUT int * price)
{
	if (shop_index < 0 || shop_index >= MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX)
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

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
	if (NULL == config)
	{
		return 1;
	}

	const RAHorcruxTianChengShopCfg * cfg = config->GetShopCfg(ramgr, shop_index);
	if (NULL == cfg)
	{
		return 1;
	}

	if (cfg->price_type != -1)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = cfg->buy_money;
		return 0;
	}

	if (m_param.shop_list[shop_index] >= cfg->limit_times)
	{
		return 2;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_RA_HORCRUX_TIANCHENG);
	}

	m_param.shop_list[shop_index] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		shop_index, m_param.shop_list[shop_index]);

	return 0;
}

int RoleActivityHorcruxTianCheng::OnBuyItemCheck(int shop_index, long long ARG_OUT & unique_key)
{
	if (shop_index < 0 || shop_index >= MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX)
		return -__LINE__;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAHorcruxTianChengShopCfg * cfg = config->GetShopCfg(ramgr, shop_index);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (cfg->price_type != -1)
	{
		return -__LINE__;
	}

	if (m_param.shop_list[shop_index] >= cfg->limit_times)
	{
		return -__LINE__;
	}

	unique_key = 0;
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
	buy_item->need_gold = cfg->buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = shop_index;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityHorcruxTianCheng::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}	

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAHorcruxTianChengShopCfg * cfg = config->GetShopCfg(ramgr, buy_item->param1);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_item.size(), &cfg->reward_item[0], PUT_REASON_RA_HORCRUX_TIANCHENG);
	}

	m_param.shop_list[buy_item->param1] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG,
		buy_item->param1, m_param.shop_list[buy_item->param1]);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

void RoleActivityHorcruxTianCheng::RASendInfo()
{
	Protocol::SCRAHorcruxTianChengRoleInfo info;
	memcpy(info.shop_list, m_param.shop_list, sizeof(info.shop_list));
	memcpy(info.exchange_list, m_param.exchange_list, sizeof(info.exchange_list));
	info.consume_fetch_mark = m_param.consume_fetch_mark;
	info.consume_gold = m_param.consume_gold;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityHorcruxTianCheng::RAExchange(int exchange_index)
{
	if (exchange_index < 0 || exchange_index >= MAX_RA_HORCRUX_TIANCHENG_EXCHANGE_INDEX)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!config->GetOtherCfg().is_open_exchange)
	{
		return;
	}

	const RAHorcruxTianChengExchangeCfg * cfg = config->GetExchangeCfg(ramgr, exchange_index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.exchange_list[exchange_index] >= cfg->limit_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR);
		return;
	}

	if (cfg->gold > 0 && !m_role_module_mgr->GetMoney()->GoldMoreThan(cfg->gold))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	if (cfg->jade > 0 && !m_role_module_mgr->GetMoney()->OtherCoinMoreThan(cfg->jade, MONEY_TYPE_SILVER_COIN))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
		return;
	}

	ItemExtern::ItemConsumeList consume_list;
	if (cfg->consume_item.size() > 0)
	{
		std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
		for (int i = 0; i < (int)cfg->consume_item.size(); ++i)
		{
			ItemExtern::ItemConsumeConfig  stuff_con;
			stuff_con.item_id = cfg->consume_item[i].item_id;
			stuff_con.num = cfg->consume_item[i].num;
			stuff_con.buyable = false;
			stuff_list.push_back(stuff_con);
		}
		
		if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}

	if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
		return;

	if (cfg->gold > 0) m_role_module_mgr->GetMoney()->UseGold(cfg->gold, __FUNCTION__);
	if (cfg->jade > 0) m_role_module_mgr->GetMoney()->UseOtherCoin(cfg->jade, MONEY_TYPE_SILVER_COIN, __FUNCTION__);

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(cfg->exchange_item, false, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->exchange_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->Put(cfg->exchange_item, PUT_REASON_RA_HORCRUX_TIANCHENG);
	}

	m_param.exchange_list[exchange_index] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] exchange_index[%d], exchange_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG,
		exchange_index, m_param.exchange_list[exchange_index]);
}

void RoleActivityHorcruxTianCheng::RAShop(int shop_index)
{
	if (shop_index < 0 || shop_index >= MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAHorcruxTianChengShopCfg * cfg = config->GetShopCfg(ramgr, shop_index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.shop_list[shop_index] >= cfg->limit_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
		return;
	}

	if (cfg->price_type == -1)
	{
		return;
	}
	else if(cfg->price_type == 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(cfg->buy_money, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetMoney()->UseOtherCoin(cfg->buy_money, cfg->price_type, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_item.size(), &cfg->reward_item[0], PUT_REASON_RA_HORCRUX_TIANCHENG);
	}

	m_param.shop_list[shop_index] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG,
		shop_index, m_param.shop_list[shop_index]);
}

void RoleActivityHorcruxTianCheng::RAFetchConsumeReward(int seq)
{
	if(seq < 0)
		return;

	if (0 != (m_param.consume_fetch_mark & (1 << seq)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAHorcruxTianChengConsumeCfg * cfg = config->GetConsumeCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.consume_gold < cfg->consume_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_PARAM_NOT_ADV);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->item.size(), &cfg->item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->item[0], (short)cfg->item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->item, PUT_REASON_RA_HORCRUX_TIANCHENG);
	}

	m_param.consume_fetch_mark |= 1 << seq;

	this->RASendInfo();
	this->SetRoleDataChange();

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		seq);
}

void RoleActivityHorcruxTianCheng::CheckConsumeReward()
{
	if (m_param.consume_gold <= 0)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	int MAX_CFG_SIZE = config->GetConsumeSize(ramgr);
	for (int i = 0; i < MAX_CFG_SIZE; i++)
	{
		if(0 != (m_param.consume_fetch_mark & (1 << i)))
			continue;

		const RAHorcruxTianChengConsumeCfg * cfg = config->GetConsumeCfg(ramgr, i);
		if (NULL == cfg)
			break;

		if (m_param.consume_gold < cfg->consume_num)
			break;

		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->item[0], (short)cfg->item.size(), SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

		m_param.consume_fetch_mark |= 1 << i;

		gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] seq[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type, i);
	}
}
