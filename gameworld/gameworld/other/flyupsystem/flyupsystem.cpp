#include "flyupsystem.hpp"
#include "other/funopen/funopen.hpp"
#include "gameworld/gameworld/engineadapter.h"
#include "gameworld/config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "gamelog.h"
#include "protocol/msgother.h"
#include "other/funopen/funopenconfig.hpp"
#include "servercommon/noticenum.h"
#include "task/taskmanager.h"
#include "flyupststemconfig.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "skill/skillpool.hpp"
#include "other/title/title.hpp"
#include "other/event/eventhandler.hpp"
#include "task/taskrecorder.h"
#include "other/capability/capability.hpp"
#include "other/activesystem/activesystem.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"

FlyUpSystem::FlyUpSystem()
{
}

FlyUpSystem::~FlyUpSystem()
{
}

void FlyUpSystem::Init(RoleModuleManager * module_mgr, const FlyUpSystemParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void FlyUpSystem::GetInitParam(FlyUpSystemParam * param)
{
	*param = m_param;
}

const AttributeList & FlyUpSystem::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		AttributePlan tmp_ap;//潜能点
		tmp_ap.Reset();
		for (int element_idx = 0; element_idx < FlyUpSystemParam::FLY_UP_ELEMENT_MAX; element_idx++)
		{
			if (m_param.element[element_idx] <= 0)continue;
			const FlyUpShiLianCfg * shilian_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetShiLianCfg(FlyUpSystemParam::FLY_UP_STAG_1, m_module_mgr->GetRole()->GetAppearanceProf(), element_idx);
			if (NULL != shilian_cfg)
			{
				if (shilian_cfg->attr_type >= 0 && shilian_cfg->attr_type <= BATTLE_ATTR_ELEMENT_END)
				{
					m_attrs_add.m_attrs[shilian_cfg->attr_type] += shilian_cfg->attr_val * m_param.element[element_idx];
				}
				else if (shilian_cfg->attr_type >= BATTLE_ADD_POINT_TYPE_BEGIN && shilian_cfg->attr_type <= BATTLE_ADD_POINT_TYPE_END)
				{
					tmp_ap.add_point_list[shilian_cfg->attr_type - BATTLE_ADD_POINT_TYPE_BEGIN] += shilian_cfg->attr_val * m_param.element[element_idx];
				}
			}
		}

		for (int organ_idx = 0; organ_idx < FlyUpSystemParam::FLY_UP_ORGAN_MAX; organ_idx++)
		{
			if (m_param.organ[organ_idx] <= 0)continue;
			const FlyUpShiLianCfg * shilian_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetShiLianCfg(FlyUpSystemParam::FLY_UP_STAG_2, m_module_mgr->GetRole()->GetAppearanceProf(), organ_idx);
			if (NULL != shilian_cfg)
			{
				if (shilian_cfg->attr_type >= 0 && shilian_cfg->attr_type <= BATTLE_ATTR_ELEMENT_END)
				{
					m_attrs_add.m_attrs[shilian_cfg->attr_type] += shilian_cfg->attr_val * m_param.organ[organ_idx];
				}
				else if (shilian_cfg->attr_type >= BATTLE_ADD_POINT_TYPE_BEGIN && shilian_cfg->attr_type <= BATTLE_ADD_POINT_TYPE_END)
				{
					tmp_ap.add_point_list[shilian_cfg->attr_type - BATTLE_ADD_POINT_TYPE_BEGIN] += shilian_cfg->attr_val * m_param.organ[organ_idx];
				}
			}
		}
		
		AttributeList extra_attrs;
		LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, extra_attrs);
		m_attrs_add.Add(extra_attrs.m_attrs);

		//阶段属性
		for (int fly_flag = 0; fly_flag < m_param.fly_up_flag && fly_flag < FlyUpSystemParam::FLY_UP_STAG_MAX; fly_flag++)
		{
			const FlyUpAttrCfg * fly_flag_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetAttrCfg(fly_flag + 1);
			if (NULL != fly_flag_cfg)
			{
				m_attrs_add.m_attrs[BATTLE_ATTR_MAX_HP] += fly_flag_cfg->attr_value[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_0];
				m_attrs_add.m_attrs[BATTLE_ATTR_MAX_MP] += fly_flag_cfg->attr_value[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_1];
				m_attrs_add.m_attrs[BATTLE_ATTR_ATTACK] += fly_flag_cfg->attr_value[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_2];
				m_attrs_add.m_attrs[BATTLE_ATTR_DEFENSE] += fly_flag_cfg->attr_value[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_3];
				m_attrs_add.m_attrs[BATTLE_ATTR_AGILITY] += fly_flag_cfg->attr_value[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_4];
				m_attrs_add.m_attrs[BATTLE_ATTR_MENTAL] += fly_flag_cfg->attr_value[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_5];
				m_attrs_add.m_attrs[BATTLE_ATTR_RECOVERY] += fly_flag_cfg->attr_value[FlyUpAttrCfg::FLY_UP_ATTR_TYPE_6];

				//技能效果
				if (fly_flag_cfg->hp_buff > 0)
				{
					int role_level = m_module_mgr->GetRole()->GetLevel();
					const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(fly_flag_cfg->hp_buff);
					if (NULL != spe_cfg)
					{
						for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
						{
							const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
							if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

							const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
							if (NULL == detail_cfg) continue;

							if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								m_attrs_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								m_attrs_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
							}
						}
					}
				}
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

int FlyUpSystem::GetFlyUpSkillLevel()
{
	return m_param.fly_skill_level;
}

void FlyUpSystem::RoleUpLevel()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_FLY_UP))return;

	if (!m_module_mgr->GetTaskManager()->CheckTaskByType(TASK_TYPE_FLY_UP_TASK, TASK_CHECK_OPERA_TYPE_HAS_TASK, 0))//没有飞升任务才刷新
	{
		//this->RankFlyUpTask();
	}
}

