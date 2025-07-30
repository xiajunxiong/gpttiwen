#include "bigdipper.hpp"
#include "other/funopen/funopen.hpp"
#include "gameworld/gameworld/engineadapter.h"
#include "gameworld/config/logicconfigmanager.hpp"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "gamelog.h"
#include "global/activity/activitymanager.hpp"
#include "config/activityconfig/activitybigdipper/activitybigdipperconfig.hpp"
#include "protocol/msgother.h"
#include "global/rank/rankmanager.hpp"
#include "other/bigdipper/bigdippermanager.hpp"
#include "global/team/team.hpp"
#include "global/team/teamconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "servercommon/noticenum.h"

BigDipper::BigDipper()
{
}

BigDipper::~BigDipper()
{
}

void BigDipper::Init(RoleModuleManager * module_mgr, const BigDipperParam & param, const BigDipperHelpParam & help_param)
{
	m_module_mgr = module_mgr;
	m_param = param;
	m_help_param = help_param;
}

void BigDipper::GetInitParam(BigDipperParam * param, BigDipperHelpParam * help_param)
{
	*param = m_param;
	*help_param = m_help_param;
}

void BigDipper::OnWeekChange()
{
	m_param.WeekReset();
	m_help_param.WeekReset();
}

void BigDipper::OnBattleFinish(BattleData_Business * business_data, int round_num)
{
	if (NULL == business_data)return;
	if (business_data->deamon_cave_beat_waves > 0)
	{
		bool send_info = false;
		if (business_data->param2 < 0 || business_data->param2 >= MAX_BIG_DIPPER_MONSTER_NUM)return;
		for (int i = 0; i < business_data->deamon_cave_beat_waves && i < MAX_BIG_DIPPER_MONSTER_REWARD_INDEX; i++)
		{
			if ((m_param.monster_list[business_data->param2].pass_flag & (1 << i)) == 0)
			{
				m_param.monster_list[business_data->param2].pass_flag |= (1 << i);
				this->GetReward(business_data->param2, i);
				send_info = true;
			}
		}

		const BigDipperCfg * base_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(business_data->param2);
		if (base_cfg != NULL)
		{
			m_param.monster_list[business_data->param2].pass_round_num = round_num;
			if (base_cfg->type == BigDipperCfg::MONSTER_TYPE_1 && business_data->deamon_cave_beat_waves < MAX_BIG_DIPPER_MONSTER_REWARD_INDEX)return;
			//完整通关
			if (m_param.monster_list[business_data->param2].first_pass_flag == 0)
			{
				m_param.monster_list[business_data->param2].first_pass_flag = 1;
				send_info = true;
			}
			EventHandler::Instance().OnPassBigDipper(m_module_mgr->GetRole(), business_data->param2);
			RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_BIG_DIPPER_1 + business_data->param2);
		}

		if (send_info)this->SendInfo();
		m_module_mgr->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_BIG_DIPPER, 1, __FUNCTION__);

		m_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_BIG_DIPPER);
	}
}

int BigDipper::GetBattleRound(int sn)
{
	if (sn < 0 || sn >= MAX_BIG_DIPPER_MONSTER_NUM)return 0;
	return m_param.monster_list[sn].pass_round_num;
}

