#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "item/knapsack.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/topicmanager/topicmanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "world.h"
#include "gamelog.h"
#include "global/usercache/usercache.hpp"

#include "protocol/randactivity/msgrashenshoujianglin.hpp"
#include "config/randactivityconfig/impl/randactivityshenshoujianglinconfig.hpp"
#include "other/roleactivity/impl/roleactivityshenshoujianglin.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "randactivityshenshoujianglin.hpp"

#include <math.h>

RandActivityShenShouJiangLin::RandActivityShenShouJiangLin(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN)
{
}

RandActivityShenShouJiangLin::~RandActivityShenShouJiangLin()
{
}

void RandActivityShenShouJiangLin::OnUserLogin(Role * role)
{
	RandActivity::OnUserLogin(role);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameShenShouJiangLinReq protocol;
		protocol.req_id = 0;
		protocol.role_id = role->GetUID();
		protocol.req_type = crossgameprotocal::HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_INFO;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&protocol, sizeof(protocol));

		return;
	}

	this->SendInfo(role);
}

void RandActivityShenShouJiangLin::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (!CrossConfig::Instance().IsHiddenServer() && to_status == ACTIVITY_STATUS_OPEN)
	{
		const RandActivityShenShouJiangLinConfig * config = static_cast<const RandActivityShenShouJiangLinConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
		if (NULL == config)
		{
			return;
		}


		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
		if (NULL == ramgr)
		{
			return ;
		}

		const RAShenShouJiangLinOtherCfg & other = config->GetOtherCfg(ramgr);
		m_param.gold_num = other.pool_gold_init_num;
		m_param.item_num = other.pool_item_init_num;
		this->SendInfo();
	}
}

void RandActivityShenShouJiangLin::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);
	memcpy(&m_param, arg_data.rand_activity_data.data, sizeof(m_param));
}

void RandActivityShenShouJiangLin::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(arg_data.rand_activity_data.data));
	memcpy(arg_data.rand_activity_data.data, &m_param, sizeof(m_param));
}

void RandActivityShenShouJiangLin::GetPoolReward(int get_percent, int ARG_OUT & gold_num, int ARG_OUT & item_num)
{
	if (get_percent > 0)
	{
		double percent = get_percent / 100.0;
		gold_num = (int)round(m_param.gold_num * percent);
		item_num = (int)round(m_param.item_num * percent);

		int old_gold_num = m_param.gold_num;
		int old_item_num = m_param.item_num;

		m_param.gold_num -= gold_num;
		m_param.item_num -= item_num;

		gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | pool_gold_num:%d pool_item_num:%d get_percent:%d curr_pool_gold_num:%d curr_pool_item_num:%d"
							  , __FUNCTION__, __LINE__, old_gold_num, old_item_num, get_percent, m_param.gold_num, m_param.item_num);

		this->SendInfo();
	}
}

