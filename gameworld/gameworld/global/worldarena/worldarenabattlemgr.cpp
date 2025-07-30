#include "worldarenabattlemgr.hpp"
#include "servercommon/basedef.h"
#include "servercommon/struct/global/worldarenaparam.hpp"
#include "scene/scene.h"
#include "world.h"
#include "gamelog.h"
#include "scene/speciallogicimpl/speciallogicworldarenstandbyroom.hpp"
#include "config/logicconfigmanager.hpp"
#include "worldarenaconfig.hpp"
#include "obj/character/role.h"
#include "worldarenauser.hpp"
#include "worldarena.hpp"

WorldArenaBattleMgr::WorldArenaBattleMgr(WorldArena* wa) : m_world_arena(wa), m_room_key_generator(2),
	m_has_create_scenes_for_today(false), m_cur_round_index(-1)
{

}

WorldArenaBattleMgr::~WorldArenaBattleMgr()
{

}

bool WorldArenaBattleMgr::HasCreateScenesForToday() const
{
	return m_has_create_scenes_for_today;
}

void WorldArenaBattleMgr::ResetCreateScenesFlag()
{
	m_has_create_scenes_for_today = false;
}

void WorldArenaBattleMgr::ResetRoundIndex()
{
	m_cur_round_index = -1;
}

void WorldArenaBattleMgr::CreateScenesForSchedulesToday(int round_index, const std::vector<const WorldArenaScheduleInfo*>& today_schedules)
{
	this->ResetLocalData();
	for (size_t i = 0; i < today_schedules.size(); ++i)
	{
		if (today_schedules[i]->fight_result != 0)
		{
			// 已有结果，不需要再开房间了
			gamelog::g_log_world_arena.printf(LL_INFO, "%s Already Have Result, Not Need To Create Scene, schedule_id[%lld] attacker_uuid[%lld] defender_uuid[%lld] result[%d]",
				__FUNCTION__, today_schedules[i]->schedule_id, today_schedules[i]->attacker_uuid, today_schedules[i]->defender_uuid, today_schedules[i]->fight_result);
			continue;
		}

		const WorldArenaUser* attacker_user = m_world_arena->GetWorldArenaUser(today_schedules[i]->attacker_uuid);
		if (NULL == attacker_user)
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s Attacker Not Found, schedule_id[%lld] attacker_uuid[%lld] defender_uuid[%lld]",
				__FUNCTION__, today_schedules[i]->schedule_id, today_schedules[i]->attacker_uuid, today_schedules[i]->defender_uuid);
			continue;
		}

		const WorldArenaUser* defender_user = m_world_arena->GetWorldArenaUser(today_schedules[i]->defender_uuid);
		if (NULL == defender_user)
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s Defender Not Found, schedule_id[%lld] attacker_uuid[%lld] defender_uuid[%lld]",
				__FUNCTION__, today_schedules[i]->schedule_id, today_schedules[i]->attacker_uuid, today_schedules[i]->defender_uuid);
			continue;
		}

		int scene_id = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().standby_scene_id;
		int try_times = 0;
		int fb_key = 0;
		while (try_times++ < 100)
		{
			fb_key = this->GenFBKey();
			if (NULL == World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_id, fb_key))
			{
				break;
			}
		}

		ScheduleInfo si;
		si.schedule_id = today_schedules[i]->schedule_id;
		si.attacker_uuid = today_schedules[i]->attacker_uuid;
		si.defender_uuid = today_schedules[i]->defender_uuid;
		si.fb_key = fb_key;
		
		Scene* scene = NULL;
		bool succ = World::GetInstWorld()->GetSceneManager()->CreateFB(scene_id, fb_key, &scene);
		if (!succ || NULL == scene)
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "CreateScenesForSchedulesToday FAILED, schedule[%lld] attaker[%d] defender[%d] scene_id[%d] scene_key[%d]",
				si.schedule_id, GetUidFromUniqueUserID(si.attacker_uuid), GetUidFromUniqueUserID(si.defender_uuid), scene_id, fb_key);
			
			continue;
		}

		SpecialLogic* spl = scene->GetSpecialLogic();
		if (spl->GetSceneType() != Scene::SCENE_TYPE_WORLD_ARENA_STANDBY_ROOM)
		{
			assert(0);
			continue;
		}

		SpecialLogicWorldArenaStandbyRoom* standby_room_spl = static_cast<SpecialLogicWorldArenaStandbyRoom*>(spl);
		standby_room_spl->SetScheduleID(si.schedule_id);
		standby_room_spl->SetAttackerUUID(si.attacker_uuid);
		standby_room_spl->SetAttackerUID(attacker_user->GetUID());
		standby_room_spl->SetAttackerName(attacker_user->GetName());
		standby_room_spl->SetDefenderUUID(si.defender_uuid);
		standby_room_spl->SetDefenderUID(defender_user->GetUID());
		standby_room_spl->SetDefenderName(defender_user->GetName());
		standby_room_spl->OnInitFinish();
		
		m_today_schedules.insert(SchduleMap::value_type(si.schedule_id, si));
		m_user_scene_key_hash.insert(UserSceneKeyHash::value_type(si.attacker_uuid, fb_key));
		m_user_scene_key_hash.insert(UserSceneKeyHash::value_type(si.defender_uuid, fb_key));
	}
	
	m_has_create_scenes_for_today = true;
	m_cur_round_index = round_index;
}

void WorldArenaBattleMgr::ResetLocalData()
{
	m_cur_round_index = -1;
	m_today_schedules.clear();
	m_user_scene_key_hash.clear();
}

int WorldArenaBattleMgr::GenFBKey()
{
	int ret_key = m_room_key_generator++;
	if (m_room_key_generator >= MAX_INT16)
	{
		m_room_key_generator = 2;
	}

	return ret_key;
}

SpecialLogic* WorldArenaBattleMgr::GetSpecialLogicForUser(long long uuid)
{
	UserSceneKeyHash::iterator it = m_user_scene_key_hash.find(uuid);
	if (it == m_user_scene_key_hash.end()) return NULL;
	
	int fb_key = it->second;
	int scene_id = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().standby_scene_id;
	Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_id, fb_key);
	if (NULL == scene) return NULL;

	assert(Scene::SCENE_TYPE_WORLD_ARENA_STANDBY_ROOM == scene->GetSceneType());
	if (Scene::SCENE_TYPE_WORLD_ARENA_STANDBY_ROOM != scene->GetSceneType()) return NULL;
	
	SpecialLogicWorldArenaStandbyRoom* spl = static_cast<SpecialLogicWorldArenaStandbyRoom*>(scene->GetSpecialLogic());	
	assert(spl->GetAttackerUUID() == uuid || spl->GetDefenderUUID() == uuid);

	if (spl->IsInFightFinishStatus())
	{
		// 结果已出，再进去都无用啦
		return NULL;
	}

	return spl;
}

void WorldArenaBattleMgr::StandbyForFight(Role* role, bool is_ready)
{
	SpecialLogic* spl = role->GetScene()->GetSpecialLogic();
	if (NULL != spl && Scene::SCENE_TYPE_WORLD_ARENA_STANDBY_ROOM == spl->GetSceneType())
	{
		SpecialLogicWorldArenaStandbyRoom* room = static_cast<SpecialLogicWorldArenaStandbyRoom*>(spl);
		room->OnRoleReady(role, is_ready);
	}
}

