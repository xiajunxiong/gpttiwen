#include "roleactivitycolorfuldouble.hpp"
#include "config/randactivityconfig/impl/racolorfuldoubleconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityColorfulDouble::RoleActivityColorfulDouble(int type)
	: RoleActivity(type)
{
}

RoleActivityColorfulDouble::~RoleActivityColorfulDouble()
{
}

void RoleActivityColorfulDouble::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAColorfulDoubleParam));
}

void RoleActivityColorfulDouble::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAColorfulDoubleParam));
}

void RoleActivityColorfulDouble::OnLoginSendInfo()
{
	this->AddTaskParam(RAColorfulDouble::TSAK_TYPE_LOGIN, 1, EngineAdapter::Instance().DayID());

	this->SendBaseInfo();
	this->SendTaskParam();
}

void RoleActivityColorfulDouble::OnLoginCheck()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{//跨服短时间离线会直接登录开服 屏蔽掉跨服  
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	this->CheckGive(m_param.ra_begin_timestamp);
}

void RoleActivityColorfulDouble::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (0 == old_dayid)
	{
		return;
	}
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

	{//清每日数据
		m_param.today_free_fetch_flag = 0;
		m_param.today_task_finish_flag = 0;
		memset(m_param.task_param_list, 0, sizeof(m_param.task_param_list));
		
	}
	this->SetRoleDataChange();

	//登录
	this->AddTaskParam(RAColorfulDouble::TSAK_TYPE_LOGIN, 1, now_dayid);

	this->SendBaseInfo();
	this->SendTaskParam();
}

void RoleActivityColorfulDouble::CheckInitRandActivity(int rand_act_type, bool is_force)
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
		this->CheckGive(m_param.ra_begin_timestamp);

		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(ramgr->GetRandActivityBeginTime(m_role_activity_type));

		this->AddTaskParam(RAColorfulDouble::TSAK_TYPE_LOGIN, 1, EngineAdapter::Instance().DayID());

		this->SetRoleDataChange();
	}
}

void RoleActivityColorfulDouble::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case Protocol::RA_COLORFUL_DOUBLE_REQ_TYPE_FETCH_FREE_REWARD:
		{
			this->FetchFreeReward();
		}
		break;
	case Protocol::RA_COLORFUL_DOUBLE_REQ_TYPE_ACT_BOX:
		{
			this->OnActBoxReq(raor->cfg_ver, raor->param_1);
		}
		break;
	case Protocol::RA_COLORFUL_DOUBLE_REQ_TYPE_FETCH_BOX_REWARD:
		{
			this->FetchBoxReaard(raor->param_1);
		}
		break;
	}
}

void RoleActivityColorfulDouble::OnRAClose()
{
	this->CheckGive(m_param.ra_begin_timestamp);
}

int RoleActivityColorfulDouble::OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver, int box_seq)
{
	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -2;
	}
	const RAColorfulDouble::OtherCfg &other_cfg = config->GetOtherCfg();

	unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -6;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -5;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = other_cfg.act_cost_num;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = other_cfg.act_type;
	buy_item->param2 = box_seq;


	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityColorfulDouble::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -700;
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u act_box_rewaed_flag:%d %s",
		__FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, m_param.act_box_rewaed_flag, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -701;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -702;
	}

	int box_seq = buy_item->param2;

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return this->ActBoxReward(box_seq);
}

int RoleActivityColorfulDouble::OnBuyGiftItem(int box_seq, int ARG_OUT * price)
{
	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	const RAColorfulDouble::OtherCfg & other_cfg = config->GetOtherCfg();

	time_t now = EngineAdapter::Instance().Time();

	if (price)
	{
		*price = other_cfg.act_cost_num;
		return 0;
	}

	if (0 != this->ActBoxReward(box_seq))
	{
		return 1;
	}

	return 0;
}

