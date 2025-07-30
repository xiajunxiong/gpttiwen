#include "randactivityhorcruxtiancheng.hpp"
#include "config/randactivityconfig/impl/randactivityhorcruxtianchengconfig.hpp"

#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "gamelog.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "protocol/msgrandactivity.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/usercache/usercache.hpp"
#include "other/roleactivity/impl/roleactivityhorcruxtiancheng.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RandActivityHorcruxTianCheng::RandActivityHorcruxTianCheng(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG)
{
}

RandActivityHorcruxTianCheng::~RandActivityHorcruxTianCheng()
{
}

void RandActivityHorcruxTianCheng::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameHorcruxTianChengReq protocol;
		protocol.req_id = 0;
		protocol.role_id = user->GetUID();
		protocol.req_type = crossgameprotocal::HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_INFO;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&protocol, sizeof(protocol));

		return;
	}

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		this->SendInfo(user);
	}
}

void RandActivityHorcruxTianCheng::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (!CrossConfig::Instance().IsHiddenServer() && to_status == ACTIVITY_STATUS_OPEN)
	{
		const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
		if (NULL == config)
			return;

		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
		if (NULL == ramgr)
			return;

		const RAHorcruxTianChengJackpotCfg * cfg = config->GetJackpotCfg(ramgr);
		if (NULL == cfg)
			return;

		m_param.jackpot_type = cfg->jackpot_type;
		m_param.item_id = cfg->item_id;
		m_param.jackpot_num = cfg->start_num;

		this->SendInfo();
	}
}

void RandActivityHorcruxTianCheng::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);
	memcpy(&m_param, arg_data.rand_activity_data.data, sizeof(m_param));
}

void RandActivityHorcruxTianCheng::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(arg_data.rand_activity_data.data));
	memcpy(arg_data.rand_activity_data.data, &m_param, sizeof(m_param));
}

void RandActivityHorcruxTianCheng::GetPoolReward(int get_percent, unsigned int ARG_OUT & item_num)
{
	if (get_percent > 0)
	{
		item_num = (int)(m_param.jackpot_num * get_percent / 100.0f + 0.5);			// +0.5 四舍五入
		if (item_num <= 0) item_num = 1;

		int old_num = m_param.jackpot_num;
		m_param.jackpot_num -= item_num;

		if (m_param.jackpot_num <= 0)
		{
			const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
			if (NULL == config)
				return;

			RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
			if (NULL == ramgr)
				return;

			const RAHorcruxTianChengJackpotCfg * cfg = config->GetJackpotCfg(ramgr);
			if (NULL == cfg)
				return;

			m_param.jackpot_num = cfg->start_num;
		}

		this->SendInfo();
	}
}

void RandActivityHorcruxTianCheng::SendInfo(Role * role)
{
	Protocol::SCRAHorcruxTianChengPoolInfo protocol;
	protocol.pool_type = m_param.jackpot_type;
	protocol.item_id = m_param.item_id;
	protocol.num = m_param.jackpot_num;

	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&protocol, sizeof(protocol));
		InternalComm::Instance().SendToHiddenUniqueServerUserThroughCross((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
}

void RandActivityHorcruxTianCheng::GameHiddenSendInfo(int role_id)
{
	Protocol::SCRAHorcruxTianChengPoolInfo protocol;
	protocol.pool_type = m_param.jackpot_type;
	protocol.item_id = m_param.item_id;
	protocol.num = m_param.jackpot_num;

	InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
}

void RandActivityHorcruxTianCheng::Draw(Role * role)
{
	if (NULL == role)
		return;

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
	if (NULL == config)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num, false))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	RoleActivityHorcruxTianCheng * role_activity = (RoleActivityHorcruxTianCheng *)role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
	if (NULL == role_activity)return;
	const RAHorcruxTianChengParam * role_param = role_activity->GetParam();
	if (NULL == role_param)return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num, __FUNCTION__))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}

		crossgameprotocal::HiddenGameHorcruxTianChengReq protocol;
		UniqueIDGenerator::Instance()->GenUniqueID(&protocol.req_id);
		protocol.role_id = role->GetUID();
		protocol.req_type = crossgameprotocal::HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_DRAW;
		protocol.rand_times = role_param->rand_times;
		protocol.floor_bitmap = role_param->floor_bitmap;
		InternalComm::Instance().SendToGameThroughCross2((const char *)&protocol, sizeof(protocol), role->GetUserId().db_index);

		return;
	}

	bool floor_back = false;
	const RAHorcruxTianChangGiftCfg * gift_cfg = config->GetGiftCfg(ramgr, role_param->rand_times + 1, &role_param->floor_bitmap, &floor_back);
	if (NULL == gift_cfg)
		return;

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num, __FUNCTION__))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	int money_type = -1;
	unsigned int reward_num = 0;
	std::vector<ItemConfigData> reward_item;
	switch (gift_cfg->reward_type)
	{
	case 0://道具奖励
		{
			reward_item.push_back(gift_cfg->reward_item);

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d,%s] item[%d, %d]",
				role->GetUID(), role->GetName(), gift_cfg->reward_item.item_id, gift_cfg->reward_item.num);
		}
		break;
	case 1://奖池百分比奖励
		{
			this->GetPoolReward(gift_cfg->reward_per, reward_num);

			if (m_param.jackpot_type == -1)
			{
				ItemConfigData item(m_param.item_id, false, reward_num);
				reward_item.push_back(item);
			}
			else
			{
				money_type = m_param.jackpot_type;
			}

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d,%s] pool_type[%d] item_id[%d], num[%d]",
				role->GetUID(), role->GetName(), m_param.jackpot_type, m_param.jackpot_type == -1 ? m_param.item_id : 0, reward_num);
		}
		break;
	case 2://元宝奖励
		{
			money_type = 0;
			reward_num = gift_cfg->gold_num;

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d,%s] gold[%d]",
				role->GetUID(), role->GetName(), reward_num);
		}
		break;
	}

	if (!reward_item.empty())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(reward_item, false))
		{
			role->GetRoleModuleManager()->GetKnapsack()->SendMail(&reward_item[0], (int)reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
		}
		else
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList((short)reward_item.size(), &reward_item[0], PUT_REASON_RA_HORCRUX_TIANCHENG_DRAW);
		}
	}

	if (money_type == MONEY_TYPE_GOLD)
	{
		role->GetRoleModuleManager()->GetMoney()->AddGold(reward_num, __FUNCTION__, false, false, true);
	}
	else if(money_type > MONEY_TYPE_COIN_BIND)
	{
		role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(reward_num, money_type, __FUNCTION__, false, true);
	}

	if (gift_cfg->hearsay != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		std::string broadcast_str;
		if (gift_cfg->hearsay == 1)
			broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_horcrux_tiancheng_broadcast_1, role->GetUID(), role->GetName(), gift_cfg->reward_per);
		if (gift_cfg->hearsay == 2)
			broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_horcrux_tiancheng_broadcast_2, role->GetUID(), role->GetName(), gift_cfg->reward_item.item_id);

		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", broadcast_str.c_str());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	{
		Protocol::SCRAHorcruxTianChengDrawResult protocol;
		protocol.seq = gift_cfg->seq;
		protocol.num = gift_cfg->reward_type != 1 ? 0 : reward_num;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}

	{
		if (money_type >= 0)
		{
			role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList(0, NULL, GET_REASON_HORCRUX_TIANCHENG, money_type, reward_num);
		}
		else
		{
			role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemListVec(reward_item, GET_REASON_HORCRUX_TIANCHENG, -1);
		}
	}

	this->AddDrawTime(1);
	role_activity->OnAddRandTimes(1);
	if (floor_back)
	{
		//role_activity->ClearRandTimes();
	}
	this->OnRoleParticipateRA(role, __FUNCTION__);
}

