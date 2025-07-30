#ifndef __CLOUD_ARENA_USER_ACHIEVEMENT_HPP__
#define __CLOUD_ARENA_USER_ACHIEVEMENT_HPP__

#include "servercommon/cloudarenadef.hpp"

class CloudArenaUser;
class CloudArenaUserAchievement
{
public:
	CloudArenaUserAchievement(CloudArenaUser* user);
	~CloudArenaUserAchievement();

	void Init(const CloudArenaAchievementParam& param);
	void GetInitParam(ARG_OUT CloudArenaAchievementParam* out_param) const;
	bool OnInitCheckAchievementStatus();

	bool OnUpdateProgress(int achieve_type, int progress_num);
	const CloudArenaAchievementItem* GetAchievementItem(int achieve_type) const;

	inline int GetContinuousChallengeWin() const { return m_param.continuous_challenge_win_times; };
	void OnFightFinish(bool is_challenge_other, bool is_win, bool is_higher_dan, int total_damage, int round_num, int use_medicine_num, int realive_num);
	void OnRankUpdate(int cur_rank);
	void OnFetchAchievementReward(int achieve_type);
	void OnNewSeasonStart();
private:
	bool IsSetProgress(int achieve_type);
	bool IsProgressNeedSmallerNum(int achieve_type);

	bool OnAddProgressNum(int achieve_type, int progress_num);
	bool OnSetProgressNum(int achieve_type, int progress_num);
	bool TryUpdateAchieveStatus(int achieve_type);
	bool ProgressCompare(int achieve_type, int progress_num, int progress_to_compare);
	void OnChallengeOtherUpdateContinuousWin(bool is_win);
	void SendAchieveUpdateNotice(int achieve_type);

	CloudArenaUser* m_user;

	CloudArenaAchievementParam m_param;
};

#endif