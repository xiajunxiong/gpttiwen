#include "role.h"
#include "world.h"
#include "engineadapter.h"
#include "gamelog.h"
#include "rmibackobjdef.h"
#include "internalcomm.h"
#include "globalconfig/spidconfig.h"
#include "item/knapsack.h"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/professionconfig.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "config/otherconfig/sceneextraconfig.hpp"
#include "friend/friendmanager.h"

#include "task/taskrecorder.h"
#include "chat/chatmanager.h"

#include "battle/battle_manager_local.hpp"
#include "npc/npcmanager.h"
#include "servercommon/logdef.h"

#include "obj/character/npc_monster.h"
#include "obj/character/npc_gather.h"
#include "obj/otherobj/specialtransport.h"
#include "obj/otherobj/treasurechest.h"
#include "obj/otherobj/npcother.h"
#include "other/loginsystem/loginsystem.hpp"

#include "globalconfig/globalconfig.h"
#include "global/rank/rankmanager.hpp"
#include "global/mail/mailmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "global/guild/guildmember.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guild.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "global/cross/crossuserregister.hpp"
#include "global/cross/crossuserregister.hpp"
#include "global/cross/crossusershadow.hpp"
#include "global/team/teammatch.hpp"
#include "global/cross/crossmanager.h"
#include "global/jieyi/jieyizumanager.h"
#include "global/trademarket/trademarketrouter.hpp"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "global/courseofgrowthmanager/courseofgrowthmanager.hpp"

#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/skilllistparam.hpp"
#include "servercommon/struct/global/maillistparam.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/userprotocal/msgcrosscommon.h"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/serverdef.h"
#include "servercommon/string/gameworldstr.h"
#include "global/cross/crossuserregister.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/newlifeskillconfig.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "global/activity/activityimpl/activitychiefpeakbattle.hpp"
#include "global/activity/activityimpl/activityguildhonorbattle.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "global/jieyi/jieyizumanager.h"

#include "scene/map.h"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"

#include "protocol/msgscene.h"
#include "protocol/msgrole.h"
#include "protocol/msgitem.h"
#include "protocol/msgnpc.h"
#include "protocol/msgserver.h"
#include "protocol/msgfishground.hpp"
#include "servercommon/internalprotocal/lgprotocal.h"

#include "other/escort/escort.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/pet/pet.hpp"
#include "other/workshop/workshop.hpp"
#include "other/event/eventhandler.hpp"
#include "other/currencyconfig.hpp"
#include "other/baby/baby.hpp"
#include "other/title/title.hpp"
#include "other/skill/roleskill.hpp"
#include "other/role_setting/rolesetting.hpp"
#include "other/mentor/shitu.hpp"
#include "other/offlineexp/offlineexp.hpp"
#include "other/buffmanager/buffmanager.hpp"
#include "other/fabao/fabao.hpp"
#include "other/mapunlock/mapunlock.hpp"
#include "other/researchtask/researchtask.h"
#include "other/shop/shop.h"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/marriage/marriage.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/funopen/funopen.hpp"
#include "other/challengefb/rolechallengefb.hpp"
#include "other/funcguide/funcguide.hpp"
#include "equipment/equipmentmanager.h"
#include "obj/character/robot.h"
#include "obj/character/npc_chosed.h"
#include "other/route/mailroute.hpp"
#include "other/specialappearance/specialappearance.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "task/taskmanager.h"
#include "other/rolemail/rolemail.hpp"
#include "other/capability/capability.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "other/monitor/systemmonitor.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "other/collection/collection.hpp"
#include "other/facescore/facescore.hpp"
#include "other/taskchain/taskchain.hpp"
#include "other/prestige/prestige.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/medal/medal.hpp"
#include "other/funcpreview/funcpreview.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "other/partner/partner.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/miyi/rolemiyi.hpp"
#include "servercommon/struct/rolefirstkillparam.hpp"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "other/surface/surface.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/train/train.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "dbdisconnectrolesavecache.hpp"
#include "other/mapexploration/mapexploration.hpp"
#include "other/salary/salary.hpp"
#include "other/roleheartskill/roleheartskill.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "other/duel/duel.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "other/yinyangeye/yinyangeye.hpp"
#include "equipment/accessoriesmentmanager.h"
#include "friend/friendintimacy.hpp"
#include "other/endlesstower/endlesstower.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "other/rolegatherfb/gatherfbconfig.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "obj/otherobj/xunyoucar.hpp"
#include "other/minigame/minigame.hpp"
#include "global/qingyuan/qingyuanmanager.h"
#include "obj/otherobj/peaktokenobj.hpp"
#include "other/dayreward/dayreward.hpp"
#include "other/levelcomplement/newlevelcomplement.hpp"
#include "obj/otherobj/tuzikuaipaotuzi.hpp"
#include "other/rolehundredgodboss/rolehundredgodboss.hpp"
#include "other/formation/formation.hpp"
#include "global/system//forbid_user_timer.hpp"
#include "other/zaohuaequip/zaohuaequipmanager.hpp"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "other/flyupsystem/flyupststemconfig.hpp"
#include "other/fuyu/fuyumanager.hpp"
#include "other/huanshou/rolehuanshou.hpp"
#include "other/couragechallenge/newcouragechallenge.hpp"

Role::Role() : Character(Obj::OBJ_TYPE_ROLE), m_user_id(INVALID_USER_ID), m_uid(0), m_userip(0), m_netid(-1, -1), m_plat_spid(0), m_account_user_num(0), m_last_msg_from_client_time(0),
m_last_msg_from_client_timestamp(0), m_isforce_kickdelay(false), m_birth_pos(0, 0), m_aoi_handle(0), m_last_scene_id(0),
m_last_scene_pos(0, 0), m_authority_type(0), m_color(0), m_avatar_type(0), m_headshot_id(0), m_level(0), m_profession(0), m_top_level(0), m_next_top_level_refresh_timestamp(0), m_avatar_timestamp(0),
m_exp(0), m_max_exp(0), m_last_online_dayid(0), m_next_reset_module_data_timestamp(0), m_last_reset_module_data_timestamp(0),
m_next_save_time(0), m_online_time(0), m_today_online_time(0), m_lastday_online_time(0), m_create_time(0), m_last_online_stati_time(0),
m_last_today_online_stati_time(0), m_login_timestamp(0), m_role_module_mgr_barrier_checker_1(1),
m_role_module_manager(NULL), m_role_module_mgr_barrier_checker_2(2), m_gather_need_timestamp(0), m_fish_fb_seq(-1), m_fish_need_timestamp(0u), m_last_check_really_in_battle_time(0u),
m_is_testing(false), m_team(NULL), m_is_auto_gather(false), m_forbid_talk_time(0), m_forbid_talk_type(FORBID_TALK_TYPE_DEFAULT), m_role_unique_id_in_scene(0)
{
	m_pos_data.m_x = 0;
	m_pos_data.m_y = 0;
	memset(m_name, 0, sizeof(m_name));
	memset(m_plat_name, 0, sizeof(m_plat_name));

	UniqueIDGenerator::Instance()->GenUniqueID(&m_role_unique_id_in_scene);

	gamelog::g_log_role_unique_check.printf(LL_INFO, "Role::Role(1) role[%d,%s] unique_id[%lld]", m_uid, m_name, m_role_unique_id_in_scene);
}

Role::Role(const GSNetID &_netid, const GameName _name) : Character(Obj::OBJ_TYPE_ROLE), m_user_id(INVALID_USER_ID), m_uid(0), m_userip(0), m_netid(_netid), m_plat_spid(0), m_account_user_num(0), m_last_msg_from_client_time(0),
m_last_msg_from_client_timestamp(0), m_isforce_kickdelay(false), m_birth_pos(0, 0), m_aoi_handle(0), m_last_scene_id(0),
m_last_scene_pos(0, 0), m_authority_type(0), m_color(0), m_avatar_type(0), m_headshot_id(0), m_level(0), m_profession(0), m_top_level(0), m_next_top_level_refresh_timestamp(0), m_avatar_timestamp(0),
m_exp(0), m_max_exp(0), m_last_online_dayid(0), m_next_reset_module_data_timestamp(0), m_last_reset_module_data_timestamp(0),
m_next_save_time(0), m_online_time(0), m_today_online_time(0), m_lastday_online_time(0), m_create_time(0), m_last_online_stati_time(0),
m_last_today_online_stati_time(0), m_login_timestamp(0), m_role_module_manager(NULL), m_gather_need_timestamp(0), m_fish_fb_seq(-1), m_fish_need_timestamp(0u), m_last_check_really_in_battle_time(0u),
m_is_testing(false), m_team(NULL), m_is_auto_gather(false), m_forbid_talk_time(0), m_forbid_talk_type(FORBID_TALK_TYPE_DEFAULT), m_role_unique_id_in_scene(0)
{
	m_pos_data.m_x = 0;
	m_pos_data.m_y = 0;
	m_bg_netid.net_id = m_netid.netid;
	m_bg_netid.gateway_index = static_cast<short>(World::GetInstWorld()->GetGateWayIndexByNetId(m_netid.gateway_netid));

	F_STRNCPY(m_name, _name, sizeof(m_name));
	memset(m_plat_name, 0, sizeof(m_plat_name));
	pre_inv_chat_group_tamp = 0;

	UniqueIDGenerator::Instance()->GenUniqueID(&m_role_unique_id_in_scene);

	gamelog::g_log_role_unique_check.printf(LL_INFO, "Role::Role(2) role[%d,%s] unique_id[%lld]", m_uid, m_name, m_role_unique_id_in_scene);
}

Role::~Role()
{
	if (NULL != m_role_module_manager)
	{
		delete m_role_module_manager;
		m_role_module_manager = NULL;
	}

	gamelog::g_log_role_unique_check.printf(LL_INFO, "Role::~Role role[%d,%s] unique_id[%lld]", m_uid, m_name, m_role_unique_id_in_scene);
}

void Role::Init(int plat_spid, PlatName pname, const UserID &user_id, RoleModuleManager* role_module_manager,
	const RoleInitParam &param,
	const RoleOtherInitParam &other_p,
	const ItemListParam& item_list_param,
	const SkillListParam& skill_list_param,
	const MailListParam& maillist_param,
	const RoleFirstKillParamList& first_kill_list,
	const RandActivityRoleParamList & rand_activity_role_data_list,
	const RoleCommonSaveDataParam & role_common_save_data_list,
	const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
	const GodPrintItemListParam & god_print_item_list,
	const NewCourageChallengeLevelParamList & new_courage_challenge_list)
{
	m_plat_spid = param.plat_spid;
	F_STRNCPY(m_plat_name, pname, sizeof(m_plat_name));

	m_user_id = user_id;
	m_uid = UidToInt(user_id);
	m_unique_server_id = UniqueServerID(LocalConfig::Instance().GetPlatType(), m_user_id.db_index);
	m_avatar_type = param.avatar_type;
	m_headshot_id = param.headshot_id;
	m_profession = param.profession;
	m_top_level = param.top_level;
	m_next_top_level_refresh_timestamp = param.next_top_level_refresh_timestamp;
	m_color = param.color;
	m_level = param.level;
	m_avatar_timestamp = param.avatar_timestamp;
	m_authority_type = param.authority_type;
	m_account_user_num = param.account_user_num;

	m_exp = param.exp;
	m_max_exp = param.max_exp;

	Posi pos = Posi(param.scene_x, param.scene_y);
	SetPos(pos);

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	m_last_scene_id = param.last_scene_id;
	m_last_scene_pos.Init(param.last_scene_x, param.last_scene_y);

	m_last_online_dayid = param.last_online_dayid;
	m_last_reset_module_data_timestamp = param.last_reset_module_timestamp;

	//数据库默认值为0，如果新号创建出来不希望立刻执行OnDayChange和OnResetData,则去除此注释
	//if (0 == m_last_online_dayid) m_last_online_dayid = static_cast<unsigned int>(EngineAdapter::Instance().DayID());
	//if (0 == m_last_reset_module_data_timestamp) m_last_reset_module_data_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	m_online_time = param.online_time;
	m_today_online_time = param.today_online_time;
	m_lastday_online_time = param.lastday_online_time;
	m_create_time = param.create_time;
	m_last_online_stati_time = now;
	m_last_today_online_stati_time = now;
	m_forbid_talk_time = (time_t)param.forbid_talk_time;
	m_forbid_talk_type = param.forbid_talk_type;
	m_last_msg_from_client_time = EngineAdapter::Instance().GetGameTime();
	m_last_msg_from_client_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	unsigned int ROLE_SAVE_TIME = GLOBALCONFIG->GetKeyConfig_RoleSaveTime();
	m_next_save_time = now + ROLE_SAVE_TIME;

	m_role_status_manager.SetRole(this);
	m_scene_status.Init(this);

	Team* team = TeamManager::Instance().GetTeam(this);
	if (NULL != team)
	{
		this->SetTeam(team);
		team->UpdateMemberRolePointer(this);
	}
	else
	{
		this->SetTeam(NULL);
	}

	// 如果外部传入了role_module_manager， 则使用它，无需再使用other_p等数据来初始化
	if (NULL != role_module_manager)
	{
		m_role_module_manager = role_module_manager;
		m_role_module_manager->SetRole(this); // -- 由于RoleModuleManager没有重新创建，所以指向的Role应该改为新的Role
	}
	else // 否则创建一个RoleModuleManager实例，并用Other_p等数据来初始化
	{
		m_role_module_manager = new RoleModuleManager(this);
		m_role_module_manager->InitRoleModuleData(param, other_p, item_list_param, skill_list_param, maillist_param, first_kill_list,
			rand_activity_role_data_list, role_common_save_data_list, role_courage_challenge_list, god_print_item_list, new_courage_challenge_list);
	}

	this->OnInitFinish();
}

void Role::OnInitFinish()
{
	this->RecalNextResetDataTimeStamp();
}

void Role::GetPlatName(PlatName pname)
{
	F_STRNCPY(pname, m_plat_name, sizeof(PlatName));
}

UniqueUserID Role::GetUniqueUserID()
{
	static const int PLAT_TYPE = LocalConfig::Instance().GetPlatType();

	return UniqueUserID(PLAT_TYPE, m_user_id);
}

UniqueUserID Role::GetUniqueUserID(const UserID& user_id)
{
	static const int PLAT_TYPE = LocalConfig::Instance().GetPlatType();

	return UniqueUserID(PLAT_TYPE, user_id);
}

long long Role::GetUUID()
{
	return UniqueUserIDToLongLong(this->GetUniqueUserID());
}

void Role::OnRecvFromClient(unsigned long game_time, IP user_ip)
{
	m_last_msg_from_client_time = game_time;
	m_last_msg_from_client_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	m_userip = user_ip;
}

void Role::OnLoginBroadCast(bool is_gm)
{
	time_t now = EngineAdapter::Instance().Time();
	CommonDataParam & data = m_role_module_manager->GetCommonData();
	if (is_gm || (now >= data.chief_election_login_broadcast_time + 3 * 3600 && WorldStatus::Instance().IsChiefElectionWinner(this->GetUID(), this->GetBaseProfession())))
	{
		data.chief_election_login_broadcast_time = (unsigned int)now;

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_winner_login_broadcast,
			m_role_module_manager->GetRole()->GetBaseProfession(), m_role_module_manager->GetRole()->GetName());
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE)))
			{
				World::GetInstWorld()->SendToALLGameAllGateway((const char *)&sm, sendlen);
			}
		}
	}
}

