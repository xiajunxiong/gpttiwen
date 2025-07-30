#include "roleactivitypetloot.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitypetlootconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/funopen/funopen.hpp"

RoleActivityPetLoot::RoleActivityPetLoot(int type)
	:RoleActivity(type)
{

}

RoleActivityPetLoot::~RoleActivityPetLoot()
{

}

void RoleActivityPetLoot::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAPetLootParam));
}

void RoleActivityPetLoot::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAPetLootParam));
}

void RoleActivityPetLoot::Update(time_t now_second)
{
	//检查活动是否结束了
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_PET_LOOT))return;

	if (m_param.end_time != 0)
	{
		int act_real_open_day = GetDayIndex(m_param.end_time, now_second) + 1; // 活动开启时的开服天数，1~N

		if (act_real_open_day <= 0)
		{
			act_real_open_day = 1;
		}

		if (act_real_open_day <= 15)//超过15天的奖励一律不发
		{
			const RandActivityPetLootConfig * config = static_cast<const RandActivityPetLootConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PET_LOOT));
			if (config != NULL)
			{
				//发放没领取完的奖励
				//普通任务的
				{
					for (int i = 0; i < MAX_PET_LOOT_TASK_LIST_COUNT; i++)
					{
						PetLootTask &task_infp = m_param.task_info[i];
						if (task_infp.can_get_reward_times > 0)
						{
							const PetLootReward * reward_cfg = config->GetRewardCfg(m_param.open_day, i);
							if (reward_cfg != NULL)
							{
								//发邮件
								static MailContentParam contentparam; contentparam.Reset();
								const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->reward_item.item_id);

								if (item_base != NULL)
								{
									contentparam.item_list[0].item_id = reward_cfg->reward_item.item_id;
									contentparam.item_list[0].num = reward_cfg->reward_item.num * task_infp.can_get_reward_times;
									contentparam.item_list[0].is_bind = (reward_cfg->reward_item.is_bind) ? 1 : 0;
									contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
								}
								int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
								int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_end_content, RAND_ACTIVITY_TYPE_PET_LOOT);
								if (lensub > 0 && length > 0)
								{
									MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
								}
							}
						}
					}
				}

				//累计完成任务奖励
				{
					const int gift_num = config->GetTaskGiftSize();
					for (int i = 0; i < gift_num && i < static_cast<int>(8 * sizeof(int)); i++)
					{
						const TaskGiftConfigure * reward_cfg = config->GetTaskGiftCfg(i);
						if (reward_cfg != NULL)
						{
							if ((m_param.task_final_reward_flag & (1 << i)) > 0)continue;
							if (reward_cfg->task_num > m_param.task_finish_times)continue;
							std::vector<ItemConfigData> real_reward;
							real_reward.push_back(reward_cfg->reward_item);
							{
								//随机奖品
								const RamdGroup *rand_cfg = config->GetRandRewardCfg(reward_cfg->group_id);
								if (NULL != rand_cfg)
								{
									real_reward.insert(real_reward.end(), rand_cfg->reward_list.begin(), rand_cfg->reward_list.end());
								}
							}
							if (real_reward.size() > 0)
							{
								//放入背包失败，改发邮件
								static MailContentParam contentparam; contentparam.Reset();
								for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < (int)real_reward.size(); i++)
								{
									const ItemBase * item_base = ITEMPOOL->GetItem(real_reward[i].item_id);

									if (item_base != NULL)
									{
										contentparam.item_list[i].item_id = real_reward[i].item_id;
										contentparam.item_list[i].num = real_reward[i].num;
										contentparam.item_list[i].is_bind = (real_reward[i].is_bind) ? 1 : 0;
										contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
									}
								}
								int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
								int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_end_content, RAND_ACTIVITY_TYPE_PET_LOOT);
								if (lensub > 0 && length > 0)
								{
									MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
								}
							}
						}
					}
				}
			}
		}
		m_param.Reset();
		this->SendInfo();
	}
}

void RoleActivityPetLoot::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_PET_LOOT))
	{
		return;
	}
}

void RoleActivityPetLoot::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_PET_LOOT))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityPetLoot::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_PET_LOOT != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_time);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.end_time = ramgr->GetRandActivityEndTime(rand_act_type);
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_PET_LOOT);
		m_param.open_day = rand_open_day;
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityPetLoot::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_PET_LOOT))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_PET_LOOT);

	switch (raor->opera_type)
	{
	case 0://请求下发
		{
			this->SendInfo();
		}
		break;
	case 1://请求领取
		{
			if (raor->param_1 == 1)
			{
				this->GetTaskReward(raor->param_2);
			}
			else if(raor->param_1 == 2)
			{
				this->GetFinalTaskReward(raor->param_2);
			}
		}
		break;
	}
}

