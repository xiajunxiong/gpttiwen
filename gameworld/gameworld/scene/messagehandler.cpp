#include "scenemanager.hpp"
#include "messagehandler.hpp"
#include "gamelog.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "timer/forcekickuserdelay.hpp"

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/msgcode.h"
#include "servercommon/userprotocal/globalmsgcode.h"
#include "servercommon/internalprotocal/ggprotocal.h"
#include "servercommon/userprotocal/battlemsgcode.h"
#include "servercommon/userprotocal/msgbattle.h"
#include "servercommon/userprotocal/msgchatmsg.h"

#include "scene/scene.h"
#include "scene/delayloginmanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/mapunlock/mapunlockconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/cross/crossuserregister.hpp"
#include "global/cross/crossusershadow.hpp"
#include "world.h"
#include "servercommon/performancecatch.hpp"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"

#include "protocol/msgescort.h"
#include "protocol/msgentergs.h"
#include "protocol/msgserver.h"
#include "protocol/msgrole.h"
#include "protocol/msggm.hpp"
#include "protocol/msgitem.h"
#include "protocol/msgteam.h"
#include "protocol/msgpet.h"
#include "protocol/msgprestige.h"
#include "protocol/msgpartner.h"
#include "protocol/msgtitle.h"
#include "protocol/msgtrain.h"
#include "protocol/msgcollection.h"
#include "protocol/msgtask.h"
#include "protocol/msgscene.h"
#include "protocol/msgother.h"
#include "protocol/msgskill.h"
#include "protocol/msgequipment.h"
#include "protocol/msgworkshop.h"
#include "protocol/msgsurface.h"
#include "protocol/msgjewelry.h"
#include "protocol/msgmarry.hpp"
#include "protocol/msgguild.hpp"
#include "protocol/msgmail.hpp"
#include "protocol/msgfriend.h"
#include "protocol/msgbaby.hpp"
#include "protocol/msgshitu.h"
#include "protocol/msgcouragechallenge.h"
#include "protocol/msgnpc.h"
#include "protocol/msgschooltask.h"
#include "protocol/msgofflineexp.h"
#include "protocol/msgworldboss.h"
#include "protocol/msgworldboss2.h"
#include "protocol/msgworldboss3.hpp"
#include "protocol/msgtreasuremap.h"
#include "protocol/msgtrademarket.h"
#include "protocol/msgfabao.hpp"
#include "protocol/msgresearchtask.h"
#include "protocol/msgmedal.h"
#include "protocol/msgcrystalfb.h"
#include "protocol/msgshop.h"
#include "protocol/msgactivity.hpp"
#include "protocol/msgmapunlock.h"
#include "protocol/msgcolosseum.h"
#include "protocol/msgbraveground.h"
#include "protocol/msgrandactivity.hpp"
#include "protocol/msgchiefelection.h"
#include "protocol/msgchiefpeakbattle.h"
#include "protocol/msgguildhonorbattle.h"
#include "protocol/msgcourseofgrowth.h"
#include "protocol/msgplatformbattle.h"
#include "protocol/msgfacescore.hpp"
#include "protocol/msgfuncguide.h"
#include "protocol/msgbosschapter.h"
#include "protocol/msgfirstkill.h"
#include "protocol/msgcsrank.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "chat/chatmanager.h"
#include "servercommon/userprotocal/crossmsgcode.h"
#include "protocol/msgwildboss.h"
#include "protocol/msghuanjiezhanchang.h"
#include "protocol/msgmiyi.h"
#include "protocol/msgshanhaiboss.hpp"
#include "protocol/msgpoker.hpp"
#include "protocol/msgdraw.h"
#include "protocol/msgschoolexercise.h"
#include "protocol/msgsmartmountssystem.hpp"
#include "protocol/msgcloudarena.h"
#include "protocol/msgmarket.hpp"
#include "protocol/msgheartskill.hpp"
#include "protocol/msgzhuling.h"
#include "protocol/msgpetgodfb.hpp"
#include "protocol/msgdujie.hpp"
#include "protocol/msghongbao.hpp"
#include "protocol/msgtiandidaohen.h"
#include "protocol/msggift.h"
#include "protocol/msgjieyi.h"
#include "protocol/msglifeskill.hpp"
#include "protocol/msgfishground.hpp"
#include "protocol/msgworldteamarena.hpp"
#include "protocol/msgqingyuanshenghui.h"
#include "protocol/msgkuafuyingxionglu.hpp"

#include "protocol/msgformation.h"
#include "protocol/randactivity/msgrasevencolorpicture.hpp"
#include "protocol/randactivity/msgrajoyseeksfavors.hpp"
#include "protocol/msglevelcomplement.h"
#include "protocol/msgpetmonopoly.h"
#include "scene/teamkeyregister/teamkeyregister.hpp"
#include "protocol/msgmoveassist.h"
#include "dbdisconnectrolesavecache.hpp"
#include "protocol/msgrolejoinactivityreward.hpp"
#include "protocol/msgaccessories.h"
#include "protocol/msgyinyangeye.h"
#include "protocol/msgcowreportgoodnews.hpp"
#include "protocol/msgadvancecareer.h"
#include "protocol/msgwanlingfb.h"
#include "protocol/msgxiuwei.hpp"
#include "protocol/msgtigerbringsblessing.hpp"
#include "protocol/msgqingyuan.h"
#include "protocol/msgworldarena.hpp"
#include "protocol/msgminigame.hpp"
#include "protocol/msgpeaktoken.hpp"
#include "protocol/msgincome.hpp"
#include "global/worldarena/worldarena.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "servercommon/userprotocal/msgutachampionrecord.h"
#include "servercommon/userprotocal/msgutaguess.h"
#include "servercommon/userprotocal/msguta.h"
#include "protocol/msghongmengtianjie.hpp"

#include "protocol/cycleactivity/msgqingyuanduiduipeng.hpp"
#include "protocol/cycleactivity/msglanternriddie.hpp"
#include "protocol/cycleactivity/msgwhereisthefairy.hpp"
#include "protocol/cycleactivity/msgtiangongxunbao.hpp"
#include "protocol/cycleactivity/msgconcentricknot.hpp"
#include "protocol/cycleactivity/msgtuzikuaipao.hpp"
#include "protocol/cycleactivity/msggotoschool.hpp"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "protocol/msgzaohuaequipment.h"
#include "protocol/msgholyequip.h"
#include "protocol/cycleactivity/msgkejuexam.hpp"
#include "protocol/msgfuyu.h"
#include "protocol/msgnewcouragechallenge.h"
#include "protocol/msgstarchart.h"
#include "protocol/msghuanshou.hpp"
#include "protocol/msgfashioncollection.h"
#include "protocol/randactivity/msgrayuexidengmi.hpp"
#include "protocol/msgroleguildtask.h"

#define GAMEWORLD_PERFORMANCE_CHECK


#define MESSAGE_HANDLER_CONSTRUCT(MSGCODE, FUNCTION_NAME, MSGSTRUCT)\
m_msg_handler_list[Protocol::MSGCODE] = HandlerItem(&MessageHandler::FUNCTION_NAME, sizeof(Protocol::MSGSTRUCT));