void FlyUpSystem::OnDayChange()
{
	m_param.task_idx = 0;
	//放弃飞升任务,重新随机
	//检查是否已经接有飞升任务了，有就放弃刷新成现在的
	m_module_mgr->GetTaskManager()->CheckTaskByType(TASK_TYPE_FLY_UP_TASK, TASK_CHECK_OPERA_TYPE_CLEAR, 0);
	this->SendInfo();
}

void FlyUpSystem::OnRoleLogin()
{
	if (m_param.fly_up_flag >= FlyUpSystemParam::FLY_UP_STAG_MAX)m_param.fly_up_flag = FlyUpSystemParam::FLY_UP_STAG_2;//复原(把超越限制的飞升复原回去)
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	if (m_module_mgr->GetTaskRecorder()->HaveFinished(other_cfg.equipmentdh_task))return;
	if (m_param.fly_up_flag + 1 <= FlyUpSystemParam::FLY_UP_STAG_1)return;
	if (m_module_mgr->GetTaskManager()->CanAcceptTask(other_cfg.equipmentdh_task))m_module_mgr->GetTaskManager()->AcceptTask(other_cfg.equipmentdh_task);//接取点化指引任务
}

void FlyUpSystem::OnRoleLogout()
{
	this->SaveAdvance(true);
}

bool FlyUpSystem::CanAcceptFeiShengTask(int task_id)
{
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	if (other_cfg.check_feishengtask == task_id)
	{
		if (m_param.fly_up_flag + 1 != FlyUpSystemParam::FLY_UP_STAG_2)
		{
			return false;
		}
	}

	return true;
}

void FlyUpSystem::RankFlyUpTask(int pre_task_id)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_FLY_UP))return;
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	if (!m_module_mgr->GetTaskRecorder()->HaveFinished(other_cfg.feishengtasklist_id))return;
	const FlyUpTaskCfg * task_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->RandTask(m_param.fly_up_flag, m_param.is_shilian, pre_task_id);
	if (task_cfg == NULL)return;
	
	m_module_mgr->GetTaskManager()->FlyUpTask(task_cfg->task_id);
}

void FlyUpSystem::StartShiLian()
{
	if (m_param.fly_up_flag + 1 > FlyUpSystemParam::FLY_UP_STAG_2)return;
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	if (m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_1)
	{
		if (m_param.is_shilian == 1)return;
		if (other_cfg.feishengtasklist_id > 0 && !m_module_mgr->GetTaskRecorder()->HaveFinished(other_cfg.feishengtasklist_id))return;
		if (m_module_mgr->GetRole()->GetLevel() < other_cfg.level_limit1)
		{
			m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}
	}
	else if(m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_2)
	{
		if (m_param.is_shilian == 1)return;
		if (other_cfg.feishengtasklist_id2 > 0 && !m_module_mgr->GetTaskRecorder()->HaveFinished(other_cfg.feishengtasklist_id2))return;
		if (m_module_mgr->GetRole()->GetLevel() < other_cfg.level_limit2)
		{
			m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}
	}
	m_param.is_shilian = 1;
	//this->RankFlyUpTask();
	this->SendInfo();
}

