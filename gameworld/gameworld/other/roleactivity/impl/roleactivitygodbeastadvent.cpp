#include "roleactivitygodbeastadvent.hpp"
#include "config/randactivityconfig/impl/randactivitygodbeastadventconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitygodbeastadvent.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "protocol/randactivity/msgragodbeastadvent.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "gamelog.h"

#include "other/funopen/funopendef.hpp"
#include "other/funopen/funopen.hpp"
#include "other/route/mailroute.hpp"

#include "gameworld/global/trademarket/trademarketmanager.hpp"
#include "other/rolemail/rolemail.hpp"

RoleActivityGodBeastAdvent::RoleActivityGodBeastAdvent(int type) :RoleActivity(type), online_task_timestamp(0)
{
}

RoleActivityGodBeastAdvent::~RoleActivityGodBeastAdvent()
{
}

void RoleActivityGodBeastAdvent::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAGodBeastAdventParam));
}

void RoleActivityGodBeastAdvent::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAGodBeastAdventParam));
}

void RoleActivityGodBeastAdvent::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityGodBeastAdvent::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	if (!activity->IsInOpen())
	{
		return;
	}

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_GOD_BEAST_ADVENT))
		return;

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
	case 1:					//请求购买门票
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
	case 2:		//领取任务奖励 p1-seq
		{
			this->ReceiveReward(raor->param_1);
		}
		break;
	case 3:		//兑换物品 p1-seq
		{
			this->ExchangeReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityGodBeastAdvent::OnLogin()
{
	online_task_timestamp = EngineAdapter::Instance().Time() + 60;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

 	if (!activity->IsInOpen())
 	{
 		return;
 	}

	if (activity->IsNextCycle(m_param.cycle_flag))		// 周期改变初始化数据, 回收道具
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] role_cycle_flag[%d] system_cycle_flag[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cycle_flag, activity->GetCycleFlag());
		m_param.cycle_flag = activity->GetCycleFlag();
		this->RecoverItem();
		m_param.CycleReset();

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] sync_cycle flag[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cycle_flag);
	}

	activity->SendCycleAllInfo(m_role_module_mgr);
	activity->SendNumAllInfo(m_role_module_mgr);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] Login!", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityGodBeastAdvent::Update(time_t now_second)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	if (!activity->IsInOpen())
	{
		return;
	}

	if (online_task_timestamp != 0 && now_second >= online_task_timestamp)
	{
		online_task_timestamp = now_second + 60;
		this->AddTaskParam(YSLSTaskCfg::TASK_TYPE_ONLINE_TIME);
	}
}

