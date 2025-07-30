#include "worldarena.hpp"
#include "worldarenaschedule.hpp"
#include "worldarenauser.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "gamelog.h"
#include <deque>
#include "global/rmibackobjdef.h"
#include "internalcomm.h"

WorldArenaSchedule::WorldArenaSchedule(WorldArena* arena_ptr) : m_arena_ptr(arena_ptr), m_data_load_status(GLOBAL_SYSTEM_DATA_STATE_INVALID)
{
	memset(m_schedule_arrangement, 0, sizeof(m_schedule_arrangement));
	this->CalculateScheduleArrangement();
}

WorldArenaSchedule::~WorldArenaSchedule()
{

}

void WorldArenaSchedule::Update(unsigned int now)
{
	ScheduleMap::iterator it = m_schedule_map.begin();
	for (; it != m_schedule_map.end(); ++it)
	{
		this->CheckResult(now, &it->second);
	}
}

bool WorldArenaSchedule::AddScheduleFromInit(const WorldArenaScheduleInfo& info)
{
	const WorldArenaScheduleData* schedule = this->GetScheduleInfo(info.schedule_id);
	if (NULL != schedule)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "WorldArenaSchedule::AddScheduleFromInit duplicate schedule_id[%lld]", info.schedule_id);
		gamelog::g_log_world_arena.printf(LL_ERROR, "WorldArenaSchedule::AddScheduleFromInit duplicate schedule_id[%lld]", info.schedule_id);
		return false;
	}

	ScheduleMap::iterator it = m_schedule_map.insert(ScheduleMap::value_type(info.schedule_id, WorldArenaScheduleData(true))).first;
	it->second.MutableScheduleData() = info;
	it->second.SetDirty(false);

	gamelog::g_log_world_arena.printf(LL_INFO, "AddScheduleFromInit schedule_id[%lld] attacker_uuid[%lld] defender_uuid[%lld]", info.schedule_id, info.attacker_uuid, info.defender_uuid);
	return true;
}

void WorldArenaSchedule::MakeScheduleForGroup(long long group_id, std::vector<WorldArenaUser*>& user_vec)
{
	assert(user_vec.size() == WORLD_ARENA_FIGHTER_NUM_PER_GROUP);

	for (size_t i = 0; i < user_vec.size(); ++i)
	{
		user_vec[i]->SetGroupID(group_id);
	}

	for (int i = 0; i < ROUND_NUM; ++i)
	{
		for (int k = 0; k < FIGHT_NUM_PER_ROUND; ++k)
		{
			int attacker_idx = m_schedule_arrangement[i][k][0];
			int defender_idx = m_schedule_arrangement[i][k][1];

			WorldArenaUser* attacker = user_vec[attacker_idx];
			WorldArenaUser* defender = user_vec[defender_idx];

			WorldArenaScheduleInfo schedule_info;
			schedule_info.attacker_uuid = attacker->GetUUID();
			schedule_info.defender_uuid = defender->GetUUID();
			schedule_info.round_index = i;
			schedule_info.fight_result = 0;
			schedule_info.group_id = group_id;
			schedule_info.check_result_timestamp = m_arena_ptr->CalculateRoundBattleEndTimestamp(schedule_info.round_index);
			UniqueIDGenerator::Instance()->GenUniqueID(&schedule_info.schedule_id);

			attacker->SetScheduleID(i, schedule_info.schedule_id);
			defender->SetScheduleID(i, schedule_info.schedule_id);

			ScheduleMap::iterator it = m_schedule_map.insert(ScheduleMap::value_type(schedule_info.schedule_id, WorldArenaScheduleData(false))).first;
			it->second.MutableScheduleData() = schedule_info;

			gamelog::g_log_world_arena.printf(LL_INFO, "WorldArenaSchedule::MakeScheduleForGroup group_id[%lld] schedule_id[%lld] attacker[%d,%s] defender[%d,%s] round_index[%d]",
				group_id, schedule_info.schedule_id, attacker->GetUID(), attacker->GetName(),
				defender->GetUID(), defender->GetName(), schedule_info.round_index);
		}
	}
}

std::vector<const WorldArenaScheduleInfo*> WorldArenaSchedule::GetScheduleInfoListByRoundIndex(int round_index)
{
	assert(m_schedule_map.size() % WORLD_ARENA_ROUND_NUM == 0);

	std::vector<const WorldArenaScheduleInfo*> schedule_vec;
	schedule_vec.reserve(m_schedule_map.size() / WORLD_ARENA_ROUND_NUM);

	for (ScheduleMap::iterator it = m_schedule_map.begin(); it != m_schedule_map.end(); ++it)
	{
		if (it->second.GetScheduleData().round_index == round_index)
		{
			schedule_vec.push_back(&it->second.GetScheduleData());
		}
	}

	return schedule_vec;
}

