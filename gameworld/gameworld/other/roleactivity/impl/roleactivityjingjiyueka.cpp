#include "roleactivityjingjiyueka.hpp"

#include <set>
#include "gamelog.h"
#include "servercommon/errornum.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/funopen/funopen.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/couragechallenge/couragechallenge.h"

#include "protocol/randactivity/msgrajingjiyueka.hpp"

#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityjingjiyuekaconfig.hpp"

RoleActivityJingJiYueKa::RoleActivityJingJiYueKa(int type)
	: RoleActivity(type)
{
}

RoleActivityJingJiYueKa::~RoleActivityJingJiYueKa()
{
}

void RoleActivityJingJiYueKa::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAJingJiYueKaParam));

	this->OnCalcMap();
}

void RoleActivityJingJiYueKa::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAJingJiYueKaParam));
}

void RoleActivityJingJiYueKa::OnLogin()
{
	this->OnCheckLevel(__FUNCTION__);
}

void RoleActivityJingJiYueKa::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);
	this->SendAllInfo();
}

void RoleActivityJingJiYueKa::OnRAClose()
{
	this->OnSeasonEnd(__FUNCTION__);
}

void RoleActivityJingJiYueKa::Update(time_t now_second)
{
	unsigned int now = (unsigned int)now_second;
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	//1.活动关闭 2.赛季切换
	if (0 != m_param.next_switch_timestamp && (!ramgr->IsRandActivityOpen(m_role_activity_type) || now >= m_param.next_switch_timestamp))
	{
		this->OnSeasonEnd(__FUNCTION__);
		this->OnCheckLevel(__FUNCTION__);
	}
	if (0 == m_param.next_add_login_day_timestamp || now >= m_param.next_add_login_day_timestamp)
	{
		m_param.next_add_login_day_timestamp = now + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
		this->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_LOGIN, 1);
		this->SetRoleDataChange();
	}
}

void RoleActivityJingJiYueKa::OnRoleLevelUp(int old_level, int to_level)
{
	this->OnCheckLevel(__FUNCTION__);
}

void RoleActivityJingJiYueKa::OnConsumeGold(long long consume_gold)
{
	this->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_GOLD, (int)consume_gold);
}

void RoleActivityJingJiYueKa::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	//不管活动开没开
	memset(m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY], 0, sizeof(m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY]));
	if (m_param.next_add_login_day_timestamp == (unsigned int)(EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0)))
	{
		this->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_LOGIN, 1);
	}
	this->SetRoleDataChange();
	if (!ramgr->IsRandActivityOpen(m_role_activity_type) || m_param.active_role_level <= 0)
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if(NULL == config) return;

	for (int i = 0; i < ARRAY_LENGTH(m_param.day_task_list); ++i)
	{
		m_param.day_task_list[i].Reset();
	}

	m_param.day_task_count = 0;
	config->GetRandTaskByType(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY, m_param.active_role_level, m_param.day_task_count, ARRAY_ITEM_COUNT(m_param.day_task_list),
		m_param.day_task_list);
	
	this->OnCalcMap();
	this->OnCheckInitTypeProgress();
	this->OnSendTaskListInfo(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY);
}

void RoleActivityJingJiYueKa::OnWeekChange()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	//不管活动开没开
	memset(m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK], 0, sizeof(m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK]));
	this->SetRoleDataChange();

	if (!ramgr->IsRandActivityOpen(m_role_activity_type) || m_param.active_role_level <= 0)
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	for (int i = 0; i < ARRAY_LENGTH(m_param.week_task_list); ++i)
	{
		m_param.week_task_list[i].Reset();
	}

	m_param.week_task_count = 0;
	config->GetRandTaskByType(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK, m_param.active_role_level, m_param.week_task_count, ARRAY_ITEM_COUNT(m_param.week_task_list),
		m_param.week_task_list);

	this->OnCalcMap();
	this->OnCheckInitTypeProgress();
	this->OnSendTaskListInfo(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK);
}

void RoleActivityJingJiYueKa::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		if (0 == m_param.ra_begin_timestamp)
		{
			m_param.Reset();
		}
		else
		{
			//先检测一下之前是否有未领取的奖励
			this->OnSeasonEnd(__FUNCTION__);
		}

		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->OnCheckLevel(__FUNCTION__);
		this->SetRoleDataChange();
	}
}