void RoleActivityColorfulDouble::AddTaskParam(int task_type, int task_param, unsigned int now_dayid)
{
	if (0 > task_param)
	{
		return;
	}
	
	if (0 > task_type
		|| RAColorfulDouble::TSAK_TYPE_MAX <= task_type)
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

	UNSTD_STATIC_CHECK(7 == RAColorfulDouble::TSAK_TYPE_MAX);
	switch (task_type)
	{
	case RAColorfulDouble::TSAK_TYPE_LOGIN:
		{
			m_param.task_param_list[task_type] = 1;//这个标记 客户端用的
			if (now_dayid != 0
				&& now_dayid != m_param.now_day_id)
			{
				m_param.now_day_id = EngineAdapter::Instance().DayID();
			}
			else
			{
				return;
			}
		}
		break;
	default:
		{
			m_param.task_param_list[task_type] += task_param;
		}
		break;
	}

	this->SetRoleDataChange();
	this->SendTaskParamChange(task_type, m_param.task_param_list[task_type]);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] task_type:%d task_param:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		task_type, m_param.task_param_list[task_type]);
	
	this->ChackTaskFinish(task_type);
}

void RoleActivityColorfulDouble::CheckGive(unsigned int act_begin_itme)
{
	//未激活
	if (0 == m_param.act_box_rewaed_flag)
	{
		return;
	}

	//已领
	if (-1 == m_param.fetch_box_rewaed_flag)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	for (int i = 0; i < RAColorfulDouble::BOX_COUNT; i++)
	{
		//已领
		if (IsSetBit(m_param.fetch_box_rewaed_flag, i))
		{
			continue;
		}

		//可领
		if (!IsSetBit(m_param.act_box_rewaed_flag, i))
		{
			continue;
		}

		const RAColorfulDouble::BosRewardCfg *box_cfg = config->GetBoxRewardCfg(ramgr, m_param.ra_begin_timestamp, i);
		if (NULL == box_cfg)
		{
			continue;
		}

		//活动期间任务总数
		if (box_cfg->need_finish_task_num > m_param.act_task_finish_num)
		{
			continue;
		}

		SetBit(m_param.fetch_box_rewaed_flag, i);

		//奖励
		if (!box_cfg->box_reward_vec.empty())
		{
			//发邮件
			m_role_module_mgr->GetKnapsack()->SendMail(&box_cfg->box_reward_vec[0], (short)box_cfg->box_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);
		}

		//log
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] box_seq:[%d] act_box_rewaed_flag:%d fetch_box_rewaed_flag:%d.",
			__FUNCTION__, __LINE__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			i, m_param.act_box_rewaed_flag, m_param.fetch_box_rewaed_flag);
	}

	this->SetRoleDataChange();

	//log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] end act_task_finish_num:[%d] act_box_rewaed_flag:%d fetch_box_rewaed_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.act_task_finish_num, m_param.act_box_rewaed_flag, m_param.fetch_box_rewaed_flag);

	m_param.fetch_box_rewaed_flag = -1;
}

