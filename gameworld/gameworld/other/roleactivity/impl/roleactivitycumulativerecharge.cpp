#include "roleactivitycumulativerecharge.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycumulativerechargeconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"

#include "protocol/randactivity/msgraexpelmonster.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "gameworld/other/route/mailroute.hpp"
#include "config/otherconfig/knapsack_config.hpp"

enum EXPEL_MONSTER_MONEY_TYPE
{
	EXPEL_MONSTER_MONEY_TYPE_GOLD = 0, // 元宝
	EXPEL_MONSTER_MONEY_TYPE_COIN = 1, // 铜币
};

RoleActivityCumulativeRecharge::RoleActivityCumulativeRecharge(int type)
	:RoleActivity(type)
{

}

RoleActivityCumulativeRecharge::~RoleActivityCumulativeRecharge()
{

}

void RoleActivityCumulativeRecharge::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACumulativeChargeParam));
}

void RoleActivityCumulativeRecharge::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RACumulativeChargeParam));
}

void RoleActivityCumulativeRecharge::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityCumulativeRecharge::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	this->SendInfo();
}

void RoleActivityCumulativeRecharge::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE != rand_act_type)
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


	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_cumulative_charge_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_cumulative_charge_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.has_get_reward = 0;
		m_param.acc_price = 0;
		this->SetRoleDataChange();
	}
}

void RoleActivityCumulativeRecharge::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_CUMULATIVE_CHARGE, true))
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 1:	//领取奖励
		{
			this->GetReward();
		}
		break;
	case 2:	//配置检测
		{
			this->OnBuyItemCfgCheck();
		}
		break;
	case 3:	//领取免费礼包
		{
			this->ReceiveFreeGift();
		}
		break;
	}
}

void RoleActivityCumulativeRecharge::OnAddChongZhi(long long chongzhi)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_CUMULATIVE_CHARGE))
	{
		return;
	}
	m_param.acc_price += chongzhi;
	this->SendInfo();
}

void RoleActivityCumulativeRecharge::OnBuyItemCfgCheck()
{
	const RandActivityCumulativeRechargeConfig * config = static_cast<const RandActivityCumulativeRechargeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE));
	if (NULL == config)
	{
		return;
	}

	if (m_param.has_get_reward == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CUMULATIVE_RECHARGE_HAS_GET_REWARD);
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE))
	{
		return;
	}
	int rand_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);

	const CumulativeRechargeCfg * item_cfg = config->GetRewardCfg(rand_open_day);
	if (NULL == item_cfg || item_cfg->acc_price != 0 || item_cfg->buy_money <= 0 || item_cfg->m_reward_vec.empty())
	{
		return;
	}
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_CUMULATIVE_RECHARGE);
	if (rcs == NULL)
	{
		return;
	}
	RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return;
	}
	time_t now = EngineAdapter::Instance().Time();
	if (now < buy_item->create_time + CMD_BUY_ITEM_WAIT_TIME)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_ORDER);

		return;
	}

	if (buy_item->create_time != 0)
	{
		std::string log_str1 = ::GetString(*buy_item);
		gamelog::g_log_cumulative_recharge.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str1.c_str());
	}
	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = item_cfg->buy_money * 10;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_cumulative_charge_begin_timestamp;
	buy_item->unique_id = 0;
	buy_item->param1 = item_cfg->type;
	buy_item->param2 = item_cfg->is_broadcast;
	::ItemConfigDataVecToBuyItem(item_cfg->m_reward_vec, buy_item->rewards_item, &buy_item->rewards_num);
	rcs->SetDataChange(0);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_cumulative_recharge.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());


	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_CUMULATIVE_RECHARGE;
	info.cfg_ver = buy_item->cfg_vesion;
	info.p[0] = item_cfg->type;
	info.p[1] = 0;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