MessageHandler::MessageHandler() : m_scene_manager(NULL)
{
	memset(m_msg_handler_list, 0, sizeof(m_msg_handler_list));

	m_msg_handler_list[Protocol::MT_LOGOUT_CS]				= HandlerItem(&MessageHandler::OnLogout, sizeof(Protocol::CSUserLogout));
	m_msg_handler_list[Protocol::MT_ALL_INFO_REQ_CS]		= HandlerItem(&MessageHandler::OnAllInfoReq, sizeof(Protocol::CSAllInfoReq));

	m_msg_handler_list[Protocol::MT_UNFULL_TEAM_LIST_BY_TYPE_REQ_CS] = HandlerItem(&MessageHandler::OnUnfullTeamListByTypeReq, sizeof(Protocol::CSUnfullTeamListByTypeReq));
	m_msg_handler_list[Protocol::MT_CHANGE_TEAM_LIMIT_CS] = HandlerItem(&MessageHandler::OnChangeTeamLimit, sizeof(Protocol::CSChangeTeamLimit));
	m_msg_handler_list[Protocol::MT_CREATE_TEAM_CS] = HandlerItem(&MessageHandler::OnCreateTeamReq, sizeof(Protocol::CSCreateTeam));
	m_msg_handler_list[Protocol::MT_DISMISS_TEAM_CS] = HandlerItem(&MessageHandler::OnDismissTeamReq, sizeof(Protocol::CSDismissTeam));
	m_msg_handler_list[Protocol::MT_INVITE_USER_CS] = HandlerItem(&MessageHandler::OnInviteJoinTeamReq, sizeof(Protocol::CSInviteJoinReq));
	m_msg_handler_list[Protocol::MT_EXIT_TEAM_CS] = HandlerItem(&MessageHandler::OnExitTeamReq, sizeof(Protocol::CSExitTeamReq));
	m_msg_handler_list[Protocol::MT_KICK_OUT_OF_TEAM_CS] = HandlerItem(&MessageHandler::OnKickOutOfTeamReq, sizeof(Protocol::CSKickOutOfTeamReq));
	m_msg_handler_list[Protocol::MT_REQ_JOIN_TEAM_CS] = HandlerItem(&MessageHandler::OnJoinTeamReq, sizeof(Protocol::CSJoinTeamReq));
	m_msg_handler_list[Protocol::MT_REQ_JOIN_TEAM_RET_CS] = HandlerItem(&MessageHandler::OnJoinTeamReqRet, sizeof(Protocol::CSJoinTeamReqRet));
	m_msg_handler_list[Protocol::MT_INVITE_JOIN_TEAM_RET_CS] = HandlerItem(&MessageHandler::OnInviteJoinTeamRet, sizeof(Protocol::CSInviteJoinRet));
	m_msg_handler_list[Protocol::MT_TEAM_MATCH_REQ_CS] = HandlerItem(&MessageHandler::OnTeamMatch, sizeof(Protocol::CSTeamMatchOp));
	m_msg_handler_list[Protocol::MT_CHANGE_TEAM_LEADER_CS] = HandlerItem(&MessageHandler::OnChangeTeamLeader, sizeof(Protocol::CSChangeTeamLeader));
	m_msg_handler_list[Protocol::MT_CHANGE_MUST_CHECK_CS] = HandlerItem(&MessageHandler::OnChangeMustCheck, sizeof(Protocol::CSChangeMustCheck));
	m_msg_handler_list[Protocol::MT_TEAM_TEMPORARILY_PART_CS] = HandlerItem(&MessageHandler::OnTemporarilyPartReq, sizeof(Protocol::CSTemporarilyPartReq));
	m_msg_handler_list[Protocol::MT_TEAM_INVITATION_REQ_CS] = HandlerItem(&MessageHandler::OnTeamInvitationReq, sizeof(Protocol::CSTeamInvitationReq));
	m_msg_handler_list[Protocol::MT_TEAM_INVITATION_RET_CS] = HandlerItem(&MessageHandler::OnTeamInvitationRet, sizeof(Protocol::CSTeamInvitationRet));
	m_msg_handler_list[Protocol::MT_TEAM_ENTER_FB_RET_CS] = HandlerItem(&MessageHandler::OnTeamEnterFbReq, sizeof(Protocol::CSTeamEnterFbRet));
	m_msg_handler_list[Protocol::MT_TEAM_RECRUIT_CS] = HandlerItem(&MessageHandler::OnTeamRecruitReq, sizeof(Protocol::CSTeamRecruit));
	m_msg_handler_list[Protocol::MT_TEAM_SWITCH_TO_CROSS] = HandlerItem(&MessageHandler::OnTeamSwitchToCrossReq, sizeof(Protocol::CSTeamSwitchToCross));
	m_msg_handler_list[Protocol::MT_TEAM_LEADER_BATTLE_PARTNER_LIST_CS] = HandlerItem(&MessageHandler::OnTeamLeaderPartnerInfoReq, sizeof(Protocol::CSLeaderPartnerInfo));
	m_msg_handler_list[Protocol::MT_TEAM_LEADER_CHECK_MEMBER_ITEM_CS] = HandlerItem(&MessageHandler::OnLeaderCheckMemberItemReq, sizeof(Protocol::CSLeaderCheckMemberItem));
	m_msg_handler_list[Protocol::MT_REQ_JOIN_TEAM_INFO_CS] = HandlerItem(&MessageHandler::OnReqJoinTeamInfo, sizeof(Protocol::CSReqJoinTeamInfo));

	m_msg_handler_list[Protocol::MT_GM_COMMAND_CS]			= HandlerItem(&MessageHandler::OnGmCommand, sizeof(Protocol::CSGMCommand));
	m_msg_handler_list[Protocol::MT_PET_OP_CS] = HandlerItem(&MessageHandler::OnPetOp, sizeof(Protocol::CSPetOp));
	m_msg_handler_list[Protocol::MT_ABANDON_PET_BY_LIST_CS] = HandlerItem(&MessageHandler::OnAbandonPetByList, sizeof(Protocol::CSAbandonPetByList));
	
	m_msg_handler_list[Protocol::MT_PET_RENAME_CS] = HandlerItem(&MessageHandler::OnPetRename, sizeof(Protocol::CSPetRename));
	m_msg_handler_list[Protocol::MT_PET_ASSIGN_ATTRIBUTE_POINTS_CS] = HandlerItem(&MessageHandler::OnPetAssignAttributePoints, sizeof(Protocol::CSPetAssignAttributePoints));

	m_msg_handler_list[Protocol::MT_TASK_SINGLE_HANDLE_CS] = HandlerItem(&MessageHandler::OnTaskInfoSingltHandle, sizeof(Protocol::CSTaskInfoSingleHandle));
	m_msg_handler_list[Protocol::MT_TASK_TALK_WITH_NPC_CS] = HandlerItem(&MessageHandler::OnTalkWithAppointedNPC, sizeof(Protocol::CSTalkWithAppointedNPC));
	m_msg_handler_list[Protocol::MT_TASK_GIVE_NPC_ITEM_CS] = HandlerItem(&MessageHandler::OnGiveNPCTaskNeedItems, sizeof(Protocol::CSGiveNPCTaskNeedItems));
	m_msg_handler_list[Protocol::MT_TASK_SET_TRACE_REQ_CS] = HandlerItem(&MessageHandler::OnSetupRaskTraceStatus, sizeof(Protocol::CSSetupTaskTraceStatus));

	m_msg_handler_list[Protocol::MT_PARTNER_HANDLE_REQUEST_CS] = HandlerItem(&MessageHandler::OnPartnerHandleReq, sizeof(Protocol::CSPartnerHandleRequest));
	m_msg_handler_list[Protocol::MT_PARTNER_SKILL_SELECTOR_CS] = HandlerItem(&MessageHandler::OnPartnerSkillSave, sizeof(Protocol::CSToSaveSkillSelection));
	m_msg_handler_list[Protocol::MT_PARTNER_SYN_ALTER_POSI_CS] = HandlerItem(&MessageHandler::OnPartnerAlterList, sizeof(Protocol::CSChangeSyncFightOrder));
	m_msg_handler_list[Protocol::MT_PARTNER_EQUIP_OPERATOR_CS] = HandlerItem(&MessageHandler::OnPartnerEqRequest, sizeof(Protocol::CSPartnerEqOperatorReq));
	m_msg_handler_list[Protocol::MT_PARTNER_SWITCH_FIGHT_POS_CS] = HandlerItem(&MessageHandler::OnPartnerSwitchFightPos, sizeof(Protocol::CSPartnerSwitchFightPos));
	m_msg_handler_list[Protocol::MT_PARTNER_QUALITY_UP_INFO_CS] = HandlerItem(&MessageHandler::ONPartnerQualityUpgrade, sizeof(Protocol::CSPartnerQualityUpReq)); 
	m_msg_handler_list[Protocol::MT_PARTNER_ELEMENT_CRYTAL_REQ_CS] = HandlerItem(&MessageHandler::OnPartnerElementCrytalReq, sizeof(Protocol::CSPartnerElementCrytalOper));
	m_msg_handler_list[Protocol::MT_PARTNER_SKIN_REQ_CS] = HandlerItem(&MessageHandler::OnPartnerSkinReq, sizeof(Protocol::CSPartnerSkinReq));

	m_msg_handler_list[Protocol::MT_WORKSHOP_HANDLE_REQUEST_CS] = HandlerItem(&MessageHandler::OnWorkshopOperateReq, sizeof(Protocol::CSWorkshopOperateRequire));
	m_msg_handler_list[Protocol::MT_WORKSHOP_EXC_SKILL_BOOK_CS] = HandlerItem(&MessageHandler::OnConvertPetSkillReq, sizeof(Protocol::CSConvertPetSkillBookReq));
	m_msg_handler_list[Protocol::MT_WORKSHOP_EQUIP_VOC_CHANGE_CS] = HandlerItem(&MessageHandler::OnEquipVocChangeReq, sizeof(Protocol::CSChangeEquipVocReq));

	m_msg_handler_list[Protocol::MT_SYSTEM_NOTICE_GET_AWARD_CS] = HandlerItem(&MessageHandler::OnFetchSystemNoticeAward, sizeof(Protocol::CSSystemNoticeGetAward));

	m_msg_handler_list[Protocol::MT_NPC_HANDLE_REQUEST_CS] = HandlerItem(&MessageHandler::OnNPCHandleRequest, sizeof(Protocol::CSNPCHandleRequest));

	m_msg_handler_list[Protocol::MT_KNAPSACK_OPERA_REQ_CS]	= HandlerItem(&MessageHandler::OnKnapsackOperaReq, sizeof(Protocol::CSKnapsackOperaReq));
	m_msg_handler_list[Protocol::MT_USE_ITEM_CS]			= HandlerItem(&MessageHandler::OnUseItem, sizeof(Protocol::CSUseItem));
	m_msg_handler_list[Protocol::MT_USE_OPTIONAL_GIFT_CS] = HandlerItem(&MessageHandler::OnUseOptionalGift, sizeof(Protocol::CSUseOptionalGift));
	m_msg_handler_list[Protocol::MT_USE_OPTIONAL_GIFT_2_CS] = HandlerItem(&MessageHandler::OnUseOptionalGift2, sizeof(Protocol::CSUseOptionalGift2));
	m_msg_handler_list[Protocol::MT_USE_GOLD_RECOVERY_CS] = HandlerItem(&MessageHandler::OnUseGoldRecovery, sizeof(Protocol::CSUseGoldRecovery));
	m_msg_handler_list[Protocol::MT_USE_SPECIAL_LOGIC_ITEM_CS] = HandlerItem(&MessageHandler::OnUseSpecialLogicItem, sizeof(Protocol::CSUseSpecialLoigcItem));
	m_msg_handler_list[Protocol::MT_ACTIVE_SPECIAL_LOGIC_ITEM_CS] = HandlerItem(&MessageHandler::OnActiveSpecialLogicItem, sizeof(Protocol::CSActiveSpecialLogicItem));
	m_msg_handler_list[Protocol::MT_HEARSAY_INFO_CS] = HandlerItem(&MessageHandler::OnHearsayInfoReq, sizeof(Protocol::CSHearsayInfoReq));
	m_msg_handler_list[Protocol::MT_BUY_CMD_GIFT_REQ_CS] = HandlerItem(&MessageHandler::OnBuyCmdGiftReq, sizeof(Protocol::CSBuyCmdGiftReq));

	m_msg_handler_list[Protocol::MT_HEARTBEAT_CS] = HandlerItem(&MessageHandler::OnHeartbeatReq, sizeof(Protocol::CSHeartbeatReq));
	m_msg_handler_list[Protocol::MT_PRESTIGE_SHOPING_CS] = HandlerItem(&MessageHandler::OnPrestigeShoping, sizeof(Protocol::CSPrestigeShopingReq));
	m_msg_handler_list[Protocol::MT_PRESTIGE_DONATE_OP_CS] = HandlerItem(&MessageHandler::OnPrestigeDonateOp, sizeof(Protocol::CSPrestigeDonateOp));
	
	m_msg_handler_list[Protocol::MT_OBJ_MOVE_CS] = HandlerItem(&MessageHandler::OnRoleMoveReq, sizeof(Protocol::CSObjMove));
	m_msg_handler_list[Protocol::MT_FLY_BY_SHOE_CS] = HandlerItem(&MessageHandler::OnFlyByShoe, sizeof(Protocol::CSFlyByShoe));
	m_msg_handler_list[Protocol::MT_TRANSPORT_REQ] = HandlerItem(&MessageHandler::OnTransportReq, sizeof(Protocol::CSTransportReq));
	m_msg_handler_list[Protocol::MT_SPECIAL_TRANSPORT_REQ_CS] = HandlerItem(&MessageHandler::OnSpecialTransportReq, sizeof(Protocol::CSSpecialTransportReq));
	m_msg_handler_list[Protocol::MT_TREASURE_CHEST_REQ_CS] = HandlerItem(&MessageHandler::OnTreasureChestReq, sizeof(Protocol::CSTreasureChestReq));
	
	m_msg_handler_list[Protocol::MT_TITLE_OP_CS] = HandlerItem(&MessageHandler::OnTitleOp, sizeof(Protocol::CSTitleOp));
	m_msg_handler_list[Protocol::MT_TRAIN_REQ_CS] = HandlerItem(&MessageHandler::OnTrainOp, sizeof(Protocol::CSTrainReq));
	m_msg_handler_list[Protocol::MT_ROLE_PET_TRAIN_SKILL_REQ_CS] = HandlerItem(&MessageHandler::OnRolePetTrainSkillOp, sizeof(Protocol::CSRolePetTrainSkillReq));
	
	m_msg_handler_list[Protocol::MT_BATTLE_DEMO_START_CS] = HandlerItem(&MessageHandler::OnBattleDemoFightStartReq, sizeof(Protocol::CSBattleDemoStart));
	m_msg_handler_list[Protocol::MT_BATTLE_FIGHT_MONSTER_CS] = HandlerItem(&MessageHandler::OnBattleFightMonsterReq, sizeof(Protocol::CSBattleFightMonsterReq));
	m_msg_handler_list[Protocol::MT_BATTLE_PREPARE_FINISH_CS] = HandlerItem(&MessageHandler::OnBattlePrepareFinishReq, sizeof(Protocol::CSBattlePrepareFinish));
	m_msg_handler_list[Protocol::MT_BATTLE_ANIMATION_FINISH_CS] = HandlerItem(&MessageHandler::OnBattleAnimationFinishReq, sizeof(Protocol::CSAnimationFinish));
	m_msg_handler_list[Protocol::MT_BATTLE_FORCE_FINISH_FIGHT_CS] = HandlerItem(&MessageHandler::OnBattleForceFinishFightReq, sizeof(Protocol::CSBattleForceFinishFight));
	m_msg_handler_list[Protocol::MT_BATTLE_ROLE_OPERATE_CS] = HandlerItem(&MessageHandler::OnBattleRoleOperateReq, sizeof(Protocol::CSBattleRoleOperate));
	m_msg_handler_list[Protocol::MT_BATTLE_COUNTDOWN_FINISH_CS] = HandlerItem(&MessageHandler::OnBattleCountDownFinishReq, sizeof(Protocol::CSBattleCountDownFinish));
	m_msg_handler_list[Protocol::MT_BATTLE_WATCH_BATTLE_CS] = HandlerItem(&MessageHandler::OnWatchBattleReq, sizeof(Protocol::CSWatchBattleReq));
	m_msg_handler_list[Protocol::MT_BATTLE_PAUSE_CS] = HandlerItem(&MessageHandler::OnRolePauseBattleReq, sizeof(Protocol::CSBattlePause));
	m_msg_handler_list[Protocol::MT_BATTLE_UNDO_CS] = HandlerItem(&MessageHandler::OnRoleBattleUndoReq, sizeof(Protocol::CSRoleUndo));

	m_msg_handler_list[Protocol::MT_COLLECTION_OP_CS] = HandlerItem(&MessageHandler::OnCollectionOp, sizeof(Protocol::CSCollectionOp));
	m_msg_handler_list[Protocol::MT_WST_QUESTION_OP_CS] = HandlerItem(&MessageHandler::OnWstQuestionOp, sizeof(Protocol::CSAnswerOp));
	m_msg_handler_list[Protocol::MT_EXP_CRYSTAL_OPEN_CS] = HandlerItem(&MessageHandler::OnExpCrystalOp, sizeof(Protocol::CSExpCrystalOp));
	m_msg_handler_list[Protocol::MT_TREVI_FOUNTAIN_OPEN_CS] = HandlerItem(&MessageHandler::OnTreviFountainOp, sizeof(Protocol::CSTreviFountainOp));

	m_msg_handler_list[Protocol::MT_TIME_REQ_CS] = HandlerItem(&MessageHandler::OnTimeReq, sizeof(Protocol::CSTimeReq));

	m_msg_handler_list[Protocol::MT_SKILL_OPERATE_REQ_CS] = HandlerItem(&MessageHandler::OnSkillOperateReq, sizeof(Protocol::CSSkillOperateReq));
	m_msg_handler_list[Protocol::MT_NEW_LIFE_SKILL_REQ_CS] = HandlerItem(&MessageHandler::OnNewLifeSkillReq, sizeof(Protocol::CSNewLifeSkillReq));
	m_msg_handler_list[Protocol::MT_FISH_GROUND_REQ_CS] = HandlerItem(&MessageHandler::OnFishGroundReq, sizeof(Protocol::CSFishGroundReq));
	m_msg_handler_list[Protocol::MT_FETCH_REWARD_REQ_CS] = HandlerItem(&MessageHandler::OnFetchRewardReq, sizeof(Protocol::CSFetchRewardReq));
	m_msg_handler_list[Protocol::MT_SURFACE_OP_CS] = HandlerItem(&MessageHandler::OnSurfaceOp, sizeof(Protocol::CSSurfaceOp));
	m_msg_handler_list[Protocol::MT_FIND_REWARD_REQ_CS] = HandlerItem(&MessageHandler::OnFindRewardReq, sizeof(Protocol::CSFindRewardReq));
	m_msg_handler_list[Protocol::MT_REWARD_NOTIFY_REQUEST_CS] = HandlerItem(&MessageHandler::OnFindRewardNotifysCancelReq, sizeof(Protocol::CSRewardFindNotifyCancelReq));
	m_msg_handler_list[Protocol::MT_SURFACE_DECOMPOSE_CS] = HandlerItem(&MessageHandler::OnSurfaceDecompose, sizeof(Protocol::CSSurefaceDecompose));

	m_msg_handler_list[Protocol::MT_EQUIPMENT_OPERA_REQ_CS] = HandlerItem(&MessageHandler::OnEquipmentOperateReq, sizeof(Protocol::CSEquipmentOperaReq));
	m_msg_handler_list[Protocol::MT_BATTLE_GIVEUP_CS] = HandlerItem(&MessageHandler::OnBattleGiveupReq, sizeof(Protocol::CSBattleGiveup));
	m_msg_handler_list[Protocol::MT_BATTLE_SURRENDER_CS] = HandlerItem(&MessageHandler::OnBattleSurrenderReq, sizeof(Protocol::CSBattleSurrender));

	m_msg_handler_list[Protocol::MT_JEWELRY_HANDLE_REQUEST_CS] = HandlerItem(&MessageHandler::OnJewelryHandleRequest, sizeof(Protocol::CSJewelryHandleReq));
	m_msg_handler_list[Protocol::MT_OFFLINE_EXP_RECEIVES_CS] = HandlerItem(&MessageHandler::OnObtainOfflineExpReq, sizeof(Protocol::CSOfflineExpReceives));
	m_msg_handler_list[Protocol::MT_MARRY_REQ_CS] = HandlerItem(&MessageHandler::OnMarryReq, sizeof(Protocol::CSMarriageReq));
	m_msg_handler_list[Protocol::MT_CREATE_GUILD_CS] = HandlerItem(&MessageHandler::OnCreateGuildReq, sizeof(Protocol::CSCreateGuild));
	m_msg_handler_list[Protocol::MT_QUIT_GUILD_REQ_CS] = HandlerItem(&MessageHandler::OnQuitGuildReq, sizeof(Protocol::CSQuitGuild));
	m_msg_handler_list[Protocol::MT_GUILD_APPLY_JOIN_REQ_CS] = HandlerItem(&MessageHandler::OnJoinGuildReq, sizeof(Protocol::CSApplyJoinGuildReq));
	m_msg_handler_list[Protocol::MT_GUILD_APPLY_JOIN_ACK_CS] = HandlerItem(&MessageHandler::OnJoinGuildAck, sizeof(Protocol::CSApplyJoinGuildAck));
	m_msg_handler_list[Protocol::MT_GUILD_EXPEL_OUT_REQ_CS] = HandlerItem(&MessageHandler::OnGuildExpelOutReq, sizeof(Protocol::CSGuildExpelOutReq));
	m_msg_handler_list[Protocol::MT_GUILD_SET_NOTICE_REQ_CS] = HandlerItem(&MessageHandler::OnGuildSetNoticeReq, sizeof(Protocol::CSSetGuildNoticeReq));
	m_msg_handler_list[Protocol::MT_GET_GUILD_LIST_CS] = HandlerItem(&MessageHandler::OnGetGuildList, sizeof(Protocol::CSGetGuildList));
	m_msg_handler_list[Protocol::MT_GUILD_ABDICATION_REQ_CS] = HandlerItem(&MessageHandler::OnGuildAbdicationOp, sizeof(Protocol::CSGuildAbdicationOp));
	m_msg_handler_list[Protocol::MT_GUILD_IMPEACHMENT_REQ_CS] = HandlerItem(&MessageHandler::OnGuildImpeachmentOp, sizeof(Protocol::CSGuildImpeachmentOp));
	m_msg_handler_list[Protocol::MT_GUILD_APPOINTMENT_REQ_CS] = HandlerItem(&MessageHandler::OnGuildAppointmentOp, sizeof(Protocol::CSGuildAppointmentOp));
	m_msg_handler_list[Protocol::MT_GUILD_SHOP_BUY_CS] = HandlerItem(&MessageHandler::OnGuildShopBuy, sizeof(Protocol::CSGuildShopBuy));
	m_msg_handler_list[Protocol::MT_GUILD_PET_TRAINING_UP_CS] = HandlerItem(&MessageHandler::OnGuildPetTrainingUp, sizeof(Protocol::CSGuildPetTrainingUp));
	m_msg_handler_list[Protocol::MT_GUILD_PET_TRAINING_CS] = HandlerItem(&MessageHandler::OnGuildPetTrainingReq, sizeof(Protocol::CSGuildPetTraining));
	m_msg_handler_list[Protocol::MT_GET_GUILD_TRAINING_INFO_CS] = HandlerItem(&MessageHandler::OnGetGuildTrainingInfo, sizeof(Protocol::CSGetGuildTrainingInfo));
	m_msg_handler_list[Protocol::MT_GUILD_INVITATION_REQ_CS] = HandlerItem(&MessageHandler::OnGuildInvitationReq, sizeof(Protocol::CSGuildInvitationReq));
	m_msg_handler_list[Protocol::MT_GUILD_INVITATION_REPLY_CS] = HandlerItem(&MessageHandler::OnGuildInvitationRep, sizeof(Protocol::CSInvitationReply));
	m_msg_handler_list[Protocol::MT_GUILD_SET_NEED_APPROVE_INFO_CS] = HandlerItem(&MessageHandler::OnSetNeedApproveJoin, sizeof(Protocol::CSSetGuildNeedApproveReq));
	m_msg_handler_list[Protocol::MT_GUILD_RENAME_CS] = HandlerItem(&MessageHandler::OnGuildRename, sizeof(Protocol::CSGuildRenameReq));
	m_msg_handler_list[Protocol::MT_GUILD_BOSS_REQ_CS] = HandlerItem(&MessageHandler::OnGetGuildBossReq, sizeof(Protocol::CSGuildBossReq));
	m_msg_handler_list[Protocol::MT_GUILD_LEVEL_UP_CS] = HandlerItem(&MessageHandler::OnGuildLevelUpReq, sizeof(Protocol::CSGuildLevelUpReq));
	m_msg_handler_list[Protocol::MT_ACTIVITY_GUILD_ANSWER_ANSWER_QUEST_CS] = HandlerItem(&MessageHandler::OnGuildAnswerAnswerQuestion, sizeof(Protocol::CSActivityGuildAnswerAnswerQuest));
	m_msg_handler_list[Protocol::MT_GUILD_RESET_BANNER_CS] = HandlerItem(&MessageHandler::OnGuildSetBannerReq, sizeof(Protocol::CSGuildSetBanner));
	m_msg_handler_list[Protocol::MT_GUILD_CONSTRUCT_BUILDING_SEQ_CS] = HandlerItem(&MessageHandler::OnBuildingGuildBuildReq, sizeof(Protocol::CSGuildConstructReq));

	m_msg_handler_list[Protocol::MT_MARRIAGE_SEEKING_REQ_CS] = HandlerItem(&MessageHandler::OnMarriageSeekingReq, sizeof(Protocol::CSMarriageSeekingOpera));
	m_msg_handler_list[Protocol::MT_BABY_REQ_CS] = HandlerItem(&MessageHandler::OnBabyReq, sizeof(Protocol::CSBabyReq));
	m_msg_handler_list[Protocol::MT_BABY_RENAME_CS] = HandlerItem(&MessageHandler::OnBabyRenameReq, sizeof(Protocol::CSBabyRename));
	m_msg_handler_list[Protocol::MT_BABY_ACTIVE_CS] = HandlerItem(&MessageHandler::OnBabyActiveReq, sizeof(Protocol::CSBabyActivte));
	m_msg_handler_list[Protocol::MT_FRIEND_REQ_CS] = HandlerItem(&MessageHandler::OnFriendReq, sizeof(Protocol::CSFriendReq));	
	m_msg_handler_list[Protocol::MT_FRIEND_EDIT_GRUOP_CS] = HandlerItem(&MessageHandler::OnFriendEditGruopReq, sizeof(Protocol::CSFriendEditGruop));
	m_msg_handler_list[Protocol::MT_FRIEND_MOVE_GRUOP_CS] = HandlerItem(&MessageHandler::OnFriendMoveGruopReq, sizeof(Protocol::CSFriendMoveGruop));
	m_msg_handler_list[Protocol::MT_FRIEND_RENAME_GRUOP_CS] = HandlerItem(&MessageHandler::OnFriendRenameGruopReq, sizeof(Protocol::CSFriendRenameGruop));

	m_msg_handler_list[Protocol::MT_RECOMMEND_FRIEND_LIST_REQ_CS] = HandlerItem(&MessageHandler::OnFriendRecommendListReq, sizeof(Protocol::CSFriendRecommendReq));
	m_msg_handler_list[Protocol::MT_ADD_RECOMMEND_FRIEND_CS] = HandlerItem(&MessageHandler::OnAddFriendRecommend, sizeof(Protocol::CSFriendRecommendAddOnListReq));
	m_msg_handler_list[Protocol::MT_REFRESH_RECOMMEND_FRIEND_CS] = HandlerItem(&MessageHandler::OnRefreshFriendRecommend, sizeof(Protocol::CSRefreshFriendRecommend));
	m_msg_handler_list[Protocol::MT_FUZZY_SEARCH_ROLE_INFO_CS] = HandlerItem(&MessageHandler::OnFuzzySearchRoleInfoByName, sizeof(Protocol::CSFuzzySearchRoleByName));

	m_msg_handler_list[Protocol::MT_SMART_MOUNTS_SYSTEM_REQ_CS] = HandlerItem(&MessageHandler::OnSmartMountsSystemReq, sizeof(Protocol::CSSmartMountsSystemReq));
	m_msg_handler_list[Protocol::MT_SMART_MOUNTS_SYSTEM_SKILL_REQ_CS] = HandlerItem(&MessageHandler::OnSmartMountsSystemSkillReq, sizeof(Protocol::CSSmartMountsSystemSkillReq));

	m_msg_handler_list[Protocol::MT_CROSS_START_REQ_CS] = HandlerItem(&MessageHandler::OnRoleStartCrossReq, sizeof(Protocol::CSCrossStartReq));

	m_msg_handler_list[Protocol::MT_ROLE_CAREER_ADVANCE_REQ_CS] = HandlerItem(&MessageHandler::OnRoleCareerAdvanceReq, sizeof(Protocol::CSRoleCareerAdvanceReq));
	m_msg_handler_list[Protocol::MT_ROLE_CARRER_CHANGE_REQ_CS] = HandlerItem(&MessageHandler::OnRoleCareerChangeReq, sizeof(Protocol::CSRoleCareerChangeReq));

	m_msg_handler_list[Protocol::MT_ATTRIBUTE_PLAN_CREATE_CS] = HandlerItem(&MessageHandler::OnRoleCreateAttributePlanReq, sizeof(Protocol::CSAttributePlanCreate));
	m_msg_handler_list[Protocol::MT_ATTRIBUTE_PLAN_CHANGE_NAME_CS] = HandlerItem(&MessageHandler::OnRoleChangeAttributePlanNameReq, sizeof(Protocol::CSAttributePlanChangeName));
	m_msg_handler_list[Protocol::MT_ATTRIBUTE_PLAN_SWITCH_PLAN_CS] = HandlerItem(&MessageHandler::OnRoleSwitchAttributePlanReq, sizeof(Protocol::CSAttributePlanSwitchPlan));
	m_msg_handler_list[Protocol::MT_ATTRIBUTE_PLAN_ADD_POINT_CS] = HandlerItem(&MessageHandler::OnRoleAddAttributePointReq, sizeof(Protocol::CSAttributePlanAddPoint));
	m_msg_handler_list[Protocol::MT_ATTRIBUTE_PLAN_CLEAR_POINT_CS] = HandlerItem(&MessageHandler::OnRoleClearAttributePointReq, sizeof(Protocol::CSAttributePlanClearPoint));

	m_msg_handler_list[Protocol::MT_ROLE_SYSTEM_SET_REQ_CS] = HandlerItem(&MessageHandler::OnRoleClientSystemSetReq, sizeof(Protocol::CSRoleSystemSetReq));

	m_msg_handler_list[Protocol::MT_SHITU_OPERA_MAKE_RELATION_REQ_CS] = HandlerItem(&MessageHandler::OnShiTuMakeRelationReq, sizeof(Protocol::CSShiTuMakeRelationReq));
	m_msg_handler_list[Protocol::MT_SHITU_OPERA_MAKE_RELATION_AKC_CS] = HandlerItem(&MessageHandler::OnShiTuMakeRelationAck, sizeof(Protocol::CSShiTuMakeRelationAck));
	m_msg_handler_list[Protocol::MT_SHITU_OPERA_BREAK_RELATION_REQ_CS] = HandlerItem(&MessageHandler::OnShiTuBreakRelationReq, sizeof(Protocol::CSShiTuBreakRelationReq));
	m_msg_handler_list[Protocol::MT_SHITU_OPERA_BREAK_RELATION_AKC_CS] = HandlerItem(&MessageHandler::OnShiTuBreakRelationAck, sizeof(Protocol::CSShiTuBreakRelationAck));
	m_msg_handler_list[Protocol::MT_SHITU_OPERA_REQ_CS] = HandlerItem(&MessageHandler::OnShiTuReq, sizeof(Protocol::CSShiTuReq));
	m_msg_handler_list[Protocol::MT_SHITU_SEEKING_REQ_CS] = HandlerItem(&MessageHandler::OnShiTuSeekingReq, sizeof(Protocol::CSShiTuSeekingReq));
	m_msg_handler_list[Protocol::MT_SERVER_FIRST_KILL_REQ_CS] = HandlerItem(&MessageHandler::OnFirstKillReq, sizeof(Protocol::CSServerFirstKillReq));

	m_msg_handler_list[Protocol::MT_MAIL_SEND_CS] = HandlerItem(&MessageHandler::OnSendMail, sizeof(Protocol::CSMailSend));
	m_msg_handler_list[Protocol::MT_MAIL_DELETE_CS] = HandlerItem(&MessageHandler::OnDeleteMail, sizeof(Protocol::CSMailDelete));
	m_msg_handler_list[Protocol::MT_MAIL_GETLIST_CS] = HandlerItem(&MessageHandler::OnGetMailList, sizeof(Protocol::CSMailGetList));
	m_msg_handler_list[Protocol::MT_MAIL_READ_CS] = HandlerItem(&MessageHandler::OnReadMail, sizeof(Protocol::CSMailRead));
	m_msg_handler_list[Protocol::MT_MAIL_ATTACHMENT_CS] = HandlerItem(&MessageHandler::OnFetchMailAttachment, sizeof(Protocol::CSMailFetchAttachment));
	m_msg_handler_list[Protocol::MT_MAIL_CLEAN_CS] = HandlerItem(&MessageHandler::OnCleanMail, sizeof(Protocol::CSMailClean));
	m_msg_handler_list[Protocol::MT_MAIL_ONE_KEY_FETCH_ATTACHMENT] = HandlerItem(&MessageHandler::OnOneKeyFetchMailAttachment, sizeof(Protocol::CSMailOneKeyFetchAttachment));

	m_msg_handler_list[Protocol::MT_BATTLE_MODE_FIGHT_CS] = HandlerItem(&MessageHandler::OnRoleBattleModeFightReq, sizeof(Protocol::CSBattleModeFightReq));
	m_msg_handler_list[Protocol::MT_BATTLE_FIGHT_OTHERS_CS] = HandlerItem(&MessageHandler::OnRoleChallengeOthersReq, sizeof(Protocol::CSChallengeOthersReq));
	m_msg_handler_list[Protocol::MT_BATTLE_QIE_CUO_REQ_CS] = HandlerItem(&MessageHandler::OnRoleQieCuoWithOtherReq, sizeof(Protocol::CSBattleQieCuoWithOthers));
	m_msg_handler_list[Protocol::MT_BATTLE_QIE_CUO_REP_CS] = HandlerItem(&MessageHandler::OnRoleQieCuoRequestReply, sizeof(Protocol::CSBattleQieCuoMessageRep));
	m_msg_handler_list[Protocol::MT_SET_ENCOUNTER_MINE_MONSTER_FLAG_CS] = HandlerItem(&MessageHandler::OnSetEncounterMineMonsterFlag, sizeof(Protocol::CSSetEncounterMineMonsterFlag));
	m_msg_handler_list[Protocol::MT_SET_BATTLE_POSITION_CS] = HandlerItem(&MessageHandler::OnSetBattlePosition, sizeof(Protocol::CSSetBattlePosition));

	m_msg_handler_list[Protocol::MT_COURAGE_CHALLENGE_REQ_CS] = HandlerItem(&MessageHandler::OnCourageChallengeReq, sizeof(Protocol::CSCourageChallengeOp));
	m_msg_handler_list[Protocol::MT_RANK_GET_COURAGE_CHALLENGE_REQ_CS] = HandlerItem(&MessageHandler::OnCourageChallengeRankListReq, sizeof(Protocol::CSCourageChallengeRankListReq));

	m_msg_handler_list[Protocol::MT_CHAT_RECORD_SET_CS] = HandlerItem(&MessageHandler::OnChatRecordReq, sizeof(Protocol::CSChatRecordSet));
	m_msg_handler_list[Protocol::MT_CHAT_SIMPLE_RECORD_SET_CS] = HandlerItem(&MessageHandler::OnChatSimpleRecordReq, sizeof(Protocol::CSChatSimpleRecordSet));

	m_msg_handler_list[Protocol::MT_SET_BATTLE_AUTO_CS] = HandlerItem(&MessageHandler::OnSetBattleAuto, sizeof(Protocol::CSSetBattleAuto));
	m_msg_handler_list[Protocol::MT_SET_BATTLE_AUTO_MOVE_CS] = HandlerItem(&MessageHandler::OnSetBattleAutoMove, sizeof(Protocol::CSSetBattleAutoMove));
	m_msg_handler_list[Protocol::MT_CHANGE_APPEARANCE_CS] = HandlerItem(&MessageHandler::OnChangeAppearacne, sizeof(Protocol::CSChangeAppearance));
	m_msg_handler_list[Protocol::MT_CHANGE_HEADSHOT_CS] = HandlerItem(&MessageHandler::OnChangeHeadshot, sizeof(Protocol::CSChangeHeadshot));
	m_msg_handler_list[Protocol::MT_SIGNLE_ATTR_REASON_REQ_CS] = HandlerItem(&MessageHandler::OnAttrListReasonReq, sizeof(Protocol::CSAttrListReasonReq));

	m_msg_handler_list[Protocol::MT_FSS_OPERATOR_REQUEST_CS] = HandlerItem(&MessageHandler::OnFaceOperatorReq, sizeof(Protocol::CSFaceScoreRequest));
	m_msg_handler_list[Protocol::MT_FUNCGUIDE_OPERATOR_REQUEST_CS] = HandlerItem(&MessageHandler::OnFuncGuideRequest, sizeof(Protocol::CSFuncGuideOperator));

	m_msg_handler_list[Protocol::MT_RESET_NAME_CS] = HandlerItem(&MessageHandler::OnResetNameReq, sizeof(Protocol::CSRoleResetName));

	m_msg_handler_list[Protocol::MT_ENTER_FB_CS] = HandlerItem(&MessageHandler::OnEnterFBReq, sizeof(Protocol::CSEnterFB));
	m_msg_handler_list[Protocol::MT_LEAVE_FB_CS] = HandlerItem(&MessageHandler::OnLeaveFBReq, sizeof(Protocol::CSLeaveFB));
	m_msg_handler_list[Protocol::MT_ACTIVITY_ENTER_REQ_CS] = HandlerItem(&MessageHandler::OnActivityEnterReq, sizeof(Protocol::CSActivityEnterReq));
	m_msg_handler_list[Protocol::MT_ACTIVITY_TEAM_FIGHT_MATCH_CS] = HandlerItem(&MessageHandler::OnActivityTeamFightReq, sizeof(Protocol::CSActivityTeamFightMatchReq));
	m_msg_handler_list[Protocol::MT_ACTIVITY_MAZE_RACER_HANDLE_REQ_CS] = HandlerItem(&MessageHandler::OnActivityMazeRacerReq, sizeof(Protocol::CSActivityMazeRacerHandleReq));
	m_msg_handler_list[Protocol::MT_ACTIVITY_GUILD_FIGHT_HANDLE_REQ_CS] = HandlerItem(&MessageHandler::OnActivityGuildFightReq, sizeof(Protocol::CSActivityGuildFightOperatorReq));
	m_msg_handler_list[Protocol::MT_ACTIVITY_ONLY_FIGHT_HANDLE_REQ_CS] = HandlerItem(&MessageHandler::OnActivityOnlyFightReq, sizeof(Protocol::CSActivityOnlyFightOperatorReq));
	m_msg_handler_list[Protocol::MT_ACTIVITY_SILK_ROADS_REQ_CS] = HandlerItem(&MessageHandler::OnActivitySilkRoadsReq, sizeof(Protocol::CSActivitySilkRoadsReq));

	m_msg_handler_list[Protocol::MT_ACTIVITY_SIGN_UP_REQ_CS] = HandlerItem(&MessageHandler::OnActivitySignUpReq, sizeof(Protocol::CSActivitySignUpReq));
	m_msg_handler_list[Protocol::MT_ACTIVITY_ADVANCE_FETCH_REQ_CS] = HandlerItem(&MessageHandler::OnActivityAdvanceFetchReq, sizeof(Protocol::CSActivityAdvanceFetchReq));
	m_msg_handler_list[Protocol::MT_CMD_BUY_ITEM_CHECK_REQ_CS] = HandlerItem(&MessageHandler::OnCmdBuyItemCheckReq, sizeof(Protocol::CSCmdBuyItemCheckReq));

	m_msg_handler_list[Protocol::MT_WROLD_BOSS_REQ_CS] = HandlerItem(&MessageHandler::OnWorldBossReq, sizeof(Protocol::CSWorldBossReq));
	m_msg_handler_list[Protocol::MT_WROLD_BOSS_2_REQ_CS] = HandlerItem(&MessageHandler::OnWorldBoss2Req, sizeof(Protocol::CSWorldBoss2Req));
	m_msg_handler_list[Protocol::MT_WORLD_BOSS_3_REQ_CS] = HandlerItem(&MessageHandler::OnWorldBoss3Req, sizeof(Protocol::CSWorldBoss3Req));

	m_msg_handler_list[Protocol::MT_ROLE_PROFESSION_EXTRA_INFO_REQ_CS] = HandlerItem(&MessageHandler::OnRoleProfExtraInfoReq, sizeof(Protocol::CSProfessionExtraInfoReq));

	m_msg_handler_list[Protocol::MT_SCHOOL_TASK_REQ_CS] = HandlerItem(&MessageHandler::OnSchoolTaskReq, sizeof(Protocol::CSSchoolTaskReq));

	m_msg_handler_list[Protocol::MT_RESEARCH_TASK_OPERATOR_CS] = HandlerItem(&MessageHandler::OnResearchTaskRequest, sizeof(Protocol::CSResearchTaskRequest));

	m_msg_handler_list[Protocol::MT_ESCORT_REQ_CS] = HandlerItem(&MessageHandler::OnEscortOp, sizeof(Protocol::CSEscortOp));

	m_msg_handler_list[Protocol::MT_TREASURE_MAP_REQ_CS] = HandlerItem(&MessageHandler::OnTreasureMapReq, sizeof(Protocol::CSTreasureMapReq));

	m_msg_handler_list[Protocol::MT_FABAO_REQ_CS] = HandlerItem(&MessageHandler::OnFaBaoReq, sizeof(Protocol::CSFabaoReq));
	m_msg_handler_list[Protocol::MT_MAP_UNLOCK_REMOVE_UNLOCK_ANIMATIONS_CS] = HandlerItem(&MessageHandler::OnMapUnlockRemoveAnimation, sizeof(Protocol::CSMapUnlockRemoveAnima));

	m_msg_handler_list[Protocol::MT_TRADE_MARKET_USER_SHELVE_DATA_CS] = HandlerItem(&MessageHandler::OnTradeMarketUserShelveDataReq, sizeof(Protocol::CSTradeMarketUserShelveData));
	m_msg_handler_list[Protocol::MT_ADD_ITEM_ON_SHELVE_CS] = HandlerItem(&MessageHandler::OnTradeMarketAddItemOnShelveReq, sizeof(Protocol::CSAddItemOnShelve));
	m_msg_handler_list[Protocol::MT_REMOVE_ITEM_ON_SHELVE_CS] = HandlerItem(&MessageHandler::OnTradeMarketRemoveItemOnShelveReq, sizeof(Protocol::CSRemoveItemOnShelve));
	m_msg_handler_list[Protocol::MT_CASH_OUT_CS] = HandlerItem(&MessageHandler::OnTradeMarketCashOutReq, sizeof(Protocol::CSTradeMarketCashOut));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_BUY_ITEM_CS] = HandlerItem(&MessageHandler::OnTradeMarketBuyItemReq, sizeof(Protocol::CSTradeMarketBuyItem));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_SEARCH_BY_ITEM_ID_CS] = HandlerItem(&MessageHandler::OnTradeMarketSearchByItemReq, sizeof(Protocol::CSTradeMarketSearchByItemID));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_SEARCH_BY_ITEM_SUB_TYPE_CS] = HandlerItem(&MessageHandler::OnTradeMarketSearchByTypeReq, sizeof(Protocol::CSTradeMarketSearchByItemSearchType));
	m_msg_handler_list[Protocol::MT_RECENT_TRANSACTION_RECORDS_CS] = HandlerItem(&MessageHandler::OnTradeMarketGetRecentTransactionRecordsReq, sizeof(Protocol::CSRecentTransactionRecords));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_ADD_FOCUS_CS] = HandlerItem(&MessageHandler::OnTradeMarketAddFocusReq, sizeof(Protocol::CSTradeMarketAddFocus));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_REMOVE_FOCUS_CS] = HandlerItem(&MessageHandler::OnTradeMarketRemoveFocusReq, sizeof(Protocol::CSTradeMarketRemoveFocus));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_GET_FOCUS_LIST_CS] = HandlerItem(&MessageHandler::OnTradeMarketGetFocusListReq, sizeof(Protocol::CSTradeMarketGetFocusList));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_GET_FOCUS_ITEM_INFO_CS] = HandlerItem(&MessageHandler::OnTradeMarketGetFocusItemInfoReq, sizeof(Protocol::CSTradeMarketGetFocusItemInfo));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_SEARCH_ADVERTISE_CS] = HandlerItem(&MessageHandler::OnTradeMarketSearchAdvertiseReq, sizeof(Protocol::CSTradeMarketSearchAdvertise));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_GET_SELLING_LIST_CS] = HandlerItem(&MessageHandler::OnTradeMarketGetSellingListReq, sizeof(Protocol::CSTradeMarketGetSellingList));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_ROLE_GET_TRADE_RECORD_CS] = HandlerItem(&MessageHandler::OnTradeMarketGetRoleTransactionRecordReq, sizeof(Protocol::CSTradeMarketGetRoleTransactionRecord));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_ADVERTISE_CS] = HandlerItem(&MessageHandler::OnTradeMarketAdvertiseReq, sizeof(Protocol::CSTradeMarketAdvertise));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_CS] = HandlerItem(&MessageHandler::OnTradeMarketRedoAddItemOnShelveReq, sizeof(Protocol::CSTradeMarketRedoAddItemOnShelve));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY_CS] = HandlerItem(&MessageHandler::OnTradeMarketSearchPetForQuickBuyReq, sizeof(Protocol::CSTradeMarketSearchPetForQuickBuy));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_GET_MERCHANDISE_NUM_CS] = HandlerItem(&MessageHandler::OnTradeMarketGetMerchandiseNumReq, sizeof(Protocol::CSTradeMarketGetMerchandiseNum));
	m_msg_handler_list[Protocol::MT_TRADE_MARKET_SEARCH_MEDAL_BY_SPE_TYPE_AND_COLOR_CS] = HandlerItem(&MessageHandler::OnTradeMarketSearchMedalByEffectTypeAndColorReq, sizeof(Protocol::CSTradeMarketSearchMedalByEffectTypeAndColor));

	m_msg_handler_list[Protocol::MT_TASK_CHAIN_OP_CS] = HandlerItem(&MessageHandler::OnTaskChainOpReq, sizeof(Protocol::CSTaskChainOp));
	m_msg_handler_list[Protocol::MT_TASK_GAME_PLAY_CS] = HandlerItem(&MessageHandler::OnTaskGamePlayReq, sizeof(Protocol::CSTaskGameReq));
	m_msg_handler_list[Protocol::MT_TASK_COMMIT_MONEY_CS] = HandlerItem(&MessageHandler::OnCommitTaskMoneyReq, sizeof(Protocol::CSTaskCommitMoneyReq));

	m_msg_handler_list[Protocol::MT_BRAVE_GROUND_REQ_CS] = HandlerItem(&MessageHandler::OnBraveGroundReq, sizeof(Protocol::CSBraveGroundReq));
	m_msg_handler_list[Protocol::MT_RAND_ACTIVITY_OP_CS] = HandlerItem(&MessageHandler::OnRandActivityOpReq, sizeof(Protocol::CSRandActivityOperaReq));
	m_msg_handler_list[Protocol::MT_FIRST_CHONGZHI_ACTIVITY_REQ_CS] = HandlerItem(&MessageHandler::OnFirstChongzhiReq, sizeof(Protocol::SCRAFirstChongZhiReq));
	m_msg_handler_list[Protocol::MT_RAND_ACTIVITY_SEVEN_COLOR_REQ_CS] = HandlerItem(&MessageHandler::OnSevenColorReq, sizeof(Protocol::CSRASevenColorReq));
	m_msg_handler_list[Protocol::MT_RAND_ACTIVITY_BONUS_OP_CS] = HandlerItem(&MessageHandler::OnRandActivityBonusOpReq, sizeof(Protocol::CSRandActivityBonusOperaReq));
	m_msg_handler_list[Protocol::MT_RAND_ACTIVITY_MAKE_UP_REWARD_REQ_CS] = HandlerItem(&MessageHandler::OnMakeUpReq, sizeof(Protocol::CSRandActivityMakeUpReq));
	m_msg_handler_list[Protocol::MT_RAND_ACTIVITY_DUI_HUAN_LING_YU_CS] = HandlerItem(&MessageHandler::OnRandActivityDuiHuanLingYuReq, sizeof(Protocol::CSRandActivityDuiHuanLingYuReq));

	m_msg_handler_list[Protocol::MT_MEDAL_REQ_CS] = HandlerItem(&MessageHandler::OnMedalReq, sizeof(Protocol::CSMedalReq));
	m_msg_handler_list[Protocol::MT_MEDAL_ONE_KEY_GRADE_CS] = HandlerItem(&MessageHandler::OnMedalOneKeyGradeReq, sizeof(Protocol::CSMedalOneKeyGradeReq));
	m_msg_handler_list[Protocol::MT_ZHU_LING_REQ_CS] = HandlerItem(&MessageHandler::OnZhuLingReq, sizeof(Protocol::CSZhuLingReq));
	m_msg_handler_list[Protocol::MT_ZHU_LING_DECOMPOSE_REQ_CS] = HandlerItem(&MessageHandler::OnZhuLingDecomposeReq, sizeof(Protocol::CSZhuLingDecomposeReq));
	m_msg_handler_list[Protocol::MT_ZHU_LING_UP_LEVEL_REQ_CS] = HandlerItem(&MessageHandler::OnZhuLingUpLevelReq, sizeof(Protocol::CSZhuLingUpLevelReq));
	m_msg_handler_list[Protocol::MT_CRYSTAL_FB_REQ_CS] = HandlerItem(&MessageHandler::OnCrystalFbReq, sizeof(Protocol::CSCrystalFbReq));
	m_msg_handler_list[Protocol::MT_SHOP_BUY_REQ_CS] = HandlerItem(&MessageHandler::OnShopBuyReq, sizeof(Protocol::CSShopBuyReq));
	m_msg_handler_list[Protocol::MT_SHOP_GOLD_SHOP_BUY_CS] = HandlerItem(&MessageHandler::OnGoldShopBuyReq, sizeof(Protocol::CSGoldShopBuyReq));
	m_msg_handler_list[Protocol::MT_SHOP_FAST_BUY_CS] = HandlerItem(&MessageHandler::OnShopFastBuyReq, sizeof(Protocol::CSFastPurchase));
	m_msg_handler_list[Protocol::MT_CANCEL_NOTIFY_REQUEST_CS] = HandlerItem(&MessageHandler::OnGoldShopCancel, sizeof(Protocol::CSGoldShopNotifyCancelReq));
	m_msg_handler_list[Protocol::MT_MARKET_REQ_CS] = HandlerItem(&MessageHandler::OnMarketReq, sizeof(Protocol::CSMarketReq));

	m_msg_handler_list[Protocol::MT_XUN_BAO_REQ_CS] = HandlerItem(&MessageHandler::OnXunBaoReq, sizeof(Protocol::CSXunBaoReq));
	m_msg_handler_list[Protocol::MT_XUN_BAO_EXCHANGE_CS] = HandlerItem(&MessageHandler::OnXunBaoExchange, sizeof(Protocol::CSXunBaoExchange));
	m_msg_handler_list[Protocol::MT_XUN_BAO_BELSS_CS] = HandlerItem(&MessageHandler::OnXunBaoBelssReq, sizeof(Protocol::CSXunBaoBelssReq));
	m_msg_handler_list[Protocol::MT_COLOSSEUM_REQ_CS] = HandlerItem(&MessageHandler::OnColosseumReq, sizeof(Protocol::CSColosseumReq));
	m_msg_handler_list[Protocol::MT_EXCHANGE_REQ_CS] = HandlerItem(&MessageHandler::OnExchangeReq, sizeof(Protocol::CSExchangeReq));
	m_msg_handler_list[Protocol::MT_CHIEF_ELECTION_REQ_CS] = HandlerItem(&MessageHandler::OnActivityChiefElectionReq, sizeof(Protocol::CSChiefElectionReq));
	m_msg_handler_list[Protocol::MT_CHIEF_PEAK_BATTLE_REQ_CS] = HandlerItem(&MessageHandler::OnActivityChiefPeakBattleReq, sizeof(Protocol::CSChiefPeakBattleReq));
	m_msg_handler_list[Protocol::MT_GUILD_HONOR_REQ_CS] = HandlerItem(&MessageHandler::OnActivityGuildHonorReq, sizeof(Protocol::CSGuildHonorReq));
	m_msg_handler_list[Protocol::MT_GUILD_HONOR_REQ_2_CS] = HandlerItem(&MessageHandler::OnActivityGuildHonorReq2, sizeof(Protocol::CSGuildHonorReq2));
	m_msg_handler_list[Protocol::MT_PLATFORM_BATTLE_REQ_CS] = HandlerItem(&MessageHandler::OnPlatformBattleReq, sizeof(Protocol::CSPlatformBattleReq));
	m_msg_handler_list[Protocol::MT_PLATFORM_BATTLE_REQ_2_CS] = HandlerItem(&MessageHandler::OnPlatformBattleReq2, sizeof(Protocol::CSPlatformBattleReq2));
	m_msg_handler_list[Protocol::MT_PLATFORM_BATTLE_RANK_FIRST_INFO_REQ_CS] = HandlerItem(&MessageHandler::OnPlatformBattleFirstInfoReq, sizeof(Protocol::CSPlatformBattleRankFirstInfoReq));
	m_msg_handler_list[Protocol::MT_HUAN_JIE_ZHAN_CHANG_REQ_CS] = HandlerItem(&MessageHandler::OnActivityHuanJieZhanChangReq, sizeof(Protocol::CSHuanJieZhanChangReq));
	m_msg_handler_list[Protocol::MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_REQ_CS] = HandlerItem(&MessageHandler::OnActivityHuanJieZhanChangRoleRankListReq, sizeof(Protocol::CSHuanJieZhanChangRoleRankListReq));

	m_msg_handler_list[Protocol::MT_LUN_HUI_WANG_CHUAN_REQ_CS] = HandlerItem(&MessageHandler::OnLunHuiWangChuanReq, sizeof(Protocol::CSLunHuiWangChuanReq));

	m_msg_handler_list[Protocol::MT_NETHER_WORLD_PURGATORY_REQ_CS] = HandlerItem(&MessageHandler::OnNetherWorldPurgatoryReq, sizeof(Protocol::CSNetherWorldPurgatoryReq));

	m_msg_handler_list[Protocol::MT_CHONGZHI_OPERATE_CS] = HandlerItem(&MessageHandler::OnChongzhiOperateReq, sizeof(Protocol::CSChongzhiOperateReq));
	m_msg_handler_list[Protocol::MT_REFER_ROLE_EQUIPMENT_CS] = HandlerItem(&MessageHandler::OnQueryOthersInfo, sizeof(Protocol::CSQueryRoleInfo));
	m_msg_handler_list[Protocol::MT_COG_OPERATOR_REQUEST_CS] = HandlerItem(&MessageHandler::OnCourseOfGrowthRequest, sizeof(Protocol::CSCOGOperatorRequest));

	m_msg_handler_list[Protocol::MT_ROLE_SUBSCRIBE_REQ_CS] = HandlerItem(&MessageHandler::OnRoleSubscribeTopic, sizeof(Protocol::CSRoleSubscribe));

	m_msg_handler_list[Protocol::MT_TEAM_SWITCH_POSITION_CS] = HandlerItem(&MessageHandler::OnTeamSwitchPositionReq, sizeof(Protocol::CSTeamSwitchPosition));
	m_msg_handler_list[Protocol::MT_ROLE_SALARY_INFO_CS] = HandlerItem(&MessageHandler::OnRoleSalaryInfoReq, sizeof(Protocol::CSRoleSalaryInfo));
	m_msg_handler_list[Protocol::MT_LOUD_SPEAKER_CS] = HandlerItem(&MessageHandler::OnSpeakerTalk, sizeof(Protocol::CSSpeaker));

	m_msg_handler_list[Protocol::MT_MYSTERIOUS_TRIAL_CS] = HandlerItem(&MessageHandler::OnMysteriousTrialReq, sizeof(Protocol::CSMysteriousTrialReq));
	m_msg_handler_list[Protocol::MT_TRANSACTION_TASK_CS] = HandlerItem(&MessageHandler::OnTransactionTaskReq, sizeof(Protocol::CSTransactionTaskReq));
	m_msg_handler_list[Protocol::MT_ACTIVE_SYSTEM_CS] = HandlerItem(&MessageHandler::OnActiveSystemReq, sizeof(Protocol::CSActiveSystemReq));
	m_msg_handler_list[Protocol::MT_HEART_SKILL_REQ_CS] = HandlerItem(&MessageHandler::OnHeartSkillReq, sizeof(Protocol::CSHeartSkillReq));
	m_msg_handler_list[Protocol::MT_HEART_SKILL_DISPLACE_REQ_CS] = HandlerItem(&MessageHandler::OnHeartSkillDisplaceReq, sizeof(Protocol::CSHeartSkillDisplaceReq));

	m_msg_handler_list[Protocol::MT_BATTLE_SET_STRATEGY_CS] = HandlerItem(&MessageHandler::OnBattleSetStrategyReq, sizeof(Protocol::CSBattleSetStrategy));
	m_msg_handler_list[Protocol::MT_BATTLE_CLEAR_STRATEGY_CS] = HandlerItem(&MessageHandler::OnBattleClearStrategyReq, sizeof(Protocol::CSBattleClearStrategy));
	m_msg_handler_list[Protocol::MT_BATTLE_LEAVE_OB_CS] = HandlerItem(&MessageHandler::OnBattleLeaveOBReq, sizeof(Protocol::CSBattleLeaveOB));

	m_msg_handler_list[Protocol::MT_TEAM_ROLE_VOTE_CS] = HandlerItem(&MessageHandler::OnTeamMemberVote, sizeof(Protocol::CSTeamVote));
	m_msg_handler_list[Protocol::MT_TEAM_LAUNCH_LEADER_VOTE_CS] = HandlerItem(&MessageHandler::OnTeamMemberLaunchVote, sizeof(Protocol::CSTeamLaunchLeaderVote));

	m_msg_handler_list[Protocol::MT_PATROL_CS] = HandlerItem(&MessageHandler::OnRolePatrolReq, sizeof(Protocol::CSRolePatrol));
	m_msg_handler_list[Protocol::MT_NEW_CHONG_ZHI_CONFIG_INFO_CS] = HandlerItem(&MessageHandler::OnNewChongzhiConfigInfoReq, sizeof(Protocol::CSNewChongzhiConfigInfoReq));
	m_msg_handler_list[Protocol::MT_TEAM_TEAM_LEADER_NPC_REQ_CS] = HandlerItem(&MessageHandler::OnTeamLeaderNpcReq, sizeof(Protocol::CSTeamLeaderNpcReq));
	m_msg_handler_list[Protocol::MT_NPC_SHOP_BUY_CS] = HandlerItem(&MessageHandler::OnNpcShopBuyReq, sizeof(Protocol::CSNPCShopBuyReq));

	m_msg_handler_list[Protocol::MT_BATTLE_CHAT_CS] = HandlerItem(&MessageHandler::OnRoleSendBattleChatReq, sizeof(Protocol::CSBattleChat));
	m_msg_handler_list[Protocol::MT_CONVERT_SHOP_BUY_REQ_CS] = HandlerItem(&MessageHandler::OnConvertShopBuyReq, sizeof(Protocol::CSConvertShopBuyReq));
	m_msg_handler_list[Protocol::MT_BACK_TO_ORIGIN_SERVER_CS] = HandlerItem(&MessageHandler::OnRoleBackToOriginServerReq, sizeof(Protocol::CSBackToOriginServer));

	m_msg_handler_list[Protocol::MT_TASK_PATH_FINDING_CS] = HandlerItem(&MessageHandler::OnRoleTaskPathFinding, sizeof(Protocol::CSRoleTaskPathFinding));
	m_msg_handler_list[Protocol::MT_ROLE_TALK_TO_NPC_CS] = HandlerItem(&MessageHandler::OnRoleTalkToNpcReq, sizeof(Protocol::CSRoleTalkToNpc));
	m_msg_handler_list[Protocol::MT_ROLE_LOGIN_SYSTEM_REQ_CS] = HandlerItem(&MessageHandler::OnRoleLoginSystemReq,sizeof(Protocol::CSLoginSystemREQ));
	m_msg_handler_list[Protocol::MT_CLIENT_BEHAVIOR_REQ_CS] = HandlerItem(&MessageHandler::OnAuditBehaviorSubmitReq,sizeof(Protocol::CSAuditBehaviorSubmitReq));

	m_msg_handler_list[Protocol::MT_BOSS_CHAPTER_REQ_CS] = HandlerItem(&MessageHandler::OnBossChapterReq, sizeof(Protocol::CSBossChapterReq));
	m_msg_handler_list[Protocol::MT_BOUNTY_TASK_TURN_TABLE_FETCH_REWARD_CS] = HandlerItem(&MessageHandler::OnBountyTaskTurnTableFetchRewardReq, sizeof(Protocol::CSBountyTaskTurnTableRewardFetch));

	m_msg_handler_list[Protocol::MT_PLATFORM_BATTLE_SWITCH_PLATFORM_CS] = HandlerItem(&MessageHandler::OnSwitchPlatformSceneReq, sizeof(Protocol::CSPlatformBattlePlaformSwitch));
	m_msg_handler_list[Protocol::MT_ESCAPE_FROM_CURRENT_POSITION] = HandlerItem(&MessageHandler::OnEscapeFromCurrentPositionReq, sizeof(Protocol::CSEscapeFromCurrentPosition));
	m_msg_handler_list[Protocol::MT_AUTO_ESCAPE_FROM_CURRENT_POSITION] = HandlerItem(&MessageHandler::OnAutoEscapeFromCurrentPositionReq, sizeof(Protocol::CSAutoEscapeFromCurrentPosition));
	m_msg_handler_list[Protocol::MT_WORLD_QUESTION_REQ_CS] = HandlerItem(&MessageHandler::OnWorldQuestionReq, sizeof(Protocol::CSWorldBoss2Req));
	m_msg_handler_list[Protocol::MT_REALTIME_COMMENT_CS] = HandlerItem(&MessageHandler::OnRealtimeCommentReq, sizeof(Protocol::CSRealTimeComment));
	m_msg_handler_list[Protocol::MT_RESEARCH_TASK_AUTO_START_CS] = HandlerItem(&MessageHandler::OnResearchTaskAutoStartReq, sizeof(Protocol::CSResearchTaskAutoStart));

	m_msg_handler_list[Protocol::MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_MATH_REQ_CS] = HandlerItem(&MessageHandler::OnSpecialBattleJoyReq, sizeof(Protocol::CSRAJoySeeksFavorsMatchReq));
	m_msg_handler_list[Protocol::MT_BATTLE_SPECIAL_JOY_REQ_CS] = HandlerItem(&MessageHandler::OnBattleJoyRoleOperateReq, sizeof(Protocol::CSBattleSpecialJoyReqInfo));	
	m_msg_handler_list[Protocol::MT_CHANNEL_CHAT_CS] = HandlerItem(&MessageHandler::OnChannelChat, sizeof(Protocol::CSChannelChat));
	m_msg_handler_list[Protocol::MT_SINGLE_CHAT_CS] = HandlerItem(&MessageHandler::OnSingleChat, sizeof(Protocol::CSSingleChat));
	m_msg_handler_list[Protocol::MT_CHAT_ITEM_INFO_REQ_CS] = HandlerItem(&MessageHandler::OnChatItemInfoReq, sizeof(Protocol::CSChatItemInfoReq));
	m_msg_handler_list[Protocol::MT_TEAM_LEADER_SUMMON_MEMBER_CS] = HandlerItem(&MessageHandler::OnTeamLeaderSummonMemberReq, sizeof(Protocol::CSTeamLeaderSummonMember));
	m_msg_handler_list[Protocol::MT_TEAM_MEMBER_REPLY_SUMMON_CS] = HandlerItem(&MessageHandler::OnTeamMemberReplySummon, sizeof(Protocol::CSTeamMemberReplyLeaderSummon));
	m_msg_handler_list[Protocol::MT_RANK_GET_PERSONRANK_LIST_CS] = HandlerItem(&MessageHandler::OnGetPersonRank, sizeof(Protocol::CSGetPersonRankList));
	m_msg_handler_list[Protocol::MT_RANK_GET_PETRANK_LIST_CS] = HandlerItem(&MessageHandler::OnGetPetRank, sizeof(Protocol::CSGetPetRankList));
	m_msg_handler_list[Protocol::MT_RANK_GET_REFRESH_TIMESTAMP_CS] = HandlerItem(&MessageHandler::OnGetRankRefreshReq, sizeof(Protocol::CSGetRankRefreshTime));
	m_msg_handler_list[Protocol::MT_RANK_GET_PETRANK_DETAILED_INFO_CS] = HandlerItem(&MessageHandler::OnGetPetRankDetailedInfoReq, sizeof(Protocol::CSGetRankPetDetailedInfo));
	m_msg_handler_list[Protocol::MT_TASK_CHOSED_REQ_CS] = HandlerItem(&MessageHandler::OnTaskChosedReq, sizeof(Protocol::CSTaskChosedReq));

	m_msg_handler_list[Protocol::MT_BATTLE_REPORT_SIMPLE_INFO_CS] = HandlerItem(&MessageHandler::OnBattleReportSimpleInfoReq, sizeof(Protocol::CSBattleReportSimpleInfo));
	m_msg_handler_list[Protocol::MT_BATTLE_REPORT_DETAIL_INFO_CS] = HandlerItem(&MessageHandler::OnBattleReportDetailInfoReq, sizeof(Protocol::CSBattleReportDetailInfo));
	m_msg_handler_list[Protocol::MT_BATTLE_REPORT_STOP_WATCHING_CS] = HandlerItem(&MessageHandler::OnBattleReportStopWatchingReq, sizeof(Protocol::CSBattleReportStopWatching));

	m_msg_handler_list[Protocol::MT_CHAT_GROUP_ROLE_INVITE_ENTER_CS] = HandlerItem(&MessageHandler::OnInviteEnterGroup, sizeof(Protocol::CSRoleInviteEnterGroup));
	m_msg_handler_list[Protocol::MT_CHAT_GROUP_ROLE_ENTER_GROUP_CS] = HandlerItem(&MessageHandler::OnEnterGroup, sizeof(Protocol::CSEnterGroupReq));
	m_msg_handler_list[Protocol::MT_CHAT_GROUP_CREATE_GROUP_CS] = HandlerItem(&MessageHandler::OnCreateGroup, sizeof(Protocol::CSCreateGroupReq));
	m_msg_handler_list[Protocol::MT_CHAT_GROUP_KICK_MEMBER_CS] = HandlerItem(&MessageHandler::OnGroupKickMember, sizeof(Protocol::CSGroupKickMember));
	m_msg_handler_list[Protocol::MT_CHAT_GROUP_DISSMISS_CS] = HandlerItem(&MessageHandler::OnGroupDismissReq, sizeof(Protocol::CSGroupDissmiss));
	m_msg_handler_list[Protocol::MT_CHAT_GROUP_LEAVE_CS] = HandlerItem(&MessageHandler::OnGroupLeaveReq, sizeof(Protocol::CSGroupLeaveReq));
	m_msg_handler_list[Protocol::MT_CHAT_GROUP_GET_GROUP_LIST_CS] = HandlerItem(&MessageHandler::OnGetGroupList, sizeof(Protocol::CSGetRoleGroupList));
	m_msg_handler_list[Protocol::MT_CHAT_GROUP_GET_GROUP_INFO_CS] = HandlerItem(&MessageHandler::OnGetGroupInfo, sizeof(Protocol::CSGetGroupInfo));
	m_msg_handler_list[Protocol::MT_WILD_BOSS_REQ_CS] = HandlerItem(&MessageHandler::OnWildBossReq, sizeof(Protocol::CSWildBossReq));
	m_msg_handler_list[Protocol::MT_WILD_BOSS_POKER_CS] = HandlerItem(&MessageHandler::OnWildBossPokerReq, sizeof(Protocol::CSWildBossPokerReq));

	m_msg_handler_list[Protocol::MT_ONE_KEY_FINISH_SC] = HandlerItem(&MessageHandler::OnTaskOneKeyFinish, sizeof(Protocol::CSOneKeyFinish));
	m_msg_handler_list[Protocol::MT_SKILL_POSITION_MODIFY_CS] = HandlerItem(&MessageHandler::OnSkillPositionModify, sizeof(Protocol::CSSkillPositionModify));
	m_msg_handler_list[Protocol::MT_PARTNER_SKILL_ACTIVATE_CS] = HandlerItem(&MessageHandler::OnPartnerSkillActivate, sizeof(Protocol::CSPartnerSkillActivate));

	m_msg_handler_list[Protocol::MT_POKER_REWARD_CS] = HandlerItem(&MessageHandler::OnPokerRewardReq, sizeof(Protocol::CSPokerReq));

	m_msg_handler_list[Protocol::MT_CHAT_GROUP_CHANGE_NOTICE_CS] = HandlerItem(&MessageHandler::OnGroupNoticeChangeReq, sizeof(Protocol::CSChangeGroupNoticeReq));

	m_msg_handler_list[Protocol::MT_HUASHEN_MEDAL_REQ_CS] = HandlerItem(&MessageHandler::OnHuaShenMedalReq, sizeof(Protocol::CSHuaShenMedalReq));
	m_msg_handler_list[Protocol::MT_HUASHEN_EQUIPMENT_OPERA_REQ_CS] = HandlerItem(&MessageHandler::OnHuaShenEquipmentReq, sizeof(Protocol::CSHuaShenEquipmentOperaReq));
	m_msg_handler_list[Protocol::MT_HUASHEN_JEWELRY_HANDLE_REQ_CS] = HandlerItem(&MessageHandler::OnHuaShenJewelryHandleReq, sizeof(Protocol::CSHuaShenJewelryHandleReq));
	m_msg_handler_list[Protocol::MT_HUASHEN_ATTR_PLAN_POINT_CS] = HandlerItem(&MessageHandler::OnHuaShenAttrPlanPonitReq, sizeof(Protocol::CSHuaShenAttributePlanPointReq));
	m_msg_handler_list[Protocol::MT_HUASHEN_PET_REQ_CS] = HandlerItem(&MessageHandler::OnHuaShenPetReq, sizeof(Protocol::CSHuaShenPetOp));
	m_msg_handler_list[Protocol::MT_HUASHEN_PET_ASSIGN_ATTR_POINT_CS] = HandlerItem(&MessageHandler::OnHuaShenPetAssignAttrPointReq, sizeof(Protocol::CSHuaShenPetAssignAttributePoints));
	m_msg_handler_list[Protocol::MT_HUASHEN_INFO_REQ_CS] = HandlerItem(&MessageHandler::OnHuaShenInfoReq, sizeof(Protocol::CSHuaShenInfoReq));
	m_msg_handler_list[Protocol::MT_HUASHEN_SET_BATTLE_POSITION_CS] = HandlerItem(&MessageHandler::OnHuaShenSetBattlePositionReq, sizeof(Protocol::CSSetHuaShenBattlePosition));
	m_msg_handler_list[Protocol::MT_HUASHEN_ATTR_PLAN_CHANGE_NAME_CS] = HandlerItem(&MessageHandler::OnHuaShenAttrPlanChangeNameReq, sizeof(Protocol::CSHuaShenAttributePlanChangeName));
	m_msg_handler_list[Protocol::MT_BATTLE_START_NPC_DEMO_FIGHT_CS] = HandlerItem(&MessageHandler::OnStartNpcDemoBattleReq, sizeof(Protocol::CSBattleStartNpcDemoFight));
	m_msg_handler_list[Protocol::MT_BATTLE_ROLE_REJOIN_BATTLE_SCENE_CS] = HandlerItem(&MessageHandler::OnRoleRejoinBattleSceneReq, sizeof(Protocol::CSRoleRejoinBattleScene));

	m_msg_handler_list[Protocol::MT_MIYI_POKER_REQ_CS] = HandlerItem(&MessageHandler::OnMiYiPokerReq, sizeof(Protocol::CSMiYiPokerReq));
	m_msg_handler_list[Protocol::MT_MIYI_REQ_CS] = HandlerItem(&MessageHandler::OnMiYiReq, sizeof(Protocol::CSMiYiReq));
	m_msg_handler_list[Protocol::MT_MEDAL_TASK_PLAY_VIDEO_CS] = HandlerItem(&MessageHandler::OnMedalTaskPlayVideoReq, sizeof(Protocol::CSMedalTaskPlayVideo));

	m_msg_handler_list[Protocol::MT_PET_EQUIPMENT_REQ_CS] = HandlerItem(&MessageHandler::OnPetEquipmentReq, sizeof(Protocol::CSPetEquipMentReq));
	m_msg_handler_list[Protocol::MT_FUNC_PREVIEW_CLAIM_REWARD_CS] = HandlerItem(&MessageHandler::OnClaimFuncPreviewReq, sizeof(Protocol::CSFuncPreviewClaimReward));

	m_msg_handler_list[Protocol::MT_REALM_BREAK_REQ_CS] = HandlerItem(&MessageHandler::OnRealmBreakReq, sizeof(Protocol::CSRealmBreakReq));
	m_msg_handler_list[Protocol::MT_MAP_EXPLORATION_REQ_CS] = HandlerItem(&MessageHandler::OnMapExplorationReq, sizeof(Protocol::CSMapExplorationReq));
	m_msg_handler_list[Protocol::MT_SET_AUTO_ADD_POINT_FLAG_CS] = HandlerItem(&MessageHandler::OnSetAutoAddPointFlagReq, sizeof(Protocol::CSSetAutoAddPointFlag));

	m_msg_handler_list[Protocol::MT_MATERIAL_REQ_CS] = HandlerItem(&MessageHandler::OnMaterialReq, sizeof(Protocol::CSMaterialReq));

	m_msg_handler_list[Protocol::MT_AUTO_CATCH_PET_CS] = HandlerItem(&MessageHandler::OnSetAutoCatchPetReq, sizeof(Protocol::CSSetAutoCatchPet));
	m_msg_handler_list[Protocol::MT_FORMATION_SIMPLE_INFO_CS] = HandlerItem(&MessageHandler::OnFormationSimpleInfoReq, sizeof(Protocol::CSFormationSimpleInfo));
	m_msg_handler_list[Protocol::MT_FORMATION_DETAIL_INFO_CS] = HandlerItem(&MessageHandler::OnFormationDetailInfoReq, sizeof(Protocol::CSFormationDetailInfo));
	m_msg_handler_list[Protocol::MT_FORMATION_SAVE_CS] = HandlerItem(&MessageHandler::OnFormationSaveReq, sizeof(Protocol::CSFormationSave));
	m_msg_handler_list[Protocol::MT_FORMATION_RENAME_CS] = HandlerItem(&MessageHandler::OnFormationRenameReq, sizeof(Protocol::CSFormationRename));
	m_msg_handler_list[Protocol::MT_FORMATION_DELETE_CS] = HandlerItem(&MessageHandler::OnFormationDeleteReq, sizeof(Protocol::CSFormationDelete));
	m_msg_handler_list[Protocol::MT_PET_HELPER_FORMATION_SET_CS] = HandlerItem(&MessageHandler::OnSetPetHelperFormationReq, sizeof(Protocol::CSSetPetHelperFormation));
	m_msg_handler_list[Protocol::MT_PET_HELPER_FORMATION_QUERY_CS] = HandlerItem(&MessageHandler::OnQueryPetHelperFormationReq, sizeof(Protocol::CSPetHelperFormationQuery));

	m_msg_handler_list[Protocol::MT_MYSTERY_STORE_OP_CS] = HandlerItem(&MessageHandler::OnMysteryStoreReq, sizeof(Protocol::SCMysteryStoreReq));
	m_msg_handler_list[Protocol::MT_GUILD_INBORN_REQ_CS] = HandlerItem(&MessageHandler::OnGuildInbornReq, sizeof(Protocol::CSGuildInbornReq));
	m_msg_handler_list[Protocol::MT_SHAN_HAI_BOSS_OP_CS] = HandlerItem(&MessageHandler::OnShanHaiBossReq, sizeof(Protocol::CSShanHaiBossReq));

	m_msg_handler_list[Protocol::MT_GET_CUR_POS_CS] = HandlerItem(&MessageHandler::OnGetCurPosReq, sizeof(Protocol::CSGetCurPos));
	m_msg_handler_list[Protocol::MT_CRYSTAL_FB_FAST_PASS_CS] = HandlerItem(&MessageHandler::OnCrysalFbFastPassReq, sizeof(Protocol::CSCrystalFbFastPassInfo));
	m_msg_handler_list[Protocol::MT_WEN_XIANG_LOU_FB_REQ_CS] = HandlerItem(&MessageHandler::OnWenXiangLouReq, sizeof(Protocol::CSWenXiangLouReq));
	m_msg_handler_list[Protocol::MT_NEW_TASK_ACTION_REQ_CS] = HandlerItem(&MessageHandler::OnNewTaskActionReq, sizeof(Protocol::CSNewTaskAction));
	m_msg_handler_list[Protocol::MT_NEW_TASK_GIVE_NPC_ITEM_CS] = HandlerItem(&MessageHandler::OnNewTaskGiveNPCItems, sizeof(Protocol::CSNewTaskGiveNPCTaskNeedItems));
	m_msg_handler_list[Protocol::MT_ANECDOTE_TASK_OPERATE_REQ_CS] = HandlerItem(&MessageHandler::OnAnecdoteTaskOperateReq, sizeof(Protocol::CSAnecdoteTaskOperate));
	m_msg_handler_list[Protocol::MT_ANECDOTE_TASK_SPECIAL_SET_CS] = HandlerItem(&MessageHandler::OnAnecdoteTaskSpecialReq, sizeof(Protocol::CSAnecdoteTaskSpecialReq));
	m_msg_handler_list[Protocol::MT_NORMAL_TASK_FINISH_ANEDOTE_GAME_CS] = HandlerItem(&MessageHandler::OnNormalTaskFinishAnedoteGameReq, sizeof(Protocol::CSNormalTaskFinishAnedoteGame));

	m_msg_handler_list[Protocol::MT_NEW_TASK_CATCH_FOX_REQ_CS] = HandlerItem(&MessageHandler::OnSpeciallogicCatchFoxReq, sizeof(Protocol::CSCatchFoxSpecialLogicReq));
	m_msg_handler_list[Protocol::MT_SCHOOL_EXERCISE_CHALLENGE_CS] = HandlerItem(&MessageHandler::OnSchoolExerciseChallengeReq, sizeof(Protocol::CSSchoolExerciseChallenge));
	m_msg_handler_list[Protocol::MT_SCHOOL_EXERCISE_FIRST_OPEN_UI] = HandlerItem(&MessageHandler::OnSchoolExerciseFirstOpenUI, sizeof(Protocol::CSSchoolExerciseFirstOpenUI));
	m_msg_handler_list[Protocol::MT_MI_JING_QI_WEN_QUESTION_REQ_CS] = HandlerItem(&MessageHandler::OnSpeciallogicMiJingQiWenReq, sizeof(Protocol::CSRAMiJingQiWenQuestionReq));
	m_msg_handler_list[Protocol::MT_MI_JING_QI_WEN_BUY_ITEM_REQ_CS] = HandlerItem(&MessageHandler::OnSpeciallogicMiJingQiWenBuyReq, sizeof(Protocol::CSRAMiJingQiWenBuyItemReq));
	m_msg_handler_list[Protocol::MT_MI_JING_QI_WEN_2_QUESTION_REQ_CS] = HandlerItem(&MessageHandler::OnSpeciallogicMiJingQiWenReq2, sizeof(Protocol::CSRAMiJingQiWenQuestionReq2));
	m_msg_handler_list[Protocol::MT_MI_JING_QI_WEN_2_BUY_ITEM_REQ_CS] = HandlerItem(&MessageHandler::OnSpeciallogicMiJingQiWenBuyReq2, sizeof(Protocol::CSRAMiJingQiWenBuyItemReq2));
	m_msg_handler_list[Protocol::MT_ROLE_INFO_NOTICE_REQ_CS] = HandlerItem(&MessageHandler::OnRoleInfoNoticeReq, sizeof(Protocol::CSRoleInfoNoticeReq));
	m_msg_handler_list[Protocol::MT_ROLE_TOP_LEVEL_REQ_CS] = HandlerItem(&MessageHandler::OnRoleTopLevelReq, sizeof(Protocol::CSRoleTopLevelReq));

	m_msg_handler_list[Protocol::MT_TIAN_DI_DAO_HEN_REQ_CS] = HandlerItem(&MessageHandler::OnTianDiDaoHenReq, sizeof(Protocol::CSTianDiDaoHenReq));
	m_msg_handler_list[Protocol::MT_GIVE_GIFT_REQ_CS] = HandlerItem(&MessageHandler::OnGiftReq, sizeof(Protocol::CSGiftReq));
	m_msg_handler_list[Protocol::MT_CAPACITY_TARGET_REQ_CS] = HandlerItem(&MessageHandler::OnCapacityTargetReq, sizeof(Protocol::CSCapacityTargetReq));

	MESSAGE_HANDLER_CONSTRUCT(MT_CLOSE_WINDOS_REQ_CS, OnTeamLeaderCloseWindosReq, CSCloseWindosReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_DRAW_REQ_CS, OnDrawManagerReq, CSDrawReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_LEVEL_COMPLEMENT_CS, OnLevelComplementReq, CSLevelComplementReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_NEW_LEVEL_COMPLEMENT_REQ_CS, OnNewLevelComplementReq, CSRoleNewLevelComplementReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_DREAM_NOTES_CHALLENGE_CS, OnDreamNotesChallengeReq, CSDreamNotesChallengeOp);
	MESSAGE_HANDLER_CONSTRUCT(MT_WORKSHOP_EQUIP_DECOMPOSE_CS, OnWorkShopDecomposeReq, CSWorkshopDecomposeReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_CS, OnTempChatGroupBecomeOfficialReq, CSTempChatGroupBecomeOfficialReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_SYSTEM_SET_CHAT_GROUP_CS, OnRoleClientSystemSetChatGroupReq, CSRoleSystemSetChatGroupReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_BATTLE_ANIMATION_SPEED_MODIFY_CS, OnBattleRoleModifyAnimationSpeedReq, CSBattleAnimationSpeedModify);
	MESSAGE_HANDLER_CONSTRUCT(MT_CHAT_GROUP_HANDOVER_TO_OTHERS_CS, OnHandoverChatGroupLeadershipReq, CSHandoverChatGroupLeadership);
	MESSAGE_HANDLER_CONSTRUCT(MT_ACTIVE_SYSTEM_EXTRA_REWARD_FETCH_CS, OnActiveSystemFetchExtraRewardReq, CSActiveSystemExtraRewardFetch);
	MESSAGE_HANDLER_CONSTRUCT(MT_MONTH_CARD_REQ_CS, OnMonthCardReq, CSMonthCardReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_PET_MONOPOLY_OPERATE_CS, OnPetMonopolyOperateReq, CSPetMonopolyOperate);
	MESSAGE_HANDLER_CONSTRUCT(MT_CLOUD_ARENA_OPERATE_CS, OnCloudArenaOperateReq, CSCloudArenaOperate);
	MESSAGE_HANDLER_CONSTRUCT(MT_CLOUD_ARENA_SET_FORMATION_REQ, OnCloudArenaSetFormationReq, CSCloudArenaSetFormation);
	MESSAGE_HANDLER_CONSTRUCT(MT_CLOUD_ARENA_REAL_TIME_INFO_REQ, OnCloudArenaRealTimeInfoReq, CSCloudArenaRealTimeInfo);
	MESSAGE_HANDLER_CONSTRUCT(MT_RELIC_LOOTING_MATH_CS, OnRelicLootingMachReq, CSRelicLootingMatchReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_RELIC_LOOTING_PLAYER_REQ_CS, OnRelicLootingUseSkillReq, CSRelicLootingPlayerReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_MOVE_ASSIST_RET_CS, OnMoveAssistReq, CSMoveAssistRet);
	MESSAGE_HANDLER_CONSTRUCT(MT_PET_DEAD_GUIDE_TRIGGER_CS, OnRoleTriggerPetDeadSummonGuideReq, CSPetDeadSummonGuideTrigger);
	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_HISTORY_CHIVALROUS_REWARD_FETCH, OnRoleFetchHistoryChivalrousRewardReq, CSRoleHistoryChivalrousRewardFetch);
	MESSAGE_HANDLER_CONSTRUCT(MT_CHIVALROUS_TEAM_LIST_REQ_CS, OnUnfullChivalrousTeamListReq, CSChivalrousTeamListReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_RANK_GET_CHIVALROUS_REQ_CS, OnQueryWeekChivalrousRankListReq, CSChivalrousRankListReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_JOIN_ACTIVITY_REWARD_REQ_CS, OnRoleJoinActivityRewardReq, CSRoleJoinActivityRewardReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_GUILD_ACTIVE_PERSON_RANK_CS, OnGuildActivePersonRankReq, CSGuildActivePersonRank);
	MESSAGE_HANDLER_CONSTRUCT(MT_GUILD_ACTIVE_GET_GIFT_CS, OnGuildActiveGetGiftReq, CSGuildActiveGetGift);
	MESSAGE_HANDLER_CONSTRUCT(MT_CUSTOM_INSTRUCTION_ADD_CS, OnAddCustomInstructionReq, CSCustomInstructionAdd);
	MESSAGE_HANDLER_CONSTRUCT(MT_CUSTOM_INSTRUCTION_REMOVE_CS, OnRemoveCustomInstructionReq, CSCustomInstructionRemove);
	MESSAGE_HANDLER_CONSTRUCT(MT_BATTLE_SET_CUSTOM_INSTRUCTION_CS, OnBattleSetCustomInstructionReq, CSBattleSetCustomInstruction);
	
	MESSAGE_HANDLER_CONSTRUCT(MT_CUSTOM_INSTRUCTION_BATCH_REMOVE_CS, OnBatchRemoveCustomInstructionReq, CSCustomInstructionBatchRemove);
	MESSAGE_HANDLER_CONSTRUCT(MT_PET_SOUL_UPLEVEL_REQ_CS, OnPetSoulUpLevelReq, CSPetSoulUplevelReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_PET_GOD_PRINT_SET_WEAR_SIGLE_REQ_CS, SetPetGodPrintWearSchemeReq, CSPetGodPrintSetearSchemeReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_PET_GOD_PRINT_USE_ITEM_UPLEVEL_CS, OnPetGodPrintUseItemUpLevelReq, CSPetGodPrintUplevelReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_PETY_GOD_FB_REQ_CS, OnPetGodFBReq, CSPetGodFBReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_DU_JIE_REQ_CS, OnDuJieReq, CSDuJieReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_HONG_BAO_CLIENT_REQ_CS, OnHongBaoClientReq, CSHongBaoClientReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_BIG_DIPPER_REQ_CS, OnBigDipperReq, CSBigDipperReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_FALLEN_GOD_REQ_CS, OnFallenGodReq, CSFallenGodReq);

	//获取资源下载奖励
	MESSAGE_HANDLER_CONSTRUCT(MT_FETCH_RESOURCE_DOWNLOAD_REWARD_CS, OnFetchRecourceDownloadReward, CSFetchRecourceDownloadReward);
	MESSAGE_HANDLER_CONSTRUCT(MT_PRESTIGE_TASK_SELECT_VILLAGE_CS, OnPrestigeTaskSelectVillageReq, CSPrestigeTaskSelectVillage);
	MESSAGE_HANDLER_CONSTRUCT(MT_PRESTIGE_TASK_REQ_CS, OnPrestigeTaskReq, CSPrestigeTaskReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_SVIP_CLIENT_CUSTOM_DATA_CS, OnSVipReq, CSSvipClientCustomData);

	MESSAGE_HANDLER_CONSTRUCT(MT_MEDAL_UP_GRADE_SPECIAL, OnMedalUpGradeSpecialReq, CSMedalUpGradeSpecialReq);

	//防诈骗奖励回答
	MESSAGE_HANDLER_CONSTRUCT(MT_FANG_FRADU_REWARD_REQ_CS, OnFangFraudRewardReq, CSFangFraudRewardReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_ATTR_PELLET_REQ_CS, OnRoleAttrPelletReq, CSRoleAttrPelletReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_ACTIVE_TURNTABLE_REQ_CS, OnActiveTurntableReq, CSActiveTurntableReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_RAND_ACTIVITY_BACK_REWARD_REQ_CS, OnBackRewardReq, CSBackRewardReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_ACCESSORIES_REQ_CS, OnAccessoriseReq, CSAccessoriesMentOperaReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ACCESSORIES_VOC_COMPOSE_CS, OnAccessoriseVocChangeReq, CSAccessoriesMentVocComposeReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_YIN_YANG_EYE_OP_REQ_CS, OnYinYangEyeOpReq, CSYinYangEyeOpReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ENDLESS_TOWER_REQ_CS, OnEndlessTowerOpReq, CSEndlessTowerReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_REQ_CS, OnJieYiReq, CSJieYiReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_CREATE_CS, OnJieYiCreate, CSJieYiCreate);
	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_CREATE_NAME_CS, OnJieYiCreateName, CSJieYiCreateName);
	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_ZU_MEMBER_NAME_CS, OnJieYiMemberName, CSJieYiZuMemberName);
	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_ZU_TEXT_CS, OnJieYiText, CSJieYiZuText);
	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_ZU_NAME_CS, OnJieYiName, CSJieYiZuName);
	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_ZU_XUAN_YAN_REQ_CS, OnJieYiXuanYanReq, CSJieYiZuXuanYanReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_JIE_YI_ZU_XUAN_YAN_REQ_LIST_CS, OnJieYiXuanYanListReq, CSJieYiZuXuanYanReqList);

	MESSAGE_HANDLER_CONSTRUCT(MT_QING_YUAN_REQ_CS, OnQingYuanReq, CSQingYuanReq);

	// 福牛报喜
	MESSAGE_HANDLER_CONSTRUCT(MT_COW_REPORT_GOOD_NEWS_CLIENT_REQ_CS, OnCowReportGoodNewsClientReq, CSCowReportGoodNewsClientReq);

	// 寅虎纳福
	MESSAGE_HANDLER_CONSTRUCT(MT_TIGER_BRINGS_BLESSING_CLIENT_REQ_CS, OnTigerBringsBlessingClientReq, CSTigerBringsBlessingClientReq);

	//万灵话本
	MESSAGE_HANDLER_CONSTRUCT(MT_WAN_LING_FB_REQ_CS, OnWanLingFBReq, CSWanLingFBReq);

	//情缘 婚宴
	MESSAGE_HANDLER_CONSTRUCT(MT_WEDDING_FB_REQ_CS, OnWeddingFBReq, CSWeddingFBReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_TRADE_MARKET_QUERY_ITEM_RECOMMEND_PRICE, OnQueryItemRecommendPriceReq, CSQueryItemRecommendPrice);
	MESSAGE_HANDLER_CONSTRUCT(MT_BATTLE_SKIP_CS, OnBattleSkipReq, CSBattleSkipReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_BATTLE_FAST_FORWARD_CS, OnBattleFastForwardReq, CSBattleFastForwardReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_FLY_UP_SYSTEM_REQ_CS, OnFlyUpSystemReq, CSFlyUpSystemReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_MIRAGE_TERRITORY_REQ_CS, OnMirageTerritoryReq, CSMirageTerritoryReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_MIRAGE_TERRITORY_SET_PARTNER_REQ_CS, OnMirageTerritoryPartnerReq, CSMirageTerritorySetPartnerReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_CHANGE_COLOR_REQ_CS, OnRoleChangeColorReq, CSRoleChangeColorReq);
	
	MESSAGE_HANDLER_CONSTRUCT(MT_XIU_WEI_REQ_CS, OnXiuWeiReq, CSXiuWeiReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_TREASURE_TASK_REQ_CS, OnTreasureTaskReq, CSTreasureTaskReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_TEAM_LEADER_EJECT_WINDOWS_CS, OnTeamLeaderEjectWindowsReq, CSTeamLeaderEjectWindowsReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_WORLD_ARENA_OPERATE_CS, OnWorldArenaRoleOperateReq, CSWorldArenaOperate);

	MESSAGE_HANDLER_CONSTRUCT(MT_PET_BATCH_COMPOSE_CS, OnPetComposeBatch, CSPetComposeBatch);
	MESSAGE_HANDLER_CONSTRUCT(MT_PET_BATCH_COMPOSE_AND_DECOMPOSE_CS, OnPetComposeAndDecomposeBatch, CSPetComposeAndDecomposeBatch);
	MESSAGE_HANDLER_CONSTRUCT(MT_PET_DECOMPOSE_DECOMPOSE_CS, OnPetBadgeDecompose, CSPetBadgeDecompose);

	MESSAGE_HANDLER_CONSTRUCT(MT_MINI_GAME_FINISH_STAGE_CS, OnMiniGameClearNextStageReq, CSMiniGameFinishNextStage);
	MESSAGE_HANDLER_CONSTRUCT(MT_MINI_GAME_CLAIM_PHASE_REWARD_CS, OnMiniGameFetchPhaseRewardReq, CSMiniGameClaimPhaseReward);
	MESSAGE_HANDLER_CONSTRUCT(MT_MINI_GAME_START_BATTLE, OnMiniGameStartBattleReq, CSMiniGameStartBattle);
	MESSAGE_HANDLER_CONSTRUCT(MT_MINI_GAME_SET_MINI_GAME_ID, OnMiniGameSetMiniGameIdReq, CSMiniGameSetMiniGameId);

	MESSAGE_HANDLER_CONSTRUCT(MT_WORLD_TEAM_ARENA_REQ_CS, OnWorldTeamArenaReq, CSWorldTeamArenaReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_WORLD_TEAM_ARENA_SIGN_UP_REQ_CS, OnWorldTeamArenaSignUpReq, CSWorldTeamArenaSignUpReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_WORLD_TEAM_ARENA_SCENE_OP_REQ_CS, OnWorldTeamArenaSceneReq, CSActivityWorldTeamArenaSceneReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_GUILD_HALLOFFAME_REQ_CS, OnGuildHallOfFameReq, CSGuildHallOfFameReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_TASK_SKIP_CS, OnTaskSkipReq, CSTaskSkipReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_WORLD_TEAM_ARENA_FINALLY_RANK_REQ_CS, OnWorldTeamArenaFinallyRankReq, CSActivityWorldTeamArenaRankReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_PEAK_TOKEN_ROLE_REQ_CS, OnPeaktokenRoleReq, CSPeaktokenRoleReq);
	
	MESSAGE_HANDLER_CONSTRUCT(MT_INCOME_ROLE_REQ_CS, OnIncomeRoleReq, CSIncomeRoleReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_PET_STRENGTHEN_REWARD_REQ_CS, OnPetStrengthenRewardReq, CSPetStrengthenRewardReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_PARTNER_INTENSIFY_CS, OnPartnerIntensifyReq, CSPartnerIntensifyReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_QING_YUAN_SHENG_HUI_REQ_CS, OnQingYuanShengHuiReq, CSQingYuanShengHuiReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_QING_YUAN_SHENG_HUI_REQ_2_CS, OnQingYuanShengHuiReq2, CSQingYuanShengHuiReq2);
	MESSAGE_HANDLER_CONSTRUCT(MT_TU_ZI_KUAI_PAO_REQ_CS, OnTuZiKuaiPaoReq, CSTuZiKuaiPaoReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_GOTO_SCHOOL_REQ_CS, OnGotoSchoolReq, CSGotoSchoolReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_YAO_XING_SHI_LIAN_REQ_CS, OnYaoXingShiLianReq, CSYaoXingShiLianReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_SHI_TU_CHAUN_GONG_FB_REQ, OnShiTuChuanGongFBReq, CSShiTuChuanGongFBReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_QINGYUAN_DUIDUIPENG_REQ_CS, OnQingYuanDuiDuiPengReq, CSQingYuanDuiDuiPengReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_SAN_XIAN_GUI_DONG_REQ_CS, OnSanXianGuiDongReq, CSSanXianGuiDongReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_LANTERN_RIDDIE_REQ_CS, OnLanternRiddieReq, LanternRiddieCSReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_CA_WHERE_IS_THE_FAIRY_OPERA_REQ_CS, OnWhereIsTheFairyReq, CSRoleWhereIsTheFairyOperaReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_TAO_QUAN_NA_JI_REQ_CS, OnTaoQuanNaJiReq, CSTaoQuanNaJiReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_HUNDRED_GOD_BOSS_REQ_CS, OnHundredGodBossReq, CSHundredGodBossReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_CA_TIAN_GONG_XUN_BAO_OP_REQ_CS, OnTianGongXunBaoReq, CSTianGongXunBaoClientOpReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_LOVERS_MARKET_REQ_CS, OnLoversMarketReq, CSLoversMarketReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_CONCENTRIC_KNOT_REQ_CS, OnConcentricKnotReq, CSConcentricKnotReq);


	MESSAGE_HANDLER_CONSTRUCT(MT_GET_UTA_CHAMPION_RECORD_CS, OnGetUTAChampionRecord, CSGetUTAChampionRecord);

	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_GUESS_CLIENT_REQ_CS, OnUTAGuessClientReq, CSUTAGuessClienReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_TEAM_LIST_CS, OnUTATeamListReq, CSUTATeamListReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_TEAM_DETAIL_CS, OnUTATeamDetailInfoReq, CSUTATeamDetailReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_FETCH_WIN_BOX_CS, OnUTAFetchWinBoxReq, CSUTAFetchWinBox);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_CS, OnUTAMyTeamQualificationSchedulesReq, CSUTAMyTeamQualificationScheduleReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_TEAM_QUALIFICATION_RANK_LIST_CS, OnUTAGetTeamQualificationRankListReq, CSUTATeamScoreRankListReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_MAIN_SCHEDULES_CS, OnUTAGetMainScheduleReq, CSUTAMainSchedules);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_FINAL_RANK_LIST_CS, OnUTAFinalRankListReq, CSUTAFinalRankList);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_QUALIFICATION_SCHEDULES_CS, OnUTAGetQualificationSchedulesReq, CSUTAQualificationSchedules);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_MAIN_SCHEDULES_OVERVIEW_CS, OnUTAGetMainSchedulesOverviewReq, CSUTAMainSchedulesOverview);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_ENTER_SCENE_CS, OnUTAEnterBattleSceneReq, CSUTAEnterScene);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_WATCH_VIDEO_CS, OnUTAWatchVideoReq, CSUTAWatchVideo);
	MESSAGE_HANDLER_CONSTRUCT(MT_UTA_START_OB_CS, OnUTAStartOBReq, CSUTAStartOB);

	MESSAGE_HANDLER_CONSTRUCT(MT_PINGJIA_REWARD_REQ_CS, OnFetchPingJiaRewardReq, CSFetchPingJiaRewardReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_ZAOHUA_EQUIP_OPERA_REQ_CS, OnZaoHuaEquipmentOperateReq, CSZaoHuaEquipmentOperaReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_HONGMENG_TIANJIE_REQ_CS, OnHongMengTianJieReq, CSHongMengTianJieReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ZAO_HUA_EQUIPMENT_REQ_CS, OnZaoHuaEquipmentReq, CSZaoHuaEquipmentReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ZAO_HUA_YUAN_HUN_ZHU_REQ_CS, OnZaoHuaYuanHunZhuReq, CSZaoHuaYuanHunZhuReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ZAOHUA_DISCOMPOSE_CS, OnZaoHuaEquipmentDiscomposeReq, CSZaoHuaEquipmentDiscomposeReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ZAO_HUA_YUAN_HUN_ZHU_UP_LEVEL_CS, OnZaoHuaYuanHunZhuUpLevelReq, CSYuanHunZhuUpLevelReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_HOLY_EQUIP_REQ_CS, OnHolyEquipReq, CSHolyEquipOpReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_HOLY_EQUIP_LEVEL_UP_REQ_CS, OnHolyItemLevelUpReq, CSHolyEquipLevelUpReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_TEAM_ROLE_RECRUIT_ANSWER_CS, OnRoleAnswerTeamRecruitReq, CSTeamRoleRecruitAnswer);
	MESSAGE_HANDLER_CONSTRUCT(MT_KE_JU_EXAM_REQ_CS, OnKeJuExamReq, CSKeJuExamCSReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_USER_CHAT_RECORD_CS, OnQueryUserChatRecord, CSUserChatRecord);

	MESSAGE_HANDLER_CONSTRUCT(MT_PARTNER_PET_RANK_REQ_CS, OnPartnerPetRankReq, CSPartnerPetRankReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_KUA_FU_YING_XIONG_LU_REQ_CS, OnKuaFuYingXiongLuReq, CSKuaFuYingXiongLuReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_FU_YU_OPERATE_REQ_CS, OnFuYuManagerReq, CSFuYuReq); 

	MESSAGE_HANDLER_CONSTRUCT(MT_RA_YUE_XI_DENG_MI_REQ_CS, OnRAYueXiDengMiReq, CSRAYueXiDengMiRequest);

	MESSAGE_HANDLER_CONSTRUCT(MT_STAR_CHART_REQUEST_CS, OnStarChartOpReq, CSStarChartRequest);
	MESSAGE_HANDLER_CONSTRUCT(MT_FASHION_COLLECTION_REQ_CS, OnFashionCollectionOpReq, CSFashionCollectionRequest);
	MESSAGE_HANDLER_CONSTRUCT(MT_NEW_COURAGE_CHALLENGE_REQ_CS, OnNewCourageChallengeOpReq, CSNewCourageChallengeOp);

	MESSAGE_HANDLER_CONSTRUCT(MT_HUAN_SHOU_REQ_CS, OnHuanShouReq, CSHuanShouReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_HUAN_SHOU_DECOMPOSE_REQ_CS, OnHuanShouDecomposeReq, CSHuanShouDecomposeReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_SPECIAL_ANECDOTE_STAR_CS, OnAnecdoteSpecialReq, CSSpecialAnecdoteReq);
	MESSAGE_HANDLER_CONSTRUCT(MT_ROLE_GUILD_TASK_REQ_CS, OnRoleGuildTaskReq, CSRoleGuildTaskReq);

	MESSAGE_HANDLER_CONSTRUCT(MT_REAL_NAME_REWARD_RECEIVE_CS, OnReceiveRealNameRewardReq, CSRealNameRewardReq);
}

