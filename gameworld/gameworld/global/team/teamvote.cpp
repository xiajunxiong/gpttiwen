#include "teamvote.hpp"
#include "servercommon/teamdef.hpp"
#include "team.hpp"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "protocol/msgteam.h"
#include "teammanager.hpp"

TeamVote::TeamVote() : m_team(NULL), m_vote_end_timestamp(0u), m_campaigner_uid(0), 
	m_yes_num(0), m_no_num(0)
{

}

TeamVote::~TeamVote()
{

}

void TeamVote::Update(unsigned long interval, time_t now_second)
{
	if (!this->IsVoting()) return;

	if (now_second > m_vote_end_timestamp)
	{
		this->EndVote(false);
	}
}

void TeamVote::StartVote(Role* campaigner)
{
	if (NULL == campaigner) return;

	m_vote_map.clear();
	m_campaigner_uid = campaigner->GetUID();
	m_vote_end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 20;
	m_yes_num = 0;
	m_no_num = 0;

	static Protocol::SCTeamVoteStart msg;
	F_STRNCPY(msg.campaigner_name, campaigner->GetName(), sizeof(GameName));
	msg.vote_end_timestamp = m_vote_end_timestamp;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember* member_info = m_team->GetMemberInfo(i);
		if (NULL == member_info || !member_info->is_follow || !member_info->is_online || 
			member_info->uid == campaigner->GetUID())
		{
			continue;
		}

		Role* member_role = m_team->GetMemberRoleByIndex(i);
		if (NULL == member_role)
		{
			continue;
		}

		EngineAdapter::Instance().NetSend(member_role->GetNetId(), (const char*)&msg, sizeof(msg));

		m_vote_map.insert(VoteMap::value_type(member_info->uid, TEAM_VOTE_INVALID));
	}
}

void TeamVote::EndVote(bool result)
{
	if (result)
	{
		do 
		{
			Role* campaigner = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_campaigner_uid));
			if (NULL == campaigner)
			{
				result = false; // 发起者不在线， 投票作废
				break;
			}
		
			bool is_found = false;
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				TeamMember* member = m_team->GetMemberInfo(i);
				if (NULL == member || member->uid != campaigner->GetUID())
				{
					continue;
				}

				Role* leader_role = m_team->GetLeaderRole();
				if (NULL != leader_role)
				{
					TeamManager::Instance().OnChangeLeader(leader_role, i);		
				}
				else
				{
					result = false;
				}
				
				is_found = true;
				break;
			}

			if (!is_found)
			{
				result = false; // 发起者在队伍里找不到，投票作废
				break;
			}

		} while (0);		
	}
	
	if (!result)
	{
		Role* campaigner = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_campaigner_uid));
		if (NULL != campaigner)
		{
			campaigner->NoticeNum(errornum::EN_TEAM_VOTE_NO);
		}
	}
	
	m_vote_end_timestamp = 0u;
	m_campaigner_uid = 0;
	m_yes_num = 0;
	m_no_num = 0;
}

void TeamVote::OnRoleVote(Role* role, bool is_yes)
{
	VoteMap::iterator it = m_vote_map.find(role->GetUID());
	if (it == m_vote_map.end())
	{
		role->NoticeNum(errornum::EN_TEAM_VOTE_HAS_NO_VOTE);
	}
	else if (it->second != TEAM_VOTE_INVALID)
	{
		role->NoticeNum(errornum::EN_TEAM_VOTE_ALREADY_VOTE);
	}
	else
	{
		if (is_yes)
		{
			it->second = TEAM_VOTE_YES;
			m_yes_num += 1;
			if (m_yes_num > (int)m_vote_map.size() / 2)
			{
				this->EndVote(true);
			}
		}
		else
		{
			it->second = TEAM_VOTE_NO;
			m_no_num += 1;
			if (m_no_num > (int)m_vote_map.size() / 2)
			{
				this->EndVote(false);
			}
		}
	}
}