void RoleActivityJingJiYueKa::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor || !FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_RA_JING_JI_YUE_KA))
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
		case RA_JINGJI_YUEKA_REQ_TYPE_INFO:
		{
			this->SendAllInfo();
		}
		break;
		case RA_JINGJI_YUEKA_REQ_TYPE_FETCH_COMMON:
		{
			this->OnFetchCommonReward(raor->param_1);
		}
		break;
		case RA_JINGJI_YUEKA_REQ_TYPE_FETCH_SENIOR:
		{
			this->OnFetchSeniorReward(raor->param_1);
		}
		break;
		case RA_JINGJI_YUEKA_REQ_TYPE_BUY_LEVEL:
		{
			this->OnBuyLevel(raor->param_1);
		}
		break;
		case RA_JINGJI_YUEKA_REQ_TYPE_REFRESH_TASK:
		{
			this->OnRefreshDayTask(raor->param_1);
		}
		break;
		case RA_JINGJI_YUEKA_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK:
		{
			this->OnBuyCmdCheck();
		}
		break;
	}
}

void RoleActivityJingJiYueKa::OnAddTaskTypeProgress(int task_type, int count)
{
	if(task_type < RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN || task_type >= RA_JING_JI_YUE_KA_TASK_TYPE_MAX || count < 0) return;

	bool is_add = true;
	if (this->GetSpecialTypeCount(task_type, count))
	{
		is_add = false;
	}

	if (is_add)
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.progress_list); ++i)
		{
			m_param.progress_list[i][task_type] += count;
		}
	}
	else
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.progress_list); ++i)
		{
			m_param.progress_list[i][task_type] = count;
		}
	}

	this->OnCheckTaskStatus(task_type);
	this->SetRoleDataChange();

	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] task_type[%d] count[%d] card[level:%d exp:%d] next_switch[%u]", __FUNCTION__, 
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), task_type, count, m_param.role_card_level, m_param.role_card_exp, m_param.next_switch_timestamp);
}

int RoleActivityJingJiYueKa::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	unsigned int last_switch_timestamp = (unsigned int)buy_item->param1;
	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp && last_switch_timestamp == m_param.last_switch_timestamp)
	{
		m_param.is_active_buy_flag = 1;
		this->OnSendNoticeInfo(Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_BUY_CMD_SUCC, m_param.is_active_buy_flag);
		this->SetRoleDataChange();
	}
	else
	{
		//旧赛季购买的激活,则默认补发旧赛季等级的高级奖励
		if (m_param.last_active_role_level > 0 && m_param.last_role_card_level > 0)
		{
			const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
			if (NULL != config)
			{
				const std::map<int, RandActivityJingJiYueKaBaseCfg> * base_cfg = config->GetBaseMapCfg(m_param.last_active_role_level);
				if (NULL != base_cfg)
				{
					std::vector<ItemConfigData> reward_list;
					for (std::map<int, RandActivityJingJiYueKaBaseCfg>::const_iterator it = base_cfg->begin(); it != base_cfg->end(); ++it)
					{
						if (it->first > m_param.last_role_card_level || it->first >= MAX_RA_JING_JI_YUE_KA_LEVEL_NUM) break;

						if (!it->second.extra_reward_list.empty())
						{
							reward_list.insert(reward_list.end(), it->second.extra_reward_list.begin(), it->second.extra_reward_list.end());
						}
					}
					if (!reward_list.empty())
					{
						m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_JINGJI_YUEKA_REISSUE_REWARD, false, m_role_activity_type);
					}
				}
			}
		}
		gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] last_card[level:%d exp:%d] last_switch[%u]", __FUNCTION__, m_role_module_mgr->GetUid(),
			m_role_module_mgr->GetRoleName(), m_param.last_active_role_level, m_param.last_role_card_level, m_param.last_switch_timestamp);
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_ra_jingji_yueka, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

