#include "battle_manager_local.hpp"
#include "world.h"
#include "gamelog.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "servercommon/internalprotocal/battleprotocol.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/commondata.hpp"
#include "servercommon/userprotocal/msgbattle.h"
#include "scene/scene.h"
#include "scene/scenemanager.hpp"
#include "scene/speciallogic.hpp"
#include "item/itemgriddata.h"
#include "item/knapsack.h"
#include "item/itembase.h"
#include "item/itempool.h"
#include "item/expense/norexitem.hpp"
#include "item/other/otheritem.hpp"
#include "equipment/equipment.h"
#include "equipment/equipmentmanager.h"
#include "monster/monsterpool.hpp"
#include "battle/battle_business_local.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityonlyfight.hpp"
#include "global/activity/activityimpl/activityguildfight.hpp"
#include "global/activity/activityimpl/activityworldboss.hpp"
#include "global/activity/activityimpl/activityworldboss3.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"
#include "global/team/team.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "config/otherconfig/newlifeskillconfig.hpp"
#include "config/otherconfig/duchuangzeiyingconfig.hpp"
#include "config/activityconfig/worldboss/worldbossconfig.hpp"
#include "config/activityconfig/worldboss/worldboss3config.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"
#include "config/activityconfig/worldteamarena/activityworldteamarenaconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleattribute/roleattributeplan.hpp"
#include "other/taskchain/taskchain.hpp"
#include "other/pet/petconfig.hpp"
#include "other/pet/pet.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/couragechallenge/couragechallengeconfig.h"
#include "other/monster_wave/monsterwaveconfig.h"
#include "other/monster_wave/monsterwave.h"
#include "other/roleactivity/roleactivity.hpp"
#include "other/treasure_map/treasuremap.h"
#include "other/treasure_map/treasuremapconfig.h"
#include "other/salary/salary.hpp"
#include "other/escort/escort.hpp"
#include "other/escort/escortconfig.hpp"
#include "other/researchtask/researchtask.h"
#include "other/researchtask/researchtaskconfig.hpp"
#include "other/medal/medalconfig.hpp"
#include "other/medal/medal.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "servercommon/string/gameworldstr.h"
#include "scene/speciallogicimpl/speciallogicguildhonorbattle.hpp"
#include "other/funopen/funopen.hpp"
#include "monster/monster_group_pool.hpp"
#include "global/activity/activityimpl/activityworldboss2.hpp"
#include "obj/character/robot.h"
#include "npc/npcmanager.h"
#include "other/robot/robotmanager.hpp"
#include "global/team/teammanager.hpp"
#include "other/route/mailroute.hpp"
#include "other/schooltask/schooltask.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "battle/battle_video_config.hpp"
#include "battle/battle_video_manager.hpp"
#include "other/capability/capability.hpp"
#include "other/partner/partner.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "scene/speciallogicimpl/speciallogicbraveground.hpp"
#include "scene/speciallogicimpl/speciallogicmiyi.hpp"
#include "task/taskpool.h"
#include "scene/speciallogicimpl/mazeracermanager.hpp"
#include "scene/speciallogicimpl/speciallogicmiyi.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "other/auto_catch_pet/auto_catch_pet.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/funcguide/funcguideconfig.h"
#include "config/activityconfig/guildhonorbattle/activityguildhonorbattleconfig.hpp"

#include "config/otherconfig/rolematerialconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysealpromoteconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysummerlilianconfig.hpp"
#include "global/shanhaibossmanager/shanhaibossconfig.hpp"
#include "config/activityconfig/chiefelection/activitychiefelectionconfig.hpp"
#include "global/team/teammatch.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/catchfox/catchfoxconfig.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "config/otherconfig/initialbattleconfig.hpp"
#include "other/catchfox/maintaskcatchfoxconfig.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "other/formation/pethelperformation.hpp"
#include "other/netherworldpurgatory/netherworldpurgatoryconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "config/activityconfig/hundredghost/hundredghostconfig.hpp"
#include "other/duel/duel.hpp"
#include "other/duel/duelconfig.hpp"
#include "servercommon/dueldef.h"
#include "protocol/msgduel.h"
#include "config/randactivityconfig/impl/randactivityshanhaizhengfengconfig.hpp"
#include "global/team/teamconfig.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "global/colosseum/gladiator.hpp"
#include "other/rolematerial/rolematerial.hpp"
#include "qingyuan/qingyuanconfig.hpp"
#include "other/fallengod/fallengod.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/fallengod/fallengodconfig.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddieconfig.hpp"
#include "other/yaoxingshilian/yaoxingshilianconfig.hpp"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include "config/otherconfig/lilianconfig.hpp"
#include "config/otherconfig/fastforwardbattleconfig.hpp"
#include "global/cross/crossmanager.h"
#include "scene/speciallogicimpl/speciallogichongmengtianjie.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "config/otherconfig/animationspeedconfig.hpp"
#include "config/otherconfig/elementelfconfig.hpp"
#include "other/elementelf/elementelf.hpp"

BattleManagerLocal::BattleManagerLocal() : m_battle_id_generator(0), m_next_check_battle_start_failed_time(0u)
{

}

BattleManagerLocal::~BattleManagerLocal()
{

}

BattleManagerLocal* g_battle_mgr_local = NULL;
BattleManagerLocal& BattleManagerLocal::GetInstance()
{
	if (NULL == g_battle_mgr_local)
	{
		g_battle_mgr_local = new BattleManagerLocal();
	}

	return *g_battle_mgr_local;
}

void BattleManagerLocal::Update()
{
	this->CheckBattleStartFailed();
}

void BattleManagerLocal::OnUserLogin(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	if (1 == role->GetLevel() && 269 == role->GetScene()->GetSceneID()
		&& 0 == role->GetRoleModuleManager()->GetCommonData().initial_battle_flag)
	{
		role->GetRoleModuleManager()->GetCommonData().initial_battle_flag = 1;

		//this->StartInitialBattle(role);

		//没有初始战斗时要设置第一个技能为一动自动行为
		role->SetBattleAutoOnInitialFightDisabled();
		return;
	}

	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID());
	if (NULL != rbi)
	{
		battlegameprotocol::GameBattleRoleLogin msg;
		msg.role_id = role->GetUID();
		msg.bg_netid = role->GetBGNetID();
		msg.is_ob = rbi->is_ob ? 1 : 0;
		msg.reserve_sh = 0;
		World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&msg, sizeof(msg));
	}
	else
	{
		do
		{
			Team* team = role->GetMyTeam();
			if (NULL == team) break;

			Role* leader = team->GetLeaderRole();
			if (NULL == leader) break;

			if (leader->GetScene()->GetIndex() != role->GetScene()->GetIndex()
				|| leader->GetUID() == role->GetUID())
			{
				// 不在同一个场景，可能去了其他场景，以防万一就不观战了
				break;
			}

			TeamMember* member = team->GetMemberInfoByUID(role->GetUID());
			if (NULL == member || !member->is_follow)
			{
				// 如果是暂离了，也不进入观战
				break;
			}

			RoleBattleInfo* leader_rbi = this->GetRoleBattleInfo(leader->GetUID());
			if (NULL == leader_rbi) break;

			this->RoleOBReq(role, leader->GetUID());

		} while (0);
	}
}

void BattleManagerLocal::OnRoleLogout(Role* role)
{
	if (NULL == role) return;

	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID());
	if (NULL == rbi) return;

	static battlegameprotocol::GameBattleRoleLogout msg;
	msg.role_id = role->GetUID();
	msg.battle_id = rbi->battle_id;

	World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&msg, sizeof(msg));

	if (rbi->is_ob)
	{
		BattleLocalInfo* local_info = this->GetBattleLocalInfo(rbi->battle_id);
		this->OnRoleStopOB(role);
		if (NULL != local_info)
		{
			local_info->RemoveObserver(role->GetUID());
		}
		this->RemoveRoleBattleInfo(role->GetUID());
	}

	m_qie_cuo_fetch_map.erase(role->GetUID());
}

void BattleManagerLocal::OnBattleServerDisconnect(int battle_server_index)
{
	BattleLocalInfoMap::iterator info_it = m_local_info_map.begin();
	for (; info_it != m_local_info_map.end();)
	{
		if (info_it->second.battle_server_index != battle_server_index)
		{
			++info_it;
		}
		else
		{
			BattleLocalInfo& info = info_it->second;
			for (int i = 0; i < info.role_count; ++i)
			{
				int role_id = info.role_list[i];
				Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
				if (NULL != role)
				{
					this->OnRoleStopFight(role);

					static Protocol::SCBattleForceFinishFight msg;
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));

					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_battle_server_disconnect);
					if (length > 0)
					{
						Protocol::SCSystemMsg sm;
						int sendlen = 0;
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
						{
							EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
						}
					}
				}

				this->RemoveRoleBattleInfo(role_id);
			}

			std::set<int>::iterator it = info.observer_set.begin();
			for (; it != info.observer_set.end(); ++it)
			{
				RoleBattleInfo* rbi = this->GetRoleBattleInfo(*it);
				if (NULL != rbi && rbi->battle_id == info.battle_id)
				{
					Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*it));
					if (NULL != role)
					{
						this->OnRoleStopOB(role);
					}

					this->RemoveRoleBattleInfo(*it);
				}
			}

			if (NULL != info_it->second.local_impl)
			{
				info_it->second.local_impl->OnBattleInstanceDeleted();
				delete info_it->second.local_impl;
				info_it->second.local_impl = NULL;
			}

			m_local_info_map.erase(info_it++);
		}
	}
}

bool BattleManagerLocal::IsBattleServerConnected(int battle_server_index)
{
	const World::BattleInfo* info = World::GetInstWorld()->GetBattleServerInfoConst(battle_server_index);
	if (NULL == info || (NetID)-1 == info->battle_server_netid) return false;

	return true;
}

bool BattleManagerLocal::HasBattleServerConnected()
{
	return World::GetInstWorld()->HasBattleServerConnected();
}

bool BattleManagerLocal::IsRoleInBattle(int role_id)
{
	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role_id);

	return NULL != info;
}

bool BattleManagerLocal::StartSinglePVEReq(Role* role, int monster_group_id, int battle_mode, const BattleData_Business& business_data)
{
	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_1);
#endif
		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}

	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	if (role->GetRoleModuleManager()->GetEscort()->HaveEscortTask() &&
		BATTLE_MODE_ESCORT != battle_mode)
	{
		role->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	int battle_id = this->GenBattleID();

	// 判断完后 收集玩家的战斗数据
	static battlegameprotocol::GameBattleStartSinglePVEReq req;
	if (!role->GetRoleModuleManager()->GetPetHelperFormation()->CollectBattleData(battle_mode, req.role_battle_data, business_data.IsUseSystemFood()))
	{
		role->GetRoleModuleManager()->CollectRoleBattleData(&req.role_battle_data, true, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
	}
	else
	{
		req.business_data.no_sync_auto_move_setting = 1;
		req.business_data.only_sync_pet_auto_move = 1;
	}

	req.business_data.monster_list_size = 0;
	memset(req.business_data.monster_id_list, 0, sizeof(req.business_data.monster_id_list));
	memset(req.business_data.monster_level_list, 0, sizeof(req.business_data.monster_level_list));

	int animation_mode = role->GetRoleModuleManager()->GetCommonData().animation_speed; // 单人PVE默认使用角色自身速度设置
	bool is_force_speed = false;
	bool is_mine_monster = this->IsMineMonster(monster_group_id, battle_mode, business_data.is_real_mine > 0);
	this->GetRoleAnimationSpeed(role, battle_mode, is_mine_monster, false, &is_force_speed, &animation_mode);

	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();
	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = animation_mode;
	req.monster_group_id = monster_group_id;
	req.business_data = business_data;
	req.business_data.is_force_speed = is_force_speed ? 1 : 0;
	req.battle_id = battle_id;

	// 默认允许暂停，因客户端这边无论什么模式都可能出现宠物复活指引
	req.business_data.can_battle_pause_by_client = 1;

	if (monster_group_id > 0)
	{
		this->CheckRoleElementElf(role, monster_group_id, battle_mode, req.business_data);

		const VideoCfg* video_cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(monster_group_id);
		if (NULL != video_cfg && video_cfg->monster_group_id == monster_group_id)
		{
			req.business_data.is_record_battle = 1;
			const BattleVideoInfo* bvi = BattleVideoMgr::Instance().GetVideoInfoConst(monster_group_id, false);
			if (NULL != bvi)
			{
				req.business_data.expect_score_upper_bound = bvi->param.average_score;
			}
			else
			{
				req.business_data.expect_score_upper_bound = MAX_INT;
			}
		}

		if (ServerFirstKill::Instance().IsFirstKill(monster_group_id))
		{
			req.business_data.is_record_first_kill = 1;
		}
	}

	int battle_server_index = this->MakeBattleLocalInfoSingle(role, battle_id, battle_mode, &req.role_battle_data, monster_group_id, business_data.monster_gen_id);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(role, battle_id);
	}

	//gamelog::g_log_debug.printf(LL_INFO, "StartSinglePVEReq role[%d,%s] animation_speed[%d] is_force[%d]",
	//	role->GetUID(), role->GetName(), animation_mode, is_force_speed ? 1 : 0);

	return ret;
}

bool BattleManagerLocal::StartTeamPVEReq(Team* team, int monster_group_id, int battle_mode, const BattleData_Business& business_data)
{
	if (NULL == team)
	{
		return false;
	}

	struct TempCharacter
	{
		Character* character;
		bool is_role_or_robot; // false:robot true:role
	};
	TempCharacter character_list[MAX_TEAM_MEMBER_NUM];
	int character_num = 0;
	int role_num = 0;
	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };

	int team_member_num = team->GetMemberCount();
	for (int idx = 0; idx < team_member_num; ++idx)
	{
		TeamMember* member = team->GetMemberInfo(idx);
		if (NULL == member || !member->is_follow || !member->is_online) continue;

		if (!member->IsRobot())
		{
			Role* role = team->GetMemberRoleByIndex(idx);
			if (NULL != role)
			{
				character_list[character_num].character = role;
				character_list[character_num].is_role_or_robot = true;
				++character_num;
				role_list[role_num++] = role;
			}
		}
		else
		{
			Robot* robot = team->GetMemberRobotByIndex(idx);
			if (NULL != robot)
			{
				character_list[character_num].character = robot;
				character_list[character_num].is_role_or_robot = false;
				++character_num;
			}
		}
	}

	if (character_num <= 0 || role_num <= 0)
	{
		return false;
	}

	if (!this->HasBattleServerConnected())
	{
		for (int i = 0; i < character_num; ++i)
		{
			if (character_list[i].is_role_or_robot)
			{
				((Role*)character_list[i].character)->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
			}
		}

		return false;
	}

	for (int i = 0; i < character_num; ++i)
	{
		if (!character_list[i].is_role_or_robot) continue;

		Role* role = (Role*)character_list[i].character;
		const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
		if (NULL != info)
		{
			// 有人正在打， 不能开打
			return false;
		}

		if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
		{
			// 有人处于不能开打状态，不能开打
			return false;
		}
	}

	static RoleBattleData* role_data_list[BATTLE_MAX_ROLE_NUM];
	memset(role_data_list, 0, sizeof(role_data_list));

	int battle_id = this->GenBattleID();

	static battlegameprotocol::GameBattleStartTeamPVEReq req;
	req.business_data.monster_list_size = 0;
	memset(req.business_data.monster_id_list, 0, sizeof(req.business_data.monster_id_list));
	memset(req.business_data.monster_level_list, 0, sizeof(req.business_data.monster_level_list));

	int animation_mode = ANIMATION_SPEED_NORMAL;
	bool is_force_speed = true;
	Role * leader = team->GetLeaderRole();
	if (NULL != leader)
	{
		bool is_mine_monster = this->IsMineMonster(monster_group_id, battle_mode, business_data.is_real_mine > 0);
		this->GetRoleAnimationSpeed(leader, battle_mode, is_mine_monster, true, &is_force_speed, &animation_mode);
	}

	req.leader_idx = 0;
	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = animation_mode;
	req.monster_group_id = monster_group_id;
	req.battle_id = battle_id;

	req.business_data = business_data;
	req.business_data.is_force_speed = is_force_speed ? 1 : 0;
	req.role_num = character_num;

	int temp_role_idx = 0;
	for (int i = 0; i < character_num; ++i)
	{
		if (character_list[i].is_role_or_robot)
		{
			// 收集玩家的战斗数据
			Role* role = (Role*)character_list[i].character;
			req.member_battle_data[i].bg_netid = role->GetBGNetID();
			req.member_battle_data[i].uid = role->GetUID();

			bool is_leader = false;
			if (role->GetUID() == team->GetLeaderUID())
			{
				req.leader_idx = i;
				is_leader = true;
			}

			role->GetRoleModuleManager()->CollectRoleBattleData(&req.member_battle_data[i].role_battle_data, is_leader, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
			role_data_list[temp_role_idx++] = &req.member_battle_data[i].role_battle_data;
		}
		else
		{
			// 收集机器人的战斗数据
			Robot* robot = (Robot*)character_list[i].character;
			req.member_battle_data[i].bg_netid = INVALID_BGNETID;
			req.member_battle_data[i].uid = robot->GetRobotID();
			req.member_battle_data[i].role_battle_data = robot->GetRoleBattleData();

			bool is_robot_runaway_before_start = this->IsRobotRunAwayBeforeStart(robot, leader, battle_mode, monster_group_id, req.business_data);
			if (is_robot_runaway_before_start)
			{
				req.business_data.robot_run_away_before_start = 1;
			}
		}
	}

	memset(req.pos_id_list, 0, sizeof(req.pos_id_list));
	bool is_need_team_stand_pos = IsBattleModeNeedTeamStandPos(battle_mode);
	if (is_need_team_stand_pos)
	{
		team->GetPosIdList(req.pos_id_list, ARRAY_ITEM_COUNT(req.pos_id_list), true);
	}

	if (monster_group_id > 0)
	{
		this->CheckTeamElementElf(team, monster_group_id, battle_mode, req.business_data);

		const VideoCfg* video_cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(monster_group_id);
		if (NULL != video_cfg && video_cfg->monster_group_id == monster_group_id)
		{
			req.business_data.is_record_battle = 1;
			const BattleVideoInfo* bvi = BattleVideoMgr::Instance().GetVideoInfoConst(monster_group_id, false);
			if (NULL != bvi)
			{
				req.business_data.expect_score_upper_bound = bvi->param.average_score;
			}
			else
			{
				req.business_data.expect_score_upper_bound = MAX_INT;
			}
		}

		if (ServerFirstKill::Instance().IsFirstKill(monster_group_id))
		{
			req.business_data.is_record_first_kill = 1;
		}
	}

	int battle_server_index = this->MakeBattleLocalInfoMulti(role_num, role_list, battle_id, battle_mode, role_data_list, monster_group_id, business_data.monster_gen_id);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		for (int i = 0; i < role_num; ++i)
		{
			this->OnRoleStartFigntSpecialDeal(role_list[i], monster_group_id);
			this->OnRoleStartFight(role_list[i], battle_id);
		}
	}

	//if (1 == character_num && leader != NULL)
	//{
	//	gamelog::g_log_debug.printf(LL_INFO, "StartSinglePVEReq role[%d,%s] animation_speed[%d] is_force[%d]",
	//		leader->GetUID(), leader->GetName(), animation_mode, is_force_speed ? 1 : 0);
	//}

	return ret;
}

bool BattleManagerLocal::PrepareFinishReq(Role* role)
{
	static battlegameprotocol::GameBattlePrepareFinishReq req;
	req.uid = role->GetUID();
	req.bg_netid = role->GetBGNetID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	const RoleBattleInfo* rbi = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL == rbi)
	{
		this->CheckRoleInBigCrossBattle(role);
		return false;
	}

	if (!this->IsBattleServerConnected(rbi->battle_server_index))
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&req, sizeof(req));

	return ret;
}

bool BattleManagerLocal::AnimationFinishReq(Role* role)
{
	static battlegameprotocol::GameBattleAnimationFinishReq req;
	req.uid = role->GetUID();
	req.bg_netid = role->GetBGNetID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	const RoleBattleInfo* rbi = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL == rbi)
	{
		this->CheckRoleInBigCrossBattle(role);
		return false;
	}

	if (!this->IsBattleServerConnected(rbi->battle_server_index))
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&req, sizeof(req));

	return ret;
}

bool BattleManagerLocal::CountDownFinishReq(Role* role)
{
	static battlegameprotocol::GameBattleCountDownFinishReq req;
	req.uid = role->GetUID();
	req.bg_netid = role->GetBGNetID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL == info)
	{
		this->CheckRoleInBigCrossBattle(role);
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return false;
	}

	if (!this->IsBattleServerConnected(info->battle_server_index))
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(info->battle_server_index, (const char*)&req, sizeof(req));

	return ret;
}

bool BattleManagerLocal::ForceFinishFightReq(Role* role)
{
#ifndef _DEBUG      // 线上不会开启这个功能
	if (!role->IsGM() && !role->IsFromDev())
	{
		return false;
	}
#endif

	static battlegameprotocol::GameBattleForceFinishFight req;
	req.uid = role->GetUID();
	req.bg_netid = role->GetBGNetID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL == info)
	{
		this->CheckRoleInBigCrossBattle(role);
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return false;
	}

	if (!this->IsBattleServerConnected(info->battle_server_index))
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(info->battle_server_index, (const char*)&req, sizeof(req));

	return ret;
}

bool BattleManagerLocal::ForceFinishFightReq2(Role * role, bool is_notice)
{
	static battlegameprotocol::GameBattleForceFinishFight req;
	req.uid = role->GetUID();
	req.bg_netid = role->GetBGNetID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	const RoleBattleInfo * info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL == info)
	{
		if (is_notice)
			role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);

		this->CheckRoleInBigCrossBattle(role);
		return false;
	}

	if (!this->IsBattleServerConnected(info->battle_server_index))
	{
		if (is_notice)
			role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(info->battle_server_index, (const char *)&req, sizeof(req));

	return ret;
}

void BattleManagerLocal::ForceFinishBattleMode(int mode, int param1 /* = 0 */, int specify_battle_idx /* = -1 */)
{
	battlegameprotocol::GameBattleForceFinishBattleModeReq ffbmr;
	ffbmr.battle_mode = mode;
	ffbmr.param1 = param1;
	ffbmr.param2 = 0;
	ffbmr.param3 = 0;

	if (-1 == specify_battle_idx)
	{
		World::GetInstWorld()->SendMsgToAllBattle((const char *)&ffbmr, sizeof(ffbmr));
	}
	else
	{
		World::GetInstWorld()->SendMsgToOneBattle(specify_battle_idx, (const char *)&ffbmr, sizeof(ffbmr));
	}
}

void BattleManagerLocal::BattleGiveupReq(Role* role)
{
	battlegameprotocol::GameBattleBattleGiveup gg;
	gg.bg_netid = role->GetBGNetID();
	gg.uid = role->GetUID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&gg, sizeof(gg));
		return;
	}

	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID());
	if (NULL == rbi)
	{
		this->CheckRoleInBigCrossBattle(role);
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return;
	}

	if (!this->IsBattleServerConnected(rbi->battle_server_index))
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
	}

	World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&gg, sizeof(gg));
}

void BattleManagerLocal::BattleSurrenderReq(Role* role)
{
	battlegameprotocol::GameBattleBattleSurrender gg;
	gg.bg_netid = role->GetBGNetID();
	gg.uid = role->GetUID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&gg, sizeof(gg));
		return;
	}

	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID());
	if (NULL == rbi)
	{
		this->CheckRoleInBigCrossBattle(role);
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return;
	}

	if (!this->IsBattleServerConnected(rbi->battle_server_index))
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
	}

	World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&gg, sizeof(gg));
}

bool BattleManagerLocal::RoleOperateReq(Role* role, int operate_type, int param1, int param2, int param3, int param4)
{
	static battlegameprotocol::GameBattleRoleOperate op;
	op.uid = role->GetUID();
	op.bg_netid = role->GetBGNetID();
	op.operate_type = operate_type;
	op.param1 = param1;
	op.param2 = param2;
	op.param3 = param3;
	op.param4 = param4;

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_ITEM == operate_type)
		{
			return this->RoleUseItemReq(-1, role, param1, param2, param3, param4);
		}
		else if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_SEAL_PET == operate_type)
		{
			return this->RoleSealPetReq(-1, role, param1, param2, param3, param4);
		}
		else if (BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_CHANGE_CARD_SKILL == operate_type)
		{
			if (!role->GetRoleModuleManager()->GetRoleNewLifeSkill()->IsCanPerformSkillByBattle())
			{
				return false;
			}
		}

		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&op, sizeof(op));
		return true;
	}

	if (!this->IsRoleInBattle(role->GetUID()))
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_NOT_IN_BATTLE);
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
#endif
		this->CheckRoleInBigCrossBattle(role);
		return false;
	}

	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID());
	if (NULL == rbi)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		this->CheckRoleInBigCrossBattle(role);
		return false;
	}

	if (!this->IsBattleServerConnected(rbi->battle_server_index))
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_BATTLE_SERVER_NOT_CONNECT);
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	if (operate_type <= BATTLE_ROLE_OPERATE_TYPE_INVALID || operate_type >= BATTLE_ROLE_OPERATE_TYPE_MAX)
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_INPUT_PARAM_ERROR);
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	//有些operate需要先在本服做完，再同步到战斗服（如换装备、吃药）则特殊处理 
	if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_ITEM == operate_type)
	{
		return this->RoleUseItemReq(rbi->battle_server_index, role, param1, param2, param3, param4);
	}
	else if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_SEAL_PET == operate_type)
	{
		return this->RoleSealPetReq(rbi->battle_server_index, role, param1, param2, param3, param4);
	}
	else if (BTALLE_ROLE_OPERATE_TYPE_SET_MOVE_PERFORM_CHANGE_CARD_SKILL == operate_type)
	{
		if (!role->GetRoleModuleManager()->GetRoleNewLifeSkill()->IsCanPerformSkillByBattle())
		{
			return false;
		}
	}
	//else if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_INST_KILL_ITEM == operate_type)
	//{
	//	return this->RoleInstKillMonsterReq(rbi->battle_server_index, role, param1, param2, param3);
	//}

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&op, sizeof(op));

	return ret;
}



bool BattleManagerLocal::RoleOBReq(Role* role, int watch_uid, bool is_forbid_quit_ob /* = false */)
{
	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID(), false);
	if (NULL != rbi)
	{
		if (rbi->is_ob)
		{
			role->NoticeNum(errornum::EN_ROLE_IS_OB_ING);
			return false;
		}
		else
		{
			role->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
			return false;
		}
	}

	RoleBattleInfo* info = this->GetRoleBattleInfo(watch_uid, false);
	if (NULL == info)
	{
		role->NoticeNum(errornum::EN_TARGET_NOT_IN_BATTLE);
		return false;
	}

	if (info->is_ob)
	{
		role->NoticeNum(errornum::EN_TARGET_IS_OB_ING);
		return false;
	}

	if (role->GetRoleModuleManager()->GetRoleCross()->IsPreparingForCross())
	{
		role->NoticeNum(errornum::EN_IN_PREPARING_CROSS_STATUS_OB_LIMIT);
		role->GetRoleModuleManager()->GetRoleCross()->ResetIsPreparingForCross();
		return false;
	}

	// 进入观战前退出匹配
	TeamMatch::Instance().OnUserStopMatch(role->GetUniqueUserID());

	static battlegameprotocol::GameBattleStartOBReq req;
	req.battle_id = info->battle_id;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();
	req.watch_uid = watch_uid;
	req.forbid_quit_ob_flag = is_forbid_quit_ob ? 1 : 0;

	return World::GetInstWorld()->SendMsgToOneBattle(info->battle_server_index, (const char*)&req, sizeof(req));
}

bool BattleManagerLocal::RoleOBSideReq(Role* role, int battle_id, short watch_side)
{
	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID(), false);
	if (NULL != rbi)
	{
		if (rbi->is_ob)
		{
			role->NoticeNum(errornum::EN_ROLE_IS_OB_ING);
			return false;
		}
		else
		{
			role->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
			return false;
		}
	}

	BattleLocalInfo* bli = this->GetBattleLocalInfo(battle_id);
	if (NULL == bli)
	{
		role->NoticeNum(errornum::EN_BATTLE_NOT_FOUND);
		return false;
	}

	// 进入观战前退出匹配
	TeamMatch::Instance().OnUserStopMatch(role->GetUniqueUserID());

	static battlegameprotocol::GameBattleStartOBSideReq req;
	req.battle_id = battle_id;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();
	req.watch_side = watch_side;
	req.reserve_sh = 0;

	return World::GetInstWorld()->SendMsgToOneBattle(bli->battle_server_index, (const char*)&req, sizeof(req));
}

