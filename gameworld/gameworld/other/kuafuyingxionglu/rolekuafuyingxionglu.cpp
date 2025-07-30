#include "rolekuafuyingxionglu.hpp"
#include "gamelog.h"

#include "item/knapsack.h"
#include "obj/character/role.h"

#include "skill/skillpool.hpp"
#include "equipment/equipmentmanager.h"
#include "global/xunbao/xunbaoconfig.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/kuafuyingxiongluconfig.hpp"

#include "servercommon/errornum.h"

#include "other/pet/pet.hpp"
#include "other/medal/medal.hpp"
#include "other/partner/partner.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/capability/capability.hpp"

#include "protocol/msgkuafuyingxionglu.hpp"

RoleKuaFuYingXiongLu::RoleKuaFuYingXiongLu()
{
}

RoleKuaFuYingXiongLu::~RoleKuaFuYingXiongLu()
{
}

void RoleKuaFuYingXiongLu::Init(RoleModuleManager * module_mgr, const RoleKuaFuYingXiongParam & param)
{
	m_role_module_mgr = module_mgr;
	m_param = param;
}

void RoleKuaFuYingXiongLu::GetInitParam(RoleKuaFuYingXiongParam * param)
{
	*param = m_param;
}

void RoleKuaFuYingXiongLu::OnRoleLogin()
{
	this->OnCheckInitActivity();
	this->OnCheckTime();
	this->SendAllInfo();
}

void RoleKuaFuYingXiongLu::OnUpdate(unsigned int now_second)
{
	this->OnCheckTime();
}

void RoleKuaFuYingXiongLu::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	
	m_param.user_recently_participate_timestamp = (unsigned int)GetZeroTime(EngineAdapter::Instance().Time());
}

void RoleKuaFuYingXiongLu::OnCheckInitActivity()
{
	if (!KuaFuYingXiongLuManager::Instance().IsOpen())
	{
		return;
	}

	if ((0 != KuaFuYingXiongLuManager::Instance().GetActivityUniqueID()) && (KuaFuYingXiongLuManager::Instance().GetActivityUniqueID() != m_param.activity_unique_id))
	{
		this->OnCheckTime();
		m_param.Reset();
		m_param.activity_begin_timestamp = KuaFuYingXiongLuManager::Instance().GetActivityBeginTimestamp();
		m_param.user_begin_participate_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.activity_open_cross_day = KuaFuYingXiongLuManager::Instance().GetActivityOpenCrossDay();
		m_param.activity_unique_id = KuaFuYingXiongLuManager::Instance().GetActivityUniqueID();
		m_param.user_recently_participate_timestamp = (unsigned int)GetZeroTime(EngineAdapter::Instance().Time());
		this->OnCheckAllTaskData(true);
	}
}

void RoleKuaFuYingXiongLu::OnFetchTaskReward(int rank_type, int seq)
{
	if (this->IsInvalidRankType(rank_type) || this->IsInvalidTaskSeq(seq))
	{
		return;
	}
	if (KuaFuYingXiongLuManager::Instance().GetActivityUniqueID() != m_param.activity_unique_id)
	{
		return;
	}

	if (!KuaFuYingXiongLuManager::Instance().IsOpen())
	{
		return;
	}
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	int activity_open_day = World::GetInstWorld()->GetDayByTimestamp(m_param.activity_begin_timestamp, now_second);
	const KuaFuYingXiongLuTimeCfg * time_cfg = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTimeCfg(rank_type);
	if (NULL == time_cfg)
	{
		return;
	}
	if (activity_open_day < time_cfg->start_time)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KUA_FU_YING_XIONG_LU_FETCH_FAIL);
		return;
	}
	if (!KuaFuYingXiongLuManager::Instance().IsRankInTime(rank_type))
	{
		return;
	}
	
	const KuaFuYingXiongLuTaskCfg * task_cfg = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTaskCfg(m_param.activity_open_cross_day, rank_type, seq);
	if (NULL == task_cfg)
	{
		return;
	}
	KuaFuYingXiongLuTaskInfo & task_info = m_param.rank_info_list[rank_type].task_list[seq];
	if (0 != task_info.is_fetch_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	if (task_info.schedule < task_cfg->param2)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(task_cfg->reward_list))
	{
		if (m_role_module_mgr->GetKnapsack()->PutVec(task_cfg->reward_list, PUT_REASON_KUA_FU_YING_XIONG_LU))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(task_cfg->reward_list, SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_BAG_FULL);
	}

	task_info.is_fetch_flag = 1;
	this->SendSingleTaskInfo(rank_type, seq, task_info);
	gamelog::g_log_kua_fu_ying_xiong_lu.printf(LL_INFO, "%s user[%d, %s] finish task[rank_type:%d seq:%d]", __FUNCTION__, 
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), rank_type, seq);
}