void RoleActivityJingJiYueKa::SendAllInfo()
{
	Protocol::SCRAJingJiYueKaAllInfo info;
	info.active_role_level = m_param.active_role_level;
	info.next_switch_timestamp = m_param.next_switch_timestamp;
	info.last_switch_timestamp = m_param.last_switch_timestamp;
	info.is_active_buy_flag = m_param.is_active_buy_flag;
	info.reserve_ch = 0;
	info.role_card_level = m_param.role_card_level;
	info.role_card_exp = m_param.role_card_exp;
	info.fetch_common_flag = m_param.fetch_common_flag;
	info.fetch_adv_flag = m_param.fetch_adv_flag;
	info.reserve_sh = 0;
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(info.day_task_list) == ARRAY_ITEM_COUNT(m_param.day_task_list));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(info.week_task_list) == ARRAY_ITEM_COUNT(m_param.week_task_list));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(info.season_task_list) == ARRAY_ITEM_COUNT(m_param.season_task_list));
	info.day_task_count = GetMin((short)ARRAY_ITEM_COUNT(info.day_task_list), m_param.day_task_count);
	info.week_task_count = GetMin((short)ARRAY_ITEM_COUNT(info.week_task_list), m_param.week_task_count);
	info.season_task_count = GetMin((short)ARRAY_ITEM_COUNT(info.season_task_list), m_param.season_task_count);
	memcpy(info.day_task_list, m_param.day_task_list, sizeof(info.day_task_list[0]) * info.day_task_count);
	memcpy(info.week_task_list, m_param.week_task_list, sizeof(info.week_task_list[0]) * info.week_task_count);
	memcpy(info.season_task_list, m_param.season_task_list, sizeof(info.season_task_list[0]) * info.season_task_count);

	UNSTD_STATIC_CHECK(sizeof(info.progress_list) == sizeof(m_param.progress_list));
	memcpy(info.progress_list, m_param.progress_list, sizeof(info.progress_list));

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

bool RoleActivityJingJiYueKa::GetSpecialTypeCount(int task_type, int &count)
{
	bool is_special = true;
	switch (task_type)
	{
		case RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_RANK:
		{
			count = m_role_module_mgr->GetColosseumRank();
		}
		break;
		case RA_JING_JI_YUE_KA_TASK_TYPE_COURAGE_CHALLENGE_STAR:
		{
			count = m_role_module_mgr->GetCourageChallenge()->GetReachStarScoreCount();
		}
		break;
		case RA_JING_JI_YUE_KA_TASK_TYPE_BRAVE_GROUND:
		{
			count = m_role_module_mgr->GetBraveGroundMaxLevelByWeek();
		}
		break;
		default:
		is_special = false;
		break;
	}

	return is_special;
}

void RoleActivityJingJiYueKa::OnCheckInitTypeProgress()
{
	for (int i = RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN; i < RA_JING_JI_YUE_KA_TASK_TYPE_MAX; i++)
	{
		if (this->IsSpecialType(i))
		{
			this->OnAddTaskTypeProgress(i, 1);
		}
		else
		{
			this->OnCheckTaskStatus(i);
		}
	}
}

void RoleActivityJingJiYueKa::OnCalcMap()
{
	if(m_param.active_role_level <= 0) return;

	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if(NULL == config) return;

	m_task_type_map.clear();
	for (int i = 0; i < m_param.day_task_count && i < ARRAY_ITEM_COUNT(m_param.day_task_list); ++i)
	{
		if(m_param.day_task_list[i].task_id < 0 || 0 != m_param.day_task_list[i].finish_flag) continue;	//序号错误或者任务已完成不加入映射

		const RandActivityJingJiYueKaTaskCfg * task_cfg = config->GetTaskCfg(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY, m_param.day_task_list[i].task_id);
		if(NULL == task_cfg) continue;

		m_task_type_map[task_cfg->task_type].push_back(RAJingJiYueKaTaskSimpleInfo(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY, i));
	}

	for (int i = 0; i < m_param.week_task_count && i < ARRAY_ITEM_COUNT(m_param.week_task_list); ++i)
	{
		if (m_param.week_task_list[i].task_id < 0 || 0 != m_param.week_task_list[i].finish_flag) continue;	//序号错误或者任务已完成不加入映射

		const RandActivityJingJiYueKaTaskCfg * task_cfg = config->GetTaskCfg(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK, m_param.week_task_list[i].task_id);
		if (NULL == task_cfg) continue;

		m_task_type_map[task_cfg->task_type].push_back(RAJingJiYueKaTaskSimpleInfo(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK, i));
	}

	for (int i = 0; i < m_param.season_task_count && i < ARRAY_ITEM_COUNT(m_param.season_task_list); ++i)
	{
		if (m_param.season_task_list[i].task_id < 0 || 0 != m_param.season_task_list[i].finish_flag) continue;	//序号错误或者任务已完成不加入映射

		const RandActivityJingJiYueKaTaskCfg * task_cfg = config->GetTaskCfg(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON, m_param.season_task_list[i].task_id);
		if (NULL == task_cfg) continue;

		m_task_type_map[task_cfg->task_type].push_back(RAJingJiYueKaTaskSimpleInfo(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON, i));
	}
}