void Role::OnLogin(long long last_save_time, IP userip, int plat_spid)
{
	m_userip = userip;
	m_login_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	if (AUTHORITY_TYPE_TEST == m_authority_type)
	{
		unsigned int no_login_sec = SECOND_PER_DAY * 3;

		bool too_long_no_login = false;
		unsigned int last_logout_timestamp = this->GetRoleModuleManager()
			->GetCommonData().last_logout_timestamp;
		if (m_login_timestamp > last_logout_timestamp + no_login_sec)
		{
			unsigned int last_logout_timestamp_2 = this->GetRoleModuleManager()
				->GetCommonData().last_logout_timestamp_2;
			if (last_logout_timestamp_2 > 0u &&
				m_login_timestamp > last_logout_timestamp_2 + no_login_sec)
			{
				too_long_no_login = true;
			}
		}

		if (too_long_no_login) // 该内玩达到封号条件
		{
			ForbidUserTimer *timer = new ForbidUserTimer(m_user_id, "forbid_neiwan_account");
			EngineAdapter::Instance().CreateTimer(1, timer);
			gamelog::g_log_forbid.printf(LL_INFO, "Role::OnLogin【NeiWan Forbid】role[%d,%s] last_logout_timestamp[%u] m_login_timestamp[%u] last_logout_timestamp_2[%u]",
				m_uid, m_name, last_logout_timestamp, m_login_timestamp,
				this->GetRoleModuleManager()
				->GetCommonData().last_logout_timestamp_2);
		}
	}

	{
		gamelog::g_log_login.printf(LL_DEBUG, "[Role::OnLogin][%d][%s]", UidToInt(m_user_id), m_name);
		ROLE_LOG_QUICK10(LOG_TYPE_LOGIN, this, this->GetIP(), plat_spid, NULL, NULL, this->GetLevel(),
			this->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam().history_chongzhi_num,
			0, this->GetRoleModuleManager()->GetCapability()->GetRoleCapability(), 0);
		//gamelog::g_log_global_user_state.printf(LL_INFO, "[LoginServer::OnGlobalUserLogin Success] user[%s, %d] ip[%u] is_micro_pc[%d].", m_name, this->GetUID(), this->GetIP(), 1);
	}

	EngineAdapter::Instance().NetHasCheck(this->GetNetId(), this->GetName(), this->GetUID());
	EventHandler::Instance().OnRoleLogin(this);

	//先计算一些天数,可供后面一些使用
	m_role_module_manager->OnLoginCalOfflineDay(m_login_timestamp, (unsigned int)last_save_time);
	m_role_module_manager->OnLoginCalCreateDay();


	m_role_module_manager->GetTaskRecorder()->OnRoleLogin();
	m_role_module_manager->GetBaby()->OnRoleLogin();
	m_role_module_manager->GetWorkshop()->OnRoleLogin();
	m_role_module_manager->GetRoleSetting()->OnRoleLogin();
	m_role_module_manager->GetShiTu()->OnRoleLogin();
	m_role_module_manager->GetOfflineExp()->OnRoleLogin(last_save_time);
	m_role_module_manager->GetFaBao()->OnRoleLogin();
	m_role_module_manager->GetEscort()->OnRoleLogin(last_save_time);
	m_role_module_manager->GetMapUnlock()->OnRoleLogin();
	m_role_module_manager->GetResearchTask()->OnResearchTaskRoleLogin();
	m_role_module_manager->GetShop()->OnUserLogin();
	m_role_module_manager->GetRoleChallengeFb()->OnRoleLogin();
	m_role_module_manager->GetFuncGuide()->OnFuncGuideLogin();
	m_role_module_manager->GetTitle()->OnRoleLogin();
	m_role_module_manager->GetBuffManager()->OnLogin();
	m_role_module_manager->GetTaskManager()->SendPatrolTaskInfo();
	m_role_module_manager->GetLoginSystem()->OnRoleLogin();
	m_role_module_manager->GetMysteriousTrial()->OnRoleLogin();
	m_role_module_manager->GetTransactionTask()->OnRoleLogin();
	m_role_module_manager->GetCollection()->OnRoleLogin();
	m_role_module_manager->GetRoleSkill()->OnRoleLogin();
	m_role_module_manager->GetRoleActivityManager()->OnLogin();
	m_role_module_manager->GetTaskChain()->OnLogin();
	m_role_module_manager->GetTaskManager()->OnRoleLogin();
	m_role_module_manager->GetFuncPreview()->OnLogin();
	m_role_module_manager->GetHuaShenManager()->OnRoleLogin();
	m_role_module_manager->GetCourageChallenge()->OnRoleLogin();
	m_role_module_manager->GetMiYi()->OnLogin();
	m_role_module_manager->GetCourseOfGrowth()->OnLogin();	//该接口发放数据中使用到了创角天数,需要放在OnLoginCalCreateDay后面,防止天数不准确
	m_role_module_manager->GetRoleGatherDream()->OnRoleLogin();
	m_role_module_manager->GetBountyTask()->OnRoleLogin();
	m_role_module_manager->GetRoleCrystalFb()->OnLogin(m_login_timestamp, (unsigned int)last_save_time);
	m_role_module_manager->GetAnecdoteTask()->OnLogin();
	m_role_module_manager->GetPartner()->OnRoleLogin();
	m_role_module_manager->GetTrain()->OnRoleLogin();
	m_role_module_manager->ClearMixedElementCrystal();
	m_role_module_manager->GetRealmBreak()->OnLogin(last_save_time);
	m_role_module_manager->GetLunHuiWangChuan()->OnRoleLogin();
	m_role_module_manager->GetMapExploration()->OnRoleLogin();
	m_role_module_manager->GetSalary()->CheckSalaryVersion();
	m_role_module_manager->GetKnapsack()->OnLogin();
	m_role_module_manager->GetRoleJoinActivityReward()->OnLogin();
	m_role_module_manager->GetRoleGuildInborn()->OnRoleLogin();
	m_role_module_manager->GetNetherWorldPurgatory()->OnRoleLogin();
	m_role_module_manager->GetRoleHeartSkill()->OnRoleLogin();
	m_role_module_manager->GetDuel()->OnRoleLogin();
	m_role_module_manager->GetYinYangEye()->OnRoleLogin();
	m_role_module_manager->OnRoleLogin();
	m_role_module_manager->GetEquipmentManager()->OnLogin();
	m_role_module_manager->GetEndlessTower()->OnRoleLogin();
	m_role_module_manager->GetPet()->CheckGodPrintAttr();//神印检查
	m_role_module_manager->GetFlyUpSystem()->SendSigleSkillInfo();
	m_role_module_manager->GetMiniGame()->OnLogin();
	m_role_module_manager->GetDayReward()->OnRoleLogin();
	m_role_module_manager->GetRoleNewLevelComplement()->OnRoleLogin();
	m_role_module_manager->GetRoleHundredGodBoss()->OnRoleLogin();

	WildBossManager::Instance().OnRoleLogin();
	ShanHaiBossManager::Instance().OnRoleLogin(this);
	FriendIntimacyManager::Instance().OnLogin(this);
	CourseOfGrowthManager::Instance().OnRoleLogin(this);

	this->OnLoginBroadCast();

	//上线请求世界答题下发
	WorldStatus::Instance().SendWorldAnswerInfo(this);

	if (m_role_status_manager.IsInBattleStatus())
	{
		BattleManagerLocal::GetInstance().CheckRoleReallyInBattle(this, __FUNCTION__);
	}

	if (!m_scene->GetSpecialLogic()->IsForceEncounterMineMonster())
	{
		this->SetEncounterMonsterFlag(false); // 上线默认不遭遇暗怪
	}
	else
	{
		this->RefreshEncounterInterval(); // 否则刷新时间
	}

	this->LoginOriginGameNoticeHidden();

	/****************************************************
	* 注意：全局的OnUserLogin都搬到了World::OnUserLogin *
	****************************************************/


}

void Role::OnLogout()
{
	EventHandler::Instance().OnRoleLogout(this);
	this->LogoutOriginGameNoticeHidden();

	{
		unsigned int & logout_timestamp = this->GetRoleModuleManager()->GetCommonData().last_logout_timestamp;
		logout_timestamp = (unsigned int)EngineAdapter::Instance().Time();
		unsigned int online_time_s = logout_timestamp - m_login_timestamp;
		this->GetRoleModuleManager()->GetCommonData().last_logout_timestamp_2 = logout_timestamp;

		ROLE_LOG_QUICK10(LOG_TYPE_LOGOUT, this, online_time_s, this->GetRoleModuleManager()->GetCapability()->GetRoleCapability(), NULL, NULL, this->GetRoleModuleManager()->GetMoney()->GetGold(), this->GetRoleModuleManager()->GetMoney()->GetCoinBind(), this->GetLevel(), this->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam().history_chongzhi_num, 0);
		gamelog::g_log_login.printf(LL_INFO, "[Role::OnRoleLogout][%d][%s]", m_uid, m_name);
		//gamelog::g_log_global_user_state.printf(LL_INFO, "[LoginServer::OnGlobalUserLogout ret:%d] user[%s, %d] ip[%u] is_micro_pc[%d].", 0, m_name, this->GetUID(), this->GetIP(), 1);
	}
}

bool Role::Save(bool is_logout /* = false */, int* target_scene_id /* = NULL */, Posi* target_pos /* = NULL */)
{
	static RoleInitParam role_param;
	static RoleOtherInitParam other_p;
	static ItemListParam item_list_param;
	static SkillListParam skill_list_param;
	static MailListParam maillist_param;
	static RoleFirstKillParamList first_kill_list;
	static RandActivityRoleParamList rand_activity_role_list;
	static RoleCommonSaveDataParam role_common_save_data;
	static RoleCourageChallengeLevelParamList role_courage_challenge_list;
	static GodPrintItemListParam god_print_item_list;
	static NewCourageChallengeLevelParamList new_courage_challenge_list;

	item_list_param.count = 0;
	maillist_param.count = 0;
	first_kill_list.count = 0;
	rand_activity_role_list.count = 0;
	role_common_save_data.count = 0;
	role_courage_challenge_list.count = 0;
	god_print_item_list.count = 0;
	new_courage_challenge_list.count = 0;

	this->GetRoleInitParam(&role_param);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		F_STRNCPY(role_param.role_name, m_role_module_manager->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
	}

	this->GetRoleModuleManager()->GetRoleModuleData(&role_param, &other_p, &item_list_param, &skill_list_param, &maillist_param, &first_kill_list, &rand_activity_role_list,
		&role_common_save_data, &role_courage_challenge_list, &god_print_item_list, &new_courage_challenge_list, is_logout);

	// 当target_scene_id和target_pos均不为空时，表示切场景保存
	if (NULL != target_scene_id && NULL != target_pos)
	{
		if (Scene::IsStaticScene(this->GetScene()->GetSceneType()))	// 只有普通场景才会保存到last_scene中
		{
			role_param.last_scene_id = role_param.scene_id;
			role_param.last_scene_x = role_param.scene_x;
			role_param.last_scene_y = role_param.scene_y;
		}

		role_param.scene_id = *target_scene_id;
		role_param.scene_x = target_pos->x;
		role_param.scene_y = target_pos->y;
	}

	unsigned int ROLE_SAVE_TIME = GLOBALCONFIG->GetKeyConfig_RoleSaveTime();
	m_next_save_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + ROLE_SAVE_TIME;

	UserID original_user_id = m_user_id;
	const DBSession* db_session = &InternalComm::Instance().GetDBHandler().GetRoleDBSession();

	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(m_user_id);
		if (NULL == cui)
		{
			return false;
		}

		UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
		const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
		if (NULL == db_handler)
		{
			return false;
		}

		db_session = &db_handler->GetRoleDBSession();
		original_user_id = cui->original_user_id;
	}

	RMIRoleSaveBackObjectImplRole *rsbo = new RMIRoleSaveBackObjectImplRole();
	rsbo->log_user_id = m_user_id;
	rsbo->is_logout = is_logout;
	rsbo->role_id = m_uid;

	RMIRoleClient rc;
	rc.__bind_session(db_session->session);
	bool ret = rc.RoleSaveAsyn(UidToInt(original_user_id), role_param, other_p,
		item_list_param,
		skill_list_param,
		maillist_param,
		first_kill_list,
		rand_activity_role_list,
		role_common_save_data,
		role_courage_challenge_list,
		god_print_item_list,
		new_courage_challenge_list,
		rsbo, is_logout);
	if (ret)
	{
		m_role_module_manager->GetKnapsack()->ClearDirtyMark();
		m_role_module_manager->GetPet()->ClearDirtyMark();
		m_role_module_manager->GetPet()->ClearGodPrintBagDirtyMark();
		m_role_module_manager->GetRoleSkill()->ClearDirtyMark();
		m_role_module_manager->GetEquipmentManager()->ClearDirtyMark();
		m_role_module_manager->GetEquipmentManager()->ClearJewelryDirty();
		m_role_module_manager->GetRoleMail()->ClearDirtyMark();
		m_role_module_manager->GetHuaShenManager()->ClearDirtyMark();
		m_role_module_manager->GetHuaShenManager()->ClearPetDirtyMark();
		m_role_module_manager->GetMedal()->ClearDirtyMark();
		m_role_module_manager->GetHuaShenManager()->ClearMedalDirtyMark();
		m_role_module_manager->GetRoleFirstKill()->ClearDirtyMark();
		m_role_module_manager->GetRoleActivityManager()->ClearDirtyMark();
		m_role_module_manager->GetRoleCommonSaveManager()->ClearDirtyMark(role_common_save_data);
		m_role_module_manager->GetCourageChallenge()->ClearDirtyMark();
		m_role_module_manager->GetAccessoriesmentManager()->ClearDirtyMark();
		m_role_module_manager->GetZaoHuaEquipManager()->ClearDirtyMark();
		m_role_module_manager->GetZaoHuaEquipManager()->ClearWearDirtyMark();
		m_role_module_manager->GetHolyEquipManager()->ClearHolyDirtyMark();
		m_role_module_manager->GetHolyEquipManager()->ClearFuShiDirtyMark();
		m_role_module_manager->GetFuYuManager()->ClearDirtyMark();
		m_role_module_manager->GetNewCourageChallenge()->ClearDirtyMark();
		m_role_module_manager->GetRoleHuanShou()->ClearBagDirtyMark();
	}
	else if (is_logout && db_session->IsNeedReconnect())
	{
		DBDisconnectRoleSaveCache::Instance().CacheSaveData(UidToInt(original_user_id), role_param, other_p,
			item_list_param,
			skill_list_param,
			maillist_param,
			first_kill_list,
			rand_activity_role_list,
			role_common_save_data,
			role_courage_challenge_list,
			god_print_item_list,
			new_courage_challenge_list);
	}

	this->UpdateCacheNode();
	return true;
}

bool Role::IsRoleSaveDBSessionDisconnect()
{
	UserID original_user_id = m_user_id;
	const DBSession* db_session = &InternalComm::Instance().GetDBHandler().GetRoleDBSession();

	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(m_user_id);
		if (NULL == cui)
		{
			return false;
		}

		UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
		const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
		if (NULL == db_handler)
		{
			return false;
		}

		db_session = &db_handler->GetRoleDBSession();
		original_user_id = cui->original_user_id;
	}

	if (NULL == db_session)
	{
		return false;
	}

	return db_session->IsNeedReconnect();
}

void RMIRoleSaveBackObjectImplRole::RoleSaveRet(int ret)
{
	if (0 != ret)
	{
		Error(log_user_id, GAME_ERROR_EXECUTE, ret);
	}
	else
	{
		if (is_logout)
		{
			CrossUserRegister::Instance().OnUserLogout(role_id);
		}
	}
}

void Role::GetRoleInitParam(RoleInitParam *param)
{
	if (NULL == param) return;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	int delta_time = static_cast<unsigned int>(now > m_last_online_stati_time ? now - m_last_online_stati_time : 0);
	m_online_time += delta_time;
	m_last_online_stati_time = now;
	this->TodayOnlineStatiTime(now);

	param->last_reset_module_timestamp = m_last_reset_module_data_timestamp;
	param->forbid_talk_time = m_forbid_talk_time;
	param->forbid_talk_type = m_forbid_talk_type;
	param->role_id = this->GetUID();
	F_STRNCPY(param->role_name, this->GetName(), sizeof(param->role_name));
	param->avatar_type = this->GetAvatarType();
	param->headshot_id = this->GetHeadshotID();
	param->color = this->GetColor();
	param->authority_type = this->GetAuthorityType();
	param->vip_level = 1; //TODO vip 模块
	param->profession = this->GetProfession();
	param->level = this->GetLevel();
	param->top_level = this->GetTopLevel();
	param->next_top_level_refresh_timestamp = this->GetNextTopLevelRefreshTimestamp();
	param->role_fly_flag = this->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();

	param->scene_id = this->GetScene()->GetSceneID();
	param->scene_x = this->GetPos().x;
	param->scene_y = this->GetPos().y;
	param->last_scene_id = m_last_scene_id;
	param->last_scene_x = m_last_scene_pos.x;
	param->last_scene_y = m_last_scene_pos.y;

	param->plat_spid = m_plat_spid;

	param->online_time = m_online_time;
	param->today_online_time = m_today_online_time;
	param->lastday_online_time = m_lastday_online_time;
	param->create_time = m_create_time;
	param->last_online_dayid = m_last_online_dayid;
	param->capability = m_role_module_manager->GetCapability()->GetRoleCapability();
	param->exp = this->GetExp();
	param->max_exp = this->GetMaxExp();
	param->weapon_id = this->GetRoleModuleManager()->GetEquipmentManager()->GetCurWeaponId();

	EventHandler::Instance().OnTimeInterval(this, delta_time);
}

void Role::TodayOnlineStatiTime(unsigned int now)
{
	int today_delta_time = static_cast<unsigned int>(now > m_last_today_online_stati_time ? now - m_last_today_online_stati_time : 0);
	m_today_online_time += today_delta_time;
	m_last_today_online_stati_time = now;
}

void Role::OnEnterScene()
{
	//!< \note 把RegisterRoleLocal调整到Role::OnEnterScene下是因为G6的OnAOIEnter需要用到World的GetRole函数，
	//!<       如果不先Register好，就会使得GetRole获取到NULL

	// 执行完RegisterRoleLocal之后SceneManager的m_net_to_rolelocal和m_uid_to_rolelocal才响应地有了映射。
	// 依赖这2个映射的函数如：SceneManager::GetRole才能正确找到Role对象
	// 才能保证在m_special_logic->OnRoleEnterScene中一定能get到role
	World::GetInstWorld()->GetSceneManager()->RegisterRoleLocal(this->GetNetId(), m_scene->GetIndex(), this->GetId());

	int scene_id = m_scene->GetSceneID();

	if (!m_scene->GetMap()->Validate(Obj::OBJ_TYPE_ROLE, m_posi.x, m_posi.y))
	{
		Posi pre_pos = m_posi;
		Posi realive_pos(0, 0); m_scene->GetTownPoint(&realive_pos);
		if (0 != realive_pos.x && 0 != realive_pos.y)
		{
			this->SetPos(Posi(realive_pos.x, realive_pos.y));
		}
	}

	m_birth_pos = m_posi;
	m_last_check_pos = m_posi;

	// 发送进入场景消息通知
	Protocol::SCEnterScene es;
	es.sceneid = scene_id;
	es.obj_id = m_obj_id;
	es.is_first = m_role_module_manager->GetMapUnlock()->IsRoleHaveBeenEntered(scene_id) ? 0 : 1;
	es.pos_x = (short)m_posi.x;
	es.pos_y = (short)m_posi.y;
	es.scene_key = m_scene->GetSceneKey();
	EngineAdapter::Instance().NetSend(m_netid, (const char*)&es, sizeof(Protocol::SCEnterScene));

	Character::OnEnterScene();

	{

		// 创建AOI
		int CLIENT_SCREEN_X = (int)GLOBALCONFIG->GetKeyConfig_RoleAoiX();			// 真实屏幕50 该数字用于计算Role的AOI进入范围
		int CLIENT_SCREEN_Y = (int)GLOBALCONFIG->GetKeyConfig_RoleAoiY();			// 真实屏幕30


		if (m_scene->GetSceneType() == Scene::SCENE_TYPE_TU_ZI_KUAI_PAO)
		{
			CLIENT_SCREEN_X = 2500;
			CLIENT_SCREEN_Y = 2500;
		}


		int ROLE_AOI_LEAVE_MORE_THEN_ENTER = (int)GLOBALCONFIG->GetKeyConfig_RoleAoiLeaveMoreThenEnter();	// Role的AOI离开大于进入范围的值
		int screen_x = CLIENT_SCREEN_X;
		int screen_y = CLIENT_SCREEN_Y;

		m_aoi_handle = m_scene->GetZoneMatrix()->CreateRectAOI(m_obj_id, m_posi, Posi(screen_x / 2, screen_y / 2),
			Posi(screen_x / 2 + ROLE_AOI_LEAVE_MORE_THEN_ENTER, screen_y / 2 + ROLE_AOI_LEAVE_MORE_THEN_ENTER));
	}

	if (m_scene->GetSpecialLogic()->IsForceEncounterMineMonster())
	{
		this->RefreshEncounterInterval();
	}

	// 玩家个人模块在ModuleManager里调用
	m_role_module_manager->OnEnterScene(scene_id);

	EventHandler::Instance().OnRoleEnterScene(this);
}

