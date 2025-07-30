#include "roleactivityheritagetreasure.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityheritagetreasureconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgraheritagetreasure.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RoleActivityHeritageTreasure::RoleActivityHeritageTreasure(int type)
	:RoleActivity(type)
{

}

RoleActivityHeritageTreasure::~RoleActivityHeritageTreasure()
{

}

void RoleActivityHeritageTreasure::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAHeritageTreasureParam));
}

void RoleActivityHeritageTreasure::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAHeritageTreasureParam));
}

void RoleActivityHeritageTreasure::OnLoginSendInfo()
{
	this->RandActivityHeritageTreasureSendInfo();
}

void RoleActivityHeritageTreasure::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_HERITAGE_TREASURE!= rand_act_type)
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

	const RandActivityHeritageTreasireConfig * const_config = static_cast<const RandActivityHeritageTreasireConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE));
	if (NULL == const_config)
	{
		return;
	}

	RandActivityHeritageTreasireConfig * config = const_cast<RandActivityHeritageTreasireConfig * >(const_config);
	if (NULL == config)
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_heritage_treasure_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_heritage_treasure_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		memset(m_param.ra_heritage_treasure_piece, 0, sizeof(m_param.ra_heritage_treasure_piece));
		memset(m_param.ra_heritage_treasure_clue, 0, sizeof(m_param.ra_heritage_treasure_clue));
		m_param.ra_heritage_treasure_is_fetch_final_reward = 0;
		m_param.ra_heritage_treasure_reserved_ch = 0;

		config->GetRandClueId(m_param.ra_heritage_treasure_piece);

		this->SetRoleDataChange();
	}
}

void RoleActivityHeritageTreasure::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
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
	case Protocol::RA_HERITAGE_TREASURE_OPERA_TYPE_GET_INFO:
		{
			this->RandActivityHeritageTreasureSendInfo();
		}
		break;
	case Protocol::RA_HERITAGE_TREASURE_OPERA_TYPE_ACTIVE_CLUES:
		{
			this->RandActivityHeritageTreasureActiveClues(raor->param_1);
		}
		break;
	case Protocol::RA_HERITAGE_TREASURE_OPERA_TYPE_FETCH_REWARD:
		{
			this->RandActivityHeritageTreasureFetchReward(raor->param_1);
		}
		break;
	case Protocol::RA_HERITAGE_TREASURE_OPERA_TYPE_FETCH_FINALE_REWARD:
		{
			this->RandActivityHeritageTreasureFetchLastReward();
		}
		break;
	}
}

void RoleActivityHeritageTreasure::OnRandActivityMakeUpReq(Protocol::CSRandActivityMakeUpReq * req)
{
	if (0 != m_param.ra_heritage_treasure_is_fetch_final_reward)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	int count = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_heritage_treasure_clue); ++i)
	{
		if (m_param.ra_heritage_treasure_clue[i] != RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD)
		{
			count++;
		}
	}
	
	if (count <= 0)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityHeritageTreasireConfig * config = static_cast<const RandActivityHeritageTreasireConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE));
	if (NULL == config)
	{
		return;
	}

	const RAHeritageTreasureOtherCfg & other_cfg = config->GetHeritageTreasureOtherCfg(ramgr);
	if (0 == other_cfg.is_complement)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE);
		return;
	}

	if (0 == other_cfg.is_complement)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE);
		return;
	}

	int end_day_time = config->GetCurRealEndDay(ramgr, RAND_ACTIVITY_TYPE_HERITAGE_TREASURE);
	if (end_day_time > other_cfg.end_day_time)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE);
		return;
	}

	switch (req->make_up_type)
	{
		case RAND_ACTIVITY_MAKE_UP_TYPE_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.coin_num * count, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		case RAND_ACTIVITY_MAKE_UP_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.gold_num * count, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
	}

	if (!other_cfg.final_reward.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)other_cfg.final_reward.size(), &other_cfg.final_reward[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)other_cfg.final_reward.size(), &other_cfg.final_reward[0], PUT_REASON_RA_HERITAGE_TREASURE);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&other_cfg.final_reward[0], 1, SEND_MAIL_TYPE_MAKE_UP_REWARD, true, RAND_ACTIVITY_TYPE_HERITAGE_TREASURE);
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_heritage_treasure_clue); ++i)
	{
		if (m_param.ra_heritage_treasure_clue[i] != RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD)
		{
			m_param.ra_heritage_treasure_clue[i] = RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD;
		}
	}

	m_param.ra_heritage_treasure_is_fetch_final_reward += 1;
	this->RandActivityHeritageTreasureSendInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityHeritageTreasure::RandActivityHeritageTreasureActiveClues(int index)
{
	if (index < 0 || index >= RA_HERITAGE_TREASURE_MAX_PIECE)
	{
		return;
	}

	int piece_num = m_param.ra_heritage_treasure_piece[index];
	unsigned char & clue_num = m_param.ra_heritage_treasure_clue[index];
	if (clue_num >= RA_HERITAGE_TREASURE_MAX_CLUE_NUM)
	{
		return;
	}

	const RandActivityHeritageTreasireConfig * config = static_cast<const RandActivityHeritageTreasireConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE));
	if (NULL == config)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RAHeritageTreasureOtherCfg & cfg = config->GetHeritageTreasureOtherCfg(ramgr);

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg.active_clue_item_id, 1, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, false))
	{
		return;
	}

	clue_num += 1;
	this->RandActivityHeritageTreasureSendInfo();

	this->SetRoleDataChange();
}