void RoleActivityJingJiYueKa::OnCheckLevel(const char * reason)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_RA_JING_JI_YUE_KA) || m_role_module_mgr->GetRole()->GetLevel() < config->GetOtherCfg().open_role_level)
	{
		return;
	}
	if (m_param.active_role_level <= 0)
	{
		this->OnSeasonStart(reason);
		this->SendAllInfo();
	}
}

void RoleActivityJingJiYueKa::OnCheckFetchStatusReissue()
{
	if (m_param.role_card_level <= 0) return;

	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const std::map<int, RandActivityJingJiYueKaBaseCfg> * base_cfg = config->GetBaseMapCfg(m_param.active_role_level);
	if(NULL == base_cfg) return;

	std::vector<ItemConfigData> reward_list;
	for (std::map<int, RandActivityJingJiYueKaBaseCfg>::const_iterator it = base_cfg->begin(); it != base_cfg->end(); ++it)
	{
		if(it->first > m_param.role_card_level || it->first >= MAX_RA_JING_JI_YUE_KA_LEVEL_NUM) break;

		if (!m_param.fetch_common_flag.IsBitSet(it->first) && it->second.is_has_base_reward)
		{
			reward_list.push_back(it->second.base_item);
		}
		//激活过+没领取+有物品奖励
		if (0 != m_param.is_active_buy_flag && !m_param.fetch_adv_flag.IsBitSet(it->first) && !it->second.extra_reward_list.empty())	
		{
			reward_list.insert(reward_list.end(), it->second.extra_reward_list.begin(), it->second.extra_reward_list.end());
		}
	}
	if (!reward_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_JINGJI_YUEKA_REISSUE_REWARD, false, m_role_activity_type);
	}
}