MessageHandler::~MessageHandler()
{ 

}

void MessageHandler::Init(SceneManager* scene_manager)
{
	m_scene_manager = scene_manager;
}

void MessageHandler::Release()
{
	m_scene_manager = NULL;
}

void MessageHandler::HandleMessageFromClient(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, const char *msg, unsigned int length)
{
	Protocol::MessageHeader *header = (Protocol::MessageHeader*)msg;

	unsigned short msg_type = header->msg_type;

	{
		// crossserver 协议 转发
		if (msg_type >= Protocol::CROSS_SERVER_MSG_BEGIN && msg_type < Protocol::CROSS_SERVER_MSG_END)
		{
			InternalComm::Instance().UserMsgTransmitToCross(role->GetUserId(), msg_type, msg, length);
			return;
		}
	}

	if (NULL == m_msg_handler_list[msg_type].func)
	{
		printf("MessageHandler::HandleMessage NULL == m_msg_handler_list[msg_type].func Error type:%d \n", (int)msg_type);
		// EngineAdapter::Instance().NetDisconnect(netid);
		gamelog::g_log_messagehandler.printf(LL_WARNING, "HandleMessage Unknow msg[%d].", (int)header->msg_type);
		return;
	}

	#ifdef GAMEWORLD_PERFORMANCE_CHECK
		PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_MESSAGE_HANDLE);
	#endif

	unsigned int msg_size = static_cast<unsigned int>(m_msg_handler_list[msg_type].data_size);

	if (Protocol::MT_TASK_GIVE_NPC_ITEM_CS == msg_type ||
	    Protocol::MT_TASK_SET_TRACE_REQ_CS == msg_type ||
	    Protocol::MT_FRIEND_EDIT_GRUOP_CS == msg_type ||
	    Protocol::MT_FRIEND_MOVE_GRUOP_CS == msg_type ||
	    Protocol::MT_ROLE_SYSTEM_SET_REQ_CS == msg_type ||
	    Protocol::MT_WORKSHOP_EXC_SKILL_BOOK_CS == msg_type ||
	    Protocol::MT_USE_OPTIONAL_GIFT_CS == msg_type ||
	    Protocol::MT_LOUD_SPEAKER_CS == msg_type ||
	    Protocol::MT_TEAM_TEAM_LEADER_NPC_REQ_CS == msg_type ||
	    Protocol::MT_PLATFORM_BATTLE_RANK_FIRST_INFO_REQ_CS == msg_type ||
	    Protocol::MT_BATTLE_CHAT_CS == msg_type ||
	    Protocol::MT_ABANDON_PET_BY_LIST_CS == msg_type ||
	    Protocol::MT_REALTIME_COMMENT_CS == msg_type ||
	    Protocol::MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_REQ_CS == msg_type ||
	    Protocol::MT_SKILL_POSITION_MODIFY_CS == msg_type ||
		Protocol::MT_RANK_GET_COURAGE_CHALLENGE_REQ_CS == msg_type ||
		Protocol::MT_AUTO_CATCH_PET_CS == msg_type ||
	    Protocol::MT_XUN_BAO_EXCHANGE_CS == msg_type ||
	    Protocol::MT_XUN_BAO_REQ_CS == msg_type ||	    
	    Protocol::MT_ZHU_LING_DECOMPOSE_REQ_CS == msg_type ||
	    Protocol::MT_ZHU_LING_UP_LEVEL_REQ_CS == msg_type ||
	    Protocol::MT_GIVE_GIFT_REQ_CS == msg_type ||
		Protocol::MT_FORMATION_SAVE_CS == msg_type ||
		Protocol::MT_NEW_TASK_GIVE_NPC_ITEM_CS == msg_type ||
		Protocol::MT_MOVE_ASSIST_RET_CS == msg_type ||
		Protocol::MT_CUSTOM_INSTRUCTION_BATCH_REMOVE_CS == msg_type ||
		Protocol::MT_PET_BATCH_COMPOSE_CS == msg_type ||
		Protocol::MT_PET_BATCH_COMPOSE_AND_DECOMPOSE_CS == msg_type ||
		Protocol::MT_PET_DECOMPOSE_DECOMPOSE_CS == msg_type ||
		Protocol::MT_PARTNER_INTENSIFY_CS == msg_type ||
		Protocol::MT_HUAN_SHOU_DECOMPOSE_REQ_CS == msg_type 
	    ? length <= msg_size : length >= msg_size)
	{
		// 执行协议
		(this->*m_msg_handler_list[msg_type].func)(netid, scene, role, obj_id, (void*)msg, length);
	}
	else
	{
		printf("MessageHandler::HandleMessage msg_size Error type:%d \n", (int)msg_type);
		// EngineAdapter::Instance().NetDisconnect(netid);
		gamelog::g_log_messagehandler.printf(LL_CRITICAL, "HandleMessage msg[%d] length[%d] is not %d.", (int)header->msg_type, length, msg_size);
	}

