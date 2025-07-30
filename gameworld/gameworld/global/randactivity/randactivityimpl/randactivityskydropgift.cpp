#include "randactivityskydropgift.hpp"
#include <algorithm>

#include "scene/scene.h"
#include "world.h"
#include "servercommon/activitydef.hpp"
#include "scene/scenemanager.hpp"
#include "engineadapter.h"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityskydropluxurygiftconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "obj/character/npc_gather.h"
#include "gameworld/gamelog.h"
#include "global/randactivity/randactivityadapter.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityskydropluxurygiftconfig.hpp"


RandActivitySkyDropGift::RandActivitySkyDropGift(RandActivityManager *activity_manager) : RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT)
{
	next_refresh_tamp = 0;
}

RandActivitySkyDropGift::~RandActivitySkyDropGift()
{

}

void RandActivitySkyDropGift::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	if (to_status == ACTIVITY_STATUS_CLOSE)
	{//活动关闭，回收所有宝箱
		this->ClearBox();
		m_box_scene_map.clear();
		next_refresh_tamp = 0;
	}
	else if(to_status == ACTIVITY_STATUS_OPEN)
	{
		this->RefreshBox();
	}
}

void RandActivitySkyDropGift::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (next_refresh_tamp != 0 && next_refresh_tamp <= now_second)
	{
		this->RefreshBox();
	}
}

void RandActivitySkyDropGift::GetBoxReward(Role * role,NPCGather * gahter)
{
	if (role == NULL || gahter == NULL)return;
	int m_scene_id = role->GetScene()->GetSceneID();
	std::map<int, std::vector<ObjID> >::iterator it = m_box_scene_map.find(m_scene_id);
	if (it != m_box_scene_map.end())
	{
		for (std::vector<ObjID>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
		{
			if (*it2 == gahter->GetId())
			{
				it->second.erase(it2);
				break;
			}
		}
	}
}

void RandActivitySkyDropGift::RefreshBox()
{
	const UniqueServerID& usid = LocalConfig::Instance().GetUniqueServerID();
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(usid);
	if (NULL == ramgr) return;

	//获取刷新场景列表，刷新最大数量
	const RandActivitySkyDropLuxuryGiftConfig * config = static_cast<const RandActivitySkyDropLuxuryGiftConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT));
	if (NULL == config)
	{
		return;
	}

	const std::vector<SkyDropGiftCfg>* cfg = config->GetSkyDropGiftCfg(ramgr);
	if (cfg == NULL)return;
	
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if(scene_manager != NULL)
	{
		for (std::vector<SkyDropGiftCfg>::const_iterator it_cfg = cfg->begin(); it_cfg != cfg->end(); it_cfg++)
		{
			for (int i = 0;i < it_cfg->mount && i < it_cfg->max_box_num; i++)
			{
				std::map<int, std::vector<ObjID> >::iterator it = m_box_scene_map.find(it_cfg->scene_id);
				if (it != m_box_scene_map.end() && (int)it->second.size() >= it_cfg->max_box_num)continue;
				Scene * scene = scene_manager->GetSceneById(it_cfg->scene_id, COMMON_SCENE_KEY);
				if (scene != NULL)
				{
					NPCGather * gather = NpcManager::Instance().CreateNPCGather(it_cfg->treasure_chest_id, scene, scene->GetARandPosi());
					if (gather == NULL)continue;
					gather->SetGatherByTeam(false);
					if (it == m_box_scene_map.end())
					{
						m_box_scene_map[it_cfg->scene_id].push_back(gather->GetId());
					}
					else
					{
						it->second.push_back(gather->GetId());
					}
				}
			}
		}
	}
	next_refresh_tamp = EngineAdapter::Instance().Time() + cfg->begin()->next_time;//把第一个的时间作为刷新间隔时间
}

void RandActivitySkyDropGift::ClearBox()
{
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (scene_manager == NULL)return;
	for (std::map<int, std::vector<ObjID> >::iterator it = m_box_scene_map.begin();it != m_box_scene_map.end();it++)
	{
		Scene * scene = scene_manager->GetSceneById(it->first, COMMON_SCENE_KEY);
		if (scene != NULL)
		{
			for (std::vector<ObjID>::iterator obj_it = it->second.begin();obj_it != it->second.end();obj_it++)
			{
				Obj * box_obj = scene->GetObj(*obj_it);
				if(box_obj == NULL)continue;
				if (box_obj->GetObjType() != Obj::OBJ_TYPE_NPC_GATHERS)continue;
				if (!scene->DeleteObj(*obj_it))
				{
					gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivitySkyDropGift::ClearBox() activity_type %d, clear_err[scene_id:%d , objid:%d]",
						m_rand_activity_type, it->first, *obj_it);
				}
			}
		}
	}
}