void RoleActivityJingJiYueKa::OnCheckTaskStatus(int task_type)
{
	if(task_type < RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN || task_type >= RA_JING_JI_YUE_KA_TASK_TYPE_MAX || m_param.active_role_level <= 0) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type) || m_param.active_role_level <= 0)
	{
		return;
	}

	std::map<int, std::vector<RAJingJiYueKaTaskSimpleInfo> >::iterator it = m_task_type_map.find(task_type);
	if (it != m_task_type_map.end())
	{
		this->OnSendSingleTypeInfo(task_type);

		const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config)
		{
			return;
		}

		int add_exp = 0;
		std::vector<int> delete_vec;
		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			short task_info_index = it->second[i].task_info_index;
			if(task_info_index < 0) continue;

			const RandActivityJingJiYueKaTaskCfg * yue_ka_task_cfg = NULL;
			RAJingJiYueKaTaskInfo * task_info = NULL;
			int notice_type = 0;

			switch (it->second[i].task_group_type)
			{
				case RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON:
				{
					if (task_info_index >= m_param.season_task_count || task_info_index >= ARRAY_ITEM_COUNT(m_param.season_task_list)) break;
					if (0 != m_param.season_task_list[task_info_index].finish_flag) break;

					const RandActivityJingJiYueKaTaskCfg *task_cfg = config->GetTaskCfg(it->second[i].task_group_type, m_param.season_task_list[task_info_index].task_id);
					if(NULL == task_cfg) break;

					yue_ka_task_cfg = task_cfg;
					task_info = &m_param.season_task_list[task_info_index];
					notice_type = Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_SEASON_TASK_FINISH;
				}
				break;
				case RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY:
				{
					if (task_info_index >= m_param.day_task_count || task_info_index >= ARRAY_ITEM_COUNT(m_param.day_task_list)) break;
					if (0 != m_param.day_task_list[task_info_index].finish_flag) break;

					const RandActivityJingJiYueKaTaskCfg *task_cfg = config->GetTaskCfg(it->second[i].task_group_type, m_param.day_task_list[task_info_index].task_id);
					if(NULL == task_cfg) break;

					yue_ka_task_cfg = task_cfg;
					task_info = &m_param.day_task_list[task_info_index];
					notice_type = Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_DAY_TASK_FINISH;
				}
				break;
				case RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK:
				{
					if (task_info_index >= m_param.week_task_count || task_info_index >= ARRAY_ITEM_COUNT(m_param.week_task_list)) break;
					if (0 != m_param.week_task_list[task_info_index].finish_flag) break;

					const RandActivityJingJiYueKaTaskCfg *task_cfg = config->GetTaskCfg(it->second[i].task_group_type, m_param.week_task_list[task_info_index].task_id);
					if(NULL == task_cfg) break;

					yue_ka_task_cfg = task_cfg;
					task_info = &m_param.week_task_list[task_info_index];
					notice_type = Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_WEEK_TASK_FINISH;
				}
				break;
				default:
				break;
			}
		
			if (NULL != yue_ka_task_cfg && NULL != task_info)
			{
				int is_can_finish = false;
				if (RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_RANK == yue_ka_task_cfg->task_type)
				{
					if (m_param.progress_list[it->second[i].task_group_type][yue_ka_task_cfg->task_type] <= (unsigned int)yue_ka_task_cfg->param1)
					{
						is_can_finish = true;
					}
				}
				else
				{
					if (m_param.progress_list[it->second[i].task_group_type][yue_ka_task_cfg->task_type] >= (unsigned int)yue_ka_task_cfg->param1)
					{
						is_can_finish = true;
					}
				}
				if (is_can_finish)
				{
					task_info->finish_flag = 1;
					add_exp += yue_ka_task_cfg->task_exp;
					delete_vec.push_back(i);
					this->OnSendNoticeInfo(notice_type, task_info_index);
					gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] active_level[%d] card[level:%d exp:%d] task[group:%d index:%d id:%d exp:%d] \
						next_switch[%u]", __FUNCTION__,	m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.active_role_level, m_param.role_card_level, \
						 m_param.role_card_exp, it->second[i].task_group_type, task_info_index, task_info->task_id, yue_ka_task_cfg->task_exp, m_param.next_switch_timestamp);
				}
			}
		}
	

		//去除已完成的任务映射
		if (!delete_vec.empty())
		{
			for (int i = 0; i < (int)delete_vec.size() && delete_vec[i] - i < (int)it->second.size(); ++i)
			{
				it->second.erase(it->second.begin() + (delete_vec[i] - i));   //注意括号不可删除,否则会造vector迭代器越界
			}
		}
	
		if (add_exp > 0)
		{
			m_param.role_card_exp += add_exp;
			this->SetRoleDataChange();
			int old_card_level = m_param.role_card_level;
			const std::map<int, RandActivityJingJiYueKaBaseCfg> * base_map = config->GetBaseMapCfg(m_param.active_role_level);
			if (NULL != base_map)
			{
				for (std::map<int, RandActivityJingJiYueKaBaseCfg>::const_iterator base_it = base_map->begin(); base_it != base_map->end(); ++base_it)
				{
					if(base_it->first < m_param.role_card_level) continue;
					if(base_it->first > m_param.role_card_level) break;
						
					if (m_param.role_card_exp >= base_it->second.upgrade_exp)
					{
						m_param.role_card_level += 1;
						m_param.role_card_exp -= base_it->second.upgrade_exp;
					}
				}
				if (old_card_level != m_param.role_card_level)
				{
					this->OnSendNoticeInfo(Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_LEVEL_UP, m_param.role_card_level);
					gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] active_level[%d] card[level:%d exp:%d] last_card_level[%d] next_switch[%u]", 
						__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.active_role_level, m_param.role_card_level, 
						m_param.role_card_exp, old_card_level, m_param.next_switch_timestamp);
				}
			}
			this->OnSendNoticeInfo(Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_EXP_CHANGE, m_param.role_card_exp);
		}
	}
}

