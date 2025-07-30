#include "roleactivityhuxingdayun.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityhuxingdayunconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "global/usercache/usercache.hpp"

RoleActivityHuXingDaYun::RoleActivityHuXingDaYun(int type) :RoleActivity(type)
{
}

RoleActivityHuXingDaYun::~RoleActivityHuXingDaYun()
{
}

void RoleActivityHuXingDaYun::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAHuXingDaYunParam));
}

void RoleActivityHuXingDaYun::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAHuXingDaYunParam));
}

void RoleActivityHuXingDaYun::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	this->RASendInfo();
	this->RASendListInfo();
}

void RoleActivityHuXingDaYun::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityHuXingDaYun::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 0: // 请求信息
		{
			this->RASendInfo();
			this->RASendListInfo();
		}
		break;
	case 1:	//邀请开始 p1: uid
		{
			this->RAYaoQing(raor->param_1);
		}
		break;
	case 2: //接受邀请 p1: list_index
		{
			this->RAAcceptYaoQing(raor->param_1);
		}
		break;
	case 3:	//领取奖励
		{
			this->RAFetchReward();
		}
		break;
	case 4:	//领取被邀请奖励
		{
			this->RAFetchInvuteReward();
		}
		break;
	}
}

void RoleActivityHuXingDaYun::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	if (m_param.complete_time != 0 && now_second >= m_param.complete_time)
	{
		m_param.is_reward = 1;
		m_param.complete_time = 0;

		this->RASendInfo();
		this->SetRoleDataChange();
	}
	if (m_param.be_invited_complete_time != 0 && now_second >= m_param.be_invited_complete_time)
	{
		m_param.is_be_invited_reward = 1;
		m_param.be_invited_complete_time = 0;

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityHuXingDaYun::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	if (m_param.complete_time != 0 || m_param.is_reward != 0)
	{
		if (m_param.complete_time > 0)
		{
			unsigned int zero_time = GetZeroTime((unsigned int)EngineAdapter::Instance().Time());
			if (m_param.complete_time < zero_time)
			{
				m_param.is_reward = 1;
				m_param.complete_time = 0;
			}
		}	

		if(m_param.is_reward == 1) this->RAFetchReward();
	}

	m_param.task_pos_id = 0;
	m_param.is_reward = 0;
	m_param.complete_time = 0;
	m_param.be_invited_times = 0;
	m_param.be_invited_complete_time = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}

bool RoleActivityHuXingDaYun::IsStartTask()
{
	bool is_start = true;
	if (m_param.complete_time > 0 || m_param.is_reward == 1)
		is_start = false;

	return is_start;
}

unsigned int RoleActivityHuXingDaYun::OnStartTask()
{
	if (m_param.complete_time > 0 || 1 == m_param.is_reward)
		return 0;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return 0;

	const RandActivityHuXingDaYunConfig * config = static_cast<const RandActivityHuXingDaYunConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return 0;

	const RAHuXingDaYunGiftCfg * cfg = config->GetGiftCfg(ramgr, m_param.task_pos_id);
	if (NULL == cfg)
		return 0;

	m_param.complete_time = (unsigned int)EngineAdapter::Instance().Time() + config->GetOtherCfg()->consume_time;
	
	m_role_module_mgr->NoticeNum(noticenum::NT_RA_HU_XING_DA_YUN_SUCC);

	this->RASendInfo();
	this->SetRoleDataChange();

	return m_param.complete_time;
}

bool RoleActivityHuXingDaYun::IsBeInvited()
{
	bool is_be_invited = true;
	if (m_param.be_invited_complete_time > 0 || m_param.is_be_invited_reward == 1)
		is_be_invited = false;

	return is_be_invited;
}