void Role::OnLeaveScene()
{
	//TODO this->ResetRoleStatus();


	// 因角色logout函数不一定能调用到 （譬如 切换场景 不成功 角色不会存在于任何gameworld上）
	{
		//TODO 一些全局系统的OnLeaveScene
	}

	// 玩家个人模块在ModuleManager里调用
	m_role_module_manager->OnLeaveScene(m_scene->GetSceneID());

	m_scene->GetZoneMatrix()->DestroyAOI(m_aoi_handle);

	Character::OnLeaveScene();

	EventHandler::Instance().OnRoleLeaveScene(this);

}

typedef void (Role::*ObjEnterFun)(Obj *obj);
const ObjEnterFun g_obj_enter_fun[] = {
	0,
	&Role::OnAOIEnterRole,
	&Role::OnAOIEnterNPCMonster,
	&Role::OnAOIEnterNPCGathers,
	&Role::OnAOIEnterSpecialTransport,
	&Role::OnAOIEnterTreasureChest,
	&Role::OnAOIEnterNPCFuncNpc,
	&Role::OnAOIEnterRobot,
	&Role::OnAOIEnterNPCChosed,
	&Role::OnAOIEnterXunYouCar,
	&Role::OnAOIEnterPeakTokenObj,
	&Role::OnAOIEnterTuZiKuaiPaoTuZi,
};

UNSTD_STATIC_CHECK(sizeof(g_obj_enter_fun) / sizeof(g_obj_enter_fun[0]) == Obj::OBJ_TYPE_COUNT);


void Role::OnAOIEnter(ObjID obj_id)
{
	if (NULL == m_scene || obj_id == m_obj_id) return;

	Obj *obj = (Obj *)m_scene->GetObj(obj_id);
	if (NULL == obj) return;

	ObjEnterFun enter_fun = g_obj_enter_fun[int(obj->GetObjType())];
	if (NULL == enter_fun) return;

	(this->*enter_fun)(obj);
}

void Role::OnAOILeave(ObjID obj_id)
{
	if (obj_id == m_obj_id) return;

	Protocol::SCVisibleObjLeave voj;
	voj.obj_id = obj_id;
	voj.reserved = 0;
	EngineAdapter::Instance().NetSend(m_netid, (const char *)&voj, sizeof(Protocol::SCVisibleObjLeave));

	Obj *obj = (Obj *)m_scene->GetObj(obj_id);
	if (NULL == obj) return;

	if (OBJ_TYPE_ROLE == obj->GetObjType())
	{
		Role* leave_role = (Role*)obj;
		m_visible_role_map.erase(leave_role->GetUID());
	}
}

// 其他角色进入我的AOI范围，应把其他角色的信息发给我
void Role::OnAOIEnterRole(Obj *obj)
{
	Role *role = (Role*)obj;
	static Protocol::SCVisibleObjEnterRole esr;
	esr.role_id = role->GetUID();
	F_STRNCPY(esr.role_name, role->GetName(), sizeof(esr.role_name));

	esr.obj_id = role->GetId();
	esr.level = role->GetLevel();
	esr.top_level = role->GetTopLevel();
	esr.kill_value = role->GetRoleModuleManager()->GetDuel()->GetKillValue();

	esr.behavior_state = role->GetRoleStatusManager()->GetRoleStatus();
	esr.appearance = role->GetRoleModuleManager()->GetAppearance();
	esr.move_state = (char)role->GetRoleStatusManager()->GetRoleMoveStatus();

	esr.move_speed = role->GetSpeedRate();
	esr.pos_x = role->GetPos().x;
	esr.pos_y = role->GetPos().y;
	esr.distance = role->GetDirDistance();
	esr.dir = role->GetDir();
	esr.scene_id = role->GetScene()->GetSceneID();

	esr.guild_post = -1;	// 初始化-1, 后面如果有家族再赋值
	esr.sh_reserve = 0;

	bool is_teammate = false;
	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		if (team->GetLeaderUID() == role->GetUID())
		{
			esr.is_leader = 1;
		}
		else
		{
			esr.is_leader = 0;
		}
		esr.is_in_team = 1;

		Team* myteam = this->GetMyTeam();
		if (NULL != myteam && NULL != team && myteam->GetTeamIndex() == team->GetTeamIndex())
		{
			is_teammate = true;
		}

		esr.team_id = team->GetTeamIndex();
		esr.team_is_red = team->TeamIsRedName() ? 1 : 0;
	}
	else
	{
		esr.is_leader = 0;
		esr.is_in_team = 0;
		esr.team_id = -1;
	}

	int number_of_screens = this->GetRoleModuleManager()->GetRoleSetting()->GetNumberOfScreens();
	if (!is_teammate)
	{
		// 不是队友，则要判断同屏人数与当前在视野范围内的人数
		if ((int)m_visible_role_map.size() >= number_of_screens)
		{
			return;
		}

		if (!LOGIC_CONFIG->GetSceneExtraConfig()->IsShowOtherRole(m_scene->GetSceneID())) return;
	}

	// AttachData 用于那些不是所有人都有的数据字段（比如家族、镖车等）
	memset(esr.attach_data, 0, sizeof(esr.attach_data));
	char *attach_offset_ptr = esr.attach_data;
	esr.attach_mask = 0;
	unsigned int attach_len = 0;
	{
		// 镖车
		if (role->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
		{
			int escort_id = role->GetRoleModuleManager()->GetEscort()->GetEscortSeq();
			*(int*)attach_offset_ptr = escort_id; attach_offset_ptr += sizeof(int);
			esr.attach_mask |= (1 << Protocol::SCVisibleObjEnterRole::ATTACH_TYPE_ESCORT);
		}

		// 家族
		Guild *guild = GuildManager::Instance().GetGuild(role->GetGuildID());
		if (NULL != guild)
		{
			F_STRNCPY(attach_offset_ptr, guild->GetGuildNamePtr(), sizeof(GuildName)); attach_offset_ptr += sizeof(GuildName);
			F_STRNCPY(attach_offset_ptr, guild->GetBanner(), sizeof(GuildBanner)); attach_offset_ptr += sizeof(GuildBanner);
			esr.attach_mask |= (1 << Protocol::SCVisibleObjEnterRole::ATTACH_TYPE_GUILD);

			GuildMember * member = guild->GetMemberManager()->GetMember(role->GetUserId());
			if (NULL != member)
			{
				esr.guild_post = (char)member->GetPost();
			}
		}

		// 称号
		int title_id = role->GetRoleModuleManager()->GetTitle()->GetWearTitle();
		if (title_id > 0)
		{
			*(int*)attach_offset_ptr = title_id; attach_offset_ptr += sizeof(int);
			esr.attach_mask |= (1 << Protocol::SCVisibleObjEnterRole::ATTACH_TYPE_TITLE);
		}

		// 颜值系统头衔资源ID
		int facescore_id = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_TITLENAME);
		if (-1 != facescore_id)
		{
			*(int*)attach_offset_ptr = facescore_id; attach_offset_ptr += sizeof(int);
			esr.attach_mask |= (1 << Protocol::SCVisibleObjEnterRole::ATTACH_TYPE_FACESCORE);
		}

		// 跟随宠物
		int follow_pet_index = role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[NEW_FOLLOW_INDEX];
		const ItemGridData* pet_grid_data = role->GetRoleModuleManager()->GetPet()->GetPetGridData(follow_pet_index);
		if (NULL != pet_grid_data && NULL != pet_grid_data->param && 0 != pet_grid_data->param->pet_param.pet_id)
		{
			const PetParam* pet_param = &pet_grid_data->param->pet_param;
			int pet_rank = RankManager::Instance().GetPetTopRankShadow()
				->GetPetTopRank(role->GetUID(), pet_param->pet_id, pet_grid_data->item_unique_id);

			*(int*)attach_offset_ptr = pet_param->pet_id; attach_offset_ptr += sizeof(int);
			F_STRNCPY(attach_offset_ptr, pet_param->name, sizeof(GameName)); attach_offset_ptr += sizeof(GameName);
			*(int *)attach_offset_ptr = pet_param->skin_id; attach_offset_ptr += sizeof(int);
			*(int *)attach_offset_ptr = pet_param->fei_sheng_times; attach_offset_ptr += sizeof(int);
			*(int *)attach_offset_ptr = pet_rank; attach_offset_ptr += sizeof(int);
			esr.attach_mask |= (1 << Protocol::SCVisibleObjEnterRole::ATTACH_TYPE_FOLLOW_PET);
		}

		//结义组名字
		JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role->GetUID());
		if (NULL != jie_yi_zu)
		{
			JieYIZuName jie_yi_name;
			jie_yi_name.Reset();
			JieYIZuMemberName member_name = { 0 };
			unsigned int add_offset = sizeof(jie_yi_name) + sizeof(member_name);
			if (attach_offset_ptr + add_offset - esr.attach_data < Protocol::SCVisibleObjEnterRole::ATTACH_DATA_LENGTH)
			{
				jie_yi_zu->GetName(&jie_yi_name, &member_name, role->GetUID());
				*(JieYIZuName *)attach_offset_ptr = jie_yi_name;
				attach_offset_ptr += sizeof(jie_yi_name);
				F_STRNCPY(attach_offset_ptr, member_name, sizeof(member_name));
				attach_offset_ptr += sizeof(member_name);
				esr.attach_mask |= (1 << Protocol::SCVisibleObjEnterRole::ATTACH_TYPE_JIE_YI_ZU_NAME);
			}
		}

		attach_len = (unsigned int)(attach_offset_ptr - esr.attach_data);
	}

	int send_len = sizeof(esr) - Protocol::SCVisibleObjEnterRole::ATTACH_DATA_LENGTH + attach_len;
	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&esr, send_len);

	if (NULL != team)
	{
		team->SendUpdateFollowListMsg(role, this);
	}

	// 特殊逻辑的玩家闯入视野
	this->GetScene()->GetSpecialLogic()->OnAOIEnterRole(this, role);

	m_visible_role_map.insert(std::map<int, int>::value_type(role->GetUID(), 1));
}

void Role::OnAOIEnterNPCMonster(Obj* obj)
{
	NPCMonster* monster = (NPCMonster*)obj;

	static Protocol::SCVisibleObjEnterNPCMonster msg;
	msg.npc_id = monster->GetNpcId();
	msg.client_show_id = monster->GetHeadMonsterId();
	msg.monster_group_id = monster->GetMonsterGroupId();
	msg.battle_mode = monster->GetBattleMode();
	msg.npc_seq = monster->GetNpcSeq();

	msg.obj_id = monster->GetId();
	msg.reserve_sh = 0;

	msg.speed = 0;
	msg.pos_x = monster->GetPos().x;
	msg.pos_y = monster->GetPos().y;
	msg.distance = monster->GetDirDistance();
	msg.dir = monster->GetDir();
	msg.scene_id = monster->GetScene()->GetSceneID();

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&msg, sizeof(msg));
}

void Role::OnAOIEnterNPCGathers(Obj * obj)
{
	NPCGather * gather = (NPCGather *)obj;
	if (!gather->CanSee(this))return;
	static Protocol::SCVisibleObjEnterNPCGathers voeng;
	voeng.npc_id = gather->GetNpcId();
	voeng.npc_seq = gather->GetNpcSeq();
	voeng.posi_x = gather->GetPos().x;
	voeng.posi_y = gather->GetPos().y;
	voeng.scene_id = gather->GetScene()->GetSceneID();
	voeng.obj_id = gather->GetId();
	voeng.gather_times = 0;

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&voeng, sizeof(voeng));
}

void Role::OnAOIEnterSpecialTransport(Obj * obj)
{
	SpecialTransport * transport_obj = (SpecialTransport *)obj;

	static Protocol::SCVisibleObjEnterSpecialTransport voeng;

	voeng.posi_x = transport_obj->GetPos().x;
	voeng.posi_y = transport_obj->GetPos().y;
	voeng.scene_id = transport_obj->GetScene()->GetSceneID();
	voeng.obj_id = transport_obj->GetId();
	voeng.param1 = transport_obj->GetExtraInfo().send_param1;
	voeng.param2 = transport_obj->GetExtraInfo().send_param2;

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&voeng, sizeof(voeng));
}

void Role::OnAOIEnterTreasureChest(Obj * obj)
{
	TreasureChest * treasure_chest = (TreasureChest *)obj;

	static Protocol::SCVisibleObjEnterTreasureChest voeng;

	voeng.posi_x = treasure_chest->GetPos().x;
	voeng.posi_y = treasure_chest->GetPos().y;
	voeng.scene_id = treasure_chest->GetScene()->GetSceneID();
	voeng.obj_id = treasure_chest->GetId();
	voeng.is_open = treasure_chest->IsOpen();
	voeng.treasure_chest_id = treasure_chest->GetTreasureChestid();

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&voeng, sizeof(voeng));
}

void Role::OnAOIEnterNPCFuncNpc(Obj * obj)
{
	NPCOther * npc = (NPCOther *)obj;

	static Protocol::SCVisibleObjEnterNPCFuncNpc voenfn;

	voenfn.scene_id = npc->GetScene()->GetSceneID();
	voenfn.obj_id = npc->GetId();
	voenfn.fun_type = Protocol::SCVisibleObjEnterNPCFuncNpc::FUN_NPC_TYPE_NORMAL;
	voenfn.posi_x = npc->GetPos().x;
	voenfn.posi_y = npc->GetPos().y;
	voenfn.npc_id = npc->GetNpcId();

	NPCExtraInfo &  info = npc->GetNpcExtraInfo();
	voenfn.info_type = info.info_type;
	voenfn.param1 = info.param1;
	voenfn.param2 = info.param2;
	voenfn.param3 = info.param3;
	voenfn.dir = npc->GetDir();

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&voenfn, sizeof(voenfn));
}

void Role::OnAOIEnterNPCChosed(Obj * obj)
{
	NPCChosed * choser = (NPCChosed *)obj;

	static Protocol::SCVisibleObjEnterNPCFuncNpc voenfn;
	voenfn.scene_id = choser->GetScene()->GetSceneID();
	voenfn.obj_id = choser->GetId();
	voenfn.fun_type = Protocol::SCVisibleObjEnterNPCFuncNpc::FUN_NPC_TYPE_CHOSED;
	voenfn.posi_x = choser->GetPos().x;
	voenfn.posi_y = choser->GetPos().y;
	voenfn.npc_id = choser->GetNpcId();

	voenfn.info_type = 0;
	voenfn.param1 = 0;
	voenfn.param2 = 0;
	voenfn.param3 = 0;
	voenfn.dir = choser->GetDir();

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&voenfn, sizeof(voenfn));
}

void Role::OnAOIEnterRobot(Obj* obj)
{
	Robot* robot = (Robot*)obj;

	bool can_see = false;
	Team* team = robot->GetTeam();
	Team* myteam = this->GetMyTeam();
	if (NULL != team && NULL != myteam && team->GetTeamIndex() == myteam->GetTeamIndex())
	{
		can_see = true; // 机器人是队友，可见
	}
	else if (NULL != team)
	{
		int leader_uid = team->GetLeaderUID();
		if (m_visible_role_map.find(leader_uid) != m_visible_role_map.end())
		{
			can_see = true; // 机器人的队长可见，则机器人可见
		}
	}
	else
	{
		// 机器人没有队伍，如果我还有位置，则可以见，没有则不可见
		int number_of_screens = this->GetRoleModuleManager()->GetRoleSetting()->GetNumberOfScreens();
		if ((int)m_visible_role_map.size() < number_of_screens)
		{
			can_see = true;
		}

		if (!LOGIC_CONFIG->GetSceneExtraConfig()->IsShowOtherRole(m_scene->GetSceneID())) return;
	}

	if (!can_see)
	{
		return;
	}

	static Protocol::SCVisibleObjEnterRole msg;
	msg.role_id = robot->GetRobotID();
	F_STRNCPY(msg.role_name, robot->GetName(), sizeof(GameName));
	msg.obj_id = robot->GetId();
	msg.level = robot->GetLevel();
	msg.top_level = 0;
	msg.kill_value = 0;

	msg.appearance = robot->GetAppearance();

	msg.move_speed = robot->GetSpeedRate();
	msg.pos_x = robot->GetPos().x;
	msg.pos_y = robot->GetPos().y;
	msg.dir = robot->GetDir();
	msg.distance = robot->GetDirDistance();
	msg.scene_id = robot->GetScene()->GetSceneID();

	msg.behavior_state = robot->GetBehaviorState();
	msg.is_leader = 0;
	msg.is_in_team = (NULL != robot->GetTeam() ? 1 : 0);
	msg.move_state = 0;
	msg.team_id = -1;
	msg.team_is_red = 0;
	msg.guild_post = -1;
	msg.sh_reserve = 0;

	msg.attach_mask = 0;
	char *attach_offset_ptr = msg.attach_data;
	{//附加信息
		// 称号
		int title_id = robot->GetWearTitle();
		if (title_id > 0)
		{
			*(int*)attach_offset_ptr = title_id;
			attach_offset_ptr += sizeof(int);
			msg.attach_mask |= (1 << Protocol::SCVisibleObjEnterRole::ATTACH_TYPE_TITLE);
		}
	}
	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&msg, sizeof(msg));

	if (NULL != team)
	{
		team->SendUpdateFollowListMsg(robot, this);
	}
}