void RoleActivityJingJiYueKa::OnSeasonStart(const char * reason)
{
	//重置相关信息
	m_param.SeasonReset();

	m_param.active_role_level = m_role_module_mgr->GetRole()->GetLevel();
	m_param.next_switch_timestamp = (unsigned int)(GetZeroTime(EngineAdapter::Instance().Time()) + MAX_RA_JING_JI_YUE_KA_DAY_NUM * SECOND_PER_DAY);
	m_param.last_switch_timestamp = (unsigned int)GetZeroTime(EngineAdapter::Instance().Time());

	//随机任务
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL != config)
	{
		config->GetRandTaskByType(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON, m_param.active_role_level, m_param.season_task_count, ARRAY_ITEM_COUNT(m_param.season_task_list),
		m_param.season_task_list);
		config->GetRandTaskByType(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY, m_param.active_role_level, m_param.day_task_count, ARRAY_ITEM_COUNT(m_param.day_task_list),
			m_param.day_task_list);
		config->GetRandTaskByType(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK, m_param.active_role_level, m_param.week_task_count, ARRAY_ITEM_COUNT(m_param.week_task_list),
			m_param.week_task_list);
	}
	//赛季刚开始,赛季需要把当天已经产生的放入(看运营需求,也可改为本周)
	memcpy(m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON], m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY], 
		sizeof(m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON]));

	this->OnCalcMap();
	this->OnCheckInitTypeProgress();
	this->SetRoleDataChange();

	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s reason[%s] user[%d, %s] active_level[%d] card[level:%d exp:%d] next_switch[%u]", __FUNCTION__, reason,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.active_role_level, m_param.role_card_level, m_param.role_card_exp, 
		m_param.next_switch_timestamp);
}

void RoleActivityJingJiYueKa::OnSeasonEnd(const char * reason)
{
	//先补发未领取奖励
	this->OnCheckFetchStatusReissue();
	//保存一些所需信息
	//防止开关活动多次调用赛季重置导致覆盖
	if (m_param.next_switch_timestamp > 0)
	{
		m_param.last_active_role_level = m_param.active_role_level;
		m_param.last_role_card_level = m_param.role_card_level;
		m_param.last_switch_timestamp = m_param.next_switch_timestamp;
	}
	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s reason[%s] user[%d, %s] card[level:%d exp:%d] next_switch[%u]", __FUNCTION__, reason,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.role_card_level, m_param.role_card_exp, m_param.next_switch_timestamp);

	//重置相关信息
	m_param.SeasonReset();
	m_task_type_map.clear();
	this->SetRoleDataChange();
}

bool RoleActivityJingJiYueKa::IsSpecialType(int task_type)
{
	UNSTD_STATIC_CHECK(RA_JING_JI_YUE_KA_TASK_TYPE_MAX == 28);
	switch (task_type)
	{
		case RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_RANK:
		case RA_JING_JI_YUE_KA_TASK_TYPE_COURAGE_CHALLENGE_STAR:
		case RA_JING_JI_YUE_KA_TASK_TYPE_BRAVE_GROUND:
		{
			return true;
		}
		default:
		break;
	}

	return false;
}

void RoleActivityJingJiYueKa::OnFetchCommonReward(int card_level)
{
	if(card_level <= 0) return;
	if (m_param.role_card_level < card_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_JINGJI_YUEKA_LEVEL_NOT_ENOUGH);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const RandActivityJingJiYueKaBaseCfg * base_cfg = config->GetBaseCfg(m_param.active_role_level, card_level);
	if (NULL == base_cfg)
	{
		return;
	}
	if (m_param.fetch_common_flag.IsBitSet(card_level))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NETHER_TASK_REWARD_HAS_GET);
		return;
	}
	
	if (base_cfg->is_has_base_reward)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(base_cfg->base_item.item_id, base_cfg->base_item.num, false))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->Put(base_cfg->base_item, PUT_REASON_RA_JINGJI_YUEKA_REWARD))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
			return;
		}
	}
	
	m_param.fetch_common_flag.SetBit(card_level);
	this->SetRoleDataChange();
	this->OnSendNoticeInfo(Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_FETCH_COMMON_SUCC, card_level);

	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] card[level:%d exp:%d] fetch_level[%d] next_switch[%u]", __FUNCTION__, m_role_module_mgr->GetUid(),
		m_role_module_mgr->GetRoleName(), m_param.role_card_level, m_param.role_card_exp, card_level, m_param.next_switch_timestamp);
}