void RoleActivityHuXingDaYun::YaoQingTongXing(int uid)
{
	if (0 == uid)
		return;

	for (int i = 0; i < RA_HU_XING_DA_YUN_USER_NUM; i++)
	{
		if (uid == m_param.be_invited_list[i].uid)
			return;
	}

	Role *invite_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (NULL != invite_role)
	{
		RAHuXingDaYunBeInvitedData data;
		data.uid = uid;
		data.prof_id = invite_role->GetProfession();
		data.avatar_type = invite_role->GetAvatarType();
		data.headshot_id = invite_role->GetHeadshotID();
		memcpy(data.role_name, invite_role->GetName(), sizeof(GameName));

		RAHuXingDaYunBeInvitedData tem_be_invited_list[RA_HU_XING_DA_YUN_USER_NUM];

		memcpy(&tem_be_invited_list[0], &data, sizeof(RAHuXingDaYunBeInvitedData));
		memcpy(&tem_be_invited_list[1], &m_param.be_invited_list[0], sizeof(RAHuXingDaYunBeInvitedData) * (RA_HU_XING_DA_YUN_USER_NUM - 1));

		memcpy(m_param.be_invited_list, tem_be_invited_list, sizeof(m_param.be_invited_list));

		this->RASendListInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityHuXingDaYun::RASendInfo()
{
	Protocol::SCRAHuXingDaYunInfo info;
	info.complete_time = m_param.complete_time;
	info.task_pos_id = m_param.task_pos_id;
	info.task_first_pass = m_param.task_first_pass;
	info.is_reward = m_param.is_reward;

	info.is_be_invited_reward = m_param.is_be_invited_reward;
	info.be_invited_times = m_param.be_invited_times;
	info.be_invited_complete_time = m_param.be_invited_complete_time;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityHuXingDaYun::RASendListInfo()
{
	Protocol::SCRAHuXingDaYunBeInvitedInfo info;

	int count = 0;
	for (int i = 0; i < RA_HU_XING_DA_YUN_USER_NUM; i++)
	{
		if (m_param.be_invited_list[i].uid != 0)
		{
			memcpy(&info.user_info[count], &m_param.be_invited_list[i], sizeof(RAHuXingDaYunBeInvitedData));
			count++;
		}
	}

	info.count = count;
	int len = sizeof(info) - sizeof(info.user_info) + count * sizeof(RAHuXingDaYunBeInvitedData);
	m_role_module_mgr->NetSend(&info, len);
}

void RoleActivityHuXingDaYun::RAYaoQing(int invite_uid)
{
	if (0 == invite_uid)
		return;

	if (m_param.complete_time > 0 || 1 == m_param.is_reward)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_HAS_NOT_GET);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHuXingDaYunConfig * config = static_cast<const RandActivityHuXingDaYunConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAHuXingDaYunGiftCfg * cfg = config->GetGiftCfg(ramgr, m_param.task_pos_id);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	Role *invite_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(invite_uid));
	if (NULL == invite_role)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_HU_XING_DA_YUN_NOT_SERVER);
		return;
	}
	
	RoleActivityHuXingDaYun * role_activity = static_cast<RoleActivityHuXingDaYun *>
		(invite_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(m_role_activity_type));
	if (NULL != role_activity)
	{
		if (!role_activity->IsBeInvited())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_HU_XING_DA_YUN_NOT_INVITED);
			return;
		}

		if (role_activity->GetBeInvitedTimes() >= config->GetOtherCfg()->be_invite_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_HU_XING_DA_YUN_INVITED_TIMES);
			return;
		}

		role_activity->YaoQingTongXing(m_role_module_mgr->GetRole()->GetUID());
	}
}

