#include "roleactivityshanhaimet.hpp"
#include "config/randactivityconfig/impl/rashanhaimetconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "obj/character/role.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/string/gameworldstr.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

#include "gamelog.h"

RoleActivityShanHaiMet::RoleActivityShanHaiMet(int type)
	: RoleActivity(type)
{
}

RoleActivityShanHaiMet::~RoleActivityShanHaiMet()
{
}

void RoleActivityShanHaiMet::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAShanHaiMetParam));
}

void RoleActivityShanHaiMet::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAShanHaiMetParam));
}

void RoleActivityShanHaiMet::OnLoginSendInfo()
{
	this->SendBaseInfo();
	this->SendTaskParam();
}

void RoleActivityShanHaiMet::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	this->CheckInitRandActivity(m_role_activity_type);

	const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const int now_act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type) - 1;
	this->CheckToDayTaskFinish(now_act_open_day);

	m_param.today_task_finish = 0;
	// 改需求  任务完成还是按照每日统计
	// 达成条件改为活动期间总次数
	//for (int i =0
	//	;	i < RAShanHaiMet::DAY_RESET_TASK_CONUT
	//		&& i < RAShanHaiMet::TSAK_TYPE_MAX
	//	; i++)
	//{
	//	m_param.task_param_list[RAShanHaiMet::DAY_RESET_TASK_[i]] = 0;
	//}

	this->SetRoleDataChange();
	this->OnLoginSendInfo();
}

void RoleActivityShanHaiMet::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

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

void RoleActivityShanHaiMet::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case Protocol::RA_SHAN_HAI_MET_REQ_TYPE_FETCH_DAILY_REWARD:
		{
			switch (raor->param_2)
			{
			case RAShanHaiMet::DAILY_REWARD_TYPE_NORMAL:
				{
					this->FetchNormal(raor->param_1);
				}
				break;
			case RAShanHaiMet::DAILY_REWARD_TYPE_UNFINISH:
				{
					this->FetchUnfinish(raor->param_1);
				}
				break;
			case RAShanHaiMet::DAILY_REWARD_TYPE_EXTRA:
				{
					//this->FetchExtra(raor->param_1);
				}
				break;
			case RAShanHaiMet::DAILY_REWARD_TYPE_ALL:
				{
					//this->FetchNormal(raor->param_1);
					//this->FetchExtra(raor->param_1);
				}
				break;
			default:
				return;
			}
		}
		break;
	case Protocol::RA_SHAN_HAI_MET_REQ_TYPE_CMD_BUY:
		{
			long long unique_id = 0;
			int ret_num = this->OnBuyItemCheck(unique_id, raor->cfg_ver);
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
	}
}

int RoleActivityShanHaiMet::OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver)
{
	if (0 != m_param.act_extra_rewaed_flag)
	{
		return -1;
	}

	const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -2;
	}

	const RAShanHaiMet::OtherCfg &other_cfg = config->GetOtherCfg();
	unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -3;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -4;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = other_cfg.high_reward_buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityShanHaiMet::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -1;
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u act_extra_rewaed_flag:%d %s",
		__FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, m_param.act_extra_rewaed_flag, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -2;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -3;
	}

	m_param.act_extra_rewaed_flag = 1;
	this->SetRoleDataChange();
	this->SendBaseInfo();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityShanHaiMet::OnBuyGiftItem(ARG_OUT int * price)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}
	if (1 == m_param.act_extra_rewaed_flag)
	{
		return 2;
	}
	const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	if (NULL != price)
	{
		 *price = config->GetOtherCfg().high_reward_buy_money;
		 return 0;
	}

	m_param.act_extra_rewaed_flag = 1;
	this->SetRoleDataChange();
	this->SendBaseInfo();

	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u act_extra_rewaed_flag:%d",
		__FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, m_param.act_extra_rewaed_flag);

	return 0;
}

