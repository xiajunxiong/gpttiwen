#include "roleactivitysanbaichou.hpp"
#include "config/randactivityconfig/impl/randactivitysanbaichouconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"


RoleActivitySanBaiChou::RoleActivitySanBaiChou(int type) :RoleActivity(type)
{
}

RoleActivitySanBaiChou::~RoleActivitySanBaiChou()
{
}

void RoleActivitySanBaiChou::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASanBaiChouParam));
}

void RoleActivitySanBaiChou::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RASanBaiChouParam));
}

void RoleActivitySanBaiChou::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	this->RASendAllInfo();
}

void RoleActivitySanBaiChou::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		this->SetRoleDataChange();
	}
}

void RoleActivitySanBaiChou::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
			this->RASendAllInfo();
		}
		break;
	case 1:	// 请求对应类型开始抽奖	p1:type
		{
			this->OnDraw(raor->param_1);
		}
		break;
	case 2:	// 设置预选奖励	p1:type	p2:index
		{
			this->OnSetSelect(raor->param_1, raor->param_2);
		}
		break;
	case 3:	// 领取对应类型预设奖励	p1:type
		{
			this->OnFetchReward(raor->param_1);
		}
		break;
	}
}

void RoleActivitySanBaiChou::OnRAClose()
{
	this->OnRewardBuFa();
}

void RoleActivitySanBaiChou::OnLoginCheck()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		this->OnRewardBuFa();
	}
}

void RoleActivitySanBaiChou::RASendAllInfo()
{
	Protocol::SCRASanBaiChouInfo info;
	info.draw_times = m_param.draw_times;
	info.reward_item_seq = m_param.reward_item_seq;
	memcpy(info.select, m_param.select, sizeof(info.select));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySanBaiChou::OnDraw(int type)
{
	if (type < 0 || type >= MAX_RA_SANBAI_CHOU_TYPE_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RandActivitySanBaiChouConfig * config = static_cast<const RandActivitySanBaiChouConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RASanBaiChouTypeCfg * cfg = config->GetTypeCfg(type);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < cfg->level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	RASanBaiChouTypeParam &type_param = m_param.select[type];

	// 这个类型的抽奖次数是否用完
	if (type_param.times >= cfg->extract_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SANBAI_CHOU_NOT_DRAW);
		return;
	}

	const RASanBaiChouGroupCfg * group_cfg = config->GetGroupCfg(cfg->reward_group);
	if (NULL == group_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	bool is_zhenxi = false;
	{		
		std::vector<RASanBaiChouItem> item_list = group_cfg->item_list;
		int max_rate = group_cfg->max_rate;

		for (int count = 0; count < MAX_RA_SANBAI_CHOU_DRAW_NUM; count++)
		{
			int rate = RandomNum(max_rate);
			int totla_rate = 0;
			for (int i = 0; i < (int)item_list.size(); i++)
			{
				totla_rate += item_list[i].rate;
				if (rate < totla_rate)
				{
					max_rate -= item_list[i].rate;
					if (item_list[i].is_baodi) is_zhenxi = true;

					type_param.pool[count] = item_list[i].seq;

					item_list.erase(item_list.begin() + i);
					break;
				}
			}
		}
	}
	
	if (is_zhenxi)
	{
		m_param.baodi_total_times = 0;
	}
	else
	{
		m_param.baodi_total_times += MAX_RA_SANBAI_CHOU_DRAW_NUM;
		if (m_param.baodi_total_times >= config->GetOtherCfg()->guaranteed_times)
		{
			std::vector<RASanBaiChouItem> item_list = group_cfg->baodi_item_list;
			int max_rate = group_cfg->baodi_rate;

			int rate = RandomNum(max_rate);
			int totla_rate = 0;
			for (int i = 0; i < (int)item_list.size(); i++)
			{
				totla_rate += item_list[i].rate;
				if (rate < totla_rate)
				{
					int rand_index = RandomNum(MAX_RA_SANBAI_CHOU_DRAW_NUM);

					type_param.pool[rand_index] = item_list[i].seq;

					m_param.baodi_total_times = 0;
					break;
				}
			}
		}
	}

	m_param.draw_times += MAX_RA_SANBAI_CHOU_DRAW_NUM;
	type_param.times += MAX_RA_SANBAI_CHOU_DRAW_NUM;

	this->RASendInfo(type);
}

void RoleActivitySanBaiChou::OnSetSelect(int type, int index)
{
	if (type < 0 || type >= MAX_RA_SANBAI_CHOU_TYPE_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (index >= MAX_RA_SANBAI_CHOU_DRAW_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RASanBaiChouTypeParam &type_param = m_param.select[type];
	if (index >= 0)
	{
		if (type_param.pool[index] == -1)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		type_param.select_seq = type_param.pool[index];
		memset(type_param.pool, -1, sizeof(type_param.pool));
	}

	memset(type_param.pool, -1, sizeof(type_param.pool));

	this->RASendInfo(type);
}

void RoleActivitySanBaiChou::OnFetchReward(int type)
{
	const RandActivitySanBaiChouConfig * config = static_cast<const RandActivitySanBaiChouConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 是否领取过最终奖励
	if (m_param.reward_item_seq != -1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	// 看奖励是否全部抽完
	if (m_param.draw_times != config->GetOtherCfg()->reward_extract_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SANBAI_CHOU_NOT_DRAW);
		return;
	}

	RASanBaiChouTypeParam &type_param = m_param.select[type];

	if (type_param.select_seq == -1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemConfigData * item = config->GetItemCfg(type_param.select_seq);
	if (NULL == item)
		return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(*item, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(*item, PUT_REASON_RA_SANBAI_CHOU);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(item, 1, SEND_MAIL_TYPE_TIGER_ACTIVITY, true, m_role_activity_type);
	}

	m_param.reward_item_seq = type_param.select_seq;

	this->RASendAllInfo();

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_type[%d] reward_item[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		type, item->item_id, item->num);
}

void RoleActivitySanBaiChou::RASendInfo(int type)
{
	Protocol::SCRASanBaiChouDrawInfo info;
	info.type = type;
	info.select_req = m_param.select[type].select_seq;
	info.type_times = m_param.select[type].times;
	info.draw_times = m_param.draw_times;

	memcpy(info.pool, m_param.select[type].pool, sizeof(info.pool));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySanBaiChou::OnRewardBuFa()
{
	const RandActivitySanBaiChouConfig * config = static_cast<const RandActivitySanBaiChouConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	// 是否领取过最终奖励
	if (m_param.reward_item_seq != -1)
	{
		return;
	}

	if (m_param.draw_times != config->GetOtherCfg()->reward_extract_times)
	{
		return;
	}

	std::vector<int> req_list;
	for (int i = 0; i < MAX_RA_SANBAI_CHOU_TYPE_NUM; i++)
	{
		if (m_param.select[i].select_seq == -1) continue;
		req_list.push_back(m_param.select[i].select_seq);
	}

	int rand_index = RandomNum((int)req_list.size());

	int rewatd_req = req_list[rand_index];
	const ItemConfigData * item = config->GetItemCfg(rewatd_req);
	if (NULL == item)
		return;

	m_role_module_mgr->GetKnapsack()->SendMail(item, 1, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);
	m_param.reward_item_seq = rewatd_req;

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_type[%d] reward_item[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		rand_index, item->item_id, item->num);
}