int RandActivityShenShouJiangLin::Draw(Role * role)
{
	if (role == NULL)
	{
		return -__LINE__;
	}

	const RandActivityShenShouJiangLinConfig * config = static_cast<const RandActivityShenShouJiangLinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetActivityType()));
	if (NULL == config)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RAShenShouJiangLinOtherCfg & other = config->GetOtherCfg(ramgr);

	RoleActivityShenShouJiangLin * role_activity = static_cast<RoleActivityShenShouJiangLin *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN));
	if (!role_activity)
	{
		return -__LINE__;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other.draw_comsume_item_id, other.draw_comsume_num, __FUNCTION__))
		{
			return -__LINE__;
		}

		crossgameprotocal::HiddenGameShenShouJiangLinReq protocol;
		UniqueIDGenerator::Instance()->GenUniqueID(&protocol.req_id);
		protocol.role_id = role->GetUID();
		protocol.req_type = crossgameprotocal::HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_DRAW;
		int ret = InternalComm::Instance().SendToGameThroughCross2((const char *)&protocol, sizeof(protocol), role->GetUserId().db_index);

		gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role[%d %s] req_id:%lld shen_shou_jiang_lin draw ret:%d", __FUNCTION__, __LINE__, role->GetUID(), role->GetName(), protocol.req_id, ret);

		return -__LINE__;
	}

	const RAShenShouJiangLinItemCfg * cfg = config->GetRandReward(ramgr);
	if (cfg == NULL)
	{
		return -__LINE__;
	}


	std::set<int> bao_di_id_list;
	std::vector<const RAShenShouJiangLinItemCfg *> bao_di_cfg_list = config->GetRAShenShouJiangLinBaoDiCfgList(ramgr, bao_di_id_list);

	for (int bao_di_index = 0; bao_di_index < (int)bao_di_cfg_list.size(); ++bao_di_index)
	{
		const RAShenShouJiangLinItemCfg * curr = bao_di_cfg_list[bao_di_index];

		BaoDiInfoItem * bao_di_info_item = role_activity->GetBaoDiInfoItem(curr->bao_di_id);
		if (bao_di_info_item)
		{
			if (bao_di_info_item->is_get != 0)
			{
				continue;
			}

			if (bao_di_info_item->times + 1 >= curr->bao_di_times)
			{
				cfg = curr;


				gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:[%d %s] get_bao_di_reward bao_di_id:%d seq:%d need_times:%d draw_times:%d", __FUNCTION__, __LINE__, role->GetUID(), role->GetName(),
					curr->bao_di_id, curr->seq, curr->bao_di_times, bao_di_info_item->times + 1);

				break;
			}
		}
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other.draw_comsume_item_id, other.draw_comsume_num, __FUNCTION__))
	{
		return -__LINE__;
	}

	int gold_num = cfg->gold_num;
	std::vector<ItemConfigData> rewards;
	if (cfg->reward.item_id != 0)
	{
		rewards.push_back(cfg->reward);
	}

	int pool_gold_num = 0;
	int pool_item_num = 0;
	this->GetPoolReward(cfg->pool_get_reward_percent, pool_gold_num, pool_item_num);
	gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role:%d %s seq:%d pool_gold_num:%d pool_item_num:%d get_percent:%d"
						  , __FUNCTION__, __LINE__, role->GetUID(), role->GetName(), cfg->seq, pool_gold_num, pool_item_num, cfg->pool_get_reward_percent);

	gold_num += pool_gold_num;
	if (pool_item_num > 0)
	{
		rewards.push_back(ItemConfigData(other.pool_item_id, true, pool_item_num));
	}

	role->GetRoleModuleManager()->GetMoney()->AddGold(gold_num, __FUNCTION__, false, false, true);
	if (!rewards.empty())
	{
		if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_SHEN_SHOU_JIANG_LIN);
		}
		else
		{
			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, rewards);

			std::string mail_str = STRING_SPRINTF(gamestring::g_act_full_bag_content, this->GetActivityType());

			if (cfg->broadcast == 1)
			{				
				std::string tmp_str;
				bool b = false;
				if (pool_gold_num > 0)
				{
					b = true;
					tmp_str += STRING_SPRINTF("{i;%d} * %d", GOLD_ITEM_ID, pool_gold_num);
				}

				if (pool_item_num > 0)
				{
					if (b)tmp_str += ", ";
					tmp_str += STRING_SPRINTF("{i;%d} * %d", other.pool_item_id, pool_item_num);
				}

				mail_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_content_1, cfg->pool_get_reward_percent, tmp_str.c_str());
			}

			if (cfg->broadcast == 2)
			{
				mail_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_content_2, cfg->reward.item_id);
			}


			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, this->GetActivityType());
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), "%s", mail_str.c_str());
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(role->GetUID(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}

	role_activity->SetBaoDiInfoAfterDraw(cfg->bao_di_id, bao_di_id_list);

	if (cfg->broadcast != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		std::string broadcast_str;
		if (cfg->broadcast == 1)
			broadcast_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_broadcast_1, role->GetUID(), role->GetName(), cfg->pool_get_reward_percent);
		if (cfg->broadcast == 2)
			broadcast_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_broadcast_2, role->GetUID(), role->GetName(), cfg->reward.item_id);

		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", broadcast_str.c_str());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	Protocol::SCRAShenShouJiangLinDrawResult protocol;
	protocol.seq = cfg->seq;
	protocol.gold_num = gold_num;
	protocol.item_count = (int)rewards.size();
	for (int i = 0; i < ARRAY_LENGTH(protocol.item) && i < (int)rewards.size(); ++i)
	{
		protocol.item[i].item_id = rewards[i].item_id;
		protocol.item[i].num = rewards[i].num;
		protocol.item[i].is_bind = rewards[i].is_bind;
	}
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

	this->AddDrawTime(1);

	std::string log_str;
	for (int i = 0; i < (int)rewards.size(); ++i)
	{
		log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d]", rewards[i].item_id, rewards[i].num, rewards[i].is_bind);
	}
	gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:%d %s seq:%d gold_num:%d %s", __FUNCTION__, __LINE__, role->GetUID(), role->GetName(), cfg->seq, gold_num, log_str.c_str());

	return 0;
}

