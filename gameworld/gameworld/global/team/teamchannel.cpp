#include "teamchannel.hpp"
#include "global/team/team.hpp"
#include "protocol/msgteam.h"
#include "global/topicmanager/topicmanager.hpp"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "world.h"
#include "servercommon/string/gameworldstr.h"

TeamChannel::TeamChannel()
{

}

TeamChannel::~TeamChannel()
{

}

void TeamChannel::OnTeamCreate(int team_index, bool is_cross, unsigned int create_time, const TeamItem& team_item)
{
	// �´����Ķ��飬ֻ���ڼ���״̬�£��Ż���ֲ����б����˲��Ҫ���б���ɾȥ

	TeamOrder to = GetTeamOrder(team_index, is_cross, create_time);
	if (m_team_map.find(to) != m_team_map.end()) return;

	m_team_map.insert(TeamItemMap::value_type(to, team_item));
	this->SendTeamInfoChange(team_item, Protocol::SCTeamChannelInfo::REASON_TYPE_ADD_TEAM);
	
	if (m_team_map.size() > MAX_TEAM_CHANNEL_ITEM_NUM)
	{
		TeamItemMap::reverse_iterator rit = m_team_map.rbegin();

		this->SendTeamInfoChange(rit->second, Protocol::SCTeamChannelInfo::REASON_TYPE_REMOVE_TEAM);
		m_team_map.erase(rit->first);	
	}
}

void TeamChannel::OnTeamCreate(int team_index, bool is_cross, unsigned int create_time, Team* team)
{
	if (NULL == team) return;

	static TeamItem ti;
	team->GetTeamItemInfo(&ti);

	this->OnTeamCreate(team_index, is_cross, create_time, ti);
}

void TeamChannel::OnTeamInfoChange(int team_index, bool is_cross, unsigned int create_time, const TeamItem& team_item, int team_check_type)
{
	TeamOrder to = GetTeamOrder(team_index, is_cross, create_time);

	if (team_item.cur_member_num >= MAX_TEAM_MEMBER_NUM || TEAM_CHECK_TYPE_REFUSE_ALL == team_check_type)
	{
		//�����˾ʹ��б���ȥ��
		TeamItemMap::iterator it = m_team_map.find(to);
		if (it == m_team_map.end())
		{
			return;
		}
		this->SendTeamInfoChange(it->second, Protocol::SCTeamChannelInfo::REASON_TYPE_REMOVE_TEAM);
		m_team_map.erase(it);
		return;
	}
	else
	{
		TeamItemMap::iterator it = m_team_map.find(to);
		if (it == m_team_map.end())
		{
			//û����team_map�У�һ����������˺�ɾ���ˣ��������¼ӽ���
			m_team_map.insert(TeamItemMap::value_type(to, team_item));

			this->SendTeamInfoChange(team_item, Protocol::SCTeamChannelInfo::REASON_TYPE_ADD_TEAM);
		}
		else
		{
			it->second = team_item;
			this->SendTeamInfoChange(team_item, Protocol::SCTeamChannelInfo::REASON_TYPE_INFO_CHANGE);
		}
	}
}

void TeamChannel::OnTeamInfoChange(Team* team)
{
	if (NULL == team) return;

	static TeamItem ti;
	team->GetTeamItemInfo(&ti);

	this->OnTeamInfoChange(team->GetTeamIndex(), team->GetIsCross(), team->GetTeamCreateTime(), ti, team->GetCheckType());
}

void TeamChannel::OnTeamDismiss(int team_index, bool is_cross, unsigned int create_time, const TeamItem& team_item)
{
	TeamOrder to = GetTeamOrder(team_index, is_cross, create_time);

	TeamItemMap::iterator it = m_team_map.find(to);
	if (it == m_team_map.end()) return;

	this->SendTeamInfoChange(it->second, Protocol::SCTeamChannelInfo::REASON_TYPE_REMOVE_TEAM);
	m_team_map.erase(it);
}

void TeamChannel::OnTeamDismiss(int team_index, bool is_cross, unsigned int create_time, Team* team)
{
	if (NULL == team) return;

	static TeamItem ti;
	team->GetTeamItemInfo(&ti);

	this->OnTeamDismiss(team_index, is_cross, create_time, ti);
}

void TeamChannel::OnTeamEnterSpecialScene(Team* team)
{
	static TeamItem ti;
	team->GetTeamItemInfo(&ti);

	// �൱��Dismiss�� �ڽ����ϳ���
	this->OnTeamDismiss(ti.team_index, (0 != ti.is_cross), ti.create_team_timestamp, ti);
}

void TeamChannel::OnTeamEnterNormalScene(Team* team)
{
	static TeamItem ti;
	team->GetTeamItemInfo(&ti);

	// �൱��Create�� �ڽ���������
	this->OnTeamCreate(ti.team_index, (0 != ti.is_cross), ti.create_team_timestamp, ti);
}

void TeamChannel::SendTeamList(Role* role)
{
	static Protocol::SCTeamChannelInfo info;
	info.reason = Protocol::SCTeamChannelInfo::REASON_TYPE_NORMAL;
	info.count = 0;

	TeamItemMap::iterator it = m_team_map.begin();
	for (; it != m_team_map.end() && info.count < MAX_TEAM_CHANNEL_ITEM_NUM; ++it)
	{
		info.team_list[info.count++] = it->second;
	}
	
	int length = sizeof(info) - sizeof(info.team_list) + info.count * sizeof(info.team_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, length);
}

const TeamItem* TeamChannel::GetTeamItem(int team_index, bool is_cross, unsigned int create_time)
{
	static TeamOrder to;
	to.team_index = team_index;
	to.is_cross = is_cross ? 1 : 0;
	to.create_time = create_time;

	TeamItemMap::iterator it = m_team_map.find(to);
	if (it != m_team_map.end())
	{
		return &it->second;
	}

	return NULL;
}

void TeamChannel::SendTeamInfoChange(const TeamItem& team_item, short reason)
{
	static Protocol::SCTeamChannelInfo info;
	info.reason = reason;
	info.count = 1;
	info.team_list[0] = team_item;

	int length = sizeof(info) - sizeof(info.team_list) + info.count * sizeof(info.team_list[0]);
	TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_TEAM_CHANNEL, (const char*)&info, length);
}

TeamChannel::TeamOrder TeamChannel::GetTeamOrder(int team_index, bool is_cross, unsigned int create_time)
{
	static TeamOrder to;
	to.team_index = team_index;
	to.is_cross = is_cross ? 1 : 0;
	to.create_time = create_time;

	return to;
}

