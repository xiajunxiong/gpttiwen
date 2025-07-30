#include "fallengod.hpp"
#include "other/funopen/funopen.hpp"
#include "other/fallengod/fallengodconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "global/team/team.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "servercommon/noticenum.h"
#include "item/knapsack.h"
#include "global/rank/rankmanager.hpp"
#include "scene/scene.h"

FallenGod::FallenGod()
{
}

FallenGod::~FallenGod()
{
}

void FallenGod::Init(RoleModuleManager* module_mgr, const FallenGodParam &param, const FallenGodHelpParam &help_param)
{
	m_module_mgr = module_mgr;
	m_param = param;
	m_help_param = help_param;
}

void FallenGod::GetInitParam(FallenGodParam * param, FallenGodHelpParam *help_param)
{
	*param = m_param;
	*help_param = m_help_param;
}

void FallenGod::OnWeekChange()
{
	m_param.Reset();
	m_help_param.WeekReset();
	this->SendInfo();
}

void FallenGod::OnBattleFinish(BattleData_Business * business_data, int round_num)
{
	if (NULL == business_data)return;
	bool send_info = false;
	if (business_data->param2 < 0 || business_data->param2 >= MAX_FALLEN_GOD_MONSTER_NUM)return;

	const FallenGodBossCfg * base_cfg = LOGIC_CONFIG->GetFallenGodConfig()->GetBaseCfg(business_data->param2);
	if (base_cfg != NULL)
	{
		if (m_param.monster_list[business_data->param2].pass_round_num == 0)m_param.monster_list[business_data->param2].pass_round_num = round_num;
		if (m_param.top_sn < business_data->param2)
		{
			m_param.top_sn = business_data->param2;
			m_param.monster_list[business_data->param2].pass_round_num = round_num;
		}
		else if(m_param.top_sn == business_data->param2)
		{
			if(m_param.monster_list[business_data->param2].pass_round_num > round_num)m_param.monster_list[business_data->param2].pass_round_num = round_num;
		}

		//完整通关
		if (m_param.monster_list[business_data->param2].first_pass_flag == 0)
		{
			m_param.monster_list[business_data->param2].first_pass_flag = 1;
			if (m_param.monster_list[business_data->param2].first_reward_flag == 0)
			{
				const FallenGodBossCfg * base_cfg = LOGIC_CONFIG->GetFallenGodConfig()->GetBaseCfg(business_data->param2);
				if (base_cfg != NULL)
				{
					if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)base_cfg->reward_list.size(), &base_cfg->reward_list[0]))
					{
						m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
						return;
					}
					m_module_mgr->GetKnapsack()->PutList((short)base_cfg->reward_list.size(), &base_cfg->reward_list[0], PUT_REASON_FALLEN_GOD_REWARD);
					m_module_mgr->GetKnapsack()->NotifyGetItemList((short)base_cfg->reward_list.size(), &base_cfg->reward_list[0], GET_REASON_FALLEN_GOD_REWARD);
					m_param.monster_list[business_data->param2].first_reward_flag = 1;
					this->SendInfo();
				}
			}
			send_info = true;
		}

		RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_FALLEN_GOD);
	}

	if (send_info)this->SendInfo();
}

