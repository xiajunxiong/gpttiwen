#include "roleactivityqionglouyuyan.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "scene/fbmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "global/randactivity/randactivityimpl/randactivityqionglouyuyan.hpp"
#include "config/randactivityconfig/impl/randactivityqionglouyuyanconfig.hpp"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "global/randactivity/randactivityimpl/randactivityqionglouyuyan.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityQiongLouYuYan::RoleActivityQiongLouYuYan(int type)
	:RoleActivity(type)
{

}

RoleActivityQiongLouYuYan::~RoleActivityQiongLouYuYan()
{
}

void RoleActivityQiongLouYuYan::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAQiongLouYuYanRoleParam));
}

void RoleActivityQiongLouYuYan::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAQiongLouYuYanRoleParam));
}

void RoleActivityQiongLouYuYan::OnLogin()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	RandActivityQiongLouYuYan * rand_activity = (RandActivityQiongLouYuYan *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	if (rand_activity != NULL)
	{
		this->CheckBanquetEndTamp(rand_activity);
		this->SendRoleInfo();
		rand_activity->SendInfo(m_role_module_mgr->GetRole());
	}
}

void RoleActivityQiongLouYuYan::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN))
	{
		return;
	}
	RandActivityQiongLouYuYan * rand_activity = (RandActivityQiongLouYuYan *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	if (rand_activity == NULL)return;

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	this->CheckBanquetEndTamp(rand_activity);
	switch (raor->opera_type)
	{
	case 0:	//下发信息
		{
			this->SendRoleInfo();
			rand_activity->SendInfo(m_role_module_mgr->GetRole());
		}
		break;
	case 1://提交道具
		this->PreparationItem(raor->param_1);
		break;
	case 2://领取奖励
		this->GetReward(raor->param_1);
		break;
	case 3://吃席
		this->EatFood();
		break;
	default:
		break;
	}

}

void RoleActivityQiongLouYuYan::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_tamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
		this->SetRoleDataChange();
		this->SendRoleInfo();
	}
}

void RoleActivityQiongLouYuYan::CheckBanquetEndTamp(RandActivityQiongLouYuYan * rand_activity)
{
	if (NULL != rand_activity)
	{
		unsigned int open_times = rand_activity->GetOpenTimes();
		if (m_param.open_times != open_times)
		{
			m_param.open_times = open_times;
			m_param.OnPartEnd();
		}
	}
}

void RoleActivityQiongLouYuYan::SendRoleInfo()
{
	Protocol::SCQiongLouYuYanRoleInfo info;
	info.reward_flag = m_param.reward_flag;
	info.eat_food_times = m_param.eat_food_times;
	info.next_eat_tamp = m_param.next_eat_food_tamp;
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void RoleActivityQiongLouYuYan::GetReward(int seq)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}
	const RandActivityQiongLouYuYanConfig * config = static_cast<const RandActivityQiongLouYuYanConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN));
	if (config == NULL)return;
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN))
	{
		return;
	}
	if (m_param.join_activity != 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_QIONG_LOU_YU_YAN_CANNT_REWARD);
		return;
	}
	
	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	const RAQiongLouYuYanConfig * reward_cfg = config->GetConfigBySeq(open_day, seq);
	if (reward_cfg != NULL)
	{
		if ((m_param.reward_flag &(1 << reward_cfg->seq)) > 0)return;

		RandActivityQiongLouYuYan * activity = (RandActivityQiongLouYuYan *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
		if (activity == NULL)return;

		int accumulate_num = activity->GetAccumulate();

		if (accumulate_num < reward_cfg->prep_down)return;

		const RAQiongLouYuYanRewardConfig * sp_reward_cfg = config->RandRewardByGroup(reward_cfg->reward_group);
		if (sp_reward_cfg != NULL)
		{
			if ((short)sp_reward_cfg->m_reward_vec.size() > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)sp_reward_cfg->m_reward_vec.size(), &sp_reward_cfg->m_reward_vec[0]))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
					return;
				}
				m_param.reward_flag |= (1 << reward_cfg->seq);
				m_role_module_mgr->GetKnapsack()->PutList((short)sp_reward_cfg->m_reward_vec.size(), &sp_reward_cfg->m_reward_vec[0], PUT_REASON_QIONG_LOU_YU_YAN);
				this->SendRoleInfo();
			}
		}
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityQiongLouYuYan::PreparationItem(int item_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}
	const RandActivityQiongLouYuYanConfig * config = static_cast<const RandActivityQiongLouYuYanConfig *>
	(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN));
	if (config == NULL)return;
	
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN))
	{
		return;
	}

	const RAQiongLouYuYanOtherCfg & other_cfg = config->GetOtherCfg();
	bool true_item = false;
	for (int i = 0; i < RAQiongLouYuYanOtherCfg::MAX_ITEM_NUM; i++)
	{
		if (other_cfg.item_id_list[i] == item_id)
		{
			true_item = true;
		}
	}
	if (!true_item)return;
	
	if (!m_role_module_mgr->GetKnapsack()->HasItem(item_id, other_cfg.prep_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	bool succ_add = false;
	RandActivityQiongLouYuYan * rand_activity = (RandActivityQiongLouYuYan *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	if (rand_activity != NULL)
	{
		succ_add = rand_activity->OnRoleAdd(m_role_module_mgr->GetRole(), other_cfg.prep_num);
	}

	if (!succ_add)return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(item_id, other_cfg.prep_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	//发放奖励
	const RAQiongLouYuYanRewardConfig *  reward_cfg = config->RandRewardByGroup(other_cfg.reward_group_2);
	if (reward_cfg != NULL)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0]))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->m_reward_vec[0], (short)reward_cfg->m_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, false, RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_QIONG_LOU_YU_YAN);
		}
	}

	m_param.join_activity = 1;
	this->SendRoleInfo();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityQiongLouYuYan::EatFood()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	if (m_param.next_eat_food_tamp > EngineAdapter::Instance().Time())
	{
		return;
	}

	const RandActivityQiongLouYuYanConfig * config = static_cast<const RandActivityQiongLouYuYanConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN));
	if (config == NULL)return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN))
	{
		return;
	}

	RandActivityQiongLouYuYan * rand_activity = (RandActivityQiongLouYuYan *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	if (rand_activity == NULL)return;
	
	if (!rand_activity->CanEatFood(m_role_module_mgr->GetRole()))
	{
		return;
	}

	const RAQiongLouYuYanOtherCfg & other_cfg = config->GetOtherCfg();

	if (m_param.eat_food_times >= other_cfg.banquet_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_QIONG_LOU_YU_YAN_NO_EAT_TIMES);
		return;
	}

	int banquet_type = rand_activity->GetBanquetType();
	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	const RAQiongLouYuYanConfig * banquet_cfg = config->GetConfigByBanquetType(open_day, banquet_type);
	if (banquet_cfg == NULL)return;

	const RAQiongLouYuYanRewardConfig * reward_cfg = config->RandRewardByGroup(banquet_cfg->reward_group);

	if (reward_cfg == NULL)return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_QIONG_LOU_YU_YAN);

	m_param.eat_food_times += 1;
	m_param.next_eat_food_tamp = (unsigned int)EngineAdapter::Instance().Time() + other_cfg.refresh;
	this->SendRoleInfo();

	this->OnParticipateRA(__FUNCTION__);
}
