#include "cloudarenauserachievement.hpp"
#include "config/logicconfigmanager.hpp"
#include "cloudarenaconfig.hpp"
#include "protocol/msgcloudarena.h"
#include "engineadapter.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "obj/character/role.h"
#include "cloudarenauser.hpp"
#include "gamelog.h"

CloudArenaUserAchievement::CloudArenaUserAchievement(CloudArenaUser* user) : m_user(user)
{

}

CloudArenaUserAchievement::~CloudArenaUserAchievement()
{

}

void CloudArenaUserAchievement::Init(const CloudArenaAchievementParam& param)
{
	m_param = param;

	if (!m_user->IsRobot())
	{
		this->OnInitCheckAchievementStatus();
	}
}

void CloudArenaUserAchievement::GetInitParam(ARG_OUT CloudArenaAchievementParam* out_param) const
{
	if (NULL != out_param) *out_param = m_param;
}

bool CloudArenaUserAchievement::OnInitCheckAchievementStatus()
{
	bool ret = false;
	for (int i = 0; i < CLOUD_ARENA_ACHIEVEMENT_MAX; ++i)
	{
		ret = ret || this->TryUpdateAchieveStatus(i);
	}	

	return ret;
}

bool CloudArenaUserAchievement::OnUpdateProgress(int achieve_type, int progress_num)
{
	if (achieve_type < 0 || achieve_type >= CLOUD_ARENA_ACHIEVEMENT_MAX) return false;

	bool has_change = false;
	if (this->IsSetProgress(achieve_type))
	{
		has_change = this->OnSetProgressNum(achieve_type, progress_num);
	}
	else
	{
		has_change = this->OnAddProgressNum(achieve_type, progress_num);
	}

	if (has_change)
	{
		this->SendAchieveUpdateNotice(achieve_type);
	}

	return has_change;
}

const CloudArenaAchievementItem* CloudArenaUserAchievement::GetAchievementItem(int achieve_type) const
{
	if (achieve_type < 0 || achieve_type >= CLOUD_ARENA_ACHIEVEMENT_MAX) return NULL;

	return &m_param.achieve_list[achieve_type];
}

void CloudArenaUserAchievement::OnFightFinish(bool is_challenge_other, bool is_win, bool is_higher_dan, int total_damage, int round_num, int use_medicine_num, int realive_num)
{
	if (is_challenge_other)
	{
		this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_CHALLENGE_TIMES, 1);
		if (is_win)
		{
			this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_WIN_TIMES, 1);
			if (round_num <= 5)
			{
				this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_WIN_ROUND_NO_LARGER_THAN_5, 1);
			}
		}

		this->OnChallengeOtherUpdateContinuousWin(is_win);
		this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_CONTINUOUS_WIN, m_param.continuous_challenge_win_times);
	
		if (is_higher_dan)
		{
			this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_WIN_HIGHER_RANK, 1);
		}
	}
	
	//this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_TOTAL_DAMAGE, total_damage); 不再统计单场总伤害

	if (use_medicine_num > 0)
	{
		this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_USE_MEDICINE, use_medicine_num);
	}

	if (realive_num > 0)
	{
		this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_REALIVE_TIMES, realive_num);
	}
}

void CloudArenaUserAchievement::OnRankUpdate(int cur_rank)
{
	this->OnUpdateProgress(CLOUD_ARENA_ACHIEVEMENT_RANK, cur_rank + 1); // 成就的排名是从1开始
}

void CloudArenaUserAchievement::OnFetchAchievementReward(int achieve_type)
{
	if (achieve_type < 0 || achieve_type >= CLOUD_ARENA_ACHIEVEMENT_MAX) return;

	m_param.achieve_list[achieve_type].reward_status = CLOUD_ARENA_ACHIEVE_STATUS_ALREADY_CLAIM;

	this->TryUpdateAchieveStatus(achieve_type);

	this->SendAchieveUpdateNotice(achieve_type);
}

void CloudArenaUserAchievement::OnNewSeasonStart()
{
	m_param.ResetOnNewSeasonStart();
}

bool CloudArenaUserAchievement::IsSetProgress(int achieve_type)
{
	UNSTD_STATIC_CHECK(9 == CLOUD_ARENA_ACHIEVEMENT_MAX);

	if (CLOUD_ARENA_ACHIEVEMENT_RANK == achieve_type ||
		CLOUD_ARENA_ACHIEVEMENT_TOTAL_DAMAGE == achieve_type ||
		CLOUD_ARENA_ACHIEVEMENT_CONTINUOUS_WIN == achieve_type)
	{
		return true;
	}

	return false;
}

bool CloudArenaUserAchievement::IsProgressNeedSmallerNum(int achieve_type)
{
	if (CLOUD_ARENA_ACHIEVEMENT_RANK == achieve_type)
	{
		return true;
	}

	return false;
}

