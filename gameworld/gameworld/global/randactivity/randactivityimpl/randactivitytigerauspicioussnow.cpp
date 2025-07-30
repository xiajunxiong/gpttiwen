#include "randactivitytigerauspicioussnow.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitytigerauspicioussnowconfig.hpp"
#include "world.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "gameworld/gamelog.h"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "obj/character/npc_gather.h"
#include "servercommon/string/gameworldstr.h"
#include "engineadapter.h"
#include "global/usercache/usercache.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitytigerauspicioussnow.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/randactivity/randactivityadapter.hpp"


RandActivityTigerAuspiciousSnow::RandActivityTigerAuspiciousSnow(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW)
{
	next_refresh_timestamp = 0;
}

RandActivityTigerAuspiciousSnow::~RandActivityTigerAuspiciousSnow()
{
}

void RandActivityTigerAuspiciousSnow::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (to_status == ACTIVITY_STATUS_OPEN)
	{
		this->RefreshSnow();
	}

	if (to_status == ACTIVITY_STATUS_CLOSE)
	{
		this->ClearSnow();			//活动关闭，回收所有雪堆
		m_scene_snow_map.clear();
		next_refresh_timestamp = 0;
	}
}

void RandActivityTigerAuspiciousSnow::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN && now_second >= next_refresh_timestamp)
	{
		const RandActivityTigerAuspiciousSnowConfig * config = static_cast<const RandActivityTigerAuspiciousSnowConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW));
		if (NULL == config)
		{
			return;
		}

		const RATigerSpSnowOtherCfg & other_cfg = config->GetOtherCfg();

		next_refresh_timestamp = m_rand_activity_manager->GetRandActivityBeginTime(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);
		next_refresh_timestamp = next_refresh_timestamp - ((next_refresh_timestamp + 8 * 3600) % 86400);

		while (now_second >= next_refresh_timestamp)
		{
			next_refresh_timestamp += other_cfg.refresh_times;
		}

		this->RefreshSnow();
	}
}

bool RandActivityTigerAuspiciousSnow::IsReceiveSnow(Role * role, NPCGather * gahter)
{
	if (NULL == role || NULL == gahter) return false;

	int m_scene_id = role->GetScene()->GetSceneID();

	std::map<int, std::vector<ObjID> >::iterator it = m_scene_snow_map.find(m_scene_id);
	if (it != m_scene_snow_map.end())
	{
		for (std::vector<ObjID>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
		{
			if (*vit == gahter->GetId()) return true;
		}
	}
	return false;
}

bool RandActivityTigerAuspiciousSnow::ReceiveSnowOp(Role * role, NPCGather * gather)
{
	const RandActivityTigerAuspiciousSnowConfig * config = static_cast<const RandActivityTigerAuspiciousSnowConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW));
	if (NULL == config)
	{
		return false;
	}

	const RATigerSpSnowOtherCfg & other_cfg = config->GetOtherCfg();
	const RATigerSpSonwDriftCfg * reward_cfg = config->GetDriftCfg(m_rand_activity_manager);
	if (NULL == reward_cfg) return false;

	RoleActivityTigerAuspiciousSnow * role_activity = static_cast<RoleActivityTigerAuspiciousSnow *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW));
	if (NULL == role_activity)
	{
		return false;
	}

	int times = role_activity->GetRATigerSpSnowTimes();
	if (times >= other_cfg.collect_times)
	{
		role->NoticeNum(errornum::EN_RAND_ACTIVITY_MAX_RECEIVE_NUM);
		return false;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg->v_reward))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	Scene* scene = role->GetScene();
	if (NULL == scene)
	{
		return false;
	}

	//删除采集物
	bool is_delete = false;
	std::map<int, std::vector<ObjID> >::iterator it = m_scene_snow_map.find(scene->GetSceneID());
	if (it != m_scene_snow_map.end())
	{
		for (std::vector<ObjID>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
		{
			if (*vit == gather->GetId())
			{
				Obj* box_obj = scene->GetObj(*vit);
				if (NULL == box_obj) return false;
				if (box_obj->GetObjType() != Obj::OBJ_TYPE_NPC_GATHERS) return false;
				if (!scene->DeleteObj(*vit))
				{
					return false;
				}

				it->second.erase(vit);
				is_delete = true;
				break;
			}
		}
	}

	if (is_delete)
	{
		// 下发奖励
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(reward_cfg->v_reward, PUT_REASON_RA_AUSPICIOUS_SNOW_PICK_UP);
		role_activity->RATigerSpSnowAddTimes();

		return true;
	}

	return false;
}

void RandActivityTigerAuspiciousSnow::RefreshSnow()
{
	const RandActivityTigerAuspiciousSnowConfig * config = static_cast<const RandActivityTigerAuspiciousSnowConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW));
	if (NULL == config)
	{
		return;
	}

	const std::vector<RAtigerSpSnowSceneCfg> & scene_cfg = config->GetAllSceneCfg();
	const RATigerSpSnowOtherCfg & other_cfg = config->GetOtherCfg();

	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (NULL != scene_manager)
	{
		for (std::vector<RAtigerSpSnowSceneCfg>::const_iterator it_cfg = scene_cfg.begin(); it_cfg != scene_cfg.end(); ++it_cfg)
		{
			for (int i = 0; i < other_cfg.first_num; ++i)
			{
				std::map<int, std::vector<ObjID> >::iterator it = m_scene_snow_map.find(it_cfg->scene_id);
				if (it != m_scene_snow_map.end() && (int)it->second.size() >= other_cfg.first_num) break;

				Scene * scene = scene_manager->GetSceneById(it_cfg->scene_id, COMMON_SCENE_KEY);
				if (NULL != scene)
				{
					NPCGather* gather = NpcManager::Instance().CreateNPCGather(it_cfg->npc_id, scene, scene->GetARandPosi());
					if(NULL == gather) continue;

					if (it == m_scene_snow_map.end())
					{
						m_scene_snow_map[it_cfg->scene_id].push_back(gather->GetId());
					}
					else
					{
						it->second.push_back(gather->GetId());
					}

					gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityTigerAuspiciousSnow::RefreshSnow scene_id[%d] snow_pos[%d,%d] max_case_num[%d] case_idx[%d]",
						it_cfg->scene_id, gather->GetPos().x, gather->GetPos().y, other_cfg.first_num, i);
				}
			}
		}
	}
}

void RandActivityTigerAuspiciousSnow::ClearSnow()
{
	SceneManager* scene_manager = World::GetInstWorld()->GetSceneManager();
	if (NULL == scene_manager)return;

	for (std::map<int, std::vector<ObjID> >::iterator it = m_scene_snow_map.begin(); it != m_scene_snow_map.end(); ++it)
	{
		Scene* scene = scene_manager->GetSceneById(it->first, COMMON_SCENE_KEY);
		if (NULL != scene)
		{
			for (std::vector<ObjID>::iterator obj_it = it->second.begin(); obj_it != it->second.end(); ++obj_it)
			{
				Obj * snow_obj = scene->GetObj(*obj_it);
				if(NULL == snow_obj) continue;

				if(snow_obj->GetObjType() != Obj::OBJ_TYPE_NPC_GATHERS) continue;

				scene->DeleteObj(*obj_it);
			}
		}
	}

}