void RoleActivityPetLoot::OnPetIntensi(int pet_type, int str_level)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_PET_LOOT))return;
	const RandActivityPetLootConfig * config = static_cast<const RandActivityPetLootConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PET_LOOT));
	if (config == NULL)return;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_PET_LOOT))
	{
		return;
	}
	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_PET_LOOT);
	//检查所有任务条件
	bool has_change = false;
	for (int i = 0; i < MAX_PET_LOOT_TASK_LIST_COUNT; i++)
	{
		const PetLootReward * cfg = config->GetRewardCfg(rand_open_day, i);
		if (NULL != cfg)
		{
			bool allow_type = false;
			for (std::vector<int>::const_iterator it = cfg->pet_type.begin(); it != cfg->pet_type.end(); it++)
			{
				if (*it == pet_type)
				{
					allow_type = true;
				}
			}
			PetLootTask &node_task = m_param.task_info[i];
			if (allow_type)
			{
				switch (cfg->int_type)
				{
				case PetLootReward::INT_TYPE_0:
					{
						node_task.param += 1;
						//检查是否满足可领取
						if (cfg->reward_times > (node_task.has_get_reward_times + node_task.can_get_reward_times))
						{
							if (cfg->parameter <= node_task.param)
							{
								node_task.param -= cfg->parameter;
								node_task.can_get_reward_times += 1;
								m_param.task_finish_times += 1;//记录已完成任务次数
							}
						}
					}
					break;
				case PetLootReward::INT_TYPE_1:
					{
						node_task.param = str_level;
						//检查是否满足可领取
						if (cfg->reward_times > (node_task.has_get_reward_times + node_task.can_get_reward_times))
						{
							if (cfg->parameter <= node_task.param)
							{
								node_task.param = 0;
								node_task.can_get_reward_times += 1;
								m_param.task_finish_times += 1;//记录已完成任务次数
							}
						}
					}
					break;
				default:
					break;
				}
				has_change = true;
			}
		}
	}
	if (has_change)
	{
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityPetLoot::GetTaskReward(int seq)
{
	PetLootTask &task_info = m_param.task_info[seq];
	if (task_info.can_get_reward_times <= 0)return;
	
	const RandActivityPetLootConfig * config = static_cast<const RandActivityPetLootConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PET_LOOT));
	if (config == NULL)return;
	const PetLootReward *  reward_cfg = config->GetRewardCfg(m_param.open_day, seq);
	if (reward_cfg == NULL)return;
	
	//给予
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(reward_cfg->reward_item.item_id, reward_cfg->reward_item.num, false))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->reward_item.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = reward_cfg->reward_item.item_id;
			contentparam.item_list[0].num = reward_cfg->reward_item.num;
			contentparam.item_list[0].is_bind = (reward_cfg->reward_item.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_PET_LOOT);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(reward_cfg->reward_item, PUT_REASON_PET_LOOT))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		}
	}
	task_info.can_get_reward_times -= 1;
	task_info.has_get_reward_times += 1;
	this->SetRoleDataChange();
	this->SendInfo();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityPetLoot::GetFinalTaskReward(int seq)
{
	const RandActivityPetLootConfig * config = static_cast<const RandActivityPetLootConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PET_LOOT));
	if (config == NULL)return;
	const TaskGiftConfigure *  reward_cfg = config->GetTaskGiftCfg(seq);
	if (reward_cfg == NULL)return;
	if ((m_param.task_final_reward_flag & (1 << seq)) > 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_LOOT_REWARD_HAS_FATCH_ERR);//奖励已经领取过了
		return;
	}
	if (reward_cfg->task_num > m_param.task_finish_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PET_LOOT_TASK_NOT_ALLOW);//条件未满足无法领取
		return;
	}

	std::vector<ItemConfigData> real_reward;
	real_reward.push_back(reward_cfg->reward_item);
	{
		//随机奖品
		const RamdGroup *rand_cfg = config->GetRandRewardCfg(reward_cfg->group_id);
		if (NULL != rand_cfg)
		{
			real_reward.insert(real_reward.end(), rand_cfg->reward_list.begin(), rand_cfg->reward_list.end());
		}
	}
	{
		if (real_reward.size() > 0 && !m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)real_reward.size(), &real_reward[0]))
		{
			//放入背包失败，改发邮件
			static MailContentParam contentparam; contentparam.Reset();
			for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < (int)real_reward.size(); i++)
			{
				const ItemBase * item_base = ITEMPOOL->GetItem(real_reward[i].item_id);

				if (item_base != NULL)
				{
					contentparam.item_list[i].item_id = real_reward[i].item_id;
					contentparam.item_list[i].num = real_reward[i].num;
					contentparam.item_list[i].is_bind = (real_reward[i].is_bind) ? 1 : 0;
					contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
				}
			}
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_PET_LOOT);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
		else
		{
			//给予
			if (real_reward.size() > 0 && !m_role_module_mgr->GetKnapsack()->PutList((short)real_reward.size(), &real_reward[0], PUT_REASON_PET_LOOT))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}
		}
	}

	m_param.task_final_reward_flag |= (1 << seq);
	this->SendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityPetLoot::SendInfo()
{
	Protocol::SCRAPetLootInfo info;
	info.open_day = m_param.open_day;
	info.task_final_reward_flag = m_param.task_final_reward_flag;
	info.task_finish_times = m_param.task_finish_times;
	for (int i = 0; i < MAX_PET_LOOT_TASK_LIST_COUNT; i++)
	{
		info.task_info[i].param = m_param.task_info[i].param;
		info.task_info[i].has_get_reward_times = m_param.task_info[i].has_get_reward_times;
		info.task_info[i].can_get_reward_times = m_param.task_info[i].can_get_reward_times;
	}
	m_role_module_mgr->NetSend(&info, sizeof(info));
}
