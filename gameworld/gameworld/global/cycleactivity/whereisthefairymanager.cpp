#include "whereisthefairymanager.hpp"
#include "engineadapter.h"
#include "world.h"

#include "npc/npcmanager.h"
#include "obj/character/role.h"

#include "protocol/cycleactivity/msgwhereisthefairy.hpp"
#include "global/zhouqimanager/zhouqimanager.h"

#include "servercommon/zhouqidef.hpp"

#include "config/logicconfigmanager.hpp"
#include "other/cycleactivity/whereisthefairy/whereisthefairyconfig.hpp"

WhereIsTheFairyManager & WhereIsTheFairyManager::Instance()
{
	static WhereIsTheFairyManager where_is_the_fairy_manager;
	return where_is_the_fairy_manager;
}

void WhereIsTheFairyManager::Update(time_t now_second)
{
	if (ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_WHERE_IS_THE_FAIRY) && ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_WHERE_IS_THE_FAIRY))
	{
		if (0 == m_last_scene_id && 0 == m_last_obj_id)	//刚开服/周期刚开始
		{
			this->OnCreateNpc();
		}
		else if (0 != m_next_refresh_timestamp && m_next_refresh_timestamp < (unsigned int)now_second)
		{
			this->OnDeleteNpc();
			this->OnCreateNpc();
		}
	}
	else
	{
		this->OnDeleteNpc();
	}
}

void WhereIsTheFairyManager::SendInfo(Role * role)
{
	Protocol::SCWhereIsTheFairyInfo info;
	info.scene_id = m_last_scene_id;
	info.next_refresh_timestamp = m_next_refresh_timestamp;

	if (NULL == role)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
}

WhereIsTheFairyManager::WhereIsTheFairyManager() :m_last_scene_id(0), m_last_obj_id(0), m_next_refresh_timestamp(0)
{
}

WhereIsTheFairyManager::~WhereIsTheFairyManager()
{
}

void WhereIsTheFairyManager::OnCreateNpc()
{
	const WhereIsTheFairyBaseCfg & base_cfg = LOGIC_CONFIG->GetWhereIsTheFairyConfig()->GetBaseCfg();
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int zero_timestamp = (unsigned int)GetZeroTime(now_second);
	unsigned int last_refresh_timestamp = now_second - ((now_second - zero_timestamp) % base_cfg.live_time);
	WhereIsTheFairySceneCfg scene_cfg;
	if(!LOGIC_CONFIG->GetWhereIsTheFairyConfig()->GetRandSceneCfg(last_refresh_timestamp, scene_cfg))
	{
		return;
	}

	Scene * npc_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_cfg.scene_id, COMMON_SCENE_KEY);
	if (NULL == npc_scene)
	{
		return;
	}
	NPCChosed * npc_chosed = NpcManager::Instance().CreateNPCChosed(base_cfg.npc_id, npc_scene, scene_cfg.npc_pos, 0, &scene_cfg.npc_dir);
	if (NULL == npc_chosed)
	{
		return;
	}

	m_last_scene_id = scene_cfg.scene_id;
	m_last_obj_id = npc_chosed->GetId();
	m_next_refresh_timestamp = last_refresh_timestamp + base_cfg.live_time;
}

void WhereIsTheFairyManager::OnDeleteNpc()
{
	if (m_last_scene_id <= 0) return ;

	const WhereIsTheFairyBaseCfg & base_cfg = LOGIC_CONFIG->GetWhereIsTheFairyConfig()->GetBaseCfg();
	Scene * npc_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(m_last_scene_id, COMMON_SCENE_KEY);
	if (NULL != npc_scene)
	{
		npc_scene->DeleteObj(m_last_obj_id);
	}

	m_last_scene_id = 0;
	m_last_obj_id = 0;
	m_next_refresh_timestamp = 0;
}