void RoleKuaFuYingXiongLu::SendAllInfo()
{
	Protocol::SCKuaFuYingXiongLuRoleAllInfo info;
	info.count = 0;

	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.rank_info_list) && info.count < ARRAY_ITEM_COUNT(info.rank_info_list) && i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++i)
	{
		const KFYXLRoleRankTypeItem & rank_item = m_param.rank_info_list[i];
		KFYXLRoleRankTypeInfo & rank_info = info.rank_info_list[info.count++];
		rank_info.count = 0;
		for (int task_index = 0; task_index < ARRAY_ITEM_COUNT(rank_item.task_list) && rank_info.count < ARRAY_ITEM_COUNT(rank_info.task_list); ++task_index)
		{
			if (rank_item.task_list[task_index].IsInvalid())
			{
				break;
			}
			rank_info.task_list[rank_info.count++] = rank_item.task_list[task_index];
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleKuaFuYingXiongLu::SendSingleRankInfo(int rank_type, KFYXLRoleRankTypeItem & rank_item)
{
	Protocol::SCKuaFuYingXiongLuRoleSingleRankInfo info;
	info.rank_type = rank_type;

	info.rank_info.count = 0;
	for (int task_index = 0; task_index < ARRAY_ITEM_COUNT(rank_item.task_list) && info.rank_info.count < ARRAY_ITEM_COUNT(info.rank_info.task_list); ++task_index)
	{
		if (rank_item.task_list[task_index].IsInvalid())
		{
			break;
		}
		info.rank_info.task_list[info.rank_info.count++] = rank_item.task_list[task_index];
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleKuaFuYingXiongLu::SendSingleTaskInfo(int rank_type, int seq, KuaFuYingXiongLuTaskInfo & task_info)
{
	Protocol::SCKuaFuYingXiongLuRoleSingleTaskInfo info;
	info.rank_type = rank_type;
	info.seq = seq;
	info.task_info = task_info;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleKuaFuYingXiongLu::SendRoleRankValue()
{
	Protocol::SCKuaFuYingXiongLuRoleRankValue info;
	info.count = 0;

	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.rank_info_list) && info.count < ARRAY_ITEM_COUNT(info.rank_value_list) &&
		i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++i)
	{
		info.rank_value_list[info.count++] = this->GetValueByRankType(i);
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleKuaFuYingXiongLu::OnReloadConfig()
{
	this->OnCheckAllTaskData();
}

void RoleKuaFuYingXiongLu::OnRelatedInfoChange(int task_type, int param1, int add_value)
{
	if (this->IsInvalidTaskType(task_type) || !KuaFuYingXiongLuManager::Instance().IsOpen())
	{
		return;
	}
	this->OnCheckInitActivity();
	if (this->IsAddTypeByTaskType(task_type) && add_value <= 0)
	{
		return;
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.rank_info_list) && i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++i)
	{
		const std::vector<KuaFuYingXiongLuTaskCfg> * task_vec = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTaskVec(m_param.activity_open_cross_day, i);
		if (NULL == task_vec || 0 != m_param.rank_info_list[i].is_fetch_rank_end_reward_flag)
		{
			continue;
		}
		//榜单还未开启也可以积攒进度
		for (int task_index = 0; task_index < ARRAY_ITEM_COUNT(m_param.rank_info_list[i].task_list) && task_index < (int)task_vec->size(); ++task_index)
		{
			KuaFuYingXiongLuTaskInfo & task_info = m_param.rank_info_list[i].task_list[task_index];
			if(task_info.task_type != task_type || 0 != task_info.is_fetch_flag)
			{
				continue;
			}
			if (this->OnTaskInfoChange(i, task_index,task_info, param1, (*task_vec)[task_index].param1, add_value))
			{
				this->SendSingleTaskInfo(i, task_index, task_info);
			}
		}
	}

}

bool RoleKuaFuYingXiongLu::IsAddTypeByTaskType(int task_type) const
{
	//任务划分二种类型,1:活动期间积攒次数 2:活动期间触发则根据当前任务关联模块,与之前记录取最高
	UNSTD_STATIC_CHECK(10 == KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX);
	bool is_add_type = false;
	switch (task_type)
	{
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_XUN_BAO:
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_LEARN_PET_SKILL:
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_PARTNER_STR:
		{
			is_add_type = true;
		}
		default:
		break;
	}

	return is_add_type;
}

int RoleKuaFuYingXiongLu::GetValueByRankType(int rank_type) const
{
	int rank_value = 0;
	switch (rank_type)
	{
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_ROLE_CAP:
		{
			rank_value = m_role_module_mgr->GetCapability()->GetRoleCapability();
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_EQUIP_CAP:
		{
			rank_value = m_role_module_mgr->GetEquipmentManager()->GetEquipJewelryCap();
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_PARTNER_CAP:
		{
			rank_value = m_role_module_mgr->GetPartner()->GetMaxFourPartnerCap();
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_PET_CAP:
		{
			rank_value = m_role_module_mgr->GetPet()->GetPetCapabilityMax();
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_COMPLEX_CAP:
		{
			rank_value = m_role_module_mgr->GetRole()->GetCapabilityWithFightPetAndPartner();
		}
		break;
		default:
		break;
	}

	return rank_value;
}

void RoleKuaFuYingXiongLu::OnCheckAllTaskData(bool is_init)
{
	bool is_change = false;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.rank_info_list) && i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++i)
	{
		const std::vector<KuaFuYingXiongLuTaskCfg> * task_vec = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTaskVec(m_param.activity_open_cross_day, i);
		if (NULL == task_vec || 0 != m_param.rank_info_list[i].is_fetch_rank_end_reward_flag)
		{
			continue;
		}

		for (int task_index = 0; task_index < ARRAY_ITEM_COUNT(m_param.rank_info_list[i].task_list) && task_index < (int)task_vec->size(); ++task_index)
		{
			KuaFuYingXiongLuTaskInfo & task_info = m_param.rank_info_list[i].task_list[task_index];
			if (!is_init && (0 != task_info.is_fetch_flag || task_info.task_type == (*task_vec)[task_index].task_type))
			{
				continue;
			}
			is_change = true;
			task_info.Reset();
			task_info.task_type = (*task_vec)[task_index].task_type;
			this->OnTaskInfoChange(i, task_index, task_info, 0, (*task_vec)[task_index].param1);
		}
	}
	if (is_change || is_init)
	{
		this->SendAllInfo();
	}
}

bool RoleKuaFuYingXiongLu::OnTaskInfoChange(int rank_type, int task_index, KuaFuYingXiongLuTaskInfo & task_info, int param1, int task_param1, int add_value)
{
	if (task_info.IsInvalid())
	{
		return false;
	}

	if (this->IsAddTypeByTaskType(task_info.task_type) && add_value <= 0)
	{
		return false;
	}

	int old_schedule = task_info.schedule;
	switch (task_info.task_type)
	{
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_MEDAL:
		{
			task_info.schedule = GetMax(m_role_module_mgr->GetMedal()->GetCurWearNumByGrade(task_param1), task_info.schedule);
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE:
		{
			task_info.schedule = GetMax(m_role_module_mgr->GetEquipmentManager()->GetEquipGemNumByGrade(task_param1), task_info.schedule);
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_EQUIP:
		{
			task_info.schedule = GetMax(m_role_module_mgr->GetEquipmentManager()->GetWearEquipNumByColor(task_param1), task_info.schedule);
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_PET_EQUIP:
		{
			task_info.schedule = GetMax(m_role_module_mgr->GetPet()->GetPetEquipmentByColor(task_param1), task_info.schedule);
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_XUN_BAO:
		{
			if (XUN_BAO_TREASURE_TYPE_INVALID == task_param1 || param1 == task_param1)
			{
				task_info.schedule += add_value;
			}
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_HAS_PET:
		{
			task_info.schedule = GetMax(m_role_module_mgr->GetPet()->GetPetNumByQuality(task_param1), task_info.schedule);
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_SOUL_EQUIP:
		{
			task_info.schedule = GetMax(m_role_module_mgr->GetPet()->GetPetSoulEquipmentByColor(task_param1), task_info.schedule);
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_LEARN_PET_SKILL:
		{
			if (GAME_PASSIVE_SKILL_PRIORITY_TYPE_INVALID == task_param1 || param1 == task_param1)
			{
				task_info.schedule += add_value;
			}
		}
		break;
		case KUA_FU_YING_XIONG_LU_TASK_TYPE_PARTNER_STR:
		{
			if (PQT_MIN == task_param1 || param1 == task_param1)
			{
				task_info.schedule += add_value;
			}
		}
		break;
	}

	if (old_schedule != task_info.schedule)
	{
		gamelog::g_log_kua_fu_ying_xiong_lu.printf(LL_INFO, "%s user[%d, %s] rank_type[%d] task[index:%d type:%d task_param1:%d param1:%d] add_value[%d] old[%d] new[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), rank_type, task_index, task_info.task_type, task_param1, param1, add_value, old_schedule, task_info.schedule);
		return true;
	}
	return false;
}

void RoleKuaFuYingXiongLu::OnCheckTime()
{
	if(m_param.activity_begin_timestamp <= 0) return;

	bool is_jump_time_jud = (m_param.activity_unique_id == KuaFuYingXiongLuManager::Instance().GetActivityUniqueID()) ? false : true;
	for (int rank_type = 0; rank_type < ARRAY_ITEM_COUNT(m_param.rank_info_list) && rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++rank_type)
	{
		if (0 != m_param.rank_info_list[rank_type].is_fetch_rank_end_reward_flag) continue;		//已经经过检测查询补发
		if(!is_jump_time_jud && KuaFuYingXiongLuManager::Instance().IsRankInTime(rank_type)) continue;

		//计算榜单开始时间,如果玩家最近参与时间不满足,则不给补发任务奖励)
		const KuaFuYingXiongLuTimeCfg * time_cfg = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTimeCfg(rank_type);
		if (NULL != time_cfg)
		{
			int open_day = World::GetInstWorld()->GetDayByTimestamp(m_param.activity_begin_timestamp, m_param.user_recently_participate_timestamp);
			if (open_day < time_cfg->start_time)
			{
				m_param.rank_info_list[rank_type].is_fetch_rank_end_reward_flag = 1;
				continue;
			}
		}

		bool is_send = false;
		std::vector<ItemConfigData> reward_list;
		m_param.rank_info_list[rank_type].is_fetch_rank_end_reward_flag = 1;
		const std::vector<KuaFuYingXiongLuTaskCfg> * task_vec = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTaskVec(m_param.activity_open_cross_day, rank_type);
		if (NULL == task_vec)
		{
			continue;
		}
		for (int task_index = 0; task_index < ARRAY_ITEM_COUNT(m_param.rank_info_list[rank_type].task_list) && task_index < (int)task_vec->size(); ++task_index)
		{
			KuaFuYingXiongLuTaskInfo & task_info = m_param.rank_info_list[rank_type].task_list[task_index];
			if (task_info.IsInvalid() || 0 != task_info.is_fetch_flag || task_info.schedule < (*task_vec)[task_index].param2)
			{
				continue;
			}

			task_info.is_fetch_flag = 1;
			is_send = true;
			reward_list.insert(reward_list.end(), (*task_vec)[task_index].reward_list.begin(), (*task_vec)[task_index].reward_list.end());
			gamelog::g_log_kua_fu_ying_xiong_lu.printf(LL_INFO, "%s user[%d, %s] rank_type[%d] task_index[%d]", __FUNCTION__, 
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), rank_type, task_index);
		}

		if (is_send)
		{
			this->SendSingleRankInfo(rank_type, m_param.rank_info_list[rank_type]);
		}
		//邮件文本有榜单名字,无法所有榜单汇总邮件
		if (!reward_list.empty())
		{
			m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_RANK_END_TASK, false, rank_type);
		}
	}
}