void FlyUpSystem::OnTaskFinish(int task_type, int task_id)
{
	if (task_type == TASK_TYPE_FLY_UP_TASK)
	{
		std::vector<ItemConfigData> real_item_vec;

		const FlyUpTaskRewardCfg * task_reward_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetTaskRewardCfg(m_param.fly_up_flag, m_param.is_shilian, m_param.task_idx);
		if (NULL != task_reward_cfg)
		{
			const FlyUpTaskRewardGroupCfg * rand_reward_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->RandTaskRewardCfg(task_reward_cfg->reward_group);
			if (NULL != rand_reward_cfg)
			{
				real_item_vec.push_back(rand_reward_cfg->reward);
			}
		}

		m_param.task_idx += 1;

		//每日最后一个任务的额外奖励
		if (m_param.task_idx >= MAX_FLY_UP_TASK_IDX)
		{
			const FlyUpFinishRewardCfg * finish_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetTaskFinishRewardCfg(m_param.fly_up_flag, m_param.is_shilian);
			if (NULL != finish_cfg)
			{
				real_item_vec.insert(real_item_vec.end(), finish_cfg->reward_vec.begin(), finish_cfg->reward_vec.end());
			}
		}

		if ((int)real_item_vec.size() > 0 && !m_module_mgr->GetKnapsack()->PutList((short)real_item_vec.size(), &real_item_vec[0], PUT_REASON_RA_FLY_UP_REWARD))
		{
			m_module_mgr->GetKnapsack()->SendMail(&real_item_vec[0], (short)real_item_vec.size(), SNED_MAIL_TYPE_DEFAULT);
		}

		this->SendInfo();
		m_module_mgr->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_FLY_UP_TASK, 1, __FUNCTION__);
		this->RankFlyUpTask();
	}
}

void FlyUpSystem::OnUpAdvance()
{
	if (m_param.add_value != 0 || m_param.add_idx != 0)return;
	if (m_param.is_shilian != 1)return;
	int rank_value = 0;
	int * real_rand_idx = NULL;
	const FlyUpShiLianCfg* shilian_cfg = NULL;
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	Protocol::SCFlyUpSystemAdvInfo info;
	info.fly_up_flag = m_param.fly_up_flag;
	if (m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_1)//初阶
	{
		int max_value = other_cfg.max_exp1;
		int out_num = 0;
		int *element[FlyUpSystemParam::FLY_UP_ELEMENT_MAX] = { NULL };
		int idx[FlyUpSystemParam::FLY_UP_ELEMENT_MAX] = { 0 };
		m_param.GetElementNotFullList(max_value, &out_num, element, &idx[0]);
		//随机一个返回出去给rand_idx
		if (out_num <= 0)return;
		
		int rand_idx = rand() % out_num;
		shilian_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetShiLianCfg(m_param.fly_up_flag + 1, m_module_mgr->GetRole()->GetAppearanceProf(), idx[rand_idx]);
		
		if (NULL == shilian_cfg)return;
		
		if (shilian_cfg->attr_max - shilian_cfg->attr_min + 1 > 0)
		{
			rank_value = rand() % (shilian_cfg->attr_max - shilian_cfg->attr_min + 1) + shilian_cfg->attr_min;
		}
		else
		{
			rank_value = shilian_cfg->attr_max;
		}
		int add_idx = idx[rand_idx];
		if (add_idx < 0 || add_idx >= FlyUpSystemParam::FLY_UP_ELEMENT_MAX)return;
		real_rand_idx = element[rand_idx];
		info.fly_adv_idx = add_idx;
		if (real_rand_idx == NULL)return;
		if (*real_rand_idx + rank_value > max_value)
		{
			rank_value = max_value - *real_rand_idx;//不允许值超过最大值修改随机出来的值
		}
		m_param.add_idx = add_idx;
	}
	else if(m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_2)//高阶
	{
		int max_value = other_cfg.max_exp2;
		int out_num = 0;
		int *organ[FlyUpSystemParam::FLY_UP_ORGAN_MAX] = { NULL };
		int idx[FlyUpSystemParam::FLY_UP_ORGAN_MAX] = { 0 };
		m_param.GetOrganNotFullList(max_value, &out_num, organ, &idx[0]);
		//随机一个返回出去给rand_idx

		if (out_num <= 0)return;

		int rand_idx = rand() % out_num;
		shilian_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetShiLianCfg(m_param.fly_up_flag + 1, m_module_mgr->GetRole()->GetAppearanceProf(), idx[rand_idx]);

		if (NULL == shilian_cfg)return;
		
		if (shilian_cfg->attr_max - shilian_cfg->attr_min + 1 > 0)
		{
			rank_value = rand() % (shilian_cfg->attr_max - shilian_cfg->attr_min + 1) + shilian_cfg->attr_min;
		}
		else
		{
			rank_value = shilian_cfg->attr_max;
		}
		int add_idx = idx[rand_idx];
		if (add_idx < 0 || add_idx >= FlyUpSystemParam::FLY_UP_ORGAN_MAX)return;
		real_rand_idx = organ[rand_idx];
		info.fly_adv_idx = add_idx;
		if (real_rand_idx == NULL)return;
		if (*real_rand_idx + rank_value > max_value)
		{
			rank_value = max_value - *real_rand_idx;//不允许值超过最大值修改随机出来的值
		}
		m_param.add_idx = add_idx;
	}
	if (NULL == real_rand_idx)return;
	if (NULL == shilian_cfg)return;
	if (rank_value <= 0)return;

	//消耗
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(shilian_cfg->stuff_id, shilian_cfg->stuff_num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}
	m_param.add_value = rank_value;
	/**real_rand_idx += rank_value;*/
	info.value = rank_value;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FLY_UP_SYSTEM, __FUNCTION__);	
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));
}

