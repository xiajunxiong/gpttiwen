#include <algorithm>
#include <math.h>
#include "eventhandler.hpp"
#include "world.h"
#include "gamelog.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "obj/character/robot.h"
#include "battle/battle_manager_local.hpp"
#include "task/taskmanager.h"
#include "chat/chatmanager.h"
#include "chat/crosschatgroupmanager.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/noticeconfig.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "drop/droppool.hpp"
#include "friend/friendmanager.h"
#include "monster/monsterpool.hpp"
#include "monster/monster_group_pool.hpp"
#include "equipment/equipment.h"
#include "equipment/equipmentmanager.h"
#include "servercommon/rolenewlifeskilldef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/mysterioustrialdef.hpp"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "item/expense/norexitem.hpp"
#include "item/treasuremap/treasureitem.hpp"
#include "protocol/msgchiefelection.h"

#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "scene/fbmanager.hpp"

#include "protocol/msgwildboss.h"
#include "protocol/msgother.h"

#include "global/worldstatus/worldstatus.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guildmember.hpp"
#include "global/marriageseeking/marriageseekingmanager.h"
#include "global/usercache/usercache.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/guild/guild.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "global/team/teammatch.hpp"
#include "global/topicmanager/topicmanager.hpp"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "global/gatherdreammanager/gatherdreamrecordmanager.hpp"
#include "global/mail/mailmanager.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"

#include "other/roleactivity/roleactivity.hpp"
#include "other/marriage/marriage.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/baby/baby.hpp"
#include "other/mentor/shitu.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/collection/collection.hpp"
#include "other/monster_wave/monsterwave.h"
#include "other/schooltask/schooltask.h"
#include "other/roleattribute/roleattributeplan.hpp"
#include "other/workshop/gemstoneconfig.hpp"
#include "other/researchtask/researchtask.h"
#include "other/treasure_map/treasuremap.h"
#include "other/escort/escort.hpp"
#include "other/taskchain/taskchain.hpp"
#include "other/pet/pet.hpp"
#include "other/pet/petconfig.hpp"
#include "other/medal/medal.hpp"
#include "other/mapunlock/mapunlock.hpp"
#include "other/fabao/fabao.hpp"
#include "other/partner/partner.hpp"
#include "other/route/mailroute.hpp"
#include "other/queryhandler/queryhandler.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/marriage/marriagetask.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/salary/salary.hpp"
#include "other/challengefb/rolechallengefb.hpp"
#include "other/title/title.hpp"
#include "global/platformbattle/platformbattle.h"
#include "servercommon/errornum.h"
#include "other/funcguide/funcguide.hpp"
#include "other/title/titleconfig.hpp"
#include "other/prestige/prestige.hpp"
#include "other/roledrop/roledrop.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "other/boss_chapter/boss_chapter.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/facescore/facescore.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityexpelmonster.hpp"
#include "global/xunbao/xunbao.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityjoyseeksfavorsconfig.hpp"
#include "other/funcpreview/funcpreview.hpp"
#include "other/trade_credit/trade_credit.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/roletiandidaohen/roletiandidaohen.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/rolehundredgodboss/rolehundredgodboss.hpp"

#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "global/activity/activityimpl/activitychiefpeakbattle.hpp"
#include "global/activity/activityimpl/activityguildhonorbattle.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "config/activityconfig/worldboss/worldbossconfig.hpp"
#include "config/activityconfig/worldboss/worldboss3config.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "config/activityconfig/mazeracer/activitymazeracerconfig.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/tiandidaohen/tiandidaohen.h"
#include "global/givegift/givegift.h"
#include "global/jieyi/jieyizumanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "global/zhouqimanager/zhouqimanager.h"

#include "global/randactivity/randactivityimpl/randactivityskydropgift.hpp"
#include "global/randactivity/randactivityimpl/randactivitytianjianghaoli.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerauspicioussnow.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/roleactivity/impl/roleactivityluckyturntable.hpp"
#include "other/roleactivity/impl/roleactivityjiandanqinxin.hpp"
#include "other/roleactivity/impl/roleactivitytruntable.hpp"
#include "other/roleactivity/impl/roleactivitytujiantreasure.hpp"
#include "other/roleactivity/impl/roleactivityjoyseeksfavors.hpp"
#include "other/roleactivity/impl/roleactivityadventuregift.hpp"
#include "other/roleactivity/impl/roleactivitysports.hpp"
#include "other/roleactivity/impl/roleactivityhelpothers.hpp"
#include "other/roleactivity/impl/roleactivityhelpothers2.hpp"
#include "other/roleactivity/impl/roleactivityfateentanglement.hpp"
#include "other/roleactivity/impl/roleactivityservercompetition.hpp"
#include "other/roleactivity/impl/roleactivityzaizhanyunze.hpp"
#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"

#include "config/otherconfig/hundredgodbossconfig.hpp"
#include "battle/battle_manager_local.hpp"
#include "other/train/train.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "other/formation/formation.hpp"
#include "other/roleactivity/impl/roleactivitymonpolygift.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "other/roleactivity/impl/roleactivitygivesecretkey.hpp"
#include "other/roleactivity/impl/roleactivitypetloot.hpp"
#include "other/roleactivity/impl/roleactivitysummerlilian.hpp"
#include "other/roleactivity/impl/roleactivitycoolbreeze.hpp"
#include "other/roleactivity/impl/roleactivitytrademarketreturngold.hpp"
#include "other/schoolexercise/schoolexercise.hpp"
#include "chat/muteplayerrecorder.hpp"
#include "other/collection/collectionconfig.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "global/randactivity/randactivityimpl/randactivitylaodonghaoli.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/monitor/systemmonitor.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "global/marketmanager/marketmanager.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"
#include "scene/teamkeyregister/teamkeyregister.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "other/hongbao/hongbaomanager.hpp"
#include "equipment/petsoulequipment.h"
#include "other/bigdipper/bigdippermanager.hpp"
#include "other/prestige_task/prestige_task.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "other/dujie/dujie.hpp"
#include "other/roleactivity/impl/roleactivitymazesummer.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/roleactivity/impl/roleactivitybackreward.hpp"
#include "other/yinyangeye/yinyangeye.hpp"
#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "friend/friendintimacy.hpp"
#include "other/endlesstower/endlesstower.hpp"
#include "servercommon/pethelperformationdef.hpp"
#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include "global/randactivity/randactivityimpl/randactivitymozuzainali.hpp"
#include "other/roleactivity/impl/roleactivityshanhaizhengfeng.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"
#include "other/roleactivity/impl/roleactivityjingjiyueka.hpp"
#include "other/roleactivity/impl/roleactivityyunzecutepet.hpp"
#include "other/mirageterritory/mirageterritory.hpp"
#include "other/roleactivity/impl/roleactivitytigerfisheveryyear.hpp"
#include "other/roleactivity/impl/roleactivitytigerluckymoney.hpp"
#include "global/tigerbringsblessing/tigerbringsblessing.hpp"
#include "other/fallengod/fallengod.hpp"
#include "other/roleactivity/impl/roleactivityronglianyouli.hpp"
#include "obj/otherobj/xunyoucar.hpp"
#include "qingyuan/weddingmanger.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanshizhuang.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanleichong.hpp"
#include "global/worldarena/worldarena.hpp"
#include "other/roleactivity/impl/roleactivityyunzecutepet.hpp"
#include "other/mirageterritory/mirageterritory.hpp"
#include "other/income/income.hpp"
#include "other/roleactivity/impl/roleactivitygodbeastadvent.hpp"
#include "config/randactivityconfig/impl/randactivitygodbeastadventconfig.hpp"
#include "other/roleactivity/impl/roleactivityqiyuankongmingdeng.hpp"

#include "other/capability/capability.hpp"
#include "other/dayreward/dayreward.hpp"
#include "other/pet/petbadgeconfig.hpp"
#include "other/levelcomplement/newlevelcomplement.hpp"
#include "other/yaoxingshilian/yaoxingshilian.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddie.hpp"
#include "other/cycleactivity/concentricknot/concentricknot.hpp"
#include "other/cycleactivity/sanxianguidong/sanxianguidong.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"
#include "other/roleactivity/impl/roleactivityduanyangxiangnang.hpp"
#include "config/randactivityconfig/impl/randactivityduanyangxiangnangconfig.hpp"
#include "other/roleactivity/impl/roleactivitylantangchumu.hpp"
#include "config/randactivityconfig/impl/randactivitylantangchumuconfig.hpp"
#include "other/cycleactivity/kejuexam/kejuexam.hpp"
#include "other/kuafuyingxionglu/rolekuafuyingxionglu.hpp"

#include "servercommon/redis/redismanager.hpp"
#include "servercommon/performancecatch.hpp"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "scene/speciallogicimpl/speciallogicuta.hpp"
#include "global/team/teamrecruit.hpp"
#include "other/hongmengtianjie/hongmengtianjiemanager.hpp"
#include "other/roleactivity/impl/roleactivitycharismatictanabata.hpp"
#include "config/otherconfig/elementelfconfig.hpp"
#include "other/elementelf/elementelf.hpp"
#include "global/offlinebattledata/offlinebattledata.hpp"
#include "other/couragechallenge/newcouragechallenge.hpp"
#include "other/roleguildtask/roleguildtask.hpp"

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

EventHandler & EventHandler::Instance()
{
	static EventHandler event_handler;
	return event_handler;
}

void EventHandler::OnRoleLogin(Role* role)
{
	if (NULL == role) return;

	int online_status = CrossConfig::Instance().IsHiddenServer() ? UserStatusNode::STATUS_TYPE_CROSS : UserStatusNode::STATUS_TYPE_ONLINE;
	UserCacheManager::Instance().UpdateUserOnlineStatus(role->GetUID(), online_status);
	role->UpdateCacheNode();

	WorldStatus::Instance().OnUserLogin(role);
	ZhouQiManager::Instance().OnUserLogin(role);
	OfflineRegisterManager::Instance().OnUserFirstEnterScene(role);
	GuildManager::Instance().OnRoleLogin(role);
	ActivityManager::Instance().OnUserLogin(role);
	GuildHonorBattleManager::Instance().OnUserLogin(role);

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		ramgr->OnUserLogin(role);
	}
	role->GetRoleModuleManager()->GetFlyUpSystem()->OnRoleLogin();
	ChatManager::Instance().OnUserLogin(role);
	BigDipperManager::Instance().OnUserLogin(role);
	CrossChatGroupManager::Instance().OnUserLogin(role);
	RankManager::Instance().OnUserLogin(role);
	ColosseumManager::Instance().OnUserLogin(role);
	MarriageTask::Instance().OnUserLogin(role);
	PlatformBattle::Instance().OnUserLogin(role);
	JieYiZuManager::Instance().OnUserLogin(role);
	QingYuanManager::Instance().OnUserLogin(role);
	XunBao::Instance().OnUserLogin(role);
	TianDiDaoHen::Instance().OnUserLogin(role);
	GiveGift::Instance().OnUserLogin(role);	
	CowReportGoodNewsManager::Instance().OnLogin(role);
	FriendManager::Instance().OnLogin(role);
	// 如果是online，聊天就成了本地发送了
	UserCacheManager::Instance().SyncOnlineStatus(role->GetUID(), online_status);
	TigerBringsBlessingManager::Instance().OnLogin(role);
	WeddingManger::Instance().OnRoleLogIn(role);
	WeddingLocalManger::Instance().OnRoleLogIn(role);
	WorldTeamArenaSignUpManager::Instance().OnUserLogin(role);
	KuaFuYingXiongLuManager::Instance().OnRoleLogin(role);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenUserLoginInOrigin msg;
		msg.role_id = role->GetUID();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}

#ifdef REDIS_TEST
	{
		//TEST 1
		PerformanceCatch::Instance().SimplePerformanceCatchBegin(PERFORMANCE_TEST_REDIS);
		RedisManager::Instance().SET(STRING_SPRINTF("Role_%d#OnlineStatus", role->GetUID()), "ONLINE");
		PerformanceCatch::Instance().SimplePerformanceCatchEnd(PERFORMANCE_TEST_REDIS);

		PerformanceCatch::Instance().SimplePerformanceCatchBegin(PERFORMANCE_TEST_REDIS);
		RedisManager::Instance().SET(STRING_SPRINTF("Role_%d#CrossStatus", role->GetUID()), CrossConfig::Instance().IsHiddenServer() ? "CROSS" : "ORIGIN");
		PerformanceCatch::Instance().SimplePerformanceCatchEnd(PERFORMANCE_TEST_REDIS);

		PerformanceCatch::Instance().SimplePerformanceCatchBegin(PERFORMANCE_TEST_REDIS);
		std::string rOnlineStatus;
		RedisManager::Instance().GET(STRING_SPRINTF("Role_%d#OnlineStatus", role->GetUID()), rOnlineStatus);
		PerformanceCatch::Instance().SimplePerformanceCatchEnd(PERFORMANCE_TEST_REDIS);

		PerformanceCatch::Instance().SimplePerformanceCatchBegin(PERFORMANCE_TEST_REDIS);
		std::string rCrossStatus;
		RedisManager::Instance().GET(STRING_SPRINTF("Role_%d#CrossStatus", role->GetUID()), rCrossStatus);
		PerformanceCatch::Instance().SimplePerformanceCatchEnd(PERFORMANCE_TEST_REDIS);

		gamelog::g_log_debug.printf(LL_INFO, "EventHandler::OnRoleLogin role[%d,%s] OnlineStatus[%s] CrossStatus[%s]",
			role->GetUID(), role->GetName(), rOnlineStatus.c_str(), rCrossStatus.c_str());
	}

	{
		//TEST 2
		KEYS keys;
		VDATA vdata;
		keys.reserve(2);
		vdata.reserve(2);
		
		keys.push_back(STRING_SPRINTF("Role_%d#LoginTimestamp", role->GetUID()));
		vdata.push_back(STRING_SPRINTF("%u", static_cast<unsigned int>(EngineAdapter::Instance().Time())));

		keys.push_back(STRING_SPRINTF("Role_%d#Level", role->GetUID()));
		vdata.push_back(STRING_SPRINTF("%d", role->GetLevel()));

		RedisManager::Instance().MSET(keys, vdata);

		VDATA results;
		RedisManager::Instance().MGET(keys, results);

		for (size_t i = 0; i < results.size(); ++i)
		{
			gamelog::g_log_debug.printf(LL_INFO, "EventHandler::OnRoleLogin role[%d,%s] %s[%s]",
				role->GetUID(), role->GetName(), keys[i].c_str(), results[i].c_str());
		}
	}
#endif
}

void EventHandler::OnRoleLogout(Role* role)
{
	if (NULL == role) return;

	FriendManager::Instance().OnLogout(role);
	role->GetRoleModuleManager()->GetMarriage()->OnLogout();
	role->GetRoleModuleManager()->GetKnapsack()->OnRoleLogout();
	role->GetRoleModuleManager()->GetFlyUpSystem()->OnRoleLogout();
	role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnRoleLogout();
	role->GetRoleModuleManager()->GetConcentricKnot()->OnLogout();
	role->GetRoleModuleManager()->GetSanXianGuiDong()->OnLogout();
	// 如果是前往跨服，那这里不管如何都先设置为离线，待跨服返回信息再设置为 UserStatusNode::STATUS_TYPE_CROSS
	UserCacheManager::Instance().UpdateUserOnlineStatus(role->GetUID(), UserStatusNode::STATUS_TYPE_OFFLINE);
	role->UpdateCacheNode();

	WorldTeamArenaSignUpManager::Instance().OnUserLogout(role);
	BattleManagerLocal::GetInstance().OnRoleLogout(role);
	TeamManager::Instance().UserLogout(role);
	GuildManager::Instance().OnRoleLogout(role);
	ActivityManager::Instance().OnUserLogout(role);
	JieYiZuManager::Instance().OnUserLogout(role);
	QingYuanManager::Instance().OnUserLogout(role);

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		ramgr->OnUserLogout(role);
	}

	ChatManager::Instance().OnUserLogout(role);
	CrossChatGroupManager::Instance().OnUserLogout(role);
	QueryHandler::Instance().OnUserLogout(role);
	MarriageTask::Instance().OnUserLogout(role);
	ColosseumManager::Instance().OnUserLogout(role);
	TopicManager::Instance()->OnRoleLogout(role);
	UserCacheManager::Instance().SyncOnlineStatus(role->GetUID(), UserStatusNode::STATUS_TYPE_OFFLINE);
	DrawManager::Instance().OnRoleLogOut(role);
	TeamMatch::Instance().OnUserLogout(role);
	SystemMonitor::Instance().OnRoleLogout(role);
	WildBossManager::Instance().OnRoleLogout(role);
	WeddingManger::Instance().OnRoleLogOut(role);
	WorldArena::Instance().OnUserLogout(role->GetUserId());
	TeamRecruit::Instance().OnRoleLogout(role->GetUID());
	OfflineBattleDataMgr::Instance().OnLogout(role);

#ifdef REDIS_TEST
	RedisManager::Instance().SET(STRING_SPRINTF("Role_%d#OnlineStatus", role->GetUID()), "OFFLINE");

	std::string rOnlineStatus;
	RedisManager::Instance().GET(STRING_SPRINTF("Role_%d#OnlineStatus", role->GetUID()), rOnlineStatus);
	gamelog::g_log_debug.printf(LL_INFO, "EventHandler::OnRoleLogout role[%d,%s] OnlineStatus[%s]",
		role->GetUID(), role->GetName(), rOnlineStatus.c_str());
#endif

	

}

void EventHandler::OnRoleChangeStatus(Role * role, int old_status, int cur_status)
{
	if (NULL == role || old_status == cur_status) return;	// 如果新的状态和旧的状态一样，则认为状态没有改变

	role->NotifyMyStatusToOthers();
}

void EventHandler::OnRoleStartFight(Role* role, int battle_mode)
{
	if (NULL == role)
	{
		return;
	}

	role->GetRoleModuleManager()->GetEquipmentManager()->OnRoleStartFight(battle_mode);
	role->GetRoleModuleManager()->GetPet()->RecordBattlePetIndex();	
	role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnRoleStartFight();

	if (BATTLE_MODE_PLATFORM_BATTLE == battle_mode)
	{
		RoleActivityGodToken * role_activity = static_cast<RoleActivityGodToken * >
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != role_activity)
		{
			role_activity->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_PLATFORM_BATTLE);
		}		
		RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
		if (NULL != yueka_activity)
		{
			yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_PLATFORM_BATTLE_PAR, 1);
		}

	}
	else if(BATTLE_MODE_WORLD_BOSS == battle_mode)
	{
		this->OnParticipateLimitActivity(role, ACTIVITY_TYPE_WORLD_BOSS, __FUNCTION__);
	}
	else if (BATTLE_MODE_WORLD_BOSS_2 == battle_mode)
	{
		this->OnParticipateLimitActivity(role, ACTIVITY_TYPE_WORLD_BOSS_2, __FUNCTION__);
	}
	else if (BATTLE_MODE_WORLD_BOSS_3 == battle_mode)
	{
		this->OnParticipateLimitActivity(role, ACTIVITY_TYPE_WORLD_BOSS_3, __FUNCTION__);
	}
	else if(BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI == battle_mode)
	{
		RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
		if (NULL != yueka_activity)
		{
			yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_HUAN_JIE_ZHAN_CHANG_PAR, 1);
		}
	}

	Team * team = role->GetMyTeam();
	if(NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		if (battle_mode == BATTLE_MODE_DEMON_CAVE ||
			battle_mode == BATTLE_MODE_ZHEN_ZHONG ||
			battle_mode == BATTLE_MODE_RA_MIJINGQIWEN_2)	//万妖谷,三界，万灵队伍类型进入战斗后,队伍要移除匹配列表
		{
			TeamMatch::Instance().OnTeamStopMatch(team);

			if (CrossConfig::Instance().IsHiddenServer()) // 进入万妖谷战斗的队伍，可以从队伍列表中去除
			{
				TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE);
			}
		}
	}

	int &pet_join_fight_times = role->GetRoleModuleManager()->GetCommonData().pet_join_fight_times;
	pet_join_fight_times += 1;
	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_5);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_5; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_5)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 <= pet_join_fight_times)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnRoleFinishFight(Role* role, int battle_mode, bool is_win, bool is_flyaway, BattleData_Business& business_data, BattleRoleList battle_role_list)
{
	if (NULL == role)
	{
		return;
	}
	
	role->GetRoleModuleManager()->GetEquipmentManager()->OnRoleFinishFight();
	role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnRoleFinishFight();
	
	if (BATTLE_MODE_PET_MONOPOLY == battle_mode)
	{
		role->GetRoleModuleManager()->GetPetMonopoly()->OnBattleFinish();
	}
	else if (BATTLE_MODE_INITIAL_FIGHT == battle_mode)
	{
		role->SetBattleAutoAfterInitialFight();
	}
	else if (BATTLE_MODE_SPECIAL_ANECDOTE_TASK == battle_mode)
	{
		if(!is_flyaway && is_win)role->GetRoleModuleManager()->GetAnecdoteTask()->CheckSpecialTask(SPECIAL_ANECDOTE_TASK_CHALLENGE, business_data.param3);
	}

	// 角色完成某个模式的战斗， 该接口可用于活跃系统等只需要知道战斗模式不需要详细战斗数据的系统调用
	if (!is_flyaway)
	{
		role->GetRoleModuleManager()->GetEscort()->OnRoleFinishFight(is_win);
	}	

	if (is_win && !is_flyaway)
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_WIN_FIGHT_TIMES_ACC, 0, 1);
		int activity_type = this->GetActivityTypeByBattleMode(battle_mode);
		if (ACTIVITY_TYPE_INVALID != activity_type)
		{
			role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACTIVITY_FIGHT_WIN, activity_type, 1);
		}
		//role->GetRoleModuleManager()->GetSalary()->OnRoleWinBattle(battle_mode, business_data);

		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
		if (NULL != ramgr)
		{
			if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_EXPEL_MONSTER))
			{
				RandAcitivityExpelMonster * randactivity_info = ramgr->GetRAExpelMonster();
				if (randactivity_info != NULL && battle_mode == BATTLE_MODE_EXPEL_MONSTER)
				{
					randactivity_info->OnFightFinish(role);
				}
			}

			if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MONPOLY_GIFT))
			{
				RoleActivity * role_activit = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_MONPOLY_GIFT);
				if (NULL != role_activit)
				{
					RoleActivityMonpolyGift * monpolygift_ractivity = (RoleActivityMonpolyGift *)role_activit;
					monpolygift_ractivity->OnBattleFinish(is_win, battle_mode);
				}
			}

			if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MAZE_SUMMER))
			{
				RoleActivity * role_activit = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_MAZE_SUMMER);
				if (NULL != role_activit)
				{
					RoleActivityMazeSummer * maze_summer_ractivity = (RoleActivityMazeSummer *)role_activit;
					maze_summer_ractivity->OnBattleFinish(is_win, battle_mode);
				}
			}
		}	
	}

	if (is_win && !is_flyaway && BATTLE_MODE_SCHOOL_EXERCISE == battle_mode)
	{
		int exercise_seq = business_data.param3;
		role->GetRoleModuleManager()->GetSchoolExercise()->OnChallengeSucc(exercise_seq);
	}

	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		for (int i = 0; i < battle_role_list.role_count && i < SIDE_MAX_ROLE_NUM; i++)
		{
			if (role->GetUID() == battle_role_list.role_id_list[i]) continue;

			if (NULL != guild->GetMemberManager()->GetMember(battle_role_list.role_id_list[i]))
			{
				role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GUILD_TEAM_FIGHT, 0, 1);
				break;
			}
		}
	}


	Team * team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		if (battle_mode == BATTLE_MODE_DEMON_CAVE)
		{
			if (CrossConfig::Instance().IsHiddenServer()) // 进入万妖谷战斗的队伍，结束了战斗后，重新加入到队伍列表
			{
				TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_ADD);
			}
		}
	}
	
	role->GetRoleModuleManager()->GetElementElf()
		->OnFightFinish(is_win, is_flyaway, business_data);
}