void RoleActivityGodBeastAdvent::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	// 补发未领取的任务奖励
	this->ReissueReward();

	online_task_timestamp = EngineAdapter::Instance().Time() + 60;
	m_param.DayReset();

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	if (activity->IsNextCycle(m_param.cycle_flag))		// 周期改变初始化数据, 回收道具
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] role_cycle_flag[%d] system_cycle_flag[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cycle_flag, activity->GetCycleFlag());

		m_param.cycle_flag = activity->GetCycleFlag();
		this->RecoverItem();
		m_param.CycleReset();

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] sync_cycle flag[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cycle_flag);
	}

	for (int i = 0; i < MAX_REWARD_GROUP_GOD_BEAST_ADVENT; i++)
	{
		const YSLSExchangeCfg* exchange_cfg = config->GetExchangeCfg(activity->GetRewardGroup(), i);
		if (NULL == exchange_cfg) continue;

		if (exchange_cfg->limit_type == YSLSExchangeCfg::DAY_LIMIT)
		{
			m_param.buy_limit[i] = 0;
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityGodBeastAdvent::AddTaskParam(int task_type, int task_param)
{
	if (task_type < 0 || task_type >= YSLSTaskCfg::TASK_TYPE_MAX || task_param <= 0) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	if (!activity->IsInOpen())
	{
		return;
	}

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int task_num = config->GetTaskNumCfg();
	if (task_num == 0) return;

	for (int i = 0; i < task_num; ++i)
	{
		const YSLSTaskCfg* task_cfg = config->GetTaskCfg(ramgr, i, activity->GetRewardGroup());
		if (NULL == task_cfg) continue;

		if(task_cfg->task_type == task_type)
		{
			if (!m_param.today_task_finish.IsBitSet(i))
			{
				m_param.today_task_count[i] += task_param;

				if (m_param.today_task_count[i] >= task_cfg->task_param)
				{
					m_param.today_task_count[i] = task_cfg->task_param;
					m_param.today_task_finish.SetBit(i);

					gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] task_finish[seq:%d, task_type:%d]", __FUNCTION__,
						m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), i, task_type);
				}
			}

			if (task_type == YSLSTaskCfg::TASK_TYPE_ONLINE_TIME && m_param.today_task_finish.IsBitSet(i))
			{
				online_task_timestamp = 0;
			}
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityGodBeastAdvent::ReceiveReward(int seq)
{
	if (seq < 0 || seq >= MAX_TASK_NUM_GOD_BEAST_ADVENT || !this->IsCanReceive()) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (!m_param.today_task_finish.IsBitSet(seq))		// 任务是否已完成
	{
		return;
	}

	if (m_param.is_receive_reward.IsBitSet(seq))		// 任务奖励是否已领取
	{
		return;
	}

	const YSLSTaskRewardCfg* task_reward_cfg = config->GetTaskRewardCfg(ramgr, seq, activity->GetRewardGroup());
	if (NULL == task_reward_cfg) return;

	const YSLSOtherCfg& other_cfg = config->GetOtherCfg();


	// 所有判定过了,发奖励
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(task_reward_cfg->reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(task_reward_cfg->reward_list, PUT_REASON_RA_GOD_BEAST_ADVENT);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&task_reward_cfg->reward_list[0], (int)task_reward_cfg->reward_list.size(), SEND_MAIL_TYPE_GOD_BEAST_ADVENT);
	}

	m_param.cur_like = (m_param.cur_like + task_reward_cfg->like) > config->GetOtherCfg().integral_up ? config->GetOtherCfg().integral_up : (m_param.cur_like + task_reward_cfg->like);	
	{
		Protocol::SCGodBeastAdventNotifyInfo info;
		info.add_like = task_reward_cfg->like;
		m_role_module_mgr->NetSend(&info, sizeof(info));
	}

	{	//  记录日志
		std::string log_str;
		for (int i = 0; i < (int)task_reward_cfg->reward_list.size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", task_reward_cfg->reward_list[i].item_id, task_reward_cfg->reward_list[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] cur_like[%d] receive_reward[seq:%d] reward[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cur_like, seq, log_str.c_str());
	}

	{
		const YSLSRecycleCfg* recycle_cfg = config->GetCurRecycleCfg(activity->GetRewardGroup());
		if (NULL != recycle_cfg)
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] cur_cycle_item[%d, num:%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(),m_role_module_mgr->GetRoleName(), recycle_cfg->item_id, m_role_module_mgr->GetKnapsack()->Count(recycle_cfg->item_id, true, true));
		}		
	}

	m_param.is_receive_reward.SetBit(seq);	// 置为已领取
	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityGodBeastAdvent::ReissueReward()
{
	if (!this->IsCanReceive()) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int task_num = config->GetTaskNumCfg();
	if (task_num <= 0) return;

	int add_like = 0;
	std::vector<ItemConfigData> reward_list;
	for (int i = 0; i < task_num; i++)
	{
		if (m_param.today_task_finish.IsBitSet(i) && !m_param.is_receive_reward.IsBitSet(i))
		{
			const YSLSTaskRewardCfg* task_rwcfg = config->GetTaskRewardCfg(ramgr, i, activity->GetRewardGroup());
			if(NULL == task_rwcfg) continue;

			add_like += task_rwcfg->like;

			reward_list.insert(reward_list.end(), task_rwcfg->reward_list.begin(), task_rwcfg->reward_list.end());

			m_param.is_receive_reward.SetBit(i);
		}
	}
	
	if (!reward_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_GOD_BEAST_ADVENT_YESTERDAY);		
		m_param.cur_like = (m_param.cur_like + add_like) > config->GetOtherCfg().integral_up ? config->GetOtherCfg().integral_up : (m_param.cur_like + add_like);
		if(add_like != 0)
		{
			Protocol::SCGodBeastAdventNotifyInfo info;
			info.add_like = add_like;
			m_role_module_mgr->NetSend(&info, sizeof(info));
		}	

		{	//日志
			std::string log_str;
			for (int i = 0; i < (int)reward_list.size(); i++)
			{
				log_str += STRING_SPRINTF(" [%d, %d]", reward_list[i].item_id, reward_list[i].num);
			}

			gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] cur_like[%d] reward[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.cur_like, log_str.c_str());
		}
	}

	{
		const YSLSRecycleCfg* recycle_cfg = config->GetCurRecycleCfg(activity->GetRewardGroup());
		if (NULL != recycle_cfg)
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] cur_cycle_item[%d, num:%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), recycle_cfg->item_id, m_role_module_mgr->GetKnapsack()->Count(recycle_cfg->item_id, true, true));
		}
	}
}