void Role::OnAOIEnterXunYouCar(Obj * obj)
{
	XunYouCar * car = (XunYouCar *)obj;

	static Protocol::SCVisibleObjEnterXunYouCar msg;
	msg.obj_id = car->GetId();
	msg.is_stop = car->IsStop();
	msg.car_type = car->GetCarType();

	msg.speed = car->GetSpeedRate();
	msg.pos_x = car->GetPos().x;
	msg.pos_y = car->GetPos().y;
	msg.dir = car->GetDir();
	msg.distance = car->GetDirDistance();
	msg.scene_id = car->GetScene()->GetSceneID();

	//情缘信息
	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(car->GetQingYuanID());
	if (NULL != p_qing_yuan)
	{
		std::vector<int> host_vec = p_qing_yuan->GetMemberList();
		for (int i = 0; i < host_vec.size(); i++)
		{
			//todo 测试
			msg.host_info_[i].uid = host_vec[i];

			UserCacheNode *node = UserCacheManager::Instance().GetUserNode(msg.host_info_[i].uid);
			if (NULL == node)
			{
				continue;
			}
			node->GetName(msg.host_info_[i].role_name);
			msg.host_info_[i].avatar_type = node->avatar_type;

			msg.host_info_[i].obj_id = INVALID_OBJ_ID;

			{
				Role *host_role = car->GetScene()->GetRoleByUID(msg.host_info_[i].uid);
				if (NULL != host_role)
				{
					msg.host_info_[i].obj_id = host_role->GetId();
				}
			}

			msg.host_info_[i].hole_sh = 0;
		}
	}

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&msg, sizeof(msg));
}

void Role::OnAOIEnterPeakTokenObj(Obj * obj)
{
	PeakTokenObj * peak_token_obj = (PeakTokenObj *)obj;

	static Protocol::SCVisibleObjEnterPeakTokenObj msg;
	msg.obj_id = peak_token_obj->GetId();
	msg.reserved = 0;

	msg.speed = 0;
	msg.pos_x = peak_token_obj->GetPos().x;
	msg.pos_y = peak_token_obj->GetPos().y;
	msg.dir = peak_token_obj->GetDir();
	msg.scene_id = peak_token_obj->GetScene()->GetSceneID();


	msg.creat_uid = peak_token_obj->GetCreatUid();
	peak_token_obj->GetCreatName(msg.creat_name);
	msg.creat_appearance = peak_token_obj->GetCreatAppearance();

	msg.act_id = peak_token_obj->GetActID();
	msg.act_begin_time = (unsigned int)peak_token_obj->GetCreatTime();
	msg.act_end_time = (unsigned int)peak_token_obj->GetRemoveTime();

	msg.role_fetch_time = peak_token_obj->GetRoleFetchTime(this);

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&msg, sizeof(msg));
}

void Role::OnAOIEnterTuZiKuaiPaoTuZi(Obj * obj)
{
	TuZiKuaiPaoTuZi * tu_zi_obj = (TuZiKuaiPaoTuZi *)obj;

	static Protocol::SCVisibleObjEnterTuZiKuaiPaoTuzi msg;
	msg.obj_id = tu_zi_obj->GetId();
	msg.reserved = 0;

	msg.speed = tu_zi_obj->GetSpeed();
	msg.pos_x = tu_zi_obj->GetPos().x;
	msg.pos_y = tu_zi_obj->GetPos().y;
	msg.distance = tu_zi_obj->GetDirDistance();
	msg.dir = tu_zi_obj->GetDir();
	msg.scene_id = tu_zi_obj->GetScene()->GetSceneID();
	msg.tu_zi_type = tu_zi_obj->GetTuZiType();
	msg.role_id = tu_zi_obj->GetCreateRoleId();
	msg.time = (unsigned int)EngineAdapter::Instance().Time();
	msg.track_type = tu_zi_obj->GetTrackType();
	msg.track_point_index = tu_zi_obj->GetTrackPointGroupIndex();

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&msg, sizeof(msg));
}

int Role::GetRoleBaseAva()
{
	if (this->GetHeadshotID() >= 1000)
	{
		return this->GetHeadshotID();
	}
	else
	{
		return this->GetAvatarType();
	}
}

int Role::SetProfession(int new_profession, const char* reason)
{
	int old_prof = m_profession;
	m_profession = new_profession;

	gamelog::g_log_role_data.printf(LL_INFO, "Role::SetProfession role[%d,%s] old_prof[%d] new_prof[%d] reason[%s]",
		this->GetUID(), this->GetName(), old_prof, new_profession, reason);

	static Protocol::SCRoleProfessionChangeNotice notice;
	notice.new_profession = m_profession;

	m_role_module_manager->ReCalcAttr(AttributeList::RECALC_REASON_INVALID, __FUNCTION__);

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char*)&notice, sizeof(notice));
	EventHandler::Instance().OnRoleProfProEvent(this, old_prof, new_profession);

	this->UpdateCacheNode();

	return 0;
}

bool Role::SetAuthorityType(char type, bool save_now)
{
	if (type < AUTHORITY_TYPE_INVALID || type >= AUTHORITY_TYPE_MAX)
	{
		return false;
	}

	char old_auth = m_authority_type;

	m_authority_type = type;

	if (save_now) this->Save();

	if (AUTHORITY_TYPE_GUIDER != old_auth && AUTHORITY_TYPE_GUIDER == m_authority_type)
	{
		this->NoticeNum(noticenum::NT_NEWHAND_GUILD_SUCC);
	}

	{
		//Protocol::SCRoleAuthority ra;
		//ra.obj_id = m_obj_id;
		//ra.authority_type = m_authority_type;
		//EngineAdapter::Instance().NetSend(m_netid, (const char*)&ra, sizeof(ra));
	}

	//TODO this->OnRoleBaseInfoChange();
	this->SendRoleAuthorityInfo();
	ROLE_LOG_QUICK6(LOG_TYPE_CHANGE_AUTHORITY_TYPE, this, old_auth, m_authority_type, NULL, NULL);

	return true;
}

long long Role::AddExp(long long add_exp, short add_reason /* = ADD_EXP_TYPE_NORMAL */, const char* reason_str /* = NULL */, bool is_extra /* = true */)
{
	int role_fly_limit_level = this->GetRoleModuleManager()->GetFlyUpSystem()->IsAddExpFlyUpFlag();
	bool role_fly_limit_level_flag = m_level >= role_fly_limit_level;

	long long origin_add_exp = add_exp;

	if (ADD_EXP_TYPE_KILL_NORMAL_MONSTER == add_reason)
	{
		if (m_role_module_manager->GetKnapsack()->UseExpCrystal())
		{
			add_exp += static_cast<long long>(add_exp * 0.01 * LOGIC_CONFIG->GetCurrencyconfig()->GetExpCrystalCfg()->bonus_exp_num_percentage);
		}

		// 经验buff加成
		add_exp += static_cast<long long>(add_exp * 0.01 * m_role_module_manager->GetBuffManager()->GetExpUpPer());

		if (Scene::SCENE_TYPE_COMMON_SCENE == this->GetScene()->GetSceneType())
		{
			add_exp = static_cast<long long>(add_exp * 1.5 * LOGIC_CONFIG->GetRoleExpConfig()->GetOtherCfg().exp_coefficient);
		}

		{
			add_exp -= this->GetGuaJiExpAttenuation(add_exp);	// 计算挂机经验衰减
			this->GetRoleModuleManager()->GetCommonData().today_add_guaji_exp += add_exp;
			this->GetRoleModuleManager()->SendGuaJiExpInfo();
		}
	}
	//世界等级经验加成
	int world_level = WorldStatus::Instance().GetWorldLevel();
	if (is_extra && !role_fly_limit_level_flag)
	{
		if (m_level >= LOGIC_CONFIG->GetRoleExpConfig()->GetOtherCfg().world_level_effect &&
			World::GetInstWorld()->GetCurRealOpenDay(this->GetUniqueServerID()) >= LOGIC_CONFIG->GetRoleExpConfig()->GetOtherCfg().world_level_open_day)
		{
			if (0 != world_level && m_level < MAX_ROLE_LEVEL)	//满级不再有衰减
			{
				add_exp = (add_exp * LOGIC_CONFIG->GetRoleExpConfig()->GetWorldLevelUp(m_level, world_level)) / BASE_EXP_PERCENTAGE_NUM;
			}
		}
	}

	EventHandler::Instance().OnAddExp(this, add_exp, add_reason);

	const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_level);
	if (NULL == cfg) return 0;

	bool is_notice = false;
	int try_count = 0;
	long long add_exp_count = add_exp;
	bool is_force = ADD_EXP_TYPE_ROLE_FLY_UP_FINISH == add_reason;
	while ((is_force || add_exp > 0) && try_count++ < 50)
	{
		if (m_level >= MAX_ROLE_LEVEL)	//巅峰等级
		{
			if (m_top_level <= 0 || !this->GetRoleModuleManager()->IsOpenTopLevelNotice())	//第一种是玩家首次达到120级,第二种玩家经过重置(重置1级)
			{
				m_exp += add_exp;
				add_exp = 0;
				break;
			}
			const RoleTopLevelCfg * top_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(m_top_level);
			if (NULL == top_cfg)
			{
				break;
			}
			if (m_exp + add_exp >= top_cfg->up_exp)
			{
				add_exp -= (top_cfg->up_exp - m_exp);
				m_exp = top_cfg->up_exp;
				if (!this->RoleChangeLevel(m_top_level + 1, false))
				{
					if (add_exp > 0)
					{
						is_notice = true;
					}
					break;
				}

				const RoleTopLevelCfg * top_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(m_top_level);
				if (NULL == top_cfg)
				{
					break;
				}
			}
			else
			{
				m_exp += add_exp;
				add_exp = 0;
			}
		}
		else
		{					
			int old_level = m_level;
			role_fly_limit_level_flag = old_level >= role_fly_limit_level;
			bool world_level_limit_flag = (0 == world_level || old_level < LOGIC_CONFIG->GetRoleExpConfig()->GetOtherCfg().world_level_overflow + world_level);
			if (m_exp + add_exp >= cfg->exp && !role_fly_limit_level_flag && world_level_limit_flag)
			{
				add_exp -= (cfg->exp - m_exp);
				m_exp = cfg->exp;

				if (!this->RoleChangeLevel(old_level + 1))
				{
					if (add_exp > 0)
					{
						is_notice = true;
					}
					break;
				}

				cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(old_level + 1);
				if (NULL == cfg)              
				{
					continue;
				}
			}
			else
			{
				m_exp += add_exp;
				add_exp = 0;
			}
		}
	}

	Protocol::SCRoleExpChange exp_change_notice;
	exp_change_notice.change_exp = add_exp_count;
	exp_change_notice.cur_exp = m_exp;
	exp_change_notice.change_reason = add_reason;
	exp_change_notice.reserve_sh = 0;

	EngineAdapter::Instance().NetSend(m_netid, (const char*)&exp_change_notice, sizeof(exp_change_notice));
	if (is_notice)
	{
		this->NoticeNum(errornum::EN_ROLE_ADD_EXP_EXCEED_LEVEL_LIMIT);
	}

	if (NULL != m_scene)
	{
		m_scene->GetSpecialLogic()->OnAddExp(this, static_cast<int>(add_exp), add_reason);
	}
	gamelog::g_log_role_exp.printf(LL_INFO, "AddExp role[%d,%s] cur_exp[%lld] m_level[%d] add_exp[%lld] add_reason[%d] reason_str[%s]",
		this->GetUID(), this->GetName(), m_exp, m_level, add_exp_count, add_reason, reason_str);

	ROLE_LOG_QUICK10(LOG_TYPE_ROLE_EXP, this, add_exp_count, m_exp, reason_str, NULL, m_level, origin_add_exp, m_top_level, 0, 0);

	return add_exp_count;
}

long long Role::ReduceExp(long long reduce_exp, short reduce_reason, const char * reason_str)
{
	const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_level);
	if (NULL == cfg) return 0;

	if (0 >= reduce_exp) return 0;

	long long real_reduce_exp = reduce_exp;
	if (reduce_reason == REDUCE_EXP_TYPE_DUEL)
	{
		if (real_reduce_exp > m_exp)
		{
			real_reduce_exp = m_exp;
		}
	}

	m_exp -= real_reduce_exp;
	if (0 > m_exp)				//暂没有掉等级逻辑
	{
		m_exp = 0;
	}

	this->SendRoleInfo();
	gamelog::g_log_role_exp.printf(LL_INFO, "ReduceExp role[%d,%s] cur_exp[%lld] m_level[%d] reduce_exp[%lld] real_reduce_exp[%lld] add_reason[%d] reason_str[%s]",
		this->GetUID(), this->GetName(), m_exp, m_level, reduce_exp, real_reduce_exp, reduce_reason, reason_str);

	return real_reduce_exp;
}

