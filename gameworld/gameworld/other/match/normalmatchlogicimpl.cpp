#include "normalmatchlogicimpl.hpp"

NormalMatchLogicImpl::NormalMatchLogicImpl()
{

}

bool NormalMatchLogicImpl::IsTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB)
{
	return true;
}

bool NormalMatchLogicImpl::IsTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	return true;
}

void NormalMatchLogicImpl::RecalcMatchTeamLogic(MatchTeam* mt)
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