void EventHandler::OnRoleFinishFightDetail(Role* role, int battle_mode, bool is_win, bool is_flyaway, BattleData_Business* business_data, BattleData_StatisticRole* role_statistic_data, 
	BattleData_Statistic* statistic_data, int pet_stat_num, BattleData_StatisticRole* pet_stat_list, long long * pet_unique_id_list)
{
	// 一些简单的玩法模式放在这里
	if (NULL == role || NULL == business_data) return;
	
	role->GetRoleModuleManager()->GetSmartMountsSystem()->OnFinishBattle(battle_mode, pet_stat_num, pet_unique_id_list);
	switch (battle_mode)
	{
	case BATTLE_MODE_KE_JU_EXAM:
		{
			role->GetRoleModuleManager()->GetKeJuExam()->OnBattleFinish(is_win);
		}
		break;
	case BATTLE_MODE_LANTERN_RIDDIR:
		{
			role->GetRoleModuleManager()->GetLanternRiddie()->OnBattleFinish(is_win);
		}
		break;
	case BATTLE_MODE_YAO_XING_SHI_LIAN:
		{
			if (is_win && !is_flyaway)
			{
				role->GetRoleModuleManager()->GetYaoXingShiLian()->OnBattleFinishWin(is_win);
			}
		}
		break;
	case BATTLE_MODE_FALLEN_GOD:
		{
			if (is_win)role->GetRoleModuleManager()->GetFallenGod()->OnBattleFinish(business_data, statistic_data->round_num);
		}
		break;
	case BATTLE_MODE_PARTNER_PVE:
		{
			if (is_win && !is_flyaway)
			{
				role->GetRoleModuleManager()->GetMirageTerritory()->OnBattleFinishWin(business_data, statistic_data->round_num);
			}
		}
		break;
	case BATTLE_MODE_MO_ZU_ZAI_NA_LI:
		{
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
			if (NULL != ramgr)
			{
				if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI))
				{					
					RandAcitivityMoZuZaiNaLi * randactivity_info = (RandAcitivityMoZuZaiNaLi *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI);
					if (randactivity_info != NULL && battle_mode == BATTLE_MODE_MO_ZU_ZAI_NA_LI)
					{
						randactivity_info->OnFightFinish(role, is_win);
					}
				}
			}

			if (is_win && !is_flyaway)
			{
				RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
				if (NULL != role_activity)
				{
					role_activity->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_MOZUZAINALI);
				}
				{
					RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
						(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
					if (NULL != role_activity)
					{
						role_activity->AddTaskParam(RAColorfulDouble::TSAK_TYPE_MO_ZU_ZAI_NA_LI, 1);
					}
				}
				{
					RoleActivityYunZeCutePet * role_activity = static_cast<RoleActivityYunZeCutePet *>
						(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
					if (NULL != role_activity)
					{
						role_activity->AddTaskParam(RAYZCutePet::TASK_TYPE_MO_ZU_ZAI_NA_LI);
					}
				}
			}
		}
		break;
	case BATTLE_MODE_ZAI_ZHAN_YUN_ZE:
		{
			if (is_win && !is_flyaway)
			{
				RoleActivityZaiZhanYunZe * role_activity = static_cast<RoleActivityZaiZhanYunZe *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE));
				if (NULL != role_activity)
				{
					role_activity->OnBarrleFinish(business_data->param2);
				}

				{
					RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
						(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
					if (NULL != role_activity)
					{
						role_activity->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_ZAIZHANYUNZE);
					}
				}

				{
					RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
						(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
					if (NULL != role_activity)
					{
						role_activity->AddTaskParam(RAColorfulDouble::TSAK_TYPE_ZAI_ZHAN_YUN_ZE, 1);
					}
				}
				{
					RoleActivityYunZeCutePet * role_activity = static_cast<RoleActivityYunZeCutePet *>
						(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
					if (NULL != role_activity)
					{
						role_activity->AddTaskParam(RAYZCutePet::TASK_TYPE_YZ_ZAI_ZHAN);
					}
				}
			}
		}
		break;
	
	case BATTLE_MODE_ENDLESS_TOWER:
		{
			{
				// 职业晋级
				role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_WU_JIN_ZHI_TA, business_data->param2, 0);
			}

			if (is_win)role->GetRoleModuleManager()->GetEndlessTower()->OnBattleFinishWin(business_data, statistic_data->round_num);
		}
		break;
	case BATTLE_MODE_DU_JIE:
		{
		}
		break;
	case BATTLE_MODE_BIG_DIPPER:
		{
			role->GetRoleModuleManager()->GetBigDipper()->OnBattleFinish(business_data, statistic_data->round_num);

			RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
			if (NULL != back_reward)
			{
				back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_BIG_DIPPER, 1);
			}

			{
				RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
				if (NULL != nnyy_activity)
				{
					nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_BEI_DOU_QI_XING);
				}
			}

			{
				RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_BEI_DOU_QI_XING);
				}
			}

			{
				RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_BEI_DOU_QI_XING);
				}
			}

			{
				RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_BEI_DOU_QI_XING);
				}
			}

			{
				RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_BEI_DOU_QI_XING);
				}
			}

		}
		break;
	case BATTLE_MODE_NETHER_WORLD_PURGATORY:
		{
			if (is_win && !is_flyaway)
			{
				role->GetRoleModuleManager()->GetNetherWorldPurgatory()->OnBattleFinishWin(business_data, statistic_data->round_num);
			}
			else if (!is_win)
			{
				role->GetRoleModuleManager()->GetNetherWorldPurgatory()->OnBattleFinishLose(business_data);
			}
		}
		break;
	case BATTLE_MODE_LUN_HUI_WANG_CHUAN:
		{
			if (is_win && !is_flyaway)
			{
				role->GetRoleModuleManager()->GetLunHuiWangChuan()->OnBattleFinishWin(is_win);
				this->OnPassLunHuiWangChuan(role, business_data->param1);
			}
			role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_LUNHUIWANGCHUAN, 1, __FUNCTION__);
		}
		break;

	case BATTLE_MODE_CLOUD_ARENA:
		{
			{
				RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
				if (NULL != nnyy_activity)
				{
					nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_QIONG_DING_ZHI_ZHENG);
				}
			}

			{
				RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_QIONG_DING_ZHI_ZHENG);
				}
			}

			{
				RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_QIONG_DING_ZHI_ZHENG);
		}
			}

			{
				RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_QIONG_DING_ZHI_ZHENG);
		}
			}

			{
				RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_QIONG_DING_ZHI_ZHENG);
				}
			}

		}
		break;

	case BATTLE_MODE_DEMO:
		{
			// demo好像没有什么需要触发的，不过也可以当做测试
#ifdef _DEBUG

#endif
		}
	case BATTLE_MODE_MAZE_SUMMER:
		{

		}
		break;
	case BATTLE_MODE_MI_YI:
		{
			if (is_win)
			{
				role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_MIYI, 1, __FUNCTION__);

			}			

			{
				RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
				if (NULL != nnyy_activity)
				{
					nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_YUAN_ZHENG_MO_ZHONG);
		}
			}

			{
				RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_YUAN_ZHENG_MO_ZHONG);
				}
			}

			{
				RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_YUAN_ZHENG_MO_ZHONG);
		}
			}

			{
				RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_YUAN_ZHENG_MO_ZHONG);
		}
			}

			{
				RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_YUAN_ZHENG_MO_ZHONG);
				}
			}
		}
		break;
	case BATTLE_MODE_PLATFORM_BATTLE:
		{
			PlatformBattle::Instance().OnBattleFinish(business_data->param1, business_data->param2, business_data->param3, *business_data, is_win);

			{
				RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
				if (NULL != nnyy_activity)
				{
					nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_WU_FANG_LEI_TAI);
				}
			}

			{
				RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_WU_FANG_LEI_TAI);
				}
			}

			if (is_win && business_data->param2 == role->GetUID())
			{
				RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
				if (NULL != yueka_activity)
				{
					yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_PLATFORM_BATTLE_WIN, 1);
				}

			}

			{
				RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_WU_FANG_LEI_TAI);

				}
			}

			{
				RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_WU_FANG_LEI_TAI);
		}
			}

			{
				RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_WU_FANG_LEI_TAI);
				}
			}
		}
		break;
	case BATTLE_MODE_MARRIAGE_FB:
		{
			if (!is_flyaway)
			{
				role->GetRoleModuleManager()->GetMarriage()->SendMarriageFbReward(business_data->deamon_cave_beat_waves);
				role->GetRoleModuleManager()->GetMonsterWave()->SendInfo();
				role->GetRoleModuleManager()->GetSalary()->OnRolePlayDemonCave(business_data->deamon_cave_beat_waves);
			}
		}
		break;
	case BATTLE_MODE_DEMON_CAVE:
		{
			role->GetRoleModuleManager()->GetMonsterWave()->SendPassReward(business_data->deamon_cave_beat_waves);
			role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_DEMON_CAVE, 1, __FUNCTION__);
			
			role->GetRoleModuleManager()->GetSalary()->OnRolePlayDemonCave(business_data->deamon_cave_beat_waves);
			RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
			if (NULL != lucky_turn_table)
			{
				lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_DEMON_CAVE, 1);
			}
		}
		break;

	case BATTLE_MODE_NORMAL_MONSTER:
		{
			
		}
		break;
	case BATTLE_MODE_TASK_ROLE_MONSTER:
	{
		int task_id = business_data->param1;
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
		if (task_cfg && task_cfg->task_condition == TASK_NEED_ROLE_BEAT_MONGROUP)
		{
			role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(task_id, 1);
		}		
	}
	break;

	case BATTLE_MODE_FIELD_BOSS:
		{
			this->OnRoleFightWildBoss(role, is_flyaway, is_win);

			{
				RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
				if (NULL != nnyy_activity)
				{
					nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_YE_WAI_SHOU_LING);
		}
			}

			{
				RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_YE_WAI_SHOU_LING);
				}
			}

			{
				RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_YE_WAI_SHOU_LING);
		}
			}

			{
				RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_YE_WAI_SHOU_LING);
		}
			}

			{
				RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_YE_WAI_SHOU_LING);
				}
			}

		}
		break;

	case BATTLE_MODE_ZHEN_ZHONG:
		{
			if (is_win && !is_flyaway)
			{
				role->GetRoleModuleManager()->GetResearchTask()->OnDefeatMonsterGroup();
				role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACCOMPLISH_ACTIVITY, COG_ACT_TYPE_ZHENZHO_TASK, 1);
				role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG, 1, __FUNCTION__);
				role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_ZHEN_ZHONG, 1);

				role->GetRoleModuleManager()->GetTaskManager()->OnFinsihResearchTask(1);

				RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
				if (NULL != lucky_turn_table)
				{
					lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_ZHEN_ZHONG, 1);
				}

				RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
				if (NULL != god_token)
				{
					god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_RESEARCH_TASK);
				}
			}
		}
		break;

	case BATTLE_MODE_COURAGE_CHALLENGE:
		{
			if (is_win)
			{
				role->GetRoleModuleManager()->GetCourageChallenge()->SendPassReward(business_data->monster_group_id, business_data->condition_flag);

				//WorldStatus::Instance().SetCourageChallengeFirstKill(business_data->courage_challenge_level, role);				
			}
			this->OnEnterCourageChallenge(role);

			//锢魔之塔调整, 首杀榜暂时屏蔽
		/*	int uid = role->GetUID();
			switch (business_data->courage_challenge_level)
			{
			case 1:
				this->OnClearanceFB(HONOR_ROLL_TYPE_COURAGE_CHALLENGE_1, 1, &uid, statistic_data->round_num, EngineAdapter::Instance().Time());
				break;
			case 2:
				this->OnClearanceFB(HONOR_ROLL_TYPE_COURAGE_CHALLENGE_2, 1, &uid, statistic_data->round_num, EngineAdapter::Instance().Time());
				break;
			case 3:
				this->OnClearanceFB(HONOR_ROLL_TYPE_COURAGE_CHALLENGE_3, 1, &uid, statistic_data->round_num, EngineAdapter::Instance().Time());
				break;
			case 4:
				this->OnClearanceFB(HONOR_ROLL_TYPE_COURAGE_CHALLENGE_4, 1, &uid, statistic_data->round_num, EngineAdapter::Instance().Time());
				break;
			case 5:
				this->OnClearanceFB(HONOR_ROLL_TYPE_COURAGE_CHALLENGE_5, 1, &uid, statistic_data->round_num, EngineAdapter::Instance().Time());
				break;
			case 6:
				this->OnClearanceFB(HONOR_ROLL_TYPE_COURAGE_CHALLENGE_6, 1, &uid, statistic_data->round_num, EngineAdapter::Instance().Time());
				break;
			}
		*/
		}
		break;

	case BATTLE_MODE_NEW_COURAGE_CHALLENGE:
		{
			if (is_win)
			{
				role->GetRoleModuleManager()->GetNewCourageChallenge()->SendPassReward(business_data->monster_group_id, business_data->condition_flag);			
			}
		}
		break;

	case BATTLE_MODE_GUILD_BOSS:
		{
			Guild* guild = GuildManager::Instance().GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				long long partner_damage = statistic_data->total_attacker_partner_damage_to_enemy;
				long long total_damage_without_partner = statistic_data->attacker_total_damage;
				long long pet_help_total_damage = statistic_data->total_attacker_pet_help_damage_to_enemy;

				{
					Team* team = role->GetMyTeam();
					if (NULL != team && team->GetLeaderUID() != role->GetUID())
					{
						partner_damage = 0;
						pet_help_total_damage = 0;
					}
				}

				guild->OnFightGuildBoss(role, business_data->guild_boss_chapter, business_data->guild_boss_seq, business_data->guild_boss_monster_remain_hp_percent_list, is_win, total_damage_without_partner + partner_damage + pet_help_total_damage);
			}
		}
		break;
	case BATTLE_MODE_TREASURE_MAP:
		{
			if (is_win && role->GetUID() == business_data->param1)
			{
				role->GetRoleModuleManager()->GetTreasureMap()->OnFightWin();
			}
			this->OnFightTreasureMonster(role, is_win);
		}
		break;
	case BATTLE_MODE_TASK_CHAIN:
		{
			if (is_win && role->GetUID() == business_data->param1)
			{
				role->GetRoleModuleManager()->GetTaskChain()->OnKillMonster();
			}
		}
		break;

	case BATTLE_MODE_SINGLE_ARENA:
		{
			ColosseumManager::Instance().SysReport(role, business_data->colosseum_target_uid, is_win, business_data, role_statistic_data, statistic_data, pet_stat_num, pet_stat_list);
		
			{
				RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
				if (NULL != nnyy_activity)
				{
					nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_JING_JI_CHANG);
				}
			}

			{
				RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_JING_JI_CHANG);
				}
			}
			if (is_win)
			{
				RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
				if (NULL != yueka_activity)
				{
					yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_WIN, 1);
				}

			}

			{
				RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_JING_JI_CHANG);

				}
			}

			{
				RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_JING_JI_CHANG);
				}
			}

			{
				RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
				if (NULL != role_activity)
				{
					role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_JING_JI_CHANG);
				}
			}

		}		
		break;

	case BATTLE_MODE_WORLD_BOSS:
		{
			this->OnRoleFightWorldBoss(role, is_flyaway, is_win);	
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_2:
		{
			this->OnRoleFightWorldBoss2(role, is_flyaway, is_win);
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_3:
		{
			this->OnRoleFightWorldBoss3(role, is_flyaway, is_win);
		}
		break;

	case BATTLE_MODE_COURSE_OF_GROWTH:
		{
			if (is_win) role->GetRoleModuleManager()->GetCourseOfGrowth()->OnDefeatCOGBoss();
		}
		break;
	case BATTLE_MODE_JIAN_DAN_QIN_XIN:
		{
			if (is_win)
			{
				RoleActivityJianDanQinXin * jian_dan_qin_xin = static_cast<RoleActivityJianDanQinXin *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN));
				if (NULL != jian_dan_qin_xin)
				{
					jian_dan_qin_xin->OnRAJianDanQinXinReward(business_data->monster_group_id);
				}
			}
		}
		break;
	case BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI:
		{
			if (is_win)
			{
				RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
				if (NULL != yueka_activity)
				{
					yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_HUAN_JIE_ZHAN_CHANG_WIN, 1);
				}
			}
		}
		break;
	case BATTLE_MODE_REALM_BREAK:
		{
			//role->GetRoleModuleManager()->GetRealmBreak()->OnBattleFinish(is_win);
		}	
		break;
	case BATTLE_MODE_SUMMER_LI_LIAN:
		{
			if (!is_flyaway)
			{
				RoleActivitySummerLiLian * summer_li_lian = static_cast<RoleActivitySummerLiLian *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN));
				if (NULL != summer_li_lian)
				{
					summer_li_lian->OnBattleFinish(business_data->param1, is_win);
				}
			}
		}
		break;
	case BATTLE_MODE_YIN_YANG_EYE:
		{
			role->GetRoleModuleManager()->GetYinYangEye()->OnBattleFinish(business_data->param2, business_data->param3, is_win);
		}
		break;
	case BATTLE_MODE_RA_MIJINGQIWEN:
		{
			if (is_win && !is_flyaway)
			{
				RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
				if (NULL != role_activity)
				{
					role_activity->OnCompleteTask(RAND_ACTIVITY_TYPE_ANECDOTE);
				}
			}
		}
		break;
	case BATTLE_MODE_RA_MIJINGQIWEN_2:
		{
			if (is_win && !is_flyaway)
			{
				RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
				if (NULL != role_activity)
				{
					role_activity->OnCompleteTask(RAND_ACTIVITY_TYPE_ANECDOTE_2);
				}
			}
		}
		break;
	case BATTLE_MODE_REALM_BREAK_FIGHT:
		{
			role->GetRoleModuleManager()->GetRealmBreak()->OnBattleFinish(is_win, business_data->param1);
		}
		break;
	case BATTLE_MODE_SHAN_HAI_ZHENG_FENG:
		{
			if (!is_flyaway)
			{
				RoleActivityShanHaiZhengFeng * summer_li_lian = static_cast<RoleActivityShanHaiZhengFeng *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG));
				if (NULL != summer_li_lian)
				{
					summer_li_lian->OnBattleFinish(business_data->param1, is_win);
				}
			}
		}
		break;
	}
	switch (battle_mode)
	{
		case BATTLE_MODE_FIELD_BOSS:
		case BATTLE_MODE_WORLD_BOSS:
		case BATTLE_MODE_GUILD_BOSS:
		case BATTLE_MODE_COURSE_OF_GROWTH:
		case BATTLE_MODE_WORLD_BOSS_2:
			{
				if(is_win && !is_flyaway)
				{ 
				RoleActivityTrunTable * trun_table = static_cast<RoleActivityTrunTable *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TRUN_TABLE));
				if (NULL != trun_table)
				{
					trun_table->OnRaTurnTableAddKillBossNum();
				}
				}
			}
		break;
	}
}

void EventHandler::OnRoleRunAway(Role* role, bool is_leave_team, int monster_group_id, int battle_mode)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		team->OnRoleRunAwayFromBattle(role, is_leave_team);
	}

	if (NULL == role->GetRoleModuleManager())
	{
#ifdef _DEBUG
		assert(0);
#endif 
		//外网出现了诡异的bug, rolemodulemanager为空了，可能是之前某个地方切了地图
		role = World::GetInstWorld()->GetSceneManager()->GetRole(role->GetUserId());
		if (NULL == role || NULL == role->GetRoleModuleManager())
		{
			return;
		}
	}

	if (monster_group_id > 0)
	{
		role->GetRoleModuleManager()->GetAnecdoteTask()->OnBattleFail(monster_group_id);
	}

	if (BATTLE_MODE_PET_MONOPOLY == battle_mode)
	{
		role->GetRoleModuleManager()->GetPetMonopoly()->OnBattleFinish();
	}
}

void EventHandler::OnRobotRunAway(Robot* robot, bool is_leave_team)
{
	if (NULL == robot) return;

	Team* team = robot->GetTeam();
	if (NULL != team)
	{
		team->OnRobotRunAwayFromBattle(robot, is_leave_team);
	}
}

void EventHandler::OnRoleForceFinishBattle(Role* role, int monster_group_id, int battle_mode)
{
	if (NULL == role) return;

	if (BATTLE_MODE_PET_MONOPOLY == battle_mode)
	{
		role->GetRoleModuleManager()->GetPetMonopoly()->OnBattleFinish();
	}
}

int EventHandler::GetActivityTypeByBattleMode(int battle_mode)
{
	UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX == 19);	//新增需要加,否则成就收集不到
	int activity_type = ACTIVITY_TYPE_INVALID;
	switch (battle_mode)
	{
		case BATTLE_MODE_QING_YUAN_SHENG_HUI:
		{
			activity_type = ACTIVITY_TYPE_QING_YUAN_SHENG_HUI;
		}
		break;
		case BATTLE_MODE_WORLD_BOSS:
		{
			activity_type = ACTIVITY_TYPE_WORLD_BOSS;
		}
		break;
		case BATTLE_MODE_SQUARD_FIGHT:
		{
			activity_type = ACTIVITY_TYPE_TEAM_FIGHT;
		}
		break;
		case BATTLE_MODE_MAZE_RACER:
		{
			activity_type = ACTIVITY_TYPE_MAZE_RACER;
		}
		break;
		case BATTLE_MODE_BRAVE_GROUND:
		{
			activity_type = ACTIVITY_TYPE_BRAVE_GROUND;
		}
		break;
		case BATTLE_MODE_GUILD_FIGHT:
		{
			activity_type = ACTIVITY_TYPE_GUILD_FIGHT;
		}
		break;
		case BATTLE_MODE_CHIEF_ELECTION:
		case BATTLE_MODE_CHIEF_ELECTION_SEA:
		{
			activity_type = ACTIVITY_TYPE_CHIEF_ELECTION;
		}
		break;
		case BATTLE_MODE_CHIEF_PEAK_BATTLE:
		{
			activity_type = ACTIVITY_TYPE_CHIEF_PEAK_BATTLE;
		}
		break;
		case BATTLE_MODE_GUILD_HONOR_BATTLE:
		{
			activity_type = ACTIVITY_TYPE_GUILD_HONOR_BATTLE;
		}
		break;
		case BATTLE_MODE_WORLD_BOSS_2:
		{
			activity_type = ACTIVITY_TYPE_WORLD_BOSS_2;
		}
		break;
		case BATTLE_MODE_ONLY_FIGHT:
		{
			activity_type = ACTIVITY_TYPE_ONLY_FIGHT;
		}
		break;
		case BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI:
		{
			activity_type = ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG;
		}
		break;
		case BATTLE_MODE_HUNDRED_GHOST:
		{
			activity_type = ACTIVITY_TYPE_HUNDRED_GHOST;
		}
		break;
		case BATTLE_MODE_WORLD_TEAM_ARENA:
		{
			activity_type = ACTIVITY_TYPE_WORLD_TEAM_ARENA;
		}
		break;
		case BATTLE_MODE_WORLD_BOSS_3:
		{
			activity_type = ACTIVITY_TYPE_WORLD_BOSS_3;
		}
		break;
		default:
		break;
	}
	return activity_type;
}

void EventHandler::OnRoleFightWorldBoss(Role* role, bool is_flyaway, bool is_win)
{
	if (NULL == role) return;

	if (!is_flyaway && is_win)
	{
		//世界boss 击杀奖励
		WorldBossRankRewardGroupCfg  kill_reward_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossRankRewardCfg(role->GetLevel(), WORLD_BOSS_REWARD_TYPE_KILL);
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = kill_reward_cfg.bind_coin;
		for (int i = 0; i < (int)kill_reward_cfg.rewards.size() && i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			const ItemConfigData & reward = kill_reward_cfg.rewards[i];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);

			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = reward.item_id;
				contentparam.item_list[i].num = reward.num;
				contentparam.item_list[i].is_bind = (reward.is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_boss_kill_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_boss_kill_reward);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(role->GetUID(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void EventHandler::OnRoleFightWorldBoss2(Role * role, bool is_flyaway, bool is_win)
{
	if (NULL == role) return;

	if (!is_flyaway && is_win)
	{
		//世界boss 击杀奖励
		WorldBossRankRewardGroupCfg  kill_reward_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBoss2RankRewardCfg(role->GetLevel(), WORLD_BOSS_2_REWARD_TYPE_KILL);
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = kill_reward_cfg.bind_coin;
		for (int i = 0; i < (int)kill_reward_cfg.rewards.size() && i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			const ItemConfigData & reward = kill_reward_cfg.rewards[i];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);

			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = reward.item_id;
				contentparam.item_list[i].num = reward.num;
				contentparam.item_list[i].is_bind = (reward.is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_boss_2_kill_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_boss_2_kill_reward);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(role->GetUID(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void EventHandler::OnRoleFightWorldBoss3(Role * role, bool is_flyaway, bool is_win)
{
	if (NULL == role) return;

	if (!is_flyaway && is_win)
	{
		//世界boss 击杀奖励
		WorldBoss3RankRewardGroupCfg  kill_reward_cfg = LOGIC_CONFIG->GetWorldBoss3Config()->GetWorldBossRankRewardCfg(role->GetLevel(), WORLD_BOSS_3_REWARD_TYPE_KILL);
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = kill_reward_cfg.bind_coin;
		for (int i = 0; i < (int)kill_reward_cfg.rewards.size() && i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			const ItemConfigData & reward = kill_reward_cfg.rewards[i];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);

			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = reward.item_id;
				contentparam.item_list[i].num = reward.num;
				contentparam.item_list[i].is_bind = (reward.is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_boss_3_kill_subject, ACTIVITY_TYPE_WORLD_BOSS_3);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_boss_3_kill_reward, ACTIVITY_TYPE_WORLD_BOSS_3);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(role->GetUID(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void EventHandler::OnRoleFightWildBoss(Role * role, bool is_flyaway, bool is_win)
{
	if (NULL == role) return;

	if (!is_flyaway && is_win)
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_DEFEAT_BOSS_MONSTER, COG_BOSS_WILD, 1);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_KILL_BOSS, 1);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_KILL_YW_BOSS, 1);
		role->GetRoleModuleManager()->GetTradeCredit()->OnKillFieldBoss();
		role->GetRoleModuleManager()->GetTaskManager()->OnRoleFightWildBoss();

		RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != god_token)
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_WILD_BOSS);
			god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_WILD_BOSS);
		}

		RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
		if (NULL != fortune_maze)
		{
			fortune_maze->KillWildBoos();
		}

		//竞技月卡
		RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
		if (NULL != yueka_activity)
		{
			yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_KILL_WILD_BOSS, 1);
		}

		Guild * guild = role->GetGuild();
		if (NULL != guild)
		{
			guild->OnGuildWeekTaskEvent(role->GetUserId(), GWT_COND_KILL_WILD_BOSSNUM, 0);
		}

		role->GetRoleModuleManager()->GetSalary()->OnRoleKillWildBoss();
	}
}

void EventHandler::OnRoleUpLevelEvent(Role * role, int old_level, int cur_level)
{
	if (NULL == role || old_level == cur_level) return;
	//if (old_level > cur_level) std::swap(old_level, cur_level);

	role->GetRoleModuleManager()->GetTaskManager()->OnRoleLevelUp();
	role->GetRoleModuleManager()->GetTaskChain()->OnRoleLevelUp();
	role->GetRoleModuleManager()->GetPet()->OnRoleLevelUp(cur_level);
	GuildManager::Instance().OnRoleLevelUp(role, cur_level);
	role->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_SELF, __FUNCTION__); // 有些属性要按等级算
	role->GetRoleModuleManager()->GetShiTu()->OnRoleUpLevel(old_level, cur_level);
	role->GetRoleModuleManager()->GetRoleAttributePlan()->OnRoleLevelUp(cur_level - old_level, old_level);
	role->GetRoleModuleManager()->GetSchoolTask()->OnLevelUp(cur_level);
	role->GetRoleModuleManager()->GetTreasureMap()->OnRoleUpLevel(old_level, cur_level);
	role->GetRoleModuleManager()->GetPartner()->ReCalAllPartnerAttr(Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_ROLE_UP_LEVEL);
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnRoleLevelUp(old_level, cur_level);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACHS_APPOINTED_LV, 0, 0);
	role->GetRoleModuleManager()->GetKnapsack()->OnLevelUp(cur_level);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ROLE_LEVEL,cur_level);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnOpenTrial();
	role->GetRoleModuleManager()->GetBountyTask()->OnLevelUp();
	role->GetRoleModuleManager()->GetFuncPreview()->OnLevelUp();
	role->GetRoleModuleManager()->GetTransactionTask()->RoleLevelUp(cur_level);
	role->GetRoleModuleManager()->GetActiveSystem()->RoleLevelUp(cur_level);
	role->GetRoleModuleManager()->GetTradeCredit()->OnRoleLevelUp(old_level, cur_level);
	role->GetRoleModuleManager()->GetRealmBreak()->OnRoleLevelUp();
	role->GetRoleModuleManager()->GetNetherWorldPurgatory()->OnRoleUplevel();
	RankManager::Instance().SyncPersonRankInfo(role , PERSON_RANK_TYPE_LEVEL);
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_LEVEL + role->GetAppearanceProf());
	role->GetRoleModuleManager()->GetRoleNewLevelComplement()->OnRoleLevelUp();
	role->GetRoleModuleManager()->GetAnecdoteTask()->OnRoleLevelUp();
	role->GetRoleModuleManager()->GetFlyUpSystem()->RoleUpLevel();
	role->GetRoleModuleManager()->GetMapUnlock()->OnRoleLevelUp();
	role->GetRoleModuleManager()->GetDayReward()->OnRoleLevelUp(cur_level);

	ColosseumManager::Instance().OnLevelUp(role, cur_level);
	PlatformBattle::Instance().SynRoleInfo(role);
	this->OnWorldTeamArenaChangeByRole(role);

	{
		RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
		if (NULL != role_activity)
		{
			role_activity->LevelUpToFirstTask(cur_level);
		}
	}

	{
		RoleActivityTigerFishEveryYear * role_activity = static_cast<RoleActivityTigerFishEveryYear *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
		if (NULL != role_activity)
		{
			if (cur_level == 32)
			{
				role_activity->UpdateLevel();
			}
		}
	}

	Team *team = role->GetMyTeam();
	if (NULL != team)
	{
		team->UserLevelUp(role);
	}

	if (cur_level >= 40)
	{
		gamelog::g_log_score_task.printf(LL_INFO, "%s plat_name[%s] role[%d, %s] level[%d] score[%d]", __FUNCTION__, role->GetPlatName(),
			role->GetRoleModuleManager()->GetUid(), role->GetRoleModuleManager()->GetRoleName(),cur_level, role->GetRoleModuleManager()->GetCapability()->GetRoleCapability());

		if (CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SyncLogToOriginServer(role->GetUserId().db_index, SYNC_LOG_TYPE_SCORE_TASK, "%s plat_name[%s] role[%d, %s] level[%d] score[%d]",
				__FUNCTION__, role->GetPlatName(), role->GetRoleModuleManager()->GetUid(), role->GetRoleModuleManager()->GetRoleName(), 
				cur_level, role->GetRoleModuleManager()->GetCapability()->GetRoleCapability());
		}
	}

	WorldArena::Instance().UpdateUserSignupInfo(role);
	OfflineBattleDataMgr::Instance().OnRoleLevelUp(role);
}