#ifdef _DEBUG
	 	//gamelog::g_log_messagehandler.printf(LL_DEBUG, "HandleMessage msg[%d] length[%d]", (int)header->msg_type, length);
	 	//printf("HandleMessage msg[%d] length[%u]\n", (int)header->msg_type, length);
#endif
	#ifdef GAMEWORLD_PERFORMANCE_CHECK
		long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_MESSAGE_HANDLE);
		//printf("[DEBUG] client msg type=%d\n", (int)header->msg_type);
		if (elapse_us > 2000)
		{	
			gamelog::g_log_elapse.printf(LL_CRITICAL, "HandleMessage msg[%d] length[%d] cost_time[%lld]", (int)header->msg_type, length, elapse_us);
		}
	#endif

}

void MessageHandler::HandleMessageFromClientNoLogin(const GSNetID &netid, IP ip, const char *msg, unsigned int length)
{
	Protocol::MessageHeader *header = (Protocol::MessageHeader*)msg;

	switch (header->msg_type)
	{
	case Protocol::MT_ENTER_GS_REQ_CS:
		{
			if (length == sizeof(Protocol::CSUserEnterGSReq))
			{
				this->OnReqEnter(netid, ip, msg);
			}
			else
			{
				gamelog::g_log_messagehandler.printf(LL_CRITICAL, "HandleMessageNoLogin Unknow msg type:%d length:%d.", (int)header->msg_type, length);
				// EngineAdapter::Instance().NetDisconnect(netid);
				//printf("HandleMessageNoLogin Unknow msg type : %d length : %u.\n", (int)header->msg_type, length);
			
#ifdef _DEBUG
				printf("NoLogin Unknown msg from ip[%s] netid[%d]\n", GetIPStr(ip), (int)netid.gateway_netid);
#endif
			}
		}
		break;

	case Protocol::MT_HEARTBEAT_CS:
		{
			Protocol::SCHeartbeatResp resp;
			EngineAdapter::Instance().NetSend(netid, &resp, sizeof(resp));
		}
		break;

	default:
		{
			gamelog::g_log_messagehandler.printf(LL_CRITICAL, "HandleMessageNoLogin Unknow msg type:%d.", (int)header->msg_type);
			// EngineAdapter::Instance().NetDisconnect(netid);
			printf("HandleMessageNoLogin Unknow msg type : %d length : %u.\n", (int)header->msg_type, length);
		
#ifdef _DEBUG
			printf("NoLogin Unknown msg from ip[%s] netid[%d]\n", GetIPStr(ip), (int)netid.gateway_netid);
#endif
		}
	}
}