bool BattleManagerLocal::RoleBattleRealTimeCommentReq(Role* role, const char* msg, unsigned int length)
{
	static battlegameprotocol::GameBattleRoleRealTimeComment req;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();
	req.msg_length = length;
	memcpy(req.msg_content, msg, length);
	int send_length = sizeof(req) - sizeof(req.msg_content) + req.msg_length;

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, send_length);
		return true;
	}

	RoleBattleInfo* info = this->GetRoleBattleInfo(role->GetUID(), false);
	if (NULL == info)
	{
		this->CheckRoleInBigCrossBattle(role);
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return false;
	}

	if (length >= REAL_TIME_COMMENT_LEN)
	{
		return false;
	}

	return World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&req, send_length);
}

bool BattleManagerLocal::RoleModifyAnimationSpeedReq(Role* role, int speed_type)
{
	battlegameprotocol::GameBattleModifyAnimationSpeedReq req;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();
	req.speed_type = speed_type;

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	RoleBattleInfo* info = this->GetRoleBattleInfo(role->GetUID(), false);
	if (NULL == info)
	{
		this->CheckRoleInBigCrossBattle(role);
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return false;
	}

	if (speed_type < 0 || speed_type >= ANIMATION_SPEED_MAX)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	return World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&req, sizeof(req));
}

bool BattleManagerLocal::RoleSetCustomInstructionReq(Role* role, const char* istr, short grid_idx)
{
	battlegameprotocol::GameBattleSetCustomInstruction req;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();
	req.grid_idx = grid_idx;
	req.reserve_sh = 0;
	F_STRNCPY(req.istr, istr, sizeof(req.istr));

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	RoleBattleInfo* info = this->GetRoleBattleInfo(role->GetUID(), false);
	if (NULL == info)
	{
		this->CheckRoleInBigCrossBattle(role);
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return false;
	}

	return World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&req, sizeof(req));
}

bool BattleManagerLocal::RoleSkipBattleReq(Role* role)
{
	battlegameprotocol::GameBattleSkipBattleReq req;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	return World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&req, sizeof(req));
}

bool BattleManagerLocal::RoleFastForwardBattleReq(Role* role)
{
	battlegameprotocol::GameBattleFastForwardBattleReq req;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();

	if (this->IsRoleBattleRequestSendToBigCross(role))
	{
		this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
		return true;
	}

	return World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&req, sizeof(req));
}

bool BattleManagerLocal::JoySeeksJavorsOperateReq(Role * role, int operate_type, int param1, int param2, int param3, int param4, int param5)
{
	if (!this->IsRoleInBattle(role->GetUID()))
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_NOT_IN_BATTLE);
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
#endif
		return false;
	}

	RoleBattleInfo* rbi = this->GetRoleBattleInfo(role->GetUID());
	if (NULL == rbi)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_BATTLE);
		return false;
	}

	if (!this->IsBattleServerConnected(rbi->battle_server_index))
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_BATTLE_SERVER_NOT_CONNECT);
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	if (operate_type <= BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_INVALID || operate_type >= BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_MAX)
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_INPUT_PARAM_ERROR);
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	//有些operate需要先在本服做完，再同步到战斗服（如换装备、吃药）则特殊处理 
	/*if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_ITEM == operate_type)
	{
		return this->RoleUseItemReq(rbi->battle_server_index, role, param1, param2, param3, param4);
	}
	else if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_SEAL_PET == operate_type)
	{
		return this->RoleSealPetReq(rbi->battle_server_index, role, param1, param2, param3, param4);
	}*/
	//else if (BATTLE_ROLE_OPERATE_TYPE_SET_MOVE_USE_INST_KILL_ITEM == operate_type)
	//{
	//	return this->RoleInstKillMonsterReq(rbi->battle_server_index, role, param1, param2, param3);
	//}

	static battlegameprotocol::GameBattleJoySeeksJavorsOperate op;
	op.uid = role->GetUID();
	op.bg_netid = role->GetBGNetID();
	op.operate_type = operate_type;
	op.param1 = param1;
	op.param2 = param2;
	op.param3 = param3;
	op.param4 = param4;
	op.param5 = param5;

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(rbi->battle_server_index, (const char*)&op, sizeof(op));

	return ret;
}

void BattleManagerLocal::OnFightStart(battlegameprotocol::BattleGameStartFightAck* ack)
{
	//for (int i = 0; i < ack->role_count; ++i)
	//{
	//	int role_id = ack->role_list[i].uid;
	//	this->OnRoleStartFight(role_id, ack->battle_id);
	//}



	BattleLocalInfoMap::iterator it = m_local_info_map.find(ack->battle_id);
	if (it != m_local_info_map.end())
	{
		it->second.is_battle_start = true;

		if (NULL != it->second.local_impl)
		{
			it->second.local_impl->OnBattleStart(ack);
		}

		World::GetInstWorld()->ModifyBattleIndexHandleNum(it->second.battle_server_index, 1);
	}

	this->CheckTeamNoticeDoubleSpeed(ack);
}

bool BattleManagerLocal::StartDemoFightReq(Role* role, int npc_demo_seq /* = 0 */, int battle_mode /* = BATTLE_MODE_DEMO */, BattleData_Business* in_business_data /* = NULL */)
{
	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_1);
#endif
		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}

	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	int battle_id = this->GenBattleID();

	// 判断完后
	static battlegameprotocol::GameBattleDemoStartReq req;
	req.bg_netid = role->GetBGNetID();
	req.uid = role->GetUID();
	req.battle_id = battle_id;
	req.demo_seq = npc_demo_seq;
	req.battle_mode = battle_mode;
	req.avatar_type = role->GetAvatarType();

	req.is_auto = role->GetRoleModuleManager()->GetCommonData().battle_is_auto;
	req.auto_first_move_type = role->GetRoleModuleManager()->GetCommonData().battle_auto_first_move_type;
	req.auto_first_skill_id = role->GetRoleModuleManager()->GetCommonData().battle_auto_first_skill_id;
	req.auto_first_skill_level = role->GetRoleModuleManager()->GetCommonData().battle_auto_first_skill_level;
	req.auto_second_move_type = role->GetRoleModuleManager()->GetCommonData().battle_auto_second_move_type;
	req.auto_second_skill_id = role->GetRoleModuleManager()->GetCommonData().battle_auto_second_skill_id;
	req.auto_second_skill_level = role->GetRoleModuleManager()->GetCommonData().battle_auto_second_skill_level;

	static RoleBattleData role_battle_data;
	int battle_server_index = this->MakeBattleLocalInfoSingle(role, battle_id, battle_mode, &role_battle_data, 0, 0);

	req.business_data.Reset();
	if (NULL != in_business_data)
	{
		req.business_data = *in_business_data;
	}

	int fast_forward_round = 0;
	if (LOGIC_CONFIG->GetFastForwardBattleConfig()->GetFastBattleRound(role->GetLevel(), battle_mode, &fast_forward_round))
	{
		req.business_data.allow_fast_forward_round_num = static_cast<short>(fast_forward_round);
	}

	this->MarkBattleLaunchInfo(req.business_data, role, __FUNCTION__);

	bool ret = World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(role, battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);

	return ret;
}

bool BattleManagerLocal::StartMonsterFightReq(Role* role, int monster_group_id, int battle_mode, int monster_gen_id)
{
	static BattleData_Business business_data;
	business_data.Reset();

	business_data.monster_group_id = monster_group_id;
	business_data.monster_gen_id = monster_gen_id;

	this->MarkBattleLaunchInfo(business_data, role, __FUNCTION__);

	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		return this->StartTeamPVEReq(role->GetMyTeam(), monster_group_id, battle_mode, business_data);
	}
	else if (NULL == team || !team->IsRoleMemberFollow(role->GetUID())) // 不在队伍中或暂离
	{
		business_data.allow_fast_forward_round_num = MonsterGroupPool::GetInstance().GetFastForwardRoundByMonsterGroup(monster_group_id, role->GetLevel());

		return this->StartSinglePVEReq(role, monster_group_id, battle_mode, business_data);
	}
	else
	{
		return false;
	}
}

bool BattleManagerLocal::StartTeamMonsterFightReq(Role* role, int monster_group_id, int battle_mode, int monster_gen_id)
{
	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return false;
	}

	if (team->GetLeaderUID() != role->GetUID())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return false;
	}

	static BattleData_Business business_data;
	business_data.Reset();

	business_data.team_idx = team->GetTeamIndex();
	business_data.monster_group_id = monster_group_id;
	business_data.monster_gen_id = monster_gen_id;

	this->MarkBattleLaunchInfo(business_data, role, __FUNCTION__);

	return this->StartTeamPVEReq(team, monster_group_id, battle_mode, business_data);
}

bool BattleManagerLocal::StartBattleModeFightReq(Role * role, short battle_mode, int param1, int param2, int param3, bool is_from_server_req, int use_level)
{
	// is_from_server_req 打世界BOSS只能从其他请求中转进来， 防止发包直接打BOSS

	if (NULL == role) return false;
	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_2);
#endif
		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}

	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	bool is_in_team = role->InTeam();
	bool is_follow = role->IsFollow();
	bool is_leader = role->IsLeader();
	short least_follow_member_num = 1, min_level_limit = 1, least_real_player_member_num = 1;
	if (IsBattleModeMustInTeam(battle_mode, &least_follow_member_num, &min_level_limit, &least_real_player_member_num) && !is_in_team)
	{
		if (!EventHandler::Instance().NotInTeam(role, battle_mode))
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		}

		return false;
	}

	if (IsBattleModeMustNotInTeam(battle_mode) && is_in_team)
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return false;
	}

	static BattleData_Business business_data;
	business_data.Reset();

	this->MarkBattleLaunchInfo(business_data, role, __FUNCTION__);

	int monster_group_id = 0;
	if (is_from_server_req)
	{
		business_data.monster_group_id = param1;
		business_data.monster_gen_id = param2;
		monster_group_id = business_data.monster_group_id;
	}

	if (use_level != 0)
	{
		business_data.is_use_level_change = 1;
		business_data.use_level = use_level;
	}

	business_data.attacker_sidebuff_id = this->GetBattleModeSceneBuff(role, battle_mode);

	int fast_forward_rounds = 0;
	if (LOGIC_CONFIG->GetFastForwardBattleConfig()->GetFastBattleRound(role->GetLevel(), battle_mode, &fast_forward_rounds))
	{
		business_data.allow_fast_forward_round_num = fast_forward_rounds;
	}

	//装配business_data以及获取monster_group_id
	switch (battle_mode)
	{
	case BATTLE_MODE_TASK_ROLE_MONSTER:
	{
		business_data.param1 = param2;
		business_data.monster_group_id = param1;
		monster_group_id = business_data.monster_group_id;
	}
	break;
	case BATTLE_MODE_TEAM_CHALLENGE_TASK:
	case BATTLE_MODE_SPECIAL_ANECDOTE_TASK:
	{
		business_data.param1 = param1;
		business_data.param2 = param2;
		business_data.param3 = param3;
		business_data.monster_group_id = param1;
		monster_group_id = business_data.monster_group_id;
	}
	break;
	case BATTLE_MODE_KE_JU_EXAM:
	{
		Team * team = role->GetMyTeam();
		if (team == NULL)
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
			return false;
		}

		business_data.monster_group_id = param1;
		business_data.param1 = param1;
		business_data.param2 = param2;
		business_data.param3 = param3;
		monster_group_id = business_data.monster_group_id;
		business_data.is_use_level_change = 1;
		business_data.use_level = team->GetFollowMemberMaxLevel();
	}
	break;
	case BATTLE_MODE_GOTO_SCHOOL:
	{
		business_data.monster_group_id = param1;
		monster_group_id = business_data.monster_group_id;
		business_data.use_level = role->GetLevel();
	}
	break;
	case BATTLE_MODE_HONGMENG_TIANJIE:
		{
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			business_data.monster_group_id = param1;
			monster_group_id = business_data.monster_group_id;

			Scene * scene = role->GetScene();
			if (NULL == scene || Scene::SCENE_TYPE_HONGMENG_TIANJIE != scene->GetSceneType())
			{
				return false;
			}
			SpecialLogicHongMengTianJie * fb_logic = (SpecialLogicHongMengTianJie *)scene->GetSpecialLogic();
			if (NULL == fb_logic)
			{
				return false;
			}

			business_data.monster_attr_weaken_percent = fb_logic->GetDifficulty();
		}
		break;
	case BATTLE_MODE_LANTERN_RIDDIR:
		{
			Team * team = role->GetMyTeam();
			if (team == NULL)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			business_data.monster_group_id = param1;
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			monster_group_id = business_data.monster_group_id;
			business_data.is_use_level_change = 1;
			business_data.use_level = team->GetFollowMemberMaxLevel();
		}
		break;
	case BATTLE_MODE_FALLEN_GOD:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_FALLEN_GOD, true))
			{
				return false;
			}

			Team * team = role->GetMyTeam();
			if (team == NULL)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			if (!role->IsLeader())
			{
				return false;
			}

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			monster_group_id = business_data.monster_group_id;
			business_data.is_use_level_change = 1;
			Team * m_team = role->GetMyTeam();
			if (m_team == NULL)return false;
			business_data.use_level = m_team->GetFollowMemberMaxLevel();
			const FallenGodOtherCfg & other_cfg = LOGIC_CONFIG->GetFallenGodConfig()->GetOtherCfg();
			if (other_cfg.last_times)
			{
				business_data.limit_max_round = other_cfg.last_times;
			}
		}
		break;
	case BATTLE_MODE_WEDDING_FB_BAO_WEI_XIU_QIU:
		{
			SpecialLogic * special_logic = role->GetScene()->GetSpecialLogic();
			if (NULL == special_logic || special_logic->GetSceneType() != Scene::SCENE_TYPE_WEDDING_FB)
			{
				return false;
			}

			//配置
			const WeddingFB::SubActBaoWeiXiuQiu * reward_cfg
				= LOGIC_CONFIG->GetQingYuanConfig()->GetSubActBaoWeiXiuQiu(param1);
			if (NULL == reward_cfg)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}

			business_data.param1 = reward_cfg->npc_id;
			business_data.param2 = reward_cfg->reward_index;
			business_data.param3 = reward_cfg->monster_group_id;
			business_data.monster_group_id = reward_cfg->monster_group_id;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_WEDDING_FB:
		{
			return false;
		}
		break;
	case BATTLE_MODE_WAN_LING_FB:
		{
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			business_data.monster_group_id = param1;
			monster_group_id = business_data.monster_group_id;

			Scene * scene = role->GetScene();
			if (NULL == scene || Scene::SCENE_TYPE_WAN_LING_FB != scene->GetSceneType())
			{
				return false;
			}
			SpecialLogicWanLingFB * fb_logic = (SpecialLogicWanLingFB *)scene->GetSpecialLogic();
			if (NULL == fb_logic)
			{
				return false;
			}
			fb_logic->FightStart(role, monster_group_id);

		}
		break;
	case BATTLE_MODE_SHAN_HAI_ZHENG_FENG:
		{
			const RandActivityShanHaiZhengFengConfig * config = static_cast<const RandActivityShanHaiZhengFengConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG));
			if (NULL == config) return false;

			const RAShanHaiZhengFengOtherCfg other_cfg = config->GetOtherCfg();
			if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other_cfg.consume_item_id, other_cfg.consume_num, __FUNCTION__))
			{
				role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return false;
			}

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param3;
		}
		break;
	case BATTLE_MODE_ZAI_ZHAN_YUN_ZE:
		{
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			business_data.monster_group_id = param1;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_ENDLESS_TOWER:
	case BATTLE_MODE_YAO_XING_SHI_LIAN:
		{
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			business_data.monster_group_id = param1;
			monster_group_id = business_data.monster_group_id;
			const YaoXingShiLianOtherCfg other_cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetOhterCfg();
			business_data.limit_max_round = other_cfg.max_rounds;
		}
		break;
	case BATTLE_MODE_DU_JIE:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_DU_JIE, true))
			{
				return false;
			}

			if (is_from_server_req)
			{//组队则必须是队长
				if (role->InTeam() && !role->IsLeader())
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
					return false;
				}
			}
			else
			{
				if (role->InTeam())
				{
					role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
					return false;
				}
			}

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_BIG_DIPPER:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_BIG_DIPPER, true))
			{
				return false;
			}

			Team * team = role->GetMyTeam();
			if (team == NULL)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			if (!role->IsLeader())
			{
				return false;
			}

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			monster_group_id = business_data.monster_group_id;
			business_data.is_use_level_change = 1;
			Team * m_team = role->GetMyTeam();
			if (m_team == NULL)return false;
			business_data.use_level = m_team->GetFollowMemberMaxLevel();
		}
		break;
	case BATTLE_MODE_PET_GOD_FB:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_PET_GOD_FB, true))
			{
				return false;
			}

			SpecialLogic * special_logic = role->GetScene()->GetSpecialLogic();
			if (NULL == special_logic || special_logic->GetSceneType() != Scene::SCENE_TYPE_PET_GOD_FB)
			{
				return false;
			}

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_NETHER_WORLD_PURGATORY:
		{
			business_data.param1 = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			business_data.monster_group_id = param1;
			monster_group_id = business_data.monster_group_id;
		
		{
			NetherWorldPurgatory * role_nether = role->GetRoleModuleManager()->GetNetherWorldPurgatory();
			if (NULL != role_nether)
			{
				bool challenge_top = false;
				if (param2 > role_nether->GetChallengeChapter())
				{
					if (param2 == role_nether->GetChallengeChapter() + 1)
					{
						challenge_top = true;
					}
				}
				else if (param2 == role_nether->GetChallengeChapter())
				{
					if (param3 == role_nether->GetChallengeId() + 1)
					{
						challenge_top = true;
					}
				}
				if (challenge_top)
				{
					//挑战当前最高关卡，查询是否有易损debuff
					int buff_level = role_nether->GetDeBuffLevel();
					if (buff_level > 0)
					{
						const NetherWorldPurgatoryOtherCfg & other_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetOtherCfg();
						business_data.defender_buff_id = other_cfg.debuff_id;
						business_data.defender_buff_lv = buff_level;
					}
				}
			}
		}
		const NetherWorldPurgatoryMonsterCfg * challenge_cfg = LOGIC_CONFIG->GetNetherWorldPurgatoryConfig()->GetMonsterConditionCfg(param2, param3, business_data.monster_group_id);
			if (NULL != challenge_cfg)
			{
			for (int i = 0; i < MAX_NETHER_WORLD_GATORY_TASK_NUM - 1 && i < BATTLE_CONDITION_MAX_NUM; i++)
				{
					business_data.condition_list[i] = (unsigned char)challenge_cfg->condition_id[i];
					business_data.condition_num++;
				}
			}
		}
		break;
	case BATTLE_MODE_MATERIAL:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_MATERIAL, true))
			{
				return false;
			}

			SpecialLogic * special_logic = role->GetScene()->GetSpecialLogic();
			if (NULL == special_logic || special_logic->GetSceneType() != Scene::SCENE_TYPE_MATERIAL_FB)
			{
				return false;
			}
			SpecialLogicMaterial * material_logic = (SpecialLogicMaterial *)special_logic;
			const MaterialSceneCfg * scene_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetSceneCfg(material_logic->GetChapterId(), material_logic->GetLevelSeq());
			if (scene_cfg == NULL) return false;

			role->GetRoleModuleManager()->GetRoleMaterial()->IsCanTips();

			business_data.condition_num = 0;
			business_data.condition_flag = 0;
			for (int i = 0; i < MAX_MATERIAL_CONDITON_NUM + MAX_MATERIAL_ACHIEVENMENT_CONDITION_NUM && i < BATTLE_CONDITION_MAX_NUM; i++)
			{
				if (i < MAX_MATERIAL_CONDITON_NUM)
				{
					business_data.condition_list[i] = scene_cfg->condition[i];
				}
				else
				{
					break;
					//策划说去除成就奖励
					//int index = i - MAX_MATERIAL_CONDITON_NUM;
					//business_data.condition_list[i] = scene_cfg->achievement_condition[index];
				}
				business_data.condition_num++;
			}

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_LUN_HUI_WANG_CHUAN:
		{
			if (!is_from_server_req) return false;

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.is_use_level_change = 1;
			business_data.use_level = role->GetRoleModuleManager()->GetLunHuiWangChuan()->GetLevel();
			business_data.param1 = role->GetRoleModuleManager()->GetLunHuiWangChuan()->GetPassIDX();
		}
		break;
	case BATTLE_MODE_REALM_BREAK:
		{
			Team * m_team = role->GetMyTeam();
			if (NULL != m_team)
			{
				role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
				return false;
			}
			if (!Scene::IsStaticScene(role->GetScene()->GetSceneType()))
			{
				role->NoticeNum(errornum::EN_REALM_BREAK_SCENE_TYPE_FAILD_NOT_BREAK);
				return false;
			}
			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_SUMMER_LI_LIAN:
		{
			const RandActivitySummerLiLianConfig * config = static_cast<const RandActivitySummerLiLianConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN));
			if (NULL == config) return false;

			const RASummerLiLianOtherCfg other_cfg = config->GetOtherCfg();
			if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other_cfg.consume_item_id, other_cfg.consume_num, __FUNCTION__))
			{
				role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return false;
			}

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param3;
		}
		break;
	case BATTLE_MODE_DREAM_NOTES_CHALLENGE:
		{
			SpecialLogic * special_lobgic = role->GetScene()->GetSpecialLogic();
			if (NULL == special_lobgic || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_DREAM_NOTES_CHALLENGE)
			{
				return false;
			}
			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_DU_CHUANG_ZEI_YING:
		{
			SpecialLogic * special_lobgic = role->GetScene()->GetSpecialLogic();
			if (NULL == special_lobgic || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_DU_CHUANG_ZEI_YING)
			{
				return false;
			}

			int npc_id = param2;
			SpecialLogicDuChuangZeiYing * duchuangzeiying_special_lobgic = (SpecialLogicDuChuangZeiYing *)special_lobgic;
			if (duchuangzeiying_special_lobgic->IsLastLayer())
			{
				NPC* npc = NpcManager::Instance().GetServerNPC(param2);
				if (NULL == npc)  return false;

				npc_id = npc->GetNpcId();
			}

			business_data.monster_gen_id = param2;
			business_data.monster_group_id = LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetMonsterGroupId(npc_id, role->GetLevel());
			if (0 == business_data.monster_group_id)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_CATCH_FOX:
		{
			SpecialLogic * special_lobgic = role->GetScene()->GetSpecialLogic();
			if (NULL == special_lobgic || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_CATCH_FOX)
			{
				return false;
			}

			int npc_id = param2;

			NPC* npc = NpcManager::Instance().GetServerNPC(param2);
			if (NULL == npc)  return false;

			npc_id = npc->GetNpcId();

			business_data.monster_gen_id = param2;
			business_data.monster_group_id = 0;

			if (special_lobgic->GetSceneType() == Scene::SCENE_TYPE_CATCH_FOX)
			{
				SpecialLogicCatchFoxFb* logic = (SpecialLogicCatchFoxFb*)special_lobgic;
				if (logic->IsMainTaskCatchFox())
				{
					const MainTaskCatchFoxMonsterConfig * config = LOGIC_CONFIG->GetMainTaskCatchFoxFbConfig()->GetMonsterConfig(role->GetLevel());
					if (config != NULL)
					{
						business_data.monster_group_id = config->monster_group_id;
					}
				}
				else
				{
					const CatchFoxMonsterConfig * config = LOGIC_CONFIG->GetCatchFoxFbConfig()->GetMonsterConfig(role->GetLevel());
					if (config != NULL)
					{
						business_data.monster_group_id = config->monster_group_id;
					}
				}
			}

			if (0 == business_data.monster_group_id)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_EXPEL_MONSTER: // 驱除妖兽活动战斗
	case BATTLE_MODE_MO_ZU_ZAI_NA_LI:
		{
			if (!is_from_server_req)return false;

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param3;
		}
		break;
	case BATTLE_MODE_NPC_FIGHT:
		{
			if (!is_from_server_req) return false;

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param3;
		}
		break;
	case BATTLE_MODE_FANTASY_FB:
	case BATTLE_MODE_MEDAL_FB:
	case BATTLE_MODE_BRAVE_GROUND:
	case BATTLE_MODE_CHALLENGE_FB:
	case BATTLE_MODE_ADVANCE_FB:
	case BATTLE_MODE_COURSE_OF_GROWTH:
	case BATTLE_MODE_SHI_TU_FB:
	case BATTLE_MODE_MONPOLY_GIFT:
	case BATTLE_MODE_RA_MIJINGQIWEN:
	case BATTLE_MODE_RA_MIJINGQIWEN_2:
	case BATTLE_MODE_MAZE_SUMMER:
		{
			if (!is_from_server_req) return false;

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param3;
		}
		break;
	case BATTLE_MODE_CRYSTAL_FB:
	case BATTLE_MODE_ANCIENT_RUINS:
	case BATTLE_MODE_ELITE_TRAILS:
	case BATTLE_MODE_ANCIENT_RUINS_NORMAL_MONSTER:
	case BATTLE_MODE_ELITE_TRAILS_NORMAL_MONSTER:
		{
			// 精英试炼、上古遗迹的战斗模式从水晶副本中抽出来，但逻辑依旧采用水晶副本的逻辑吧

			if (!is_from_server_req) return false;

			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param3;

			SpecialLogic* splogic = role->GetScene()->GetSpecialLogic();
			if (NULL != splogic && splogic->GetSceneType() == Scene::SCENE_TYPE_CRYSTAL_FB)
			{
				SpecialLogicCrystalFB* fb_logic = (SpecialLogicCrystalFB*)splogic;
				int level_seq = fb_logic->GetLevelSeq();
				const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(level_seq);
				if (NULL != cfg && ROLE_CRYSTAL_FB_TYPE_NORMAL != cfg->type)
				{
					business_data.allow_fast_forward_round_num = 0;
				}
			}
		}
		break;
	case BATTLE_MODE_TREASURE_ITEM:
		{
			if (!is_from_server_req)
			{
				return false;
			}
			business_data.is_no_auto = 1;
		}
		break;
	case BATTLE_MODE_MI_YI:
		{
			if (!is_from_server_req)
			{
				return false;
			}

			NPCMonster* npc = (NPCMonster*)NpcManager::Instance().GetServerNPC(param2);
			if (NULL == npc)  return false;

			int level = npc->GetParam1();
			business_data.no_auto_level = level + LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().auto_level_difference;
		}
		break;
	case BATTLE_MODE_MARRIAGE_FB:
	case BATTLE_MODE_FIELD_BOSS: // 打野外BOSS只能从其他请求中转进来， 防止发包直接打BOSS
		{
			if (!is_from_server_req)
			{
				return false;
			}
		}
		break;
	case BATTLE_MODE_DEMON_CAVE:
		{
			int highest_level = role->GetLevel();
			if (!is_from_server_req)
			{
				Team * team = role->GetMyTeam();
				if (team != NULL)
				{
					if (!role->IsLeader())
					{
						return false;
					}

					if (team->HasDismissMember())
					{
						role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
						return false;
					}
					const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_DEMON_CAVE);
					if (NULL == cfg) return false;

					for (int i = 0; i < team->GetMemberCount() && i < MEMBER_NAME_MAX_NUM; i++)
					{
						TeamMember * member = team->GetMemberInfoByIndex(i);
						if (NULL == member) continue;

						if (member->level < cfg->limit_level)
						{
							role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
							return false;
						}

						highest_level = GetMax(member->level, highest_level);
					}

					/*if (team->GetMemberCount() > 1)
					{
						team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_BATTLE, BATTLE_MODE_DEMON_CAVE, param1, param2);
						return false;
					}*/
				}
			}

			const MonsterWaveLevelCfg *  const cfg = LOGIC_CONFIG->GetMonsterWaveConfig()->GetLevelCfg(highest_level);
			if (cfg == NULL)
			{
				return false;
			}

			business_data.monster_group_id = cfg->monster_group_id;
			business_data.limit_max_round = LOGIC_CONFIG->GetMonsterWaveConfig()->GetOtherCfg().max_round_limit;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_WORLD_BOSS:
		{
			if (!is_from_server_req) // 打世界BOSS只能从其他请求中转进来， 防止发包直接打BOSS
			{
				return false;
			}

			ActivityWorldBoss* act = (ActivityWorldBoss*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS);
			if (NULL == act || ACTIVITY_STATUS_OPEN != act->GetActivityStatus())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}

			if (act->GetBossCurHp() <= 0)
			{
				role->NoticeNum(errornum::EN_WORLD_BOSS_ALREADY_DEAD);
				return false;
			}

			business_data.world_boss_max_hp = act->GetBossMaxHp();
			business_data.world_boss_cur_hp = act->GetBossCurHp();
			business_data.role_runaway_always_fail = 1;

			if (!this->CanStartWorldBoss(role)) return false;
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_2:
		{
			if (!is_from_server_req) // 打世界BOSS只能从其他请求中转进来， 防止发包直接打BOSS
			{
				return false;
			}

			ActivityWorldBoss2 * act = (ActivityWorldBoss2*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2);
			if (NULL == act || ACTIVITY_STATUS_OPEN != act->GetActivityStatus())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}

			if (act->GetBossCurHp() <= 0)
			{
				role->NoticeNum(errornum::EN_WORLD_BOSS_2_ALREADY_DEAD);
				return false;
			}

			business_data.world_boss_max_hp = act->GetBossMaxHp();
			business_data.world_boss_cur_hp = act->GetBossCurHp();
			business_data.role_runaway_always_fail = 1;

			if (!this->CanStartWorldBoss2(role)) return false;
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_3:
	{
		if (!is_from_server_req) // 打世界BOSS只能从其他请求中转进来， 防止发包直接打BOSS
		{
			return false;
		}

		ActivityWorldBoss3* act = (ActivityWorldBoss3*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3);
		if (NULL == act || ACTIVITY_STATUS_OPEN != act->GetActivityStatus())
		{
			role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
			return false;
		}

		if (act->GetBossCurHp() <= 0)
		{
			role->NoticeNum(errornum::EN_WORLD_BOSS_ALREADY_DEAD);
			return false;
		}

		business_data.world_boss_max_hp = act->GetBossMaxHp();
		business_data.world_boss_cur_hp = act->GetBossCurHp();
		business_data.role_runaway_always_fail = 1;

		if (!this->CanStartWorldBoss3(role)) return false;
	}
	break;
	case BATTLE_MODE_ZHEN_ZHONG:
		{
			Team* team = role->GetMyTeam();
			if (NULL == team || !team->IsLeader(role))
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}
			int mem_count = team->GetMemberCount();
			for (int mem_cur = 0; mem_cur < mem_count; ++mem_cur)
			{
				Role * mem_role = team->GetMemberRoleByIndex(mem_cur);
				if (NULL == mem_role) continue;

				int role_level = mem_role->GetLevel();
				if (!FunOpen::IsFunOpen(mem_role, FUN_TYPE_ZHENZHONG_RESERCH, false))
				{
					role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
					return false;
				}
			}

			ResearchTaskInfo t_info;
			role->GetRoleModuleManager()->GetResearchTask()->OnGainTaskInfo(&t_info);

			business_data.monster_group_id = t_info.battle_group_id;
			monster_group_id = t_info.battle_group_id;
		}
		break;
	case BATTLE_MODE_COURAGE_CHALLENGE:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_COURAGE_CHALLENGE, true))
			{
				return false;
			}

			const CourageChallengeLevelCfg * level_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfg(param1, param2);
			if (level_cfg == NULL) return false;

			if (level_cfg->level > role->GetLevel())
			{
				role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}

			business_data.condition_num = 0;
			business_data.condition_flag = 0;
			for (int i = 0; i < COURAGE_CHALLENGE_CONDITION_MAX_NUM && i < BATTLE_CONDITION_MAX_NUM; i++)
			{
				business_data.condition_list[i] = level_cfg->condition[i];
				business_data.condition_num++;
			}

			business_data.attacker_buff_id = level_cfg->attacker_buff_id;
			business_data.attacker_buff_lv = level_cfg->attacker_buff_level;

			business_data.monster_group_id = level_cfg->monster_group_id;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_NEW_COURAGE_CHALLENGE:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_NEW_COURAGE_CHALLENGE, true))
			{
				return false;
			}

			const NewCourageChallengeLevelCfg * level_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewLevelCfg(param1, param2);
			if (level_cfg == NULL) return false;

			if (level_cfg->level > role->GetLevel())
			{
				role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}

			business_data.condition_num = 0;
			business_data.condition_flag = 0;
			for (int i = 0; i < COURAGE_CHALLENGE_CONDITION_MAX_NUM && i < BATTLE_CONDITION_MAX_NUM; i++)
			{
				business_data.condition_list[i] = level_cfg->condition[i];
				business_data.condition_num++;
			}

			business_data.monster_group_id = level_cfg->monster_group_id;
			monster_group_id = business_data.monster_group_id;
		}
		break;

	case BATTLE_MODE_REALIVE_GUIDE:
		{
			business_data.monster_group_id = TaskPool::Instance()->GetTaskOtherCfg().realive_guide_monster_group;
			monster_group_id = business_data.monster_group_id;
		}
		break;

	case BATTLE_MODE_GUILD_BOSS:
		{
			if (role->IsInCross())
			{
				role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
				return false;
			}

			Guild* guild = GuildManager::Instance().GetGuild(role->GetGuildID());
			if (NULL == guild)
			{
				role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
				return false;
			}

			business_data.guild_boss_seq = guild->GetGuildBossSeq();
			business_data.guild_boss_guild_id = guild->GetGuildID();
			for (int i = 0; i < ARRAY_ITEM_COUNT(business_data.guild_boss_monster_remain_hp_percent_list); ++i)
			{
				business_data.guild_boss_monster_remain_hp_percent_list[i] = static_cast<short>(GuildBossParam::BOSS_HP_PERCENT);
			}
			//memcpy(business_data.guild_boss_monster_remain_hp_percent_list, guild->GetGuildBossRemainHpPercentList(), sizeof(business_data.guild_boss_monster_remain_hp_percent_list));
			business_data.guild_boss_chapter = guild->GetGuildBossChapter();
			business_data.limit_max_round = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->boss_round_limit;

			const GuildBossCfg::GuildBossCfgItem * cfg = LOGIC_CONFIG->GetGuildConfig()->GetBossCfg(guild->GetGuildBossChapter());
			if (NULL == cfg)
			{
				role->NoticeNum(errornum::EN_GUILD_BOSS_FINISH);
				return false;
			}

			business_data.monster_group_id = cfg->boss_group_id;
			monster_group_id = cfg->boss_group_id;
		}
		break;
	case BATTLE_MODE_TREASURE_MAP:
		{
			if (role->InTeam())
			{
				if (!role->IsLeader())
				{
					return false;
				}
			}

			if (!role->GetRoleModuleManager()->GetTreasureMap()->CanStart()) return false;

			business_data.monster_group_id = role->GetRoleModuleManager()->GetTreasureMap()->GetMonsterId();
			business_data.param1 = role->GetUID();
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_TASK_CHAIN:
		{
			business_data.monster_group_id = role->GetRoleModuleManager()->GetTaskChain()->GetMonsterGroupId();
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = role->GetUID();
			if (0 == monster_group_id)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}
		}
		break;
	case BATTLE_MODE_SHAN_HAI_BOSS:
		{
			if (!CrossConfig::Instance().IsHiddenServer())
			{
				//只能在跨服挑战
				return false;
			}
			Team * team = role->GetMyTeam();
			if (team == NULL)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			const ShanHaiBossMonsterCfg* monster_cfg = LOGIC_CONFIG->GetShanHaiBossConfig()->GetMonsterCfg(param1);
			if (NULL == monster_cfg) return false;

			NPCMonster* npc = (NPCMonster*)NpcManager::Instance().GetServerNPC(param2);
			if (NULL == npc)  return false;

			if (npc->GetDeathNum() + npc->GetBattleTeamNum() >= monster_cfg->fight_times)
			{
				role->NoticeNum(errornum::EN_MONSTER_IS_LOCK);
				return false;
			}

			if (!is_from_server_req)
			{
				if (!role->IsLeader())
				{
					return false;
				}

				if (team->HasDismissMember())
				{
					role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
					return false;
				}

				if (team->GetMemberCount() < LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().limit_team_people)
				{
					role->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
					return false;
				}

				/*if (team->GetFollowMemberMaxLevel() - team->GetFollowMemberMinLevel() >= 10)
				{
					role->NoticeNum(errornum::EN_TEAM_LEVEL_MORE_TEN);
					return false;
				}*/

				for (int idx = 0; idx < team->GetMemberCount(); ++idx)
				{
					Role * member = team->GetMemberRoleByIndex(idx);
					if (NULL == member) continue;		//return 先改成 continue

					if (!FunOpen::IsFunOpen(member, FUN_TYPE_SHAN_HAI_BOSS, true))
					{
						return false;
					}

					if (member->GetRoleModuleManager()->IsChallengeShanHaiBoss(monster_cfg->seq))
					{
						role->NoticeNum(errornum::EN_SHAN_HAI_BOSS_NOT_CHALLENGE_COUNT);
						return false;
					}
				}

				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_BATTLE, BATTLE_MODE_SHAN_HAI_BOSS, param1, param2, param3);
				return false;
			}

			business_data.monster_group_id = monster_cfg->monster_group_id;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param1;
			business_data.param3 = static_cast<int>(ShanHaiBossManager::Instance().GetNextRefreshTime());
			business_data.is_use_level_change = 1;
			business_data.use_level = team->GetFollowMemberMaxLevel();
			business_data.no_auto_level = business_data.use_level + LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().auto_level_difference;
		}
		break;
	case BATTLE_MODE_PET_MONOPOLY:
		{
			business_data.monster_group_id = param1;
			business_data.monster_gen_id = 0;
			monster_group_id = business_data.monster_group_id;
			business_data.is_catch_bind_pet = 1;
			business_data.is_no_auto = 1;
		}
		break;
	case BATTLE_MODE_HUNDRED_GHOST:
		{
			if (!CrossConfig::Instance().IsHiddenServer())
			{
				//只能在跨服挑战
				return false;
			}

			const HundredGhostMonsterCfg* monster_cfg = LOGIC_CONFIG->GetHundredGhostConfig()->GetMonsterCfg(param1);
			if (NULL == monster_cfg)
			{
				return false;
			}

			Team * team = role->GetMyTeam();
			if (team == NULL)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			if (!role->IsLeader())
			{
				return false;
			}

			if (team->HasDismissMember())
			{
				role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
				return false;
			}

			if (team->GetMemberCount() < LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg().team_num)
			{
				role->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
				return false;
			}

			for (int idx = 0; idx < team->GetMemberCount(); ++idx)
			{
				Role * member = team->GetMemberRoleByIndex(idx);
				if (NULL == member) continue;

				/*if (!FunOpen::IsFunOpen(member, FUN_TYPE_SHAN_HAI_BOSS, true))
				{
				return false;
				}*/

				if (member->GetLevel() < LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg().fight_min_level)
				{
					team->NoticeMemberLevelNotEnough(member->GetName(), LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg().fight_min_level);
					return false;
				}
			}

			business_data.monster_group_id = monster_cfg->monster_group_id;
			business_data.monster_gen_id = param2;
			monster_group_id = business_data.monster_group_id;
			business_data.param1 = param1;
			business_data.is_use_level_change = 1;
			business_data.use_level = team->GetFollowMemberMaxLevel();
			business_data.limit_max_round = LOGIC_CONFIG->GetHundredGhostConfig()->GetOtherCfg().max_rounds;
			//business_data.no_auto_level = business_data.use_level + LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().auto_level_difference;
		}
		break;
	case BATTLE_MODE_YIN_YANG_EYE:
		{
			Team * team = role->GetMyTeam();
			if (team != NULL)
			{
				if (!role->IsLeader())
				{
					return false;
				}
			}

			business_data.monster_group_id = param1;
			business_data.param2 = param2;
			business_data.param3 = param3;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	case BATTLE_MODE_REALM_BREAK_FIGHT:
		{
			Team * m_team = role->GetMyTeam();
			if (NULL != m_team)
			{
				role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
				return false;
			}
			business_data.monster_group_id = param1;
			business_data.monster_gen_id = param2;
			business_data.param1 = param3;
			monster_group_id = business_data.monster_group_id;
		}
		break;
	default:
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}
		break;
	}

	bool start_ret = false;
	if (!is_in_team || !is_follow)
	{
		start_ret = this->StartSinglePVEReq(role, monster_group_id, battle_mode, business_data);
	}
	else if (!is_leader)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return false;
	}
	else
	{
		Team* team = role->GetMyTeam();
		if (NULL == team || team->GetFollowMemberCount() < least_follow_member_num)
		{
			if (!EventHandler::Instance().TeamNotEnoughMembers(role, battle_mode))
			{
				role->NoticeNum(errornum::EN_FOLLOW_MEMBER_NOT_ENOUGH);
			}

			return false;
		}
		else//检测真实玩家数量
		{
			int real_player_num = 0;
			int member_count = team->GetMemberCount();
			for (int i = 0; i < member_count; ++i)
			{
				TeamMember* member = team->GetMemberInfoByIndex(i);
				if (member == NULL)continue;
				if (member->IsRobot())continue;
				real_player_num += 1;
			}

			if (real_player_num < least_real_player_member_num)
			{
				role->NoticeNum(errornum::EN_FOLLOW_MEMBER_NOT_ENOUGH);
			}
		}

		int memcount = team->GetMemberCount();
		for (int idx = 0; idx < memcount; ++idx)
		{
			TeamMember* member = team->GetMemberInfoByIndex(idx);
			if (NULL == member || member->level < min_level_limit)
			{
				role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
				return false;
			}
		}

		start_ret = this->StartTeamPVEReq(team, monster_group_id, battle_mode, business_data);
	}

	if (start_ret)
	{
		switch (battle_mode)
		{
		case BATTLE_MODE_WORLD_BOSS:
			{
				Team * team = role->GetMyTeam();
				if (NULL == team)
				{
					break;		//理论不可能走到这里，世界boss上面已经判断要组队才能打
				}

				ActivityWorldBoss * act = static_cast<ActivityWorldBoss *>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS));
				for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
				{
					Role * member = team->GetMemberRoleByIndex(i);
					if (NULL == member)
					{
						continue;		//上面已经判断所有人都必须在线,所以不存在没 增加PlayTimes情况
					}

					if (act != NULL)
					{
						act->AddWorldBossPlayTimes(member->GetUID());
					}
				}
			}
			break;

		case BATTLE_MODE_WORLD_BOSS_2:
			{
				Team * team = role->GetMyTeam();
				if (NULL == team)
				{
					break;		//理论不可能走到这里，世界boss上面已经判断要组队才能打
				}

				ActivityWorldBoss2 * act = static_cast<ActivityWorldBoss2 *>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2));
				for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
				{
					Role * member = team->GetMemberRoleByIndex(i);
					if (NULL == member)
					{
						continue;		//上面已经判断所有人都必须在线,所以不存在没 增加PlayTimes情况
					}

					if (act != NULL)
					{
						act->AddWorldBoss2PlayTimes(member->GetUID());
					}
				}
			}
			break;

		case BATTLE_MODE_WORLD_BOSS_3:
			{
				Team * team = role->GetMyTeam();
				if (NULL == team)
				{
					break;		//理论不可能走到这里，世界boss上面已经判断要组队才能打
				}

				ActivityWorldBoss3 * act = static_cast<ActivityWorldBoss3 *>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3));
				for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
				{
					Role * member = team->GetMemberRoleByIndex(i);
					if (NULL == member)
					{
						continue;		//上面已经判断所有人都必须在线,所以不存在没 增加PlayTimes情况
					}

					if (act != NULL)
					{
						act->AddWorldBossPlayTimes(member->GetUID());
					}
				}
			}
			break;
		case BATTLE_MODE_SHAN_HAI_BOSS:
			{
				Team * team = role->GetMyTeam();
				if (NULL == team)
				{
					break;		//理论不可能走到这里，山海boss上面已经判断要组队才能打
				}

				NPCMonster* npc = (NPCMonster*)NpcManager::Instance().GetServerNPC(param2);
				if (NULL != npc)
				{
					npc->AddBattleTeamNum();
				}

				for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
				{
					Role * member = team->GetMemberRoleByIndex(i);
					if (NULL == member)
					{
						continue;		//上面已经判断所有人都必须在线
					}

					member->GetRoleModuleManager()->SetShanHaiBossChallengeDayId();
				}
			}
			break;
		default:
			break;
		}
	}

	if (is_in_team)
	{
		Team * team = role->GetMyTeam();
		if (team != NULL)
		{
			for (int idx = 0; idx < team->GetMemberCount(); ++idx)
			{
				Role* member = team->GetMemberRoleByIndex(idx);
				if (NULL != member)
				{
					WorldStatus::Instance().LogFunctionStats(member, "BattleManagerLocal_mode_fight", battle_mode);
				}
			}
		}
	}
	else
	{
		WorldStatus::Instance().LogFunctionStats(role, "BattleManagerLocal_mode_fight", battle_mode);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);

	return start_ret;
}