void EventHandler::OnRoleProfProEvent(Role * role, int old_prof, int cur_prof)
{
	if (NULL == role) return;
	
	//转职返回铜币与重置技能要放在转职后给予技能前
	if (old_prof / PROFESSION_BASE != cur_prof / PROFESSION_BASE)
	{
		role->GetRoleModuleManager()->ChangeProfSendItem(old_prof, cur_prof);
		role->GetRoleModuleManager()->GetRoleAttributePlan()->OnRoleChangeProf();
		role->GetRoleModuleManager()->GetHuaShenManager()->OnRoleChangeProf();
		role->GetRoleModuleManager()->GetBountyTask()->OnRoleChangeProf();
		role->GetRoleModuleManager()->GetTrain()->OnRoleChangeProf();
	}
	if (old_prof != cur_prof && (cur_prof % PROFESSION_BASE == 1))//转职，且为第一阶
	{
		//role->GetRoleModuleManager()->GetRoleAttributePlan()->OnRoleFirstTransfer();(目前创建角色就是第一阶段，策划说屏蔽）
	}
	role->GetRoleModuleManager()->GetTaskManager()->OnProfPromote();
	role->GetRoleModuleManager()->GetTitle()->OnRoleChangeProf(old_prof, cur_prof);
	role->GetRoleModuleManager()->GetRoleSkill()->OnChangeProfession(old_prof, cur_prof);
	role->GetRoleModuleManager()->GetEquipmentManager()->OnChangeProfession();
	role->GetRoleModuleManager()->GetKnapsack()->OnChangeProf();
	role->GetRoleModuleManager()->MakeAppearanceData();
	role->SendChangeAppearanceNotice();
	role->GetRoleModuleManager()->GetCollection()->OnChangeProfession(old_prof, cur_prof);
	role->GetRoleModuleManager()->GetRoleFormation()->OnChangeProf(old_prof, cur_prof);
	role->GetRoleModuleManager()->OnChangeProfession(old_prof,cur_prof);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PROF_PRO, cur_prof % PROFESSION_BASE);
	role->GetRoleModuleManager()->GetRoleCrystalFb()->OnRoleChangeProfOrEneryChange(true);
	RankManager::Instance().SyncPersonBaseInfo(role, old_prof, cur_prof);
	if (old_prof / PROFESSION_BASE != cur_prof / PROFESSION_BASE)
	{
		RankManager::Instance().SyncPersonRankInfo(role, cur_prof / PROFESSION_BASE + PERSON_RANK_TYPE_LEVEL);
	}
	EventHandler::Instance().OnRoleChangeAppearance(role, true);
	PlatformBattle::Instance().SynRoleInfo(role);

	Team *team = role->GetMyTeam();
	if (NULL != team)
	{
		team->SendTeamMemberBaseInfoChangeNotice(role);
	}

	WorldArena::Instance().UpdateUserSignupInfo(role);
}

void EventHandler::OnRoleUpTopLevelEvent(Role * role, int old_level, int cur_level)
{
	if (NULL == role || old_level == cur_level) return;

	role->UpdateCacheNode();
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_LEVEL);
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_LEVEL + role->GetAppearanceProf());
	this->OnWorldTeamArenaChangeByRole(role);
	Team *team = role->GetMyTeam();
	if (NULL != team)
	{
		team->UserLevelUp(role);
	}

	WorldArena::Instance().UpdateUserSignupInfo(role);
}

void EventHandler::OnRoleFlyUpEvent(Role* role, int fly_up_flag)
{	
	if (NULL == role || fly_up_flag <= 0) return;

	this->OnRoleChangeAppearance(role, true);
	if (fly_up_flag == FlyUpSystemParam::FLY_UP_STAG_1)
	{
		role->GetRoleModuleManager()->GetTaskManager()->OnRoleFlyUpFinish();
	}

	role->AddExp(0, ADD_EXP_TYPE_ROLE_FLY_UP_FINISH);
	role->GetRoleModuleManager()->GetPet()->CheckUnlockLianYaoFurance();
	WorldArena::Instance().UpdateUserSignupInfo(role);
}

void EventHandler::OnRoleRealmBreak(Role * role, int cur_level)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REALM_BREAK_LEVEL, cur_level, 0);
}

void EventHandler::OnRoleRealmBreakFight(Role * role, int fight_seq)
{
	if(NULL == role) return ;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REAML_BREAK_FIGHT, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(NSC_CON_REALM_BREAK_FIGHT, 1);
	role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_XIN_LING_ZHI_JING, fight_seq, 0);
	role->GetRoleModuleManager()->GetTaskManager()->OnRoleRealmBreakFight(fight_seq);

	RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL != lucky_turn_table)
	{
		lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_REALM_BREAK_SOUL_FIGHT, 1);
	}
}

void EventHandler::OnRolePutOnMedal(Role * role, int medal_num, int low_color)
{
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GET_MEDAL, medal_num, low_color);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_MEDAL, 0);

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PURPLE_TOKEN);
	}
}

void EventHandler::OnRolePutOffMedal(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GET_MEDAL, 1);

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PURPLE_TOKEN);
	}
}

void EventHandler::OnRoleMedalUpGrade(Role * role, int quality_color, int grade)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_MEDAL_UP_GRADE, 1, quality_color);
}

void EventHandler::OnYinYangEyeReward(Role * role, bool is_need_open_eye)
{
	if(NULL == role) return;
	
	if (is_need_open_eye)
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_YIN_YANG_EYE_REWARD, 0, 1);
	}
}

void EventHandler::OnJieYiRoleChnage(Role * role, int join_jie_yi_day)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_JIE_YI_DAY_NUM, 0, join_jie_yi_day);
}

void EventHandler::OnTimeInterval(Role * role, int delta_time)
{
	if(NULL == role || delta_time <= 0) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ONLINE_TIME, delta_time);
	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_ONLINE_TIME, delta_time);
	}
}

void EventHandler::OnDig(Role * role)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetRoleSkill()->OnDig();
}

void EventHandler::OnLumbering(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleSkill()->OnLumbering();
}

void EventHandler::OnPickFlower(Role * role)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetRoleSkill()->OnPickFlower();
}

void EventHandler::OnMakeEquipment(Role * role, ItemID equip_id, short num)
{
	if (NULL == role || num <= 0) return;
	const ItemBase * itembase = ITEMPOOL->GetItem(equip_id);
	if (NULL == itembase) return;

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));

	if (I_TYPE_EQUIPMENT == itembase->GetItemType())
	{
		role->GetRoleModuleManager()->GetRoleSkill()->OnMakeEquipment(equip_id, num);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_EQPM_FORG, num, equip_id);

		if (NULL != god_token) god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_MAKE_EQUIP, num);
	}
	else if (I_TYPE_JEWELRY == itembase->GetItemType())
	{
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_EQPM_FORG, num, equip_id);
		if (NULL != god_token) god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_MAKE_JEWELTY, num);
	}

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_WORKSHOP_CRAFT_ITEM, COG_WORKSHOP_MAKE_EQUIP, num);
	role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
}

void EventHandler::OnGemMounting(Role * role, int item_id)
{
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GEMS_MOSAIC, 0, item_id);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE, 0);
}

void EventHandler::OnResearchTask(Role * role, int num, bool is_find)
{
	if (NULL == role || num <= 0) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACCOMPLISH_ACTIVITY, COG_ACT_TYPE_ZHENZHO_TASK, num/*1*/);
	role->GetRoleModuleManager()->GetTaskManager()->OnFinsihResearchTask(num);
	if (!is_find)
	{
		role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG, num, __FUNCTION__);
		role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_ZHEN_ZHONG, num);

		RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
		if (NULL != lucky_turn_table)
		{
			lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_ZHEN_ZHONG, num);
		}
	}

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_RESEARCH_TASK);
	}
}

void EventHandler::OnAnswerQuestion(Role * role, bool is_correct, int num, bool is_find)
{
	if (NULL == role || num <= 0) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACCOMPLISH_ACTIVITY, COG_ACT_TYPE_WAN_SHI_TONG, num);
	if (!is_find)
	{
		role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_WST_QUESTION, num, __FUNCTION__);

		RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
		if (NULL != lucky_turn_table)
		{
			lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_WST_QUESTION, 1);
		}

		RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != god_token)
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_WST_QUESTION);
		}
	}
}

void EventHandler::OnMakeAWishOnce(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACCOMPLISH_ACTIVITY, COG_ACT_TYPE_WISHING_POOL, 1);
	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_TREVI_FOUNTAIN, 1, __FUNCTION__);
}

void EventHandler::OnEscortOneTime(Role * role, bool is_succ, int escort_seq)
{
	if (NULL == role) return;

	if (is_succ)
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACCOMPLISH_ACTIVITY, COG_ACT_TYPE_ESCORT_CARGO, 1);
		role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_ESCORT, escort_seq, 0);
		role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();

		RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != god_token)
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_ESCORT);
		}
	}
}

void EventHandler::OnEscortDayActivity(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_ESCORT, 1, __FUNCTION__);

	RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL != lucky_turn_table)
	{
		lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_ESCORT, 1);
	}
}

void EventHandler::OnEscortStart(Role* role, int escort_seq)
{
	if (NULL == role) return;

	TeamMatch::Instance().OnUserStopMatch(role->GetUniqueUserID());

	role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_ACCEPT_ESCORT, escort_seq, 0);

	role->LogActive(LOG_ACTIVE_TYPE_ESCORT);
}

void EventHandler::OnFightTreasureMonster(Role * role, bool is_win)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
}

void EventHandler::OnGetTreasureMap(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_ARRESTING_THIEVES, 1, __FUNCTION__);

	RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL != lucky_turn_table)
	{
		lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPEARRESTING_THIEVES, 1);
	}
}

void EventHandler::OnBountyTaskCommit(Role * role, int num, bool is_find)
{
	if (NULL == role || num <= 0) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_BOUNTY_TASK_DONE, 0, num);
	role->GetRoleModuleManager()->GetTaskManager()->OnFinishBountyTask(num);
	if (!is_find)
	{
		role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_BOUNTY_TASK, num, __FUNCTION__);
		role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_BOUNTY_TASK, 1);


		RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
		if (NULL != lucky_turn_table)
		{
			lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_BOUNTY_TASK, num);
		}
	}
}

void EventHandler::OnBountyTaskFinish(Role * role)
{
	if(NULL == role) return;

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_BOUNTY_TASK, 1);
	}
	role->GetRoleModuleManager()->GetRoleHundredGodBoss()->OnFinishBounty();
	role->GetRoleModuleManager()->GetTaskManager()->OnFinishBountyToday();
}

void EventHandler::OnTransactionTask(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_TRANSACTION_TASK, 1, __FUNCTION__);

	RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL != lucky_turn_table)
	{
		lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_TRANSACTION_TASK, 1);
	}
	
	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_TRANSACTION_TASK);
	}
}

void EventHandler::OnTransactionTaskCommit(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnTransactionTask();
}

void EventHandler::OnFantasyFBReferReward(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_FANTASY_FB, 1, __FUNCTION__);
}

void EventHandler::OnMarry(Role * role, int marry_type)
{
	if (NULL == role) return;

	MarriageSeekingManager::Instance().OnMarry(role);
}

void EventHandler::OnDivorce(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetBaby()->OnDivorce();
}

void EventHandler::OnUseItem(Role * role, int item_id, int use_num)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetTaskManager()->OnUseProperty(item_id, use_num);
	role->GetRoleModuleManager()->GetTaskManager()->OnItemChange(item_id);

	Scene * scene = role->GetScene();
	if (NULL != scene)
	{
		scene->GetSpecialLogic()->OnUseItem(role, item_id, use_num);
	}

	const ItemBase * item_ib = ITEMPOOL->GetItem(item_id);
	if (NULL == item_ib) return;

	int item_type = item_ib->GetItemType();
	if (I_TYPE_EXPENSE == item_type)
	{
		NorexItem * norex = (NorexItem *)item_ib;
		if (NULL != norex)
		{
			role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_USE_NOREXITEM_TYPES, norex->GetUseType(), use_num);
		}
	}
	else if (I_TYPE_TREASURE_MAP == item_type)
	{
		TreasureMapItem * treasure_map = (TreasureMapItem *)item_ib;
		if (NULL != treasure_map)
		{
			role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_USE_HIGH_TREASURE, treasure_map->GetMapType(), use_num);
		}
	}
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_USE_APPOINTED_ITEMS, item_type, use_num);
}

void EventHandler::OnBuyItem(Role * role, int item_id, int buy_num)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetTaskManager()->OnBuyProperty(item_id, buy_num);

}

void EventHandler::OnGetItem(Role * role, int item_id, int get_num, int reason)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnItemChange(item_id);

	if (PUT_REASON_GATHER_OBJECT == reason)
	{
		this->OnGetItemByGather(role, item_id, get_num);
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return;
	}


	{
		//宠物徽章获取传闻
		const PetBadgeCfg * cfg = LOGIC_CONFIG->GetPetBadgeConfig()->GetPetBadgeCfg(item_id);
		if (cfg && cfg->is_broadcast)
		{
			// 传闻
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_get_pet_badge_broadcast, role->GetUID(), role->GetName(), item_id);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
			}
		}
	}


	switch (item_base->GetItemType())
	{
	case I_TYPE_PET_SOUL_EQUIP:
		{
			this->OnGetPetSoul(role, item_id, true);
		}
		break;
	default:
		break;
	}

}

void EventHandler::OnConsumeItem(Role* role, int item_id, int consume_num)
{
	if (NULL == role) return;

}

void EventHandler::OnAddCoinBind(Role * role, long long money)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_OBTAIN_COIN_NUMBERS, 0, money);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GET_MONEY, MONEY_TYPE_COIN_BIND, money);
	

	role->GetRoleModuleManager()->GetRoleIncome()->OnIncomeAdd(INCOME_ROLE_TYPE_DAY_COIN_BIND, money);
}

void EventHandler::OnAddImmortalCoin(Role * role, long long money)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GET_MONEY, MONEY_TYPE_IMMORTAL_COIN, money);
}

void EventHandler::OnAddGold(Role * role, long long money)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GET_MONEY, MONEY_TYPE_GOLD, money);
}

void EventHandler::OnComsuneCoinBind(Role * role, long long money)
{
	if (NULL == role) return;

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_CONSUME_COIN, money);
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_COMSUME_COIN, money);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->CostCoin((int)money);
	}

	RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL != cool_breeze)
	{
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_CONSUME_COIN, money, 0);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_COIN, money);
	}
}

void EventHandler::OnComsuneImmortalCoin(Role * role, long long money)
{
	if (NULL == role) return;

}

void EventHandler::OnComsuneGold(Role * role, long long money)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleActivityManager()->OnConsumeGold(money);

	long long &today_gold_shop_cost = role->GetRoleModuleManager()->GetCommonData().today_gold_shop_cost;
	today_gold_shop_cost += money;
	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_6);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_6; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_6)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 <= today_gold_shop_cost)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnAddOtherCoin(Role * role, int money_type, long long money)
{
	if(NULL == role) return;
	
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GET_MONEY, money_type, money);
	if (money_type == MONEY_TYPE_GUILD_CONTRIBUTION)
	{
		Guild * guild = role->GetGuild();
		if(NULL == guild) return;

		GuildMember *guild_member = guild->GetMemberManager()->GetMember(role->GetUserId());
		if(guild_member == NULL) return;

		guild_member->SysContribution(role->GetRoleModuleManager()->GetMoney()->GetOtherCoin(MONEY_TYPE_GUILD_CONTRIBUTION));
		if (money != 0)
		{
			guild_member->AddGongXian(money, __FUNCTION__);
		}
	}

	switch (money_type)
	{
	case MONEY_TYPE_COIN_BIND:
		{
			role->GetRoleModuleManager()->GetRoleIncome()->OnIncomeAdd(INCOME_ROLE_TYPE_DAY_COIN_BIND, money);
		}
		break;
	case MONEY_TYPE_SILVER_COIN:
		{
			role->GetRoleModuleManager()->GetRoleIncome()->OnIncomeAdd(INCOME_ROLE_TYPE_DAY_SILVER_COIN, money);
		}
		break;
	default:
		break;
	}

}

void EventHandler::OnUseOtherCoin(Role *role , int money_type, long long money)
{
	if (NULL == role) return;

	if (money_type == MONEY_TYPE_GUILD_CONTRIBUTION)
	{
		Guild * guild = role->GetGuild();
		if (NULL == guild) return;

		GuildMember *guild_member = guild->GetMemberManager()->GetMember(role->GetUserId());
		if (guild_member == NULL) return;

		guild_member->SysContribution(role->GetRoleModuleManager()->GetMoney()->GetOtherCoin(MONEY_TYPE_GUILD_CONTRIBUTION));
	}
	if (MONEY_TYPE_SILVER_COIN == money_type)
	{
		//竞技月卡
		RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
		if (NULL != yueka_activity)
		{
			yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_CONSUME_SILVER_COIN, money);
		}
	}
}

void EventHandler::OnAddExp(Role * role, long long add_exp, short add_reason)
{
	if (NULL == role)
	{
		return;
	}
	if (0 >= add_exp)
	{
		return;
	}

	role->GetRoleModuleManager()->GetRoleIncome()->OnIncomeAdd(INCOME_ROLE_TYPE_DAY_EXP, add_exp);
}

void EventHandler::OnRoleJoinTeam(Role * role, Team * team)
{
	if (role == NULL || team == NULL) return;

	int uid = role->GetUID();

	static Protocol::SCJoinTeamNotifyArea notice;
	notice.obj_id = role->GetId();
	notice.is_red = team->TeamIsRedName() ? 1 : 0;
	notice.team_id = team->GetTeamIndex();
	role->GetScene()->GetZoneMatrix()->NotifyAreaMsg(role->GetObserHandle(), &notice, sizeof(notice));

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
	{
		TeamMember *member = team->GetMemberInfoByIndex(i);
		if (NULL == member || member->uid == role->GetUID()) continue;

		Role * team_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (team_role != NULL)
		{
			FriendManager::Instance().AddRecentlyTeam(team_role , role->GetUID());
		}

		FriendManager::Instance().AddRecentlyTeam(role, member->uid);
	}

	role->GetRoleModuleManager()->GetTaskManager()->OnTeamConvert();
	role->GetRoleModuleManager()->GetResearchTask()->OnRoleEnterTeam();
	role->GetRoleModuleManager()->GetSchoolTask()->OnRoleEnterTeam();
	role->GetRoleModuleManager()->GetLanternRiddie()->ClearTeamInfo();

	if (CrossConfig::Instance().IsHiddenServer())
	{
		UniqueUserID uuid = LongLongToUniqueUserID(role->GetUUID());
		TeamMatch::Instance().OnEnterTeamSucc(uuid, team->GetTeamIndex());
	}
	else
	{
		TeamMatch::Instance().OnEnterTeamSucc(role->GetUniqueUserID(), team->GetTeamIndex());
	}

	const RoleBattleInfo* rbi = BattleManagerLocal::GetInstance().GetRoleBattleInfoConst(role->GetUID());
	if (NULL != rbi)
	{
		if (rbi->is_ob)
		{
			BattleManagerLocal::GetInstance().ForceStopRoleOB(role);
		}
	}

	{
		Role* leader = team->GetLeaderRole();
		if (NULL != leader && leader->GetRoleStatusManager()->IsInBattleStatus())
		{	
			BattleManagerLocal::GetInstance().RoleOBReq(role, leader->GetUID());		
		}
	}

	team->OnRoleEnterTeamKickRealRoleRobot(uid);
}

void EventHandler::OnRoleLeaveTeam(Role * role, int team_index, bool is_team_dismiss)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnTeamConvert();
	role->GetRoleModuleManager()->GetResearchTask()->OnRoleLeaveTeam();
	if (!is_team_dismiss)
	{
		WorldTeamArenaSignUpManager::Instance().OnLeaveTeam(role, team_index);
	}
	Scene * scene = role->GetScene();
	if (NULL != scene)
	{
		scene->GetSpecialLogic()->OnRoleLeaveTeam(role);

		static Protocol::SCLeaveTeamNotifyArea notice;
		notice.obj_id = role->GetId();
		notice.reserve_sh = 0;
		role->GetScene()->GetZoneMatrix()->NotifyAreaMsg(role->GetObserHandle(), &notice, sizeof(notice));
	}

	TeamMatch::Instance().SendMatchInfo(role, 0);
	role->GetRoleModuleManager()->GetLanternRiddie()->ClearTeamInfo();
}

void EventHandler::OnRoleBecomeLeader(Role * role, Team * team)
{
	if (NULL == role || NULL == team) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnTeamConvert();
}

void EventHandler::OnTeamChangeLeader(Role * leader,int last_leader_id, Team * team)
{
	if (NULL == leader || NULL == team) return;


	SpecialLogic * sp_logic = leader->GetScene()->GetSpecialLogic();
	if (NULL != sp_logic && Scene::SCENE_TYPE_MAZE_RACER == sp_logic->GetSceneType())
	{
		((SpecialLogicMazeRacer *)sp_logic)->OnTeamChangeLeader(leader, last_leader_id, team);
	}

	leader->GetRoleModuleManager()->GetTaskManager()->OnTeamConvert();
}

void EventHandler::OnRoleRelieveLeader(Role * role, Team * team)
{
	if (NULL == role || NULL == team) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnTeamConvert();
}

void EventHandler::OnTeamDismiss(Role * role, Team * team)
{
	if (NULL == role || NULL == team) return;

	Scene * scene = role->GetScene();
	if (NULL != scene) 
	{
		scene->GetSpecialLogic()->OnTeamDismiss(role, team);
	}
}

void EventHandler::OnRoleRejoinTeam(Role* role, Team* team)
{
	if (NULL == role || NULL == team) return;

	role->GetRoleModuleManager()->GetResearchTask()->OnRoleRejoinTeam();
}

void EventHandler::OnRoleTrapCreature(Role * role, int pet_id, bool is_succ)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_EXECUTES_SEAL_ORDER, 0, 1);

	if (is_succ)
	{
		role->GetRoleModuleManager()->GetTaskManager()->OnTrapCreature(pet_id);
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_LEVEL, 0, 1);
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_TRAPS_CREATURES, 0, 1);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN, 1);
		role->GetRoleModuleManager()->GetFuncGuide()->OnCatchPetSucc();
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_CATCH_PET, 1, pet_id);

		RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken * >
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != god_token)
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_SEAL_PET);
			god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_SEAL_PET);
		}

		RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
		if (NULL != fortune_maze)
		{
			fortune_maze->CatchPet();
		}

		RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
		if (NULL != cool_breeze)
		{
			cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_SEAL_PET, 1, 0);
		}

		Guild* guild = role->GetGuild();
		if (NULL != guild)
		{
			guild->OnGuildWeekTaskEvent(role->GetUserId(), GWT_COND_SEAL_PET, 0);
		}
		
		int &trap_creature_times = role->GetRoleModuleManager()->GetCommonData().trap_creature_times;
		trap_creature_times += 1;
		const AutoTitleCfg *cfg_node = NULL;
		int seq = -1;
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_4);
		if (cfg_node == NULL)
		{
			return;
		}
		//限制循环在100次以内
		for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_4; round++, seq++)
		{
			cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
			if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_4)
			{
				return;
			}
			bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
			if (!has_title && cfg_node->cond_param1 <= trap_creature_times)
			{
				//没有这个称号，同时满足称号获取条件
				role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
			}
		}
	}
}

