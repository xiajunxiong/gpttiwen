#include "npcmanager.h"
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "obj/character/npc_gather.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "scene/scene.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "battle/battle_manager_local.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "monster/monster_group_pool.hpp"
#include "obj/character/npc_chosed.h"
#include "global/team/team.hpp"
#include "other/salary/salary.hpp"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "monster/monster_group_pool.hpp"
#include "servercommon/noticenum.h"
#include "scene/speciallogicimpl/speciallogichongmengtianjie.hpp"
#include "other/hongmengtianjie/rolehongmengtianjie.hpp"
#include "task/taskpool.h"

NpcManager::NpcManager() : m_npc_seq_generator(MIN_NPC_SEQ)
{
}

NpcManager::~NpcManager()
{
}

NpcManager & NpcManager::Instance()
{
	static NpcManager npc_manager;
	return npc_manager;
}

void NpcManager::Update(time_t now_second)
{
}

bool NpcManager::CanTouchNpc(Role * role, int seq_id)
{
	if (NULL == role) return false;

	Scene * t_scene = role->GetScene();
	if (NULL == t_scene) return false;

	const ConstantCfg & p_sdc = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg();
	if (seq_id < MIN_NPC_SEQ)
	{
		const NPCDynamicInformaCfg * npc = NPCPOOL->GetNPCInformaCfg(seq_id);
		if (NULL == npc || npc->scene_id != t_scene->GetSceneID() || (npc->npc_posi - role->GetPos()).IsLongerThan(p_sdc.max_npc_touch_distance))
		{
			role->NoticeNum(errornum::EN_NPC_ROLE_IS_FAR_FROM_NPC);
			return false;
		}
	}
	else
	{
		NPC * npc = this->GetServerNPC(seq_id);
		if (NULL == npc || npc->GetScene()->GetSceneID() != t_scene->GetSceneID() ||
			npc->GetScene()->GetSceneKey() != t_scene->GetSceneKey() ||
			(npc->GetPos() - role->GetPos()).IsLongerThan(p_sdc.max_npc_touch_distance))
		{
			role->NoticeNum(errornum::EN_NPC_ROLE_IS_FAR_FROM_NPC);
			return false;
		}
	}
	
	return true;
}

void NpcManager::OnNPCPurchaseItem(Role * role, int seq_id, int item_id, int buy_num)
{
	if (NULL == role || buy_num <= 0 || !this->CanTouchNpc(role, seq_id)) return;
	
	const ItemBase * itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase) return;
	
	// TODO: 等待商店模块


	EventHandler::Instance().OnBuyItem(role, item_id, buy_num);
}