bool BattleManagerLocal::StartChallengeOthersReq(Role * role, int enemy_uid, int battle_mode)
{
	//printf("\nChallOthers -> MyUid: %d | Enemy: %d | BattleMode: %d", role->GetUID(), enemy_uid, battle_mode);
	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_3);
#endif
		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}

	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	Role * enemy = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(enemy_uid));
	if (NULL == enemy)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
		return false;
	}

	if (!enemy->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ENEMY_ROLE_CAN_NOT_BATLLE);
		return false;
	}

	Scene * scene = role->GetScene();
	if (NULL == scene || !scene->GetSpecialLogic()->CanChallengeOthers(role, enemy)) return false;

	Scene * enemy_scene = enemy->GetScene();
	if (NULL == enemy_scene || enemy_scene->GetSceneID() != scene->GetSceneID() || enemy_scene->GetSceneKey() != scene->GetSceneKey())
	{
		role->NoticeNum(errornum::EN_TARGET_SCENE_NO_SAME);
		return false;
	}

	// 检测通过，录入战斗人员表
	int atk_num = 0, def_num = 0;
	Role * atk_list[MAX_TEAM_MEMBER_NUM] = { NULL }, *def_list[MAX_TEAM_MEMBER_NUM] = { NULL };

	Team * my_team = role->GetMyTeam();
	if (NULL != my_team)		// 发起挑战的人一定要是队长才行；
	{
		if (role->GetUID() != my_team->GetLeaderUID())
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			return false;
		}

		int member_count = my_team->GetMemberCount();
		for (int mem_idx = 0; mem_idx < member_count; ++mem_idx)
		{
			TeamMember* member = my_team->GetMemberInfoByIndex(mem_idx);
			if (NULL == member || !member->is_follow) continue;

			Role* role = my_team->GetMemberRoleByIndex(mem_idx);
			if (NULL != role)
			{
				atk_list[atk_num++] = role;
			}
		}
	}
	else
	{
		atk_list[atk_num++] = role;
	}

	Team * enemy_team = enemy->GetMyTeam();
	if (NULL != enemy_team)
	{
		int member_count = enemy_team->GetMemberCount();
		for (int mem_idx = 0; mem_idx < member_count; ++mem_idx)
		{
			TeamMember* member = enemy_team->GetMemberInfoByIndex(mem_idx);
			if (NULL == member || !member->is_follow) continue;

			Role* role = enemy_team->GetMemberRoleByIndex(mem_idx);
			if (NULL != role)
			{
				def_list[def_num++] = role;
			}
		}
	}
	else
	{
		def_list[def_num++] = enemy;
	}

	static BattleData_Business business;
	business.Reset();
	this->MarkBattleLaunchInfo(business, role, __FUNCTION__);
	switch (battle_mode)
	{
	case BATTLE_MODE_WORLD_TEAM_ARENA:
		{
			ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA));
		if (NULL == activity) return false;

			if (!activity->IsActivityOpen())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}
			if (activity->IsActivityEnterTime())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_WAITING_FOR_COUNTDOWN);
				return false;
			}
			//检测一下战斗成员是否是同一个战队的
		if (NULL != my_team && !WorldTeamArenaSignUpManager::Instance().IsSameTeamByTeam(my_team, true))
			{
				return false;
			}
			if (NULL != enemy_team && !WorldTeamArenaSignUpManager::Instance().IsSameTeamByTeam(enemy_team, true))
			{
				return false;
			}

			const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
			business.limit_max_round = other_cfg.max_rounds;
		}
		break;
	case BATTLE_MODE_ONLY_FIGHT:
		{
			ActivityOnlyFight * activity = (ActivityOnlyFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_ONLY_FIGHT);
			if (NULL == activity) return false;

			if (!activity->IsActivityOpen())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}
			if (activity->IsActivityEnterTime())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_WAITING_FOR_COUNTDOWN);
				return false;
			}

			const OnlyFightOtherCfg & of_other = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightOtherCfg();
			business.limit_max_round = of_other.max_rounds_num;
			business.limit_max_time = 20 * SECOND_PER_MIN;		// 以一敌百(个人版家族大乱斗) 单场战斗时间限制 20 分钟；
		}
		break;
	case BATTLE_MODE_GUILD_FIGHT:
		{
			ActivityGuildFight * activity = (ActivityGuildFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
			if (NULL == activity) return false;

			if (!activity->IsActivityOpen())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}
			if (activity->IsActivityEnterTime())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_WAITING_FOR_COUNTDOWN);
				return false;
			}

			const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
			business.limit_max_round = gf_other.max_rounds_num;
			business.limit_max_time = 20 * SECOND_PER_MIN;		// 家族大乱斗 单场战斗时间限制 20 分钟；
		}
		break;
	case BATTLE_MODE_CHIEF_ELECTION_SEA:
		{
			if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_CHIEF_ELECTION))
			{
				return false;
			}

			if (scene->GetSceneType() != Scene::SCENE_TYPE_CHIEF_ELECTION_SEA)
			{
				return false;
			}

			const ChiefElectionOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg();
			business.limit_max_round = other_cfg.max_round;
			business.until_one_side_all_dead = 1;
			business.attacker_sidebuff_id = 0;
			business.defender_sidebuff_id = 0;
			business.param2 = role->GetRoleModuleManager()->GetCapability()->GetRoleCapability() + role->GetRoleModuleManager()->GetPet()->GetFightPetCapability();
			business.param3 = enemy->GetRoleModuleManager()->GetCapability()->GetRoleCapability() + enemy->GetRoleModuleManager()->GetPet()->GetFightPetCapability();
		}
		break;
	case BATTLE_MODE_NORMAL_PRACTICE:
	case BATTLE_MODE_SINGLE_PVP_PRACTICE:
		{

		}
		break;
	case BATTLE_MODE_DUEL:
		{
			if (!LOGIC_CONFIG->GetDuelConfig()->SceneCanDuel(role->GetScene()->GetSceneID()))
			{
				role->NoticeNum(errornum::EN_DUEL_SCENE_CAN_NOT_DUEL);
				return false;
			}

			const DuelOtherCfg& other_cfg = LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg();


			int att_idx = 0;
			int def_idx = 0;			//对方队长idx
			bool def_is_red = false;
			bool is_has_excort_task = false;

			int att_uid_list[MAX_TEAM_MEMBER_NUM] = { 0 };
			int def_uid_list[MAX_TEAM_MEMBER_NUM] = { 0 };

			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
			{
				if (NULL != atk_list[i])
				{
					if (!FunOpen::IsFunOpen(atk_list[i], FUN_TYPE_DUEL))
					{
						role->NoticeNum(errornum::EN_DUEL_MINE_CAN_NOT_DUEL);
						return false;
					}

					if (role->GetUID() == atk_list[i]->GetUID())
					{
						att_idx = i;
					}
					att_uid_list[i] = atk_list[i]->GetUID();
				}

				if (NULL != def_list[i])
				{
					if (!FunOpen::IsFunOpen(def_list[i], FUN_TYPE_DUEL))
					{
						role->NoticeNum(errornum::EN_DUEL_TARGET_CAN_NOT_DUEL);
						return false;
					}

					if (def_list[i]->GetRoleModuleManager()->GetDuel()->IsRedName())
					{
						def_is_red = true;
					}

					if (def_list[i]->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
					{
						is_has_excort_task = true;
					}

					if (NULL != enemy_team)
					{
						if (def_list[i]->GetUID() == enemy_team->GetLeaderUID())
						{
							def_idx = i;
						}
					}
					def_uid_list[i] = def_list[i]->GetUID();
				}
			}

			if (0 > att_idx || 0 > def_idx || att_idx >= MAX_TEAM_MEMBER_NUM || def_idx >= MAX_TEAM_MEMBER_NUM)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			int duel_type = DUEL_BATTLE_TYPE_BEGIN;
			if (is_has_excort_task)
			{
				duel_type = DUEL_BATTLE_TYPE_ESCORT;

				role->GetRoleModuleManager()->GetDuel()->OnDuelEscort(def_list[def_idx]->GetRoleModuleManager()->GetCommonData().escort_task_seq, def_list[def_idx]->GetRoleModuleManager()->GetCommonData().escort_npc_id);
			}
			else
			{
				if (def_is_red)
				{
					duel_type = DUEL_BATTLE_TYPE_ADJUDICATION;
				}
				else
				{
					duel_type = DUEL_BATTLE_TYPE_DUEL;
				}
			}

			if (duel_type == DUEL_BATTLE_TYPE_DUEL || duel_type == DUEL_BATTLE_TYPE_ESCORT)
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(other_cfg.duel_item_id, 1))
				{
					role->NoticeNum(errornum::EN_DUEL_ITEM_NOT_ENOUGH);
					return false;
				}

				role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other_cfg.duel_item_id, 1, __FUNCTION__);
			}

			if (duel_type == DUEL_BATTLE_TYPE_DUEL)
			{
				int length = 0;
				if (NULL != enemy_team)
				{
					length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_begin_duel_battle_team,
						role->GetName(), other_cfg.duel_item_id, def_list[def_idx]->GetName(), role->GetScene()->GetSceneID(), role->GetPos().x, role->GetPos().y);
				}
				else
				{
					length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_begin_duel_battle,
						role->GetName(), other_cfg.duel_item_id, def_list[def_idx]->GetName(), role->GetScene()->GetSceneID(), role->GetPos().x, role->GetPos().y);
				}
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
						0, 0, 0, 0);
				}
			}
			else if (duel_type == DUEL_BATTLE_TYPE_ADJUDICATION)
			{
				int length = 0;
				length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_begining_duel_two,
					role->GetName(), def_list[def_idx]->GetName(), role->GetScene()->GetSceneID(), role->GetPos().x, role->GetPos().y);

				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
						0, 0, 0, 0);
				}
			}

			Protocol::SCDuelBattleInfo dule_info;
			dule_info.duel_type = duel_type;
			memcpy(dule_info.att_name, role->GetName(), sizeof(GameName));
			memcpy(dule_info.def_name, def_list[def_idx]->GetName(), sizeof(GameName));
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
			{
				dule_info.att_list[i] = 0;
				if (NULL != atk_list[i])
				{
					dule_info.att_list[i] = atk_list[i]->GetUID();
				}

				dule_info.def_list[i] = 0;
				if (NULL != def_list[i])
				{
					dule_info.def_list[i] = def_list[i]->GetUID();
				}
			}
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
			{
				if (NULL != atk_list[i])
				{
					EngineAdapter::Instance().NetSend(atk_list[i]->GetNetId(), (const char*)&dule_info, sizeof(dule_info));
				}
				if (NULL != def_list[i])
				{
					EngineAdapter::Instance().NetSend(def_list[i]->GetNetId(), (const char*)&dule_info, sizeof(dule_info));
				}
			}

			int level_diff = role->GetLevel() - def_list[def_idx]->GetLevel();
			business.param1 = duel_type;
			business.param2 = level_diff;
			business.param3 = att_idx + def_idx * 10;

			gamelog::g_log_duel.printf(LL_INFO, "%s DUEL_BEGIN att[%s][%d %d %d %d %d] def[%s][%d %d %d %d %d] duel_type[%d] param1[%d] param2[%d] param3[%d]", 
				__FUNCTION__,
				role->GetName(), att_uid_list[0], att_uid_list[1], att_uid_list[2], att_uid_list[3], att_uid_list[4],
				def_list[def_idx]->GetName(), def_uid_list[0], def_uid_list[1], def_uid_list[2], def_uid_list[3], def_uid_list[4],
				duel_type, business.param1, business.param2, business.param3);
		}
		break;

	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
		break;
	}

	bool ret = false;
	if (atk_num <= 1 && def_num <= 1)
	{
		ret = this->StartSinglePVPReq(role, enemy, battle_mode, business);
	}
	else
	{
		ret = this->StartMultiPVPReq(atk_num, atk_list, def_num, def_list, battle_mode, business);
	}

	return ret;
}

bool BattleManagerLocal::StartMineMonsterFightReq(Role* role, bool is_real_mine,
	int head_monster_id /* = 0 */, bool is_all_lv1 /* = false */, int npc_seq_id /* = -1 */)
{
	if (!role->GetScene()->HasMineMonster()) return false;

	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_4);
#endif
		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}

	int battle_mode = role->GetScene()->GetSpecialLogic()->GetMineMonsterBattleMode();
	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	int monster_group_id = BATTLE_MINE_MONSTER_GROUP; // 用怪物组ID来标识暗雷怪，而不是用battle_mode(因为battle_mode可能因不同场景有不同需求）

	int escort_seq = role->GetRoleModuleManager()->GetEscort()->GetEscortSeq();
	if (0 != escort_seq)		// 如果在押镖 则把怪替换成强盗
	{
		int temp = LOGIC_CONFIG->GetEscortConfig()->RandMonsterGroup(escort_seq);
		if (0 != temp)monster_group_id = temp;
	}

	static BattleData_Business business_data;
	business_data.Reset();
	this->MarkBattleLaunchInfo(business_data, role, __FUNCTION__);

	business_data.monster_list_size = role->GetScene()->RandMineMonster(role, SIDE_CHARACTER_NUM, business_data.monster_id_list, business_data.monster_level_list);
	business_data.monster_group_id = monster_group_id;
	business_data.monster_gen_id = npc_seq_id;
	business_data.is_real_mine = (is_real_mine ? 1 : 0);
	business_data.attacker_sidebuff_id = this->GetBattleModeSceneBuff(role, battle_mode);
	business_data.notice_num = this->GetBattleModeNoticeNum(role, battle_mode);

	if (is_all_lv1)
	{
		for (int i = 0; i < business_data.monster_list_size; ++i)
		{
			business_data.monster_level_list[i] = 1;
		}
	}

	if (is_real_mine)
	{
		this->SetAutoCatchBusinessData(role, business_data);
	}

	// 如果是使用暗怪逻辑的明怪，则判断随机出来的怪物有没有head_monster，如果没有则加上一个
	if (!is_real_mine)
	{
		bool is_found_head_monster = false;
		for (int i = 0; i < business_data.monster_list_size; ++i)
		{
			if (business_data.monster_id_list[i] == head_monster_id)
			{
				is_found_head_monster = true;
				break;
			}
		}

		if (!is_found_head_monster)
		{
			business_data.monster_id_list[0] = head_monster_id;
		}
	}


	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		return this->StartTeamPVEReq(team, monster_group_id, battle_mode, business_data);
	}
	else if (NULL == team || !team->IsRoleMemberFollow(role->GetUID()))
	{
		return this->StartSinglePVEReq(role, monster_group_id, battle_mode, business_data);
	}
	else
	{
		return false;
	}
}