void Role::OnRoleResetTopLevelAndCheck()
{
	int old_top_level = m_top_level;
	m_top_level = 1;
	const RoleTopLevelCfg * top_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(m_top_level);
	if (NULL == top_cfg) return;

	int try_count = 0;
	while (m_exp > 0 && try_count++ < MAX_ROLE_TOP_LEVEL_NUM)
	{
		if (m_exp >= top_cfg->up_exp)
		{
			m_exp -= top_cfg->up_exp;
			m_top_level += 1;
			const RoleTopLevelCfg * top_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(m_top_level);
			if (NULL == top_cfg)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (old_top_level != m_top_level || 1 != m_top_level)
	{
		Protocol::SCRoleLevelChange msg;
		msg.change_type = Protocol::SCRoleLevelChange::ROLE_LEVEL_CHANGE_TYPE_TOP;
		msg.level = m_top_level;
		msg.exp = m_exp;

		EngineAdapter::Instance().NetSend(m_netid, (const char*)&msg, sizeof(msg));

		EventHandler::Instance().OnRoleUpTopLevelEvent(this, 0, m_top_level);
		gamelog::g_log_role_exp.printf(LL_INFO, "%s role[%d,%s] cur_exp[%lld] m_level[%d] old_top_level[%d] m_top_Level[%d] next_refresh_timestamp[%u]", __FUNCTION__, this->GetUID(),
			this->GetName(), m_exp, m_level, old_top_level, m_top_level, m_next_top_level_refresh_timestamp);
	}

	ROLE_LOG_QUICK10(LOG_TYPE_ROLE_EXP, this, 0, m_exp, __FUNCTION__, NULL, m_level, 0, m_top_level, m_next_top_level_refresh_timestamp, 0);
}

void Role::GmSetRoleLevel(int to_level)
{
	if (to_level <= 0 || to_level > MAX_ROLE_LEVEL) return;

	this->RoleChangeLevel(to_level);
}

void Role::GmSetRoleTopLevel(int top_level)
{
	if (m_level < MAX_ROLE_LEVEL || top_level <= 0 || top_level > MAX_ROLE_TOP_LEVEL_NUM) return;

	this->RoleChangeLevel(top_level, false);
}

void Role::GmRetrieveExp(long long retrieve_exp)
{
	if (retrieve_exp <= 0)
	{
		gamelog::g_log_role_exp.printf(LL_ERROR, "Role::GmRetrieveExp role[%d,%s] Param Error, retrieve_exp[%lld]",
			this->GetUID(), this->GetName(), retrieve_exp);
		return;
	}

	long long origin_exp = m_exp;
	if (m_exp < MIN_INT64 + retrieve_exp)
	{
		m_exp = MIN_INT64;
	}
	else
	{
		m_exp -= retrieve_exp;
	}

	Protocol::SCRoleExpChange exp_change_notice;
	exp_change_notice.change_exp = -retrieve_exp;
	exp_change_notice.cur_exp = m_exp;
	exp_change_notice.change_reason = ADD_EXP_TYPE_EXP_RETRIEVE;
	exp_change_notice.reserve_sh = 0;

	EngineAdapter::Instance().NetSend(m_netid, (const char*)&exp_change_notice, sizeof(exp_change_notice));

	gamelog::g_log_role_exp.printf(LL_INFO, "Role::GmRetrieveExp role[%d,%s] origin_exp[%lld] retrieve_exp[%lld] cur_exp[%lld]",
		this->GetUID(), this->GetName(), origin_exp, retrieve_exp, m_exp);

	// 发邮件通知	
	std::stringstream ss_exp;
	ss_exp << retrieve_exp;

	static MailContentParam contentparam; contentparam.Reset();
	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_retrieve_mail_subject);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_retrieve_mail_content_exp, ss_exp.str().c_str());
	if (length1 > 0 && length2 > 0)
	{
		MailRoute::MailToUser(this->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}
}

void Role::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	if (NULL == m_scene) return;

	Character::Update(interval, now_second, now_dayid);

	int role_status = m_role_status_manager.GetRoleStatus();
	switch (role_status)
	{
	case RoleStatusManager::ROLE_STATUS_GATHER:
		this->UpdateGather(now_second);
		break;
	default:
		break;
	}

	if (m_pos_data.m_x > 0)
	{
		this->Move(m_pos_data.m_x, m_pos_data.m_y, m_pos_data.m_dir, m_pos_data.m_distance, 0);
		m_pos_data.m_x = 0;
	}

	unsigned int now = static_cast<unsigned int>(now_second);
	if (now >= m_role_module_manager->GetCommonData().next_month_timestamp)
	{
		m_role_module_manager->GetCommonData().next_month_timestamp = now + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
		this->OnMonthChange();
	}
	if (now >= m_role_module_manager->GetCommonData().next_week_timestamp)
	{
		m_role_module_manager->GetCommonData().next_week_timestamp = now + EngineAdapter::Instance().NextWeekInterval(1, 0, 0, 0);
		this->OnWeekChange();
		this->OnTwoWeekChange(WorldStatus::Instance().GetCommonData().week_refresh_flag);
	}

	if (m_last_online_dayid != now_dayid) // 所有的OnDayChange函数只能在这里调用
	{
		// OnDayChange放最下面 否则在周一 OnWeekChange 和 OnDayChange 都要下发协议时要发两次
		this->OnDayChange(m_last_online_dayid, now_dayid);

		m_last_online_dayid = now_dayid;
	}

	if (now >= m_role_module_manager->GetCommonData().next_month_6_OClock)
	{
		m_role_module_manager->GetCommonData().next_month_6_OClock = now + EngineAdapter::Instance().NextMouthInterval(1, 6, 0, 0);
		this->OnResetModuleDataMonth();
	}
	if (now >= m_role_module_manager->GetCommonData().next_week_6_OClock)
	{
		m_role_module_manager->GetCommonData().next_week_6_OClock = now + EngineAdapter::Instance().NextWeekInterval(1, 6, 0, 0);
		this->OnResetModuleDataWeek();
	}

	{
		// 重置角色数据相关逻辑
		if (m_next_reset_module_data_timestamp > 0 && now_second >= m_next_reset_module_data_timestamp)
		{
			int interval_second = (int)(now_second - m_last_reset_module_data_timestamp);
			int interval_day = 0 == m_last_reset_module_data_timestamp ? 1 : (interval_second / SECOND_PER_DAY) + 1;

			this->OnResetModuleData(now_dayid - interval_day, now_dayid);

			m_last_reset_module_data_timestamp = static_cast<unsigned int>(now_second);

			this->RecalNextResetDataTimeStamp();
		}
	}

	if (m_next_save_time < now_second)
	{
		static const unsigned int hearbeat_dead_time = ServerConfigPool::Instance().common_config.timeout.gameworld_heartbeat_dead_time_ms;
		unsigned long now = EngineAdapter::Instance().GetGameTime();
		if (now - m_last_msg_from_client_time > hearbeat_dead_time)
		{
			m_next_save_time += 20;	// 暂时先这么着了 等实际发现问题了 再改好
			gamelog::g_log_debug.printf(LL_WARNING, "[Role::Update Warring][User(%d, %d) Save NOT Active more then hearbeat_dead_time:%d]",
				m_user_id.db_index, m_user_id.role_id, (int)hearbeat_dead_time);
		}
		else
		{
			this->Save();
		}
	}

	//static const int TEST_UID = 84956060; //83927485
	//if (interval == 343 && this->GetUID() == TEST_UID)
	//{
	//	const Posi &pos = this->GetPos();
	//	char dst_str[128];
	//	int length = 0;
	//	length = SNPRINTF(dst_str, sizeof(dst_str), "Pos[%d, %d]", pos.x, pos.y);
	//	//length = SNPRINTF(dst_str, sizeof(dst_str), "[%s %d]: Pos = (%d, %d), scene_id = (%d), scene_type = (%d), map[%d,%d]=[%c,%d]", 
	//	//	this->GetName(), this->GetUID(), pos.x, pos.y, m_scene->GetSceneID(), m_scene->GetSceneType(), 
	//	//	pos.x, pos.y, m_scene->GetMap()->GetMapGrid(pos.x, pos.y), m_scene->GetMap()->GetMapBoundryInfo(pos.x, pos.y));
	//	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(this->GetUserId(), dst_str, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
	//}

	m_role_module_manager->Update(interval, now_second, now_dayid);

	// 运镖中遭遇强盗
	if (0 != m_role_module_manager->GetEscort()->GetEscortSeq() &&
		m_role_module_manager->GetCommonData().encounter_escort_monster_ms > 0 &&
		!this->IsStatic())
	{
		m_role_module_manager->GetCommonData().encounter_escort_monster_ms -= interval;
		if (m_role_module_manager->GetCommonData().encounter_escort_monster_ms <= 0)
		{
			m_role_module_manager->GetCommonData().encounter_escort_monster_ms = 0;
			BattleManagerLocal::GetInstance().StartEsortMonsterFightReq(this);
			int rand_ms = RandomNum(20000, 40000);
			m_role_module_manager->GetCommonData().encounter_escort_monster_ms = rand_ms;
		}
	}

	// 遭遇暗怪条件：
	// 1.开启遭遇暗怪   2.场景有暗怪   3.正在移动   4.时间到0
	else if (m_scene->HasMineMonster()
		&& (1 == m_role_module_manager->GetCommonData().encounter_monster_flag
			|| m_scene->GetSpecialLogic()->IsForceEncounterMineMonster()
			|| m_role_module_manager->IsPatrolling()
			)
		)
	{
		if (m_role_module_manager->GetCommonData().encounter_monster_interval_ms > 0
			&& !this->IsStatic()
			&& !m_role_status_manager.IsInBattleStatus()
			&& m_scene->GetSpecialLogic()->CanEncounterMineMonster(this))
		{
			m_role_module_manager->GetCommonData().encounter_monster_interval_ms -= interval;
			if (m_role_module_manager->GetCommonData().encounter_monster_interval_ms <= 0)
			{
				m_role_module_manager->GetCommonData().encounter_monster_interval_ms = 0;
				if (!BattleManagerLocal::GetInstance().StartMineMonsterFightReq(this, true)) // 如果进入战斗失败，则重置遇怪时间再重试
				{
					this->RefreshEncounterInterval();
				}
			}
		}

	}
}

void Role::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	this->SendServerTime();
	this->SendServerTimeDate();
	m_lastday_online_time = m_today_online_time;
	m_today_online_time = 0;
	JieYiZuManager::Instance().OnRoleDayChange(this);
	m_role_module_manager->OnDayChange(old_dayid, now_dayid);
}

void Role::OnWeekChange()
{
	m_role_module_manager->OnWeekChange();
}

void Role::OnMonthChange()
{
	m_role_module_manager->OnMonthChange();
}

void Role::OnTwoWeekChange(int week_falg)
{
	if (week_falg == 0)//0以为第一周刚刚刷新，1为现在是第二周
	{
		m_role_module_manager->OnTwoWeekChange(week_falg);
	}
}

void Role::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	m_role_module_manager->OnResetModuleData(old_dayid, now_dayid);
}

void Role::OnResetModuleDataWeek()
{
	m_role_module_manager->OnResetModuleDataWeek();
}

void Role::OnResetModuleDataMonth()
{
	m_role_module_manager->OnResetModuleDataMonth();
}

void Role::RecalNextResetDataTimeStamp()
{
	time_t last_reset_module_data_timestamp = static_cast<time_t>(m_last_reset_module_data_timestamp);

#ifdef _WIN32
	const tm *last_reset_tm = ::localtime(&last_reset_module_data_timestamp);
#else	
	struct tm local_time_item;
	struct tm *last_reset_tm = &local_time_item;
	::localtime_r(&last_reset_module_data_timestamp, &local_time_item);
#endif

	if (NULL != last_reset_tm)
	{
		time_t zero_timestamp = GetZeroTime(last_reset_module_data_timestamp);
		if (last_reset_tm->tm_hour >= ROLE_RESET_MODULE_DATA_TM_HOUR)
		{
			m_next_reset_module_data_timestamp = static_cast<unsigned int>(zero_timestamp + SECOND_PER_DAY + ROLE_RESET_MODULE_DATA_TM_HOUR * SECOND_PER_HOUR);
		}
		else
		{
			m_next_reset_module_data_timestamp = static_cast<unsigned int>(zero_timestamp + ROLE_RESET_MODULE_DATA_TM_HOUR * SECOND_PER_HOUR);
		}
	}
}

void Role::SendRoleInfo()
{
	static Protocol::SCRoleInfoAck ack;
	ack.role_id = m_uid;
	F_STRNCPY(ack.name, m_name, sizeof(ack.name));
	ack.level = m_level;
	ack.profession = m_profession;
	ack.top_level = m_top_level;
	ack.title_id = m_role_module_manager->GetTitle()->GetWearTitle();
	ack.facescroe_id = m_role_module_manager->GetFaceScore()->GetFaceEquipSeq(FACE_TITLENAME);
	ack.cur_exp = m_exp;
	memset(ack.guild_name, 0, sizeof(ack.guild_name));
	ack.move_speed = this->GetSpeedRate();
	ack.appearance = m_role_module_manager->GetAppearance();
	ack.create_time = (unsigned int)this->GetRoleCreateTime();
	ack.kill_value = m_role_module_manager->GetDuel()->GetKillValue();
	ack.authority_type = m_authority_type;

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char*)&ack, sizeof(ack));
}

void Role::SendRoleAuthorityInfo()
{
	Protocol::SCRoleAuthorityInfoChange info;
	info.authority_type = m_authority_type;
	info.reserve_ch = 0;
	info.reserve_sh = 0;

	EngineAdapter::Instance().NetSend(this->GetNetId(), &info, sizeof(info));
}

void Role::SendAllInfo()
{
	this->SendServerTime();
	this->SendServerTimeDate();

	this->SendRoleInfo();
	m_role_module_manager->SendAllRoleInfo();

	MailRoute::Instance().GetMailList(this);
	WorldStatus::Instance().SendCourageChallengeFirstKillInfo(this);
	WorldStatus::Instance().SendChiefElectionInfo(this);
	WorldStatus::Instance().SendChiefPeakBattleInfo(this);
	WorldStatus::Instance().SendWorldLevelInfo(this);

	this->SendBattlePosition();
	this->SendBattleAutoInfo();
}

bool Role::ResetPos(Axis x, Axis y, short reset_type /*= 0*/)
{
	Posi old_pos = m_posi;

	if (Move(x, y, Dir_S, 0, 0, false))
	{
		m_last_check_valid_time = 0;

		if (Protocol::SCResetPost::RESET_TYPE_JUMP != reset_type)
		{
#ifdef _DEBUG
			gamelog::g_log_world.printf(LL_MAINTANCE, "Role[%d,%s] reset post", this->GetUID(), this->GetName());
#endif

			Protocol::SCResetPost rp;
			rp.x = m_posi.x;
			rp.y = m_posi.y;
			rp.scene_id = m_scene->GetSceneID();
			return EngineAdapter::Instance().NetSend(m_netid, (const char *)&rp, sizeof(Protocol::SCResetPost));
		}
	}

	return false;
}

bool Role::Move(Axis _x, Axis _y, Dir _dir, Scalar distance, short height, bool check_valid /*= false*/)
{
	if (NULL == m_scene) return false;

	if (Character::Move(_x, _y, _dir, distance, height, check_valid))
	{
		m_scene->GetZoneMatrix()->MoveAOI(m_aoi_handle);
		return true;
	}

	return false;
}


void Role::NoticeNum(int notice_num)
{
	Protocol::SCNoticeNum en;
	en.notice_num = notice_num;
	EngineAdapter::Instance().NetSend(m_netid, (const char *)&en, sizeof(Protocol::SCNoticeNum));
}

void Role::NoticeItemLack(int item_id, int nun)
{
	/*
	Protocol::SCLackItem li;
	li.item_id = item_id;
	li.item_count = nun;
	EngineAdapter::Instance().NetSend(m_netid, (const char*)&li, sizeof(li));
	*/
}

bool Role::RoleChangeLevel(int to_level, bool is_role_level)
{
	int old_level = m_level;
	int old_top_level = m_top_level;
	if (is_role_level)
	{
		if (to_level <= 0 || to_level > MAX_ROLE_LEVEL) return false;

		m_level = to_level;
	}
	else
	{
		if (m_level < MAX_ROLE_LEVEL || to_level <= 0 || to_level > MAX_ROLE_TOP_LEVEL_NUM) return false;

		m_top_level = to_level;
	}
	m_exp = 0;

	Protocol::SCRoleLevelChange msg;
	msg.change_type = is_role_level ? Protocol::SCRoleLevelChange::ROLE_LEVEL_CHANGE_TYPE_NORMAL : Protocol::SCRoleLevelChange::ROLE_LEVEL_CHANGE_TYPE_TOP;
	msg.level = to_level;
	msg.exp = m_exp;

	EngineAdapter::Instance().NetSend(m_netid, (const char*)&msg, sizeof(msg));

	if (is_role_level)
	{
		ROLE_LOG_QUICK6(LOG_TYPE_UPGRADE_LEVEL, this, m_level, m_role_module_manager->GetCapability()->GetRoleCapability(), NULL, NULL);
		EventHandler::Instance().OnRoleUpLevelEvent(this, old_level, m_level);
	}
	else
	{
		Protocol::SCRoleTopLevelAOINotice aoi_notice;
		aoi_notice.obj_id = this->GetId();
		aoi_notice.reserve_sh = 0;
		aoi_notice.top_level = this->GetTopLevel();
		this->GetScene()->GetZoneMatrix()->NotifyAreaMsg(this->GetObserHandle(), &aoi_notice, sizeof(aoi_notice));

		EventHandler::Instance().OnRoleUpTopLevelEvent(this, old_top_level, m_top_level);
	}
	return true;
}

Guild * Role::GetGuild()
{
	Guild * guild = GuildManager::Instance().GetGuild(m_role_module_manager->GetCommonData().m_guild_id);
	return guild;
}

GuildID Role::GetGuildID()
{
	return m_role_module_manager->GetCommonData().m_guild_id;
}

void Role::SetGuildID(GuildID id, bool is_join_guild /* = false */)
{
	int old_guild_id = m_role_module_manager->GetCommonData().m_guild_id;
	if (m_role_module_manager->GetCommonData().m_guild_id == id) return;
	m_role_module_manager->GetCommonData().m_guild_id = id;
	if (0 != id && is_join_guild)
	{
		EventHandler::Instance().OnJoinGuild(this, id);
	}

	this->GetRoleModuleManager()->GetTitle()->CalGuildFightTitle();
	this->GetRoleModuleManager()->GetTitle()->CalGuildHonorTitle2();

	gamelog::g_log_guild.printf(LL_INFO, "Role::SetGuildID role[%d,%s] old_guild_id[%d] new_guild_id[%d]\n",
		m_uid, m_name, old_guild_id, m_role_module_manager->GetCommonData().m_guild_id);
}

int Role::GetGuildTotalGongXian()
{
	Guild * guild = GuildManager::Instance().GetGuild(m_role_module_manager->GetCommonData().m_guild_id);
	if (NULL == guild) return 0;

	GuildMember * member = guild->GetMemberManager()->GetMember(m_user_id);
	if (NULL == member) return 0;

	return member->GetTotalGongXian();
}

void Role::SetGuildPostion(int positon)
{
	if (m_role_module_manager->GetCommonData().m_guild_positon == positon)return;

	m_role_module_manager->GetCommonData().m_guild_positon = positon;
	m_role_module_manager->GetGuildCommonData().position_mark.SetBit(positon);

	EventHandler::Instance().OnSetGuildPost(this, positon);
}

int Role::GetGuildPostion()
{
	return m_role_module_manager->GetCommonData().m_guild_positon;
}

bool Role::InTeam()
{
	Team* team = TeamManager::Instance().GetTeam(this);
	return NULL != team;
}

bool Role::IsLeader()
{
	Team * team = this->GetMyTeam();
	return (NULL != team && this->GetUID() == team->GetLeaderUID());
}

bool Role::IsFollow()
{
	Team * team = this->GetMyTeam();
	if (NULL == team) return false;		// 如果没有队伍，视为不跟随；

	TeamMember * mem_info = team->GetMemberInfo(this);
	return NULL != mem_info && mem_info->is_follow;
}

bool Role::IsMember(int uid)
{
	Team* me_team = TeamManager::Instance().GetTeam(this);
	if (NULL == me_team) return false;
	return me_team->IsMember(uid);
}

Team * Role::GetMyTeam(bool is_auto_create)
{
	Team * team = TeamManager::Instance().GetTeamByUid(m_uid);
	if (NULL != team)
	{
#ifdef _DEBUG
		assert(team == m_team);
#endif
	}

	if (is_auto_create && team == NULL)
	{
		team = TeamManager::Instance().CreateTeam(this);
	}


	return team;
}

bool Role::CanJoinTeam(bool is_notice)
{
	if (0 != m_role_module_manager->GetEscort()->GetEscortSeq())
	{
		if (is_notice) this->NoticeNum(errornum::EN_ESCORT_NOT_CAN_JOIN_TEAM);
		return false;
	}

	return true;
}

