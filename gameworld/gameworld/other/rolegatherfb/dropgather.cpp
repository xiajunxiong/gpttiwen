#include "dropgather.hpp"
#include "gameworld/config/logicconfigmanager.hpp"
#include "other/rolegatherfb/gatherfbconfig.hpp"
#include "obj/character/npc_gather.h"
#include "gameworld/gamelog.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "world.h"
#include "servercommon/serverconfig/crossconfig.hpp"

DropGather::DropGather()
{
}

DropGather::~DropGather()
{
}

DropGather & DropGather::Instance()
{
	static DropGather instance;
	return instance;
}

void DropGather::OnStarDropGather()
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	const std::map<int, FieldGatherCfg> * cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetFieldGatherList();
	if (cfg == NULL)return;
	for (std::map<int, FieldGatherCfg>::const_iterator it = cfg->begin(); it != cfg->end(); it++)
	{
		m_seq_refresh_time_map[it->first] = EngineAdapter::Instance().Time() + it->second.time_second;
	}
}

void DropGather::OnGather(Scene * scene, Obj * gather_id)
{
	if (scene == NULL || gather_id == NULL)return;
	std::map<int, std::vector<Gather_info> >::iterator it = m_scene_gather_pos_map.find(scene->GetSceneID());
	if (it != m_scene_gather_pos_map.end())
	{
		for (std::vector<Gather_info>::iterator it2 = it->second.begin(); it2 != it->second.end();it2++)
		{
			if (it2->gather_id == gather_id->GetId())
			{
				it->second.erase(it2);
				return;
			}
		}
	}
}

void DropGather::Update(time_t now_second)
{
	for (std::map<int, time_t>::iterator it = m_seq_refresh_time_map.begin(); it != m_seq_refresh_time_map.end(); it++)
	{
		if (it->second <= now_second)
		{
			const FieldGatherCfg * cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetFieldGatherCfg(it->first);
			if (cfg != NULL)
			{
				it->second = now_second + cfg->time_second;
				SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
				if (scene_manager != NULL)
				{
					Scene * scene = scene_manager->GetSceneById(cfg->scene_id, COMMON_SCENE_KEY);
					if (scene != NULL)
					{
						const RoleGatherFbPosCfg * pos_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetPosCfg(cfg->pos_group);
						if (pos_cfg == NULL || 0 == (int)pos_cfg->create_pos.size())continue;
						std::vector<Gather_info> & info = m_scene_gather_pos_map[cfg->scene_id];
						if ((int)info.size() >= cfg->limit_num || (cfg->max_num - cfg->min_num + 1) == 0)continue;
						int num = rand() % (cfg->max_num - cfg->min_num + 1) + cfg->min_num;
						for (int j = (int)info.size(); j < cfg->limit_num && num > 0;j++)
						{
							int i = rand() % (int)pos_cfg->create_pos.size();
							//检测该位置是否有采集物了
							if(this->HasGatherInPos(cfg->scene_id, pos_cfg->create_pos[i]))continue;
							NPCGather * gather = NpcManager::Instance().CreateNPCGather(cfg->gather_id, scene, pos_cfg->create_pos[i]);
							if (gather == NULL)continue;
							Gather_info data;
							data.gather_id = gather->GetId();
							data.gather_pos = gather->GetPos();
							info.push_back(data);
							num -= 1;
						}
						
					}
				}
			}
		}
	}
}

bool DropGather::HasGatherInPos(int scene_id, Posi gather_pos)
{
	std::map<int, std::vector<Gather_info> >::iterator it = m_scene_gather_pos_map.find(scene_id);
	if (it != m_scene_gather_pos_map.end())
	{
		for (std::vector<Gather_info>::iterator it_2 = it->second.begin();it_2 != it->second.end();it_2++)
		{
			if (it_2->gather_pos == gather_pos)
			{
				return true;
			}
		}
	}
	return false;
}
