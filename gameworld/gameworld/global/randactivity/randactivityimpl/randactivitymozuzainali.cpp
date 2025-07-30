#include "randactivitymozuzainali.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitymozuzainaliconfig.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "npc/npcmanager.h"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitymozuzainali.hpp"

RandAcitivityMoZuZaiNaLi::RandAcitivityMoZuZaiNaLi(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI), check_init(false), next_refresh_monster_tamp(0)
{
}

RandAcitivityMoZuZaiNaLi::~RandAcitivityMoZuZaiNaLi()
{
}

void RandAcitivityMoZuZaiNaLi::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	if (ACTIVITY_STATUS_OPEN == to_status)
	{
		this->OnRefreshMonster(EngineAdapter::Instance().Time());
	}
	else if (ACTIVITY_STATUS_CLOSE == to_status)
	{
		next_refresh_monster_tamp = 0;
		this->CleanMonster();
		m_monster_vec.clear();
	}
}

void RandAcitivityMoZuZaiNaLi::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (next_refresh_monster_tamp != 0 && next_refresh_monster_tamp <= now_second)
	{
		next_refresh_monster_tamp = 0;
		this->OnRefreshMonster(now_second);//刷新怪物
	}

	if (!check_init && this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		check_init = true;
		this->OnRefreshMonster(now_second);//刷新怪物
	}
}

void RandAcitivityMoZuZaiNaLi::OnRefreshMonster(time_t now_second)
{
	if (this->GetStatus() != ACTIVITY_STATUS_OPEN)return;
	if (CrossConfig::Instance().IsHiddenServer())return;

	this->CleanMonster();
	m_monster_vec.clear();

	const RandActivityMoZuZaiNaLiConfig * config = static_cast<const RandActivityMoZuZaiNaLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI));
	if (NULL == config)
		return;

	const std::vector<RAMoZuZaiNaLiPosiCfg> monster_map = config->GetPosiMapCfg();
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (scene_manager != NULL)
	{
		for (int i = 0; i < (int)monster_map.size(); i++)
		{
			Scene * scene = scene_manager->GetSceneById(monster_map[i].scene_id, COMMON_SCENE_KEY);
			if (scene != NULL)
			{
				NPCMonster* boss_monster = NpcManager::Instance().CreateNPCMonster(monster_map[i].npc_id, scene, monster_map[i].pos, BATTLE_MODE_MO_ZU_ZAI_NA_LI);
				if (boss_monster != NULL)
				{
					MonsterInfo data;
					data.monster_objid = boss_monster->GetId();
					data.monster_scene = monster_map[i].scene_id;
					data.monster_seq = i;
					data.npc_seq = boss_monster->GetNpcSeq();
					m_monster_vec.push_back(data);
				}
			}
		}
	}

	next_refresh_monster_tamp = now_second + EngineAdapter::Instance().NextHourInterval(0, 0);
}

void RandAcitivityMoZuZaiNaLi::OnFightFinish(Role * role, bool is_win)
{
	if (NULL == role) return;

	const RandActivityMoZuZaiNaLiConfig * config = static_cast<const RandActivityMoZuZaiNaLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI));
	if (NULL == config)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr)
		return;

	RoleActivityMoZuZaiNaLi * role_activity = static_cast<RoleActivityMoZuZaiNaLi *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI));
	if (NULL == role_activity)
		return;

	if (role_activity->GetExpelMonsterNum() >= config->GetOtherCfg().frequency)
	{
		role->NoticeNum(errornum::EN_EXPELMONSTER_NO_REWARD_TIMES);
		return;
	}

	role_activity->OnExpelMonster();

	if (!is_win)
		return;

	const RAMoZuZaiNaLiRewardCfg * cfg = config->GetRewardCfg(ramgr);
	if (NULL == cfg)
		return;

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporaryVec(cfg->reward_item))
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(&cfg->reward_item[0], (int)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_MO_ZU_ZAI_NA_LI);
	}
}

void RandAcitivityMoZuZaiNaLi::CleanMonster()
{
	for (std::vector<MonsterInfo>::iterator monster_it = m_monster_vec.begin(); monster_it != m_monster_vec.end(); monster_it++)
	{
		NPCMonster* npc_info = NpcManager::Instance().GetNPCMonster(monster_it->npc_seq);
		if (npc_info != NULL)
		{
			Scene * monster_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(monster_it->monster_scene, COMMON_SCENE_KEY);
			if (monster_scene != NULL)
			{
				Obj* monster_obj = monster_scene->GetObj(monster_it->monster_objid);
				if (NULL != monster_obj && monster_obj->GetObjType() == Obj::OBJ_TYPE_NPC_MONSTER)
				{
					monster_scene->DeleteObj(monster_it->monster_objid);
				}
			}
		}
	}
}
