#include "roleactivityqingyuanshizhuang.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityqingyuanshizhuangconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/qingyuan/qingyuanmanager.h"
#include "other/roleactivity/roleactivitymanager.hpp"

RoleActivityQingYuanShiZhuang::RoleActivityQingYuanShiZhuang(int type) :RoleActivity(type)
{
}

RoleActivityQingYuanShiZhuang::~RoleActivityQingYuanShiZhuang()
{
}

void RoleActivityQingYuanShiZhuang::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAQingYuanShiZhuangParam));
}

void RoleActivityQingYuanShiZhuang::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAQingYuanShiZhuangParam));
}

void RoleActivityQingYuanShiZhuang::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	this->RASendInfo();
}

void RoleActivityQingYuanShiZhuang::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(m_role_module_mgr->GetRole()->GetUID());
		if (NULL != qing_yuan)
		{
			m_param.qing_yuan_id = qing_yuan->GetQingYuanId();

			unsigned int join_times = 0;
			qing_yuan->GetRAQingYuanShiZhuangParam(&join_times, NULL);

			if (join_times < ramgr->GetRandActivityBeginTime(m_role_activity_type) || join_times > ramgr->GetRandActivityEndTime(m_role_activity_type))
			{
				join_times = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				int buy_mark = 0;
				qing_yuan->SetRAQingYuanShiZhuangParam(&join_times, &buy_mark);
			}
		}
		this->SetRoleDataChange();
	}
}

void RoleActivityQingYuanShiZhuang::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->RAShop(raor->param_1);
		}
		break;
	case 2:
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
	case 3:
		{
			this->RAFetchReward(raor->param_1);
		}
		break;
	case 4:
		{
			m_param.is_open_client = 1;
			this->RASendInfo();
			this->SetRoleDataChange();
		}
		break;
	}
}

void RoleActivityQingYuanShiZhuang::OnRAClose()
{
	if (m_param.qing_yuan_id <= 0)
		return;

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return;
	}

	int buy_mark = 0;
	qing_yuan->GetRAQingYuanShiZhuangParam(NULL, &buy_mark);

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQingYuanShiZhuangConfig * config = static_cast<const RandActivityQingYuanShiZhuangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	int gift_size = config->GetGiftSize(ramgr);
	for (int i = 0; i <= gift_size; i++)
	{
		if(!IsSetBit(buy_mark, i))
			continue;

		if(IsSetBit(m_param.fetch_mark, i))
			continue;

		const RAQingYuanShiZhuangGiftCfg * cfg = config->GetGiftCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

		SetBit(m_param.fetch_mark, i);

		gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			i);
	}
}

void RoleActivityQingYuanShiZhuang::OnJieHun(long long qingyuan_id)
{
	if (qingyuan_id <= 0)
		return;

	m_param.qing_yuan_id = qingyuan_id;
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] qingyuan_id[%lld]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		qingyuan_id);
}

int RoleActivityQingYuanShiZhuang::OnBuyItemCheck(int seq_index, long long ARG_OUT & unique_key)
{
	if (m_param.qing_yuan_id <= 0)
		return -__LINE__;

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return -__LINE__;
	}

	int buy_mark = 0;
	qing_yuan->GetRAQingYuanShiZhuangParam(NULL , &buy_mark);
	if (IsSetBit(buy_mark, seq_index))
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityQingYuanShiZhuangConfig * config = static_cast<const RandActivityQingYuanShiZhuangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAQingYuanShiZhuangGiftCfg * cfg = config->GetGiftCfg(ramgr, seq_index);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (cfg->price_type != 3)
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
	buy_item->need_gold = cfg->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq_index;
	buy_item->rewards_item[0].item_id = cfg->reward_item.item_id;
	buy_item->rewards_item[0].num = cfg->reward_item.num;
	buy_item->rewards_item[0].is_bind = cfg->reward_item.is_bind;
	buy_item->rewards_num = 1;

	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityQingYuanShiZhuang::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
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

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return -__LINE__;
	}	

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty()) return false;
	
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList((short)rewards.size(), &rewards[0], true))
	{
		m_role_module_mgr->GetKnapsack()->Put(rewards[0], PUT_REASON_RA_QINGYUAN_SHIZHUANG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&rewards[0], 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}

	int buy_mark = 0;
	qing_yuan->GetRAQingYuanShiZhuangParam(NULL, &buy_mark);
	SetBit(buy_mark, buy_item->param1);
	qing_yuan->SetRAQingYuanShiZhuangParam(NULL, &buy_mark);

	SetBit(m_param.fetch_mark, buy_item->param1);
	SetBit(m_param.is_role_buy_mark, buy_item->param1);

	this->RASendInfo();
	this->SetRoleDataChange();
	this->BuyNotice();

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] buy_index[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		buy_item->param1);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