void RoleActivityHeritageTreasure::RandActivityHeritageTreasureFetchReward(int index)
{
	if (index < 0 || index >= RA_HERITAGE_TREASURE_MAX_PIECE)
	{
		return;
	}

	const RandActivityHeritageTreasireConfig * config = static_cast<const RandActivityHeritageTreasireConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE));
	if (NULL == config)
	{
		return;
	}

	int piece_num = m_param.ra_heritage_treasure_piece[index];
	unsigned char & clue_num = m_param.ra_heritage_treasure_clue[index];

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RAHeritageTreasureOtherCfg & cfg = config->GetHeritageTreasureOtherCfg(ramgr);
	if (clue_num < 1 || clue_num == RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD)
	{
		return;
	}

	const RAHeritageTreasureRewardCfg::ItemCfg * item_cfg = config->GetRandReward(ramgr);
	if (item_cfg == NULL)
	{
		return;
	}


	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(item_cfg->reward))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(item_cfg->reward, PUT_REASON_RA_HERITAGE_TREASURE);

	clue_num = RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD;
	if (item_cfg->is_broadcast != 0 && !CrossConfig::Instance().IsHiddenServer())
	{
		// 传闻
		for (std::vector<ItemConfigData>::const_iterator it = item_cfg->reward.begin(); item_cfg->reward.end() != it;++it)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_heritage_treasure_broadcast, m_role_module_mgr->GetRole()->GetName(),
								  RAND_ACTIVITY_TYPE_HERITAGE_TREASURE, it->item_id, RAND_ACTIVITY_TYPE_HERITAGE_TREASURE);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
			}
		}
	}

	this->RandActivityHeritageTreasureSendInfo();

	this->SetRoleDataChange();
}

void RoleActivityHeritageTreasure::RandActivityHeritageTreasureFetchLastReward()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_heritage_treasure_clue); ++i)
	{
		if (m_param.ra_heritage_treasure_clue[i] != RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD)
		{
			return;
		}
	}

	if (m_param.ra_heritage_treasure_is_fetch_final_reward != 0)
	{
		return;
	}

	const RandActivityHeritageTreasireConfig * config = static_cast<const RandActivityHeritageTreasireConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE));
	if (NULL == config)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RAHeritageTreasureOtherCfg & cfg = config->GetHeritageTreasureOtherCfg(ramgr);
	if (cfg.final_reward.empty())
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutList((short)cfg.final_reward.size(), &cfg.final_reward[0], PUT_REASON_RA_HERITAGE_TREASURE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_heritage_treasure_is_fetch_final_reward += 1;

	this->RandActivityHeritageTreasureSendInfo();

	this->SetRoleDataChange();

	{
		RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE);
		}
	}
}

void RoleActivityHeritageTreasure::RandActivityHeritageTreasureSendInfo()
{
	Protocol::SCRAHeritageTreasureInfo msg;

	for (int i = 0; i < RA_HERITAGE_TREASURE_MAX_PIECE; ++i)
	{
		msg.ra_heritage_treasure_piece[i] = m_param.ra_heritage_treasure_piece[i];
		msg.ra_heritage_treasure_clue[i] = m_param.ra_heritage_treasure_clue[i];
		msg.ra_heritage_treasure_is_fetch_final_reward = m_param.ra_heritage_treasure_is_fetch_final_reward;		//最终奖励领取标志
	}

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}