void EventHandler::OnRoleGetACreature(Role * role, int pet_id, long long unique_id, PetParam * pet_param)
{
	if (NULL == role) return;

	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == pet_cfg) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnGetACreature(pet_id);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_LEVEL, 0, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_OBTAIN_RACE_NUM, pet_cfg->race, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GET_QUALITY_PET, 0, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GET_QUALITY_PET, pet_cfg->quality, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_CATCH_PET, 1, pet_id);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_HAS_PET, 0);

	if (pet_param != NULL)
	{
		const PetCfg*  pet_config = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL != pet_config)
		{
			role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_SPECIAL_PET_INTEN, pet_config->quality, pet_param->str_level);
		}
	}
	role->GetRoleModuleManager()->GetCollection()->OnPetCollection(pet_cfg->prototype_pet_id);
	role->GetRoleModuleManager()->GetRoleGatherDream()->OnGetPet(pet_cfg->prototype_pet_id);
	//role->GetRoleModuleManager()->GetHuaShenManager()->AddPet(pet_data, pet_index);

	if (NULL != pet_param)
	{
		role->GetRoleModuleManager()->GetDayReward()->OnUnlockPetStrengthenReward(pet_cfg->quality, pet_param->str_level);
	}
}

void EventHandler::OnLearnSkill(Role * role, int skill_id)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetTaskManager()->OnLearnSkill();
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ROLE_SKILL_LEVEL, 1);
}

void EventHandler::OnAddSkill(Role * role, RoleSkill::SkillItem * skill_item)
{
	if (NULL == role || NULL == skill_item) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_STUDY_BATTLE_SKILLS, 0, 1);
	role->GetRoleModuleManager()->GetHuaShenManager()->AddSkill(skill_item);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ROLE_SKILL_LEVEL, skill_item->skill_level);
}

void EventHandler::OnSkillLevelUp(Role * role, int skill_id, int old_level, int cur_level)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnLearnSkill();
	role->GetRoleModuleManager()->GetHuaShenManager()->SkillUp(skill_id, cur_level);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_SKILLS_REACHS_LEVEL, 0, 0);
	role->OnRoleSkillUplevel(static_cast<UInt16>(skill_id), static_cast<UInt16>(cur_level));
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ROLE_SKILL_LEVEL, cur_level);
	// 这个检查放在最后
	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_10, skill_id);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_10; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_10 || skill_id != cfg_node->cond_param1)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param2 <= cur_level)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnLifeSkillLevelUp(Role * role, int skill_id, int old_level, int cur_level)
{
	if (NULL == role) return;

	// 这个检查放在最后
	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_10, skill_id);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_10; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_10 || skill_id != cfg_node->cond_param1)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param2 <= cur_level)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnPetAquireSkill(Role* role, int pet_id, bool is_fight_pet, UInt16 skill_id, short skill_level)
{
	if (NULL == role) return;

	if (is_fight_pet)
	{
		role->OnFightPetSkillLevelChange(skill_id, skill_level);
	}
}

void EventHandler::OnPetForgetSkill(Role* role, int pet_id, bool is_fight_pet, UInt16 skill_id, short skill_level)
{
	if (NULL == role) return;

	if (is_fight_pet)
	{
		role->OnFightPetSkillLevelChange(skill_id, 0);
	}
}

void EventHandler::OnRoleTrainLevelUp(Role * role, int train_type, int old_lv, int cur_lv)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ROLE_SKILL_LEVEL, cur_lv);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_TRAIN_SKILL_LEVEL, cur_lv, 0);
}

void EventHandler::OnRoleTrainPetSkillLevelUp(Role * role, int type, int pet_cur_level)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetPet()->OnRolePetTrainSkillLevelUp();
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_PRACTICE_LEVEL, 0, 0);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ROLE_SKILL_LEVEL, pet_cur_level);
}

void EventHandler::OnRoleTrainPartnerSkillLevelUp(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetPartner()->ReCalAllPartnerAttr(Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_TRAIN_UP);
}

void EventHandler::OnRoleActiveSmartMounts(Role * role, int index)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_UNLOCK_SMART_MOUNTS, 1);
}

void EventHandler::OnRoleStrSmartMountsJewelry(Role * role, int index)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_UNLOCK_SMART_MOUNTS, 1);
}

void EventHandler::OnRoleGuildInbornRoleChange(Role * role, int inborn_type)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_GUILD_INBORN, __FUNCTION__);
}

void EventHandler::OnRoleGuildInbornPetChange(Role * role, int inborn_type)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetPet()->OnRoleGuildInbornSkillChange();
}

void EventHandler::OnRoleGuildInbornPartnerChange(Role * role, int inborn_type)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetPartner()->OnRoleGuildInbornChange();
}

void EventHandler::OnRoleHeartSkillUnlock(Role * role, int heart_skill_seq)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCollection()->OnReadyCollection(CollectionConfig::COLLECTION_TYPE_HEART_SKILL, heart_skill_seq);
}

void EventHandler::OnRoleHeartSkillLevelUp(Role * role, int heart_skill_seq, int level)
{
	if(NULL == role) return;
}

void EventHandler::OnFightMonsterGroup(Role* role, int monster_group_id, bool is_win, bool is_flyaway)
{
	if (NULL == role) return;
	
	if (is_win && !is_flyaway)
	{
		const MonsterGroupCfg* cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
		bool is_need_drop = true;
		SpecialLogic * special_logic = role->GetScene()->GetSpecialLogic();
		if (special_logic != NULL)
		{
			if (special_logic->GetSceneType() == Scene::SCENE_TYPE_CRYSTAL_FB)
			{
				SpecialLogicCrystalFB * crystal_logic = (SpecialLogicCrystalFB *)special_logic;

				const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(crystal_logic->GetLevelSeq());
				if (cfg != NULL && cfg->type == ROLE_CRYSTAL_FB_TYPE_JING_YING)
				{
					is_need_drop = false;
				}
			}
		}

		if (NULL != cfg && is_need_drop /*&& !role->GetRoleModuleManager()->GetRoleDrop()->IsMonsterGroupDropLimit(monster_group_id)*/)
		{
			bool drop_succ = false;
			if (role->InTeam() && role->GetMyTeam()->IsLeader(role) && cfg->captain_only_drop)
			{
				for (size_t i = 0; i < cfg->captain_drop_list.size(); ++i)
				{
					if (DropPool::Instance()->DropToRole(role, cfg->captain_drop_list[i], monster_group_id, true))
					{
						drop_succ = true;
					}
				}
			}
			else
			{
				for (size_t i = 0; i < cfg->dropid_list.size(); ++i)
				{
					if (DropPool::Instance()->DropToRole(role, cfg->dropid_list[i], monster_group_id, true))
					{
						drop_succ = true;
					}
				}
			}
			
			if (drop_succ && -1 == cfg->drop_item_id_checker) // 只要掉落成功，不管掉落什么都算一次
			{
				role->GetRoleModuleManager()->GetRoleDrop()->AddMonsterGroupDrop(monster_group_id);
			}
		}
		//role->GetRoleModuleManager()->GetSalary()->OnRoleKillMonsterGroup(monster_group_id);
		role->GetRoleModuleManager()->GetTaskManager()->OnVictBattle(monster_group_id);
		role->GetRoleModuleManager()->GetAnecdoteTask()->OnBattleSucc(monster_group_id);
		
		if (NULL != cfg)
		{
			role->AddExp(cfg->kill_exp, ADD_EXP_TYPE_FB_GOLD_BOX);
		}

		// 暗怪掉落
		if (BATTLE_MINE_MONSTER_GROUP == monster_group_id)
		{
			Scene* scene = role->GetScene();
			if (NULL != scene)
			{
				const Scene::MineMonster& mm = scene->GetMineMonsterCfg();
				for (int i = 0; i < mm.mine_monster_drop_num; ++i)
				{
					int drop_id = mm.mine_monster_drop_list[i];
					DropPool::Instance()->DropToRole(role, drop_id, monster_group_id, true);
				}
			}
		}

		if (NULL != cfg && cfg->is_task_monster)
		{		
			role->GetRoleModuleManager()->GetSalary()->OnRoleWinTaskMonsterGroup(monster_group_id);	
		}

		int prestige_village = role->GetScene()->GetMineMonsterCfg().prestige_id;
		if (BATTLE_MINE_MONSTER_GROUP == monster_group_id && prestige_village > 0)
		{
			role->GetRoleModuleManager()->GetPrestige()->AddPrestige(prestige_village, 1, __FUNCTION__);
		}

		role->GetRoleModuleManager()->GetBossChapter()->OnKillMonsterGroup(monster_group_id);

		//渡劫
		role->GetRoleModuleManager()->GetDuJie()->ChangeTaskParam(DU_JIE_TASK_TYPE_MONSTER_GROUP, monster_group_id);
	}
	else
	{
		role->GetRoleModuleManager()->GetAnecdoteTask()->OnBattleFail(monster_group_id);
	}
	
	if (!is_flyaway)
	{
		role->GetRoleModuleManager()->GetTaskManager()->OnJoinBattle(monster_group_id);
	}
}

void EventHandler::OnKillMonster(Role* role, int monster_num, int* monster_id_list, short* monster_level_list, bool is_mine_monster)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCollection()->OnKillMonsterDropCard(monster_num, monster_id_list);
	role->GetRoleModuleManager()->GetTaskChain()->OnKillMineMonster();

	int total_drop_element_num = 0;
	long long total_add_exp = 0;
	bool is_static_scene = role->GetScene()->IsInStaticScene();
	for (int i = 0; i < monster_num; ++i)
	{
		int monster_id = monster_id_list[i];
		short monster_level = monster_level_list[i];

		// 目前只有野外场景的暗怪需要提供经验
		if (is_mine_monster && is_static_scene)
		{
			int exp = (int)(12.0 * role->GetLevel() * (30 - abs(role->GetLevel() - monster_level)) / 30 / 0.85 * 0.2);
			if (exp <= 0)
			{
				exp = 1;
			}
			total_add_exp += exp;
		}
		
		role->GetRoleModuleManager()->GetTaskManager()->OnKillMonster(monster_id);
		role->GetRoleModuleManager()->GetSchoolTask()->OnKillMonster(monster_id);
		role->GetRoleModuleManager()->GetTaskManager()->AddPatrolTaskProgress();

		const MonsterCfg * cfg = MonsterPool::GetInstance().GetMonsterCfg(monster_id);
		if (NULL == cfg) continue;

		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_DEFEAT_RACE_MONSTER, cfg->m_race, 1);
		Guild * guild = role->GetGuild();
		if (NULL != guild)
		{
			guild->OnGuildWeekTaskEvent(role->GetUserId(), GWT_COND_KILL_RACE_MONSTER, cfg->m_race);
		}

		this->DropElementFaction(role, is_mine_monster, cfg, total_drop_element_num);
	}
	
	RoleActivityHelpOthers * help_others = static_cast<RoleActivityHelpOthers *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_HELP_OTHERS));
	if (NULL != help_others)
	{
		help_others->AddTaskProgress(monster_num);
	}

	RoleActivityHelpOthers2 * help_others_2 = static_cast<RoleActivityHelpOthers2 *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_HELP_OTHERS_2));
	if (NULL != help_others_2)
	{
		help_others_2->AddTaskProgress(monster_num);
	}

	RoleActivityFateEntanglement * fate_entanglement = static_cast<RoleActivityFateEntanglement *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT));
	if (NULL != fate_entanglement)
	{
		fate_entanglement->AddTaskProgress(monster_num);
	}

	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		guild->OnGuildWeekTaskEvent(role->GetUserId(), GWT_COND_KILL_MONSTER, 0, monster_num);
	}

	if (total_add_exp > 0)
	{
		total_add_exp += role->GetTeamExpUpCountByMineMonster(total_add_exp);
		long long real_add_exp = role->AddExp(total_add_exp, ADD_EXP_TYPE_KILL_NORMAL_MONSTER, __FUNCTION__);
		role->GetRoleModuleManager()->GetLevelComplement()->OnAddExp(real_add_exp);//计算助力经验
		role->GetRoleModuleManager()->GetPet()->OnBattleFinishAddExp(real_add_exp);	
		this->OnFinishLevelComplementRelatedFun(role, total_add_exp, __FUNCTION__);
	}
}

void EventHandler::OnRoleStartMove(Role* role)
{
	if (NULL == role) return;

	role->GetRoleStatusManager()->OnRoleStartMove();
}

void EventHandler::OnRoleStopMove(Role* role)
{
	if (NULL == role) return;

	role->GetRoleStatusManager()->OnRoleStopMove();
}

void EventHandler::OnReloadConfig(int config_type, int param1, int param2)
{
	static battlegameprotocol::GameBattleReloadConfig msg;
	msg.config_type = config_type;
	msg.param1 = param1;
	msg.param2 = param2;

	World::GetInstWorld()->SendMsgToAllBattle((const char*)&msg, sizeof(msg));

	if (RELOAD_CONFIG_TYPE_NOTICE_CONFIG == config_type)
	{
		std::vector<NoticeCfg*> changed_list = NoticeConfig::Instance()->GetChangedNotice();
		for (size_t i = 0; i < changed_list.size(); ++i)
		{
			NoticeCfg* cfg = changed_list[i];
			if (NULL == cfg) continue;

			Protocol::SCNoticeCfgUpdate msg;
			msg.notice_id = cfg->notice_id;
			msg.is_show = cfg->is_show ? 1 : 0;

			if (0 == msg.is_show)
			{
				msg.title_len = 0;
				msg.subject_len = 0;
				msg.content_len = 0;
			}
			else
			{
				msg.title_len = cfg->subject.length();
				F_STRNCPY(msg.title, cfg->title.c_str(), sizeof(msg.title));

				msg.subject_len = cfg->subject.length();
				F_STRNCPY(msg.subject, cfg->subject.c_str(), sizeof(msg.subject));

				msg.content_len = cfg->content.length();
				F_STRNCPY(msg.content, cfg->content.c_str(), sizeof(msg.content));
			}
			
			World::GetInstWorld()->SendSerializeToAllGateway(&msg);
		}
	}
	else if (RELOAD_CONFIG_TYPE_ACTIVITY == config_type)
	{
		static Protocol::SCActivityConfigReloadNotice notice;
		World::GetInstWorld()->SendToAllGateway((const char *)&notice, sizeof(notice));

		if (!CrossConfig::Instance().IsHiddenServer())
		{
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
			if (NULL != ramgr)
			{
				ramgr->OnReloadActivityConfig();
			}
		}
	}
	else if (RELOAD_CONFIG_TYPE_LOGIC_CONFIG == config_type || RELOAD_CONFIG_TYPE_LOGIC_CONFIG_ASYNC == config_type)
	{
		XunBao::Instance().SendOpenInfo();
		MarketManager::Instance().OnReloadConfig();
		KuaFuYingXiongLuManager::Instance().OnReloadConfig();
	}
}

void EventHandler::OnRoleCapabilityChange(Role * role, int old_capability, int new_capability)
{
	if(NULL == role || old_capability == new_capability) return;

	RoleActivityServerCompetition * role_server_competition = static_cast<RoleActivityServerCompetition *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL != role_server_competition)
	{
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_ROLE_CAP, new_capability);
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP, role->GetCapabilityWithFight());
	}
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_ROLE_CAPABILITY);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACH_CAPABILITY, COG_CON_REACH_CAPABILITY_TYPE_COUNT, 0);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACH_CAPABILITY, COG_CON_REACH_CAPABILITY_TYPE_ROLE, new_capability);
	this->OnWorldTeamArenaChangeByRole(role);

	if (role->GetScene()->GetSpecialLogic()->GetSceneType() == Scene::SCENE_TYPE_UTA)
	{
		SpecialLogicUTA* spl = dynamic_cast<SpecialLogicUTA*>(role->GetScene()->GetSpecialLogic());
		if (NULL != spl)
		{
			spl->OnBattleDataChanged(role);
		}
	}
}

void EventHandler::OnPetCapabilityChange(Role * role, int pet_capability)
{
	if(NULL == role) return;

	RoleActivityServerCompetition * role_server_competition = static_cast<RoleActivityServerCompetition *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL != role_server_competition)
	{
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_PET_CAP, pet_capability);
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP, role->GetCapabilityWithFight());
	}
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_PET_MAX_CAPABILITY);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACH_CAPABILITY, COG_CON_REACH_CAPABILITY_TYPE_COUNT, 0);

	if (role->GetScene()->GetSpecialLogic()->GetSceneType() == Scene::SCENE_TYPE_UTA)
	{
		SpecialLogicUTA* spl = dynamic_cast<SpecialLogicUTA*>(role->GetScene()->GetSpecialLogic());
		if (NULL != spl)
		{
			spl->OnBattleDataChanged(role);
		}
	}
}

void EventHandler::OnPartnerCapabilityChange(Role * role, int old_partner_capability, int new_partner_capability)
{
	if(NULL == role || old_partner_capability == new_partner_capability) return;

	RoleActivityServerCompetition * role_server_competition = static_cast<RoleActivityServerCompetition *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL != role_server_competition)
	{
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP, role->GetRoleModuleManager()->GetPartner()->GetMaxFourPartnerCap());
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP, role->GetCapabilityWithFight());
	}
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_FOUR_MAX_PARTNER);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACH_CAPABILITY, COG_CON_REACH_CAPABILITY_TYPE_COUNT, 0);
}

void EventHandler::OnRoleEquipCapabilityChange(Role * role, int old_equip_capability, int new_quip_capability)
{
	if(NULL == role || old_equip_capability == new_quip_capability) return;

	RoleActivityServerCompetition * role_server_competition = static_cast<RoleActivityServerCompetition *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL != role_server_competition)
	{
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP, new_quip_capability);
	}
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_EQUIPMENT_CAP);
}

void EventHandler::OnRoleColosseumRankChange(Role * role, int old_colosseum_rank, int new_colosseum_rank)
{
	if(NULL == role || old_colosseum_rank == new_colosseum_rank) return;

	RoleActivityServerCompetition * role_server_competition = static_cast<RoleActivityServerCompetition *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL != role_server_competition)
	{
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_COLOSSEUM, new_colosseum_rank);
	}
	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_RANK, new_colosseum_rank);
	}

}

void EventHandler::OnRoleCourageChallengeChange(Role * role, int star_count)
{
	if(NULL == role || star_count < 0) return;

	RoleActivityServerCompetition * role_server_competition = static_cast<RoleActivityServerCompetition *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL != role_server_competition)
	{
		role_server_competition->OnRankValueChange(SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE, star_count);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_COURAGE_CHALLENGE_STAR, star_count);
	}
}

void EventHandler::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;
	ActivityManager::Instance().OnUserEnterScene(role);
	role->GetRoleModuleManager()->GetTreasureMap()->OnRoleEnterScene();

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		//if (team->GetLeaderUID() == role->GetUID())
		//{
		//	TeamMatch::Instance().OnTeamStopMatch(team);
		//
		//	if (CrossConfig::Instance().IsHiddenServer()) // 进入万妖谷战斗的队伍，可以从队伍列表中去除
		//	{
		//		TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE);
		//	}
		//}
		team->SendUpdateFollowListMsg(role, role); // 进入场景时，给自己发就可以，其他人通过AOI来发，机器人则发AddFollow
	
		if (team->IsLeader(role))
		{
			if (role->GetScene()->IsInStaticScene())
			{
				TeamManager::Instance().GetTeamChannel()->OnTeamEnterNormalScene(team);
			}
			else
			{
				TeamManager::Instance().GetTeamChannel()->OnTeamEnterSpecialScene(team);
			}
		}
	}

	if (!role->GetScene()->IsInStaticScene())
	{
		// 进副本后应该停止玩家个人匹配
		TeamMatch::Instance().OnUserStopMatch(role->GetUniqueUserID()); 
	}

	role->GetRoleModuleManager()->GetRoleCrystalFb()->OnRoleEnterScene();
	role->GetRoleModuleManager()->GetDreamNotesChallenge()->OnRoleEnterScene();
}

void EventHandler::OnRoleLeaveScene(Role * role)
{
	if (NULL == role) return;
	ActivityManager::Instance().OnUserLeaveScene(role);
}

void EventHandler::OnStartGather(Role * role, int gather_id, int seq_id, const Posi &gather_pos)
{
	if (NULL == role) return;

	Scene * scene = role->GetScene();
	if (NULL != scene)
	{
		scene->GetSpecialLogic()->OnStartGather(role, gather_id, seq_id, gather_pos);
	}
}

void EventHandler::OnSuccsGather(Role * role, int gather_id, int seq_id, const Posi & gather_pos)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnLoadProgress(gather_id);
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnSuccsGather(gather_id , seq_id , gather_pos);

	Scene * scene = role->GetScene();
	if (NULL != scene)
	{
		SpecialLogic* sp_fb = scene->GetSpecialLogic();
		if (sp_fb != NULL && sp_fb->OnGatherActiveAdventures())
		{
			role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
		}
		scene->GetSpecialLogic()->OnGather(role, gather_id, seq_id, gather_pos);
	}
}

void EventHandler::OnGetItemByGather(Role * role, int item_id, int item_num)
{
	const ItemBase * item_ib = ITEMPOOL->GetItem(item_id);
	if (NULL == item_ib) return;

	Scene * scene = role->GetScene();
	if (NULL != scene)
	{
		scene->GetSpecialLogic()->OnGetItemByGather(role, item_id, item_num);
	}

	int gather_type = 0;
	switch (item_ib->GetItemSubType())
	{
	case ItemNamespace::ITEM_SUB_TYPE_STONE:
		gather_type = GATHER_TYPE_MINING;
		if (item_ib->GetItemType() != 3)this->OnDig(role);
		break;
	case ItemNamespace::ITEM_SUB_TYPE_WOOD:
		gather_type = GATHER_TYPE_LUMBER;
		if (item_ib->GetItemType() != 3)this->OnLumbering(role);
		break;
	case ItemNamespace::ITEM_SUB_TYPE_HERB:
		gather_type = GATHER_TYPE_FLOWER;
		if(item_ib->GetItemType() != 3)this->OnPickFlower(role);
		break;
	default:
		return;
		break;
	}

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GATHER_TIMES_ACCUMU, gather_type, item_num);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GATHER_TIMES_ACCUMU, COG_GATHER_TYPE_COUNT, item_num);

	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		guild->OnGuildWeekTaskEvent(role->GetUserId(), GWT_COND_ITEM_TYPE_GATHERS, gather_type);
	}
}

void EventHandler::OnSuccFish(Role * role)
{
	if(NULL == role) return;

	Scene * scene = role->GetScene();
	if (NULL != scene)
	{
		SpecialLogic* sp_fb = scene->GetSpecialLogic();
		if (sp_fb != NULL && sp_fb->OnGatherActiveAdventures())
		{
			role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
		}
		scene->GetSpecialLogic()->OnFish(role);
	}
}

void EventHandler::OnNpcGatherGatherThis(Role * role, Scene * scene, NPCGather * npc_gather)
{
	if (NULL == role) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		//活动宝箱
		RandActivitySkyDropGift * act = (RandActivitySkyDropGift*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT);
		if (NULL != act && ACTIVITY_STATUS_OPEN == act->GetStatus() && scene->GetSceneKey() == COMMON_SCENE_KEY)
		{
			act->GetBoxReward(role, npc_gather);
			act->OnRoleParticipateRA(role, __FUNCTION__);
		}

		//活动 天降好礼 宝箱(有领取次数限制) 
		RandActivityTianJiangHaoLi * ract_tian_jiang_hao_li = (RandActivityTianJiangHaoLi*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI);
		if (NULL != ract_tian_jiang_hao_li && ACTIVITY_STATUS_OPEN == ract_tian_jiang_hao_li->GetStatus() && scene->GetSceneKey() == COMMON_SCENE_KEY)
		{
			if (ract_tian_jiang_hao_li->IsReceiveBox(role, npc_gather))
			{
				ract_tian_jiang_hao_li->ReceiveBoxOp(role, npc_gather);
				if (NULL != ract_tian_jiang_hao_li)
				{
					ract_tian_jiang_hao_li->OnRoleParticipateRA(role, __FUNCTION__);
				}
			}
		}

		RandActivityLaoDongHaoLi * ract_lao_dong_hao_li = (RandActivityLaoDongHaoLi*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI);
		if (NULL != ract_lao_dong_hao_li && ACTIVITY_STATUS_OPEN == ract_lao_dong_hao_li->GetStatus() && scene->GetSceneKey() == COMMON_SCENE_KEY)
		{
			if (ract_lao_dong_hao_li->IsReceiveBox(role, npc_gather))
			{
				ract_lao_dong_hao_li->ReceiveBoxOp(role, npc_gather);
				if (NULL != ract_lao_dong_hao_li)
				{
					ract_lao_dong_hao_li->OnRoleParticipateRA(role, __FUNCTION__);
				}
			}
		}

		// 瑞雪丰年
		RandActivityTigerAuspiciousSnow * ract_rui_xue_feng_nian = (RandActivityTigerAuspiciousSnow*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);
		if (NULL != ract_rui_xue_feng_nian && ACTIVITY_STATUS_OPEN == ract_rui_xue_feng_nian->GetStatus() && scene->GetSceneKey() == COMMON_SCENE_KEY)
		{
			if (ract_rui_xue_feng_nian->IsReceiveSnow(role, npc_gather))
			{
				ract_rui_xue_feng_nian->ReceiveSnowOp(role, npc_gather);
			}
		}
	}

	if (NULL == npc_gather)
	{
		return;
	}
	switch (npc_gather->GetSubType())
	{
	case NPCGather::SUB_TYPE_XUN_YOU:
		{
			XunYouCar::OnGatherGift(role, npc_gather);
		}
		break;
	default:
		break;
	}
}

