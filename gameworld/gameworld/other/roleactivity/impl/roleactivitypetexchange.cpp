#include "roleactivitypetexchange.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitypetexchangeconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/pet/petconfig.hpp"
#include "protocol/randactivity/msgrapetexchange.hpp"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include "world.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"



RoleActivityPetExchange::RoleActivityPetExchange(int type)
	:RoleActivity(type)
{

}

RoleActivityPetExchange::~RoleActivityPetExchange()
{

}

void RoleActivityPetExchange::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAPetExchangeParam));
}

void RoleActivityPetExchange::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAPetExchangeParam));
}

void RoleActivityPetExchange::OnLoginSendInfo()
{
	this->SendRAPetExchangeInfo();
}

void RoleActivityPetExchange::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	const RandActivityPetExchangeConfig * config = static_cast<const RandActivityPetExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL != config)
	{
		const RAPetExchangeShopSectionCfg * section_cfg = config->GetShopSectionCfg(ramgr);
		if (NULL != section_cfg)
		{
			for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.ra_shop_buy_times) && i < (int)section_cfg->shop_list.size(); ++i)
			{
				if (ACTVITY_BUY_LIMIT_TYPE_DAY == section_cfg->shop_list[i].limit_type)
				{
					m_param.ra_shop_buy_times[i] = 0;
				}
			}
		}
	}

	memset(m_param.ra_pet_exchange_day_times, 0, sizeof(m_param.ra_pet_exchange_day_times));
	this->SetRoleDataChange();
	this->SendRAPetExchangeInfo();
}

void RoleActivityPetExchange::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_PET_EXCHANGE != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_pet_exchange_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_pet_exchange_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityPetExchange::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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
	case Protocol::RA_PET_EXCHANGE_OPERA_TYPE_GET_INFO:
		{
			this->SendRAPetExchangeInfo();
		}
		break;
	case Protocol::RA_PET_EXCHANGE_OPERA_TYPE_EXCHANGE:
		{
			this->OnActivityPetExchange(raor->param_1);
		}
		break;
	case Protocol::RA_PET_EXCHANGE_OPERA_TYPE_BUY_ITEM:
		{
			this->OnBuyItem(raor->param_1);	
		}
		break;	
	case Protocol::RA_PET_EXCHANGE_OPERA_TYPE_BUY_ITEM_CHECK:
		{
			this->OnBuyItemChecek(raor->param_1);
		}
		break;
	default:
		break;
	}
}