void RoleActivityColorfulDouble::SendBaseInfo()
{
	Protocol::SCRAColorfulDoubleBase info;
	info.today_free_fetch_flag = m_param.today_free_fetch_flag;
	info.act_box_rewaed_flag = m_param.act_box_rewaed_flag;
	info.fetch_box_rewaed_flag = m_param.fetch_box_rewaed_flag;
	info.act_task_finish_num = m_param.act_task_finish_num;
	
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityColorfulDouble::SendTaskParam()
{
	Protocol::SCRAColorfulDoubleTakeParam info;
	info.task_param_count = RAColorfulDouble::TSAK_TYPE_MAX - 1;
	UNSTD_STATIC_CHECK(sizeof(info.task_param_list) == sizeof(m_param.task_param_list));
	for (int i= 0; i < RAColorfulDouble::TSAK_TYPE_MAX - 1; i++)
	{
		info.task_param_list[i] = m_param.task_param_list[i + 1];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityColorfulDouble::SendTaskParamChange(int task_type, int task_param)
{
	Protocol::SCRAColorfulDoubleTaskParamChange info;
	info.task_type = task_type;
	info.task_param = task_param;
	
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityColorfulDouble::OnActBoxReq(int cfg_ver, int box_seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAColorfulDouble::OtherCfg &other_cfg = config->GetOtherCfg();
	if (cfg_ver != other_cfg.cfg_ver)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CFG_VER_CHANGE);
		return;
	}

	//激活类型校验
	switch (other_cfg.act_type)
	{
	case RAColorfulDouble::OtherCfg::ACT_TYPE_BOX:
		{

			if (NULL == config->GetBoxRewardCfg(ramgr, m_param.ra_begin_timestamp, box_seq))
			{
				return;
			}

			if (IsSetBit(m_param.act_box_rewaed_flag, box_seq))
			{
				return;
			}
		}
		break;
	case RAColorfulDouble::OtherCfg::ACT_TYPE_TOTAL:
		{
			if (0 != m_param.act_box_rewaed_flag
				|| 0 != box_seq)
			{
				return;
			}
		}
		break;
	default:
		return;
	}

	UNSTD_STATIC_CHECK(3 == RAColorfulDouble::OtherCfg::ACT_COST_TYPE_MAX);
	switch (other_cfg.act_cost_type)
	{
	case RAColorfulDouble::OtherCfg::ACT_COST_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.act_cost_num, __FUNCTION__))
			{
				return;
			}
		}
		break;
	case RAColorfulDouble::OtherCfg::ACT_COST_TYPE_CMD_BUY:
		{//注 成功也退出 直购激活在其他地方
			long long unique_id = 0;
			int ret_num = this->OnBuyItemCheck(unique_id, cfg_ver, box_seq);
			if (ret_num == 0)
			{
				Protocol::SCCmdBuyItemCheckRet protocol;
				protocol.unique_id = unique_id;
				protocol.cfg_ver = cfg_ver;
				protocol.buy_type = BUY_TYPE;

				m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
			}

			return;
		}
		break;
	case RAColorfulDouble::OtherCfg::ACT_COST_TYPE_SILVER_COIN:
		{
			if (!m_role_module_mgr->GetMoney()
				->UseOtherCoin(other_cfg.act_cost_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	default:
		return;
	}
	
	//激活
	int ret = this->ActBoxReward(box_seq);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] act_box_rewaed_flag:%d act_cost_type:%d ret:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.act_box_rewaed_flag,
		other_cfg.act_cost_type, ret);

	this->OnParticipateRA(__FUNCTION__);
}

int RoleActivityColorfulDouble::ActBoxReward(int box_seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -1;
	}

	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -2;
	}
	const RAColorfulDouble::OtherCfg &other_cfg = config->GetOtherCfg();

	if (0 == box_seq)
	{
		m_param.act_box_rewaed_flag = -1;
	}
	else
	{
		SetBit(m_param.act_box_rewaed_flag, box_seq);
	}
	this->SetRoleDataChange();
	
	this->SendBaseInfo();

	UNSTD_STATIC_CHECK(4 == RAColorfulDouble::OtherCfg::ACT_REWARD_TYPE_MAX);
	switch (other_cfg.box_act_reward_type)
	{
	case RAColorfulDouble::OtherCfg::ACT_REWARD_TYPE_NONE:
		{

		}
		break;
	case RAColorfulDouble::OtherCfg::ACT_REWARD_TYPE_GOLD:
		{
			m_role_module_mgr->GetMoney()->AddGold(other_cfg.box_act_reward_num, __FUNCTION__);
		}
		break;
	case RAColorfulDouble::OtherCfg::ACT_REWARD_TYPE_SILVER_COIN:
		{
			m_role_module_mgr->GetMoney()->AddOtherCoin(other_cfg.box_act_reward_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
		}
		break;
	case RAColorfulDouble::OtherCfg::ACT_REWARD_TYPE_COIN_BIND:
		{
			m_role_module_mgr->GetMoney()->AddCoinBind(other_cfg.box_act_reward_num, __FUNCTION__);
		}
		break;
	default:
		return -3;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] box_seq:[%d] act_box_rewaed_flag:%d box_act_reward_type:%d box_act_reward_num:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		box_seq, m_param.act_box_rewaed_flag,
		other_cfg.box_act_reward_type, other_cfg.box_act_reward_num);

	return 0;
}