std::vector<long long> WorldArenaSchedule::GetAllScheduleIds()
{
	std::vector<long long> schedule_vec;
	schedule_vec.reserve(m_schedule_map.size());

	for (ScheduleMap::iterator it = m_schedule_map.begin(); it != m_schedule_map.end(); ++it)
	{
		schedule_vec.push_back(it->second.GetScheduleData().schedule_id);
	}

	return schedule_vec;
}

std::vector<long long> WorldArenaSchedule::GetAllGroupIds()
{
	std::set<long long> group_set;
	for (ScheduleMap::iterator it = m_schedule_map.begin(); it != m_schedule_map.end(); ++it)
	{
		group_set.insert(it->second.GetScheduleData().group_id);
	}

	return std::vector<long long>(group_set.begin(), group_set.end());
}

void WorldArenaSchedule::SetFightResult(long long schedule_id, int result)
{
	WorldArenaScheduleData* info = this->MutableScheduleInfo(schedule_id);
	if (NULL != info)
	{
		info->MutableScheduleData().fight_result = static_cast<short>(result);
		info->MutableScheduleData().check_result_timestamp = 0u;
		gamelog::g_log_world_arena.printf(LL_INFO, "WorldArenaSchedule::SetFightResult schedule_id[%lld] result[%d]",
			schedule_id, result);
	}
	else
	{
		gamelog::g_log_world_arena.printf(LL_INFO, "WorldArenaSchedule::SetFightResult SCHEDULE_NOT_FOUND schedule_id[%lld] result[%d]",
			schedule_id, result);
	}
}

int WorldArenaSchedule::GetFightResult(long long schedule_id)
{
	const WorldArenaScheduleData* info = this->GetScheduleInfo(schedule_id);
	if (NULL == info)
	{
		return WORLD_ARENA_RESULT_INVALID;
	}
	else
	{
		return info->GetScheduleData().fight_result;
	}
}

const WorldArenaScheduleData* WorldArenaSchedule::GetScheduleInfo(long long schedule_id)
{
	return this->MutableScheduleInfo(schedule_id);
}

void WorldArenaSchedule::OnSeasonRestart()
{
	ScheduleMap::iterator it = m_schedule_map.begin();
	for (; it != m_schedule_map.end(); ++it)
	{
		m_to_delete_schedule_map.insert(it->first);
	}
	m_schedule_map.clear();
}

bool WorldArenaSchedule::IsLoadFinish()
{
	return GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_data_load_status;
}

bool WorldArenaSchedule::LoadSchedule(long long id_from)
{
	if (GLOBAL_SYSTEM_DATA_STATE_INVALID == m_data_load_status)
	{
		m_data_load_status = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	}

	RMIInitWorldArenaScheduleDataBackObjectImpl* impl = new RMIInitWorldArenaScheduleDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	return ac.InitWorldArenaScheduleDataAsyn(id_from, impl);
}

void WorldArenaSchedule::OnLoadScheduleSucc()
{
	m_data_load_status = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
}

bool WorldArenaSchedule::Save(bool is_save_all)
{
	WorldArenaScheduleListParamForSave param;

	ScheduleMap::iterator it = m_schedule_map.begin();
	for (; it != m_schedule_map.end(); ++it)
	{
		if (!is_save_all && !it->second.IsDirty()) continue;

		param.save_schedule_list.push_back(it->second.GetScheduleData());
	}

	std::set<long long>::iterator it2 = m_to_delete_schedule_map.begin();
	for (; it2 != m_to_delete_schedule_map.end(); ++it2)
	{
		param.delete_schedule_list.push_back(*it2);
	}

	RMISaveWorldArenaScheduleDataBackObjectImpl* impl = new RMISaveWorldArenaScheduleDataBackObjectImpl();
	impl->SetDeleteList(param.delete_schedule_list);

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveWorldArenaScheduleDataAsyn(param, impl);
	if (ret)
	{
		this->SetScheduleLastSaveTimestampMS(param.save_schedule_list);
	}

	return ret;
}

void WorldArenaSchedule::ClearDirtyMark()
{
	ScheduleMap::iterator it = m_schedule_map.begin();
	for (; it != m_schedule_map.end(); ++it)
	{
		WorldArenaScheduleData* schedule_data = &it->second;
		this->ClearDirtyMark(schedule_data);
	}
}