int RandActivityShenShouJiangLin::DrawFromHidden(int role_id, long long req_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return -__LINE__;
	}
	
	const RandActivityShenShouJiangLinConfig * config = static_cast<const RandActivityShenShouJiangLinConfig *>
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

	const RAShenShouJiangLinOtherCfg & other = config->GetOtherCfg(ramgr);

	const RAShenShouJiangLinItemCfg * cfg = config->GetRandReward(ramgr);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	int gold_num = cfg->gold_num;
	std::vector<ItemConfigData> rewards;
	if (cfg->reward.item_id != 0)
	{
		rewards.push_back(cfg->reward);
	}

	int pool_gold_num = 0;
	int pool_item_num = 0;
	this->GetPoolReward(cfg->pool_get_reward_percent, pool_gold_num, pool_item_num);
	gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role:%d  seq:%d pool_gold_num:%d pool_item_num:%d get_percent:%d"
						  , __FUNCTION__, __LINE__, role_id, cfg->seq, pool_gold_num, pool_item_num, cfg->pool_get_reward_percent);

	gold_num += pool_gold_num;
	if (pool_item_num > 0)
	{
		rewards.push_back(ItemConfigData(other.pool_item_id, true, pool_item_num));
	}

	this->AddDrawTime(1);

	std::string log_str;
	for (int i = 0; i < (int)rewards.size(); ++i)
	{
		log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d]", rewards[i].item_id, rewards[i].num, rewards[i].is_bind);
	}
	gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:%d  seq:%d gold_num:%d %s", __FUNCTION__, __LINE__, role_id, cfg->seq, gold_num, log_str.c_str());

	std::string role_name;
	UserCacheNode * uc = UserCacheManager::Instance().GetUserNode(role_id);
	if (NULL != uc)
	{
		role_name = STRING_SPRINTF("%s_%d", uc->GetNamePtr(), IntToUid(role_id).db_index);
	}

	std::string mail_str;
	if (cfg->broadcast == 1)
	{
		std::string tmp_str;
		bool b = false;
		if (pool_gold_num > 0)
		{
			b = true;
			tmp_str += STRING_SPRINTF("{i;%d} * %d", GOLD_ITEM_ID, pool_gold_num);
		}

		if (pool_item_num > 0)
		{
			if (b)tmp_str += ", ";
			tmp_str += STRING_SPRINTF("{i;%d} * %d", other.pool_item_id, pool_item_num);
		}

		mail_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_content_1, cfg->pool_get_reward_percent, tmp_str.c_str());
	}

	if (cfg->broadcast == 2)
	{
		mail_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_content_2, cfg->reward.item_id);
	}

	std::string broadcast_str;
	if (cfg->broadcast == 1)
		broadcast_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_broadcast_1, role_id, role_name.c_str(), cfg->pool_get_reward_percent);
	if (cfg->broadcast == 2)
		broadcast_str = STRING_SPRINTF(gamestring::g_shen_shou_jiang_lin_broadcast_2, role_id, role_name.c_str(), cfg->reward.item_id);

	crossgameprotocal::GameHiddenShenShouJiangLinDrawRet protocol;
	protocol.req_id = req_id;
	protocol.seq = cfg->seq;
	protocol.bao_di_id = cfg->bao_di_id;
	protocol.role_id = role_id;
	protocol.gold_num = gold_num;
	protocol.items = rewards;
	protocol.mail_str = mail_str;
	protocol.broadcast_str = broadcast_str;

	return InternalComm::Instance().SendSerializeMsgToHiddenThroughCross2(&protocol) ? 0 : -__LINE__;
}

void RandActivityShenShouJiangLin::SendInfo(Role * role)
{
	Protocol::SCRAShenShouJiangLinInfo protocol;
	protocol.pool_gold_num = m_param.gold_num;
	protocol.pool_item_num = m_param.item_num;

	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&protocol, sizeof(protocol));
		InternalComm::Instance().SendToHiddenAllUserThroughCross((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
}

void RandActivityShenShouJiangLin::GameHiddenSendInfo(int role_id)
{
	Protocol::SCRAShenShouJiangLinInfo protocol;
	protocol.pool_gold_num = m_param.gold_num;
	protocol.pool_item_num = m_param.item_num;

	InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
}

int RandActivityShenShouJiangLin::AddDrawTime(int draw_times)
{
	m_param.draw_times += draw_times;
	const RandActivityShenShouJiangLinConfig * config = static_cast<const RandActivityShenShouJiangLinConfig *>
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

	const RAShenShouJiangLinOtherCfg & other = config->GetOtherCfg(ramgr);
	if (other.pool_add_item_draw_items == 0)
	{
		return -__LINE__;
	}

	int add_times = m_param.draw_times / other.pool_add_item_draw_items;
	if (add_times > 0)
	{
		m_param.draw_times = m_param.draw_times % other.pool_add_item_draw_items;
		m_param.gold_num += add_times * other.pool_add_gold_num;
		m_param.item_num += add_times * other.pool_add_item_num;
		this->SendInfo();
	}

	return 0;
}