void RoleActivityJingJiYueKa::OnFetchSeniorReward(int card_level)
{
	if (card_level <= 0) return;
	if (m_param.role_card_level < card_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_JINGJI_YUEKA_LEVEL_NOT_ENOUGH);
		return;
	}
	if (0 == m_param.is_active_buy_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_JINGJI_YUEKA_NOT_ACTIVE);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const RandActivityJingJiYueKaBaseCfg * base_cfg = config->GetBaseCfg(m_param.active_role_level, card_level);
	if (NULL == base_cfg)
	{
		return;
	}
	if (m_param.fetch_adv_flag.IsBitSet(card_level))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NETHER_TASK_REWARD_HAS_GET);
		return;
	}
	if (!base_cfg->extra_reward_list.empty())
	{	
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(base_cfg->extra_reward_list, false))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutVec(base_cfg->extra_reward_list, PUT_REASON_RA_JINGJI_YUEKA_REWARD))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
			return;
		}
	}

	m_param.fetch_adv_flag.SetBit(card_level);
	this->SetRoleDataChange();
	this->OnSendNoticeInfo(Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_FETCH_SENIOR_SUCC, card_level);

	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] card[level:%d exp:%d] fetch_level[%d] next_switch[%u]", __FUNCTION__, m_role_module_mgr->GetUid(),
		m_role_module_mgr->GetRoleName(), m_param.role_card_level, m_param.role_card_exp, card_level, m_param.next_switch_timestamp);
}

void RoleActivityJingJiYueKa::OnBuyLevel(int buy_level)
{
	if (buy_level <= 0 || m_param.active_role_level <= 0) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	
	const RandActivityJingJiYueKaOtherCfg & other_cfg = config->GetOtherCfg();
	if (!other_cfg.is_open)
	{
		return;
	}

	int open_day = GetDayIndex((time_t)m_param.last_switch_timestamp, EngineAdapter::Instance().Time()) + 1;
	if (open_day < other_cfg.open_day)
	{	
		return;
	}

	int max_card_level = config->GetMaxCardLevel(m_param.active_role_level);
	if (m_param.role_card_level + buy_level > max_card_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_JINGJI_YUEKA_LEVEL_LIMIT);
		return;
	}
	int price_count = other_cfg.buy_gold * buy_level;
	if (price_count > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(price_count, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	m_param.role_card_level += buy_level;
	this->SetRoleDataChange();
	this->OnSendNoticeInfo(Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_BUY_LEVEL, m_param.role_card_level);

	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] card[level:%d exp:%d] buy_level[%d] next_switch[%u]", __FUNCTION__, m_role_module_mgr->GetUid(),
		m_role_module_mgr->GetRoleName(), m_param.role_card_level, m_param.role_card_exp, buy_level, m_param.next_switch_timestamp);
}

void RoleActivityJingJiYueKa::OnRefreshDayTask(int task_index)
{
	if(task_index < 0 || task_index >= m_param.day_task_count) return;
	if(m_param.active_role_level <= 0) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RandActivityJingJiYueKaOtherCfg & other_cfg = config->GetOtherCfg();
	if (0 != m_param.day_task_list[task_index].finish_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_JINGJI_YUEKA_TASK_FINISH_NOT_REFRESH);
		return;
	}
	if (m_param.day_task_list[task_index].refresh_num >= MAX_RA_JING_JI_YUE_KA_DAY_TASK_REFRESH_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COOL_BREEZE_NOT_REFRESH);
		return;
	}

	const RandActivityJingJiYueKaTaskCfg * task_cfg = config->GetTaskCfg(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY, m_param.day_task_list[task_index].task_id);
	if (NULL == task_cfg)
	{
		return;
	}
	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(other_cfg.day_task_refresh_gold))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	//先获取刷新结果
	std::set<int> accept_task_list;
	for (int i = 0; i < m_param.day_task_count && i < ARRAY_ITEM_COUNT(m_param.day_task_list); i++)
	{
		if(m_param.day_task_list[i].task_id < 0) continue;

		accept_task_list.insert(m_param.day_task_list[i].task_id);
	}
	int new_task_id = 0, new_task_type = 0;
	if (!config->GetSingleRandDayTask(accept_task_list,new_task_id, new_task_type))
	{
		return;
	}
	//再消耗元宝
	if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.day_task_refresh_gold, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	//移除映射
	std::map<int, std::vector<RAJingJiYueKaTaskSimpleInfo> >::iterator it = m_task_type_map.find(task_cfg->task_type);
	if(it != m_task_type_map.end() && !it->second.empty())
	{
		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			if (RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY == it->second[i].task_group_type && task_index == it->second[i].task_info_index)
			{
				it->second.erase(it->second.begin() + i);
				break;
			}
		}
	}

	//最后更新任务信息
	m_param.day_task_list[task_index].task_id = new_task_id;
	m_param.day_task_list[task_index].refresh_num += 1;
	m_task_type_map[new_task_type].push_back(RAJingJiYueKaTaskSimpleInfo(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY, task_index));
	this->OnSendRefreshInfo(task_index, m_param.day_task_list[task_index]);	//先下发任务信息
	if (this->IsSpecialType(new_task_type))
	{
		this->OnAddTaskTypeProgress(new_task_type, 1);
	}
	else
	{
		this->OnCheckTaskStatus(new_task_type);
	}
	this->SetRoleDataChange();

	gamelog::g_log_ra_jingji_yueka.printf(LL_INFO, "%s user[%d, %s] card[level:%d exp:%d] task[index:%d, id:%d] next_switch[%u]", __FUNCTION__, m_role_module_mgr->GetUid(),
		m_role_module_mgr->GetRoleName(), m_param.role_card_level, m_param.role_card_exp, task_index, new_task_id, m_param.next_switch_timestamp);
}

