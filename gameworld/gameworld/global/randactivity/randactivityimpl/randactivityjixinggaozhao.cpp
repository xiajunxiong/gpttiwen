#include "randactivityjixinggaozhao.hpp"
#include "config/randactivityconfig/impl/randactivityjixinggaozhaoconfig.hpp"

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
#include "protocol/msgitem.h"

#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/usercache/usercache.hpp"
#include "other/roleactivity/impl/roleactivityjixinggaozhao.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RandActivityJiXingGaoZhao::RandActivityJiXingGaoZhao(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO)
{
}

RandActivityJiXingGaoZhao::~RandActivityJiXingGaoZhao()
{
}

void RandActivityJiXingGaoZhao::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameJiXingGaoZhaoReq protocol;
		protocol.req_id = 0;
		protocol.role_id = user->GetUID();
		protocol.req_type = crossgameprotocal::HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_INFO;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&protocol, sizeof(protocol));

		return;
	}

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		this->SendInfo(user);
		this->SendRecordInfo(user);
	}
}

void RandActivityJiXingGaoZhao::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (!CrossConfig::Instance().IsHiddenServer() && to_status == ACTIVITY_STATUS_OPEN)
	{
		const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
		if (NULL == config)
			return;

		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
		if (NULL == ramgr)
			return;

		const RAJiXingGaoZhaoJackpotCfg * cfg = config->GetJackpotCfg(ramgr);
		if (NULL == cfg)
			return;

		m_param.item_id = cfg->jackpot_item_id;
		m_param.jackpot_num = cfg->jackpot_item_id != 0 ? cfg->jackpot_item_num : cfg->jackpot_gold_num;
		
		this->SendInfo();
		this->SendRecordInfo();
	}
	else if(!CrossConfig::Instance().IsHiddenServer() && to_status == ACTIVITY_STATUS_CLOSE)
	{
		queue_map.clear();
	}
}

void RandActivityJiXingGaoZhao::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);
	memcpy(&m_param, arg_data.rand_activity_data.data, sizeof(m_param));

	for (int i = 0; i < 10; i++)
	{
		if (m_param.queue_list[i].reward_type != -1)
		{
			queue_map.push_front(m_param.queue_list[i]);
		}
	}
}

void RandActivityJiXingGaoZhao::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	for (int i = 0; i < 10; i++)
	{
		if (i < queue_map.size())
		{
			m_param.queue_list[i] = queue_map[i];
		}
		else
		{
			m_param.queue_list[i].Reset();
		}
	}

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(arg_data.rand_activity_data.data));
	memcpy(&arg_data.rand_activity_data.data, &m_param, sizeof(m_param));
}

void RandActivityJiXingGaoZhao::GetPoolReward(int get_percent, unsigned int ARG_OUT & item_num)
{
	if (get_percent > 0)
	{
		item_num = (int)(m_param.jackpot_num * get_percent / 100.0f + 0.5);			// +0.5 四舍五入
		if (item_num <= 0) item_num = 1;

		int old_num = m_param.jackpot_num;
		m_param.jackpot_num -= item_num;

		if (m_param.jackpot_num <= 0)
		{
			const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
			if (NULL == config)
				return;

			RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
			if (NULL == ramgr)
				return;

			const RAJiXingGaoZhaoJackpotCfg * cfg = config->GetJackpotCfg(ramgr);
			if (NULL == cfg)
				return;

			m_param.jackpot_num = cfg->jackpot_item_id != 0 ? cfg->jackpot_item_num : cfg->jackpot_gold_num;
		}

		this->SendInfo();
	}
}

void RandActivityJiXingGaoZhao::SendInfo(Role * role)
{
	Protocol::SCRAJiXingGaoZhaoPoolInfo protocol;
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

void RandActivityJiXingGaoZhao::GameHiddenSendInfo(int role_id)
{
	{
		Protocol::SCRAJiXingGaoZhaoPoolInfo protocol;
		protocol.item_id = m_param.item_id;
		protocol.num = m_param.jackpot_num;

		InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
	}

	{
		Protocol::SCRAJiXingGaoZhaoRecordListInfo info;
		info.record_count = 0;

		UNSTD_STATIC_CHECK(sizeof(info.record_list[0]) == sizeof(queue_map[0]));
		for (int i = 0; i < 10 && i < (int)queue_map.size(); ++i)
		{
			memcpy(&info.record_list[i], &queue_map[i], sizeof(JiXingGaoZhaoRecordInfo));
			++info.record_count;
		}

		int length = sizeof(Protocol::SCRAJiXingGaoZhaoRecordListInfo) - sizeof(info.record_list) + info.record_count * sizeof(info.record_list[0]);
		InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&info, length);
	}
}