void FlyUpSystem::OnFinishFly()
{
	if (m_param.fly_up_flag >= FlyUpSystemParam::FLY_UP_STAG_2)return;
	//按照阶段检测
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	int length = 0;
	if (m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_1)
	{
		for (int element_idx = 0; element_idx < FlyUpSystemParam::FLY_UP_ELEMENT_MAX; element_idx++)
		{
			if (!m_param.ElementAtleastValue(other_cfg.max_exp1))
			{
				return;
			}
		}

		if (other_cfg.level_limit1 > m_module_mgr->GetRole()->GetLevel())
		{
			m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}

		if (m_module_mgr->GetCapability()->GetRoleCapability() < other_cfg.score_limit1)
		{
			m_module_mgr->NoticeNum(errornum::EN_FLY_UP_ROLE_CAPBILITY_ERR);
			return;
		}

		//飞升1特有技能
		m_param.fly_skill_level = 1;
		//获得称号
		m_module_mgr->GetTaskManager()->AcceptTask(other_cfg.equipmentdh_task);//接取点化指引任务
		m_module_mgr->GetTitle()->AddTitle(other_cfg.feisheng_title1, __FUNCTION__);
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_flyup_succ_notice_1, m_module_mgr->GetUid(), m_module_mgr->GetRoleName());
	}
	else if (m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_2)
	{
		for (int organ_idx = 0; organ_idx < FlyUpSystemParam::FLY_UP_ORGAN_MAX; organ_idx++)
		{
			if (!m_param.OrganAtleastValue(other_cfg.max_exp2))
			{
				return;
			}
		}

		if (other_cfg.level_limit1 > m_module_mgr->GetRole()->GetLevel())
		{
			m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}

		if (m_module_mgr->GetCapability()->GetRoleCapability() < other_cfg.score_limit2)
		{
			m_module_mgr->NoticeNum(errornum::EN_FLY_UP_ROLE_CAPBILITY_ERR);
			return;
		}
		//获得称号
		m_module_mgr->GetTitle()->AddTitle(other_cfg.feisheng_title2, __FUNCTION__);
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_flyup_succ_notice_2, m_module_mgr->GetUid(), m_module_mgr->GetRoleName());
	}
	else
	{
		return;
	}
	
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE));
	}
	m_param.fly_up_flag += 1;
	m_param.is_shilian = 0;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FLY_UP_SYSTEM, __FUNCTION__);
	m_module_mgr->MakeAppearanceData();
	m_module_mgr->GetRole()->SendRoleInfo();
	m_module_mgr->GetRole()->SendChangeAppearanceNotice();
	EventHandler::Instance().OnRoleFlyUpEvent(m_module_mgr->GetRole(), m_param.fly_up_flag);
	this->SendInfo();
	gamelog::g_log_role_fly_system.printf(LL_INFO, "%s fly_flag[%d]  skill_level[%d] skill_exp[%d]", __FUNCTION__, m_param.fly_up_flag,
		m_param.fly_skill_level, m_param.skill_exp);
}