bool BattleManagerLocal::StartSinglePVPPraticeReq(Role* attacker, Role* defender)
{
	if (NULL == attacker || NULL == defender) return false;

	int battle_mode = BATTLE_MODE_SINGLE_PVP_PRACTICE;
	static BattleData_Business business;
	business.Reset();
	this->MarkBattleLaunchInfo(business, attacker, __FUNCTION__);

	return this->StartSinglePVPReq(attacker, defender, battle_mode, business);
}

bool BattleManagerLocal::StartEsortMonsterFightReq(Role * role)
{
	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_5);
#endif
		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}
	int battle_mode = BATTLE_MODE_ESCORT;
	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	int escort_seq = role->GetRoleModuleManager()->GetEscort()->GetEscortSeq();
	if (0 == escort_seq) return false;

	int monster_group_id = LOGIC_CONFIG->GetEscortConfig()->RandMonsterGroup(escort_seq);

	static BattleData_Business business_data;
	business_data.Reset();
	business_data.monster_list_size = role->GetScene()->RandMineMonster(role, SIDE_CHARACTER_NUM, business_data.monster_id_list, business_data.monster_level_list);
	business_data.monster_group_id = monster_group_id;
	this->MarkBattleLaunchInfo(business_data, role, __FUNCTION__);

	bool ret = false;
	ret = this->StartSinglePVEReq(role, monster_group_id, battle_mode, business_data);

	return ret;
}

bool BattleManagerLocal::StartTeamPVPReq(int attacker_num, Role** attacker_list, int defender_num, Role** defender_list, int battle_mode)
{
	if (NULL == attacker_list || NULL == defender_list) return false;

	static BattleData_Business business;
	business.Reset();
	if (attacker_num > 0)
	{
		this->MarkBattleLaunchInfo(business, attacker_list[0], __FUNCTION__);
	}

	switch (battle_mode)
	{
	case BATTLE_MODE_SQUARD_FIGHT:
		{
			business.limit_max_round = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetOtherCfg()->limit_max_round;
		}
		break;
	default:
		{
			return false;
		}
		break;
	}

	return this->StartMultiPVPReq(attacker_num, attacker_list, defender_num, defender_list, battle_mode, business);
}

int BattleManagerLocal::StartGuildHonorPVPReq(int zone_id, long long battle_length_of_time, std::vector<Role *> attacker_list, std::vector<Role *> defender_list, std::vector<Role *> attacker_observer_list, std::vector<Role *> defender_observer_list, int ARG_OUT * battle_id_ptr)
{
	int attacker_num = (int)attacker_list.size();
	int defender_num = (int)defender_list.size();
	int attacker_observer_num = (int)attacker_observer_list.size();
	int defender_observer_num = (int)defender_observer_list.size();
	Role ** attacker_observer_list_ptr = attacker_observer_num > 0 ? &attacker_observer_list[0] : NULL;
	Role ** defender_observer_list_ptr = defender_observer_num > 0 ? &defender_observer_list[0] : NULL;

	if (attacker_num == 0 && defender_num == 0)
	{
		return RESULT_BOTH_FAIL;
	}

	if (attacker_num == 0)
	{
		return RESULT_ATTACKER_FAIL;
	}

	if (defender_num == 0)
	{
		return RESULT_DEFENDER_FAIL;
	}

	const ActivityGuildHonorBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildHonorBattleConfig()->GetOtherCfg();
	static BattleData_Business business; business.Reset();
	business.limit_max_round = other_cfg.max_battle_rounds;
	business.limit_max_time = (int)battle_length_of_time;
	business.param1 = zone_id;
	if (attacker_num > 0)
	{
		this->MarkBattleLaunchInfo(business, attacker_list[0], __FUNCTION__);
	}

	return this->StartMultiPVPReq(attacker_num, &attacker_list[0], defender_num, &defender_list[0], BATTLE_MODE_GUILD_HONOR_BATTLE,
		business, attacker_observer_num, attacker_observer_list_ptr, defender_observer_num, defender_observer_list_ptr, battle_id_ptr) == false
		? RESULT_BOTH_FAIL : START_BATTLE_RESULT_SUCC;
}

int BattleManagerLocal::StartQingYuanShengHuiReq(std::vector<Role *> attacker_list, std::vector<Role *> defender_list)
{
	int attacker_num = (int)attacker_list.size();
	int defender_num = (int)defender_list.size();

	if (attacker_num == 0 && defender_num == 0)
	{
		return RESULT_BOTH_FAIL;
	}

	if (attacker_num == 0)
	{
		return RESULT_ATTACKER_FAIL;
	}

	if (defender_num == 0)
	{
		return RESULT_DEFENDER_FAIL;
	}

	static BattleData_Business business; business.Reset();
	business.limit_max_round = 20;

	if (attacker_num > 0)
	{
		this->MarkBattleLaunchInfo(business, attacker_list[0], __FUNCTION__);
	}

	return this->StartMultiPVPReq(attacker_num, &attacker_list[0], defender_num, &defender_list[0], BATTLE_MODE_QING_YUAN_SHENG_HUI,
		business) == false ? RESULT_BOTH_FAIL : START_BATTLE_RESULT_SUCC;
}