void NpcManager::OnNPCStartsBattle(Role * role, int seq_id, int group_id, int param)
{
	//printf("OnNPCStartsBattle seq[%d] group_id[%d]\n", seq_id, group_id);

	if (NULL == role || !this->CanTouchNpc(role, seq_id)) return;
	//printf("\nOnNpcStartBattle -> SeqID: %d | GroupID: %d\n", seq_id, group_id);
	if (seq_id < MIN_NPC_SEQ) // 打的是客户端NPC，随意了
	{
		//挑战任务只可单人,不能有队伍
		const MonsterGroupCfg* monster_group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(group_id);
		if(NULL == monster_group_cfg) return;

		if (monster_group_cfg->is_challenge_monster)
		{
			Team * team = role->GetMyTeam();
			if (NULL != team)
			{
				role->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
				return;
			}
		}

		if (monster_group_cfg->is_team_challenge_monster)
		{
			Team * team = role->GetMyTeam();
			if (NULL == team)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return;
			}

			if (team->GetFollowMemberCount() < 3)
			{
				role->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
				return;
			}
			
			//任务检查，怪物id校正
			const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(param);
			if (NULL == task_cfg)return;
			
			if (task_cfg->pre_task_id > 0)
			{
				//等级检查
				if (!team->IsAllMemberEnoughLevel(task_cfg->level_min))
				{
					return;
				}

				//全队人员任务进度校正
				if (!team->CheckAllTeamMemberTaskRecord(task_cfg->pre_task_id))
				{
					return;
				}
			}

			team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_TASK_BATTLE, BATTLE_MODE_TEAM_CHALLENGE_TASK, group_id);
			return;
		}

		BattleManagerLocal::GetInstance().StartMonsterFightReq(role, group_id, BATTLE_MODE_NORMAL_MONSTER, 0);
	}
	else
	{
		NPCMonster* npc_monster = this->GetNPCMonster(seq_id);
		if (NULL == npc_monster) return;

		if (npc_monster->IsMonsterLock())
		{
			if (!EventHandler::Instance().MonsterIsLockNotice(role, npc_monster->GetBattleMode()))
			{
				// 怪物被锁定，暂时无法打
				role->NoticeNum(errornum::EN_MONSTER_IS_LOCK);
				
			}
			return;
		}
		
		if (BATTLE_MODE_NORMAL_MONSTER == npc_monster->GetBattleMode())
		{
			BattleManagerLocal::GetInstance().StartMonsterFightReq(role, npc_monster->GetMonsterGroupId(), 
				npc_monster->GetBattleMode(), npc_monster->GetNpcSeq());
		}
		else if (npc_monster->IsNPCMineMonster())
		{
			// 这是用明怪包装的暗雷怪
			BattleManagerLocal::GetInstance().StartMineMonsterFightReq(role, false, npc_monster->GetHeadMonsterId(), false, npc_monster->GetNpcSeq());
		}
		else if (BATTLE_MODE_SHAN_HAI_BOSS == npc_monster->GetBattleMode())
		{
			BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, npc_monster->GetBattleMode(), npc_monster->GetNpcId(), npc_monster->GetNpcSeq(), 0, false);
		}
		else if (BATTLE_MODE_DU_CHUANG_ZEI_YING == npc_monster->GetBattleMode())
		{
			BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, npc_monster->GetBattleMode(), npc_monster->GetNpcId(), npc_monster->GetNpcSeq(), 0, false);
		}
		else if (BATTLE_MODE_HUNDRED_GHOST == npc_monster->GetBattleMode())
		{
			if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, npc_monster->GetBattleMode(), npc_monster->GetNpcId(), npc_monster->GetNpcSeq(), 0, false))
			{
				npc_monster->SetMonsterLock(true); // 百鬼夜行同时只能一个人打
			}
		}
		else if (BATTLE_MODE_FIELD_BOSS == npc_monster->GetBattleMode())
		{
			Team* team = role->GetMyTeam();
			if (NULL == team)
			{
				if (!EventHandler::Instance().TeamNotEnoughMembers(role, BATTLE_MODE_FIELD_BOSS))
				{
					role->NoticeNum(errornum::EN_FOLLOW_MEMBER_NOT_ENOUGH);
				}
				return;
			}
			const WildBossMonsterCfg* wild_boss_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterCfgByNpcId(npc_monster->GetNpcId());
			if (NULL == wild_boss_cfg) return;

			const WildBossItemCfg * boss_item_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetWildBossItemCfg(npc_monster->GetParam1());
			if(NULL == boss_item_cfg) return;

			int level_cfg = LOGIC_CONFIG->GetWildBossConfig()->GetLevelGroupCfg(boss_item_cfg->level_group_id);
			if (level_cfg <= 0) return;

			for (int i = 0; i < team->GetMemberCount(); ++i)
			{
				TeamMember* member = team->GetMemberInfoByIndex(i);
				if (NULL == member) continue;

				if (member->level < level_cfg / 1000)
				{
					role->NoticeNum(errornum::EN_TEAM_MEMBER_LEVL_NOT_ENOUGH);
					return;
				}
			}
			if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, npc_monster->GetBattleMode(), npc_monster->GetMonsterGroupId(), npc_monster->GetNpcSeq(), 0, true))
			{
				npc_monster->SetMonsterLock(true); // 野外BOSS同时只能一个人打
			}
		}
		else if (BATTLE_MODE_MO_ZU_ZAI_NA_LI == npc_monster->GetBattleMode())
		{
			Team * team = role->GetMyTeam();
			if (NULL != team)
			{
				role->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
				return;
			}

			if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, npc_monster->GetBattleMode(), npc_monster->GetMonsterGroupId(), npc_monster->GetNpcSeq(), 0, true))
			{
				npc_monster->SetMonsterLock(true); // 同时只能有一个人打
			}
		}
		else if (BATTLE_MODE_HONGMENG_TIANJIE == npc_monster->GetBattleMode())
		{
			SpecialLogic * special_lobgic = role->GetScene()->GetSpecialLogic();
			if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_HONGMENG_TIANJIE)
				return;
			SpecialLogicHongMengTianJie * special = (SpecialLogicHongMengTianJie *)special_lobgic;
			if (NULL == special) return;

			Team* team = role->GetMyTeam();
			if (NULL == team) return;		

			for (int i = 0; i < team->GetMemberCount(); ++i)
			{
				Role * member = team->GetMemberRoleByIndex(i);
				if (NULL == member) continue;

				if (member->GetRoleModuleManager()->GetRoleHongMengTianJie()->IsZhuZhuan(special->GetFBStage()))
				{
					member->NoticeNum(noticenum::NT_HMTJ_ZHUZHAN_NOTICE);
				}
			}

			BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, npc_monster->GetBattleMode(), npc_monster->GetMonsterGroupId(), npc_monster->GetNpcSeq(), 0, true, npc_monster->GetLevel());
		}
		else
		{
			bool ret = BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, npc_monster->GetBattleMode(),
				npc_monster->GetMonsterGroupId(), npc_monster->GetNpcSeq(), 0, true, npc_monster->GetLevel());

			if (ret && (BATTLE_MODE_EXPEL_MONSTER == npc_monster->GetBattleMode()))
			{
				npc_monster->SetMonsterLock(true); // 世界boss同时只能有一个人打
			}
		}	
	}
}