bool Role::CanRoleStartFight(int battle_mode)
{
	return m_role_status_manager.CanRoleStartFight(battle_mode);
}

int Role::GetTeamMember(int * uid_list, int max_count, bool oneself, bool follow, bool online)
{
	Team* team = TeamManager::Instance().GetTeam(this);
	if (NULL == team) return 0;
	int member_count = 0;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember *member = team->GetMemberInfo(i);
		if (NULL == member) continue;
		if (!oneself && member->uid == m_uid) continue;
		if (follow && !member->is_follow) continue;
		if (online && !member->is_online) continue;

		uid_list[member_count++] = member->uid;
	}
	return member_count;
}

int Role::GetTeamCount()
{
	Team * my_team = this->GetMyTeam();
	if (NULL == my_team)
	{
		return 1;
	}

	return my_team->GetMemberCount();
}

//组队挂机场景暗怪经验提升
static const double ROLE_TEAM_ADD_EXP_PERCENT_BY_MINE_MONSTER_LIST[MAX_TEAM_MEMBER_NUM] = {
	0, 0.1, 0.2, 0.3, 0.4
};

long long Role::GetTeamExpUpCountByMineMonster(long long total_exp)
{
	if (total_exp <= 0)
	{
		return 0;
	}
	int team_add_exp_index = this->GetTeamCount() - 1;
	if (team_add_exp_index < 0 || team_add_exp_index >= ARRAY_ITEM_COUNT(ROLE_TEAM_ADD_EXP_PERCENT_BY_MINE_MONSTER_LIST))
	{
		return 0;
	}

	return (long long)(total_exp * ROLE_TEAM_ADD_EXP_PERCENT_BY_MINE_MONSTER_LIST[team_add_exp_index]);
}

long long Role::GetGuaJiExpAttenuation(long long total_exp)
{
	const std::pair<double, int>* guaji_exp_cfg = LOGIC_CONFIG->GetSundryConfig()->GetMaxGuaJiExpCfg(this->GetLevel());
	if (NULL == guaji_exp_cfg) return 0;

	double today_add_guaji_exp = this->GetRoleModuleManager()->GetCommonData().today_add_guaji_exp;

	int today_fatigue = (today_add_guaji_exp / guaji_exp_cfg->first) * guaji_exp_cfg->second;

	const std::pair<int, int>* fatigue_cfg = LOGIC_CONFIG->GetSundryConfig()->GetFatigueCfg(today_fatigue);
	if (NULL == fatigue_cfg) return 0;

	if (fatigue_cfg->second == 0) return 0;
	return total_exp * fatigue_cfg->second / 100;
}

bool Role::CanEnterFB(bool is_notice)
{
	if (!m_scene->IsInStaticScene() && (m_scene->GetSceneType() != Scene::SCENE_TYPE_GATHER_FB))//特殊处理，采集副本允许直接跳到其他副本
	{
		if (is_notice) this->NoticeNum(errornum::EN_YOU_ARE_IN_FB_NOW);
		return false;
	}

	if (0 != m_role_module_manager->GetEscort()->GetEscortSeq())
	{
		if (is_notice) this->NoticeNum(errornum::EN_ESCORT_NOT_CAN_ENTER_FB);
		return false;
	}

	return true;
}

bool Role::RoleChangeCareer(int to_profession, int avatar_type, int color)
{
	if (!this->GetScene()->GetSpecialLogic()->CanChangeProf(this))
	{
		return false;
	}

	if (!FunOpen::IsFunOpen(this, FUN_TYPE_PROF_ADVANCE, true))
	{
		return false;
	}

	if (m_profession == to_profession && m_avatar_type == avatar_type && m_color == color)//无变化返回
	{
		return false;
	}

	if (to_profession % PROFESSION_BASE != m_profession % PROFESSION_BASE) // 保证转职是平级转
	{
		to_profession = to_profession / PROFESSION_BASE * PROFESSION_BASE + m_profession % PROFESSION_BASE;
	}

	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(to_profession, m_role_module_manager->GetAdvanceCareer()->GetSubLevel());
	if (NULL == cfg)
	{
		this->NoticeNum(errornum::EN_CARRER_ADVANCE_UNKNOWN_PROF);
		return false;
	}

	int change_career_times = this->GetRoleModuleManager()->GetCommonData().change_career_times;
	const ProfOtherCfg& other_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetOtherCfg();
	int cost = other_cfg.change_career_cost_min;
	for (int i = 0; i < change_career_times; ++i)
	{
		int tmp_cost = static_cast<int>(0.01 * other_cfg.change_cost_ratio_per_time * cost);
		if (tmp_cost > other_cfg.change_career_cost_max) break;

		cost = tmp_cost;
	}

	if (cost != 0 && !this->GetRoleModuleManager()->GetMoney()->UseAllCoin(cost, __FUNCTION__))
	{
		this->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}

	const ConstantCfg &c_cfg = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg();
	if (!m_role_module_manager->GetKnapsack()->HasItem(c_cfg.change_appearance_item, 1))
	{
		m_role_module_manager->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return false;
	}

	bool change_app = false;
	if (m_avatar_type != avatar_type)
	{
		if (!AppearanceConfig::Instance().CheckAvatarAndColor(avatar_type, color, to_profession))
		{
			this->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		m_avatar_type = avatar_type;
		m_headshot_id = 0;

		change_app = true;
	}

	if (m_color != color)
	{
		if (!AppearanceConfig::Instance().CheckAvatarAndColor(avatar_type, color, to_profession))
		{
			this->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		m_color = color;

		change_app = true;
	}

	if (!m_role_module_manager->GetKnapsack()->ConsumeItem(c_cfg.change_appearance_item, 1, __FUNCTION__, 0, true))
	{
		return false;
	}

	if (change_app)
	{
		m_role_module_manager->MakeAppearanceData();
		this->SendRoleInfo();
	}

	EventHandler::Instance().OnRoleChangeAppearance(this, change_app);
	ChatManager::Instance().OnChangeProf(this, m_profession, to_profession);
	this->GetRoleModuleManager()->GetCommonData().change_career_times += 1;
	this->SetProfession(cfg->profession_id, __FUNCTION__);
	this->SendProfExtraInfo();

	return true;
}

void Role::SendProfExtraInfo()
{
	static Protocol::SCProfessionExtraInfo info;
	info.change_prof_times = this->GetRoleModuleManager()->GetCommonData().change_career_times;

	EngineAdapter::Instance().NetSend(m_netid, (const char*)&info, sizeof(info));
}

void Role::UpdateCacheNode()
{
	static UserCacheNode node;
	node.level = m_level;
	node.avatar_type = m_avatar_type;
	node.uid = m_uid;
	node.profession = m_profession;
	node.last_online_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	node.avatar_timestamp = m_avatar_timestamp;
	node.appearance = m_role_module_manager->GetAppearance();
	node.capability = m_role_module_manager->GetCapability()->GetRoleCapability();
	this->GetName(node.role_name);
	node.top_level = m_top_level;
	node.next_top_level_refresh_timestamp = m_next_top_level_refresh_timestamp;

	UserCacheManager::Instance().UpdateCacheNode(node);
}

void Role::RoleSetBattlePosition(char position)
{
	m_role_module_manager->GetCommonData().battle_position = position;

	this->SendBattlePosition();
}

char Role::GetBattlePosition() const
{
	return m_role_module_manager->GetCommonData().battle_position;
}

void Role::SendBattlePosition()
{
	static Protocol::SCSetBattlePosition msg;
	msg.position = m_role_module_manager->GetCommonData().battle_position;

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char*)&msg, sizeof(msg));
}

void Role::SetEncounterMonsterFlag(bool is_encounter)
{
	m_role_module_manager->GetCommonData().last_encounter_monster_flag = m_role_module_manager->GetCommonData().encounter_monster_flag;
	m_role_module_manager->GetCommonData().last_encounter_monster_ms = m_role_module_manager->GetCommonData().encounter_monster_interval_ms;
	m_role_module_manager->GetCommonData().encounter_monster_flag = is_encounter ? 1 : 0;
	if (is_encounter)
	{
		this->RefreshEncounterInterval();
	}

	static Protocol::SCEncounterMineMonsterFlag msg;
	msg.flag = (is_encounter ? 1 : 0);
	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char*)&msg, sizeof(msg));
}

void Role::RefreshEncounterInterval()
{
	// 若没有暗怪，则不刷新
	if (!m_scene->HasMineMonster())
	{
		m_role_module_manager->GetCommonData().encounter_monster_interval_ms = 0;
		return;
	}

	// 1 : 场景强制遇暗怪 2: : 设置暗怪标记且设置巡逻  刷新   
	if (m_scene->GetSpecialLogic()->IsForceEncounterMineMonster() ||
		(m_role_module_manager->GetCommonData().encounter_monster_flag > 0 && m_role_module_manager->IsPatrolling()))
	{
		SpecialLogic * sp_logic = m_scene->GetSpecialLogic();
		m_role_module_manager->GetCommonData().encounter_monster_interval_ms =
			(NULL != sp_logic) ? sp_logic->GetMineMonsterEncounterInterval(this) : m_scene->RandMineMonsterEncounterTimeMS();

		return;
	}

	m_role_module_manager->GetCommonData().encounter_monster_interval_ms = 0;
	return;
}

void Role::RestoreLastEncounterMonsterFlag()
{
	m_role_module_manager->GetCommonData().encounter_monster_flag = m_role_module_manager->GetCommonData().last_encounter_monster_flag;
	m_role_module_manager->GetCommonData().encounter_monster_interval_ms = m_role_module_manager->GetCommonData().last_encounter_monster_ms;

	static Protocol::SCEncounterMineMonsterFlag msg;
	msg.flag = m_role_module_manager->GetCommonData().encounter_monster_flag;
	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char*)&msg, sizeof(msg));
}

void Role::SetPatrolFlag(bool is_patrol)
{
	m_role_module_manager->SetPatrolFlag(is_patrol);
	if (is_patrol)
	{
		this->RefreshEncounterInterval();
	}
}

void Role::SetBattleAuto(int is_auto)
{
	this->GetRoleModuleManager()->GetCommonData().battle_is_auto = (is_auto ? 1 : 0);

	//this->SendBattleAutoInfo(); 本地游戏服玩家不需要同步自己是否自动
}

void Role::SetBattleAutoMove(bool is_first_move, short move_type, UInt16 skill_id, short skill_level,
	bool is_check, bool is_notice, bool is_update_pet_auto_move /* = false */)
{
	if (BATTLE_MOVE_TYPE_PERFORM_SKILL == move_type || BATTLE_MOVE_TYPE_AI_SKILL == move_type)
	{
		if (skill_level <= 0) return;

		if (is_first_move)
		{
			RoleSkill::SkillItem* si = this->GetRoleModuleManager()->GetRoleSkill()->GetSkillBySkillId(skill_id);
			if (NULL == si || si->Invalid() || si->skill_show_level < skill_level)
			{
				return;
			}
		}
		else
		{
			int fight_pet_idx = this->GetRoleModuleManager()->GetPet()->GetFightIndex();
			if (-1 == fight_pet_idx)
			{
				RoleSkill::SkillItem* si = this->GetRoleModuleManager()->GetRoleSkill()->GetSkillBySkillId(skill_id);
				if (NULL == si || si->Invalid() || si->skill_show_level < skill_level)
				{
					return;
				}
			}
			else
			{
				const PetParam* param = this->GetRoleModuleManager()->GetPet()->GetPetParam(fight_pet_idx);
				if (NULL == param)
				{
					return;
				}

				bool has_skill = false;
				const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(param->pet_id);
				if (NULL != pet_cfg && pet_cfg->HasSkill(skill_id))
				{
					has_skill = true;
				}
				else
				{
					pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(param->lian_yao_pet_id);
					if (NULL != pet_cfg && pet_cfg->HasSkill(skill_id))
					{
						has_skill = true;
					}
				}
					
				if (!has_skill)
				{
					return;
				}
			}
		}
	}
	else if (BATTLE_MOVE_TYPE_NORMAL_ATTACK != move_type && BATTLE_MOVE_TYPE_NORMAL_DEFENCE != move_type)
	{
		return;
	}

	if (is_first_move)
	{
		this->GetRoleModuleManager()->GetCommonData().battle_auto_first_move_type = move_type;
		this->GetRoleModuleManager()->GetCommonData().battle_auto_first_skill_id = skill_id;
		this->GetRoleModuleManager()->GetCommonData().battle_auto_first_skill_level = skill_level;
	}
	else
	{
		this->GetRoleModuleManager()->GetCommonData().battle_auto_second_move_type = move_type;
		this->GetRoleModuleManager()->GetCommonData().battle_auto_second_skill_id = skill_id;
		this->GetRoleModuleManager()->GetCommonData().battle_auto_second_skill_level = skill_level;

		//if (is_update_pet_auto_move)
		//{
		//	// 若是宠物， 更新到宠物的默认技能中
		//	int fight_pet_idx = this->GetRoleModuleManager()->GetPet()->GetFightIndex();
		//	if (-1 != fight_pet_idx)
		//	{
		//		this->GetRoleModuleManager()->GetPet()->ModifyAutoMove(fight_pet_idx, move_type, skill_id, skill_level);
		//	}
		//}
	}

	if (is_notice)
	{
		this->SendBattleAutoInfo();
	}
}

void Role::SetBattleAutoAfterInitialFight()
{
	this->SetBattleAuto(1);

	RoleSkill::SkillItem* si = this->GetRoleModuleManager()->GetRoleSkill()->GetSkill(0);
	if (NULL != si)
	{
		this->SetBattleAutoMove(true, BATTLE_MOVE_TYPE_PERFORM_SKILL, si->skill_id, si->skill_level, true, true);
	}
}

void Role::SetBattleAutoOnInitialFightDisabled()
{
	RoleSkill::SkillItem* si = this->GetRoleModuleManager()->GetRoleSkill()->GetSkill(0);
	if (NULL != si)
	{
		this->SetBattleAutoMove(true, BATTLE_MOVE_TYPE_PERFORM_SKILL, si->skill_id, si->skill_level, true, true);
	}
}

void Role::SendBattleAutoInfo()
{
	static Protocol::SCBattleAutoInfo info;

	info.is_auto = this->GetRoleModuleManager()->GetCommonData().battle_is_auto;
	info.first_move_type = this->GetRoleModuleManager()->GetCommonData().battle_auto_first_move_type;
	info.first_skill_id = this->GetRoleModuleManager()->GetCommonData().battle_auto_first_skill_id;
	info.first_skill_level = this->GetRoleModuleManager()->GetCommonData().battle_auto_first_skill_level;
	info.second_move_type = this->GetRoleModuleManager()->GetCommonData().battle_auto_second_move_type;
	info.second_skill_id = this->GetRoleModuleManager()->GetCommonData().battle_auto_second_skill_id;
	info.second_skill_level = this->GetRoleModuleManager()->GetCommonData().battle_auto_second_skill_level;

	EngineAdapter::Instance().NetSend(m_netid, (const char*)&info, sizeof(info));
}

void Role::OnRoleSkillUplevel(UInt16 skill_id, short skill_level)
{
	bool has_change = false;
	CommonDataParam& param = this->GetRoleModuleManager()->GetCommonData();
	if (param.battle_auto_first_move_type == BATTLE_MOVE_TYPE_PERFORM_SKILL &&
		param.battle_auto_first_skill_id == skill_id)
	{
		param.battle_auto_first_skill_level = skill_level;
		has_change = true;
	}

	if (-1 == m_role_module_manager->GetPet()->GetFightIndex() &&
		param.battle_auto_second_move_type == BATTLE_MOVE_TYPE_PERFORM_SKILL &&
		param.battle_auto_second_skill_id == skill_id)
	{
		param.battle_auto_second_skill_level = skill_level;
		has_change = true;
	}

	if (has_change)
	{
		this->SendBattleAutoInfo();
	}
}

void Role::OnFightPetSkillLevelChange(UInt16 skill_id, short skill_level)
{
	bool has_change = false;
	CommonDataParam& param = this->GetRoleModuleManager()->GetCommonData();
	if (param.battle_auto_second_move_type == BATTLE_MOVE_TYPE_PERFORM_SKILL &&
		param.battle_auto_second_skill_id == skill_id)
	{
		if (skill_level > 0)
		{
			param.battle_auto_second_skill_level = skill_level;
		}
		else
		{
			param.battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
			param.battle_auto_second_skill_id = 0;
			param.battle_auto_second_skill_level = 0;
		}

		has_change = true;
	}

	if (has_change)
	{
		this->SendBattleAutoInfo();
	}
}

void Role::OnResetNameReq(GameName new_name)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return; // 跨服不允许改名
	}

	const ConstantCfg & cfg = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg();
	if (!m_role_module_manager->GetKnapsack()->HasItem(cfg.name_change_item, cfg.name_change_num))
	{
		this->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	new_name[sizeof(GameName) - 1] = 0;

	InvalidCharFilte(new_name);
	RoleNameCatPostfix(new_name, m_user_id.db_index);

	RMIQueryNameExistBackObjectImpl *impl = new RMIQueryNameExistBackObjectImpl();
	impl->user_id = m_user_id;
	F_STRNCPY(impl->role_name, new_name, sizeof(GameName));

	RMILoginClient rc;
	rc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
	rc.QueryNameExistAsyn((const char*)new_name, impl);

	ROLE_LOG_QUICK6(LOG_TYPE_RESET_NAME, m_role_module_manager->GetRole(), 0, 0, new_name, NULL);
}

bool Role::ResetName(GameName name)
{
	GameName old_name;
	this->GetName(old_name);

	F_STRNCPY(m_name, name, sizeof(GameName));

	EventHandler::Instance().OnRoleResetName(this, old_name, name);
	return true;
}

