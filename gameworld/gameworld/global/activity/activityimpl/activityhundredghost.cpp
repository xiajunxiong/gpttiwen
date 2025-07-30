#include <algorithm>
#include "scene/scene.h"
#include "world.h"
#include "npc/npcmanager.h"
#include "obj/character/npc_monster.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "item/itempool.h"
#include "item/itembase.h"
#include "global/usercache/usercache.hpp"
#include "scene/scenemanager.hpp"
#include "obj/character/role.h"
#include "world.h"
#include "global/worldstatus/worldstatus.hpp"
#include "globalconfig/globalconfig.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/hundredghost/hundredghostconfig.hpp"
#include "protocol/msghundredghost.h"
#include "activityhundredghost.hpp"
#include "battle/battle_manager_local.hpp"
#include "other/funopen/funopendef.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "gameworld/internalcomm.h"
#include "gamelog.h"
#include "global/topicmanager/topicmanager.hpp"
#include "scene/speciallogicimpl/speciallogiccommon.hpp"

ActivityHundredGhost::ActivityHundredGhost(ActivityManager *activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_HUNDRED_GHOST)
{
	m_death_num = 0;
}

ActivityHundredGhost::~ActivityHundredGhost()
{

}

bool ActivityHundredGhost::IsNeedSave()
{
	return false;
}

void ActivityHundredGhost::Init(const ActivityData & arg_data)
{
}

void ActivityHundredGhost::GetInitParam(ActivityData & arg_data)
{
}

void ActivityHundredGhost::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	switch (to_status)
	{
	case ACTIVITY_STATUS_CLOSE:
		{
			BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_HUNDRED_GHOST, 0); // 只发给指定战斗服
			this->ClearMonster();
			this->ReSetData();
		}
		break;
	case ACTIVITY_STATUS_STANDY:
		{
		}
		break;
	case ACTIVITY_STATUS_OPEN:
		{
			this->ReSetData();
			this->CreateMonster(false);
			this->SendMonsterInfo(NULL, true);
		}
		break;
	}
}

void ActivityHundredGhost::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);
}

void ActivityHundredGhost::OnUserLogin(Role * role)
{
	Activity::OnUserLogin(role);

	if (!this->IsActivityOpen())
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendMonsterInfo(role);
	}
	else
	{
		crossgameprotocal::GameHiddenHundredGhostBossInfoReq ghhgbir;
		ghhgbir.uid = role->GetUID();
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghhgbir, sizeof(ghhgbir));
	}
}

void ActivityHundredGhost::OnUserLogout(Role * role)
{
}

void ActivityHundredGhost::OnKillMonster(int npc_seq)
{
	const HundredGhostOtherCfg& cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg();

	bool is_delete = false;
	for (std::vector<HundredGhostNpcInfo>::iterator it = m_monster_list.begin(); it != m_monster_list.end();)
	{
		if (it->npc_seq == npc_seq)
		{
			it = m_monster_list.erase(it);
			is_delete = true;
			break;
		}
		else
		{
			it++;
		}
	}

	if (is_delete)
	{
		m_death_num++;
		if (m_death_num >= cfg.max_num)
		{
			this->ForceToNextState();	// 结束活动
		}
		else
		{
			if (!this->CheckCreateMonsterBoss(HUNDRED_GHOST_MONSTER_TYPE_3))
			{
				if (cfg.max_num - m_death_num >= cfg.scene_num)		//当剩余击杀数量的鬼怪小于场景怪物数量的时候 不再刷新的小鬼
				{
					this->CreateMonster(true);
					//this->CheckCreateMonsterBoss(HUNDRED_GHOST_MONSTER_TYPE_2);
				}
			}
		}
		this->SendMonsterInfo(NULL, true);
	}
}

void ActivityHundredGhost::SendMonsterInfo(Role * role, bool send_all)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	if (!send_all)
	{
		if (NULL == role) return;
	}

	const HundredGhostOtherCfg& cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg();

	Protocol::SCHundredGhostInfo info;
	info.surplus_num = (short)(cfg.max_num - m_death_num);
	info.count = 0;
	for (std::vector<HundredGhostNpcInfo>::const_iterator it = m_monster_list.begin(); it != m_monster_list.end(); ++it)
	{
		info.boss_info[info.count].scene_id = it->scene_id;
		info.boss_info[info.count].npc_id = it->npc_id;

		info.count++;
	}

	int len = sizeof(info) - sizeof(info.boss_info[0]) * (HUNDRED_GHOST_BOSS_MAXNUM - info.count);
	if (send_all)
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&info, len);
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, len);
	}
}

void ActivityHundredGhost::HiddenSendBossInfoToGameRole(int uid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	const HundredGhostOtherCfg& cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg();

	Protocol::SCHundredGhostInfo info;
	info.surplus_num = (short)(cfg.max_num - m_death_num);
	info.count = 0;
	memset(info.boss_info, 0, sizeof(info.boss_info));

	for (std::vector<HundredGhostNpcInfo>::const_iterator it = m_monster_list.begin(); it != m_monster_list.end(); ++it)
	{
		info.boss_info[info.count].scene_id = it->scene_id;
		info.boss_info[info.count].npc_id = it->npc_id;

		info.count++;
	}

	int len = sizeof(info) - sizeof(info.boss_info[0]) * (HUNDRED_GHOST_BOSS_MAXNUM - info.count);
	InternalComm::Instance().SendToGameUserThroughCross(uid, (const char *)&info, len);
}