bool RoleActivityCumulativeRecharge::OnBuyItem(int chong_zhi_gold, int type)
{
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_CUMULATIVE_RECHARGE);
	if (rcs == NULL)
	{
		return false;
	}
	const RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0, false);
	if (buy_item == NULL)
	{
		return false;
	}
	gamelog::g_log_cumulative_recharge.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%ud %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_cumulative_charge_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->param1 != type)
	{
		return false;
	}
	if (buy_item->need_gold != chong_zhi_gold)
	{
		return false;
	}
	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return false;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_CUMULATIVE_RECHARGE);
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

	if (buy_item->param2 == 1)
	{
		//找出神装id
		int god_equip_id = 0;
		for (std::vector<ItemConfigData>::const_iterator it = rewards.begin(); it != rewards.end(); it++)
		{
			const ItemBase * reward_item = ITEMPOOL->GetItem(it->item_id);

			if (NULL == reward_item)continue;

			if (reward_item->GetItemType() == I_TYPE_CONVERT_ITEM)
			{
				int real_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(it->item_id, m_role_module_mgr->GetRole()->GetAppearanceProf());
				const ItemBase * real_item_base = ITEMPOOL->GetItem(real_item_id);
				if (real_item_base != NULL && real_item_base->GetColor() >= ItemBase::I_COLOR_STAR && real_item_base->GetColor() <= ItemBase::I_COLOR_GOD)
				{
					god_equip_id = real_item_id;
					break;
				}
			}
		}
		const ItemBase * god_ib = ITEMPOOL->GetItem(god_equip_id);
		if (god_ib != NULL && god_ib->GetColor() >= ItemBase::I_COLOR_STAR && god_ib->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_cumulative_recharge_notice,m_role_module_mgr->GetRole()->GetName(), 
				RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE, god_equip_id, RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);
			if (length > 0 && !CrossConfig::Instance().IsHiddenServer())
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
	}

	//如果活动开启时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_cumulative_charge_begin_timestamp)
	{
		m_param.has_get_reward = 1;
		this->SendInfo();
		this->SetRoleDataChange();
		gamelog::g_log_cumulative_recharge.printf(LL_INFO, "%s role[%d %s] tpye[%d] chong_zhi_gold[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			buy_item->param1, chong_zhi_gold);
	}
	rcs->RemoveData(0);
	m_role_module_mgr->GetRole()->Save();
	this->OnParticipateRA(__FUNCTION__);
	return true;
}

int RoleActivityCumulativeRecharge::OnBuyGiftItem(ARG_OUT int * price)
{
	const RandActivityCumulativeRechargeConfig * config = static_cast<const RandActivityCumulativeRechargeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE));
	if (NULL == config)
	{
		return 1;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE))
	{
		return 1;
	}
	if (m_param.has_get_reward == 1)
	{
		return 2;
	}
	int rand_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);
	const CumulativeRechargeCfg * item_cfg = config->GetRewardCfg(rand_open_day);
	if (NULL == item_cfg || item_cfg->acc_price != 0 || item_cfg->buy_money <= 0 || item_cfg->m_reward_vec.empty())
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = item_cfg->buy_money * 10;	//返还元宝数
		return 0;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)item_cfg->m_reward_vec.size(), &item_cfg->m_reward_vec[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)item_cfg->m_reward_vec.size(), &item_cfg->m_reward_vec[0], PUT_REASON_CUMULATIVE_RECHARGE);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, item_cfg->m_reward_vec);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	if (1 == item_cfg->is_broadcast)
	{
		//找出神装id
		int god_equip_id = 0;
		for (std::vector<ItemConfigData>::const_iterator it = item_cfg->m_reward_vec.begin(); it != item_cfg->m_reward_vec.end(); it++)
		{
			const ItemBase * reward_item = ITEMPOOL->GetItem(it->item_id);

			if (NULL == reward_item)continue;

			if (reward_item->GetItemType() == I_TYPE_CONVERT_ITEM)
			{
				int real_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(it->item_id, m_role_module_mgr->GetRole()->GetAppearanceProf());
				const ItemBase * real_item_base = ITEMPOOL->GetItem(real_item_id);
				if (real_item_base != NULL && real_item_base->GetColor() >= ItemBase::I_COLOR_STAR && real_item_base->GetColor() <= ItemBase::I_COLOR_GOD)
				{
					god_equip_id = real_item_id;
					break;
				}
			}
		}
		const ItemBase * god_ib = ITEMPOOL->GetItem(god_equip_id);
		if (god_ib != NULL && god_ib->GetColor() >= ItemBase::I_COLOR_STAR && god_ib->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_cumulative_recharge_notice,
				m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE, god_equip_id, RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);
			if (length > 0 && !CrossConfig::Instance().IsHiddenServer())
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
	}

	m_param.has_get_reward = 1;
	this->SendInfo();
	this->SetRoleDataChange();
	return 0;
}

