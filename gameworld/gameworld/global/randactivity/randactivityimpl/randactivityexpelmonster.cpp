#include "randactivitydiscountstore.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "randactivityexpelmonster.hpp"
#include "npc/npcmanager.h"
#include "obj/character/role.h"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityexpelmonsterconfig.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityexpelmonster.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/randactivity/randactivityadapter.hpp"

RandAcitivityExpelMonster::RandAcitivityExpelMonster(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_EXPEL_MONSTER),check_init(false), next_refresh_monster_tamp(0)
{
}

RandAcitivityExpelMonster::~RandAcitivityExpelMonster()
{
}

void RandAcitivityExpelMonster::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	if (ACTIVITY_STATUS_OPEN == to_status)
	{
		this->OnRefreshMonster(EngineAdapter::Instance().Time());
	}
	else if(ACTIVITY_STATUS_CLOSE == to_status)
	{
		next_refresh_monster_tamp = 0;
		m_monster_scene_id = 0;
		this->CleanMonster();
		m_monster_vec.clear();
	}
}

void RandAcitivityExpelMonster::Update(unsigned long interval, time_t now_second)
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

void RandAcitivityExpelMonster::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);
	if (this->GetStatus() != ACTIVITY_STATUS_CLOSE)
	{

	}
}

void RandAcitivityExpelMonster::OnRefreshMonster(time_t now_second)
{
	if (this->GetStatus() != ACTIVITY_STATUS_OPEN)return;
	if (CrossConfig::Instance().IsHiddenServer())return;
	this->CleanMonster();

	m_monster_vec.clear();

	const RandActivityExpelMonsterConfig * config = static_cast<const RandActivityExpelMonsterConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_EXPEL_MONSTER));
	if (NULL == config)
	{
		return;
	}

	const std::map<int, ExpelMonsterCoordinateCfg> & monster_map = config->GetMonsterList();
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (scene_manager != NULL)
	{
		for (std::map<int, ExpelMonsterCoordinateCfg>::const_iterator it = monster_map.begin(); it != monster_map.end(); it++)
		{
			//刷新怪物
			Scene * scene = scene_manager->GetSceneById(it->second.scene_id, COMMON_SCENE_KEY);
			if (scene != NULL)
			{
				NPCMonster* boss_monster = NpcManager::Instance().CreateNPCMonster(it->second.npc_id, scene, it->second.monster_pos, BATTLE_MODE_EXPEL_MONSTER);
				if (boss_monster != NULL)
				{
					ExpelMonsterInfo data;
					data.monster_objid = boss_monster->GetId();
					data.monster_scene = it->second.scene_id;
					data.monster_seq = it->second.seq;
					data.npc_seq = boss_monster->GetNpcSeq();
					m_monster_vec.push_back(data);
				}
			}
		}
	}

	next_refresh_monster_tamp = now_second + EngineAdapter::Instance().NextHourInterval(0, 0);
}

void RandAcitivityExpelMonster::OnFightFinish(Role * role)
{
	const RandActivityExpelMonsterConfig * config = static_cast<const RandActivityExpelMonsterConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_EXPEL_MONSTER));
	if (NULL == config)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	RoleActivityExpelMonster * role_activity = static_cast<RoleActivityExpelMonster * >
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_EXPEL_MONSTER));
	if (NULL == role_activity)
	{
		return;
	}

	const ExpelMonsterRewardCfg * reward_cfg = config->GetRewardCfg(role_activity->GetExpelRoleLevel(), ramgr);
	const ExpelMonsterOtherCfg & other_cfg = config->GetOtherCfg();
	if (reward_cfg != NULL)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg->item))return;
		if (role_activity->GetExpelMonsterNum() >= other_cfg.reward_frequency)
		{
			role->NoticeNum(errornum::EN_EXPELMONSTER_NO_REWARD_TIMES);
			return;
		}

		if (reward_cfg->hearsay == 1 && !CrossConfig::Instance().IsHiddenServer())
		{
			for (std::vector<ItemConfigData>::const_iterator it = reward_cfg->item.begin(); reward_cfg->item.end() != it; ++it)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_expelmonster_notice,
									  role->GetName(), it->item_id, RAND_ACTIVITY_TYPE_EXPEL_MONSTER);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_WORLD_CHAT)))
					{
						EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
					}
				}
			}
		}
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(reward_cfg->item, PUT_REASON_EXPEL_MONSTER);
		role_activity->OnExpelMonster();
	}
	
}

void RandAcitivityExpelMonster::CleanMonster()
{
	for (std::vector<ExpelMonsterInfo>::iterator monster_it = m_monster_vec.begin(); monster_it != m_monster_vec.end(); monster_it++)
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