void WorldArenaSchedule::ClearDirtyMark(WorldArenaScheduleData* schedule_data)
{
	schedule_data->SetOldState(true);

	if (schedule_data->GetLastSaveTimestamp_MS() > schedule_data->GetDirtyTimestamp_MS())
	{
		schedule_data->SetDirty(false);
		schedule_data->SetLastSaveTimestamp_MS(0ULL);
	}
}

void WorldArenaSchedule::CheckResult(unsigned int now, WorldArenaScheduleData* schedule)
{
	unsigned int check_result_timestamp = schedule->GetScheduleData().check_result_timestamp;
	if (check_result_timestamp > 0u && now > check_result_timestamp)
	{
		if (WORLD_ARENA_RESULT_INVALID == schedule->GetScheduleData().fight_result)
		{
			m_arena_ptr->SetFightResult(schedule->GetScheduleId(), WORLD_ARENA_RESULT_DRAW, 0);
		}
	}
}

void WorldArenaSchedule::ClearDeletedSchedule(const std::vector<long long>& delete_list)
{
	for (size_t i = 0; i < delete_list.size(); ++i)
	{
		m_to_delete_schedule_map.erase(delete_list[i]);
	}
}

void WorldArenaSchedule::ModifyScheduleCheckResultTimestamp(long long schedule_id, unsigned int timestamp)
{
	WorldArenaScheduleData* schedule = this->MutableScheduleInfo(schedule_id);
	if (NULL != schedule)
	{
		schedule->MutableScheduleData().check_result_timestamp = timestamp;
	}
}

void WorldArenaSchedule::CalculateScheduleArrangement()
{
	//用数字表示，排成一个环形。
	//0号队不动，每一轮，逆时针转动，让各队从0号头上依次跳过，每移位一次得出的对阵就是本轮对阵。

	std::deque<int> loop_vec;
	for (int i = 0; i < WORLD_ARENA_FIGHTER_NUM_PER_GROUP; ++i)
	{
		loop_vec.push_back(i);
	}

	UNSTD_STATIC_CHECK(WORLD_ARENA_FIGHTER_NUM_PER_GROUP % 2 == 0 && WORLD_ARENA_FIGHTER_NUM_PER_GROUP > 2);

	for (int i = 0; i < ROUND_NUM; ++i)
	{
		int head = 1;
		int tail = static_cast<int>(loop_vec.size() - 1);
		for (int k = 0; k < FIGHT_NUM_PER_ROUND; ++k)
		{
			m_schedule_arrangement[i][k][0] = loop_vec[head];
			m_schedule_arrangement[i][k][1] = loop_vec[tail];
			head += 1;
			tail -= 1;
			if (head == tail)
			{
				head = 0;
			}
		}

		int number = loop_vec[1];
		loop_vec.erase(loop_vec.begin() + 1);
		loop_vec.push_back(number);
	}

	assert(this->ValidateArrangementArray());

	return;
}

WorldArenaScheduleData* WorldArenaSchedule::MutableScheduleInfo(long long schedule_id)
{
	ScheduleMap::iterator it = m_schedule_map.find(schedule_id);
	if (it != m_schedule_map.end())
	{
		return &it->second;
	}

	return NULL;
}

bool WorldArenaSchedule::ValidateArrangementArray()
{
	std::set<std::pair<int, int> > checker;
	for (int i = 0; i < ROUND_NUM; ++i)
	{
		for (int k = 0; k < FIGHT_NUM_PER_ROUND; ++k)
		{
			int attacker = m_schedule_arrangement[i][k][0];
			int defender = m_schedule_arrangement[i][k][1];

			std::pair<int, int> p1(attacker, defender);
			std::pair<int, int> p2(attacker, defender);

			if (checker.find(p1) != checker.end() || checker.find(p2) != checker.end())
			{
				return false;
			}

			checker.insert(p1);
			checker.insert(p2);
		}
	}

	return true;
}

void WorldArenaSchedule::SetScheduleLastSaveTimestampMS(const std::vector<WorldArenaScheduleInfo>& save_schedule_list)
{
	for (size_t i = 0; i < save_schedule_list.size(); ++i)
	{
		long long schedule_id = save_schedule_list[i].schedule_id;
		WorldArenaScheduleData* schedule_data = this->MutableScheduleInfo(schedule_id);
		if (NULL != schedule_data)
		{
			schedule_data->SetLastSaveTimestamp_MS(GetNowTime_MS());
		}
	}
}