void RoleActivityColorfulDouble::FetchBoxReaard(int box_seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAColorfulDouble::BosRewardCfg *box_cfg = config->GetBoxRewardCfg(ramgr, m_param.ra_begin_timestamp, box_seq);
	if (NULL == box_cfg)
	{
		return;
	}

	//已领
	if (IsSetBit(m_param.fetch_box_rewaed_flag, box_seq))
	{
		return;
	}

	//可领
	if (!IsSetBit(m_param.act_box_rewaed_flag, box_seq))
	{
		return;
	}

	//活动天数
	const int now_act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (box_cfg->need_act_open_day > now_act_open_day)
	{
		return;
	}

	//活动期间任务总数
	if (box_cfg->need_finish_task_num > m_param.act_task_finish_num)
	{
		return;
	}

	//记录
	SetBit(m_param.fetch_box_rewaed_flag, box_seq);
	this->SetRoleDataChange();
	this->SendBaseInfo();

	//奖励
	if (!box_cfg->box_reward_vec.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)box_cfg->box_reward_vec.size(), &box_cfg->box_reward_vec[0], false))
		{
			//发邮件
			m_role_module_mgr->GetKnapsack()->SendMail(&box_cfg->box_reward_vec[0], (short)box_cfg->box_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)box_cfg->box_reward_vec.size(), &box_cfg->box_reward_vec[0], PUT_REASON_RA_COLORFUL_DOUBLE);
		}
	}

	//log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] box_seq:[%d] act_box_rewaed_flag:%d fetch_box_rewaed_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		box_seq, m_param.act_box_rewaed_flag, m_param.fetch_box_rewaed_flag);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityColorfulDouble::FetchFreeReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAColorfulDouble::FreeRewardCfg *free_cfg = config->GetFreeRewardCfg(ramgr);
	if (NULL == free_cfg)
	{
		return;
	}

	//已领
	if (0 != m_param.today_free_fetch_flag)
	{
		return;
	}

	//记录
	m_param.today_free_fetch_flag = 1;
	this->SetRoleDataChange();
	this->SendBaseInfo();
	this->OnParticipateRA(__FUNCTION__);

	//奖励
	if (!free_cfg->free_reward_vec.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)free_cfg->free_reward_vec.size(), &free_cfg->free_reward_vec[0], false))
		{
			//发邮件
			m_role_module_mgr->GetKnapsack()->SendMail(&free_cfg->free_reward_vec[0], (short)free_cfg->free_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)free_cfg->free_reward_vec.size(), &free_cfg->free_reward_vec[0], PUT_REASON_RA_COLORFUL_DOUBLE);
		}
	}
	//日志
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] today_free_fetch_flag:%d.",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.today_free_fetch_flag);
}

void RoleActivityColorfulDouble::ChackTaskFinish(int task_type)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RAColorfulDoubleConfig * config = static_cast<const RAColorfulDoubleConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	int act_open_dey = config->GetActOpenDay(ramgr, m_role_activity_type);
	const RAColorfulDouble::TaskVec *task_vec = config->GetTaskVecByType(ramgr, act_open_dey, task_type);
	if (NULL == task_vec)
	{
		return;
	}
	bool is_change = false;
	for (RAColorfulDouble::TaskVec::const_iterator task_cit = task_vec->begin()
		; task_vec->end() != task_cit
		; task_cit++)
	{
		const RAColorfulDouble::TaskCfg &task_cfg = *task_cit;

		if (task_cfg.task_param > m_param.task_param_list[task_type])
		{
			continue;
		}

		//已记录完成
		if (IsSetBit(m_param.today_task_finish_flag, task_cfg.task_index))
		{
			continue;
		}

		//记录
		SetBit(m_param.today_task_finish_flag, task_cfg.task_index);
		m_param.act_task_finish_num++;
		is_change = true;
		//log
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] task_type:%d, task_index:%d, today_task_finish_flag:%d, act_task_finish_num:%d.",
			__FUNCTION__, __LINE__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			task_type, task_cfg.task_index,
			m_param.today_task_finish_flag, m_param.act_task_finish_num);
	}

	if (is_change)
	{
		this->SetRoleDataChange();
		this->SendBaseInfo();
	}
}