int RoleActivityPetExchange::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_pet_exchange_begin_timestamp, ::GetString(*buy_item).c_str());

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

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		if (m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_PET_EXCHANGE_BUY_ITEM))
		{
			is_put_succ = true;
		}
	}
	if(!is_put_succ)
	{
		// 发邮件
		m_role_module_mgr->GetKnapsack()->SendMail(rewards, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, false, m_role_activity_type);
	}

	int & seq = buy_item->param1;
	int & buy_limit = buy_item->param2;
	unsigned int buy_zero_timestamp = (unsigned int)buy_item->param3;
	if (m_param.ra_pet_exchange_begin_timestamp == buy_item->ra_begin_timestamp)
	{
		if (seq >= 0 && seq < MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM)
		{
			if (ACTVITY_BUY_LIMIT_TYPE_DAY == buy_limit)
			{
				if ((unsigned int)GetZeroTime(EngineAdapter::Instance().Time()) == buy_zero_timestamp)
				{
					m_param.ra_shop_buy_times[seq] += 1;
				}
			}
			else
			{
				m_param.ra_shop_buy_times[seq] += 1;
			}
			this->OnSendSingleInfo(Protocol::SCRAPetExchangeSingleInfo::INFO_TYPE_BUY_ITEM, seq, m_param.ra_shop_buy_times[seq]);
			this->SetRoleDataChange();
		}
	}
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_qi_xi_te_hui, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityPetExchange::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityPetExchangeConfig * config = static_cast<const RandActivityPetExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RAPetExchangeShopCfg * buy_item_cfg = config->GetShopBySeq(ramgr, seq);
	if (NULL == buy_item_cfg || buy_item_cfg->price <= 0 || !buy_item_cfg->is_cmd_buy)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	//seq已经在配置读取判断过了
	int & buy_times = m_param.ra_shop_buy_times[buy_item_cfg->seq];
	if (buy_times > 0 && buy_times >= buy_item_cfg->buy_times)
	{
		return BUY_GIFT_ITEM_RET_TYPE_2;
	}

	if (NULL != price)
	{
		*price = buy_item_cfg->price;
		return 0;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num, false))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_RA_PET_EXCHANGE_BUY_ITEM))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, m_role_activity_type);
	}


	++buy_times;
	this->OnSendSingleInfo(Protocol::SCRAPetExchangeSingleInfo::INFO_TYPE_BUY_ITEM, buy_item_cfg->seq, buy_times);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityPetExchange::SendRAPetExchangeInfo()
{
	Protocol::SCRAActivityPetExchangeInfo info;

	UNSTD_STATIC_CHECK(sizeof(info.ra_pet_exchange_times) == sizeof(m_param.ra_pet_exchange_times));
	UNSTD_STATIC_CHECK(sizeof(info.ra_pet_exchange_today_times) == sizeof(m_param.ra_pet_exchange_day_times));
	UNSTD_STATIC_CHECK(sizeof(info.ra_shop_buy_times) == sizeof(m_param.ra_shop_buy_times));
	memcpy(info.ra_pet_exchange_times, m_param.ra_pet_exchange_times, sizeof(info.ra_pet_exchange_times));
	memcpy(info.ra_pet_exchange_today_times, m_param.ra_pet_exchange_day_times, sizeof(info.ra_pet_exchange_today_times));
	memcpy(info.ra_shop_buy_times, m_param.ra_shop_buy_times, sizeof(info.ra_shop_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityPetExchange::OnSendSingleInfo(int type, int seq, int param1, int param2)
{
	Protocol::SCRAPetExchangeSingleInfo info;
	info.type = type;
	info.seq = seq;
	info.param1 = param1;
	info.param2 = param2;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityPetExchange::OnActivityPetExchange(int seq)
{
	if (seq < 0 || seq >= MAX_RA_PET_NUM) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityPetExchangeConfig * config = static_cast<const RandActivityPetExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAPetExchangeCfg* cfg = config->GetPetExchangeBySeq(ramgr, seq);
	if (cfg == NULL)return;

	unsigned short & has_buy_times_today = m_param.ra_pet_exchange_day_times[seq];
	unsigned short & has_buy_times_activity = m_param.ra_pet_exchange_times[seq];
	switch (cfg->limit_type)
	{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (has_buy_times_activity > 0 && has_buy_times_activity >= cfg->daily_purchase)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR);
				return;
			}
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (has_buy_times_today > 0 && has_buy_times_today >= cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR);
				return;
			}
		}
		break;
		default:
		break;
	}

	if (0 != cfg->open_level_limit && m_role_module_mgr->GetRole()->GetLevel() < cfg->open_level_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->reward_item.item_id, cfg->reward_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int i = 0; i < (int)cfg->consume_item_list.size() && stuff_count < ARRAY_LENGTH(stuff_list); ++i)
	{
		stuff_list[stuff_count].item_id = cfg->consume_item_list[i].item_id;
		stuff_list[stuff_count].num = cfg->consume_item_list[i].num;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
	}
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR);
			return;
		}

		if (consume_list.count > 0)
		{
			if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR);
				return;
			}
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->reward_item, PUT_REASON_RA_PET_EXCHANGE))
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnActivityPetExchange err[uid:%d,name:%s] Put[item_id:%d,num:%d,is_bind:%d]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
			cfg->reward_item.item_id, cfg->reward_item.num, cfg->reward_item.is_bind ? 1 : 0);
		return;
	}

	++has_buy_times_today;
	++has_buy_times_activity;
	this->SetRoleDataChange();
	this->OnSendSingleInfo(Protocol::SCRAPetExchangeSingleInfo::INFO_TYPE_EXCHANGE, seq, has_buy_times_activity, has_buy_times_today);

	const PetCfg *pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(cfg->reward_item.item_id);
	if (cfg->hearsay == 1 && pet_cfg != NULL && !CrossConfig::Instance().IsHiddenServer())
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_pet_exchange_notice,
			m_role_module_mgr->GetRole()->GetName(), pet_cfg->quality, cfg->reward_item.item_id, RAND_ACTIVITY_TYPE_PET_EXCHANGE);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				m_role_module_mgr->NetSend((const char*)&sm, sendlen);
			}
		}
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityPetExchange::OnBuyItem(int seq)
{
	if (seq < 0 || seq >= MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityPetExchangeConfig * config = static_cast<const RandActivityPetExchangeConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RAPetExchangeShopCfg *buy_item_cfg = config->GetShopBySeq(ramgr, seq);
	if (NULL == buy_item_cfg || buy_item_cfg->is_cmd_buy)
	{
		return;
	}

	int & buy_times = m_param.ra_shop_buy_times[buy_item_cfg->seq];
	if (buy_times > 0 && buy_times >= buy_item_cfg->buy_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
		return;
	}

	//检查消耗
	if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(buy_item_cfg->price_type, buy_item_cfg->price))
	{
		return;
	}

	if (!m_role_module_mgr->GetMoney()->UseMoneyType(buy_item_cfg->price_type, buy_item_cfg->price, __FUNCTION__))
	{
		return;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num, false))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_RA_PET_EXCHANGE_BUY_ITEM))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, false, m_role_activity_type);
	}
	++buy_times;
	this->OnSendSingleInfo(Protocol::SCRAPetExchangeSingleInfo::INFO_TYPE_BUY_ITEM, buy_item_cfg->seq, buy_times);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		m_role_activity_type, seq);
}

void RoleActivityPetExchange::OnBuyItemChecek(int seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityPetExchangeConfig * config = static_cast<const RandActivityPetExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAPetExchangeShopCfg * buy_item_cfg = config->GetShopBySeq(ramgr, seq);
	if (NULL == buy_item_cfg || buy_item_cfg->price <= 0 || !buy_item_cfg->is_cmd_buy)
	{
		return;
	}

	//seq已经在配置读取判断过了
	int & buy_times = m_param.ra_shop_buy_times[buy_item_cfg->seq];
	if (buy_times > 0 && buy_times >= buy_item_cfg->buy_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return;
	}
	
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&buy_item->unique_id))
	{
		return;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = buy_item_cfg->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_pet_exchange_begin_timestamp;
	buy_item->param1 = seq;
	buy_item->param2 = buy_item_cfg->limit_type;
	buy_item->param3 = (int)GetZeroTime(EngineAdapter::Instance().Time());
	buy_item->rewards_item[0].item_id = buy_item_cfg->reward_item.item_id;
	buy_item->rewards_item[0].num = buy_item_cfg->reward_item.num;
	buy_item->rewards_item[0].is_bind = buy_item_cfg->reward_item.is_bind;
	buy_item->rewards_num = 1;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TYPE;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = buy_item->unique_id;
	m_role_module_mgr->NetSend(&info, sizeof(info));

}