bool RandActivityJiXingGaoZhao::Draw(Role * role)
{
	if (NULL == role)
		return false;

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
	if (NULL == config)
		return false;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr)
		return false;

	if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num, false))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return false;
	}

	RoleActivityJiXingGaoZhao * role_activity = (RoleActivityJiXingGaoZhao *)role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
	if (NULL == role_activity)return false;
	const RAJiXingGaoZhaoParam * role_param = role_activity->GetParam();
	if (NULL == role_param)return false;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num, __FUNCTION__))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return false;
		}

		crossgameprotocal::HiddenGameJiXingGaoZhaoReq protocol;
		UniqueIDGenerator::Instance()->GenUniqueID(&protocol.req_id);
		protocol.role_id = role->GetUID();
		protocol.req_type = crossgameprotocal::HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_DRAW;
		protocol.rand_times = role_param->rand_times;
		protocol.floor_bitmap = role_param->floor_bitmap;
		InternalComm::Instance().SendToGameThroughCross2((const char *)&protocol, sizeof(protocol), role->GetUserId().db_index);

		return true;
	}

	bool floor_back = false;
	const RAJiXingGaoZhaoBaseCfg * gift_cfg = config->GetBaseCfg(ramgr, role_param->rand_times + 1, &role_param->floor_bitmap, &floor_back);
	if (NULL == gift_cfg)
		return false;

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num, __FUNCTION__))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return false;
	}

	int reward_type = -1;
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
	case 1://元宝奖励
		{
			reward_type = 0;
			reward_num = gift_cfg->gold_num;

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d,%s], gold_num[%d]",
				role->GetUID(), role->GetName(), reward_num);
		}
		break;
	case 2://奖池百分比奖励
		{
			this->GetPoolReward(gift_cfg->reward_per, reward_num);

			if (m_param.item_id != 0)
			{
				ItemConfigData item(m_param.item_id, false, reward_num);
				reward_item.push_back(item);
			}
			else
			{
				reward_type = 0;
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d,%s] item_id[%d], num[%d]",
				role->GetUID(), role->GetName(), m_param.item_id != 0 ? m_param.item_id : 0, reward_num);
		}	
		break;
	}

	if (!reward_item.empty())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(reward_item, false))
		{
			role->GetRoleModuleManager()->GetKnapsack()->SendMail(&reward_item[0], (int)reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
		}
		else
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutVec(reward_item, PUT_REASON_RA_JIXING_GAOZHAO_DRAW);
		}
	}

	if (reward_type == MONEY_TYPE_GOLD)
	{
		role->GetRoleModuleManager()->GetMoney()->AddGold(reward_num, __FUNCTION__, false, false, true);
	}

	if (gift_cfg->hearsay != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		std::string broadcast_str;
		if (gift_cfg->hearsay == 1)
			broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_jixing_gaozhao_broadcast_1, role->GetUID(), role->GetName(), gift_cfg->reward_per);
		if (gift_cfg->hearsay == 2)
			broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_jixing_gaozhao_broadcast_2, role->GetUID(), role->GetName(), 
				gift_cfg->reward_item.item_id, gift_cfg->reward_item.num);

		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", broadcast_str.c_str());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}

		{
			JiXingGaoZhaoRecordInfo record;
			record.reward_type = gift_cfg->reward_type;
			memcpy(record.role_name, role->GetName(), sizeof(GameName));

			if (0 == record.reward_type)
			{
				record.item_id = gift_cfg->reward_item.item_id;
				record.num = gift_cfg->reward_item.num;
			}
			else if(2 == record.reward_type)
			{
				record.num = gift_cfg->reward_per;
			}

			this->AddDeque(record);
		}
	}

	{
		Protocol::SCRAJiXingGaoZhaoDrawResult protocol;
		protocol.seq = gift_cfg->seq;
		protocol.num = gift_cfg->reward_type != 2 ? 0 : reward_num;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}

	{
		if (reward_type > 0)
		{
			role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList(0, NULL, GET_REASON_JIXING_GAOZHAO, reward_num);
		}
		else
		{
			role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemListVec(reward_item, GET_REASON_JIXING_GAOZHAO, -1);
		}
	}

	this->AddDrawTime(1);
	role_activity->OnAddRandTimes(1);
	if (floor_back)
	{
		//role_activity->ClearRandTimes();
	}
	return true;
}