void NpcManager::OnNPCCommitChosed(Role * role, int objid, int param1, int param2)
{
	if (NULL == role)return;
	Obj * npc_obj = role->GetScene()->GetObj(objid);
	if (NULL == npc_obj)return;
	if(npc_obj->GetObjType() != Obj::OBJ_TYPE_NPC_CHOSED)return;
	NPCChosed * npc_chosed = (NPCChosed*)npc_obj;
	if (NULL == npc_chosed || !this->CanTouchNpc(role, npc_chosed->GetNpcSeq())) return;
	//printf("\nOnNPCCommitChosed -> SeqID: %d | param1: %d | param2: %d\n", npc_chosed->GetNpcSeq(), param1, param2);
	if (npc_chosed->GetNpcSeq() < MIN_NPC_SEQ) // 这类NPC只能是服务端NPC
	{
		return;
	}
	else
	{
		switch (param1)
		{
		case NPCChosed::NPC_CHOSED_REQ_TALK:
			npc_chosed->SendTalkInfoToRole(role);
			break;
		case NPCChosed::NPC_CHOSED_REQ_OPTION1:
		case NPCChosed::NPC_CHOSED_REQ_OPTION2:
			npc_chosed->ChosedOption(role,param1 -1);
			break;
		default:
			return;
		}
	}
}

NPCMonster* NpcManager::CreateNPCMonster(int npc_id, Scene* scene, const Posi& pos, int battle_mode /* = 0 */,const Dir* dir)
{
	const ServerNPCInfoCfg* cfg = NPCPOOL->GetServerNPCInfo(npc_id);
	if (NULL == cfg || SERVER_NPC_TYPE_MONSTER != cfg->server_npc_type) return NULL;

	int real_battle_mode = 0;
	int monster_group_id = 0;
	int head_monster_id = 0;

	if (npc_id != NPC_MINE_MONSTER_NPC_ID)
	{
		const MonsterGroupCfg* group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg->server_npc_param.monster_param.monster_group_id);
		if (NULL == group_cfg) return NULL;

		real_battle_mode = BATTLE_MODE_NORMAL_MONSTER;
		monster_group_id = cfg->server_npc_param.monster_param.monster_group_id;
		head_monster_id = group_cfg->head_monster_id;
	}
	else
	{
		if (!scene->HasMineMonster()) return NULL;

		real_battle_mode = BATTLE_MODE_MINE_MONSTER;
		monster_group_id = BATTLE_MINE_MONSTER_GROUP;
		head_monster_id = scene->RandHeadMineMonster();
	}

	if (battle_mode != BATTLE_MODE_INVALID)
	{
		real_battle_mode = battle_mode;
	}

	NPCMonster* monster = new NPCMonster();
	monster->SetNpcId(npc_id);
	monster->InitNPCMonster(real_battle_mode, monster_group_id, head_monster_id);
	monster->SetPos(pos);
	if(dir != NULL)
	{
		Dir real_dir = ((*dir - 180) / 180) * PI;
		monster->SetDir(real_dir);
	}
	
	scene->AddObj(monster);
	 
	return monster;
}

