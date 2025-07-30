#include "activityteamfightmatchlogicimpl.hpp"
#include "engineadapter.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"

ActivityTeamFightMatchLogicImpl::ActivityTeamFightMatchLogicImpl()
{

}

bool ActivityTeamFightMatchLogicImpl::IsTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB)
{
	int level_gap = mtA->team_info.mtlb.average_level - mtB->team_info.mtlb.average_level;
	if (level_gap < 0) level_gap = -level_gap;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	unsigned int wait_time = now - mtA->start_match_timestamp;
	int allow_level_gap = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetTeamMateLevelDiffByWaitSeconds(wait_time);

	return level_gap <= allow_level_gap;
}

bool ActivityTeamFightMatchLogicImpl::IsTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	int level_gap = mtA->team_info.mtlb.average_level - mtB->team_info.mtlb.average_level;
	if (level_gap < 0) level_gap = -level_gap;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	unsigned int wait_time = now - mtA->start_match_timestamp;
	int allow_level_gap = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetTeamMateLevelDiffByWaitSeconds(wait_time);

	return level_gap <= allow_level_gap;
}

void ActivityTeamFightMatchLogicImpl::RecalcMatchTeamLogic(MatchTeam* mt)
{
	if (NULL == mt) return;

	int total_level = 0;
	long long total_capability = 0;
	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		total_level += mt->user_list[i]->user_info.mulb.level;
		total_capability += mt->user_list[i]->user_info.mulb.capability;
	}
	int user_num = mt->user_list.size();
	int average_level = 0;
	long long average_capability = 0;
	if (user_num > 0)
	{
		average_level = total_level / user_num;
		average_capability = total_capability / user_num;
	}
	mt->team_info.mtlb.average_capability = average_capability;
	mt->team_info.mtlb.average_level = average_level;
	mt->team_info.mtlb.total_capability = total_capability;
	mt->team_info.mtlb.total_level = total_level;
}


