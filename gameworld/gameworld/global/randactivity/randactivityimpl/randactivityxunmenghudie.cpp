#include "randactivityxunmenghudie.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityxunmenghudieconfig.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "obj/otherobj/npcother.h"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityxunmenghudie.hpp"
#include "gamelog.h"

RandAcitivityXunMengHuDie::RandAcitivityXunMengHuDie(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_XUNMENG_HUDIE), check_init(false), next_refresh_npc_tamp(0)
{
}

RandAcitivityXunMengHuDie::~RandAcitivityXunMengHuDie()
{
}

void RandAcitivityXunMengHuDie::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	if (ACTIVITY_STATUS_OPEN == to_status)
	{
		this->OnRefreshNpc(EngineAdapter::Instance().Time());
	}
	else if (ACTIVITY_STATUS_CLOSE == to_status)
	{
		next_refresh_npc_tamp = 0;
		this->CleanNpc();
		m_npc_vec.clear();
	}
}

void RandAcitivityXunMengHuDie::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (next_refresh_npc_tamp != 0 && next_refresh_npc_tamp <= now_second)
	{
		next_refresh_npc_tamp = 0;
		this->OnRefreshNpc(now_second);//刷新怪物
	}

	if (!check_init && this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		check_init = true;
		this->OnRefreshNpc(now_second);//刷新怪物
	}
}

void RandAcitivityXunMengHuDie::OnRefreshNpc(time_t now_second)
{
	if (this->GetStatus() != ACTIVITY_STATUS_OPEN)return;
	if (CrossConfig::Instance().IsHiddenServer())return;

	this->CleanNpc();
	m_npc_vec.clear();

	const RandActivityXunMengHuDieConfig * config = static_cast<const RandActivityXunMengHuDieConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_XUNMENG_HUDIE));
	if (NULL == config)
		return;

	const std::vector<RAXunMengHuDieSceneCfg> npc_map = config->GetSceneMapCfg();
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (scene_manager != NULL)
	{
		for (unsigned int i = 0; i < npc_map.size(); i++)
		{
			Scene * scene = scene_manager->GetSceneById(npc_map[i].scene_id, COMMON_SCENE_KEY);
			if (scene != NULL)
			{
				NPCOther * npc = new NPCOther();
				npc->SetNpcId(npc_map[i].npc_id);
				npc->SetPos(npc_map[i].pos);

				scene->AddObj(npc);

				NpcInfo npc_info;
				npc_info.npc_objid = npc->GetId();
				npc_info.scene_id = npc_map[i].scene_id;
				npc_info.npc_seq = npc->GetNpcSeq();

				m_npc_vec.push_back(npc_info);
			}
		}
	}

	next_refresh_npc_tamp = now_second + EngineAdapter::Instance().NextHourInterval(0, 0);
}

void RandAcitivityXunMengHuDie::OnComplete(Role * role, bool is_win, ObjID npc_objid)
{
	if (NULL == role) return;

	Obj* npc_obj = role->GetScene()->GetObj(npc_objid);
	if (NULL == npc_obj || npc_obj->GetObjType() != Obj::OBJ_TYPE_NPC_FUNC)
	{
		role->NoticeNum(errornum::EN_RA_XUNMENG_HUDIE_NOT_NPC);
		return;
	}
	
	role->GetScene()->DeleteObj(npc_objid);
	
	RoleActivityXunMengHuDie * role_activity = static_cast<RoleActivityXunMengHuDie *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XUNMENG_HUDIE));
	if (NULL == role_activity)
		return;

	role_activity->OnComplete(is_win);

	NPC * npc_other = (NPC *)npc_obj;
	//
	gamelog::g_log_rand_activity.printf(LL_INFO, "RandAcitivityXunMengHuDie::OnComplete role[%d, %s] role_level[%d] is_win[%d] npc_id[%d] pos[%d, %d]",
		role->GetUID(), role->GetName(), role->GetLevel(), is_win, npc_other->GetNpcId(), npc_other->GetPos().x, npc_other->GetPos().y);
}


void RandAcitivityXunMengHuDie::CleanNpc()
{
	for (std::vector<NpcInfo>::iterator npc_it = m_npc_vec.begin(); npc_it != m_npc_vec.end(); npc_it++)
	{
		Scene * npc_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(npc_it->scene_id, COMMON_SCENE_KEY);
		if (npc_scene != NULL)
		{
			Obj* npc_obj = npc_scene->GetObj(npc_it->npc_objid);
			if (NULL != npc_obj && npc_obj->GetObjType() == Obj::OBJ_TYPE_NPC_FUNC)
			{
				npc_scene->DeleteObj(npc_it->npc_objid);
			}
		}
	}
}