void RoleActivityShanHaiMet::AddTaskParam(int task_type, int task_param)
{
	if (0 > task_param)
	{
		return;
	}
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	this->CheckInitRandActivity(m_role_activity_type);

	UNSTD_STATIC_CHECK(7 == RAShanHaiMet::TSAK_TYPE_MAX);
	switch (task_type)
	{
	case RAShanHaiMet::TSAK_TYPE_SUMMER_LI_LIAN:
	case RAShanHaiMet::TSAK_TYPE_COLORFUL_LANTERN:
	case RAShanHaiMet::TSAK_TYPE_HAPPY_LOOK:
	case RAShanHaiMet::TSAK_TYPE_SHANHAI_QUESTION:
	case RAShanHaiMet::TSAK_TYPE_INTEGRAL_DRAW:
	case RAShanHaiMet::TSAK_TYPE_LOTUS_MI_BAO_ACT_ROW:
	case RAShanHaiMet::TSAK_TYPE_LOTUS_MI_BAO_ACT_PAGE:
		{
			m_param.task_param_list[task_type] += task_param;
			this->SetRoleDataChange();

			this->SendTaskParamChange(task_type, m_param.task_param_list[task_type]);
		}
		break;
	default:
		return;
	}
	
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] task_type:%d task_param:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		task_type, m_param.task_param_list[task_type]);

	if(0 == m_param.today_task_finish)
	{//客户端需要服务器提前校验完成情况
		const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config)
		{
			return;
		}
		const int now_act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
		this->CheckToDayTaskFinish(now_act_open_day);

		if (1 == m_param.today_task_finish)
		{
			this->SendBaseInfo();
		}
	}
}

void RoleActivityShanHaiMet::SendBaseInfo()
{
	Protocol::SCRAShanHaiMetBase info;
	info.act_extra_rewaed_flag = m_param.act_extra_rewaed_flag;
	info.day_task_finish_flag = m_param.day_task_finish_flag;
	info.fetch_reward_flag = m_param.fetch_reward_flag;
	info.fetch_extra_rewaed_flag = m_param.fetch_extra_rewaed_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityShanHaiMet::SendTaskParam()
{
	Protocol::SCRAShanHaiMetTakeParam info;
	info.task_param_count = RAShanHaiMet::TSAK_TYPE_MAX;
	UNSTD_STATIC_CHECK(sizeof(info.task_param_list) == sizeof(m_param.task_param_list));
	memcpy(info.task_param_list, m_param.task_param_list, sizeof(info.task_param_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityShanHaiMet::SendTaskParamChange(int task_type, int task_param)
{
	Protocol::SCRAShanHaiMetTaskParamChange info;
	info.task_type = task_type;
	info.task_param = task_param;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityShanHaiMet::FetchNormal(const int fetch_index)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAShanHaiMet::DailyCfg *daily_cfg = config->GetDailyCfg(ramgr, fetch_index);
	if (NULL == daily_cfg)
	{
		return;
	}
	
	if (1 != m_param.act_extra_rewaed_flag)
	{//额外奖励未激活
		return;
	}

	if (IsSetBit(m_param.fetch_reward_flag, fetch_index))
	{//已领
		return;
	}

	const int now_act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (now_act_open_day == daily_cfg->act_day_index + 1)
	{
		this->CheckToDayTaskFinish(now_act_open_day);
	}

	if (!IsSetBit(m_param.day_task_finish_flag, fetch_index))
	{//任务未完成
		return;
	}

	//记录
	SetBit(m_param.fetch_reward_flag, fetch_index);
	this->SetRoleDataChange();

	//奖励
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(daily_cfg->normal_reward_vec, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(daily_cfg->normal_reward_vec, PUT_REASON_SHAN_HAI_MET_REWARD);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, daily_cfg->normal_reward_vec);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_act_full_bag_title,
			m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_act_full_bag_content,
			m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	this->SendBaseInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] fetch_index:%d fetch_reward_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		fetch_index, m_param.fetch_reward_flag);
}

void RoleActivityShanHaiMet::FetchUnfinish(const int fetch_index)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAShanHaiMet::DailyCfg *daily_cfg = config->GetDailyCfg(ramgr, fetch_index);
	if (NULL == daily_cfg)
	{
		return;
	}

	if (1 != m_param.act_extra_rewaed_flag)
	{//额外奖励未激活
		return;
	}

	const int now_act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (IsSetBit(m_param.fetch_reward_flag, fetch_index))
	{//已领
		return;
	}

	if (daily_cfg->act_day_index + 1 > now_act_open_day)
	{//没超过完成时限
		return;
	}

	if (IsSetBit(m_param.day_task_finish_flag, fetch_index))
	{//任务已完成 未完成才是这个奖励
		return;
	}

	//记录
	SetBit(m_param.fetch_reward_flag, fetch_index);
	this->SetRoleDataChange();

	//奖励
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(daily_cfg->unfinish_reward_vec, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(daily_cfg->unfinish_reward_vec, PUT_REASON_SHAN_HAI_MET_REWARD);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, daily_cfg->unfinish_reward_vec);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_act_full_bag_title,
			m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_act_full_bag_content,
			m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	this->SendBaseInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] fetch_index:%d fetch_reward_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		fetch_index, m_param.fetch_reward_flag);
}

