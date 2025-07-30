#include "messagehandler.hpp"
#include "engineadapter.h"
#include "gamelog.h"
#include "gamecommon.h"
#include "world.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "obj/otherobj/specialtransport.h"
#include "obj/otherobj/treasurechest.h"
#include "obj/character/robot.h"
#include "obj/character/npc_chosed.h"
#include "task/taskmanager.h"
#include "chat/chatconfig.h"
#include "npc/npcmanager.h"
#include "battle/battle_manager_local.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"
#include "equipment/equipmentmanager.h"
#include "globalconfig/globalconfig.h"
#include "chat/chatmanager.h"
#include "friend/friendmanager.h"
#include "friend/friendrecommend.hpp"
#include "config/otherconfig/sundryconfig.hpp"

#include "global/colosseum/colosseummanager.hpp"
#include "global/guild/guild.hpp"
#include "global/mail/mailmanager.hpp"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "global/team/teammatch.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/marriageseeking/marriageseekingmanager.h"
#include "global/usercache/usercache.hpp"
#include "global/shituseeking/shituseekingmanager.h"
#include "global/worldstatus/worldstatus.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"
#include "global/trademarket/trademarketrouter.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activitysilkroads.hpp"
#include "global/activity/activityimpl/activityworldboss.hpp"
#include "global/activity/activityimpl/activityworldboss2.hpp"
#include "global/activity/activityimpl/activityworldboss3.hpp"
#include "global/activity/activityimpl/activityteamfight.hpp"
#include "global/activity/activityimpl/activitymazeracer.hpp"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "global/activity/activityimpl/activitychiefpeakbattle.hpp"
#include "global/activity/activityimpl/activityguildhonorbattle.hpp"
#include "config/randactivityconfig/impl/randactivityduihuanlingyuconfig.hpp"
#include "global/topicmanager/topicmanager.hpp"
#include "global/cross/crossmanager.h"
#include "global/platformbattle/platformbattle.h"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "global/xunbao/xunbao.hpp"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "global/givegift/givegift.h"
#include "global/courseofgrowthmanager/courseofgrowthmanager.hpp"
#include "global/jieyi/jieyizumanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "global/cycleactivity/whereisthefairymanager.hpp"

#include "other/robot/robotmanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/funopen/funopen.hpp"
#include "other/robot/robotmanager.hpp"
#include "other/custominstruction/custominstruction.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/userprotocal/msgbattle.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/namefilter.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/robotdef.hpp"

#include "scene/map.h"
#include "scene/scene.h"
#include "scene/scenemanager.hpp"
#include "scene/speciallogic.hpp"
#include "scene/fbmanager.hpp"
#include "scene/speciallogicimpl/speciallogicmiyi.hpp"
#include "scene/speciallogicimpl/speciallogiccommon.hpp"

#include "speciallogicimpl/speciallogicteamfight.hpp"
#include "speciallogicimpl/speciallogiccrystalfb.hpp"
#include "speciallogicimpl/speciallogicmazeracer.hpp"
#include "speciallogicimpl/speciallogicbraveground.hpp"
#include "speciallogicimpl/speciallogicguildhonorbattle.hpp"
#include "scene/speciallogicimpl/speciallogicfshitufb.hpp"
#include "scene/speciallogicimpl/mazeracermanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"

#include "protocol/msgrandactivity.hpp"
#include "protocol/msgcolosseum.h"
#include "protocol/msgescort.h"
#include "protocol/msgschooltask.h"
#include "protocol/msgmail.hpp"
#include "protocol/msgscmail.hpp"
#include "protocol/msgguild.hpp"
#include "protocol/msgsurface.h"
#include "protocol/msgscene.h"
#include "protocol/msgteam.h"
#include "protocol/msgrole.h"
#include "protocol/msggm.hpp"
#include "protocol/msgpet.h"
#include "protocol/msgpartner.h"
#include "protocol/msgserver.h"
#include "protocol/msgitem.h"
#include "protocol/msgtitle.h"
#include "protocol/msgtrain.h"
#include "protocol/msgcollection.h"
#include "protocol/msgtask.h"
#include "protocol/msgother.h"
#include "protocol/msgprestige.h"
#include "protocol/msgskill.h"
#include "protocol/msgequipment.h"
#include "protocol/msgworkshop.h"
#include "protocol/msgmarry.hpp"
#include "protocol/msgjewelry.h"
#include "protocol/msgfriend.h"
#include "protocol/msgbaby.hpp"
#include "protocol/msgshitu.h"
#include "protocol/msgentergs.h"
#include "protocol/msgcouragechallenge.h"
#include "protocol/msgnpc.h"
#include "protocol/msgactivity.hpp"
#include "protocol/msgofflineexp.h"
#include "protocol/msgtreasuremap.h"
#include "protocol/msgtrademarket.h"
#include "protocol/msgworldboss.h"
#include "protocol/msgworldboss2.h"
#include "protocol/msgworldboss3.hpp"
#include "protocol/msgresearchtask.h"
#include "protocol/msgshop.h"
#include "protocol/msgfabao.hpp"
#include "protocol/msgmedal.h"
#include "protocol/msgcrystalfb.h"
#include "protocol/msgmapunlock.h"
#include "protocol/msgbraveground.h"
#include "protocol/msgchiefelection.h"
#include "protocol/msgchiefpeakbattle.h"
#include "protocol/msgcourseofgrowth.h"
#include "protocol/msgplatformbattle.h"
#include "protocol/msgguildhonorbattle.h"
#include "protocol/msgfacescore.hpp"
#include "protocol/msgfuncguide.h"
#include "protocol/msgbosschapter.h"
#include "protocol/msgfirstkill.h"
#include "protocol/msghuanjiezhanchang.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "protocol/msgshanhaiboss.hpp"
#include "protocol/msgpoker.hpp"
#include "protocol/msgdraw.h"
#include "protocol/msgdreamnoteschallenge.hpp"
#include "protocol/msgschoolexercise.h"
#include "protocol/msgsmartmountssystem.hpp"
#include "protocol/msgpetmonopoly.h"
#include "protocol/msgcloudarena.h"
#include "protocol/msgmaterial.hpp"
#include "protocol/msgmoveassist.h"
#include "protocol/msgmarket.hpp"
#include "protocol/msgheartskill.hpp"
#include "protocol/msgzhuling.h"
#include "protocol/msgformation.h"
#include "protocol/msgpetgodfb.hpp"
#include "protocol/msgdujie.hpp"
#include "protocol/msgtiandidaohen.h"
#include "protocol/msggift.h"
#include "protocol/msgyinyangeye.h"
#include "protocol/msgjieyi.h"
#include "protocol/msghuanshou.hpp"
#include "protocol/msglifeskill.hpp"
#include "protocol/msgfishground.hpp"
#include "protocol/msgworldteamarena.hpp"
#include "protocol/msgkuafuyingxionglu.hpp"
#include "protocol/msgnewcouragechallenge.h"
#include "protocol/cycleactivity/msgtuzikuaipao.hpp"
#include "protocol/cycleactivity/msggotoschool.hpp"
#include "protocol/cycleactivity/msgwhereisthefairy.hpp"

#include "other/exchange/exchange.hpp"
#include "other/escort/escort.hpp"
#include "other/route/mailroute.hpp"
#include "other/surface/surface.hpp"
#include "other/wst_question/wst_question.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleattribute/roleattributeplan.hpp"
#include "other/gm/gm.hpp"
#include "other/pet/pet.hpp"
#include "other/boss_chapter/boss_chapter.hpp"
#include "other/partner/partner.hpp"
#include "other/prestige/prestige.hpp"
#include "other/title/title.hpp"
#include "other/collection/collection.hpp"
#include "other/train/train.hpp"
#include "other/skill/roleskill.hpp"
#include "other/dayreward/dayreward.hpp"
#include "other/workshop/workshop.hpp"
#include "other/marriage/marriage.hpp"
#include "other/baby/baby.hpp"
#include "other/baby/babymanager.hpp"
#include "other/role_setting/rolesetting.hpp"
#include "other/mentor/shitu.hpp"
#include "other/systemnotice/systemnotice.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/monster_wave/monsterwave.h"
#include "other/roleactivity/roleactivity.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "other/schooltask/schooltask.h"
#include "other/offlineexp/offlineexp.hpp"
#include "other/event/eventhandler.hpp"
#include "other/treasure_map/treasuremap.h"
#include "other/researchtask/researchtask.h"
#include "other/shop/shop.h"
#include "other/taskchain/taskchain.hpp"
#include "other/fabao/fabao.hpp"
#include "other/medal/medal.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/mapunlock/mapunlock.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/marriage/marriagetask.hpp"
#include "other/queryhandler/queryhandler.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/salary/salary.hpp"
#include "other/facescore/facescore.hpp"
#include "other/funcguide/funcguide.hpp"
#include "other/roleconvertshop/roleconvertshop.hpp"
#include "other/specialappearance/specialappearance.hpp"
#include "other/partner/partnerconfig.hpp"
#include "other/rolemail/rolemail.hpp"
#include "other/loginsystem/loginsystem.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/miyi/miyiconfig.hpp"
#include "global/platformbattle/platformbattleconfig.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "other/activesystem/activesystem.hpp"
#include "global/team/teamconfig.hpp"
#include "other/robot/robotconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityjoyseeksfavors.hpp"
#include "protocol/msgcsrank.h"
#include "global/rank/rankmanager.hpp"
#include "task/taskrecorder.h"
#include "battle/battle_video_config.hpp"
#include "battle/battle_video_manager.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "protocol/msgwildboss.h"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "protocol/msgmiyi.h"
#include "servercommon/string/gameworldstr.h"
#include "other/funcpreview/funcpreview.hpp"
#include "other/miyi/miyiconfig.hpp"
#include "global/couragechallengerank/couragechallengerank.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "chat/crosschatgroupmanager.h"
#include "other/auto_catch_pet/auto_catch_pet.hpp"
#include "other/formation/formation.hpp"
#include "other/roleheartskill/roleheartskill.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "global/drawmanager/drawmanager.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitysevencolorpicture.hpp"
#include "other/rolezhuling/rolezhuling.hpp"
#include "other/rolehundredgodboss/rolehundredgodboss.hpp"

#include "protocol/randactivity/msgrasevencolorpicture.hpp"
#include "protocol/randactivity/msgrajoyseeksfavors.hpp"

#include "other/roleactivity/impl/roleactivityanimalrace.hpp"
#include "scene/speciallogicimpl/speciallogicwenxianglou.hpp"
#include "scene/speciallogicimpl/speciallogicdreamnoteschallenge.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/huanjie/rolehuanjie.hpp"
#include "protocol/msglevelcomplement.h"
#include "other/levelcomplement/levelcomplement.h"
#include "other/levelcomplement/newlevelcomplement.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/schoolexercise/schoolexercise.hpp"
#include "scene/speciallogicimpl/speciallogicmijingqiwen.hpp"
#include "scene/speciallogicimpl/speciallogicmijingqiwen2.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "global/cloudarena/cloudarena.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "other/rolematerial/rolematerial.hpp"
#include "obj/MoveAssist/MoveAssist.hpp"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "scene/speciallogicimpl/speciallogicreliclooting.hpp"
#include "global/cloudarena/cloudarenauser.hpp"
#include "other/mapexploration/mapexploration.hpp"
#include "global/guild/guildmember.hpp"
#include "other/formation/pethelperformation.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "other/petgodfb/petgodfb.hpp"
#include "other/dujie/dujie.hpp"
#include "other/kuafuyingxionglu/rolekuafuyingxionglu.hpp"

#include "protocol/msgrolejoinactivityreward.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"
#include "other/roletiandidaohen/roletiandidaohen.hpp"
#include "other/rolegivegift/rolegivegift.hpp"

#include "protocol/msghongbao.hpp"
#include "other/hongbao/hongbaomanager.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "other/prestige_task/prestige_task.hpp"
#include "other/svip/svip.hpp"
#include "other/roleattrpellet/roleattrpellet.hpp"
#include "other/roleactivity/impl/roleactivitybackreward.hpp"
#include "other/duel/duel.hpp"
#include "protocol/msgaccessories.h"
#include "other/yinyangeye/yinyangeye.hpp"
#include "equipment/accessoriesmentmanager.h"
#include "protocol/msgcowreportgoodnews.hpp"
#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include <string>
#include "other/endlesstower/endlesstower.hpp"
#include "friend/friendintimacy.hpp"
#include "protocol/msgadvancecareer.h"
#include "other/advancecareer/advancecareer.hpp"
#include "protocol/msgwanlingfb.h"
#include "other/wanlingfb/wanlingfb.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/mirageterritory/mirageterritory.hpp"
#include "other/xiuwei/xiuwei.hpp"
#include "protocol/msgxiuwei.hpp"
#include "other/treasuretask/treasuretask.hpp"
#include "protocol/msgtigerbringsblessing.hpp"
#include "global/tigerbringsblessing/tigerbringsblessing.hpp"
#include "protocol/msgqingyuan.h"
#include "qingyuan/weddingmanger.hpp"
#include "global/worldarena/worldarena.hpp"
#include "protocol/msgworldarena.hpp"
#include "other/fallengod/fallengod.hpp"
#include "protocol/msgminigame.hpp"
#include "other/minigame/minigame.hpp"
#include "protocol/msgpeaktoken.hpp"
#include "protocol/msgincome.hpp"
#include "obj/otherobj/peaktokenobj.hpp"
#include "other/peaktoken/rolepeaktoken.hpp"
#include "other/income/income.hpp"
#include "protocol/msgqingyuanshenghui.h"
#include "global/activity/activityimpl/activityqingyuanshenghui.hpp"
#include "other/yaoxingshilian/yaoxingshilian.hpp"
#include "scene/speciallogicimpl/speciallogicqingyuanduiduipeng.hpp"
#include "other/cycleactivity/sanxianguidong/sanxianguidong.hpp"
#include "protocol/cycleactivity/msgqingyuanduiduipeng.hpp"
#include "other/cycleactivity/whereisthefairy/rolewhereisthefairy.hpp"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengmanager.hpp"
#include "protocol/cycleactivity/msglanternriddie.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddie.hpp"
#include "protocol/cycleactivity/msgtiangongxunbao.hpp"
#include "other/cycleactivity/tiangongxunbao/tiangongxunbao.hpp"
#include "other/cycleactivity/loversmarket/loversmarket.hpp"
#include "other/cycleactivity/taoquannaji/taoquannaji.hpp"
#include "other/cycleactivity/concentricknot/concentricknot.hpp"
#include "scene/speciallogicimpl/speciallogictuzikuaipao.hpp"
#include "scene/speciallogicimpl/speciallogicgotoschool.hpp"
#include "protocol/msghongmengtianjie.hpp"
#include "scene/speciallogicimpl/speciallogichongmengtianjie.hpp"
#include "other/hongmengtianjie/rolehongmengtianjie.hpp"
#include "other/hongmengtianjie/hongmengtianjiemanager.hpp"

#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"

#include "servercommon/userprotocal/msgutachampionrecord.h"
#include "global/bigcross/utachampionrecordmanager.hpp"
#include "servercommon/userprotocal/msgutaguess.h"
#include "global/bigcross/utaguessshadow.hpp"

#include "global/universe_team_arena/UTAShadow.hpp"
#include "servercommon/userprotocal/msguta.h"
#include "global/bigcross/utaobshadow.hpp"

#include "other/zaohuaequip/zaohuaequipmanager.hpp"
#include "protocol/msgzaohuaequipment.h"
#include "protocol/msgholyequip.h"
#include "other/shengqi/holyequipmanager.hpp"
#include "global/team/teamrecruit.hpp"
#include "protocol/cycleactivity/msgkejuexam.hpp"
#include "other/cycleactivity/kejuexam/kejuexam.hpp"
#include "global/rank/partnertoprankshadow.hpp"
#include "global/rank/pettoprankshadow.hpp"
#include "other/fuyu/fuyumanager.hpp"
#include "other/couragechallenge/newcouragechallenge.hpp"
#include "other/fashioncollection/fashioncollection.hpp"
#include "other/starchart/starchart.h"
#include "other/roleactivity/impl/roleactivityyuexidengmi.hpp"
#include "protocol/randactivity/msgrayuexidengmi.hpp"
#include "other/huanshou/rolehuanshou.hpp"
#include "other/fashioncollection/fashioncollection.hpp"
#include "protocol/msgroleguildtask.h"
#include "other/roleguildtask/roleguildtask.hpp"

void MessageHandler::OnTimeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	role->SendServerTime(0);
}

void MessageHandler::OnLogout(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	m_scene_manager->Logout(scene, obj_id);

	EngineAdapter::Instance().NetDisconnect(netid, role->GetUserId(), role->GetName(), scene->GetSceneID(), "Logout");
}

void MessageHandler::OnAllInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	return; // 暂时屏蔽，改用Role::SendAllInfo	
}

void MessageHandler::OnTransportReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSTransportReq *rt = (Protocol::CSTransportReq*)msg;

	Transport *transport = scene->GetTransport(rt->door_id);
	if (NULL == transport)
	{
		gamelog::g_log_messagehandler.printf(LL_DEBUG, "Scene: [%d]%s Can NOT Find Transport[%d],", scene->GetSceneID(), scene->GetSceneName(), rt->door_id);
		return;
	}

	if (role->GetLevel() < transport->require_level)
	{
		role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	static const unsigned int TransportReqLength = GLOBALCONFIG->GetOtherConfig_MaxTransportLength();
	if ((role->GetPos() - transport->location).IsLongerThan(TransportReqLength))
	{
		role->NoticeNum(errornum::EN_SCENE_TRANSPORT_TOO_FAR);
		return;
	}

	if (!scene->GetSpecialLogic()->CanTransport(role))
	{
		return;
	}

	if (!role->GetRoleStatusManager()->CanTransportTo(transport->target_scene))
	{
		role->NoticeNum(errornum::EN_SCENE_TRANSPORT_LIMIT_BY_ROLE_STATUS);
		return;
	}

	//暂时不允许从野外传送到副本里
	if (m_scene_manager->IsFB(transport->target_scene))
	{
		role->NoticeNum(errornum::EN_SCENE_TRANSPORT_FB_LIMIT);
		return;
	}

	Scene * to_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(transport->target_scene, COMMON_SCENE_KEY);
	if (NULL == to_scene) return;

	if (!to_scene->CanEnter(role, true))
	{
		return;
	}

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		TeamMember *team_member = team->GetMemberInfo(role);
		if(team_member == NULL) return;

		if (!team->IsLeader(role))
		{	
			if(team_member->is_follow)return;

			m_scene_manager->GoTo(role, transport->target_scene, scene->GetSceneKey(), transport->target_pos);
			return;
		}

		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
		{
			TeamMember *member = team->GetMemberInfo(i);
			if (NULL == member) continue;
			if (!member->is_follow) continue;
			if (member->uid == role->GetUID()) continue;

			if (!member->IsRobot())
			{
				Role* member_role = team->GetMemberRoleByIndex(i);
				if (NULL != member_role)
				{
					if (!to_scene->CanEnter(member_role, false))
					{
						team->NoticeAllMember(errornum::EN_MEMBER_LEVEL_NOT_ENTER_SCENE);
						//role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENTER_SCENE);
						return;
					}
				}
			}
		}

		m_scene_manager->TeamGoTo(team, transport->target_scene, scene->GetSceneKey(), transport->target_pos);
	}
	else
	{
		m_scene_manager->GoTo(role, transport->target_scene, scene->GetSceneKey(), transport->target_pos);
	}
	
}

void MessageHandler::OnSpecialTransportReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSpecialTransportReq * req = (Protocol::CSSpecialTransportReq*)msg;

	Obj *obj = scene->GetObj(req->obj_id);
	if (NULL == obj)
	{
		return;
	}

	if (Obj::OBJ_TYPE_SPECIAL_TRANSPORT != obj->GetObjType())
	{
		return;
	}

	SpecialTransport * special_transport = (SpecialTransport*)obj;
	special_transport->Transport(role);
}

void MessageHandler::OnTreasureChestReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTreasureChestReq * req = (Protocol::CSTreasureChestReq*)msg;

	Obj *obj = scene->GetObj(req->obj_id);
	if (NULL == obj)
	{
		return;
	}

	if (Obj::OBJ_TYPE_TREASURE_CHEST != obj->GetObjType())
	{
		return;
	}

	TreasureChest * treasure_chest = (TreasureChest*)obj;
	treasure_chest->Open(role);
}

void MessageHandler::OnRoleMoveReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSObjMove *um = (Protocol::CSObjMove*)msg;

	bool role_can_move = true;

	if (!role->GetRoleStatusManager()->CanRoleMove())
	{
		role_can_move = false;
	}

	if (Scene::SCENE_TYPE_COMMON_SCENE != scene->GetSceneType() && !scene->GetSpecialLogic()->CanMoveOn(role))
	{
		role_can_move = false;
	}

	Team * team = role->GetMyTeam();
	if (NULL != team && !team->IsLeader(role))
	{
		TeamMember *member_info = team->GetMemberInfo(role);
		if (NULL != member_info && member_info->is_follow)
		{
			role_can_move = false;
		}
	}

	if (um->cur_scene_id != role->GetScene()->GetSceneID())
	{
		// 场景对不上，不移动
		return;
	}

#ifdef _DEBUG
	if (!scene->GetMap()->Validate(Obj::OBJ_TYPE_ROLE, um->target_pos_x, um->target_pos_y))
	{
		role_can_move = false;
		role->NoticeNum(errornum::EN_SCENE_TARGET_POS_NOT_VALIDATE);
	}
#endif

	Posi target_pos(um->target_pos_x, um->target_pos_y);
	Posi now_pos(um->pos_x, um->pos_y);
	Posi deltaP = target_pos - now_pos;
	Scalar distance = 0.0f;
	Dir dir = gamecommon::GetMoveDir(deltaP, &distance);
	Posi role_pos(role->GetPos());
	Scalar role_distance = 0.0f;
	gamecommon::GetMoveDir(role_pos - now_pos, &role_distance);

	if (role_can_move)
	{
		//printf("client send:..... roleid:%d  scene_id[%d]  cur[%d,%d]  target[%d,%d]\n", role->GetUID(), role->GetScene()->GetSceneID(), um->pos_x, um->pos_y, um->target_pos_x, um->target_pos_y);
		role->Move(um->pos_x, um->pos_y, dir, distance, 0, true);

		if (NULL != team && team->IsLeader(role))
		{
			int count = 1;
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
			{
				TeamMember *member = team->GetMemberInfo(i);
				if (NULL != member && member->is_follow)
				{
					PosData pos_data(um->pos_x, um->pos_y, dir, distance - count * 5);
					if (!member->IsRobot())
					{
						Role* member_role = team->GetMemberRoleByIndex(i);
						if (NULL != member_role && member_role->GetUID() != role->GetUID())
						{
							member_role->AddPosData(pos_data);
							count++;
						}
					}
					else
					{
						Robot* robot = team->GetMemberRobotByIndex(i);
						if (NULL != robot)
						{
							robot->AddPosData(pos_data);
							count++;
						}
					}
				}
			}
		}
	}
}

void MessageHandler::OnUnfullTeamListByTypeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSUnfullTeamListByTypeReq *ctl = (Protocol::CSUnfullTeamListByTypeReq*)msg;
	int ret = TeamManager::Instance().SendUnfullTeamListToUser(role, ctl->team_type);
	if (ret < 0)
	{
		printf("%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnUnfullChivalrousTeamListReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	TeamManager::Instance().QueryUnfullChivalrousTeamList(role);
}

void MessageHandler::OnChangeTeamLimit(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSChangeTeamLimit *ct = (Protocol::CSChangeTeamLimit*)msg;
	if (ct->limit_level_low > role->GetLevel())
	{
		role->NoticeNum(errornum::EN_TEAM_LEVEL_LOW_LIMIT);
		return;
	}

	int ret = TeamManager::Instance().SetTeamInfo(role, ct->limit_level_low, ct->limit_level_high, ct->team_type, ct->allow_red_join);
	if (ret < 0)
	{
		printf("%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnCreateTeamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSCreateTeam *ct = (Protocol::CSCreateTeam*)msg;

	if (!ct->allow_red_join && role->GetRoleModuleManager()->GetDuel()->IsRedName())
	{
		role->NoticeNum(errornum::EN_DUEL_TEAM_NOT_ALLOW_RED_JOIN);
		return;
	}

	if (0 == ct->is_cross || CrossConfig::Instance().IsHiddenServer())
	{
		TeamManager::Instance().CreateTeam(role, ct->limit_level_low, ct->limit_level_high, ct->team_type, true, ct->is_match > 0, ct->allow_red_join > 0);
	}
	else
	{
		// 在本服请求开跨服队伍，如果在副本里，不允许
		if (!role->GetScene()->IsInStaticScene())
		{
			role->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
			return;
		}

		CrossData_Business business_data;
		business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_CREATE_CROSS_TEAM;
		business_data.level_limit_high = ct->limit_level_high;
		business_data.level_limit_low = ct->limit_level_low;
		business_data.team_type = ct->team_type;
		business_data.is_match = ct->is_match;
		business_data.allown_red_join = ct->allow_red_join;

		role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
	}
}

void MessageHandler::OnRoleStartCrossReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	// 在本服请求开跨服队伍，如果在副本里，不允许
	if (!role->GetScene()->IsInStaticScene())
	{
		role->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
		return;
	}

	Team* team = role->GetMyTeam(false);
	if (NULL == team)
	{
		CrossData_Business business_data;
		business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_INVALID;
		role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
	}
	else
	{
		if (role->GetUID() != team->GetLeaderUID())
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			return;
		}
		TeamManager::Instance().OnTeamSwitchToCrossReq(role);
	}

}

void MessageHandler::OnDismissTeamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	TeamManager::Instance().DismissTeam(role);
}

void MessageHandler::OnInviteJoinTeamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSInviteJoinReq *req = (Protocol::CSInviteJoinReq*)msg;
	Role* to_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(req->uid));
	if (NULL == to_role)return;
	Team * team = role->GetMyTeam();
	if (NULL == team) return;

	if (!team->TeamIsAllowRedNameJoin() && to_role->GetRoleModuleManager()->GetDuel()->IsRedName())
	{
		role->NoticeNum(errornum::EN_DUEL_TEAM_NOT_ALLOW_RED_JOIN);
		return;
	}

	if (NULL != to_role->GetMyTeam())
	{
		role->NoticeNum(errornum::EN_TARGET_HAVE_TEAM);
	}
	else
	{
		role->NoticeNum(noticenum::NT_TEAM_INVITE_SUCC);

		//  邀请对方
		TeamItem team_item;
		team->GetTeamItemInfo(&team_item);
		Protocol::SCTeamInvitationNotice notice;
		notice.notice_type = Protocol::TEAM_INVITATION_NOTICE_TYPE_DEFAULT;
		notice.is_from_cross = CrossConfig::Instance().IsHiddenServer();
		notice.invite_type = 0;
		notice.xia_yi_zhi = to_role->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(team->GetTeamType(), team_item);
		notice.team_index = team->GetTeamIndex();
		notice.team_type = team->GetTeamType();
		notice.team_member_num = team->GetMemberCount();
		notice.inviter_info = role->GetMsgRoleBaseInfo();
		EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char *)&notice, sizeof(notice));
	}
}

void MessageHandler::OnExitTeamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	if (BattleManagerLocal::GetInstance().IsRoleInBattle(role->GetUID()))
	{
		battlegameprotocol::GameBattleRoleLeaveTeamAfterBattle rltab;
		rltab.bg_netid = role->GetBGNetID();
		rltab.uid = role->GetUID();

		World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&rltab, sizeof(rltab));
		return;
	}

	int ret = TeamManager::Instance().ExitTeam(role);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnKickOutOfTeamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSKickOutOfTeamReq *req = (Protocol::CSKickOutOfTeamReq*)msg;
	int ret = TeamManager::Instance().KickOutMember(role, req->kick_index);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnJoinTeamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSJoinTeamReq *req = (Protocol::CSJoinTeamReq*)msg;
	int ret = TeamManager::Instance().JoinTeam(role, req->team_index, req->is_apply_cross_team > 0);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnJoinTeamReqRet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSJoinTeamReqRet *req = (Protocol::CSJoinTeamReqRet*)msg;
	int ret = TeamManager::Instance().JoinReqRet(role, req->uid, req->is_agree != 0);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnInviteJoinTeamRet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSInviteJoinRet *ret_msg = (Protocol::CSInviteJoinRet*)msg;
	int ret = TeamManager::Instance().InviteRet(role, ret_msg->uid, ret_msg->is_agree != 0);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnTeamMatch(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetEscort()->HaveEscortTask())
	{
		role->NoticeNum(errornum::EN_ESCORT_CANNOT_TEAM_MATCH);
		return;
	}

	Protocol::CSTeamMatchOp *req = (Protocol::CSTeamMatchOp*)msg;
	
	if (0 == req->op_type)
	{
		Team* team = role->GetMyTeam();
		if (NULL == team)
		{
			TeamMatch::Instance().OnUserStopMatch(role->GetUniqueUserID()); //这里会顺便调用UserStopCrossMatch
			TeamMatch::Instance().SendMatchInfo(role, 0);
		}
		else
		{
			TeamMatch::Instance().OnTeamStopMatch(team);
			TeamMatch::Instance().SendMatchInfo(role, 0);
		}
	}
	else
	{
		bool ret = false;
		Team* team = role->GetMyTeam();
		if (NULL != team)
		{
			ret = TeamMatch::Instance().OnTeamStartMatch(team);
		}
		else
		{
			if (!role->GetScene()->GetSpecialLogic()->CanEnterTeam(role, NULL, false))
			{
				role->NoticeNum(errornum::EN_NOW_STATUS_UN_JOIN_TEAM);
				return;
			}
		
			if (req->is_cross)
			{
				if (!CrossManager::Instance().IsCrossServerConnected())
				{
					role->NoticeNum(errornum::EN_CROSS_TEAM_NOT_AVALIABLE);
					ret = TeamMatch::Instance().OnUserStartMatch(role->GetUniqueUserID(), role->GetLevel(), req->match_type, role->GetGuildID(), 0 != req->is_auto_create);
				}
				else
				{
					ret = TeamMatch::Instance().OnUserStartCrossMatch(role->GetUniqueUserID(), role->GetLevel(), req->match_type, role->GetGuildID(), 0 != req->is_auto_create);
				}
			}
			else
			{
				ret = TeamMatch::Instance().OnUserStartMatch(role->GetUniqueUserID(), role->GetLevel(), req->match_type, role->GetGuildID(), 0 != req->is_auto_create);
			}
		}	

		if (ret)
		{
			TeamMatch::Instance().SendMatchInfo(role, 1);
		}
	}
}

void MessageHandler::OnChangeTeamLeader(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSChangeTeamLeader *req = (Protocol::CSChangeTeamLeader*)msg;
	int ret = TeamManager::Instance().OnChangeLeader(role, req->member_index);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnChangeMustCheck(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSChangeMustCheck *req = (Protocol::CSChangeMustCheck*)msg;
	int ret = TeamManager::Instance().OnChangeTeamCheckType(role, req->check_type);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}

}

void MessageHandler::OnTemporarilyPartReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	//Protocol::CSTemporarilyPartReq *req = (Protocol::CSTemporarilyPartReq*)msg;
	int ret = TeamManager::Instance().OnTemporarilyPart(role);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_DEBUG, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnTeamInvitationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSTeamInvitationReq *req = (Protocol::CSTeamInvitationReq*)msg;
	int ret = TeamManager::Instance().OnInvitationReq(role, req->uid, req->invite_type);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_DEBUG, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnTeamInvitationRet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSTeamInvitationRet *req = (Protocol::CSTeamInvitationRet*)msg;
	int ret = TeamManager::Instance().OninvitationRet(role, req->uid, 0 != req->is_accept, req->is_from_cross > 0, req->team_index);
	if (ret < 0)
	{
		gamelog::g_log_debug.printf(LL_INFO, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnTeamEnterFbReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	Protocol::CSTeamEnterFbRet *req = (Protocol::CSTeamEnterFbRet*)msg;
		
	Team * team = role->GetMyTeam();
	if (team == NULL)
	{
		return;
	}

	team->OnEnterFbRet(role, 0 != req->is_determine);
}

void MessageHandler::OnTeamRecruitReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TEAM, true))
	{
		return;
	}

	//Protocol::CSTeamRecruit* req = (Protocol::CSTeamRecruit*)msg;

	TeamManager::Instance().OnTeamRecruitReq(role);
}

void MessageHandler::OnTeamSwitchToCrossReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	//Protocol::CSTeamSwitchToCross* req = (Protocol::CSTeamSwitchToCross*)msg;

	TeamManager::Instance().OnTeamSwitchToCrossReq(role);
}

void MessageHandler::OnGmCommand(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGMCommand *gm_command = (Protocol::CSGMCommand*)msg;
	Gm::Instance().OnCommand(netid, scene, role, obj_id, gm_command);
}

void MessageHandler::OnPetOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSPetOp *op = (Protocol::CSPetOp*)msg;

	if (op->op_type != Protocol::CSPetOp::PetAttrsReq && !FunOpen::IsFunOpen(role, FUN_TYPE_PET, true))
	{
		return;
	}
	
	int ret = 0;
	ret = role->GetRoleModuleManager()->GetPet()->OnPetOp(op);
	if (ret < 0)
	{
		gamelog::g_log_pet.printf(LL_WARNING, "%s Error user[%d %s] [%d] op_type[%d]p1[%d]p2[%d]p3[%d]\n", __FUNCTION__, role->GetUID(), role->GetName(),
			   ret, op->op_type, op->p1, op->p2, op->p3);
	}
}

void MessageHandler::OnAbandonPetByList(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSAbandonPetByList * protocol = (Protocol::CSAbandonPetByList *)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSAbandonPetByList, del_pet_index_list));
	if (length < offset)
	{
		return;
	}

	int del_list_count = (length - offset) / sizeof(protocol->del_pet_index_list[0]);
	
	for (int i = 0; i < del_list_count && i < ARRAY_LENGTH(protocol->del_pet_index_list); ++i)
	{
		if (role->GetRoleModuleManager()->GetPet()->AbandonmentPet(protocol->del_pet_index_list[i]) != 0)
		{
			break;
		}
	}
}

void MessageHandler::OnPetRename(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetRename *op = (Protocol::CSPetRename*)msg;
	int ret = role->GetRoleModuleManager()->GetPet()->ReName(op->index, op->name);
	if (ret < 0)
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s Error %d user[%d %s] index[%d] name[%s]", 
								  __FUNCTION__, ret, role->GetUID(),role->GetName(),
								  op->index, op->name);
	}
}

void MessageHandler::OnPetAssignAttributePoints(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetAssignAttributePoints *op = (Protocol::CSPetAssignAttributePoints*)msg;
	int ret = role->GetRoleModuleManager()->GetPet()->AssignAttributePoints(op->index, op->con, op->str, op->def, op->agi, op->magic);
	if (ret < 0)
	{
		gamelog::g_log_pet.printf(LL_INFO, "%s Error %d user[%d %s] index[%d] con[%d] str[%d] def[%d] agi[%d] magic[%d]", 
								  __FUNCTION__, ret, role->GetUID(), role->GetName(),
								  op->index, op->con, op->str, op->def, op->agi, op->magic);
	}
}

void MessageHandler::OnKnapsackOperaReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_KNAPSACK, true))
	{
		return;
	}

	Protocol::CSKnapsackOperaReq* req = (Protocol::CSKnapsackOperaReq*)msg;

	switch (req->opera_type)
	{
	case Protocol::CSKnapsackOperaReq::OPERA_TYPE_REQUEST_INFO:
		{
			role->GetRoleModuleManager()->GetKnapsack()->SendKnapsackInfoToRole();
		}
		break;

	case Protocol::CSKnapsackOperaReq::OPERA_TYPE_EXTEND:
		{
			role->GetRoleModuleManager()->GetKnapsack()->Extend(req->param1, req->param2);
		}
		break;

	case Protocol::CSKnapsackOperaReq::OPERA_TYPE_DEPOSIT:
		{
			role->GetRoleModuleManager()->GetKnapsack()->Deposit(req->param1, req->param2, req->param3);
		}
		break;

	case Protocol::CSKnapsackOperaReq::OPERA_TYPE_WITHDRAW:
		{
			role->GetRoleModuleManager()->GetKnapsack()->Withdraw(req->param1, req->param2, NULL, true);
		}
		break;

	case Protocol::CSKnapsackOperaReq::OPERA_TYPE_WITHDRAW_ALL:
		{
			role->GetRoleModuleManager()->GetKnapsack()->WithdrawAll(req->param1);
		}
		break;

	case Protocol::CSKnapsackOperaReq::OPERA_TYPE_INORDER:
		{
			role->GetRoleModuleManager()->GetKnapsack()->InOrder(req->param1, req->param2, req->param3);
		}
		break;

	case Protocol::CSKnapsackOperaReq::OPERA_TYPE_DISCARD:
		{
			role->GetRoleModuleManager()->GetKnapsack()->Discard(req->param1, req->param2, req->param3);
		}
		break;
	}
}

void MessageHandler::OnUseItem(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSUseItem* pstUseItem = (Protocol::CSUseItem*)msg;
	role->GetRoleModuleManager()->GetKnapsack()->Use(pstUseItem->column, pstUseItem->index, pstUseItem->num, pstUseItem->param1, pstUseItem->param2, pstUseItem->param3);
}

void MessageHandler::OnUseOptionalGift(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUseOptionalGift* req = (Protocol::CSUseOptionalGift*)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSUseOptionalGift, select_info));
	if (length < offset)
	{
		return;
	}

	int select_item_count = (length - offset) / sizeof(req->select_info[0]);

	role->GetRoleModuleManager()->GetKnapsack()->Use(req->column, req->index, req->num, select_item_count, 0, 0, ITEM_PARAM_TYPE_OPTIOAN_GIFT, req->select_info);
}

void MessageHandler::OnUseOptionalGift2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUseOptionalGift2 * req = (Protocol::CSUseOptionalGift2*)msg;
	role->GetRoleModuleManager()->GetKnapsack()->Use(req->column, req->index, 1, 0, 0, 0, ITEM_PARAM_TYPE_OPTIOAN_GIFT_2, &req->select_index_flag);
}

void MessageHandler::OnUseGoldRecovery(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (role->GetRoleStatusManager()->IsInBattleStatus())
	{
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return;
	}

	Protocol::CSUseGoldRecovery * req = (Protocol::CSUseGoldRecovery *)msg;

	SpecialLogic * sp = scene->GetSpecialLogic();
	if (sp == NULL)
	{
		return;
	}

	if (req->param4 == 1)
	{
		sp->UseGoldRecoveryHp(role, req->param1, req->param2, req->param3);
	}

	if (req->param4 == 2)
	{
		sp->UseGoldRecoveryMp(role, req->param1, req->param2, req->param3);
	}
}

void MessageHandler::OnUseSpecialLogicItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUseSpecialLoigcItem * req = (Protocol::CSUseSpecialLoigcItem *)msg;
	switch (req->notice_type)
	{
		case Protocol::CSUseSpecialLoigcItem::USE_SPECIAL_LOGIC_ITEM_TYPE_CRYSTAL_FB:
		{
			role->GetRoleModuleManager()->GetRoleCrystalFb()->UseCrystalFbEnergyItem(req->item_id, req->num);
		}
		break;
		case Protocol::CSUseSpecialLoigcItem::USE_SPECIAL_LOGIC_ITEM_TYPE_BREAK_ITEM:
		{	
			role->GetRoleModuleManager()->GetRealmBreak()->OnUseItem(req->item_id, req->num);
		}
		break;
		default:
		break;
	}

}

void MessageHandler::OnActiveSpecialLogicItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActiveSpecialLogicItem * req = (Protocol::CSActiveSpecialLogicItem *)msg;
	switch (req->notice_type)
	{
		case Protocol::CSActiveSpecialLogicItem::ACTIVE_SPECIAL_LOGIC_ITEM_TYPE_STAR_TREASURE_MAP:
		{
			role->GetRoleModuleManager()->GetTreasureMap()->OnActiveStarTreasureMap(req->param1, req->param2);
		}
		break;
		case Protocol::CSActiveSpecialLogicItem::ACTIVE_SPECIAL_LOGIC_ITEM_RESET_TREASURE_MAP_POS:
		{
			role->GetRoleModuleManager()->GetTreasureMap()->OnResetTreasureMap(req->param1, req->param2);
		}
		break;
	}
	
}

void MessageHandler::OnHearsayInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHearsayInfoReq * req = (Protocol::CSHearsayInfoReq *)msg;
	switch (req->notice_type)
	{
		case Protocol::CSHearsayInfoReq::HEARSAY_INFO_TYPE_TREASURE_MAP:
		{
			role->GetRoleModuleManager()->GetTreasureMap()->SendNotice(req->param1, req->param2, req->param3);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnBuyCmdGiftReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBuyCmdGiftReq * req = (Protocol::CSBuyCmdGiftReq *)msg;
	//先判断新型万能直购卡,不满足再判断旧的
	if (role->GetRoleModuleManager()->OnNewBuyGiftItem(req->param1, req->param2, req->param3, __FUNCTION__))
	{
		return;
	}

	const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();
	role->GetRoleModuleManager()->OnBuyGiftItem(req->param1, req->param2, req->param3, other_cfg.universal_card_item_id, __FUNCTION__, true);
}

void MessageHandler::OnPartnerHandleReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSPartnerHandleRequest * op_info = (Protocol::CSPartnerHandleRequest *)msg;

	Partner * role_partner = role->GetRoleModuleManager()->GetPartner();
	switch (op_info->op_type)
	{
	case Protocol::CSPartnerHandleRequest::PARTNER_INFO_REQUIRE:	// 0 - 请求所有伙伴信息
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
			{
				return;
			}
			role_partner->SendAllPartnerInfo();
		}
		break;

	case Protocol::CSPartnerHandleRequest::PARTNER_DISP_PARTNER:	// 1 - 伙伴出战状态改变
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
			{
				return;
			}
			role_partner->ChangePartnerFightState(op_info->partner, __FUNCTION__);
		}	
		break;

	case Protocol::CSPartnerHandleRequest::PARTNER_ATTR_BREAKTH:	// 2 - 属性突破 / 能力进阶
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
			{
				return;
			}
			role_partner->PartnerBreakTAndUpGrade(op_info->partner);
		}
		break;

	case Protocol::CSPartnerHandleRequest::PARTNER_WASH_FETTERS:	// 3 - 情缘洗练
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
			{
				return;
			}
			role_partner->PartnerFettersBaptize(op_info->partner);
		}
		
		break;

	case Protocol::CSPartnerHandleRequest::PARTNER_SAVE_FETTERS:	// 4 - 保存情缘洗练结果
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
			{
				return;
			}
			role_partner->PartnerFettersResultSave(op_info->partner);
		}
		break;

	case Protocol::CSPartnerHandleRequest::PARTNER_ACTI_PARTNER:	// 5 - 激活伙伴
		{
			role_partner->ActivePartnerByConsumingItem(op_info->partner);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_SIGNLE_INFO:		//6 - 请求单个伙伴信息
		{
			role_partner->SendPartnerSingleInfo(op_info->partner);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_UP_LEVEL:		
		{
			role_partner->UpLevel(op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_RESET_LEVEL:		
		{
			role_partner->ResetLevel(op_info->partner);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_OPEN_BIOGRAPHY:
		{
			role_partner->OnOpenBiography(op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_FETCH_BGP_REWARD:
		{
			role_partner->OnFetchBiography(op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_PAGE_COMPOSE:
		{
			role_partner->OnPageCompose(op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_UPLEVEL_GIFT_TREE:
		{
			role_partner->UpLevelGiftTree(op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_RESET_GIFT_TREE:
		{
			role_partner->ResetGiftTree(op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_PACT_PUT_ON:
		{
			role_partner->PactPutOn(op_info->partner, op_info->param1, op_info->param2);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_PACT_GET_OFF:
		{
			role_partner->PactGetOff(op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_PACT_UP_STAR:
		{
			role_partner->PactUpStar(op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_PACT_SWAP:
		{
			role_partner->PactSwap(op_info->param2, op_info->param3, op_info->partner, op_info->param1);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_SOAR:
		{
			role_partner->OnPartnerSoar(op_info->partner);
		}
		break;
	case Protocol::CSPartnerHandleRequest::PARTNER_SOAR_ARMS_LEVEL_UP:
		{
			role_partner->OnPartnerSoarArmsLevelUp(op_info->partner);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnPartnerSkillSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	/*
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
	{
		return;
	}

	Protocol::CSToSaveSkillSelection * p_s_info = (Protocol::CSToSaveSkillSelection *)msg;
	role->GetRoleModuleManager()->GetPartner()->PartnerSkillSeletorSave(p_s_info->partner, p_s_info->index, p_s_info->skill);
	*/
}

void MessageHandler::OnPartnerAlterList(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
	{
		return;
	}

	Protocol::CSChangeSyncFightOrder * p_s_csfo = (Protocol::CSChangeSyncFightOrder *)msg;
	role->GetRoleModuleManager()->GetPartner()->PartnerSynFightListAlter(p_s_csfo->insert_partner, p_s_csfo->target_partner);
}

void MessageHandler::OnPartnerSwitchFightPos(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
	{
		return;
	}

	Protocol::CSPartnerSwitchFightPos * req = (Protocol::CSPartnerSwitchFightPos *)msg;
	role->GetRoleModuleManager()->GetPartner()->PartnerSwitchFightPost(req->partnerA_id, req->partnerB_id);
}

void MessageHandler::ONPartnerQualityUpgrade(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
	{
		return;
	}
	Protocol::CSPartnerQualityUpReq * req = (Protocol::CSPartnerQualityUpReq *)msg;

	if (req->use_type == NorexItem::I_NOREX_ITEM_PARTNER_QUALITYUP)
	{
		role->GetRoleModuleManager()->GetPartner()->QualityUpgrade(req->partner_id, req->up_type);
	}

}

void MessageHandler::OnPartnerElementCrytalReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
	{
		return;
	}

	Protocol::CSPartnerElementCrytalOper * req = (Protocol::CSPartnerElementCrytalOper *)msg;
	switch (req->oper_type)
	{
	case Protocol::CSPartnerElementCrytalOper::ELEMENT_CRYTALE_USE_TO_ONCE:
		role->GetRoleModuleManager()->GetPartner()->UseElementCrystal(req->param_1, req->param_2);
		break;

	case Protocol::CSPartnerElementCrytalOper::ELEMENT_CRAYTAL_REMOVE_TO_ONCE:
		role->GetRoleModuleManager()->GetPartner()->RemoveElementCrystal(req->param_1);
		break;

	case Protocol::CSPartnerElementCrytalOper::ELEMENT_CRAYTAL_USE_TO_FIGHT_PARTNER:
		role->GetRoleModuleManager()->GetPartner()->UseElementCrystalToFight(req->param_1, req->param_2, req->param_3, req->param_4);
		break;

	case Protocol::CSPartnerElementCrytalOper::ELEMENT_CRAYTAL_REMOVE_FROM_EXIST_PARTNER:
		role->GetRoleModuleManager()->GetPartner()->RemoveAllCurPartnerElementCrystal();
		break;
	}
}

void MessageHandler::OnPartnerSkinReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPartnerSkinReq * req = (Protocol::CSPartnerSkinReq *)msg;
	switch (req->oper_type)
	{
	case Protocol::CSPartnerSkinReq::SKIN_AVTIVE: // 激活伙伴皮肤
		role->GetRoleModuleManager()->GetPartner()->ActiveSkin(req->param_1, req->param_2);
		break;

	case Protocol::CSPartnerSkinReq::SKIN_PUT_ON: // 穿戴皮肤
		role->GetRoleModuleManager()->GetPartner()->PutOnSkin(req->param_1, req->param_2);
		break;

	case Protocol::CSPartnerSkinReq::SKIN_PUT_OFF: // 卸下皮肤
		role->GetRoleModuleManager()->GetPartner()->PutOffSkin(req->param_1, req->param_2);
		break;
	}
}

void MessageHandler::OnPartnerEqRequest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PARTNER, true))
	{
		return;
	}

	Protocol::CSPartnerEqOperatorReq * req = (Protocol::CSPartnerEqOperatorReq *)msg;
	switch (req->op_type)
	{
	case Protocol::CSPartnerEqOperatorReq::PE_OP_PULL_ON_EQUIP:
		role->GetRoleModuleManager()->GetPartner()->PartnerEqPullOnEquipment(req->partner_id, req->param1);
		break;
	case Protocol::CSPartnerEqOperatorReq::PE_OP_GET_OFF_EQUIP:
		role->GetRoleModuleManager()->GetPartner()->PartnerEqGetOffEquipment(req->partner_id, req->param1);
		break;
	case Protocol::CSPartnerEqOperatorReq::PE_OP_UNLOCK_EQUIP:
		role->GetRoleModuleManager()->GetPartner()->PartnerEqUnlockEquipment(req->partner_id, req->param1);
		break;
	case Protocol::CSPartnerEqOperatorReq::PE_OP_RESOLVE_EQUIP:
		role->GetRoleModuleManager()->GetPartner()->PartnerEqResolveEquipment(req->param1);
		break;
	}
}

void MessageHandler::OnTitleOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TITLE, true))
	{
		return;
	}

	Protocol::CSTitleOp * op = (Protocol::CSTitleOp *)msg;
	switch (op->type)
	{
	case Protocol::CSTitleOp::WEAR:
	case Protocol::CSTitleOp::GETOFF:		
		role->GetRoleModuleManager()->GetTitle()->OnWearTitle(op->param);	
		break;
	default:
		break;
	}
}

void MessageHandler::OnTrainOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_XIULIAN, true))
	{
		return;
	}

	Protocol::CSTrainReq * op = (Protocol::CSTrainReq *)msg;

	switch (op->req_type)
	{
		case Protocol::CS_TRAIN_REQ_TYPE_TRAIN:
			{
				if (1 >= op->param2)
				{//op->param2>1 时 若跨等级升级 消耗资源只会按当前等级的配置计算 
				role->GetRoleModuleManager()->GetTrain()->OnTrainUp(op->param1, op->param2);
			}
				else
				{//一键升级
					role->GetRoleModuleManager()->GetTrain()->OnTrainUpFast(op->param1, op->param2);
				}
			}
			break;
		case Protocol::CS_TRAIN_REQ_TYPE_LIU_LI_DING:
			{
				role->GetRoleModuleManager()->GetTrain()->ConvertItem(op->param1);
			}
			break;
		case Protocol::CS_TRAIN_REQ_TYPE_INFO_PROMOTED:
			{
				role->GetRoleModuleManager()->GetTrain()->SendPromotedInfo();
			}
			break;
		case Protocol::CS_TRAIN_REQ_TYPE_SET_RED_DOT:
			{
				role->GetRoleModuleManager()->GetTrain()->SetRedDotFlag(op->param1);
			}
			break;
		case Protocol::CS_TRAIN_REQ_TYPE_PET_TRAIN_2:
			{
				role->GetRoleModuleManager()->GetTrain()->OnPetTrainUp2(op->param1, op->param2);
			}
			break;
		case Protocol::CS_TRAIN_REQ_TYPE_PARTNER_TRAIN:
			{
				role->GetRoleModuleManager()->GetTrain()->OnPartnerTrainUp(op->param1, op->param2);
			}
			break;
	}	
}

void MessageHandler::OnRolePetTrainSkillOp(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRolePetTrainSkillReq* op = (Protocol::CSRolePetTrainSkillReq*)msg;
	role->GetRoleModuleManager()->GetTrain()->OnPetTrainUp(op->train_num);
}

void MessageHandler::OnHeartbeatReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	//Protocol::CSHeartbeatReq* req = (Protocol::CSHeartbeatReq*)msg;
	
	//在gateway回包
	//Protocol::SCHeartbeatResp resp;
	//EngineAdapter::Instance().NetSend(netid, (const char*)&resp, sizeof(resp));
}

void MessageHandler::OnPrestigeShoping(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPrestigeShopingReq* req = (Protocol::CSPrestigeShopingReq*)msg;
	role->GetRoleModuleManager()->GetPrestige()->OnShoping(req->index, req->num);
}

void MessageHandler::OnPrestigeDonateOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPrestigeDonateOp * op = (Protocol::CSPrestigeDonateOp *)msg;
	role->GetRoleModuleManager()->GetPrestige()->OnDonate(op->village_id, op->column, op->index);
}

void MessageHandler::OnTaskInfoSingltHandle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTaskInfoSingleHandle * tish = (Protocol::CSTaskInfoSingleHandle *)msg;

	switch (tish->op_type)
	{
	case Protocol::CSTaskInfoSingleHandle::TASK_ACCEPT:
		role->GetRoleModuleManager()->GetTaskManager()->AcceptTask(tish->task_id);
		break;

	case Protocol::CSTaskInfoSingleHandle::TASK_COMMIT:
		role->GetRoleModuleManager()->GetTaskManager()->CommitTask(tish->task_id);
		break;

	case Protocol::CSTaskInfoSingleHandle::TASK_GIVEUP:
		role->GetRoleModuleManager()->GetTaskManager()->GiveupTask(tish->task_id);
		break;

	case Protocol::CSTaskInfoSingleHandle::TASK_DEMAND:
		role->GetRoleModuleManager()->GetTaskManager()->DemandTask(tish->task_id);
		break;
	
	case Protocol::CSTaskInfoSingleHandle::TASK_PATROL:
		if (tish->task_id == 0)
		{
			role->GetRoleModuleManager()->GetTaskManager()->SendPatrolTaskInfo();
		}
		else if (tish->task_id == 1)
		{
			role->GetRoleModuleManager()->GetTaskManager()->AcceptPatrolTask();
		}
		else if (tish->task_id == 2)
		{
			role->GetRoleModuleManager()->GetTaskManager()->CommitPatrolTask();
		}
		break;
	
	case Protocol::CSTaskInfoSingleHandle::TASK_SKIP_BATTLE:
		{
			role->GetRoleModuleManager()->GetTaskManager()->TaskSkipBattle(tish->task_id);
		}
		break;

	default:
		break;
	}
}

void MessageHandler::OnTalkWithAppointedNPC(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTalkWithAppointedNPC * twan = (Protocol::CSTalkWithAppointedNPC *)msg;
	role->GetRoleModuleManager()->GetTaskManager()->OnTalkWithNPC(twan->task_id, twan->npc_id);
}

void MessageHandler::OnGiveNPCTaskNeedItems(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGiveNPCTaskNeedItems * gntni = (Protocol::CSGiveNPCTaskNeedItems *)msg;
	if (gntni->list_count <= 0) return;
	
	if (length != static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSGiveNPCTaskNeedItems, commit_list) + sizeof(gntni->commit_list[0]) * gntni->list_count))
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}
	
	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(gntni->task_id);
	if (NULL == p_tic)
	{
		role->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
		return;
	}

	switch (p_tic->task_condition)
	{
	case TASK_NEED_GIVE_PROGROUP:
		{
			std::set<ItemContainer> t_ic_list;
			for (int t_cur = 0; t_cur < gntni->list_count; ++t_cur)
			{
				const ItemContainer & item = gntni->commit_list[t_cur];
				t_ic_list.insert(item);
			}

			role->GetRoleModuleManager()->GetTaskManager()->OnGiveProGroup(gntni->task_id, t_ic_list);


			Team* team = role->GetMyTeam();
			if (NULL != team && team->GetLeaderUID() == role->GetUID())
			{
				for (int i = 0; i < team->GetMemberCount(); ++i)
				{
					TeamMember* member = team->GetMemberInfoByIndex(i);
					if (NULL == member || member->IsRobot() || !member->is_follow) continue;

					Role* member_role = team->GetMemberRoleByIndex(i);
					if (NULL != member_role && member_role->GetUID() != role->GetUID())
					{
						member_role->GetRoleModuleManager()->GetTaskManager()->OnGiveProGroup(gntni->task_id, t_ic_list);
					}
				}
			}
		}
		break;
	case TASK_NEED_GIVE_PROPERTY:
		{
			const ItemContainer & item = gntni->commit_list[0];
			role->GetRoleModuleManager()->GetTaskManager()->OnGiveProperty(gntni->task_id, item.item_id, item.com_num);

			Team* team = role->GetMyTeam();
			if (NULL != team && team->GetLeaderUID() == role->GetUID())
			{
				for (int i = 0; i < team->GetMemberCount(); ++i)
				{
					TeamMember* member = team->GetMemberInfoByIndex(i);
					if (NULL == member || member->IsRobot() || !member->is_follow) continue;

					Role* member_role = team->GetMemberRoleByIndex(i);
					if (NULL != member_role && member_role->GetUID() != role->GetUID())
					{
						member_role->GetRoleModuleManager()->GetTaskManager()->OnGiveProperty(gntni->task_id, item.item_id, item.com_num);
					}
				}
			}
		}
		break;
	case TASK_NEED_COMMIT_PET:
	{
		const ItemContainer & item = gntni->commit_list[0];
		int pet_index = item.item_id;
		role->GetRoleModuleManager()->GetTaskManager()->OnCommitPet(gntni->task_id, pet_index);
	}
	break;
	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

void MessageHandler::OnSetupRaskTraceStatus(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSetupTaskTraceStatus * stts = (Protocol::CSSetupTaskTraceStatus *)msg;

	if (stts->list_num <= 0) return;
	
	if (length != static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSSetupTaskTraceStatus, param_list) + sizeof(stts->param_list[0]) * stts->list_num))
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}
		
	role->GetRoleModuleManager()->GetTaskManager()->SetupTrace(stts->list_num, stts->param_list);
}

void MessageHandler::OnPlayTaskGame(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
}

void MessageHandler::OnFlyByShoe(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSFlyByShoe *fly_req = (Protocol::CSFlyByShoe*)msg;

	gamelog::g_log_debug.printf(LL_DEBUG, "MessageHandler::OnFlyByShoe user(%d,%s) target_pos(%d,%d) target_scene(%d) cur_pos(%d,%d) cur_scene(%d)", 
		role->GetUID(), role->GetName(), fly_req->pos_x, fly_req->pos_y, fly_req->scene_id, role->GetPos().x, role->GetPos().y, scene->GetSceneID());

	World::GetInstWorld()->GetSceneManager()->FlyByShoe(role, fly_req->scene_id, fly_req->pos_x, fly_req->pos_y);
}

void MessageHandler::OnBattleDemoFightStartReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	BattleManagerLocal::GetInstance().StartDemoFightReq(role);
}

void MessageHandler::OnBattlePrepareFinishReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	BattleManagerLocal::GetInstance().PrepareFinishReq(role);
}

void MessageHandler::OnBattleAnimationFinishReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	BattleManagerLocal::GetInstance().AnimationFinishReq(role);
}

void MessageHandler::OnBattleForceFinishFightReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	BattleManagerLocal::GetInstance().ForceFinishFightReq(role);
}

void MessageHandler::OnCollectionOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCollectionOp *op = (Protocol::CSCollectionOp*)msg;
	switch (op->op_type)
	{
	case Protocol::CSCollectionOp::ACTIVATE:
		if (!FunOpen::IsFunOpen(role, FUN_TYPE_COLLECTION, true))
		{
			return;
		}
		role->GetRoleModuleManager()->GetCollection()->OnActivateCollection(op->p1);
		break;
	case Protocol::CSCollectionOp::CONVERT:
		if (!FunOpen::IsFunOpen(role, FUN_TYPE_COLLECTION, true))
		{
			return;
		}
		role->GetRoleModuleManager()->GetCollection()->OnConvert(op->p1);
		break;
	case Protocol::CSCollectionOp::INCLUDED:
		if (!FunOpen::IsFunOpen(role, FUN_TYPE_COLLECTION_INCLUDED, true))
		{
			return;
		}
		role->GetRoleModuleManager()->GetCollection()->ActiveCollection(op->p1);
		break;
	case Protocol::CSCollectionOp::TREASURES:
		/*if (!FunOpen::IsFunOpen(role, FUN_TYPE_COLLECTION_INCLUDED, true))
		{
			return;
		}*/
		role->GetRoleModuleManager()->GetCollection()->UpLevelAtlasCard(op->p1);
		break;
	}
}

void MessageHandler::OnWstQuestionOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	return;//屏蔽神算子玩法
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ANSWER, true))
	{
		return;
	}
	Protocol::CSAnswerOp *op = (Protocol::CSAnswerOp*)msg;
	switch (op->op_type)
	{
	case Protocol::CSAnswerOp::OP_TYPE_BUY_EXP:
		role->GetRoleModuleManager()->GetWstQuestion()->OnBuyExp(op->param1);
		break;
	case Protocol::CSAnswerOp::OP_TYPE_GET_INFO:
		role->GetRoleModuleManager()->GetWstQuestion()->SendAllInfo();
		break; 
	case Protocol::CSAnswerOp::OP_TYPE_QUESTION:
		role->GetRoleModuleManager()->GetWstQuestion()->OnQuestion(op->param1);
		break;
	case Protocol::CSAnswerOp::OP_TYPE_GET_WST_INFO:
		role->GetRoleModuleManager()->GetWstQuestion()->SendWstInfo();
	}
}

void MessageHandler::OnExpCrystalOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetKnapsack()->OnRoleModifyExpCrystal();
}

void MessageHandler::OnTreviFountainOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_WISH_POOL, true)) return;

	role->GetRoleModuleManager()->OnTreviFountainBlessing();
}

void MessageHandler::OnBattleRoleOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleRoleOperate* req = (Protocol::CSBattleRoleOperate*)msg;

	BattleManagerLocal::GetInstance().RoleOperateReq(role, req->operate_type, req->param1, req->param2, req->param3, req->param4);
}

void MessageHandler::OnSkillOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSkillOperateReq* req = (Protocol::CSSkillOperateReq*)msg;

	role->GetRoleModuleManager()->GetRoleSkill()->OnRoleOperateReq(req->operate_type, req->param1, req->param2, req->param3);
}

void MessageHandler::OnNewLifeSkillReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSNewLifeSkillReq * req = (Protocol::CSNewLifeSkillReq *)msg;

	switch (req->op_type)
	{
		case Protocol::CSNewLifeSkillReq::NEW_LIFE_SKILL_OP_TYPE_ALL_INFO:
		{
			role->GetRoleModuleManager()->GetRoleNewLifeSkill()->SendAllInfo();
		}
		break;
		case Protocol::CSNewLifeSkillReq::NEW_LIFE_SKILL_OP_TYPE_LEVEL_UP:
		{
			role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnSkillLevelUp(req->param1);
		}
		break;
		case Protocol::CSNewLifeSkillReq::NEW_LIFE_SKILL_OP_TYPE_MAKE_ITEM:
		{
			role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnMakeItem(req->param1, 0 != req->param2, req->param3);
		}
		break;
		case Protocol::CSNewLifeSkillReq::NEW_LIFE_SKILL_OP_TYPE_PROPS_INFO:
		{
			role->GetRoleModuleManager()->GetRoleNewLifeSkill()->SendPropsAllInfo();
		}
		break;
		case Protocol::CSNewLifeSkillReq::NEW_LIFE_SKILL_OP_TYPE_USE_CARD:
		{
			role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnSetUseChangeCard(0 != req->param1);
		}
		break;
		case Protocol::CSNewLifeSkillReq::NEW_LIFE_SKILL_OP_TYPE_START_SURFACE:
		{
			role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnStartSurface(req->param1);
		}
		break;
		case Protocol::CSNewLifeSkillReq::NEW_LIFE_SKILL_OP_TYPE_RELIEVE_SURFACE:
		{
			role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnRelieveSurface(NEW_LIFE_SKILL_RELIEVE_SURFACE_TYPE_CLIENT);
		}
		break;
		default:
		break;
	}
}

void MessageHandler::OnFishGroundReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFishGroundReq * req = (Protocol::CSFishGroundReq *)msg;
	SpecialLogic * special_logic = scene->GetSpecialLogic();
	if (special_logic == NULL || special_logic->GetSceneType() != Scene::SCENE_TYPE_FISHING_GROUND)
	{
		return;
	}
	SpecialLogicFishingGround * fish_ground_scene = (SpecialLogicFishingGround *)special_logic;
	switch (req->op_type)
	{
		case Protocol::CSFishGroundReq::FISH_GROUND_REQ_TYPE_START:
		{
			fish_ground_scene->OnStartFish(role);
		}
		break;
		case Protocol::CSFishGroundReq::FISH_GROUND_REQ_TYPE_END:
		{
			fish_ground_scene->OnEndFishing(role);
		}
		break;
		case Protocol::CSFishGroundReq::FISH_GROUND_REQ_TYPE_STOP:
		{
			fish_ground_scene->OnStopFish(role);
		}
		break;
		default:
		break;
	}
}

void MessageHandler::OnFetchRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFetchRewardReq* req = (Protocol::CSFetchRewardReq*)msg;
	int ret = 0;
	switch (req->reward_type)
	{
	case Protocol::CSFetchRewardReq::ONLINE_REWARD:
		ret = role->GetRoleModuleManager()->GetDayReward()->OnFetchOnlineReward(req->seq);
		break;
	case Protocol::CSFetchRewardReq::SIGN_REWARD:
		ret = role->GetRoleModuleManager()->GetDayReward()->OnFetchSignReward(req->seq);
		break;
	case Protocol::CSFetchRewardReq::LEVEL_REWARD:
		ret = role->GetRoleModuleManager()->GetDayReward()->OnFetchLevelReward(req->seq);
		break;
	case Protocol::CSFetchRewardReq::SCORE_REWARD:
		ret = role->GetRoleModuleManager()->GetRoleActivityManager()->OnFetchScoreReward(req->seq);
		break;
	case Protocol::CSFetchRewardReq::MONTH_INVESTMENT:
		ret = role->GetRoleModuleManager()->GetRoleActivityManager()->OnMonthInvestmentOp(0,req->seq);
		break;
	case Protocol::CSFetchRewardReq::FETCH_DAY_FOOD:
		ret = role->GetRoleModuleManager()->GetDayReward()->OnFetchDayFood(req->seq);
		break;
	case Protocol::CSFetchRewardReq::FETCH_ONLINE_GIFT:
		ret = role->GetRoleModuleManager()->GetDayReward()->OnFetchOnlineGift(req->seq);
		break;
	}

	if (ret < 0)
	{
		gamelog::g_log_role_activity.printf(LL_WARNING, "%s Error user[%d %s] reward_type:%d ret:%d", __FUNCTION__, role->GetUID(), role->GetName(), req->reward_type, ret);
	}
}

void MessageHandler::OnSurfaceOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSurfaceOp* req = (Protocol::CSSurfaceOp*)msg;
	int ret = 0;
	switch (req->op_type)
	{
	case Protocol::CSSurfaceOp::WEAR:
		ret = role->GetRoleModuleManager()->GetSurface()->WearSurface(req->param2, req->param1);
		break;
	case Protocol::CSSurfaceOp::EXCHANGE:
		ret = role->GetRoleModuleManager()->GetSurface()->ExchangegItem(req->param1);
		break;
	}

	if (ret < 0)
	{
		gamelog::g_log_surface.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}

}

void MessageHandler::OnFindRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFindRewardReq* req = (Protocol::CSFindRewardReq*)msg;
	int ret = role->GetRoleModuleManager()->GetDayReward()->OnFindReward(req->type, req->count, req->consume_type);
	if (ret < 0)
	{
		gamelog::g_log_role_activity.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnFindRewardNotifysCancelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	//Protocol::CSRewardFindNotifyCancelReq* req = (Protocol::CSRewardFindNotifyCancelReq*)msg;
	role->GetRoleModuleManager()->GetDayReward()->OnFindRewardCancelNotify();
}

void MessageHandler::OnSurfaceDecompose(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSurefaceDecompose* req = (Protocol::CSSurefaceDecompose*)msg;
	int ret = 0;
	ret = role->GetRoleModuleManager()->GetFashionCollection()->DecomposeSurface(req->column, req->index, req->num);
	if (ret < 0)
	{
		gamelog::g_log_surface.printf(LL_WARNING, "%s Error user[%d %s] DecomposeSurface Failed!! ret[%d]\n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnEquipmentOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSEquipmentOperaReq* req = (Protocol::CSEquipmentOperaReq*)msg;

	switch (req->msg_type)
	{
	case Protocol::CSEquipmentOperaReq::OPERA_TYPE_INFO_REQ:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->SendEquipmentInfoToRole();
		}
		break;

	case Protocol::CSEquipmentOperaReq::OPERA_TYPE_PUTON:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->PutOn(static_cast<short>(req->param1), 
				static_cast<short>(req->param2), static_cast<short>(req->param3));
		}
		break;

	case Protocol::CSEquipmentOperaReq::OPERA_TYPE_TAKEOFF:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->TakeOff(static_cast<short>(req->param1));
		}
		break;

	case Protocol::CSEquipmentOperaReq::OPERA_TYPE_TAKEOFF_CRYSTAL:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->TakeOffElementCrystal();
		}
		break;
	case Protocol::CSEquipmentOperaReq::OPERA_TYPE_VOC_LOCK:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->LockEquipMentVoc(static_cast<short>(req->param1), 
				static_cast<short>(req->param2), static_cast<short>(req->param3), static_cast<short>(req->param4));
		}
		break;
	case Protocol::CSEquipmentOperaReq::OPERA_TYPE_UPGREADE:
		{
			if (req->param1 == 0)
			{
				role->GetRoleModuleManager()->GetEquipmentManager()->SendUpGreadeInfo();
			}
			else if (req->param1 == 1)
			{
				role->GetRoleModuleManager()->GetEquipmentManager()->UpGreade(req->param2);
			}
		}
		break;
	case Protocol::CSEquipmentOperaReq::OPERA_TYPE_RESONACE:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->OpenResonaceLevel();
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnJewelryHandleRequest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_LINGSHI, true))
	{
		return;
	}

	Protocol::CSJewelryHandleReq * jhr = (Protocol::CSJewelryHandleReq *)msg;

	switch (jhr->op_type)
	{
	case Protocol::CSJewelryHandleReq::OPERA_TYPE_ALL_INFO_REQ:
		role->GetRoleModuleManager()->GetEquipmentManager()->SendAllJewelryInfo();
		break;

	case Protocol::CSJewelryHandleReq::OPERA_TYPE_WEAR_JEWELRY:
		role->GetRoleModuleManager()->GetEquipmentManager()->WearJewelry(jhr->param);
		break;

	case Protocol::CSJewelryHandleReq::OPERA_TYPE_TAKE_OFF_ONE:
		role->GetRoleModuleManager()->GetEquipmentManager()->TakeOffJewelry(jhr->param);
		break;

	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

void MessageHandler::OnObtainOfflineExpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	//Protocol::CSOfflineExpReceives * oer = (Protocol::CSOfflineExpReceives *)msg;
	role->GetRoleModuleManager()->GetOfflineExp()->ReceiveExp();
}

void MessageHandler::OnWorkshopOperateReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_WORKSHOP, true))
	{
		return;
	}

	Protocol::CSWorkshopOperateRequire * wor = (Protocol::CSWorkshopOperateRequire *)msg;
	
	switch (wor->op_type)
	{
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_COMPOUND_REQ:
		role->GetRoleModuleManager()->GetWorkshop()->CompoundItem(wor->param1, wor->param2, wor->param3, 0 != wor->param4);
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_GEM_MOUNTING:
		role->GetRoleModuleManager()->GetWorkshop()->EquipGemMounting(wor->param1, wor->param2, wor->param3, wor->param4);
		break;
		
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_GEM_DISMOUNT:
		role->GetRoleModuleManager()->GetWorkshop()->EquipGemDismount(wor->param1, wor->param2, wor->param3, wor->param4);
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_MAKE_JEWELTY:
		//role->GetRoleModuleManager()->GetWorkshop()->CraftJewelry(wor->param1);
		role->GetRoleModuleManager()->GetWorkshop()->CompoundItem(wor->param1, wor->param2, wor->param3);
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_JEWE_REFORGE:
		role->GetRoleModuleManager()->GetWorkshop()->ReforgeJewelry(wor->param1, wor->param2);
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_GAIN_REFORGE:
		role->GetRoleModuleManager()->GetWorkshop()->ChooseReforgeRetain(wor->param1);
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_GEM_UPGRADES:
		role->GetRoleModuleManager()->GetWorkshop()->EquipGemUpgrades(wor->param1, wor->param2, wor->param3, wor->param4);
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_GEM_CONF_UGD:
		role->GetRoleModuleManager()->GetWorkshop()->ConfirmGemUpgrade();
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_SYNTHESIS:
		role->GetRoleModuleManager()->GetWorkshop()->ExchangeTarget(wor->param1, wor->param2);
		break;

	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_DISCOMPOUND_REQ:
		role->GetRoleModuleManager()->GetWorkshop()->DiscompoundItem(wor->param1, wor->param2,wor->param3);
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_BATCH_COMP_REQ:
		role->GetRoleModuleManager()->GetWorkshop()->BatchCompoundItem(wor->param1, wor->param2);
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_AVTIVE_EQUIP:
		{
			if (FunOpen::IsFunOpen(role, FUN_TYPE_SMELT, true))
			{
		role->GetRoleModuleManager()->GetWorkshop()->OnActiveEquipVocAttr(wor->param1, wor->param2, wor->param3, wor->param4, wor->param5);
			}
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_REFRESH_EQUIP:
		{
			if (FunOpen::IsFunOpen(role, FUN_TYPE_SMELT, true))
			{
		role->GetRoleModuleManager()->GetWorkshop()->OnRefreshEquipVocAttr(wor->param1, wor->param2, wor->param3, wor->param4, wor->param5, wor->param6 == 1 ? true : false);
			}
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_SAVE_EQUIP_REFRESH:
		{
			if (FunOpen::IsFunOpen(role, FUN_TYPE_SMELT, true))
			{
		role->GetRoleModuleManager()->GetWorkshop()->OnSaveEquipVocAttr(wor->param1, wor->param2, wor->param3, wor->param4);
			}
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_ULTIMATE_WORK:
		{
			if (wor->param1 == 1)
			{
				role->GetRoleModuleManager()->GetWorkshop()->NormalUltimate(wor->param2, wor->param3, wor->param4, wor->param5);
			}
			else if (wor->param1 == 2)
			{
				role->GetRoleModuleManager()->GetWorkshop()->StrongUltimate(wor->param2, wor->param3, wor->param4, wor->param5);
			}
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_GOD_STRONGTHEN:
		{
			if (wor->param1 == 1)
			{
				role->GetRoleModuleManager()->GetWorkshop()->UpEquipGrade(wor->param2, wor->param3, wor->param4, wor->param5);
			}
			else if (wor->param1 == 2)
			{
				role->GetRoleModuleManager()->GetWorkshop()->BreakThroughGarde(wor->param2, wor->param3, wor->param4, wor->param5);
			}
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_DIAN_HUA:
		{
			role->GetRoleModuleManager()->GetWorkshop()->DianHuaEquip(wor->param1, wor->param2, wor->param3, wor->param4);
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_SAVE_DIAN_HUA:
		{
			role->GetRoleModuleManager()->GetWorkshop()->SaveDianHuaEquip(wor->param1, wor->param2);
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_FU_MO:
		{
			role->GetRoleModuleManager()->GetWorkshop()->FuMoEquip(wor->param1, wor->param2);
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_XI_FU:
		{
			role->GetRoleModuleManager()->GetWorkshop()->XiFuEquip(wor->param1, wor->param2);
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_DISCOMPOSE:
		{
			role->GetRoleModuleManager()->GetWorkshop()->DisComposeDianHuaShi(wor->param1, wor->param2);
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_UP_STAR:
		{
			role->GetRoleModuleManager()->GetWorkshop()->EquipUpStar(wor->param1, wor->param2, wor->param3, wor->param4, wor->param5, wor->param6);
		}
		break;
	case Protocol::CSWorkshopOperateRequire::WORKSHOP_OP_UP_LEVEL_DIANHUA:
		{
			role->GetRoleModuleManager()->GetWorkshop()->ComposeDianHuaShi(wor->param1, wor->param2, wor->param3);
		}
		break;
	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

void MessageHandler::OnConvertPetSkillReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSConvertPetSkillBookReq * req = (Protocol::CSConvertPetSkillBookReq *)msg;
	int offset = UNSTD_OFFSETOFF(Protocol::CSConvertPetSkillBookReq, grid_list);
	if (length < offset || req->list_num <= 0 || length != offset + (int)sizeof(req->grid_list[0]) * req->list_num)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}

	std::vector<ItemPairCon> tmp_grid_list; std::set<ItemID> tmp_item_id_checker;
	for (int idx = 0; idx < PET_SKILL_BOOK_NEED_ITEM_TYPE_NUM && idx < req->list_num; ++idx)
	{
		const ItemPairCon & item_pair = req->grid_list[idx];
		if (item_pair.item_num <= 0) continue;

		if (tmp_item_id_checker.end() != tmp_item_id_checker.find(item_pair.item_id))
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		tmp_grid_list.push_back(item_pair);
		tmp_item_id_checker.insert(item_pair.item_id);
	}

	role->GetRoleModuleManager()->GetWorkshop()->ExchangePetSkillBook(req->exc_seq, tmp_grid_list);
}

void MessageHandler::OnEquipVocChangeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SMELT, true))
	{
		return;
	}
	Protocol::CSChangeEquipVocReq * req = (Protocol::CSChangeEquipVocReq *)msg;
	switch (req->change_voc_type)
	{
	case Protocol::CSChangeEquipVocReq::VOC_CHANGE_TYPE_EQUIP:
		role->GetRoleModuleManager()->GetWorkshop()->ChangeEquipVoc(req->replacer_posi, req->replacer_grid, req->bereplacer_posi, req->bereplacer_grid, req->role_type);
		break;
	case Protocol::CSChangeEquipVocReq::VOC_CHANGE_TYPE_JEWELRY:
		role->GetRoleModuleManager()->GetWorkshop()->ChangeJewelryVoc(req->replacer_posi, req->replacer_grid, req->bereplacer_posi, req->bereplacer_grid, req->role_type);
		break;
	case Protocol::CSChangeEquipVocReq::VOC_CHANGE_TYPE_ULTMATE_EQUIP:
	case Protocol::CSChangeEquipVocReq::VOC_CHANGE_TYPE_ULTMATE_JEWELRY:
		{
			int item_type = Protocol::CSChangeEquipVocReq::VOC_CHANGE_TYPE_ULTMATE_EQUIP == req->change_voc_type ? EQUIP_TYPE_EQUIPMENT : EQUIP_TYPE_JEWELRY;
			role->GetRoleModuleManager()->GetWorkshop()->VocUltimate(item_type, req->replacer_posi, req->replacer_grid, req->replacer_voc_index, req->role_type);
		}
		break;
	default:
		break;
	}
	
}

void MessageHandler::OnMarryReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_MARRIGE, true))
	{
		return;
	}

	Protocol::CSMarriageReq * protocol = (Protocol::CSMarriageReq *)msg;
	

	switch (protocol->req_type)
	{
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_INFO:
			{
				role->GetRoleModuleManager()->GetMarriage()->SendInfo();
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_PROPOSE:
			{
				role->GetRoleModuleManager()->GetMarriage()->Propose(protocol->param1, protocol->param2);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_CANCEL_PROPOSE:
			{
				role->GetRoleModuleManager()->GetMarriage()->CancelPropose();
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_PROPOSE_REPLY:
			{
				Role * reply_target = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(protocol->param2));
				if (NULL != reply_target)
				{
					reply_target->GetRoleModuleManager()->GetMarriage()->ProposeReply(role, 0 != protocol->param1);
				}				
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_DIVORCE:
			{
				role->GetRoleModuleManager()->GetMarriage()->DivorceReq();
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_DIVORCE_REPLY:
			{
				Role * reply_target = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (NULL != reply_target)
				{
					reply_target->GetRoleModuleManager()->GetMarriage()->ProposeReply(role, 0 != protocol->param1);
				}
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_DIVORCE_FORCE:
			{
				role->GetRoleModuleManager()->GetMarriage()->ForceDivorceReq();
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_REGRET_DIVORCE_FORCE:
			{
				role->GetRoleModuleManager()->GetMarriage()->CancelForceDivorce();
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_TASK_REQ_ROUTE:
			{
				MarriageTask::Instance().SendRoute(role,protocol->param1, protocol->param2, protocol->param3);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_REQ_TASK:
			{
				MarriageTask::Instance().ReqStartTask(role, protocol->param1);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_AUTO_FB:
			{
				MarriageTask::Instance().ReqAutoFb(role);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_ANSWSER_DUET:
			{
				MarriageTask::Instance().AnswerDuetQuestion(role,protocol->param1);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_ANSWSER_SINGLE:
			{
				MarriageTask::Instance().AnswerSingleQuestion(role, protocol->param1);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_QUERY_OTHER:
			{
				role->GetRoleModuleManager()->GetMarriage()->QueryOtherMarriageInfo(protocol->param1);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_DUET_PUZZLE:
			{
				MarriageTask::Instance().OnDuetPuzzleReq(role, protocol->param1);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_NEXT_QUESTION_DUET:
			{
				MarriageTask::Instance().NextDuetQuestion(role);
			}
			break;
		case Protocol::CSMarriageReq::MARRIAGE_REQ_TYPE_NEXT_QUESTION_SINGLE:
			{
				MarriageTask::Instance().NextSingleQuestion(role);
			}
			break;
		default:
			break;
	}
}

void MessageHandler::OnCreateGuildReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCreateGuild * req = (Protocol::CSCreateGuild *)msg;
	req->name[sizeof(req->name) - 1] = 0;
	req->notice[sizeof(req->notice) - 1] = 0;
	req->banner[sizeof(req->banner) - 1] = 0;

	int ret = GuildManager::Instance().CreateGuild(role, req);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildSetBannerReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGuildSetBanner* req = (Protocol::CSGuildSetBanner*)msg;
	req->banner[sizeof(req->banner) - 1] = 0;

	GuildManager::Instance().GuildSetBanner(role, req->banner);
}

void MessageHandler::OnBuildingGuildBuildReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildConstructReq* req = (Protocol::CSGuildConstructReq*)msg;

	int ret = 0;
	if (req->op_type == CSGuildConstructReq::OP_TYPE_MEMBER_SPEED_UP)
	{
		ret = GuildManager::Instance().SpeedUpBuilding(role);
	}
	else
	{
		ret = GuildManager::Instance().BuildingGuildBuild(role, req);
	}
	
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnQuitGuildReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSQuitGuild * req = (Protocol::CSQuitGuild *)msg;
	int ret = GuildManager::Instance().QuitGuild(role, req);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
	else
	{
		role->GetRoleModuleManager()->AddTodayQuitGuildCount();
	}
}

void MessageHandler::OnJoinGuildReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSApplyJoinGuildReq * req = (Protocol::CSApplyJoinGuildReq *)msg;

	if (!FunOpen::IsFunOpen(role, FUN_TYPE_GUILD, true))
	{
		return;
	}

	if (req->one_click)
	{
		GuildManager::Instance().ApplyForJoinOneClick(role);
	}
	else
	{
		GuildManager::Instance().ApplyForJoin(role, req->guild_id);
	}
}

void MessageHandler::OnJoinGuildAck(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSApplyJoinGuildAck * req = (Protocol::CSApplyJoinGuildAck *)msg;
	int ret = GuildManager::Instance().ApplyForJoinAck(role, req);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildExpelOutReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildExpelOutReq * req = (Protocol::CSGuildExpelOutReq *)msg;
	int ret = GuildManager::Instance().ExpelOut(role, req);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildSetNoticeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSetGuildNoticeReq * req = (Protocol::CSSetGuildNoticeReq *)msg;
	int ret = GuildManager::Instance().SetGuildNotice(role, req);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGetGuildList(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGetGuildList * req = (Protocol::CSGetGuildList *)msg;
	int ret = GuildManager::Instance().GetGuildInfo(role, req);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildPetTrainingUp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (NULL == role->GetGuild()) return;
	Protocol::CSGuildPetTrainingUp * req = (Protocol::CSGuildPetTrainingUp *)msg;
	int ret = GuildManager::Instance().GuildTrainingUp(role, req->training_type);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildPetTrainingReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (NULL == role->GetGuild()) return;
	Protocol::CSGuildPetTraining * req = (Protocol::CSGuildPetTraining *)msg;
	int ret = GuildManager::Instance().GuildTraining(role, req->pet_index, req->places_index);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGetGuildTrainingInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (NULL == role->GetGuild()) return;
	GuildManager::Instance().SendPetTrainingInfo(role);
}

void MessageHandler::OnGuildAbdicationOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildAbdicationOp *op = (Protocol::CSGuildAbdicationOp *)msg;
	int ret = GuildManager::Instance().OnGuildAbdicationOp(role, op);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildImpeachmentOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildImpeachmentOp *op = (Protocol::CSGuildImpeachmentOp *)msg;
	int ret = GuildManager::Instance().OnGuildImpeachmentOp(role, op);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildAppointmentOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildAppointmentOp *op = (Protocol::CSGuildAppointmentOp *)msg;
	int ret = GuildManager::Instance().OnGuildAppointmentOp(role, op);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildShopBuy(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildShopBuy *op = (Protocol::CSGuildShopBuy *)msg;
	int ret = GuildManager::Instance().OnGuildShopBuy(role, op);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnGuildInvitationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildInvitationReq *op = (Protocol::CSGuildInvitationReq *)msg;
	GuildManager::Instance().OnGuildInviteOther(role, op->uid);
}

void MessageHandler::OnGuildInvitationRep(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_GUILD, true))
	{
		return;
	}

	Protocol::CSInvitationReply * rep = (Protocol::CSInvitationReply *)msg;
	GuildManager::Instance().OnGuildInviteReply(role, rep->result);
}

void MessageHandler::OnSetNeedApproveJoin(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSetGuildNeedApproveReq * req = (Protocol::CSSetGuildNeedApproveReq *)msg;
	Guild * guild = role->GetGuild();
	if (guild != NULL)
	{
		guild->SetNeedApproveJoin(role, (0 != req->is_need_approve));
	}
}

void MessageHandler::OnMarriageSeekingReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMarriageSeekingOpera *msi = (Protocol::CSMarriageSeekingOpera *)msg;
	MarriageSeekingManager::Instance().MarriageSeekingOperateType(role, msi->type, msi->marriage_seeking_notice);
}

void MessageHandler::OnBabyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_BABY, true))
	{
		return;
	}

	Protocol::CSBabyReq * protocol = (Protocol::CSBabyReq *)msg;

	switch (protocol->req_type)
	{
		case Protocol::CSBabyReq::BABY_REQ_TYPE_INFO:
			{
				role->GetRoleModuleManager()->GetBaby()->SendInfo();
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_DRAW_LOTS:
			{
				role->GetRoleModuleManager()->GetBaby()->Draw();
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_CONTRIBUTION:
			{
				role->GetRoleModuleManager()->GetBaby()->Contribution(protocol->param1);
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_REMOVE:
			{
				role->GetRoleModuleManager()->GetBaby()->RemoveBaby(protocol->param1);
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_UP_LEVEL:
			{
				role->GetRoleModuleManager()->GetBaby()->UpLevel(protocol->param1);
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_PLAY_REQ:
			{
				Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (lover == NULL)
				{
					return;
				}

				if (role->GetRoleModuleManager()->GetBaby()->CanStartPlay())
				{
					return;
				}

				Protocol::SCBabyReqRoute route;
				route.req_type = Protocol::CSBabyReq::BABY_REQ_TYPE_PLAY_REQ;
				EngineAdapter::Instance().NetSend(lover->GetNetId(), (const char*)&route, sizeof(route));
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_PLAY_RET:
			{
				Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (lover == NULL)
				{
					return;
				}

				if (protocol->param1 == 0)
				{
					lover->NoticeNum(errornum::EN_BABY_REFUSE_YOUR_REQ);
					return;
				}

				if (role->GetRoleModuleManager()->GetBaby()->CanStartPlay())
				{
					return;
				}

				Protocol::SCBabyStartPlay start;
				EngineAdapter::Instance().NetSend(lover->GetNetId(), (const char*)&start, sizeof(start));
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&start, sizeof(start));
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_ADD_PLAY_JI_FEN	:
			{
				role->GetRoleModuleManager()->GetBaby()->AddPlayJiFen(protocol->param1);
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_DUET_CREATE_REQ:
			{
				Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (lover == NULL)
				{
					return;
				}

				Protocol::SCBabyReqRoute route;
				route.req_type = Protocol::CSBabyReq::BABY_REQ_TYPE_DUET_CREATE_REQ;
				EngineAdapter::Instance().NetSend(lover->GetNetId(), (const char*)&route, sizeof(route));
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_DUET_CREATE_RET:
			{
				Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (lover == NULL)
				{
					return;
				}

				if (protocol->param1 == 0)
				{
					lover->NoticeNum(errornum::EN_BABY_REFUSE_YOUR_REQ);
					return;
				}

				role->GetRoleModuleManager()->GetBaby()->SelectCreateType(BABY_CREATE_TYPE_DUET);
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_SELECT_SINGLE_CREATE:
			{
				role->GetRoleModuleManager()->GetBaby()->SelectCreateType(BABY_CREATE_TYPE_SINGLE);
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_DUET_CREATE_STAGE_1:
			{
				role->GetRoleModuleManager()->GetBaby()->DuetCreateRegStage1();
			}
			break;
		case Protocol::CSBabyReq::BABY_REQ_TYPE_GIVE_UP_CREATE:
			{
				role->GetRoleModuleManager()->GetBaby()->GiveUpCurrCreateType();
			}
			break;
		default:
			break;
	}
}

void MessageHandler::OnBabyRenameReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBabyRename * protocol = (Protocol::CSBabyRename *)msg;
	role->GetRoleModuleManager()->GetBaby()->ReName(protocol->index, protocol->name);
}

void MessageHandler::OnBabyActiveReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBabyActivte * protocol = (Protocol::CSBabyActivte *)msg;
	switch (protocol->req_type)
	{
		case Protocol::CSBabyActivte::BABY_REQ_TYPE_DUET_ACTIVE:
			{
				role->GetRoleModuleManager()->GetBaby()->ActiveBabyDuet(protocol->name);
			}
			break;
		case Protocol::CSBabyActivte::BABY_REQ_TYPE_SINGLE_ACTIVE:
			{
				role->GetRoleModuleManager()->GetBaby()->ActiveBabySingle(protocol->name);
			}
			break;
		default:
			break;
	}
}

void MessageHandler::OnFriendReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFriendReq * req = (Protocol::CSFriendReq *)msg;

	FriendManager::Instance().OnClientReq(role , req);
}

void MessageHandler::OnFriendEditGruopReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFriendEditGruop * protocol = (Protocol::CSFriendEditGruop *)msg;
	
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSFriendEditGruop, role_id));
	if (length < offset)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}

	int role_id_num = (length - offset) / sizeof(protocol->role_id[0]);

	FriendManager::Instance().EditGroup(role, protocol->group_type, protocol->group_name, role_id_num, protocol->role_id);
}

void MessageHandler::OnFriendMoveGruopReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFriendMoveGruop * protocol = (Protocol::CSFriendMoveGruop *)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSFriendMoveGruop, role_id));
	if (length < offset)
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}

	int role_id_num = (length - offset) / sizeof(protocol->role_id[0]);

	FriendManager::Instance().MoveGroup(role, protocol->group_type, role_id_num, protocol->role_id);
}

void MessageHandler::OnFriendRenameGruopReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFriendRenameGruop * protocol = (Protocol::CSFriendRenameGruop *)msg;

	FriendManager::Instance().RenameGroup(role, protocol->group_type, protocol->group_name);
}

void MessageHandler::OnFriendRecommendListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetFriendRecommend()->SendFriendRecommend();
}

void MessageHandler::OnAddFriendRecommend(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFriendRecommendAddOnListReq *fraolr = (Protocol::CSFriendRecommendAddOnListReq *)msg;

	if (fraolr->is_add_all)
	{		
		role->GetRoleModuleManager()->GetFriendRecommend()->AddAllRecommendFriend();
	}
	else
	{
		role->GetRoleModuleManager()->GetFriendRecommend()->AddOneRecommendFriend(fraolr->add_index);
	}
}

void MessageHandler::OnRefreshFriendRecommend(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (role->GetRoleModuleManager()->GetFriendRecommend()->IsCanRefresh(LAST_USE_TYPE_FLAG_REFRESH))
	{
		role->GetRoleModuleManager()->GetFriendRecommend()->RefreshFriendRecommendList(true);
		role->GetRoleModuleManager()->GetFriendRecommend()->SendFriendRecommend();
		role->GetRoleModuleManager()->GetFriendRecommend()->SetNextRefreshTimestamp(LAST_USE_TYPE_FLAG_REFRESH);
	}
	else
	{
		role->NoticeNum(errornum::EN_FRIEND_NOT_REFRESH);
	}
}

void MessageHandler::OnFuzzySearchRoleInfoByName(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFuzzySearchRoleByName *fsrbn = (Protocol::CSFuzzySearchRoleByName*)msg;
	fsrbn->name[sizeof(GameName) - 1] = 0;

	Protocol::SCFuzzySearchRolesInfo scfsri;
	scfsri.count = 0;
	int role_id = atoi(fsrbn->name);
	if (0 != role_id)
	{
		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
		if (NULL != user_node)
		{
			scfsri.role_info[0].role_id = user_node->uid;
			STRNCPY(scfsri.role_info[0].role_name, user_node->role_name, sizeof(GameName));
			scfsri.role_info[0].avatar_type = user_node->avatar_type;
			scfsri.role_info[0].prof = user_node->profession;
			scfsri.role_info[0].level = user_node->level;
			scfsri.role_info[0].headshot_id = user_node->appearance.headshot_id;
			scfsri.role_info[0].top_level = user_node->top_level;
			scfsri.role_info[0].avatar_timestamp = user_node->avatar_timestamp;
			scfsri.count = 1;
		}
	}
	if (0 != scfsri.count)
	{
		role_id = scfsri.role_info[0].role_id;
	}

	int count = 0;
	UserCacheNode **uc = UserCacheManager::Instance().FuzzySearchUsersByName(fsrbn->name, &count, role_id);
	int index = 0;
	for (int i = scfsri.count; index < count && i < ARRAY_ITEM_COUNT(scfsri.role_info); ++i, ++index)
	{
		scfsri.role_info[i].role_id = uc[index]->uid;
		STRNCPY(scfsri.role_info[i].role_name, uc[index]->role_name, sizeof(GameName));
		scfsri.role_info[i].avatar_type = uc[index]->avatar_type;
		scfsri.role_info[i].prof = uc[index]->profession;
		scfsri.role_info[i].level = uc[index]->level;
		scfsri.role_info[i].top_level = uc[index]->top_level;
		scfsri.role_info[i].headshot_id = uc[index]->appearance.headshot_id;
		scfsri.role_info[i].avatar_timestamp = uc[index]->avatar_timestamp;
	}
	if (scfsri.count + count > ARRAY_ITEM_COUNT(scfsri.role_info))
	{
		scfsri.count = ARRAY_ITEM_COUNT(scfsri.role_info);
	}
	else
	{
		scfsri.count += count;
	}

	if (0 == scfsri.count)
	{
		role->NoticeNum(errornum::EN_FUZZY_SEARCH_NOT_FOUND);
	}
	else if (scfsri.count > 0)
	{
		int size = sizeof(Protocol::SCFuzzySearchRolesInfo) - (ARRAY_ITEM_COUNT(scfsri.role_info) - scfsri.count) * sizeof(scfsri.role_info[0]);
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&scfsri, size);
	}
}

void MessageHandler::OnSmartMountsSystemReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SMART_MOUNTS_SYSTEM, true))
	{
		return;
	}

	Protocol::CSSmartMountsSystemReq *req = (Protocol::CSSmartMountsSystemReq *)msg;
	switch (req->op_type)
	{
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_ALL_INFO:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->SendAllInfo();
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_SINGLE_INFO:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->SendSingleInfo(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnActiveSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_LEVEL_UP:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnLevelUpSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_BREAK_LEVEL:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnBreakLevelSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_REFINE_NATURE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnRefineNatureSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_SKILL_UP:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnSkillUpSmartMounts(req->param1, (req->param2 == 0) ? true : false, req->param3);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_REFINE_SKILL:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnRefineSkillSmartMounts(req->param1, req->param2, (0 != req->param3));
		}
		break;
		case  Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_SAVE_REFINE_SKILL:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnSaveRefineSkillSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_REFINE_SKILL:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnCancelRefineSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_WEAR_ORNAMENTS:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnWearOrnaments(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_TAKE_OFF:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnTakeOffOrnaments(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_ADD_ENERGY:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnAddEnergySmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_RESET_SKILL:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnResetSkillSmartMounts(req->param1, (req->param2 == 0) ? true : false, req->param3);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_DOMINANCE_PET:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnDominancePetSmartMounts(req->param1, req->param2, req->param3);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_DOMINANCE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnCancelDominancePet(req->param1, req->param2);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_ATTR_REQ:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->SendSingleAttr(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_FINISH_FUN_GUIDE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnFinishFunGuide();
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_RIDE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnRideSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_CANCEL_RIDE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnCancelRideSmartMounts();
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_SET_IMAGE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnSetImage(req->param1, req->param2 != 0);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE_EQUIP:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnActiveEquip(req->param1, req->param2);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_EQUIP_LEVEL_UP:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnEquipLevelUp(req->param1, req->param2);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_JEWELRY_STRENGTH:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnJewelryStrength(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_ACTIVE_HOLY:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnActiveHoly(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_HOLY_LEVEL_UP:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnHolyLevelUp(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_REVEAL:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnRevealSmartMounts(req->param1);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_CONVERT_ITEM:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnConvertItem(req->param1, req->param2);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_USE_ACTIVE_CONVERT_ITEM:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnActiveConvertItem(req->param1, req->param2);
		}
		break;
		case Protocol::CSSmartMountsSystemReq::SMART_MOUNTS_SYSTEM_OP_TYPE_SKILL_ITEM_COMPOSE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnSkillItemComposeItem(req->param1, req->param2, req->param3);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnSmartMountsSystemSkillReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SMART_MOUNTS_SYSTEM, true))
	{
		return;
	}

	Protocol::CSSmartMountsSystemSkillReq *req = (Protocol::CSSmartMountsSystemSkillReq *)msg;
	switch (req->op_type)
	{
		case Protocol::CSSmartMountsSystemSkillReq::SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_LEARN_SKILL:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnLearnSkillSmartMounts(req->param1, req->param2, req->param3, req->param4);
		}
		break;
		case Protocol::CSSmartMountsSystemSkillReq::SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_SKILL_UP:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnSkillUpSmartMounts(req->param1, req->param2, req->param3, req->param4);
		}
		break;
		case Protocol::CSSmartMountsSystemSkillReq::SMART_MOUNTS_SYSTEM_SKILL_OP_TYPE_SKILL_REPLACE:
		{
			role->GetRoleModuleManager()->GetSmartMountsSystem()->OnSkillReplaceSmartMounts(req->param1, req->param2, req->param3, req->param4);
		}
		break;
	}
}

void MessageHandler::OnBattleCountDownFinishReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	BattleManagerLocal::GetInstance().CountDownFinishReq(role);
}

void MessageHandler::OnWatchBattleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWatchBattleReq* req = (Protocol::CSWatchBattleReq*)msg;
	if (req->id_type == 0)
	{
		BattleManagerLocal::GetInstance().RoleOBReq(role, req->uid);
	}
	if (req->id_type == 1)
	{
		BattleManagerLocal::GetInstance().RoleOBSideReq(role, req->uid, req->watch_side);
	}
}

void MessageHandler::OnRoleCareerAdvanceReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSRoleCareerAdvanceReq* req = (Protocol::CSRoleCareerAdvanceReq*)msg;

	role->GetRoleModuleManager()->GetAdvanceCareer()->RoleAdvanceCareer(req->to_profession, req->to_color, false);
}

void MessageHandler::OnRoleCareerChangeReq(const GSNetID &netid, Scene* scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSRoleCareerChangeReq* req = (Protocol::CSRoleCareerChangeReq*)msg;
	role->RoleChangeCareer(req->to_profession,req->avartar_type,req->color);
}

void MessageHandler::OnRoleProfExtraInfoReq(const GSNetID &netid, Scene* scene, Role *role, ObjID obj_id, void *msg, int length)
{
	role->SendProfExtraInfo();
}

void MessageHandler::OnShiTuMakeRelationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHITU, true))
	{
		return;
	}

	Protocol::CSShiTuMakeRelationReq *stmrr = (Protocol::CSShiTuMakeRelationReq*)msg;

	int uid = stmrr->uid;

	role->GetRoleModuleManager()->GetShiTu()->OnMakeRelationReq(stmrr->type, uid);
}

void MessageHandler::OnShiTuMakeRelationAck(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHITU, true))
	{
		return;
	}

	Protocol::CSShiTuMakeRelationAck *stmra = (Protocol::CSShiTuMakeRelationAck*)msg;

	role->GetRoleModuleManager()->GetShiTu()->OnMakeRelationAck(stmra->type, stmra->uid, (0 != stmra->is_agree));
}

void MessageHandler::OnShiTuBreakRelationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHITU, true))
	{
		return;
	}

	Protocol::CSShiTuBreakRelationReq *stbrr = (Protocol::CSShiTuBreakRelationReq*)msg;

	switch (stbrr->req_type)
	{
		case SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK:
			{
				role->GetRoleModuleManager()->GetShiTu()->OnBreakRelationReq(stbrr->relation_type, stbrr->uid);
			}
			break;
		case SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK_FORCE:
			{
				role->GetRoleModuleManager()->GetShiTu()->OnBreakRelationForece(stbrr->uid, stbrr->param1);
			}
			break;
		case SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK_CANCEL:
			{
				role->GetRoleModuleManager()->GetShiTu()->OnCancelBreakRelation(stbrr->uid);
			}
			break;
	}


}

void MessageHandler::OnShiTuBreakRelationAck(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHITU, true))
	{
		return;
	}

	Protocol::CSShiTuBreakRelationAck * protocol = (Protocol::CSShiTuBreakRelationAck*)msg;

	role->GetRoleModuleManager()->GetShiTu()->OnBreakRelationAck(protocol->uid, 0 != protocol->is_agree);	
}

void MessageHandler::OnShiTuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHITU, true))
	{
		return;
	}

	Protocol::CSShiTuReq * req = (Protocol::CSShiTuReq*)msg;

	switch (req->req_type)
	{
		case Protocol::SHI_TU_REQ_TYPE_RELATION_INFO:
			{
				role->GetRoleModuleManager()->GetShiTu()->SendRelationInfo();
			}
			break;
		case Protocol::SHI_TU_REQ_TYPE_INVITATION:
			{
				role->GetRoleModuleManager()->GetShiTu()->InvitationShiTuFb();
			}
			break;
		case Protocol::SHI_TU_REQ_TYPE_FINISH_FB:
			{
				if (Scene::SCENE_TYPE_SHI_TU == scene->GetSceneType())		// 必须在场景里
				{
					SpecialLogicShiTuFb * sp_logic = (SpecialLogicShiTuFb *)scene->GetSpecialLogic();
					sp_logic->ReqFinish(role);
				}
			}
			break;
	}	
}

void MessageHandler::OnShiTuSeekingReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHITU, true))
	{
		return;
	}

	Protocol::CSShiTuSeekingReq * protocol = (Protocol::CSShiTuSeekingReq*)msg;

	switch (protocol->req_type)
	{
		case Protocol::CSShiTuSeekingReq::SHI_TU_SEEKING_REQ_TYPE_INFO:
			{
				ShiTuSeekingManager::Instance().SendShiTuSeekingNoticeList(protocol->seeking_type, role);
			}
			break;
		case Protocol::CSShiTuSeekingReq::SHI_TU_SEEKING_REQ_TYPE_INSERT:
			{
				if (CrossConfig::Instance().IsHiddenServer())
				{
					role->NoticeNum(errornum::EN_TRADE_MARKET_CROSS_CANNOT_OPERATE);
					return;
				}

				protocol->seeking_notice[sizeof(protocol->seeking_notice) - 1] = 0;
				ShiTuSeekingManager::Instance().OnUserChangeSeeking(role, protocol->seeking_type, protocol->seeking_notice);
			}
			break;
		case Protocol::CSShiTuSeekingReq::SHI_TU_SEEKING_REQ_TYPE_REMOVE:
			{
				if (CrossConfig::Instance().IsHiddenServer())
				{
					role->NoticeNum(errornum::EN_TRADE_MARKET_CROSS_CANNOT_OPERATE);
					return;
				}

				ShiTuSeekingManager::Instance().RemoveSeeking(role, true);
			}
			break;
		case Protocol::CSShiTuSeekingReq::SHI_TU_SEEKING_REQ_TYPE_PREFERENCES:
			{
				ShiTuSeekingManager::Instance().SetPreferences(role, protocol->preferences);
			}
			break;
	}	
}

void MessageHandler::OnFirstKillReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSServerFirstKillReq* req = (Protocol::CSServerFirstKillReq*)msg;
	bool ret = ServerFirstKill::Instance().SendFirstKillInfo(role, req->type, req->level_group);
	if (!ret)
	{
		gamelog::g_log_first_kill.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnBattleFightMonsterReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleFightMonsterReq* req = (Protocol::CSBattleFightMonsterReq*)msg;
	BattleManagerLocal::GetInstance().StartMonsterFightReq(role, req->monster_group_id, BATTLE_MODE_NORMAL_MONSTER, 0);
}

void MessageHandler::OnRoleCreateAttributePlanReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSAttributePlanCreate* req = (Protocol::CSAttributePlanCreate*)msg;
	req->plan_name[ATTRIBUTE_PLAN_NAME_LENGTH - 1] = 0;
	role->GetRoleModuleManager()->GetRoleAttributePlan()->CreateAttributePlanReq(req->plan_name);
}

void MessageHandler::OnRoleChangeAttributePlanNameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSAttributePlanChangeName* req = (Protocol::CSAttributePlanChangeName*)msg;
	req->plan_name[ATTRIBUTE_PLAN_NAME_LENGTH - 1] = 0;
	role->GetRoleModuleManager()->GetRoleAttributePlan()->ChangeAttributePlanNameReq(req->plan_index, req->plan_name);
}

void MessageHandler::OnRoleSwitchAttributePlanReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSAttributePlanSwitchPlan* req = (Protocol::CSAttributePlanSwitchPlan*)msg;
	role->GetRoleModuleManager()->GetRoleAttributePlan()->SwitchAttributePlanReq(static_cast<short>(req->switch_to_index));
}

void MessageHandler::OnRoleAddAttributePointReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSAttributePlanAddPoint* req = (Protocol::CSAttributePlanAddPoint*)msg;
	role->GetRoleModuleManager()->GetRoleAttributePlan()->AddAttributePointReq(req->plan_index, req->delta_attr_list[ADD_POINT_TYPE_VITALITY],
		req->delta_attr_list[ADD_POINT_TYPE_STRENGTH], req->delta_attr_list[ADD_POINT_TYPE_POWER],
		req->delta_attr_list[ADD_POINT_TYPE_AGILITY], req->delta_attr_list[ADD_POINT_TYPE_MAGIC]);
}

void MessageHandler::OnRoleClearAttributePointReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSAttributePlanClearPoint* req = (Protocol::CSAttributePlanClearPoint*)msg;
	role->GetRoleModuleManager()->GetRoleAttributePlan()->ClearAttributePointReq(static_cast<short>(req->plan_index));
}

void MessageHandler::OnRoleClientSystemSetReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	static Protocol::CSRoleSystemSetReq systemset;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSRoleSystemSetReq, system_set_list));
	if (length < offset)return;
	
	const char *pt = (const char*)msg;

	memcpy((void*)&systemset, pt, offset);
	pt += offset; length -= offset;

	static const int TYPE_MAX = ARRAY_ITEM_COUNT(systemset.system_set_list);

	if (systemset.count > TYPE_MAX) return;
	int actual_count = 0;
	const int SET_LIST_SIZE = sizeof(systemset.system_set_list[0]);
	for (int i = 0; i < systemset.count && length >= SET_LIST_SIZE; i++)
	{
		memcpy(&systemset.system_set_list[i], pt, SET_LIST_SIZE);
		pt += SET_LIST_SIZE;
		length -= SET_LIST_SIZE;
		actual_count += 1;
	}
	systemset.count = actual_count;

	for (int i = 0;i < systemset.count ; ++i)
	{
		role->GetRoleModuleManager()->GetRoleSetting()->SetRoleSetting(systemset.system_set_list[i].system_set_type, systemset.system_set_list[i].system_set_param);
	}

	//role->NoticeNum(noticenum::NT_SYSTEM_SET_SUCC);
}


void MessageHandler::OnSendMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	static Protocol::CSMailSend stMailSend;
	stMailSend.ResetMailContentTxt();

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSMailSend, contenttxt));
	if (length < offset) return;

	const char *pt = (const char *)msg;

	memcpy((void*)&stMailSend, pt, offset);
	pt += offset; length -= offset;

	if (length < 0 || length >= MAX_CONTENT_TXT_LENTH || length != stMailSend.contenttxt_len) return;

	memcpy(&stMailSend.contenttxt[0], pt, stMailSend.contenttxt_len);
	stMailSend.contenttxt[sizeof(stMailSend.contenttxt) - 1] = '\0';

	UserID m_id = role->GetUserId();
	Role *target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(stMailSend.recver_uid));
	if (NULL != target_role)
	{
		if (FriendManager::Instance().IsBlack(target_role->GetUID(), UidToInt(m_id)))
		{
			role->NoticeNum(errornum::EN_YOU_ARE_IN_BLACKLISTS);
			return;
		}
	}

	if (FriendManager::Instance().IsBlack(role->GetUID(), stMailSend.recver_uid))
	{
		role->NoticeNum(errornum::EN_IN_YOUR_BLACKLISTS);
		return;
	}

	MailRoute::Instance().SendMail(role, &stMailSend);
}

void MessageHandler::OnDeleteMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSMailDelete *md = (Protocol::CSMailDelete*)msg;
	MailRoute::Instance().RemoveMail(role, md->mail_index);
}

void MessageHandler::OnLockMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	// 	Protocol::CSMailLock *ml = (Protocol::CSMailLock*)msg;
	// 	MailRoute::Instance().LockMail(role, ml->mail_index);
}

void MessageHandler::OnUnlockMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	// 	Protocol::CSMailLock *ml = (Protocol::CSMailLock*)msg;
	// 	MailRoute::Instance().UnlockMail(role, ml->mail_index);
}

void MessageHandler::OnGetMailList(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	MailRoute::Instance().GetMailList(role);
}

void MessageHandler::OnReadMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSMailRead *mr = (Protocol::CSMailRead*)msg;
	
	MailRoute::Instance().ReadMail(role, mr->mail_index);
}

void MessageHandler::OnFetchMailAttachment(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSMailFetchAttachment *mgi = (Protocol::CSMailFetchAttachment*)msg;
	
	MailRoute::Instance().FetchMailAttachment(role, mgi);
}

void MessageHandler::OnCleanMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	MailRoute::Instance().CleanMail(role);
}

void MessageHandler::OnOneKeyFetchMailAttachment(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	MailRoute::Instance().OneKeyFetchAttachment(role);
}

void MessageHandler::OnGetGuildBossReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Guild* guild = role->GetGuild();
	if (NULL == guild) return;

	Protocol::CSGuildBossReq * req = (Protocol::CSGuildBossReq *)msg;

	switch (req->req_type)
	{
		case Protocol::GUILD_BOSS_REQ_TYPE_INFO:		//信息
			{
				guild->SendGuildBossInfo(role);
			}
			break;
		case Protocol::GUILD_BOSS_REQ_TYPE_RANK:		//排行榜
			{
				guild->SendGuildBossRank(role);
			}
			break;
		case Protocol::GUILD_BOSS_REQ_TYPE_FETCH_CHAPTER_REWARD:
			{
			}
			break;
		//屏蔽家族间的排行
		//case Protocol::GUILD_BOSS_REQ_TYPE_GUILD_RANK:
		//	{
		//		GuildManager::Instance().SendGuildBossChapterRankList(role);
		//	}
		//	break;
	}	
}

void MessageHandler::OnGuildLevelUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	int ret = GuildManager::Instance().OnLevelUp(role);
	if (ret < 0)
	{
		gamelog::g_log_guild.printf(LL_WARNING, "%s Error user[%d %s] %d \n", __FUNCTION__, role->GetUID(), role->GetName(), ret);
	}

}

void MessageHandler::OnGuildAnswerAnswerQuestion(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActivityGuildAnswerAnswerQuest * req = (Protocol::CSActivityGuildAnswerAnswerQuest *)msg;

	if (Scene::SCENE_TYPE_GUILD_ANSWER == scene->GetSceneType())		// 必须在场景里
	{
		SpecialLogicGuildAnswer * sp_logic = (SpecialLogicGuildAnswer *)scene->GetSpecialLogic();
		if (NULL != sp_logic) sp_logic->CommitAnswer(role, req->answer);
	}
}

void MessageHandler::OnGuildRename(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildRenameReq * req = (Protocol::CSGuildRenameReq *)msg;
	GuildManager::Instance().GuildRename(role, req->new_name);
}

void MessageHandler::OnRoleBattleModeFightReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleModeFightReq* req = (Protocol::CSBattleModeFightReq*)msg;
	BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, req->battle_mode, req->param1, req->param2, req->param3, false);
}

void MessageHandler::OnRoleChallengeOthersReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSChallengeOthersReq* req = (Protocol::CSChallengeOthersReq*)msg;
	BattleManagerLocal::GetInstance().StartChallengeOthersReq(role, req->role_id, req->battle_mode);
}

void MessageHandler::OnRoleQieCuoWithOtherReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBattleQieCuoWithOthers* req = (Protocol::CSBattleQieCuoWithOthers*)msg;
	BattleManagerLocal::GetInstance().SendQieCuoReqToTarget(role, req->role_id);
}

void MessageHandler::OnRoleQieCuoRequestReply(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBattleQieCuoMessageRep* req = (Protocol::CSBattleQieCuoMessageRep*)msg;
	if (!BattleManagerLocal::GetInstance().OnQieCuoRequestReply(role, req->reply_role_id, 0 != req->rep_result))
	{
		BattleManagerLocal::GetInstance().SendQieCuoDequeFront(role);
	}
}

void MessageHandler::OnFetchSystemNoticeAward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	//Protocol::CSSystemNoticeGetAward * snga = (Protocol::CSSystemNoticeGetAward *)msg;
	//role->GetRoleModuleManager()->GetSystemNotice()->GetAward(snga->type_seq);
}

void MessageHandler::OnClaimFuncPreviewReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSFuncPreviewClaimReward* req = (Protocol::CSFuncPreviewClaimReward*)msg;
	role->GetRoleModuleManager()->GetFuncPreview()->ClaimReward(req->notice_id);
}

void MessageHandler::OnRealmBreakReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_REALM_BREAK, true)) return;

	UNSTD_STATIC_CHECK(Protocol::CSRealmBreakReq::REAL_BREAK_OP_TYPE_MAX == 4);
	Protocol::CSRealmBreakReq* req = (Protocol::CSRealmBreakReq *)msg;
	switch (req->notice_type)
	{
		case Protocol::CSRealmBreakReq::REALM_BREAK_OP_TYPE_BREAK:
		{
			role->GetRoleModuleManager()->GetRealmBreak()->OnBreak();
		}
		break;
		case Protocol::CSRealmBreakReq::REALM_BREAK_OP_TYPE_TUNA:
		{
			//role->GetRoleModuleManager()->GetRealmBreak()->OnStartTuna();
		}
		break;
		case Protocol::CSRealmBreakReq::REALM_BREAK_OP_TYPE_FETCH_EXP:
		{
			role->GetRoleModuleManager()->GetRealmBreak()->OnFetchExp();
		}
		break;
		case Protocol::CSRealmBreakReq::REALM_BREAK_OP_TYPE_FIGHT:
		{
			role->GetRoleModuleManager()->GetRealmBreak()->OnSoulFight(req->reserve_sh);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnMaterialReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMaterialReq * req = (Protocol::CSMaterialReq *)msg;
	UNSTD_STATIC_CHECK(4 == Protocol::CSMaterialReq::MATERIAL_OP_TYPE_MAX);
	switch (req->op_type)
	{
		case Protocol::CSMaterialReq::MATERIAL_OP_TYPE_CHALLENGE:
		{
			FBManager::Instance().EnterFB(role, FBManager::FB_TYPE_MATERIAL_SCENE, req->param1, 0);
		}
		break;
		case Protocol::CSMaterialReq::MATERIAL_OP_TYPE_FAST_PASS:
		{
			role->GetRoleModuleManager()->GetRoleMaterial()->OnFastPassFb(req->param1);
		}
		break;
		case Protocol::CSMaterialReq::MATERIAL_OP_TYPE_FETCH_BOX:
		{
			role->GetRoleModuleManager()->GetRoleMaterial()->OnFetchBoxReward(req->param1);
		}
		break;
		case Protocol::CSMaterialReq::MATERIAL_OP_TYPE_BUY_TIMES:
		{
			role->GetRoleModuleManager()->GetRoleMaterial()->OnBuyTimes(req->param1);
		}
		break;
	default:
		break;
	}
	
}

void MessageHandler::OnNPCHandleRequest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSNPCHandleRequest * npchr = (Protocol::CSNPCHandleRequest *)msg;
	
	switch (npchr->op_type)
	{
	case Protocol::CSNPCHandleRequest::NPC_REQUEST_STARTS_BATTLE:
		NpcManager::Instance().OnNPCStartsBattle(role, npchr->npc_seq, npchr->param1, npchr->param2);
		break;

	case Protocol::CSNPCHandleRequest::NPC_REQUEST_PURCHASE_ITEM:
		NpcManager::Instance().OnNPCPurchaseItem(role, npchr->npc_seq, npchr->param1, npchr->param2);
		break;

	case Protocol::CSNPCHandleRequest::NPC_REQUEST_STARTS_GATHER:
		role->OnStartsGather(npchr->npc_seq,npchr->param2);
		break;
	case Protocol::CSNPCHandleRequest::NPC_REQUEST_END_GATHER:
		role->OnEndGather(npchr->npc_seq,npchr->param1,npchr->param2);
		break;
	case Protocol::CSNPCHandleRequest::NPC_REQUEST_CHOSED:
		NpcManager::Instance().OnNPCCommitChosed(role, npchr->npc_objid, npchr->param1, npchr->param2);
		break;
	case Protocol::CSNPCHandleRequest::NPC_REQUEST_START_BATTLE:
		BattleManagerLocal::GetInstance().StartMonsterFightReq(role, npchr->param1, BATTLE_MODE_NORMAL_MONSTER, 0);
		break;
	}
}

void MessageHandler::OnSetEncounterMineMonsterFlag(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSetEncounterMineMonsterFlag* req = (Protocol::CSSetEncounterMineMonsterFlag*)msg;
	if (scene->GetSpecialLogic()->IsForceEncounterMineMonster()) return; // 强制遇暗怪，则不允许玩家自行调节

	role->SetEncounterMonsterFlag(req->flag > 0);
}

void MessageHandler::OnRolePatrolReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRolePatrol* req = (Protocol::CSRolePatrol*)msg;

	role->SetPatrolFlag(req->is_start_or_stop > 0);

	//gamelog::g_log_debug.printf(LL_INFO, "OnRolePatrolReq role[%d,%s] flag[%d]", role->GetUID(), role->GetName(), req->is_start_or_stop);
}

void MessageHandler::OnTeamLeaderNpcReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTeamLeaderNpcReq* req = (Protocol::CSTeamLeaderNpcReq*)msg;
	size_t offset = UNSTD_OFFSETOFF(Protocol::CSTeamLeaderNpcReq, npc_seq_list);
	if (length < (int)offset || req->npc_count <= 0 || length != (int)(offset + sizeof(req->npc_seq_list[0]) * req->npc_count))
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}

	Team* team = role->GetMyTeam();
	if (team != NULL && team->IsLeader(role))
	{
		if (req->type == Protocol::TEAM_LEADER_NPC_REQ_TYPE_LOAD)
		{
			team->OnTeamLeaderLoadNPC((const char*)msg);	
		}
		else if (req->type == Protocol::TEAM_LEADER_NPC_REQ_TYPE_ADD)
		{
			team->OnTeamLeaderAddNPC((const char*)msg);
		}
		else if (req->type == Protocol::TEAM_LEADER_NPC_REQ_TYPE_DELETE)
		{
			team->OnTeamLeaderDeleteNPC((const char*)msg);
		}
	}
}

void MessageHandler::OnNpcShopBuyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSNPCShopBuyReq * req_info = (Protocol::CSNPCShopBuyReq*)msg;
	Obj * npc_obj = scene->GetObj(req_info->obj_id);
	if (npc_obj != NULL)
	{
		if (Obj::OBJ_TYPE_NPC_CHOSED == npc_obj->GetObjType())
		{
			NPCChosed *npc_chosed = (NPCChosed*)npc_obj;
			npc_chosed->OnBuyItem(role, req_info->buy_index,req_info->buy_num);
		}
	}
}

void MessageHandler::OnSetBattlePosition(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSetBattlePosition* req = (Protocol::CSSetBattlePosition*)msg;
	role->RoleSetBattlePosition(req->position > 0 ? 1 : 0);
}

void MessageHandler::OnCourageChallengeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCourageChallengeOp * req = (Protocol::CSCourageChallengeOp*)msg;

	switch (req->op_type)
	{
	case Protocol::CSCourageChallengeOp::OP_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetCourageChallenge()->SendAllInfo();
		}
		break;
	case Protocol::CSCourageChallengeOp::OP_TYPE_FETCH:
		{
			role->GetRoleModuleManager()->GetCourageChallenge()->FetchEveryDayReward(req->param1, req->param2);
		}
		break;
	case Protocol::CSCourageChallengeOp::OP_TYPE_FETCH_MAX_REWARD:
		{
			role->GetRoleModuleManager()->GetCourageChallenge()->FetchMaxScoreReward(req->param1);
		}
		break;
	case Protocol::CSCourageChallengeOp::OP_TYPE_OPEN_UI:
		{
			role->GetRoleModuleManager()->GetCourageChallenge()->OnRoleOpenUI();
		}
		break;
	}
}

void MessageHandler::OnCourageChallengeRankListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCourageChallengeRankListReq * req = (Protocol::CSCourageChallengeRankListReq *)msg;

	CourageChallengeRankManager::Instance().SendRankListInfo(role, req->rank_type, req->count, req->role_id);
}

void MessageHandler::OnSetBattleAuto(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSetBattleAuto* req = (Protocol::CSSetBattleAuto*)msg;
	role->SetBattleAuto(req->is_auto);
}

bool MoveIsSkill(int move_type, int param1)
{
	if (BATTLE_MOVE_TYPE_PERFORM_SKILL == move_type ||
		(BATTLE_MOVE_TYPE_AI_SKILL == move_type && param1 != BATTLE_NORMAL_ATTACK_SKILL_ID && param1 != BATTLE_NORMAL_DEFEND_SKILL_ID)
		)
	{
		return true;
	}

	return false;
}

bool IsNormalAutoMove(int move_type)
{
	return BATTLE_MOVE_TYPE_PERFORM_SKILL == move_type
		|| BATTLE_MOVE_TYPE_NORMAL_ATTACK == move_type
		|| BATTLE_MOVE_TYPE_NORMAL_DEFENCE == move_type
		|| BATTLE_MOVE_TYPE_AI_SKILL == move_type;
}

void MessageHandler::OnSetBattleAutoMove(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSetBattleAutoMove* req = (Protocol::CSSetBattleAutoMove*)msg;

	if (!IsNormalAutoMove(req->first_move_type) || !IsNormalAutoMove(req->second_move_type))
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	bool first_move_is_skill = MoveIsSkill(req->first_move_type, req->first_skill_id);
	bool second_move_is_skill = MoveIsSkill(req->second_move_type, req->second_skill_id);
	if (first_move_is_skill && second_move_is_skill)
	{
		int fight_index = role->GetRoleModuleManager()->GetPet()->GetFightIndex();
		if (-1 == fight_index)
		{
			//两动不能都为技能
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		else if (BATTLE_MOVE_TYPE_AI_SKILL == req->second_move_type)
		{
			// 宠物没有智能施法
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	role->SetBattleAutoMove(true, req->first_move_type, req->first_skill_id, req->first_skill_level, true, false, true);
	role->SetBattleAutoMove(false, req->second_move_type, req->second_skill_id, req->second_skill_level, true, true, true);
}

void MessageHandler::OnChangeAppearacne(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSChangeAppearance *req = (Protocol::CSChangeAppearance *)msg;
	role->ChangeAppearance(req->avartar_type, req->color);
}

void MessageHandler::OnChangeHeadshot(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSChangeHeadshot* req = (Protocol::CSChangeHeadshot*)msg;
	role->ChangeHeadshot(req->headshot_id);
}

void MessageHandler::OnAttrListReasonReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSAttrListReasonReq * req = (Protocol::CSAttrListReasonReq *)msg;
	switch (req->reason_type)
	{
	case Protocol::SCAttrListReason::REASON_TYPE_ROLE:
	{
		role->GetRoleModuleManager()->SendAttrListReason(req->param1);
	}
	break;
	case Protocol::SCAttrListReason::REASON_TYPE_PET:
	{
		role->GetRoleModuleManager()->GetPet()->SendPetAttrListReason(req->param1);
	}
	break;
	case Protocol::SCAttrListReason::REASON_TYPE_PARTNER:
	{
		role->GetRoleModuleManager()->GetPartner()->SendPartnerAttrListReason(req->param1);
	}
	break;
	case Protocol::SCAttrListReason::REASON_TYPE_HUASHEN_ROLE:
	{
		role->GetRoleModuleManager()->GetHuaShenManager()->SendRoleAttrReasonList(req->param1);
	}
	break;
	case Protocol::SCAttrListReason::REASON_TYPE_HUASHEN_PET:
	{
		role->GetRoleModuleManager()->GetHuaShenManager()->SendPetAttrReasonList(req->param1);
	}
	break;
	default:
		break;
	}

}

void MessageHandler::OnResetNameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSRoleResetName *rrn = (Protocol::CSRoleResetName *)msg;

	if (0 == rrn->new_name[0]) return;

	rrn->new_name[sizeof(rrn->new_name) - 1] = 0;

	static const bool NAMEFILTER_OPEN_FLAG = ServerConfigPool::Instance().common_config.namefilter_open_flag;
	if (NAMEFILTER_OPEN_FLAG && NameFilter::Instance().HasSensitiveWord(sizeof(GameName), rrn->new_name)) return;

	GameName role_name = { 0 };
	role->GetName(role_name);
	
	role->OnResetNameReq(rrn->new_name);
}

void MessageHandler::OnChatRecordReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSChatRecordSet * req = (Protocol::CSChatRecordSet *)msg;

	ChatManager::Instance().SetChatRecrodList(role, req->recent_chat_list);
}

void MessageHandler::OnEnterFBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSEnterFB* req = (Protocol::CSEnterFB*)msg;
	if (!role->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		role->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return;
	}
	FBManager::Instance().EnterFB(role, req->fb_type, req->param_1, req->param_2);
}

void MessageHandler::OnLeaveFBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	//Protocol::CSLeaveFB* req = (Protocol::CSLeaveFB*)msg;

	FBManager::Instance().LeaveFB(role);
}

void MessageHandler::OnActivityEnterReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActivityEnterReq *aer = (Protocol::CSActivityEnterReq*)msg;
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnActivityEnterReq(aer);
}

void MessageHandler::OnActivityTeamFightReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActivityTeamFightMatchReq *req = (Protocol::CSActivityTeamFightMatchReq*)msg;
	ActivityTeamFight* act = static_cast<ActivityTeamFight*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT));
	if (NULL != act)
	{
		if (!act->IsActivityOpen())
		{
			role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
			return;
		}

		switch (req->is_match)
		{
		case Protocol::CSActivityTeamFightMatchReq::TYPE_MATCH:
			act->OnMatchReq(role);
			break;
		case Protocol::CSActivityTeamFightMatchReq::TYPE_CANCEL:
			act->OnCancelReq(role);
			break;
		case Protocol::CSActivityTeamFightMatchReq::TYPE_ROLE_INFO:
			act->SendRoleInfo(role);
			break;
		case Protocol::CSActivityTeamFightMatchReq::TYPE_RANK_INFO:
			act->SendRankInfo(role);
			break;
		case Protocol::CSActivityTeamFightMatchReq::TYPE_RECORD_INFO:
			act->SendRecordInfo(role, false);
			break;
		case Protocol::CSActivityTeamFightMatchReq::TYPE_RECORD_ONLY_INFO:
			act->SendRecordInfo(role, true);
			break;
		case Protocol::CSActivityTeamFightMatchReq::TYPE_FETCH_BOX:
			act->OnFetchBoxReward(role, req->param1);
			break;
		}
	}
}

void MessageHandler::OnActivityMazeRacerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActivityMazeRacerHandleReq * maze_racer_req = (Protocol::CSActivityMazeRacerHandleReq *)msg;

	switch (maze_racer_req->op_type)
	{
	case Protocol::CSActivityMazeRacerHandleReq::MAZE_RACER_REQ_TRANSP:
		{
			SpecialLogic * sp_l = role->GetScene()->GetSpecialLogic();
			if (sp_l == NULL || sp_l->GetSceneType() != Scene::SCENE_TYPE_MAZE_RACER)return;
			SpecialLogicMazeRacer * special_mr = (SpecialLogicMazeRacer *)sp_l;
		special_mr->RequestTransport(role);
		}		
		break;
	case Protocol::CSActivityMazeRacerHandleReq::MAZE_RACER_USE_AMULET:
		{
			SpecialLogic * sp_l = role->GetScene()->GetSpecialLogic();
			if (sp_l == NULL || sp_l->GetSceneType() != Scene::SCENE_TYPE_MAZE_RACER)return;
			SpecialLogicMazeRacer * special_mr = (SpecialLogicMazeRacer *)sp_l;
		special_mr->RequestUseAmulet(role, maze_racer_req->param1);
		}		
		break;
	case Protocol::CSActivityMazeRacerHandleReq::MAZE_RACER_ROLE_INFO:
		{
			MazeRacerManager().Instance().OnSendRoleInfo(role);
		}		
		break;
	}
}

void MessageHandler::OnActivityGuildFightReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
	SpecialLogicGuildFight * sp_gf = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildFight(gf_other.scene_id, COMMON_FB_KEY, false);
	if (NULL == sp_gf) return;

	Protocol::CSActivityGuildFightOperatorReq * gf_req = (Protocol::CSActivityGuildFightOperatorReq *)msg;

	switch (gf_req->op_type)
	{
	case Protocol::CSActivityGuildFightOperatorReq::GF_OP_TYPE_REQ_ALL_RANK_INFOR:
		sp_gf->RequestGuildFightRankInfo(role);
		break;
	case Protocol::CSActivityGuildFightOperatorReq::GE_OP_TYPE_REQ_CLOSE_RANK:
		sp_gf->OnCloseRank(role);
		break;
	}
}

void MessageHandler::OnActivityOnlyFightReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	SpecialLogic * special_lobgic = scene->GetSpecialLogic();
	if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_ONLY_FIGHT)
	{
		return;
	}
	SpecialLogicOnlyFight * only_fight_scene = (SpecialLogicOnlyFight *)special_lobgic;
	if (NULL == only_fight_scene) return;

	Protocol::CSActivityOnlyFightOperatorReq * gf_req = (Protocol::CSActivityOnlyFightOperatorReq *)msg;

	switch (gf_req->op_type)
	{
	case Protocol::CSActivityOnlyFightOperatorReq::OF_OP_TYPE_REQ_ALL_RANK_INFO:
		only_fight_scene->RequestOnlyFightRankInfo(role);
		break;
	case Protocol::CSActivityOnlyFightOperatorReq::OF_OP_TYPE_REQ_CLOSE_RANK:
		only_fight_scene->OnCloseRank(role);
		break;
	}
}

void MessageHandler::OnActivitySilkRoadsReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
		return;
	}
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SILK_ROADS, true))
	{
		return;
	}

	ActivitySilkRoads * activity = (ActivitySilkRoads *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_SILK_ROADS);
	if (NULL == activity || !activity->IsActivityOpen())
	{
		return;
	}
	Protocol::CSActivitySilkRoadsReq *req = (Protocol::CSActivitySilkRoadsReq *)msg;
	switch (req->op_type)
	{
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_ROLE_INFO:
		{
			activity->OnSendRoleAllInfo(role);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_READY:
		{
			activity->OnReady(role);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_START_OUTSET:
		{
			activity->OnStartOutSet(role);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_KEEP_GOING:
		{
			activity->OnKeepGoing(role);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_BUY_COMMODITY:
		{
			activity->OnBuyCommodity(role, req->param1, req->param2);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_SALE_COMMODITY:
		{
			activity->OnSaleCommodity(role, req->param1, req->param2);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_BUY_NEWS:
		{
			activity->OnBuyNews(role);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_EXPANSION_WAREHOUSE:
		{
			activity->OnExpansionWarehouse(role, req->param1);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_BUY_SUPPLY:
		{
			activity->OnBuySupply(role, req->param1);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_END:
		{
			activity->OnEnd(role);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_RANK_ALL_INFO:
		{
			activity->OnSendAllRankInfo(role);
		}
		break;
		case Protocol::CSActivitySilkRoadsReq::SILK_ROADS_REQ_OP_TYPE_ALL_TRADING_INFO:
		{
			activity->OnSendAllTradingLogInfo(role);
		}
		break;
		default:
		break;
	}

}

void MessageHandler::OnActivitySignUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActivitySignUpReq * req = (Protocol::CSActivitySignUpReq *)msg;

	role->GetRoleModuleManager()->GetRoleActivityManager()->OnActivitySignUpReq(req->seq);
}

void MessageHandler::OnActivityAdvanceFetchReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActivityAdvanceFetchReq * req = (Protocol::CSActivityAdvanceFetchReq *)msg;
	if (NULL == req || req->op_type < 0 || req->op_type >= Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_MAX) return;

	SpecialLogic * special_lobgic = scene->GetSpecialLogic();
	if(NULL == special_lobgic) return;

	switch (req->activity_type)
	{
	case ACTIVITY_TYPE_TEAM_FIGHT:
	{
		if (special_lobgic->GetSceneType() != Scene::SCENE_TYPE_TEAM_FIGHT) return;

		SpecialLogicTeamFight * team_fight_scene = (SpecialLogicTeamFight *)special_lobgic;
		if (NULL == team_fight_scene) return;
	
		ActivityTeamFight * activity = (ActivityTeamFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
		if (NULL == activity) return;

		activity->OnAdavanceFetch(role, req->op_type, req->param1);
	}
	break;
	case ACTIVITY_TYPE_GUILD_FIGHT:
	{
		if (special_lobgic->GetSceneType() != Scene::SCENE_TYPE_GUILD_FIGHT) return;

		SpecialLogicGuildFight * guild_fight_scene = (SpecialLogicGuildFight *)special_lobgic;
		if (NULL == guild_fight_scene) return;

		guild_fight_scene->OnAdavanceFetch(role, req->op_type, req->param1);
	}
	break;
	case ACTIVITY_TYPE_CHIEF_ELECTION:
	{
		if (special_lobgic->GetSceneType() != Scene::SCENE_TYPE_CHIEF_ELECTION_SEA) return;

		SpecialLogicChiefElectionSea * chief_election_sea_scene = (SpecialLogicChiefElectionSea *)special_lobgic;
		if (NULL == chief_election_sea_scene) return;
		
		chief_election_sea_scene->OnAdavanceFetch(role, req->op_type, req->param1);
	}
	break;
	case ACTIVITY_TYPE_ONLY_FIGHT:
	{
		if(special_lobgic->GetSceneType() != Scene::SCENE_TYPE_ONLY_FIGHT) return;

		SpecialLogicOnlyFight * only_fight_scene = (SpecialLogicOnlyFight *)special_lobgic;
		if (NULL == only_fight_scene) return;

		only_fight_scene->OnAdavanceFetch(role, req->op_type, req->param1);
	}
	break;
	case ACTIVITY_TYPE_WORLD_TEAM_ARENA:
	{
		if (special_lobgic->GetSceneType() != Scene::SCENE_TYPE_WORLD_TEAM_ARENA) return;

		SpecialLogicWorldTeamArena * world_team_arena_scene = (SpecialLogicWorldTeamArena *)special_lobgic;
		if (NULL == world_team_arena_scene) return;

		world_team_arena_scene->OnAdavanceFetch(role, req->op_type, req->param1);
	}
	break;
	default:
		break;
	}

}

void MessageHandler::OnCmdBuyItemCheckReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCmdBuyItemCheckReq * req = (Protocol::CSCmdBuyItemCheckReq *)msg;
	switch (req->buy_type)
	{
	case BUY_TIME_TYPE_YAO_XING_SHI_LIAN:
	{
			role->GetRoleModuleManager()->GetYaoXingShiLian()->OnBuyItemCfgCheck(req->param1, req->param2);
	}
	break;
	case BUY_TIME_TYPE_MYSTERIOUS_TRIAL_NEW:		//param1:type param2:day
	{
		const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();
		if (0 != other_cfg.mysterious_cfg_ver && req->cfg_ver != other_cfg.mysterious_cfg_ver)
		{
			Protocol::SCCmdBuyItemCheckRet protocol;
			protocol.unique_id = 0;
			protocol.cfg_ver = other_cfg.mysterious_cfg_ver;
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

			role->NoticeNum(errornum::EN_NEED_UPDATE_CFG_VER);
			return;
		}
		role->GetRoleModuleManager()->GetMysteriousTrial()->OnBuyItemCfgCheck(req->param1, req->param2);
	}
	break;
	case BUY_TIME_TYPE_MONTH_PURCHASE_NEW:
	{
		const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();
		if (0 != other_cfg.month_investment_cfg_ver && req->cfg_ver != other_cfg.month_investment_cfg_ver)
		{
			Protocol::SCCmdBuyItemCheckRet protocol;
			protocol.unique_id = 0;
			protocol.cfg_ver = other_cfg.month_investment_cfg_ver;
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

			role->NoticeNum(errornum::EN_NEED_UPDATE_CFG_VER);
			return;
		}
		role->GetRoleModuleManager()->GetRoleActivityManager()->OnCmdBuyMonthInvestmentCheck();
	}
	break;
	case BUY_TIME_TYPE_NEW_COURAGE_CHALLENGE:
		{
			role->GetRoleModuleManager()->GetNewCourageChallenge()->OnBuyItemCheck();
		}
		break;
	default:
		break;
	}

}

void MessageHandler::OnChannelChat(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	ChatManager::Instance().OnChannelChat(role, msg, length);
}

void MessageHandler::OnSingleChat(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	ChatManager::Instance().OnSingleChat(role, msg, length);
}

void MessageHandler::OnChatItemInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSChatItemInfoReq * req = (Protocol::CSChatItemInfoReq *)msg;
	ChatManager::Instance().SendChatItemInfo(role, req->item_info_seq);
}

void MessageHandler::OnSchoolTaskReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SCHOOL_TASK))
	{
		return;
	}
	
	Protocol::CSSchoolTaskReq* req = (Protocol::CSSchoolTaskReq*)msg;
	switch (req->ret_type)
	{
	case Protocol::CSSchoolTaskReq::SCHOOL_TASK_TYPE_GET:
		role->GetRoleModuleManager()->GetSchoolTask()->OnTaskReq(req->param_1);
		break;
	case Protocol::CSSchoolTaskReq::SCHOOL_TASK_TYPE_QUICK:
		role->GetRoleModuleManager()->GetSchoolTask()->OnQuickFinish(req->param_1);
		break;
	}
}

void MessageHandler::OnResearchTaskRequest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ZHENZHONG_RESERCH, true))
	{
		return;
	}

	Protocol::CSResearchTaskRequest * rtr = (Protocol::CSResearchTaskRequest *)msg;

	switch (rtr->op_type)
	{
	case Protocol::CSResearchTaskRequest::RESEARCH_TASK_ACCEPT:
		role->GetRoleModuleManager()->GetResearchTask()->OnAcceptResearchTask();
		break;

	case Protocol::CSResearchTaskRequest::RESEARCH_TASK_COMMIT:
		role->GetRoleModuleManager()->GetResearchTask()->OnCommitResearchTask();
		break;

	default:
		break;
	}
}

void MessageHandler::OnEscortOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ESCORT, true))
	{
		return;
	}

	Protocol::CSEscortOp * req = (Protocol::CSEscortOp*)msg;
	switch (req->op_type)
	{
	case Protocol::CSEscortOp::ACCEPT_TASK:
		role->GetRoleModuleManager()->GetEscort()->AcceptTask(req->param1, req->param2);
		break;
	case Protocol::CSEscortOp::COMMIT_TASK:
		role->GetRoleModuleManager()->GetEscort()->CommitTask();
		break;
	case Protocol::CSEscortOp::GIVEUP_TASK:
		role->GetRoleModuleManager()->GetEscort()->GiveupTask();
		break;
	}
}

void MessageHandler::OnWorldBossReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWorldBossReq * req = (Protocol::CSWorldBossReq*)msg;

	if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_WORLD_BOSS))
	{
		return;
	}

	ActivityWorldBoss* act = static_cast<ActivityWorldBoss*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS));
	if (NULL == act)
	{
		return;
	}

	switch (req->req_type)
	{
		case Protocol::WORLD_BOSS_REQ_TYPE_REQ_INFO:
			{
				act->SendBossInfo(role);
			}
			break;
		case Protocol::WORLD_BOSS_REQ_TYPE_CNACEL_INFO:
			{
				act->CancelBossInfo(role);
			}
			break;

		default:
			break;
	}


}

void MessageHandler::OnWorldBoss2Req(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWorldBoss2Req * req = (Protocol::CSWorldBoss2Req*)msg;

	if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_WORLD_BOSS_2))
	{
		return;
	}

	ActivityWorldBoss2* act = static_cast<ActivityWorldBoss2*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2));
	if (NULL == act)
	{
		return;
	}

	switch (req->req_type)
	{
		case Protocol::WORLD_BOSS_2_REQ_TYPE_REQ_INFO:
			{
				act->SendBossInfo(role);
			}
			break;
		case Protocol::WORLD_BOSS_2_REQ_TYPE_CNACEL_INFO:
			{
				act->CancelBossInfo(role);
			}
			break;

		default:
			break;
	}
}

void MessageHandler::OnWorldBoss3Req(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWorldBoss3Req * req = (Protocol::CSWorldBoss3Req*)msg;

	if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_WORLD_BOSS_3))
	{
		return;
	}

	ActivityWorldBoss3* act = static_cast<ActivityWorldBoss3*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3));
	if (NULL == act)
	{
		return;
	}

	switch (req->req_type)
	{
		case Protocol::WORLD_BOSS_REQ_TYPE_REQ_INFO:
		{
			act->SendBossInfo(role);
		}
		break;
		case Protocol::WORLD_BOSS_REQ_TYPE_CNACEL_INFO:
		{
			act->CancelBossInfo(role);
		}
		break;
		default:
		break;
	}
}

void MessageHandler::OnTreasureMapReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TREASURE_MAP_TASK, true))
	{
		return;
	}

	Protocol::CSTreasureMapReq * req = (Protocol::CSTreasureMapReq*)msg;

	switch (req->req_type)
	{
		case Protocol::TREASURE_MAP_REQ_TYPE_INFO:
			{
				role->GetRoleModuleManager()->GetTreasureMap()->SendInfo();
			}
			break;
		case Protocol::TREASURE_MAP_REQ_TYPE_ACCPET:
			{
				role->GetRoleModuleManager()->GetTreasureMap()->ReFresh(false);
			}
			break;
		case Protocol::TREASURE_MAP_REQ_TYPE_GIVE_UP:
			{
				role->GetRoleModuleManager()->GetTreasureMap()->Clear();
			}
			break;
		case Protocol::TREASURE_MAP_REQ_TYPE_ONE_CLICK_USE:
			{
				role->GetRoleModuleManager()->GetTreasureMap()->OneClickUsedTreasureMap((unsigned int)req->param1);
			}
			break;
		default:
			break;
	}
}

void MessageHandler::OnTradeMarketUserShelveDataReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	TradeMarketRouter::Instance().OnUserShelveInfoReq(role);
}

void MessageHandler::OnTradeMarketAddItemOnShelveReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSAddItemOnShelve* req = (Protocol::CSAddItemOnShelve*)msg;
	TradeMarketRouter::Instance().OnAddItemOnShelveReq(role, req);
}

void MessageHandler::OnTradeMarketRemoveItemOnShelveReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRemoveItemOnShelve* req = (Protocol::CSRemoveItemOnShelve*)msg;
	TradeMarketRouter::Instance().OnRemoveItemOnShelveReq(role, req);
}

void MessageHandler::OnTradeMarketCashOutReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketCashOut* req = (Protocol::CSTradeMarketCashOut*)msg;
	TradeMarketRouter::Instance().OnCashOutReq(role, req);
}

void MessageHandler::OnTradeMarketBuyItemReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketBuyItem* req = (Protocol::CSTradeMarketBuyItem*)msg;
	TradeMarketRouter::Instance().OnBuyItemReq(role, req);
}

void MessageHandler::OnTradeMarketGetRecentTransactionRecordsReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRecentTransactionRecords* req = (Protocol::CSRecentTransactionRecords*)msg;
	TradeMarketRouter::Instance().OnGetTransactionRecordsReq(role, req);
}

void MessageHandler::OnTradeMarketGetFocusListReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketGetFocusList* req = (Protocol::CSTradeMarketGetFocusList*)msg;
	TradeMarketRouter::Instance().OnGetFocusListReq(role, req);
}

void MessageHandler::OnTradeMarketAddFocusReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketAddFocus* req = (Protocol::CSTradeMarketAddFocus*)msg;
	TradeMarketRouter::Instance().OnAddFocusReq(role, req);
}

void MessageHandler::OnTradeMarketRemoveFocusReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketRemoveFocus* req = (Protocol::CSTradeMarketRemoveFocus*)msg;
	TradeMarketRouter::Instance().OnRemoveFocusReq(role, req);
}

void MessageHandler::OnTradeMarketGetFocusItemInfoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketGetFocusItemInfo* req = (Protocol::CSTradeMarketGetFocusItemInfo*)msg;
	TradeMarketRouter::Instance().OnGetFocusItemInfoReq(role, req);
}

void MessageHandler::OnTradeMarketAdvertiseReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketAdvertise* req = (Protocol::CSTradeMarketAdvertise*)msg;
	TradeMarketRouter::Instance().OnAdvertiseReq(role, req);
}

void MessageHandler::OnTradeMarketGetRoleTransactionRecordReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketGetRoleTransactionRecord* req = (Protocol::CSTradeMarketGetRoleTransactionRecord*)msg;
	TradeMarketRouter::Instance().OnGetRoleTransactionRecordsReq(role, req);
}

void MessageHandler::OnTradeMarketRedoAddItemOnShelveReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketRedoAddItemOnShelve* req = (Protocol::CSTradeMarketRedoAddItemOnShelve*)msg;
	TradeMarketRouter::Instance().OnRedoAddItemOnShelveReq(role, req);
}

void MessageHandler::OnTaskChainOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_TASK_CHAIN, true))
	{
		return;
	}

	Protocol::CSTaskChainOp* req = (Protocol::CSTaskChainOp*)msg;
	switch (req->type)
	{
	case	Protocol::CSTaskChainOp::ACCEPT:
		role->GetRoleModuleManager()->GetTaskChain()->OnAcceptTask();
		break;
	case	Protocol::CSTaskChainOp::GIVEUP:
		role->GetRoleModuleManager()->GetTaskChain()->OnGiveupTask();
		break;
	case	Protocol::CSTaskChainOp::DELIVERY:
		role->GetRoleModuleManager()->GetTaskChain()->OnDelivery(req->param1, req->param2);
		break;
	case	Protocol::CSTaskChainOp::TALK:
		role->GetRoleModuleManager()->GetTaskChain()->OnTalkToNpc();
		break;
	case  Protocol::CSTaskChainOp::SKIP:
		role->GetRoleModuleManager()->GetTaskChain()->OnSkipFight();
		break;
	case  Protocol::CSTaskChainOp::DELIVERY_PET:
		role->GetRoleModuleManager()->GetTaskChain()->OnCommitPet(req->param1);
		break;
	case  Protocol::CSTaskChainOp::FETCH_REWARD:
		role->GetRoleModuleManager()->GetTaskChain()->OnGiveTurnTableReward();
		break;
	case  Protocol::CSTaskChainOp::WAIT_ITEM:
		role->GetRoleModuleManager()->GetTaskChain()->OnGetItemByWait();
		break;
	}
}

void MessageHandler::OnTaskGamePlayReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTaskGameReq* req = (Protocol::CSTaskGameReq*)msg;
	role->GetRoleModuleManager()->GetTaskManager()->OnPlayGame(req->task_game_type, req->param1, req->param2);
}

void MessageHandler::OnCommitTaskMoneyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTaskCommitMoneyReq* req = (Protocol::CSTaskCommitMoneyReq*)msg;
	role->GetRoleModuleManager()->GetTaskManager()->OnCommitMoney(req->task_id);
}

void MessageHandler::OnFaBaoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_FABAO, true))
	{
		return;
	}

	Protocol::CSFabaoReq * req = (Protocol::CSFabaoReq*)msg;
	switch (req->req_type)
	{
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetFaBao()->SendFaBaoAllInfo();
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_ACTIVE:
		{
			role->GetRoleModuleManager()->GetFaBao()->ActiveFaBao(req->param1);
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_UPLEVEL:
		{
			role->GetRoleModuleManager()->GetFaBao()->UpLevelFaBao(req->param1, true);
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_ADVANCED:
		{
			role->GetRoleModuleManager()->GetFaBao()->AdvancedFaBao(req->param1);
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_GUIDE_MONSTER:
		{
			role->GetRoleModuleManager()->GetFaBao()->FabaoGuideFight();
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_DRAW_MATERIAL:
		{
			int index = req->param1;
			int items[] = { req->param2,req->param3, req->param4, req->param5, req->param6, };
			role->GetRoleModuleManager()->GetFaBao()->FaBaoSetDrawMaterial(index, items);
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_DRAW_SPEED_UP:
		{
			role->GetRoleModuleManager()->GetFaBao()->FaBaoDrawAccelerate(req->param1, req->param2);
		}
		break;
	case Protocol::CSFabaoReq::FABAO_REQ_TYPE_DRAW:
		{
			int index = req->param1;
			role->GetRoleModuleManager()->GetFaBao()->FaBaoDraw(index);
		}
		break;

	default:
		break;
	}
}

void MessageHandler::OnMapUnlockRemoveAnimation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMapUnlockRemoveAnima * mura = (Protocol::CSMapUnlockRemoveAnima *)msg;

	role->GetRoleModuleManager()->GetMapUnlock()->ReqestRemoveActiveIrregularID(mura->remove_irregular_id);
}

void MessageHandler::OnMedalReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_MEDAL, true))
	{
		return;
	}

	Protocol::CSMedalReq *req = (Protocol::CSMedalReq*)msg;

	switch (req->req_type)
	{
		case Protocol::MEDAL_REQ_TYPE_INFO:
			{
				role->GetRoleModuleManager()->GetMedal()->SendInfo();
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_PUT_ON:
			{
				role->GetRoleModuleManager()->GetMedal()->PutOn(req->param1, req->param2,req->param3);
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_TAKE_OFF:
			{
				role->GetRoleModuleManager()->GetMedal()->TakeOff(req->param1);
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_EXCHANGE_MATERIAL:
			{
				role->GetRoleModuleManager()->GetMedal()->ExcahngFbMaterial(req->param1);
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_DECOMPOSE:
			{
				role->GetRoleModuleManager()->GetMedal()->DecomposeMedal(req->param1, req->param2);
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_LOTTREY:
			{
				role->GetRoleModuleManager()->GetMedal()->OnlottreyMedal(req->param1, req->param2);
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_UPGRADE:
			{
				role->GetRoleModuleManager()->GetMedal()->OnMedalUpGrade(req->param1, req->param2, req->param3, req->param4, false);
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_UPGRADE_2:
			{
				role->GetRoleModuleManager()->GetMedal()->OnMedalUpGrade(0, req->param1, req->param3, req->param4, true);
			}
			break;
		case Protocol::MEDAL_REQ_TYPE_SPE_EXCHANGE:
			{
				role->GetRoleModuleManager()->GetMedal()->OnSpeMedalExchange(req->param1, req->param2, req->param3, req->param4, (0 != req->param5));
			}
			break;
		default:
			break;
	}
}

void MessageHandler::OnMedalOneKeyGradeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_MEDAL, true))
	{
		return;
	}

	Protocol::CSMedalOneKeyGradeReq *req = (Protocol::CSMedalOneKeyGradeReq*)msg;

	role->GetRoleModuleManager()->GetMedal()->OnOneKeyMedalUpGrade(req->count, req->column, req->index_flag);
}

void MessageHandler::OnZhuLingReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSZhuLingReq * req = (Protocol::CSZhuLingReq *)msg;

	int ret = 0;
	switch (req->req_type)
	{
		case Protocol::ZHU_LING_REQ_TYPE_UP_LEVEL:
			{
				
			}
			break;
		case Protocol::ZHU_LING_REQ_TYPE_PUT_ON:
			{
				ret = role->GetRoleModuleManager()->GetRoleZhuLing()->PutOn(req->param1, req->param2, req->param3, req->param4, req->param5, req->param6, req->param7, req->param8);
			}
			break;

		case Protocol::ZHU_LING_REQ_TYPE_TAKE_OFF:
			{
				ret = role->GetRoleModuleManager()->GetRoleZhuLing()->TakeOff(req->param1, req->param2, req->param3, req->param4);
			}
			break;
	}

	if (ret != 0)
	{
		gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s] req->req_type:%d ret:%d", __FUNCTION__, __LINE__,
					       role->GetUID(), role->GetName(), req->req_type, ret);
	}
}

void MessageHandler::OnZhuLingDecomposeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSZhuLingDecomposeReq * req = (Protocol::CSZhuLingDecomposeReq *)msg;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSZhuLingDecomposeReq, item_list));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->item_list[0]);
	if (count <= 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->item_list))
	{
		return;
	}

	int ret=role->GetRoleModuleManager()->GetRoleZhuLing()->Decompose(count, req->item_list);

	if (ret != 0)
	{
		gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s]  ret:%d", __FUNCTION__, __LINE__,
					       role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnZhuLingUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSZhuLingUpLevelReq * req = (Protocol::CSZhuLingUpLevelReq *)msg;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSZhuLingUpLevelReq, consume_list));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->consume_list[0]);
	if (count <= 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->consume_list))
	{
		return;
	}

	int ret = role->GetRoleModuleManager()->GetRoleZhuLing()->UpLevelZhuLing(req->item_src, req->index, req->column, req->slot_index, req->buy_item_num, count, req->consume_list);

	if (ret != 0)
	{
		gamelog::g_log_zhu_ling.printf(LL_INFO, "%s line:%d | role[%d %s]  ret:%d", __FUNCTION__, __LINE__,
					       role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnCrystalFbReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCrystalFbReq *req = (Protocol::CSCrystalFbReq*)msg;

	switch (req->req_type)
	{
		case Protocol::CRYSTAL_FB_REQ_TYPE_NEXT_LAYER:
			{
				SpecialLogic * special_lobgic = scene->GetSpecialLogic();
				if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_CRYSTAL_FB)
				{
					return;
				}
				SpecialLogicCrystalFB * crystal_special_lobgic = (SpecialLogicCrystalFB *)special_lobgic;
				crystal_special_lobgic->OnReqNextLevel(role);
			}
			break;
		case Protocol::CRYSTAL_FB_REQ_TYPE_EXIT_SCENE:
			{
				SpecialLogic * special_lobgic = scene->GetSpecialLogic();
				if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_FANTASY_FB)
				{
					return;
				}
				SpecialLogicFantasyFb * fantasy_special_logic = (SpecialLogicFantasyFb *)special_lobgic;
				fantasy_special_logic->OnLeaveFBSituation(role, req->param1);
			}
			break;
		case Protocol::CRYSTAL_FB_REQ_TYPE_BUY_ENERGY:	
			{
				role->GetRoleModuleManager()->GetRoleCrystalFb()->OnBuyEnergy();
			}
			break;
		case Protocol::CRYSTAL_FB_REQ_TYPE_BUY_TIMES:
			{
				role->GetRoleModuleManager()->GetRoleCrystalFb()->OnRoleBuyNormalFBTimes(req->param1);
			}
			break;
		case  Protocol::CRYSTAL_FB_REQ_TYPE_GET_REWARD:
			{
				role->GetRoleModuleManager()->GetRoleCrystalFb()->GetRewardPassCrystalFb(req->param1);
			}
			break;
		default:
			break;
	}
}

void MessageHandler::OnShopBuyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHOP, true))
	{
		return;
	}

	Protocol::CSShopBuyReq* req = (Protocol::CSShopBuyReq*)msg;
	
	role->GetRoleModuleManager()->GetShop()->OnBuy(req->index, req->num);
}

void MessageHandler::OnGoldShopBuyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHOP, true))
	{
		return;
	}

	// 改为了商会协议
	Protocol::CSGoldShopBuyReq* req = (Protocol::CSGoldShopBuyReq*)msg;
	role->GetRoleModuleManager()->GetShop()->OnBuyNewShop(req->index, req->num, req->is_huashen_buy);
}

void MessageHandler::OnXunBaoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSXunBaoReq* req = (Protocol::CSXunBaoReq*)msg;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSXunBaoReq, comsume_info));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->comsume_info[0]);
	if (count < 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->comsume_info))
	{
		return;
	}

	if (req->guide_draw == 0)		//0普通寻宝
	{
		int ret = XunBao::Instance().Draw(role, req->draw_type, req->draw_times, count, req->comsume_info, req->is_use_free != 0);
		if (ret != 0)
		{
			gamelog::g_log_xun_bao.printf(LL_INFO, "XunBao::DrawRet role_id:%d,role_name:%s draw_ret:%d  draw_type:%d draw_times:%d is_use_free:%d", 
						      role->GetUID(), role->GetName(), ret, req->draw_type, req->draw_times, req->is_use_free);
		}
	}

	if (req->guide_draw == 1)		//1 指引寻宝1
	{
		XunBao::Instance().DrawGuidePet(role, req->comsume_info[0].col, req->comsume_info[0].idx);
	}

	if (req->guide_draw == 2)		//2 指引寻宝2
	{
		XunBao::Instance().DrawGuidePet2(role, req->comsume_info[0].col, req->comsume_info[0].idx);
	}

	if (req->guide_draw == 3)		//3 指引伙伴寻宝
	{
		XunBao::Instance().DrawGuidePartner(role);
	}

	if (req->guide_draw == 4)		//4 指引伙伴寻宝2
	{
		XunBao::Instance().DrawGuidePartner2(role);
	}
}

void MessageHandler::OnXunBaoExchange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSXunBaoExchange * req = (Protocol::CSXunBaoExchange *)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSXunBaoExchange, comsume_item));
	if (length < offset)
	{
		return;
	}

	std::set<Protocol::XunBaoExchangeComsumeItem> comsume1, comsume2;

	int count = (length - offset) / sizeof(req->comsume_item[0]);

	for (int index = 0; index < count && index < ARRAY_LENGTH(req->comsume_item); ++index)
	{
		if (req->comsume_item[index].comsume_type == 0)
		{
			comsume1.insert(req->comsume_item[index]);
		}

		if (req->comsume_item[index].comsume_type == 1)
		{
			comsume2.insert(req->comsume_item[index]);
		}
	}

	XunBao::Instance().ExchangeItem(role, req->draw_type, req->get_index, req->comsume_index1, req->comsume_index2, comsume1, comsume2);
}

void MessageHandler::OnXunBaoBelssReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSXunBaoBelssReq * req = (Protocol::CSXunBaoBelssReq *)msg;

	role->GetRoleModuleManager()->OnXunBaoBelssReq(req->index, req->pet_id);
}

void MessageHandler::OnShopFastBuyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHOP, true))
	{
		return;
	}

	Protocol::CSFastPurchase * req = (Protocol::CSFastPurchase *)msg;

	role->GetRoleModuleManager()->GetShop()->OnFastBuy(req->item_id, req->buy_num);
}

void MessageHandler::OnGoldShopCancel(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHOP, true))
	{
		return;
	}

	Protocol::CSGoldShopNotifyCancelReq * req = (Protocol::CSGoldShopNotifyCancelReq *)msg;
	if (req->cancel_index >= 0)
	{
		role->GetRoleModuleManager()->GetShop()->CancelSingleNotify(req->cancel_index);
	}
	else
	{
		role->GetRoleModuleManager()->GetShop()->SendPurchaseNotify();
	}
}

void MessageHandler::OnMarketReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMarketReq * req = (Protocol::CSMarketReq *)msg;
	switch (req->op_type)
	{
		case Protocol::CSMarketReq::MARKET_REQ_TYPE_INFO:
		{
			MarketManager::Instance().SendAllInfo(role);
		}
		break;
		case Protocol::CSMarketReq::MARKET_REQ_TYPE_BUY_ITEM:
		{
			MarketManager::Instance().OnBuyItem(role, req->param1, req->param2);
		}
		break;
		case Protocol::CSMarketReq::MARKET_REQ_TYPE_SALE_ITEM:
		{
			MarketManager::Instance().OnSaleItem(role, req->param1, req->param2, req->param3);
		}
		break;
		case  Protocol::CSMarketReq::MARKET_REQ_TYPE_SALE_FU_SHI:
		{
			MarketManager::Instance().OnSaleFuShi(role, req->param1);
		}
		break;
		default:
		break;
	}
}

void MessageHandler::OnColosseumReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSColosseumReq * req = (Protocol::CSColosseumReq *)msg;

	if (CrossConfig::Instance().IsHiddenServer() && req->req_type != Protocol::CSColosseumReq::REQ_TYPE_GET_AWARD && req->req_type != Protocol::CSColosseumReq::REQ_TYPE_SWEEP)
	{
		crossgameprotocal::HiddenGameRoleColosseumReqRouter router;
		router.uid = role->GetUID();
		router.req_type = req->req_type;
		router.param = req->param;

		int plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
		int server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
		InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&router, sizeof(router));
		
		return;
	}

	switch (req->req_type)
	{
	case Protocol::CSColosseumReq::REQ_TYPE_FIHGT:
		ColosseumManager::Instance().OnFightReq(role, req->param);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_REFRESH_TARGET:
		if (!role->GetRoleModuleManager()->GetFuncGuide()->HasChallengeColosseumBefore())
		{
			ColosseumManager::Instance().SendTargetInfo(role->GetUID());
			break;
		}
		ColosseumManager::Instance().OnRefreshTarget(role->GetUID());
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_ROLE_INFO:
		ColosseumManager::Instance().SendRoleInfo(role);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_TARGET_INFO:
		ColosseumManager::Instance().SendTargetInfo(role->GetUID());
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_TOP_INFO:
		ColosseumManager::Instance().SendTopInfo(role);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_RANK_INFO:
		ColosseumManager::Instance().SendRankInfo(role, req->param);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_REPORT_INFO:
		ColosseumManager::Instance().SendReportInfo(role);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_FETCH_EXP:
		//ColosseumManager::Instance().OnFetchExp(role, req->param);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_GET_AWARD:
		//排名奖励已经去除
		//ColosseumManager::Instance().FetchRankReward(role, req->param);
		//新排名奖励
		ColosseumManager::Instance().FetchRankEveryDayReward(role);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_ACHIEVEMENT_INFO:
		ColosseumManager::Instance().SendRoleAchievement(role);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_GET_ACHIEVEMENT:
		ColosseumManager::Instance().OnRoleGetAchievement(role, req->param);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_GET_RANK_TOP_INFO:
		ColosseumManager::Instance().SendRankTopSimpleInfo(role);
		break;
	case Protocol::CSColosseumReq::REQ_TYPE_SWEEP:
		ColosseumManager::Instance().OnSweepReq(role, req->param);
		break;
	}
}

void MessageHandler::OnExchangeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSExchangeReq* req = (Protocol::CSExchangeReq*)msg;
	role->GetRoleModuleManager()->GetExchange()->OnExchange(req->seq, req->num);
}

void MessageHandler::OnBraveGroundReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBraveGroundReq * req = (Protocol::CSBraveGroundReq*)msg;

	//if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_BRAVE_GROUND)) //以改成常开活动，不需要判断开启
	//{
	//	return;
	//}

	if (!CrossConfig::Instance().IsHiddenServer() && Protocol::BRAVE_GROUND_REQ_TYPE_SAO_DANG != req->req_type)
	{
		crossgameprotocal::GameHiddenHBraveGroundReq send_hidden_req;
		send_hidden_req.origin_role_id = role->GetUID();
		send_hidden_req.req_type = req->req_type;
		send_hidden_req.p1 = req->p1;
		send_hidden_req.p2 = req->p2;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

		return;
	}


	ActivityBraveGround* act = static_cast<ActivityBraveGround*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_BRAVE_GROUND));
	if (NULL == act)
	{
		return;
	}

	switch (req->req_type)
	{
		case Protocol::BRAVE_GROUND_REQ_TYPE_RANK_INFO:
			{
				act->SendRankInfo(role);
			}
			break;
		case Protocol::BRAVE_GROUND_REQ_TYPE_USE_NPC:
			{
				if (scene->GetSceneType() == Scene::SCENE_TYPE_BRAVE_GROUND)
				{
					SpecialLogicBraveGround * special_lobgic = (SpecialLogicBraveGround *)scene->GetSpecialLogic();
					special_lobgic->UseNpcFunction(role, req->p1);
				}
			}
			break;
		case Protocol::BRAVE_GROUND_REQ_TYPE_INFO:
			{
				act->SendInfo(role);
			}
			break;
		case Protocol::BRAVE_GROUND_REQ_TYPE_ROUTE:
			{
				if (scene->GetSceneType() == Scene::SCENE_TYPE_BRAVE_GROUND)
				{
					SpecialLogicBraveGround * special_lobgic = (SpecialLogicBraveGround *)scene->GetSpecialLogic();
					special_lobgic->SendRoute(role, req->p1, req->p2);
				}
			}
			break;
		case Protocol::BRAVE_GROUND_REQ_TYPE_SELECT_LEVEL:
			{
				if (scene->GetSceneType() == Scene::SCENE_TYPE_BRAVE_GROUND)
				{
					SpecialLogicBraveGround * special_lobgic = (SpecialLogicBraveGround *)scene->GetSpecialLogic();
					special_lobgic->SelectLevel(role, req->p1);
				}
			}
			break;
		case Protocol::BRAVE_GROUND_REQ_TYPE_SAO_DANG:
			{
				int ret = act->SaoDang(role);
				gamelog::g_log_brave_ground.printf(LL_INFO, " %s %d | role[%d %s] ret:%d", __FUNCTION__, __LINE__, role->GetUID(), role->GetName(), ret);
			}
			break; 

		default:
			break;
	}
}

void MessageHandler::OnRandActivityOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRandActivityOperaReq* req = (Protocol::CSRandActivityOperaReq*)msg;
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityOpReq(req);
}

void MessageHandler::OnActivityChiefElectionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_CHIEF_ELETION, true))
	{
		return;
	}

	Protocol::CSChiefElectionReq * req = (Protocol::CSChiefElectionReq * )msg;
	
	if (ActivityManager::Instance().IsActivityClose(ACTIVITY_TYPE_CHIEF_ELECTION))
	{
		return;
	}

	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity == NULL)
	{
		return;
	}

	if (req->req_type == Protocol::CHIEF_ELECTION_REQ_TYPE_OP_LOG)
	{
		gamelog::g_Log_chief_election.printf(LL_INFO, "chief_election op log role[%d %s] p1:%d p2:%d", role->GetUID(), role->GetName(), req->p1, req->p2);
	}

	if (!role->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
			return;
		}

		switch (req->req_type)
		{
			case Protocol::CHIEF_ELECTION_REQ_TYPE_READY:
				{
					CrossData_Business business_data;
					business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
					business_data.auto_join_type_param1 = ACTIVITY_TYPE_CHIEF_ELECTION;
					business_data.param1 = Protocol::CHIEF_ELECTION_REQ_TYPE_READY;
					business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
					role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
				}
				break;
			case Protocol::CHIEF_ELECTION_REQ_TYPE_GOTO_SEA:
				{
					CrossData_Business business_data;
					business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
					business_data.auto_join_type_param1 = ACTIVITY_TYPE_CHIEF_ELECTION;
					business_data.param1 = Protocol::CHIEF_ELECTION_REQ_TYPE_GOTO_SEA;
					business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
					role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
				}
				break;

			default:
				{
					UniqueUserID uuid = role->GetUniqueUserID();
					crossgameprotocal::GameHiddenChiefElectionReq send_hidden_req;
					send_hidden_req.origin_plat_type = uuid.plat_type;
					send_hidden_req.origin_server_id = LocalConfig::Instance().GetMergeServerId();
					send_hidden_req.origin_role_id = UidToInt(uuid.user_id);
					send_hidden_req.base_prof = role->GetBaseProfession();
					send_hidden_req.req_type = req->req_type;
					send_hidden_req.p1 = req->p1;
					send_hidden_req.p2 = req->p2;
					InternalComm::Instance().SendToHiddenThroughCross((const char*)&send_hidden_req, sizeof(send_hidden_req));
				}
				break;
		}

		return;
	}
	else
	{
		switch (req->req_type)
		{
			case Protocol::CHIEF_ELECTION_REQ_TYPE_MATCH_INFO:
				{
					activity->SendMatchInfo(role);
				}
				break;
			case Protocol::CHIEF_ELECTION_REQ_TYPE_READY:
				{
					if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_CHIEF_ELECTION))
					{
						return;
					}

					activity->GotoReadyScene(role);
				}
				break;
			case Protocol::CHIEF_ELECTION_REQ_TYPE_GOTO_SEA:
				{
					if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_CHIEF_ELECTION))
					{
						return;
					}

					activity->GotoChiefElectionSea(role);
				}
				break;
		}
	}
}

void MessageHandler::OnActivityGuildHonorReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildHonorReq * req = (Protocol::CSGuildHonorReq *)msg;

	if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_HONOR_BATTLE))
	{
		return;
	}

	ActivityGuildHonorBattle * activity = (ActivityGuildHonorBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_HONOR_BATTLE);
	if (activity == NULL)
	{
		return;
	}

	switch (req->req_type)
	{
		case Protocol::GUILD_HONOR_REQ_TYPE_ASSIGN:			//p1 分配到区域（0,1,2,3);  P2 role_id
			{
				if (scene->GetSceneType() != Scene::SCENE_TYPE_GUILD_HONOR_BATTLE)
				{
					return;
				}

				SpecialLogicGuildHonorBattle * guild_honor_special = (SpecialLogicGuildHonorBattle *)scene->GetSpecialLogic();
				guild_honor_special->AssignedRole(role, req->p1, req->p2);
			}
			break;
		case Protocol::GUILD_HONOR_REQ_TYPE_JOIN:			// 到活动场景
			{
				activity->GoToBattleScene(role);
			}
			break;
		case Protocol::GUILD_HONOR_REQ_TYPE_AUTO_ASSIGN:			//自动分配
			{
				if (scene->GetSceneType() != Scene::SCENE_TYPE_GUILD_HONOR_BATTLE)
				{
					return;
				}

				SpecialLogicGuildHonorBattle * guild_honor_special = (SpecialLogicGuildHonorBattle *)scene->GetSpecialLogic();
				guild_honor_special->AutoAssignedRole(role);
			}
			break;
	}
}

void MessageHandler::OnActivityGuildHonorReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildHonorReq2 * req = (Protocol::CSGuildHonorReq2 *)msg;

	if (!role->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
			return;
		}

		crossgameprotocal::GameHiddenGuildHonorBattleReqRoute send_hidden_req;
		send_hidden_req.role_id = role->GetUID();
		send_hidden_req.role_guild_id = role->GetGuildID();
		send_hidden_req.req_type = req->req_type;
		send_hidden_req.p1 = req->p1;
		send_hidden_req.p2 = req->p2;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));
	}
	else
	{
		switch (req->req_type)
		{
			case Protocol::GUILD_HONOR_REQ_TYPE_2_SCORE_INFO:
				{
					GuildHonorBattleManager::Instance().SendGuildScoreInfo(req->p1, role);
				}
				break;
			case Protocol::GUILD_HONOR_REQ_TYPE_2_SCORE_RANK:
				{
					GuildHonorBattleManager::Instance().SendGuildScoreRankInfo(role);
				}
				break;
			case Protocol::GUILD_HONOR_REQ_TYPE_2_GUESSING:
				{
					GuildHonorBattleManager::Instance().Guessing(req->p2, req->p1, false, role->GetGuildID(), role->GetUID());
				}
				break;
			case Protocol::GUILD_HONOR_REQ_TYPE_2_CANCEL_GUESSING:
				{
					GuildHonorBattleManager::Instance().CancelGuessing(req->p1, req->p2, false, role->GetUID());
				}
				break;
			case Protocol::GUILD_HONOR_REQ_TYPE_2_GUESSING_CHAMPION:
				{
					GuildHonorBattleManager::Instance().Guessing(1, req->p1, false, role->GetGuildID(), role->GetUID());
				}
				break;
			case Protocol::GUILD_HONOR_REQ_TYPE_2_CANCEL_GUESSING_CHAMPION:
				{
					GuildHonorBattleManager::Instance().CancelGuessing(1, req->p2, false, role->GetUID());
				}
				break;
			case Protocol::GUILD_HONOR_REQ_TYPE_2_MATCH_RECORD_INFO:
				{
					GuildHonorBattleManager::Instance().SendMatchRecordInfo(role);
				}
				break;
			case Protocol::GUILD_HONOR_REQ_TYPE_2_GUESSING_INFO:
				{
					GuildHonorBattleManager::Instance().SendGuessInfo(role->GetUID());
				}
				break;
		}
	}
}

void MessageHandler::OnActivityChiefPeakBattleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_CHIEF_PEAK, true))
	{
		return;
	}

	Protocol::CSChiefPeakBattleReq * req = (Protocol::CSChiefPeakBattleReq *)msg;

	if (ActivityManager::Instance().IsActivityClose(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE))
	{
		return;
	}

	ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE);
	if (activity == NULL)
	{
		return;
	}

	if (!role->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
			return;
		}

		switch (req->req_type)
		{
			case Protocol::CHIEF_PEAK_BATTLE_REQ_TYPE_READY:
				{
					CrossData_Business business_data;
					business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
					business_data.auto_join_type_param1 = ACTIVITY_TYPE_CHIEF_PEAK_BATTLE;
					business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
					role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
				}
				break;
			case Protocol::CHIEF_PEAK_BATTLE_REQ_TYPE_BET:
				{
					role->NoticeNum(errornum::EN_ACTIVITY_CHIEF_PEAK_BATTLE_BET_NOT_IN_CROSS);
				}
				break;
			default:
				{
					UniqueUserID uuid = role->GetUniqueUserID();
					crossgameprotocal::GameHiddenChiefPeakBattleReq send_hidden_req;
					send_hidden_req.origin_plat_type = uuid.plat_type;
					send_hidden_req.origin_server_id = LocalConfig::Instance().GetMergeServerId();
					send_hidden_req.origin_role_id = UidToInt(uuid.user_id);
					send_hidden_req.req_type = req->req_type;
					send_hidden_req.p1 = req->p1;
					send_hidden_req.p2 = req->p2;
					InternalComm::Instance().SendToHiddenThroughCross((const char*)&send_hidden_req, sizeof(send_hidden_req));
				}
				break;
		}

		return;
	}
	else
	{
		switch (req->req_type)
		{
			case Protocol::CHIEF_PEAK_BATTLE_REQ_TYPE_ROLE_INFO:
				{
					activity->SendRoleInfo(role);
				}
				break;
			case Protocol::CHIEF_PEAK_BATTLE_REQ_TYPE_MATCH_INFO:
				{
					activity->SendMatchInfo(role);
				}
				break;
			case Protocol::CHIEF_PEAK_BATTLE_REQ_TYPE_BET:
				{
					if (!ActivityManager::Instance().IsActivityStandy(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE))
					{
						return;
					}

					activity->Bet(role, req->p1, req->p2);
				}
				break;
			case Protocol::CHIEF_PEAK_BATTLE_REQ_TYPE_READY:
				{
					if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE))
					{
						return;
					}

					activity->GotoReadyScene(role);
				}
				break;
		}
	}
}

void MessageHandler::OnActivityHuanJieZhanChangReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHuanJieZhanChangReq * req = (Protocol::CSHuanJieZhanChangReq *)msg;

	if (req->req_type == Protocol::HUAN_JIE_ZHAN_CHANG_REQ_TYPE_MATCH && !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG))
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity == NULL)
	{
		return;
	}

	if (req->req_type != HUAN_JIE_ZHAN_CHANG_LEADER_VIEW && !role->IsInCross())
	{
		switch (req->req_type)
		{
			case Protocol::HUAN_JIE_ZHAN_CHANG_REQ_TYPE_MATCH:
				{
					if (!CrossManager::Instance().IsCrossServerConnected())
					{
						role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
						return;
					}

					role->GetRoleModuleManager()->GetRoleActivityManager()->JoinActivityInCross(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG, 0);
				}
				break;
			case Protocol::HUAN_JIE_ZHAN_CHANG_FETCH_TODAY_REWARD:
				{
					role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
					return;
				}
				break;
			default:
				{
					crossgameprotocal::GameHiddenHuanJieZhanChangeReq send_hidden_req;
					send_hidden_req.origin_role_id = role->GetUID();
					send_hidden_req.req_type = req->req_type;
					send_hidden_req.p1 = req->param1;
					send_hidden_req.p2 = 0;
					InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));
				}
				break;
		}

		return;
	}
	else
	{
		switch (req->req_type)
		{
			case Protocol::HUAN_JIE_ZHAN_CHANG_REQ_TYPE_MATCH:
				{
					activity->OnRoleMatchReq(role);
				}
				break;
			case Protocol::HUAN_JIE_ZHAN_CHANG_REQ_TYPE_CANCEL_MATCH:
				{
					activity->OnRoleCancelMatchReq(role->GetUID());
				}
				break;
			case Protocol::HUAN_JIE_ZHAN_CHANG_REQ_TYPE_RANK_INFO:
				{
					activity->SendRankInfo(role, req->param1);
				}
				break;
			case Protocol::HUAN_JIE_ZHAN_CHANG_REQ_INFO:
				{
					activity->SendRoleInfo(role);
				}
				break;
			case Protocol::HUAN_JIE_ZHAN_CHANG_PROF_WIN_LOSE_INFO:
				{
				}
				break;
			case Protocol::HUAN_JIE_ZHAN_CHANG_LEADER_VIEW:
				{
					Team * team = role->GetMyTeam();
					if (team == NULL)
					{
						return;
					}

					team->LeaderViewHuanJie(req->param1);
				}
				break;
			case Protocol::HUAN_JIE_ZHAN_CHANG_FETCH_TODAY_REWARD:
				{
					activity->FetchTodayWinReward(role, req->param1);
				}
				break;
		}
	}
}

void MessageHandler::OnActivityHuanJieZhanChangRoleRankListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHuanJieZhanChangRoleRankListReq * req = (Protocol::CSHuanJieZhanChangRoleRankListReq *)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSHuanJieZhanChangRoleRankListReq, role_id));
	if (length < offset)
	{
		return;
	}

	int role_id_num = (length - offset) / sizeof(req->role_id[0]);

	if (role_id_num > ARRAY_LENGTH(req->role_id))
	{
		return;
	}


	if (!role->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
			return;
		}

		crossgameprotocal::GameHiddenHuanJieZhanChangeReq2 send_hidden_req;
		send_hidden_req.origin_role_id = role->GetUID();
		send_hidden_req.count = 0;
		for (int i = 0; i < role_id_num && i < ARRAY_LENGTH(req->role_id) && i < ARRAY_LENGTH(send_hidden_req.role_id_list); ++i)
		{
			send_hidden_req.role_id_list[i] = req->role_id[i];
			send_hidden_req.count += 1;
		}

		int send_len = sizeof(send_hidden_req) - (ARRAY_LENGTH(send_hidden_req.role_id_list) - send_hidden_req.count) * sizeof(send_hidden_req.role_id_list[0]);
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, send_len);
		return;
	}
	else
	{
		ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
		if (activity == NULL)
		{
			return;
		}

		std::set<int> role_id_list(req->role_id, req->role_id + role_id_num);
		activity->QueryRoleRankInfo(role, role_id_list);
	}
}

void MessageHandler::OnPlatformBattleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PLATFORM, true))
	{
		return;
	}

	Protocol::CSPlatformBattleReq * req = (Protocol::CSPlatformBattleReq *)msg;
	switch (req->req_type)
	{
		case Protocol::PLATFORM_BATTLE_REQ_TYPE_ROLE_INFO:		//角色信息
			{
				PlatformBattle::Instance().SendRoleInfo(role);
			}
			break;

		case Protocol::PLATFORM_BATTLE_REQ_TYPE_RANK_INFO:		//擂台信息 p1 擂台编号
			{
				if (!role->IsInCross())
				{
					crossgameprotocal::GameHiddenPlatformBattleReq send_hidden_req;
					send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_PLATFORM_INFO;
					send_hidden_req.param1 = req->req_type;
					send_hidden_req.param2 = role->GetUID();
					send_hidden_req.param3 = req->p1;
					send_hidden_req.param4 = req->p2;
					InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

					return;
				}

				PlatformBattle::Instance().SendPlatformInfo(role, req->p1);
			}
			break;
		case Protocol::PLATFORM_BATTLE_REQ_TYPE_CHALLENGE:			//挑战 p1 擂台编号	,p2 rank ,  pet_index要上场的宠物，partner_id要上场的伙伴
			{
				PlatformBattle::Instance().ChallengePlatform(role, req->p1, req->p2, req->pet_index, req->partner_id);
			}
			break;
		case Protocol::PLATFORM_BATTLE_REQ_TYPE_REMOVE:			//移除
			{
				PlatformBattle::Instance().RemovePlatformPlayInfo(role, req->p1, true, __FUNCTION__);
			}
			break;
		case Protocol::PLATFORM_BATTLE_REQ_TYPE_RECORD:			//挑战 p1 擂台编号	,p2 rank ,  pet_index要上场的宠物，partner_id要上场的伙伴
			{
				if (!role->IsInCross())
				{
					crossgameprotocal::GameHiddenPlatformBattleReq send_hidden_req;
					send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_RQE_ROUTE_HIDDEN;
					send_hidden_req.param1 = req->req_type;
					send_hidden_req.param2 = role->GetUID();
					send_hidden_req.param3 = req->p1;
					send_hidden_req.param4 = req->p2;
					InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

					return;
				}

				PlatformBattle::Instance().SendRecord(role);
			}
			break;
		case Protocol::PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE:	
		case Protocol::PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE_2:
			{
				if (!role->IsInCross())
				{
					crossgameprotocal::GameHiddenPlatformBattleReq send_hidden_req;
					send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_RQE_ROUTE_HIDDEN;
					send_hidden_req.param1 = req->req_type;
					send_hidden_req.param2 = role->GetUID();
					send_hidden_req.param3 = req->p1;
					send_hidden_req.param4 = req->p2;
					InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

					return;
				}

				PlatformBattle::Instance().SetClientNotice(role->GetUID(), req->req_type, req->p1);
			}
			break;
		case Protocol::PLATFORM_BATTLE_REQ_TYPE_LAST_RANK_INFO:			//挑战 p1 擂台编号	,p2 rank ,  pet_index要上场的宠物，partner_id要上场的伙伴
			{
				if (!role->IsInCross())
				{
					crossgameprotocal::GameHiddenPlatformBattleReq send_hidden_req;
					send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_RQE_ROUTE_HIDDEN;
					send_hidden_req.param1 = req->req_type;
					send_hidden_req.param2 = role->GetUID();
					send_hidden_req.param3 = req->p1;
					send_hidden_req.param4 = req->p2;
					InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

					return;
				}

				PlatformBattle::Instance().SendLastRankInfo(role);
			}
			break;
	}
}

void MessageHandler::OnPlatformBattleReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPlatformBattleReq2 * req = (Protocol::CSPlatformBattleReq2 *)msg;

	//if (!role->IsInCross())
	//{
	//	crossgameprotocal::GameHiddenPlatformBattleReq send_hidden_req;
	//	send_hidden_req.type = crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_RQE_ROUTE_HIDDEN;
	//	send_hidden_req.param1 = req->req_type;
	//	send_hidden_req.param2 = role->GetUID();
	//	send_hidden_req.param3 = req->p1;
	//	send_hidden_req.param4 = req->p2;
	//	InternalComm::Instance().SendToHiddenThroughCross((const char *)&send_hidden_req, sizeof(send_hidden_req));

	//	return;
	//}

	switch (req->req_type)
	{
		case Protocol::PLATFORM_BATTLE_REQ2_TYPE_ACHIEVEMENT:		//角色信息
			{
				if (!role->IsInCross())
				{
					role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
					return;
				}
				PlatformBattle::Instance().FetchAchievementInfoReward(role, req->param1, req->param2);
			}
			break;
	}
}

void MessageHandler::OnPlatformBattleFirstInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPlatformBattleRankFirstInfoReq * protocol = (Protocol::CSPlatformBattleRankFirstInfoReq *)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSPlatformBattleRankFirstInfoReq, platform_number_list));
	if (length < offset)
	{
		return;
	}

	int platform_number_count = (length - offset) / sizeof(protocol->platform_number_list[0]);

	std::vector<unsigned char> platform_number_list(protocol->platform_number_list, protocol->platform_number_list + platform_number_count);
	PlatformBattle::Instance().SendPlatformFirstInfo(role, platform_number_list);
}

void MessageHandler::OnCourseOfGrowthRequest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCOGOperatorRequest * req = (Protocol::CSCOGOperatorRequest *)msg;

	switch (req->op_type)
	{
	case Protocol::CSCOGOperatorRequest::COG_OP_RECEIVES_AWARD:
		role->GetRoleModuleManager()->GetCourseOfGrowth()->GetTaskAward(req->param);
		break;
	case Protocol::CSCOGOperatorRequest::COG_OP_FIGHT_THE_BOSS:
		role->GetRoleModuleManager()->GetCourseOfGrowth()->BeginToFightCOGBoss();
		break;
	case Protocol::CSCOGOperatorRequest::COG_OP_FETCH_LEVEL_REWARD:
		role->GetRoleModuleManager()->GetCourseOfGrowth()->OnFetchCourseLevelReward(req->param);
		break;
	case Protocol::CSCOGOperatorRequest::COG_OP_SERVER_COURSE_INFO:
		CourseOfGrowthManager::Instance().OnSendAllInfo(role);
		break;
	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

void MessageHandler::OnTradeMarketSearchByItemReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketSearchByItemID* req = (Protocol::CSTradeMarketSearchByItemID*)msg;
	
	TradeMarketRouter::Instance().OnSearchByItemIdReq(role, req);
}

void MessageHandler::OnTradeMarketSearchByTypeReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketSearchByItemSearchType* req = (Protocol::CSTradeMarketSearchByItemSearchType*)msg;

	if (req->item_big_search_type >= 0)
	{
		TradeMarketRouter::Instance().OnSearchByItemSearchBigTypeReq(role, req);
	}
	else
	{
		TradeMarketRouter::Instance().OnSearchByItemSearchTypeReq(role, req);
	}
}

void MessageHandler::OnTradeMarketSearchAdvertiseReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketSearchAdvertise* req = (Protocol::CSTradeMarketSearchAdvertise*)msg;
	TradeMarketRouter::Instance().OnSearchByAdvertiseReq(role, req);
}

void MessageHandler::OnTradeMarketGetSellingListReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketGetSellingList* req = (Protocol::CSTradeMarketGetSellingList*)msg;
	TradeMarketRouter::Instance().OnGetSellingListReq(role, req);
}

void MessageHandler::OnTradeMarketSearchPetForQuickBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketSearchPetForQuickBuy* req = (Protocol::CSTradeMarketSearchPetForQuickBuy*)msg;
	TradeMarketRouter::Instance().OnSearchPetForQuickBuyReq(role, req);
}

void MessageHandler::OnTradeMarketGetMerchandiseNumReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketGetMerchandiseNum* req = (Protocol::CSTradeMarketGetMerchandiseNum*)msg;
	TradeMarketRouter::Instance().OnGetMerchandiseNumReq(role, req);
}

void MessageHandler::OnTradeMarketSearchMedalByEffectTypeAndColorReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTradeMarketSearchMedalByEffectTypeAndColor* req = (Protocol::CSTradeMarketSearchMedalByEffectTypeAndColor*)msg;
	TradeMarketRouter::Instance().OnSearchMedalByEffectTypeAndColorReq(role, req);
}

void MessageHandler::OnChongzhiOperateReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSChongzhiOperateReq* req = (Protocol::CSChongzhiOperateReq*)msg;

	role->GetRoleModuleManager()->GetRoleActivityManager()->OnChongzhiOperateReq(req);
}

void MessageHandler::OnQueryOthersInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSQueryRoleInfo * req = (Protocol::CSQueryRoleInfo *)msg;
	switch (req->query_type)
	{
		case Protocol::CSQueryRoleInfo::QUERY_ROLE_TYPE_DETAILED:
		{
			QueryHandler::Instance().OnQueryOther(req->role_id, role->GetUID());
		}
		break;
		case Protocol::CSQueryRoleInfo::QUERY_ROLE_TYPE_SIMPLE:
		{
			UserCacheManager::Instance().SendRoleSimpleInfo(role, req->role_id);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnFirstChongzhiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::SCRAFirstChongZhiReq * req = (Protocol::SCRAFirstChongZhiReq*) msg;
	switch (req->req_type)
	{
		case Protocol::SCRAFirstChongZhiReq::FIRST_CHONG_ZHI_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetRoleActivityManager()->SendFristChongzhiReward();
		}
		break;
		case Protocol::SCRAFirstChongZhiReq::FIRST_CHONG_ZHI_REQ_TYPE_FETCH_FIRST_REWARD:
		{
			role->GetRoleModuleManager()->GetRoleActivityManager()->GetFirstChongzhiReward(req->param1);
		}
		break;
		case Protocol::SCRAFirstChongZhiReq::FIRST_CHONG_ZHI_REQ_TYPE_FETCH_EXCLUSIVE:
		{
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnFetchExclusiveReward(req->param1);
		}
		break;
		case Protocol::SCRAFirstChongZhiReq::FIRST_CHONG_ZHI_REQ_TYPE_BUY_CMD_CHECK:
		{
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnFirstBuyItemCheck(req->param1, req->param2);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnSevenColorReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRASevenColorReq* req = (Protocol::CSRASevenColorReq*)msg;
	req->ra_seven_color_area_data[sizeof(req->ra_seven_color_area_data) - 1] = 0;
	req->reserve = 0;

	RoleActivitySevenColorPicture * role_activity = static_cast<RoleActivitySevenColorPicture *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE));
	if (NULL == role_activity)
	{
		return;
	}

	role_activity->OnRASevenColorPictureOp(req->op_type, req->index, req->seven_color_picture_pigment, req->ra_seven_color_area_data);
}

void MessageHandler::OnRandActivityBonusOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRandActivityBonusOperaReq* req = (Protocol::CSRandActivityBonusOperaReq *)msg;
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityBonusOpReq(req);
}

void MessageHandler::OnMakeUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRandActivityMakeUpReq * req = (Protocol::CSRandActivityMakeUpReq *)msg;
	role->GetRoleModuleManager()->GetRoleActivityManager()->OnRandActivityMakeUpReq(req);
}

void MessageHandler::OnRandActivityDuiHuanLingYuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU))
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	Protocol::CSRandActivityDuiHuanLingYuReq * req = (Protocol::CSRandActivityDuiHuanLingYuReq *)msg;
	const RandActivityDuiHuanLingYuConfig * config = static_cast<const RandActivityDuiHuanLingYuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU));
	if (NULL == config)
	{
		return;
	}

	config->OnSaleItem(role, req->index);

	RandActivity* act = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU);
	if (NULL != act)
	{
		act->OnRoleParticipateRA(role, __FUNCTION__);
	}
}

void MessageHandler::OnRoleSubscribeTopic(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRoleSubscribe* req = (Protocol::CSRoleSubscribe *)msg;

	if (req->is_unsubscribe > 0)
	{
		TopicManager::Instance()->OnRoleUnSubscribe(role, req->topic);
	}
	else
	{
		TopicManager::Instance()->OnRoleSubscribe(role, req->topic);
	}	
}

void MessageHandler::OnTeamSwitchPositionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTeamSwitchPosition* req = (Protocol::CSTeamSwitchPosition *)msg;
	TeamManager::Instance().OnSwitchPositionReq(role, req->pos1, req->pos2);
}

void MessageHandler::OnRoleSalaryInfoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRoleSalaryInfo* req = (Protocol::CSRoleSalaryInfo *)msg;
	role->GetRoleModuleManager()->GetSalary()->SendAllInfo(req->salary_type);
}

void MessageHandler::OnSpeakerTalk(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSpeaker * speaker = (Protocol::CSSpeaker *) msg;
	speaker->speaker_msg[MAX_SPEAKER_MSG_LENGTH - 1] = '\0';
	int use_item_id = 0;
	int offset = UNSTD_OFFSETOFF(Protocol::CSSpeaker, speaker_msg);
	if (length < offset) return;

	int world_length = length - offset;

	if (world_length < 0 || world_length >= MAX_SPEAKER_MSG_LENGTH)
	{
		return;
	}

	if (Protocol::CSSpeaker::SPEAKER_TYPE_LOCAL == speaker->speaker_type)
	{
		if (GLOBALCONFIG->GetSpeakerLevel() > role->GetLevel())
		{
			role->NoticeNum(errornum::EN_ITEM_LEVEL_LIMIT);
			return;
		}
		if (CrossConfig::Instance().IsHiddenServer())
		{
			role->NoticeNum(errornum::EN_CHAT_GROUP_CROSS_CANNOT_OPERATE);
			return;
		}
		use_item_id = LOGIC_CONFIG->GetChatConfig()->GetOtherCfg().speaker_consume_item_id;
	}
	else if (Protocol::CSSpeaker::SPEAKER_TYPE_CROSS == speaker->speaker_type)
	{
		if (GLOBALCONFIG->GetCrossSpeakerLevel() > role->GetLevel())
		{
			role->NoticeNum(errornum::EN_ITEM_LEVEL_LIMIT);
			return;
		}
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_ACTIVITY_CROSS_SERVER_NOT_OPEN);
			return;
		}

		use_item_id = LOGIC_CONFIG->GetChatConfig()->GetOtherCfg().cross_speaker_consume_item_id;
	}
	else
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(use_item_id, 1, __FUNCTION__))
	{
		return;
	}
	UNSTD_STATIC_CHECK(MAX_SPEAKER_MSG_LENGTH <= MAX_CHAT_LENGTH);
	Protocol::SCChannelChat speaker_chat;
	speaker_chat.from_uid = role->GetUID();
	role->GetName(speaker_chat.username);
	speaker_chat.avatar_type = role->GetAvatarType();
	speaker_chat.prof = role->GetProfession();
	speaker_chat.headshot_id = role->GetHeadshotID();
	speaker_chat.content_type = Protocol::CHAT_CONTENT_TYPE_TEXT;
	speaker_chat.avatar_timestamp = role->GetAvatarTimestamp();
	speaker_chat.level = role->GetLevel();
	speaker_chat.channel_type = Protocol::CSSpeaker::SPEAKER_TYPE_CROSS == speaker->speaker_type ? chatdef::CHANNEL_TYPE_COMPLEX : chatdef::CHANNEL_TYPE_WORLD;
	speaker_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	speaker_chat.face_chatframe = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
	speaker_chat.face_signature = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
	speaker_chat.msg_type = Protocol::SCChannelChat::CHANEEL_CHAT_MSG_TYPE_SPEAKER;
	speaker_chat.group_id = 0;
	speaker_chat.top_level = role->GetTopLevel();
	speaker_chat.fly_flag = role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
	Guild * guild = role->GetGuild();
	if (NULL != guild)
	{
		guild->GetGuildName(speaker_chat.guild_name);
	}
	speaker_chat.msg_length = speaker->speaker_msg_length;
	if (world_length <= static_cast<int>(sizeof(speaker_chat.content)))
	{
		memcpy(speaker_chat.content, speaker->speaker_msg, world_length);
	}

	ChatManager::Instance().OnSpeakerMsg(role, &speaker_chat);
	role->NoticeNum(noticenum::NT_SPEAK_TALK_SUCC);
}

void MessageHandler::OnMysteriousTrialReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_MYSTERIOUS_TRIAL))
		return;

	Protocol::CSMysteriousTrialReq* req = (Protocol::CSMysteriousTrialReq *)msg;
	switch (req->op_type)
	{
		case Protocol::CSMysteriousTrialReq::MYSTERIOUS_TRIAL_INFO_TYPE:
			role->GetRoleModuleManager()->GetMysteriousTrial()->SendInfo();
			break;
		case Protocol::CSMysteriousTrialReq::MYSTERIOUS_TRIAL_REWARD_TYPE:
			role->GetRoleModuleManager()->GetMysteriousTrial()->OnGetMysteriousTrialReward(req->param1, req->param2);
			break;
		case Protocol::CSMysteriousTrialReq::MYSTERIOUS_TRIAL_FINALLY_REWATD_TYPE:
			role->GetRoleModuleManager()->GetMysteriousTrial()->OnGetMysteriousTrialFinallyReward(req->param1);
			break;
	}
}

void MessageHandler::OnTransactionTaskReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTransactionTaskReq* req = (Protocol::CSTransactionTaskReq *)msg;
	role->GetRoleModuleManager()->GetTransactionTask()->TransactionTaskOp(req);
}

void MessageHandler::OnActiveSystemReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActiveSystemReq* req = (Protocol::CSActiveSystemReq *)msg;
	role->GetRoleModuleManager()->GetActiveSystem()->ActiveSystemOp(req);
}

void MessageHandler::OnHeartSkillReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_HEART_SKILL))
	{
		return;
	}

	Protocol::CSHeartSkillReq * req = (Protocol::CSHeartSkillReq *)msg;
	switch (req->op_type)
	{
		case Protocol::CSHeartSkillReq::HEART_SKILL_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetRoleHeartSkill()->SendAllInfo();
		}
		break;
		case Protocol::CSHeartSkillReq::HEART_SKILL_REQ_TYPE_LEARN_SKILL:
		{	
			role->GetRoleModuleManager()->GetRoleHeartSkill()->OnLearnHeartSkill(req->param1, req->param2);
		}
		break;
		case Protocol::CSHeartSkillReq::HEART_SKILL_REQ_TYPE_EQUIP_SKILL:
		{
			role->GetRoleModuleManager()->GetRoleHeartSkill()->OnEquipHeartSkill(req->param1, req->param2);
		}
		break;
		case Protocol::CSHeartSkillReq::HEART_SKILL_REQ_TYPE_TAKE_OFF_SKILL:
		{
			role->GetRoleModuleManager()->GetRoleHeartSkill()->OnTakeOffHeartSkill(req->param1);
		}
		break;
		case Protocol::CSHeartSkillReq::HEART_SKILL_REQ_TYPE_COMPOSE:
		{
			role->GetRoleModuleManager()->GetRoleHeartSkill()->OnCompose(req->param1);
		}
		break;
		case Protocol::CSHeartSkillReq::HEART_SKILL_REQ_TYPE_CONVERT:
		{
			role->GetRoleModuleManager()->GetRoleHeartSkill()->OnConvert(req->param1, req->param2, req->param3);
		}
		break;
		case Protocol::CSHeartSkillReq::HEART_SKILL_REQ_TYPE_UN_LOCK:
		{
			role->GetRoleModuleManager()->GetRoleHeartSkill()->OnUnLock(req->param1);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnHeartSkillDisplaceReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHeartSkillDisplaceReq * req = (Protocol::CSHeartSkillDisplaceReq *)msg;
	std::vector<HeartSkillDisplaceInfo> displace_vec;
	for (int i = 0; i < req->count && i < ARRAY_LENGTH(req->displace_list); ++i)
	{
		displace_vec.push_back(req->displace_list[i]);
	}

	role->GetRoleModuleManager()->GetRoleHeartSkill()->OnDisplace(displace_vec);
}

void MessageHandler::OnFaceOperatorReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFaceScoreRequest* req = (Protocol::CSFaceScoreRequest *)msg;

	switch (req->op_type)
	{
	case Protocol::CSFaceScoreRequest::FACE_REQ_SEND_INFO:
		role->GetRoleModuleManager()->GetFaceScore()->SendFaceAllInfo();
		break;
	case Protocol::CSFaceScoreRequest::FACE_REQ_EQUIPS_RES:
		role->GetRoleModuleManager()->GetFaceScore()->RequestEquips(req->param1);
		break;
	case Protocol::CSFaceScoreRequest::FACE_REQ_REMOVE_RES:
		role->GetRoleModuleManager()->GetFaceScore()->RequestRemove(req->param1);
		break;
	}
}

void MessageHandler::OnFuncGuideRequest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFuncGuideOperator * req = (Protocol::CSFuncGuideOperator *)msg;

	switch (req->op_type)
	{
	case Protocol::CSFuncGuideOperator::FUNC_GUIDE_CHOOSE_PET:
		role->GetRoleModuleManager()->GetFuncGuide()->ChooseNewbiePet(req->param);
		break;

	case Protocol::CSFuncGuideOperator::FUNC_GUIDE_CATCH_PET_BATTLE:
		role->GetRoleModuleManager()->GetFuncGuide()->StartCatchPetGuideBattle();
		break;

	case Protocol::CSFuncGuideOperator::FUNC_GUIDE_FINISH_CLOUD_ARENA_GUIDE:
		role->GetRoleModuleManager()->GetFuncGuide()->FinishCloudArenaGuide();
		break;

	case Protocol::CSFuncGuideOperator::FUNC_GUIDE_FINISH_LUNHUI_WANGCHUAN_GUIDE:
		role->GetRoleModuleManager()->GetFuncGuide()->FinishLunhuiWangchuanGuide();
		break;
	}
}

void MessageHandler::OnBattleSetStrategyReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleSetStrategy* req = (Protocol::CSBattleSetStrategy*)msg;

	static battlegameprotocol::GameBattleSetStrategyReq gbssr;
	gbssr.bg_netid = role->GetBGNetID();
	gbssr.uid = role->GetUID();
	gbssr.strategy_type = req->strategy_type;
	gbssr.target_grid_idx = req->target_grid_idx;

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbssr, sizeof(gbssr));
}

void MessageHandler::OnBattleClearStrategyReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleClearStrategy* req = (Protocol::CSBattleClearStrategy*)msg;

	static battlegameprotocol::GameBattleClearStrategyReq gbssr;
	gbssr.bg_netid = role->GetBGNetID();
	gbssr.uid = role->GetUID();
	gbssr.grid_idx = req->grid_idx;
	gbssr.is_clear_all = req->is_clear_all;

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbssr, sizeof(gbssr));
}

void MessageHandler::OnBattleLeaveOBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleLeaveOB* req = (Protocol::CSBattleLeaveOB*)msg;

	const RoleBattleInfo* rbi = BattleManagerLocal::GetInstance().GetRoleBattleInfoConst(role->GetUID());
	if (NULL != rbi)
	{
		static battlegameprotocol::GameBattleLeaveOBReq gbfrlbq;
		gbfrlbq.bg_netid = role->GetBGNetID();
		gbfrlbq.uid = role->GetUID();
		gbfrlbq.is_leave_team = req->is_leave_team;
		gbfrlbq.battle_id = rbi->battle_id;

		World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbfrlbq, sizeof(gbfrlbq));
	}
	else if (UTAOBShadow::Instance().IsRoleOBIng(role))
	{
		UTAOBShadow::Instance().StopOB(role);
	}
	else
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_OB_ING);
	}
}	

void MessageHandler::OnTeamMemberLaunchVote(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	//Protocol::CSTeamLaunchLeaderVote* req = (Protocol::CSTeamLaunchLeaderVote*)msg;

	Team* team = role->GetMyTeam();
	if (team != NULL)
	{
		team->OnRoleLaunchLeaderVote(role);
	}
	else
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
	}
}

void MessageHandler::OnTeamMemberVote(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSTeamVote* req = (Protocol::CSTeamVote*)msg;

	Team* team = role->GetMyTeam();
	if (team != NULL)
	{
		team->OnRoleVote(role, req->is_yes > 0);
	}
	else
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
	}
}

void MessageHandler::OnRolePauseBattleReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSBattlePause* req = (Protocol::CSBattlePause*)msg;

	static battlegameprotocol::GameBattleRolePauseBattle gbrpb;
	gbrpb.bg_netid = role->GetBGNetID();
	gbrpb.uid = role->GetUID();
	gbrpb.is_pause = req->is_pause;

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbrpb, sizeof(gbrpb));
}

void MessageHandler::OnNewChongzhiConfigInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSNewChongzhiConfigInfoReq *req = (Protocol::CSNewChongzhiConfigInfoReq*)msg;
	if (req->currency < 0 || req->currency >= CHONGZHI_CURRENCY_TYPE_MAX)
	{
		return;
	}
	req->spid_str[3] = '\0';

	std::string spid_str = req->spid_str;
	int spid = globalconfig::SpidConfig::GetSpidByStrPlatName(spid_str);

	role->SetCurrencyType(req->currency);
	role->SetClientShowSpid(spid);

	const ChongzhiReward* reward_list[CHONGZHI_REWARD_CONFIG_MAX_NUM] = { NULL };
	memset(reward_list, 0, sizeof(reward_list));
	int reward_count = 0;

	std::string plat_user_name = role->GetPlatName();

	LOGIC_CONFIG->GetChongzhiConfig()->GetNewChongzhiConfig(spid, req->currency, 
		role->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam().chong_zhi_flag, &reward_count, reward_list);

	Protocol::SCNewChongzhiConfigInfo info;
	info.info_count = 0;
	info.currency_type = req->currency;
	for (int i = 0; i < reward_count && info.info_count < CHONGZHI_REWARD_CONFIG_MAX_NUM; ++i)
	{
		if (reward_list[i] != NULL)
		{
			info.info_list[info.info_count].seq = reward_list[i]->seq;
			info.info_list[info.info_count].add_gold = reward_list[i]->chongzhi;
			info.info_list[info.info_count].extra_reward_type = reward_list[i]->reward_type;

			if (0 == reward_list[i]->reward_type || 2 == reward_list[i]->reward_type)
			{
				info.info_list[info.info_count].extra_reward = 0;
			}
			else if (1 == reward_list[i]->reward_type)
			{
				info.info_list[info.info_count].extra_reward = reward_list[i]->reward_coin;
			}
			else if (3 == reward_list[i]->reward_type)
			{
				info.info_list[info.info_count].extra_reward = reward_list[i]->reward_coin;
			}
			else if (4 == reward_list[i]->reward_type)
			{
				info.info_list[info.info_count].extra_reward = reward_list[i]->reward_coin;
			}
			else if (5 == reward_list[i]->reward_type)
			{
				//月卡
				info.info_list[info.info_count].extra_reward = 0;
			}

			info.info_list[info.info_count].reserve_sh = 0;
			info.info_list[info.info_count].money_show = reward_list[i]->money_show[req->currency];
			F_STRNCPY(info.info_list[info.info_count].description_first_chongzhi, reward_list[i]->description_first_chongzhi, sizeof(info.info_list[info.info_count].description_first_chongzhi));
			info.info_count += 1;
		}
	}

	int len = sizeof(info) - (CHONGZHI_REWARD_CONFIG_MAX_NUM - info.info_count) * sizeof(info.info_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, len);
}

void MessageHandler::OnRoleSendBattleChatReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSBattleChat* req = (Protocol::CSBattleChat*)msg;

	int chat_len = static_cast<int>(req->msg_length);
	if (chat_len <= 0 || chat_len >= REAL_TIME_COMMENT_LEN) return;
	
	if (length != (static_cast<int>(sizeof(Protocol::CSBattleChat)) - static_cast<int>(sizeof(req->msg_content)) + req->msg_length))
	{
		return;
	}

	BattleManagerLocal::GetInstance().RoleBattleRealTimeCommentReq(role, req->msg_content, chat_len);
}

void MessageHandler::OnConvertShopBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSConvertShopBuyReq* req = (Protocol::CSConvertShopBuyReq*)msg;
	role->GetRoleModuleManager()->GetRoleConvertShop()->BuyReq(req->convert_type, req->seq, req->buy_num, req->item_id);
}

void MessageHandler::OnRoleBackToOriginServerReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	role->GetRoleModuleManager()->GetRoleCross()->OnBackToOriginServerReq();
}

void MessageHandler::OnRoleTalkToNpcReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (scene != NULL && scene->GetSceneKey() == COMMON_SCENE_KEY)
	{
		role->GetRoleModuleManager()->GetTaskManager()->AdventureTaskTriggerJudge(TaskManager::ADV_TYPE_TALK);
	}
}

void MessageHandler::OnRoleLoginSystemReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSLoginSystemREQ * req = (Protocol::CSLoginSystemREQ*)msg;
	if (role != NULL)
	{
		role->GetRoleModuleManager()->GetLoginSystem()->OnGetReward(req->day);
	}
}

void MessageHandler::OnAuditBehaviorSubmitReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSAuditBehaviorSubmitReq * req = (Protocol::CSAuditBehaviorSubmitReq *)msg;
	role->GetRoleModuleManager()->OnClientBehavior(req->behavior_type);
}

void MessageHandler::OnRoleTaskPathFinding(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRoleTaskPathFinding* req = (Protocol::CSRoleTaskPathFinding*)msg;

	if (0 == req->task_id) return;

	const TaskInfomaConfig* cfg = TASKPOOL->GetTaskCfgByID(req->task_id);
	if (NULL == cfg || !cfg->is_add_robot) return;

	if (!req->is_stop) // 开始寻路，给予一个机器人到队伍里
	{
		Team* team = role->GetMyTeam();
		if (NULL == team)
		{
			team = TeamManager::Instance().CreateTeam(role);
			if (NULL == team) return;
		}

		if (team->GetLeaderUID() != role->GetUID())
		{
			return;
		}

		if (team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM)
		{
			return;
		}

		int member_count = team->GetMemberCount();
		for (int i = 0; i < member_count; ++i)
		{
			if (NULL != team->GetMemberRobotByIndex(i))
			{
				return; // 已有机器人了
			}
		}

		int partner_id = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().partner_robot_partner_id;
		const PartnerBasicCfg* partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
		if (NULL == partner_cfg)
		{
			return;
		}

		RobotParam robot_param;
		if (!ROBOTMANAGER.ConstructRobotParamByLevelAndProf(role->GetLevel(), partner_cfg->profession, ROBOT_CONFIG_USE_TYPE_ZHUZHAN, false, &robot_param, role->GetUserId().db_index))
		{
			return;
		}
		robot_param.robot_type = ROBOT_TYPE_TASK_ROBOT;
		F_STRNCPY(robot_param.robot_name, partner_cfg->name, sizeof(GameName));
		F_STRNCPY(robot_param.role_battle_data.role_character_data.name, partner_cfg->name, sizeof(GameName));
		robot_param.role_battle_data.appearance.special_appearance_type = SPECIAL_APPEARANCE_PARTNER;
		robot_param.role_battle_data.appearance.special_appearance_param = partner_id;
		robot_param.role_battle_data.role_character_data.skill_num = 2;
		robot_param.role_battle_data.role_character_data.skill_list[0].skill_id = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().partner_robot_skill_id;
		robot_param.role_battle_data.role_character_data.skill_list[0].skill_level = 1;
		robot_param.role_battle_data.role_character_data.skill_list[1].skill_id = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().partner_robot_skill_id_1;
		robot_param.role_battle_data.role_character_data.skill_list[1].skill_level = 1;
		robot_param.role_battle_data.role_stand_row = BATTLE_ROW_TYPE_BACK;
		robot_param.role_battle_data.auto_first_move_type = BATTLE_MOVE_TYPE_PERFORM_SKILL;
		robot_param.role_battle_data.auto_first_skill_id = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().partner_robot_skill_id;
		robot_param.role_battle_data.auto_first_skill_level = 1;
		robot_param.role_battle_data.auto_second_move_type = BATTLE_MOVE_TYPE_DO_NOTHING; // 阻止伙伴机器人二动
		robot_param.role_battle_data.auto_second_skill_id = 0;
		robot_param.role_battle_data.auto_second_skill_level = 0;
		robot_param.role_battle_data.is_auto = 1;
		robot_param.role_battle_data.fight_pet_idx = -1;
		robot_param.role_battle_data.role_character_data.character_type = BATTLE_CHARACTER_TYPE_PARTNER_ROBOT;

		Robot* robot = ROBOTMANAGER.CreateRobotToScene(scene, role->GetPos(), robot_param, __FUNCTION__);
		if (NULL != robot)
		{
			robot->SetSpeedRate(role->GetSpeedRate());
			team->AddRobot(robot);
		}
	}
}

void MessageHandler::OnBossChapterReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	/*
	Protocol::CSBossChapterReq* req = (Protocol::CSBossChapterReq*)msg;
	switch (req->req_type)
	{
	case Protocol::CSBossChapterReq::REQ_TYPE_CLAIM_BOSS_REWARD:
		{
			role->GetRoleModuleManager()->GetBossChapter()->ClaimReward(req->param);
		}
		break;

	case Protocol::CSBossChapterReq::REQ_TYPE_CLAIM_CHAPTER_REWARD:
		{
			role->GetRoleModuleManager()->GetBossChapter()->ClaimChapterReward(req->param);
		}
		break;

	case Protocol::CSBossChapterReq::REQ_TYPE_START_BATTLE:
		{
			role->GetRoleModuleManager()->GetBossChapter()->StartBattleReq(req->param);
		}
		break;
	}
	*/
}

void MessageHandler::OnBountyTaskTurnTableFetchRewardReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	role->GetRoleModuleManager()->GetBountyTask()->OnTurnTableRewardFetchReq();
}

void MessageHandler::OnEscapeFromCurrentPositionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Posi pos;
	int scene_id = scene->GetTownPoint(&pos);

	Posi center_pos;
	int center_scene_id = GLOBALCONFIG->GetCenterCityBornPos(center_pos);

	if (center_scene_id == scene_id)
	{
		role->ResetPos(pos.x, pos.y);
	}
	else
	{
		Scene* center_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(center_scene_id, COMMON_SCENE_KEY);
		if (NULL != center_scene)
		{
			center_scene->GetTownPoint(&center_pos);
			World::GetInstWorld()->GetSceneManager()->GoTo(role, center_scene_id, COMMON_SCENE_KEY, center_pos);
		}
		else
		{
			role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		}
	}
}

void MessageHandler::OnAutoEscapeFromCurrentPositionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Posi pos;
	scene->GetTownPoint(&pos);
	role->ResetPos(pos.x, pos.y);
}

void MessageHandler::OnSwitchPlatformSceneReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSPlatformBattlePlaformSwitch* req = (Protocol::CSPlatformBattlePlaformSwitch*)msg;
	if (scene->GetSceneType() != Scene::SCENE_TYPE_PLATFORM_BATTLE) return;

	const PlatformBattleSceneCfg * cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetSceneCfg(req->platform_seq);
	if (cfg == NULL)
	{
		return;
	}

	World::GetInstWorld()->GetSceneManager()->GoTo(role, cfg->scene_id, COMMON_FB_KEY, cfg->enter_pos);

}

void MessageHandler::OnWorldQuestionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWorldQuestionReq* req = (Protocol::CSWorldQuestionReq*)msg;

	switch (req->type)
	{
	case 0:
		WorldStatus::Instance().SendWorldAnswerInfo(role);
		break;
	default:
		break;
	}
}

void MessageHandler::OnRealtimeCommentReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRealTimeComment* req = (Protocol::CSRealTimeComment*)msg;

	unsigned int chat_len = req->msg_length;
	if (chat_len <= 0 || chat_len >= REAL_TIME_COMMENT_LEN) return;

	if (length != (sizeof(Protocol::CSRealTimeComment) - sizeof(req->msg_content) + req->msg_length))
	{
		return;
	}

	switch (req->comment_type)
	{
	case Protocol::CSRealTimeComment::COMMENT_TYPE_BATTLE:
		{
			BattleManagerLocal::GetInstance().RoleBattleRealTimeCommentReq(role, req->msg_content, chat_len);
		}
		break;
	case Protocol::CSRealTimeComment::COMMENT_TYPE_ACT_RACE_HORSE:
		{
			RoleActivityAnimalRace * role_activity = static_cast<RoleActivityAnimalRace *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
			if (NULL != role_activity)
			{
				role_activity->OnRAAnimalRaceRoleComment(req->msg_content, chat_len);
			}
		}
		break;
	case Protocol::CSRealTimeComment::COMMENT_TYPE_WEDDING_FB:
		{
			WeddingManger::Instance().SendFBBarrage(role, req->msg_content, chat_len);
		}
		break;
	default:
		return;
	}
}

void MessageHandler::OnResearchTaskAutoStartReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	if (!scene->IsInStaticScene())
	{
		return;
	}

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		return;
	}

	Team* research_team = TeamManager::Instance().GetJoinableTeamByTeamType(TEAM_TYPE_RESEARCH_TASK);
	if (NULL != research_team)
	{
		TeamManager::Instance().JoinTeam(role, research_team->GetTeamIndex(), false);
	}
	else
	{
		const TeamTypeCfg* cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(TEAM_TYPE_RESEARCH_TASK);
		if (NULL == cfg) return;

		const TeamTypeRobotCfg* team_robot_cfg = ROBOTCONFIG->GetAutoTeamRobotCfg(cfg->team_type);
		if (NULL == team_robot_cfg) return;

		TeamMatch::Instance().OnUserStartMatch(role->GetUniqueUserID(), role->GetLevel(), TEAM_TYPE_RESEARCH_TASK, role->GetGuildID());

		if (!role->GetRoleModuleManager()->GetResearchTask()->IsHaveResearchTask())
		{
			role->GetRoleModuleManager()->GetResearchTask()->OnAcceptResearchTask();
		}
	}
}

void MessageHandler::OnSpecialBattleJoyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr) return;

	Protocol::CSRAJoySeeksFavorsMatchReq* req = (Protocol::CSRAJoySeeksFavorsMatchReq*)msg;
	RandactivityJoyseeksfavors* act = static_cast<RandactivityJoyseeksfavors*>(ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS));
	if (NULL != act)
	{
		if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS))
		{
			role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
			return;
		}

		switch (req->is_match)
		{
		case Protocol::CSRAJoySeeksFavorsMatchReq::TYPE_MATCH:
			act->OnMatchReq(role);
			break;
		case Protocol::CSRAJoySeeksFavorsMatchReq::TYPE_CANCEL:
			act->OnCancelReq(role);
			break;
		}
	}
}

void MessageHandler::OnBattleJoyRoleOperateReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBattleSpecialJoyReqInfo* req = (Protocol::CSBattleSpecialJoyReqInfo*)msg;

	BattleManagerLocal::GetInstance().JoySeeksJavorsOperateReq(role, req->operate_type, req->param1, req->param2, req->param3, req->param4,req->param5);
}

void MessageHandler::OnTeamLeaderSummonMemberReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSTeamLeaderSummonMember* req = (Protocol::CSTeamLeaderSummonMember*)msg;
	TeamManager::Instance().OnSummonMemberReq(role, req->summon_role_id);
}

void MessageHandler::OnTeamMemberReplySummon(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSTeamMemberReplyLeaderSummon* req = (Protocol::CSTeamMemberReplyLeaderSummon*)msg;
	TeamManager::Instance().OnReplySummon(role, req->is_return);
}

void MessageHandler::OnGetPersonRank(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGetPersonRankList *req = (Protocol::CSGetPersonRankList *)msg;
	RankManager::Instance().OnGetPersonRankList(role, req);
}

void MessageHandler::OnGetPetRank(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGetPetRankList *req = (Protocol::CSGetPetRankList *)msg;
	RankManager::Instance().OnGetPetRankList(role, req->rank_type, req->from_index, req->to_index);
}

void MessageHandler::OnGetRankRefreshReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	RankManager::Instance().SendRefreshTimestamp(role);
}

void MessageHandler::OnGetPetRankDetailedInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGetRankPetDetailedInfo * req = (Protocol::CSGetRankPetDetailedInfo *)msg;
	RankManager::Instance().OnGetPetRankDetailedInfo(role, req->target_uid, req->rank_type, req->pet_unique_item_id);
}

void MessageHandler::OnTeamLeaderPartnerInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	team->SendLeaderBattlePartnerInfo(role);
}

void MessageHandler::OnBattleGiveupReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	BattleManagerLocal::GetInstance().BattleGiveupReq(role);
}

void MessageHandler::OnBattleSurrenderReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	BattleManagerLocal::GetInstance().BattleSurrenderReq(role);
}

void MessageHandler::OnTaskChosedReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTaskChosedReq* req = (Protocol::CSTaskChosedReq*)msg;
	switch (req->req_type)
	{
	case Protocol::CSTaskChosedReq::TASK_CHOSED_REQ_TYPE_INFO:
		role->GetRoleModuleManager()->GetTaskRecorder()->SendTaskChosedInfo();
		break;
	case Protocol::CSTaskChosedReq::TASK_CHOSED_REQ_TYPE_SET:
		role->GetRoleModuleManager()->GetTaskRecorder()->SetTaskChosed(req->param1,req->param2);
		break;
	default:
		break;
	}
}

void MessageHandler::OnLeaderCheckMemberItemReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (role == NULL)return;
	Team* role_team = role->GetMyTeam();
	if (role_team == NULL)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}

	Protocol::CSLeaderCheckMemberItem* req = (Protocol::CSLeaderCheckMemberItem*)msg;

	role_team->CheckMemberItem(role, req->item_id, req->item_num);
}

void MessageHandler::OnReqJoinTeamInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSReqJoinTeamInfo * req = (Protocol::CSReqJoinTeamInfo *)msg;
	TeamManager::Instance().ReqJoinTeamInfo(role, req->is_from_cross != 0, req->team_index, req->inviter_uid);
}

void MessageHandler::OnBattleReportSimpleInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleReportSimpleInfo* req = (Protocol::CSBattleReportSimpleInfo*)msg;
	
	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(req->monster_group_id);
	if (NULL == cfg)
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if ((0 != req->is_server_first_kill))
		{
			const ServerFirstKillCfg * first_kill_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(req->monster_group_id);
			if (NULL == first_kill_cfg)
			{
				return;
			}
			crossgameprotocal::GameHiddenVideoSimpleInfoReq ghvsir;
			ghvsir.uid = role->GetUID();
			ghvsir.monster_group_id = req->monster_group_id;
			ghvsir.is_server_first_kill = req->is_server_first_kill;
			ghvsir.param1 = req->param1;
			ghvsir.param2 = req->param2;
			ghvsir.param3 = req->param3;

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghvsir, sizeof(ghvsir));
			return;
		}
		else
		{
			if (cfg->is_cross)
			{
				crossgameprotocal::GameHiddenVideoSimpleInfoReq ghvsir;
				ghvsir.uid = role->GetUID();
				ghvsir.monster_group_id = req->monster_group_id;
				ghvsir.is_server_first_kill = req->is_server_first_kill;
				ghvsir.param1 = req->param1;
				ghvsir.param2 = req->param2;
				ghvsir.param3 = req->param3;

				InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghvsir, sizeof(ghvsir));
			}
			else
			{
				long long file_id = BattleVideoMgr::Instance().GetVideoFileID(req->monster_group_id, false);

				Protocol::SCBattleReportSimpleInfo resp;
				resp.monster_group_id = req->monster_group_id;
				resp.param1 = req->param1;
				resp.param2 = req->param2;
				resp.param3 = req->param3;
				resp.report_file_id = file_id;
				resp.is_server_first_kill = req->is_server_first_kill;

				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&resp, sizeof(resp));
			}
		}
	}
	else
	{
		if ((0 != req->is_server_first_kill))
		{
			const ServerFirstKillCfg * first_kill_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(req->monster_group_id);
			if (NULL == first_kill_cfg)
			{
				return;
			}

			long long record_file_id = BattleVideoMgr::Instance().GetVideoFileID(req->monster_group_id, true);

			Protocol::SCBattleReportSimpleInfo resp;
			resp.monster_group_id = req->monster_group_id;
			resp.param1 = req->param1;
			resp.param2 = req->param2;
			resp.param3 = req->param3;
			resp.report_file_id = record_file_id;
			resp.is_server_first_kill = req->is_server_first_kill;

			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&resp, sizeof(resp));
			return;
		}
		else
		{
			if (cfg->is_cross)
			{
				long long file_id = BattleVideoMgr::Instance().GetVideoFileID(req->monster_group_id, false);

				Protocol::SCBattleReportSimpleInfo resp;
				resp.monster_group_id = req->monster_group_id;
				resp.param1 = req->param1;
				resp.param2 = req->param2;
				resp.param3 = req->param3;
				resp.report_file_id = file_id;
				resp.is_server_first_kill = req->is_server_first_kill;

				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&resp, sizeof(resp));
			}
			else
			{
				static crossgameprotocal::HiddenGameVideoSimpleInfoReq hgvsir;
				hgvsir.is_server_first_kill = req->is_server_first_kill;
				hgvsir.monster_group_id = req->monster_group_id;
				hgvsir.param1 = req->param1;
				hgvsir.param2 = req->param2;
				hgvsir.param3 = req->param3;
				hgvsir.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		
				int plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
				int server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
				InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&hgvsir, sizeof(hgvsir));
			}
		}
	}
	
}

void MessageHandler::OnBattleReportDetailInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleReportDetailInfo* req = (Protocol::CSBattleReportDetailInfo*)msg;

	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(req->monster_group_id);
	if (NULL == cfg)
	{
		role->NoticeNum(errornum::EN_BATTLE_VIDEO_NOT_FOUND);
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if ((0 != req->is_server_first_kill))
		{
			const ServerFirstKillCfg * first_kill_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(req->monster_group_id);
			if (NULL == first_kill_cfg)
			{
				role->NoticeNum(errornum::EN_BATTLE_VIDEO_NOT_FOUND);
				return;
			}
			crossgameprotocal::GameHiddenVideoDetailInfoReq ghvdirq;
			ghvdirq.uid = role->GetUID();
			ghvdirq.monster_group_id = req->monster_group_id;
			ghvdirq.is_server_first_kill = req->is_server_first_kill;

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghvdirq, sizeof(ghvdirq));
			return;
		}
		else
		{
			if (cfg->is_cross)
			{
				crossgameprotocal::GameHiddenVideoDetailInfoReq ghvdirq;
				ghvdirq.uid = role->GetUID();
				ghvdirq.monster_group_id = req->monster_group_id;
				ghvdirq.is_server_first_kill = req->is_server_first_kill;

				InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghvdirq, sizeof(ghvdirq));
			}
			else
			{
				BattleVideoMgr::Instance().SendVideo(role, req->monster_group_id, (0 != req->is_server_first_kill));
			}
		}
	}
	else
	{
		if ((0 != req->is_server_first_kill))
		{
			const ServerFirstKillCfg * first_kill_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(req->monster_group_id);
			if (NULL == first_kill_cfg)
			{
				role->NoticeNum(errornum::EN_BATTLE_VIDEO_NOT_FOUND);
				return;
			}
			
			BattleVideoMgr::Instance().SendVideo(role, req->monster_group_id, (0 != req->is_server_first_kill));
			return;
		}
		else
		{
			if (cfg->is_cross)
			{
				BattleVideoMgr::Instance().SendVideo(role, req->monster_group_id, (0 != req->is_server_first_kill));
			}
			else
			{
				crossgameprotocal::HiddenGameVideoDetailInfoReq hgvdir;
				hgvdir.type = crossgameprotocal::VIDEO_DETAIL_INFO_REQ_TYPE_WATCH;
				hgvdir.is_server_first_kill = req->is_server_first_kill;
				hgvdir.monster_group_id = req->monster_group_id;
				hgvdir.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();

				int plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
				int server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
				InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&hgvdir, sizeof(hgvdir));
			}
		}
	}
}

void MessageHandler::OnBattleReportStopWatchingReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::SCBattleReportStopWatching resp;

	Team* team = role->GetMyTeam(false);
	if (NULL == team)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&resp, sizeof(resp));
	}
	else
	{
		if (team->GetLeaderUID() == role->GetUID())
		{
			team->SendToMember((const char*)&resp, sizeof(resp));
		}
		else
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&resp, sizeof(resp));
		}
	}
}

void MessageHandler::OnGroupLeaveReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGroupLeaveReq *req = (Protocol::CSGroupLeaveReq *)msg;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		const ChatGroupInfo * info = CrossChatGroupManager::Instance().GetGroupInfo(req->group_id);
		if (info == NULL)return;
		if (info->is_cross == 1)
		{
			CrossChatGroupManager::Instance().OnRoleLeaveGroup(role, req->group_id, true);
		}
		else
		{
			role->NoticeNum(errornum::EN_CHAT_GROUP_CROSS_CANNOT_OPERATE);
			return;
		}
	}
	else
	{
		const ChatGroupInfo * info = ChatManager::Instance().GetGroupInfo(req->group_id);
		if (info == NULL)return;
		if (info->is_cross == 1)
		{
			static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
			chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LEAVEN_GROUP;
			chat_msg.uid = role->GetUID();
			chat_msg.group_id = req->group_id;
			F_STRNCPY(chat_msg.group_master, role->GetName(), sizeof(GameName));
			chat_msg.param1 = 1;//跨服群聊标记
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));
		}
		else
		{
			ChatManager::Instance().OnRoleLeaveGroup(role, req->group_id, true);
		}
	}
}

void MessageHandler::OnGroupDismissReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGroupDissmiss *req = (Protocol::CSGroupDissmiss *)msg;
	
	if (CrossConfig::Instance().IsHiddenServer())
	{
		const ChatGroupInfo * info = CrossChatGroupManager::Instance().GetGroupInfo(req->group_id);
		if (info == NULL)return;
		if (info->is_cross == 1)
		{
			CrossChatGroupManager::Instance().OnDismissGroup(role, req->group_id);
		}
		else
		{
			role->NoticeNum(errornum::EN_CHAT_GROUP_CROSS_CANNOT_OPERATE);
			return;
		}
		
	}
	else
	{
		const ChatGroupInfo * info = ChatManager::Instance().GetGroupInfo(req->group_id);
		if (info == NULL)return;
		if (info->is_cross == 1)
		{
			static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
			chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_DISSMISS_GROUP;
			chat_msg.uid = role->GetUID();
			chat_msg.group_id = req->group_id;
			chat_msg.param1 = 1;//跨服群聊标记
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));
		}
		else
		{
			ChatManager::Instance().OnDismissGroup(role, req->group_id);
		}
	}
}

void MessageHandler::OnGroupKickMember(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGroupKickMember *req = (Protocol::CSGroupKickMember *)msg;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		const ChatGroupInfo * info = CrossChatGroupManager::Instance().GetGroupInfo(req->group_id);
		if (info == NULL)return;
		if (info->is_cross == 1)
		{
			CrossChatGroupManager::Instance().OnKickMemberFromGroup(role, req->be_kicker, req->group_id);
		}
		else
		{
			role->NoticeNum(errornum::EN_CHAT_GROUP_CROSS_CANNOT_OPERATE);
			return;
		}
	}
	else
	{
		const ChatGroupInfo * info = ChatManager::Instance().GetGroupInfo(req->group_id);
		if (info == NULL)return;
		if (info->is_cross == 1)
		{
			static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
			chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_TICKOUT_MEMBER;
			chat_msg.uid = req->be_kicker;
			chat_msg.group_id = req->group_id;
			chat_msg.inviter_id = role->GetUID();
			chat_msg.param1 = 1;//跨服群聊标记
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));
		}
		else
		{
			ChatManager::Instance().OnKickMemberFromGroup(role, req->be_kicker, req->group_id);
		}
	}
}

void MessageHandler::OnCreateGroup(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSCreateGroupReq *req = (Protocol::CSCreateGroupReq *)msg;

	int limit_level = GLOBALCONFIG->GetCreateChatGroupLevel();
	if (role->GetLevel() < limit_level)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_create_chat_group_level_limit_notice, limit_level);
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		return;
	}

	unsigned int remain_cd_minutes = 0;
	if (role->GetRoleModuleManager()->IsInCreateChatGroupCD(&remain_cd_minutes))
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_create_chat_group_cd_limit_notice, remain_cd_minutes);
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		return;
	}

	role->GetRoleModuleManager()->SetCreateChatGroupCD();

	if (CrossConfig::Instance().IsHiddenServer())
	{
		CrossChatGroupManager::Instance().OnCreateGroup(role, req->group_name);
		return;
	}
	else
	{
		if (req->is_cross)
		{
			int create_invalid_index = -1;
			bool can_create = ChatManager::Instance().CheckRoleCreateGroup(role, true, &create_invalid_index);
			if (!can_create)return;
			static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
			chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CREATE_GROUP;
			chat_msg.uid = role->GetUID();
			F_STRNCPY(chat_msg.group_name, req->group_name, sizeof(GameName));
			F_STRNCPY(chat_msg.group_master, role->GetName(), sizeof(GameName));
			chat_msg.param1 = ChatManager::Instance().GetRoleBaseAva(role);
			chat_msg.param2 = role->GetLevel();
			chat_msg.param3 = role->GetBaseProfession();
			chat_msg.param4 = create_invalid_index;
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));
		}
		else
		{
			ChatManager::Instance().OnCreateGroup(role, req->group_name, req->is_cross == 1);
		}
	}
	
}

void MessageHandler::OnEnterGroup(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSEnterGroupReq *req = (Protocol::CSEnterGroupReq *)msg;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		CrossChatGroupManager::Instance().OnRoleEnterGroup(role, req->group_id, req->invite_uid);
	}
	else
	{
		ChatManager::Instance().OnRoleEnterGroup(role, req->group_id, req->invite_uid);
	}
}

void MessageHandler::OnInviteEnterGroup(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSRoleInviteEnterGroup *req = (Protocol::CSRoleInviteEnterGroup *)msg;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		CrossChatGroupManager::Instance().OnRoleInviteGroup(role, req->be_invite_uid, req->group_id);
	}
	else
	{
		ChatManager::Instance().OnRoleInviteGroup(role, req->be_invite_uid, req->group_id);
	}
	
}

void MessageHandler::OnGetGroupList(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	ChatManager::Instance().SendRoleGroupInfo(role);
}

void MessageHandler::OnGetGroupInfo(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGetGroupInfo *req = (Protocol::CSGetGroupInfo *)msg;

	if (TEMP_CHAT_GROUP_ID == req->group_id)
	{
		Team* team = role->GetMyTeam();
		if (NULL != team)
		{
			team->OnGetTempGroupDetailInfoReq(role);
		}
	}
	else
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			CrossChatGroupManager::Instance().OnGetGroupDetailInfo(role, req->group_id);
		}
		else
		{
			ChatManager::Instance().OnGetGroupDetailInfo(role, req->group_id);
		}
	}
}

void MessageHandler::OnWildBossReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWildBossReq *req = (Protocol::CSWildBossReq *)msg;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		switch (req->req_type)
		{
		case 1:
			WildBossManager::Instance().SendInfoToRole(role);
			break;
		case 2:
			WildBossManager::Instance().SendMonsterPosiToRole(role, req->param);
		default:
			break;
		}
	}
	else
	{	
		crossgameprotocal::GameHiddenWildBossInfoReq ghwbir;
		ghwbir.uid = role->GetUID();
		ghwbir.req_type = req->req_type;
		ghwbir.param = req->param;
		ghwbir.param1 = role->GetWildBossRewardTimes();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghwbir, sizeof(ghwbir));
	}
}

void MessageHandler::OnWildBossPokerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWildBossPokerReq* req = (Protocol::CSWildBossPokerReq*)msg;
	if (req != NULL)
	{
		int is_in_scene = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterIdBySceneId(scene->GetSceneID());		// 判断这个怪是不是在场景中
		SpecialLogic *sp = role->GetScene()->GetSpecialLogic();
		if (is_in_scene != 0 && sp != NULL && sp->GetSceneType() == Scene::SCENE_TYPE_COMMON_SCENE)
		{
			SpecialLogicCommon *wild_co = (SpecialLogicCommon *) scene->GetSpecialLogic();
			if (wild_co != NULL)
			{
				wild_co->GetPoker(req->role_uid, req->index,req->req_reason, GAME_MODE_WILD_BOSS);
			}
		}
	}

}

void MessageHandler::OnTaskOneKeyFinish(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSOneKeyFinish *req = (Protocol::CSOneKeyFinish *)msg;

	switch (req->op_type)
	{
	case Protocol::CSOneKeyFinish::FINISH_TYPE_RESEARCH_TASK:
		role->GetRoleModuleManager()->GetResearchTask()->OnOneKeyFinishZhenZhong();
		break;

	case Protocol::CSOneKeyFinish::FINISH_TYPE_WST_QUESTION:
		role->GetRoleModuleManager()->GetWstQuestion()->OneKeyFinishWstQuation();
		break;

	case Protocol::CSOneKeyFinish::FINISH_TYPE_BOUNTY_TASK:
		role->GetRoleModuleManager()->GetBountyTask()->OneKeyFinishBountyTask();
		break;

	case Protocol::CSOneKeyFinish::FINISH_TYPE_FANTASY_FB:
		role->GetRoleModuleManager()->OneKeyFinishFantasyFB();
		break;

	case Protocol::CSOneKeyFinish::FINISH_TYPE_SILK_ROAD:
		role->GetRoleModuleManager()->GetTransactionTask()->OneKeyFinishTransactionTask();
		break;

	case Protocol::CSOneKeyFinish::FINISH_TYPE_JIBUDAOZEI:
		role->GetRoleModuleManager()->GetTreasureMap()->OneKeyFinishTreasureMap();
		break;
	}

}

void MessageHandler::OnSkillPositionModify(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSkillPositionModify* req = (Protocol::CSSkillPositionModify*)msg;
	if (req->skill_num <= 0 || length != static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSSkillPositionModify, common_use_skill_idx_list) + sizeof(req->common_use_skill_idx_list[0]) * req->skill_num))
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}

	role->GetRoleModuleManager()->GetRoleSkill()->OnRoleModifySkillPositionReq(req->skill_num, req->common_use_skill_idx_list);
}

void MessageHandler::OnPartnerSkillActivate(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSPartnerSkillActivate* req = (Protocol::CSPartnerSkillActivate*)msg;
	role->GetRoleModuleManager()->GetPartner()->PartnerSkillActivate(req->partner_id, req->is_activate, req->skill_idx);
}

void MessageHandler::OnPokerRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPokerReq *req = (Protocol::CSPokerReq *)msg;

	switch (req->oper_type)
	{
	case Protocol::CSPokerReq::POKER_REQ_MI_YI:
		{
			const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(scene->GetSceneID());// 判断是不是在场景中

			if (cfg != NULL)
			{
				const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(scene->GetSceneID());// 判断是不是在场景中
				SpecialLogic *sp = role->GetScene()->GetSpecialLogic();
				if (cfg != NULL && sp != NULL && sp->GetSceneType() == Scene::SCENE_TYPE_MIYI)
				{
					SpecialLogicMiYi * miyi_fb = (SpecialLogicMiYi *) scene->GetSpecialLogic();
					if (miyi_fb != NULL)
					{
						miyi_fb->GetPoker(req->role_uid, req->index);
					}
				}
			}
		}
		break;

	case Protocol::CSPokerReq::POKER_REQ_WILD_BOSS:
		{
			int is_in_scene = LOGIC_CONFIG->GetWildBossConfig()->GetMonsterIdBySceneId(scene->GetSceneID());		// 判断这个怪是不是在场景中
			SpecialLogic *sp = role->GetScene()->GetSpecialLogic();
			if (is_in_scene != 0 && sp != NULL && sp->GetSceneType() == Scene::SCENE_TYPE_COMMON_SCENE)
			{
				SpecialLogicCommon *wild_co = (SpecialLogicCommon *) scene->GetSpecialLogic();
				if (wild_co != NULL)
				{
					wild_co->GetPoker(req->role_uid, req->index, req->reason, GAME_MODE_WILD_BOSS);
				}
			}
		}
		break;

	}
}

void MessageHandler::OnGroupNoticeChangeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSChangeGroupNoticeReq *req = (Protocol::CSChangeGroupNoticeReq *)msg;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		CrossChatGroupManager::Instance().OnChangeGroupNotice(role, req->group_name, req->group_notice, req->group_id);
	}
	else
	{
		ChatManager::Instance().OnChangeGroupNotice(role, req->group_name, req->group_notice, req->group_id);
	}
}

void MessageHandler::OnHuaShenMedalReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_MEDAL, true))
	{
		return;
	}

	Protocol::CSHuaShenMedalReq *req = (Protocol::CSHuaShenMedalReq*)msg;

	switch (req->req_type)
	{
	case Protocol::CSHuaShenMedalReq::HUASHEN_MEDAL_REQ_TYPE_INFO:
	{
		role->GetRoleModuleManager()->GetHuaShenManager()->SendMedalInfo();
	}
	break;
	case Protocol::CSHuaShenMedalReq::HUASHEN_MEDAL_REQ_TYPE_PUT_ON:
	{
		role->GetRoleModuleManager()->GetHuaShenManager()->MedalPutOn(req->param1, req->param2);
	}
	break;
	case Protocol::CSHuaShenMedalReq::HUASHEN_MEDAL_REQ_TYPE_TAKE_OFF:
	{
		role->GetRoleModuleManager()->GetHuaShenManager()->MedalTakeOff(req->param1);
	}
	break;
	default:
		break;
	}
}

void MessageHandler::OnHuaShenEquipmentReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHuaShenEquipmentOperaReq * req = (Protocol::CSHuaShenEquipmentOperaReq*)msg;
	switch (req->msg_type)
	{
		case Protocol::CSHuaShenEquipmentOperaReq::OPERA_TYPE_INFO_REQ:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->SendHuaShenEquipmentInfo();
		}
		break;
		case Protocol::CSHuaShenEquipmentOperaReq::OPERA_TYPE_PUTON:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->PutOn(req->param1, req->param2, req->param3, req->param4);
		}
		break;
		case Protocol::CSHuaShenEquipmentOperaReq::OPERA_TYPE_TAKEOFF:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->TakeOff(req->param1, false, req->param2);
		}
		break;
		case Protocol::CSHuaShenEquipmentOperaReq::OPERA_TYPE_TAKEOFF_CRYSTAL:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->HuaShenTakeOffElementCrystal();
		}
		break;
		case Protocol::CSHuaShenEquipmentOperaReq::OPERA_TYPE_PUTON_CRYSTAL:
		{
			role->GetRoleModuleManager()->GetEquipmentManager()->HuaShenPutOnElementCrystal(req->param1);
		}
		break;
		case Protocol::CSHuaShenEquipmentOperaReq::OPERA_TYPE_INORDER:
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->InOrder();
		}
		break;
	}


}

void MessageHandler::OnHuaShenJewelryHandleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_LINGSHI, true))
	{
		return;
	}

	Protocol::CSHuaShenJewelryHandleReq * jhr = (Protocol::CSHuaShenJewelryHandleReq *)msg;
	switch (jhr->op_type)
	{
	case Protocol::CSHuaShenJewelryHandleReq::OPERA_TYPE_ALL_INFO_REQ:
		role->GetRoleModuleManager()->GetEquipmentManager()->SendHuaShenAllJewelryInfo();
		break;

	case Protocol::CSHuaShenJewelryHandleReq::OPERA_TYPE_WEAR_JEWELRY:
		role->GetRoleModuleManager()->GetEquipmentManager()->HuaShenWearJewelry(jhr->param);
		break;

	case Protocol::CSHuaShenJewelryHandleReq::OPERA_TYPE_TAKE_OFF_ONE:
		role->GetRoleModuleManager()->GetEquipmentManager()->HuaShenTakeOffJewelry(jhr->param);
		break;

	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}


}

void MessageHandler::OnHuaShenAttrPlanPonitReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHuaShenAttributePlanPointReq * req = (Protocol::CSHuaShenAttributePlanPointReq *)msg;
	if(req->cur_plan_index < 0 || req->cur_plan_index >= MAX_HUASHEN_ATTR_PLAN_NUM) return;

	switch (req->op_type)
	{
		case Protocol::CSHuaShenAttributePlanPointReq::ATTRIBUTE_PLAN_TYPE_CLEAR_POINT:
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->ClearAttributePointReq(req->cur_plan_index);
		}
		break;
		case Protocol::CSHuaShenAttributePlanPointReq::ATTRIBUTE_PLAN_TYPE_ADD_POINT:
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->AddAttributePointReq(req->delta_attr_list[ADD_POINT_TYPE_VITALITY],
				req->delta_attr_list[ADD_POINT_TYPE_STRENGTH], req->delta_attr_list[ADD_POINT_TYPE_POWER],
				req->delta_attr_list[ADD_POINT_TYPE_AGILITY], req->delta_attr_list[ADD_POINT_TYPE_MAGIC], req->cur_plan_index);
		}
		break;
		case Protocol::CSHuaShenAttributePlanPointReq::ATTRIBUTE_PLAN_TYPE_SWITCH:
		{
			role->GetRoleModuleManager()->GetHuaShenManager()->SwitchAttrPlan(req->cur_plan_index);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnHuaShenPetReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PET, true))
	{
		return;
	}
	Protocol::CSHuaShenPetOp * req = (Protocol::CSHuaShenPetOp *)msg;
	bool ret = true;
	switch (req->op_type)
	{
		case Protocol::CSHuaShenPetOp::SetPetState:
		{
			ret = role->GetRoleModuleManager()->GetHuaShenManager()->SetPetState(req->p1, req->p2);
		}
		break;
		case Protocol::CSHuaShenPetOp::RefreshPoints:
		{
			ret = role->GetRoleModuleManager()->GetHuaShenManager()->OnRefreshPoints(req->p1);
		}
		break;
		case Protocol::CSHuaShenPetOp::PetAttrsReq:
		{
			ret = role->GetRoleModuleManager()->GetHuaShenManager()->SendPetAttrInfo(req->p1);
		}
		break;
		case Protocol::CSHuaShenPetOp::PetEquipmentPutOn:
		{
			ret = role->GetRoleModuleManager()->GetHuaShenManager()->PutOnPetEquipment(req->p1, req->p2);
		}
		break;
		case Protocol::CSHuaShenPetOp::PetEquipmentPutOff:
		{
			ret = role->GetRoleModuleManager()->GetHuaShenManager()->PutOffPetEquipment(req->p1);
		}
		break;
	default:
		break;
	}
	if (!ret)
	{
		
	}

}

void MessageHandler::OnHuaShenPetAssignAttrPointReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PET, true))
	{
		return;
	}

	Protocol::CSHuaShenPetAssignAttributePoints * req = (Protocol::CSHuaShenPetAssignAttributePoints *)msg;
	role->GetRoleModuleManager()->GetHuaShenManager()->AssignAttributePoints(req->index, req->con, req->str, req->def, req->agi, req->magic);
}

void MessageHandler::OnHuaShenInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHuaShenInfoReq * req = (Protocol::CSHuaShenInfoReq *)msg;
	role->GetRoleModuleManager()->GetHuaShenManager()->OnInfoReq(*req);
}

void MessageHandler::OnHuaShenSetBattlePositionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSetHuaShenBattlePosition * req = (Protocol::CSSetHuaShenBattlePosition *)msg;
	role->GetRoleModuleManager()->GetHuaShenManager()->OnSetBattlePosition(req->position ? 1 : 0);
}

void MessageHandler::OnHuaShenAttrPlanChangeNameReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHuaShenAttributePlanChangeName *req = (Protocol::CSHuaShenAttributePlanChangeName *)msg;
	req->plan_name[ATTRIBUTE_PLAN_NAME_LENGTH - 1] = 0;
	role->GetRoleModuleManager()->GetHuaShenManager()->AttrPlanChangeName(req->plan_index, req->plan_name);
}

void MessageHandler::OnStartNpcDemoBattleReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleStartNpcDemoFight* req = (Protocol::CSBattleStartNpcDemoFight*)msg;
	
	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	BattleData_Business business;
	business.Reset();

	int battle_mode = BATTLE_MODE_DEMO;
	if (1 == req->is_initial_fight)
	{
		battle_mode = BATTLE_MODE_INITIAL_FIGHT;
		business.role_runaway_always_fail = 1;
	}
	BattleManagerLocal::GetInstance().StartDemoFightReq(role, req->npc_demo_seq, battle_mode, &business);
}

void MessageHandler::OnRoleRejoinBattleSceneReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSRoleRejoinBattleScene* req = (Protocol::CSRoleRejoinBattleScene*)msg;
	bool is_connected = BattleManagerLocal::GetInstance().IsBattleServerConnected(req->battle_server_idx);
	if (!is_connected)
	{
		role->NoticeNum(errornum::EN_BATTLE_SERVER_NOT_CONNECT);
		gamelog::g_log_battle.printf(LL_ERROR, "OnRoleRejoinBattleSceneReq battle_not_connected! role[%d,%s] battle_server[%d] battle_id[%d]",
			role->GetUID(), role->GetName(), req->battle_server_idx, req->battle_id);
	}

	if (NULL == BattleManagerLocal::GetInstance().GetRoleBattleInfoConst(role->GetUID()))
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_ROLE_BATTLE_INFO_NOT_FOUND);
#endif
		gamelog::g_log_battle.printf(LL_ERROR, "OnRoleRejoinBattleSceneReq rolebattleinfo not found!! role[%d,%s] battle_server[%d] battle_id[%d]",
			role->GetUID(), role->GetName(), req->battle_server_idx, req->battle_id);
	}

	if (NULL == BattleManagerLocal::GetInstance().GetBattleLocalInfoConst(req->battle_id))
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_BATTLE_LOCAL_INFO_NOT_FOUND);
#endif
		gamelog::g_log_battle.printf(LL_ERROR, "OnRoleRejoinBattleSceneReq localbattleinfo not found!! role[%d,%s] battle_server[%d] battle_id[%d]",
			role->GetUID(), role->GetName(), req->battle_server_idx, req->battle_id);
	}

	battlegameprotocol::GameBattleRoleRejoinBattleScene gbrrbs;
	gbrrbs.uid = role->GetUID();
	gbrrbs.bg_netid = role->GetBGNetID();
	gbrrbs.battle_id = req->battle_id;
	F_STRNCPY(gbrrbs.role_name, role->GetName(), sizeof(gbrrbs.role_name));

	World::GetInstWorld()->SendMsgToOneBattle(req->battle_server_idx, (const char*)&gbrrbs, sizeof(gbrrbs));
}

void MessageHandler::OnMiYiPokerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMiYiPokerReq* req = (Protocol::CSMiYiPokerReq*)msg;
	const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(scene->GetSceneID());// 判断是不是在场景中

	if (cfg != NULL)
	{
		const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(scene->GetSceneID());// 判断是不是在场景中
		SpecialLogic *sp = role->GetScene()->GetSpecialLogic();
		if (cfg != NULL && sp != NULL && sp->GetSceneType() == Scene::SCENE_TYPE_MIYI)
		{
			SpecialLogicMiYi * miyi_fb = (SpecialLogicMiYi *) scene->GetSpecialLogic();
			if (miyi_fb != NULL)
			{
				miyi_fb->GetPoker(req->role_uid, req->index);
			}
		}
	}
	
}

void MessageHandler::OnMiYiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMiYiReq * req = (Protocol::CSMiYiReq *)msg;
	switch (req->req_type)
	{
		case  Protocol::MI_YI_REQ_TYPE_FETCH_WEEK_KILL_REWARD:
			{
				role->GetRoleModuleManager()->GetMiYi()->FetchWeekFirstKillReward(req->param1, req->param2);
			}
			break;
		case  Protocol::MI_YI_REQ_TYPE_SAO_DANG:
			{
				role->GetRoleModuleManager()->GetMiYi()->SaoDang(req->param1);
			}
			break;
		default:
			break;
	}	
}

void MessageHandler::OnMedalTaskPlayVideoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSMedalTaskPlayVideo* req = (Protocol::CSMedalTaskPlayVideo*)msg;

	role->GetRoleModuleManager()->GetTaskManager()->OnPlayMedalTaskVideo(req->video_id);
}

void MessageHandler::OnPetEquipmentReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetEquipMentReq* req = (Protocol::CSPetEquipMentReq*)msg;
	switch (req->req_type)
	{
	case Protocol::CSPetEquipMentReq::REQ_TYPR_REFRESH:
		switch (req->param1)
		{
		case 1://洗练背包里的
			role->GetRoleModuleManager()->GetWorkshop()->RefreshPetEquipMent(req->param2, req->param3);
			break;
		case 2://洗练宠物身上的
			role->GetRoleModuleManager()->GetWorkshop()->RefreshPetEquipMentInPet(req->param2, req->param3);
			break;
		}
		break;
	case Protocol::CSPetEquipMentReq::REQ_TYPR_COMPOSE:
		role->GetRoleModuleManager()->GetWorkshop()->CompoundPetEquipMent(req->param1, req->param2, req->param3);
		break;
	case Protocol::CSPetEquipMentReq::REQ_TYPR_SAVE_REFRESH:
		switch (req->param1)
		{
		case 1://洗练背包里的
			role->GetRoleModuleManager()->GetWorkshop()->SaveRefreshPetEquipment(req->param2);
			break;
		case 2://洗练宠物身上的
			role->GetRoleModuleManager()->GetWorkshop()->SaveRefreshPetEquipmentInPet(req->param2);
			break;
		}
		break;
	case Protocol::CSPetEquipMentReq::REQ_TYPR_BREAK:
		{
			role->GetRoleModuleManager()->GetWorkshop()->BreakPetEquipMent(req->param1);
		}
		break;
	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

void MessageHandler::OnSetAutoCatchPetReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSetAutoCatchPet* req = (Protocol::CSSetAutoCatchPet*)msg;

	if (length < static_cast<int>(sizeof(Protocol::CSSetAutoCatchPet)) - static_cast<int>(sizeof(req->setting_list)))
	{
		return;
	}

	if (length > static_cast<int>(sizeof(Protocol::CSSetAutoCatchPet)))
	{
		return;
	}

	if (length != static_cast<int>(sizeof(Protocol::CSSetAutoCatchPet)) - static_cast<int>(sizeof(req->setting_list))
		+ req->auto_catch_info_num * sizeof(req->setting_list[0]))
	{
		return;
	}

	Protocol::CSSetAutoCatchPet real_req;
	real_req.scene_id = req->scene_id;
	real_req.auto_catch_info_num = req->auto_catch_info_num;
	real_req.max_seal_card_common = req->max_seal_card_common;
	real_req.max_seal_card_silver = req->max_seal_card_silver;
	real_req.max_seal_card_gold = req->max_seal_card_gold;
	for (int i = 0; i < req->auto_catch_info_num; ++i)
	{
		real_req.setting_list[i] = req->setting_list[i];
	}

	role->GetRoleModuleManager()->GetAutoCatchPet()->SetAutoCatchPetInfo(&real_req);
}

void MessageHandler::OnFormationSimpleInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	role->GetRoleModuleManager()->GetRoleFormation()->SendSimpleInfo();
}

void MessageHandler::OnFormationDetailInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSFormationDetailInfo* req = (Protocol::CSFormationDetailInfo*)msg;
	role->GetRoleModuleManager()->GetRoleFormation()->FormationDetailInfoReq(req->formation_seq);
}

void MessageHandler::OnFormationRenameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSFormationRename* req = (Protocol::CSFormationRename*)msg;
	req->new_name[sizeof(GameName) - 1] = 0;

	role->GetRoleModuleManager()->GetRoleFormation()->RenameFormationReq(req->formation_seq, req->new_name);
}

void MessageHandler::OnFormationSaveReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSFormationSave* req = (Protocol::CSFormationSave*)msg;

	if (length < sizeof(Protocol::CSFormationSave) - sizeof(req->member_list))
	{
		return;
	}

	if (length > sizeof(Protocol::CSFormationSave))
	{
		return;
	}

	if (length != sizeof(Protocol::CSFormationSave) - sizeof(req->member_list)
		+ req->team_member_num * sizeof(req->member_list[0]))
	{
		return;
	}

	Team* team = role->GetMyTeam();
	bool is_leader = false;
	if (NULL != team && team->GetLeaderUID() == role->GetUID())
	{
		is_leader = true;
	}

	if (req->is_save_new_format > 0 && is_leader)
	{
		role->NoticeNum(errornum::EN_NEW_FORMATION_LIMIT_WITH_LEADER);
		return;
	}

	Protocol::CSFormationSave real_req;
	real_req.is_save_new_format = req->is_save_new_format;
	real_req.formation_seq = req->formation_seq;
	real_req.role_row = req->role_row;
	real_req.pet_idx = req->pet_idx;
	real_req.partner_num = req->partner_num;
	real_req.team_member_num = req->team_member_num;
	for (int i = 0; i < req->partner_num; ++i)
	{
		real_req.partner_list[i] = req->partner_list[i];
	}
	for (int i = 0; i < req->team_member_num; ++i)
	{
		real_req.member_list[i] = req->member_list[i];

		int battle_pos = real_req.member_list[i].member_pos;
		if (battle_pos == 2 || battle_pos < 0 || battle_pos >= ARRAY_LENGTH(REVERSE_BATTLE_POS_RELATION))
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		int team_member_pos = REVERSE_BATTLE_POS_RELATION[battle_pos];
		real_req.member_list[i].member_pos = team_member_pos;
	}
	real_req.auto_info = req->auto_info;

	if (!role->GetRoleModuleManager()->GetRoleFormation()->SaveFormationReq(&real_req))
	{
		return;
	}

	if (NULL != team && is_leader)
	{
		team->SaveFormationReq(&real_req);
	}
}

void MessageHandler::OnFormationDeleteReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSFormationDelete* req = (Protocol::CSFormationDelete*)msg;

	role->GetRoleModuleManager()->GetRoleFormation()->DeleteFormationReq(req->formation_seq);
}

void MessageHandler::OnSetPetHelperFormationReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSetPetHelperFormation* req = (Protocol::CSSetPetHelperFormation*)msg;

	role->GetRoleModuleManager()->GetPetHelperFormation()->OnRoleSetFormation(req);
}

void MessageHandler::OnQueryPetHelperFormationReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSPetHelperFormationQuery* req = (Protocol::CSPetHelperFormationQuery*)msg;

	role->GetRoleModuleManager()->GetPetHelperFormation()->OnRoleQueryFormation(req->system_type);
}

void MessageHandler::OnMysteryStoreReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::SCMysteryStoreReq* req = (Protocol::SCMysteryStoreReq*)msg;

	switch (req->req_type)
	{
	case Protocol::SCMysteryStoreReq::REQ_REFRESH:
		role->GetRoleModuleManager()->GetPartner()->OnStoreRefresh(false);
		break;
	case Protocol::SCMysteryStoreReq::REQ_BUY:
		role->GetRoleModuleManager()->GetPartner()->OnStoreBuy(req->param);
		break;
	case Protocol::SCMysteryStoreReq::REQ_INFO:
		role->GetRoleModuleManager()->GetPartner()->SendStoreInfo();
		break;
	default:
		break;
	}
}

void MessageHandler::OnGuildInbornReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGuildInbornReq* req = (Protocol::CSGuildInbornReq*)msg;

	switch (req->req_type)
	{
		case Protocol::CSGuildInbornReq::GUILD_INBORN_REQ_TYPE_ALL_INFO:
		{
			role->GetRoleModuleManager()->GetRoleGuildInborn()->SendAllInfo();
		}
		break;
		case Protocol::CSGuildInbornReq::GUILD_INBORN_REQ_TYPE_LEARN_SKILL:
		{
			role->GetRoleModuleManager()->GetRoleGuildInborn()->OnLearnSkill(req->param1, req->param2);
		}
		break;
		case Protocol::CSGuildInbornReq::GUILD_INBORN_REQ_TYPE_RESET_SKILL:
		{
			role->GetRoleModuleManager()->GetRoleGuildInborn()->OnResetSkill(req->param1, req->param2);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnShanHaiBossReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSShanHaiBossReq* req = (Protocol::CSShanHaiBossReq*)msg;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		switch (req->req_type)
		{
		case Protocol::CSShanHaiBossReq::REQ_TYPE_BOSS_INFO:
			ShanHaiBossManager::Instance().SendBossInfo(role);
			break;

		case Protocol::CSShanHaiBossReq::REQ_TYPE_ROLE_INFO:
			ShanHaiBossManager::Instance().SendRoleInfo(role);
			break;

		default:
			break;
		}
	}
	else
	{
		switch (req->req_type)
		{
		case Protocol::CSShanHaiBossReq::REQ_TYPE_BOSS_INFO:
		case Protocol::CSShanHaiBossReq::REQ_TYPE_ROLE_INFO:
			{
				crossgameprotocal::GameHiddenShanHaiBossInfoReq ghshbir;
				ghshbir.uid = role->GetUID();
				ghshbir.req_type = req->req_type;
				ghshbir.param = req->param;
				ghshbir.param1 = role->GetRoleModuleManager()->GetShanHaiBossChallengeTime();

				InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghshbir, sizeof(ghshbir));
			}
			break;

		default:
			break;
		}
	}
}

void MessageHandler::OnGetCurPosReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::SCGetCurPos resp;
	resp.scene_id = role->GetScene()->GetSceneID();
	resp.pos_x = role->GetPos().x;
	resp.pos_y = role->GetPos().y;

	EngineAdapter::Instance().NetSend(netid, &resp, sizeof(resp));
}

void MessageHandler::OnCrysalFbFastPassReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCrystalFbFastPassInfo *info = (Protocol::CSCrystalFbFastPassInfo*)msg;

	role->GetRoleModuleManager()->GetRoleCrystalFb()->UseFastPassFb(info->fb_seq, info->times);
}

void MessageHandler::OnChatSimpleRecordReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSChatSimpleRecordSet *info = (Protocol::CSChatSimpleRecordSet*)msg;
	ChatManager::Instance().SetSimpleRecord(role, info->index, info->simple_data);
}

void MessageHandler::OnRoleBattleUndoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	static battlegameprotocol::GameBattleRoleUndoReq gbrpb;
	gbrpb.bg_netid = role->GetBGNetID();
	gbrpb.uid = role->GetUID();

	World::GetInstWorld()->SendMsgToOneBattle(role->GetBattleIndex(), (const char*)&gbrpb, sizeof(gbrpb));
}

void MessageHandler::OnWenXiangLouReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (role == NULL)return;
	
	if (scene->GetSceneType() != Scene::SCENE_TYPE_WEN_XIANG_LOU)return;
	
	SpecialLogic * special_scene = scene->GetSpecialLogic();
	if (special_scene == NULL)return;

	SpecialLogicWenXiangLouFb* wenxianglou_fb = (SpecialLogicWenXiangLouFb*)special_scene;

	Protocol::CSWenXiangLouReq *req = (Protocol::CSWenXiangLouReq*)msg;
	
	switch (req->req_type)
	{
	case Protocol::CSWenXiangLouReq::REQ_TYPE_TALK:
		wenxianglou_fb->TalkToNpc(role, req->param1);
		break;
	case Protocol::CSWenXiangLouReq::REQ_TYPE_FIND_ITEM:
		wenxianglou_fb->findItem(role, req->param1);
		break;
	case Protocol::CSWenXiangLouReq::REQ_TYPE_IDENTITY:
		wenxianglou_fb->IdentificatinNpc(role, req->param1);
		break;
	case Protocol::CSWenXiangLouReq::REQ_TYPE_FIGHT:
		wenxianglou_fb->Finalfight(role);
		break;
	case Protocol::CSWenXiangLouReq::REQ_TYPE_NPC_INFO:
		wenxianglou_fb->SendNPCInfo(role);
		break;
	case Protocol::CSWenXiangLouReq::REQ_TYPE_SCENE_INFO:
		wenxianglou_fb->SendSceneInfo(role);
		break;
	default:
		break;
	}
}

void MessageHandler::OnNewTaskActionReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	// 未有行为监控前，直接调用奇闻任务的行为

	Protocol::CSNewTaskAction* req = (Protocol::CSNewTaskAction*)msg;
	switch (req->action_type)
	{
	case NEW_TASK_ACTION_TALK_TO_NPC:
		{
			int npc_id = req->param1;
			role->GetRoleModuleManager()->GetAnecdoteTask()->OnTalkToNpc(npc_id);
		}
		break;

	case NEW_TASK_ACTION_COMMIT_ITEM:
		{
			int task_id = req->param3;
			int item_id = req->param1;
			int num = req->param2;
			role->GetRoleModuleManager()->GetAnecdoteTask()->OnCommitItem(task_id, item_id, num);
		}
		break;

	case NEW_TASK_ACTION_ANECDOTE_SUCC:
		{
			int anecdote_type = req->param1;
			role->GetRoleModuleManager()->GetAnecdoteTask()->OnChallengeAnecdoteSucc(anecdote_type);
		}
		break;
	}
}

void MessageHandler::OnNewTaskGiveNPCItems(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSNewTaskGiveNPCTaskNeedItems * gntni = (Protocol::CSNewTaskGiveNPCTaskNeedItems *)msg;
	if (gntni->list_count <= 0) return;

	if (length != static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSNewTaskGiveNPCTaskNeedItems, commit_list) + sizeof(gntni->commit_list[0]) * gntni->list_count))
	{
#ifdef _DEBUG
		role->NoticeNum(errornum::EN_REQ_PARAM_NOT_LONG_ENOUGH);
		gamelog::g_log_debug.printf(LL_INFO, "PARAM_NOT_LONG_ENOUGH  func[%s] length[%d]", __FUNCTION__, length);
#endif
		return;
	}

	const TaskInfomaConfig * p_tic = TASKPOOL->GetTaskCfgByID(gntni->task_id);
	if (NULL == p_tic)
	{
		role->NoticeNum(errornum::EN_TASK_NOLONGER_EXISTED);
		return;
	}

	switch (p_tic->task_condition)
	{
	case TASK_NEED_GIVE_PROGROUP:
		{
			role->GetRoleModuleManager()->GetAnecdoteTask()->OnCommitItemGroup(gntni->task_id, gntni->list_count, gntni->commit_list);
		}
		break;

	case TASK_NEED_GIVE_PROPERTY:
		{
			role->GetRoleModuleManager()->GetAnecdoteTask()->OnCommitItem(gntni->task_id, gntni->commit_list[0].item_id, gntni->commit_list[0].com_num);
		}
		break;
	default:
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

void MessageHandler::OnAnecdoteTaskOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSAnecdoteTaskOperate* req = (Protocol::CSAnecdoteTaskOperate*)msg;
	switch (req->operate_type)
	{
	case Protocol::CSAnecdoteTaskOperate::OPERATE_TYPE_ACCEPT_TASK:
		{
			role->GetRoleModuleManager()->GetAnecdoteTask()->AcceptTask(req->param1);
		}
		break;

	case Protocol::CSAnecdoteTaskOperate::OPERATE_TYPE_COMMIT_TASK:
		{
			role->GetRoleModuleManager()->GetAnecdoteTask()->CommitTask(req->param1);
		}
		break;

	case Protocol::CSAnecdoteTaskOperate::OPERATE_TYPE_CLAIM_REWARD:
		{
			role->GetRoleModuleManager()->GetAnecdoteTask()->OnRoleClaimReward(req->param1);
		}
		break;

	case Protocol::CSAnecdoteTaskOperate::OPERATE_TYPE_FETCH_BIG_EVENT_REWARD:
		{
			role->GetRoleModuleManager()->GetAnecdoteTask()->OnRoleFetchBigEventReward(req->param1);
		}
		break;
	}
}

void MessageHandler::OnAnecdoteTaskSpecialReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSAnecdoteTaskSpecialReq* req = (Protocol::CSAnecdoteTaskSpecialReq*)msg;
	role->GetRoleModuleManager()->GetAnecdoteTask()->SpecialSet(req->req_type, req->req_value);
}

void MessageHandler::OnNormalTaskFinishAnedoteGameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSNormalTaskFinishAnedoteGame* req = (Protocol::CSNormalTaskFinishAnedoteGame*)msg;
	role->GetRoleModuleManager()->GetTaskManager()->OnFinishAnedoteGame(req->anedote_type);
}

void MessageHandler::OnSpeciallogicCatchFoxReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCatchFoxSpecialLogicReq* req = (Protocol::CSCatchFoxSpecialLogicReq*)msg;
	Scene* role_scene = role->GetScene();
	if (role_scene != NULL)
	{
		SpecialLogic * role_sepciallogic = role_scene->GetSpecialLogic();
		if (role_sepciallogic != NULL)
		{
			if (Scene::SCENE_TYPE_CATCH_FOX == role_sepciallogic->GetSceneType())
			{
				SpecialLogicCatchFoxFb * special_logic = (SpecialLogicCatchFoxFb *)role_sepciallogic;
				special_logic->OnCatchFox(req->x, req->y, req->dir);
			}
		}
	}
}

void MessageHandler::OnDrawManagerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSDrawReq* req = (Protocol::CSDrawReq *)msg;
	DrawManager::Instance().OnReq(role, req->type, req->param_1, req->param_2);
}

void MessageHandler::OnLevelComplementReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	/*Protocol::CSLevelComplementReq* req = (Protocol::CSLevelComplementReq *)msg;
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_LEVEL_COMPLEMENT, true))return;
	switch (req->type)
	{
	case Protocol::CSLevelComplementReq::LEVEL_COMPLEMENT_TYPE_0:
		role->GetRoleModuleManager()->GetLevelComplement()->SendInfo();
		break;
	case Protocol::CSLevelComplementReq::LEVEL_COMPLEMENT_TYPE_1:
		role->GetRoleModuleManager()->GetLevelComplement()->BuyExp();
		break;
	default:
		break;
	}*/
}

void MessageHandler::OnNewLevelComplementReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRoleNewLevelComplementReq* req = (Protocol::CSRoleNewLevelComplementReq *)msg;
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_NEW_LEVEL_COMPLEMENT, false))return;

	switch (req->req_type)
	{
		case Protocol::CSRoleNewLevelComplementReq::REQ_TYPE_INFO:
		role->GetRoleModuleManager()->GetRoleNewLevelComplement()->SendInfo();
		break;
		case Protocol::CSRoleNewLevelComplementReq::REQ_TYPE_BUY_EXP:
		role->GetRoleModuleManager()->GetRoleNewLevelComplement()->OnBuyExp();
		break;
		default:
		break;
	}
}

void MessageHandler::OnDreamNotesChallengeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSDreamNotesChallengeOp* req = (Protocol::CSDreamNotesChallengeOp*)msg;
	switch (req->op_type)
	{
	case Protocol::CSDreamNotesChallengeOp::DREAM_NOTES_CHALLENGE_OP_TPYE_START_CHALLENGE:
		{
			FBManager::Instance().EnterFB(role, FBManager::FB_TYPE_DREAM_NOTES_CHALLENGE, req->param1, 0);
		}
		break;
	case Protocol::CSDreamNotesChallengeOp::DREAM_NOTES_CHALLENGE_OP_TPYE_FAST_PASS:
		{
			role->GetRoleModuleManager()->GetDreamNotesChallenge()->OnFastPassFb(req->param1);
		}
		break;
	case Protocol::CSDreamNotesChallengeOp::DREAM_NOTES_CHALLENGE_OP_TPYE_GO_ON_CHALLENGE:
		{
			SpecialLogic * special_lobgic = scene->GetSpecialLogic();
			if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_DREAM_NOTES_CHALLENGE)
			{
				return;
			}
			SpecialLogicDreamNotesChallengeFb * dream_notes_challenge_scene = (SpecialLogicDreamNotesChallengeFb *)special_lobgic;
			if (NULL == dream_notes_challenge_scene) return;

			dream_notes_challenge_scene->RefreshScene(role, req->param1);
		}
		break;
	case Protocol::CSDreamNotesChallengeOp::DREAM_NOTES_CHALLENGE_OP_TYPE_BUY_TIMES:
		{
			role->GetRoleModuleManager()->GetDreamNotesChallenge()->OnBuyTimes(req->param1);
		}
		break;
	default:
		break;
	}

}

void MessageHandler::OnSchoolExerciseChallengeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSSchoolExerciseChallenge* req = (Protocol::CSSchoolExerciseChallenge*)msg;
	
	if (!role->GetRoleModuleManager()->GetSchoolExercise()->Challenge(req->seq))
	{
		gamelog::g_log_school_exercise.printf(LL_WARNING, "OnSchoolExerciseChallengeReq challenge return false! role[%d,%s] seq[%d]", role->GetUID(), role->GetName(), req->seq);
	}
}

void MessageHandler::OnSchoolExerciseFirstOpenUI(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	role->GetRoleModuleManager()->GetSchoolExercise()->FirstOpenUI();
}

void MessageHandler::OnWorkShopDecomposeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWorkshopDecomposeReq *req = (Protocol::CSWorkshopDecomposeReq *)msg;

	if (req->grid_num > 0)
	{
		role->GetRoleModuleManager()->GetWorkshop()->DeComposeItem(req->grid_num, req->decompose_grid);
	}
}

void MessageHandler::OnTempChatGroupBecomeOfficialReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSTempChatGroupBecomeOfficialReq* req = (Protocol::CSTempChatGroupBecomeOfficialReq*)msg;

	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}

	if (NULL == team->GetMemberInfoByUID(role->GetUID()))
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}

	team->TempGroupBecomeOfficialReq(role, req->group_name);
}

void MessageHandler::OnTeamLeaderCloseWindosReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCloseWindosReq * req = (Protocol::CSCloseWindosReq *)msg;
	//队长窗口控制转发给队员
	if (role != NULL)
	{
		if (role->InTeam())
		{
			Protocol::SCCloseWindosRet new_msg;
			new_msg.reason = req->reason;
			Team* m_team = role->GetMyTeam();
			if (m_team != NULL)
			{
				m_team->SendToFollowMember((const char*)&new_msg, length, false);
			}
		}
	}
}

void MessageHandler::OnRoleClientSystemSetChatGroupReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSRoleSystemSetChatGroupReq* req = (Protocol::CSRoleSystemSetChatGroupReq*)msg;

	role->GetRoleModuleManager()->GetRoleSetting()->SetRoleChatGroupSetting(req->chat_group_id, req->set_value);
}

void MessageHandler::OnBattleRoleModifyAnimationSpeedReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleAnimationSpeedModify* req = (Protocol::CSBattleAnimationSpeedModify*)msg;

	BattleManagerLocal::GetInstance().RoleModifyAnimationSpeedReq(role, req->speed_type);
}

void MessageHandler::OnSpeciallogicMiJingQiWenReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRAMiJingQiWenQuestionReq* req = (Protocol::CSRAMiJingQiWenQuestionReq *)msg;

	if (scene->GetSpecialLogic()->GetSceneType() == Scene::SCENE_TYPE_MI_JING_QI_WEN)
	{
		SpecialLogic * m_special = scene->GetSpecialLogic();
		SpecialLogicMiJingQiWenFB *mijiqiwen_speciallogic = (SpecialLogicMiJingQiWenFB *)m_special;
		mijiqiwen_speciallogic->OnRoleAnswerQuestion(role, req->npc_obj_id, req->answer_id);
	}
}

void MessageHandler::OnSpeciallogicMiJingQiWenBuyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRAMiJingQiWenBuyItemReq* req = (Protocol::CSRAMiJingQiWenBuyItemReq *)msg;

	if (scene->GetSpecialLogic()->GetSceneType() == Scene::SCENE_TYPE_MI_JING_QI_WEN)
	{
		SpecialLogic * m_special = scene->GetSpecialLogic();
		SpecialLogicMiJingQiWenFB *mijiqiwen_speciallogic = (SpecialLogicMiJingQiWenFB *)m_special;
		mijiqiwen_speciallogic->OnRoleBuyItem(role, req->npc_obj_id, req->item_idx);
	}
}

void MessageHandler::OnSpeciallogicMiJingQiWenReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRAMiJingQiWenQuestionReq2 * req = (Protocol::CSRAMiJingQiWenQuestionReq2 *)msg;

	if (scene->GetSpecialLogic()->GetSceneType() == Scene::SCENE_TYPE_MI_JING_QI_WEN_2)
	{
		SpecialLogic * m_special = scene->GetSpecialLogic();
		SpecialLogicMiJingQiWenFB2 * mijiqiwen_speciallogic = (SpecialLogicMiJingQiWenFB2 *)m_special;
		mijiqiwen_speciallogic->OnRoleAnswerQuestion(role, req->npc_obj_id, req->answer_id);
	}
}

void MessageHandler::OnSpeciallogicMiJingQiWenBuyReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRAMiJingQiWenBuyItemReq2 * req = (Protocol::CSRAMiJingQiWenBuyItemReq2 *)msg;

	if (scene->GetSpecialLogic()->GetSceneType() == Scene::SCENE_TYPE_MI_JING_QI_WEN_2)
	{
		SpecialLogic * m_special = scene->GetSpecialLogic();
		SpecialLogicMiJingQiWenFB2 * mijiqiwen_speciallogic = (SpecialLogicMiJingQiWenFB2 *)m_special;
		mijiqiwen_speciallogic->OnRoleBuyItem(role, req->npc_obj_id, req->item_idx);
	}
}

void MessageHandler::OnHandoverChatGroupLeadershipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHandoverChatGroupLeadership* req = (Protocol::CSHandoverChatGroupLeadership*)msg;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		CrossChatGroupManager::Instance().OnRoleHandOverLeadershipToOther(req->group_id, role->GetUID(), req->new_leader_uid);
	}
	else
	{
		crossgameprotocal::GameHiddenChatGroupLeadershipHandover ghcglh;
		ghcglh.group_id = req->group_id;
		ghcglh.old_leader_uid = role->GetUID();
		ghcglh.new_leader_uid = req->new_leader_uid;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghcglh, sizeof(ghcglh));
	}
}

void MessageHandler::OnActiveSystemFetchExtraRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActiveSystemExtraRewardFetch* req = (Protocol::CSActiveSystemExtraRewardFetch*)msg;
	
	role->GetRoleModuleManager()->GetActiveSystem()->OnFetchActiveExtraReward(req->active_type);
}

void MessageHandler::OnMonthCardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMonthCardReq * req = (Protocol::CSMonthCardReq *)msg;
	switch (req->req_type)
	{
		case Protocol::CSMonthCardReq::MONTH_CARD_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnSendMonthCardInfo();
		}
		break;
		case Protocol::CSMonthCardReq::MONTH_CARD_REQ_TYPE_BUY_ITEM:
		{
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnBuyMonthShopItem(req->param1);
		}
		break;
		case Protocol::CSMonthCardReq::MONTH_CARD_REQ_TYPE_BUY_CMD_CHECK:
		case Protocol::CSMonthCardReq::MONTH_CARD_REQ_TYPE_BUY_CARD_CHECK:
		{
			long long unique_id = 0;
			bool is_succ = role->GetRoleModuleManager()->GetRoleActivityManager()->OnBuyMonthCheck(req->req_type, req->param1, req->param2, unique_id);
			if (is_succ)
			{
				Protocol::SCCmdBuyItemCheckRet protocol;
				protocol.unique_id = unique_id;
				protocol.cfg_ver = req->param2;
				protocol.buy_type = BUY_TIME_TYPE_MONTH_CARD;

				EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
			}
		}
		break;
		case Protocol::CSMonthCardReq::MONTH_CARD_REQ_TYPE_FETCH_DAY_REWARD:
		{
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnFetchMonthDayReward(req->param1);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnPetMonopolyOperateReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetMonopolyOperate* req = (Protocol::CSPetMonopolyOperate*)msg;

	role->GetRoleModuleManager()->GetPetMonopoly()->OnOperateReq(req->operate_type, req->param1, req->param2);
}

void MessageHandler::OnLunHuiWangChuanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSLunHuiWangChuanReq* req = (Protocol::CSLunHuiWangChuanReq*)msg;
	switch (req->req_type)
	{
	case Protocol::CSLunHuiWangChuanReq::CS_LHWC_TYPE_INFO:
		role->GetRoleModuleManager()->GetLunHuiWangChuan()->SendInfo();
		break;
	case Protocol::CSLunHuiWangChuanReq::CS_LHWC_TYPE_BATTLE:
		role->GetRoleModuleManager()->GetLunHuiWangChuan()->OnStartBattle();
		break;
	case Protocol::CSLunHuiWangChuanReq::CS_LHWC_TYPE_REWARD:
		role->GetRoleModuleManager()->GetLunHuiWangChuan()->GetReward(req->param);
	default:
		break;
	}
}

void MessageHandler::OnCloudArenaOperateReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_ONLY_IN_CROSS);
		return;
	}

	Protocol::CSCloudArenaOperate* req = (Protocol::CSCloudArenaOperate*)msg;
	
	CloudArena::Instance().OnRoleOperateReq(role, req->operate_type, req->param1, req->param2);
}

void MessageHandler::OnCloudArenaSetFormationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	/*
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_ONLY_IN_CROSS);
		return;
	}

	Protocol::CSCloudArenaSetFormation* req = (Protocol::CSCloudArenaSetFormation*)msg;

	CloudArena::Instance().OnRoleSetFormation(req, role);
	*/
}

void MessageHandler::OnCloudArenaRealTimeInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenCloudArenaRealTimeInfoReq req;
		req.role_id = role->GetUID();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
	}
	else
	{
		const CloudArenaUser* user = CloudArena::Instance().GetUser(role->GetUID());
		if (NULL != user)
		{
			Protocol::SCCloudArenaRealTimeInfo info;
			info.cur_score = user->GetScore();
			info.cur_rank = CloudArena::Instance().GetUserRank(role->GetUID());

			EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
		}
	}
}

void MessageHandler::OnRelicLootingMachReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_RELIC_LOOTING))
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	Protocol::CSRelicLootingMatchReq* req = (Protocol::CSRelicLootingMatchReq*)msg;
	ActivityRelicLooting * activity = (ActivityRelicLooting *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_RELIC_LOOTING);
	if (activity != NULL)
	{
		if (req->is_match == Protocol::CSRelicLootingMatchReq::TYPE_MATCH)
		{
			activity->OnMatchReq(role);
		}
		else if(req->is_match == Protocol::CSRelicLootingMatchReq::TYPE_CANCEL)
		{
			activity->OnCancelReq(role);
		}
		else if (req->is_match == Protocol::CSRelicLootingMatchReq::TYPE_RANK)
		{
			activity->OnRoleCheckRankList(role);
		}
		else if (req->is_match == Protocol::CSRelicLootingMatchReq::TYPE_REWARD)
		{
			activity->OnGetRoleReward(role, req->param);
		}
		else if (req->is_match == Protocol::CSRelicLootingMatchReq::TYPE_REWARD_INFO)
		{
			activity->OnRoleCheckAchievementReward(role);
		}
		else if (req->is_match == Protocol::CSRelicLootingMatchReq::TYPE_SET_SHOW)
		{
			activity->SetShow(role->GetUID());
		}
	}
}

void MessageHandler::OnRelicLootingUseSkillReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRelicLootingPlayerReq* req = (Protocol::CSRelicLootingPlayerReq*)msg;
	if (scene->GetSceneType() != Scene::SCENE_TYPE_RELIC_LOOTING)return;
	switch (req->type)
	{
	case Protocol::CSRelicLootingPlayerReq::MSG_TYPE_USE_SKILL:
		{
			SpecialLogicRelicLootingFb *speciallogic = (SpecialLogicRelicLootingFb *)scene->GetSpecialLogic();
			speciallogic->OnRoleUseSkill(role, req->param1, req->param2, req->param3, req->param4);
		}
		break;
	case Protocol::CSRelicLootingPlayerReq::MSG_TYPE_GET_SKILL:
		{
			SpecialLogicRelicLootingFb *speciallogic = (SpecialLogicRelicLootingFb *)scene->GetSpecialLogic();
			speciallogic->GetSkillItem(role, req->param1);
		}
		break;
	case Protocol::CSRelicLootingPlayerReq::MSG_TYPE_GIVE_UP_SKILL:
		{
			SpecialLogicRelicLootingFb *speciallogic = (SpecialLogicRelicLootingFb *)scene->GetSpecialLogic();
			speciallogic->GiveUpSkillItem(role);
		}
		break;
	case Protocol::CSRelicLootingPlayerReq::MSG_TYPE_TS:
		{
			SpecialLogicRelicLootingFb *speciallogic = (SpecialLogicRelicLootingFb *)scene->GetSpecialLogic();
			speciallogic->OnRoleTs(role, req->param1);
		}
		break;
	case Protocol::CSRelicLootingPlayerReq::MSG_TYPE_MARK:
		{
			SpecialLogicRelicLootingFb *speciallogic = (SpecialLogicRelicLootingFb *)scene->GetSpecialLogic();
			speciallogic->SendMake(role, req->param1, req->param2, req->param3);
		}
		break;
	case Protocol::CSRelicLootingPlayerReq::MSG_TYPE_GIVE_UP_SKILL_STATUS:
		{
			SpecialLogicRelicLootingFb *speciallogic = (SpecialLogicRelicLootingFb *)scene->GetSpecialLogic();
			speciallogic->GiveUpSkillStatus(role);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnRoleInfoNoticeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{	
	Protocol::SCRoleInfoNotice protocol;
	protocol.is_cross = CrossConfig::Instance().IsHiddenServer();
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, sizeof(protocol));

	if (!CrossConfig::Instance().IsHiddenServer())
	{		
		crossgameprotocal::GameHiddenRoleInfoNoticeReqRoute router;
		router.role_id = role->GetUID();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&router, sizeof(router));
	}
}

void MessageHandler::OnRoleTopLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if(role->GetLevel() < MAX_ROLE_LEVEL) return;

	Protocol::CSRoleTopLevelReq * req = (Protocol::CSRoleTopLevelReq *)msg;
	switch (req->op_type)
	{
	case Protocol::CSRoleTopLevelReq::ROLE_TOP_LEVEL_REQ_TYPE_NOTICE:
	{
		role->GetRoleModuleManager()->OnSetTopLevelNoticeFlag();
	}
	break;
	case Protocol::CSRoleTopLevelReq::ROLE_TOP_LEVEL_REQ_TYPE_FETCH:
	{
		role->GetRoleModuleManager()->OnFetchTopLevelReward(req->param1);
	}
	break;
	case Protocol::CSRoleTopLevelReq::ROLE_TOP_LEVEL_REQ_TYPE_FETCH_ACHIEVE:
	{
		role->GetRoleModuleManager()->OnFetchTopLevelAchieveReward(req->param1);
	}	
	break;
	default:
		break;
	}
}

void MessageHandler::OnMoveAssistReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMoveAssistRet * ret_msg = (Protocol::CSMoveAssistRet *)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSMoveAssistRet, way_point_list));
	if (length < offset)
	{
		return;
	}
	if (0 >= ret_msg->way_point_count
		|| MOVE_ASSIXT_WAY_POINT_NUM < ret_msg->way_point_count)
	{
		return;
	}
	offset += sizeof(Protocol::CSMoveAssistRet::WayPoint) * ret_msg->way_point_count;
	if (length < offset)
	{
		return;
	}

	static Protocol::CSMoveAssistRet ret_info;
	memcpy(&ret_info, msg, offset);

	Scene * target_scene = World::GetInstWorld()->GetSceneManager()
		->GetSceneById(ret_info.scene_id, ret_info.scene_key);
	if (NULL == target_scene)
	{
		return;
	}
	Obj * target_obj = target_scene->GetObj(ret_info.obj_id);
	if (NULL == target_obj)
	{
		return;
	}

	//获得疑似的移动辅助申请者 需要内部验证 不准确
	MoveAssist * target_move_assist = NULL;
	if (target_obj->IsCharacter(target_obj->GetObjType()))
	{
			Character *target_cha = (Character *)target_obj;
			target_move_assist = target_cha->GetMoveAssist(false);
			if (NULL == target_move_assist)
			{
				return;
			}
		}
	else
	{
		return;
	}

	target_move_assist->OnMoveAssistRet(ret_info);
}

void MessageHandler::OnMapExplorationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMapExplorationReq * ret_msg = (Protocol::CSMapExplorationReq *)msg;
	
	switch (ret_msg->req_type)
	{
	case Protocol::CSMapExplorationReq::MAP_EXPLORATION_TYPE_INFO:
		role->GetRoleModuleManager()->GetMapExploration()->SendInfo();
		break;
	case Protocol::CSMapExplorationReq::MAP_EXPLORATION_TYPE_REQ:
		role->GetRoleModuleManager()->GetMapExploration()->ExplorationMap(ret_msg->param);
		break;
	default:
		break;
	}
}

void MessageHandler::OnSetAutoAddPointFlagReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSetAutoAddPointFlag * req = (Protocol::CSSetAutoAddPointFlag *)msg;

	switch (req->req_type)
	{
		case Protocol::SET_AUTO_ADD_POINT_FLAG_ROLE:
			{
				role->GetRoleModuleManager()->GetRoleAttributePlan()->SetCustomAddPoint(req->index, req->param);
			}
			break;
		case Protocol::SET_AUTO_ADD_POINT_FLAG_PET:
			{
				if (ARRAY_LENGTH(req->param) >= 2)
				{
					role->GetRoleModuleManager()->GetPet()->SetAutoAddInfo(req->param[0], req->param[1]);
				}				
			}			
			break;
		default:
			break;
	}
}

void MessageHandler::OnPetSoulUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetSoulUplevelReq * ret_msg = (Protocol::CSPetSoulUplevelReq *)msg;

	role->GetRoleModuleManager()->GetPet()->UpLevelPetSoulEquip(ret_msg->is_put_on, ret_msg->index, ret_msg->cost_num, ret_msg->bag_colum, ret_msg->bag_idx, ret_msg->bag_num, ret_msg->pet_soul_index);
}

void MessageHandler::SetPetGodPrintWearSchemeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetGodPrintSetearSchemeReq * ret_msg = (Protocol::CSPetGodPrintSetearSchemeReq *)msg;

	role->GetRoleModuleManager()->GetPet()->SetPetGodPrintScheme(ret_msg);
}

void MessageHandler::OnPetGodPrintUseItemUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetGodPrintUplevelReq * ret_msg = (Protocol::CSPetGodPrintUplevelReq *)msg;

	role->GetRoleModuleManager()->GetPet()->UpLevelPetGodPrintEquip2(ret_msg->index, ret_msg->bag_num, ret_msg->bag_idx);
}

void MessageHandler::OnNetherWorldPurgatoryReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSNetherWorldPurgatoryReq * ret_msg = (Protocol::CSNetherWorldPurgatoryReq *)msg;
	role->GetRoleModuleManager()->GetNetherWorldPurgatory()->OnNetherWorldPurgatoryOp(ret_msg->req_type, ret_msg->param1, ret_msg->param2, ret_msg->param3);
}

void MessageHandler::OnPetGodFBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetGodFBReq * req_msg = (Protocol::CSPetGodFBReq *)msg;

	role->GetRoleModuleManager()->GetPetGodFB()->OnReq(req_msg->req_type, req_msg->param_1);
}

void MessageHandler::OnDuJieReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSDuJieReq * req_msg = (Protocol::CSDuJieReq *)msg;

	role->GetRoleModuleManager()->GetDuJie()->OnReq(req_msg->req_type, req_msg->param_1);
}

void MessageHandler::OnRoleTriggerPetDeadSummonGuideReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetFuncGuide()->OnRoleTriggerPetDeadSummonGuide();
}

void MessageHandler::OnRoleFetchHistoryChivalrousRewardReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRoleHistoryChivalrousRewardFetch* req = (Protocol::CSRoleHistoryChivalrousRewardFetch*)msg;
	role->GetRoleModuleManager()->GetSalary()->OnFetchHistoryChivalrousReward(req->fetch_seq);
}

void MessageHandler::OnQueryWeekChivalrousRankListReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSChivalrousRankListReq* req = (Protocol::CSChivalrousRankListReq*)msg;
	switch (req->rank_type)
	{
	case CHIVALROUS_RANK_TYPE_SERVER:
		{
			crossgameprotocal::GameCrossQueryChivalrousRankListTop100Req rank_req;
			rank_req.plat_type = LocalConfig::Instance().GetPlatType();
			rank_req.server_id = LocalConfig::Instance().GetMergeServerId();
			rank_req.uid = role->GetUID();
			InternalComm::Instance().SendToCross((const char*)&rank_req, sizeof(rank_req));
		}
		break;

	case CHIVALROUS_RANK_TYPE_GUILD:
		{
			crossgameprotocal::GameCrossQuerySomeRoleChivalrousRankReq rank_req;
			rank_req.plat_type = LocalConfig::Instance().GetPlatType();
			rank_req.server_id = LocalConfig::Instance().GetMergeServerId();
			rank_req.uid = role->GetUID();
			rank_req.rank_type = CHIVALROUS_RANK_TYPE_GUILD;
			rank_req.count = 0;

			Guild* guild = role->GetGuild();
			if (NULL != guild)
			{		
				GuildMemberManager& gmm = guild->GetMemberManagerRef();
				for (int i = 0; i < GUILD_MAX_MEMBER_COUNT && rank_req.count < ARRAY_LENGTH(rank_req.role_id_list); ++i)
				{
					GuildMember* member = gmm.GetMemberByIndex(i);
					if (NULL == member) continue;

					rank_req.role_id_list[rank_req.count++] = member->GetUid();
				}			
			}

			int send_len = sizeof(rank_req) - sizeof(rank_req.role_id_list) + rank_req.count * sizeof(rank_req.role_id_list[0]);
			InternalComm::Instance().SendToCross((const char*)&rank_req, send_len);
		}
		break;

	case CHIVALROUS_RANK_TYPE_FRIEND:
		{
			crossgameprotocal::GameCrossQuerySomeRoleChivalrousRankReq rank_req;
			rank_req.plat_type = LocalConfig::Instance().GetPlatType();
			rank_req.server_id = LocalConfig::Instance().GetMergeServerId();
			rank_req.uid = role->GetUID();
			rank_req.rank_type = CHIVALROUS_RANK_TYPE_GUILD;
			rank_req.count = 0;
			rank_req.role_id_list[rank_req.count++] = role->GetUID();

			const FriendManager::UserFrinedSet * friend_set = FriendManager::Instance().GetUserFriendSetInfo(role);
			if (NULL != friend_set)
			{
				for (FriendManager::UserFrinedSet::const_iterator it = friend_set->begin(); it != friend_set->end() && rank_req.count < CHIVALROUS_RANK_LIST_NUM; it++)
				{
					long long friend_for_index = *it;
					int friend_uid = (friend_for_index << 32) >> 32;

					rank_req.role_id_list[rank_req.count++] = friend_uid;
				}
			}

			int send_len = sizeof(rank_req) - sizeof(rank_req.role_id_list) + rank_req.count * sizeof(rank_req.role_id_list[0]);
			InternalComm::Instance().SendToCross((const char*)&rank_req, send_len);
		}
		break;
	}
}

void MessageHandler::OnRoleJoinActivityRewardReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSRoleJoinActivityRewardReq* req = (Protocol::CSRoleJoinActivityRewardReq*)msg;
	role->GetRoleModuleManager()->GetRoleJoinActivityReward()->OnClientReq(req);
}

void MessageHandler::OnGuildActivePersonRankReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Guild * guild = role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_NO_GUILD);
		return;
	}

	guild->SendGuildActivePersonRank(role);
}

void  MessageHandler::OnGuildActiveGetGiftReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSGuildActiveGetGift * req = (Protocol::CSGuildActiveGetGift *)msg;

	Guild * guild = role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_NO_GUILD);
		return;
	}

	guild->OnGuildActiveGetGiftReq(role , req->seq);
}

void MessageHandler::OnAddCustomInstructionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCustomInstructionAdd* req = (Protocol::CSCustomInstructionAdd*)msg;
	req->instruction_str[sizeof(InstructionStr) - 1] = 0;

	role->GetRoleModuleManager()->GetCustomInstruction()->AddCustomInstructionReq(req->instruction_str, sizeof(InstructionStr));
}

void MessageHandler::OnRemoveCustomInstructionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCustomInstructionRemove* req = (Protocol::CSCustomInstructionRemove*)msg;
	role->GetRoleModuleManager()->GetCustomInstruction()->RemoveCustomInstructionReq(req->index);
}

void MessageHandler::OnBattleSetCustomInstructionReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSBattleSetCustomInstruction* req = (Protocol::CSBattleSetCustomInstruction*)msg;
	
	role->GetRoleModuleManager()->GetCustomInstruction()->OnBattleSetCustomInstructionReq(req->index, req->grid_idx);
}

void MessageHandler::OnBatchRemoveCustomInstructionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCustomInstructionBatchRemove* req = (Protocol::CSCustomInstructionBatchRemove*)msg;
	if (length < static_cast<int>(sizeof(Protocol::CSCustomInstructionBatchRemove)) - static_cast<int>(sizeof(req->index_list)))
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	
	if (req->num <= 0 || req->num > ARRAY_ITEM_COUNT(req->index_list))
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (length != static_cast<int>(sizeof(Protocol::CSCustomInstructionBatchRemove)) - static_cast<int>(sizeof(req->index_list)) + req->num * static_cast<int>(sizeof(req->index_list[0])))
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	role->GetRoleModuleManager()->GetCustomInstruction()->BatchRemoveCustomInstructionReq(req->num, req->index_list);
}

void MessageHandler::OnHongBaoClientReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHongBaoClientReq * req = (Protocol::CSHongBaoClientReq*)msg;

	HongBaoManager::Instance().OnClientReq( role ,req);
}

void MessageHandler::OnBigDipperReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBigDipperReq * req = (Protocol::CSBigDipperReq*)msg;

	switch (req->type)
	{
	case Protocol::CSBigDipperReq::REQ_TYPE_0:
		role->GetRoleModuleManager()->GetBigDipper()->SendInfo();
		break;
	case Protocol::CSBigDipperReq::REQ_TYPE_1:
		role->GetRoleModuleManager()->GetBigDipper()->StarBattle(req->param1);
		break;
	case Protocol::CSBigDipperReq::REQ_TYPE_2:
		role->GetRoleModuleManager()->GetBigDipper()->GetFirstPassReward(req->param1);
		break;
	case Protocol::CSBigDipperReq::REQ_TYPE_3:
		role->GetRoleModuleManager()->GetBigDipper()->GetReward(req->param1, req->param2);
		break;
	default:
		break;
	}
}

void MessageHandler::OnFallenGodReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFallenGodReq * req = (Protocol::CSFallenGodReq*)msg;

	switch (req->type)
	{
	case Protocol::CSFallenGodReq::REQ_TYPE_0:
		role->GetRoleModuleManager()->GetFallenGod()->SendInfo();
		break;
	case Protocol::CSFallenGodReq::REQ_TYPE_1:
		role->GetRoleModuleManager()->GetFallenGod()->StarBattle(req->param1);
		break;
	default:
		break;
	}
}

void MessageHandler::OnTianDiDaoHenReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTianDiDaoHenReq * req = (Protocol::CSTianDiDaoHenReq *)msg;

	switch (req->req_type)
	{
		case Protocol::TIAN_DI_DAO_HEN_REQ_TYPE_ACTIVE:
			{
				role->GetRoleModuleManager()->GetRoleTianDiDaoHen()->ActiveItem(req->param1);
			}
			break;
		case Protocol::TIAN_DI_DAO_HEN_REQ_TYPE_FETCH:
			{
				role->GetRoleModuleManager()->GetRoleTianDiDaoHen()->FetchFinalReward();
			}
			break;

	}	
}

void MessageHandler::OnGiftReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGiftReq * req = (Protocol::CSGiftReq *)msg;

	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSGiftReq, str));
	if (length < offset)
	{
		return;
	}


	switch (req->req_type)
	{
		case Protocol::GIFT_REQ_TYPE_GIVE:
			{
				int str_len = (length - offset);
				if (str_len != COMMON_SAVE_GIVE_GIFT_STR_NUM)
				{
					return;
				}

				role->GetRoleModuleManager()->GetRoleGiveGift()->GiveItem(req->p1, req->p2, req->p3, req->str);
			}
			break;
		case Protocol::GIFT_REQ_TYPE_FETCH:
			{
				GiveGift::Instance().FetchItem(role->GetUID(), ::ConvertParamToLongLong(req->p1, req->p2));
			}
			break;
		case Protocol::GIFT_REQ_TYPE_FLOWER:
			{
				role->GetRoleModuleManager()->GetRoleGiveGift()->GiveFlower(req->p1, req->p2, req->p3,req->p4, req->str);
			}
			break;
		case Protocol::GIFT_REQ_TYPE_BACK_TO_KISS:
			{
				FriendIntimacyManager::Instance().OnBackToKiss(role, req->p1);
			}
			break;
	}
}

void MessageHandler::OnCapacityTargetReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSCapacityTargetReq * req = (Protocol::CSCapacityTargetReq *)msg;

	if (req->req_type == Protocol::CAPACITY_TARGET_REQ_TYPE_START)
	{
		role->GetRoleModuleManager()->StartCapacityTarget();
	}

	if (req->req_type == Protocol::CAPACITY_TARGET_REQ_TYPE_FETCH)
	{
		role->GetRoleModuleManager()->CapacityTargetFetchReward();
	}
}

void MessageHandler::OnFetchRecourceDownloadReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFetchRecourceDownloadReward * fetch = (Protocol::CSFetchRecourceDownloadReward *)msg;

	role->GetRoleModuleManager()->FetchResourceDownloadReward();
}

void MessageHandler::OnPrestigeTaskSelectVillageReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPrestigeTaskSelectVillage* req = (CSPrestigeTaskSelectVillage*)msg;
	role->GetRoleModuleManager()->GetPrestigeTask()->OnSelectVillageReq(req->village_id);
}

void MessageHandler::OnPrestigeTaskReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetPrestigeTask()->OnReq(msg);
}

void MessageHandler::OnSVipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSvipClientCustomData* req = (Protocol::CSSvipClientCustomData*)msg;

	switch (req->oper_type)
	{
	case 0:
		{
			role->GetRoleModuleManager()->GetSVip()->SendSVipInfo();
		}
		break;

	case 1:
		{
			role->GetRoleModuleManager()->GetSVip()->SetClientCustomData(req->param1, req->param2);
		}
		break;
	}
	
}

void MessageHandler::OnMedalUpGradeSpecialReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMedalUpGradeSpecialReq* req = (Protocol::CSMedalUpGradeSpecialReq*)msg;
	role->GetRoleModuleManager()->GetMedal()->OnSpeMedalUpGrade(req->is_wear_grid == 1 ? true : false, req->column, req->index, req->consume_column_spe, req->consume_index_spe, req->consume_column, req->consume_index);
}

void MessageHandler::OnFangFraudRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFangFraudRewardReq * fetch = (Protocol::CSFangFraudRewardReq *)msg;

	role->GetRoleModuleManager()->FetchFangFraudReward(fetch->count, fetch->list);
}

void MessageHandler::OnRoleAttrPelletReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRoleAttrPelletReq * req = (Protocol::CSRoleAttrPelletReq *)msg;

	if (req->op_type != Protocol::CSRoleAttrPelletReq::ROLE_ATTR_PELLET_REQ_TYPE_ALL_INFO &&
		!FunOpen::IsFunOpen(role, FUN_TYPE_ATTR_PELLET, true))
	{
		return;
	}

	switch (req->op_type)
	{
		case Protocol::CSRoleAttrPelletReq::ROLE_ATTR_PELLET_REQ_TYPE_ALL_INFO:
		{
			role->GetRoleModuleManager()->GetRoleAttrPellet()->OnSendAllInfo();
		}
		break;
		case Protocol::CSRoleAttrPelletReq::ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM:
		{
			role->GetRoleModuleManager()->GetRoleAttrPellet()->OnUseAttrPellet(req->param1, req->param2);
		}
		break;
		case  Protocol::CSRoleAttrPelletReq::ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM_PET:
		{
			role->GetRoleModuleManager()->GetRoleAttrPellet()->OnUsePetAttrPellet(req->param1, req->param2, req->param3);
		}
		break;
		case  Protocol::CSRoleAttrPelletReq::ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM_PARNTER:
		{
			role->GetRoleModuleManager()->GetRoleAttrPellet()->OnUsePartnerAttrPellet(req->param1, req->param2, req->param3);
		}
		break;
		default:
		break;
	}
}

void MessageHandler::OnActiveTurntableReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSActiveTurntableReq * req = (Protocol::CSActiveTurntableReq *)msg;

	switch (req->type)
	{
	case Protocol::CSActiveTurntableReq::REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetActiveSystem()->SendTurntableInfo();
		}
		break;
	case Protocol::CSActiveTurntableReq::REQ_TYPE_LUCK:
		{
			role->GetRoleModuleManager()->GetActiveSystem()->OnLuckTurntable();
		}
		break;
	default:
		return;
	}
}

void MessageHandler::OnBackRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSBackRewardReq* req = (Protocol::CSBackRewardReq*)msg;

	RoleActivityBackReward * role_activity = static_cast<RoleActivityBackReward *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL == role_activity)
	{
		return;
	}

	role_activity->OnReq(req->type, req->param);
}

void MessageHandler::OnAccessoriseReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PEISHI, true))return;
	Protocol::CSAccessoriesMentOperaReq* req = (Protocol::CSAccessoriesMentOperaReq*)msg;

	switch (req->msg_type)
	{
	case Protocol::CSAccessoriesMentOperaReq::OPERA_TYPE_INFO_REQ:
		role->GetRoleModuleManager()->GetAccessoriesmentManager()->SendAccessoriesMentInfo();
		break;
	case Protocol::CSAccessoriesMentOperaReq::OPERA_TYPE_PUTON:
		role->GetRoleModuleManager()->GetAccessoriesmentManager()->PutOn(req->param1, req->param2, req->param3);
		break;
	case Protocol::CSAccessoriesMentOperaReq::OPERA_TYPE_TAKEOFF:
		role->GetRoleModuleManager()->GetAccessoriesmentManager()->TakeOff(req->param1);
		break;
	case Protocol::CSAccessoriesMentOperaReq::OPERA_TYPE_DISCOMPOSE:
		role->GetRoleModuleManager()->GetAccessoriesmentManager()->Discompose(req->param1, req->param2, req->param3);
		break;
	case Protocol::CSAccessoriesMentOperaReq::OPERA_TYPE_ACCESSORIES:
		role->GetRoleModuleManager()->GetAccessoriesmentManager()->Strengthen(req->param1, req->param2);
		break;
	case Protocol::CSAccessoriesMentOperaReq::OPERA_TYPE_CHANGE:
		role->GetRoleModuleManager()->GetAccessoriesmentManager()->ChangeAccessories(req->param1, req->param2, req->param3, req->param4);
		break;
	case Protocol::CSAccessoriesMentOperaReq::OPERA_TYPE_SPECIAL:
		role->GetRoleModuleManager()->GetAccessoriesmentManager()->UpLevelSpecialEquip(req->param1);
		break;
	default:
		break;
	}
}

void MessageHandler::OnAccessoriseVocChangeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PEISHI, true))return;
	Protocol::CSAccessoriesMentVocComposeReq* req = (Protocol::CSAccessoriesMentVocComposeReq*)msg;

	role->GetRoleModuleManager()->GetAccessoriesmentManager()->ComposeVod(req->is_wear == 1 ? true : false, req->index, req->replace_index, req->compose_voc_list);
}

void MessageHandler::OnYinYangEyeOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_YIN_YANG_EYE, true)) return;

	UNSTD_STATIC_CHECK(Protocol::CSYinYangEyeOpReq::YIN_YANG_EYE_OP_TYPE_MAX == 3);
	Protocol::CSYinYangEyeOpReq* req = (Protocol::CSYinYangEyeOpReq *)msg;

	switch (req->op_type)
	{
	case Protocol::CSYinYangEyeOpReq::YIN_YANG_EYE_OP_TYPE_OPEN:
		{
			role->GetRoleModuleManager()->GetYinYangEye()->OnOpen();
		}
		break;
	case Protocol::CSYinYangEyeOpReq::YIN_YANG_EYE_OP_TYPE_BOX:
		{
			role->GetRoleModuleManager()->GetYinYangEye()->OnFetchBox(req->param);
		}
		break;
	case Protocol::CSYinYangEyeOpReq::YIN_YANG_EYE_OP_TYPE_NPC:
		{
			role->GetRoleModuleManager()->GetYinYangEye()->OnNpcOp(req->param);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnEndlessTowerOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ENDLESS_TOWER, true)) return;

	Protocol::CSEndlessTowerReq* req = (Protocol::CSEndlessTowerReq *)msg;

	role->GetRoleModuleManager()->GetEndlessTower()->OnOpreat(req->req_type, req->param1, req->param2);
}

void MessageHandler::OnJieYiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return;
	}

	Protocol::CSJieYiReq * req = (Protocol::CSJieYiReq *)msg;
	int role_id = role->GetUID();
	switch (req->req_type)
	{
		case Protocol::JIE_YI_REQ_RET_TYPE_CREATE_CONFIRM:
			{
				JieYiZuManager::Instance().CreateJieYiZuMemberConfirm(role->GetUID());
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_CREATE_CANCEL:
			{
				JieYiZuManager::Instance().CancelCreateJieYiZuByRoleId(role->GetUID(), JIE_YI_CANCEL_TYPE_ROLE);
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_CREATE_NAME_FINISH:
			{
				JieYiZuManager::Instance().CreateJieYiZuNameFinish(role->GetUID());
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_LEAVE:
			{				
				JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
				if (!jie_yi_zu)
				{
					return;
				}

				jie_yi_zu->JieYiZuLeave(role_id, req->p1 == 0);
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_KICK:
			{
				JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
				if (!jie_yi_zu)
				{
					return;
				}

				jie_yi_zu->JieYiZuKick(role_id, (int)req->p1);
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_INVITE:
			{
				int target_role_id = (int)req->p1;
				JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
				if (jie_yi_zu)
				{
					jie_yi_zu->JieYiZuInvite(role_id, target_role_id);
				}			
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_INVITE_APPLY:
			{
				JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->p1);
				if (!jie_yi_zu)
				{
					return;
				}

				jie_yi_zu->JieYiZuInviteReply(role_id, req->p2 != 0);
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_ZU_INFO:
			{
				JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->p1);
				if (!jie_yi_zu)
				{
					return;
				}

				jie_yi_zu->SendInfo(role, JIE_YI_ZU_INFO_TYPE_QUERY);
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_TAN_HE:
			{
				JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
				if (!jie_yi_zu)
				{
					return;
				}

				jie_yi_zu->TanHe(role_id);
			}
			break;
		case Protocol::JIE_YI_REQ_RET_TYPE_TAN_HE_VOTE:
			{
				int target_role_id = (int)req->p1;
				JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
				if (!jie_yi_zu)
				{
					return;
				}

				jie_yi_zu->TanHeVote(role_id, target_role_id);
			}
			break;
		default:
			break;
	}
}

void MessageHandler::OnJieYiCreate(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSJieYiCreate * req = (Protocol::CSJieYiCreate *)msg;

	JieYiZuManager::Instance().CreateJieYiZu(role, req->jie_yi_name);
}

void MessageHandler::OnJieYiCreateName(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSJieYiCreateName * req = (Protocol::CSJieYiCreateName *)msg;
	JieYiZuManager::Instance().CreateJieYiZuName(role->GetUID(), req->jie_yi_name);
}

void MessageHandler::OnJieYiMemberName(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSJieYiZuMemberName * req = (Protocol::CSJieYiZuMemberName *)msg;
	int role_id = role->GetUID();
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuMemberName(role, req->member_name);
}

void MessageHandler::OnJieYiText(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSJieYiZuText * req = (Protocol::CSJieYiZuText *)msg;
	int role_id = role->GetUID();
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuText(role_id, req->text);
}

void MessageHandler::OnJieYiName(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSJieYiZuName * req = (Protocol::CSJieYiZuName *)msg;
	int role_id = role->GetUID();
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuName(role, req->jie_yi_name);
}

void MessageHandler::OnJieYiXuanYanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSJieYiZuXuanYanReq * req = (Protocol::CSJieYiZuXuanYanReq *)msg;
	int role_id = role->GetUID();

	if (req->type == JIE_YI_XUAN_YAN_TYPE_ROLE)
	{
		JieYiZuManager::Instance().RoleXuanYan(role_id, req->is_delete, &req->text);
	}

	if (req->type == JIE_YI_XUAN_YAN_TYPE_ZU)
	{
		JieYiZuManager::Instance().JieYiZuXuanYan(role_id, req->text, req->is_delete);
	}
}

void MessageHandler::OnJieYiXuanYanListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSJieYiZuXuanYanReqList * req = (Protocol::CSJieYiZuXuanYanReqList *)msg;
	int role_id = role->GetUID();
	if (req->type == JIE_YI_XUAN_YAN_TYPE_ROLE)
	{
		JieYiZuManager::Instance().SendRoleXuanYan(role_id);
	}

	if (req->type == JIE_YI_XUAN_YAN_TYPE_ZU)
	{
		JieYiZuManager::Instance().SendJieYiZuXuanYan(role_id);
	}
}

void MessageHandler::OnQingYuanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSQingYuanReq * req = (Protocol::CSQingYuanReq *)msg;

	int role_id = role->GetUID();

	int ret = 1;
	switch (req->req_type)
	{
		case QING_YUAN_REQ_TYPE_INVITE:
			{
				ret = QingYuanManager::Instance().Invite(role, req->p1,req->p2);
			}
			break;
		case QING_YUAN_REQ_TYPE_INVITE_REPLY:
			{
				ret = QingYuanManager::Instance().InviteReply(role, req->p1, req->p2);
			}
			break;
		case QING_YUAN_REQ_TYPE_GIFT:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->GiveGift(role, req->p1);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_GIFT_FETCH:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->GiftFetch(role);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_GIFT_FETCH_DAY:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->GiveFetchDay(role);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_SKILL_NOT_EFFECT_FLAG:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->SetSkillNoEffectFlag(role, req->p1);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_LEAVE:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->Leave(role);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_LEAVE_REPLY:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->LeaveReply(role, req->p1 != 0);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_LEAVE_SINGLE:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->LeaveSingle(role);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_LEAVE_FORCE:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					qing_yuan->LeaveForce(role);
				}
			}
			break;
		case QING_YUAN_REQ_TYPE_LEAVE_CANCEL:
			{
				QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
				if (qing_yuan)
				{
					ret = qing_yuan->LeaveCancel(role);
				}
			}
			break;
	}

	if (ret != 0)
	{
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | role_id[%d] req->req_type:%d ret:%d", __FUNCTION__, __LINE__, role_id, req->req_type, ret);
	}
}

void MessageHandler::OnCowReportGoodNewsClientReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSCowReportGoodNewsClientReq* req = (Protocol::CSCowReportGoodNewsClientReq *)msg;

	CowReportGoodNewsManager::Instance().OnClientReq(role , req);
}

void MessageHandler::OnTigerBringsBlessingClientReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSTigerBringsBlessingClientReq* req = (Protocol::CSTigerBringsBlessingClientReq *)msg;

	TigerBringsBlessingManager::Instance().OnClientReq(role, req);
}

void MessageHandler::OnWanLingFBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWanLingFBReq* req = (Protocol::CSWanLingFBReq *)msg;

	role->GetRoleModuleManager()->GetRoleWanLingFB()->OnReq(req->req_type, req->param_1, req->param_2);
}

void MessageHandler::OnQueryItemRecommendPriceReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSQueryItemRecommendPrice* req = (CSQueryItemRecommendPrice*)msg;
	TradeMarketRouter::Instance().OnQueryItemRecommendPriceReq(role, req);
}

void MessageHandler::OnBattleSkipReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleSkipReq* req = (CSBattleSkipReq*)msg;
	BattleManagerLocal::GetInstance().RoleSkipBattleReq(role);
}

void MessageHandler::OnBattleFastForwardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSBattleFastForwardReq* req = (CSBattleFastForwardReq*)msg;
	BattleManagerLocal::GetInstance().RoleFastForwardBattleReq(role);
}

void MessageHandler::OnFlyUpSystemReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFlyUpSystemReq *req = (Protocol::CSFlyUpSystemReq*)msg;
	switch (req->type)
	{
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_INFO:
		role->GetRoleModuleManager()->GetFlyUpSystem()->SendInfo();
		break;
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_ADV:
		role->GetRoleModuleManager()->GetFlyUpSystem()->OnUpAdvance();
		break;
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_FINISH:
		role->GetRoleModuleManager()->GetFlyUpSystem()->OnFinishFly();
		break;
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_SHILIAN:
		role->GetRoleModuleManager()->GetFlyUpSystem()->StartShiLian();
		break;
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_UP_SKILL:
		role->GetRoleModuleManager()->GetFlyUpSystem()->UplevelFlyUpSkill();
		break;
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_RAND_TASK:
		role->GetRoleModuleManager()->GetFlyUpSystem()->RankFlyUpTask();
		break;
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_ADV_SAVE:
		role->GetRoleModuleManager()->GetFlyUpSystem()->SaveAdvance();
		break;
	case Protocol::CSFlyUpSystemReq::REQ_TYPE_ONE_KEY_UP_SKILL:
		role->GetRoleModuleManager()->GetFlyUpSystem()->OnKeyUplevelFlyUpSkill();
		break;
	default:
		break;
	}
}

void MessageHandler::OnMirageTerritoryReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMirageTerritoryReq *req = (Protocol::CSMirageTerritoryReq*)msg;
	role->GetRoleModuleManager()->GetMirageTerritory()->OnOpreat(req->type, req->param1, req->param2, req->param3, req->param4);
}

void MessageHandler::OnMirageTerritoryPartnerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSMirageTerritorySetPartnerReq *req = (Protocol::CSMirageTerritorySetPartnerReq*)msg;
	role->GetRoleModuleManager()->GetMirageTerritory()->SetPartnerBattleInfo(req->programme_idx, req->partner_id_list, req->partner_posi_list);
}

void MessageHandler::OnRoleChangeColorReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSRoleChangeColorReq * req = (Protocol::CSRoleChangeColorReq*)msg;
	role->GetRoleModuleManager()->GetAdvanceCareer()->OnChangeColor(req->to_color);
}

void MessageHandler::OnXiuWeiReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSXiuWeiReq * req = (Protocol::CSXiuWeiReq*)msg;
	role->GetRoleModuleManager()->GetXiuWei()->OnClientReq(req);
}

void MessageHandler::OnTreasureTaskReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetTreasureTask()->OnReq(msg);
}

void MessageHandler::OnTeamLeaderEjectWindowsReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTeamLeaderEjectWindowsReq * req = (Protocol::CSTeamLeaderEjectWindowsReq *)msg;
	if (!role->InTeam())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}

	Team * m_team = role->GetMyTeam();
	if (m_team != NULL)
	{
		m_team->EjectWindosControl(role, req->windows_type);
	}
}

void MessageHandler::OnWeddingFBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWeddingFBReq* req = (Protocol::CSWeddingFBReq *)msg;

	WeddingManger::Instance().OnReq(role, req->req_type, req->param_1, req->param_2);
}

void MessageHandler::OnWorldArenaRoleOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSWorldArenaOperate* req = (Protocol::CSWorldArenaOperate*)msg;

	WorldArena::Instance().OnRoleOperate(role, req->oper_type, req->param1, req->param2);
}

void MessageHandler::OnPetComposeBatch(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetComposeBatch * req = (Protocol::CSPetComposeBatch *)msg;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSPetComposeBatch, batch_req));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->batch_req[0]);
	if (count <= 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->batch_req))
	{
		return;
	}

	Protocol::SCPetComposeBatchRet ret;
	ret.count = 0;

	for (int i = 0; i < count && ret.count < ARRAY_LENGTH(ret.pet_index_list); ++i)
	{
		const CSPetComposeBatch::Item & curr = req->batch_req[i];
		int pet_index = -1;
		if (role->GetRoleModuleManager()->GetPet()->Compose(curr.seq, curr.use_shards_num, GET_PET_REASON_TYPE_BATCH_COMPOSE_PET, &pet_index) != 0)
		{
			break;
		}

		ret.pet_index_list[ret.count] = pet_index;
		ret.count += 1;
	}

	int send_len = sizeof(ret) - (ARRAY_LENGTH(ret.pet_index_list) - ret.count) * sizeof(ret.pet_index_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &ret, sizeof(ret));
}

void MessageHandler::OnPetComposeAndDecomposeBatch(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetComposeAndDecomposeBatch * req = (Protocol::CSPetComposeAndDecomposeBatch *)msg;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSPetComposeAndDecomposeBatch, batch_req));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->batch_req[0]);
	if (count <= 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->batch_req))
	{
		return;
	}

	std::vector<ItemConfigData> rewards;
	std::map<int, int> consume_item_id_to_num;
	for (int i = 0; i < count; ++i)
	{
		const CSPetComposeAndDecomposeBatch::Item & curr = req->batch_req[i];
		const PetComposeCfg * compose_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetComposeCfg(curr.seq);
		if (compose_cfg == NULL)
		{
			return;
		}

		if (curr.num <= 0)
		{
			continue;
		}

		for (int k = 0; k < (int)compose_cfg->batch_decompose_rewards.size(); k++)
		{
			ItemConfigData batch_reward(compose_cfg->batch_decompose_rewards[k]);
			batch_reward.num *= curr.num;
			rewards.push_back(batch_reward);
		}

		consume_item_id_to_num[compose_cfg->need_item_id] += compose_cfg->need_item_num * curr.num;
	}
	
	MergeItemConfigData(rewards);

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(rewards))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	for (std::map<int, int>::iterator iter = consume_item_id_to_num.begin(); iter != consume_item_id_to_num.end(); ++iter)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(iter->first,iter->second))
		{
			return;
		}
	}

	for (std::map<int, int>::iterator iter = consume_item_id_to_num.begin(); iter != consume_item_id_to_num.end(); ++iter)
	{
		role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(iter->first, iter->second, __FUNCTION__);
	}

	role->GetRoleModuleManager()->GetKnapsack()->PutVec(rewards, PUT_REASON_PET_BATCH_EXCHANGE);

}

void MessageHandler::OnPetBadgeDecompose(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetBadgeDecompose * req = (Protocol::CSPetBadgeDecompose *)msg;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSPetBadgeDecompose, decompose_list));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->decompose_list[0]);
	if (count <= 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->decompose_list))
	{
		return;
	}

	std::set<KnapsackItemIndex> list;
	for (int i = 0; i < count; ++i)
	{
		list.insert(req->decompose_list[i]);
	}

	role->GetRoleModuleManager()->GetPet()->PetBadgeDeCompose(list);	
}

void MessageHandler::OnGuildHallOfFameReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (NULL == role)return;
	Protocol::CSGuildHallOfFameReq * req = (Protocol::CSGuildHallOfFameReq *)msg;

	Guild * m_guild = role->GetGuild();
	if (NULL != m_guild)
	{
		m_guild->HallOfFameReq(role, req->type, req->param);
	}
}

void MessageHandler::OnTaskSkipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (NULL == role)return;
	Protocol::CSTaskSkipReq * req = (Protocol::CSTaskSkipReq *)msg;

	role->GetRoleModuleManager()->GetTaskManager()->SkipTask(req->task_id);
}

void MessageHandler::OnPeaktokenRoleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPeaktokenRoleReq* req = (Protocol::CSPeaktokenRoleReq*)msg;

	if (NULL == scene)
	{
		return;
	}

	switch (req->req_type)
	{
	case Protocol::CSPeaktokenRoleReq::REQ_TYPE_ROLE_INFO:
		{
			role->GetRoleModuleManager()->GetRolePeakToken()->SendInfo();
		}
		break;
	case Protocol::CSPeaktokenRoleReq::REQ_TYPE_ROLE_JOIN:
		{
			Obj * act_obj = scene->GetObj(req->param_1);
			if (NULL == act_obj)
			{
				return;
			}

			if (Obj::OBJ_TYPE_PEAK_TOKEN_OBJ != act_obj->GetObjType())
			{
				return;
			}

			((PeakTokenObj *)act_obj)->RoleJoin(role);
		}
		break;
	case Protocol::CSPeaktokenRoleReq::REQ_TYPE_ROLE_FETCH:
		{
			Obj * act_obj = scene->GetObj(req->param_1);
			if (NULL == act_obj)
			{
				return;
			}

			if (Obj::OBJ_TYPE_PEAK_TOKEN_OBJ != act_obj->GetObjType())
			{
				return;
			}

			((PeakTokenObj *)act_obj)->RoleFetch(role);
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnIncomeRoleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSIncomeRoleReq* req = (Protocol::CSIncomeRoleReq*)msg;
	role->GetRoleModuleManager()->GetRoleIncome()->OnReq(req->req_type);
}

void MessageHandler::OnQingYuanShengHuiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSQingYuanShengHuiReq * req = (Protocol::CSQingYuanShengHuiReq *)msg;

	SpecialLogicQingYuanShengHui * spc_qing_yuan_sheng_hui = NULL;
	if (req->req_type != QING_YUAN_SHENG_HUI_REQ_TYPE_YU_YUE)
	{
		SpecialLogic * special_lobgic = scene->GetSpecialLogic();
		if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_QING_YUAN_SHENG_HUI)
		{
			return;
		}
		spc_qing_yuan_sheng_hui = (SpecialLogicQingYuanShengHui *)special_lobgic;
	}

	switch (req->req_type)
	{
		case QING_YUAN_SHENG_HUI_REQ_TYPE_YU_YUE:
			{
				
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_ENTER:
			{
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_GATHER:
			{
				spc_qing_yuan_sheng_hui->OnRoleGather(role);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_OPEN:
			{
				spc_qing_yuan_sheng_hui->OnRoleQuestionUiOp(role,true);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_ANSWER:
			{
				spc_qing_yuan_sheng_hui->OnRoleAnswerQuestion(role, req->p1);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_CLOSE:
			{
				spc_qing_yuan_sheng_hui->OnRoleQuestionUiOp(role, false);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI_OPEN:
			{
				spc_qing_yuan_sheng_hui->OnRoleBattleUi(role, true);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI_CLOSE:
			{
				spc_qing_yuan_sheng_hui->OnRoleBattleUi(role, false);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI:
			{
				spc_qing_yuan_sheng_hui->OnRoleBattleMatch(role, req->p1, true);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_QI_YUAN:
			{
				spc_qing_yuan_sheng_hui->OnQiYuan(role);
			}
			break;
		case QING_YUAN_SHENG_HUI_REQ_TYPE_BATTLE_REWARD:
			{
				spc_qing_yuan_sheng_hui->OnRoleFetchBattleReward(role, req->p1);
			}
			break;
	}
}

void MessageHandler::OnQingYuanShengHuiReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{

}

void MessageHandler::OnTuZiKuaiPaoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTuZiKuaiPaoReq * req = (Protocol::CSTuZiKuaiPaoReq *)msg;

	SpecialLogic * special_lobgic = scene->GetSpecialLogic();
	if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_TU_ZI_KUAI_PAO)
	{
		return;
	}
	SpecialLogicTuZiKuaiPao * tuzi_spec = (SpecialLogicTuZiKuaiPao *)special_lobgic;


	switch (req->req_type)
	{
		case TU_ZI_KUAI_PAO_REQ_TYPE_SET:
			{
				int tu_zi_type[TU_ZI_KUAI_PAO_MAX_TRACK_NUM] = { req->p1,req->p2,req->p3 };
				tuzi_spec->Set(role, tu_zi_type);
			}
			break;
		case TU_ZI_KUAI_PAO_REQ_TYPE_ENCOURAGE:
			{
				tuzi_spec->Encourage(role, req->p2, req->p1);
			}
			break;
		case TU_ZI_KUAI_PAO_REQ_TYPE_SET_OK:
			{
				tuzi_spec->SetReady(role);
			}
			break;
		case TU_ZI_KUAI_PAO_LEAVE:
			{
				tuzi_spec->Leave(role);
			}
			break;
	}
}

void MessageHandler::OnGotoSchoolReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSGotoSchoolReq * req = (Protocol::CSGotoSchoolReq *)msg;


	switch (req->req_type)
	{
		case GOTO_SCHOOL_REQ_TYPE_REQ_FIGHT:
			{

				SpecialLogic * special_lobgic = scene->GetSpecialLogic();
				if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_GOTO_SCHOOL)
				{
					return;
				}
				SpecialLogicGotoSchool * goto_spec = (SpecialLogicGotoSchool *)special_lobgic;
				goto_spec->Fight(role,req->p1);
			}
			break;
		case GOTO_SCHOOL_REQ_TYPE_REQ_SUCC:
			{
				role->GetRoleModuleManager()->GotoSchoolGetReward(1);
			}
			break;
		case GOTO_SCHOOL_REQ_TYPE_REQ_FIGHT_SUCC:
			{
				role->GetRoleModuleManager()->GotoSchoolGetReward(0);
			}
			break;
	}
}

void MessageHandler::OnYaoXingShiLianReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSYaoXingShiLianReq* req = (Protocol::CSYaoXingShiLianReq*)msg;
	switch (req->type)
	{
	case Protocol::CSYaoXingShiLianReq::REQ_TYPE_INFO:
		role->GetRoleModuleManager()->GetYaoXingShiLian()->SendInfo();
		break;
	case Protocol::CSYaoXingShiLianReq::REQ_TYPE_BATTLE:
		role->GetRoleModuleManager()->GetYaoXingShiLian()->OnStartBattle();
		break;
	case Protocol::CSYaoXingShiLianReq::REQ_TYPE_REWARD:
		role->GetRoleModuleManager()->GetYaoXingShiLian()->GetReward(req->param);
		break;
	case Protocol::CSYaoXingShiLianReq::REQ_TYPE_REWARD2:
		role->GetRoleModuleManager()->GetYaoXingShiLian()->GetReward2(req->param);
		break;
	}
}

void MessageHandler::OnShiTuChuanGongFBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSShiTuChuanGongFBReq* req = (Protocol::CSShiTuChuanGongFBReq*)msg;

	role->GetRoleModuleManager()->GetShiTu()->OnShiTuReq(req->req_type, req->p_1, req->p_2);
}

void MessageHandler::OnQingYuanDuiDuiPengReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSQingYuanDuiDuiPengReq* req = (Protocol::CSQingYuanDuiDuiPengReq*)msg;

	switch (req->req_type)
	{
	case Protocol::CSQingYuanDuiDuiPengReq::REQ_TYPE_RANK_INFO:
		{
			QingYuanDuiDuiPengManager::Instance().OnReqRank(role);
		}
		break;
	case Protocol::CSQingYuanDuiDuiPengReq::REQ_TYPE_DAY_COUNT:
		{
			QingYuanDuiDuiPengManager::Instance().SendRoleInfo(role);
		}
		break;
	default:
		{
			SpecialLogic * special_lobgic = scene->GetSpecialLogic();
			if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_QINGYUAN_DUIDUIPENG)
			{
				return;
			}
			SpecialLogicQingYuanDuiDuiPeng * special = (SpecialLogicQingYuanDuiDuiPeng *)special_lobgic;
			if (NULL == special) return;

			special->OnClientReq(role, msg);
		}
		return;
	}
}

void MessageHandler::OnLanternRiddieReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::LanternRiddieCSReq* req = (Protocol::LanternRiddieCSReq*)msg;
	role->GetRoleModuleManager()->GetLanternRiddie()->OnClientOpReq(req);
}

void MessageHandler::OnSanXianGuiDongReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (NULL == role)return;
	Protocol::CSSanXianGuiDongReq* req = (Protocol::CSSanXianGuiDongReq*)msg;
	if (!ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_SAN_XIAN_GUI_DONG))
	{
		role->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_IN_ACTIVE_TIME);
		return;
	}
	switch (req->type)
	{
	case Protocol::CSSanXianGuiDongReq::SAN_XIAN_INFO:
		role->GetRoleModuleManager()->GetSanXianGuiDong()->SendInfo();
		break;
	case Protocol::CSSanXianGuiDongReq::SAN_XIAN_START:
		role->GetRoleModuleManager()->GetSanXianGuiDong()->StarChoose();
		break;
	case Protocol::CSSanXianGuiDongReq::SAN_XIAN_CHOOSE:
		role->GetRoleModuleManager()->GetSanXianGuiDong()->Choose(req->param == 1);
		break;
	case Protocol::CSSanXianGuiDongReq::SAN_XIAN_FB:
		role->GetRoleModuleManager()->GetSanXianGuiDong()->EnterFb();
		break;
	default:
		break;
	}
}

void MessageHandler::OnWhereIsTheFairyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRoleWhereIsTheFairyOperaReq * req = (Protocol::CSRoleWhereIsTheFairyOperaReq *)msg;
	switch (req->opera_req_type)
	{
		case Protocol::CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetRoleWhereIsTheFairy()->SendAllInfo();
		}
		break;
		case Protocol::CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_FINISH_GAME:
		{
			role->GetRoleModuleManager()->GetRoleWhereIsTheFairy()->OnFinishGame();
		}
		break;
		case Protocol::CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_MANAGER_INFO:
		{
			WhereIsTheFairyManager::Instance().SendInfo(role);
		}
		break;
		default:
		break;
	}
}

void MessageHandler::OnTaoQuanNaJiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTaoQuanNaJiReq* req = (Protocol::CSTaoQuanNaJiReq*)msg;
	if (!ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_TAO_QUAN_NA_JI))
	{
		role->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_IN_ACTIVE_TIME);
		return;
	}
	switch (req->req_type)
	{
	case Protocol::CSTaoQuanNaJiReq::NA_JI_REQ_INFO:
		role->GetRoleModuleManager()->GetTaoQuanNaJi()->SendInfo();
		break;
	case Protocol::CSTaoQuanNaJiReq::NA_JI_REQ_START:
		role->GetRoleModuleManager()->GetTaoQuanNaJi()->Begain();
		break;
	case Protocol::CSTaoQuanNaJiReq::NA_JI_REQ_CHOOSE:
		role->GetRoleModuleManager()->GetTaoQuanNaJi()->TaoQuan(req->param1, req->param2);
		break;
	case Protocol::CSTaoQuanNaJiReq::NA_JI_REQ_FB:
		role->GetRoleModuleManager()->GetTaoQuanNaJi()->EnterFb();
		break;
	case Protocol::CSTaoQuanNaJiReq::NA_JI_REQ_LEAVE:
		role->GetRoleModuleManager()->GetTaoQuanNaJi()->LeaveFb();
		break;
	case Protocol::CSTaoQuanNaJiReq::NA_JI_REQ_PLAY_INFO:
		role->GetRoleModuleManager()->GetTaoQuanNaJi()->SendPlayInfo();
		break;
	default:
		break;
	}
	
}

void MessageHandler::OnHundredGodBossReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHundredGodBossReq * req = (Protocol::CSHundredGodBossReq *)msg;
	switch (req->req_type)
	{
		case Protocol::CSHundredGodBossReq::HUNDRED_GOD_BOSS_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetRoleHundredGodBoss()->SendAllInfo();
		}
		break;
		case Protocol::CSHundredGodBossReq::HUNDRED_GOD_BOSS_REQ_TYPE_FETCH_TARGET:
		{
			role->GetRoleModuleManager()->GetRoleHundredGodBoss()->OnFetchTargetReward(req->param);
		}
		break;
		case Protocol::CSHundredGodBossReq::HUNDRED_GOD_BOSS_REQ_TYPE_FINISH_REWARD:
		{
			role->GetRoleModuleManager()->GetRoleHundredGodBoss()->OnFecthFinishReward();
		}
		break;
		case Protocol::CSHundredGodBossReq::HUNDRED_GOD_BOSS_REQ_TYPE_START:
		{	
			role->GetRoleModuleManager()->GetRoleHundredGodBoss()->OnStart();
		}
		break;
		default:
		break;
	}


}

void MessageHandler::OnTianGongXunBaoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTianGongXunBaoClientOpReq * req = (Protocol::CSTianGongXunBaoClientOpReq *)msg;

	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_TIAN_GONG_XUN_BAO) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_TIAN_GONG_XUN_BAO))
	{
		return;
	}

	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ZHOU_QI))return;

	switch (req->opera_req_type)
	{
	case 0:
		{
			role->GetRoleModuleManager()->GetRoleTianGongXunBao()->SendInfo();
		}
		break;
	case 1:
		{
			role->GetRoleModuleManager()->GetRoleTianGongXunBao()->StartAction();
		}
		break;
	default:
		break;
	}
}

void MessageHandler::OnLoversMarketReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetLoversMarket()->OnClientReq(msg);
}

void MessageHandler::OnConcentricKnotReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetConcentricKnot()->OnClientReq(msg);
}

void MessageHandler::OnFetchPingJiaRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->FetchPingJiaReward();
}

void MessageHandler::OnZaoHuaEquipmentOperateReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetZaoHuaEquipManager()->OperatorMsg((Protocol::CSZaoHuaEquipmentOperaReq*)msg);
}

void MessageHandler::OnZaoHuaEquipmentDiscomposeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetZaoHuaEquipManager()->DisComposeList((Protocol::CSZaoHuaEquipmentDiscomposeReq*)msg);
}

void MessageHandler::OnHongMengTianJieReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHongMengTianJieReq* req = (Protocol::CSHongMengTianJieReq*)msg;

	switch (req->req_type)
	{
	case CSHongMengTianJieReq::REQ_TYPE_TALK:
	case CSHongMengTianJieReq::REQ_TYPE_SEND_REWARD:
		{
			SpecialLogic * special_lobgic = scene->GetSpecialLogic();
			if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_HONGMENG_TIANJIE)
			{
				return;
			}
			SpecialLogicHongMengTianJie * special = (SpecialLogicHongMengTianJie *)special_lobgic;
			if (NULL == special) return;

			special->OnReq(role, msg);
		}
		break;
	case CSHongMengTianJieReq::REQ_TYPE_ROLE_INFO:
		{
			role->GetRoleModuleManager()->GetRoleHongMengTianJie()->SendInfo();
		}
		break;
	case CSHongMengTianJieReq::REQ_TYPE_SHOP:
		{
			role->GetRoleModuleManager()->GetRoleHongMengTianJie()->OnBuyShop(req->p1, req->p2);
		}
		break;
	case CSHongMengTianJieReq::REQ_TYPE_WORLD_PASS:
		{
			WorldHongMengTianJieManager::Instance().SendPassTime(role);
		}
		break;
	case CSHongMengTianJieReq::REQ_TYPE_PASS_TEAM:
		{
			WorldHongMengTianJieManager::Instance().SendPassTeam(role, req->p1);
		}
		break;
	}
}

void MessageHandler::OnZaoHuaEquipmentReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSZaoHuaEquipmentReq * req = (Protocol::CSZaoHuaEquipmentReq *)msg;

	switch (req->req_type)
	{
		case ZAO_HUA_EQUIPMENT_REQ_TYPE_REFINE:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->Refine(req->param1);
			}
			break;
		case ZAO_HUA_EQUIPMENT_REQ_TYPE_HUN_LI_UP_LEVEL:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->HunLiUplv(req->param1, req->param2, req->param3);
			}
			break;
		case ZAO_HUA_LONG_ZHU_DRAW:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->LongZhuDraw(req->param1, req->param2, req->param3);
			}
			break;
		case ZAO_HUA_LONG_ZHU_PUT_ON:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->LongZhuPutOn(req->param1, req->param2, req->param3);
			}
			break;
		case ZAO_HUA_LONG_ZHU_CLEAR_1:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->LongZhuClear(req->param1, req->param2, req->param3);
			}
			break;
		case ZAO_HUA_LONG_ZHU_CLEAR_ALL:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->LongZhuClear(req->param1, req->param2, req->param3, true);
			}
			break;
		case ZAO_HUA_LONG_ZHU_UNLOCK:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->LongZhuUnlockSlot(req->param1, req->param2);
			}
			break;
		case ZAO_HUA_RU_MO:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->RuMo(req->param1, req->param2);
			}
			break;
		case ZAO_HUA_DECOMPOSE:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->YuanHunZhuDecompose(req->param1, req->param2);
			}
			break;
		case ZAO_HUA_COMPOSE:
			{
				role->GetRoleModuleManager()->GetZaoHuaEquipManager()->YuanHunZhuCompose(req->param1);
			}
			break;
	}
}

void MessageHandler::OnZaoHuaYuanHunZhuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSZaoHuaYuanHunZhuReq * req = (Protocol::CSZaoHuaYuanHunZhuReq *)msg;

	int ret = 0;
	switch (req->req_type)
	{
		case ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE_PUT_ON:
			{
				ret = role->GetRoleModuleManager()->GetZaoHuaEquipManager()->YuanHunZhuPutOn(
					req->param1, req->param2, req->param3, req->param4, req->param5, req->param6, req->param7, req->param8);
			}
			break;
		case ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE_TAKE_OFF:
			{
				ret = role->GetRoleModuleManager()->GetZaoHuaEquipManager()->YuanHunZhuTakeOff(req->param1, req->param2, req->param3, req->param4);
			}
			break;
	}

	if (ret != 0)
	{
		gamelog::g_log_yuan_hun_zhu.printf(LL_INFO, "%s line:%d | role[%d %s] req->req_type:%d ret:%d", __FUNCTION__, __LINE__,
			role->GetUID(), role->GetName(), req->req_type, ret);
	}
}

void MessageHandler::OnZaoHuaYuanHunZhuUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSYuanHunZhuUpLevelReq * req = (Protocol::CSYuanHunZhuUpLevelReq *)msg;

	int ret = role->GetRoleModuleManager()->GetZaoHuaEquipManager()->YuanHunZhuUpLevel(req->item_src, req->index, req->column, req->slot_index);

	if (ret != 0)
	{
		gamelog::g_log_yuan_hun_zhu.printf(LL_INFO, "%s line:%d | role[%d %s]  ret:%d", __FUNCTION__, __LINE__,
			role->GetUID(), role->GetName(), ret);
	}
}

void MessageHandler::OnHolyEquipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHolyEquipOpReq * req = (Protocol::CSHolyEquipOpReq *)msg;

	role->GetRoleModuleManager()->GetHolyEquipManager()->ClientReq(req);
}

void MessageHandler::OnHolyItemLevelUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSHolyEquipLevelUpReq * req = (Protocol::CSHolyEquipLevelUpReq *)msg;

	role->GetRoleModuleManager()->GetHolyEquipManager()->HolyItemLevelUp(req);
}

void MessageHandler::OnKeJuExamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSKeJuExamCSReq * req = (Protocol::CSKeJuExamCSReq *)msg;

	role->GetRoleModuleManager()->GetKeJuExam()->OnClientOpReq(req);
}

void MessageHandler::OnPartnerPetRankReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPartnerPetRankReq * req = (Protocol::CSPartnerPetRankReq *)msg;

	// 0--宠物排行, 1--伙伴排行
	if (req->rank_type == 0)
	{
		RankManager::Instance().SendPetTopRankInfo(role->GetUID());
	}
	else if (req->rank_type == 1)
	{
		RankManager::Instance().SendPartnerTopRankInfo(role->GetUID());
	}
}

void MessageHandler::OnFuYuManagerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSFuYuReq * req = (Protocol::CSFuYuReq *)msg;

	role->GetRoleModuleManager()->GetFuYuManager()->OnClientReq(req);
}

void MessageHandler::OnRAYueXiDengMiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRAYueXiDengMiRequest * req = (Protocol::CSRAYueXiDengMiRequest *)msg;

	RoleActivityYueXiDengMi * yue_xi_deng_mi = static_cast<RoleActivityYueXiDengMi *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI));
	if (NULL != yue_xi_deng_mi)
	{
		yue_xi_deng_mi->OnRandActivityOpRequest(req);
	}
}

void MessageHandler::OnKuaFuYingXiongLuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSKuaFuYingXiongLuReq * req = (Protocol::CSKuaFuYingXiongLuReq*)msg;

	role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnCheckInitActivity();
	switch (req->req_type)
	{
		case Protocol::CSKuaFuYingXiongLuReq::REQ_TYPE_ALL_INFO:
		{
			KuaFuYingXiongLuManager::Instance().SendAllInfo(role);
			role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->SendAllInfo();
		}
		break;
		case Protocol::CSKuaFuYingXiongLuReq::REQ_TYPE_ACTIVITY_INFO:
		{
			KuaFuYingXiongLuManager::Instance().SendAllInfo(role);
		}
		break;
		case Protocol::CSKuaFuYingXiongLuReq::REQ_TYPE_ROLE_ALL_INFO:
		{
			role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->SendAllInfo();
		}
		break;
		case Protocol::CSKuaFuYingXiongLuReq::REQ_TYPE_FETCH_TASK_REWARD:
		{
			role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnFetchTaskReward(req->param1, req->param2);
		}
		break;
		case Protocol::CSKuaFuYingXiongLuReq::REQ_TYPE_MY_RANK_VALUE:
		{
			role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->SendRoleRankValue();
		}
		break;
		case Protocol::CSKuaFuYingXiongLuReq::REQ_TYPE_CHECK_ROLE_INIT:	//检测在最前面
		break;
		default:
		break;
	}
}

void MessageHandler::OnNewCourageChallengeOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_NEW_COURAGE_CHALLENGE, true))
	{
		return;
	}
	Protocol::CSNewCourageChallengeOp * req = (Protocol::CSNewCourageChallengeOp *)msg;
	switch (req->op_type)
	{
	case Protocol::CSNewCourageChallengeOp::OP_TYPE_LEVEL_INFO:
		role->GetRoleModuleManager()->GetNewCourageChallenge()->SendLevelListInfo();
		break;
	case Protocol::CSNewCourageChallengeOp::OP_TYPE_REWARD_INFO:
		role->GetRoleModuleManager()->GetNewCourageChallenge()->SendRewardInfo();
		break;
	case Protocol::CSNewCourageChallengeOp::OP_TYPE_FETCH_COMMON:
		role->GetRoleModuleManager()->GetNewCourageChallenge()->OnFetchCommomReward(req->param);
		break;
	case Protocol::CSNewCourageChallengeOp::OP_TYPE_FETCH_TOKEN:
		role->GetRoleModuleManager()->GetNewCourageChallenge()->OnFetchTokenReward(req->param);
		break;
	case Protocol::CSNewCourageChallengeOp::OP_TYPE_FETCH_ALL:
		role->GetRoleModuleManager()->GetNewCourageChallenge()->OnFetchOneKey();
		break;
	}
}

void MessageHandler::OnStarChartOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_STAR_CHART, true)) return;
	Protocol::CSStarChartRequest* req = (Protocol::CSStarChartRequest*)msg;
	role->GetRoleModuleManager()->GetStarChart()->OnClientRequest(*req);
}

void MessageHandler::OnFashionCollectionOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->GetFashionCollection()->OnClientRequest(msg);
}

void MessageHandler::OnHuanShouReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ROLE_HUAN_SHOU, true))
	{
		return;
	}
	Protocol::CSHuanShouReq* req = (Protocol::CSHuanShouReq*)msg;
	switch (req->op_type)
	{
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_INFO:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->SendHuanShouAllInfo();
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnSendBagAllInfo();
			role->GetRoleModuleManager()->GetRoleHuanShou()->SendOtherInfo();
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_OPEN_EGG:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnOpenHuanShouEgg(req->param1, req->param2);
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_STR_LEVEL:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnStrHuanShouLevel(req->param1);
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_LEARN_SKILL:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnLearnSkill(req->param1, req->param2, req->param3);
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_SAVE_SKILL:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnSaveSkill(req->param1, (0 != req->param2));
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_P_POINT_LEVEL_UP:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnLearnPPointLevelUp(req->param1, req->param2);
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_LIAN_DAN:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnLianDan();
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_FU_SHEN:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnFuShen(req->param1);
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_CANCEL_FU_SHEN:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnCancelFuShen();
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_ANNOUNCEMENT_INFO:
		{
			WorldStatus::Instance().OnSendHuanShouAnnInfo(role);
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_SINGLE_HUANSHOU:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->SendHuanShouSingleInfo(req->param1);
		}
		break;
		case Protocol::CSHuanShouReq::HUAN_SHOU_REQ_TYPE_USE_HUAN_SHOU_IMAGE:
		{
			role->GetRoleModuleManager()->GetRoleHuanShou()->OnSetCurUseImage(0 != req->param1);
		}
		break;
		default:
		break;
	}

}

void MessageHandler::OnHuanShouDecomposeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ROLE_HUAN_SHOU, true))
	{
		return;
	}
	Protocol::CSHuanShouDecomposeReq* req = (Protocol::CSHuanShouDecomposeReq*)msg;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::CSHuanShouDecomposeReq, decompose_list));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->decompose_list[0]);
	if (count <= 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->decompose_list))
	{
		return;
	}

	std::set<short> list;
	for (int i = 0; i < count; ++i)
	{
		list.insert(req->decompose_list[i]);
	}
	role->GetRoleModuleManager()->GetRoleHuanShou()->OnHuanShouItemDecompose(list, req->bag_column);
}

void MessageHandler::OnAnecdoteSpecialReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSSpecialAnecdoteReq * req = (Protocol::CSSpecialAnecdoteReq *)msg;
	role->GetRoleModuleManager()->GetAnecdoteTask()->StarSpecialTask(req->task_type, req->task_seq);
}

void MessageHandler::OnRoleGuildTaskReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSRoleGuildTaskReq * req = (Protocol::CSRoleGuildTaskReq *)msg;
	switch (req->req_type)
	{
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_ACCEPT:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqStartTask(req->param1);
		}
		break;
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_GIVE_UP:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqGiveUpTask(req->param1);
		}
		break;
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_SOLICIT:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqSolicit();
		}
		break;
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_GIVE:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqGive(req->param1);
		}
		break;
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_REFRESH:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqRefresh();
		}
		break;
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_BATTLE:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqBattle();
		}
		break;
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_SEND_MAIL:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqMailNotice(req->param1 != 0, req->param2);
		}
		break;
		case Protocol::ROLE_GUILD_TASK_REQ_TYPE_FETCH:
		{
			role->GetRoleModuleManager()->GetRoleGuildTask()->ReqFetch();
		}
		break;
	}
}

void MessageHandler::OnReceiveRealNameRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	role->GetRoleModuleManager()->ReceiveRealNameReward();
}

void MessageHandler::OnMiniGameFetchPhaseRewardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSMiniGameClaimPhaseReward* req = (Protocol::CSMiniGameClaimPhaseReward*)msg;
	role->GetRoleModuleManager()->GetMiniGame()->OnFetchPhaseRewardReq(req->phase_reward_seq);
}

void MessageHandler::OnMiniGameClearNextStageReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	role->GetRoleModuleManager()->GetMiniGame()->OnFinishNextStageReq();
}

void MessageHandler::OnMiniGameStartBattleReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSMiniGameStartBattle* req = (Protocol::CSMiniGameStartBattle*)msg;
	role->GetRoleModuleManager()->GetMiniGame()->OnStartMiniGameBattleReq(req->seq, req->has_pet, req->has_partner, req->has_skill);
}

void MessageHandler::OnWorldTeamArenaReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSWorldTeamArenaReq * req = (Protocol::CSWorldTeamArenaReq *)msg;
	if (CrossConfig::Instance().IsHiddenServer())
	{
	switch (req->op_type)
	{
		case Protocol::CSWorldTeamArenaReq::OP_TYPE_SIGN_UP_ALL_INFO:
		{
			WorldTeamArenaSignUpManager::Instance().SendAllSignUpInfo(role);
			}
		break;
		case Protocol::CSWorldTeamArenaReq::OP_TYPE_MEMBER_SIGN_UP:
		{
			WorldTeamArenaSignUpManager::Instance().OnTeamMemberSignUp(role);
		}
		break;
		case Protocol::CSWorldTeamArenaReq::OP_TYPE_CANCEL_SIGN_UP:
		{
			WorldTeamArenaSignUpManager::Instance().OnCancelTeamSignUp(role);
		}
		break;
		case Protocol::CSWorldTeamArenaReq::OP_TYPE_SIGN_UP_DETAILED_INFO:
		{
			WorldTeamArenaSignUpManager::Instance().OnQueryTeamDetailedInfo(role, req->param1, req->param2);
		}
		break;
		case  Protocol::CSWorldTeamArenaReq::OP_TYPE_RANK_DETAILED_INFO:
		{
			WorldTeamArenaRankManager::Instance().OnQueryTeamDetailedInfo(role, req->param1);
		}
		break;
		case Protocol::CSWorldTeamArenaReq::OP_TYPE_TEAM_SPECIAL_INFO:
		{
			WorldTeamArenaSignUpManager::Instance().OnSendTeamSpecialInfo(role);
		}
		break;
		default:
		break;
	}
	}
	if (Protocol::CSWorldTeamArenaReq::OP_TYPE_OTHER_INFO == req->op_type)
	{
		WorldTeamArenaSignUpManager::Instance().SendOtherInfo(role);
	}
}

void MessageHandler::OnWorldTeamArenaSignUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::CSWorldTeamArenaSignUpReq * req = (Protocol::CSWorldTeamArenaSignUpReq *)msg;

	if (0 == req->name[0]) return;

	req->name[sizeof(req->name) - 1] = 0;

	static const bool NAMEFILTER_OPEN_FLAG = ServerConfigPool::Instance().common_config.namefilter_open_flag;
	if (NAMEFILTER_OPEN_FLAG && NameFilter::Instance().HasSensitiveWord(sizeof(GameName), req->name)) 
	{
		role->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_NAME);
		return;
	}
	switch (req->op_type)
	{
		case Protocol::CSWorldTeamArenaSignUpReq::OP_TYPE_SIGN_UP:
		{
			WorldTeamArenaSignUpManager::Instance().OnTeamSignUp(role, req->name);
		}
		break;
		case Protocol::CSWorldTeamArenaSignUpReq::OP_TYPE_QUERY_TEAM:
		{
			WorldTeamArenaSignUpManager::Instance().OnQueryTeamDetailedInfo(role, req->name);
		}
		break;
		default:
		break;
	}
}

void MessageHandler::OnWorldTeamArenaSceneReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	SpecialLogic * special_lobgic = scene->GetSpecialLogic();
	if (special_lobgic == NULL || special_lobgic->GetSceneType() != Scene::SCENE_TYPE_WORLD_TEAM_ARENA)
	{
		return;
	}
	SpecialLogicWorldTeamArena * world_team_arena_scene = (SpecialLogicWorldTeamArena *)special_lobgic;
	if (NULL == world_team_arena_scene) return;

	Protocol::CSActivityWorldTeamArenaSceneReq * req = (Protocol::CSActivityWorldTeamArenaSceneReq *)msg;

	switch (req->op_type)
	{
		case Protocol::CSActivityWorldTeamArenaSceneReq::WTA_OP_TYPE_REQ_ALL_RANK_INFO:
		world_team_arena_scene->RequestFightRankInfo(role);
		break;
		case Protocol::CSActivityWorldTeamArenaSceneReq::WTA_OP_TYPE_REQ_CLOSE_RANK:
		world_team_arena_scene->OnCloseRank(role);
		break;
		case Protocol::CSActivityWorldTeamArenaSceneReq::WTA_OP_TYPE_REQ_FIGHT_OTHER:
		world_team_arena_scene->OnInitiateFight(role, req->param1);
		break;
	}

}

void MessageHandler::OnWorldTeamArenaFinallyRankReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	WorldTeamArenaRankManager::Instance().OnSendAllRankInfo(role);
}

void MessageHandler::OnPetStrengthenRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPetStrengthenRewardReq* req = (Protocol::CSPetStrengthenRewardReq*)msg;
	role->GetRoleModuleManager()->GetDayReward()->OnFetchPetStrengthenReward(req->seq);
}

void MessageHandler::OnPartnerIntensifyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSPartnerIntensifyReq * req = (Protocol::CSPartnerIntensifyReq *)msg;
	role->GetRoleModuleManager()->GetPartner()->OnPartnerIntensify(req->partner_id, req);
}

void MessageHandler::OnGetUTAChampionRecord(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSGetUTAChampionRecord* get_utacr = (Protocol::CSGetUTAChampionRecord*)msg;

	UTAChampionRecordManager::Instance().OnGetUTAChampionRecord(role, get_utacr);
}

void MessageHandler::OnUTAGuessClientReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length)
{
	Protocol::CSUTAGuessClienReq* req = (Protocol::CSUTAGuessClienReq*)msg;

	UTAGuessShadow::Instance().OnClientReq(role, req);
}

void MessageHandler::OnUTATeamListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTATeamListReq* req = (Protocol::CSUTATeamListReq*)msg;
	UTAShadow::Instance().OnTeamListReq(role);
}

void MessageHandler::OnUTATeamDetailInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTATeamDetailReq* req = (Protocol::CSUTATeamDetailReq*)msg;
	UTAShadow::Instance().OnTeamDetailInfoReq(role, req->team_unique_id);
}

void MessageHandler::OnUTAFetchWinBoxReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTAFetchWinBox* req = (Protocol::CSUTAFetchWinBox*)msg;
	UTAShadow::Instance().OnFetchWinBoxReq(role, req->sn);
}

void MessageHandler::OnUTAMyTeamQualificationSchedulesReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	UTAShadow::Instance().OnMyTeamQualificationSchedulesReq(role);
}

void MessageHandler::OnUTAGetTeamQualificationRankListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	UTAShadow::Instance().OnTeamQualificationRankListReq(role);
}

void MessageHandler::OnUTAGetMainScheduleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTAMainSchedules* req = (Protocol::CSUTAMainSchedules*)msg;
	UTAShadow::Instance().OnMainSchedulesReq(role, req->round_idx);
}

void MessageHandler::OnUTAFinalRankListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	UTAShadow::Instance().OnFinalRankListReq(role);
}

void MessageHandler::OnUTAGetQualificationSchedulesReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTAQualificationSchedules* req = (Protocol::CSUTAQualificationSchedules*)msg;
	UTAShadow::Instance().OnGetQualificationSchedulesReq(role, req->round_idx);
}

void MessageHandler::OnUTAGetMainSchedulesOverviewReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTAMainSchedulesOverview* req = (Protocol::CSUTAMainSchedulesOverview*)msg;
	UTAShadow::Instance().OnGetMainSchedulesOverviewReq(role, req->overview_type, req->is_last_season > 0);
}

void MessageHandler::OnUTAEnterBattleSceneReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	UTAShadow::Instance().TryEnterBattleScene(role);
}

void MessageHandler::OnUTAWatchVideoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTAWatchVideo* req = (Protocol::CSUTAWatchVideo*)msg;
	UTAShadow::Instance().OnUserWatchVideoReq(role->GetUserId(), req->video_id);
}

void MessageHandler::OnUTAStartOBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSUTAStartOB* req = (Protocol::CSUTAStartOB*)msg;
	UTAOBShadow::Instance().StartOB(role, req->schedule_id);
}

void MessageHandler::OnRoleAnswerTeamRecruitReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	Protocol::CSTeamRoleRecruitAnswer* req = (Protocol::CSTeamRoleRecruitAnswer*)msg;

	TeamRecruit::Instance().OnRoleAnswerRecruit(role, req);
}

void MessageHandler::OnQueryUserChatRecord(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length)
{
	//ChatManager::Instance().QueryUserChatRecord(role);
}

void MessageHandler::OnMiniGameSetMiniGameIdReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length)
{
	Protocol::CSMiniGameSetMiniGameId* req = (Protocol::CSMiniGameSetMiniGameId*)msg;

	role->GetRoleModuleManager()->GetMiniGame()->OnSetMiniGameIdReq(req->mini_game_id);
}