void EventHandler::OnRolePartnerJiBanNum(Role * role, int jiban_num)
{
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PARTNER_FETTERS, jiban_num);
}

void EventHandler::OnActivePartner(Role * role, int partner_id)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetTaskManager()->OnActivePartner(partner_id);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PARTNER_NUM, 1);
	role->GetRoleModuleManager()->GetCollection()->OnPartnerCollection(partner_id);
	//role->GetRoleModuleManager()->GetPartner()->GetPartnerParam()
}

void EventHandler::OnPartnerUpLevel(Role * role, int partner_id, int old_level, int new_level)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetTaskManager()->OnPartnerUpLevel(partner_id, old_level, new_level);
}

void EventHandler::OnPartnerStateChange(Role* role, int partner_id, bool is_dismiss)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleFormation()->OnPartnerFightStateChange();
}

void EventHandler::OnPartnerPutOnEquipment(Role * role, int partner_id)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PARTNER_EQUIPMENT, 1);

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_WEAR_EQUIPNUM);
	}
}

void EventHandler::OnPartnerTakeOffEquipment(Role * role, int partner_id)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PARTNER_EQUIPMENT, 1);
	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_WEAR_EQUIPNUM);
	}
}

void EventHandler::OnPartnerQualityUp(Role * role, int partner_id)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PARTNER_QUALITY, 1);
}

void EventHandler::OnPartnerIntensify(Role * role, int paranter_id, int str_level, int quality)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnRoleStrParnter(paranter_id);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_PARTNER_STR, quality, 1);
}

void EventHandler::OnPartnerBreak(Role * role, int paranter_id, int quality)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnRoleParnterBreak(paranter_id);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PARTNER_BREAK, 1, quality);
}

void EventHandler::OnAddTitle(Role * role, int title_id, int title_count)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetFaceScore()->OnGainTitle(title_id);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_OBTAIN_TITLE_NUMBER, 0, title_count);
}

void EventHandler::OnLoseTitle(Role * role, int title_id, int title_count)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetFaceScore()->OnLoseTitle(title_id);
}

bool EventHandler::OnWearOrGetOffTitle(Role * role, int title_id, bool is_wear)
{
	if(NULL == role) return false;
	
	return role->GetRoleModuleManager()->GetFaceScore()->OnWearOrGetOffTitle(title_id, is_wear);;
}

void EventHandler::OnRolePetMonopolyCatchPet(Role* role, const ItemDataWrapper& pet_wrapper)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetPetMonopoly()->AddPetToRecord(pet_wrapper);
	role->GetRoleModuleManager()->GetTaskManager()->OnPetMonopolyCatchPet();
}

void EventHandler::OnRolePetMonopolyGetReward(Role* role, const ItemConfigData& reward)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetPetMonopoly()->AddRewardToRecord(reward);
}

void EventHandler::OnAddApprentice(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_APPRENTICE_COUNT, 0, 1);
}

void EventHandler::OnDeleteApprentice(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_APPRENTICE_COUNT, 0, 1);
}

void EventHandler::OnAddMaster(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_APPRENTICE_DAY_NUM, 0, 1);
}

void EventHandler::OnGraduation(Role * role, int apprentice_day_num)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_APPRENTICE_DAY_NUM, 0, apprentice_day_num);
}

bool HonorRollSort(HonorRollData a, HonorRollData b)
{
	if (a.rounds < b.rounds)
	{
		return true;
	}
	else if (a.rounds == b.rounds)
	{
		if (a.pass_time < b.pass_time)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool UidSort(int a, int b)
{
	return a > b;
}

void EventHandler::OnClearanceFB(int fb_type, int member_count, int member_list[MAX_TEAM_MEMBER_NUM], int rounds, time_t time)
{
	if (fb_type < 0 || fb_type >= WORLD_HONOR_ROLL_TYPE_COUNT) return;

	static HonorRollData honor_roll_node;
	honor_roll_node.Reset();

	// 填充队伍数据
	{
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			honor_roll_node.member_list[i] = member_list[i];
		}
		std::sort(honor_roll_node.member_list, honor_roll_node.member_list + MAX_TEAM_MEMBER_NUM, UidSort);

		honor_roll_node.rounds = rounds;
		honor_roll_node.pass_time = (unsigned int)time;
	}

	WorldHonorRollDataParam& roll_data = WorldStatus::Instance().GetHonorRollData();
	HonorRollTypeData& roll_type_data = roll_data.roll_type_list[fb_type];

	// 如果首杀榜是空的 就放进去
	if (0 == roll_type_data.first_data.pass_time)
	{
		roll_type_data.first_data = honor_roll_node;
	}

	// 速杀榜整理
	std::vector<HonorRollData> roll_list(roll_type_data.roll_data, roll_type_data.roll_data + WORLD_HONOR_ROLL_TEAM_COUNT);
	{
		std::vector<HonorRollData>::iterator it = std::find(roll_list.begin(), roll_list.end(), honor_roll_node);
		if (it == roll_list.end())	// 如果没有这队的成绩 就插入一条
		{
			roll_list.push_back(honor_roll_node);
		}
		else		// 有则替换
		{
			*it = honor_roll_node;
		}

		// 排序一下
		std::sort(roll_list.begin(), roll_list.end(), HonorRollSort);
	}

	// 存数据
	int count = 0;
	for (std::vector<HonorRollData>::iterator it = roll_list.begin(), end = roll_list.end(); count < WORLD_HONOR_ROLL_TEAM_COUNT && it != end; ++it)
	{
		roll_type_data.roll_data[count++] = *it;
	}

}

void EventHandler::OnClearFBOneLayer(Role* role, int fb_type, int param)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetSalary()->OnRoleClearFBOneLayer(fb_type, param);
}

void EventHandler::OnAddFace(Role * role, int res_type)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_HAS_ACTIVE_FACE, res_type, 1);
}

void EventHandler::OnLostFace(Role * role, int res_type)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_HAS_ACTIVE_FACE, res_type, 1);
}

void EventHandler::OnChannelChat(Role * role, int channel_type)
{
	if(NULL == role || channel_type < 0 || channel_type >= chatdef::CHANNEL_TYPE_MAX) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_CHANNEL_CHAT, channel_type, 1);
}

void EventHandler::OnFetchFriendGift(Role * role, int recevice_count /* = 1 */)
{
	if(NULL == role || recevice_count <= 0) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_FETCH_FRIEND_GIFT, 0, recevice_count);
}

void EventHandler::OnAddPrestige(Role * role, int village_id, int value, int old_level, int new_level)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACHS_PRESTIGE_NUM, village_id, value);

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken * >
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_GET_PRESTIGE, value);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->GetPrestige(value);
	}
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PRESTIG_LEVEL, 1, new_level);
	role->GetRoleModuleManager()->GetTaskManager()->OnAddPrestige(village_id, new_level);
}

void EventHandler::OnPrestigeShopping(Role * role, int village_id, int item_id)
{
	if(NULL == role) return;

	if (village_id == PARTNER_UPGRADE_TASK_PRESTIGE_VILLAGE_ID && item_id == PARTNER_UPGRADE_TASK_ITEM_ID)
	{
		role->GetRoleModuleManager()->GetTaskManager()->OnSetPartnerUpgradeTaskBuyFlag();
	}
}

void EventHandler::OnAddSurface(Role * role, int type, int id)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_HAS_SURFACE, type, 1);
}

void EventHandler::OnAcceptTask(Role* role, int task_id)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleDrop()->OnRoleAcceptTask(task_id);

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL != task_cfg)
	{
		if (TASK_NEED_KILL_MONSTERS == task_cfg->task_condition)
		{
			role->GetRoleModuleManager()->AddNeedMonster(task_cfg->param1);
		}
		else if (TASK_NEED_KILL_MONGROUP == task_cfg->task_condition || TASK_NEED_KILL_IN_ORDER == task_cfg->task_condition)
		{
			const std::vector<int>* monster_list = TASKPOOL->GetMonsterGroupMonsterList(task_cfg->param1);
			if (NULL != monster_list)
			{
				for (size_t k = 0; k < monster_list->size(); ++k)
				{
					role->GetRoleModuleManager()->AddNeedMonster((*monster_list)[k]);
				}
			}
		}
		else if (TASK_NEED_GAIN_TASKPROP == task_cfg->task_condition)
		{
			const DefeatMonsGroups * group_cfg = TASKPOOL->GetDefeatMonsGroups(task_cfg->param1);
			if (NULL != group_cfg)
			{
				std::set<int>::iterator it = group_cfg->drop_monster_list.begin();
				for (; it != group_cfg->drop_monster_list.end(); ++it)
				{
					role->GetRoleModuleManager()->AddNeedMonster(*it);
				}		
			}
		}
		else if (TASK_NEED_HAVE_PROPERTY_LIST == task_cfg->task_condition)
		{
			// 触发一次检查背包
			role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_HAVE_PROPERTY_LIST, 0, 0);
		}

		if (role->InTeam())
		{
			Team * team = role->GetMyTeam();
			if (NULL != team && team->GetLeaderUID() == role->GetUID())
			{
				if (task_cfg->task_type == TASK_TYPE_RESEARCHES)
				{
					TeamMatch::Instance().OnTeamStopMatch(team);

					if (CrossConfig::Instance().IsHiddenServer()) // 进入万妖谷战斗的队伍，可以从队伍列表中去除
					{
						TeamManager::Instance().SyncCrossTeamInfo(team, crossgameprotocal::HiddenGameSyncCrossTeamInfo::NOTIFY_REASON_REMOVE);
					}
				}
			}
		}
	}
}

void EventHandler::OnGiveupTask(Role* role, int task_id)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleDrop()->OnRoleGiveupTask(task_id);

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL != task_cfg)
	{
		if (TASK_NEED_KILL_MONSTERS == task_cfg->task_condition)
		{
			role->GetRoleModuleManager()->RemoveNeedMonster(task_cfg->param1);
		}
		else if (TASK_NEED_KILL_MONGROUP == task_cfg->task_condition || TASK_NEED_KILL_IN_ORDER == task_cfg->task_condition)
		{
			const std::vector<int>* monster_list = TASKPOOL->GetMonsterGroupMonsterList(task_cfg->param1);
			if (NULL != monster_list)
			{
				for (size_t k = 0; k < monster_list->size(); ++k)
				{
					role->GetRoleModuleManager()->RemoveNeedMonster((*monster_list)[k]);
				}
			}
		}
		else if (TASK_NEED_GAIN_TASKPROP == task_cfg->task_condition)
		{
			const DefeatMonsGroups * group_cfg = TASKPOOL->GetDefeatMonsGroups(task_cfg->param1);
			if (NULL != group_cfg)
			{
				std::set<int>::iterator it = group_cfg->drop_monster_list.begin();
				for (; it != group_cfg->drop_monster_list.end(); ++it)
				{
					role->GetRoleModuleManager()->RemoveNeedMonster(*it);
				}
			}
		}
	}
}

void EventHandler::OnCommitTask(Role * role, int id)
{
	if (NULL == role) return;

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(id);
	if (NULL == task_cfg) return;

	role->GetRoleModuleManager()->CheckStartCapacityTarget(id);
	role->GetRoleModuleManager()->GetPartner()->ActiveAppointedPartner(id, UNLOCK_TYPE_TASK);
	role->GetRoleModuleManager()->GetRoleDrop()->OnRoleCommitTask(id);
	role->GetRoleModuleManager()->GetPet()->OnFinishFeiShengTask(id);
	role->GetRoleModuleManager()->GetRoleGuildTask()->OnFinishTask(id);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_APPOINTED_TASK_DONE, 0, 0);	
	role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_MISSION, id, 0);
	
	if (task_cfg->task_type == TASK_TYPE_TEAM_CHALLENGES)
	{
		const TeamChallengeRewardCfg * team_challenge_cfg = TASKPOOL->GetTeamChallengeRewardCfg(id);
		if (NULL != team_challenge_cfg)
		{
			if (team_challenge_cfg->coin_bind > 0)
			{
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(team_challenge_cfg->coin_bind, __FUNCTION__);
			}

			if (team_challenge_cfg->exp > 0)
			{
				role->AddExp(team_challenge_cfg->exp, ADD_EXP_TYPE_TASK);
			}

			if (team_challenge_cfg->immortal > 0)
			{
				role->GetRoleModuleManager()->GetMoney()->AddImmortalCoin(team_challenge_cfg->immortal, __FUNCTION__);
			}

			if (team_challenge_cfg->gold > 0)
			{
				role->GetRoleModuleManager()->GetMoney()->AddGold(team_challenge_cfg->gold, __FUNCTION__);
			}

			if (!role->GetRoleModuleManager()->GetKnapsack()->PutList(team_challenge_cfg->reward_vec.size(), &team_challenge_cfg->reward_vec[0], PUT_REASON_TEAM_CHALLENGE))
			{
				role->GetRoleModuleManager()->GetKnapsack()->SendMail(team_challenge_cfg->reward_vec);//放入失败，则邮件补发
			}

			role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)team_challenge_cfg->reward_vec.size(), &team_challenge_cfg->reward_vec[0], GET_REASON_TEAM_CHALLENGE_REWARD, id);
		}
	}

	/*if (task_cfg->task_id == TASKPOOL->GetTaskOtherCfg().award_surface_point_task_id)
	{
		role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnUseChangeCard(TASKPOOL->GetTaskOtherCfg().award_surface_seq, 1, false, __FUNCTION__);
	}*/
	if (task_cfg->task_id == LOGIC_CONFIG->GetHundredGodBossConfig()->GetOtherCfg().pre_task)
	{
		role->GetRoleModuleManager()->GetRoleHundredGodBoss()->OnStart();	
	}

	if (task_cfg->task_type == TASK_TYPE_FLY_UP_TASK || task_cfg->task_type == TASK_TYPE_FLY_UP_FRONT_TASK)
	{
		role->GetRoleModuleManager()->GetFlyUpSystem()->OnTaskFinish(task_cfg->task_type, id);
	}
	if (TASK_TYPE_PARTNER_SOAR_TASK == task_cfg->task_type && 0 == task_cfg->nex_task_id)	//表示一个伙伴的飞升任务
	{
		role->GetRoleModuleManager()->GetTaskManager()->OnPartnerSoarFinish();
	}

	if (task_cfg->task_type == TASK_TYPE_MAIN_QUEST ||
		task_cfg->task_type == TASK_TYPE_SIDE_QUEST ||
		task_cfg->task_type == TASK_TYPE_CHALLENGES ||
		task_cfg->task_type == TASK_TYPE_TEAM_CHALLENGES)
	{
		role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
	}

	if (TASK_NEED_KILL_MONSTERS == task_cfg->task_condition)
	{
		role->GetRoleModuleManager()->RemoveNeedMonster(task_cfg->param1);
	}
	else if (TASK_NEED_KILL_MONGROUP == task_cfg->task_condition || TASK_NEED_KILL_IN_ORDER == task_cfg->task_condition)
	{
		const std::vector<int>* monster_list = TASKPOOL->GetMonsterGroupMonsterList(task_cfg->param1);
		if (NULL != monster_list)
		{
			for (size_t k = 0; k < monster_list->size(); ++k)
			{
				role->GetRoleModuleManager()->RemoveNeedMonster((*monster_list)[k]);
			}
		}
	}
	else if (TASK_NEED_GAIN_TASKPROP == task_cfg->task_condition)
	{
		const DefeatMonsGroups * group_cfg = TASKPOOL->GetDefeatMonsGroups(task_cfg->param1);
		if (NULL != group_cfg)
		{
			std::set<int>::iterator it = group_cfg->drop_monster_list.begin();
			for (; it != group_cfg->drop_monster_list.end(); ++it)
			{
				role->GetRoleModuleManager()->RemoveNeedMonster(*it);
			}
		}
	}
	role->GetRoleModuleManager()->GetBountyTask()->OnTaskCommit(id);


	if (TASK_TYPE_PRESTIGE_TASK == task_cfg->task_type)
	{
		role->GetRoleModuleManager()->GetPrestigeTask()->OnTaskCommit(id);
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_CARRY_OUT_TYPE_TASK, COG_TASK_TYPE_PRESTIGE_TASK, 1);
	}
	else if (TASK_TYPE_ADVENTURES == task_cfg->task_type && 0 == task_cfg->nex_task_id)	//完成一整环才算
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_CARRY_OUT_TYPE_TASK, COG_TASK_TYPE_ADVENTURES_TASK, 1);
	}

	if (task_cfg->task_type == TASK_TYPE_CHALLENGES || task_cfg->task_type == TASK_TYPE_TEAM_CHALLENGES)
	{
		role->LogActive(LOG_ACTIVE_TYPE_CHALLENGE_TASK);
	}
}

void EventHandler::OnFinishAnecdoteFinalTask(Role * role, int anecdote_task_type)
{
	if (NULL == role || anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ANECDOTE_TASK, anecdote_task_type, 1);
	role->GetRoleModuleManager()->GetTaskManager()->OnFinishAnedoteGame(anecdote_task_type);

	if (ANECDOTE_TASK_TYPE_FIND_FOOD == anecdote_task_type)
	{
		role->LogActive(LOG_ACTIVE_TYPE_FIND_FOOD);
	}

	if (ANECDOTE_TASK_TYPE_REAL_FAKE_PAINTING == anecdote_task_type)
	{
		role->LogActive(LOG_ACTIVE_TYPE_REAL_FAKE_PAINTING);
	}

	if (ANECDOTE_TASK_TYPE_3X3_DIGITAL_KLOTSKI == anecdote_task_type)
	{
		role->LogActive(LOG_ACTIVE_TYPE_HUARONGDAO);
	}

	if (ANECDOTE_TASK_TYPE_POEMS_AND_SONGS == anecdote_task_type)
	{
		role->LogActive(LOG_ACTIVE_TYPE_POEMS_AND_SONGS);
	}

	if (ANECDOTE_TASK_TYPE_TREASURE_BOX_UNLOCK == anecdote_task_type)
	{
		role->LogActive(LOG_ACTIVE_TYPE_TREASURE_BOX_UNLOCK);
	}
}

void EventHandler::OnCompoundGem(Role * role, int item_id)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GEMS_MOSAIC, 1, item_id);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE, 0);
}

void EventHandler::OnCompoundItem(Role * role, int item_id, int num)
{
	if (NULL == role) return;
	
	const ItemBase * item_ib = ITEMPOOL->GetItem(item_id);
	if (NULL == item_ib) return;

	int item_type = item_ib->GetItemType();
	if (I_TYPE_EXPENSE == item_type)
	{
		NorexItem * norex = (NorexItem *)item_ib;
		if (NULL != norex)
		{
			switch (norex->GetUseType())
			{
			case NorexItem::I_NOREX_ITEM_HP_RECOVER:
			case NorexItem::I_NOREX_ITEM_RECOVER_MP:
			case NorexItem::I_NOREX_ITEM_REALIVE:
			case NorexItem::I_NOREX_ITEM_HP_PERCENT_RECOVER:
			case NorexItem::I_NOREX_ITEM_MP_PERCENT_RECOVER:
				role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_WORKSHOP_CRAFT_ITEM, COG_WORKSHOP_MAKE_POTION, num);
				role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
				break;
			}
		}
	}
	else if (I_TYPE_JEWELRY == item_type)
	{
		RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != god_token)
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_MAKE_JEWELTY);
		}
	}
}

void EventHandler::OnCompoundBook(Role * role)
{
	if (NULL == role) return;
	//role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_WORKSHOP_CRAFT_ITEM, COG_WORKSHOP_MAKE_BOOKS, 1);
}

void EventHandler::OnGemDismount(Role * role, int item_id)
{
	if(NULL == role) return;
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GEMS_MOSAIC, 2, item_id);
}

void EventHandler::OnCompoundCard(Role * role, int num)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_WORKSHOP_CRAFT_ITEM, COG_WORKSHOP_MAKE_CARD, num);
}

void EventHandler::OnEquipmentSmelt(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_EQUIPMENT_SMELT, 0, 1);
}

void EventHandler::OnEquipmentSmeltHigh(Role * role, int quality_type, int num)
{
	if (NULL == role) return;

	if (quality_type < ItemBase::I_COLOR_PURPLE || quality_type >= ItemBase::I_COLOR_MAX)
	{
		return;
	}

	RoleActivityRongLianYouLi * role_activity = static_cast<RoleActivityRongLianYouLi *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI));
	if (NULL != role_activity)
	{
		if (quality_type == ItemBase::I_COLOR_PURPLE)
		{
			role_activity->AddTaskScore(num);
		}
		else if (quality_type == ItemBase::I_COLOR_ORANGE)
		{
			role_activity->AddTaskScore(0, num);
		}
		else if (quality_type >= ItemBase::I_COLOR_RED)
		{
			role_activity->AddTaskScore(0, 0, num);
		}
		else
		{
			return;
		}
	}
}

void EventHandler::OnPartnerUpgradeLevel(Role * role, int id, int grade)
{
	if (NULL == role) return;
	if (role->GetRoleModuleManager()->GetCommonData().partner_pro_level < grade && grade > 0)
		role->GetRoleModuleManager()->GetCommonData().partner_pro_level = grade;
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PARTNER_UP_LEVEL, 1, grade);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PARTNER_PRO, grade);

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PARTNER_UPGRADE, grade);
	}
}

void EventHandler::OnActivateCollection(Role * role, int id, int page, int color)
{
	if (NULL == role) return;	

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACTIVATE_COLLECTION, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ACT_COLLECT, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_ACT_COLLECT_QUAILTY, 1, color);
}

void EventHandler::OnJoinGuild(Role * role, GuildID id)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_FIRST_ADD_GUILD, 0, 1);
	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
	//	guild->SendGuildWeekTaskInfo(role);		//下发修改为加入家族时,上线也会自动下发
		role->GetRoleModuleManager()->GetRoleGuildInborn()->OnSetGuildLevel(guild->GetGuildLevel());
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_JOIN_FAMILY, 1);
		role->GetRoleModuleManager()->GetTaskManager()->OnJoinGuild();
		role->GetRoleModuleManager()->GetTitle()->CalGuildMemberTitle(TITLE_NOTICE_TYPE_DEFAULT);	// 计算家族称号
	}

	WorldArena::Instance().UpdateUserSignupInfo(role);
}

void EventHandler::OnSetGuildPost(Role * role, int post)
{
	if (NULL == role) return;

}

void EventHandler::OnCommitGuildTask(Role * role)
{
	if (NULL == role) return;

}

void EventHandler::OnFetchGuildHongBao(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_FETCH_GUILD_HONGBAO, 0, 1);
}

void EventHandler::OnCorrectGuildAnswer(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_GUILD_ANSWER, 0, 1);
}

void EventHandler::OnCommitSchoolTask(Role * role, int task_index)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_CARRY_OUT_TYPE_TASK, COG_TASK_TYPE_PROF_TASK, 1);
	role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_SCHOOL_TASK, 1, __FUNCTION__);

	RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL != lucky_turn_table)
	{
		lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_SCHOOL_TASK, 1);
	}


	// 队友一起提交
	Team* team = role->GetMyTeam();
	if (NULL != team && role->GetUID() == team->GetLeaderUID())
	{
		int member_count = team->GetMemberCount();
		for (int i = 0; i < member_count; ++i)
		{
			Role* member_role = team->GetMemberRoleByIndex(i);
			if (NULL != member_role && member_role->GetUID() != role->GetUID())
			{
				member_role->GetRoleModuleManager()->GetSchoolTask()->OnTaskReq(task_index);
			}
		}
	}
}

void EventHandler::OnCommitTaskChain(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_FINISHED_TASK_CHAIN, 0, 1);
	role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge();
}

void EventHandler::OnAddFriend(Role * role, int now_friend_count)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_HAS_FRIEND_COUNT, 0, 1);

	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq,AUTO_TITLE_TYPE_1);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 &&seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_1; round++,seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_1)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 <= now_friend_count)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id,"AutoTitleAdd");
		}
	}
}

void EventHandler::OnFriendIntimacyChange(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_HAS_FRIEND_COUNT, 0, 1);
	role->GetRoleModuleManager()->GetTitle()->CalQingYuanTitle();
}

void EventHandler::OnIntimacyAddBySend(Role * role, int to_uid, int _add_val)
{
	if (NULL == role)return;

	RoleActivityCharismaticTanabata * role_activity = (RoleActivityCharismaticTanabata *)role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
	if (NULL != role_activity)
	{
		role_activity->OnSendRoleFlasw(_add_val);
	}

	Role * to_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(to_uid));
	if (NULL != to_role)
	{
		RoleActivityCharismaticTanabata * to_role_activity = (RoleActivityCharismaticTanabata *)to_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
		if (NULL != to_role_activity)
		{
			to_role_activity->OnRoleSendFlows(_add_val);
		}
	}
	else//找不到收花人
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			//发往原服
			crossgameprotocal::HiddenGameFriendInitmacy req;
			req.to_uid = to_uid;
			req.add_val = _add_val;
			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(to_uid).db_index, (const char*)&req, sizeof(req));
		}
		else
		{
			//发往跨服
			crossgameprotocal::GameHiddenFriendInitmacy req;
			req.to_uid = to_uid;
			req.add_val = _add_val;
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
		}
	}
}

void EventHandler::OnTradeMarketPutaway(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_TRADEMARKET_PUTAWAY, 0, 1);
}

void EventHandler::OnTradeMarketPutawayHour(Role * role)
{
}