bool BattleManagerLocal::StartSinglePVPReq(Role* attacker, Role* defender, int battle_mode, const BattleData_Business& business_data)
{
	if (NULL == attacker || NULL == defender) return false;

	if (!this->HasBattleServerConnected())
	{
		attacker->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		defender->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* attacker_info = this->GetRoleBattleInfoConst(attacker->GetUID());
	if (NULL != attacker_info)
	{
#ifdef _DEBUG
		attacker->NoticeNum(errornum::EN_ROLE_IN_BATTLE_6);
#endif

		this->CheckRoleReallyInBattle(attacker, __FUNCTION__);
		return false;
	}

	if (!attacker->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		attacker->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	if (attacker->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
	{
		attacker->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		defender->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	const RoleBattleInfo* defender_info = this->GetRoleBattleInfoConst(defender->GetUID());
	if (NULL != defender_info)
	{
#ifdef _DEBUG
		defender->NoticeNum(errornum::EN_ROLE_IN_BATTLE_6);
#endif

		this->CheckRoleReallyInBattle(defender, __FUNCTION__);
		return false;
	}

	if (!defender->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		defender->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	if (defender->GetRoleModuleManager()->GetEscort()->HaveEscortTask() && battle_mode != BATTLE_MODE_DUEL)
	{
		attacker->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		defender->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	int battle_id = this->GenBattleID();

	// 判断完后 收集玩家的战斗数据
	static battlegameprotocol::GameBattleStartSinglePVPReq req;

	req.attacker_bgnetid = attacker->GetBGNetID();
	req.attacker_uid = attacker->GetUID();
	req.defender_bgnetid = defender->GetBGNetID();
	req.defender_uid = defender->GetUID();

	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = ANIMATION_SPEED_NORMAL;
	req.business_data = business_data;
	req.battle_id = battle_id;

	attacker->GetRoleModuleManager()->CollectRoleBattleData(&req.attacker_battle_data, true, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
	defender->GetRoleModuleManager()->CollectRoleBattleData(&req.defender_battle_data, true, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());

	static Role* role_list[BATTLE_MAX_ROLE_NUM];
	static RoleBattleData* role_data_list[BATTLE_MAX_ROLE_NUM];

	int role_num = 0;
	role_list[role_num] = attacker;
	role_data_list[role_num++] = &req.attacker_battle_data;
	role_list[role_num] = defender;
	role_data_list[role_num++] = &req.defender_battle_data;

	int battle_server_index = this->MakeBattleLocalInfoMulti(role_num, role_list, battle_id, battle_mode, role_data_list, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(attacker, battle_id);
		this->OnRoleStartFight(defender, battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);

	return ret;
}

int BattleManagerLocal::StartIntSinglePVPReq(Role* attacker, Role* defender, int battle_mode, const BattleData_Business * business_data)
{
	if (NULL == attacker && NULL == defender)
	{
		return RESULT_BOTH_FAIL;
	}

	if (NULL == attacker)
	{
		return RESULT_ATTACKER_FAIL;
	}

	if (NULL == defender)
	{
		return RESULT_DEFENDER_FAIL;
	}

	if (!this->HasBattleServerConnected())
	{
		attacker->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		defender->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return RESULT_BOTH_FAIL;
	}

	const RoleBattleInfo* attacker_info = this->GetRoleBattleInfoConst(attacker->GetUID());
	if (NULL != attacker_info)
	{
#ifdef _DEBUG
		attacker->NoticeNum(errornum::EN_ROLE_IN_BATTLE_7);
#endif
		this->CheckRoleReallyInBattle(attacker, __FUNCTION__);
		return RESULT_ATTACKER_FAIL;
	}

	if (!attacker->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		attacker->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return RESULT_ATTACKER_FAIL;
	}

	if (attacker->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
	{
		attacker->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		defender->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	const RoleBattleInfo* defender_info = this->GetRoleBattleInfoConst(defender->GetUID());
	if (NULL != defender_info)
	{
#ifdef _DEBUG
		defender->NoticeNum(errornum::EN_ROLE_IN_BATTLE_7);
#endif
		this->CheckRoleReallyInBattle(defender, __FUNCTION__);
		return RESULT_DEFENDER_FAIL;
	}

	if (!defender->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		defender->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return RESULT_DEFENDER_FAIL;
	}

	if (defender->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
	{
		attacker->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		defender->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	int battle_id = this->GenBattleID();

	// 判断完后 收集玩家的战斗数据
	static battlegameprotocol::GameBattleStartSinglePVPReq req;

	req.attacker_bgnetid = attacker->GetBGNetID();
	req.attacker_uid = attacker->GetUID();
	req.defender_bgnetid = defender->GetBGNetID();
	req.defender_uid = defender->GetUID();

	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = ANIMATION_SPEED_NORMAL;
	if (business_data != NULL)
	{
		req.business_data = *business_data;
	}

	req.battle_id = battle_id;

	bool is_need_partner = IsBattleModeNeedPartner(battle_mode);
	if (battle_mode == BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI)
	{
		attacker->GetRoleModuleManager()->GetHuaShenManager()->CollectRoleBattleData(&req.attacker_battle_data, false, true);
		defender->GetRoleModuleManager()->GetHuaShenManager()->CollectRoleBattleData(&req.defender_battle_data, false, true);
	}
	else
	{
		attacker->GetRoleModuleManager()->CollectRoleBattleData(&req.attacker_battle_data, is_need_partner, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data->IsUseSystemFood());
		defender->GetRoleModuleManager()->CollectRoleBattleData(&req.defender_battle_data, is_need_partner, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data->IsUseSystemFood());
	}


	static Role* role_list[BATTLE_MAX_ROLE_NUM];
	static RoleBattleData* role_data_list[BATTLE_MAX_ROLE_NUM];

	int role_num = 0;
	role_list[role_num] = attacker;
	role_data_list[role_num++] = &req.attacker_battle_data;
	role_list[role_num] = defender;
	role_data_list[role_num++] = &req.defender_battle_data;

	int battle_server_index = this->MakeBattleLocalInfoMulti(role_num, role_list, battle_id, battle_mode, role_data_list, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(attacker, battle_id);
		this->OnRoleStartFight(defender, battle_id);
	}

	return ret ? START_BATTLE_RESULT_SUCC : RESULT_BOTH_FAIL;
}

int BattleManagerLocal::StartIntSinglePVPReqByRoleId(int attacker_role_id, int defender_role_id, int battle_mode, const BattleData_Business * business_data)
{
	Role * attacker = World::GetInstWorld()->GetSceneManager()->GetRole(attacker_role_id);
	Role * defender = World::GetInstWorld()->GetSceneManager()->GetRole(defender_role_id);

	return this->StartIntSinglePVPReq(attacker, defender, battle_mode, business_data);
}

bool BattleManagerLocal::StartMultiPVPReq(int attacker_num, Role ** attacker_list,
	int defender_num, Role ** defender_list, int battle_mode, const BattleData_Business & business_data,
	int attacker_observer_num /* = 0 */, Role ** attacker_observer_list /* = NULL */,
	int defender_observer_num /* = 0 */, Role ** defender_observer_list /* = NULL */, int ARG_OUT * out_battle_id_ptr /* = NULL */)
{
	if (!this->HasBattleServerConnected())
	{
		for (int i = 0; i < attacker_num; ++i)
		{
			attacker_list[i]->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		}

		for (int i = 0; i < defender_num; ++i)
		{
			defender_list[i]->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		}

		return false;
	}

	for (int i = 0; i < attacker_num; ++i)
	{
		Role* role = attacker_list[i];
		const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
		if (NULL != info)
		{
			// 有人正在打， 不能开打
			return false;
		}

		if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
		{
			// 有人处于不能开打状态，不能开打
			return false;
		}
	}

	for (int i = 0; i < defender_num; ++i)
	{
		Role* role = defender_list[i];
		const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
		if (NULL != info)
		{
			// 有人正在打， 不能开打
			return false;
		}

		if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
		{
			// 有人处于不能开打状态，不能开打
			return false;
		}
	}

	int total_role_num = 0;
	static Role* role_list[BATTLE_MAX_ROLE_NUM];
	memset(role_list, 0, sizeof(role_list));
	static RoleBattleData* role_data_list[BATTLE_MAX_ROLE_NUM];
	memset(role_data_list, 0, sizeof(role_data_list));
	int battle_id = this->GenBattleID();

	static battlegameprotocol::GameBattleStartMultiPVPReq req;
	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = ANIMATION_SPEED_NORMAL;
	req.battle_id = battle_id;
	req.business_data = business_data;

	bool is_need_partner = IsBattleModeNeedPartner(battle_mode);
	bool is_need_team_stand_pos = IsBattleModeNeedTeamStandPos(battle_mode);

	// 收集攻方玩家数据
	req.attacker_num = attacker_num;
	memset(req.attacker_pos_id_list, 0, sizeof(req.attacker_pos_id_list));
	for (int i = 0; i < attacker_num; ++i)
	{
		Role* role = attacker_list[i];
		req.attacker_data_list[i].bg_netid = role->GetBGNetID();
		req.attacker_data_list[i].uid = role->GetUID();

		bool is_leader = (i == 0);

		if (battle_mode == BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI)
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->CollectRoleBattleData(&req.attacker_data_list[i].role_battle_data, false, true);
		}
		else
		{
			role->GetRoleModuleManager()->CollectRoleBattleData(&req.attacker_data_list[i].role_battle_data, is_leader && is_need_partner, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
		}

		role_data_list[total_role_num] = &req.attacker_data_list[i].role_battle_data;
		role_list[total_role_num] = role;
		++total_role_num;

		if (is_need_team_stand_pos && is_leader)
		{
			Team* team = role->GetMyTeam();
			if (NULL != team)
			{
				team->GetPosIdList(req.attacker_pos_id_list, ARRAY_ITEM_COUNT(req.attacker_pos_id_list), true);
			}
		}
	}

	// 收集守方玩家数据
	req.defender_num = defender_num;
	memset(req.defender_pos_id_list, 0, sizeof(req.defender_pos_id_list));
	for (int i = 0; i < defender_num; ++i)
	{
		Role* role = defender_list[i];
		req.defender_data_list[i].bg_netid = role->GetBGNetID();
		req.defender_data_list[i].uid = role->GetUID();

		bool is_leader = (i == 0);

		if (battle_mode == BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI)
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->CollectRoleBattleData(&req.defender_data_list[i].role_battle_data, false, true);
		}
		else
		{
			role->GetRoleModuleManager()->CollectRoleBattleData(&req.defender_data_list[i].role_battle_data, is_leader && is_need_partner, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
		}

		role_data_list[total_role_num] = &req.defender_data_list[i].role_battle_data;
		role_list[total_role_num] = role;
		++total_role_num;

		if (is_need_team_stand_pos && is_leader)
		{
			Team* team = role->GetMyTeam();
			if (NULL != team)
			{
				team->GetPosIdList(req.defender_pos_id_list, ARRAY_ITEM_COUNT(req.defender_pos_id_list), true);
			}
		}
	}

	int real_observer_num = 0;
	for (int i = 0; i < attacker_observer_num && i < BATTLE_OBSERVER_MAX_NUM && real_observer_num < BATTLE_OBSERVER_MAX_NUM; ++i)
	{
		req.observer[real_observer_num].bg_netid = attacker_observer_list[i]->GetBGNetID();
		req.observer[real_observer_num].uid = attacker_observer_list[i]->GetUID();
		req.observer[real_observer_num].side = BATTLE_SIDE_TYPE_ATTACKER;
		++real_observer_num;
	}

	for (int i = 0; i < defender_observer_num && i < BATTLE_OBSERVER_MAX_NUM && real_observer_num < BATTLE_OBSERVER_MAX_NUM; ++i)
	{
		req.observer[real_observer_num].bg_netid = defender_observer_list[i]->GetBGNetID();
		req.observer[real_observer_num].uid = defender_observer_list[i]->GetUID();
		req.observer[real_observer_num].side = BATTLE_SIDE_TYPE_DEFENDER;
		++real_observer_num;
	}
	req.observer_num = real_observer_num;

	int battle_server_index = this->MakeBattleLocalInfoMulti(total_role_num, role_list, battle_id, battle_mode, role_data_list, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		for (int i = 0; i < total_role_num; ++i)
		{
			this->OnRoleStartFight(role_list[i], battle_id);
		}

		if (out_battle_id_ptr != NULL)
		{
			*out_battle_id_ptr = battle_id;
		}
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	return ret;
}

int BattleManagerLocal::StartMultiPVPReq2(std::vector<Role *> attacker_list, std::vector<Role *> defender_list, const std::vector<RoleBattleData>  & attacker_robot_list, const  std::vector<RoleBattleData> & defender_robot_list, int battle_mode, const BattleData_Business & business_data, int ARG_OUT * out_battle_id_ptr)
{
	if (!this->HasBattleServerConnected())
	{
		for (int i = 0; i < (int)attacker_list.size(); ++i)
		{
			attacker_list[i]->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		}

		for (int i = 0; i < (int)defender_list.size(); ++i)
		{
			defender_list[i]->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		}

		return -__LINE__;
	}

	for (int i = 0; i < (int)attacker_list.size(); ++i)
	{
		Role * role = attacker_list[i];
		if (!role)
		{
			return -__LINE__;
		}

		const RoleBattleInfo * info = this->GetRoleBattleInfoConst(role->GetUID());
		if (NULL != info)
		{
			// 有人正在打， 不能开打
			return -__LINE__;
		}

		if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
		{
			// 有人处于不能开打状态，不能开打
			return -__LINE__;
		}
	}

	for (int i = 0; i < (int)defender_list.size(); ++i)
	{
		Role * role = defender_list[i];
		if (!role)
		{
			return -__LINE__;
		}

		const RoleBattleInfo * info = this->GetRoleBattleInfoConst(role->GetUID());
		if (NULL != info)
		{
			// 有人正在打， 不能开打
			return -__LINE__;
		}

		if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
		{
			// 有人处于不能开打状态，不能开打
			return -__LINE__;;
		}
	}

	std::vector< Role *> role_list;
	std::vector < RoleBattleData * >role_data_list;

	int battle_id = this->GenBattleID();
	static battlegameprotocol::GameBattleStartMultiPVPReq req;
	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = ANIMATION_SPEED_NORMAL;
	req.battle_id = battle_id;
	req.business_data = business_data;

	bool is_need_partner = IsBattleModeNeedPartner(battle_mode);
	bool is_need_team_stand_pos = IsBattleModeNeedTeamStandPos(battle_mode);

	// 收集攻方玩家数据	
	memset(req.attacker_pos_id_list, 0, sizeof(req.attacker_pos_id_list));

	req.attacker_num = 0;
	for (int i = 0; i < (int)attacker_list.size() && req.attacker_num < SIDE_MAX_ROLE_NUM; ++i)
	{
		Role * role = attacker_list[i];
		req.attacker_data_list[req.attacker_num].bg_netid = role->GetBGNetID();
		req.attacker_data_list[req.attacker_num].uid = role->GetUID();

		bool is_leader = (i == 0);
		if (battle_mode == BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI)
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->CollectRoleBattleData(&req.attacker_data_list[req.attacker_num].role_battle_data, is_leader && is_need_partner, true);
		}
		else
		{
			role->GetRoleModuleManager()->CollectRoleBattleData(&req.attacker_data_list[req.attacker_num].role_battle_data, is_leader && is_need_partner, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
		}
		role_data_list.push_back(&req.attacker_data_list[req.attacker_num].role_battle_data);
		role_list.push_back(role);

		req.attacker_num += 1;
	}

	for (int i = 0; i < (int)attacker_robot_list.size() && req.attacker_num < SIDE_MAX_ROLE_NUM; ++i)
	{
		req.attacker_data_list[req.attacker_num].bg_netid = INVALID_BGNETID;
		req.attacker_data_list[req.attacker_num].uid = attacker_robot_list[i].role_character_data.character_id;
		req.attacker_data_list[req.attacker_num].role_battle_data = attacker_robot_list[i];

		req.attacker_num += 1;
	}

	// 收集守方玩家数据
	memset(req.defender_pos_id_list, 0, sizeof(req.defender_pos_id_list));
	req.defender_num = 0;
	for (int i = 0; i < (int)defender_list.size(); ++i)
	{
		Role * role = defender_list[i];
		req.defender_data_list[req.defender_num].bg_netid = role->GetBGNetID();
		req.defender_data_list[req.defender_num].uid = role->GetUID();

		bool is_leader = (i == 0);
		if (battle_mode == BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI)
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->CollectRoleBattleData(&req.defender_data_list[req.defender_num].role_battle_data, is_leader && is_need_partner, true);
		}
		else
		{
			role->GetRoleModuleManager()->CollectRoleBattleData(&req.defender_data_list[req.defender_num].role_battle_data, is_leader && is_need_partner, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
		}
		role_data_list.push_back(&req.defender_data_list[req.defender_num].role_battle_data);
		role_list.push_back(role);

		req.defender_num += 1;
	}

	for (int i = 0; i < (int)defender_robot_list.size() && req.defender_num < SIDE_MAX_ROLE_NUM; ++i)
	{
		req.defender_data_list[req.defender_num].bg_netid = INVALID_BGNETID;
		req.defender_data_list[req.defender_num].uid = defender_robot_list[i].role_character_data.character_id;
		req.defender_data_list[req.defender_num].role_battle_data = defender_robot_list[i];

		req.defender_num += 1;
	}

	if (role_list.empty() || role_data_list.empty())
	{
		return -__LINE__;
	}

	int battle_server_index = this->MakeBattleLocalInfoMulti((int)role_list.size(), &role_list[0], battle_id, battle_mode, &role_data_list[0], 0, 0);
	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		for (int i = 0; i < (int)role_list.size(); ++i)
		{
			this->OnRoleStartFight(role_list[i], battle_id);
		}

		if (out_battle_id_ptr != NULL)
		{
			*out_battle_id_ptr = battle_id;
		}
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	return ret ? 0 : -__LINE__;
}

bool BattleManagerLocal::StartSpecialPVPReq(Role * attacker, Role * defender, int battle_mode, const BattleData_Business & business_data)
{
	if (NULL == attacker || NULL == defender) return false;

	if (!this->HasBattleServerConnected())
	{
		attacker->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		defender->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* attacker_info = this->GetRoleBattleInfoConst(attacker->GetUID());
	if (NULL != attacker_info)
	{
#ifdef _DEBUG
		attacker->NoticeNum(errornum::EN_ROLE_IN_BATTLE_6);
#endif

		this->CheckRoleReallyInBattle(attacker, __FUNCTION__);
		return false;
	}

	if (!attacker->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		attacker->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	if (attacker->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
	{
		attacker->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		defender->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	const RoleBattleInfo* defender_info = this->GetRoleBattleInfoConst(defender->GetUID());
	if (NULL != defender_info)
	{
#ifdef _DEBUG
		defender->NoticeNum(errornum::EN_ROLE_IN_BATTLE_6);
#endif

		this->CheckRoleReallyInBattle(defender, __FUNCTION__);
		return false;
	}

	if (!defender->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		defender->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	if (attacker->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
	{
		attacker->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		defender->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	int battle_id = this->GenBattleID();

	// 判断完后 收集玩家的战斗数据
	static battlegameprotocol::GameBattleStartSpecialReq req;

	req.attacker_data[0].bg_netid = attacker->GetBGNetID();
	req.attacker_data[0].uid = attacker->GetUID();
	req.attacker_data[0].m_avatar_timestamp = attacker->GetAvatarTimestamp();
	req.attacker_data[0].m_avatar_type = attacker->GetAvatarType();
	attacker->GetName(req.attacker_data[0].role_name);
	req.attacker_count = 1;

	req.defender_data[0].bg_netid = defender->GetBGNetID();
	req.defender_data[0].uid = defender->GetUID();
	req.defender_data[0].m_avatar_timestamp = defender->GetAvatarTimestamp();
	req.defender_data[0].m_avatar_type = defender->GetAvatarType();
	defender->GetName(req.defender_data[0].role_name);
	req.defender_count = 1;

	req.battle_mode = static_cast<short>(battle_mode);
	req.battle_id = battle_id;

	static Role* role_list[BATTLE_MAX_ROLE_NUM];
	static RoleBattleData* role_data_list[BATTLE_MAX_ROLE_NUM];

	int role_num = 0;
	role_list[role_num++] = attacker;
	role_list[role_num++] = defender;

	int battle_server_index = this->MakeBattleLocalInfoMulti(role_num, role_list, battle_id, battle_mode, role_data_list, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(attacker, battle_id);
		this->OnRoleStartFight(defender, battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);

	return ret;
}

bool BattleManagerLocal::StartSingleFightShadowReq(Role* role, const RoleBattleData& defender_data, int battle_mode, const BattleData_Business& business_data)
{
	if (NULL == role) return false;

	RoleBattleData attacker_data;
	role->GetRoleModuleManager()->CollectRoleBattleData(&attacker_data, true, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());

	return this->StartSingleFightShadowReq(role, attacker_data, defender_data, battle_mode, business_data);
}

bool BattleManagerLocal::StartSingleFightShadowReq(Role* role, const RoleBattleData& attacker_data, const RoleBattleData& defender_data, int battle_mode, const BattleData_Business& business_data)
{
	if (NULL == role) return false;

	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	const RoleBattleInfo* attacker_info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != attacker_info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_8);
#endif
		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}

	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	int battle_id = this->GenBattleID();
	int animation_mode = role->GetRoleModuleManager()->GetCommonData().animation_speed; // 单人PVE默认使用角色自身速度设置
	bool is_force_speed = false;
	bool is_mine_monster = false;
	this->GetRoleAnimationSpeed(role, battle_mode, is_mine_monster, false, &is_force_speed, &animation_mode);

	// 判断完后 收集玩家的战斗数据
	static battlegameprotocol::GameBattleStartSingleFightShadowReq req;

	req.attacker_bgnetid = role->GetBGNetID();
	req.attacker_uid = role->GetUID();

	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = animation_mode;
	req.business_data = business_data;
	req.business_data.is_force_speed = is_force_speed ? 1 : 0;

	req.battle_id = battle_id;

	req.attacker_battle_data = attacker_data;
	req.defender_battle_data = defender_data;

	int battle_server_index = this->MakeBattleLocalInfoSingle(role, battle_id, battle_mode, &req.attacker_battle_data, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(role, battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	return ret;
}

bool BattleManagerLocal::StartMultiFightShadowReq(int attacker_num, Role** attacker_list, int defender_num, const RoleBattleData* defender_data_list, int battle_mode, const BattleData_Business& business_data)
{
	if (!this->HasBattleServerConnected())
	{
		for (int i = 0; i < attacker_num; ++i)
		{
			attacker_list[i]->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		}

		return false;
	}

	for (int i = 0; i < attacker_num; ++i)
	{
		Role* role = attacker_list[i];
		const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
		if (NULL != info)
		{
			// 有人正在打， 不能开打
			return false;
		}

		if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
		{
			// 有人处于不能开打状态，不能开打
			return false;
		}
	}

	int total_role_num = 0;
	static Role* role_list[BATTLE_MAX_ROLE_NUM];
	memset(role_list, 0, sizeof(role_list));
	static RoleBattleData* role_data_list[BATTLE_MAX_ROLE_NUM];
	memset(role_data_list, 0, sizeof(role_data_list));
	int battle_id = this->GenBattleID();

	static battlegameprotocol::GameBattleStartMultiFightShadowReq req;
	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = ANIMATION_SPEED_NORMAL;
	req.battle_id = battle_id;
	req.business_data = business_data;

	// 收集攻方玩家数据
	bool is_need_partner = IsBattleModeNeedPartner(battle_mode);
	req.attacker_num = 0;
	for (int i = 0; i < attacker_num && req.attacker_num < SIDE_MAX_ROLE_NUM; ++i)
	{
		Role* role = attacker_list[i];
		req.attacker_list[req.attacker_num].bgnetid = role->GetBGNetID();
		req.attacker_list[req.attacker_num].uid = role->GetUID();

		bool is_leader = (i == 0);

		role->GetRoleModuleManager()->CollectRoleBattleData(&req.attacker_data_list[req.attacker_num], is_leader && is_need_partner, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
		role_data_list[total_role_num] = &req.attacker_data_list[req.attacker_num];
		role_list[total_role_num] = role;
		++total_role_num;
		++req.attacker_num;
	}

	// 收集守方数据
	req.defender_num = 0;
	for (int i = 0; i < defender_num && req.defender_num < SIDE_MAX_ROLE_NUM; ++i)
	{
		req.defender_data_list[req.defender_num++] = defender_data_list[i];
	}

	int battle_server_index = this->MakeBattleLocalInfoMulti(total_role_num, role_list, battle_id, battle_mode, role_data_list, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		for (int i = 0; i < total_role_num; ++i)
		{
			this->OnRoleStartFight(role_list[i], battle_id);
		}
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	return ret;
}

bool BattleManagerLocal::StartSpecialBattleJoyReq(Role * attacker, Role * defender)
{
	if (NULL == attacker || NULL == defender) return false;

	int battle_mode = BATTLE_MODE_JOY;
	static BattleData_Business business;
	business.Reset();
	this->MarkBattleLaunchInfo(business, attacker, __FUNCTION__);

	return this->StartSpecialPVPReq(attacker, defender, battle_mode, business);
}

bool BattleManagerLocal::StartSingleArenaReq(Role * role, int defender_uid, int defender_buff_id, int max_round, const RoleBattleData & defender_data)
{
	if (NULL == role) return false;

	int battle_mode = BATTLE_MODE_SINGLE_ARENA;
	RoleBattleData attacker_data;
	if (!role->GetRoleModuleManager()->GetPetHelperFormation()->CollectBattleData(battle_mode, attacker_data, false))
	{
		role->GetRoleModuleManager()->CollectRoleBattleData(&attacker_data, true, IsBattleModeMustUseMaxHpMp(battle_mode));
	}

	// 玩家前两次进入且低于40级时提示
	Gladiator* gladitor = ColosseumManager::Instance().GetUserGladiator(role->GetUserId());
	if (gladitor == NULL) return false;

	if (role->GetLevel() < 40 && gladitor->colosseum_param.tips_times <= 2)
	{
		bool partner_status1 = false, partner_status2 = false;
		for (int i = 0; i < SIDE_MAX_ROLE_NUM - 1; ++i)
		{
			if (attacker_data.partner_character_data[i].battle_data.character_id == 5)
			{
				partner_status1 = true;
			}

			if (attacker_data.partner_character_data[i].battle_data.character_id == 22)
			{
				partner_status2 = true;
			}
		}

		if (!partner_status1 && !partner_status2)
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_LACK_AOE_AND_TREAT_PARTNER);
		}
		else if (!partner_status1 && partner_status2)
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_LACK_TREAT_PARTNER);
		}
		else if (partner_status1 && !partner_status2)
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_LACK_AOE_PARTNER);
		}

		gladitor->colosseum_param.tips_times++;
	}

	static BattleData_Business business;
	business.Reset();
	business.colosseum_target_uid = defender_uid;
	business.until_one_side_all_dead = 0;
	business.limit_max_round = max_round;
	business.is_allow_skip_battle = 1;

	int buff_id_list[BATTLE_BUSINESS_BUFF_MAX_NUM];
	int buff_num = 0;
	ColosseumManager::Instance().GetDefenderBuffid(BATTLE_BUSINESS_BUFF_MAX_NUM, &buff_num, buff_id_list);
	if (buff_num > 0)
	{
		for (int i = 0; i < buff_num; ++i)
		{
			business.defender_buff_id_list[i] = buff_id_list[i];
			business.defender_buff_lv_list[i] = 1;
		}
		business.defender_buff_num = buff_num;
	}

	this->MarkBattleLaunchInfo(business, role, __FUNCTION__);

	return this->StartSingleFightShadowReq(role, attacker_data, defender_data, battle_mode, business);
}

bool BattleManagerLocal::StartCloudArenaReq(Role* role, const RoleBattleData& attacker_data, int defender_uid, int defender_buff_num, int* defender_buff_list, int max_round, int max_time_s, const RoleBattleData & defender_data, int allow_surrender_round)
{
	if (NULL == role) return false;

	int battle_mode = BATTLE_MODE_CLOUD_ARENA;

	static BattleData_Business business;
	business.Reset();
	business.cloud_arena_target_uid = defender_uid;
	business.until_one_side_all_dead = 0;
	business.limit_max_round = max_round;
	business.limit_max_time = max_time_s;
	business.no_sync_auto_move_setting = 0;
	business.only_sync_pet_auto_move = 1;
	business.allow_surrender_round_num = allow_surrender_round;

	if (defender_buff_num > 0)
	{
		for (int i = 0; i < defender_buff_num && i < BATTLE_BUSINESS_BUFF_MAX_NUM; ++i)
		{
			business.defender_buff_id_list[i] = defender_buff_list[i];
			business.defender_buff_lv_list[i] = 1;
		}
		business.defender_buff_num = defender_buff_num;
	}

	this->MarkBattleLaunchInfo(business, role, __FUNCTION__);

	return this->StartSingleFightShadowReq(role, attacker_data, defender_data, battle_mode, business);
}

int BattleManagerLocal::StartChiefElectionPVPReq(Role* attacker, Role* defender, int which_prof_match, int max_round, int time, int field_buff_id)
{
	BattleData_Business battle_data;
	battle_data.limit_max_round = max_round;
	battle_data.until_one_side_all_dead = 1;
	battle_data.limit_max_time = time;
	battle_data.param1 = which_prof_match;
	battle_data.attacker_sidebuff_id = field_buff_id;
	battle_data.defender_sidebuff_id = field_buff_id;

	if (attacker != NULL)
	{
		battle_data.param2 = attacker->GetRoleModuleManager()->GetCapability()->GetRoleCapability() + attacker->GetRoleModuleManager()->GetPet()->GetFightPetCapability();
	}

	if (defender != NULL)
	{
		battle_data.param3 = defender->GetRoleModuleManager()->GetCapability()->GetRoleCapability() + defender->GetRoleModuleManager()->GetPet()->GetFightPetCapability();;
	}

	this->MarkBattleLaunchInfo(battle_data, attacker, __FUNCTION__);

#ifdef _DEBUG
	battle_data.until_one_side_all_dead = 0;
#endif

	int result = BattleManagerLocal::GetInstance().StartIntSinglePVPReq(attacker, defender, BATTLE_MODE_CHIEF_ELECTION, &battle_data);

	return result;
}

int BattleManagerLocal::StartChiefPeakBattlePVPReq(Role * attacker, Role * defender, int max_round, int time)
{
	BattleData_Business battle_data;
	battle_data.limit_max_round = max_round;
	battle_data.until_one_side_all_dead = 1;
	battle_data.limit_max_time = time;
	this->MarkBattleLaunchInfo(battle_data, attacker, __FUNCTION__);

#ifdef _DEBUG
	battle_data.until_one_side_all_dead = 0;
#endif

	int result = BattleManagerLocal::GetInstance().StartIntSinglePVPReq(attacker, defender, BATTLE_MODE_CHIEF_PEAK_BATTLE, &battle_data);

	return result;
}

bool BattleManagerLocal::StartHuanJieZhanChangPVPReq(std::vector<Role *> attacker_list, std::vector<Role *> defender_list, const std::vector<RoleBattleData> & attacker_robot_list, const std::vector<RoleBattleData> & defender_robot_list, int max_round, int fight_seq)
{
	if (attacker_list.empty() && attacker_robot_list.empty())
	{
		gamelog::g_log_battle.printf(LL_DEBUG, "%s fail , line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	if (defender_list.empty() && defender_robot_list.empty())
	{
		gamelog::g_log_battle.printf(LL_DEBUG, "%s fail , line:%d", __FUNCTION__, __LINE__);
		return false;
	}

	BattleData_Business battle_data;
	battle_data.limit_max_round = max_round;
	battle_data.until_one_side_all_dead = 0;
	battle_data.role_runaway_always_fail = 1;
	battle_data.param1 = fight_seq;
	battle_data.allow_surrender_round_num = 2;

	int ret = BattleManagerLocal::GetInstance().StartMultiPVPReq2(attacker_list, defender_list, attacker_robot_list, defender_robot_list, BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI, battle_data);
	if (ret != 0)
	{
		gamelog::g_log_battle.printf(LL_DEBUG, "BattleManagerLocal::StartMultiPVPReq2 fail , ret:%d", ret);
	}

	return ret == 0;
}

bool BattleManagerLocal::StartPlatformBattleReq(Role* attacker, int atk_pet_num,
	PetBattleData* atk_pet_list, int * atk_pet_pos_info, int atk_partner_num, PartnerBattleData* atk_partner_list, int * atk_partner_pos_info,
	UserID def_user_id, int def_pet_num, PetBattleData* def_pet_list, int *def_pet_pos_info, int def_partner_num,
	PartnerBattleData* def_partner_list, int * def_partner_pos_info, const BattleData_Business& business_data)
{
	if (!this->HasBattleServerConnected())
	{
		attacker->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	if (atk_pet_num < 0 || atk_pet_num > SIDE_MAX_ROLE_NUM) return false;
	if (atk_partner_num < 0 || atk_partner_num > SIDE_MAX_ROLE_NUM) return false;
	if (def_pet_num < 0 || def_pet_num > SIDE_MAX_ROLE_NUM) return false;
	if (def_partner_num < 0 || def_partner_num > SIDE_MAX_ROLE_NUM) return false;

	if (atk_pet_num != 0 && (atk_pet_list == NULL || atk_pet_pos_info == NULL))
	{
		return false;
	}

	if (atk_partner_num != 0 && (atk_partner_list == NULL || atk_partner_pos_info == NULL))
	{
		return false;
	}

	if (def_pet_num != 0 && (def_pet_list == NULL || def_pet_pos_info == NULL))
	{
		return false;
	}

	if (def_partner_list != 0 && NULL == def_partner_pos_info)
	{
		return false;
	}

	const RoleBattleInfo* attacker_info = this->GetRoleBattleInfoConst(attacker->GetUID());
	if (NULL != attacker_info)
	{
#ifdef _DEBUG
		attacker->NoticeNum(errornum::EN_ROLE_IN_BATTLE_11);
#endif

		this->CheckRoleReallyInBattle(attacker, __FUNCTION__);
		return false;
	}

	if (!attacker->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_PLATFORM_BATTLE))
	{
		attacker->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	static battlegameprotocol::GameBattleStartPlatformFightReq req;
	req.bg_netid = attacker->GetBGNetID();
	req.attacker_uid = attacker->GetUID();
	req.atk_pet_num = atk_pet_num;
	req.atk_partner_num = atk_partner_num;
	for (int i = 0; i < req.atk_pet_num; ++i)
	{
		req.atk_pet_list[i] = atk_pet_list[i];
		req.atk_pet_pos_list[i] = atk_pet_pos_info[i];
	}
	for (int i = 0; i < req.atk_partner_num; ++i)
	{
		req.atk_partner_list[i] = atk_partner_list[i];
		req.atk_partner_pos_list[i] = atk_partner_pos_info[i];
	}
	req.def_pet_num = def_pet_num;
	req.def_partner_num = def_partner_num;

	if (NULL != def_pet_list && NULL != def_pet_pos_info)
	{
		for (int i = 0; i < req.def_pet_num; ++i)
		{
			req.def_pet_list[i] = def_pet_list[i];
			req.def_pet_pos_list[i] = def_pet_pos_info[i];
		}
	}

	if (NULL != def_partner_list && NULL != def_partner_pos_info)
	{
		for (int i = 0; i < req.def_partner_num; ++i)
		{
			req.def_partner_list[i] = def_partner_list[i];
			req.def_partner_pos_list[i] = def_partner_pos_info[i];
		}
	}

	req.business_data = business_data;

	// 获取本地战斗ID
	int battle_id = this->GenBattleID();
	req.battle_id = battle_id;
	req.business_data.until_one_side_all_dead = 1; // 要打到双方角色全死光才可以结束
	req.business_data.limit_max_round = 20;
	req.animation_speed_mode = attacker->GetRoleModuleManager()->GetCommonData().animation_speed; // 单人PVE默认使用角色自身速度设置
	req.business_data.is_force_speed = 0;
	req.business_data.is_allow_skip_battle = 1;

	int battle_server_index = this->MakeBattleLocalInfoSingle(attacker, battle_id, BATTLE_MODE_PLATFORM_BATTLE, NULL, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(attacker, battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	return ret;
}

bool BattleManagerLocal::StartPartnerPVEReq(Role* attacker, int atk_partner_num, PartnerBattleData* atk_partner_list, int * atk_partner_pos_info, int monster_group_id, const BattleData_Business& business_data)
{
	if (!this->HasBattleServerConnected())
	{
		attacker->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	if (atk_partner_num < 0 || atk_partner_num > SIDE_MAX_ROLE_NUM) return false;

	if (atk_partner_num != 0 && (atk_partner_list == NULL || atk_partner_pos_info == NULL))
	{
		return false;
	}

	const RoleBattleInfo* attacker_info = this->GetRoleBattleInfoConst(attacker->GetUID());
	if (NULL != attacker_info)
	{
#ifdef _DEBUG
		attacker->NoticeNum(errornum::EN_ROLE_IN_BATTLE_11);
#endif

		this->CheckRoleReallyInBattle(attacker, __FUNCTION__);
		return false;
	}

	if (!attacker->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_PARTNER_PVE))
	{
		attacker->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	static battlegameprotocol::GameBattleStartPartnerPVEReq req;

	req.battle_mode = BATTLE_MODE_PARTNER_PVE;
	req.animation_speed_mode = ANIMATION_SPEED_DOUBLE;
	req.monster_group_id = monster_group_id;

	req.bg_netid = attacker->GetBGNetID();
	req.attacker_uid = attacker->GetUID();
	req.atk_partner_num = atk_partner_num;
	for (int i = 0; i < req.atk_partner_num; ++i)
	{
		req.atk_partner_list[i] = atk_partner_list[i];
		req.atk_partner_pos_list[i] = atk_partner_pos_info[i];
	}

	req.business_data = business_data;
	req.business_data.until_one_side_all_dead = 1; // 要打到双方角色全死光才可以结束

	int fast_forward_rounds = 0;
	if (LOGIC_CONFIG->GetFastForwardBattleConfig()->GetFastBattleRound(attacker->GetLevel(), req.battle_mode, &fast_forward_rounds))
	{
		req.business_data.allow_fast_forward_round_num = fast_forward_rounds;
	}

	// 获取本地战斗ID
	int battle_id = this->GenBattleID();
	req.battle_id = battle_id;

	int battle_server_index = this->MakeBattleLocalInfoSingle(attacker, battle_id, BATTLE_MODE_PARTNER_PVE, NULL, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		this->OnRoleStartFight(attacker, battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	return ret;
}

bool BattleManagerLocal::StartSinglePVPReq2(Character* attacker, Character* defender, int battle_mode, const BattleData_Business& business_data, ARG_OUT bool* attacker_failed, ARG_OUT bool* defender_failed)
{
	if (NULL == attacker || NULL == defender)
	{
		*attacker_failed = true;
		*defender_failed = true;
		return false;
	}

	Role* attacker_role = dynamic_cast<Role*>(attacker);
	Role* defender_role = dynamic_cast<Role*>(defender);
	Robot* attacker_robot = dynamic_cast<Robot*>(attacker);
	Robot* defender_robot = dynamic_cast<Robot*>(defender);

	if (NULL == attacker_role && NULL == attacker_robot)
	{
		// 这情况算系统的错
		*attacker_failed = true;
		*defender_failed = true;
		return false;
	}
	if (NULL == defender_role && NULL == defender_robot)
	{
		// 这情况算系统的错
		*attacker_failed = true;
		*defender_failed = true;
		return false;
	}

	if (!this->HasBattleServerConnected())
	{
		if (NULL != attacker_role) attacker_role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		if (NULL != defender_role) defender_role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);

		// 这情况算系统的错
		*attacker_failed = true;
		*defender_failed = true;
		return false;
	}

	if (NULL != attacker_role)
	{
		if (!this->CanRoleStartBattle(attacker_role, battle_mode))
		{
			*attacker_failed = true;
			return false;
		}
	}

	if (NULL != defender_role)
	{
		if (!this->CanRoleStartBattle(defender_role, battle_mode))
		{
			*defender_failed = true;
			return false;
		}
	}

	int battle_id = this->GenBattleID();

	// 判断完后 收集玩家的战斗数据
	static battlegameprotocol::GameBattleStartSinglePVPReq req;
	req.battle_mode = static_cast<short>(battle_mode);
	req.animation_speed_mode = ANIMATION_SPEED_NORMAL;
	req.business_data = business_data;
	req.battle_id = battle_id;
	req.attacker_battle_data.Reset();
	req.defender_battle_data.Reset();

	if (NULL != attacker_role)
	{
		req.attacker_bgnetid = attacker_role->GetBGNetID();
		req.attacker_uid = attacker_role->GetUID();
		req.attacker_battle_data.is_robot = 0;
		if (!attacker_role->GetRoleModuleManager()->GetPetHelperFormation()->CollectBattleData(battle_mode, req.attacker_battle_data, business_data.IsUseSystemFood()))
		{
			attacker_role->GetRoleModuleManager()->CollectRoleBattleData(&req.attacker_battle_data, true, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
		}
	}
	else if (NULL != attacker_robot)
	{
		req.attacker_bgnetid = INVALID_BGNETID;
		req.attacker_uid = attacker_robot->GetRobotID();
		req.attacker_battle_data = attacker_robot->GetRoleBattleData();
		req.attacker_battle_data.role_character_data.character_type = BATTLE_CHARACTER_TYPE_ROLE_SHADOW;
		req.attacker_battle_data.is_robot = 1;
		if (business_data.IsUseSystemFood())
		{
			RoleNewLifeSkill::AddSystemFoorAttr(&req.attacker_battle_data.role_character_data);
		}
	}

	if (NULL != defender_role)
	{
		req.defender_bgnetid = defender_role->GetBGNetID();
		req.defender_uid = defender_role->GetUID();
		req.defender_battle_data.is_robot = 0;
		if (!defender_role->GetRoleModuleManager()->GetPetHelperFormation()->CollectBattleData(battle_mode, req.defender_battle_data, business_data.IsUseSystemFood()))
		{
			defender_role->GetRoleModuleManager()->CollectRoleBattleData(&req.defender_battle_data, true, IsBattleModeMustUseMaxHpMp(battle_mode), true, business_data.IsUseSystemFood());
		}
	}
	else if (NULL != defender_robot)
	{
		req.defender_bgnetid = INVALID_BGNETID;
		req.defender_uid = defender_robot->GetRobotID();
		req.defender_battle_data = defender_robot->GetRoleBattleData();
		req.defender_battle_data.role_character_data.character_type = BATTLE_CHARACTER_TYPE_ROLE_SHADOW;
		req.defender_battle_data.is_robot = 1;
		if (business_data.IsUseSystemFood())
		{
			RoleNewLifeSkill::AddSystemFoorAttr(&req.defender_battle_data.role_character_data);
		}
	}

	static Role* role_list[BATTLE_MAX_ROLE_NUM];
	static RoleBattleData* role_data_list[BATTLE_MAX_ROLE_NUM];

	int role_num = 0;
	if (NULL != attacker_role)
	{
		role_list[role_num] = attacker_role;
		role_data_list[role_num++] = &req.attacker_battle_data;
	}

	if (NULL != defender_role)
	{
		role_list[role_num] = defender_role;
		role_data_list[role_num++] = &req.defender_battle_data;
	}

	int battle_server_index = this->MakeBattleLocalInfoMulti(role_num, role_list, battle_id, battle_mode, role_data_list, 0, 0);

	bool ret = World::GetInstWorld()->SendSerializeMsgToOneBattle(battle_server_index, &req);
	if (ret)
	{
		// 标记玩家已开始战斗
		if (NULL != attacker_role) this->OnRoleStartFight(attacker_role, battle_id);
		if (NULL != defender_role) this->OnRoleStartFight(defender_role, battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);

	return ret;
}

bool BattleManagerLocal::SendQieCuoReqToTarget(Role * role, int enemy_uid)
{
	if (!this->HasBattleServerConnected())
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		return false;
	}

	Role * enemy = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(enemy_uid));
	if (NULL == enemy)
	{
		role->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		return false;
	}

	if (enemy->InTeam() && !enemy->IsLeader())
	{
		role->NoticeNum(errornum::EN_TARGET_IS_NOT_LEADER);
		return false;
	}

	if (enemy->GetRoleStatusManager()->IsInBattleStatus())
	{
		role->NoticeNum(errornum::EN_TARGET_IS_IN_BATLLE);
		return false;
	}

	int myself_uid = role->GetUID();
	bool need_send = false;
	unsigned int now_sec = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	std::map<int, std::deque<QieCuoReqCon> >::iterator find_it = m_qie_cuo_fetch_map.find(enemy_uid);
	if (m_qie_cuo_fetch_map.end() != find_it)
	{
		if (find_it->second.empty())
		{
			need_send = true;
		}
		else
		{
			for (std::deque<QieCuoReqCon>::iterator loop_check = find_it->second.begin(); loop_check != find_it->second.end();)
			{
				if (loop_check->role_id == role->GetUID())		// 如果请求早已发送过
				{
					if (now_sec <= loop_check->time_out_timestamp)	// 且请求还没超时，提示耐心等待
					{
						role->NoticeNum(errornum::EN_QIE_CUO_REQ_ALREADY_SEND);
						return false;
					}
					else		// 否则，请求早已超时，清理这个请求
					{
						loop_check = find_it->second.erase(loop_check);
					}
				}
				else
				{
					++loop_check;
				}
			}
		}

		find_it->second.push_back(QieCuoReqCon(myself_uid, now_sec + 30));
	}
	else
	{
		std::deque<QieCuoReqCon> tmp_req_deque; tmp_req_deque.push_back(QieCuoReqCon(myself_uid, now_sec + 30));
		m_qie_cuo_fetch_map[enemy_uid] = tmp_req_deque;

		need_send = true;
	}

	if (need_send)
	{
		Protocol::SCOthersReqQieCuoWithYou orqcwy;
		orqcwy.role_id = myself_uid;
		role->GetName(orqcwy.sponsor_name);
		enemy->GetRoleModuleManager()->NetSend((const void *)&orqcwy, sizeof(orqcwy));
	}

	role->NoticeNum(noticenum::NT_FIGHT_REQUEST_ALREADY_SEND);
	return true;
}

bool BattleManagerLocal::OnQieCuoRequestReply(Role * role, int target_uid, bool is_agree)
{
	if (NULL == role) return false;

	int my_uid = role->GetUID();
	std::map<int, std::deque<QieCuoReqCon> >::iterator find_it = m_qie_cuo_fetch_map.find(my_uid);
	if (m_qie_cuo_fetch_map.end() == find_it || find_it->second.empty())
	{
		role->NoticeNum(errornum::EN_REQ_ALREADY_TIMEOUT);
		return false;
	}

	bool found = false; QieCuoReqCon tmp_req_con = find_it->second.front();
	if (tmp_req_con.role_id != target_uid)		// 从切磋请求队列中删掉这个请求
	{
		for (std::deque<QieCuoReqCon>::iterator loop = find_it->second.begin(); loop != find_it->second.end(); ++loop)
		{
			if (loop->role_id == target_uid)
			{
				found = true;
				tmp_req_con = *loop;
				find_it->second.erase(loop);
				break;
			}
		}
	}
	else
	{
		found = true;
		find_it->second.pop_front();
	}

	if (!found)
	{
		if (is_agree) role->NoticeNum(errornum::EN_REQ_ALREADY_TIMEOUT);
		return false;
	}

	time_t now_sec = EngineAdapter::Instance().Time();
	if (!is_agree)
	{
		if (now_sec <= tmp_req_con.time_out_timestamp)	// 请求没超时才发，超时了就不发了
		{
			Role * target = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
			if (NULL != target)		// 如果拒绝且拒绝对象在线，发送通知告知
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_refuse_qie_cuo_float, role->GetName());
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
					{
						EngineAdapter::Instance().NetSend(target->GetNetId(), (const char*)&sm, sendlen);
					}
				}
			}
		}

		return false;
	}

	if (now_sec > tmp_req_con.time_out_timestamp)
	{
		role->NoticeNum(errornum::EN_REQ_ALREADY_TIMEOUT);
		return false;
	}

	if (!this->StartChallengeOthersReq(role, target_uid, BATTLE_MODE_NORMAL_PRACTICE))
	{
		return false;
	}

	m_qie_cuo_fetch_map.erase(my_uid);		// 清除切磋请求列表
	return true;
}

void BattleManagerLocal::SendQieCuoDequeFront(Role * role)
{
	std::map<int, std::deque<QieCuoReqCon> >::iterator find_it = m_qie_cuo_fetch_map.find(role->GetUID());
	if (m_qie_cuo_fetch_map.end() == find_it || find_it->second.empty()) return;

	time_t now_sec = EngineAdapter::Instance().Time();
	for (std::deque<QieCuoReqCon>::iterator loop = find_it->second.begin(); loop != find_it->second.end();)
	{
		Role * target = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(loop->role_id));
		if (NULL == target || now_sec > loop->time_out_timestamp)
		{
			loop = find_it->second.erase(loop);
		}
		else
		{
			Protocol::SCOthersReqQieCuoWithYou orqcwy;
			orqcwy.role_id = loop->role_id;
			target->GetName(orqcwy.sponsor_name);
			role->GetRoleModuleManager()->NetSend((const void *)&orqcwy, sizeof(orqcwy));
			break;
		}
	}
}

bool BattleManagerLocal::RoleUseItemReq(int battle_server_index, Role* role, int move_idx, int target_grid_idx, int item_column, int item_index)
{
	if (NULL == role) return false;

	static ItemGridData item_grid_data;

	if (!role->GetRoleModuleManager()->GetKnapsack()->GetItemGridData(item_column, item_index, &item_grid_data))
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_KNAPSACK_LOAD_ERROR);
		role->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return false;
	}

	const ItemBase* item = ITEMPOOL->GetItem(item_grid_data.item_id);
	if (NULL == item)
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_KNAPSACK_LOAD_ERROR);
		role->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return false;
	}

	bool is_use_succ = false;
	if (!role->GetScene()->GetSpecialLogic()->CanUseItem(role, item->GetItemId(), 1))
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_ITEM_USE_ERROR);
		role->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return false;
	}

	if (I_TYPE_EXPENSE == item->GetItemType())
	{
		const NorexItem* expense = (const NorexItem*)item;
		if (NorexItem::I_NOREX_ITEM_HP_RECOVER == expense->GetUseType() || NorexItem::I_NOREX_ITEM_HP_PERCENT_RECOVER == expense->GetUseType())
		{
			static battlegameprotocol::GameBattleRoleUseHealItem req;
			req.bg_netid = role->GetBGNetID();
			req.uid = role->GetUID();
			req.item_column = item_column;
			req.item_index = item_index;
			req.move_idx = move_idx;
			req.item_id = expense->GetItemId();
			req.item_count = item_grid_data.num;
			req.heal_num = expense->GetParam2();
			req.purify_percent = expense->GetParam3();
			req.target_grid_idx = target_grid_idx;
			req.is_realive = 0;
			req.limit_level = expense->GetLimitLevel();
			req.reply_percent = 0;
			req.is_percent = NorexItem::I_NOREX_ITEM_HP_PERCENT_RECOVER == expense->GetUseType() ? 1 : 0;
			req.reserve_sh = 0;

			if (this->IsRoleBattleRequestSendToBigCross(role))
			{
				is_use_succ = true;
				this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
			}
			else
			{
				is_use_succ = World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
			}
		}
		else if (NorexItem::I_NOREX_ITEM_REALIVE == expense->GetUseType())
		{
			static battlegameprotocol::GameBattleRoleUseHealItem req;
			req.bg_netid = role->GetBGNetID();
			req.uid = role->GetUID();
			req.item_column = item_column;
			req.item_index = item_index;
			req.move_idx = move_idx;
			req.item_id = expense->GetItemId();
			req.item_count = item_grid_data.num;
			req.heal_num = expense->GetParam2();
			req.target_grid_idx = target_grid_idx;
			req.is_realive = 1;
			req.limit_level = expense->GetLimitLevel();
			req.reply_percent = expense->GetParam1();

			if (this->IsRoleBattleRequestSendToBigCross(role))
			{
				is_use_succ = true;
				this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
			}
			else
			{
				is_use_succ = World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
			}
		}
		else if (NorexItem::I_NOREX_ITEM_INST_KILL_MONSTER == expense->GetUseType())
		{
			static battlegameprotocol::GameBattleRoleUseInstKillMonsterItem req;
			req.uid = role->GetUID();
			req.bg_netid = role->GetBGNetID();
			req.move_idx = move_idx;
			req.target_grid_idx = target_grid_idx;
			req.monster_id = expense->GetParam1();
			req.instant_kill_item_id = item->GetItemId();
			req.succ_rate = expense->GetParam2();

			if (this->IsRoleBattleRequestSendToBigCross(role))
			{
				is_use_succ = true;
				this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
			}
			else
			{
				is_use_succ = World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
			}
		}
		else if (NorexItem::I_NOREX_ITEM_RECOVER_MP == expense->GetUseType() || NorexItem::I_NOREX_ITEM_MP_PERCENT_RECOVER == expense->GetUseType())
		{
			static battlegameprotocol::GameBattleRoleUseMpItem req;
			req.uid = role->GetUID();
			req.bg_netid = role->GetBGNetID();
			req.item_column = item_column;
			req.item_index = item_index;
			req.item_count = item_grid_data.num;
			req.move_idx = move_idx;
			req.item_id = expense->GetItemId();
			req.mp_value = expense->GetParam2();
			req.target_grid_idx = target_grid_idx;
			req.limit_level = expense->GetLimitLevel();
			req.is_percent = NorexItem::I_NOREX_ITEM_MP_PERCENT_RECOVER == expense->GetUseType() ? 1 : 0;
			req.reserve_sh = 0;

			if (this->IsRoleBattleRequestSendToBigCross(role))
			{
				is_use_succ = true;
				this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
			}
			else
			{
				is_use_succ = World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
			}
		}
	}
	else if (I_TYPE_EQUIPMENT == item->GetItemType())
	{
		const Equipment* equipment = (const Equipment*)item;
		if (Equipment::E_TYPE_WEAPON == equipment->GetEquipType() &&
			(-1 == equipment->GetLimitProf() || role->GetBaseProfession() == equipment->GetLimitProf()))
		{
			static battlegameprotocol::GameBattleRoleUseEquipment req;
			req.bg_netid = role->GetBGNetID();
			req.uid = role->GetUID();
			req.item_column = item_column;
			req.item_index = item_index;
			req.move_idx = move_idx;
			req.item_id = equipment->GetItemId();

			req.equip_attr_num = 0;
			memset(req.equip_attr_list, 0, sizeof(req.equip_attr_list));
			for (int i = 0; i < item_grid_data.param->equip_param.attr_num && req.equip_attr_num < BATTLE_WEAPON_ATTR_NUM; ++i)
			{
				req.equip_attr_list[req.equip_attr_num++] = item_grid_data.param->equip_param.attr_list[i];
			}

			req.equip_sp_list[0] = item_grid_data.param->equip_param.sp_id;
			req.equip_sp_num = 1;

			if (this->IsRoleBattleRequestSendToBigCross(role))
			{
				is_use_succ = true;
				this->TransferBattleRequestToBigcrossBattle(role->GetUID(), (const char*)&req, sizeof(req));
			}
			else
			{
				is_use_succ = World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
			}
		}
	}

	if (!is_use_succ)
	{
		//this->SendBattleOperateFailNotice(role, Protocol::ROLE_OPER_REASON_ITEM_USE_ERROR);
		role->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);

		gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	}

	return is_use_succ;
}

bool BattleManagerLocal::RoleSealPetReq(int battle_server_index, Role* role, int move_idx, int target_grid_idx, int seal_card_type, int monster_id)
{
	if (NULL == role) return false;

	if (!role->GetRoleModuleManager()->GetPet()->HaveSpace(1, true)) return false;

	const MonsterCfg* monster_cfg = MonsterPool::GetInstance().GetMonsterCfg(monster_id);
	if (NULL == monster_cfg) return false;

	if (!monster_cfg->CanCatch()) return false;

	int pet_id = monster_cfg->GetCatchPetId();

	const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == pet_cfg)
	{
		return false;
	}

	int rate_common_promote = 0;
	int rate_silver_promote = 0;
	int rate_gold_promote = 0;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SEAL_PROMOTE))
		{
			const RandActivitySealPromoteConfig * config = static_cast<const RandActivitySealPromoteConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SEAL_PROMOTE));
			if (NULL != config)
			{
				const RASealPromoteCfg * act_cfg = config->GetSealPromoteCfg(ramgr, pet_id);
				if (NULL != act_cfg)
				{
					rate_common_promote = act_cfg->rate_common;
					rate_silver_promote = act_cfg->rate_silver;
					rate_gold_promote = act_cfg->rate_gold;

					RandActivity* act = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SEAL_PROMOTE);
					if (NULL != act)
					{
						act->OnRoleParticipateRA(role, __FUNCTION__);
					}
				}
			}
		}
	}



	ItemID item_id = 0;
	short succ_rate = 0;
	if (BATTLE_SEAL_CARD_TYPE_COMMON == seal_card_type)
	{
		succ_rate = pet_cfg->rate_common + rate_common_promote;
		item_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->common_sealcard;
	}

	if (BATTLE_SEAL_CARD_TYPE_SILVER == seal_card_type)
	{
		succ_rate = pet_cfg->rate_silver + rate_common_promote;
		item_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->silver_sealcard;
	}

	if (BATTLE_SEAL_CARD_TYPE_GOLD == seal_card_type)
	{
		succ_rate = pet_cfg->rate_gold + rate_common_promote;
		item_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->gold_sealcard;
	}

	if (succ_rate >= THE_RATE_MAX_PERCENT)		//加上封印提升活动提升的概率后 概率有可能超过一百
	{
		succ_rate = THE_RATE_MAX_PERCENT;
	}

	if (0 == item_id) return false;
	if (seal_card_type < pet_cfg->seal_card_type) return false;

	if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(item_id, 1)) return false;

	{
		static battlegameprotocol::GameBattleRoleSealPet req;
		req.uid = role->GetUID();
		req.bg_netid = role->GetBGNetID();
		req.move_idx = move_idx;
		req.target_grid_idx = target_grid_idx;
		req.monster_id = monster_id;
		req.seal_card_item_id = item_id;
		req.pet_id = pet_id;
		req.succ_rate = succ_rate;
		req.fixed_times_must_succ = static_cast<short>(pet_cfg->catch_fixed_times);
		req.pr_up_on_fail = static_cast<short>(pet_cfg->pr_up_on_fail);

		World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
		gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	}

	return true;
}

bool BattleManagerLocal::RoleInstKillMonsterReq(int battle_server_index, Role* role, int move_idx, int target_grid_idx, int monster_id)
{
	const MonsterCfg* monster_cfg = MonsterPool::GetInstance().GetMonsterCfg(monster_id);
	if (NULL == monster_cfg) return false;

	ItemID item_id = 0;//26248;
	short succ_rate = 0;// 100;
	if (0 == item_id) return false;

	if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(item_id, 1)) return false;

	{
		static battlegameprotocol::GameBattleRoleUseInstKillMonsterItem req;
		req.uid = role->GetUID();
		req.bg_netid = role->GetBGNetID();
		req.move_idx = move_idx;
		req.target_grid_idx = target_grid_idx;
		req.monster_id = monster_id;
		req.instant_kill_item_id = item_id;
		req.succ_rate = succ_rate;

		World::GetInstWorld()->SendMsgToOneBattle(battle_server_index, (const char*)&req, sizeof(req));
		gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
	}

	return true;
}

void BattleManagerLocal::OnRoleStartFight(int role_id, int battle_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL != role)
	{
		this->OnRoleStartFight(role, battle_id);
	}
}

void BattleManagerLocal::OnRoleStartFight(Role* role, int battle_id)
{
	if (NULL == role) return;

	BattleLocalInfo* local_info = this->GetBattleLocalInfo(battle_id);
	if (NULL == local_info) return;

	RoleBattleInfo* info = this->GetRoleBattleInfo(role->GetUID(), true); // 此处为true, 如果battle中的战斗开始前，BattleLocal的信息已被删除，则在玩家结束后，玩家下一次战斗将会提示正在战斗
	if (NULL == info) return;

	info->role_id = role->GetUID();
	info->battle_id = battle_id;
	info->battle_server_index = local_info->battle_server_index;
	info->is_ob = false;

	role->MoveStop(true);
	role->GetRoleStatusManager()->OnRoleStartFight(local_info->battle_mode);

	EventHandler::Instance().OnRoleStartFight(role, local_info->battle_mode);

	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		int member_count = team->GetMemberCount();
		for (int i = 0; i < member_count; ++i)
		{
			Robot* robot = team->GetMemberRobotByIndex(i);
			if (NULL != robot)
			{
				robot->MoveStop();
				robot->OnRobotStartBattle();
			}
		}
	}
}

void BattleManagerLocal::OnRoleStartFigntSpecialDeal(Role * role, int monster_group_id)
{
	if (NULL == role) return;

	Protocol::SCSalaryBattleStatInfo info;
	info.salary = 0;
	info.seq = 0;
	role->GetRoleModuleManager()->GetSalary()->OnBattleStartStatSalaryInfo(info, monster_group_id);
	if (info.salary <= 0 || info.seq < 0 || info.seq >= MAX_SALARY_INFO_NUM)
	{
		return;
	}
	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void BattleManagerLocal::OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	this->OnBattleFinish(ack);

	// 以下调用一些特殊逻辑的OnBattleFinish
	BattleLocalInfo* info = this->GetBattleLocalInfo(ack->battle_id);
	if (NULL != info && NULL != info->local_impl)
	{
		info->local_impl->OnBattleFinish(ack);
	}

	if (NULL != info)
	{
		Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(info->scene_id, info->scene_key);
		if (NULL != scene && NULL != scene->GetSpecialLogic())
		{
			scene->GetSpecialLogic()->OnFightFinish(ack);
		}
	}

	//下发战斗结果 注意！这段代码必须在所有特殊玩法需求执行完才执行
	bool is_attacker_win = (RESULT_TYPE_ATTACKER_WIN == ack->battle_result);
	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		int role_id = ack->attacker_list.role_list[i].uid;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL == role)
		{
			continue;
		}

		static Protocol::SCBattleResult result;
		result.battle_mode = ack->battle_mode;
		result.is_win = is_attacker_win ? 1 : 0;
		result.result_type = ack->battle_result;
		result.reserve_ch = 0;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&result, sizeof(result));
	}

	bool is_defender_win = (RESULT_TYPE_DEFENDER_WIN == ack->battle_result);
	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		int role_id = ack->defender_list.role_list[i].uid;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL == role)
		{
			continue;
		}

		static Protocol::SCBattleResult result;
		result.battle_mode = ack->battle_mode;
		result.is_win = is_defender_win ? 1 : 0;
		result.result_type = ack->battle_result;
		result.reserve_ch = 0;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&result, sizeof(result));
	}

	if (NULL != info)
	{
		std::set<int>::iterator iter = info->observer_set.begin();
		for (; iter != info->observer_set.end(); ++iter)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*iter));
			if (NULL != role)
			{
				this->OnRoleStopOB(role);
			}
			this->RemoveRoleBattleInfo(*iter);
		}

		this->RemoveBattleLocalInfo(info->battle_id);
		info = NULL; // 标识着info已被清除不可再用
	}

	do
	{
		if (!is_attacker_win) break;
		if (1 != ack->business_data.is_record_battle) break;
		if (ack->business_data.record_file_id <= 0) break;

		static const int THREE_STAR_CLEAR_FLAG = ((1 << 0) | (1 << 1) | (1 << 2));
		if (BATTLE_MODE_COURAGE_CHALLENGE == ack->battle_mode && ack->business_data.condition_flag != THREE_STAR_CLEAR_FLAG)
		{
			break;
		}
		else if (BATTLE_MODE_NEW_COURAGE_CHALLENGE == ack->battle_mode && ack->business_data.condition_flag != THREE_STAR_CLEAR_FLAG)
		{
			break;
		}
		else if (BATTLE_MODE_MATERIAL == ack->battle_mode)
		{
			if (!(ack->business_data.condition_flag & (1 << 0)) || !(ack->business_data.condition_flag & (1 << 1)) || !(ack->business_data.condition_flag & (1 << 2))) break;
		}

		const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(ack->business_data.monster_group_id);
		if (cfg != NULL)
		{
			if (cfg->is_cross == 1)
			{
				//发往跨服询问一下是否需要替换
				static crossgameprotocal::GameHiddenChangeVideoReq req;
				req.monster_group_id = ack->business_data.monster_group_id;
				req.video_score = ack->business_data.record_score;
				InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
			}
		}

		BattleVideoMgr::Instance().TryRecordVideo(ack->battle_mode, ack->business_data.monster_group_id, ack->business_data.record_score, ack->business_data.record_file_id, false);

	} while (0);

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
}