void Role::OnResetNameRetConsumeItem()
{
	const ConstantCfg & cfg = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg();
	if (!m_role_module_manager->GetKnapsack()->ConsumeItem(cfg.name_change_item, cfg.name_change_num, "Role::OnResetNameReq"))
	{
		this->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
}

void Role::ForbidTalk(time_t forbid_time, const char * reason, const char * last_msg, int forbid_talk_type)
{
	if (NULL == reason || NULL == last_msg) return;

	m_forbid_talk_time = EngineAdapter::Instance().Time() + forbid_time;
	m_forbid_talk_type = forbid_talk_type;

	gamelog::g_log_forbid.printf(LL_INFO, "[Role::ForbidTalk][%d][%s] plat_name:%s forbid_time:%u forbid_talk_type:%d reason:%s last_msg:%s",
		UidToInt(m_user_id), (const char*)m_name, (const char*)m_plat_name, (unsigned int)forbid_time, forbid_talk_type, (const char*)reason, (const char*)last_msg);

	ROLE_LOG_QUICK6(LOG_TYPE_FORBID_TALK, this, forbid_time, forbid_talk_type, reason, last_msg);
}

void Role::OnStartsGather(int seq_id, int is_auto_gather)
{
	if (!NpcManager::Instance().CanTouchNpc(this, seq_id)) return;

	if (!m_role_status_manager.CanGather(seq_id))
	{
		this->NoticeNum(errornum::EN_NPC_ROLE_IS_BUSY_CURRENT);
		return;
	}

	Posi position(0, 0); int tmp_gather_id = 0, tmp_gather_time = 0;
	if (seq_id < NpcManager::MIN_NPC_SEQ)		// 场景采集物
	{
		const NPCDynamicInformaCfg * npc_cfg = NPCPOOL->GetNPCInformaCfg(seq_id);
		if (NULL == npc_cfg) return;

		const ServerNPCInfoCfg * npc_info = NPCPOOL->GetServerNPCInfo(npc_cfg->npc_id);
		if (NULL == npc_info || SERVER_NPC_TYPE_GATHERS != npc_info->server_npc_type) return;

		position = npc_cfg->npc_posi;
		tmp_gather_id = npc_cfg->npc_id;
		tmp_gather_time = npc_info->server_npc_param.gather_param.gather_time;
	}
	else		// 服务器采集物
	{
		NPC * npc = NpcManager::Instance().GetServerNPC(seq_id);
		if (NULL == npc || OBJ_TYPE_NPC_GATHERS != npc->GetObjType()) return;

		NPCGather * gathers = (NPCGather *)npc;
		if (NULL == gathers) return;

		position = gathers->GetPos();
		tmp_gather_id = gathers->GetNpcId();
		tmp_gather_time = gathers->GetNeedTime();
	}

	this->ActionStop();

	tmp_gather_time = (int)(tmp_gather_time / (1.0 + this->GetGatherSpeedupPercent() * 1.0 / 100));	// 计算采集速率加成

	m_gather_seq_id = seq_id;
	m_gather_need_timestamp = tmp_gather_time > 0 ? (unsigned int)(EngineAdapter::Instance().Time() + tmp_gather_time) : 0;

	m_role_status_manager.OnRoleStartGather();

	if (NULL != m_scene)			// 通知周围玩家
	{
		static Protocol::SCNPCGathersNotify ngn;
		ngn.role_obj_id = this->GetId();
		ngn.gather_seq = m_gather_seq_id;
		ngn.npc_id = tmp_gather_id;
		m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&ngn, sizeof(ngn));
	}

	// 回发协议通知采集所需时间
	static Protocol::SCNPCStartToGather nstg;
	nstg.need_time = tmp_gather_time;
	EngineAdapter::Instance().NetSend(m_netid, (const char *)&nstg, sizeof(Protocol::SCNPCStartToGather));
	m_is_auto_gather = (is_auto_gather == 1);
	EventHandler::Instance().OnStartGather(this, tmp_gather_id, m_gather_seq_id, position);
}

int Role::GetGatherSpeedupPercent()
{
	// 按照百分比计算
	int sum = 0;



	return sum;
}

void Role::UpdateGather(time_t now)
{
	if (!NpcManager::Instance().CanTouchNpc(this, m_gather_seq_id))
	{
		this->OnStopGathering(false);
		return;
	}

	if (m_gather_seq_id < NpcManager::MIN_NPC_SEQ)		// 场景采集物 - 直接走事件
	{
		if (now >= m_gather_need_timestamp + NpcManager::MAX_NPC_GATHER_LIMIT)
		{
			const NPCDynamicInformaCfg * gather_cfg = NPCPOOL->GetNPCInformaCfg(m_gather_seq_id);
			if (NULL == gather_cfg)
			{
				this->OnStopGathering(false);
				return;
			}

			const ServerNPCInfoCfg * npc_info = NPCPOOL->GetServerNPCInfo(gather_cfg->npc_id);
			if (NULL == npc_info || SERVER_NPC_TYPE_GATHERS != npc_info->server_npc_type) return;

			if (!this->OnGiveGatherReward(npc_info->server_npc_param.gather_param))
			{
				this->OnStopGathering(false);
				return;
			}

			EventHandler::Instance().OnSuccsGather(this, gather_cfg->npc_id, m_gather_seq_id, gather_cfg->npc_posi);
			this->OnStopGathering(true);
		}
	}
	else		// 服务器采集物
	{
		NPC * gather_obj = NpcManager::Instance().GetServerNPC(m_gather_seq_id);
		if (NULL == gather_obj || OBJ_TYPE_NPC_GATHERS != gather_obj->GetObjType())
		{
			this->OnStopGathering(false);
			return;
		}

		NPCGather * gather_emp = (NPCGather *)gather_obj;
		if (NULL == gather_emp || !gather_emp->CanGather(this) ||
			!this->GetScene()->GetSpecialLogic()->CanGather(this, gather_emp))
		{
			this->OnStopGathering(false);
			return;
		}

		if (m_gather_need_timestamp > 0)
		{
			if (now >= m_gather_need_timestamp + NpcManager::MAX_NPC_GATHER_LIMIT)
			{
				if (gather_emp->GatherThis(this))
				{
					this->OnStopGathering(true);
				}
				else
				{
					this->OnStopGathering(false);
				}
			}
		}
		else
		{
			if (gather_emp->GatherThis(this))
			{
				this->OnStopGathering(true);
			}
			else
			{
				this->OnStopGathering(false);
			}
		}
	}
}

void Role::OnStopGathering(bool is_succ)
{
	if (RoleStatusManager::ROLE_STATUS_GATHER != m_role_status_manager.GetRoleStatus()) return;

	m_role_status_manager.OnRoleStopGather();

	if (NULL != m_scene)			// 通知周围玩家
	{
		static Protocol::SCNPCStopGathering nsg;
		nsg.role_obj_id = this->GetId();
		nsg.gather_seq = m_gather_seq_id;
		nsg.reasons = is_succ ? Protocol::SCNPCStopGathering::CANCEL_REASON_SUCC : Protocol::SCNPCStopGathering::CANCEL_REASON_FAIL;
		m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&nsg, sizeof(nsg));
	}
	m_gather_seq_id = 0;
}

void Role::OnEndGather(int seq_id, ObjID gather_id, int is_auto_gather)
{
	m_is_auto_gather = (is_auto_gather == 1);
	if (!NpcManager::Instance().CanTouchNpc(this, seq_id))
	{
		this->OnStopGathering(false);
		return;
	}
	if (seq_id < NpcManager::MIN_NPC_SEQ)		// 场景采集物
	{
		const NPCDynamicInformaCfg * npc_cfg = NPCPOOL->GetNPCInformaCfg(seq_id);
		if (NULL == npc_cfg)
		{
			this->OnStopGathering(false);
			return;
		}

		const ServerNPCInfoCfg * npc_info = NPCPOOL->GetServerNPCInfo(npc_cfg->npc_id);
		if (NULL == npc_info || SERVER_NPC_TYPE_GATHERS != npc_info->server_npc_type) return;

		unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
		if (now_time < m_gather_need_timestamp)
		{
			this->NoticeNum(errornum::EN_GATHER_TIME_NOT_ENOUGH);
			this->OnStopGathering(false);
			return;
		}

		if (!this->OnGiveGatherReward(npc_info->server_npc_param.gather_param))
		{
			this->OnStopGathering(false);
			return;
		}

		EventHandler::Instance().OnSuccsGather(this, npc_cfg->npc_id, m_gather_seq_id, npc_cfg->npc_posi);
	}
	else		// 服务器采集物
	{
		Obj * gather_obj = m_scene->GetObj(gather_id);
		if (gather_obj == NULL || gather_obj->GetObjType() != Obj::OBJ_TYPE_NPC_GATHERS)
		{
			this->OnStopGathering(false);
			return;
		}
		NPCGather * gathers = (NPCGather *)gather_obj;
		if (seq_id != gathers->GetNpcSeq())
		{
			this->OnStopGathering(false);
			return;
		}

		NPCGather * gather_emp = (NPCGather *)gather_obj;
		if (NULL == gather_emp || !gather_emp->CanGather(this) ||
			!this->GetScene()->GetSpecialLogic()->CanGather(this, gather_emp))
		{
			this->OnStopGathering(false);
			return;
		}

		unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
		if (now_time < m_gather_need_timestamp)
		{
			this->NoticeNum(errornum::EN_GATHER_TIME_NOT_ENOUGH);
			this->OnStopGathering(false);
			return;
		}
		if (!gathers->GatherThis(this))
		{
			this->OnStopGathering(false);
			return;
		}
	}

	this->OnStopGathering(true);
}

bool Role::OnGiveGatherReward(const ServerNPCGatherParam & gather_param)
{
	std::vector<ItemConfigData> award_list;
	for (int award_num = 0; award_num < gather_param.award_item_count && award_num < ARRAY_LENGTH(gather_param.gather_item_group_list); award_num++)
	{
		const ItemConfigData * award_obj = NPCPOOL->RandGatherAwardItem(gather_param.gather_item_group_list[award_num]);
		if (NULL != award_obj)
		{
			award_list.push_back(*award_obj);
		}
	}

	if (!award_list.empty())
	{
		const ItemBase * item_base = ITEMPOOL->GetItem(award_list[0].item_id);
		if (NULL == item_base)
		{
			return false;
		}
		int new_life_skill_type = 0, new_life_skill_level = 0;
		int multiple = this->GetRoleModuleManager()->GetRoleNewLifeSkill()->GetMultipleByItemSubType(item_base->GetItemSubType(), &new_life_skill_type, &new_life_skill_level);
		const GatherFbSkillLevelCfg * skill_level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetSkillLevelCfg(this->GetScene()->GetSceneID(), new_life_skill_level);
		int special_reward_group_id = 0;
		int skill_level_multiple = 1;
		if (NULL != skill_level_cfg)
		{
			if (skill_level_cfg->multiple > 0)
			{
				skill_level_multiple = skill_level_cfg->multiple;
			}
			special_reward_group_id = skill_level_cfg->special_reward_group;
		}
		if (multiple > 1 || skill_level_multiple > 1)
		{
			for (int i = 0; i < (int)award_list.size(); i++)
			{
				award_list[i].num *= multiple * skill_level_multiple;
			}
		}
		//特殊额外奖励不享有倍数
		if (special_reward_group_id > 0)
		{
			const GatherFbSpecialRewardGroupCfg * reward_group_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetRandSpecialRewardCfg(special_reward_group_id);
			if (NULL != reward_group_cfg)
			{
				award_list.push_back(reward_group_cfg->reward);
			}
		}

		Knapsack * role_bag = this->GetRoleModuleManager()->GetKnapsack();
		if (NULL == role_bag || !role_bag->CheckForPutList2(award_list) ||
			!role_bag->PutVec(award_list, PUT_REASON_GATHER_OBJECT))
		{
			return false;
		}
		this->OnSendLifeSkillNotice(item_base->GetItemId(), award_list[0].num, multiple > 1, true, new_life_skill_type);
	}
	return true;
}

void Role::OnStartFish(int fish_fb_seq)
{
	if (!m_role_status_manager.CanFish())
	{
		this->NoticeNum(errornum::EN_NPC_ROLE_IS_BUSY_CURRENT);
		return;
	}

	const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfg(fish_fb_seq);
	if (NULL == level_cfg)
	{
		return;
	}
	m_fish_fb_seq = fish_fb_seq;
	m_role_status_manager.OnRoleStartFishing();
	m_fish_need_timestamp = (unsigned int)EngineAdapter::Instance().Time() + LOGIC_CONFIG->GetRoleGatherFbConfig()->GetOtherCfg().one_way_time;

	if (NULL != m_scene)
	{
		Protocol::SCFishingNotify notify_info;
		notify_info.role_obj_id = this->GetId();

		m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&notify_info, sizeof(notify_info));
	}
}

void Role::OnStopFishing(int reason, int param1, int param2)
{
	if (RoleStatusManager::ROLE_STATUS_FISHING != m_role_status_manager.GetRoleStatus()) return;

	m_role_status_manager.OnRoleStopFishing();
	m_fish_fb_seq = -1;
	m_fish_need_timestamp = 0;
	if (NULL != m_scene)
	{
		static Protocol::SCStopFishingNotify info;
		info.role_obj_id = this->GetId();
		info.stop_reason = reason;
		info.param1 = param1;
		info.param2 = param2;

		m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&info, sizeof(info));
	}
}

void Role::OnEndFishing(int fish_fb_seq)
{
	if (-1 == m_fish_fb_seq)
	{
		this->OnStopFishing(Protocol::SCStopFishingNotify::STOP_REASON_TYPE_FAIL);
		return;
	}

	if ((unsigned int)EngineAdapter::Instance().Time() < m_fish_need_timestamp)
	{
		this->OnStopFishing(Protocol::SCStopFishingNotify::STOP_REASON_TYPE_NOT_ENOUGH_TIME);
		return;
	}

	const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfg(m_fish_fb_seq);
	if (NULL == level_cfg)
	{
		this->OnStopFishing(Protocol::SCStopFishingNotify::STOP_REASON_TYPE_FAIL);
		return;
	}
	const GatherFbRewardGroupCfg * reward_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetRewardGroupRandCfg(level_cfg->reward_group_id);
	if (NULL == reward_cfg)
	{
		this->OnStopFishing(Protocol::SCStopFishingNotify::STOP_REASON_TYPE_FAIL);
		return;
	}
	int item_count = 0;
	if (!this->OnGiveGatherReward(reward_cfg->item, false, &item_count))
	{
		this->OnStopFishing(Protocol::SCStopFishingNotify::STOP_REASON_TYPE_FAIL);
		return;
	}

	this->OnStopFishing(Protocol::SCStopFishingNotify::STOP_REASON_TYPE_SUCC, reward_cfg->item.item_id, item_count);
	EventHandler::Instance().OnSuccFish(this);
}

bool Role::OnGiveGatherReward(ItemConfigData reward, bool is_gather, ARG_OUT int * out_item_num)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
	if (NULL == item_base)
	{
		return false;
	}
	std::vector<ItemConfigData> reward_list;
	int new_life_skill_type = 0, new_life_skill_level = 0;
	int multiple = this->GetRoleModuleManager()->GetRoleNewLifeSkill()->GetMultipleByItemSubType(item_base->GetItemSubType(), &new_life_skill_type, &new_life_skill_level);
	const GatherFbSkillLevelCfg * skill_level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetSkillLevelCfg(this->GetScene()->GetSceneID(), new_life_skill_level);
	int skill_level_multiple = 1;
	if (NULL != skill_level_cfg)
	{
		if (skill_level_cfg->multiple > 0)
		{
			skill_level_multiple = skill_level_cfg->multiple;
		}
		// 特殊额外奖励不享有倍数
		const GatherFbSpecialRewardGroupCfg * reward_group_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetRandSpecialRewardCfg(skill_level_cfg->special_reward_group);
		if (NULL != reward_group_cfg)
		{
			reward_list.push_back(reward_group_cfg->reward);
		}
	}
	if (multiple > 1 || skill_level_multiple > 1)
	{
		reward.num *= multiple * skill_level_multiple;
	}
	reward_list.push_back(reward);
	bool is_put_succ = false;
	if (this->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(reward_list))
	{
		if (this->GetRoleModuleManager()->GetKnapsack()->PutVec(reward_list, is_gather ? PUT_REASON_GATHER_OBJECT : PUT_REASON_FISHING))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		return false;
	}
	if (NULL != out_item_num)
	{
		*out_item_num = reward.num;
	}

	this->OnSendLifeSkillNotice(reward.item_id, reward.num, multiple > 1, is_gather, new_life_skill_type);
	return true;
}

void Role::OnSendLifeSkillNotice(int item_id, int item_num, bool is_double, bool is_gather, int new_life_skill_type)
{
	if (is_double)
	{
		this->GetRoleModuleManager()->GetKnapsack()->SendGetItemNotice(Protocol::SCKnapsackGetItemNotice::NOTICE_TYPE_LIFE_SKILL, item_id, item_num, new_life_skill_type);
	}
	else
	{
		/*int length = 0;
		if (is_gather)
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_life_skill_gather_notice, item_id);
		}
		else
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_life_skill_fish_notice, item_id);
		}
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(this, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
		}*/
	}
}