void RoleActivityShanHaiMet::FetchExtra(const int fetch_index)
{
	//修改 删除额外奖励
	return;
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAShanHaiMet::DailyCfg *daily_cfg = config->GetDailyCfg(ramgr, fetch_index);
	if (NULL == daily_cfg)
	{
		return;
	}

	if (1 != m_param.act_extra_rewaed_flag)
	{//额外奖励未激活
		return;
	}

	if (IsSetBit(m_param.fetch_extra_rewaed_flag, fetch_index))
	{//已领
		return;
	}

	const int now_act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (now_act_open_day == daily_cfg->act_day_index + 1)
	{
		this->CheckToDayTaskFinish(now_act_open_day);
	}

	if (!IsSetBit(m_param.day_task_finish_flag, fetch_index))
	{//任务未完成
		return;
	}

	//记录
	SetBit(m_param.fetch_extra_rewaed_flag, fetch_index);
	this->SetRoleDataChange();

	//奖励
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(daily_cfg->extra_reward_vec, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(daily_cfg->extra_reward_vec, PUT_REASON_SHAN_HAI_MET_REWARD);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, daily_cfg->extra_reward_vec);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_act_full_bag_title,
			m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_act_full_bag_content,
			m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	this->SendBaseInfo();

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] fetch_index:%d fetch_extra_rewaed_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		fetch_index, m_param.fetch_extra_rewaed_flag);
}

void RoleActivityShanHaiMet::CheckToDayTaskFinish(const int now_act_open_day)
{
	if (1 == m_param.today_task_finish)
	{//任务完成
		return;
	}

	const RAShanHaiMetConfig * config = static_cast<const RAShanHaiMetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAShanHaiMet::DailyCfg *daily_cfg = config->GetDailyCfg(ramgr, now_act_open_day - 1);
	if (NULL == daily_cfg)
	{
		return;
	}

	//任务组
	const RAShanHaiMet::TaskGroupCfg * task_group_cfg = config->GetTaskGroupCfg(daily_cfg->task_group_id);
	if (NULL == task_group_cfg)
	{
		return;
	}

	bool day_task_finish = true;
	for (RAShanHaiMet::TaskGroupCfg::const_iterator task_cit = task_group_cfg->begin()
		; task_cit != task_group_cfg->end()
		; ++task_cit)
	{
		const RAShanHaiMet::TaskCfg &task_cfg = task_cit->second;

		UNSTD_STATIC_CHECK(7 == RAShanHaiMet::TSAK_TYPE_MAX);
		switch (task_cfg.task_type)
		{
		case RAShanHaiMet::TSAK_TYPE_SUMMER_LI_LIAN:
		case RAShanHaiMet::TSAK_TYPE_COLORFUL_LANTERN:
		case RAShanHaiMet::TSAK_TYPE_HAPPY_LOOK:
		case RAShanHaiMet::TSAK_TYPE_SHANHAI_QUESTION:
		case RAShanHaiMet::TSAK_TYPE_INTEGRAL_DRAW:
		case RAShanHaiMet::TSAK_TYPE_LOTUS_MI_BAO_ACT_ROW:
		case RAShanHaiMet::TSAK_TYPE_LOTUS_MI_BAO_ACT_PAGE:
			{
				if (m_param.task_param_list[task_cfg.task_type] < task_cfg.task_param_1)
				{
					day_task_finish = false;
					return;
				}
			}
			break;
		default:
			return;
		}
	}
	if (!day_task_finish)
	{
		return;
	}
	//记录
	m_param.today_task_finish = 1;
	SetBit(m_param.day_task_finish_flag, daily_cfg->act_day_index);
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] now_act_open_day:%d day_task_finish_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		now_act_open_day, m_param.day_task_finish_flag);
}