void EventHandler::OnTradeMarketPurchase(Role* role, ItemID buy_item_id, int consume_gold)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_LEVEL, 0, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_TRADEMARKET_DEALING, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_TRA_MAR_PUR, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN, 1);
	role->GetRoleModuleManager()->GetTradeCredit()->OnBuyFromTradeMarket(consume_gold);

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_MARKET);
	}

	RoleActivityTradeMarketReturnGold * ra_return_gold = static_cast<RoleActivityTradeMarketReturnGold *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN));
	if (NULL != ra_return_gold)
	{
		ra_return_gold->OnTradeMarketBuyItemSucc(consume_gold);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->ExchangeShop();
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		//图鉴夺宝活动
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
		{
			RoleActivityTuJianTreasure * tu_jian = static_cast<RoleActivityTuJianTreasure *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
			if (NULL != tu_jian)
			{
				tu_jian->OnTujianTreasureBuyCard(buy_item_id, 1);
			}
		}
	}

	
}

void EventHandler::OnTradeMarketSaleItem(int seller_uid, ItemID sel_item_id, bool is_myself)
{
	if (is_myself)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(seller_uid));
		if (NULL != role)
		{
			//都自己买自己东西了，肯定不能为空
			//目前自己买自己东西只对图鉴夺宝活动有效
			RoleActivityTuJianTreasure * tu_jian = static_cast<RoleActivityTuJianTreasure *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
			if (NULL != tu_jian)
			{
				tu_jian->OnTujianTreasureBuyCard(sel_item_id, 0);
			}
		}
		return;
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(seller_uid));
	if (NULL == role)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::HiddenGameTradeMarketSaleItemEventHandler hgtmsieh;
			hgtmsieh.uid = seller_uid;
			hgtmsieh.sel_item_id = sel_item_id;
			hgtmsieh.reserve_sh = 0;

			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(seller_uid).db_index, (const char *)&hgtmsieh, sizeof(hgtmsieh));
		}
		else
		{
			crossgameprotocal::GameHiddenTradeMarketSaleItemEventHandler ghtmsieh;
			ghtmsieh.uid = seller_uid;
			ghtmsieh.sel_item_id = sel_item_id;
			ghtmsieh.reserve_sh = 0;

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghtmsieh, sizeof(ghtmsieh));
		}
		return;
	}
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_LEVEL, 0, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_TRADEMARKET_DEALING, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_TRA_MAR_PUR, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN, 1);

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_MARKET);
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		//图鉴夺宝活动
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
		{
			RoleActivityTuJianTreasure * tu_jian = static_cast<RoleActivityTuJianTreasure *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
			if (NULL != tu_jian)
			{
				tu_jian->OnTujianTreasureBuyCard(sel_item_id, 0);
			}
		}
	}
}

void EventHandler::OnTradeMarketOffline(Role * role, int add_num, const ItemID * item_list)
{
	if (NULL == role || add_num <= 0 || NULL == item_list) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_LEVEL, 0, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_TRADEMARKET_DEALING, 0, add_num);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_TRA_MAR_PUR, add_num);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN, add_num);

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_MARKET, add_num);
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr)
	{
		//图鉴夺宝活动
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE))
		{
			for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
			{
				if (0 == item_list[i]) continue;
				RoleActivityTuJianTreasure * tu_jian = static_cast<RoleActivityTuJianTreasure *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE));
				if (NULL != tu_jian)
				{
					tu_jian->OnTujianTreasureBuyCard(item_list[i], 0);
				}
			}
		}
	}
}

void EventHandler::OnPassFBCourageChallenge(Role * role)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COUR_CHAG, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COURAGE_CHALLENGE, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACHS_FB_TIER_NUMB, FBManager::FB_TYPE_COURAGE_CHALLENGE, 1);
	role->GetRoleModuleManager()->GetTaskManager()->OnPassCourageChallenge();

	{
		// 职业晋级
		int seq = role->GetRoleModuleManager()->GetCourageChallenge()->GetPassMaxSeq();
		role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_GU_MO_ZHI_TA, seq, 0);
	}

	//锢魔之塔调整,以下关联内容暂时屏蔽
	//role->GetRoleModuleManager()->GetRoleActivity()->OnPassCourageChallenge(old_layer,layer);
	//role->GetRoleModuleManager()->OnAddGuildEvent(layer, 0, GUILD_EVENTHANDLER_TYPE_COURAGE_CHALLENGE);
	//this->OnRoleClearFB(role, FBManager::FB_TYPE_COURAGE_CHALLENGE);
}

void EventHandler::OnEnterCourageChallenge(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_COURAGE_CHALLENGE, 1, __FUNCTION__);

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_TOWER);
	}
}

void EventHandler::OnPassCrystalEctype(Role * role, int diff_type, int monster_id, int count/* = 1*/)
{
	if (NULL == role) return;

	this->OnRoleClearFB(role, FBManager::FB_TYPE_CRYSTAL_FB, diff_type, count);
	
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_JOIN_XIYOUFB, 1);

	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		if (diff_type != ROLE_CRYSTAL_FB_TYPE_XI_YOU)guild->OnGuildWeekTaskEvent(role->GetUserId(), GWT_COND_PLAY_COPY_CRYSTAL, 0);
	}
	if (diff_type != ROLE_CRYSTAL_FB_TYPE_XI_YOU)
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PASS_CRYSTAL_ECTYPE, 0, 1);
		RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != god_token)
		{
			god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB, count);
			god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB, count);
		}

		RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
		if (NULL != cool_breeze)
		{
			cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_PASS_CRYSTAL, count, 0);
		}

		//竞技月卡
		RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
		if (NULL != yueka_activity)
		{
			yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_CRYSTAL, count);
		}

	}

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		if (diff_type != ROLE_CRYSTAL_FB_TYPE_XI_YOU) back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PASS_FB, count);
		if (diff_type == ROLE_CRYSTAL_FB_TYPE_XI_YOU) back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PASS_ELITEFB, count);
	}
}

void EventHandler::OnKillCrystalBoss(Role * role, int boss_id, std::vector<ItemConfigData>* drop_list, bool is_has_activity_reward)
{
	if(NULL == role) return;

	if (is_has_activity_reward && NULL != drop_list)
	{
		role->GetRoleModuleManager()->GetRoleActivityManager()->OnGiveRoleActivityItem(ACTIVITY_GIVE_ITEM_TYPE_CRYSTAL_BOSS, boss_id, drop_list);
	}
}

void EventHandler::OnUseCrystalEnergy(Role * role, int num, int enter_energy_change_type)
{
	if (NULL == role) return;
	if (0 >= num) return;
	
	if (ENTER_ENERGY_CHANGE_TYPE_ADVANCE_CRYSTAL != enter_energy_change_type && ENTER_ENERGY_CHANGE_TYPE_ADVANCE_DREAM_NOTES != enter_energy_change_type)
	{
		this->OnSpecialUseEnergy(role, num, enter_energy_change_type);
	}
}

void EventHandler::OnSpecialUseEnergy(Role * role, int num, int enter_energy_change_type)
{
	if (NULL == role || 0 >= num) return;
	
	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_ENERGY, num, __FUNCTION__);
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_ENERGY, num);
	role->GetRoleModuleManager()->GetRoleTianDiDaoHen()->UseCrystalEnergy(num);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_USE_CRYSTAL_ENERGY, 0, num);
	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB_TI_LI, num);
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB_TI_LI, num);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->UseCrystalPower(num);
	}

	RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL != cool_breeze)
	{
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_CONSUME_ENERGY, num, 0);
	}

	{
		RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
		if (NULL != nnyy_activity)
		{
			nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_CONSUME_TI_LI, num);
		}
	}

	{
		RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_CONSUME_TI_LI, num);
		}
	}

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_CONSUME_TI_LI, num);
		}
	}

	{
		RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_CONSUME_TI_LI, num);
		}
	}

	{
		RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_CONSUME_TI_LI, num);
		}
	}

	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_USE_CRYSTAL_FB_ENERGY, num);
	}
}

void EventHandler::OnPassMedalEctype(Role * role)
{
	if(NULL == role) return;

	this->OnRoleClearFB(role, FBManager::FB_TYPE_MEDAL);
}

void EventHandler::OnPassChallengeEctype(Role * role, int fb_id, int chapter)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleChallengeFb()->OnPassChallengeFb(fb_id, chapter);
	this->OnRoleClearFB(role, FBManager::FB_TYPE_CHALLENGE_FB);
}

void EventHandler::OnPassGuideEctype(Role * role)
{
	if(NULL == role) return;
	
	this->OnRoleClearFB(role, FBManager::FB_TYPE_GUIDE_FB);
}

void EventHandler::OnPassFantasyEctype(Role * role)
{
	if (NULL == role) return;

	this->OnRoleClearFB(role, FBManager::FB_TYPE_FANTASY_FB);
}

void EventHandler::OnPassMazeRacerEctype(Role * role, int maze_id)
{
	if (NULL == role) return;

	this->OnRoleClearFB(role, FBManager::FB_TYPE_MAZE_RACER);
}

void EventHandler::OnPassMiYiFB(Role * role, int pass_level, int pass_index)
{
	if (role == NULL)return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_MIYI_KILL_LAYER_NUM, pass_level, 1);    //怕当前有玩家已经拿了首杀,导致无法完成成就

	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_2);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_2; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_2)
		{
			break;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 == pass_level && cfg_node->cond_param2 == pass_index)   
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnEnterPetMonopoly(Role * role, int level)
{
	if(NULL == role || level <= 0) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_MONOPOLY_LEVEL, 0, level);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_MONOPOLY, 1);
}

void EventHandler::OnPassLunHuiWangChuan(Role * role, int level)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_LUN_HUI_WANG_CHUAN, 0, 0);
	role->GetRoleModuleManager()->GetTaskManager()->OnPassLunHuiWangChuan();
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_LUN_HUI_WANG_CHUAN, 1);
	role->GetRoleModuleManager()->GetDuJie()->ChangeTaskParam(DU_JIE_TASK_TYPE_LUN_HUI_WANG_CHUAN, 0);
	role->GetRoleModuleManager()->GetFuncGuide()->OnPassLunHuiWangChuan(level);

	{
		// 职业晋级
		int idx = (role->GetRoleModuleManager()->GetLunHuiWangChuan()->GetPassIDX() <= 0) ? 0 : role->GetRoleModuleManager()->GetLunHuiWangChuan()->GetPassIDX() - 1;
		role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_LUN_HUI_WANG_CHUAN, idx, 0);
	}

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PASS_LHWC);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_LUN_HUI_WANG_CHUAN, 1);
	}
}

void EventHandler::OnPassShanHai(Role * role, int grade_type)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_SHAN_HAI_XIANG_YAO, 0, 0);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_SHAN_HAI_BOSS, grade_type, 1);
	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_SHAN_HAI_BOSS, 1, __FUNCTION__);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_KILL_SHAN_HAI_BOSS, 1, grade_type);
	role->GetRoleModuleManager()->GetDuJie()->ChangeTaskParam(DU_JIE_TASK_TYPE_SHAN_HAI_BOSS, grade_type);
	role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_SHANG_HAI_BOSS_SUCC, role->GetRoleModuleManager()->GetPassShanHaiTimes(), 0);

	role->LogActive(LOG_ACTIVE_TYPE_SHAN_HAI_BOSS);
}

void EventHandler::OnKillMiYiFBBoss(Role * role, int level_seq, int boss_index, bool is_first)
{
	if(NULL == role) return;
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_TRIAL_BOSS, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PASS_MI_YI_BOSS, 1);

	if (is_first)
	{
		role->GetRoleModuleManager()->OnAddGuildEvent(level_seq, 0, GUILD_EVENTHANDLER_TYPE_MIYI);					
		role->GetRoleModuleManager()->GetTaskManager()->OnPassKillMiYiBoss(level_seq);
	}

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_MIYI_KILL_LAYER_NUM, level_seq, 1);
	role->GetRoleModuleManager()->GetTaskManager()->OnPassKillMiYiBoss(level_seq);
	role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_YUAN_ZHENG_BOSS, level_seq, boss_index);


	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_2);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_2; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_2)
		{
			break;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 == level_seq && cfg_node->cond_param2 == boss_index)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}

}

void EventHandler::OnPassMaterial(Role * role, int chapter_id, int level_seq, bool is_first)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnPassMaterial(chapter_id);

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->PassCrystal();
	}

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB, 1);
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB, 1);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_MATERIAL, 1);
	}

	role->LogActive(LOG_ACTIVE_TYPE_MATERIAL_FB);
}

void EventHandler::OnPassBigDipper(Role * role, int sn)
{
	if(NULL == role || sn < 0) return;

	role->GetRoleModuleManager()->OnBigDipperFightSucc();
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_NTHER_WORLD_PURGATORY, 1, sn);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_NTHER_WORLD_PURGATORY_ALL, 1, sn);
	role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_BIG_DIPPER_SUCC, role->GetRoleModuleManager()->GetBigDipperFightSuccTimes(), 0);

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_BIG_DIPPER, 1);
	}
}

void EventHandler::OnPassNetherWorldPurgatory(Role * role, int pass_chapters, int challenge_id)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_NETHER_WORLD_PURGATORY, pass_chapters, challenge_id);
}

void EventHandler::OnPassYaoXingShiLian(Role * role, int pass_seq)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_YAO_XING_SHI_LIAN, pass_seq, 0);
}

void EventHandler::OnPassFBNewCourageChallenge(Role * role)
{
	if(NULL == role) return;

	{
		// 职业晋级
		int seq = role->GetRoleModuleManager()->GetNewCourageChallenge()->GetPassMaxSeq();
		role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_SHI_SHEN_ZHI_TA, seq, 0);
	}
}

void EventHandler::OnPassMirageTerritory(Role * role, int area, int pass_seq)
{
	if(NULL == role) return;

	{
		// 职业晋级
		int mt_min_top_seq = role->GetRoleModuleManager()->GetMirageTerritory()->GetMinAllTopBattleSeq();
		role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_MIRAGE_TERRITORY, mt_min_top_seq, 0);
	}
}

void EventHandler::OnPassHongMengTianJie(Role * role, int fb_id, bool is_first)
{
	if(NULL == role) return;

	if (is_first)
	{
		role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_HONG_MENG_TIAN_JIE, fb_id, 0);
	}
}

void EventHandler::OnRealKillMiYiFBBoss(Role * role)
{
	if (NULL == role) return;

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_MI_YI);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->ExpeditionModron();
	}
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_MI_YI, 1);
}

void EventHandler::OnParticipateWorldBoss(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_DEFEAT_BOSS_MONSTER, COG_BOSS_WORLD, 1);
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_WORLD_BOSS, 1);
	role->LogActive(LOG_ACTIVE_TYPE_WORLD_BOSS);

	{
		RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
		if (NULL != nnyy_activity)
		{
			nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

}

void EventHandler::OnParticipateWorldBoss2(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_DEFEAT_BOSS_MONSTER, COG_BOSS_WORLD, 1);
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_WORLD_BOSS_2, 1);
	role->LogActive(LOG_ACTIVE_TYPE_WORLD_BOSS_2);
}

void EventHandler::OnParticipateWorldBoss3(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_DEFEAT_BOSS_MONSTER, COG_BOSS_WORLD, 1);
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_WORLD_BOSS_2, 1);
	role->LogActive(LOG_ACTIVE_TYPE_WORLD_BOSS_3);
}

void EventHandler::OnEnterBraveGround(Role * role)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_REACHS_FB_TIER_NUMB, FBManager::FB_TYPE_BRAVE_GROUND, 1);
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_BRAVE_GROUND, 1);

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	role->LogActive(LOG_ACTIVE_TYPE_BRAVE_GROUND);
}

void EventHandler::OnEnterTeamFight(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_TEAM_FIGHT, 1);
	role->LogActive(LOG_ACTIVE_TYPE_TEAM_FIGHT);
}

void EventHandler::OnEnterMazeRacer(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_MAZE_RACER, 1);
	role->LogActive(LOG_ACTIVE_TYPE_MAZE_RACE);
}

void EventHandler::OnEnterGuildFight(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_GUILD_FIGHT, 1);
	role->LogActive(LOG_ACTIVE_TYPE_GUILD_FIGHT);
}

void EventHandler::OnParticipateChiefElection(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_CHIEF_ELECTION, 1);
	role->LogActive(LOG_ACTIVE_TYPE_CHIEF_ELECT);
}

void EventHandler::OnEnterGuildHonorBattle(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_GUILD_HONOR_BATTLE, 1);
	role->LogActive(LOG_ACTIVE_TYPE_GUILD_HONOR);
}

void EventHandler::OnEnterGuildAnswer(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_GUILD_ANSWER, 1);

	role->LogActive(LOG_ACTIVE_TYPE_GUILD_QUESTION);
}

void EventHandler::OnEnterOnlyFight(Role * role)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_ONLY_FIGHT, 1);
	role->LogActive(LOG_ACTIVE_TYPE_ONLY_FIGHT);
}

void EventHandler::OnParticipateHuanJieZhanChange(Role * role)
{
	if (NULL == role) return;

	role->LogActive(LOG_ACTIVE_TYPE_HUANJIE_BATTLE);
}

void EventHandler::OnParticipateHundredGhost(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_HUNDRED_GHOST, 1);

	{
		RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
		if (NULL != nnyy_activity)
		{
			nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}
}

void EventHandler::OnParticipateSilkRoads(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_SILK_ROADS, 1);

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}
}

void EventHandler::OnParticipateRelicLooting(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddWeekDegreeParam(ACTIVE_DEGREE_TYPE_RELIC_LOOTING, 1);
}

void EventHandler::OnParticipateWorldTeamArena(Role * role)
{
	if (NULL == role) return;
}

void EventHandler::OnPassDreamNotesChallenge(Role * role, int level, bool is_first, bool is_fast)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnPassDreamNotesChallenge(level);
	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_DREAM_NOTES_CHALLENGE, 1);	//首通活跃进度也要改变
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_DREAM_NOTES_CHALLENGE, 1);	
	if (is_fast)
	{
		role->GetRoleModuleManager()->GetFuncGuide()->OnFastDreamNotesChallenge(level);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_FAST_PASS_DREAM_NOTES, 1);
	}

	{
		// 职业晋级
		int level = role->GetRoleModuleManager()->GetDreamNotesChallenge()->GetPassLevel();
		role->GetRoleModuleManager()->GetAdvanceCareer()->CheckAdvanceCond(ADVANCE_COND_TYPE_MENG_YUAN, level, 0);
	}

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB, 1);
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB, 1);
	}

	RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL != lucky_turn_table)
	{
		lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_DREAM_NORES_CHALLENGE, 1);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_DREAM_NOTES_CHALLENGE, 1);
	}

	role->LogActive(LOG_ACTIVE_TYPE_DREAM_NOTE);
}

void EventHandler::OnBraveGroundPassNewLevel(Role * role, int level)
{
	if(NULL == role || level < 0) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_BRAVE_GROUD_PASS, level);
}

void EventHandler::OnBraveGroundPassLevelChange(Role * role, int level)
{
	if(NULL == role || level < 0) return;

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_BRAVE_GROUND, level);
	}
}

void EventHandler::OnHuanJieZhanChanDataChange(Role * role, int rank_score)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_HUAN_JIE_ZHAN_CHANG, rank_score);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_HUAN_JIE_ZHAN_CHANG, 0, rank_score);
}

void EventHandler::OnRAGodTokenFinishLimitActivity(Role * role, int activity_type)
{
	if (NULL == role || ACTIVITY_TYPE_INVALID >= activity_type || activity_type >= ACTIVITY_TYPE_MAX) return;

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_LIMIT_ACTIVITY, 1, activity_type);
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_LIMIT_ACTIVITY, 1, activity_type);
	}
}

void EventHandler::OnParticipateLimitActivity(Role * role, int activity_type, const char * reason_str, int param1)
{
	if (NULL == role) return;

	CommonDataParam& common_data = role->GetRoleModuleManager()->GetCommonData();
	if (ACTIVITY_TYPE_BRAVE_GROUND != activity_type && common_data.participate_activity_flag.IsBitSet(activity_type))		//限时活动一天不会开两次相同的活动
	{
		return;
	}

	//新增限时活动请在添加一下,防止相关逻辑BUG
	UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX == 19);
	switch (activity_type)
	{
	case ACTIVITY_TYPE_QING_YUAN_SHENG_HUI:	//屏蔽
		break;
	case ACTIVITY_TYPE_RELIC_LOOTING:	
	{
		this->OnParticipateRelicLooting(role);
	}
	break;
	case ACTIVITY_TYPE_WORLD_BOSS:
	{
		this->OnParticipateWorldBoss(role);
	}
	break;
	case ACTIVITY_TYPE_TEAM_FIGHT:
	{
		this->OnEnterTeamFight(role);
	}
	break;
	case ACTIVITY_TYPE_MAZE_RACER:
	{
		this->OnEnterMazeRacer(role);
	}
	break;
	case ACTIVITY_TYPE_BRAVE_GROUND:		//勇闯地宫目前不属于限时活动(周常)
	{
		this->OnEnterBraveGround(role);
		return;
	}
	break;
	case ACTIVITY_TYPE_GUILD_FIGHT:
	{
		this->OnEnterGuildFight(role);
	}
	break;
	case ACTIVITY_TYPE_CHIEF_ELECTION:
	{
		this->OnParticipateChiefElection(role);
	}
	break;
	case ACTIVITY_TYPE_CHIEF_PEAK_BATTLE:	//屏蔽
	{
		return;
	}
	break;
	case ACTIVITY_TYPE_GUILD_HONOR_BATTLE:
	{
		this->OnEnterGuildHonorBattle(role);
	}
	break;
	case ACTIVITY_TYPE_GUILD_ANSWER:
	{
		this->OnEnterGuildAnswer(role);
	}
	break;
	case ACTIVITY_TYPE_WORLD_BOSS_2:
	{
		this->OnParticipateWorldBoss2(role);
	}
	break;
	case ACTIVITY_TYPE_ONLY_FIGHT:
	{
		this->OnEnterOnlyFight(role);
	}
	break;
	case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
	{
		this->OnParticipateHuanJieZhanChange(role);
	}
	break;
	case ACTIVITY_TYPE_HUNDRED_GHOST:
	{
		this->OnParticipateHundredGhost(role);
	}
	break;
	case ACTIVITY_TYPE_SILK_ROADS:
	{
		this->OnParticipateSilkRoads(role);
	}
	break;
	case ACTIVITY_TYPE_WORLD_TEAM_ARENA:
	{
		this->OnParticipateWorldTeamArena(role);
	}
	break;
	case ACTIVITY_TYPE_WORLD_BOSS_3:
	{
		this->OnParticipateWorldBoss3(role);
	}
	break;
	default:
		return;
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->JoinTimeLimitActivity();
	}

	{
		RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
		if (NULL != nnyy_activity)
		{
			nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	{
		RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_XIAN_SHI_HUO_DONG);
		}
	}

	this->OnRAGodTokenFinishLimitActivity(role, activity_type);
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnPutOutSignUpReward(activity_type);
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnParticipateLimitActivity(activity_type);
	WorldStatus::Instance().SendNoticeLog(role->GetUID(), role->GetName(), role->GetPlatName(), activity_type, param1, reason_str);
	common_data.participate_activity_flag.SetBit(activity_type);

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PARTICIPATE_ACT, activity_type, 1);
	role->GetRoleModuleManager()->GetRoleJoinActivityReward()->RecodeJoinInfo(activity_type);
	role->GetRoleModuleManager()->GetActiveSystem()->OnCompleteTurntableTask(ACTIVE_TURNTABLE_TYPE_JOIN_ACTIVITY, activity_type);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PAR_LIMIT_ACTIVITY, 1);
}

void EventHandler::OnFinishLevelComplementRelatedFun(Role * role, long long add_exp, const char *reason)
{
	if(NULL == role || add_exp <= 0) return;

	role->GetRoleModuleManager()->GetRoleNewLevelComplement()->OnAddExp(add_exp, reason);
}


void EventHandler::OnWinChiefElection(Role * role, int prof)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_WIN_CHIEF_ELECTION, 0, 1);
}

void EventHandler::OnRoleFinishActivityTeamFight(Role * role, int rank)
{
	if (role == NULL)return;

	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_3);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_3; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_3)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 <= rank)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnGoldShopConsume(Role * role, int consume_num)
{
	if (role == NULL)return;

}

void EventHandler::OnNewShopConsume(Role * role, int consume_num)
{
	if (NULL == role) return;
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_SHOP_CONSUME,consume_num);
}

void EventHandler::OnOpenTreasureChest(Role * role)
{
	if (role == NULL)return;

	int &open_treasure_chest_times = role->GetRoleModuleManager()->GetCommonData().open_treasure_chest_times;
	open_treasure_chest_times += 1;
	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_8);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_8; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_8)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 <= open_treasure_chest_times)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnTueasureHunt(Role * role, int count, int treasure_type)
{
	if (role == NULL)return;

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_XUN_BAO, count);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->TreasureHunt(count);
	}
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_XUN_BAO, treasure_type, count);

	int &treasure_hunt_times = role->GetRoleModuleManager()->GetCommonData().treasure_hunt_times;
	treasure_hunt_times += count;
	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_9);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_9; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_9)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 <= treasure_hunt_times)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnRecaleReason(Role * role, int recale_reason, bool is_init)
{
	if (is_init || NULL == role) return;//初始化不重算

	if (recale_reason == AttributeList::RECALC_REASON_SELF)// 人物等级属性->法宝属性->宠物属性
	{
		role->GetRoleModuleManager()->GetPet()->ReCalcAttrByFaBao();
	}

	if (recale_reason == AttributeList::RECALC_REASON_ATTRIBUTE_PLAN)
	{
		role->GetRoleModuleManager()->GetHolyEquipManager()->ReCalAllHolyBaseAttr();
	}

	role->GetRoleModuleManager()->GetRoleActivityManager()->CalcRoleCap();
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_USER_CAPABIL,1);
	if (!is_init)
	{
		role->UpdateCacheNode();
	}

	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_CAPABILITY);
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_CAPABILITY + role->GetAppearanceProf());
	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP);

	ColosseumManager::Instance().SyncUserData(role);
}