void BigDipper::StarBattle(int sn, bool is_client_req)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_BIG_DIPPER))return;
	if (sn < 0 || sn >= MAX_BIG_DIPPER_MONSTER_NUM)return;
	//获取配置
	const BigDipperCfg * base_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(sn);
	if (base_cfg == NULL)
	{
		m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_NO_DATA);
		return;
	}
	Team* m_team = m_module_mgr->GetRole()->GetMyTeam();
	if (m_team == NULL || !m_team->IsLeader(m_module_mgr->GetRole()))
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}
	
	if (!BigDipperManager::Instance().CanBattleMonsterBySn(sn))
	{
		if (base_cfg->type == BigDipperCfg::MONSTER_TYPE_0)m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_NO_MONSTER);
		else if (base_cfg->type == BigDipperCfg::MONSTER_TYPE_1)m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_NO_BOSS);
		return;
	}
	//检查今天是否可以挑战
	const tm * m_tm = EngineAdapter::Instance().LocalTime();
	if (base_cfg->type == BigDipperCfg::MONSTER_TYPE_1)
	{
		bool on_challeng_day = false;
		for (std::vector<short>::const_iterator it = base_cfg->open_time_list.begin(); it != base_cfg->open_time_list.end(); it++)
		{
			if (*it == m_tm->tm_wday)
			{
				on_challeng_day = true;
				break;
			}
		}
		if (!on_challeng_day)
		{
			m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_NO_BOSS);
			return;
		}
	}

	if (m_team->GetMemberCount() < 3)
	{
		m_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
		return;
	}

	if (is_client_req)
	{
		bool is_help = false;
		for (int idx = 0; idx < m_team->GetMemberCount(); ++idx)
		{
			Role * member = m_team->GetMemberRoleByIndex(idx);
			if (NULL == member) continue;

			const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_BIG_DIPPER);
			if (NULL == cfg) return;

			if (member->GetLevel() < cfg->limit_level)
			{
				m_team->NoticeMemberLevelNotEnough(member->GetName(), cfg->limit_level);
				return;
			}

			if (!member->GetRoleModuleManager()->GetBigDipper()->IsWeekPass(sn))
				is_help = true;
		}

		if (is_help)
		{
			for (int i = 0; i < m_team->GetMemberCount(); ++i)
			{
				Role * member = m_team->GetMemberRoleByIndex(i);
				if (NULL == member) continue;

				if (member->GetRoleModuleManager()->GetBigDipper()->IsWeekPass(sn) && member->GetRoleModuleManager()->GetBigDipper()->GetHelpPassCount(sn) < base_cfg->help_times)
				{
					member->GetRoleModuleManager()->NoticeNum(noticenum::NT_BIG_DIPPER_HELP_NOTIC);
				}
			}
		}

		m_team->SetEnterFbInfo(m_module_mgr->GetRole(), AGREE_BEFORE_ENTER_TYPE_BATTLE, BATTLE_MODE_BIG_DIPPER, base_cfg->monster_guoup, sn);
		return;
	}
}

void BigDipper::GetFirstPassReward(int sn)
{
	if (sn < 0 || sn >= MAX_BIG_DIPPER_MONSTER_NUM)return;
	if (m_param.monster_list[sn].first_pass_flag == 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_NO_PASS);
		return;
	}

	if (m_param.monster_list[sn].first_reward_flag != 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_HAS_REWARD);
		return;
	}

	const BigDipperCfg * base_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(sn);
	if (base_cfg != NULL && (int)base_cfg->first_reward.size() > 0)
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)base_cfg->first_reward.size(), &base_cfg->first_reward[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}
		m_module_mgr->GetKnapsack()->PutList(base_cfg->first_reward.size(), &base_cfg->first_reward[0], PUT_REASON_BIG_DIPPER_REWARD);

		m_param.monster_list[sn].first_reward_flag = 1;
		this->SendInfo();
	}
}

void BigDipper::GetReward(int sn, int reward_idx)
{
	if (sn < 0 || sn >= MAX_BIG_DIPPER_MONSTER_NUM)return;
	if (reward_idx < 0 || reward_idx >= MAX_BIG_DIPPER_MONSTER_REWARD_INDEX)return;
	if ((m_param.monster_list[sn].pass_flag & (1 << reward_idx)) == 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_NO_PASS_LEVEL);
		return;
	}

	if ((m_param.monster_list[sn].pass_reward_flag & (1 << reward_idx)) > 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_HAS_REWARD);
		return;
	}

	const BigDipperCfg * base_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(sn);
	if (base_cfg != NULL && (short)base_cfg->reward_list[reward_idx].size() > 0)
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)base_cfg->reward_list[reward_idx].size(), &base_cfg->reward_list[reward_idx][0]))
		{
			m_module_mgr->GetKnapsack()->SendMail(&base_cfg->reward_list[reward_idx][0], (short)base_cfg->reward_list[reward_idx].size(), SNED_MAIL_TYPE_DEFAULT);
		}
		else
		{
			m_module_mgr->GetKnapsack()->PutList((short)base_cfg->reward_list[reward_idx].size(), &base_cfg->reward_list[reward_idx][0], PUT_REASON_BIG_DIPPER_REWARD);
		}

		m_param.monster_list[sn].pass_reward_flag |= 1 << reward_idx;
		this->SendInfo();
	}
}