void FlyUpSystem::UplevelFlyUpSkill()
{
	const FlyUpSkillUpLevelCfg * next_skill_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetSkillUplevelCfg(m_param.fly_skill_level + 1);
	if (NULL != next_skill_cfg)
	{
		if (m_module_mgr->GetRole()->GetLevel() < next_skill_cfg->role_level)
		{
			m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}

		const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
		if (other_cfg.add_exp > 0)
		{
			if (!m_module_mgr->GetKnapsack()->ConsumeItem(next_skill_cfg->item_id, next_skill_cfg->consume, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_ENOUGH_COMMIT_ERROR);
				return;
			}

			m_param.skill_exp += other_cfg.add_exp;
			if(m_param.skill_exp >= next_skill_cfg->need_exp)
			{
				m_param.skill_exp -= next_skill_cfg->need_exp;
				m_param.fly_skill_level = next_skill_cfg->skill_level;
			}
		}
		
		this->SendSigleSkillInfo();
		gamelog::g_log_role_fly_system.printf(LL_INFO, "%s fly_flag[%d] skill_level[%d] skill_exp[%d]", __FUNCTION__, m_param.fly_up_flag,
			m_param.fly_skill_level, m_param.skill_exp);
	}
}

void FlyUpSystem::OnKeyUplevelFlyUpSkill()
{
	const FlyUpSkillUpLevelCfg * next_skill_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetSkillUplevelCfg(m_param.fly_skill_level + 1);
	if (NULL == next_skill_cfg)
	{
		return;
	}

	if (m_module_mgr->GetRole()->GetLevel() < next_skill_cfg->role_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	int item_count = m_module_mgr->GetKnapsack()->Count(next_skill_cfg->item_id);
	if (item_count <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_ITEM);
		return;
	}
	if (next_skill_cfg->consume <= 0 || other_cfg.add_exp <= 0)
	{
		return;
	}

	//当前已有道具所能增加经验的次数
	int add_exp_count = item_count / next_skill_cfg->consume;
	//当前升级所需经验数量
	int need_exp = next_skill_cfg->need_exp - m_param.skill_exp;
	//升级所需加经验次数
	int need_add_exp_count = need_exp / other_cfg.add_exp;
	if (0 != (need_exp % other_cfg.add_exp))
	{
		need_add_exp_count += 1;
	}

	//实际加经验次数
	int real_need_add_exp_count = GetMin(need_add_exp_count, add_exp_count);
	//实际所需消耗道具数量
	int need_consume_item_num = real_need_add_exp_count * next_skill_cfg->consume;
	if (need_consume_item_num <= 0)
	{
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItem(next_skill_cfg->item_id, need_consume_item_num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_ENOUGH_COMMIT_ERROR);
		return;
	}

	m_param.skill_exp += real_need_add_exp_count * other_cfg.add_exp;
	if (m_param.skill_exp >= next_skill_cfg->need_exp)
	{
		m_param.skill_exp -= next_skill_cfg->need_exp;
		m_param.fly_skill_level = next_skill_cfg->skill_level;
	}

	this->SendSigleSkillInfo();
	gamelog::g_log_role_fly_system.printf(LL_INFO, "%s fly_flag[%d] skill_level[%d] skill_exp[%d]", __FUNCTION__, m_param.fly_up_flag,
		m_param.fly_skill_level, m_param.skill_exp);
}

void FlyUpSystem::SaveAdvance(bool is_logout)
{
	if (m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_1)
	{
		m_param.element[m_param.add_idx] += m_param.add_value;
	}
	else if(m_param.fly_up_flag + 1 == FlyUpSystemParam::FLY_UP_STAG_2)
	{
		m_param.organ[m_param.add_idx] += m_param.add_value;
	}
	m_param.add_idx = 0;
	m_param.add_value = 0;
	if(!is_logout)m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FLY_UP_SYSTEM, __FUNCTION__);
}

int FlyUpSystem::IsAddExpFlyUpFlag()
{
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();

	int role_fly_limit_level = INT_MAX;
	if (m_param.fly_up_flag == FlyUpSystemParam::FLY_UP_STAG_NO)
	{
		role_fly_limit_level = other_cfg.level_limit1;
	}

	if (m_param.fly_up_flag == FlyUpSystemParam::FLY_UP_STAG_1)
	{
		role_fly_limit_level = other_cfg.level_limit2;
	}

	return role_fly_limit_level;
}