void EventHandler::OnSwitchPetToFight(Role* role)
{
	if (NULL == role) return;

	int fight_pet_idx = role->GetRoleModuleManager()->GetPet()->GetFightIndex();
	if (-1 != fight_pet_idx)
	{
		const PetParam* param = role->GetRoleModuleManager()->GetPet()->GetPetParam(fight_pet_idx);
		if (NULL != param)
		{
			if (param->pet_auto_move_type > 0)
			{
				role->SetBattleAutoMove(false, param->pet_auto_move_type, param->pet_auto_skill_id, param->pet_auto_skill_level, false, true);
				return;
			}
			else
			{
				role->SetBattleAutoMove(false, BATTLE_MOVE_TYPE_NORMAL_ATTACK, 0, 0, false, true, true);
			}
		}

		
		ColosseumManager::Instance().SyncUserData(role);
	}
	
	role->SetBattleAutoMove(false, BATTLE_MOVE_TYPE_NORMAL_ATTACK, 0, 0, false, true);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnFightPetToStandby(Role* role)
{
	if (NULL == role) return;

	role->SetBattleAutoMove(false, BATTLE_MOVE_TYPE_NORMAL_ATTACK, 0, 0, false, true);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnFightPetRest(Role* role)
{
	if (NULL == role) return;

	role->SetBattleAutoMove(false, BATTLE_MOVE_TYPE_NORMAL_ATTACK, 0, 0, false, true);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnPetAllotAttrpoint(Role * role, int con, int str, int def, int agi, int mgc)
{
	int total = con + str + def + agi + mgc;
	if (NULL == role || 0 == total) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_ALLOT_ATTRPOINT, 0, total);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnPetUpLevel(Role * role, int pet_index, int pet_level)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_NUM_REACH_LEVEL, 0, 0);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnPetAbandon(Role * role, int pet_quality)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_NUM_REACH_LEVEL, 0, 0);
	role->GetRoleModuleManager()->GetTaskManager()->OnPetRelease();
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_SPECIAL_PET_ABANDON, 1, pet_quality);
}

void EventHandler::OnPetCompose(Role * role, bool is_compose)
{
	if(NULL == role) return;

	if (is_compose)
	{
		role->GetRoleModuleManager()->GetTaskManager()->OnPetCompose();
	}
}

void EventHandler::OnPetIntensi(Role * role, int pet_index, int pet_quality, int pet_level)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_TIMES, 0, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_LEVEL, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_SPECIAL_PET_INTEN, pet_quality, pet_level);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN_NUMBER, 1, pet_quality);

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_PET_BREAKTHROUGHS);
	}

	RoleActivityPetLoot * pet_loot = static_cast<RoleActivityPetLoot *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_PET_LOOT));
	if (NULL != pet_loot)
	{
		pet_loot->OnPetIntensi(pet_quality, pet_level);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->StrengThenPet();
	}

	RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL != cool_breeze)
	{
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_NUM, 1, 0);
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_LEVEL, pet_level, 0);
	}

	RoleActivityBackReward * back_reward = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL != back_reward)
	{
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PET_INTENSI, 1);
		back_reward->CheckTaskRate(RA_BACK_REWARD_TYPE_PET_INTENSI_LEVEL);
		back_reward->OnParticipateRA(__FUNCTION__);
	}
	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_PET_INTEN, 1);
	}
}

void EventHandler::OnPetRefined(Role * role, int pet_index, int pet_quality, bool is_mutant)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_SCOUR_TALENTS, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_REFINED, 1, pet_quality);
	if (is_mutant)
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_SCOUR_TALENTS_MUT, 0, 1);
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_SCOUR_TALENTS_MUT, pet_quality, 1);
	}
}

void EventHandler::OnPetChangeTalent(Role * role, int pet_index, PetParam * pet_param)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_REPLACE_TALENTS, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnUseSkillBook(Role * role, int pet_index, int replace_gift_index, int skill_id, int pet_quality, int skill_type)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_USE_BOOK,1, pet_quality, skill_type);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_LEARN_PET_SKILL, skill_type, 1);


	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_PET_LEARN_SKILL);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->OnPetLearnSkill();
	}

	RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL != cool_breeze)
	{
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_PET_SKILL_LEARN, 1, 0);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_LEARN_PET_SKILL, 1);
	}

}

void EventHandler::OnComprehendPassiveSkill(Role * role, int pet_index, PetParam * pet_param, int f_index)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_SKILL_NUM, 1, f_index + 1);
	RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL != cool_breeze)
	{
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_PET_SKILL_LEARN, 1, 0);
	}
}

void EventHandler::OnDeletePet(Role * role, long long unique_id)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PET_INTENSIFY_LEVEL, 0, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_INTEN, 1);
	role->GetRoleModuleManager()->GetHolyEquipManager()->DeleteBlessPet(unique_id);
	PlatformBattle::Instance().OnDelPet(role, unique_id);
}

void EventHandler::OnPetChangeName(Role * role, int pet_index, GameName name)
{
	if(NULL == role) return;

	RankManager::Instance().SyncPetRankInfo(role, PET_RANK_TYPE_CAPABILITY, pet_index, true);
}

void EventHandler::OnPetMoveStorage(Role * role, long long unique_id)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnPetStrorageMoveBag(Role * role, long long  unique_id)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnPetPutEquipment(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_PET_EQUIP, 0);
}

void EventHandler::OnPetTakeOffEquipment(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
}

void EventHandler::OnAddPetBenediction(Role * role, long long vice_item_unique_id)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetHuaShenManager()->AddPetBendiction(vice_item_unique_id);
}

void EventHandler::OnPetLianYao(Role * role, int pet_quality)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PET_LIAN_YAO, 1, pet_quality);
}

void EventHandler::OnPetPutSoulEquip(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_SOUL_EQUIP, 0);
}

void EventHandler::OnPetBaseInfoChange(Role* role, int pet_index)
{
	if (NULL == role) return;

	if (pet_index == role->GetRoleModuleManager()->GetPet()->GetFightIndex())
	{
		role->GetRoleModuleManager()->GetPet()->SendFightPetInfoToTeamMember();
	}
}

bool EventHandler::MonsterIsLockNotice(Role * role, int battle_type)
{
	if (role == NULL)
	{
		return false;
	}

	if (battle_type == BATTLE_MODE_FIELD_BOSS)
	{
		Protocol::SCWildBossNotice protocol;
		protocol.notice_type = Protocol::WILD_BOSS_NOTICE_MONSTER_BATTLE;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

		return true;
	}

	return false;
}

bool EventHandler::TeamNotEnoughMembers(Role * role, int battle_type)
{
	if (role == NULL)
	{
		return true;
	}

	if (battle_type == BATTLE_MODE_FIELD_BOSS)
	{
		Protocol::SCWildBossNotice protocol;
		protocol.notice_type = Protocol::WILD_BOSS_NOTICE_NOT_ENOUGH_MEMBERS;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

		return true;
	}

	if (battle_type == BATTLE_MODE_WORLD_BOSS)
	{
		role->NoticeNum(errornum::EN_WORLD_BOSS_NOT_ENOUGH_TEAM_MEMBER);
		return true;
	}

	if (battle_type == BATTLE_MODE_WORLD_BOSS_2)
	{
		role->NoticeNum(errornum::EN_WORLD_BOSS_2_NOT_ENOUGH_TEAM_MEMBER);
		return true;
	}

	if (battle_type == BATTLE_MODE_WORLD_BOSS_3)
	{
		role->NoticeNum(errornum::EN_WORLD_BOSS_3_NOT_ENOUGH_TEAM_MEMBER);
		return true;
	}

	if (battle_type == BATTLE_MODE_SHAN_HAI_BOSS)
	{
		role->NoticeNum(errornum::EN_SHAN_HAI_BOSS_NOT_ENOUGH_TEAM_MEMBER);
		return true;
	}

	return false;
}

bool EventHandler::NotInTeam(Role * role, int battle_type)
{
	if (role == NULL)
	{
		return false;
	}

	if (battle_type == BATTLE_MODE_FIELD_BOSS)
	{
		Protocol::SCWildBossNotice protocol;
		protocol.notice_type = Protocol::WILD_BOSS_NOTICE_NOT_ENOUGH_MEMBERS;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

		return true;
	}

	if (battle_type == BATTLE_MODE_WORLD_BOSS)
	{
		role->NoticeNum(errornum::EN_WORLD_BOSS_NOT_ENOUGH_TEAM_MEMBER);

		return true;
	}

	if (battle_type == BATTLE_MODE_WORLD_BOSS_2)
	{
		role->NoticeNum(errornum::EN_WORLD_BOSS_2_NOT_ENOUGH_TEAM_MEMBER);

		return true;
	}

	if (battle_type == BATTLE_MODE_WORLD_BOSS_3)
	{
		role->NoticeNum(errornum::EN_WORLD_BOSS_3_NOT_ENOUGH_TEAM_MEMBER);

		return true;
	}

	return false;
}

void EventHandler::OnPutOnJewelry(Role * role, int jewelry_index)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GEMS_MOSAIC, 1, 0);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE, 0);
	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_EQUIP, 0);
}

void EventHandler::OnTakeOffJewelry(Role * role, int jewelry_index)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GEMS_MOSAIC, 1, 0);
}

void EventHandler::OnRoleTakeOffEquip(Role* role, int equip_index, bool is_from_puton /*= false*/, int role_type /* = ROLE_TYPE_BENZUN*/)
{
	if (NULL == role) return;

	if (!is_from_puton && (Equipment::E_INDEX_WEAPON == equip_index || Equipment::E_INDEX_SHIELD == equip_index))
	{
		role->GetRoleModuleManager()->MakeAppearanceData();
		role->SendChangeAppearanceNotice();
		this->OnRoleChangeAppearance(role);
	}
	if (role_type == ROLE_TYPE_BENZUN)
	{
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GEMS_MOSAIC, 1, 0);
	}
}

void EventHandler::OnRolePutOnEquip(Role* role, int equip_index, int role_type)
{
	if (NULL == role) return;

	if (Equipment::E_INDEX_WEAPON == equip_index || Equipment::E_INDEX_SHIELD == equip_index)
	{
		role->GetRoleModuleManager()->MakeAppearanceData();
		role->SendChangeAppearanceNotice();
		this->OnRoleChangeAppearance(role);
	}

	if (role_type == ROLE_TYPE_BENZUN)
	{
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PUT_ON_EQUIPM_TIMES, 0, 1);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GEMS_MOSAIC, 1, 0);
		role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE, 0);
		role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnRelatedInfoChange(KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_EQUIP, 0);
	}
}

void EventHandler::OnRoleWearOrTakeOffSurface(Role * role)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->MakeAppearanceData();
	role->SendChangeAppearanceNotice();
	this->OnRoleChangeAppearance(role);
}

void EventHandler::OnRoleEquipStr(Role * role, int equip_jewelry_type, int str_level)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_EQUIP_FULL_SET_STR, 1);
}

void EventHandler::OnRoleLifeSkillPropsChange(Role * role, int new_life_skill_props_type, bool is_use)
{
	if(NULL == role) return;

	if (NEW_LIFE_SKILL_PROPS_TYPE_CHANGE_CARD == new_life_skill_props_type)
	{
		role->GetRoleModuleManager()->MakeAppearanceData();
		role->SendChangeAppearanceNotice();
		this->OnRoleChangeAppearance(role);
	}
}

void EventHandler::OnAddChongzhi(Role *role, long long chongzhi_gold,bool is_need_mail)
{
	if (NULL == role) return;

	if (is_need_mail)
	{
		// 邮件通知 
		static MailContentParam contentparam; contentparam.Reset();
		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_money_chongzhi_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_money_chongzhi_content, static_cast<int>(chongzhi_gold));
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(role->GetUserId(), MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_CHONGZHI);
		}
	}

	role->GetRoleModuleManager()->GetTradeCredit()->OnChongzhi(chongzhi_gold);
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnAddChongZhi(chongzhi_gold);
	role->GetRoleModuleManager()->OnAddChongZhiRecord(chongzhi_gold);
	long long history_chongzhi = role->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam().history_chongzhi_num;
	role->GetRoleModuleManager()->GetCommonData().chongzhi_count += 1;

	HongBaoManager::Instance().AddChongZhi(role, chongzhi_gold);

	const AutoTitleCfg *cfg_node = NULL;
	int seq = -1;
	cfg_node = LOGIC_CONFIG->GetTitleConfig()->CheckAutoTitleFirst(&seq, AUTO_TITLE_TYPE_7);
	if (cfg_node == NULL)
	{
		return;
	}
	//限制循环在100次以内
	for (int round = 0; round < 100 && seq != -1 && cfg_node->cond_type == AUTO_TITLE_TYPE_7; round++, seq++)
	{
		cfg_node = LOGIC_CONFIG->GetTitleConfig()->GetAutoTitleCfg(seq);
		if (cfg_node == NULL || cfg_node->cond_type != AUTO_TITLE_TYPE_7)
		{
			return;
		}
		bool has_title = role->GetRoleModuleManager()->GetTitle()->HasTitle(cfg_node->title_id);
		if (!has_title && cfg_node->cond_param1 <= history_chongzhi)
		{
			//没有这个称号，同时满足称号获取条件
			role->GetRoleModuleManager()->GetTitle()->AddTitle(cfg_node->title_id, "AutoTitleAdd");
		}
	}
}

void EventHandler::OnAddChongzhiNewGiftBuy(Role * role, long long chongzhi_gold, bool is_need_mail, bool is_add_day_chong_zhi)
{
	if (NULL == role) return;

	if (is_need_mail)
	{
		// 邮件通知 
		static MailContentParam contentparam; contentparam.Reset();
		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_money_chongzhi_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_money_chongzhi_content, static_cast<int>(chongzhi_gold));
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(role->GetUserId(), MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_CHONGZHI);
		}
	}
	if (is_add_day_chong_zhi)
	{
		role->GetRoleModuleManager()->GetRoleActivityManager()->OnNewBuyGiftAddChongZhi(chongzhi_gold);
	}
}

void EventHandler::OnOpenHuanShouEgg(Role * role)
{
	if(NULL == role) return;

}

void EventHandler::OnGetNewHuanShou(Role * role, int huanshou_id)
{
	if(NULL == role) return;
}

void EventHandler::OnStrHuanShouLevel(Role * role, int huanshou_id, int level)
{
	if(NULL == role) return;

}

void EventHandler::OnHuanShouLearnSkill(Role * role, int huanshou_id)
{
	if(NULL == role) return;
}

void EventHandler::OnHuanShouLianDan(Role * role)
{
	if(NULL == role) return;
}

void EventHandler::OnHuanShouDecompose(Role * role, int decompose_count)
{
	if(NULL == role || decompose_count <= 0) return;

}

void EventHandler::OnHuanShouFuShenChange(Role * role)
{
	if (NULL == role) return;
	
	role->GetRoleModuleManager()->MakeAppearanceData();
	role->SendChangeAppearanceNotice();
	this->OnRoleChangeAppearance(role);
}

void EventHandler::OnUserLoginCrossNoticeGame(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	// 主角登录跨服通知到原服

	UserCacheManager::Instance().UpdateUserOnlineStatus(UidToInt(user_id), UserStatusNode::STATUS_TYPE_CROSS);	
	UserMailManager::Instance().OnUserLoginToCross(user_id);
	ColosseumManager::Instance().GameHiddenSendRoleInfo(UidToInt(user_id));
	TeamKeyRegister::Instance().ClearTeamRegisterByRoleLoginHidden(user_id);
}

void EventHandler::OnUserLogoutCrossNoticeGame(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	// 同步玩家登录状态，标记玩家当前为离线状态
	UserCacheNode *node = UserCacheManager::Instance().GetUserNode(user_id);
	if (NULL == node) return;

	// 判断是否在线
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role)
	{
		UserCacheManager::Instance().UpdateUserOnlineStatus(UidToInt(user_id), UserStatusNode::STATUS_TYPE_OFFLINE);
	}
	else
	{
		UserCacheManager::Instance().UpdateUserOnlineStatus(UidToInt(user_id), UserStatusNode::STATUS_TYPE_ONLINE);
	}
}

void EventHandler::OnUserLoginCrossHidden(const UserID& user_id, const CrossData_Business& business_data)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	switch (business_data.cross_business_type)
	{
	case CrossData_Business::BUSINESS_TYPE_CREATE_CROSS_TEAM:
		{
			TeamManager::Instance().CreateTeam(role, business_data.level_limit_low, business_data.level_limit_high, business_data.team_type, true, business_data.is_match > 0, business_data.allown_red_join > 0);
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_JOIN_CROSS_TEAM:
		{
			int res = TeamManager::Instance().JoinTeamDirectly(role, business_data.join_cross_team_id);
			if (res != TeamManager::TOR_SUC)
			{
				UniqueUserID uuid = role->GetUniqueUserID();
				TeamMatch::Instance().OnEnterTeamFail(uuid);
			}
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_TEAM_SWITCH_TO_CROSS:
		{
			Team* team = TeamManager::Instance().CreateTeam(role, business_data.level_limit_low, business_data.level_limit_high, business_data.team_type, false, false, business_data.allown_red_join);
			if (NULL != team)
			{
				int origin_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
				int origin_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();

				static crossgameprotocal::CrossGameTeammateJoinCrossTeam hctjct;
				hctjct.cross_team_index = team->GetTeamIndex();
				hctjct.cross_business_data = business_data;

				InternalComm::Instance().SendToGameThroughCross(origin_plat_type, origin_server_id, (const char *)&hctjct, sizeof(hctjct));
			}
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_TEAM_GO_TO_HIDDEN:
		{
			Team * team = TeamManager::Instance().CreateTeam(role, business_data.level_limit_low, business_data.level_limit_high, business_data.team_type, false, false, business_data.allown_red_join);
			if (NULL != team)
			{
				std::map<int, int> all_team_member;
				for (int i = 0; i < ARRAY_LENGTH(business_data.teammate_list); ++i)
				{
					if (business_data.teammate_list[i] > 0)
					{
						all_team_member[business_data.teammate_list[i]] = business_data.teammate_list[i];
					}
				}
				all_team_member[role->GetUID()] = role->GetUID();

				team->SetAutoInfoGotoHidden(business_data.auto_join_type, business_data.auto_join_type_param1, business_data.param1, all_team_member);

				int origin_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
				int origin_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
				static crossgameprotocal::CrossGameTeammateJoinCrossTeam hctjct;
				hctjct.cross_team_index = team->GetTeamIndex();
				hctjct.cross_business_data = business_data;

				InternalComm::Instance().SendToGameThroughCross(origin_plat_type, origin_server_id, (const char *)&hctjct, sizeof(hctjct));
			}
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN:
		{
			this->OnRoleAutoJoin(user_id,business_data);
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_ORIGIN_INVITE_CROSS:
		{
			TeamManager::Instance().OnInvitationReq(role, business_data.team_invite_target_uid, business_data.team_invite_type);
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_TEAM_SWITCH_TO_CROSS_AND_INVITE:
		{
			Team* team = TeamManager::Instance().CreateTeam(role, business_data.level_limit_low, business_data.level_limit_high, business_data.team_type, false, false, business_data.allown_red_join);
			if (NULL != team)
			{
				int origin_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
				int origin_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();

				static crossgameprotocal::CrossGameTeammateJoinCrossTeam hctjct;
				hctjct.cross_team_index = team->GetTeamIndex();
				hctjct.cross_business_data = business_data;

				InternalComm::Instance().SendToGameThroughCross(origin_plat_type, origin_server_id, (const char *)&hctjct, sizeof(hctjct));
			}

			TeamManager::Instance().OnInvitationReq(role, business_data.team_invite_target_uid, false);
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_ENTER_WORLD_ARENA_BATTLE_SCENE:
		{
			WorldArena::Instance().EnterScene(role);
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_ENTER_UTA_BATTLE_SCENE:
		{
			UTAShadow::Instance().TryEnterBattleScene(role);
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_TEAM_ENTER_UTA_BATTLE_SCENE:
		{
			Team* team = TeamManager::Instance().CreateTeam(role, business_data.level_limit_low, business_data.level_limit_high, business_data.team_type, false, false, business_data.allown_red_join);
			if (NULL != team)
			{
				int origin_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
				int origin_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();

				static crossgameprotocal::CrossGameTeammateJoinCrossTeam hctjct;
				hctjct.cross_team_index = team->GetTeamIndex();
				hctjct.cross_business_data = business_data;

				InternalComm::Instance().SendToGameThroughCross(origin_plat_type, origin_server_id, (const char *)&hctjct, sizeof(hctjct));
			}

			UTAShadow::Instance().TryEnterBattleScene(role);
		}
		break;

	case CrossData_Business::BUSINESS_TYPE_TEAM_ENTER_HONG_MENG_TIAN_JIE:
		{
			Team* team = TeamManager::Instance().CreateTeam(role, business_data.level_limit_low, business_data.level_limit_high, business_data.team_type, false, false, business_data.allown_red_join);
			if (NULL != team)
			{
				int origin_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
				int origin_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();

				static crossgameprotocal::CrossGameTeammateJoinCrossTeam hctjct;
				hctjct.cross_team_index = team->GetTeamIndex();
				hctjct.cross_business_data = business_data;

				InternalComm::Instance().SendToGameThroughCross(origin_plat_type, origin_server_id, (const char *)&hctjct, sizeof(hctjct));
			}

			int uid_list[TEAM_MEMBER_MAX_NUM] = { 0 };
			int member_num = 0;
			uid_list[member_num++] = role->GetUID();
			for (int i = 0; i < ARRAY_ITEM_COUNT(business_data.teammate_list)
				&& member_num < TEAM_MEMBER_MAX_NUM; ++i)
			{	
				uid_list[member_num++] = business_data.teammate_list[i];	
			}

			WorldHongMengTianJieManager::Instance().EnterFB(role, business_data.param1, business_data.param2, member_num, uid_list);
		}
		break;

	default:
		break;
	}

}

void EventHandler::OnRoleAutoJoin(const UserID & user_id, const CrossData_Business & business_data)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	switch (business_data.auto_join_type)
	{
		case CROSS_AUTO_JION_TYPE_ACTIVITY:
			{
				this->GameHiddenAutoJoinActivity(role, business_data);
			}
			break;
		case CROSS_AUTO_JION_TYPE_FB:
			{
				this->GameHiddenAutoJoinFb(role, business_data);
			}
			break;
	}
}

void EventHandler::GameHiddenAutoJoinActivity(Role * role, const CrossData_Business & business_data)
{
	int activity_type = business_data.auto_join_type_param1;
	switch (activity_type)
	{
		case ACTIVITY_TYPE_ONLY_FIGHT:
		case ACTIVITY_TYPE_GUILD_ANSWER:
		case ACTIVITY_TYPE_GUILD_FIGHT:
		case ACTIVITY_TYPE_TEAM_FIGHT:
		case ACTIVITY_TYPE_WORLD_TEAM_ARENA:
			{
				role->GetRoleModuleManager()->GetRoleActivityManager()->OnActivityEnterReq2(activity_type);
			}
			break;

		case ACTIVITY_TYPE_CHIEF_ELECTION:
			{
				ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(activity_type);
				if (activity == NULL || !activity->IsActivityOpen())
				{
					break;
				}

				if (business_data.param1 == Protocol::CHIEF_ELECTION_REQ_TYPE_READY)
				{
					activity->GotoReadyScene(role);
				}

				if (business_data.param1 == Protocol::CHIEF_ELECTION_REQ_TYPE_GOTO_SEA)
				{
					activity->GotoChiefElectionSea(role);
				}
			}
			break;

		case ACTIVITY_TYPE_CHIEF_PEAK_BATTLE:
			{
				ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(activity_type);
				if (activity == NULL || !activity->IsActivityOpen())
				{
					break;
				}
				activity->GotoReadyScene(role);
			}
			break;

		case ACTIVITY_TYPE_GUILD_HONOR_BATTLE:
			{
				ActivityGuildHonorBattle * activity = (ActivityGuildHonorBattle *)ActivityManager::Instance().ForceGetActivity(activity_type);
				if (activity == NULL || !activity->IsActivityOpen())
				{
					break;
				}

				activity->GoToBattleScene(role);
			}
			break;

		case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
			{
				ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(activity_type);
				if (activity == NULL || !activity->IsActivityOpen())
				{
					break;
				}
				activity->OnRoleMatchReq(role);
			}
			break;

		default:
			break;
	}
}

void EventHandler::GameHiddenAutoJoinFb(Role * role, const CrossData_Business & business_data)
{
	FBManager::Instance().EnterFB(role, business_data.auto_join_type_param1, business_data.param1, business_data.param2);
}

void EventHandler::OnRoleChangeName(Role* role)
{
	if (NULL == role) return;

	WorldArena::Instance().UpdateUserSignupInfo(role);
}

void EventHandler::OnBattleServerDisconnect(int battle_server_index)
{
	ActivityManager::Instance().OnBattleServerDisconnect(battle_server_index);
}

void EventHandler::OnRoleChangeAppearance(Role* role, bool is_app_change)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		team->OnMemberAppearanceChange(role);
	}
	if (is_app_change)
	{
		role->GetRoleModuleManager()->GetCourageChallenge()->UpdateRoleRank();
		this->OnRoleChangeGatherDreamInfo(role, role->GetGuildID());
		Guild * guild = role->GetGuild();
		if (NULL != guild)
		{
			GuildMember * guild_member = guild->GetMemberManagerRef().GetMember(role->GetUserId());
			if (NULL != guild_member)
			{
				guild_member->UpdateMemberInfo(role);
	}
		}
	}

	role->UpdateCacheNode();
	ColosseumManager::Instance().OnUserInfoChange(role);
	PlatformBattle::Instance().SynRoleInfo(role);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		CrossChatGroupManager::Instance().OnRoleAvaChange(role);
	}
	else
	{
		ChatManager::Instance().OnRoleAvaChange(role);
	}

	WorldArena::Instance().UpdateUserSignupInfo(role);
}

void EventHandler::OnRoleChangeGatherDreamInfo(Role * role, int guild_id, SynGatherDreamRecord *syn_info, int *uid)
{
	if(NULL == role) 
	{
		if (NULL != syn_info && NULL != uid)
		{
			GatherDreamRecordManager::Instance().UpdateDataInfo(*syn_info, *uid, guild_id);
		}
		return;
	}
	Guild *guild = role->GetGuild();
	if (NULL != guild)
	{
		GuildMember * guild_member = guild->GetMemberManager()->GetMember(role->GetUserId());
		if (NULL != guild_member)
		{
			SynGatherDreamRecord info;
			role->GetName(info.name);
			info.headshot_id = role->GetHeadshotID();
			info.post = guild_member->GetPost();
			info.avatar_type = role->GetAvatarType();
			GatherDreamRecordManager::Instance().UpdateDataInfo(info, role->GetUID(), guild_id);
		}
	}
}

void EventHandler::OnRoleClearFB(Role* role, int fb_type, int param /* = 0 */, int count/* = 1*/)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetSalary()->OnRoleClearFB(fb_type, param);
	role->GetRoleModuleManager()->GetTaskManager()->OnPassFB(fb_type, count);
	switch (fb_type)
	{
		case FBManager::FB_TYPE_CRYSTAL_FB:
		case FBManager::FB_TYPE_MEDAL:
		case FBManager::FB_TYPE_CHALLENGE_FB:
		case FBManager::FB_TYPE_ADVANCE_FB:
		case FBManager::FB_TYPE_GUIDE_FB:
		case FBManager::FB_TYPE_FANTASY_FB:
		{
			role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COMPLETE_FB, 1);
			if (NULL != role->GetMyTeam() && role->GetMyTeam()->GetMemberCount() >= 2)	//最少要有一个队友
			{
				role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_TEMP_COMP_FB, 1);
		}
		}
		break;
	}
	role->GetRoleModuleManager()->GetMapUnlock()->OnRolePassEctype(fb_type);
}

void EventHandler::OnRolePassCrystalFB(Role* role, int crystal_fb_seq, int count/* = 1*/)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnPassCrystalFB(crystal_fb_seq);
	RoleActivityGiveSecretKey * role_activity = static_cast<RoleActivityGiveSecretKey *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY));
	if (role_activity != NULL)
	{
		role_activity->SetFristPassCrystalFb(crystal_fb_seq);
	}

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->PassCrystal();
	}
}