void RoleActivityGodBeastAdvent::ExchangeReward(int seq)
{
	if (!this->IsCanReceive()) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const YSLSExchangeCfg* exchange_cfg = config->GetExchangeCfg(activity->GetRewardGroup(), seq);
	if (NULL == exchange_cfg) return;

	// 兑换条件判断
	if (m_param.cur_like < exchange_cfg->unlock_integral)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_GOD_BEAST_ADVENT_UNLOCK_REWARD);
		return;
	}

	if (m_param.buy_limit[seq] >= exchange_cfg->limit_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(exchange_cfg->consume_item_id, exchange_cfg->consume_num, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	int ret = -1;
	if (exchange_cfg->limit_type == YSLSExchangeCfg::SERVER_LIMIT)
	{	// 全服限购道具
		if (CrossConfig::Instance().IsHiddenServer())		// 如果在跨服直接买
		{
			ret = activity->BuyServerLimitItem(m_role_module_mgr->GetUid());
		}
		else				//不在跨服传到跨服验证
		{
			crossgameprotocal::GameHiddenGodBeastAdventReq req;
			req.plat_type = LocalConfig::Instance().GetPlatType();
			req.server_id = LocalConfig::Instance().GetMergeServerId();
			req.uid = m_role_module_mgr->GetUid();
			req.seq = seq;
			InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, sizeof(req));

			return;
		}	

		if (ret < 0)
		{
			if (ret == -2)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_GOD_BEAST_ADVENT_EXCHANGE_LIMIT);
			}
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(exchange_cfg->consume_item_id, exchange_cfg->consume_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(exchange_cfg->reward.item_id, exchange_cfg->reward.num, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(exchange_cfg->reward, PUT_REASON_RA_GOD_BEAST_ADVENT);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&exchange_cfg->reward, 1, SEND_MAIL_TYPE_GOD_BEAST_ADVENT);
	}

	m_param.buy_limit[seq] += 1;

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] consume[%d,%d] reward[%d, %d] buy_limit[%d]", __FUNCTION__,m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		exchange_cfg->consume_item_id, exchange_cfg->consume_num, exchange_cfg->reward.item_id, exchange_cfg->reward.num, m_param.buy_limit[seq]);

	this->RASendInfo();
}

void RoleActivityGodBeastAdvent::OnHiddenGameBuyRecviceReq(crossgameprotocal::HiddenGameGodBeastAdventInfo* info)
{
	if (info == NULL) return;

	switch (info->ret)
	{
	case -2:
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_GOD_BEAST_ADVENT_EXCHANGE_LIMIT);
	}
	break;
	}
	if (info->ret < 0) return;

	/////////////////////////////////////////
	if (!this->IsCanReceive()) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityGodBeastAdvent * activity = (RandActivityGodBeastAdvent *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const YSLSExchangeCfg* exchange_cfg = config->GetExchangeCfg(activity->GetRewardGroup(), info->seq);
	if (NULL == exchange_cfg) return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(exchange_cfg->consume_item_id, exchange_cfg->consume_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(exchange_cfg->reward.item_id, exchange_cfg->reward.num, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(exchange_cfg->reward, PUT_REASON_RA_GOD_BEAST_ADVENT);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&exchange_cfg->reward, 1, SEND_MAIL_TYPE_GOD_BEAST_ADVENT);
	}

	m_param.buy_limit[info->seq] += 1;

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] consume[%d,%d] reward[%d, %d] buy_limit[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		exchange_cfg->consume_item_id, exchange_cfg->consume_num, exchange_cfg->reward.item_id, exchange_cfg->reward.num, m_param.buy_limit[info->seq]);

	this->RASendInfo();
}