void MessageHandler::OnReqEnter(const GSNetID &netid, IP userip, const char* msg)
{
	Protocol::CSUserEnterGSReq *ul = (Protocol::CSUserEnterGSReq*)msg;
	UserID user_id = IntToUid(ul->role_id);

	ul->key[sizeof(SessionKey) - 1] = 0;
	ul->role_name[sizeof(GameName) - 1] = 0;
	ul->pname[sizeof(PlatName) - 1] = 0;
	ul->plat_spid_str[sizeof(ul->plat_spid_str) - 1] = 0;

	if (m_scene_manager->GetOnlineRoleNum() >= m_scene_manager->GetMaxRoleNum()) // 在线人数超过允许上限
	{
		Protocol::SCServerBusy sb;
		EngineAdapter::Instance().NetSend(netid, (const char*)&sb, sizeof(sb));

		gamelog::g_log_login.printf(LL_INFO, "OnReqEnter ServerBusy user[%d,%s]", ul->role_id, ul->role_name);
		return;
	}

	if (!World::GetInstWorld()->IsAllDataLoadFinish()) // 所有数据加载完了才准update
	{
		Protocol::SCUserEnterGSAck login_ack;
		login_ack.result = Protocol::ENTERGS_SERVER_NOT_READY;
		login_ack.is_in_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
		EngineAdapter::Instance().NetSend(netid, (char *)&login_ack, sizeof(Protocol::SCUserEnterGSAck));
		
		gamelog::g_log_login.printf(LL_INFO, "OnReqEnter ServerNotReady user[%d,%s]", ul->role_id, ul->role_name);
		return;
	}

	PlatName pname;
	F_STRNCPY(pname, ul->pname, sizeof(PlatName));
	PlatNameCatPostfix(pname, ul->server);

	GameName role_name;
	F_STRNCPY(role_name, ul->role_name, sizeof(GameName));

	SessionKey key;
	GetSessionKey(InternalComm::Instance().m_session_key_base, ul->scene_id, ul->scene_key, ul->last_scene_id, ul->role_id, role_name,
		ul->time, ul->is_login, pname, key);
	key[sizeof(SessionKey) - 1] = 0;

	/*
	printf("%s  %d  %d  %d  %d  %s  %u  %d  %s  %s\n",
	InternalComm::Instance().m_session_key_base.c_str(),
	ul->scene_id,
	ul->scene_key,
	ul->last_scene_id,
	ul->role_id,
	role_name,
	ul->time,
	ul->is_login,
	pname,
	key);
	*/
	// md5校验
	if (memcmp(key, ul->key, sizeof(SessionKey)) != 0) // 验证失败 果断地把你断开
	{
		EngineAdapter::Instance().NetDisconnect(netid, user_id, role_name, ul->scene_id, "SessionKeyError");
		gamelog::g_log_login.printf(LL_INFO, "OnReqEnter  SessionKeyError  user[%d,%s]", ul->role_id, role_name);
		return;
	}


	Role *role = m_scene_manager->GetRole(user_id);
	if (NULL != role)
	{
//#ifdef _DEBUG
//		printf("MessageHandler::OnEnter role[%d,%s] exist\n", role->GetUID(), role->GetName());
//#endif
		{
			// 发给已经在的 让他下线
			Protocol::SCDisconnectNotice notice;
			notice.reason = DISCONNECT_NOTICE_TYPE_LOGIN_OTHER_PLACE;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&notice, sizeof(notice));
		}

		if (!role->IsForceKickDelay()) // 如果这个玩家没有被T过 
		{
//#ifdef _DEBUG
//			printf("MessageHandler::OnEnter role[%d,%s] set kick in 1 seconed\n", role->GetUID(), role->GetName());
//#endif

			role->SetForceKickDelay(); // 切换场景会导致这个值失效 导致没有办法踢玩家下线 但是踢人这一端可以不断重试

			ForceKickUserDelay *timer = new ForceKickUserDelay(role->GetUserId(), DISCONNECT_NOTICE_TYPE_LOGIN_OTHER_PLACE);  // 定时器启动 一定会T 不能相信客户端
			EngineAdapter::Instance().CreateTimerSecond(1, timer);

			// 出现玩家顶号，加入延迟登录器中等待
			if (!DelayLoginManager::GetInstance().IsUserDelayLoginInfoExist(user_id))
			{
				DelayLoginManager::GetInstance().AddDelayLoginRequest(user_id, netid, userip, ul);
			} 
			else
			{
//#ifdef _DEBUG
//				printf("MessageHandler::OnEnter role[%d,%s] delay info exist, reject login\n", role->GetUID(), role->GetName());
//#endif

				// 发给新上来的
				Protocol::SCUserEnterGSAck login_ack;
				login_ack.result = Protocol::ENTERGS_USER_EXIST;
				login_ack.is_in_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
				EngineAdapter::Instance().NetSend(netid, (char *)&login_ack, sizeof(Protocol::SCUserEnterGSAck));
			}
		}
		else
		{
			// 发给新上来的
			Protocol::SCUserEnterGSAck login_ack;
			login_ack.result = Protocol::ENTERGS_USER_EXIST;
			login_ack.is_in_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
			EngineAdapter::Instance().NetSend(netid, (char *)&login_ack, sizeof(Protocol::SCUserEnterGSAck));	
		}

		return;
	}
	else if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (CrossUserShadow::Instance().IsUserInCross(ul->role_id))
		{
			// 出现玩家角色仍在跨服的情况，加入延迟登录器，并通知跨服踢人
			if (!DelayLoginManager::GetInstance().IsUserDelayLoginInfoExist(user_id))
			{
				CrossUserShadow::Instance().OnUserAttemptLogin(ul->role_id);
				DelayLoginManager::GetInstance().AddDelayLoginRequest(user_id, netid, userip, ul);
			}

			return;
		}	
	}
	else
	{
		bool ok_to_enter = false;
		const CrossUserRegister::CrossUserInfo* info = CrossUserRegister::Instance().GetCrossUserInfo(user_id);
		if (NULL != info)
		{
			if (!info->can_enter_hidden)
			{
				if (!DelayLoginManager::GetInstance().IsUserDelayLoginInfoExist(user_id))
				{
					DelayLoginManager::GetInstance().AddDelayLoginRequest(user_id, netid, userip, ul);
				
					static crossgameprotocal::HiddenCrossUserEnteringHidden hcueh;
					hcueh.entering_new_uid = UidToInt(user_id);
					hcueh.origin_plat_type = info->original_plat_type;
					hcueh.origin_server_id = info->original_server_id;
					hcueh.origin_uid = UidToInt(info->original_user_id);
					InternalComm::Instance().SendToCross((const char*)&hcueh, sizeof(hcueh));
				}
			}
			else
			{
				ok_to_enter = true;
			}
		}
		
		if (!ok_to_enter)
		{
			return;
		}
	}

	if (!DBDisconnectRoleSaveCache::Instance().CanUserLogin(ul->role_id))
	{
		// 上次登出由于dataaccess断开连接导致无法保存好数据，
		// 缓存了数据但仍未成功保存，所以无法让其登录
		// 因为即使继续让其登录，都过不了下面的RMIRoleInit调用
		return;
	}

	int	plat_spid = globalconfig::SpidConfig::GetSpidByPlatName(ul->plat_spid_str);
	int use_key = ul->scene_key; // 这个scene_key, 在login里用于生成SessionKey，且在login里值为0， 所以收到的值也应为0
	if (1 == ul->is_login)
	{
		bool is_follow_team = false;
		do
		{
			Team* team = TeamManager::Instance().GetTeamByUid(ul->role_id);
			if (NULL == team) break;
			
			Role* leader = team->GetLeaderRole();
			if (NULL == leader) break;
				
			TeamMember* member = team->GetMemberInfoByUID(ul->role_id);
			if (NULL == member || member->IsRobot() || !member->is_follow_before_logout) break;
			Scene * leader_scene = leader->GetScene();
			if(NULL == leader_scene) break;
			if(member->level < leader_scene->GetScenenMinLevel() || (0 != leader_scene->GetScenenMaxLevel() && member->level > leader_scene->GetScenenMaxLevel())) break;

			if(!LOGIC_CONFIG->GetMapUnlockConfig()->IsCanEnterScene(leader_scene->GetSceneID(), member->level)) break;
					
			if (ul->scene_id == leader_scene->GetSceneID())
			{
				use_key = leader_scene->GetSceneKey();
				is_follow_team = true;
			}
			else
			{
				// 若与队长同在相同的场景类型，则允许进入队长的场景
				int role_scene_type = World::GetInstWorld()->GetSceneManager()->GetSceneTypeBySceneID(ul->scene_id);
				int leader_scene_type = leader->GetScene()->GetSceneType();
				if (role_scene_type == leader_scene_type)
				{
					ul->scene_id = leader_scene->GetSceneID();
					use_key = leader_scene->GetSceneKey();
					is_follow_team = true;
				}
			}	
		} while (0);
		
		if (is_follow_team)
		{
			//nothing to do
		}
		else if (m_scene_manager->CheckIsDelayDeleteScene(ul->scene_id))		// 目前仅仅允许单人副本 进行延迟删除 
		{
			use_key = ul->role_id;
		}
		else if (m_scene_manager->CheckIsCommonFBScene(ul->scene_id))
		{
			use_key = COMMON_FB_KEY;
		}
		else if (m_scene_manager->CheckIsChiefElectionSeaScene(ul->scene_id))
		{
			use_key = COMMON_FB_KEY;
		}

		int scene_id = 0;
		int scene_key = 0;
		if (m_scene_manager->GetChiefElectionScene(ul->role_id, ul->scene_id, &scene_id, &scene_key))
		{
			use_key = scene_key;
			ul->scene_key = scene_key;
			ul->scene_id = scene_id;
		}

		Team * m_team_register = TeamKeyRegister::Instance().GetTeamRegister(IntToUid(ul->role_id), &scene_id, &scene_key);//检查是否存在队伍记录
		if (m_team_register != NULL)
		{
			//有记录，进入队伍记录场景
			use_key = scene_key;
			ul->scene_key = scene_key;
			ul->scene_id = scene_id;
		}

		int guild_honor_scene_key = 0;
		if (GuildHonorBattleManager::Instance().GetSceneKey(ul->role_id, guild_honor_scene_key))
		{
			use_key = guild_honor_scene_key;
		}

		int world_arena_battle_scene_key = 0;
		int world_arena_battle_scene_id = 0;
		long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_spid, ul->role_id));
		if (WorldArena::Instance().TryReEnterBattleScene(uuid, &world_arena_battle_scene_id, &world_arena_battle_scene_key))
		{
			use_key = world_arena_battle_scene_key;
			ul->scene_id = world_arena_battle_scene_id;
			ul->scene_key = use_key;
		}

		int uta_scene_key = 0;
		int uta_scene_id = 0;
		if (UTAShadow::Instance().TryReEnterBattleScene(ul->role_id, &uta_scene_id, &uta_scene_key)
			&& ul->scene_id == uta_scene_id)
		{
			// 如果原本就在UTA场景里，则应重新拉入UTA场景
			use_key = uta_scene_key;
			ul->scene_id = uta_scene_id;
			ul->scene_key = use_key;
		}

		if (CrossConfig::Instance().IsHiddenServer())
		{
			//检查遗迹夺宝是否开启中，并且检查玩家是否在玩法中
			ActivityRelicLooting * relic_activity = (ActivityRelicLooting* )ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_RELIC_LOOTING);
			if (NULL != relic_activity && relic_activity->IsActivityOpen())
			{
				int relic_scene_key = relic_activity->IsUserPlay(UidToInt(user_id));
				if (relic_scene_key >= 0)
				{
					use_key = relic_scene_key;
				}
			}
		}
	}

	Scene *scene = m_scene_manager->GetSceneById(ul->scene_id, use_key);
	if (NULL != scene)
	{
		Protocol::SCUserEnterGSAck login_ack;
		login_ack.result = Protocol::ENTERGS_RESULT_SUC;
		login_ack.is_in_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
		EngineAdapter::Instance().NetSend(netid, (char *)&login_ack, sizeof(Protocol::SCUserEnterGSAck));

		bool is_login = (ul->is_login == 1);
		scene->RoleEnterSceneAsyn((0 != ul->is_micro_pc), netid, userip, user_id, pname, is_login, plat_spid);
	}
	else
	{
		if (ul->scene_key != 0)
		{
			Scene *fb_scene = NULL;
			bool ret = m_scene_manager->CreateFB(ul->scene_id, ul->scene_key, &fb_scene);
			if (ret)
			{
				Protocol::SCUserEnterGSAck login_ack;
				login_ack.result = Protocol::ENTERGS_RESULT_SUC;
				login_ack.is_in_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
				EngineAdapter::Instance().NetSend(netid, (char *)&login_ack, sizeof(Protocol::SCUserEnterGSAck));

				bool is_login = (ul->is_login == 1);
				fb_scene->RoleEnterSceneAsyn((0 != ul->is_micro_pc), netid, userip, user_id, pname, is_login, plat_spid);
				return;
			}
		}
		else
		{
			// scene_key < 0 则代表如果是副本但不存在的话不创建 那就到进入fb之前的地点去

			Scene *scene = m_scene_manager->GetSceneById(ul->last_scene_id, COMMON_SCENE_KEY);
			if (NULL == scene)
			{
				m_scene_manager->GetSceneById(ul->last_scene_id, COMMON_FB_KEY);
			}
			if (NULL != scene)
			{
				Protocol::SCUserEnterGSAck login_ack;
				login_ack.result = Protocol::ENTERGS_RESULT_SUC;
				login_ack.is_in_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
				EngineAdapter::Instance().NetSend(netid, (char *)&login_ack, sizeof(Protocol::SCUserEnterGSAck));

				bool is_login = (ul->is_login == 1);
				scene->RoleEnterSceneAsyn((0 != ul->is_micro_pc), netid, userip, user_id, pname, is_login, plat_spid);
				return;
			}
		}

		// 登陆错服务器了
		Protocol::SCUserEnterGSAck login_ack;
		login_ack.result = Protocol::ENTERGS_RESULT_SCENE_NO_EXIST;
		login_ack.is_in_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
		EngineAdapter::Instance().NetSend(netid, (char *)&login_ack, sizeof(Protocol::SCUserEnterGSAck));

		// 登陆错服务器了 通知后 是否要断掉连接
		// EngineAdapter::Instance().NetDisconnect(netid);
	}

}



