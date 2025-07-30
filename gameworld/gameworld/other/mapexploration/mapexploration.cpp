#include "mapexploration.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/mapexploration/mapexplorationconfig.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "protocol/msgother.h"

MapExploration::MapExploration() : m_role_module_mgr(NULL)
{
}

MapExploration::~MapExploration()
{
}

void MapExploration::Init(RoleModuleManager * role_module_manager, const MapExplorationParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
}

void MapExploration::GetOtherInitParam(MapExplorationParam * param)
{
	*param = m_param;
}

void MapExploration::OnRoleLogin()
{
	this->SendInfo();
}

void MapExploration::ExplorationMap(int seq)
{
	if (seq < 0 || seq >= MAX_MAP_EXPLORATION_NUM)
	{
		//参数不对
		return;
	}
	if (m_param.m_map.IsBitSet(seq))
	{
		//已经探索过了
		m_role_module_mgr->NoticeNum(errornum::EN_MAP_EXPLORATION_HAS_CHECK);
		return;
	}
	//给予奖励
	const MapExplorationCfg * cfg = LOGIC_CONFIG->GetMapExplorationConfig()->GetExplorationCfg(seq);
	if (cfg == NULL)return;
	//检测是否在所符合地图位置
	Scene* m_scene = m_role_module_mgr->GetRole()->GetScene();
	if (m_scene == NULL)return;
	
	if (m_scene->GetSceneID() != cfg->scene_id)
	{
		//场景不对
		return;
	}

	const MapExplorationRewardCfg * reward_cfg = LOGIC_CONFIG->GetMapExplorationConfig()->GetExplorationRewardCfg(cfg->reward_group_id);
	if (reward_cfg == NULL)return;

	if (reward_cfg->item_list.size() > 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(reward_cfg->item_list.size(), &reward_cfg->item_list[0]))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		m_role_module_mgr->GetKnapsack()->PutList(reward_cfg->item_list.size(), &reward_cfg->item_list[0], PUT_REASON_MAP_EXPLORATION_REWARD);
	}
	m_param.m_map.SetBit(seq);
	this->SendInfo();
}

void MapExploration::SendInfo()
{
	Protocol::SCMapExplorationInfo info;
	info.m_map = m_param.m_map;
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}