int RoleActivityQingYuanShiZhuang::OnBuyGiftItem(int seq_index, ARG_OUT int * price)
{
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	int buy_mark = 0;
	qing_yuan->GetRAQingYuanShiZhuangParam(NULL, &buy_mark);
	if (IsSetBit(buy_mark, seq_index))
	{
		return BUY_GIFT_ITEM_RET_TYPE_2;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityQingYuanShiZhuangConfig * config = static_cast<const RandActivityQingYuanShiZhuangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RAQingYuanShiZhuangGiftCfg * cfg = config->GetGiftCfg(ramgr, seq_index);
	if (NULL == cfg)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (cfg->price_type != 3)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (NULL != price)
	{
		*price = cfg->price;
		return 0;
	}
	
	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->reward_item.item_id, cfg->reward_item.num, false))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(cfg->reward_item, PUT_REASON_RA_QINGYUAN_SHIZHUANG))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}

	SetBit(buy_mark, seq_index);
	qing_yuan->SetRAQingYuanShiZhuangParam(NULL, &buy_mark);

	SetBit(m_param.fetch_mark, seq_index);
	SetBit(m_param.is_role_buy_mark, seq_index);

	this->RASendInfo();
	this->SetRoleDataChange();
	this->BuyNotice();

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] buy_index[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		seq_index);
	return 0;
}

void RoleActivityQingYuanShiZhuang::RASendInfo()
{
	Protocol::SCRAQingYuanShiZhuangInfo info;
	info.is_open_client = m_param.is_open_client;
	info.is_role_buy_mark = m_param.is_role_buy_mark;
	info.fetch_mark = m_param.fetch_mark;

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		info.buy_mark = 0;
	}
	else
	{
		int buy_mark = 0;
		qing_yuan->GetRAQingYuanShiZhuangParam(NULL, &buy_mark);
		info.buy_mark = buy_mark;
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityQingYuanShiZhuang::OnLiHunBuFa(int buy_mark)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQingYuanShiZhuangConfig * config = static_cast<const RandActivityQingYuanShiZhuangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	int gift_size = config->GetGiftSize(ramgr);
	for (int i = 0; i <= gift_size; i++)
	{
		if (!IsSetBit(buy_mark, i))
			continue;

		if (IsSetBit(m_param.fetch_mark, i))
			continue;

		const RAQingYuanShiZhuangGiftCfg * cfg = config->GetGiftCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

		SetBit(m_param.fetch_mark, i);

		gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			i);
	}
}

void RoleActivityQingYuanShiZhuang::RAFetchReward(int seq_index)
{
	if (m_param.qing_yuan_id == 0)
		return;

	if (seq_index < 0 || seq_index >= 16)
		return;

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return;
	}

	int buy_mark = 0;
	qing_yuan->GetRAQingYuanShiZhuangParam(NULL, &buy_mark);
	if (!IsSetBit(buy_mark, seq_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_GIVE_SECRET_KEY_PARAM_NOT_ADV);
		return;
	}

	if (IsSetBit(m_param.fetch_mark, seq_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityQingYuanShiZhuangConfig * config = static_cast<const RandActivityQingYuanShiZhuangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAQingYuanShiZhuangGiftCfg * cfg = config->GetGiftCfg(ramgr, seq_index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->reward_item.item_id, cfg->reward_item.num, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(cfg->reward_item, PUT_REASON_RA_QINGYUAN_SHIZHUANG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}

	SetBit(m_param.fetch_mark, seq_index);

	this->RASendInfo();
	this->SetRoleDataChange();
	
	//
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		seq_index);
}

void RoleActivityQingYuanShiZhuang::RAShop(int seq_index)
{
	if (m_param.qing_yuan_id <= 0)
		return;

	if (seq_index < 0 || seq_index >= 16)
		return;

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return;
	}

	int buy_mark = 0;
	qing_yuan->GetRAQingYuanShiZhuangParam(NULL, &buy_mark);
	if (IsSetBit(buy_mark, seq_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COW_REPORT_GOOD_NEWS_BUY_TIMES_LIMIT);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityQingYuanShiZhuangConfig * config = static_cast<const RandActivityQingYuanShiZhuangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAQingYuanShiZhuangGiftCfg * cfg = config->GetGiftCfg(ramgr, seq_index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	switch (cfg->price_type)
	{
	case MONEY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(cfg->price, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
	case MONEY_TYPE_COIN_BIND:
		{
			if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->price, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	case MONEY_TYPE_SILVER_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->UseOtherCoin(cfg->price, MONEY_TYPE_SILVER_COIN, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	default:
		return;
	}
	
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->reward_item.item_id, cfg->reward_item.num, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(cfg->reward_item, PUT_REASON_RA_QINGYUAN_SHIZHUANG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}

	SetBit(buy_mark, seq_index);
	qing_yuan->SetRAQingYuanShiZhuangParam(NULL, &buy_mark);

	SetBit(m_param.fetch_mark, seq_index);
	SetBit(m_param.is_role_buy_mark, seq_index);

	this->RASendInfo();
	this->SetRoleDataChange();
	this->BuyNotice();
	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] buy_index[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		seq_index);
}

void RoleActivityQingYuanShiZhuang::BuyNotice()
{
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return;
	}

	CommonSaveQingYuanRoleInfoItem * qingyuan_data = qing_yuan->GetOtherSideMemberData(m_role_module_mgr->GetRole()->GetUID());
	if (NULL != qingyuan_data)
	{
		Role * qingyuan_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(qingyuan_data->role_id));
		if (NULL != qingyuan_role)
		{
			RoleActivityQingYuanShiZhuang * qingyuan_shizhuang = static_cast<RoleActivityQingYuanShiZhuang *>
				(qingyuan_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(m_role_activity_type));
			if (NULL != qingyuan_shizhuang)
			{
				qingyuan_shizhuang->RASendInfo();
			}
		}
	}
}