void RoleActivityJingJiYueKa::OnBuyCmdCheck()
{
	if (0 != m_param.is_active_buy_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_JINGJI_YUEKA_ACTIVED);
		return;
	}
	if(m_param.active_role_level <= 0) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityJingJiYueKaConfig * config = static_cast<const RandActivityJingJiYueKaConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RandActivityJingJiYueKaOtherCfg & other_cfg = config->GetOtherCfg();
	if (other_cfg.buy_price <= 0)
	{
		return;
	}

	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}
	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = other_cfg.buy_price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = m_param.last_switch_timestamp;
	buy_item->param2 = 0;
	buy_item->param3 = 0;
	buy_item->rewards_num = 0;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_JINGJI_YUEKA;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = buy_item->unique_id;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityJingJiYueKa::OnSendNoticeInfo(int notice_type, int param1)
{
	if(notice_type < 0 || notice_type >= Protocol::SCRAJingJiYueKaNotice::NOTICE_TYPE_MAX) return;

	Protocol::SCRAJingJiYueKaNotice info;
	info.notice_type = notice_type;
	info.param1 = param1;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityJingJiYueKa::OnSendRefreshInfo(int task_index, const RAJingJiYueKaTaskInfo & task_info)
{
	if(task_index < 0 || task_index >= MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM) return;	//当前只有每日任务可以刷新

	Protocol::SCRAJingJiYueKaRefreshNotice info;
	info.task_index = task_index;
	info.task_info = task_info;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityJingJiYueKa::OnSendSingleTypeInfo(int task_type)
{
	if(task_type < RA_JING_JI_YUE_KA_TASK_TYPE_BEGIN || task_type >= RA_JING_JI_YUE_KA_TASK_TYPE_MAX) return;

	Protocol::SCRAJingJiYueKaSingleTypeInfo info;
	info.task_type = task_type;
	info.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON] = m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON][task_type];
	info.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY] = m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY][task_type];
	info.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK] = m_param.progress_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK][task_type];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityJingJiYueKa::OnSendTaskListInfo(int task_group_type)
{
	if(task_group_type < RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_BEGIN || task_group_type >= RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX) return;

	Protocol::SCRAJingJiYueKaTaskListInfo info;
	info.task_group_type = task_group_type;
	info.task_count = 0;
	const RAJingJiYueKaTaskInfo * task_list = NULL;
	switch (task_group_type)
	{
		case RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_SEASON:
		{
			info.task_count = GetMin(m_param.season_task_count, (short)ARRAY_ITEM_COUNT(m_param.season_task_list));
			task_list = m_param.season_task_list;
		}
		break;
		case RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_DAY:
		{
			info.task_count = GetMin(m_param.day_task_count, (short)ARRAY_ITEM_COUNT(m_param.day_task_list));
			task_list = m_param.day_task_list;
		}
		break;
		case RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_WEEK:
		{
			info.task_count = GetMin(m_param.week_task_count, (short)ARRAY_ITEM_COUNT(m_param.week_task_list));
			task_list = m_param.week_task_list;
		}
		break;
		default:
		return;
	}

	info.task_count = GetMin(info.task_count, (short)ARRAY_ITEM_COUNT(info.task_list));
	if(NULL == task_list || info.task_count <= 0) return;

	memcpy(info.task_list, task_list, sizeof(info.task_list[0]) * info.task_count);

	int length = sizeof(info) - (ARRAY_ITEM_COUNT(info.task_list) - info.task_count) * sizeof(info.task_list[0]);
	m_role_module_mgr->NetSend(&info, length);	
}