void BigDipper::SendInfo()
{
	Protocol::SCBigDipperMyInfo protocol;
	for (int i = 0; i < MAX_BIG_DIPPER_MONSTER_NUM; i++)
	{
		protocol.monster_list[i].first_pass_flag = m_param.monster_list[i].first_pass_flag;
		protocol.monster_list[i].first_reward_flag = m_param.monster_list[i].first_reward_flag;
		protocol.monster_list[i].pass_flag = m_param.monster_list[i].pass_flag;
		protocol.monster_list[i].pass_reward_flag = m_param.monster_list[i].pass_reward_flag;
		protocol.monster_list[i].pass_round_num = m_param.monster_list[i].pass_round_num;
		protocol.help_list[i] = m_help_param.help_list[i];
	}
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}

bool BigDipper::HasPassSN(int sn)
{
	if (sn < 0 || sn >= MAX_BIG_DIPPER_MONSTER_NUM)return false;
	const BigDipperCfg * base_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(sn);
	if (base_cfg != NULL)
	{
		for (int i = 0; i < base_cfg->monster_stage_reward_num && i < MAX_BIG_DIPPER_MONSTER_REWARD_INDEX; i++)
		{
			if ((m_param.monster_list[sn].pass_flag & (1 << i)) == 0)
			{
				return false;
			}
		}
	}
	return true;
}

void BigDipper::OnHelpPass(int boss_index, int waves)
{
	if (boss_index < 0 || boss_index >= MAX_BIG_DIPPER_MONSTER_NUM)return;

	// 先看自己有没有拿过本周胜利奖励，自己没有 则无事发生
	if (!this->IsWeekPass(boss_index))
		return;

	bool is_help = false;
	Team * team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL != team)
	{
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < team->GetMemberCount(); ++i)
		{
			Role * member = team->GetMemberRoleByIndex(i);
			if (NULL == member)
				continue;

			if(member->GetUID() == m_module_mgr->GetRole()->GetUID())
				continue;

			if (!member->GetRoleModuleManager()->GetBigDipper()->IsWeekPass(boss_index))
			{
				is_help = true;
				break;
			}
		}
	}

	if (!is_help)
		return;

	const BigDipperCfg * base_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(boss_index);
	if (NULL == base_cfg)
		return;

	if (m_help_param.help_list[boss_index] >= base_cfg->help_times)
		return;

	for (int i = 0; i < waves && m_help_param.help_list[boss_index] < base_cfg->help_times; i++)
	{
		if ((short)base_cfg->help_reward.size() <= 0)continue;
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)base_cfg->help_reward.size(), &base_cfg->help_reward[0]))
		{
			m_module_mgr->GetKnapsack()->SendMail(&base_cfg->help_reward[0], (short)base_cfg->help_reward.size(), SNED_MAIL_TYPE_DEFAULT);
		}
		else
		{
			m_module_mgr->GetKnapsack()->PutList((short)base_cfg->help_reward.size(), &base_cfg->help_reward[0], PUT_REASON_BIG_DIPPER_REWARD);
		}

		m_help_param.help_list[boss_index] += 1;
	}

	m_module_mgr->NoticeNum(noticenum::NT_BIG_DIPPER_HELP_SUCC);

	this->SendInfo();
}

bool BigDipper::IsFirstPass(int boss_index)
{
	if (boss_index < 0 || boss_index >= MAX_BIG_DIPPER_MONSTER_NUM)
		return false;

	return 0 != m_param.monster_list[boss_index].first_pass_flag;
}

bool BigDipper::IsWeekPass(int boss_index)
{
	if (boss_index < 0 || boss_index >= MAX_BIG_DIPPER_MONSTER_NUM)
		return false;

	return 0 != m_param.monster_list[boss_index].pass_flag;
}

int BigDipper::GetHelpPassCount(int boss_index)
{
	if (boss_index < 0 || boss_index >= MAX_BIG_DIPPER_MONSTER_NUM)
	{
		return 0;
	}

	return m_help_param.help_list[boss_index];
}