void RoleActivityCumulativeRecharge::SendInfo()
{
	Protocol::SCRACumulativeChargeInfo info;
	
	info.acc_price = m_param.acc_price;
	info.has_get_reward = m_param.has_get_reward;
	info.is_received = m_param.is_received;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCumulativeRecharge::GetReward()
{
	const RandActivityCumulativeRechargeConfig * config = static_cast<const RandActivityCumulativeRechargeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE));
	if (NULL == config)
	{
		return;
	}

	if (m_param.has_get_reward == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CUMULATIVE_RECHARGE_HAS_GET_REWARD);
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE))
	{
		return;
	}
	int rand_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);

	const CumulativeRechargeCfg * item_cfg = config->GetRewardCfg(rand_open_day);

	if (item_cfg != NULL && item_cfg->acc_price > 0)
	{
		if (m_param.acc_price < item_cfg->acc_price)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_CUMULATIVE_RECHARGE_ACC_PRICE_ERR);
			return;
		}

		if (static_cast<short>(item_cfg->m_reward_vec.size()) <= 0)return;

		if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(static_cast<short>(item_cfg->m_reward_vec.size()), &item_cfg->m_reward_vec[0]))
		{
			//放入背包失败，改发邮件
			static MailContentParam contentparam; contentparam.Reset();
			for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < static_cast<short>(item_cfg->m_reward_vec.size()); i++)
			{
				const ItemBase * item_base = ITEMPOOL->GetItem(item_cfg->m_reward_vec[i].item_id);

				if (item_base != NULL)
				{
					contentparam.item_list[i].item_id = item_cfg->m_reward_vec[i].item_id;
					contentparam.item_list[i].num = item_cfg->m_reward_vec[i].num;
					contentparam.item_list[i].is_bind = (item_cfg->m_reward_vec[i].is_bind) ? 1 : 0;
					contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
				}
			}
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
		else
		{
			//给予
			if (!m_role_module_mgr->GetKnapsack()->PutList(static_cast<short>(item_cfg->m_reward_vec.size()), &item_cfg->m_reward_vec[0], PUT_REASON_CUMULATIVE_RECHARGE))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}
		}

		if (item_cfg->is_broadcast == 1)
		{
			//找出神装id
			int god_equip_id = 0;
			for (std::vector<ItemConfigData>::const_iterator it = item_cfg->m_reward_vec.begin(); it != item_cfg->m_reward_vec.end(); it++)
			{
				const ItemBase * reward_item = ITEMPOOL->GetItem(it->item_id);
				
				if (NULL == reward_item)continue;

				if (reward_item->GetItemType() == I_TYPE_CONVERT_ITEM)
				{
					int real_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(it->item_id, m_role_module_mgr->GetRole()->GetAppearanceProf());
					const ItemBase * real_item_base = ITEMPOOL->GetItem(real_item_id);
					if (real_item_base != NULL && real_item_base->GetColor() >= ItemBase::I_COLOR_STAR && real_item_base->GetColor() <= ItemBase::I_COLOR_GOD)
					{
						god_equip_id = real_item_id;
						break;
					}
				}
			}
			const ItemBase * god_ib = ITEMPOOL->GetItem(god_equip_id);
			if (god_ib != NULL && god_ib->GetColor() >= ItemBase::I_COLOR_STAR && god_ib->GetColor() <= ItemBase::I_COLOR_GOD)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_cumulative_recharge_notice,
					m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE, god_equip_id, RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);
				if (length > 0 && !CrossConfig::Instance().IsHiddenServer())
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT)))
					{
						World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
					}
				}
			}
		}

		m_param.has_get_reward = 1;
		m_role_module_mgr->NoticeNum(noticenum::NT_GIVE_SECRET_KEY_GET_REWARD_SUCC);
		this->SendInfo();
		this->SetRoleDataChange();
		gamelog::g_log_cumulative_recharge.printf(LL_INFO, "%s role[%d %s] open_day[%d] tpye[%d] chong_zhi_gold[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			rand_open_day, item_cfg->type, item_cfg->acc_price);
	}
}

void RoleActivityCumulativeRecharge::ReceiveFreeGift()
{
	if (m_param.is_received == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const RandActivityCumulativeRechargeConfig * config = static_cast<const RandActivityCumulativeRechargeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE));
	if (NULL == config)
	{
		return;
	}

	const CumulativeRechargeOtherCfg & other_cfg = config->GetOtherCfg();

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(other_cfg.free_reward))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(other_cfg.free_reward, PUT_REASON_CUMULATIVE_RECHARGE))
	{
		return;
	}

	m_param.is_received = 1;

	std::string gamelog_str;
	for (int i = 0; i < (int)other_cfg.free_reward.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", other_cfg.free_reward[i].item_id, other_cfg.free_reward[i].num);
	}

	gamelog::g_log_cumulative_recharge.printf(LL_INFO, "%s role[%d %s] activity_type[%d] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE, gamelog_str.c_str());

	this->SendInfo();
	this->SetRoleDataChange();
}

int RoleActivityCumulativeRecharge::GetEquipScore()
{




	return 0;
}