void FallenGod::StarBattle(int sn, bool is_client_req)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_FALLEN_GOD))return;
	//获取配置
	const FallenGodBossCfg * base_cfg = LOGIC_CONFIG->GetFallenGodConfig()->GetBaseCfg(sn);
	if (base_cfg == NULL)
	{
		return;
	}

	Team* m_team = m_module_mgr->GetRole()->GetMyTeam();
	if (m_team == NULL || !m_team->IsLeader(m_module_mgr->GetRole()))
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}

	for (int idx = 0; idx < m_team->GetMemberCount(); ++idx)
	{
		Role * member = m_team->GetMemberRoleByIndex(idx);
		if (NULL == member) continue;

		if (base_cfg->pass_type == FallenGodBossCfg::PASS_TYPE_0)
		{
			if (!member->GetRoleModuleManager()->GetBigDipper()->HasPassSN(base_cfg->pass_sn))
			{
				m_team->NoticeAllMember(errornum::EN_FALLEN_GOD_MEMBER_NOT_PASS_PRE_SN);
				return;
			}
		}
		else
		{
			if (!member->GetRoleModuleManager()->GetFallenGod()->IsPassSn(base_cfg->pass_sn))
			{
				m_team->NoticeAllMember(errornum::EN_FALLEN_GOD_MEMBER_NOT_PASS_PRE_SN);
				return;
			}
		}
	}

	//检查今天是否可以挑战
	const tm * m_tm = EngineAdapter::Instance().LocalTime();
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
		//m_module_mgr->NoticeNum(errornum::EN_ACTIVITY_BIG_DIPPER_NO_BOSS);
		return;
	}
	const FallenGodOtherCfg & other_cfg = LOGIC_CONFIG->GetFallenGodConfig()->GetOtherCfg();

	if (m_module_mgr->GetRole()->GetScene()->GetSceneID() != other_cfg.scene_id)return;

	if (m_team->GetMemberCount() < other_cfg.challenge_num)
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

			if (!member->GetRoleModuleManager()->GetFallenGod()->IsPassSn(sn))is_help = true;
		}

		if (is_help)
		{
			for (int i = 0; i < m_team->GetMemberCount(); ++i)
			{
				Role * member = m_team->GetMemberRoleByIndex(i);
				if (NULL == member) continue;

				if (member->GetRoleModuleManager()->GetFallenGod()->IsPassSn(sn) && member->GetRoleModuleManager()->GetFallenGod()->GetHelpPassCount(sn) < base_cfg->help_times)
				{
					member->GetRoleModuleManager()->NoticeNum(noticenum::NT_BIG_DIPPER_HELP_NOTIC);
				}
			}
		}

		m_team->SetEnterFbInfo(m_module_mgr->GetRole(), AGREE_BEFORE_ENTER_TYPE_BATTLE, BATTLE_MODE_FALLEN_GOD, base_cfg->monster_guoup, sn);
		return;
	}
}

void FallenGod::OnHelpPass(int boss_index, int waves)
{
	if (boss_index < 0 || boss_index >= MAX_FALLEN_GOD_MONSTER_NUM)return;

	// 先看自己有没有拿过本周胜利奖励，自己没有 则无事发生
	if (!this->IsPassSn(boss_index))return;	

	bool is_help = false;
	Team * team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL != team)
	{
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < team->GetMemberCount(); ++i)
		{
			Role * member = team->GetMemberRoleByIndex(i);
			if (NULL == member)
				continue;

			if (member->GetUID() == m_module_mgr->GetRole()->GetUID())
				continue;

			if (!member->GetRoleModuleManager()->GetFallenGod()->IsPassSn(boss_index))
			{
				is_help = true;
				break;
			}
		}
	}

	if (!is_help)
		return;

	const FallenGodBossCfg * base_cfg = LOGIC_CONFIG->GetFallenGodConfig()->GetBaseCfg(boss_index);
	if (NULL == base_cfg)
		return;

	if (m_help_param.help_list[boss_index] >= base_cfg->help_times)
		return;

	if((short)base_cfg->help_reward.size() > 0)
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)base_cfg->help_reward.size(), &base_cfg->help_reward[0]))
		{
			m_module_mgr->GetKnapsack()->SendMail(&base_cfg->help_reward[0], (short)base_cfg->help_reward.size(), SNED_MAIL_TYPE_DEFAULT);
		}
		else
		{
			m_module_mgr->GetKnapsack()->PutList((short)base_cfg->help_reward.size(), &base_cfg->help_reward[0], PUT_REASON_FALLEN_GOD_HELP_PASS_REWARD);
		}
	}

	m_help_param.help_list[boss_index] += 1;

	m_module_mgr->NoticeNum(noticenum::NT_FALLEN_GOD_HELP_SUCC);

	this->SendInfo();
}

int FallenGod::GetHelpPassCount(int sn)
{
	if (sn < 0 || sn >= MAX_FALLEN_GOD_MONSTER_NUM)return 0;

	return m_help_param.help_list[sn];
}

int FallenGod::GetBattleRound(int sn)
{
	if (sn < 0 || sn >= MAX_FALLEN_GOD_MONSTER_NUM)return 0;
	return m_param.monster_list[sn].pass_round_num;
}

bool FallenGod::IsPassSn(int sn)
{
	if (sn < 0 || sn >= MAX_FALLEN_GOD_MONSTER_NUM)return 0;
	return 0 != m_param.monster_list[sn].first_pass_flag;
}

void FallenGod::SendInfo()
{
	Protocol::SCFallenGodInfo protocol;
	for (int i = 0; i < MAX_FALLEN_GOD_MONSTER_NUM; i++)
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