void BattleManagerLocal::OnFightSpecialFinish(battlegameprotocol::BattleGameSpecialFightResultAck * ack)
{
	//this->OnBattleFinish(ack);

	// 以下调用一些特殊逻辑的OnBattleFinish
	BattleLocalInfo* info = this->GetBattleLocalInfo(ack->battle_id);
	if (NULL != info && NULL != info->local_impl)
	{
		//info->local_impl->OnBattleFinish(ack);
	}

	/*if (NULL != info)
	{
		Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(info->scene_id, info->scene_key);
		if (NULL != scene && NULL != scene->GetSpecialLogic())
		{
			scene->GetSpecialLogic()->OnFightFinish(ack);
		}
	}*/

	//下发战斗结果 注意！这段代码必须在所有特殊玩法需求执行完才执行
	bool is_attacker_win = (RESULT_TYPE_ATTACKER_WIN == ack->battle_result);
	{
		for (int attacker_idx = 0; attacker_idx < SIDE_MAX_ROLE_NUM && attacker_idx < ack->attacker_num; attacker_idx++)
		{
			int role_id = ack->attacker_data[attacker_idx].uid;
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
			if (NULL != role)
			{
				static Protocol::SCBattleResult result;
				result.battle_mode = (char)ack->battle_mode;
				result.is_win = is_attacker_win ? 1 : 0;
				result.result_type = ack->battle_result;
				result.reserve_ch = 0;
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&result, sizeof(result));
				EventHandler::Instance().OnRoleFinishSpecialBattle(role, ack->battle_mode, is_attacker_win);
				this->OnRoleStopFight(role);
			}
			this->RemoveRoleBattleInfo(role_id);
		}
	}
	bool is_defender_win = (RESULT_TYPE_DEFENDER_WIN == ack->battle_result);
	{
		for (int defender_idx = 0; defender_idx < SIDE_MAX_ROLE_NUM && defender_idx < ack->defender_num; defender_idx++)
		{
			int role_id = ack->defender_data[defender_idx].uid;
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
			if (NULL != role)
			{
				static Protocol::SCBattleResult result;
				result.battle_mode = (char)ack->battle_mode;
				result.is_win = is_defender_win ? 1 : 0;
				result.result_type = ack->battle_result;
				result.reserve_ch = 0;
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&result, sizeof(result));
				EventHandler::Instance().OnRoleFinishSpecialBattle(role, ack->battle_mode, is_defender_win);
				this->OnRoleStopFight(role);
			}
			this->RemoveRoleBattleInfo(role_id);
		}
	}

	if (NULL != info)
	{
		std::set<int>::iterator iter = info->observer_set.begin();
		for (; iter != info->observer_set.end(); ++iter)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*iter));
			if (NULL != role)
			{
				this->OnRoleStopOB(role);
			}
			this->RemoveRoleBattleInfo(*iter);
		}

		this->RemoveBattleLocalInfo(info->battle_id);
		info = NULL; // 标识着info已被清除不可再用
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
}

void BattleManagerLocal::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
	// 强制结束战斗
	BattleLocalInfo* info = this->GetBattleLocalInfo(ack->battle_id);
	if (NULL != info)
	{
		info->local_impl->OnBattleForceFinish(ack);

		if (info->role_count > 0)
		{
			for (int i = 0; i < info->role_count; ++i)
			{
				int role_id = info->role_list[i];
				Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
				if (NULL != role)
				{
					SpecialLogic* spl = role->GetScene()->GetSpecialLogic();
					if (NULL != spl)
					{
						spl->OnBattleForcedFinish(ack);
						break;
					}

					EventHandler::Instance().OnRoleForceFinishBattle(role, ack->business_data.monster_group_id, ack->battle_mode);
				}
			}
		}
	}

	// 处理每个玩家的状态
	{
		BattleRoleList attacker_role_list; attacker_role_list.role_count = ack->attacker_list.role_count;
		BattleRoleList defender_role_list; defender_role_list.role_count = ack->defender_list.role_count;
		for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			attacker_role_list.role_id_list[i] = ack->attacker_list.role_list[i].uid;
		}
		for (int i = 0; i < ack->defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			defender_role_list.role_id_list[i] = ack->defender_list.role_list[i].uid;
		}

		for (int i = 0; i < ack->attacker_list.role_count; ++i)
		{
			int role_id = ack->attacker_list.role_list[i].uid;
			BattleData_StatisticRole& role_statistic = ack->attacker_list.role_list[i].role_statistic;
			BattleData_BusinessRole& role_business = ack->attacker_list.role_list[i].role_business;
			bool is_flyaway = ack->attacker_list.role_list[i].is_flyaway > 0;
			bool is_leader = ack->attacker_list.role_list[i].is_leader > 0;
			BattleManagerLocal::GetInstance().OnRoleFinishFight(role_id, ack->battle_id, false, is_flyaway, is_leader, true, ack->battle_mode, ack->business_data, role_business, role_statistic,
				ack->statistic_data, ack->attacker_list.role_list[i].pet_num, ack->attacker_list.role_list[i].pet_stat_list, ack->attacker_list.role_list[i].pet_unique_id_list, attacker_role_list);

			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
			if (NULL == role)
			{
				continue;
			}
			static Protocol::SCBattleResult result;
			result.battle_mode = ack->battle_mode;
			result.is_win = 0;
			result.result_type = ack->result_type;
			result.reserve_ch = 0;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&result, sizeof(result));
		}

		for (int i = 0; i < ack->defender_list.role_count; ++i)
		{
			int role_id = ack->defender_list.role_list[i].uid;
			BattleData_StatisticRole& role_statistic = ack->defender_list.role_list[i].role_statistic;
			BattleData_BusinessRole& role_business = ack->defender_list.role_list[i].role_business;
			bool is_flyaway = ack->defender_list.role_list[i].is_flyaway > 0;
			bool is_leader = ack->defender_list.role_list[i].is_leader > 0;
			BattleManagerLocal::GetInstance().OnRoleFinishFight(role_id, ack->battle_id, false, is_flyaway, is_leader, false, ack->battle_mode, ack->business_data, role_business, role_statistic,
				ack->statistic_data, ack->defender_list.role_list[i].pet_num, ack->defender_list.role_list[i].pet_stat_list, ack->defender_list.role_list[i].pet_unique_id_list, defender_role_list);

			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->defender_list.role_list[i].uid));
			if (NULL == role)
			{
				continue;
			}
			static Protocol::SCBattleResult result;
			result.battle_mode = ack->battle_mode;
			result.is_win = 0;
			result.result_type = ack->result_type;
			result.reserve_ch = 0;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&result, sizeof(result));
		}
	}

	if (NULL != info)
	{
		std::set<int>::iterator iter = info->observer_set.begin();
		for (; iter != info->observer_set.end(); ++iter)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*iter));
			if (NULL != role)
			{
				this->OnRoleStopFight(role);
			}
			this->RemoveRoleBattleInfo(*iter);
		}
	}

	this->RemoveBattleLocalInfo(ack->battle_id);
}

void BattleManagerLocal::OnFightStartFailed(battlegameprotocol::BattleGameStartFightFailed* ack)
{
	// 当战斗开始失败时的处理

	this->BattleStartFailed(ack->battle_id);

}

void BattleManagerLocal::OnCheckFightStartHasFailed(battlegameprotocol::BattleGameCheckBattleHasFailAck* ack)
{
	BattleLocalInfo* local_info = this->GetBattleLocalInfo(ack->battle_id);
	if (NULL == local_info) return;

	if (1 == ack->is_failed)
	{
		this->BattleStartFailed(ack->battle_id);
	}
	else
	{
		if (!local_info->is_battle_start)
		{
			local_info->is_battle_start = true;
		}
	}
}

void BattleManagerLocal::OnRoleFinishFight(int role_id, int battle_id, bool is_win, bool is_flyaway, bool is_leader, bool is_attacker, int battle_mode, BattleData_Business& business_data,
	BattleData_BusinessRole& role_business_data, BattleData_StatisticRole& role_statistic_data, BattleData_Statistic& statistic_data, int pet_stat_num,
	BattleData_StatisticRole* pet_stat_list, long long * pet_unique_id_list, BattleRoleList battle_role_list)
{
	this->RemoveRoleBattleInfo(role_id);

	if (RobotManager::IsRobot(role_id))
	{
		Robot* robot = RobotManager::Instance().GetRobot(role_id);
		if (NULL != robot)
		{
			robot->OnRobotStopBattle();
		}
		return;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role) return;

	this->OnRoleStopFight(role);

	EventHandler::Instance().OnRoleFinishFightDetail(role, battle_mode, is_win, is_flyaway, &business_data, &role_statistic_data, &statistic_data, pet_stat_num, pet_stat_list, pet_unique_id_list);
	EventHandler::Instance().OnRoleFinishFight(role, battle_mode, is_win, is_flyaway, business_data, battle_role_list);
	EventHandler::Instance().OnFightMonsterGroup(role, business_data.monster_group_id, is_win, is_flyaway);
	if (is_win && !is_flyaway)
	{
		// 赢了才算击杀怪物
		bool is_mine_monster = BATTLE_MINE_MONSTER_GROUP == business_data.monster_group_id;
		EventHandler::Instance().OnKillMonster(role, statistic_data.monster_list_size,
			statistic_data.monster_id_list, statistic_data.monster_level_list, is_mine_monster);

		if (business_data.monster_group_id > 0 && is_attacker)
		{

			RoleFirstKill* rfk = role->GetRoleModuleManager()->GetRoleFirstKill();
			rfk->OnKillMonsterGroup(business_data.monster_group_id,
				statistic_data.attacker_role_num, statistic_data.round_num,
				statistic_data.attacker_role_high_level, statistic_data.attacker_role_low_level,
				statistic_data.attacker_role_average_level,
				statistic_data.attacker_role_num - statistic_data.attacker_alive_role_num);
		}
	}

	if (1 == role_business_data.is_leave_team_after_battle)
	{
		TeamManager::Instance().ExitTeam(role);
	}

	if (is_leader)
	{
		if (is_attacker)
		{
			for (int i = 0; i < statistic_data.attacker_partner_num; ++i)
			{
				role->GetRoleModuleManager()->GetPartner()->ElementCrystalCountSubByPartnerId(statistic_data.attacker_parner_id_list[i]);
			}
		}
		else
		{
			for (int i = 0; i < statistic_data.defender_partner_num; ++i)
			{
				role->GetRoleModuleManager()->GetPartner()->ElementCrystalCountSubByPartnerId(statistic_data.defender_parner_id_list[i]);
			}
		}
	}
	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
}

void BattleManagerLocal::OnCheckRoleReallyInBattle(int role_id, bool is_in_battle, int battle_id, int battle_server_idx)
{
	if (!is_in_battle)
	{
		this->RemoveRoleBattleInfo(role_id);

		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			this->OnRoleStopFight(role);
		}
	}
	else
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			Protocol::SCBattleRoleInBattleNotice notice;
			notice.battle_id = battle_id;
			notice.battle_server_idx = battle_server_idx;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&notice, sizeof(notice));
		}

#ifdef _DEBUG
		printf("RoleInBattle, battle_id[%d] battle_server_idx[%d]\n",
			battle_id, battle_server_idx);
#endif
	}

	gamelog::g_log_battle.printf(LL_INFO, "OnCheckRoleReallyInBattle role_id[%d] is_in_battle[%s]", role_id, is_in_battle ? "true" : "false");

#ifdef _DEBUG
	gamelog::g_log_world.printf(LL_MAINTANCE, "OnCheckRoleReallyInBattle role_id[%d] is_in_battle[%s]", role_id, is_in_battle ? "true" : "false");
#endif
}

void BattleManagerLocal::OnGuildHonorFightRequestBackup(battlegameprotocol::BattleGameGuildHonorFightRequestBackup* msg)
{
	BattleLocalInfo* info = this->GetBattleLocalInfo(msg->battle_id);
	if (NULL != info)
	{
		Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(info->scene_id, info->scene_key);
		if (NULL != scene && scene->GetSceneType() == Scene::SCENE_TYPE_GUILD_HONOR_BATTLE)
		{
			SpecialLogicGuildHonorBattle * special_logic = (SpecialLogicGuildHonorBattle *)scene->GetSpecialLogic();
			special_logic->BattleAddBackUp(msg->battle_id, msg->attacker_need_num, msg->defender_need_num, msg->business_data.param1);
		}
	}
}

void BattleManagerLocal::OnGuildHonorFightRoleDieNotice(battlegameprotocol::BattleGameGuildHonorRoleDieNotice* msg)
{
	BattleLocalInfo* info = this->GetBattleLocalInfo(msg->battle_id);
	if (NULL != info)
	{
		Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(info->scene_id, info->scene_key);
		if (NULL != scene && scene->GetSceneType() == Scene::SCENE_TYPE_GUILD_HONOR_BATTLE)
		{
			SpecialLogicGuildHonorBattle * special_logic = (SpecialLogicGuildHonorBattle *)scene->GetSpecialLogic();
			special_logic->BattleChangeBackupRoleNumNotice(msg->battle_id, msg->attacker_queue_num, msg->defender_queue_num, msg->business_data.param1);
		}
	}
}

void BattleManagerLocal::OnGuildHonorFightRoleStateChangeNotice(battlegameprotocol::BattleGameGuildHonorRoleStateChangeNotice* msg)
{
	BattleLocalInfo* info = this->GetBattleLocalInfo(msg->battle_id);
	if (NULL != info)
	{
		Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(info->scene_id, info->scene_key);
		if (NULL != scene && scene->GetSceneType() == Scene::SCENE_TYPE_GUILD_HONOR_BATTLE)
		{
			SpecialLogicGuildHonorBattle * special_logic = (SpecialLogicGuildHonorBattle *)scene->GetSpecialLogic();

			std::vector<int> start_role_list, end_role_list;
			start_role_list.reserve(msg->start_role_num);
			end_role_list.reserve(msg->end_role_num);
			for (int i = 0; i < msg->start_role_num && i < ARRAY_ITEM_COUNT(msg->start_role_list); ++i)
			{
				start_role_list.push_back(msg->start_role_list[i]);
			}
			for (int i = 0; i < msg->end_role_num && i < ARRAY_ITEM_COUNT(msg->end_role_list); ++i)
			{
				end_role_list.push_back(msg->end_role_list[i]);
			}
			special_logic->BattleStateChangeNotice(msg->is_attacker_side > 0, start_role_list, end_role_list, msg->zone_id);
		}
	}
}

void BattleManagerLocal::OnGuildHonorFightSyncTotalDamage(battlegameprotocol::BattleGameGuildHonorSyncTotalDamage* msg)
{
	BattleLocalInfo* info = this->GetBattleLocalInfo(msg->battle_id);
	if (NULL != info)
	{
		Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(info->scene_id, info->scene_key);
		if (NULL != scene && scene->GetSceneType() == Scene::SCENE_TYPE_GUILD_HONOR_BATTLE)
		{
			SpecialLogicGuildHonorBattle * special_logic = (SpecialLogicGuildHonorBattle *)scene->GetSpecialLogic();
			special_logic->BattleRoundTotalDamage(msg->atk_total_damage, msg->def_total_damage, msg->zone_id);
		}
	}
}

void BattleManagerLocal::OnRoleReinforceBattle(int role_id, int battle_id)
{
	this->OnRoleStartFight(role_id, battle_id);
}

void BattleManagerLocal::OnRoleReinforceBattleEnd(int role_id, int battle_id)
{
	this->RemoveRoleBattleInfo(role_id);

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role)
	{
		return;
	}
	this->OnRoleStopFight(role);
}

