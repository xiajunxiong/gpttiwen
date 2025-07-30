#include "rolefirstkill.hpp"
#include "servercommon/logdef.h"
#include "gamelog.h"
#include "other/rolemodulemanager.hpp"
#include "world.h"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/advancecareer/advancecareer.hpp"

RoleFirstKill::RoleFirstKill() : m_mgr(NULL)
{
	
}

RoleFirstKill::~RoleFirstKill()
{

}

void RoleFirstKill::Init(RoleModuleManager* mgr, const RoleFirstKillParamList& list)
{
	m_mgr = mgr;
	for (int i = 0; i < list.count && i < MAX_FIRST_KILL_RECORD_NUM; ++i)
	{
		m_info_map.insert(FirstKillInfoMap::value_type(list.info_list[i].monster_group_id, list.info_list[i]));
	}
}

bool RoleFirstKill::GetChangedFirstKillInfo(RoleFirstKillParamList* list)
{
	m_dirty_group_map.insert(m_snap_dirty_group_map.begin(), m_snap_dirty_group_map.end());
	DirtyGroupMap::iterator it_begin = m_snap_dirty_group_map.begin();
	DirtyGroupMap::iterator it_end = m_snap_dirty_group_map.end();
	for (; it_begin != it_end; it_begin++)
	{
		const RoleFirstKillInfo* info = this->GetFirstKillInfo(it_begin->first);
		if (NULL != info)
		{
			list->info_list[list->count] = *info;
			list->info_list[list->count++].change_state = it_begin->second;
		}
		it_begin->second = structcommon::CS_NONE;
	}

	return true;

/*	for (int i = m_dirty_group_vec.size() - 1; i >= 0 && list->count < MAX_FIRST_KILL_RECORD_NUM; --i)
	{
		int group_id = m_dirty_group_vec[i];

		const RoleFirstKillInfo* info = this->GetFirstKillInfo(group_id);
		if (NULL != info)
		{
			list->info_list[list->count++] = *info;
		}
	}
*/
}

void RoleFirstKill::OnWeekChange()
{
	FirstKillInfoMap::iterator it_begin = m_info_map.begin();
	FirstKillInfoMap::iterator it_end = m_info_map.end();
	for (; it_begin != it_end; it_begin++)
	{
		it_begin->second.week_kill_flag = 0;
		m_snap_dirty_group_map[it_begin->first] = m_snap_dirty_group_map[it_begin->first] == structcommon::CS_INSERT ? structcommon::CS_INSERT : structcommon::CS_UPDATE;
	}
}

void RoleFirstKill::OnKillMonsterGroup(int group_id, int team_member_num, int win_round, int team_level_high, int team_level_low, int average_level, int dead_role_num)
{
	FirstKillInfoMap::iterator it = m_info_map.find(group_id);
	if (it != m_info_map.end())
	{
		it->second.week_kill_flag = 1;
		m_snap_dirty_group_map[group_id] = m_snap_dirty_group_map[group_id] == structcommon::CS_INSERT ? structcommon::CS_INSERT : structcommon::CS_UPDATE;
		return;
	}

	ROLE_LOG_QUICK10(LOG_TYPE_ROLE_FIRST_KILL_MONSTER_GROUP, m_mgr->GetRole(), group_id, team_member_num, NULL, NULL, win_round, team_level_low, team_level_high, average_level, dead_role_num);

	static RoleFirstKillInfo info;
	info.Reset();
	info.monster_group_id = group_id;
	info.timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	info.team_member_num = team_member_num;
	info.win_round = win_round;
	info.team_level_low = team_level_low;
	info.team_level_high = team_level_high;
	info.team_level_average = average_level;
	info.dead_role_num = dead_role_num;
	info.week_kill_flag = 1;

	m_info_map.insert(FirstKillInfoMap::value_type(group_id, info));
	m_snap_dirty_group_map[group_id] = structcommon::CS_INSERT;
	m_mgr->GetCourseOfGrowth()->EventOperator(COG_CON_KILL_MONTER_GROUP, group_id, 1);
	m_mgr->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_CHALLENGE_MONSTER_SUCC, group_id, 0);
}

void RoleFirstKill::ClearDirtyMark()
{
	DirtyGroupMap::iterator it_begin = m_dirty_group_map.begin();
	DirtyGroupMap::iterator it_end = m_dirty_group_map.end();
	for (; it_begin != it_end; it_begin++)
	{
		if (structcommon::CS_NONE == m_snap_dirty_group_map[it_begin->first])
		{
			m_snap_dirty_group_map.erase(it_begin->first);
		}
	}

	m_dirty_group_map.clear();
}

bool RoleFirstKill::HasKillMonsterGroup(int group_id)
{
	return m_info_map.find(group_id) != m_info_map.end();
}

bool RoleFirstKill::HasWeekKiilMonsterGroup(int group_id)
{
	FirstKillInfoMap::iterator it = m_info_map.find(group_id);
	if (it != m_info_map.end())
	{
		if (0 != it->second.week_kill_flag)
		{
			return true;
		}
	}
	return false;
}

const RoleFirstKillInfo* RoleFirstKill::GetFirstKillInfo(int group_id)
{
	FirstKillInfoMap::iterator it = m_info_map.find(group_id);
	if (it != m_info_map.end())
	{
		return &it->second;
	}

	return NULL;
}