bool CloudArenaUserAchievement::OnAddProgressNum(int achieve_type, int progress_num)
{
	m_param.achieve_list[achieve_type].process_num += progress_num;

	gamelog::g_log_cloud_arena.printf(LL_INFO, "UpdateProgress[ADD] user[%d,%s] achieve_type[%d] achieve_lv[%d] modify_progress[%d] cur_progress[%d]",
		m_user->GetUID(), m_user->GetName(), achieve_type, m_param.achieve_list[achieve_type].cur_achievement_lv, progress_num, m_param.achieve_list[achieve_type].process_num);

	this->TryUpdateAchieveStatus(achieve_type);
	return true;
}

bool CloudArenaUserAchievement::OnSetProgressNum(int achieve_type, int progress_num)
{
	if (0 == m_param.achieve_list[achieve_type].process_num || ProgressCompare(achieve_type, progress_num, m_param.achieve_list[achieve_type].process_num))
	{
		if (m_param.achieve_list[achieve_type].process_num > 0 &&
			m_param.achieve_list[achieve_type].process_num == progress_num)
		{
			// 进度没有变 
			return false;
		}

		m_param.achieve_list[achieve_type].process_num = progress_num;

		gamelog::g_log_cloud_arena.printf(LL_INFO, "UpdateProgress[SET] user[%d,%s] achieve_type[%d] achieve_lv[%d] modify_progress[%d] cur_progress[%d]",
			m_user->GetUID(), m_user->GetName(), achieve_type, m_param.achieve_list[achieve_type].cur_achievement_lv, progress_num, m_param.achieve_list[achieve_type].process_num);

		this->TryUpdateAchieveStatus(achieve_type);
		return true;
	}
	else
	{
		return false;
	}
}

bool CloudArenaUserAchievement::TryUpdateAchieveStatus(int achieve_type)
{
	if (CLOUD_ARENA_ACHIEVE_STATUS_NOT_FINISH == m_param.achieve_list[achieve_type].reward_status)
	{
		const CloudArenaAchievementCfg* cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetAchievementCfg(achieve_type, m_param.achieve_list[achieve_type].cur_achievement_lv);
		if (NULL == cfg) return false;
		if (0 == m_param.achieve_list[achieve_type].process_num) return false;

		if (this->ProgressCompare(achieve_type, m_param.achieve_list[achieve_type].process_num, cfg->progress_need))
		{
			m_param.achieve_list[achieve_type].reward_status = CLOUD_ARENA_ACHIEVE_STATUS_NOT_CLAIM;
			
			gamelog::g_log_cloud_arena.printf(LL_INFO, "TryUpdateAchieveStatus user[%d,%s] achieve_type[%d] cur_achieve_lv[%d] cur_progress[%d] cur_status[%d]",
				m_user->GetUID(), m_user->GetName(), achieve_type, m_param.achieve_list[achieve_type].cur_achievement_lv, m_param.achieve_list[achieve_type].process_num, m_param.achieve_list[achieve_type].reward_status);
		}
	}
	else if (CLOUD_ARENA_ACHIEVE_STATUS_ALREADY_CLAIM == m_param.achieve_list[achieve_type].reward_status)
	{
		const CloudArenaAchievementCfg* cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetAchievementCfg(achieve_type, m_param.achieve_list[achieve_type].cur_achievement_lv + 1);
		if (NULL == cfg) return false;

		m_param.achieve_list[achieve_type].cur_achievement_lv += 1;
		m_param.achieve_list[achieve_type].reward_status = CLOUD_ARENA_ACHIEVE_STATUS_NOT_FINISH;

		gamelog::g_log_cloud_arena.printf(LL_INFO, "UpdateAchieveStatus user[%d,%s] achieve_type[%d] cur_achieve_lv[%d] cur_progress[%d] cur_status[%d]",
			m_user->GetUID(), m_user->GetName(), achieve_type, m_param.achieve_list[achieve_type].cur_achievement_lv, m_param.achieve_list[achieve_type].process_num, m_param.achieve_list[achieve_type].reward_status);

		this->TryUpdateAchieveStatus(achieve_type);
	}

	return true;
}

bool CloudArenaUserAchievement::ProgressCompare(int achieve_type, int progress_num, int progress_to_compare)
{
	if (this->IsProgressNeedSmallerNum(achieve_type))
	{
		return progress_num <= progress_to_compare;
	}
	else
	{
		return progress_num >= progress_to_compare;
	}
}

void CloudArenaUserAchievement::OnChallengeOtherUpdateContinuousWin(bool is_win)
{
	if (is_win)
	{
		m_param.continuous_challenge_win_times += 1;
	}
	else
	{
		m_param.continuous_challenge_win_times = 0;
	}
}

void CloudArenaUserAchievement::SendAchieveUpdateNotice(int achieve_type)
{
	Protocol::SCCloudArenaAchievementUpdateNotice notice;
	notice.achieve_type = achieve_type;
	notice.cur_level = m_param.achieve_list[achieve_type].cur_achievement_lv;
	notice.progress_num = m_param.achieve_list[achieve_type].process_num;
	notice.reward_status = m_param.achieve_list[achieve_type].reward_status;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_user->GetUID()));
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&notice, sizeof(notice));
	}
}