void EventHandler::OnRolePassCrystalFB2(Role * role, int crystal_fb_seq, int shui_jing_type, int fb_index)
{
	if (shui_jing_type == ROLE_CRYSTAL_FB_TYPE_XI_YOU)
	{
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_JOIN_XIYOUFB_LEVEL, 0, fb_index + 1);
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_CRYSTAL_FB_XI_YOU, 0, 1);
		role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_PASS_CRYSTAL_XI_YOU, 0, 1);
		role->LogActive(LOG_ACTIVE_TYPE_CRYSTAL_FB_RARE);
		{
			RoleActivityTigerFishEveryYear * nnyy_activity = static_cast<RoleActivityTigerFishEveryYear *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR));
			if (NULL != nnyy_activity)
			{
				nnyy_activity->AddTaskParam(RATIGERFEY::TIRGER_FEY_TYPE_JING_YING_SHI_LIAN);
			}
		}

		{
			RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
			if (NULL != lucky_turn_table)
			{
				lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_CRYSTAL_JING_YING, 1);
			}
		}

		{
			RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
			if (NULL != role_activity)
			{
				role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_JING_YING_SHI_LIAN);
			}	
		}	

		{
			RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
			if (NULL != role_activity)
			{
				role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_JING_YING_SHI_LIAN);
			}
		}

		{
			RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
			if (NULL != role_activity)
			{
				role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_JING_YING_SHI_LIAN);
	}
		}

		{
			RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
			if (NULL != role_activity)
			{
				role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_JING_YING_SHI_LIAN);
			}
		}


		{
			RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
			if (NULL != role_activity)
			{
				role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_JING_YING_SHI_LIAN);
			}
		}

	}
	else if (shui_jing_type == ROLE_CRYSTAL_FB_TYPE_NORMAL)
	{
		role->LogActive(LOG_ACTIVE_TYPE_CRYSTAL_FB_COMMON);
	}
	else if (shui_jing_type == ROLE_CRYSTAL_FB_TYPE_JING_YING)
	{
		role->LogActive(LOG_ACTIVE_TYPE_CRYSTAL_FB_ELITE);
	}
}

void EventHandler::OnRoleChallengeColosseum(Role* role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->PlayColosseum();
	role->GetRoleModuleManager()->GetFuncGuide()->OnChallengeColosseum();
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COMP_TREA,1);

	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_JINGJICHANG, 1);

	// 幸运转盘
	RoleActivityLuckyTurnTable * lucky_turn_table = static_cast<RoleActivityLuckyTurnTable *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
	if (NULL != lucky_turn_table)
	{
		lucky_turn_table->OnRALuckyTurntableFinishType(RA_LUCKY_TURNTABLE_TYPE_JINGJICHANG, 1);
	}

	// 封神战令
	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_COLOSSEUM);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_COLOSSENUM_PAR, 1);
	}
}

void EventHandler::OnRoleCloudArenaChange(Role * role, int cur_season_high_score)
{
	if(NULL == role) return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_CLOUD_ARENA, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_CLOUD_ARENA_SCORE, 0, cur_season_high_score);

}

void EventHandler::OnRoleEnterFB(Role* role, int fb_type, int seq)
{
	role->GetRoleModuleManager()->GetTaskManager()->OnEnterFB(fb_type, seq);
}

void EventHandler::OnConsumeGold(Role* role, int consume_num)
{
	if (NULL == role) return;
}

void EventHandler::OnConsumeCoinBind(Role* role, int consume_num)
{
	if (NULL == role) return;
}

void EventHandler::OnRoleDrop(Role* role, ItemID item_id, int num)
{
	const ItemBase* itembase = ITEMPOOL->GetItem(item_id);
	if (NULL == itembase) return;

	if (itembase->IsLimitDrop())
	{
		role->GetRoleModuleManager()->GetRoleDrop()->AddDropRecord(itembase->GetItemId());
		
		int monster_group_id = MonsterGroupPool::GetInstance().GetMonsterGroupIDByLimitedDropItemID(item_id);
		if (monster_group_id > 0)
		{
			role->GetRoleModuleManager()->GetRoleDrop()->AddMonsterGroupDrop(monster_group_id);
		}
	}

	if (itembase->HasDropCoolDown())
	{
		role->GetRoleModuleManager()->GetRoleDrop()
			->AddItemDropCoolDown(item_id, itembase->GetDropCoolDown_S());
	}

	{
		//记录水晶副本的掉落
		Scene * scene = role->GetScene();
		if (NULL != scene)
		{
			SpecialLogic * special_lobgic = scene->GetSpecialLogic();
			if (special_lobgic != NULL && special_lobgic->GetSceneType() == Scene::SCENE_TYPE_CRYSTAL_FB)
			{
				ItemConfigData item;
				item.item_id = item_id; item.num = num; item.is_bind = itembase->IsBind();

				SpecialLogicCrystalFB * crystal_special_lobgic = (SpecialLogicCrystalFB *)special_lobgic;
				crystal_special_lobgic->ItemStack(role, &item);
			}
		}
	}
}

void EventHandler::OnRoleFinishSpecialBattle(Role * role, int battle_mode,bool is_win)
{
	switch (battle_mode)
	{
	case BATTLE_MODE_JOY:
		{
			const RandActivityJoySeeksFavorsConfig * joy_config = static_cast<const RandActivityJoySeeksFavorsConfig * >
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS));
			if (NULL != joy_config)
			{
				RoleActivityJoySeeksFavors * role_activity = static_cast<RoleActivityJoySeeksFavors * >
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS));
				if (NULL != role_activity)
				{
					const JoySeeksFavorsCfg * cfg = joy_config->GetJoySeeksFavorsCfg(role_activity->GetJoySeeksFavorsRoleLevel(), is_win);
					if (cfg != NULL)
					{
						role->GetRoleModuleManager()->GetKnapsack()->Put(cfg->reward, PUT_REASON_JOY_SEEKS_FAVORS);
					}
					role_activity->OnParticipateRA(__FUNCTION__);
				}
			}
		}
		break;
	default:
		break;
	}
}

void EventHandler::OnAddFaBao(Role * role, int fabao_id)
{
	if (role == NULL)return;

	role->GetRoleModuleManager()->GetCollection()->OnFaBaoCollection(fabao_id);
	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GET_FA_BAO, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_HAS_FA_BAO_COUNT, 0, 1);
}

void EventHandler::OnFaBaoLevelUp(Role * role, int fabao_id, int level)
{
	if (role == NULL)return;

	role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_GET_FA_BAO, 1);
}

void EventHandler::OnCourseOfGrowthProgChange(Role * role, int old_prog, int prog)
{
	if (role == NULL)
	{
		return;
	}

	// 奇遇礼包
	RoleActivityAdventureGift * adv_gift = static_cast<RoleActivityAdventureGift *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT));
	if (NULL != adv_gift)
	{
		adv_gift->OnChangeCourseOfGrowthProg(old_prog, prog);
	}

}

void EventHandler::OnRoleResetName(Role* role, GameName old_name, GameName new_name)
{
	if (role == NULL)
	{
		return;
	}

	{
		// 同步数据其他模块
		UserCacheManager::Instance().OnResetName(role->GetUID(), new_name, old_name);
		role->GetRoleModuleManager()->GetRoleCross()->OnRoleChangeName();
		role->GetRoleModuleManager()->GetCourageChallenge()->UpdateRoleRank();
		Guild * guild = role->GetGuild();
		if (NULL != guild)
		{
			GuildMember * guild_member = guild->GetMemberManagerRef().GetMember(role->GetUserId());
			if (NULL != guild_member)
			{
				guild_member->UpdateMemberInfo(role);
	}
		}
	}

	// 通知客户端
	Protocol::SCRoleResetName rrn;
	rrn.obj_id = role->GetId();
	F_STRNCPY(rrn.new_name, new_name, sizeof(GameName));
	if (NULL != role->GetScene()) role->GetScene()->GetZoneMatrix()->NotifyAreaMsg(role->GetObserHandle(), &rrn, sizeof(rrn));

	{
		static MailSubject subject;
		static MailContentTxt contenttxt;

		int length1 = SNPRINTF(subject, sizeof(subject), gamestring::g_role_rename_subject);
		int length2 = SNPRINTF(contenttxt, sizeof(contenttxt), gamestring::g_role_rename_content, old_name, new_name);
		if (length1 > 0 && length2 > 0)
		{
			FriendManager::Instance().SystemMailToAllFriend(role->GetUID(), subject, contenttxt);
		}
	}
	ServerFirstKill::Instance().OnUserResetName(role->GetUserId(), new_name);
	this->OnRoleChangeGatherDreamInfo(role, role->GetGuildID());

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		team->OnRoleResetName(role);
	}

	//修改群组名字
	ChatManager::Instance().OnGroupMemberChangeName(role,new_name);
	ColosseumManager::Instance().OnUserInfoChange(role);
	PlatformBattle::Instance().SynRoleInfo(role);
	this->OnWorldTeamArenaChangeByRole(role);

	RankManager::Instance().OnRoleResetName(role);
}

void EventHandler::OnRoleTopRank(Role * role, int rank_idx)
{
	if (NULL != role)
	{
		RoleActivitySports * sports = static_cast<RoleActivitySports *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SPORTS));
		if (NULL != sports)
		{
			short &histroy_top = sports->GetHistroyTop();
			if (-1 == histroy_top || histroy_top > rank_idx)
			{
				histroy_top = rank_idx;
			}
		}
	}
}

void EventHandler::OnAddActive(Role* role, int add_num)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTradeCredit()->OnAddActive(add_num);
	role->GetRoleModuleManager()->GetRoleTianDiDaoHen()->RandDropItem();

	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		guild->AddActiveVal(role->GetUID(), add_num);
	}

	RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL != cool_breeze)
	{
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_ACTIVE, add_num, 0);
	}
	{
		RoleActivityYunZeCutePet * yunze_cute_pet = static_cast<RoleActivityYunZeCutePet *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
		if (NULL != yunze_cute_pet)
		{
			yunze_cute_pet->AddTaskParam(RAYZCutePet::TASK_TYPE_ACTIVE, add_num);
		}
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_ACTIVE, add_num);
	}

	role->GetRoleModuleManager()->GetActiveSystem()->OnAddActiveCheckGiveGuessCoin(add_num);

	{
		RoleActivityQiYuanKongMingDeng * role_activity = static_cast<RoleActivityQiYuanKongMingDeng *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_QIYUAN_KONGMINGDENG_TASK_TYPE_ACTIVE, add_num);
		}
	}

	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_ACTIVE, add_num);
		}
	}
}

void EventHandler::OnRoleSettingNumsOfSceenChange(Role* role, int cur_num)
{
	if (NULL == role) return;

	role->OnVisibleRoleNumSettingChange(cur_num);
}

void EventHandler::OnRoleFormationChange(Role* role, bool is_current)
{
	if (NULL == role) return;

	if (is_current)
	{
		Team* team = role->GetMyTeam();
		if (NULL != team)
		{
			if (role->GetUID() == team->GetLeaderUID())
			{
				team->OnLeaderCurrentFormationChange(role);
			}
		}
	}
	
}

void EventHandler::OnSetAutoCatchPet(Role* role)
{
	if (NULL == role) return;

}

void EventHandler::OnMutePlayer(int role_id, int forbid_time_s)
{
	// 下发协议给客户端
	if (forbid_time_s > 1)
	{
		Protocol::SCMutePlayerDeleteChatRecord msg;
		msg.role_id = role_id;
		World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	MutePlayerRecorder::Instance().AddMutePlayer(role_id, now + forbid_time_s);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenDeleteMutePlayerChatRecord msg;
		msg.uid = role_id;
		msg.forbid_seconds = forbid_time_s;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void EventHandler::OnForbidPlayer(int role_id, int forbid_time_s)
{
	// 下发协议给客户端
	if (forbid_time_s > 1)
	{
		Protocol::SCMutePlayerDeleteChatRecord msg;
		msg.role_id = role_id;
		World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	MutePlayerRecorder::Instance().AddMutePlayer(role_id, now + forbid_time_s);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenDeleteMutePlayerChatRecord msg;
		msg.uid = role_id;
		msg.forbid_seconds = forbid_time_s;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void EventHandler::OnMarketItemPurchasedByOtherSystem(Role * role, int item_id, int num, int price, int money_type)
{
	if(NULL == role) return;

	MarketManager::Instance().OnMarketItemPurchased(item_id, num);
	this->OnMarketBuyItem(role, item_id, num, price, money_type);
}

void EventHandler::OnMarketBuyItem(Role * role, int item_id, int num, int price, int money_type)
{
	if (NULL == role || price <= 0) return;

	if (money_type == MONEY_TYPE_SILVER_COIN)
	{
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_MARKET_SHOPPING, price);
	}
}

void EventHandler::OnRoleChivalrousChange(Role* role,int times)
{
	if (NULL == role) return;

	RankManager::Instance().SyncPersonRankInfo(role, PERSON_RANK_TYPE_CHIVALROUS);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_CHIVALROUS, 0, 0);

	for (int i = 0; i < times; ++i)
	{
		role->GetRoleModuleManager()->GetRoleTianDiDaoHen()->RandDropItem();
	}
}

void EventHandler::OnPassPetMonopoly(Role * role)
{
	if (NULL == role) return;

	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->PassCrystal();
	}

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB, 1);
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB, 1);
	}

	role->LogActive(LOG_ACTIVE_TYPE_PET_MONOPLY);
}

void EventHandler::OnPassPetGodFB(Role * role, int fb_req)
{
	if (NULL == role) return;

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_CRYSTAL_FB, 1);
		god_token->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_CRYSTAL_FB, 1);
	}

	//竞技月卡
	RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
	if (NULL != yueka_activity)
	{
		yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_PET_GOD_FB, 1);
	}
}

void EventHandler::OnRAGodTokenBuyToken(Role * _role)
{
	if (NULL == _role) return;

	HongBaoManager::Instance().AddGuildHongBaoTimesByZhanLingUpGrade(_role);
}

void EventHandler::OnGetPetSoul(Role * role, int item_id, bool need_sync)
{
	//改为记录物品 而非 魂器seq类型
// 	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
// 	if (NULL == item_base)
// 	{
// 		return;
// 	}
// 
// 	if (I_TYPE_PET_SOUL_EQUIP != item_base->GetItemType())
// 	{
// 		return;
// 	}
// 
// 	const PetSoulEquipment * soul_base = (const PetSoulEquipment *)item_base;
	role->GetRoleModuleManager()->GetCollection()->OnPetSoulCollection(item_id, need_sync);
}

void EventHandler::OnRoleCommitPrestigeTask(Role* role, int task_id)
{
	if (NULL == role) return;

	if (TaskPool::Instance()->IsPrestigeTaskAddActiveDegree(task_id))
	{
		role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_PRESTIGE_TASK, 1, __FUNCTION__);
	}
}

void EventHandler::OnGetHolyEquip(Role * role, int shengqi_id)
{

	role->GetRoleModuleManager()->GetCollection()->OnHolyEquipCollection(shengqi_id);
}

void EventHandler::OnCloudArenaUseTimes(int role_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_CLOUD_ARENA, 1, __FUNCTION__);
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_PAR_CLOUD_ARENA, 1);

		RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
		if (NULL != yueka_activity)
		{
			yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_CLOUD_ARENA_PAR, 1);
		}
	}
	else
	{
		OfflineRegisterManager::Instance().OnUserCloudArenaUseTimes(IntToUid(role_id));
	}
}

void EventHandler::OnCloudArenaFightFinish(int role_id, bool is_win)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL != role)
	{
		RoleActivityJingJiYueKa * yueka_activity = static_cast<RoleActivityJingJiYueKa *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
		if (NULL != yueka_activity && is_win)
		{
			yueka_activity->OnAddTaskTypeProgress(RA_JING_JI_YUE_KA_TASK_TYPE_CLOUD_ARENA_WIN, 1);
		}
	}
}

void EventHandler::OnFinishFight(battlegameprotocol::BattleGameFightResultAck * _msg)
{
	if (NULL == _msg)
		return;

	FriendIntimacyManager::Instance().OnFinishFight(_msg);
}

void EventHandler::OnPetHelperFormationChanged(Role* role, int type)
{
	switch (type)
	{
	case PET_HELPER_FORMATION_COLOSSEUM:
		{
			ColosseumManager::Instance().OnRoleChangeFormation(role);
		}
		break;
	}
}

void EventHandler::OnWanLingFBPass(Role * role, int fb_index)
{
	if (NULL == role)
	{
		return;
	}

	role->GetRoleModuleManager()->GetActiveSystem()
		->AddActiveDegree(ACTIVE_SYSTEM_TYPE_WAN_LING_FB, 1, __FUNCTION__);

	role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_PASS_WANLING_FB, fb_index, 1);
	role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_MEET_REQUIRES, TASK_MEET_WANLING_FB_TIMES, 1);
	role->GetRoleModuleManager()->GetCourseOfGrowth()->EventOperator(COG_CON_ACCOMPLISH_ACTIVITY, COG_ACT_TYPE_HUA_BEN, 1);

	{
		RoleActivityTigerLuckyMoney * role_activity = static_cast<RoleActivityTigerLuckyMoney *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RATLM::RATLM_TASK_TYPE_WAN_LING_HUA_BEN);
		}
	}

	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(YSLSTaskCfg::TASK_TYPE_WAN_LING_HUA_BEN);
		}
	}

	{
		RoleActivityDuanYangXiangNang * role_activity = static_cast<RoleActivityDuanYangXiangNang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(DYXNBasicCfg::TASK_TYPE_WAN_LING_HUA_BEN);
		}
	}

	{
		RoleActivityLanTangChuMu * role_activity = static_cast<RoleActivityLanTangChuMu *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(LTCMBasicCfg::TASK_TYPE_WAN_LING_HUA_BEN);
		}
	}

}

void EventHandler::OnCareerAdvance(Role* role, int main_lv, int sub_lv)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->ExecuteATask(TASK_NEED_CAREER_ADVANCE, main_lv, sub_lv);
	role->GetRoleModuleManager()->GetRoleSkill()->OnRoleAdvanceCareer();
}

void EventHandler::OnPetWishGodPrint(Role* role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetTaskManager()->OnGodPrintWish();
	role->LogActive(LOG_ACTIVE_TYPE_PET_WISH_GOD_PRINT);
}

void EventHandler::OnXiuWeiRoleTypeLevelUp(Role * _role)
{

}

void EventHandler::OnXiuWeiPetTypeLevelUp(Role * _role)
{
	if (NULL == _role) return;

	_role->GetRoleModuleManager()->GetPet()->OnRolePetTrainSkillLevelUp();
}

void EventHandler::OnXiuWeiParnerTypeLevelUp(Role * _role)
{
	if (NULL == _role) return;
	_role->GetRoleModuleManager()->GetPartner()->ReCalAllPartnerAttr(Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_XIU_WEI);

}

void EventHandler::OnCommitTreasureTask(Role * role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetActiveSystem()->AddActiveDegree(ACTIVE_SYSTEM_TYPE_TREASURE_TASK, 1, __FUNCTION__);
	role->GetRoleModuleManager()->GetTaskManager()->OnCommitTreasureTask(1);
}

void EventHandler::OnFinishTreasureTask(Role * role)
{
	role->GetRoleModuleManager()->GetTaskManager()->OnFinishTreasureTask();
}

void EventHandler::OnOriginDisconnectFromCrossServer(int plat_type, int server_id)
{
	UniqueServerID usid(plat_type, server_id);

	World::GetInstWorld()->OnOriginServerDisconnectCross(plat_type, server_id);
	UserCacheManager::Instance().OnDisconnectOriginServer(usid);
	GuildManager::Instance().OnOriginDisconnectFromCrossServer(plat_type, server_id);
}

void EventHandler::OnQingYuanJieHun(Role * role, long long qingyuan_id)
{
	if (NULL == role || qingyuan_id <= 0)
		return;

	RoleActivityQingYuanShiZhuang * qingyuan_shizhuang = static_cast<RoleActivityQingYuanShiZhuang *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG));
	if (NULL != qingyuan_shizhuang)
	{
		qingyuan_shizhuang->OnJieHun(qingyuan_id);
	}

	RoleActivityQingYuanLeiChong * qingyuan_leichong = static_cast<RoleActivityQingYuanLeiChong *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG));
	if (NULL != qingyuan_leichong)
	{
		qingyuan_leichong->OnJieHun(qingyuan_id);
	}
}

void EventHandler::OnRecvNoticeForUserLoginOrigin(int role_id)
{
	ActivityManager::Instance().OnUserLoginOriginGame(role_id);
	PlatformBattle::Instance().OnUserLoginOriginGame(role_id);
	GuildHonorBattleManager::Instance().OnUserLoginOriginGame(role_id);
	TeamManager::Instance().OnUserLoginOriginGame(role_id);
	TianDiDaoHen::Instance().OnUserLoginOriginGame(role_id);
	GiveGift::Instance().OnUserLoginOriginGame(role_id);
	BattleManagerLocal::GetInstance().OnUserLoginOriginGame(role_id);
	JieYiZuManager::Instance().OnUserLoginOriginGame(role_id);
	WorldArena::Instance().OnUserLoginOrigin(IntToUid(role_id));
	QingYuanManager::Instance().OnUserLoginOriginGame(role_id);
	WorldTeamArenaSignUpManager::Instance().OnUserLoginOriginGame(role_id);
}

void EventHandler::OnWorldTeamArenaChangeByRole(Role * role)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		WorldTeamArenaSignUpManager::Instance().OnRoleInfoChange(role);
		WorldTeamArenaRankManager::Instance().OnRoleInfoChange(role);
	}
	else
	{
		WorldTeamArenaUserInfo member_info;
		WorldTeamArenaSignUpManager::Instance().FillMemberInfo(role, member_info);

		//同步至隐藏服
		crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn ghricwtas;
		ghricwtas.member_info = member_info;

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghricwtas, sizeof(ghricwtas));
	}
}

void EventHandler::OnHiddenOpenServerTimeChange()
{
}

void EventHandler::DropElementFaction(Role* role, bool is_mine_monster, const MonsterCfg* cfg, int& total_drop_element_num)
{
	if (!is_mine_monster) return;
	if (cfg->m_drop_element_probability <= 0) return;
	if (!role->GetScene()->IsInStaticScene()) return;
	if (NULL == cfg) return;

	static const int MAX_DROP_ELEMENT_NUM = 1;
	if (total_drop_element_num >= MAX_DROP_ELEMENT_NUM) return;

	int monster_element_total = cfg->m_element_list[ELEMENT_TYPE_WATER]
		+ cfg->m_element_list[ELEMENT_TYPE_FIRE]
		+ cfg->m_element_list[ELEMENT_TYPE_WIND]
		+ cfg->m_element_list[ELEMENT_TYPE_EARTH];
	if (monster_element_total <= 0) return;
	
	static const int MAX_PROB_NUM = 10000;
	int r = RandomNum(MAX_PROB_NUM);
	int temp_prob = 0;
	UNSTD_STATIC_CHECK(ELEMENT_TYPE_WATER == 1 && ELEMENT_TYPE_EARTH == 4);
	for (int i = ELEMENT_TYPE_WATER; i <= ELEMENT_TYPE_EARTH; ++i)
	{
		temp_prob += (int)(1.0 * cfg->m_element_list[i] / monster_element_total * cfg->m_drop_element_probability);
		if (temp_prob < r) continue;
	
		ItemID element_item = LOGIC_CONFIG->GetSundryConfig()->GetElementItem(i);
		if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(element_item, 1))
		{
			static ItemConfigData icd;
			icd.item_id = element_item;
			icd.num = 1;
			icd.is_bind = true;
			role->GetRoleModuleManager()->GetKnapsack()->Put(icd, PUT_REASON_DROP);
		}

		total_drop_element_num += 1;
		break;
	}
}