int RandActivityHorcruxTianCheng::DrawFromHidden(int role_id, long long req_id, int check_floor, const BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM>* bit_map)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return -__LINE__;
	}

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
	if (NULL == config)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}
	bool floor_back = false;
	const RAHorcruxTianChangGiftCfg * gift_cfg = config->GetGiftCfg(ramgr, check_floor + 1, bit_map, &floor_back);
	if (NULL == gift_cfg)
		return -__LINE__;

	int money_type = -1;
	unsigned int reward_num = 0;
	std::vector<ItemConfigData> reward_item;
	switch (gift_cfg->reward_type)
	{
	case 0://道具奖励
		{
			reward_item.push_back(gift_cfg->reward_item);

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role:%d item[%d, %d]",
				role_id , gift_cfg->reward_item.item_id, gift_cfg->reward_item.num);
		}
		break;
	case 1://奖池百分比奖励
		{
			this->GetPoolReward(gift_cfg->reward_per, reward_num);

			if (m_param.jackpot_type == -1)
			{
				ItemConfigData item(m_param.item_id, false, reward_num);
				reward_item.push_back(item);
			}
			else
			{
				money_type = m_param.jackpot_type;
			}

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role:%d pool_type[%d] item_id[%d], num[%d]",
				role_id, m_param.jackpot_type, m_param.jackpot_type == -1 ? m_param.item_id : 0, reward_num);
		}
		break;
	case 2://元宝奖励
		{
			money_type = 0;
			reward_num = gift_cfg->gold_num;

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role:%d, gold[%d]",
				role_id, reward_num);
		}
		break;
	}

	this->AddDrawTime(1);

	std::string role_name;
	UserCacheNode * uc = UserCacheManager::Instance().GetUserNode(role_id);
	if (NULL != uc)
	{
		role_name = STRING_SPRINTF("%s_%d", uc->GetNamePtr(), IntToUid(role_id).db_index);
	}

	std::string broadcast_str;
	if (gift_cfg->hearsay == 1)
		broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_horcrux_tiancheng_broadcast_1, role_id, role_name.c_str(), gift_cfg->reward_per);
	if (gift_cfg->hearsay == 2)
		broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_horcrux_tiancheng_broadcast_2, role_id, role_name.c_str(), gift_cfg->reward_item.item_id);

	crossgameprotocal::GameHiddenHorcruxTianChengDrawRet protocol;
	protocol.req_id = req_id;
	protocol.seq = gift_cfg->seq;
	protocol.role_id = role_id;
	protocol.is_jackpot = gift_cfg->reward_type == 1 ? 1 : 0;
	protocol.money_type = money_type;
	protocol.reward_num = reward_num;
	protocol.floor_back = floor_back ? 1 : 0;
	protocol.items = reward_item;
	protocol.broadcast_str = broadcast_str;

	return InternalComm::Instance().SendSerializeMsgToHiddenThroughCross2(&protocol) ? 0 : -__LINE__;
}

void RandActivityHorcruxTianCheng::AddDrawTime(int draw_times)
{
	m_param.draw_times += draw_times;

	const RandActivityHorcruxTianChengConfig * config = static_cast<const RandActivityHorcruxTianChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
	if (NULL == config)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RAHorcruxTianChengJackpotCfg * cfg = config->GetJackpotCfg(ramgr);
	if (NULL == cfg)
		return;

	int add_times = m_param.draw_times / cfg->all_draw_times;
	if (add_times > 0)
	{
		m_param.draw_times = m_param.draw_times % cfg->all_draw_times;
		m_param.jackpot_num += add_times * cfg->add_item_num;
		this->SendInfo();
	}
}