int FlyUpSystem::GetAttrPelletNum()
{
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	if (other_cfg.use_limit_up > 0 || m_param.fly_up_flag)
	{
		return other_cfg.use_limit_up * m_param.fly_up_flag;
	}
	return 0;
}

int FlyUpSystem::GetFlyUpFlag()
{
	return m_param.fly_up_flag;
}

int FlyUpSystem::GetTaskIdx()
{
	return m_param.task_idx;
}

void FlyUpSystem::SendInfo(bool is_one_click_finish)
{
	Protocol::SCFlyUpSystemInfo protocol;
	protocol.task_idx = m_param.task_idx;
	protocol.fly_skill_level = m_param.fly_skill_level;
	protocol.fly_up_flag = m_param.fly_up_flag;
	protocol.is_shilian = m_param.is_shilian;
	protocol.is_one_click_finish = is_one_click_finish ? 1 : 0;
	for (int idx = 0; idx < FlyUpSystemParam::FLY_UP_ELEMENT_MAX; idx++)
	{
		protocol.element[idx] = m_param.element[idx];
	}
	for (int idx = 0; idx < FlyUpSystemParam::FLY_UP_ORGAN_MAX; idx++)
	{
		protocol.organ[idx] = m_param.organ[idx];
	}
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}

void FlyUpSystem::SendSigleSkillInfo()
{
	Protocol::SCFlyUpSkillSigleInfo protocol;
	protocol.skill_level = m_param.fly_skill_level;
	protocol.skill_exp = m_param.skill_exp;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}

void FlyUpSystem::GmFlyUp(int fly_up_level)
{
	if (fly_up_level < FlyUpSystemParam::FLY_UP_STAG_NO || fly_up_level >= FlyUpSystemParam::FLY_UP_STAG_MAX) return;

	m_param.fly_up_flag = fly_up_level;
	this->SendInfo();
}

bool FlyUpSystem::IsCanOneClick()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_FLY_UP))return false;
	const FlyUpSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetOtherCfg();
	if (!m_module_mgr->GetTaskRecorder()->HaveFinished(other_cfg.feishengtasklist_id))return false;
	if (m_param.task_idx != 0)return false;
	if (m_module_mgr->GetActiveSystem()->IsStartedActive(ACTIVE_SYSTEM_TYPE_FLY_UP_TASK)) return false;
	return true;
}

void FlyUpSystem::OneClickFinishTask(unsigned int & exp, int & coin, int & jade, std::vector<ItemConfigData>& rewards)
{
	for (int i = m_param.task_idx; i < MAX_FLY_UP_TASK_IDX; i++)
	{
		const FlyUpTaskRewardCfg * task_reward_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetTaskRewardCfg(m_param.fly_up_flag, m_param.is_shilian, i);
		if (NULL != task_reward_cfg)
		{
			const FlyUpTaskRewardGroupCfg * rand_reward_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->RandTaskRewardCfg(task_reward_cfg->reward_group);
			if (NULL != rand_reward_cfg)
			{
				RoleJoinActivityReward::AddRewards(rewards, rand_reward_cfg->reward);
			}
		}

		m_param.task_idx += 1;

		//每日最后一个任务的额外奖励
		if (m_param.task_idx >= MAX_FLY_UP_TASK_IDX)
		{
			const FlyUpFinishRewardCfg * finish_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetTaskFinishRewardCfg(m_param.fly_up_flag, m_param.is_shilian);
			if (NULL != finish_cfg)
			{
				rewards.insert(rewards.end(), finish_cfg->reward_vec.begin(), finish_cfg->reward_vec.end());
				for (size_t i = 0; i < finish_cfg->reward_vec.size(); i++)
				{
					RoleJoinActivityReward::AddRewards(rewards, finish_cfg->reward_vec[i]);
				}
			}
		}
	}

	m_module_mgr->GetTaskManager()->CheckTaskByType(TASK_TYPE_FLY_UP_TASK, TASK_CHECK_OPERA_TYPE_CLEAR, 0);

	m_module_mgr->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_FLY_UP_TASK, MAX_FLY_UP_TASK_IDX, __FUNCTION__);
	this->SendInfo(true);
}