void BattleManagerLocal::OnRoleSyncOBStatus(battlegameprotocol::BattleGameRoleSyncOBStatus* msg)
{
#ifdef _DEBUG
	printf("OnRoleSyncOBStatus uid[%d] battle_id[%d] status[%d]\n", msg->uid, msg->battle_id, msg->ob_status);
#endif

	BattleLocalInfo* local_info = this->GetBattleLocalInfo(msg->battle_id);
	if (NULL == local_info) return;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->uid));
	if (NULL == role) return;

	switch (msg->ob_status)
	{
	case battlegameprotocol::BattleGameRoleSyncOBStatus::OB_STATUS_START_OB:
		{
			RoleBattleInfo* rbi = this->GetRoleBattleInfo(msg->uid, true);
			if (NULL == rbi) return;

			this->OnRoleStartOB(role);
			local_info->AddObserver(msg->uid);

			rbi->role_id = msg->uid;
			rbi->battle_id = msg->battle_id;
			rbi->is_ob = true;
			rbi->battle_server_index = local_info->battle_server_index;
		}
		break;

	case battlegameprotocol::BattleGameRoleSyncOBStatus::OB_STATUS_STOP_OB:
		{
			RoleBattleInfo* rbi = this->GetRoleBattleInfo(msg->uid, false);
			if (NULL == rbi) return;

			this->OnRoleStopOB(role);
			local_info->RemoveObserver(msg->uid);

			this->RemoveRoleBattleInfo(msg->uid);
		}
		break;

	case battlegameprotocol::BattleGameRoleSyncOBStatus::OB_STATUS_STOP_FIGHT_START_OB:
		{
			RoleBattleInfo* rbi = this->GetRoleBattleInfo(msg->uid, false);
			if (NULL == rbi) return;

			this->OnRoleStopFight(role);
			this->OnRoleStartOB(role);
			local_info->AddObserver(msg->uid);
		}
		break;

	case battlegameprotocol::BattleGameRoleSyncOBStatus::OB_STATUS_START_FIGHT_STOP_OB:
		{
			RoleBattleInfo* rbi = this->GetRoleBattleInfo(msg->uid, false);
			if (NULL == rbi) return;

			this->OnRoleStopOB(role);
			local_info->RemoveObserver(msg->uid);
			this->OnRoleStartFight(role, msg->battle_id);
		}
		break;
	}
}

int BattleManagerLocal::MakeBattleLocalInfoSingle(Role* role, int battle_id, int battle_mode, RoleBattleData* role_battle_data, int monster_group_id, int monster_gen_id)
{
	BattleLocalInfo info;
	info.battle_id = battle_id;
	info.battle_mode = battle_mode;
	info.monster_group_id = monster_group_id;
	info.monster_gen_id = monster_gen_id;

	info.role_count = 1;
	info.role_list[0] = role->GetUID();

	info.scene_id = role->GetScene()->GetSceneID();
	info.scene_key = role->GetScene()->GetSceneKey();

	info.local_info_create_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	// 根据battle_mode创建battle_business_local 
	info.local_impl = BattleBusinessLocal::CreateBBLImpl(battle_mode);

	if (BATTLE_MODE_WORLD_BOSS == battle_mode || BATTLE_MODE_WORLD_BOSS_2 == battle_mode || BATTLE_MODE_WORLD_BOSS_3 == battle_mode || 
		BATTLE_MODE_GUILD_BOSS == battle_mode)
	{
		info.battle_server_index = USE_BATTLE_SERVER_INDEX;
	}
	else
	{
		World::GetInstWorld()->ChooseBattleIndex(&info.battle_server_index);
	}

	BattleLocalInfoMap::iterator it = m_local_info_map.insert(BattleLocalInfoMap::value_type(battle_id, info)).first;
	if (NULL != it->second.local_impl)
	{
		it->second.local_impl->SetBattleLocalInfo(&it->second);
		it->second.local_impl->OnBattlePrepare(battle_mode, 1, &role, &role_battle_data);
	}

	gamelog::g_log_battle.printf(LL_INFO, "%s role[%d,%s] battle_mode[%d] monster_group_id[%d]",
		__FUNCTION__, role->GetUID(), role->GetName(), battle_mode, monster_group_id);
	return info.battle_server_index;
}

int BattleManagerLocal::MakeBattleLocalInfoMulti(int role_num, Role** role_list, int battle_id, int battle_mode, RoleBattleData** role_battle_data_list, int monster_group_id, int monster_gen_id)
{
	BattleLocalInfo info;
	info.battle_id = battle_id;
	info.battle_mode = battle_mode;
	info.monster_group_id = monster_group_id;
	info.monster_gen_id = monster_gen_id;

	info.role_count = role_num;
	for (int i = 0; i < role_num; ++i)
	{
		info.role_list[i] = role_list[i]->GetUID();
	}

	// 应确保战斗中所有人都在同一个场景才可以开始战斗
	if (role_num > 0)
	{
		info.scene_id = role_list[0]->GetScene()->GetSceneID();
		info.scene_key = role_list[0]->GetScene()->GetSceneKey();
	}

	info.local_info_create_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	// 根据battle_mode创建battle_business_local 
	info.local_impl = BattleBusinessLocal::CreateBBLImpl(battle_mode);

	if (BATTLE_MODE_WORLD_BOSS == battle_mode || BATTLE_MODE_WORLD_BOSS_2 == battle_mode || BATTLE_MODE_WORLD_BOSS_3 == battle_mode ||
		BATTLE_MODE_GUILD_BOSS == battle_mode)
	{
		info.battle_server_index = USE_BATTLE_SERVER_INDEX;
	}
	else
	{
		World::GetInstWorld()->ChooseBattleIndex(&info.battle_server_index);
	}

	BattleLocalInfoMap::iterator it = m_local_info_map.insert(BattleLocalInfoMap::value_type(battle_id, info)).first;
	if (NULL != it->second.local_impl)
	{
		it->second.local_impl->SetBattleLocalInfo(&it->second);
		it->second.local_impl->OnBattlePrepare(battle_mode, role_num, role_list, role_battle_data_list);
	}

	for (int i = 0; i < role_num; ++i)
	{
		gamelog::g_log_battle.printf(LL_INFO, "%s role[%d,%s] battle_mode[%d] monster_group_id[%d]",
			__FUNCTION__, role_list[i]->GetUID(), role_list[i]->GetName(), battle_mode, monster_group_id);
	}

	return info.battle_server_index;
}

void BattleManagerLocal::MarkBattleLaunchInfo(BattleData_Business& business_data, Role* role, const char* launch_func_name)
{
	if (role == NULL)
	{
		return;
	}

	F_STRNCPY(business_data.launch_fun_name, launch_func_name, sizeof(business_data.launch_fun_name));
	F_STRNCPY(business_data.launch_role_name, role->GetName(), sizeof(business_data.launch_role_name));
	business_data.launch_scene_id = role->GetScene()->GetSceneID();
	business_data.launch_scene_key = role->GetScene()->GetSceneKey();
	business_data.launch_scene_posx = role->GetPos().x;
	business_data.launch_scene_posy = role->GetPos().y;

#ifdef _DEBUG
	F_STRNCPY(business_data.launch_stack, PrintStackBackTrace(false).c_str(), sizeof(business_data.launch_stack));
#endif
}

void BattleManagerLocal::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* msg)
{
	BattleLocalInfo* info = this->GetBattleLocalInfo(msg->battle_id);
	if (NULL != info && NULL != info->local_impl)
	{
		info->local_impl->OnBattleRunAway(msg);
	}

	this->RemoveRoleBattleInfo(msg->uid);

	if (RobotManager::IsRobot(msg->uid))
	{
		Robot* robot = RobotManager::Instance().GetRobot(msg->uid);
		if (NULL != robot)
		{
			robot->OnRobotStopBattle();

			SpecialLogic* spl = robot->GetScene()->GetSpecialLogic();
			if (NULL != spl)
			{
				spl->OnRoleRunAway(msg);
			}

			EventHandler::Instance().OnRobotRunAway(robot, msg->is_leave_team > 0);
		}
	}
	else
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->uid));
		if (NULL == role)
		{
			return;
		}
		this->OnRoleStopFight(role);

		SpecialLogic* spl = role->GetScene()->GetSpecialLogic();
		if (NULL != spl)
		{
			spl->OnRoleRunAway(msg);
		}

		EventHandler::Instance().OnRoleRunAway(role, msg->is_leave_team > 0, msg->business_data.monster_group_id, msg->battle_mode);

		static Protocol::SCBattleResult result;
		result.battle_mode = msg->battle_mode;
		result.is_win = 0;
		result.reserve_ch = 0;
		result.result_type = RESULT_TYPE_RUNAWAY;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&result, sizeof(result));
	}
}

void BattleManagerLocal::OnRoleSetAuto(int role_id, int is_auto)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role)
	{
		return;
	}
	role->SetBattleAuto(is_auto);
}

void BattleManagerLocal::OnRoleSetAutoMove(int role_id, short first_move_type, short first_skill_id, short first_skill_level, short second_move_type, short second_skill_id, short second_skill_level)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role)
	{
		return;
	}

	role->SetBattleAutoMove(true, first_move_type, first_skill_id, first_skill_level, false, false, true);
	role->SetBattleAutoMove(false, second_move_type, second_skill_id, second_skill_level, false, true, true);
}

void BattleManagerLocal::OnRoleUseItem(int role_id, short item_column, short item_index, ItemID item_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role)
	{
		return;
	}

	static ItemGridData item_grid_data;

	if (!role->GetRoleModuleManager()->GetKnapsack()->GetItemGridData(item_column, item_index, &item_grid_data))
	{
		gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleUseItem role[%d,%s] item extract failed, column[%d] index[%d] itemid[%d]",
			role->GetUID(), role->GetName(), item_column, item_index, item_id);
		return;
	}

	if (item_grid_data.item_id != item_id)
	{
		gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleUseItem role[%d,%s] item id not same, column[%d] index[%d] itemid[%d]",
			role->GetUID(), role->GetName(), item_column, item_index, item_id);
		return;
	}

	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item)
	{
		gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleUseItem role[%d,%s] item invalid, column[%d] index[%d] itemid[%d]",
			role->GetUID(), role->GetName(), item_column, item_index, item_id);
		return;
	}

	if (I_TYPE_EXPENSE == item->GetItemType())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItemByIndex(item_column, item_index, 1, __FUNCTION__))
		{
			gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleUseItem role[%d,%s] consume failed, column[%d] index[%d] itemid[%d]",
				role->GetUID(), role->GetName(), item_column, item_index, item_id);
		}
	}
	else if (I_TYPE_EQUIPMENT == item->GetItemType())
	{
		int ret = role->GetRoleModuleManager()->GetEquipmentManager()->PutOn(item_column, item_index, Equipment::E_INDEX_WEAPON);
		if (0 != ret)
		{
			gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleUseItem role[%d,%s] puton failed, column[%d] index[%d] itemid[%d]",
				role->GetUID(), role->GetName(), item_column, item_index, item_id);
		}
	}

	role->GetScene()->GetSpecialLogic()->OnUseItem(role, item->GetItemId(), 1);
}

void BattleManagerLocal::OnRoleSealPet(int role_id, int monster_id, int monster_group_id, int pet_id, ItemID seal_card_item_id, bool is_succ, bool is_sure_give_pet_on_succ, bool is_auto_seal_pet, int battle_mode, bool is_catch_bind_pet)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role)
	{
		return;
	}

	bool is_get_pet = true;
	ItemConfigData catch_reward_item;
	if (is_sure_give_pet_on_succ)
	{
		catch_reward_item.item_id = pet_id;
		catch_reward_item.num = 1;
		catch_reward_item.is_bind = is_catch_bind_pet;
		is_get_pet = true;
	}
	else
	{
		if (!LOGIC_CONFIG->GetPetConfig()->GetSealPetReward(pet_id, catch_reward_item, &is_get_pet))		//捕获奖励有可能是碎片或宠物
		{
			gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleSealPet GetSealPetReward FAILED! role[%d,%s] monster_id[%d] pet_id[%d] seal_item[%d] is_succ[%d]",
				role->GetUID(), role->GetName(), monster_id, pet_id, seal_card_item_id, is_succ);
			return;
		}

		if (is_get_pet && !catch_reward_item.is_bind && is_catch_bind_pet)
		{
			catch_reward_item.is_bind = is_catch_bind_pet;
		}
	}


	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(seal_card_item_id, 1, __FUNCTION__))
	{
		gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleSealPet consume seal card FAILED! role[%d,%s] monster_id[%d] pet_id[%d] seal_item[%d] is_succ[%d]",
			role->GetUID(), role->GetName(), monster_id, pet_id, seal_card_item_id, is_succ);
		return;
	}

	if (is_succ)
	{
		Protocol::SCSealPetGetItemInfo get_item_info;
		get_item_info.pet_id = pet_id;
		get_item_info.item_id = catch_reward_item.item_id;
		get_item_info.num = catch_reward_item.num;
		get_item_info.is_get_pet = is_get_pet ? 1 : 0;
		EngineAdapter::Instance().NetSend(role->GetNetId(), &get_item_info, sizeof(get_item_info));

		bool is_give_guide_pet = false;
		const FuncGuideOtherCfg & other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();
		if (other_cfg.catch_pet_guide_monster_group_id == monster_group_id)
		{
			is_give_guide_pet = true;
		}

		int give_reward_times = 1;
		if (BATTLE_MODE_PET_MONOPOLY == battle_mode && role->GetRoleModuleManager()->GetPetMonopoly()->DoublePetReward())
		{
			give_reward_times += 1;
		}

		for (int i = 0; i < give_reward_times; ++i)
		{
			this->OnRoleSealPetGiveReward(role, is_get_pet, catch_reward_item, pet_id, battle_mode, is_give_guide_pet);
		}
	}

	if (is_auto_seal_pet)
	{
		role->GetRoleModuleManager()->GetAutoCatchPet()->OnAutoCatchPet(monster_id, seal_card_item_id, is_succ);
	}

	EventHandler::Instance().OnRoleTrapCreature(role, pet_id, is_succ);

	gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleSealPet role[%d,%s] monster_id[%d] pet_id[%d] seal_item[%d] is_succ[%d]",
		role->GetUID(), role->GetName(), monster_id, pet_id, seal_card_item_id, is_succ);
}

void BattleManagerLocal::OnRoleInstKillMonster(int role_id, int monster_id, ItemID inst_kill_item_id, bool is_succ)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role)
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(inst_kill_item_id, 1, __FUNCTION__))
	{
		gamelog::g_log_battle.printf(LL_WARNING, "BattleManagerLocal::OnRoleInstKillMonster consume item FAILED! role[%d,%s] monster_id[%d] item[%d] is_succ[%d]",
			role->GetUID(), role->GetName(), monster_id, inst_kill_item_id, is_succ);
		return;
	}
}

void BattleManagerLocal::OnBattleInstanceReleaseNotice(int battle_id)
{
	// 主要用于清除那些因特殊情况战斗结束的战斗本地信息
	BattleLocalInfo* info = this->GetBattleLocalInfo(battle_id);
	if (NULL == info) return;

	info->local_impl->OnBattleInstanceDeleted();

	gamelog::g_log_battle.printf(LL_INFO, "OnBattleInstanceReleaseNotice battle_id[%d] battle_mode[%d]", battle_id, info->battle_mode);

	for (int i = 0; i < info->role_count; ++i)
	{
		RoleBattleInfo* rbi = this->GetRoleBattleInfo(info->role_list[i]);
		if (NULL != rbi && rbi->battle_id == info->battle_id)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(info->role_list[i]));
			if (NULL != role)
			{
				gamelog::g_log_battle.printf(LL_INFO, "OnBattleInstanceReleaseNotice role[%d,%s]", role->GetUID(), role->GetName());

				if (!rbi->is_ob)
				{
					this->OnRoleStopFight(role);
				}
				else
				{
					this->OnRoleStopOB(role);
				}
			}

			this->RemoveRoleBattleInfo(info->role_list[i]);
		}
	}

	std::set<int>::iterator it = info->observer_set.begin();
	for (; it != info->observer_set.end(); ++it)
	{
		RoleBattleInfo* rbi = this->GetRoleBattleInfo(*it);
		if (NULL != rbi && rbi->battle_id == info->battle_id)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*it));
			if (NULL != role)
			{
				this->OnRoleStopOB(role);
			}

			this->RemoveRoleBattleInfo(*it);
		}
	}

	this->RemoveBattleLocalInfo(battle_id);

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
}

void BattleManagerLocal::OnForceFinishBattleModeResp(battlegameprotocol::BattleGameForceFinishBattleModeResp* resp)
{
	if (resp->battle_mode == BATTLE_MODE_WORLD_BOSS)
	{
		ActivityWorldBoss * act = static_cast<ActivityWorldBoss *>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS));
		if (NULL != act)
		{
			act->Settlement();
		}
	}

	if (resp->battle_mode == BATTLE_MODE_WORLD_BOSS_2)
	{
		ActivityWorldBoss2 * act = static_cast<ActivityWorldBoss2 *>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2));
		if (NULL != act)
		{
			act->Settlement();
		}
	}

	if (resp->battle_mode == BATTLE_MODE_WORLD_BOSS_3)
	{
		ActivityWorldBoss3 * act = static_cast<ActivityWorldBoss3 *>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3));
		if (NULL != act)
		{
			act->Settlement();
		}
	}
}

void BattleManagerLocal::SendBattleOperateFailNotice(Role* role, int reason)
{
	//static Protocol::SCBattleRoleOperate msg;
	//msg.is_succ = 0;
	//msg.reserve_sh = reason;

	//EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));
}

const RoleBattleInfo* BattleManagerLocal::GetRoleBattleInfoConst(int role_id)
{
	return this->GetRoleBattleInfo(role_id, false);
}

const BattleLocalInfo* BattleManagerLocal::GetBattleLocalInfoConst(int battle_id)
{
	return this->GetBattleLocalInfo(battle_id);
}

bool BattleManagerLocal::IsBattleModeMustInTeam(short battle_mode, short* out_member_num_at_least, short * min_level, short *real_player_num_at_least)
{
	bool ret = false;
	*out_member_num_at_least = 1; // 默认最少一个人
	*real_player_num_at_least = 1;// 默认最少一个玩家（真实玩家）
	*min_level = 1;

	switch (battle_mode)
	{
	case BATTLE_MODE_EXPEL_MONSTER:
		{
			ret = false;
			*out_member_num_at_least = 1;
			*real_player_num_at_least = 1;
		}
		break;
	case BATTLE_MODE_DEMON_CAVE:
		{
			ret = true;
			*out_member_num_at_least = 3;
		}
		break;
	case BATTLE_MODE_FIELD_BOSS:
		{
			ret = true;
			*out_member_num_at_least = 3;
		}
		break;

	case BATTLE_MODE_ZHEN_ZHONG:
		{
			const ResearchTaskOtherCfg rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();

			ret = true;
			*out_member_num_at_least = rt_other.team_need_member_min;
		}
		break;

	case BATTLE_MODE_WORLD_BOSS:
		{
			ret = true;
			const WorldBossOtherCfg &  other_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetOtherCfg();

			*out_member_num_at_least = other_cfg.need_role_num;
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_2:
		{
			ret = true;
			const WorldBossOtherCfg &  other_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetOtherCfg();

			*out_member_num_at_least = other_cfg.need_role_num;
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_3:
		{
			ret = true;
			const WorldBoss3OtherCfg &  other_cfg = LOGIC_CONFIG->GetWorldBoss3Config()->GetOtherCfg();
	
			*out_member_num_at_least = other_cfg.need_role_num;
		}
		break;
	case BATTLE_MODE_SHAN_HAI_BOSS:
		{
			ret = true;
			*out_member_num_at_least = LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().limit_team_people;
			*real_player_num_at_least = 1;
		}
		break;
	case BATTLE_MODE_LANTERN_RIDDIR:
	{
		ret = true;
		*out_member_num_at_least = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().member;
		*real_player_num_at_least = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().member;
	}
	break;
	}

	return ret;
}

bool BattleManagerLocal::IsBattleModeMustNotInTeam(short battle_mode)
{
	bool ret = false;
	switch (battle_mode)
	{
	case BATTLE_MODE_COURAGE_CHALLENGE:
	case BATTLE_MODE_GUILD_BOSS:
	case BATTLE_MODE_DEMO:
	case BATTLE_MODE_COURSE_OF_GROWTH:
	case BATTLE_MODE_GUIDE_FB:
	case BATTLE_MODE_REALIVE_GUIDE:
	case BATTLE_MODE_SUMMER_LI_LIAN:
	case BATTLE_MODE_SHAN_HAI_ZHENG_FENG:
	case BATTLE_MODE_NEW_COURAGE_CHALLENGE:
		ret = true;
		break;
	}

	return ret;
}

RoleBattleInfo* BattleManagerLocal::GetRoleBattleInfo(int role_id, bool is_create /*= false*/)
{
	RoleBattleInfoMap::iterator it = m_role_battle_info_map.find(role_id);
	if (it == m_role_battle_info_map.end())
	{
		if (is_create)
		{
			static RoleBattleInfo info;
			info.role_id = role_id;
			info.battle_id = -1;
			info.battle_server_index = -1;

			it = m_role_battle_info_map.insert(RoleBattleInfoMap::value_type(role_id, info)).first;
		}
		else
		{
			return NULL;
		}
	}

	return &it->second;
}

BattleLocalInfo* BattleManagerLocal::GetBattleLocalInfo(int battle_id)
{
	BattleLocalInfoMap::iterator it = m_local_info_map.find(battle_id);
	if (it == m_local_info_map.end())
	{
		return NULL;
	}

	return &it->second;
}

void BattleManagerLocal::RemoveBattleLocalInfo(int battle_id)
{
	BattleLocalInfo* info = this->GetBattleLocalInfo(battle_id);
	if (NULL != info)
	{
		World::GetInstWorld()->ModifyBattleIndexHandleNum(info->battle_server_index, -1);

		if (info->local_impl != NULL)
		{
			delete info->local_impl;
			info->local_impl = NULL;
		}
		m_local_info_map.erase(battle_id);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
}

void BattleManagerLocal::RemoveRoleBattleInfo(int role_id)
{
	RoleBattleInfoMap::iterator it = m_role_battle_info_map.find(role_id);
	if (it != m_role_battle_info_map.end())
	{
		m_role_battle_info_map.erase(it);
	}
}

int BattleManagerLocal::GenBattleID()
{
	m_battle_id_generator += 1;
	if (m_battle_id_generator >= MAX_INT)
	{
		m_battle_id_generator = 1;
	}
	return m_battle_id_generator;
}

void BattleManagerLocal::CheckBattleStartFailed()
{
	//检查没有成功开启战斗的LocalBattle
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now < m_next_check_battle_start_failed_time)
	{
		return;
	}
	m_next_check_battle_start_failed_time = now + 3;

	std::vector<int> fail_start_list;
	BattleLocalInfoMap::iterator it = m_local_info_map.begin();
	for (; it != m_local_info_map.end(); ++it)
	{
		if (it->second.local_info_create_time + 10 < now &&
			!it->second.is_battle_start &&
			it->second.last_check_start_failed_time + 3 < now)
		{
			if (!this->IsBattleServerConnected(it->second.battle_server_index))
			{
				fail_start_list.push_back(it->second.battle_id);
			}
			else
			{
				battlegameprotocol::GameBattleCheckBattleHasFail req;
				req.battle_id = it->second.battle_id;
				World::GetInstWorld()->SendMsgToOneBattle(it->second.battle_server_index, (const char*)&req, sizeof(req));

				it->second.last_check_start_failed_time = now;
			}
		}
	}

	for (size_t i = 0; i < fail_start_list.size(); ++i)
	{
		this->BattleStartFailed(fail_start_list[i]);
	}

	gamelog::g_log_battle.printf(LL_DEBUG, "%s", __FUNCTION__);
}

void BattleManagerLocal::BattleStartFailed(int battle_id)
{
	BattleLocalInfo* local_info = this->GetBattleLocalInfo(battle_id);
	if (NULL == local_info) return;

	local_info->local_impl->OnBattleInstanceDeleted();

	for (int i = 0; i < local_info->role_count; ++i)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(local_info->role_list[i]));
		if (NULL != role)
		{
			this->OnRoleStopFight(role);
		}
		this->RemoveRoleBattleInfo(local_info->role_list[i]);
	}

	std::set<int>::iterator iter = local_info->observer_set.begin();
	for (; iter != local_info->observer_set.end(); ++iter)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*iter));
		if (NULL != role)
		{
			this->OnRoleStopOB(role);
		}
		this->RemoveRoleBattleInfo(*iter);
	}

	this->RemoveBattleLocalInfo(battle_id);
}

bool BattleManagerLocal::CanStartWorldBoss(Role * role)
{
	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		return false;
	}

	Role * leader = team->GetLeaderRole();
	if (leader == NULL)
	{
		return false;
	}

	if (leader->GetUID() != role->GetUID())
	{
		return false;
	}

	if (!team->IsAllMemberFollow())
	{
		return false;
	}

	ActivityWorldBoss* act = (ActivityWorldBoss*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS);
	if (NULL == act)
	{
		return false;
	}

	const WorldBossOtherCfg &  other_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetOtherCfg();

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member)
		{
			continue;
		}

		if (!FunOpen::IsFunOpen(member, FUN_TYPE_WORLD_BOSS, true))
		{
			team->SendLeaderIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_LEVEL);
			return false;
		}

		int play_times = act->GetWorldBossPlayTimes(member->GetUID());
		if (play_times >= other_cfg.limit_play_times)
		{
			team->SendLeaderIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS1);
			return false;
		}
	}

	return true;
}

bool BattleManagerLocal::CanStartWorldBoss2(Role * role)
{
	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		return false;
	}

	Role * leader = team->GetLeaderRole();
	if (leader == NULL)
	{
		return false;
	}

	if (leader->GetUID() != role->GetUID())
	{
		return false;
	}

	if (!team->IsAllMemberFollow())
	{
		return false;
	}

	ActivityWorldBoss2 * act = (ActivityWorldBoss2*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2);
	if (NULL == act)
	{
		return false;
	}

	const WorldBossOtherCfg &  other_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetOtherCfg();//策划世界boss1 和 世界boss2 用的都是同一个限制字段

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member)
		{
			continue;
		}

		if (!FunOpen::IsFunOpen(member, FUN_TYPE_WORLD_BOSS_2, true))
		{
			team->SendLeaderIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_LEVEL);
			return false;
		}

		int play_times = act->GetWorldBoss2PlayTimes(member->GetUID());
		if (play_times >= other_cfg.limit_play_times)
		{
			team->SendLeaderIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS2);
			return false;
		}
	}

	return true;
}

bool BattleManagerLocal::CanStartWorldBoss3(Role * role)
{
	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		return false;
	}

	Role * leader = team->GetLeaderRole();
	if (leader == NULL)
	{
		return false;
	}

	if (leader->GetUID() != role->GetUID())
	{
		return false;
	}

	if (!team->IsAllMemberFollow())
	{
		return false;
	}

	ActivityWorldBoss3* act = (ActivityWorldBoss3*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3);
	if (NULL == act)
	{
		return false;
	}

	const WorldBoss3OtherCfg &  other_cfg = LOGIC_CONFIG->GetWorldBoss3Config()->GetOtherCfg();

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member)
		{
			continue;
		}

		if (!FunOpen::IsFunOpen(member, FUN_TYPE_WORLD_BOSS_3, true))
		{
			team->SendLeaderIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_LEVEL);
			return false;
		}

		int play_times = act->GetWorldBossPlayTimes(member->GetUID());
		if (play_times >= other_cfg.limit_play_times)
		{
			team->SendLeaderIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS3);
			return false;
		}
	}

	return true;
}

