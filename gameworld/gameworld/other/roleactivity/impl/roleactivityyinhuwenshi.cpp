#include "roleactivityyinhuwenshi.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyinhuwenshiconfig.hpp"

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
#include "servercommon/string/gameworldstr.h"

RoleActivityYinHuWenShi::RoleActivityYinHuWenShi(int type) :RoleActivity(type)
{
}

RoleActivityYinHuWenShi::~RoleActivityYinHuWenShi()
{
}

void RoleActivityYinHuWenShi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAYinHuWenShiParam));
}

void RoleActivityYinHuWenShi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAYinHuWenShiParam));
}

void RoleActivityYinHuWenShi::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YINHU_WENSHI))
		return;

	this->RASendInfo();
}

void RoleActivityYinHuWenShi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YINHU_WENSHI != rand_act_type)
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

void RoleActivityYinHuWenShi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YINHU_WENSHI))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YINHU_WENSHI);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			long long unique_id = 0;
			int ret_num = this->OnBuyItemCheck(unique_id);
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
			this->RAFetchReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityYinHuWenShi::OnRAClose()
{
	if (0 == m_param.zhigou_flag)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityYinHuWenShiConfig * config = static_cast<const RandActivityYinHuWenShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YINHU_WENSHI));
	if (NULL == config)
	{
		return;
	}

	int MAX_SIZE = config->GetGiftMax(ramgr);
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (IsSetBit(m_param.reward_fetch_mark, i))
			continue;

		const RAYinHuWenShiGroupCfg * cfg = config->GetGiftCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward[0], (short)cfg->reward.size(), SEND_MAIL_TYPE_ACTIVITY_END, true, m_role_activity_type);

		m_param.reward_fetch_mark |= 1 << i;

		gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d] reward_type[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_YINHU_WENSHI,
			i, cfg->type);
	}
}

int RoleActivityYinHuWenShi::OnBuyGiftItem(ARG_OUT int * price)
{
	if (0 != m_param.zhigou_flag)
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
	const RandActivityYinHuWenShiConfig * config = static_cast<const RandActivityYinHuWenShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	const RAYinHuWenShiBuyCfg * cfg = config->GetBuyCfg(ramgr);
	if (NULL == cfg)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = cfg->price;
		return 0;
	}
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(cfg->reward_item, false, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->Put(cfg->reward_item, PUT_REASON_RA_YINHU_WENSHI);
	}

	m_param.zhigou_flag = 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);
	return 0;
}

int RoleActivityYinHuWenShi::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityYinHuWenShiConfig * config = static_cast<const RandActivityYinHuWenShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YINHU_WENSHI));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAYinHuWenShiBuyCfg * cfg = config->GetBuyCfg(ramgr);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (m_param.zhigou_flag != 0)
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
	buy_item->rewards_num = 1;
	buy_item->rewards_item[0].item_id = cfg->reward_item.item_id;
	buy_item->rewards_item[0].num = cfg->reward_item.num;
	buy_item->rewards_item[0].is_bind = cfg->reward_item.is_bind ? 1 : 0;

	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityYinHuWenShi::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
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

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return  -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)rewards.size(), &rewards[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&rewards[0], (short)rewards.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_YINHU_WENSHI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_YINHU_WENSHI);
	}

	m_param.zhigou_flag = 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_YINHU_WENSHI);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

void RoleActivityYinHuWenShi::RASendInfo()
{
	Protocol::SCRAYinHuWenShiInfo info;
	info.zhigou_flag = m_param.zhigou_flag;
	info.reward_fetch_mark = m_param.reward_fetch_mark;
	memcpy(info.reward_type, m_param.reward_type, sizeof(info.reward_type));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityYinHuWenShi::RAFetchReward(int index)
{
	if (index < 0 || index >= RA_YINHU_WENSHI_DAY_MAX)
		return;

	if (0 == m_param.zhigou_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_YINHU_WENSHI_NOT_BUY);
		return;
	}

	if (0 != (m_param.reward_fetch_mark & (1 << index)))
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

	const RandActivityYinHuWenShiConfig * config = static_cast<const RandActivityYinHuWenShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YINHU_WENSHI));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int act_real_open_day = config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_YINHU_WENSHI);
	if (index > act_real_open_day - 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RAYinHuWenShiGroupCfg * cfg = config->GetGiftCfg(ramgr, index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward.size(), &cfg->reward[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward[0], (short)cfg->reward.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_YINHU_WENSHI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward, PUT_REASON_RA_YINHU_WENSHI);
	}
	
	if (cfg->is_hearsay)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_yinhu_xianshi_notice,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), cfg->reward[0].item_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	m_param.reward_type[index] = cfg->type;
	m_param.reward_fetch_mark |= 1 << index;

	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d] reward_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_YINHU_WENSHI,
		index, cfg->type);
}