void RoleActivityHuXingDaYun::RAAcceptYaoQing(int index)
{
	if (m_param.be_invited_complete_time > 0 || m_param.is_be_invited_reward == 1)
		return;

	if (index < 0 || index >= RA_HU_XING_DA_YUN_USER_NUM)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHuXingDaYunConfig * config = static_cast<const RandActivityHuXingDaYunConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.be_invited_times > config->GetOtherCfg()->be_invite_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}

	int uid = m_param.be_invited_list[index].uid;
	Role *invite_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (NULL == invite_role)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_HU_XING_DA_YUN_NOT_SERVER);
		return;
	}

	RoleActivityHuXingDaYun * role_activity = static_cast<RoleActivityHuXingDaYun *>
		(invite_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(m_role_activity_type));
	if (NULL != role_activity)
	{
		if (!role_activity->IsStartTask())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_HU_XING_DA_YUN_NOT_INVITED);
			return;
		}
		
		if (index < RA_HU_XING_DA_YUN_USER_NUM - 1)
		{
			RAHuXingDaYunBeInvitedData tem_be_invited_list[RA_HU_XING_DA_YUN_USER_NUM];
			memcpy(tem_be_invited_list, m_param.be_invited_list, sizeof(tem_be_invited_list));
			memcpy(&tem_be_invited_list[index], &m_param.be_invited_list[index + 1], sizeof(RAHuXingDaYunBeInvitedData) * (RA_HU_XING_DA_YUN_USER_NUM - (index + 1)));
			memcpy(m_param.be_invited_list, tem_be_invited_list, sizeof(m_param.be_invited_list));
		}
		memset(&m_param.be_invited_list[RA_HU_XING_DA_YUN_USER_NUM - 1], 0, sizeof(RAHuXingDaYunBeInvitedData));

		unsigned int time = role_activity->OnStartTask();
		if (time <= 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_HU_XING_DA_YUN_TASK_END);
			return;
		}

		m_param.be_invited_times += 1;
		m_param.be_invited_complete_time = time;
		m_param.be_invited_bos_id = role_activity->GetTaskPosId();

		this->RASendInfo();
		this->RASendListInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityHuXingDaYun::RAFetchReward()
{
	if (m_param.complete_time > 0 || 1 != m_param.is_reward)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHuXingDaYunConfig * config = static_cast<const RandActivityHuXingDaYunConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAHuXingDaYunGiftCfg * cfg = config->GetGiftCfg(ramgr, m_param.task_pos_id);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 是否首通
	bool is_first_pass = false;
	if (0 == (m_param.task_first_pass & (1 << m_param.task_pos_id))) is_first_pass = true;

	if (is_first_pass)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->first_reward.size(), &cfg->first_reward[0], false))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&cfg->first_reward[0], (short)cfg->first_reward.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutVec(cfg->first_reward, PUT_REASON_RA_HU_XING_DA_YUN);
		}

		m_param.task_first_pass |= 1 << m_param.task_pos_id;
	}
	else
	{
		const std::vector<ItemConfigData> * item_list = config->GetGroupCfg(cfg->repeat_reward_group);
		if (NULL != item_list)
		{
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list->size(), &(*item_list)[0], false))
			{
				m_role_module_mgr->GetKnapsack()->SendMail(&(*item_list)[0], (short)item_list->size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
			}
			else
			{
				m_role_module_mgr->GetKnapsack()->PutVec(*item_list, PUT_REASON_RA_HU_XING_DA_YUN);
			}
		}
	}

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] task_pos_id[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		m_param.task_pos_id);

	m_param.task_pos_id += 1;
	m_param.is_reward = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityHuXingDaYun::RAFetchInvuteReward()
{
	if (m_param.be_invited_complete_time > 0 || 1 != m_param.is_be_invited_reward)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHuXingDaYunConfig * config = static_cast<const RandActivityHuXingDaYunConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAHuXingDaYunGiftCfg * cfg = config->GetGiftCfg(ramgr, m_param.be_invited_bos_id);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const std::vector<ItemConfigData> * be_invited_item_list = config->GetGroupCfg(cfg->be_invited_reward_group);
	if (NULL != be_invited_item_list)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)be_invited_item_list->size(), &(*be_invited_item_list)[0], false))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&(*be_invited_item_list)[0], (short)be_invited_item_list->size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutVec(*be_invited_item_list, PUT_REASON_RA_HU_XING_DA_YUN);
		}
	}

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] invited_times[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		m_param.be_invited_times);

	m_param.is_be_invited_reward = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}