bool BattleManagerLocal::OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{
	bool is_attacker_win = (RESULT_TYPE_ATTACKER_WIN == ack->battle_result);
	bool is_defender_win = (RESULT_TYPE_DEFENDER_WIN == ack->battle_result);

	this->BattleFinishSpecialProcess(ack);

	BattleRoleList attacker_role_list; attacker_role_list.role_count = ack->attacker_list.role_count;
	BattleRoleList defender_role_list; defender_role_list.role_count = ack->defender_list.role_count;
	for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		attacker_role_list.role_id_list[i] = ack->attacker_list.role_list[i].uid;
	}
	for (int i = 0; i < ack->defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		defender_role_list.role_id_list[i] = ack->defender_list.role_list[i].uid;
	}

	for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		int role_id = ack->attacker_list.role_list[i].uid;
		BattleData_StatisticRole& role_statistic = ack->attacker_list.role_list[i].role_statistic;
		BattleData_BusinessRole& role_business = ack->attacker_list.role_list[i].role_business;
		bool is_flyaway = ack->attacker_list.role_list[i].is_flyaway > 0;
		bool is_leader = ack->attacker_list.role_list[i].is_leader > 0;
		BattleManagerLocal::GetInstance().OnRoleFinishFight(role_id, ack->battle_id, is_attacker_win, is_flyaway, is_leader, true, ack->battle_mode, ack->business_data, role_business, role_statistic,
			ack->statistic_data, ack->attacker_list.role_list[i].pet_num, ack->attacker_list.role_list[i].pet_stat_list, ack->attacker_list.role_list[i].pet_unique_id_list, attacker_role_list);
	}

	for (int i = 0; i < ack->defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		int role_id = ack->defender_list.role_list[i].uid;
		BattleData_StatisticRole& role_statistic = ack->defender_list.role_list[i].role_statistic;
		BattleData_BusinessRole& role_business = ack->defender_list.role_list[i].role_business;
		bool is_flyaway = ack->defender_list.role_list[i].is_flyaway > 0;
		bool is_leader = ack->defender_list.role_list[i].is_leader > 0;
		BattleManagerLocal::GetInstance().OnRoleFinishFight(role_id, ack->battle_id, is_defender_win, is_flyaway, is_leader, false, ack->battle_mode, ack->business_data, role_business, role_statistic,
			ack->statistic_data, ack->defender_list.role_list[i].pet_num, ack->defender_list.role_list[i].pet_stat_list, ack->defender_list.role_list[i].pet_unique_id_list, defender_role_list);
	}

	if (is_attacker_win)
	{
		switch (ack->battle_mode)
		{
		case BATTLE_MODE_FALLEN_GOD:
		case BATTLE_MODE_FIELD_BOSS:
		case BATTLE_MODE_MI_YI:
		case BATTLE_MODE_NORMAL_MONSTER:
		case BATTLE_MODE_SHAN_HAI_BOSS:
		case BATTLE_MODE_BIG_DIPPER:
		case BATTLE_MODE_HONGMENG_TIANJIE:
			{
				if (ack->business_data.is_record_first_kill > 0)
				{
					if (ack->business_data.record_file_id > 0)
					{
						ServerFirstKill::Instance().OnGiveFirstKillReward(&ack->attacker_list, ack->statistic_data.round_num, ack->monster_group_id, ack->battle_mode,
							ack->business_data.record_score, ack->business_data.record_file_id);
					}
					else
					{
						gamelog::g_log_first_kill.printf(LL_INFO, "%s add first_kill record and battle_video faild! monster_group_id[%d]", __FUNCTION__, ack->monster_group_id);
					}
				}

			}
			break;

		case BATTLE_MODE_ZHEN_ZHONG:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					int role_id = ack->attacker_list.role_list[i].uid;
					Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
					if (NULL != role)
					{
						Team* team = role->GetMyTeam();
						if (NULL != team)
						{
							Role* leader = team->GetLeaderRole();
							if (NULL != leader)
							{
								leader->GetRoleModuleManager()->GetResearchTask()->TeamLeaderSyncTaskAfterBattle();
								break;
							}
						}
					}

				}
			}
			break;
		}
	}

	return true;
}

void BattleManagerLocal::BattleFinishSpecialProcess(battlegameprotocol::BattleGameFightResultAck * ack)
{
	bool is_attacker_win = (RESULT_TYPE_ATTACKER_WIN == ack->battle_result);
	bool is_defender_win = (RESULT_TYPE_DEFENDER_WIN == ack->battle_result);
	int attack_guild = 0;
	int defend_guild = 0;
	for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		if (RobotManager::IsRobot(ack->attacker_list.role_list[i].uid))
		{
			continue;
		}
		bool is_flyaway = ack->attacker_list.role_list[i].is_flyaway > 0;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
		if (NULL == role) continue;
		if (is_attacker_win && !is_flyaway)
		{
			role->GetRoleModuleManager()->GetSalary()->OnRoleWinBattle(ack->battle_mode, ack->business_data.monster_group_id);
			role->GetRoleModuleManager()->GetSalary()->OnRoleKillMonsterGroup(ack->business_data.monster_group_id);
			if (BATTLE_MODE_BIG_DIPPER == ack->battle_mode && ack->business_data.deamon_cave_beat_waves > 0)
			{
				role->GetRoleModuleManager()->GetBigDipper()->OnHelpPass(ack->business_data.param2, ack->business_data.deamon_cave_beat_waves);
			}
			if (BATTLE_MODE_FALLEN_GOD == ack->battle_mode)
			{
				role->GetRoleModuleManager()->GetFallenGod()->OnHelpPass(ack->business_data.param2, ack->business_data.deamon_cave_beat_waves);
			}
		}
		if (role->InTeam())
		{
			Team * role_team = role->GetMyTeam();
			if (NULL != role_team)
			{
				attack_guild = role_team->GetAllTeamMemberGuildID();
			}
		}
		else
		{
			attack_guild = role->GetGuildID();
		}
	}

	for (int i = 0; i < ack->defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		if (RobotManager::IsRobot(ack->defender_list.role_list[i].uid))
		{
			continue;
		}
		bool is_flyaway = ack->defender_list.role_list[i].is_flyaway > 0;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->defender_list.role_list[i].uid));
		if (NULL == role) continue;
		if (is_defender_win && !is_flyaway)
		{
			role->GetRoleModuleManager()->GetSalary()->OnRoleWinBattle(ack->battle_mode, ack->business_data.monster_group_id);
			role->GetRoleModuleManager()->GetSalary()->OnRoleKillMonsterGroup(ack->business_data.monster_group_id);

			if (BATTLE_MODE_BIG_DIPPER == ack->battle_mode && ack->business_data.deamon_cave_beat_waves > 0)
			{
				role->GetRoleModuleManager()->GetBigDipper()->OnHelpPass(ack->business_data.param2, ack->business_data.deamon_cave_beat_waves);
			}

			if (BATTLE_MODE_FALLEN_GOD == ack->battle_mode)
			{
				role->GetRoleModuleManager()->GetFallenGod()->OnHelpPass(ack->business_data.param2, ack->business_data.deamon_cave_beat_waves);
			}
		}
		if (role->InTeam())
		{
			Team * role_team = role->GetMyTeam();
			if (NULL != role_team)
			{
				defend_guild = role_team->GetAllTeamMemberGuildID();
			}
		}
		else
		{
			defend_guild = role->GetGuildID();
		}
	}

	if (attack_guild != defend_guild)
	{
		for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			if (RobotManager::IsRobot(ack->attacker_list.role_list[i].uid))
			{
				continue;
			}
			bool is_flyaway = ack->attacker_list.role_list[i].is_flyaway > 0;
			if (is_attacker_win && !is_flyaway)
			{
				Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
				if (NULL == role) continue;

				Guild * role_guild = role->GetGuild();
				if (NULL != role_guild)
				{
					const WarthAddInfo*  add_cfg = LOGIC_CONFIG->GetGuildConfig()->GetWarthAdd(ack->battle_mode);
					if (NULL != add_cfg)
					{
						if (role->InTeam())
						{
							Team *role_team = role->GetMyTeam();
							if (NULL != role_team && role_team->GetAllTeamMemberGuildID() > 0)
							{
								int add_value = add_cfg->once_warth_add;
								if (role_team->IsLeader(role))
								{
									add_value += add_cfg->captain_warth_add;
								}
								role_guild->OnRankValueAdd(role, Guild::HALLOFFAME_TYPE_BATTLE, add_value);
							}
						}
						else
						{
							role_guild->OnRankValueAdd(role, Guild::HALLOFFAME_TYPE_BATTLE, add_cfg->once_warth_add);
						}
					}
				}
			}
		}

		for (int i = 0; i < ack->defender_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
		{
			if (RobotManager::IsRobot(ack->defender_list.role_list[i].uid))
			{
				continue;
			}
			bool is_flyaway = ack->defender_list.role_list[i].is_flyaway > 0;
			if (is_defender_win && !is_flyaway)
			{
				Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->defender_list.role_list[i].uid));
				if (NULL == role) continue;

				Guild * role_guild = role->GetGuild();
				if (NULL != role_guild)
				{
					const WarthAddInfo*  add_cfg = LOGIC_CONFIG->GetGuildConfig()->GetWarthAdd(ack->battle_mode);
					if (NULL != add_cfg)
					{
						if (role->InTeam())
						{
							Team *role_team = role->GetMyTeam();
							if (NULL != role_team && role_team->GetAllTeamMemberGuildID() > 0)
							{
								int add_value = add_cfg->once_warth_add;
								if (role_team->IsLeader(role))
								{
									add_value += add_cfg->captain_warth_add;
								}
								role_guild->OnRankValueAdd(role, Guild::HALLOFFAME_TYPE_BATTLE, add_value);
							}
						}
						else
						{
							role_guild->OnRankValueAdd(role, Guild::HALLOFFAME_TYPE_BATTLE, add_cfg->once_warth_add);
						}
					}
				}
			}
		}
	}
}

void BattleManagerLocal::OnRoleStopFight(Role* role)
{
	role->ActionStop();
	role->GetRoleStatusManager()->OnRoleFinishFight();
	role->RefreshEncounterInterval();

	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		int member_count = team->GetMemberCount();
		for (int i = 0; i < member_count; ++i)
		{
			Robot* robot = team->GetMemberRobotByIndex(i);
			if (NULL != robot)
			{
				robot->OnRobotStopBattle();
			}
		}
	}
}

void BattleManagerLocal::OnRoleStartOB(Role* role)
{

}

void BattleManagerLocal::OnRoleStopOB(Role* role)
{

}

bool BattleManagerLocal::IsBattleModeNeedPartner(int battle_mode)
{
	bool ret = true;
	switch (battle_mode)
	{
	case BATTLE_MODE_GUILD_HONOR_BATTLE:
		//case BATTLE_MODE_GUILD_FIGHT:
	case BATTLE_MODE_SQUARD_FIGHT:
	case BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI:
		//case BATTLE_MODE_ONLY_FIGHT:
		{
			ret = false;
			break;
		}
	}
	return ret;
}

bool BattleManagerLocal::IsBattleModeNeedTeamStandPos(int battle_mode)
{
	bool ret = true;
	switch (battle_mode)
	{
	case BATTLE_MODE_SQUARD_FIGHT:
		{
			ret = false;
		}
		break;

	}

	return ret;
}

void BattleManagerLocal::SetAutoCatchBusinessData(Role* role, BattleData_Business& business_data)
{
	// 寻找lv1的怪
	for (int i = 0; i < business_data.monster_list_size; ++i)
	{
		if (1 != business_data.monster_level_list[i]) continue;

		// 如果是个宝宝
		bool found = false;
		const AutoCatchPetParam& param = role->GetRoleModuleManager()->GetAutoCatchPet()->GetAutoCatchParam();
		for (int k = 0; k < MAX_AUTO_CATCH_NUM; ++k)
		{
			if (!param.auto_catch_list[k].IsValid()) continue;
			if (param.auto_catch_list[k].monster_id != business_data.monster_id_list[i]) continue;
			if (param.auto_catch_list[k].cur_catch_num >= param.auto_catch_list[k].max_catch_num) break; // 没有次数了

			const MonsterCfg* monster_cfg = MonsterPool::GetInstance().GetMonsterCfg(param.auto_catch_list[k].monster_id);
			if (NULL == monster_cfg) break;
			if (!monster_cfg->CanCatch()) break;

			int pet_id = monster_cfg->GetCatchPetId();
			const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
			if (NULL == pet_cfg)
			{
				break;
			}

			int rate_common_promote = 0;
			int rate_silver_promote = 0;
			int rate_gold_promote = 0;

			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
			if (NULL != ramgr)
			{
				if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SEAL_PROMOTE))
				{
					const RandActivitySealPromoteConfig * config = static_cast<const RandActivitySealPromoteConfig *>
						(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SEAL_PROMOTE));
					if (NULL != config)
					{
						const RASealPromoteCfg * act_cfg = config->GetSealPromoteCfg(ramgr, pet_id);
						if (NULL != act_cfg)
						{
							rate_common_promote = act_cfg->rate_common;
							rate_silver_promote = act_cfg->rate_silver;
							rate_gold_promote = act_cfg->rate_gold;
						}
					}
				}
			}

			business_data.auto_catch_monster_id = business_data.monster_id_list[i];
			business_data.auto_catch_pet_id = pet_id;
			business_data.available_common_card_num = param.max_seal_card_common_num - param.cur_seal_card_common_num;
			business_data.available_silver_card_num = param.max_seal_card_silver_num - param.cur_seal_card_silver_num;
			business_data.available_gold_card_num = param.max_seal_card_gold_num - param.cur_seal_card_gold_num;

			// 检查背包是否有足够的卡
			ItemID common_card_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->common_sealcard;
			int common_card_num = role->GetRoleModuleManager()->GetKnapsack()->Count(common_card_id);
			if (business_data.available_common_card_num > common_card_num)
			{
				business_data.available_common_card_num = common_card_num;
			}

			ItemID silver_card_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->silver_sealcard;
			int silver_card_num = role->GetRoleModuleManager()->GetKnapsack()->Count(silver_card_id);
			if (business_data.available_silver_card_num > silver_card_num)
			{
				business_data.available_silver_card_num = silver_card_num;
			}

			ItemID gold_card_id = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg()->gold_sealcard;
			int gold_card_num = role->GetRoleModuleManager()->GetKnapsack()->Count(gold_card_id);
			if (business_data.available_gold_card_num > gold_card_num)
			{
				business_data.available_gold_card_num = gold_card_num;
			}

			business_data.succ_rate_common = pet_cfg->rate_common + rate_common_promote;
			business_data.succ_rate_silver = pet_cfg->rate_silver + rate_silver_promote;
			business_data.succ_rate_gold = pet_cfg->rate_gold + rate_gold_promote;
			business_data.must_succ_times = pet_cfg->catch_fixed_times;
			business_data.pr_up_on_fail = pet_cfg->pr_up_on_fail;
			business_data.common_card_item_id = common_card_id;
			business_data.silver_card_item_id = silver_card_id;
			business_data.gold_card_item_id = gold_card_id;

			found = true;
			break;
		}

		if (found)
		{
			break;
		}
	}
}

void BattleManagerLocal::StartInitialBattle(Role* role)
{
	int seq = LOGIC_CONFIG->GetInitialBattleConfig()->GetInitialBattleSeq(role->GetProfession());
	if (seq > 0)
	{
		BattleData_Business business;
		business.Reset();
		business.role_runaway_always_fail = 1;

		int battle_mode = BATTLE_MODE_INITIAL_FIGHT;
		BattleManagerLocal::GetInstance().StartDemoFightReq(role, seq, battle_mode, &business);
	}
}

void BattleManagerLocal::OnRoleSealPetGiveReward(Role* role, bool is_get_pet, const ItemConfigData& catch_reward_item, int pet_id, int battle_mode, bool is_give_guide_pet)
{
	ItemDataWrapper temp;
	if (is_get_pet) //因为可能需要写数据入灵宠秘境奖励记录，所以提前随机宠物数据
	{
		temp.item_id = catch_reward_item.item_id;
		temp.num = 1;
		temp.is_bind = catch_reward_item.is_bind ? 1 : 0;

		if (is_give_guide_pet)
		{
			const FuncGuideOtherCfg & other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();
			if (!role->GetRoleModuleManager()->GetPet()->FillSpecialPetData(other_cfg.catch_pet_guide_monster_pet_item_id, temp))
			{
				return;
			}
		}
		else
		{
			static NetValueItemParam param;
			param.Reset();
			if (!LOGIC_CONFIG->GetPetConfig()->RandPetParam(pet_id, 1, false, param))
			{
				return;
			}
			param.GetData(temp.param_data);
		}

		temp.has_param = 1;
	}
	else
	{
		temp.item_id = catch_reward_item.item_id;
		temp.is_bind = catch_reward_item.is_bind ? 1 : 0;
		temp.num = catch_reward_item.num;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(temp, is_get_pet ? PUT_REASON_CATCH_PET : PUT_REASON_CATCH_PET_SHARD))
	{
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.item_list[0].item_id = catch_reward_item.item_id;
		contentparam.item_list[0].num = catch_reward_item.num;
		contentparam.item_list[0].is_bind = true;

		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
		MailRoute::MailToUser(IntToUid(role->GetUID()), MAIL_REASON_DEFAULT, contentparam);
	}

	if (BATTLE_MODE_PET_MONOPOLY == battle_mode)
	{
		if (is_get_pet)
		{
			EventHandler::Instance().OnRolePetMonopolyCatchPet(role, temp);
		}
		else
		{
			EventHandler::Instance().OnRolePetMonopolyGetReward(role, catch_reward_item);
		}
	}
}

void BattleManagerLocal::GetRoleAnimationSpeed(Role* role, int battle_mode, bool is_mine_monster, bool is_in_team, ARG_OUT bool* out_is_force_speed, ARG_OUT int* out_animation_speed)
{
	// 特殊处理竞技场的倍速限制
	if (BATTLE_MODE_SINGLE_ARENA == battle_mode)
	{
		*out_animation_speed = role->GetRoleModuleManager()->GetCommonData().single_arena_animation_speed;
		*out_is_force_speed = false;
		return;
	}

	// 场景限制时以场景设定为优先
	SpecialLogic * sp_logic = role->GetScene()->GetSpecialLogic();
	if (NULL != sp_logic)
	{
		int temp_mode = 0;
		bool temp_is_force_speed = false;
		if (sp_logic->GetBattleAnimationSpeedType(role, &temp_mode, &temp_is_force_speed))
		{
			*out_animation_speed = temp_mode;
			*out_is_force_speed = temp_is_force_speed;
			return;
		}
	}

	// 组队现在也可以二倍速,需看配置以及队长的设定
	if (is_in_team)
	{
		if (LOGIC_CONFIG->GetAnimationSpeedConfig()->CanTeamOpenDoubleSpeed(battle_mode))
		{
			*out_animation_speed = role->GetRoleModuleManager()->GetCommonData().team_animation_speed;
			*out_is_force_speed = false;
		}
		else
		{
		*out_animation_speed = ANIMATION_SPEED_NORMAL;
		*out_is_force_speed = true;
		}
		
		return;
	}

	// 判断单人巡逻
	if (is_mine_monster && role->GetScene()->IsInStaticScene() && role->GetRoleModuleManager()->IsPatrolling())
	{
		*out_animation_speed = ANIMATION_SPEED_NORMAL; // 单人巡逻打暗怪不能加速
		*out_is_force_speed = true;
		return;
	}

	// 若没有限制，就看是否FunOpen了
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_DOUBLE_SPEED_ANIMATION))
	{
		*out_animation_speed = ANIMATION_SPEED_NORMAL;
		*out_is_force_speed = true;

		return;
	}

	// 不限制
	*out_animation_speed = role->GetRoleModuleManager()->GetCommonData().animation_speed;
	*out_is_force_speed = false;
}

bool BattleManagerLocal::IsMineMonster(int monster_group_id, int battle_mode, bool business_is_real_mine)
{
	return BATTLE_MINE_MONSTER_GROUP == monster_group_id
		&& BATTLE_MODE_MINE_MONSTER == battle_mode
		&& business_is_real_mine;
}

bool BattleManagerLocal::CanRoleStartBattle(Role* role, int battle_mode)
{
	const RoleBattleInfo* role_info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != role_info)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE_6);
#endif

		this->CheckRoleReallyInBattle(role, __FUNCTION__);
		return false;
	}

	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return false;
	}

	if (role->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
	{
		role->NoticeNum(errornum::EN_ESCORT_CANNOT_FIGHT);
		return false;
	}

	return true;
}

bool BattleManagerLocal::IsRoleBattleRequestSendToBigCross(Role* role)
{
	SpecialLogic* spl = role->GetScene()->GetSpecialLogic();
	if (NULL == spl) return false;

	// 先简单地用场景来判断
	if (spl->GetSceneType() == Scene::SCENE_TYPE_UTA) return true;


	return false;
}

void BattleManagerLocal::TransferBattleRequestToBigcrossBattle(int uid, const char* data, int length)
{
	static bigchgameprotocol::HGameBigCTransferMsgFromHiddenToBattle msg;
	if (length <= 0 || length > sizeof(msg.data)) return;

	msg.uid = uid;
	msg.length = length;
	memcpy(msg.data, data, length);

	int send_length = sizeof(msg) - sizeof(msg.data) + msg.length;
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, send_length);
}

void BattleManagerLocal::CheckRoleInBigCrossBattle(Role* role)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	bigchgameprotocol::HGameBigCCheckUserInBigCrossBattle msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();

	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void BattleManagerLocal::CheckTeamNoticeDoubleSpeed(battlegameprotocol::BattleGameStartFightAck* ack)
{
	if (ack->is_pvp > 0) return;
	if (0 == ack->is_team_up) return;
	if (ack->role_count <= 0) return;

	for (int i = 0; i < ack->role_count; ++i)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->role_list[i].uid));
		if (NULL == role) continue;
		
		Team* team = role->GetMyTeam();
		if (NULL == team) continue;

		TeamMember* member = team->GetMemberInfoByUID(ack->role_list[i].uid);
		if (NULL == member || !member->notice_on_double_speed) continue;

		member->notice_on_double_speed = false;

		if (ANIMATION_SPEED_DOUBLE == ack->animation_speed_type)
		{
			role->NoticeNum(noticenum::NT_NOW_IS_DOUBLE_SPEED_ANIMATION);
		}
	}
}


void BattleManagerLocal::CheckElementElf(int monster_group_id, int battle_mode, BattleData_Business& business_data, int today_showup_times)
{
	if (0 == monster_group_id) return;

	int reward_group = 0;
	if (!LOGIC_CONFIG->GetElementElfConfig()
		->RandElementElfShowUp(monster_group_id, battle_mode, today_showup_times, &reward_group))
	{
		return;
	}

	business_data.gen_element_elf_monster = 1;
	business_data.element_elf_monster_id =
		LOGIC_CONFIG->GetElementElfConfig()
		->RandMonsterId();

	business_data.element_elf_reward_group = 
		static_cast<char>(reward_group);
}

void BattleManagerLocal::CheckTeamElementElf(Team* team, int monster_group_id, int battle_mode, BattleData_Business& business_data)
{
	int max_reward_times = 0;
	int max_today_showup_times = 0;
	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		Role* role = team->GetMemberRoleByIndex(i);
		if (NULL == role) continue;

		max_today_showup_times = GetMax(max_today_showup_times, 
			role->GetRoleModuleManager()
			->GetElementElf()->GetTodayShowUpTimes());
	}

	this->CheckElementElf(monster_group_id, battle_mode, business_data, max_today_showup_times);
}

void BattleManagerLocal::CheckRoleElementElf(Role* role, int monster_group_id, int battle_mode, BattleData_Business& business_data)
{
	int today_showup_times = role->GetRoleModuleManager()
		->GetElementElf()->GetTodayShowUpTimes();

	this->CheckElementElf(monster_group_id, battle_mode, business_data, today_showup_times);
}

bool BattleManagerLocal::IsRobotRunAwayBeforeStart(Robot* robot, Role* leader, int battle_mode, int monster_group_id, const BattleData_Business& business)
{
	int robot_bound_battle_mode = robot->GetBoundBattleMode();
	if (robot->GetRealRoleUID() > 0)
	{
		bool is_mine_monster = this->IsMineMonster(monster_group_id, battle_mode, business.is_real_mine > 0);
		if (is_mine_monster)
		{
			// 就让分身机器人打暗雷怪吧
			return false;
		}

		if (robot_bound_battle_mode != battle_mode)
		{
			return true;
		}

		const MonsterGroupCfg* cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
		if (NULL == cfg) return true;

		if (BATTLE_MODE_NORMAL_MONSTER == battle_mode && !cfg->is_branch_task_monster && !cfg->is_advanced_task_monster)
		{
			return true;
		}

		return false;
	}

	bool is_one_dragon_battle_mode =
		(BATTLE_MODE_ZHEN_ZHONG == battle_mode) ||
		(BATTLE_MODE_DEMON_CAVE == battle_mode) ||
		(BATTLE_MODE_FANTASY_FB == battle_mode) ||
		(BATTLE_MODE_BIG_DIPPER == battle_mode) ||
		(BATTLE_MODE_WAN_LING_FB == battle_mode);

	int team_type_target_type = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeTargetType(robot->GetBoundTeamType());
	bool is_one_dragon_team_type =
		(TEAM_TYPE_ONE_DRAGON == robot->GetBoundTeamType()) ||
		(TEAM_TYPE_RESEARCH_TASK == robot->GetBoundTeamType()) ||
		(TEAM_TYPE_MONSTER_WAVE == robot->GetBoundTeamType()) ||
		(TEAM_TYPE_FANTASY_FB == robot->GetBoundTeamType()) ||
		(TEAM_TYPE_TARGET_TYPE_WAN_LING_HUA_BEN == team_type_target_type);

	if (is_one_dragon_team_type && !is_one_dragon_battle_mode)
	{
		return true;
	}
	else if (!is_one_dragon_team_type &&
		robot_bound_battle_mode != BATTLE_MODE_INVALID &&
		robot_bound_battle_mode != battle_mode)
	{
		return true;
	}

	int robot_bound_team_type = robot->GetBoundTeamType();
	if (TEAM_TYPE_SCHOOL_TASK == robot_bound_team_type &&
		NULL != leader && !leader->GetRoleModuleManager()->GetSchoolTask()->IsInTaskScene())
	{
		return true;
	}

	return false;
}

void BattleManagerLocal::CheckRoleReallyInBattle(Role* role, const char* func)
{
#ifdef _DEBUG
	gamelog::g_log_battle.printf(LL_INFO, "CheckRoleReallyInBattle role[%d,%s] from[%s]", role->GetUID(), role->GetName(), func);
#endif

	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL == info)
	{
		return;
	}

	// Release版本才检查是否连通，因为还想暴露BUG在内网
#ifndef _DEBUG
	if (!this->IsBattleServerConnected(info->battle_server_index))
	{
		role->GetRoleStatusManager()->OnRoleFinishFight();
		return;
	}
#endif

	//!< 经常还是会出现测试9的报错（移动时出现正在战斗中）
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (role->GetLastCheckReallyInBattleTime() + 2 >= now) return;

	role->OnCheckReallyInBattle(now);

	static battlegameprotocol::GameBattleCheckRoleReallyInBattleReq req;
	req.uid = role->GetUID();
	req.bg_netid = role->GetBGNetID();

	World::GetInstWorld()->SendMsgToOneBattle(info->battle_server_index, (const char*)&req, sizeof(req));
}

int BattleManagerLocal::GetBattleModeSceneBuff(Role * role, int battle_mode)
{
	if (battle_mode == BATTLE_MODE_BRAVE_GROUND)
	{
		Scene * scene = role->GetScene();
		if (scene != NULL && scene->GetSceneType() == Scene::SCENE_TYPE_BRAVE_GROUND)
		{
			SpecialLogic * spc = role->GetScene()->GetSpecialLogic();
			if (spc != NULL)
			{
				SpecialLogicBraveGround * s = (SpecialLogicBraveGround *)spc;
				return s->GetBuffId();
			}
		}
	}

	return 0;
}

int BattleManagerLocal::GetBattleModeNoticeNum(Role * role, int battle_mode)
{
	if (battle_mode == BATTLE_MODE_MAZE_RACER)
	{
		Team * team = role->GetMyTeam();
		if (team != NULL)
		{
			if (MazeRacerManager::Instance().GetTeamSufferAmulet(team->GetTeamIndex(), MAZE_AMULET_EFFECT_SET_WAR_INTERVAL) != NULL)
			{
				return errornum::EN_MAZE_RACER_FIERCE_BATTLE;
			}
		}
	}

	return 0;
}

void BattleManagerLocal::ForceStopRoleOB(Role* role)
{
	const RoleBattleInfo* rbi = this->GetRoleBattleInfoConst(role->GetUID());
	if (NULL != rbi && rbi->is_ob)
	{
		// 强制关闭OB状态
		this->OnRoleStopOB(role);

		int battle_idx = -1;
		int battle_id = rbi->battle_id;
		BattleLocalInfo* local_info = this->GetBattleLocalInfo(rbi->battle_id);
		if (NULL != local_info)
		{
			battle_idx = local_info->battle_server_index;
			local_info->RemoveObserver(role->GetUID());
		}

		this->RemoveRoleBattleInfo(role->GetUID());

		{
			// 通知客户端退出观战
			static Protocol::SCBattleEndForAudience resp;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&resp, sizeof(resp));
		}

		// 发到跨服，强制结束OB
		battlegameprotocol::GameBattleForceStopOBNotice notice;
		notice.uid = role->GetUID();
		notice.bg_netid = role->GetBGNetID();
		notice.battle_id = battle_id;
		World::GetInstWorld()->SendMsgToOneBattle(battle_idx, (const char*)&notice, sizeof(notice));
	}
}

void BattleManagerLocal::OnUserLoginOriginGame(int role_id)
{
	const RoleBattleInfo* rbi = this->GetRoleBattleInfoConst(role_id);
	if (NULL != rbi && !rbi->is_ob)
	{
		const BattleLocalInfo* info = this->GetBattleLocalInfoConst(rbi->battle_id);
		if (NULL != info)
		{
			crossgameprotocal::HiddenGameContinueCrossBattle hgccb;
			hgccb.role_id = role_id;

			InternalComm::Instance().SendToGameThroughCrossByRoleId((const char*)&hgccb, sizeof(hgccb), role_id);
		}
	}
}

bool BattleManagerLocal::CheckRoleCanStartBattle(int role_id, int battle_mode)
{
	const RoleBattleInfo* info = this->GetRoleBattleInfoConst(role_id);
	if (NULL != info)
	{
		// 有人正在打， 不能开打
		return false;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (NULL == role)
	{
		return false;
	}

	if (!role->GetRoleStatusManager()->CanRoleStartFight(battle_mode))
	{
		// 有人处于不能开打状态，不能开打
		return false;
	}

	return true;
}