void Role::ForbidRole(const char* reason, int forbid_time)
{
	if (NULL == reason || forbid_time <= 0) return;

	lgprotocal::RoleForbid rf;
	this->GetPlatName(rf.plat_name);
	STRNCPY(rf.reason, reason, sizeof(rf.reason));
	rf.forbit_time_s = forbid_time;
	rf.role_id = m_uid;
	InternalComm::Instance().NetSend(InternalComm::Instance().m_login_server_netid, (const char *)&rf, sizeof(lgprotocal::RoleForbid));

	gamelog::g_log_forbid.printf(LL_INFO, "[Role::ForbidRole][%d][%s] plat_name:%s reason:%s",
		UidToInt(m_user_id), (const char*)m_name, (const char*)m_plat_name, (const char*)reason);

	ROLE_LOG_QUICK6(LOG_TYPE_FORBID_ROLE, this, 0, 0, reason, NULL);
}

void RMIQueryNameExistBackObjectImpl::QueryNameExistRet(int result)
{
	Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	if (0 == result)
	{
		role->OnResetNameRetConsumeItem();//实际上改名字后才扣除材料
		if (role->ResetName(role_name))
		{
			RMIResetNameBackObjectImpl *impl = new RMIResetNameBackObjectImpl();
			impl->user_id = user_id;
			F_STRNCPY(impl->role_name, role_name, sizeof(GameName));

			RMILoginClient rc;
			rc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());

			rc.ResetNameAsyn(user_id.db_index, UidToInt(user_id), (const char*)role_name, 0, impl);

			return;
		}
	}
	else
	{
		role->NoticeNum(errornum::EN_ROLE_NAME_REPEAT);
	}
}

void RMIResetNameBackObjectImpl::ResetNameRet(int result)
{
	if (0 != result)
	{
		// 失败
		gamelog::g_log_debug.printf(LL_INFO, "[RMIResetNameBackObjectImpl::ResetNameRet Fail_RoleNameMap][user[%s %d] result:%d]", role_name, UidToInt(user_id), result);
	}
}
int Role::GetAuthority()
{
	return m_authority_type;
}

int Role::GetWildBossRewardTimes()
{
	return m_role_module_manager->GetCommonData().wild_boss_get_reward_times;
}

bool Role::HasWildBossRewardTimes()
{
	int max_times = LOGIC_CONFIG->GetWildBossConfig()->GetOtherCfg().get_reward_times;
	return this->GetWildBossRewardTimes() < max_times;
}

void Role::SetWildBossRewardTimes(int times)
{
	m_role_module_manager->GetCommonData().wild_boss_get_reward_times = times;
}

void Role::AddPosData(PosData data)
{
	m_pos_data = data;
}

const PosData * Role::GetPosData()
{
	return &m_pos_data;
}

void Role::ChangeAppearance(short avatar_type, short color)
{
	if (!AppearanceConfig::Instance().CheckAvatarAndColor(avatar_type, color, this->GetProfession()))
	{
		this->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	const ConstantCfg &c_cfg = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg();
	if (!m_role_module_manager->GetKnapsack()->ConsumeItem(c_cfg.change_appearance_item, 1, __FUNCTION__, 0, true))
	{
		return;
	}
	bool is_change = false;
	if (m_avatar_type != avatar_type || m_color != color)
	{
		is_change = true;
	}
	m_avatar_type = avatar_type;
	m_color = (char)color;

	m_role_module_manager->MakeAppearanceData();
	this->SendRoleInfo();
	this->SendChangeAppearanceNotice();

	EventHandler::Instance().OnRoleChangeAppearance(this, is_change);
}

void Role::ChangeHeadshot(short headshot_id)
{
	if (headshot_id == m_headshot_id) return;

	if (0 != headshot_id)
	{
		const HeadshotCfg* cfg = AppearanceConfig::Instance().GetHeadshotCfg(headshot_id);
		if (NULL == cfg)
		{
			this->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		if (cfg->avatar_type != this->GetAvatarType())
		{
			this->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		if (HeadshotCfg::COND_TYPE_ADVANCE_TIMES == cfg->type && cfg->advance_times > this->GetAdvanceTimes())
		{
			this->NoticeNum(errornum::EN_HEADSHOT_ADVANCE_TIMES_LIMIT);
			return;
		}

		if (HeadshotCfg::COND_TYPE_SHIZHUANG == cfg->type && !m_role_module_manager->GetSurface()->IsActiveSurface(cfg->surface_id))
		{
			this->NoticeNum(errornum::EN_HEADSHOT_SHIZHUANG_LIMIT);
			return;
		}
	}

	m_headshot_id = headshot_id;
	m_role_module_manager->MakeAppearanceData();
	this->SendRoleInfo();
	this->SendChangeAppearanceNotice();

	EventHandler::Instance().OnRoleChangeAppearance(this, true);
}

void Role::SendChangeAppearanceNotice()
{
	Protocol::SCChangeAppearanceRole info;
	info.obj_id = this->GetId();
	info.appearance = m_role_module_manager->GetAppearance();
	this->GetScene()->GetZoneMatrix()->NotifyAreaMsg(this->GetObserHandle(), &info, sizeof(info));

	//printf("weapon_id:%d\n", info.appearance.weapon_id);
}

void Role::NotifyMyStatusToOthers()
{
	static Protocol::SCRoleStatusChangeNotice rscn;

	rscn.obj_id = this->GetId();
	rscn.cur_status = m_role_status_manager.GetRoleStatus();
	rscn.cur_move_status = m_role_status_manager.GetRoleMoveStatus();

	if (NULL != m_scene) m_scene->GetZoneMatrix()->NotifyAreaMsg(this->GetObserHandle(), &rscn, sizeof(rscn));
}

bool Role::IsInCross()
{
	return CrossConfig::Instance().IsHiddenServer();
}

void Role::ReturnToOriginServer()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	static Protocol::SCReturnOriginalServer msg;
	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char*)&msg, sizeof(msg));
}

void Role::OnCheckReallyInBattle(unsigned int now)
{
	m_last_check_really_in_battle_time = now;
}

bool Role::IsProfSuitable(int prof)
{
	return PROF_TYPE_PROF_NO_LIMIT == prof || this->GetBaseProfession() == prof;
}

int Role::GetBattleIndex()
{
	const RoleBattleInfo* rbi = BattleManagerLocal::GetInstance().GetRoleBattleInfoConst(this->GetUID());
	if (NULL == rbi) return -1;

	return rbi->battle_server_index; // 以后要做成按玩家正在战斗的battleindex来返回
}

void Role::GmRoleOrder(int type, int param)
{
	enum Role_Gm_Order_Type
	{
		ROLE_CHANGE_TEST_STATUS = 0,		// 设置测试状态
		ROLE_PRINT_GATHER_INFOR = 1,		// 输出采集信息
	};

	char * print = new char[2048];
	switch (type)
	{
	case ROLE_CHANGE_TEST_STATUS:
		m_is_testing = (0 != param);
		sprintf(print, "Change Test Status -> Now: %d", m_is_testing ? 1 : 0);
		this->PrintToScreen(print);
		break;
	case ROLE_PRINT_GATHER_INFOR:
		sprintf(print, "Gather Info -> SeqID: %d | RoleStatus: %d | NowTime: %lld / %u", m_gather_seq_id, m_role_status_manager.GetRoleStatus(), (long long)EngineAdapter::Instance().Time(), m_gather_need_timestamp);
		this->PrintToScreen(print);
		break;
	}

	delete[] print;
}

void Role::PrintToScreen(const char * sentence)
{
	if (0 == m_is_testing) return;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), sentence);
	if (length <= 0) return;

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
	{
		EngineAdapter::Instance().NetSend(m_netid, (const char*)&sm, sendlen);
	}
}

bool Role::IsForbidTalk()
{
	time_t now_time = EngineAdapter::Instance().Time();
	if (m_forbid_talk_time >= now_time)
	{
		//不飘字
		//this->NoticeNum(errornum::EN_FORBID_TALK);
		gamelog::g_log_forbid.printf(LL_INFO, "Role::IsForbidTalk user[%d %s] m_forbid_talk_time[%d] now_time[%d]", m_uid, m_name, (int)m_forbid_talk_time, (int)now_time);
		return true;
	}

	return false;
}

int Role::GetCapability()
{
	return m_role_module_manager->GetCapability()->GetRoleCapability();
}

int Role::GetCapabilityWithFightPetAndPartner()
{
	return this->GetCapability() + m_role_module_manager->GetPartner()->GetFightParatnerCap() + m_role_module_manager->GetPet()->GetPetCapability_1(m_role_module_manager->GetPet()->GetFightIndex());
}

int Role::GetCapabilityWithFight()
{
	return this->GetCapability() + m_role_module_manager->GetPartner()->GetFightParatnerCap() + m_role_module_manager->GetPet()->GetAllFightCapability();
}

void Role::SendServerTimeDate()
{
	return; // 客户端要求不下发

	/*
	const tm * local_time = EngineAdapter::Instance().LocalTime();
	unsigned int time_stamp = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int zero_time = time_stamp - (local_time->tm_hour * 3600 + local_time->tm_min * 60 + local_time->tm_sec);

	Protocol::SCTimeDateInfo  protocol;
	protocol.time = zero_time;
	protocol.year = (unsigned short)local_time->tm_year;
	protocol.mon = (unsigned short)local_time->tm_mon;
	protocol.day = (unsigned char)local_time->tm_mday;
	protocol.hour = 0;
	protocol.min = 0;
	protocol.sec = 0;

	EngineAdapter::Instance().NetSend(this->GetNetId(), &protocol, sizeof(protocol));
	*/
}

void Role::SendServerTime(char is_server_driving_send)
{
	UniqueServerID usid = UniqueServerID(this->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), this->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId());
	unsigned int role_server_real_start_time = World::GetInstWorld()->GetRealServerOpenTime(usid);
	unsigned int role_server_zero_time = World::GetInstWorld()->GetServerOpenZeroTime(usid);

	Protocol::SCTimeAck ta;
	ta.server_time = (unsigned int)EngineAdapter::Instance().Time();
	ta.server_real_start_time = role_server_real_start_time;
	ta.server_real_combine_time = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetServerRealCombineTime();
	ta.open_days = GetDayIndex(role_server_zero_time, EngineAdapter::Instance().Time()) + 1;
	ta.server_zero_start_time = role_server_zero_time;
	ta.is_server_driving_send = is_server_driving_send;
	ta.reserve_ch = 0;
	ta.reserve_sh = 0;
	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&ta, sizeof(Protocol::SCTimeAck));
}

void Role::LoginOriginGameNoticeHidden()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	UniqueUserID uuid = this->GetUniqueUserID();
	crossgameprotocal::GameHiddenCommonInfoNoticeHidden send_hidden_req;
	send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE_ROLE_LOGIN;
	send_hidden_req.server_id = LocalConfig::Instance().GetMergeServerId();
	send_hidden_req.is_all_sync = 0;
	send_hidden_req.param1 = UidToInt(uuid.user_id);
	InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));
}

void Role::LogoutOriginGameNoticeHidden()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	UniqueUserID uuid = this->GetUniqueUserID();
	crossgameprotocal::GameHiddenCommonInfoNoticeHidden send_hidden_req;
	send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE_ROLE_LOGOUT;
	send_hidden_req.server_id = LocalConfig::Instance().GetMergeServerId();
	send_hidden_req.is_all_sync = 0;
	send_hidden_req.param1 = UidToInt(uuid.user_id);
	InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));
}

void Role::AddSomeVisibleRole(int add_num)
{
	// 1. 查找在玩家屏幕范围内的所有obj
	int CLIENT_SCREEN_X = (int)GLOBALCONFIG->GetKeyConfig_RoleAoiX();	// AOI_X 
	int CLIENT_SCREEN_Y = (int)GLOBALCONFIG->GetKeyConfig_RoleAoiY();	// AOI_Y

	if (m_scene->GetSceneType() == Scene::SCENE_TYPE_TU_ZI_KUAI_PAO)
	{
		CLIENT_SCREEN_X = 2500;
		CLIENT_SCREEN_Y = 2500;
	}

	static ObjID obj_list[512];
	int obj_num = m_scene->GetZoneMatrix()->GetObjByArea(m_posi, CLIENT_SCREEN_X / 2, CLIENT_SCREEN_Y / 2, obj_list, ARRAY_ITEM_COUNT(obj_list));

	// 2. 洗牌一下，做一下随机
	RandomShuffle(obj_list, obj_num);

	// 3. 找出列表中的不在visible_role_map的玩家，给客户端发包
	int num = 0;
	for (int i = 0; i < obj_num && num < add_num; ++i)
	{
		Obj* obj = m_scene->GetObj(obj_list[i]);
		if (NULL == obj || OBJ_TYPE_ROLE != obj->GetObjType()) continue;

		Role* role = (Role*)obj;
		if (role->GetUID() == this->GetUID()) continue;
		if (m_visible_role_map.find(role->GetUID()) != m_visible_role_map.end()) continue;

		this->OnAOIEnterRole(role); // OnAOIEnterRole会把role_id加入m_visible_role_map
	}
}

struct VisibleRoleInfo
{
	VisibleRoleInfo() : role_id(0), obj_id(INVALID_OBJ_ID) {}

	int role_id;
	ObjID obj_id;
};

void Role::RemoveSomeVisibleRole(int remove_num)
{
	std::vector<VisibleRoleInfo> erase_vec;
	erase_vec.reserve(m_visible_role_map.size());

	std::map<int, int>::iterator it = m_visible_role_map.begin();
	for (; it != m_visible_role_map.end(); ++it)
	{
		Role* role = m_scene->GetRoleByUID(it->first);
		if (NULL != role)
		{
			Team* team = role->GetMyTeam();
			Team* myteam = this->GetMyTeam();
			if (NULL != team && NULL != myteam && team->GetTeamIndex() == myteam->GetTeamIndex())
			{
				// 同队就不删了，一定要看到队友	
				continue;
			}
			else
			{
				static VisibleRoleInfo info;
				info.obj_id = role->GetId();
				info.role_id = role->GetUID();
				erase_vec.push_back(info);
			}
		}
		else
		{
			// 人不在场景里了，这情况一般不会出现
			static VisibleRoleInfo info;
			info.obj_id = INVALID_OBJ_ID;
			info.role_id = it->first;
			erase_vec.push_back(info);

#ifdef _DEBUG
			assert(0); // 正常情况不应出现m_visible_role_map中的玩家已经不在该场景的情况
#endif
		}
	}

	std::random_shuffle(erase_vec.begin(), erase_vec.end());
	for (int i = 0; i < remove_num && i < (int)erase_vec.size(); ++i)
	{
		if (erase_vec[i].obj_id != INVALID_OBJ_ID)
		{
			Protocol::SCVisibleObjLeave msg;
			msg.obj_id = erase_vec[i].obj_id;
			msg.reserved = 0;
			EngineAdapter::Instance().NetSend(m_netid, &msg, sizeof(msg));
		}
		m_visible_role_map.erase(erase_vec[i].role_id);
	}
}

void Role::OnVisibleRoleNumSettingChange(int cur_setting_num)
{
	if (cur_setting_num > (int)m_visible_role_map.size())
	{
		this->AddSomeVisibleRole(cur_setting_num - m_visible_role_map.size());
	}
	else if (cur_setting_num < (int)m_visible_role_map.size())
	{
		this->RemoveSomeVisibleRole(m_visible_role_map.size() - cur_setting_num);
	}
}

bool Role::IsRoleInSight(Role* other_role)
{
	// 判断other_role是否已在玩家的视野中

	if (NULL == other_role) return false;

	return m_visible_role_map.find(other_role->GetUID()) != m_visible_role_map.end();
}

MsgRoleBaseInfo Role::GetMsgRoleBaseInfo() const
{
	MsgRoleBaseInfo ret;
	ret.uid = this->GetUID();
	this->GetName(ret.name);
	ret.role_level = this->GetLevel();
	ret.prof = this->GetProfession();
	ret.avatar_type = this->GetAvatarType();
	ret.headshot_id = this->GetHeadshotID();

	return ret;
}


void Role::SendGuildActiveInfo(int _seq)
{
	Protocol::SCGuildActiveInfoMember info;

	info.seq = _seq;
	info.flag = this->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag;

	EngineAdapter::Instance().NetSend(this->GetNetId(), (const char *)&info, sizeof(info));
}


void Role::LogActive(int log_avtive_type, int param1 /*= 0*/, int param2 /*= 0*/, int param3 /*= 0*/, int param4 /*= 0*/)
{
	ROLE_LOG_QUICK10(LOG_TYPE_ACTIVE_STAT, this, log_avtive_type, param1, NULL, NULL, param2, param3, param4, 0, 0);
}

bool Role::IsGM() const
{
	if (!GLOBALCONFIG->IsGmOpen())
	{
		return false;
	}

	if (Role::AUTHORITY_TYPE_GM != m_authority_type)  // 正式版本 必须设置了gm才允许开这个命令
	{
		return false;
	}

	return true;
}

bool Role::IsFromDev() const
{
	return LocalConfig::Instance().GetPlatType() == globalconfig::SpidConfig::GetSpidByStrPlatName("dev");
}

bool Role::CheckWaiGuaMove()
{
	return m_role_module_manager->CheckWaiguaMove();
}

void Role::SetCurrencyType(int currency_type)
{
	if (currency_type != m_role_module_manager->GetCommonData().show_currency_type)
	{
		m_role_module_manager->GetCommonData().show_currency_type = currency_type;
	}
}

int Role::GetCurrencyType()
{
	return m_role_module_manager->GetCommonData().show_currency_type;
}

void Role::SetClientShowSpid(int spid)
{
	if (spid != m_role_module_manager->GetCommonData().client_spid)
	{
		m_role_module_manager->GetCommonData().client_spid = spid;
	}
}

int Role::GetClientShowSpid()
{
	return m_role_module_manager->GetCommonData().client_spid;
}