void RoleActivityGodBeastAdvent::RecoverItem()
{
	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const std::map<int, YSLSRecycleCfg>& recycle_cfg = config->GetAllRecycleCfg();

	
	for (std::map<int, YSLSRecycleCfg>::const_iterator it = recycle_cfg.begin(); it != recycle_cfg.end(); ++it)
	{
		int count = m_role_module_mgr->GetKnapsack()->Count(it->second.item_id, true, true);

		if (!m_role_module_mgr->GetKnapsack()->ConsumeKnapsackItem(it->second.item_id, count, __FUNCTION__))
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s, role[%d, %s] count[%d] Recycle Failed!!!!", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), count);
			continue;
		}

		if(CrossConfig::Instance().IsHiddenServer())
		{
			int recover_num = 0;
			if (TradeMarketManager::Instance().ItemRecover(m_role_module_mgr->GetUid(), it->second.item_id, &recover_num))		// 交易行回收
			{
				count += recover_num;
			}
		}
		else
		{
			crossgameprotocal::GameHiddenGodBeastAdventReceiveTradeMarketReq req;
			req.uid = m_role_module_mgr->GetUid();
			req.item_id = it->second.item_id;
			req.exchange_group = it->first;
			InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, sizeof(req));
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] SendToHiddenThroughCross[uid:%d,item_id:%d]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), req.uid, req.item_id);
		}

		count += m_role_module_mgr->GetRoleMail()->ClearGodBeastAdventItem(it->second.item_id);		// 邮件回收

		if (count == 0) continue;	// 没有道具可回收

		int coin = 0, gold = 0, jade = 0;
		coin = it->second.coin * count;
		gold = it->second.gold * count;
		jade = it->second.silver_coin * count;

		std::vector<ItemConfigData> item_list;
		if (it->second.has_item)
		{
			for (size_t i = 0; i < it->second.item_list.size(); i++)
			{
				item_list.push_back(ItemConfigData(it->second.item_list[i].item_id, it->second.item_list[i].is_bind, it->second.item_list[i].num*count));
			}
		}

		{	// 发奖励邮件
			static MailContentParam contentparam; contentparam.Reset();
			if (coin > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = coin;
			if (gold > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = gold;
			if (jade > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] = jade;
			if (it->second.has_item)
			{
				FillMailContentParam(contentparam, item_list);
			}

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_god_beast_advent_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_god_beast_advent_content);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);
			}
		}

		std::string log_str;
		for (size_t i = 0; i < item_list.size(); i++)
		{
			log_str += STRING_SPRINTF("[%d, %d] ", item_list[i].item_id, item_list[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] recycle[%d, %d] reward: coin[%d] gold[%d] silver_coin[%d] item[%s]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), it->second.item_id, count, coin, gold, jade, log_str.c_str());
	}
}

void RoleActivityGodBeastAdvent::GmAddLike(int add_num)
{
	if (add_num <= 0) return;

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	m_param.cur_like = (m_param.cur_like + add_num) > config->GetOtherCfg().integral_up ? config->GetOtherCfg().integral_up : (m_param.cur_like + add_num);
	{
		Protocol::SCGodBeastAdventNotifyInfo info;
		info.add_like = add_num;
		m_role_module_mgr->NetSend(&info, sizeof(info));
	}	

	this->RASendInfo();
}


int RoleActivityGodBeastAdvent::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	int buy_cfg = config->GetBuyCfg(ramgr);
	if (buy_cfg < 0)
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
	buy_item->need_gold = buy_cfg;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;

	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityGodBeastAdvent::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
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

	m_param.zhigou_flag = 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	this->SetRoleDataChange();
	return 0;
}

void RoleActivityGodBeastAdvent::RASendInfo()
{
	Protocol::SCGodBeastAdventInfo msg;
	msg.zhigou_flag = m_param.zhigou_flag;
//	msg.last_buy_limit = 0;

	msg.cur_like = m_param.cur_like;

	msg.today_task_finish = m_param.today_task_finish;
	msg.is_receive_reward = m_param.is_receive_reward;
	memcpy(msg.today_task_count, m_param.today_task_count, sizeof(msg.today_task_count));

	memcpy(msg.buy_day_limit, m_param.buy_limit, sizeof(msg.buy_day_limit));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