int RandActivityJiXingGaoZhao::DrawFromHidden(int role_id, long long req_id, int check_floor, const BitMap<MAX_JI_XING_GAO_ZHAO_REWARD_GROUP_NUM>* bit_map)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return -__LINE__;
	}

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
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
	const RAJiXingGaoZhaoBaseCfg * gift_cfg = config->GetBaseCfg(ramgr, check_floor + 1, bit_map, &floor_back);
	if (NULL == gift_cfg)
		return false;

	int reward_type = -1;
	unsigned int reward_num = 0;
	std::vector<ItemConfigData> reward_item;

	switch (gift_cfg->reward_type)
	{
	case 0://道具奖励
		{
			reward_item.push_back(gift_cfg->reward_item);

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d] item[%d, %d]",
				role_id, gift_cfg->reward_item.item_id, gift_cfg->reward_item.num);
		}
		break;
	case 1://元宝奖励
		{
			reward_type = 0;
			reward_num = gift_cfg->gold_num;

			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d], gold_num[%d]",
				role_id, reward_num);
		}
		break;
	case 2://奖池百分比奖励
		{
			this->GetPoolReward(gift_cfg->reward_per, reward_num);

			if (m_param.item_id != 0)
			{
				ItemConfigData item(m_param.item_id, false, reward_num);
				reward_item.push_back(item);
			}
			else
			{
				reward_type = 0;
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d] item_id[%d], num[%d]",
				role_id, m_param.item_id != 0 ? m_param.item_id : 0, reward_num);
		}
		break;
	}

	std::string role_name;
	UserCacheNode * uc = UserCacheManager::Instance().GetUserNode(role_id);
	if (NULL != uc)
	{
		role_name = STRING_SPRINTF("%s", uc->GetNamePtr(), IntToUid(role_id).db_index);
	}

	std::string broadcast_str;
	if (gift_cfg->hearsay != 0 && !CrossConfig::Instance().IsHiddenServer())
	{	
		if (gift_cfg->hearsay == 1)
			broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_jixing_gaozhao_broadcast_1, role_id, role_name.c_str(), gift_cfg->reward_per);
		if (gift_cfg->hearsay == 2)
			broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_jixing_gaozhao_broadcast_2, role_id, role_name.c_str(),
				gift_cfg->reward_item.item_id, gift_cfg->reward_item.num);

		{
			JiXingGaoZhaoRecordInfo record;
			record.reward_type = gift_cfg->reward_type;
			memcpy(record.role_name, role_name.c_str(), sizeof(GameName));

			if (0 == record.reward_type)
			{
				record.item_id = gift_cfg->reward_item.item_id;
				record.num = gift_cfg->reward_item.num;
			}
			else if (2 == record.reward_type)
			{
				record.num = gift_cfg->reward_per;
			}

			this->AddDeque(record);
		}
	}

	this->AddDrawTime(1);

	crossgameprotocal::GameHiddenJiXingGaoZhaoDrawRet protocol;
	protocol.req_id = req_id;
	protocol.seq = gift_cfg->seq;
	protocol.role_id = role_id;
	protocol.is_jackpot = gift_cfg->reward_type == 2 ? 1 : 0;
	protocol.reward_type = reward_type;
	protocol.reward_num = reward_num;
	protocol.floor_back = floor_back ? 1 : 0;
	protocol.items = reward_item;
	protocol.broadcast_str = broadcast_str;

	return InternalComm::Instance().SendSerializeMsgToHiddenThroughCross2(&protocol) ? 0 : -__LINE__;
}

void RandActivityJiXingGaoZhao::AddDrawTime(int draw_times)
{
	m_param.draw_times += draw_times;

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
	if (NULL == config)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RAJiXingGaoZhaoJackpotCfg * cfg = config->GetJackpotCfg(ramgr);
	if (NULL == cfg)
		return;

	int add_times = m_param.draw_times / cfg->all_draw_times;
	if (add_times > 0)
	{
		m_param.draw_times = m_param.draw_times % cfg->all_draw_times;
		int add_num = cfg->jackpot_item_id != 0 ? cfg->add_item_num : cfg->add_gold;
		m_param.jackpot_num += add_times * add_num;
		this->SendInfo();
	}
}

void RandActivityJiXingGaoZhao::AddDeque(const JiXingGaoZhaoRecordInfo & info)
{
	queue_map.push_front(info);

	if (queue_map.size() > 10)
	{
		queue_map.pop_back();
	}

	Protocol::SCRAJiXingGaoZhaoRecordInfo sc_info;
	memcpy(&sc_info.record, &info, sizeof(JiXingGaoZhaoRecordInfo));

	World::GetInstWorld()->SendToAllGateway((const char *)&sc_info, sizeof(sc_info));
	InternalComm::Instance().SendToHiddenUniqueServerUserThroughCross((const char *)&sc_info, sizeof(sc_info));
}

void RandActivityJiXingGaoZhao::SendRecordInfo(Role * role)
{
	Protocol::SCRAJiXingGaoZhaoRecordListInfo info;
	info.record_count = 0;

	for (int i = 0; i < 10 && i < (int)queue_map.size(); ++i)
	{
		memcpy(&info.record_list[i], &queue_map[i], sizeof(JiXingGaoZhaoRecordInfo));
		++info.record_count;
	}

	int length = sizeof(Protocol::SCRAJiXingGaoZhaoRecordListInfo) - sizeof(info.record_list) + info.record_count * sizeof(info.record_list[0]);
	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, length);
		InternalComm::Instance().SendToHiddenUniqueServerUserThroughCross((const char *)&info, length);
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, length);
	}
}