bool ActivityHundredGhost::CreateMonster(bool is_death)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	const HundredGhostOtherCfg& other_cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg();

	std::vector<HundredGhostMonsterCfg> monster_list;
	std::vector<Posi> pos_list;

	if (is_death)
	{
		HundredGhostMonsterCfg monster;
		LOGIC_CONFIG->GetHundredGhostConfig()->GetOneMonsterCfg(monster, m_monster_list);
		monster_list.push_back(monster);

		Posi posi;
		LOGIC_CONFIG->GetHundredGhostConfig()->GetOneMonsterPosiCfg(posi, m_monster_list);
		pos_list.push_back(posi);
	}
	else
	{
		LOGIC_CONFIG->GetHundredGhostConfig()->GetMonsterListCfg(other_cfg.scene_num - LOGIC_CONFIG->GetHundredGhostConfig()->GetBigBossNum(), monster_list);
		LOGIC_CONFIG->GetHundredGhostConfig()->GetMonsterPosiListCfg(other_cfg.scene_num - LOGIC_CONFIG->GetHundredGhostConfig()->GetBigBossNum(), pos_list);
	}

	if (monster_list.empty() || pos_list.empty() || monster_list.size() != pos_list.size())
	{
		return false;
	}

	for (int i = 0; i < (int)monster_list.size(); ++i)
	{
		Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(monster_list[i].scene_id, COMMON_SCENE_KEY);
		if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
		{
			printf("ActivityHundredGhost::Get Scene Failed scene_id[%d]\n", monster_list[i].scene_id);
			continue;
		}

		scene->GetSceneName();

		SpecialLogicCommon * special_common = (SpecialLogicCommon *)scene->GetSpecialLogic();
		if (special_common == NULL)
		{
			continue;
		}

		int npc_seq = -1;
		if (special_common->CreateHundredGhostMonster(monster_list[i].npc_id, &npc_seq, pos_list[i]))
		{
			HundredGhostNpcInfo node;
			node.npc_id = monster_list[i].npc_id;
			node.scene_id = monster_list[i].scene_id;
			node.npc_seq = npc_seq;
			node.npc_pos = pos_list[i];

			m_monster_list.push_back(node);
		}
	}

	return true;
}

bool ActivityHundredGhost::CheckCreateMonsterBoss(int type)
{
	const HundredGhostMonsterCfg* cfg = NULL;
	switch (type)
	{
	case HUNDRED_GHOST_MONSTER_TYPE_2:
		cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetMonsterBossCfg(m_death_num);
		break;
	case HUNDRED_GHOST_MONSTER_TYPE_3:
		cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetMonsterBigBossCfg(m_death_num, m_ghost_king_npc_id);
		break;
	default:
		break;
	}

	if (NULL == cfg)
	{
		return false;
	}

	Posi posi;
	LOGIC_CONFIG->GetHundredGhostConfig()->GetOneMonsterPosiCfg(posi, m_monster_list);

	Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(cfg->scene_id, COMMON_SCENE_KEY);
	if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
	{
		printf("ActivityHundredGhost::Get Scene Failed scene_id[%d]\n", cfg->scene_id);
		return false;
	}

	scene->GetSceneName();

	SpecialLogicCommon * special_common = (SpecialLogicCommon *)scene->GetSpecialLogic();
	if (special_common == NULL)
	{
		return false;
	}

	int npc_seq = -1;
	if (special_common->CreateHundredGhostMonster(cfg->npc_id, &npc_seq, posi))
	{
		HundredGhostNpcInfo temp;
		temp.npc_id = cfg->npc_id;
		temp.scene_id = cfg->scene_id;
		temp.npc_seq = npc_seq;
		temp.npc_pos = posi;

		m_monster_list.push_back(temp);

		int length = 0;
		if (type == HUNDRED_GHOST_MONSTER_TYPE_2)
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hundred_ghost_broadcast);
		}
		else if (type == HUNDRED_GHOST_MONSTER_TYPE_3)
		{
			m_ghost_king_npc_id.push_back(cfg->npc_id);
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hundred_ghost_big_boss_broadcast);
		}
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
				0, 0, 0, 0);
		}
		return true;
	}

	return false;
}

void ActivityHundredGhost::ClearMonster()
{
	for (std::vector<HundredGhostNpcInfo>::const_iterator it = m_monster_list.begin(); it != m_monster_list.end(); ++it)
	{
		Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(it->scene_id, COMMON_SCENE_KEY);
		if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
		{
			continue;
		}

		NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(it->npc_seq);
		if (NULL == npc_monster)
		{
			continue;
		}

		scene->DeleteObj(npc_monster->GetId());
	}
}

void ActivityHundredGhost::ReSetData()
{
	m_monster_list.clear();
	m_death_num = 0;
	m_ghost_king_npc_id.clear();
}