NPCChosed * NpcManager::CreateNPCChosed(int npc_id, Scene * scene, const Posi & pos, int battle_mode, const Dir* dir)
{
	const ServerNPCInfoCfg* cfg = NPCPOOL->GetServerNPCInfo(npc_id);
	if (NULL == cfg || SERVER_NPC_TYPE_CHOSED != cfg->server_npc_type) return NULL;

	int real_battle_mode = 0;

	if (battle_mode != BATTLE_MODE_INVALID)
	{
		real_battle_mode = battle_mode;
	}

	NPCChosed* monster = new NPCChosed();
	monster->SetNpcId(npc_id);
	monster->InitNPCChosed(real_battle_mode);
	monster->SetPos(pos);
	if (dir != NULL)
	{
		Dir real_dir = ((*dir - 180) / 180) * PI;
		monster->SetDir(real_dir);
	}
	scene->AddObj(monster);

	return monster;
}

NPCGather * NpcManager::CreateNPCGather(int npc_id, Scene * scene, const Posi & pos)
{
	const ServerNPCInfoCfg * cfg = NPCPOOL->GetServerNPCInfo(npc_id);
	if (NULL == cfg || SERVER_NPC_TYPE_GATHERS != cfg->server_npc_type) return NULL;

	const ServerNPCGatherParam & info = cfg->server_npc_param.gather_param;

	NPCGather * gather = new NPCGather();
	gather->SetNpcId(npc_id);
	gather->InitNPCGather(info.gather_time, info.award_item_count, info.gather_item_group_list);
	gather->SetPos(pos);

	scene->AddObj(gather);

	return gather;
}

NPCMonster* NpcManager::GetNPCMonster(int npc_seq)
{
	NPC* npc = this->GetServerNPC(npc_seq);
	if (NULL == npc || Obj::OBJ_TYPE_NPC_MONSTER != npc->GetObjType()) return NULL;

	return (NPCMonster*)npc;
}

int NpcManager::GetMonsterIdByNpcId(int npc_id)
{
	const ServerNPCInfoCfg* cfg = NPCPOOL->GetServerNPCInfo(npc_id);
	if (NULL == cfg || SERVER_NPC_TYPE_MONSTER != cfg->server_npc_type)
	{
		return 0;
	}

	int monster_group_id = cfg->server_npc_param.monster_param.monster_group_id;

	return monster_group_id;
}

NPC* NpcManager::GetServerNPC(int npc_seq)
{
	ServerNPCMap::iterator it = m_server_npc_map.find(npc_seq);
	if (it != m_server_npc_map.end())
	{
		return it->second;
	}

	return NULL;
}

NPCGather * NpcManager::GetGather(int npc_seq, Scene * scene)
{
	NPC * gather = NpcManager::Instance().GetServerNPC(npc_seq);
	if (NULL == gather || Obj::OBJ_TYPE_NPC_GATHERS != gather->GetObjType())
	{
		return NULL;
	}

	if (scene != NULL)
	{
		if (gather->GetScene()->GetSceneID() != scene->GetSceneID() || gather->GetScene()->GetSceneKey() != scene->GetSceneKey())
		{
			return NULL;
		}
	}

	return (NPCGather*)gather;
}

int NpcManager::GenerateNpcSeq()
{
	int ret_seq = 0;
	do
	{
		ret_seq = m_npc_seq_generator;

		m_npc_seq_generator += 1;
		if (m_npc_seq_generator >= MAX_NPC_SEQ)
		{
			m_npc_seq_generator = MIN_NPC_SEQ;
		}

	} while (this->IsSeqExist(ret_seq));

	return m_npc_seq_generator;
}

void NpcManager::RegisterServerNPC(int seq, NPC* npc)
{
#ifdef _DEBUG
	assert(seq >= MIN_NPC_SEQ);
	assert(m_server_npc_map.find(seq) == m_server_npc_map.end());
#endif
	if (m_server_npc_map.find(seq) != m_server_npc_map.end())
	{
		return;
	}
	m_server_npc_map.insert(ServerNPCMap::value_type(seq, npc));
}

void NpcManager::UnRegisterServerNPC(int seq)
{
#ifdef _DEBUG
	assert(m_server_npc_map.find(seq) != m_server_npc_map.end());
#endif
	m_server_npc_map.erase(seq);
}

bool NpcManager::IsSeqExist(int seq)
{
	return m_server_npc_map.find(seq) != m_server_npc_map.end();
}
