#include "servercommon/userprotocal/systemmsgcode.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/msgcode.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/userprotocal/globalmsgcode.h"
#include "servercommon/userprotocal/chatmsgcode.h"
#include "servercommon/userprotocal/msgbattle.h"

#include "msgrandactivity.hpp"
#include "msgcolosseum.h"
#include "msgscrank.h"
#include "msgcsrank.h"
#include "msgshop.h"
#include "msgescort.h"
#include "msgschooltask.h"
#include "msgmail.hpp"
#include "msgscmail.hpp"
#include "msgguild.hpp"
#include "msgserver.h"
#include "msgentergs.h"
#include "msgscene.h"
#include "msgrole.h"
#include "msgitem.h"
#include "msggm.hpp"
#include "msgprestige.h"
#include "msgtitle.h"
#include "msgteam.h"
#include "msgpet.h"
#include "msgpartner.h"
#include "msgtrain.h"
#include "msgequipment.h"
#include "msgtask.h"
#include "msgcollection.h"
#include "msgother.h"
#include "msgskill.h"
#include "msgactivity.hpp"
#include "msgworkshop.h"
#include "msgsurface.h"
#include "msgmarry.hpp"
#include "msgbaby.hpp"
#include "msgjewelry.h"
#include "msgfriend.h"
#include "msgshitu.h"
#include "msgcouragechallenge.h"
#include "msgnpc.h"
#include "msgmonsterwave.h"
#include "msgmiyi.h"
#include "msgofflineexp.h"
#include "msgresearchtask.h"
#include "msgworldboss.h"
#include "msgworldboss2.h"
#include "msgfabao.hpp"
#include "msgtreasuremap.h"
#include "msgtrademarket.h"
#include "msgmedal.h"
#include "msgcrystalfb.h"
#include "msgwildboss.h"
#include "msgmapunlock.h"
#include "msggatherfb.h"
#include "msgbraveground.h"
#include "msgchiefelection.h"
#include "msgchiefpeakbattle.h"
#include "msgcourseofgrowth.h"
#include "msgguildhonorbattle.h"
#include "msgplatformbattle.h"
#include "msgchallengefb.h"
#include "msgfacescore.hpp"
#include "msgfuncguide.h"
#include "msgguidefb.h"
#include "msgbosschapter.h"
#include "msgfirstkill.h"
#include "msgmatch.h"
#include "msghuanjiezhanchang.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "msgshanhaiboss.hpp"
#include "msgpoker.hpp"
#include "msgduchuangzeiying.hpp"
#include "msgdraw.h"
#include "msgdreamnoteschallenge.hpp"
#include "msgsmartmountssystem.hpp"
#include "msgpetmonopoly.h"
#include "msgcloudarena.h"
#include "msgmaterial.hpp"
#include "msgmoveassist.h"
#include "msgmarket.hpp"
#include "msgheartskill.hpp"
#include "msgzhuling.h"
#include "msgformation.h"
#include "msgpetgodfb.hpp"
#include "msgdujie.hpp"
#include "msghongbao.hpp"
#include "msghuanshou.hpp"
#include "msghundredghost.h"
#include "msglifeskill.hpp"
#include "msgfishground.hpp"
#include "msgworldarena.hpp"
#include "msgworldboss3.hpp"
#include "msgkuafuyingxionglu.hpp"
#include "global/zhouqimanager/msgzhouqi.h"

#include "protocol/randactivity/msgraadventuregift.hpp"
#include "protocol/randactivity/msgraanimalrace.hpp"
#include "protocol/randactivity/msgracollectword.hpp"
#include "protocol/randactivity/msgraconsumecarousel.hpp"
#include "protocol/randactivity/msgradiscountcarnival.hpp"
#include "protocol/randactivity/msgradiscountstore.hpp"
#include "protocol/randactivity/msgraelementcards.hpp"
#include "protocol/randactivity/msgraexpelmonster.hpp"
#include "protocol/randactivity/msgragodtoken.hpp"
#include "protocol/randactivity/msgrahappyup.hpp"
#include "protocol/randactivity/msgraheritagetreasure.hpp"
#include "protocol/randactivity/msgrajiandanqinxin.hpp"
#include "protocol/randactivity/msgralinkagegift.hpp"
#include "protocol/randactivity/msgraluckybag.hpp"
#include "protocol/randactivity/msgraluckydiscount.hpp"
#include "protocol/randactivity/msgraluckyturntable.hpp"
#include "protocol/randactivity/msgrapartnerexchange.hpp"
#include "protocol/randactivity/msgrapetexchange.hpp"
#include "protocol/randactivity/msgraquanmindiscount.hpp"
#include "protocol/randactivity/msgrareducetaxrate.hpp"
#include "protocol/randactivity/msgrasevencolorpicture.hpp"
#include "protocol/randactivity/msgrasignincontinuity.hpp"
#include "protocol/randactivity/msgrasports.hpp"
#include "protocol/randactivity/msgratianjianghaoli.hpp"
#include "protocol/randactivity/msgratianmingdivination.hpp"
#include "protocol/randactivity/msgratimelimitstore.hpp"
#include "protocol/randactivity/msgratruntable.hpp"
#include "protocol/randactivity/msgratujiantreasure.hpp"
#include "protocol/randactivity/msgsaleshop.hpp"
#include "protocol/randactivity/msgratruntable.hpp"
#include "protocol/randactivity/msgrajoyseeksfavors.hpp"
#include "protocol/randactivity/msgracaishenjubao.hpp"
#include "protocol/randactivity/msgradingzhibaoxiang.hpp"
#include "protocol/randactivity/msgradingzhibaoxiang2.hpp"
#include "protocol/randactivity/msgradingzhibaoxiang3.hpp"
#include "protocol/randactivity/msgrashencihuafu.hpp"
#include "protocol/randactivity/msgrazhuiyueshangdian.hpp"
#include "protocol/randactivity/msgrajinyitianxiang.hpp"
#include "protocol/randactivity/msgzhuanshulibao.hpp"
#include "protocol/randactivity/msgrajingyanbiaosheng.hpp"
#include "protocol/randactivity/msgraxianshibaohe.hpp"
#include "protocol/randactivity/msgraxianshibaohe2.hpp"
#include "protocol/randactivity/msgraxianshibaohe3.hpp"
#include "protocol/randactivity/msgraxianshibaohe4.hpp"
#include "protocol/randactivity/msgraxianshibaodai.hpp"
#include "protocol/randactivity/msgraxianshibaodai2.hpp"
#include "protocol/randactivity/msgragivepetdraw.hpp"
#include "protocol/randactivity/msgratrademarketreturngold.hpp"
#include "protocol/randactivity/msgrashenshoujianglin.hpp"
#include "protocol/msglevelcomplement.h"
#include "protocol/msgschoolexercise.h"
#include "protocol/msgworldstatus.h"
#include "protocol/randactivity/msggoodgiftturn.hpp"
#include "protocol/randactivity/msgrahelpothers.hpp"
#include "protocol/randactivity/msgrahelpothers2.hpp"
#include "protocol/randactivity/msgrasecrettreasure.hpp"
#include "protocol/randactivity/msgchunridengfeng.hpp"
#include "protocol/randactivity/msgzuheheli.hpp"
#include "protocol/randactivity/msglingpaitianci.hpp"
#include "protocol/randactivity/msgfashionshop.hpp"
#include "protocol/randactivity/msgoneyuangou.hpp"
#include "protocol/randactivity/msglingyujijin.hpp"
#include "protocol/randactivity/msgrapaimaihang.hpp"
#include "protocol/msgrolejoinactivityreward.hpp"
#include "protocol/msgtiandidaohen.h"
#include "protocol/msggift.h"
#include "protocol/randactivity/msgraburningsummer.hpp"
#include "protocol/randactivity/msgracoolsummer.hpp"
#include "protocol/randactivity/msgranationalday.hpp"
#include "protocol/randactivity/msgrazhenpinhangpu.hpp"
#include "protocol/msgcowreportgoodnews.hpp"
#include "protocol/randactivity/msgraservercompetition.hpp"
#include "protocol/randactivity/msgrayunzeterritory.hpp"
#include "protocol/randactivity/msgrayunzebloomingrecord.hpp"
#include "protocol/randactivity/msgrayunzeslotmachine.hpp"
#include "protocol/randactivity/msgrajingjiyueka.hpp"
#include "protocol/msgduel.h"
#include "protocol/msgaccessories.h"
#include "protocol/msgtianming.h"
#include "protocol/msgyinyangeye.h"
#include "protocol/randactivity/msgshanhaiquestion.hpp"
#include "protocol/randactivity/msgrashanhaimeet.h"
#include "protocol/randactivity/msgrafateentanglement.hpp"
#include "protocol/randactivity/msgrayunzeersanshi.hpp"
#include "protocol/msgjieyi.h"
#include "protocol/msgqingyuan.h"
#include "protocol/msgadvancecareer.h"
#include "protocol/msgrashanhaibaoxiang.hpp"
#include "protocol/msgrashanhaibaohe.hpp"
#include "protocol/msgrashanhaibaodai.hpp"
#include "protocol/msgwanlingfb.h"
#include "protocol/randactivity/msgrayunzewish.hpp"
#include "protocol/randactivity/msgrayunzehaoli.hpp"
#include "protocol/randactivity/msgrayunzecutepet.hpp"
#include "protocol/randactivity/msgrayunzecatchelves.hpp"
#include "protocol/randactivity/msgraxianshileichong.hpp"
#include "protocol/msgxiuwei.hpp"
#include "protocol/randactivity/msgratigerauspicioussnow.hpp"
#include "protocol/randactivity/msgratigerfisheveryyear.hpp"
#include "protocol/randactivity/msgratigerfindspring.hpp"
#include "protocol/randactivity/msgratigerteasenewyear.hpp"
#include "protocol/randactivity/msgratigerluckymoney.hpp"
#include "protocol/msgtigerbringsblessing.hpp"
#include "protocol/msgqingyuan.h"
#include "protocol/randactivity/msgraronglianyouli.hpp"
#include "protocol/msgminigame.hpp"
#include "protocol/msgworldteamarena.hpp"
#include "protocol/msgpeaktoken.hpp"
#include "protocol/msgincome.hpp"
#include "protocol/msgqingyuanshenghui.h"
#include "protocol/cycleactivity/msgwhereisthefairy.hpp"
#include "protocol/cycleactivity/msgqingyuanduiduipeng.hpp"
#include "protocol/cycleactivity/msglanternriddie.hpp"
#include "protocol/cycleactivity/msgtiangongxunbao.hpp"
#include "protocol/cycleactivity/msgtuzikuaipao.hpp"
#include "protocol/cycleactivity/msggotoschool.hpp"
#include "protocol/cycleactivity/msgconcentricknot.hpp"
#include "servercommon/userprotocal/msgutachampionrecord.h"
#include "protocol/randactivity/msgragodbeastadvent.hpp"
#include "protocol/randactivity/msgrayuanqispeed.hpp"
#include "protocol/randactivity/msgrayuanqihelp.hpp"
#include "protocol/randactivity/msgrayuanqiyanhuo.hpp"
#include "protocol/randactivity/msgraduanyangxiangnang.hpp"
#include "protocol/randactivity/msgraduanyangzixuan.hpp"
#include "protocol/randactivity/msgraduanyangfuli.hpp"
#include "protocol/randactivity/msgrajinlongjubao.hpp"
#include "protocol/randactivity/msgrashenshouenze.hpp"
#include "protocol/randactivity/msgraqixitehui.hpp"
#include "protocol/msghongmengtianjie.hpp"
#include "protocol/msgzaohuaequipment.h"
#include "protocol/msgholyequip.h"
#include "protocol/cycleactivity/msgkejuexam.hpp"
#include "protocol/cycleactivity/msgzhouqishop.hpp"
#include "protocol/randactivity/msgrachongjifanli.hpp"
#include "protocol/randactivity/msgraxinfuqianghua.hpp"
#include "protocol/randactivity/msgraxinfulingchong.hpp"
#include "protocol/msgfuyu.h"
#include "protocol/randactivity/msgrahunqifanli.hpp"
#include "protocol/msgstarchart.h"
#include "protocol/msgnewcouragechallenge.h"
#include "protocol/msgfashioncollection.h"
#include "protocol/randactivity/msgrayuexidengmi.hpp"
#include "protocol/randactivity/msgradanbifanli.hpp"
#include "protocol/msgroleguildtask.h"

namespace Protocol
{
	CSTimeReq::CSTimeReq() :header(MT_TIME_REQ_CS) {}
	SCTimeAck::SCTimeAck() : header(MT_TIME_ACK_SC) {}
	SCDisconnectNotice::SCDisconnectNotice() : header(MT_DISCONNECT_NOTICE_SC) {}
	SCTimeDateInfo::SCTimeDateInfo() : header(MT_TIME_DATE_SC) {}
	SCCrossConnectInfo::SCCrossConnectInfo() : header(MT_CONNECT_CROSS_INFO_SC) {}

	CSHeartbeatReq::CSHeartbeatReq() : header(MT_HEARTBEAT_CS) {}
	SCHeartbeatResp::SCHeartbeatResp() : header(MT_HEARTBEAT_SC) {}
	

	//SCSystemMsg::SCSystemMsg() : header(MT_SYSTEM_MSG_SC) {}
	SCZeroHour::SCZeroHour() : header(MT_SYSTEM_ZERO_HOUR_SC) {}
	SCClientHeartbeatCheckModify::SCClientHeartbeatCheckModify() : header(MT_SYSTEM_CHANGE_CLIENT_HEARTBEAT_CHECK) {}

	SCForbidReason::SCForbidReason() : header(MT_SYSTEM_FORBID_REASON_SC) {}

	CSUserEnterGSReq::CSUserEnterGSReq() : header(MT_ENTER_GS_REQ_CS) {}
	SCUserEnterGSAck::SCUserEnterGSAck() : header(MT_ENTER_GS_ACK_SC) {}
	CSUserLogout::CSUserLogout() : header(MT_LOGOUT_CS) {}
	SCChangeGS::SCChangeGS() : header(MT_CHANGE_GS_SC) {}
	SCServerBusy::SCServerBusy() : header(MT_SERVER_BUSY_SC) {}
	CSTransportReq::CSTransportReq() : header(MT_TRANSPORT_REQ) {}
	CSSpecialTransportReq::CSSpecialTransportReq() : header(MT_SPECIAL_TRANSPORT_REQ_CS) {}
	CSTreasureChestReq::CSTreasureChestReq() : header(MT_TREASURE_CHEST_REQ_CS) {}
	SCSpecialAppearanceChangeNotice::SCSpecialAppearanceChangeNotice() : header(MT_SPECIAL_APPEARANCE_CHANGE_NOTICE_SC) {}

	SCEnterScene::SCEnterScene() : header(MT_ENTER_SCENE_SC) {}
	SCLeaveScene::SCLeaveScene() : header(MT_LEAVE_SCENE_SC) {}
	SCVisibleObjEnterRole::SCVisibleObjEnterRole() : header(MT_VISIBLE_ENTER_ROLE_SC) {}
	SCVisibleObjLeave::SCVisibleObjLeave() : header(MT_VISIBLE_LEAVE_SC) {}
	SCChangeSceneNotice::SCChangeSceneNotice() : header(MT_CHANGE_SCENE_NOTICE_SC) {}
	SCVisibleObjEnterNPCMonster::SCVisibleObjEnterNPCMonster() : header(MT_VISIBLE_ENTER_NPC_MONSTER_SC) {}
	SCVisibleObjEnterNPCGathers::SCVisibleObjEnterNPCGathers() : header(MT_VISIBLE_ENTER_NPC_GATHERS_SC) {}
	SCChangeAppearanceRole::SCChangeAppearanceRole() : header(MT_APPEARACNE_CHANGE_SC) {}
	SCVisibleObjEnterSpecialTransport::SCVisibleObjEnterSpecialTransport() : header(MT_VISIBLE_ENTER_SPECIAL_TRANSPORT_SC) {}
	SCVisibleObjEnterTreasureChest::SCVisibleObjEnterTreasureChest() : header(MT_VISIBLE_ENTER_TREASURE_CHEST_SC) {}
	SCVisibleObjEnterNPCFuncNpc::SCVisibleObjEnterNPCFuncNpc() : header(MT_VISIBLE_ENTER_NPC_FUNCNPC_SC) {}
	SCSceneObjList::SCSceneObjList() : header(MT_SCENE_OBJ_LIST_SC) {}
	SCUpdateFollowList::SCUpdateFollowList() : header(MT_UPDATE_FOLLOW_LIST_SC) {}
	SCAddFollow::SCAddFollow() : header(MT_ADD_FOLLOW_SC) {}
	SCRemoveFollow::SCRemoveFollow() : header(MT_REMOVE_FOLLOW_SC) {}
	SCJoinTeamNotifyArea::SCJoinTeamNotifyArea() : header(MT_JOIN_TEAM_NOTIFY_AREA_SC) {}
	SCLeaveTeamNotifyArea::SCLeaveTeamNotifyArea() : header(MT_LEAVE_TEAM_NOTIFY_AREA_SC) {}

	SCVisibleObjEnterXunYouCar::SCVisibleObjEnterXunYouCar() : header(MT_VISIBLE_ENTER_XUN_YOU_CAR_SC) {}
	SCVisibleObjEnterPeakTokenObj::SCVisibleObjEnterPeakTokenObj() : header(MT_VISIBLE_ENTER_PEAK_TOKEN_OBJ_SC) {}
	SCVisibleObjEnterTuZiKuaiPaoTuzi::SCVisibleObjEnterTuZiKuaiPaoTuzi() : header(MT_VISIBLE_ENTER_TU_ZI_KUAI_PAO_TU_ZI_OBJ_SC) {}

	CSAuditBehaviorSubmitReq::CSAuditBehaviorSubmitReq() : header(MT_CLIENT_BEHAVIOR_REQ_CS) {}

	SCYaoXingShiLianInfo::SCYaoXingShiLianInfo() : header(MT_YAO_XING_SHI_LIAN_INFO_SC) {}
	CSYaoXingShiLianReq::CSYaoXingShiLianReq() : header(MT_YAO_XING_SHI_LIAN_REQ_CS) {}

	CSSanXianGuiDongReq::CSSanXianGuiDongReq() : header(MT_SAN_XIAN_GUI_DONG_REQ_CS) {}
	SCSanXianGuiDongInfo::SCSanXianGuiDongInfo() : header(MT_SAN_XIAN_GUI_DONG_INFO_SC) {}
	SCSanXianGuiDongPlayEndInfo::SCSanXianGuiDongPlayEndInfo() : header(MT_SAN_XIAN_GUI_DONG_PLAY_INFO_SC) {}

	CSTaoQuanNaJiReq::CSTaoQuanNaJiReq() : header(MT_TAO_QUAN_NA_JI_REQ_CS) {}
	SCTaoQuanNaJiInfo::SCTaoQuanNaJiInfo() : header(MT_TAO_QUAN_NA_JI_INFO_SC) {}
	SCTaoQuanNaJiPlayEndInfo::SCTaoQuanNaJiPlayEndInfo() : header(MT_TAO_QUAN_NA_JI_PLAY_END_INFO_SC) {}

	CSHundredGodBossReq::CSHundredGodBossReq() : header(MT_HUNDRED_GOD_BOSS_REQ_CS) {}
	SCHundredGodBossInfo::SCHundredGodBossInfo() : header(MT_HUNDRED_GOD_BOSS_INFO_SC) {}

	SCWildBossOtherInfo::SCWildBossOtherInfo() : header(MT_WILD_BOSS_OTHER_INFO_SC) {}

	SCCharismaitcTanabataInfo::SCCharismaitcTanabataInfo() : header(MT_CHARISMATIC_TANABATA_INFO_SC) {}

	CSEscapeFromCurrentPosition::CSEscapeFromCurrentPosition() : header(MT_ESCAPE_FROM_CURRENT_POSITION) {}
	CSAutoEscapeFromCurrentPosition::CSAutoEscapeFromCurrentPosition() : header(MT_AUTO_ESCAPE_FROM_CURRENT_POSITION) {}

	SCRoleSpeedChangeNotice::SCRoleSpeedChangeNotice() : header(MT_ROLE_SPEED_CHANGE_NOTICE_SC) {}
	SCRoleStatusChangeNotice::SCRoleStatusChangeNotice() : header(MT_ROLE_STATUS_CHANGE_NOTICE_SC) {}
	SCRoleAutoTalkInfo::SCRoleAutoTalkInfo() : header(MT_ROLE_AUTO_TALK_INFO_SC) {}
	SCRoleAuthorityInfoChange::SCRoleAuthorityInfoChange() : header(MT_ROLE_AUTHORITY_INFO_CHANGE_SC) {}
	SCRoleHistoryMaxCapabilityChange::SCRoleHistoryMaxCapabilityChange() : header(MT_ROLE_HISTORY_MAX_CAPABILITY_CHANGE_SC) {}
	SCRoleInfoAck::SCRoleInfoAck() : header(MT_ROLE_INFO_ACK_SC) {}
	CSAllInfoReq::CSAllInfoReq() : header(MT_ALL_INFO_REQ_CS) {}
	SCMoney::SCMoney() : header(MT_ROLE_MONEY_INFO_SC) {}
	SCRoleAttrList::SCRoleAttrList() : header(MT_ROLE_ATTRIBUTE_LIST_SC) {}
	SCRoleAttributePlanInfo::SCRoleAttributePlanInfo() : header(MT_ROLE_ATTRIBUTE_PLAN_INFO_SC) {}
	CSAttributePlanAddPoint::CSAttributePlanAddPoint() : header(MT_ATTRIBUTE_PLAN_ADD_POINT_CS) {}
	CSAttributePlanClearPoint::CSAttributePlanClearPoint() : header(MT_ATTRIBUTE_PLAN_CLEAR_POINT_CS) {}
	CSAttributePlanCreate::CSAttributePlanCreate() : header(MT_ATTRIBUTE_PLAN_CREATE_CS) {}
	CSAttributePlanSwitchPlan::CSAttributePlanSwitchPlan() : header(MT_ATTRIBUTE_PLAN_SWITCH_PLAN_CS) {}
	CSAttributePlanChangeName::CSAttributePlanChangeName() : header(MT_ATTRIBUTE_PLAN_CHANGE_NAME_CS) {}
	SCAttrListReason::SCAttrListReason() : header(MT_ATTR_LIST_REASON_SC) {}
	CSAttrListReasonReq::CSAttrListReasonReq() : header(MT_SIGNLE_ATTR_REASON_REQ_CS) {}
	CSRoleInfoNoticeReq::CSRoleInfoNoticeReq() : header(MT_ROLE_INFO_NOTICE_REQ_CS) {}
	SCRoleInfoNotice::SCRoleInfoNotice() : header(MT_ROLE_INFO_NOTICE_SC) {}

	//巅峰等级
	CSRoleTopLevelReq::CSRoleTopLevelReq() : header(MT_ROLE_TOP_LEVEL_REQ_CS) {}
	SCRoleTopLevelAllInfo::SCRoleTopLevelAllInfo() : header(MT_ROLE_TOP_LEVEL_ALL_INFO_SC) {}
	SCRoleTopLevelNoticce::SCRoleTopLevelNoticce() : header(MT_ROLE_TOP_LEVEL_NOTICE_SC) {}
	SCRoleTopLevelAOINotice::SCRoleTopLevelAOINotice() : header(MT_ROLE_TOP_LEVEL_AOI_NOTICE_SC) {}

	SCRoleExpChange::SCRoleExpChange() : header(MT_ROLE_EXP_CHANGE_NOTICE_SC) {}
	SCRoleLevelChange::SCRoleLevelChange() : header(MT_ROLE_LEVEL_CHANGE_NOTICE_SC) {}
	SCRoleProfessionChangeNotice::SCRoleProfessionChangeNotice() : header(MT_ROLE_PROFESSION_CHANGE_NOTICE_SC) {}
	CSRoleCareerAdvanceReq::CSRoleCareerAdvanceReq() : header(MT_ROLE_CAREER_ADVANCE_REQ_CS) {}
	CSRoleCareerChangeReq::CSRoleCareerChangeReq() : header(MT_ROLE_CARRER_CHANGE_REQ_CS) {}
	CSProfessionExtraInfoReq::CSProfessionExtraInfoReq() : header(MT_ROLE_PROFESSION_EXTRA_INFO_REQ_CS) {}
	SCProfessionExtraInfo::SCProfessionExtraInfo() : header(MT_ROLE_PROFESSION_EXTRA_INFO_SC) {}
	CSRoleChangeColorReq::CSRoleChangeColorReq() : header(MT_ROLE_CHANGE_COLOR_REQ_CS) {}

	CSRoleSystemSetReq::CSRoleSystemSetReq() : header(MT_ROLE_SYSTEM_SET_REQ_CS) {}
	SCRoleSystemSetInfo::SCRoleSystemSetInfo() : header(MT_ROLE_SYSTEM_SET_INFO_SC) {}
	CSRoleSystemSetChatGroupReq::CSRoleSystemSetChatGroupReq() : header(MT_ROLE_SYSTEM_SET_CHAT_GROUP_CS) {}
	SCRoleSystemSetChatGroupInfo::SCRoleSystemSetChatGroupInfo() : header(MT_ROLE_SYSTEM_SET_CHAT_GROUP_INFO_SC) {}

	SCObjMove::SCObjMove() : header(MT_OBJ_MOVE_SC) {}
	CSObjMove::CSObjMove() : header(MT_OBJ_MOVE_CS) {}

	SCResetPost::SCResetPost() : header(MT_RESET_POST_SC) {}
	CSFlyByShoe::CSFlyByShoe() : header(MT_FLY_BY_SHOE_CS) {}

	CSRolePatrol::CSRolePatrol() : header(MT_PATROL_CS) {}
	SCRoleSceneEffect::SCRoleSceneEffect() : header(MT_ROLE_BUFF_EFFECT_NOTIFY_AREA_SC) {}
	CSRoleTaskPathFinding::CSRoleTaskPathFinding() : header(MT_TASK_PATH_FINDING_CS) {}
	CSRoleTalkToNpc::CSRoleTalkToNpc() : header(MT_ROLE_TALK_TO_NPC_CS) {}

	CSSetEncounterMineMonsterFlag::CSSetEncounterMineMonsterFlag() : header(MT_SET_ENCOUNTER_MINE_MONSTER_FLAG_CS) {}
	SCEncounterMineMonsterFlag::SCEncounterMineMonsterFlag() : header(MT_ENCOUNTER_MINE_MONSTER_FLAG_SC) {}

	CSSetBattlePosition::CSSetBattlePosition() : header(MT_SET_BATTLE_POSITION_CS) {}
	SCSetBattlePosition::SCSetBattlePosition() : header(MT_SET_BATTLE_POSITION_SC) {}

	// knapsack - 背包
	SCKnapsackInfoAck::SCKnapsackInfoAck() : header(MT_KNAPSACK_INFO_ACK_SC) {}
	SCKnapsackItemChange::SCKnapsackItemChange() : header(MT_KNAPSACK_GRID_CHANGE_SC) {}
	SCUseItemSuc::SCUseItemSuc() : header(MT_USE_ITEM_SUC_SC) {}
	SCKnapsackExtend::SCKnapsackExtend() : header(MT_KNAPSACK_EXTEND_SC) {}
	SCLackItem::SCLackItem() : header(MT_LACK_ITEM_SC) {}
	SCItemIntoKnpackSpecialProtocal::SCItemIntoKnpackSpecialProtocal() : header(MT_ITEM_INTO_KNAPSACK_SPECIAL) {}
	SCRAResultNotice::SCRAResultNotice() : header(MT_RAND_ACTIVITY_RESUILT_NOTICE_SC) {}
	CSKnapsackOperaReq::CSKnapsackOperaReq() : header(MT_KNAPSACK_OPERA_REQ_CS) {}
	CSUseItem::CSUseItem() : header(MT_USE_ITEM_CS) {}
	CSUseOptionalGift::CSUseOptionalGift() : header(MT_USE_OPTIONAL_GIFT_CS) {}
	SCTaskItemCommitNotice::SCTaskItemCommitNotice() : header(MT_COMMIT_TASK_ITEM_NOTICE_SC) {}
	SCTaskItemLackForCommitNotice::SCTaskItemLackForCommitNotice() : header(MT_LACK_ITEM_NOTICE_FOR_TASK_COMMIT_SC) {}
	CSUseOptionalGift2::CSUseOptionalGift2() : header(MT_USE_OPTIONAL_GIFT_2_CS) {}
	CSUseGoldRecovery::CSUseGoldRecovery() : header(MT_USE_GOLD_RECOVERY_CS) {}
	SCUseRecoveryNotice::SCUseRecoveryNotice() : header(MT_USE_RECOVERY_NOTICE_SC) {}
	SCKnapsackGetItemNotice::SCKnapsackGetItemNotice() : header(MT_KNAPSACK_ITEM_NOTICE_SC) {}
	CSUseSpecialLoigcItem::CSUseSpecialLoigcItem() : header(MT_USE_SPECIAL_LOGIC_ITEM_CS) {}
	CSActiveSpecialLogicItem::CSActiveSpecialLogicItem() : header(MT_ACTIVE_SPECIAL_LOGIC_ITEM_CS) {}
	CSHearsayInfoReq::CSHearsayInfoReq() : header(MT_HEARSAY_INFO_CS) {}
	CSBuyCmdGiftReq::CSBuyCmdGiftReq() : header(MT_BUY_CMD_GIFT_REQ_CS) {}
	SCHongBaoUseRet::SCHongBaoUseRet() : header(MT_HONG_BAO_USE_RET_SC) {}
	

	// pet - 宠物
	CSPetOp::CSPetOp() : header(MT_PET_OP_CS) {}
	CSPetRename::CSPetRename() : header(MT_PET_RENAME_CS) {}
	CSPetAssignAttributePoints::CSPetAssignAttributePoints() : header(MT_PET_ASSIGN_ATTRIBUTE_POINTS_CS) {}
	SCSendPetAllInfo::SCSendPetAllInfo() : header(MT_SEND_PET_ALL_INFO_SC) {}
	SCSendPetSingleInfo::SCSendPetSingleInfo() : header(MT_SEND_PET_SINGLE_INFO_SC) {}
	SCSendPetOtherInfo::SCSendPetOtherInfo() : header(MT_SEND_PET_OTHER_INFO_SC) {}
	SCSendPetDeleteInfo::SCSendPetDeleteInfo() : header(MT_SEND_PET_DELETE_INFO_SC) {}
	SCSendPetAttrListInfo::SCSendPetAttrListInfo() : header(MT_SEND_PET_ATTRS_INFO_SC) {}
	SCSendPetBreakthroughsRet::SCSendPetBreakthroughsRet() : header(MT_SEND_PET_BREAKTHROUGHS_RET_SC) {}
	SCSendPetGetRet::SCSendPetGetRet() : header(MT_SEND_PET_GET_NOTICE_SC) {}
	SCPetFollowStatus::SCPetFollowStatus() : header(MT_PET_FOLLOW_PET_INFO_SC) {}
	SCSealPetGetItemInfo::SCSealPetGetItemInfo() : header(MT_SEAL_PET_GET_ITEM_INFO_SC) {}
	SCPetIdRecordList::SCPetIdRecordList() : header(MT_PET_ID_RECORD_LIST_SC) {}
	CSAbandonPetByList::CSAbandonPetByList() : header(MT_ABANDON_PET_BY_LIST_CS) {}
	SCMovePetGridNotice::SCMovePetGridNotice() : header(MT_MOVE_PET_GRID_NOTICE_SC) {}
	SCPetSkinInfo::SCPetSkinInfo() : header(MT_PET_SKIN_INFO_SC) {}
	SCPetCalAttrRet::SCPetCalAttrRet() : header(MT_PET_CAL_ATTR_RET_SC) {}
	CSPetSoulUplevelReq::CSPetSoulUplevelReq() : header(MT_PET_SOUL_UPLEVEL_REQ_CS) {}
	SCPetBenedictionAllInfo::SCPetBenedictionAllInfo() : header(MT_PET_BENEDICTION_ALL_INFO_SC) {}
	SCPetBenedictionSingleInfo::SCPetBenedictionSingleInfo() : header(MT_PET_BENEDICTION_SINGLE_INFO_SC) {}
	SCPetFeiShengTaskInfo::SCPetFeiShengTaskInfo() : header(MT_PET_FEI_SHENG_TASK_INFO_SC) {}
	SCPetFeiShengRewardInfo::SCPetFeiShengRewardInfo() : header(MT_PET_FEI_SHENG_REWARD_INFO_SC) {}
	CSPetComposeBatch::CSPetComposeBatch() : header(MT_PET_BATCH_COMPOSE_CS) {}
	SCPetComposeBatchRet::SCPetComposeBatchRet() : header(MT_PET_BATCH_COMPOSE_RET_SC) {}
	CSPetComposeAndDecomposeBatch::CSPetComposeAndDecomposeBatch() : header(MT_PET_BATCH_COMPOSE_AND_DECOMPOSE_CS ) {}
	CSPetBadgeDecompose::CSPetBadgeDecompose() : header(MT_PET_DECOMPOSE_DECOMPOSE_CS) {}
	SCPetStrengthenBuffInfo::SCPetStrengthenBuffInfo() : header(MT_PET_STRENGTHEN_BUFF_INFO) {}
	SCPetLianYaoCommonInfo::SCPetLianYaoCommonInfo() : header(MT_PET_LIAN_YAO_COMMON_INFO_SC) {}
	SCPetAddExpNotice::SCPetAddExpNotice() : header(MT_PET_ADD_EXP_NOTICE_SC) {}
	


	//情缘盛会
	CSQingYuanShengHuiReq::CSQingYuanShengHuiReq() : header(MT_QING_YUAN_SHENG_HUI_REQ_CS) {}
	SCQingYuanShengHuiSceneInfo::SCQingYuanShengHuiSceneInfo() : header(MT_QING_YUAN_SHENG_SCENE_INFO_SC) {}
	SCQingYuanShengHuiRoleInfo::SCQingYuanShengHuiRoleInfo() : header(MT_QING_YUAN_SHENG_ROLE_INFO_SC) {}
	SCQingYuanShengHuiQuestionResult::SCQingYuanShengHuiQuestionResult() : header(MT_QING_YUAN_SHENG_HUI_QUESTION_RESULT_SC) {}
	SCQingYuanShengHuiQuestionRoute::SCQingYuanShengHuiQuestionRoute() : header(MT_QING_YUAN_SHENG_HUI_QUESTION_ROUTE_SC) {}
	SCQingYuanShengHuiQuestionInfo::SCQingYuanShengHuiQuestionInfo() : header(MT_QING_YUAN_SHENG_HUI_QUESTION_INFO_SC) {}
	SCQingYuanShengHuiOpponentInfo::SCQingYuanShengHuiOpponentInfo() : header(MT_QING_YUAN_SHENG_HUI_OPPONENT_INFO_SC) {}
	SCQingYuanShengHuiMatchOpRoute::SCQingYuanShengHuiMatchOpRoute() : header(MT_QING_YUAN_SHENG_HUI_MATCH_OP_ROUTE_SC) {}
	SCQingYuanShengHuiInfo::SCQingYuanShengHuiInfo() : header(MT_QING_YUAN_SHENG_HUI_INFO_SC) {}
	SCQingYuanShengHuiQiYuanNotice::SCQingYuanShengHuiQiYuanNotice() : header(MT_QING_YUAN_SHENG_HUI_QI_YUAN_NOTICE_SC) {}
	
	CSQingYuanShengHuiReq2::CSQingYuanShengHuiReq2() : header(MT_QING_YUAN_SHENG_HUI_REQ_2_CS) {}

	SCPetGodPrintBagSingleInfo::SCPetGodPrintBagSingleInfo() : header(MT_PET_GOD_PRINT_SIGLE_SC) {}
	SCPetGodPrintBagInfo::SCPetGodPrintBagInfo() : header(MT_PET_GOD_PRINT_BAG_INFO_SC) {}
	SCPetGodPrintEXPInfo::SCPetGodPrintEXPInfo() : header(MT_PET_GOD_PRINT_EXP_INFO_SC) {}
	SCPetGodPrintWearSchemeSingleInfo::SCPetGodPrintWearSchemeSingleInfo() : header(MT_PET_GOD_PRINT_WEAR_SIGLE_INFO_SC) {}
	CSPetGodPrintSetearSchemeReq::CSPetGodPrintSetearSchemeReq() : header(MT_PET_GOD_PRINT_SET_WEAR_SIGLE_REQ_CS) {}
	SCPetGodPrintWishLogListInfo::SCPetGodPrintWishLogListInfo() : header(MT_PET_GOD_PRINT_LOG_LIST_SC) {}
	SCPetGodPrintWishLogDetailInfo::SCPetGodPrintWishLogDetailInfo() : header(MT_PET_GOD_PRINT_LOG_DETAIL_INFO_SC) {}
	SCPetGodPrintWishRewardInfo::SCPetGodPrintWishRewardInfo() : header(MT_PET_GOD_PRINT_WISH_REWARD_SC) {}
	CSPetGodPrintUplevelReq::CSPetGodPrintUplevelReq() : header(MT_PET_GOD_PRINT_USE_ITEM_UPLEVEL_CS) {}
	// partner - 伙伴
	CSPartnerHandleRequest::CSPartnerHandleRequest() : header(MT_PARTNER_HANDLE_REQUEST_CS) {}
	SCPartnerAllDataInform::SCPartnerAllDataInform() : header(MT_PARTNER_ALL_DATA_INFOR_SC) {}
	SCPartnerCombatsStatus::SCPartnerCombatsStatus() : header(MT_PARTNER_COMBATS_STATUS_SC) {}
	SCPartnerAttrChangeRep::SCPartnerAttrChangeRep() : header(MT_PARTNER_ATTRIBUTE_LIST_SC) {}
	SCPartnerFettersInfoma::SCPartnerFettersInfoma() : header(MT_PARTNER_FETTERS_INFOMA_SC) {}
	SCPartnerBaseGradeInfo::SCPartnerBaseGradeInfo() : header(MT_PARTNER_ID_GRADE_STAGE_SC) {}
	CSToSaveSkillSelection::CSToSaveSkillSelection() : header(MT_PARTNER_SKILL_SELECTOR_CS) {}
	SCSavePartnersSkillRep::SCSavePartnersSkillRep() : header(MT_PARTNER_SKILL_INFORMAT_SC) {}
	SCSynFightPartnerOrder::SCSynFightPartnerOrder() : header(MT_PARTNER_SYN_FIGHT_LIST_SC) {}
	CSChangeSyncFightOrder::CSChangeSyncFightOrder() : header(MT_PARTNER_SYN_ALTER_POSI_CS) {}
	CSPartnerSwitchFightPos::CSPartnerSwitchFightPos() : header(MT_PARTNER_SWITCH_FIGHT_POS_CS) {}
	SCPartnerSingleInfo::SCPartnerSingleInfo() : header(MT_PARTNER_SINGLE_INFO_SC) {}
	SCPartnerGiftTreeListInfo::SCPartnerGiftTreeListInfo() : header(MT_PARTNER_GIFT_TREE_LIST_INFO_SC) {}
	SCPartnerSingleGiftTreeInfo::SCPartnerSingleGiftTreeInfo() : header(MT_PARTNER_SINGLE_GIFT_TREE_INFO_SC) {}
	SCPartnerSoarNoitce::SCPartnerSoarNoitce() : header(MT_PARTNER_SOAR_NOTICE_SC) {}

	// partner equipment - 伙伴装备
	CSPartnerEqOperatorReq::CSPartnerEqOperatorReq() : header(MT_PARTNER_EQUIP_OPERATOR_CS) {}
	SCPartnerEqSingleSlots::SCPartnerEqSingleSlots() : header(MT_PARTNER_EQUIP_SLOT_INF_SC) {}

	//伙伴升品
	SCPartnerQualityUp::SCPartnerQualityUp() : header(MT_PARTNER_QUALITY_UP_SC) {}
	CSPartnerQualityUpReq::CSPartnerQualityUpReq() : header(MT_PARTNER_QUALITY_UP_INFO_CS) {}

	//伙伴元素水晶装备
	//SCPartnerUseElementCrytal::SCPartnerUseElementCrytal() : header(MT_PARTNER_USE_ELEMENT_CRYTAL_INFO_SC) {}
	SCPartnerUseElementCrytalFight::SCPartnerUseElementCrytalFight() : header(MT_PARTNER_USE_ELEMENT_CRYTAL_TO_FIGHT_SC) {}
	CSPartnerElementCrytalOper::CSPartnerElementCrytalOper() : header(MT_PARTNER_ELEMENT_CRYTAL_REQ_CS) {}

	// 伙伴皮肤
	SCParnerSkinActive::SCParnerSkinActive(): header(MT_PARTNER_SKIN_ACTIVE_SC){}
	SCParnerSkinSingleOper::SCParnerSkinSingleOper() : header(MT_PARTNER_SKIN_SINGLE_OPER_SC){}
	CSPartnerSkinReq::CSPartnerSkinReq():header(MT_PARTNER_SKIN_REQ_CS){}

	// task - 任务
	CSTaskInfoSingleHandle::CSTaskInfoSingleHandle() : header(MT_TASK_SINGLE_HANDLE_CS) {}
	SCTaskDetailedInfoList::SCTaskDetailedInfoList() : header(MT_TASK_INFORMAT_LIST_SC) {}
	SCTaskDeleteNotifyList::SCTaskDeleteNotifyList() : header(MT_TASK_DETELE_HANDLE_SC) {}
	CSTalkWithAppointedNPC::CSTalkWithAppointedNPC() : header(MT_TASK_TALK_WITH_NPC_CS) {}
	CSGiveNPCTaskNeedItems::CSGiveNPCTaskNeedItems() : header(MT_TASK_GIVE_NPC_ITEM_CS) {}
	SCTaskCompleteTaskList::SCTaskCompleteTaskList() : header(MT_TASK_COMPLETE_TASK_SC) {}
	SCTaskCanAbandonedList::SCTaskCanAbandonedList() : header(MT_TASK_CAN_ABANDONED_SC) {}
	SCTaskSingleStatusInfo::SCTaskSingleStatusInfo() : header(MT_TASK_SINGLE_STATUS_SC) {}
	CSSetupTaskTraceStatus::CSSetupTaskTraceStatus() : header(MT_TASK_SET_TRACE_REQ_CS) {}
	SCTaskTraceInformation::SCTaskTraceInformation() : header(MT_TASK_TRACE_INFORMA_SC) {}
	CSTaskChainOp::CSTaskChainOp() : header(MT_TASK_CHAIN_OP_CS) {}
	SCTaskChainInfo::SCTaskChainInfo() : header(MT_TASK_CHAIN_INFO_SC) {}
	SCTaskChainMonsterInfo::SCTaskChainMonsterInfo() : header(MT_TASK_CHAIN_MONSTER_SC) {}
	SCPatrolTaskInfo::SCPatrolTaskInfo() : header(MT_TASK_PATROL_CS) {}
	CSTaskGameReq::CSTaskGameReq() : header(MT_TASK_GAME_PLAY_CS) {}
	CSTaskCommitMoneyReq::CSTaskCommitMoneyReq() : header(MT_TASK_COMMIT_MONEY_CS) {}
	CSTaskSkipReq::CSTaskSkipReq() : header(MT_TASK_SKIP_CS) {}
	// workshop - 工坊
	CSWorkshopOperateRequire::CSWorkshopOperateRequire() : header(MT_WORKSHOP_HANDLE_REQUEST_CS) {}
	SCWorkshopAllInfoRespond::SCWorkshopAllInfoRespond() : header(MT_WORKSHOP_ALL_DATA_INFOR_SC) {}
	SCSingleRecipeActiveFlag::SCSingleRecipeActiveFlag() : header(MT_WORKSHOP_INDIVID_RECIPE_SC) {}
	SCJewelryReforgeOutcomes::SCJewelryReforgeOutcomes() : header(MT_WORKSHOP_REFORGE_RESULT_SC) {}
	SCGemOneKeyUpgradesNeeds::SCGemOneKeyUpgradesNeeds() : header(MT_WORKSHOP_GEM_UPGRA_MATE_SC) {}
	CSConvertPetSkillBookReq::CSConvertPetSkillBookReq() : header(MT_WORKSHOP_EXC_SKILL_BOOK_CS) {}
	SCWorkshopComposeResults::SCWorkshopComposeResults() : header(MT_WORKSHOP_COMPOSE_RESULT_SC) {}
	CSChangeEquipVocReq::CSChangeEquipVocReq() : header(MT_WORKSHOP_EQUIP_VOC_CHANGE_CS) {}
	CSWorkShopJewelryInfo::CSWorkShopJewelryInfo() : header(MT_WORKSHOP_JEWELRY_TIMES_CS) {}
	CSWorkshopDecomposeReq::CSWorkshopDecomposeReq() : header(MT_WORKSHOP_EQUIP_DECOMPOSE_CS){}

	// 宠物装备
	CSPetEquipMentReq::CSPetEquipMentReq() : header(MT_PET_EQUIPMENT_REQ_CS) {}

	// jewelry - 灵饰
	CSJewelryHandleReq::CSJewelryHandleReq() : header(MT_JEWELRY_HANDLE_REQUEST_CS) {}
	SCJewelryAllInform::SCJewelryAllInform() : header(MT_JEWELRY_ALL_INFO_REPLY_SC) {}
	SCJewelryInfoAlter::SCJewelryInfoAlter() : header(MT_JEWELRY_SINGLE_CHANGES_SC) {}

	// systemnotice - 功能预告
	CSSystemNoticeGetAward::CSSystemNoticeGetAward() : header(MT_SYSTEM_NOTICE_GET_AWARD_CS) {}
	SCSystemNoticeFlagInfo::SCSystemNoticeFlagInfo() : header(MT_SYSTEM_NOTICE_FLAG_INFO_SC) {}
	CSChangeAppearance::CSChangeAppearance() : header(MT_CHANGE_APPEARANCE_CS) {}
	CSChangeHeadshot::CSChangeHeadshot() : header(MT_CHANGE_HEADSHOT_CS) {}

	//宝箱变化通知
	SCNoticeNearbyOnOpenTreasureChest::SCNoticeNearbyOnOpenTreasureChest() : header(MT_NOTICE_NEARBY_OPEN_TREASURE_CHEST_SC) {}
	SCTreasureChestRewardList::SCTreasureChestRewardList() : header(MT_TREASURE_CHEST_REWARD_LIST_SC) {}

	// offline exp - 离线经验
	SCOfflineExpAddition::SCOfflineExpAddition() : header(MT_OFFLINE_EXP_ADD_INFO_SC) {}
	CSOfflineExpReceives::CSOfflineExpReceives() : header(MT_OFFLINE_EXP_RECEIVES_CS) {}

	// npc - 非玩家角色
	CSNPCHandleRequest::CSNPCHandleRequest() : header(MT_NPC_HANDLE_REQUEST_CS) {}
	SCNPCStartToGather::SCNPCStartToGather() : header(MT_NPC_GATHER_TIMER_S_SC) {}
	SCNPCStopGathering::SCNPCStopGathering() : header(MT_NPC_CANCEL_GATHERS_SC) {}
	SCNPCGathersNotify::SCNPCGathersNotify() : header(MT_NPC_GATHER_NOTIFYS_SC) {}
	SCNPCInfoNotify::SCNPCInfoNotify() : header(MT_NPC_INFO_NOTIFY_SC) {}
	SCNPCCommitBack::SCNPCCommitBack() : header(MT_NPC_COMMIT_BACK_SC) {}
	SCNPCShopInfo::SCNPCShopInfo() : header(MT_NPC_SHOP_INFO_SC) {}
	CSNPCShopBuyReq::CSNPCShopBuyReq() : header(MT_NPC_SHOP_BUY_CS) {}

	// team - 队伍
	CSChangeTeamLimit::CSChangeTeamLimit() : header(MT_CHANGE_TEAM_LIMIT_CS) {}
	CSCreateTeam::CSCreateTeam() : header(MT_CREATE_TEAM_CS) {}
	CSDismissTeam::CSDismissTeam() : header(MT_DISMISS_TEAM_CS) {}
	CSUnfullTeamListByTypeReq::CSUnfullTeamListByTypeReq() : header(MT_UNFULL_TEAM_LIST_BY_TYPE_REQ_CS) {}
	SCTeamListAck::SCTeamListAck() : header(MT_TEAM_LIST_ACK_SC) {}
	SCTeamInfo::SCTeamInfo() : header(MT_TEAM_INFO_SC) {}
	CSTeamSwitchPosition::CSTeamSwitchPosition() : header(MT_TEAM_SWITCH_POSITION_CS) {}
	SCTeamEnterRoute::SCTeamEnterRoute() : header(MT_TEAM_ENTER_ROUTE_SC) {}
	SCTeamEnterFbInfo::SCTeamEnterFbInfo() : header(MT_TEAM_ENTER_INFO_SC) {}
	CSTeamEnterFbRet::CSTeamEnterFbRet() : header(MT_TEAM_ENTER_FB_RET_CS) {}
	SCTeamRemoveApply::SCTeamRemoveApply() : header(MT_TEAM_REMOVE_APPLY_SC) {}
	SCApplyList::SCApplyList() : header(MT_TEAM_APPLY_LIST_SC) {}
	SCOutOfTeam::SCOutOfTeam() : header(MT_OUT_OF_TEAM_SC) {}
	SCInviteNotice::SCInviteNotice() : header(MT_INVITE_USER_TRANSMIT_SC) {}
	SCJoinReq::SCJoinReq() : header(MT_REQ_JOIN_TEAM_SC) {}
	CSInviteJoinReq::CSInviteJoinReq() : header(MT_INVITE_USER_CS) {}
	CSExitTeamReq::CSExitTeamReq() : header(MT_EXIT_TEAM_CS) {}
	CSKickOutOfTeamReq::CSKickOutOfTeamReq() : header(MT_KICK_OUT_OF_TEAM_CS) {}
	CSJoinTeamReq::CSJoinTeamReq() : header(MT_REQ_JOIN_TEAM_CS) {}
	CSJoinTeamReqRet::CSJoinTeamReqRet() : header(MT_REQ_JOIN_TEAM_RET_CS) {}
	CSInviteJoinRet::CSInviteJoinRet() : header(MT_INVITE_JOIN_TEAM_RET_CS) {}
	SCTeamMatchInfo::SCTeamMatchInfo() : header(MT_TEAM_MATCH_INFO_SC) {}
	CSTeamMatchOp::CSTeamMatchOp() : header(MT_TEAM_MATCH_REQ_CS) {}
	CSChangeTeamLeader::CSChangeTeamLeader() : header(MT_CHANGE_TEAM_LEADER_CS) {}
	CSChangeMustCheck::CSChangeMustCheck() : header(MT_CHANGE_MUST_CHECK_CS) {}
	CSTemporarilyPartReq::CSTemporarilyPartReq() : header(MT_TEAM_TEMPORARILY_PART_CS) {}
	CSTeamInvitationReq::CSTeamInvitationReq() : header(MT_TEAM_INVITATION_REQ_CS) {}
	SCTeamInvitationNotice::SCTeamInvitationNotice() : header(MT_TEAM_INVITATION_NOTIC_SC) { invite_type = 0; }
	CSTeamInvitationRet::CSTeamInvitationRet() : header(MT_TEAM_INVITATION_RET_CS) {}
	CSTeamRecruit::CSTeamRecruit() : header(MT_TEAM_RECRUIT_CS) {}
	CSTeamSwitchToCross::CSTeamSwitchToCross() : header(MT_TEAM_SWITCH_TO_CROSS) {}
	SCTeamChannelInfo::SCTeamChannelInfo() : header(MT_TEAM_CHANNEL_INFO_SC) {}
	SCTeamMemberAppearanceChange::SCTeamMemberAppearanceChange() : header(MT_TEAM_MEMBER_APPEARANCE_CHANGE_SC) {}
	SCTeamPosInfo::SCTeamPosInfo() : header(MT_TEAM_POS_INFO_SC) {}
	SCTeamLeaderChangeNoticeArea::SCTeamLeaderChangeNoticeArea() : header(MT_TEAM_LEADER_CHANGE_NOTICE_AREA_SC) {}
	SCTeamVoteStart::SCTeamVoteStart() : header(MT_TEAM_VOTE_START_SC) {}
	CSTeamVote::CSTeamVote() : header(MT_TEAM_ROLE_VOTE_CS) {}
	CSTeamLaunchLeaderVote::CSTeamLaunchLeaderVote() : header(MT_TEAM_LAUNCH_LEADER_VOTE_CS) {}
	SCUpdateMemberInfo::SCUpdateMemberInfo() : header(MT_TEAM_SYNC_MEMBER_HPMP_SC) {}
	CSTeamLeaderNpcReq::CSTeamLeaderNpcReq() : header(MT_TEAM_TEAM_LEADER_NPC_REQ_CS) {}
	SCTeamLeaderNpcInfo::SCTeamLeaderNpcInfo() : header(MT_TEAM_TEAM_LEADER_NPC_INFO_SC) {}
	SCUpdateMemberPetHPMP::SCUpdateMemberPetHPMP() : header(MT_TEAM_SYNC_MEMBER_PET_HPMP_SC) {}
	SCTeamMemberAddHpMpNotice::SCTeamMemberAddHpMpNotice() : header(MT_TEAM_MEMBER_ADD_HP_MP_NOTICE_SC) {}
	SCTeamSimpleInfoChangeNotice::SCTeamSimpleInfoChangeNotice() : header(MT_TEAM_SIMPLE_INFO_CHANGE_NOTICE_SC) {}
	SCTeamMemberBaseInfoChangeNotice::SCTeamMemberBaseInfoChangeNotice() : header(MT_TEAM_CHANGE_MEMBER_BASE_INFO_NOTICE_SC) {}
	CSTeamLeaderSummonMember::CSTeamLeaderSummonMember() : header(MT_TEAM_LEADER_SUMMON_MEMBER_CS) {}
	SCTeamLeaderSummonMemberNotice::SCTeamLeaderSummonMemberNotice() : header(MT_TEAM_LEADER_SUMMON_MEMBER_NOTICE_SC) {}
	CSTeamMemberReplyLeaderSummon::CSTeamMemberReplyLeaderSummon() : header(MT_TEAM_MEMBER_REPLY_SUMMON_CS) {}
	TeamMemberIncompatibleNotice::TeamMemberIncompatibleNotice() : header(MT_TEAM_MEMBER_INCOMPATIBLE_NOTICE_SC) {}
	CSLeaderPartnerInfo::CSLeaderPartnerInfo() : header(MT_TEAM_LEADER_BATTLE_PARTNER_LIST_CS) {}
	SCLeaderPartnerInfo::SCLeaderPartnerInfo() : header(MT_TEAM_LEADER_BATTLE_PARTNER_LIST_SC) {}
	CSLeaderCheckMemberItem::CSLeaderCheckMemberItem() : header(MT_TEAM_LEADER_CHECK_MEMBER_ITEM_CS) {}
	SCLeaderCheckMemberItemRet::SCLeaderCheckMemberItemRet() : header(MT_TEAM_LEADER_CHECK_MEMBER_ITEM_RET_SC) {}
	CSReqJoinTeamInfo::CSReqJoinTeamInfo() : header(MT_REQ_JOIN_TEAM_INFO_CS) {}
	SCTeamRedNameNotifyArea::SCTeamRedNameNotifyArea() : header(MT_TEAM_IS_RED_NAME_INFO_CS) {}
	CSTeamLeaderEjectWindowsReq::CSTeamLeaderEjectWindowsReq() : header(MT_TEAM_LEADER_EJECT_WINDOWS_CS) {}
	SCTeamLeaderEjectWindowsInfo::SCTeamLeaderEjectWindowsInfo() : header(MT_TEAM_LEADER_EJECT_WINDOWS_SC) {}
	
	// gm - 开发者命令
	SCGMCommand::SCGMCommand() : header(MT_GM_COMMAND_SC) {}
	CSGMCommand::CSGMCommand() : header(MT_GM_COMMAND_CS) {}

	SCHearsayInfo::SCHearsayInfo() : header(MT_HEARSAY_INFO_SC) {}

	// prestige - 声望
	SCPrestigeAllVillageInfo::SCPrestigeAllVillageInfo() : header(MT_PRESTIGE_ALL_VILLAGE_INFO_SC) {}
	SCPrestigeAllShopInfo::SCPrestigeAllShopInfo() : header(MT_PRESTIGE_ALL_SHOP_IINFO_SC) {}
	SCPrestigeSingleVillageInfo::SCPrestigeSingleVillageInfo() : header(MT_PRESTIGE_SINGLE_VILLAGE_INFO_SC) {}
	SCPrestigeSingleShopInfo::SCPrestigeSingleShopInfo() : header(MT_PRESTIGE_SINGLE_SHOP_INFO_SC) {}
	CSPrestigeShopingReq::CSPrestigeShopingReq() : header(MT_PRESTIGE_SHOPING_CS) {}
	SCPrestigeDonateInfo::SCPrestigeDonateInfo() : header(MT_PRESTIGE_DONATE_INFO_SC) {}
	CSPrestigeDonateOp::CSPrestigeDonateOp() : header(MT_PRESTIGE_DONATE_OP_CS) {}
	SCPrestigeDonateNotice::SCPrestigeDonateNotice() : header(MT_PRESTIGE_DONATE_NOTICE_SC) {}
	SCPrestigeChangeNotice::SCPrestigeChangeNotice() : header(MT_PRESTIGE_CHANGE_NOTICE_SC) {}

	// func guide - 功能引导
	CSFuncGuideOperator::CSFuncGuideOperator() : header(MT_FUNCGUIDE_OPERATOR_REQUEST_CS) {}
	SCFuncGuideFlagInfo::SCFuncGuideFlagInfo() : header(MT_FUNCGUIDE_EVENT_FLAG_INFOR_SC) {}
	CSPetDeadSummonGuideTrigger::CSPetDeadSummonGuideTrigger() : header(MT_PET_DEAD_GUIDE_TRIGGER_CS) {}
	SCPetDeadSummonGuideInfo::SCPetDeadSummonGuideInfo() : header(MT_PET_DEAD_GUIDE_INFO_SC) {}

	// title - 称号
	SCTitleAllInfo::SCTitleAllInfo() : header(MT_TITLE_ALL_INFO_SC) {}
	SCTitleSingleInfo::SCTitleSingleInfo() : header(MT_TITLE_SINGLE_INFO_SC) {}
	SCTitleOtherInfo::SCTitleOtherInfo() : header(MT_TITLE_OTHER_INFO_SC) {}
	CSTitleOp::CSTitleOp() : header(MT_TITLE_OP_CS) {}
	SCTitleChange::SCTitleChange() : header(MT_TITLE_CHANGE_CS) {}
	SCTitleNotice::SCTitleNotice() : header(MT_TITLE_NOTICE_SC) {}

	SCTrainAllInfo::SCTrainAllInfo() : header(MT_TRAIN_ALL_INFO_SC) {}
	CSTrainReq::CSTrainReq() : header(MT_TRAIN_REQ_CS) {}
	CSRolePetTrainSkillReq::CSRolePetTrainSkillReq() : header(MT_ROLE_PET_TRAIN_SKILL_REQ_CS) {}
	SCTrainInfoPromoted::SCTrainInfoPromoted() : header(MT_TRAIN_INFO_PROMOTED_SC) {}
	SCTrainRedDotFlagInfo::SCTrainRedDotFlagInfo() : header(MT_TRAIN_RED_DOT_FLAG_SC) {}

	//equipment - 装备
	CSEquipmentOperaReq::CSEquipmentOperaReq() : header(MT_EQUIPMENT_OPERA_REQ_CS) {}
	SCEquipmentInfoAck::SCEquipmentInfoAck() : header(MT_EQUIPMENT_INFO_ACK_SC) {}
	SCEquipmentGridChange::SCEquipmentGridChange() : header(MT_EQUIPMENT_GRID_CHANGE_SC) {}
	SCElementCrystalChangeNotice::SCElementCrystalChangeNotice() : header(MT_ELEMENT_CRYSTAL_CHANGE_NOTCIE_SC) {}
	SCEquipmentLockInfo::SCEquipmentLockInfo() : header(MT_EQUIP_VOC_LOCK_INFO_SC) {}
	SCEquipmentJewerlyUpGreadeInfo::SCEquipmentJewerlyUpGreadeInfo() : header(MT_EQUIPMENT_JEWERLY_UP_GREADE) {}
	SCEquipmentJewerlyResonaceInfo::SCEquipmentJewerlyResonaceInfo() : header(MT_EQUIP_JEWERLY_RESONACE_INFO_SC) {}

	CSZaoHuaEquipmentOperaReq::CSZaoHuaEquipmentOperaReq() : header(MT_ZAOHUA_EQUIP_OPERA_REQ_CS) {}
	SCZaoHuaBagListInfo::SCZaoHuaBagListInfo() : header(MT_ZAOHUA_BAG_LIST_SC) {}
	SCZaoHuaBagItemChange::SCZaoHuaBagItemChange() : header(MT_ZAOHUA_BAG_SIGNLE_INFO_SC) {}
	SCZaoHuaEquipmentInfoAck::SCZaoHuaEquipmentInfoAck() : header(MT_ZAOHUA_WEAR_LIST_SC) {}
	SCZaoHuaEquipmentGridChange::SCZaoHuaEquipmentGridChange() : header(MT_ZAOHUA_WEAR_SIGNLE_SC) {}
	SCZaoHuaEquipmentComposeResults::SCZaoHuaEquipmentComposeResults() : header(MT_ZAOHUA_COMPOSE_RESULT_SC) {}
	SCZaoHuaAppearance::SCZaoHuaAppearance() : header(MT_ZAOHUA_WEAR_APPEARANCE_SC) {}
	CSZaoHuaEquipmentDiscomposeReq::CSZaoHuaEquipmentDiscomposeReq() : header(MT_ZAOHUA_DISCOMPOSE_CS) {}
	SCZaoHuaEquipmentCommonDataInfo::SCZaoHuaEquipmentCommonDataInfo() : header(MT_ZAOHUA_COMMON_INFO_SC) {}

	SCCollectionAllInfo::SCCollectionAllInfo() : header(MT_COLLECTION_ALL_INFO_SC) {}
	SCCollectionSingleInfo::SCCollectionSingleInfo() : header(MT_COLLECTION_SINGLE_INFO_SC) {}
	CSCollectionOp::CSCollectionOp() : header(MT_COLLECTION_OP_CS) {}
	SCCollectionIncluded::SCCollectionIncluded() : header(MT_COLLECTION_INCLUDED_SC) {}
	SCCollectionIncludedSignle::SCCollectionIncludedSignle() : header(MT_COLLECTION_INCLUDED_SIGNLE_SC) {}

	SCCollectionAtlasCardInfo::SCCollectionAtlasCardInfo() : header(MT_COLLECTION_ATLAS_CARD_INFO_SC) {}
	SCCollectionAtlasCardSignleInfo::SCCollectionAtlasCardSignleInfo() : header(MT_COLLECTION_ATLAS_CARD_SINGLE_SC) {}

	SCAnswerAllInfo::SCAnswerAllInfo() : header(MT_WST_QUESTION_ALL_INFO_SC) {}
	CSAnswerOp::CSAnswerOp() : header(MT_WST_QUESTION_OP_CS) {}
	CSExpCrystalOp::CSExpCrystalOp() : header(MT_EXP_CRYSTAL_OPEN_CS) {}
	SCExpCrystalInfo::SCExpCrystalInfo() : header(MT_EXP_CRYSTAL_INFO_SC) {}
	SCAnswerOverInfo::SCAnswerOverInfo() : header(MT_WST_QUESTION_OVER_INFO_SC) {}

	CSTreviFountainOp::CSTreviFountainOp() : header(MT_TREVI_FOUNTAIN_OPEN_CS) {}
	SCTreviFountainInfo::SCTreviFountainInfo() : header(MT_TREVI_FOUNTAIN_INFO_SC) {}

	SCSkillList::SCSkillList() : header(MT_SKILL_LIST_SC) {}
	SCSkillChangeNotice::SCSkillChangeNotice() : header(MT_SKILL_CHANGE_NOTICE) {}
	CSSkillOperateReq::CSSkillOperateReq() : header(MT_SKILL_OPERATE_REQ_CS) {}
	SCLifeSkillInfo::SCLifeSkillInfo() : header(MT_LIFE_SKILL_INFO_SC) {}
	SCCommonUseSkillChangeNotice::SCCommonUseSkillChangeNotice() : header(MT_COMMON_USE_SKILL_CHANGE_NOTICE) {}
	SCSkillExpDeltaChangeNotify::SCSkillExpDeltaChangeNotify() : header(MT_SKILL_EXP_CHANGE_NOTIFY_SC) {}
	CSSkillPositionModify::CSSkillPositionModify() : header(MT_SKILL_POSITION_MODIFY_CS) {}

	CSNewLifeSkillReq::CSNewLifeSkillReq() : header(MT_NEW_LIFE_SKILL_REQ_CS) {}
	SCNewLifeSkillAllInfo::SCNewLifeSkillAllInfo() : header(MT_NEW_LIFE_SKILL_ALL_INFO_SC) {}
	SCNewLifeSkillSingleInfo::SCNewLifeSkillSingleInfo() : header(MT_NEW_LIFE_SKILL_SINGLE_INFO_SC) {}
	SCNewLifeSkillPropsAllInfo::SCNewLifeSkillPropsAllInfo() : header(MT_NEW_LIFE_SKILL_PROPS_ALL_INFO_SC) {}
	SCNewLifeSkillPropsSingleInfo::SCNewLifeSkillPropsSingleInfo() : header(MT_NEW_LIFE_SKILL_PROPS_SINGLE_INFO_SC) {}
	SCNewLifeSkillChangeCardSingleInfo::SCNewLifeSkillChangeCardSingleInfo() : header(MT_LIFE_SKILL_CHANGE_CARD_SINGLE_INFO_SC) {}

	CSFishGroundReq::CSFishGroundReq() : header(MT_FISH_GROUND_REQ_CS) {}
	SCFishingNotify::SCFishingNotify() : header(MT_FISHING_NOTIFY_SC) {}
	SCStopFishingNotify::SCStopFishingNotify() : header(MT_STOP_FISHING_NOTIFY_SC) {}

	//活动
	SCActivityStatus::SCActivityStatus() : header(MT_ACTIVITY_STATUS_SC) {}
	CSActivityEnterReq::CSActivityEnterReq() : header(MT_ACTIVITY_ENTER_REQ_CS) {}
	CSChongzhiOperateReq::CSChongzhiOperateReq() : header(MT_CHONGZHI_OPERATE_CS) {}
	SCChongzhiInfo::SCChongzhiInfo() : header(MT_CHONGZHI_INFO_SC) {}

	//活动 - 组队竞技
	CSActivityTeamFightMatchReq::CSActivityTeamFightMatchReq() : header(MT_ACTIVITY_TEAM_FIGHT_MATCH_CS) {}
	SCActivityTeamFightNotice::SCActivityTeamFightNotice() : header(MT_ACTIVITY_TEAM_FIGHT_NOTIC_SC) {}
	SCActivityTeamFightScore::SCActivityTeamFightScore() : header(MT_ACTIVITY_TEAM_FIGHT_SCORE_SC) {}
	SCActivityTeamFightRank::SCActivityTeamFightRank() : header(MT_ACTIVITY_TEAM_FIGHT_RANK_SC) {}
	SCActivityTeamFightRecord::SCActivityTeamFightRecord() : header(MT_ACTIVITY_TEAM_FIGHT_RECORD_SC) {}
	SCActivityTeamFightMatchSucc::SCActivityTeamFightMatchSucc() : header(MT_ACTIVITY_TEAM_FIGHT_MATCH_SUCC_NOTICE_SC) {}

	//活动 - 迷宫竞速
	CSActivityMazeRacerHandleReq::CSActivityMazeRacerHandleReq() : header(MT_ACTIVITY_MAZE_RACER_HANDLE_REQ_CS) {}
	SCActivityMazeRacerAmuletNum::SCActivityMazeRacerAmuletNum() : header(MT_ACTIVITY_MAZE_RACER_AMULET_NUM_SC) {}
	SCActivityMazeRacerRankLists::SCActivityMazeRacerRankLists() : header(MT_ACTIVITY_MAZE_RACER_RANK_INFOR_SC) {}
	SCActivityMazeRacerWaitsInfo::SCActivityMazeRacerWaitsInfo() : header(MT_ACTIVITY_MAZE_RACER_HALL_INFOR_SC) {}
	SCActivityMazeRacerCopysInfo::SCActivityMazeRacerCopysInfo() : header(MT_ACTIVITY_MAZE_RACER_COPY_INFOR_SC) {}
	SCActivityMazeRacerPassdInfo::SCActivityMazeRacerPassdInfo() : header(MT_ACTIVITY_MAZE_RACER_PASS_INFOR_SC) {}
	SCActivityMazeRacerCurseInfo::SCActivityMazeRacerCurseInfo() : header(MT_ACTIVITY_MAZE_RACER_BUFF_INFOR_SC) {}
	SCActivityMazeRacerActInsert::SCActivityMazeRacerActInsert() : header(MT_ACTIVITY_MAZE_RACER_ACT_INSERT_SC) {}
	SCActivityMazeRacerSceneInfo::SCActivityMazeRacerSceneInfo() : header(MT_ACTIVITY_MAZE_RACER_TRANSPORT_POS_SC) {}
	SCActivityMazeRacerRoleInfo::SCActivityMazeRacerRoleInfo() : header(MT_ACTIVITY_MAZE_RACER_ROLE_INFO) {}

	//活动 - 家族大乱斗
	CSActivityGuildFightOperatorReq::CSActivityGuildFightOperatorReq() : header(MT_ACTIVITY_GUILD_FIGHT_HANDLE_REQ_CS) {}
	SCActivityGuildFightRankAllInfo::SCActivityGuildFightRankAllInfo() : header(MT_ACTIVITY_GUILD_FIGHT_RANK_INFOR_SC) {}
	SCActivityGuildFightMyGuildInfo::SCActivityGuildFightMyGuildInfo() : header(MT_ACTIVITY_GUILD_FIGHT_GUILD_INFO_SC) {}
	SCActivityGuildFightParticipNum::SCActivityGuildFightParticipNum() : header(MT_ACTIVITY_GUILD_FIGHT_PARTIC_NUM_SC) {}
	SCActivityGuildFightSpecialInfo::SCActivityGuildFightSpecialInfo() : header(MT_ACTIVITY_GUILD_FIGHT_SPECIAL_INFO_SC) {}
	SCActivityEnterInfo::SCActivityEnterInfo() : header(MT_ACTIVITY_ENTER_INFO_SC) {}

	//活动 - 家族答题
	SCActivityGuildAnswerCurQuestion::SCActivityGuildAnswerCurQuestion() : header(MT_ACTIVITY_GUILD_ANSWER_CUR_QUESTION_SC) {}
	CSActivityGuildAnswerAnswerQuest::CSActivityGuildAnswerAnswerQuest() : header(MT_ACTIVITY_GUILD_ANSWER_ANSWER_QUEST_CS) {}
	SCActivityGuildAnswerResultInfor::SCActivityGuildAnswerResultInfor() : header(MT_ACTIVITY_GUILD_ANSWER_RESULT_INFOR_SC) {}
	SCActivityGuildAnswerAwardSettle::SCActivityGuildAnswerAwardSettle() : header(MT_ACTIVITY_GUILD_ANSWER_AWARD_SETTLE_SC) {}

	//活动 - 以一敌百(个人版家族大乱斗)
	CSActivityOnlyFightOperatorReq::CSActivityOnlyFightOperatorReq() : header(MT_ACTIVITY_ONLY_FIGHT_HANDLE_REQ_CS) {}
	SCActivityOnlyFightRankAllInfo::SCActivityOnlyFightRankAllInfo() : header(MT_ACTIVITY_ONLY_FIGHT_RANK_INFO_SC) {}
	SCActivityOnlyFightMyInfo::SCActivityOnlyFightMyInfo() : header(MT_ACTIVITY_ONLY_FIGHT_MY_INFO_SC) {}
	SCActivityOnlyFightSpecialInfo::SCActivityOnlyFightSpecialInfo() : header(MT_ACTIVITY_ONLY_FIGHT_SPECIAL_INFO_SC) {}

	//限时活动 - 丝绸之路
	CSActivitySilkRoadsReq::CSActivitySilkRoadsReq() : header(MT_ACTIVITY_SILK_ROADS_REQ_CS) {}
	SCActivitySilkRoadsSimpleInfo::SCActivitySilkRoadsSimpleInfo() : header(MT_ACTIVITY_SILK_ROADS_SIMPLE_INFO_SC) {}
	SCActivitySilkRoadsRoleAllInfo::SCActivitySilkRoadsRoleAllInfo() :header(MT_ACTIVITY_SILK_ROADS_ROLE_ALL_INFO_SC) {}
	SCActivitySilkRoadsAssetsInfo::SCActivitySilkRoadsAssetsInfo() : header(MT_ACTIVITY_SILK_ROADS_ASSETS_INFO_SC) {}
	SCActivitySilkRoadsWarehouseChange::SCActivitySilkRoadsWarehouseChange() : header(MT_ACTIVITY_SILK_ROADS_WAREHOUSE_CHANGE_SC) {}
	SCActivitySilkRoadsAllRankInfo::SCActivitySilkRoadsAllRankInfo() : header(MT_ACTIVITY_SILK_ROADS_ALL_RANK_INFO_SC) {}
	SCActivitySilkRoadsAllTradingLogInfo::SCActivitySilkRoadsAllTradingLogInfo() :header(MT_ACTIVITY_SILK_ROADS_ALL_TRADING_LOG_SC) {}
	SCActivitySilkRoadsSingleTradingLogNotice::SCActivitySilkRoadsSingleTradingLogNotice() :header(MT_ACTIVITY_SILK_ROADS_SINGLE_TRADING_SC) {}

	//特殊活动货币增加
	SCRAMoneyAddInfo::SCRAMoneyAddInfo() : header(MT_RAND_ACTIVITY_MONEY_ADD_INFO_SC) {}

	//活动-赠寻宝次数
	SCRAGiveSecretKeyInfo::SCRAGiveSecretKeyInfo() : header(MT_RAND_ACTIVITY_GIVE_SECRET_KEY_INFO_SC) {}

	//随机活动 - 累计充值
	SCRACumulativeChargeInfo::SCRACumulativeChargeInfo() : header(MT_RAND_ACTIVITY_CUMULATIVE_RECHARGE_SC) {}

	//限时活动 - 报名
	CSActivitySignUpReq::CSActivitySignUpReq() : header(MT_ACTIVITY_SIGN_UP_REQ_CS) {}
	SCActivitySignUpInfo::SCActivitySignUpInfo() : header(MT_ACTIVITY_SIGN_UP_INFO_SC) {}

	CSActivityAdvanceFetchReq::CSActivityAdvanceFetchReq() : header(MT_ACTIVITY_ADVANCE_FETCH_REQ_CS) {}
	SCActivityAdvanceFetchInfo::SCActivityAdvanceFetchInfo() : header(MT_ACTIVITY_ADVANCE_FETCH_INFO_SC) {}
	SCActivityConfigReloadNotice::SCActivityConfigReloadNotice() : header(MT_RELOAD_ACTIVITY_NOTICE_SC) {}
	SCNoticeClientRestart::SCNoticeClientRestart() : header(MT_NOTICE_CLIENT_RESTART_SC) {}
	SCCmdBuyItemCheckRet::SCCmdBuyItemCheckRet() : header(MT_CMD_BUY_ITEM_CHECK_RET_SC) {}
	CSCmdBuyItemCheckReq::CSCmdBuyItemCheckReq() : header(MT_CMD_BUY_ITEM_CHECK_REQ_CS) {}

	//随机活动 - 琼楼玉宴
	SCQiongLouYuYanInfo::SCQiongLouYuYanInfo() : header(MT_RA_QIONG_LOU_YU_YAN_INFO_SC) {}
	SCQiongLouYuYanRoleInfo::SCQiongLouYuYanRoleInfo() : header(MT_RA_QIONG_LOU_YU_YAN_ROLE_INFO_SC) {}

	//随机活动 - 夏日迷宫
	SCMazeSummerMapInfo::SCMazeSummerMapInfo() : header(MT_RA_MAZE_SUMMER_MAP_INFO_SC) {}
	SCMazeSummerSingleMapInfo::SCMazeSummerSingleMapInfo() : header(MT_RA_MAZE_SUMMER_MAP_SINGLE_INFO_SC) {}
	SCMazeSummerShopInfo::SCMazeSummerShopInfo() : header(MT_RA_MAZE_SUMMER_SHOP_INFO_SC) {}
	SCMazeSummerQuestionInfo::SCMazeSummerQuestionInfo() : header(MT_RA_MAZE_SUMMER_QUESTION_INFO_SC) {}
	SCMazeSummerTaskInfo::SCMazeSummerTaskInfo() : header(MT_RA_MAZE_SUMMER_TASK_INFO_SC) {}
	SCMazeSummerStatusInfo::SCMazeSummerStatusInfo() : header(MT_RA_MAZE_SUMMER_STATUS_INFO_SC) {}
	SCMazeSummerPlayFinishInfo::SCMazeSummerPlayFinishInfo() : header(MT_RA_MAZE_SUMMER_FLAYINFO_FINISH_SC) {}
	SCMazeSummerMyPosInfo::SCMazeSummerMyPosInfo() : header(MT_RA_MAZE_SUMMER_MY_POS_INFO_SC) {}

	//伙伴兑换活动
	SCRAActivityPartnerExchangeInfo::SCRAActivityPartnerExchangeInfo() : header(MT_ACTIVITY_PARTNER_EXCHANGE_INFO_SC){}
	SCRAActivityPetExchangeInfo::SCRAActivityPetExchangeInfo() : header(MT_ACTIVITY_PET_EXCHANGE_INFO_SC) {}
	SCRAPetExchangeSingleInfo::SCRAPetExchangeSingleInfo() : header(MT_RAND_ACTIVITY_PET_EXCHANGE_SIGNLE_INFO_SC) {}

	//天命卜卦
	SCRATianMingDivinationInfo::SCRATianMingDivinationInfo() : header(MT_RA_TIAN_MING_DIVINATION_INFO_SC) {}
	SCTianMingDivinationActivityStartChouResult::SCTianMingDivinationActivityStartChouResult() : header(MT_RA_TIANMING_DIVINATION_START_CHOU_RESULT_SC) {}
	SCTianMingRewardPoolInfo::SCTianMingRewardPoolInfo() : header(MT_RA_TIANMING_DIVINATION_REWARD_POOL_SC) {}
	//新服竞技
	SCSportsHistroyInfo::SCSportsHistroyInfo() : header(MT_RA_SPORTS_HISTROY_SC) {}

	//奖励界面
	SCOnlineRewardInfo::SCOnlineRewardInfo() : header(MT_ONLINE_REWARD_INFO_SC) {}
	SCSignRewardInfo::SCSignRewardInfo() : header(MT_SIGN_REWARD_INFO_SC) {}
	SCLevelRewardInfo::SCLevelRewardInfo() : header(MT_LEVEL_REWARD_INFO_SC) {}
	CSFetchRewardReq::CSFetchRewardReq() : header(MT_FETCH_REWARD_REQ_CS) {}
	CSFindRewardReq::CSFindRewardReq() : header(MT_FIND_REWARD_REQ_CS) {}
	SCFindRewardInfo::SCFindRewardInfo() : header(MT_FIND_REWARD_INFO_SC) {}
	SCScoreRewardInfo::SCScoreRewardInfo() : header(MT_SCORE_REWARD_INFO_SC) {}
	SCMonthInvestmentInfo::SCMonthInvestmentInfo() : header(MT_MONTH_INVESTMENT_INFO) {}
	CSRewardFindNotifyCancelReq::CSRewardFindNotifyCancelReq() : header(MT_REWARD_NOTIFY_REQUEST_CS) {}
	SCRewardFindNotifys::SCRewardFindNotifys() : header(MT_REWARD_NOTIFY_INFO_SC) {}
	SCDayRewardDayFoodInfo::SCDayRewardDayFoodInfo() : header(MT_DAY_REWARD_DAY_FOOD_INFO_SC) {}
	SCPetStrengthenRewardInfo::SCPetStrengthenRewardInfo() : header(MT_PET_STRENGTHEN_REWARD_INFO_SC) {}
	CSPetStrengthenRewardReq::CSPetStrengthenRewardReq() : header(MT_PET_STRENGTHEN_REWARD_REQ_CS) {}
	SCDayRewardOnlineGiftInfo::SCDayRewardOnlineGiftInfo() : header(MT_DAY_REWARD_ONLINE_GIFT_INFO_SC) {}


	//幻化
	SCSurfaceAllInfo::SCSurfaceAllInfo() : header(MT_SURFACE_ALL_INFO_SC) {}
	SCSurfaceSingleInfo::SCSurfaceSingleInfo() : header(MT_SURFACE_SINGLE_INFO_SC) {}
	SCSurfaceWearInfo::SCSurfaceWearInfo() : header(MT_SURFACE_WEAR_INFO_SC) {}
	CSSurfaceOp::CSSurfaceOp() : header(MT_SURFACE_OP_CS) {}
	CSSurefaceDecompose::CSSurefaceDecompose() : header(MT_SURFACE_DECOMPOSE_CS) {}

	//时装升级
	CSFashionCollectionRequest::CSFashionCollectionRequest() : header(MT_FASHION_COLLECTION_REQ_CS) {}
	SCFashionCollectionInfo::SCFashionCollectionInfo() : header(MT_FASHION_COLLECTION_INFO_SC) {}

	//结婚
	CSMarriageReq::CSMarriageReq() : header(MT_MARRY_REQ_CS) {}
	SCMarryReqRoute::SCMarryReqRoute() : header(MT_MARRY_REQ_ROUTE_SC) {}
	SCMarryInfo::SCMarryInfo() : header(MT_MARRY_REQ_INFO_SC) {}
	SCDivorceReqRoute::SCDivorceReqRoute() : header(MT_DIVORCE_REQ_ROUTE_SC) {}
	SCMarryProposeNotice::SCMarryProposeNotice() : header(MT_DIVORCE_PROPOSE_NOTICE_SC) {}
	CSMarriageSeekingOpera::CSMarriageSeekingOpera() : header(MT_MARRIAGE_SEEKING_REQ_CS) {}
	SCMarriageSeekingInfo::SCMarriageSeekingInfo() : header(MT_MARRIAGE_SEEKING_INFO_SC) {}
	SCMarriageTaskRoute::SCMarriageTaskRoute() : header(MT_MARRIAGE_TASK_ROUTE_SC) {}
	SCMarriageTaskQuestionInfo::SCMarriageTaskQuestionInfo() : header(MT_MARRIAGE_TASK_QUESTION_INFO_SC) {}
	SCMarriageTaskQuestionResult::SCMarriageTaskQuestionResult() : header(MT_MARRIAGE_TASK_QUESTION_RESULT_SC) {}
	SCMarriageQueryRet::SCMarriageQueryRet() : header(MT_MARRIAGE_QUERY_OTHER_INFO_RET) {}
	SCMarriageDuetPuzzleInfo::SCMarriageDuetPuzzleInfo() : header(MT_MARRIAGE_DUET_PUZZLE_INFO) {}
	SCMarriageDuetTaskLogoutNotice::SCMarriageDuetTaskLogoutNotice() : header(MT_MARRIAGE_DUET_TAST_LOGOUT_NOTICE) {}

	//子女
	SCBabyInfo::SCBabyInfo() : header(MT_BABY_INFO_SC) {}
	CSBabyReq::CSBabyReq() : header(MT_BABY_REQ_CS) {}
	CSBabyRename::CSBabyRename() : header(MT_BABY_RENAME_CS) {}
	SCBabyReqRoute::SCBabyReqRoute() : header(MT_BABY_PLAY_ROUTE_SC) {}
	SCBabyStartPlay::SCBabyStartPlay() : header(MT_BABY_START_PLAY_SC) {}
	CSBabyActivte::CSBabyActivte() : header(MT_BABY_ACTIVE_CS) {}

	// 家族
	SCCreateRet::SCCreateRet() : header(MT_GUILD_CREATE_RET_SC) {}
	CSCreateGuild::CSCreateGuild() : header(MT_CREATE_GUILD_CS) {}
	CSQuitGuild::CSQuitGuild() : header(MT_QUIT_GUILD_REQ_CS) {}
	SCQuitGuild::SCQuitGuild() : header(MT_QUIT_GUILD_SC) {}
	SCGuildBaseInfo::SCGuildBaseInfo() : header(MT_GUILD_BASE_INFO_SC) {}
	SCGuildMemberInfo::SCGuildMemberInfo() : header(MT_GUILD_MEMBER_INFO_SC) {}
	CSApplyJoinGuildReq::CSApplyJoinGuildReq() : header(MT_GUILD_APPLY_JOIN_REQ_CS) {}
	CSApplyJoinGuildAck::CSApplyJoinGuildAck() : header(MT_GUILD_APPLY_JOIN_ACK_CS) {}
	CSGuildExpelOutReq::CSGuildExpelOutReq() : header(MT_GUILD_EXPEL_OUT_REQ_CS) {}
	CSSetGuildNoticeReq::CSSetGuildNoticeReq() : header(MT_GUILD_SET_NOTICE_REQ_CS) {}
	CSGetGuildList::CSGetGuildList() : header(MT_GET_GUILD_LIST_CS) {}
	SCSendGuildList::SCSendGuildList() : header(MT_SEND_GUILD_LIST_SC) {}
	SCSendGuildNotic::SCSendGuildNotic() : header(MT_SEND_GUILD_NOTICE_SC) {}
	SCSendGuildApplyList::SCSendGuildApplyList() : header(MT_SEND_GUILD_APPLY_LIST_SC) {}
	CSGuildAbdicationOp::CSGuildAbdicationOp() : header(MT_GUILD_ABDICATION_REQ_CS) {}
	CSGuildImpeachmentOp::CSGuildImpeachmentOp() : header(MT_GUILD_IMPEACHMENT_REQ_CS) {}
	CSGuildAppointmentOp::CSGuildAppointmentOp() : header(MT_GUILD_APPOINTMENT_REQ_CS) {}
	CSGuildShopBuy::CSGuildShopBuy() : header(MT_GUILD_SHOP_BUY_CS) {}
	SCGuildShopInfo::SCGuildShopInfo() : header(MT_GUILD_SHOP_INFO_SC) {}
	SCGuildPetTrainingExp::SCGuildPetTrainingExp() : header(MT_GUILD_PET_TRAINING_EXP_SC) {}
	CSGuildRenameReq::CSGuildRenameReq() : header(MT_GUILD_RENAME_CS) {}
	CSGuildSetBanner::CSGuildSetBanner() : header(MT_GUILD_RESET_BANNER_CS) {}

	CSGuildInvitationReq::CSGuildInvitationReq() : header(MT_GUILD_INVITATION_REQ_CS) {}
	SCGuildInvitationNotice::SCGuildInvitationNotice() : header(MT_GUILD_INVITATION_NOTIC_SC) {}
	SCGuildBossInfo::SCGuildBossInfo() : header(MT_GUILD_BOSS_INFO_SC) {}
	CSGuildBossReq::CSGuildBossReq() : header(MT_GUILD_BOSS_REQ_CS) {}
	SCGuildBossRankInfo::SCGuildBossRankInfo() : header(MT_GUILD_BOSS_RANK_INFO_SC) {}
	CSGuildLevelUpReq::CSGuildLevelUpReq() : header(MT_GUILD_LEVEL_UP_CS) {}
	SCPlayerApplyList::SCPlayerApplyList() : header(MT_PLAYER_APPLY_LIST_SC) {}
	CSInvitationReply::CSInvitationReply() : header(MT_GUILD_INVITATION_REPLY_CS) {}
	SCGuildNeedApproveInfo::SCGuildNeedApproveInfo() : header(MT_GUILD_NEED_APPROVE_INFO_SC) {}
	CSSetGuildNeedApproveReq::CSSetGuildNeedApproveReq() : header(MT_GUILD_SET_NEED_APPROVE_INFO_CS) {}
	SCGuildEventInfo::SCGuildEventInfo() : header(MT_GUILD_EVENT_INFO_CS) {}
	SCGuildBossChapterGuildRankInfo::SCGuildBossChapterGuildRankInfo() : header(MT_GUILD_BOSS_CHAPTER_GUILD_RANK_SC) {}
	SCGuildGatherDreamInfoList::SCGuildGatherDreamInfoList() : header(MT_GUILD_GATHER_DREAM_INFO_LIST_SC) {}
	SCGuildGatherDreamSignleInfo::SCGuildGatherDreamSignleInfo() : header(MT_GUILD_GATHER_DREAM_SIGNLE_INFO_SC) {}
	SCGuildQuitInfo::SCGuildQuitInfo() : header(MT_GUILD_QUIT_INFO_SC) {}
	SCGuildRecommend::SCGuildRecommend() : header(MT_GUILD_RECOMMEND_INFO_SC) {}
	CSGuildHallOfFameReq::CSGuildHallOfFameReq() : header(MT_GUILD_HALLOFFAME_REQ_CS) {}
	SCGuildHallOfFameInfo::SCGuildHallOfFameInfo() : header(MT_GUILD_HALLOFFAME_INFO_SC){}
	SCGuildQiFuInfo::SCGuildQiFuInfo() : header(MT_GUILD_QIFU_INFO_SC) {}
	
	SCGuildConstructInfo::SCGuildConstructInfo() : header(MT_GUILD_CONSTRUCT_BUILDING_INFO_SC) {}
	CSGuildConstructReq::CSGuildConstructReq() : header(MT_GUILD_CONSTRUCT_BUILDING_SEQ_CS) {}
	SCGuildBuildingFinishInfo::SCGuildBuildingFinishInfo() : header(MT_GUILD_CONSTRUCT_BUILDING_FINISH_INFO_SC) {}
	SCGuildUseBuildMaterialInfo::SCGuildUseBuildMaterialInfo() : header(MT_GUILD_CONSTRUCT_USE_BUILD_MATERIAL_INFO_SC){}
	SCGuildConstructAllInfo::SCGuildConstructAllInfo() : header(MT_GUILD_CONSTRUCT_BUILDING_ALL_INFO_SC) {}
	SCGuildSpeedUpInfo::SCGuildSpeedUpInfo() : header(MT_GUILD_CONSTRUCT_SPEED_UP_INFO_SC) {}
	SCGuildConstructTimeStampInfo::SCGuildConstructTimeStampInfo() : header(MT_GUILD_CONSTRUCT_BUILDING_FINISH_TIMESTAMP) {}
	SCGuildConstructOpenSUPInfo::SCGuildConstructOpenSUPInfo() : header(MT_GUILD_CONSTRUCT_OPEN_SUP_INFO_SC) {}
	SCGuildMemberInfoChange::SCGuildMemberInfoChange() : header(MT_GUILD_MEMBER_INFO_CHANGE_SC) {}

	// 家族任务
	SCGuildWeekTaskAllInfor::SCGuildWeekTaskAllInfor() : header(MT_GUILD_WEEK_TASK_ALL_INFOR_CS) {}
	SCGuildWeekTaskRankList::SCGuildWeekTaskRankList() : header(MT_GUILD_WEEK_TASK_RANK_LIST_CS) {}

	//好友
	CSFriendReq::CSFriendReq() : header(MT_FRIEND_REQ_CS) {}
	SCFriendInfo::SCFriendInfo() : header(MT_FRIEND_INFO_SC) {}
	SCChangeFriend::SCChangeFriend() : header(MT_FRIEND_LIST_CHANGE_SC) {}
	SCAddFriendRoute::SCAddFriendRoute() : header(MT_ADD_FRIEND_ROUTE_SC) {}
	SCAskAddFriendList::SCAskAddFriendList() : header(MT_ASK_ADD_FRIEND_LIST_SC) {}
	SCChangeBlack::SCChangeBlack() : header(MT_BLACK_LIST_CHANGE_SC) {}
	SCBlackInfo::SCBlackInfo() : header(MT_BLACK_INFO_SC) {}
	CSFriendEditGruop::CSFriendEditGruop() : header(MT_FRIEND_EDIT_GRUOP_CS) {}
	CSFriendMoveGruop::CSFriendMoveGruop() : header(MT_FRIEND_MOVE_GRUOP_CS) {}
	CSFriendRenameGruop::CSFriendRenameGruop() : header(MT_FRIEND_RENAME_GRUOP_CS) {}
	SCFriendGroupInfo::SCFriendGroupInfo() : header(MT_FRIEND_GRUOP_INFO_SC) {}
	SCRecentlyTeamInfo::SCRecentlyTeamInfo() : header(MT_RECENTLY_TEAM_INFO_SC) {}
	SCOneRecentlyTeamInfo::SCOneRecentlyTeamInfo() : header(MT_ONE_RECENTLY_TEAM_INFO_SC) {}
	SCRecentlyChatChange::SCRecentlyChatChange() : header(MT_RECENTLY_CHAT_CHANGE_SC) {}
	SCRecentlyChatInfo::SCRecentlyChatInfo() : header(MT_RECENTLY_CHAT_INFO_SC) {}
	SCReplyRoleIsOnline::SCReplyRoleIsOnline() : header(MT_REPLY_ROLE_IS_ONLINE_SC) {}

	CSGuildPetTrainingUp::CSGuildPetTrainingUp() : header(MT_GUILD_PET_TRAINING_UP_CS) {}
	CSGuildPetTraining::CSGuildPetTraining() : header(MT_GUILD_PET_TRAINING_CS) {}
	CSGetGuildTrainingInfo::CSGetGuildTrainingInfo() : header(MT_GET_GUILD_TRAINING_INFO_CS) {}
	SCSendGuildTrainingInfo::SCSendGuildTrainingInfo() : header(MT_GUILD_SEND_TRAINING_INFO_SC) {}


	CSFriendRecommendReq::CSFriendRecommendReq() : header(MT_RECOMMEND_FRIEND_LIST_REQ_CS) {}
	SCFriendRecommendRoute::SCFriendRecommendRoute() : header(MT_RECOMMEND_FRIEND_LIST_SC) {}
	CSFriendRecommendAddOnListReq::CSFriendRecommendAddOnListReq() : header(MT_ADD_RECOMMEND_FRIEND_CS) {}
	SCFriendRecommendOne::SCFriendRecommendOne() : header(MT_RECOMMEND_FRIEND_ONE_SC) {}
	CSRefreshFriendRecommend::CSRefreshFriendRecommend() : header(MT_REFRESH_RECOMMEND_FRIEND_CS) {}

	CSFuzzySearchRoleByName::CSFuzzySearchRoleByName() : header(MT_FUZZY_SEARCH_ROLE_INFO_CS) {}
	SCFuzzySearchRolesInfo::SCFuzzySearchRolesInfo() : header(MT_FUZZY_SEARCH_ROLE_INFO_SC) {}
	SCGiveFlowerNotice::SCGiveFlowerNotice() : header(MT_GIVE_FLOWER_NOTICE) {}

	// 灵骑系统
	CSSmartMountsSystemReq::CSSmartMountsSystemReq() : header(MT_SMART_MOUNTS_SYSTEM_REQ_CS) {}
	SCSmartMountsSystemAllInfo::SCSmartMountsSystemAllInfo() : header(MT_SMART_MOUNTS_SYSTEM_ALL_INFO_SC) {}
	SCSmartMountsSystemSingleInfo::SCSmartMountsSystemSingleInfo() : header(MT_SMART_MOUNTS_SYSTEM_SINGLE_INFO_SC) {}
	SCSmartMountsSystemSingleAttr::SCSmartMountsSystemSingleAttr() : header(MT_SMART_MOUNTS_SYSTEM_ATTR_LIST_SC) {}
	SCSmartMountsSystemDominanceInfo::SCSmartMountsSystemDominanceInfo() : header(MT_SMART_MOUNTS_SYSTEM_DOMINANCE_INFO_SC) {}
	SCSmartMountsSystemShowInfo::SCSmartMountsSystemShowInfo() : header(MT_SMART_MOUNTS_SYSTEM_SHOW_INFO_SC) {}
	SCSmartMountsSystemRefineSkillInfo::SCSmartMountsSystemRefineSkillInfo() : header(MT_SMART_MOUNTS_SYSTEM_REFINE_SKILL_SC) {}
	SCSmartMountsSystemOtherInfo::SCSmartMountsSystemOtherInfo() : header(MT_SMART_MOUNTS_SYSTEM_OTHER_INFO_SC) {}
	SCSmartMountsSystemRelatedInfo::SCSmartMountsSystemRelatedInfo() : header(MT_SMART_MOUNTS_SYSTEM_RELATED_INFO_SC) {}
	CSSmartMountsSystemSkillReq::CSSmartMountsSystemSkillReq() : header(MT_SMART_MOUNTS_SYSTEM_SKILL_REQ_CS) {}

	//师徒
	CSShiTuMakeRelationReq::CSShiTuMakeRelationReq() : header(MT_SHITU_OPERA_MAKE_RELATION_REQ_CS) {}
	CSShiTuMakeRelationAck::CSShiTuMakeRelationAck() : header(MT_SHITU_OPERA_MAKE_RELATION_AKC_CS) {}
	CSShiTuBreakRelationReq::CSShiTuBreakRelationReq() : header(MT_SHITU_OPERA_BREAK_RELATION_REQ_CS) {}
	SCShiTuMakeRelationRoute::SCShiTuMakeRelationRoute() : header(MT_SHITU_OPERA_MAKE_RELATION_ROUTE_SC) {}
	SCShiTuRelationInfo::SCShiTuRelationInfo() : header(MT_SHITU_OPERA_RELATION_DATA_SC) {}
	SCShiTuBreakRelationRoute::SCShiTuBreakRelationRoute() : header(MT_SHITU_OPERA_BREAK_RELATION_ROUTE_SC) {}
	CSShiTuBreakRelationAck::CSShiTuBreakRelationAck() : header(MT_SHITU_OPERA_BREAK_RELATION_AKC_CS) {}
	CSShiTuReq::CSShiTuReq() : header(MT_SHITU_OPERA_REQ_CS) {}
	SCShiTuFbSceneInfo::SCShiTuFbSceneInfo() : header(MT_SHITU_FB_SCENE_INFO_SC) {}
	SCShiTuFbCommonInfo::SCShiTuFbCommonInfo() : header(MT_SHITU_COMMON_INFO_SC) {}
	SCShiTuFbInvitationNotice::SCShiTuFbInvitationNotice() : header(MT_SHITU_INVITATION_NOTICE_SC) {}
	SCShiTuFbFinishNotice::SCShiTuFbFinishNotice() : header(MT_SHITU_FINISH_NOTICE_SC) {}
	SCShiTuFbBattleResult::SCShiTuFbBattleResult() : header(MT_SHITU_BATTLE_RESULT_SC) {}

	SCShiTuPreferencesInfo::SCShiTuPreferencesInfo() : header(MT_SHITU_PREFERENCES_INFO_SC) {}
	CSShiTuSeekingReq::CSShiTuSeekingReq() : header(MT_SHITU_SEEKING_REQ_CS) {}
	SCShiTuSeekingInfo::SCShiTuSeekingInfo() : header(MT_SHITU_SEEKING_INFO_SC) {}

	CSServerFirstKillReq::CSServerFirstKillReq() : header(MT_SERVER_FIRST_KILL_REQ_CS) {}
	SCServerFirstKillInfo::SCServerFirstKillInfo() : header(MT_SERVER_FIRST_KILL_AKC_SC) {}

	// 邮箱
	SCMailSendAck::SCMailSendAck() : header(MT_GLOBAL_MAIL_SEND_RESULT_SC) {}
	SCMailDeleteAck::SCMailDeleteAck() : header(MT_GLOBAL_MAIL_DELETE_SC) {}
	SCMailLockAck::SCMailLockAck() : header(MT_GLOBAL_MAIL_LOCK_SC) {}
	SCMailUnlockAck::SCMailUnlockAck() : header(MT_GLOBAL_MAIL_UNLOCK_SC) {}
	SCMailListAck::SCMailListAck() : header(MT_GLOBAL_MAIL_LIST_SC) {}
	SCMailDetailAck::SCMailDetailAck() : header(MT_GLOBAL_MAIL_DETAIL_SC) {}
	SCFetchAttachmentAck::SCFetchAttachmentAck() : header(MT_GLOBAL_MAIL_ATTACHMENT_SC) {}
	SCRecvNewMail::SCRecvNewMail() : header(MT_GLOBAL_MAIL_NEW_MAIL_NOTICE_SC) {}
	SCHasUnReadMail::SCHasUnReadMail() : header(MT_GLOBAL_MAIL_HAS_UNREAD_MAIL_SC) {}
	CSMailSend::CSMailSend() : header(MT_MAIL_SEND_CS) {}
	CSMailDelete::CSMailDelete() : header(MT_MAIL_DELETE_CS) {}
	CSMailGetList::CSMailGetList() : header(MT_MAIL_GETLIST_CS) {}
	CSMailRead::CSMailRead() : header(MT_MAIL_READ_CS) {}
	CSMailFetchAttachment::CSMailFetchAttachment() : header(MT_MAIL_ATTACHMENT_CS) {}
	CSMailClean::CSMailClean() : header(MT_MAIL_CLEAN_CS) {}
	CSMailOneKeyFetchAttachment::CSMailOneKeyFetchAttachment() : header(MT_MAIL_ONE_KEY_FETCH_ATTACHMENT) {}

	CSCourageChallengeOp::CSCourageChallengeOp() : header(MT_COURAGE_CHALLENGE_REQ_CS) {}
	SCCourageChallengeAllInfo::SCCourageChallengeAllInfo() : header(MT_COURAGE_CHALLENGE_ALL_INFO_SC) {}
	SCCourageChallengeFirstKillInfo::SCCourageChallengeFirstKillInfo() : header(MT_COURAGE_CHALLENGE_FIRST_KILL_INFO_SC) {}
	SCCourageChallenegeSingleInfo::SCCourageChallenegeSingleInfo() : header(MT_COURAGE_CHALLENGE_SINGLE_INFO_SC) {}
	SCCourageChallengeScoreSingleInfo::SCCourageChallengeScoreSingleInfo() : header(MT_COURAGE_CHALLENGE_SCORE_SINGLE_INFO_SC) {}

	CSCourageChallengeRankListReq::CSCourageChallengeRankListReq() : header(MT_RANK_GET_COURAGE_CHALLENGE_REQ_CS) {}
	SCCourageChallengeRankListInfo::SCCourageChallengeRankListInfo() : header(MT_RANK_GET_COURAGE_CHALLENGE_SC) {}

	CSSetBattleAuto::CSSetBattleAuto() : header(MT_SET_BATTLE_AUTO_CS) {}
	CSSetBattleAutoMove::CSSetBattleAutoMove() : header(MT_SET_BATTLE_AUTO_MOVE_CS) {}
	SCBattleAutoInfo::SCBattleAutoInfo() : header(MT_SET_BATTLE_AUTO_INFO_SC) {}

	CSRoleResetName::CSRoleResetName() : header(MT_RESET_NAME_CS) {}
	SCRoleResetName::SCRoleResetName() : header(MT_RESET_NAME_SC) {}

	SCMonsterWaveInfo::SCMonsterWaveInfo() : header(MT_MONSTER_WAVE_INFO_SC) {}
	SCMonsterWaveFinishInfo::SCMonsterWaveFinishInfo() : header(MT_MONSTER_WAVE_FINISH_INFO_SC) {}

	CSEnterFB::CSEnterFB() : header(MT_ENTER_FB_CS) {}
	CSLeaveFB::CSLeaveFB() : header(MT_LEAVE_FB_CS) {}

	//密医的阴谋
	SCMiYiInfo::SCMiYiInfo() : header(MT_MI_YI_INFO_SC) {}
	SCMiYiPoker::SCMiYiPoker() : header(MT_MIYI_POKER_SC) {}
	CSMiYiPokerReq::CSMiYiPokerReq() : header(MT_MIYI_POKER_REQ_CS) {}
	SCMiYiPokerInfo::SCMiYiPokerInfo() : header(MT_MIYI_POKER_INFO_SC) {}
	SCMiYiSceneInfo::SCMiYiSceneInfo() : header(MT_MIYI_SCENE_INFO_SC) {}
	SCMiYiKillPassInfo::SCMiYiKillPassInfo() : header(MT_MIYI_KILL_PASS_INFO_SC) {}
	CSMiYiReq::CSMiYiReq() : header(MT_MIYI_REQ_CS) {}

	//铸灵
	CSZhuLingReq::CSZhuLingReq() : header(MT_ZHU_LING_REQ_CS) {}
	SCZhuLingChangeInfo::SCZhuLingChangeInfo() : header(MT_ZHU_LING_CHANG_INFO_SC) {}
	SCZhuLingDecomposeInfo::SCZhuLingDecomposeInfo() : header(MT_ZHU_LING_DECOMPOSE_INFO_SC) {}
	CSZhuLingDecomposeReq::CSZhuLingDecomposeReq() : header(MT_ZHU_LING_DECOMPOSE_REQ_CS) {}
	CSZhuLingUpLevelReq::CSZhuLingUpLevelReq() : header(MT_ZHU_LING_UP_LEVEL_REQ_CS) {}
	SCZhuLingPutOnSuccNotice::SCZhuLingPutOnSuccNotice() : header(MT_ZHU_LING_PUT_ON_SUCC_NOTICE_SC) {}

	// ---------------------------------- 门派任务 ----------------------------------
	SCSchoolTaskAllInfo::SCSchoolTaskAllInfo() : header(MT_SCHOOL_TASK_ALL_INFO_SC) {}
	SCSchoolTaskSingleInfo::SCSchoolTaskSingleInfo() : header(MT_SCHOOL_TASK_SINGLE_INFO_SC) {}
	CSSchoolTaskReq::CSSchoolTaskReq() : header(MT_SCHOOL_TASK_REQ_CS) {}
	SCBountyTaskTurnTableRewardInfo::SCBountyTaskTurnTableRewardInfo() : header(MT_BOUNTY_TASK_TURN_TABLE_INFO_SC) {}
	CSBountyTaskTurnTableRewardFetch::CSBountyTaskTurnTableRewardFetch() : header(MT_BOUNTY_TASK_TURN_TABLE_FETCH_REWARD_CS) {}
	SCBountyTaskInfo::SCBountyTaskInfo() : header(MT_BOUNTY_TASK_INFO_SC) {}

	CSTaskChosedReq::CSTaskChosedReq() : header(MT_TASK_CHOSED_REQ_CS) {}
	SCTaskChosedInfo::SCTaskChosedInfo() : header(MT_TASK_CHOSED_INFO_SC) {}

	SCTaskChainTurnTableInfo::SCTaskChainTurnTableInfo() : header(MT_TASK_CHAIN_TURN_TABLE_INFO_SC) {}

	// --------------------------------- 真中的研究 ---------------------------------
	CSResearchTaskRequest::CSResearchTaskRequest() : header(MT_RESEARCH_TASK_OPERATOR_CS) {}
	SCResearchTaskMessage::SCResearchTaskMessage() : header(MT_RESEARCH_TASK_ALL_INFO_SC) {}
	CSResearchTaskAutoStart::CSResearchTaskAutoStart() : header(MT_RESEARCH_TASK_AUTO_START_CS) {}

	// --------------------------------- 世界boss （异族boss）---------------------------------
	CSWorldBossReq::CSWorldBossReq() : header(MT_WROLD_BOSS_REQ_CS) {}
	SCWorldBossInfo::SCWorldBossInfo() : header(MT_WROLD_BOSS_INFO_SC) {}
	SCWorldBossResult::SCWorldBossResult() : header(MT_WROLD_BOSS_RESULT_SC) {}
	SCWorldBossPosInfo::SCWorldBossPosInfo() : header(MT_WROLD_BOSS_POS_INFO_SC) {}
	SCWorldBossBattleInfo::SCWorldBossBattleInfo() : header(MT_WROLD_BOSS_BATTLE_INFO_SC) {}
	SCWorldBossRoleInfo::SCWorldBossRoleInfo() : header(MT_WROLD_BOSS_ROLE_INFO_SC) {}
	SCWorldBossRankInfo::SCWorldBossRankInfo() : header(MT_WROLD_BOSS_RANK_INFO_SC) {}

	// --------------------------------- 世界boss2(巨龙boss) ---------------------------------
	CSWorldBoss2Req::CSWorldBoss2Req() : header(MT_WROLD_BOSS_2_REQ_CS) {}
	SCWorldBoss2Info::SCWorldBoss2Info() : header(MT_WROLD_BOSS_2_INFO_SC) {}
	SCWorldBoss2Result::SCWorldBoss2Result() : header(MT_WROLD_BOSS_2_RESULT_SC) {}
	SCWorldBoss2PosInfo::SCWorldBoss2PosInfo() : header(MT_WROLD_BOSS_2_POS_INFO_SC) {}
	SCWorldBoss2BattleInfo::SCWorldBoss2BattleInfo() : header(MT_WROLD_BOSS_2_BATTLE_INFO_SC) {}
	SCWorldBoss2RoleInfo::SCWorldBoss2RoleInfo() : header(MT_WROLD_BOSS_2_ROLE_INFO_SC) {}
	SCWorldBoss2RankInfo::SCWorldBoss2RankInfo() : header(MT_WROLD_BOSS_2_RANK_INFO_SC) {}

	// --------------------------------- 世界boss3 (天神降临）---------------------------------
	CSWorldBoss3Req::CSWorldBoss3Req() : header(MT_WORLD_BOSS_3_REQ_CS) {}
	SCWorldBoss3Info::SCWorldBoss3Info() : header(MT_WORLD_BOSS_3_INFO_SC) {}
	SCWorldBoss3Result::SCWorldBoss3Result() : header(MT_WORLD_BOSS_3_RESULT_SC) {}
	SCWorldBoss3PosInfo::SCWorldBoss3PosInfo() : header(MT_WORLD_BOSS_3_POS_INFO_SC) {}
	SCWorldBoss3BattleInfo::SCWorldBoss3BattleInfo() : header(MT_WORLD_BOSS_3_BATTLE_INFO_SC) {}
	SCWorldBoss3RoleInfo::SCWorldBoss3RoleInfo() : header(MT_WORLD_BOSS_3_ROLE_INFO_SC) {}
	SCWorldBoss3RankInfo::SCWorldBoss3RankInfo() : header(MT_WORLD_BOSS_3_RANK_INFO_SC) {}

	//-----------------------------法宝 -------------------------------------
	SCFabaoInfo::SCFabaoInfo() :header(MT_FABAO_SEND_INFO_SC) {}
	CSFabaoReq::CSFabaoReq() : header(MT_FABAO_REQ_CS) {}
	SCFabaoSignInfo::SCFabaoSignInfo() : header(MT_FABAO_SEND_SIGNL_INFO_SC) {}
	SCFabaoEquipBarInfo::SCFabaoEquipBarInfo() : header(MT_FABAO_SEND_EQUIP_BAR_INFO_SC) {}
	SCFabaoDrawInfo::SCFabaoDrawInfo() : header(MT_FABAO_DRAW_INFO_SC) {}
	SCFabaoDrawRewardInfo::SCFabaoDrawRewardInfo() : header(MT_FABAO_DRAW_REWARD_INFO_SC) {}

	//--------------------------------运镖 -----------------------------------------
	SCEscortAllInfo::SCEscortAllInfo() : header(MT_ESCORT_ALL_INFO_SC) {}
	CSEscortOp::CSEscortOp() : header(MT_ESCORT_REQ_CS) {}
	SCEscortResultNotic::SCEscortResultNotic() : header(MT_ESCORT_RESULT_NOTIC_SC) {}
	SCEscortStatusChangeNotice::SCEscortStatusChangeNotice() : header(MT_ESCORT_STATUS_CHANGE_NOTICE_AREA_SC) {}

	CSTreasureMapReq::CSTreasureMapReq() : header(MT_TREASURE_MAP_REQ_CS) {}
	SCTreasureMapInfo::SCTreasureMapInfo() : header(MT_TREASURE_MAP_INFO_SC) {}
	SCTreasureMapItemOpenResult::SCTreasureMapItemOpenResult() : header(MT_TREASURE_MAP_ITEM_OPEN_RESULT_SC) {}
	SCTreasureMapBatchUsedResult::SCTreasureMapBatchUsedResult() : header(MT_TREASURE_MAP_BATCH_USED_RESULT_SC) {}

	//--------------------------------交易行--------------------------------------------
	CSAddItemOnShelve::CSAddItemOnShelve() : header(MT_ADD_ITEM_ON_SHELVE_CS) {}
	CSRemoveItemOnShelve::CSRemoveItemOnShelve() : header(MT_REMOVE_ITEM_ON_SHELVE_CS) {}
	CSTradeMarketCashOut::CSTradeMarketCashOut() : header(MT_CASH_OUT_CS) {}
	CSTradeMarketUserShelveData::CSTradeMarketUserShelveData() : header(MT_TRADE_MARKET_USER_SHELVE_DATA_CS) {}
	SCTradeMarketUserShelveData::SCTradeMarketUserShelveData() : header(MT_TRADE_MARKET_USER_SHELVE_DATA_SC) {}
	CSTradeMarketBuyItem::CSTradeMarketBuyItem() : header(MT_TRADE_MARKET_BUY_ITEM_CS) {}
	CSRecentTransactionRecords::CSRecentTransactionRecords() : header(MT_RECENT_TRANSACTION_RECORDS_CS) {}
	SCRecentTransactionRecords::SCRecentTransactionRecords() : header(MT_RECENT_TRANSACTION_RECORDS_SC) {}
	CSTradeMarketAdvertise::CSTradeMarketAdvertise() : header(MT_TRADE_MARKET_ADVERTISE_CS) {}
	CSTradeMarketSearchAdvertise::CSTradeMarketSearchAdvertise() : header(MT_TRADE_MARKET_ADVERTISE_CS) {}
	CSTradeMarketGetSellingList::CSTradeMarketGetSellingList() : header(MT_TRADE_MARKET_GET_SELLING_LIST_CS) {}
	SCTradeMarketGetSellingList::SCTradeMarketGetSellingList() : header(MT_TRADE_MARKET_GET_SELLING_LIST_SC) {}
	CSTradeMarketGetRoleTransactionRecord::CSTradeMarketGetRoleTransactionRecord() : header(MT_TRADE_MARKET_ROLE_GET_TRADE_RECORD_CS) {}
	SCTradeMarketGetRoleTransactionRecord::SCTradeMarketGetRoleTransactionRecord() : header(MT_TRADE_MARKET_ROLE_GET_TRADE_RECORD_SC) {}
	CSTradeMarketRedoAddItemOnShelve::CSTradeMarketRedoAddItemOnShelve() : header(MT_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_CS) {}
	CSTradeMarketSearchPetForQuickBuy::CSTradeMarketSearchPetForQuickBuy() : header(MT_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY_CS) {}
	SCTradeMarketSearchPetForQuickBuy::SCTradeMarketSearchPetForQuickBuy() : header(MT_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY_SC) {}
	SCTradeMarketRefreshNotice::SCTradeMarketRefreshNotice() : header(MT_TRADE_MARKET_REFRESH_NOTICE) {}
	CSTradeMarketGetMerchandiseNum::CSTradeMarketGetMerchandiseNum() : header(MT_TRADE_MARKET_GET_MERCHANDISE_NUM_CS) {}
	SCTradeMarketGetMerchandiseNum::SCTradeMarketGetMerchandiseNum() : header(MT_TRADE_MARKET_GET_MERCHANDISE_NUM_SC) {}
	SCTradeMarketCreditInfo::SCTradeMarketCreditInfo() : header(MT_TRADE_MARKET_CREDIT_INFO_SC) {}
	CSTradeMarketSearchMedalByEffectTypeAndColor::CSTradeMarketSearchMedalByEffectTypeAndColor() : header(MT_TRADE_MARKET_SEARCH_MEDAL_BY_SPE_TYPE_AND_COLOR_CS) {}
	CSQueryItemRecommendPrice::CSQueryItemRecommendPrice() : header(MT_TRADE_MARKET_QUERY_ITEM_RECOMMEND_PRICE) {}
	SCQueryItemRecommendPriceResp::SCQueryItemRecommendPriceResp() : header(MT_TRADE_MARKET_QUERY_ITEM_RECOMMEND_PRICE_RESP) {}

	//勋章
	CSMedalReq::CSMedalReq():header(MT_MEDAL_REQ_CS){}
	SCMedalInfo::SCMedalInfo():header(MT_MEDAL_INFO_SC){}
	SCMedalFbSceneInfo::SCMedalFbSceneInfo() : header(MT_MEDAL_FB_SCENE_INFO_SC) {}
	SCMedalFbKillBoss::SCMedalFbKillBoss() : header(MT_MEDAL_FB_KILL_BOSS_SC) {}
	SCMedalDecomposeNotice::SCMedalDecomposeNotice() : header(MT_MEDAL_DECOMPOSE_NOTICE_SC) {}
	CSMedalTaskPlayVideo::CSMedalTaskPlayVideo() : header(MT_MEDAL_TASK_PLAY_VIDEO_CS) {}
	SCLottreyInfo::SCLottreyInfo() : header(MT_MEDAL_LOTTREY_INFO_SC){}
	SCMedalTakeOff::SCMedalTakeOff() : header(MT_MEDAL_TAKE_OFF_SC) {}
	SCMedalUpgradeNotice::SCMedalUpgradeNotice() : header(MT_MEDAL_UPGRADE_NOTICE) {}
	SCMedalSpecialExchangeNotice::SCMedalSpecialExchangeNotice() : header(MT_MEDAL_SPECIAL_EXCHANGE) {}
	CSMedalOneKeyGradeReq::CSMedalOneKeyGradeReq() : header(MT_MEDAL_ONE_KEY_GRADE_CS) {}
	CSMedalUpGradeSpecialReq::CSMedalUpGradeSpecialReq() : header(MT_MEDAL_UP_GRADE_SPECIAL) {}

	CSTradeMarketSearchByItemID::CSTradeMarketSearchByItemID() : header(MT_TRADE_MARKET_SEARCH_BY_ITEM_ID_CS) {}
	CSTradeMarketSearchByItemSearchType::CSTradeMarketSearchByItemSearchType() : header(MT_TRADE_MARKET_SEARCH_BY_ITEM_SUB_TYPE_CS) {}
	SCTradeMarketSearchResult::SCTradeMarketSearchResult() : header(MT_TRADE_MARKET_SEARCH_RESULT_SC) {}
	CSTradeMarketAddFocus::CSTradeMarketAddFocus() : header(MT_TRADE_MARKET_ADD_FOCUS_CS) {}
	CSTradeMarketRemoveFocus::CSTradeMarketRemoveFocus() : header(MT_TRADE_MARKET_REMOVE_FOCUS_CS) {}
	CSTradeMarketGetFocusList::CSTradeMarketGetFocusList() : header(MT_TRADE_MARKET_GET_FOCUS_LIST_CS) {}
	SCTradeMarketGetFocusList::SCTradeMarketGetFocusList() : header(MT_TRADE_MARKET_GET_FOCUS_LIST_SC) {}
	CSTradeMarketGetFocusItemInfo::CSTradeMarketGetFocusItemInfo() : header(MT_TRADE_MARKET_GET_FOCUS_ITEM_INFO_CS) {}
	SCTradeMarketGetFocusItemInfo::SCTradeMarketGetFocusItemInfo() : header(MT_TRADE_MARKET_GET_FOCUS_ITEM_INFO_SC) {}

	//水晶副本
	SCCrystalFbInfo::SCCrystalFbInfo() : header(MT_CRYSTAL_FB_INFO_SC) {}
	
	SCCrystalFbUseItemInfo::SCCrystalFbUseItemInfo() : header(MT_CRYSTAL_FB_USE_ITEM_INFO_SC) {}
	CSCrystalFbReq::CSCrystalFbReq() : header(MT_CRYSTAL_FB_REQ_CS) {}
	SCCrystalFbObjInfo::SCCrystalFbObjInfo() : header(MT_CRYSTAL_FB_OBJ_INFO_SC) {}

	//进阶副本
	SCAdvanceFbObjInfo::SCAdvanceFbObjInfo() : header(MT_ADVANCE_FB_INFO_SC) {}
	SCAdvanceFbTaskInfo::SCAdvanceFbTaskInfo() : header(MT_ADVANCE_FB_TASK_INFO_SC) {}
	SCADVdropRewardInfo::SCADVdropRewardInfo() : header(MT_ADVANCE_FB_DROP_BACK_SC) {}
	SCCrystalFbEnergyInfo::SCCrystalFbEnergyInfo() : header(MT_CRYSTAL_FB_ENERGY_INFO_SC) {}
	CSCrystalFbFastPassInfo::CSCrystalFbFastPassInfo() : header(MT_CRYSTAL_FB_FAST_PASS_CS) {}
	CSCrystalFbAllNpcInfo::CSCrystalFbAllNpcInfo() : header(MT_CRYSTAL_FB_ALL_NPC_SC) {}

	SCMiJingQiWenFbObjInfo::SCMiJingQiWenFbObjInfo() : header(MT_RA_MI_JING_QI_WEN_OBJ_INFO_SC) {}

	//四象幻境副本
	SCFantasyFbSceneInfo::SCFantasyFbSceneInfo() : header(MT_FANTASY_FB_SCENE_INFO_SC) {}
	SCFantasyFbObjInfo::SCFantasyFbObjInfo() : header(MT_FANTASY_FB_INFO_SC) {}
	SCFantasyFBRewardInfo::SCFantasyFBRewardInfo() : header(MT_FANTASY_FB_REWARD_INFO_SC) {}

	//野外boss
	SCWildBossNotice::SCWildBossNotice() : header(MT_WILD_BOSS_NOTICE_SC) {}
	SCWildBossInfo::SCWildBossInfo() : header(MT_WILD_BOSS_INFO_SC) {}
	CSWildBossReq::CSWildBossReq() : header(MT_WILD_BOSS_REQ_CS) {}
	SCWildBossPosiInfo::SCWildBossPosiInfo() : header(MT_WILD_BOSS_POSI_INFO_SC) {}
	SCWildBossPokerInfo::SCWildBossPokerInfo() : header(MT_WILD_BOSS_POKER_SC){}
	CSWildBossPokerReq::CSWildBossPokerReq() :header(MT_WILD_BOSS_POKER_CS){}
	SCWildBossSingleInfo::SCWildBossSingleInfo() : header(MT_WILD_BOSS_SINGLE_SC) {}

	//buff
	SCBuffInfo::SCBuffInfo():header(MT_BUFF_INFO_SC){}

	//shop
	SCShopAllInfo::SCShopAllInfo() : header(MT_SHOP_ALL_INFO_SC) {}
	SCShopSingleInfo::SCShopSingleInfo() : header(MT_SHOP_SINGLE_INFO_SC) {}
	CSShopBuyReq::CSShopBuyReq() : header(MT_SHOP_BUY_REQ_CS) {}
	CSFastPurchase::CSFastPurchase() : header(MT_SHOP_FAST_BUY_CS) {}
	CSGoldShopBuyReq::CSGoldShopBuyReq() : header(MT_SHOP_GOLD_SHOP_BUY_CS) {}
	SCGoldShopAllInfo::SCGoldShopAllInfo() : header(MT_SHOP_GOLD_SHOP_ALL_INFO_SC) {}
	SCGoldShopSingleInfo::SCGoldShopSingleInfo() : header(MT_SHOP_GOLD_SHOP_SINGLE_INFO_SC) {}
	CSExchangeReq::CSExchangeReq() : header(MT_EXCHANGE_REQ_CS) {}
	SCExchangeAllInfo::SCExchangeAllInfo() : header(MT_EXCHANGE_ALL_INFO_SC) {}
	SCExchangeSingleInfo::SCExchangeSingleInfo() : header(MT_EXCHANGE_SINGLE_INFO_SC) {}
	CSGoldShopNotifyCancelReq::CSGoldShopNotifyCancelReq() : header(MT_CANCEL_NOTIFY_REQUEST_CS) {}
	SCGoldShopPurchaseNotifys::SCGoldShopPurchaseNotifys() : header(MT_PURCHASE_NOTIFY_INFOR_SC) {}

	//market
	CSMarketReq::CSMarketReq() : header(MT_MARKET_REQ_CS) {}
	SCMarketAllInfo::SCMarketAllInfo() : header(MT_MARKET_ALL_INFO_SC) {}
	SCMarketSingleInfo::SCMarketSingleInfo() : header(MT_MARKET_SINGLE_INFO_SC) {}


	// 地图解锁
	SCMapUnlockAllListInfo::SCMapUnlockAllListInfo() : header(MT_MAP_UNLOCK_ENTIRE_ACTIVE_FLAG_LISTS_SC) {}
	SCMapUnlockFristNotice::SCMapUnlockFristNotice() : header(MT_MAP_UNLOCK_FIRST_ENTER_SCENE_NOTICE_SC) {}
	CSMapUnlockRemoveAnima::CSMapUnlockRemoveAnima() : header(MT_MAP_UNLOCK_REMOVE_UNLOCK_ANIMATIONS_CS) {}
	SCMapUnlockFirstEnterFbType::SCMapUnlockFirstEnterFbType() : header(MT_MAP_UNLOCK_FIRST_ENTER_FB_TYPE_CS) {}

	CSXunBaoReq::CSXunBaoReq() : header(MT_XUN_BAO_REQ_CS) {}
	SCXunBaoRet::SCXunBaoRet() : header(MT_XUN_BAO_RET_SC) {}
	SCXunBaoOpenInfo::SCXunBaoOpenInfo() : header(MT_XUN_BAO_OPEN_INFO_SC) {}
	SCXunBaoDrawTimesInfo::SCXunBaoDrawTimesInfo() : header(MT_XUN_BAO_DRAW_TIMES_INFO_SC) {}
	CSXunBaoExchange::CSXunBaoExchange() : header(MT_XUN_BAO_EXCHANGE_CS) {}
	CSXunBaoBelssReq::CSXunBaoBelssReq() : header(MT_XUN_BAO_BELSS_CS) {}
	SCXunBaoBelssInfo::SCXunBaoBelssInfo() : header(MT_XUN_BAO_BELSS_INFO_SC) {}

	//轮回忘川
	CSLunHuiWangChuanReq::CSLunHuiWangChuanReq() : header(MT_LUN_HUI_WANG_CHUAN_REQ_CS) {}
	SCLunHuiWangChuanInfo::SCLunHuiWangChuanInfo() : header(MT_LUN_HUI_WANG_CHUAN_INFO_SC) {}
	
	//幽冥炼狱
	SCNetherWorldPurgatoryInfo::SCNetherWorldPurgatoryInfo() : header(MT_NETHER_WORLD_PURGATORY_INFO_SC) {}
	CSNetherWorldPurgatoryReq::CSNetherWorldPurgatoryReq() : header(MT_NETHER_WORLD_PURGATORY_REQ_CS) {}
	SCNetherWorldPurgatorySigleInfo::SCNetherWorldPurgatorySigleInfo() : header(MT_NETHER_WORLD_PURGATORY_SIGLE_INFO_SC) {}
	SCNetherWorldPurgatoryMiLingInfo::SCNetherWorldPurgatoryMiLingInfo() : header(MT_NETHER_WORLD_PURGATORY_MI_LING_INFO_SC) {}
	SCNetherWorldPurgatoryChapterInfo::SCNetherWorldPurgatoryChapterInfo() : header(MT_NETHER_WORLD_PURGATORY_SIGLE_CAPHTER_SC) {}

	// 采集副本
	SCGatherFbInfo::SCGatherFbInfo() : header(MT_GATHER_FB_INFO_SC) {}
	SCGatherFbGatherListInfo::SCGatherFbGatherListInfo() : header(MT_GATHER_FB_GATHER_LIST_SC) {}

	// 排行榜
	CSGetPersonRankList::CSGetPersonRankList() : header(MT_RANK_GET_PERSONRANK_LIST_CS) {}
	SCGetPersonRankListAck::SCGetPersonRankListAck() : header(MT_RANK_GET_PERSONRANK_LIST_ACK_SC) {}
	SCGetPetRankListAck::SCGetPetRankListAck() : header(MT_RANK_GET_PETRANK_LIST_ACK_SC) {}
	CSGetGuildRankList::CSGetGuildRankList() : header(MT_RANK_GET_GUILDRANK_LIST_CS) {}
	SCGetGuildRankListAck::SCGetGuildRankListAck() : header(MT_RANK_GET_GUILDRANK_LIST_ACK_SC) {}
	CSGetPetRankList::CSGetPetRankList() : header(MT_RANK_GET_PETRANK_LIST_CS) {}
	CSGetTeamRankList::CSGetTeamRankList() : header(MT_RANK_GET_TEAMRANK_LIST_CS) {}
	SCGetTeamRankListAck::SCGetTeamRankListAck() : header(MT_RANK_GET_TEAMRANK_LIST_ACK_SC) {}
	CSGetRankRefreshTime::CSGetRankRefreshTime() : header(MT_RANK_GET_REFRESH_TIMESTAMP_CS) {}
	SCRankRefreshTime::SCRankRefreshTime() : header(MT_RANK_GET_REFRESH_TIMESTAMP_SC) {}

	CSGetPersonRankTopUser::CSGetPersonRankTopUser() : header(MT_RANK_GET_PERSONRANK_TOP_USER_CS) {}
	SCGetPersonRankTopUser::SCGetPersonRankTopUser() : header(MT_RANK_GET_PERSONRANK_TOP_USER_SC) {}
	SCGetWorldLevelAck::SCGetWorldLevelAck() : header(MT_RANK_GET_WORLD_LEVEL_ACK_SC) {}

	CSGetRankPetDetailedInfo::CSGetRankPetDetailedInfo() : header(MT_RANK_GET_PETRANK_DETAILED_INFO_CS) {}

	//竞技场
	CSColosseumReq::CSColosseumReq() : header(MT_COLOSSEUM_REQ_CS) {}
	SCColosseumRoleInfo::SCColosseumRoleInfo() : header(MT_COLOSSEUM_ROLE_INFO_SC) {}
	SCColosseumTopInfo::SCColosseumTopInfo() : header(MT_COLOSSEUM_TOP_INFO_SC) {}
	SCColosseumTargetInfo::SCColosseumTargetInfo() : header(MT_COLOSSEUM_TARGET_INFO_SC) {}
	SCColosseumRankInfo::SCColosseumRankInfo() : header(MT_COLOSSEUM_RANK_INFO_SC) {}
	SCColosseumReportInfo::SCColosseumReportInfo() : header(MT_COLOSSEUM_REPORT_INFO_SC) {}
	SCColosseumRankRewardInfo::SCColosseumRankRewardInfo() : header(MT_COLOSSEUM_RANK_REWARD_SC) {}
	SCColosseumPvPsResultInfo::SCColosseumPvPsResultInfo() : header(MT_COLOSSEUM_PVPS_RESULT_SC) {}
	SCColosseumOpenStatusInfo::SCColosseumOpenStatusInfo() : header(MT_COLOSSEUM_OPEN_STATUS_SC) {}
	SCColosseumInPoolListInfo::SCColosseumInPoolListInfo() : header(MT_COLOSSEUM_INPOOL_LIST_SC) {}
	SCColosseumNewReport::SCColosseumNewReport() : header(MT_COLOSSEUM_NEW_REPORT_SC) {}
	SCColosseumEveryDayRewardInfo::SCColosseumEveryDayRewardInfo() : header(MT_COLOSSEUM_NEW_DAY_REWARD_SC) {}
	SCColosseumNewReportDetail::SCColosseumNewReportDetail() : header(MT_COLOSSEUM_NEW_REPORT_DETAIL_SC) {}
	SCColosseumClientRefreshTargetNotice::SCColosseumClientRefreshTargetNotice() : header(MT_COLOSSEUM_CLIENT_REFRESH_TARGET_NOTICE_SC) {}
	SCColosseumAchievementInfo::SCColosseumAchievementInfo() : header(MT_COLOSSEUM_ACHIEVEMENT_INFO_SC) {}
	SCColosseumDetalInfo::SCColosseumDetalInfo() : header(MT_COLOSSEUM_DETAL_INFO_SC) {}
	SCHistoryHighestRankChangeNotice::SCHistoryHighestRankChangeNotice() : header(MT_COLOSSEUM_HISTORY_RANK_CHANGE_NOTICE) {}
	SCSweepResultNotice::SCSweepResultNotice() : header(MT_COLOSSEUM_SWEEP_RESULT_INFO) {}
	SCBattleRewardNotice::SCBattleRewardNotice() : header(MT_COLOSSEUM_BATTLE_REWARD_NOTICE) {}

	// 勇闯地宫
	CSBraveGroundReq::CSBraveGroundReq() : header(MT_BRAVE_GROUND_REQ_CS) {}
	SCBraveGroundRankInfo::SCBraveGroundRankInfo() : header(MT_BRAVE_GROUND_RANK_SC) {}
	SCBraveGroundSceneInfo::SCBraveGroundSceneInfo() : header(MT_BRAVE_GROUND_SCENE_SC) {}
	SCBraveGroundLevelRewardInfo::SCBraveGroundLevelRewardInfo() : header(MT_BRAVE_GROUND_REWARD_INFO_SC) {}
	SCBraveGroundInfo::SCBraveGroundInfo() : header(MT_BRAVE_GROUND_INFO_SC) {}
	SCBraveGroundRoute::SCBraveGroundRoute() : header(MT_BRAVE_GROUND_ROUTE_SC) {}
	SCBraveGroundNotify::SCBraveGroundNotify() : header(MT_BRAVE_GROUND_NOTIFY_SC) {}
	SCBraveGroundUseItemInfo::SCBraveGroundUseItemInfo() : header(MT_BRAVE_GROUND_USE_ITEM_INFO_SC) {}
	SCBraveGroundTeamInfo::SCBraveGroundTeamInfo() : header(MT_BRAVE_GROUND_TEAM_INFO_SC) {}
	SCBraveGroundRoleInfo::SCBraveGroundRoleInfo() : header(MT_BRAVE_GROUND_ROLE_INFO_SC) {}
	SCBraveGroundCommonInfo::SCBraveGroundCommonInfo() : header(MT_BRAVE_GROUND_COMMON_INFO_SC) {}

	// 随机活动
	CSRandActivityOperaReq::CSRandActivityOperaReq() : header(MT_RAND_ACTIVITY_OP_CS) {}
	SCRandActivityTimeLimitStoreInfo::SCRandActivityTimeLimitStoreInfo() : header(MT_RAND_ACTIVITY_TIME_LIMIT_STORE_INFO) {}
	SCRandActivityTurnTableInfo::SCRandActivityTurnTableInfo() : header(MT_RAND_ACTIVITY_TURN_TABLE_INFO) {}
	SCRandActivityTurnTableReward::SCRandActivityTurnTableReward() : header(MT_RAND_ACTIVITY_TURN_TABLE_REWARD) {}
	SCRandActivityDiscountStoreInfo::SCRandActivityDiscountStoreInfo() : header(MT_RAND_ACTIVITY_DISCOUNT_STORE_INFO) {}
	SCRAConsumeCarouselInfo::SCRAConsumeCarouselInfo() : header(MT_RAND_ACTIVITY_CONSUME_CAROUSEL_INFO) {}
	SCRAConsumeCarouselReward::SCRAConsumeCarouselReward() : header(MT_RAND_ACTIVITY_CONSUME_CAROUSEL_REWARD) {}
	SCRAConsumeCarouselShow::SCRAConsumeCarouselShow() : header(MT_RAND_ACTIVITY_CONSUME_CAROUSEL_SHOW) {}
	SCRAFirstChongZhiInfo::SCRAFirstChongZhiInfo() : header(MT_FIRST_CHONGZHI_ACTIVITY_YIXIANER) {}
	SCRAFirstChongZhiReq::SCRAFirstChongZhiReq() : header(MT_FIRST_CHONGZHI_ACTIVITY_REQ_CS) {}
	SCRAHeritageTreasureInfo::SCRAHeritageTreasureInfo() : header(MT_RAND_ACTIVITY_HERITAGE_TREASURE_INFO_CS) {}
	SCRASevenColorPictureInfo::SCRASevenColorPictureInfo() : header(MT_RAND_ACTIVITY_SEVEN_COLOR_PICTURE) {}
	SCRAExpelMonsterInfo::SCRAExpelMonsterInfo() : header(MT_RAND_ACTIVITY_EXPEL_MONSTER_INFO_SC) {}
	SCRASaleShopInfo::SCRASaleShopInfo() : header(MT_RAND_ACTIVITY_SALE_SHOP_INFO_SC) {}
	SCRAHappyUpInfo::SCRAHappyUpInfo() : header(MT_RAND_ACTIVITY_HAPPY_UP_INFO_SC) {}
	SCRAAnimalRaceInfo::SCRAAnimalRaceInfo() : header(MT_RAND_ACTIVITY_ANIMAL_RACE_INFO_SC) {}
	SCRAAnimalRaceRoleInfo::SCRAAnimalRaceRoleInfo() : header(MT_RAND_ACTIVITY_ANIMAL_RACE_ROLE_SC) {}
	SCRALuckyBagInfo::SCRALuckyBagInfo() : header(MT_RAND_ACTIVITY_LUCKY_BAG_INFO_SC) {}
	CSRASevenColorReq::CSRASevenColorReq() : header(MT_RAND_ACTIVITY_SEVEN_COLOR_REQ_CS) {}
	SCRASevenColorTypeInfo::SCRASevenColorTypeInfo() : header(MT_RAND_ACTIVITY_SEVEN_COLOR_TYPE_CS) {}
	SCRATianJiangHaoLiInfo::SCRATianJiangHaoLiInfo() : header(MT_RAND_ACTIVITY_TIAN_JIANG_HAO_LI_INFO_CS) {}
	SCRALaoDongHaoLiInfo::SCRALaoDongHaoLiInfo() : header(MT_RAND_ACTIVITY_LAO_DONG_HAO_LI_INFO_SC) {}
	SCRASummerLiLianAllInfo::SCRASummerLiLianAllInfo() : header(MT_RAND_ACTIVITY_SUMMER_LI_LIAN_ALL_INFO_SC) {}
	SCRAColorfulLanternAllInfo::SCRAColorfulLanternAllInfo() : header(MT_RAND_ACTIVITY_COLORFUL_LANTERN_ALL_INFO_SC) {}
	SCRAColorfulLanternGameInfo::SCRAColorfulLanternGameInfo() : header(MT_RAND_ACTIVITY_COLORFUL_LANTERN_GAME_INFO_SC) {}
	SCRAColorfulLanternLightInfo::SCRAColorfulLanternLightInfo() : header(MT_RAND_ACTIVITY_COLORFUL_LANTERN_LIGHT_INFO_SC) {}
	SCRASummerStoreInfo::SCRASummerStoreInfo() : header(MT_RAND_ACTIVITY_SUMMER_STORE_INFO_SC) {}
	SCRALuckySignInfo::SCRALuckySignInfo() : header(MT_RAND_ACTIVITY_LUCKY_SIGN_INFO_SC) {}
	SCRASuperOffShopInfo::SCRASuperOffShopInfo() : header(MT_RAND_ACTIVITY_SUPER_OFF_SHOP_SC) {}
	SCRASummerStoreSingleInfo::SCRASummerStoreSingleInfo() : header(MT_RAND_ACTIVITY_SUMMER_STORE_SINGLE_INFO_SC) {}
	SCRASuperOffShopSingleInfo::SCRASuperOffShopSingleInfo() : header(MT_RAND_ACTIVITY_SUPER_OFF_SHOP_SINGLE_INFO_SC) {}
	SCRACoolBreezeInfo::SCRACoolBreezeInfo() : header(MT_RAND_ACTIVITY_COOL_BREEZE_INFO_SC) {}
	SCRACoolBreezeSingleInfo::SCRACoolBreezeSingleInfo() : header(MT_RAND_ACTIVITY_COOL_BREEZE_SINGLE_INFO_SC) {}
	SCRALotusMiBaoInfo::SCRALotusMiBaoInfo() : header(MT_RAND_ACTIVITY_LOTUS_MI_BAO_INFO_SC) {}
	SCRAMiBaoFangDianInfo::SCRAMiBaoFangDianInfo() : header(MT_RAND_ACTIVITY_MI_BAO_FANG_DIAN_INFO_SC) {}
	SCRAMiBaoFangDianSingleInfo::SCRAMiBaoFangDianSingleInfo() : header(MT_RAND_ACTIVITY_MI_BAO_FANG_DIAN_SINGLE_INFO_SC) {}
	SCRAShanHaiQuestionInfo::SCRAShanHaiQuestionInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_QUESTION){}
	SCRAShanHaiQuestionStatusInfo::SCRAShanHaiQuestionStatusInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_QUESTION_STATUS) {}
	SCRAYunZeWishInfo::SCRAYunZeWishInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_WISH_INFO_SC){}
	SCRAYunZeHaoLiInfo::SCRAYunZeHaoLiInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_HAO_LI_INFO_SC) {}
	SCRAYunZeCutePetInfo::SCRAYunZeCutePetInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_CUTE_PET_INFO_SC) {}
	SCRAYunZeCatchElvesGetInfo::SCRAYunZeCatchElvesGetInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_CATCH_ELVES_GET_INFO_SC){}
	SCRAYunZeTerritoryInfo::SCRAYunZeTerritoryInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_TERRITORY_INFO_SC) {}
	SCRAYunZeBloomingRecordInfo::SCRAYunZeBloomingRecordInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_BLOOMING_RECORD_INFO_SC) {}
	SCRAYunZeSlotMachineInfo::SCRAYunZeSlotMachineInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_INFO_SC) {}
	SCRAYunZeSlotMachineResultInfo::SCRAYunZeSlotMachineResultInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_RESULT_INFO_SC){}
	SCRAYunZeErSanShiInfo::SCRAYunZeErSanShiInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_INFO_SC) {}
	SCRAShanHaiZhengFengAllInfo::SCRAShanHaiZhengFengAllInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_ZHENG_FENG_ALL_INFO_SC) {}
	SCRAShanHaiMiBaoInfo::SCRAShanHaiMiBaoInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_MI_BAO_INFO_SC) {}
	SCRAYunZeErSanShiTurntableInfo::SCRAYunZeErSanShiTurntableInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_TURNTABLE_INFO_SC) {}
	SCRAYunZeCutePetChallengeInfo::SCRAYunZeCutePetChallengeInfo() : header(MT_RAND_ACTIVITY_YUN_ZE_CUTE_PET_CHALLENGE_INFO_SC) {}
	SCRATigerSpiciousSnowInfo::SCRATigerSpiciousSnowInfo() : header(MT_RAND_ACTIVITY_TIGERAUSPICIOUS_SNOW_INFO_SC) {}
	SCRATigerFishEveryYearInfo::SCRATigerFishEveryYearInfo() :header(MT_RAND_ACTIVITY_TIGER_FISH_EVERY_YEAR_INFO_SC){}
	SCRATigerFindSpringInfo::SCRATigerFindSpringInfo() : header(MT_RAND_ACTIVITY_TIGER_FIND_SPRING_INFO_SC) {}
	SCRATigerTeaseNewYearInfo::SCRATigerTeaseNewYearInfo() : header(MT_RAND_ACTIVITY_TIGER_TEASE_NEWYEAR_INFO_SC) {}
	SCRATigerLuckyMoneyInfo::SCRATigerLuckyMoneyInfo() : header(MT_RAND_ACTIVITY_TIGER_LUCKY_MONEY_INFO_SC) {}
	SCRARongLianYouLiInfo::SCRARongLianYouLiInfo() : header(MT_RAND_ACTIVITY_RONG_LIAN_YOU_LI_INFO_SC) {}

	SCRAPaiMaiHangNoticeInfo::SCRAPaiMaiHangNoticeInfo() : header(MT_RAND_ACTIVITY_PAI_MAI_HANG_NOTICE_INFO_SC) {}
	SCRAPaiMaiHangSingleNoticeInfo::SCRAPaiMaiHangSingleNoticeInfo() : header(MT_RAND_ACTIVITY_PAI_MAI_HANG_SINGLE_NOTICE_SC) {}
	SCRAPaiMaiHangUserInfo::SCRAPaiMaiHangUserInfo() : header(MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_INFO_SC) {}
	SCRAPaiMaiHangUserSingleInfo::SCRAPaiMaiHangUserSingleInfo() : header(MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_SINGLE_INFO_SC) {}
	SCRAPaiMaiHangUserRecordInfo::SCRAPaiMaiHangUserRecordInfo() : header(MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_RECORD_INFO_SC) {}

	SCRAYuanQiSpeedInfo::SCRAYuanQiSpeedInfo() : header(MT_RAND_ACTIVITY_YUAN_QI_SPEED_INFO_SC) {}
	SCRAYuanQiSpeedPlayInfo::SCRAYuanQiSpeedPlayInfo() : header(MT_RAND_ACTIVITY_YUAN_QI_SPEED_PLAY_INFO_SC) {}

	SCRAYuanQiHelpInfo::SCRAYuanQiHelpInfo() : header(MT_RAND_ACTIVITY_YUAN_QI_HELP_INFO_SC) {}

	//全服比拼
	SCRAServerCompetitionRoleAllInfo::SCRAServerCompetitionRoleAllInfo() : header(MT_RAND_ACTIVITY_SERVER_COMPETITION_ROLE_ALL_INFO) {}
	SCRAServerCompetitionSingleRoleInfo::SCRAServerCompetitionSingleRoleInfo() : header(MT_RAND_ACTIVITY_SERVER_COMPETITION_SINGLE_INFO) {}
	SCRAServerCompetitionActivityInfo::SCRAServerCompetitionActivityInfo() : header(MT_RAND_ACTIVITY_SERVER_COMPETITION_ACTIVITY_INFO) {}
	SCRAServerCompetitionOtherInfo::SCRAServerCompetitionOtherInfo() : header(MT_RAND_ACTIVITY_SERVER_COMPETITION_OTHER_INFO_SC) {}
	SCRAShanHaiMeetInfo::SCRAShanHaiMeetInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_MEET_INFO) {}
	SCRAServerCompetitionAllRankInfo::SCRAServerCompetitionAllRankInfo() : header(MT_SERVER_COMPETITION_ALL_RANK_INFO_SC) {}

	SCRAJianDanQinxinInfo::SCRAJianDanQinxinInfo() : header(MT_RAND_ACTIVITY_JIAN_DAN_QIN_XIN_INFO_SC) {}
	SCRALinkageGiftInfo::SCRALinkageGiftInfo() : header(MT_RAND_ACTIVITY_LINKAGE_GIFT_INFO_SC) {}
	SCRADiscountCarnivalInfo::SCRADiscountCarnivalInfo() : header(MT_RAND_ACTIVITY_DISCOUNT_CARNIVAL_INFO_SC) {}
	SCRALuckyDiscountInfo::SCRALuckyDiscountInfo() : header(MT_RAND_ACTIVITY_LUCKY_DISCOUNT_INFO_SC) {}
	SCRALuckyTurntableInfo::SCRALuckyTurntableInfo() : header(MT_RAND_ACTIVITY_LUCKY_TURNTABLE_INFO_SC) {}
	SCRALuckyTurntableReward::SCRALuckyTurntableReward() : header(MT_RAND_ACTIVITY_LUCKY_TURNTABLE_REWARD_SC) {}
	SCRaAdventureGiftInfo::SCRaAdventureGiftInfo() : header(MT_RAND_ACTIVITY_ADVENTURE_GIFT_INFO_SC) {}
	SCRAQuanMinDiscountInfo::SCRAQuanMinDiscountInfo() : header(MT_RAND_ACTIVITY_QUAN_MIN_DISCOUNT_INFO_SC) {}
	SCRAAnimalRaceAnimationInfo::SCRAAnimalRaceAnimationInfo() : header(MT_RAND_ACTIVITY_ANIMAL_RACE_ANIMATION_INFO_SC) {}
	SCRATuJianTreasureInfo::SCRATuJianTreasureInfo() : header(MT_RAND_ACTIVITY_TU_JIAN_TREASURE_INFO_SC) {}
	SCRAGodTokenInfo::SCRAGodTokenInfo() : header(MT_RAND_ACTIVITY_GOD_TOKEN_INFO_SC) {}
	SCRAElementCardsInfo::SCRAElementCardsInfo() : header(MT_RAND_ACTIVITY_ELEMENT_CARDS_INFO_SC) {}
	SCRASignInContinuityInfo::SCRASignInContinuityInfo() : header(MT_RAND_ACTIVITY_SIGN_IN_CONTINUITY_INFO_SC) {}
	SCRAGivePetDrawInfo::SCRAGivePetDrawInfo() : header(MT_RAND_ACTIVITY_GIVE_PET_DRAW_INFO_SC) {}

	CSRandActivityDuiHuanLingYuReq::CSRandActivityDuiHuanLingYuReq() : header(MT_RAND_ACTIVITY_DUI_HUAN_LING_YU_CS) {}
	CSRandActivityBonusOperaReq::CSRandActivityBonusOperaReq() : header(MT_RAND_ACTIVITY_BONUS_OP_CS) {}
	SCRandActivityBonusInfo::SCRandActivityBonusInfo() : header(MT_RAND_ACTIVITY_BONUS_INFO_SC) {}
	SCRATradeMarketReturnGoldInfo::SCRATradeMarketReturnGoldInfo() : header(MT_RAND_ACTIVITY_TRADEMARKET_RETURN_GOLD_INFO_SC) {}
	SCRATradeMarketReturnHotItemInfo::SCRATradeMarketReturnHotItemInfo() : header(MT_RAND_ACTIVITY_TRADEMARKET_RETURN_GOLD_HOT_ITEM_LIST) {}

	//牛年活动
	SCRandActivityCJYXInfo::SCRandActivityCJYXInfo() : header(MT_RAND_ACTIVITY_CI_JIU_YING_XIN_INFO_SC) {}
	SCRandActivityNXDYInfo::SCRandActivityNXDYInfo() : header(MT_RAND_ACTIVITY_NIU_XING_DA_YUN_INFO_SC) {}
	SCRASpringFestivalStoreInfo::SCRASpringFestivalStoreInfo() : header(MT_RAND_ACTIVITY_SPRING_FESTIVAL_STORE_INFO_SC){}
	SCRAMonpolyGiftInfo::SCRAMonpolyGiftInfo() : header(MT_RAND_ACTIVITY_MONPOLY_GIFT_INFO_SC) {}
	SCRAMonpolyGiftExchangeInfo::SCRAMonpolyGiftExchangeInfo() : header(MT_RAND_ACTIVITY_MONPOLY_GIFT_EXCHANGE_INFO_SC) {}
	SCRAMonpolyGiftQuestionInfo::SCRAMonpolyGiftQuestionInfo() : header(MT_RAND_ACTIVITY_MONPOLY_GIFT_QUESTION_INFO_SC) {}
	SCRACowExchangeInfo::SCRACowExchangeInfo() : header(MT_RAND_ACTIVITY_COW_EXCHANGE_INFO_SC) {}
	SCRACowStoreInfo::SCRACowStoreInfo() : header(MT_RAND_ACTIVITY_COW_STORE_INFO_SC) {}
	SCRACowLuckyExchangeInfo::SCRACowLuckyExchangeInfo() : header(MT_RAND_ACTIVITY_COW_LUCKY_EXCHANGE) {}
	SCRALittleYearGiftInfo::SCRALittleYearGiftInfo() : header(MT_RAND_ACTIVITY_LITTLE_YEAR_GIFT) {}

	//宠物夺宝
	SCRAPetLootInfo::SCRAPetLootInfo() : header(MT_RAND_ACTIVITY_PET_LOOT) {}

	CSRandActivityMakeUpReq::CSRandActivityMakeUpReq() : header(MT_RAND_ACTIVITY_MAKE_UP_REWARD_REQ_CS) {}

	SCRAFortuneMazeInfo::SCRAFortuneMazeInfo() : header(MT_RAND_ACTIVITY_FORTUNE_MAZE_SC) {}
	SCRAFortuneMazePinTuInfo::SCRAFortuneMazePinTuInfo() : header(MT_RAND_ACTIVITY_FORTUNE_MAZE_PIN_TU_MAP_SC) {}

	SCRAYuanQiTurningInfo::SCRAYuanQiTurningInfo() : header(MT_RAND_ACTIVITY_YUAN_QI_TURNING_INFO_SC) {}

	SCRAGoodGiftTurnInfo::SCRAGoodGiftTurnInfo() : header(MT_RAND_ACTIVITY_GOOD_GIFT_TURN_INFO_SC) {}
	SCRAGoodGiftTurnRewardInfo::SCRAGoodGiftTurnRewardInfo() : header(MT_RAND_ACTIVITY_GOOD_GIFT_TURN_REWARD_SC) {}
	SCRAHelpOthersInfo::SCRAHelpOthersInfo() : header(MT_RAND_ACTIVITY_HELP_OTHERS_INFO_SC) {}
	SCRAHelpOthersInfo2::SCRAHelpOthersInfo2() : header(MT_RAND_ACTIVITY_HELP_OTHERS_INFO_2_SC) {}
	SCRAFateEntanglementInfo::SCRAFateEntanglementInfo() : header(MT_RAND_ACTIVITY_FATE_ENTANGLEMENT_INFO_SC){}

	SCRASecretTreasureInfo::SCRASecretTreasureInfo() : header(MT_RAND_ACTIVITY_SECRET_TREASURE_INFO) {}

	SCRAJoySeeksFavorsInfo::SCRAJoySeeksFavorsInfo() : header(MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_SC) {}
	SCRAJoySeeksFavorsMatchSucc::SCRAJoySeeksFavorsMatchSucc() : header(MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_MATH_SUCC_SC) {}
	CSRAJoySeeksFavorsMatchReq::CSRAJoySeeksFavorsMatchReq() : header(MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_MATH_REQ_CS) {}
	SCRaCollectWordInfo::SCRaCollectWordInfo() : header(MT_RAND_ACTIVITY_COLLECT_WORD_INFO_SC) {}

	//首席竞选
	CSChiefElectionReq::CSChiefElectionReq() : header(MT_CHIEF_ELECTION_REQ_CS) {}
	SCChiefElectionNotice::SCChiefElectionNotice() : header(MT_CHIEF_ELECTION_NOTICE_SC) {}
	SCChiefElectionMatchInfo::SCChiefElectionMatchInfo() : header(MT_CHIEF_ELECTION_MATCH_INFO_SC) {}
	SCChiefElectionRoleInfo::SCChiefElectionRoleInfo() : header(MT_CHIEF_ELECTION_ROLE_INFO_SC) {}
	SCChiefElectionWinRoleInfo::SCChiefElectionWinRoleInfo() : header(MT_CHIEF_ELECTION_WIN_ROLE_INFO_SC) {}
	SCChiefElectionJoinTopRoleInfo::SCChiefElectionJoinTopRoleInfo() : header(MT_CHIEF_ELECTION_JOIN_TOP_INFO_SC) {}
	SCChiefElectionFighterFinishNotice::SCChiefElectionFighterFinishNotice() : header(MT_CHIEF_ELECTION_FIGHTER_FINISH_NOTICE_SC) {}
	SCChiefElectionChampion::SCChiefElectionChampion() : header(MT_CHIEF_ELECTION_CHAMPION_SC) {}
	SCChiefElectionSeaRoleInfo::SCChiefElectionSeaRoleInfo() : header(MT_CHIEF_ELECTION_SEA_ROLE_INFO_SC) {}
	SCChiefElectionSeaSceneInfo::SCChiefElectionSeaSceneInfo() : header(MT_CHIEF_ELECTION_SEA_SCENE_INFO_SC) {}
	SCChiefElectionStateInfo::SCChiefElectionStateInfo() : header(MT_CHIEF_ELECTION_STATE_INFO_SC) {}
	SCChiefElectionSeaRank::SCChiefElectionSeaRank() : header(MT_CHIEF_ELECTION_RANK_SC) {}

	//首席巅峰战
	CSChiefPeakBattleReq::CSChiefPeakBattleReq() : header(MT_CHIEF_PEAK_BATTLE_REQ_CS) {}
	SCChiefPeakBattleNotice::SCChiefPeakBattleNotice() : header(MT_CHIEF_PEAK_BATTLE_NOTICE_SC) {}
	SCChiefPeakBattleMatchInfo::SCChiefPeakBattleMatchInfo() : header(MT_CHIEF_PEAK_BATTLE_MATCH_INFO_SC) {}
	SCChiefPeakBattleRoleInfo::SCChiefPeakBattleRoleInfo() : header(MT_CHIEF_PEAK_BATTLE_ROLE_INFO_SC) {}
	SCChiefPeakBattleInfo::SCChiefPeakBattleInfo() : header(MT_CHIEF_PEAK_BATTLE_WINNER_INFO_SC) {}
	SCChiefPeakBattleFighterFinishNotice::SCChiefPeakBattleFighterFinishNotice() : header(MT_CHIEF_PEAK_BATTLE_FIGHTER_FINISH_NOTICE_SC) {}
	SCChiefPeakBattleChampion::SCChiefPeakBattleChampion() : header(MT_CHIEF_PEAK_BATTLE_CHAMPION_SC) {}

	//家族荣耀战
	CSGuildHonorReq::CSGuildHonorReq() : header(MT_GUILD_HONOR_REQ_CS) {}
	SCGuildHonorRoleInfo::SCGuildHonorRoleInfo() : header(MT_GUILD_HONOR_ROLE_INFO_SC) {}
	SCGuildHonorRoleChange::SCGuildHonorRoleChange() : header(MT_GUILD_HONOR_ROLE_CHANGE_SC) {}
	SCGuildHonorAddRoleInfo::SCGuildHonorAddRoleInfo() : header(MT_GUILD_HONOR_ADD_ROLE_SC) {}
	SCGuildHonorZoneBattleResultInfo::SCGuildHonorZoneBattleResultInfo() : header(MT_GUILDHONOR_ZONE_BATTLE_RESULT_INFO_SC) {}
	SCGuildHonorAllZoneBattleResultInfo::SCGuildHonorAllZoneBattleResultInfo() : header(MT_GUILD_HONOR_ALL_ZONE_BATTLE_RESULT_INFO_SC) {}
	SCGuildHonorAllZoneRoleNumInfo::SCGuildHonorAllZoneRoleNumInfo() : header(MT_GUILD_HONOR_ALL_ZONE_ROLE_NUM_INFO_SC) {}
	CSGuildHonorReq2::CSGuildHonorReq2() : header(MT_GUILD_HONOR_REQ_2_CS) {}
	SCGuildHonorBattleProcessInfo::SCGuildHonorBattleProcessInfo() : header(MT_GUILD_HONOR_PROCESSINFO_SC) {}
	SCGuildHonorBattleScoreInfo::SCGuildHonorBattleScoreInfo() : header(MT_GUILD_HONOR_SCORE_RANK_SC) {}
	SCGuildHonorScoreRankInfo::SCGuildHonorScoreRankInfo() : header(MT_GUILD_HONOR_SCORE_RANK_INFO_SC) {}
	SCGuildHonorBattleMatchInfo::SCGuildHonorBattleMatchInfo() : header(MT_GUILD_HONOR_MATCH_INFO_SC) {}
	SCGuildHonorBattleGuessInfo::SCGuildHonorBattleGuessInfo() : header(MT_GUILD_HONOR_GUESS_INFO_SC) {}
	SCGuildHonorBattleMatchRecord::SCGuildHonorBattleMatchRecord() : header(MT_GUILD_HONOR_MATCH_RECORD_SC) {}
	SCGuildHonorBattleTopInfo::SCGuildHonorBattleTopInfo() : header(MT_GUILD_HONOR_TOP_INFO_SC) {}
	SCGuildHonorRoleBattleState::SCGuildHonorRoleBattleState() : header(MT_GUILD_HONOR_ROLE_BATTLE_STATE_SC) {}

	//擂台站
	CSPlatformBattleReq::CSPlatformBattleReq() : header(MT_PLATFORM_BATTLE_REQ_CS) {}
	SCPlatformBattleRoleInfo::SCPlatformBattleRoleInfo() : header(MT_PLATFORM_BATTLE_ROLE_INFO_SC) {}
	SCPlatformBattleRankInfo::SCPlatformBattleRankInfo() : header(MT_PLATFORM_BATTLE_RANK_INFO_SC) {}
	CSPlatformBattleRankFirstInfoReq::CSPlatformBattleRankFirstInfoReq() : header(MT_PLATFORM_BATTLE_RANK_FIRST_INFO_REQ_CS) {}
	SCPlatformBattleRankFirstInfo::SCPlatformBattleRankFirstInfo() : header(MT_PLATFORM_BATTLE_RANK_FIRST_INFO_SC) {}
	CSPlatformBattlePlaformSwitch::CSPlatformBattlePlaformSwitch() : header(MT_PLATFORM_BATTLE_SWITCH_PLATFORM_CS) {}
	SCPlatformBattleRecord::SCPlatformBattleRecord() : header(MT_PLATFORM_BATTLE_RECORD_CS) {}
	SCPlatformBattleRecordNotice::SCPlatformBattleRecordNotice() : header(MT_PLATFORM_BATTLE_RECORD_NOTICE_CS) {}
	CSPlatformBattleReq2::CSPlatformBattleReq2() : header(MT_PLATFORM_BATTLE_REQ_2_CS) {}
	SCPlatformBattleAchievementInfo::SCPlatformBattleAchievementInfo() : header(MT_PLATFORM_BATTLE_ACHIEVEMENT_INFO_SC) {}
	SCPlatformBattleSettleAccountNotice::SCPlatformBattleSettleAccountNotice() : header(MT_PLATFORM_BATTLE_SETTLE_NOTICE_SC) {}
	SCPlatformBattleLastRankInfo::SCPlatformBattleLastRankInfo() : header(MT_PLATFORM_BATTLE_LAST_RANK_INFO_SC) {}

	// 查询人物装备信息
	CSQueryRoleInfo::CSQueryRoleInfo() : header(MT_REFER_ROLE_EQUIPMENT_CS) {}
	SCQueryRoleInfoRet::SCQueryRoleInfoRet() : header(MT_ROLE_QUERY_INFOR_SC) {}
	SCQueryRoleSimpleInfo::SCQueryRoleSimpleInfo() : header(MT_ROLE_QUERY_SIMPLE_INFO_SC) {}
	//SCQueryRoleFollowUpInfo::SCQueryRoleFollowUpInfo() : header(MT_ROLE_QUERY_FOLLOW_UP_INFO_SC) {}

	SCCrossStartAck::SCCrossStartAck() : header(MT_CROSS_START_ACK_SC) {}
	CSCrossStartReq::CSCrossStartReq() : header(MT_CROSS_START_REQ_CS) {}
	CSBackToOriginServer::CSBackToOriginServer() : header(MT_BACK_TO_ORIGIN_SERVER_CS) {}
	SCBackToOriginServer::SCBackToOriginServer() : header(MT_BACK_TO_ORIGIN_SERVER_SC) {}

	// CourseOfGrowth - 成长之路
	CSCOGOperatorRequest::CSCOGOperatorRequest() : header(MT_COG_OPERATOR_REQUEST_CS) {}
	SCCOGAllInformations::SCCOGAllInformations() : header(MT_COG_ALL_INFORMATIONS_SC) {}
	SCCOGSingleTaskProgs::SCCOGSingleTaskProgs() : header(MT_COG_SINGLE_TASK_PROG_SC) {}
	SCCOGRewardFetchFlag::SCCOGRewardFetchFlag() : header(MT_COG_SINGLE_FLAG_STAT_SC) {}
	SCCOGStarAwardStatus::SCCOGStarAwardStatus() : header(MT_COG_STAR_REWARD_STAT_SC) {}
	SCCOGSingleSeqNotify::SCCOGSingleSeqNotify() : header(MT_COG_SINGLE_SE_NOTIFY_SC) {}
	SCRoleCourseAllInfo::SCRoleCourseAllInfo()   : header(MT_ROLE_COURSE_ALL_INFO_SC) {}
	SCRoleCourseSingleInfo::SCRoleCourseSingleInfo() : header(MT_ROLE_COURSE_SINGLE_INFO_SC) {}
	SCRoleCourseNoticeInfo::SCRoleCourseNoticeInfo() :header(MT_ROLE_COURSE_NOTICE_INFO_SC) {}
	SCServerCourseAllInfo::SCServerCourseAllInfo() : header(MT_SERVER_COURSE_ALL_INFO_SC) {}
	SCServerCourseSingleInfo::SCServerCourseSingleInfo() : header(MT_SERVER_COURSE_SINGLE_INFO_SC) {}
 
	SCNoticeCfgUpdate::SCNoticeCfgUpdate() : header(MT_NOTICE_CFG_UPDATE_SC) {}

	CSRoleSubscribe::CSRoleSubscribe() : header(MT_ROLE_SUBSCRIBE_REQ_CS) {}

	CSRoleSalaryInfo::CSRoleSalaryInfo() : header(MT_ROLE_SALARY_INFO_CS) {}
	SCRoleSalaryInfo::SCRoleSalaryInfo() : header(MT_ROLE_SALARY_INFO_SC) {}
	SCRoleSalaryDay::SCRoleSalaryDay() : header(MT_ROLE_SALARY_DAY_SC) {}
	SCRoleHistoryChivalrous::SCRoleHistoryChivalrous() : header(MT_ROLE_HISTORY_CHIVALROUS) {}
	CSRoleHistoryChivalrousRewardFetch::CSRoleHistoryChivalrousRewardFetch() : header(MT_ROLE_HISTORY_CHIVALROUS_REWARD_FETCH) {}
	CSChivalrousTeamListReq::CSChivalrousTeamListReq() : header(MT_CHIVALROUS_TEAM_LIST_REQ_CS) {}
	SCChivalrousTeamList::SCChivalrousTeamList() : header(MT_CHIVALROUS_TEAM_LIST_SC) {}

	CSSpeaker::CSSpeaker() : header(MT_LOUD_SPEAKER_CS) {}
	SCSpeaker::SCSpeaker() : header(MT_SPEAKER_INFO_SC) {}
	SCLoginSystemInfo::SCLoginSystemInfo() : header(MT_ROLE_LOGIN_SYSTEM_INFO_SC) {}
	CSLoginSystemREQ::CSLoginSystemREQ() : header(MT_ROLE_LOGIN_SYSTEM_REQ_CS) {}

	SCSpecialNotice::SCSpecialNotice() : header(MT_ROLE_SPECIAL_NOTICE_SC) {}

	// ChallengeFb - 挑战副本
	SCChallengeFbRoleInfor::SCChallengeFbRoleInfor() : header(MT_CHALLENGE_FB_ROLE_INFOR) {}
	SCChallengeFbAwardList::SCChallengeFbAwardList() : header(MT_CHALLENGE_FB_AWARD_LIST) {}

	// MysteriousTrial - 神秘试炼(七天活动)
	CSMysteriousTrialReq::CSMysteriousTrialReq() : header(MT_MYSTERIOUS_TRIAL_CS) {}
	SCMysteriousTrialInfo::SCMysteriousTrialInfo() : header(MT_MYSTERIOUS_TRIAL_INFO_SC) {}
	SCMysteriousTrialSingleInfo::SCMysteriousTrialSingleInfo() : header(MT_MYSTERIOUS_TRIAL_SINGLE_INFO_SC) {}
	SCMysterialFinallyRewardInfo::SCMysterialFinallyRewardInfo() : header(MT_MYSTERIOUS_TRIAL_FINALLY_REWARD_SC) {}

	// TransactionTask - 购买任务(每日任务) - 丝绸之路
	CSTransactionTaskReq::CSTransactionTaskReq() : header(MT_TRANSACTION_TASK_CS) {}
	SCTransactionTaskInfo::SCTransactionTaskInfo() : header(MT_TRANSACTION_TASK_INFO_SC) {}

	// ActiveSystem - 活跃系统
	CSActiveSystemReq::CSActiveSystemReq() : header(MT_ACTIVE_SYSTEM_CS) {}
	SCActiveSystemInfo::SCActiveSystemInfo() : header(MT_ACTIVE_SYSTEM_INFO_SC) {}
	SCActiveSystemSingleInfo::SCActiveSystemSingleInfo() : header(MT_ACTIVE_SYSTEM_SINGLE_INFO_SC){}
	CSActiveSystemExtraRewardFetch::CSActiveSystemExtraRewardFetch() : header(MT_ACTIVE_SYSTEM_EXTRA_REWARD_FETCH_CS){}

	SCSpecialActiveInfo::SCSpecialActiveInfo() : header(MT_ACTIVE_SYSTEM_SPECIAL_ACTIVE_INFO_SC) {}

	//心法
	CSHeartSkillReq::CSHeartSkillReq() : header(MT_HEART_SKILL_REQ_CS) {}
	SCHeartSkillAllInfo::SCHeartSkillAllInfo() : header(MT_HEART_SKILL_ALL_INFO_SC) {}
	SCHeartSkillSingleInfo::SCHeartSkillSingleInfo() : header(MT_HEART_SKILL_SINGLE_INFO_SC) {}
	SCHeartSkillOtherInfo::SCHeartSkillOtherInfo() : header(MT_HEART_SKILL_OTHER_INFO_SC) {}
	CSHeartSkillDisplaceReq::CSHeartSkillDisplaceReq() : header(MT_HEART_SKILL_DISPLACE_REQ_CS) {}

	//月卡周卡
	CSMonthCardReq::CSMonthCardReq() : header(MT_MONTH_CARD_REQ_CS) {}
	SCMonthCardInfo::SCMonthCardInfo() : header(MT_MONTH_CARD_INFO_SC) {}

	// FaceSore - 颜值系统
	CSFaceScoreRequest::CSFaceScoreRequest() : header(MT_FSS_OPERATOR_REQUEST_CS) {}
	SCFaceScoreAllInfo::SCFaceScoreAllInfo() : header(MT_FSS_ALL_INFORMATIONS_SC) {}
	SCFaceListGridInfo::SCFaceListGridInfo() : header(MT_FSS_SINGLE_GRID_INFO_SC) {}
	SCFaceResStateInfo::SCFaceResStateInfo() : header(MT_FSS_SINGLE_RES_STATE_SC) {}
	SCFaceLevelAndExps::SCFaceLevelAndExps() : header(MT_FSS_LEVEL_EXPERIENCE_SC) {}
	SCFaceTimeSingleInfo::SCFaceTimeSingleInfo() : header(MT_FSS_TIME_SC) {}
	SCFaceTimeAllInfo::SCFaceTimeAllInfo() : header(MT_RES_TIME_ALL_SC) {}

	// 引导副本
	SCGuideFbSceneInfo::SCGuideFbSceneInfo() : header(MT_GUIDE_FB_INFO_SC) {}

	// RoleMaterial 玉虚境
	CSMaterialReq::CSMaterialReq() : header(MT_MATERIAL_REQ_CS) {}
	SCMaterialSceneInfo::SCMaterialSceneInfo() : header(MT_MATERIAL_SCENE_INFO_SC) {}
	SCMaterialAllInfo::SCMaterialAllInfo() : header(MT_MATERIAL_ALL_INFO_SC) {}
	SCMaterialSingleInfo::SCMaterialSingleInfo() : header(MT_MATERIAL_SINGLE_INFO_SC) {}
	SCMaterialOtherInfo::SCMaterialOtherInfo() : header(MT_MATERIAL_OTHER_INFO_SC) {}

	SCSalaryBattleStatInfo::SCSalaryBattleStatInfo() : header(MT_SALARY_BATTLE_STAT_INFO_SC) {}
	CSSetAutoAddPointFlag::CSSetAutoAddPointFlag() : header(MT_SET_AUTO_ADD_POINT_FLAG_CS) {}

	// 充值相关
	SCNewChongzhiConfigInfo::SCNewChongzhiConfigInfo() : header(MT_NEW_CHONG_ZHI_CONFIG_INFO_SC) {}
	CSNewChongzhiConfigInfoReq::CSNewChongzhiConfigInfoReq() : header(MT_NEW_CHONG_ZHI_CONFIG_INFO_CS) {}
	SCNewChongzhiConfigReloadNotice::SCNewChongzhiConfigReloadNotice() : header(MT_NEW_CHONG_ZHI_CONFIG_RELOAD_NOTICE_SC) {}

	// 兑换商店（百货)
	CSConvertShopBuyReq::CSConvertShopBuyReq() : header(MT_CONVERT_SHOP_BUY_REQ_CS) {}
	SCConvertShopBuyRecord::SCConvertShopBuyRecord() : header(MT_CONVERT_SHOP_BUY_RECORD_LIST_SC) {}
	SCConvertShopAddRecord::SCConvertShopAddRecord() : header(MT_CONVERT_SHOP_ADD_BUY_RECORD_SC) {}

	SCBossChapterInfo::SCBossChapterInfo() : header(MT_BOSS_CHAPTER_INFO_SC) {}
	CSBossChapterReq::CSBossChapterReq() : header(MT_BOSS_CHAPTER_REQ_CS) {}

	SCWorldQuestionNotice::SCWorldQuestionNotice() : header(MT_WORLD_QUESTION_INFO_SC) {}
	CSWorldQuestionReq::CSWorldQuestionReq() : header(MT_WORLD_QUESTION_REQ_CS) {};

	SCMatchInfoUpdate::SCMatchInfoUpdate() : header(MT_SCENE_MATCH_INFO_UPDATE_SC) {}
	SCMatchEnemy::SCMatchEnemy() : header(MT_SCENE_MATCH_MATCH_ENEMY_SC) {}
	SCStartMatch::SCStartMatch() : header(MT_SCENE_MATCH_START_SC) {}
	SCStopMatch::SCStopMatch() : header(MT_SCENE_MATCH_STOP_SC) {}

	//一键完成
	CSOneKeyFinish::CSOneKeyFinish() : header(MT_ONE_KEY_FINISH_SC) {}

	// 翻牌奖励请求
	CSPokerReq::CSPokerReq() : header(MT_POKER_REWARD_CS) {}

	//CSRAMatchReq::CSRAMatchReq() : header(MT_RAND_ACTIVITY_MATCH_REQ_CS) {}
	CSPartnerSkillActivate::CSPartnerSkillActivate() : header(MT_PARTNER_SKILL_ACTIVATE_CS) {}


	//幻界战场
	CSHuanJieZhanChangReq::CSHuanJieZhanChangReq() : header(MT_HUAN_JIE_ZHAN_CHANG_REQ_CS) {}
	SCHuanJieZhanChangTeamInfo::SCHuanJieZhanChangTeamInfo() : header(MT_HUAN_JIE_ZHAN_CHANG_TEAM_INFO_SC) {}
	SCHuanJieZhanChangErrReturn::SCHuanJieZhanChangErrReturn() : header(MT_HUAN_JIE_ZHAN_CHANG_ERR_RETURN_SC) {}
	SCHuanJieZhanChangMatchState::SCHuanJieZhanChangMatchState() : header(MT_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_SC) {}
	SCHuanJieZhanChangResult::SCHuanJieZhanChangResult() : header(MT_HUAN_JIE_ZHAN_CHANG_RESULT_SC) {}
	SCHuanJieZhanChangOpponentInfo::SCHuanJieZhanChangOpponentInfo() : header(MT_HUAN_JIE_ZHAN_CHANG_OPPONENT_INFO_SC) {}
	SCHuanJieZhanChangRoleInfo::SCHuanJieZhanChangRoleInfo() : header(MT_HUAN_JIE_ZHAN_CHANG_ROLE_INFO_SC) {}
	SCHuanJieZhanChangRankInfo::SCHuanJieZhanChangRankInfo() : header(MT_HUAN_JIE_ZHAN_CHANG_RANK_LIST_INFO_SC) {}
	CSHuanJieZhanChangRoleRankListReq::CSHuanJieZhanChangRoleRankListReq() : header(MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_REQ_CS) {}
	SCHuanJieZhanChangRoleRankListing::SCHuanJieZhanChangRoleRankListing() : header(MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_SC) {}
	SCHuanJieZhanChangTianTiRecord::SCHuanJieZhanChangTianTiRecord() : header(MT_HUAN_JIE_ZHAN_CHANG_TIAN_TI_RECORD_SC) {}

	//幻界战场-化身模块
	SCHuaShenRoleInfoAck::SCHuaShenRoleInfoAck() : header(MT_HUASHEN_ROLE_INFO_SC) {}
	SCHuaShenRoleAttrList::SCHuaShenRoleAttrList() : header(MT_HUASHEN_ROLE_ATTR_LIST_SC) {}
	CSHuaShenMedalReq::CSHuaShenMedalReq() : header(MT_HUASHEN_MEDAL_REQ_CS){}
	SCHuaShenMedalInfo::SCHuaShenMedalInfo() : header(MT_HUASHEN_MEDAL_INFO_SC) {}
	CSHuaShenEquipmentOperaReq::CSHuaShenEquipmentOperaReq() : header(MT_HUASHEN_EQUIPMENT_OPERA_REQ_CS) {}
	SCHuaShenEquipmentInfoAck::SCHuaShenEquipmentInfoAck() : header(MT_HUASHEN_EQUIPMENT_INFO_SC) {}
	SCHuaShenEquipmentGridChange::SCHuaShenEquipmentGridChange() : header(MT_HUASHEN_EQUIPMENT_GRID_CHANGE_SC) {}
	CSHuaShenJewelryHandleReq::CSHuaShenJewelryHandleReq() : header(MT_HUASHEN_JEWELRY_HANDLE_REQ_CS) {}
	SCHuaShenJewelryAllInform::SCHuaShenJewelryAllInform() : header(MT_HUASHEN_JEWELRY_ALL_FORM_SC) {}
	SCHuaShenJewelryInfoChange::SCHuaShenJewelryInfoChange() : header(MT_HUASHEN_JEWELRY_INFO_CHANGE_SC) {}
	CSHuaShenAttributePlanPointReq::CSHuaShenAttributePlanPointReq() : header(MT_HUASHEN_ATTR_PLAN_POINT_CS) {}
	SCHuaShenRoleAttributePlanInfo::SCHuaShenRoleAttributePlanInfo() : header(MT_HUASHEN_ROLE_ATTR_PLAN_SC) {}
	CSHuaShenPetOp::CSHuaShenPetOp() : header(MT_HUASHEN_PET_REQ_CS) {}
	CSHuaShenPetAssignAttributePoints::CSHuaShenPetAssignAttributePoints() : header(MT_HUASHEN_PET_ASSIGN_ATTR_POINT_CS) {}
	SCHuaShenSendPetAllInfo::SCHuaShenSendPetAllInfo() : header(MT_HUASHEN_SEND_PET_ALL_INFO_SC) {}
	SCHuaShenSendPetSingleInfo::SCHuaShenSendPetSingleInfo() : header(MT_HUASHEN_PET_SINGLE_INFO_SC) {}
	SCHuaShenSendPetOtherInfo::SCHuaShenSendPetOtherInfo() : header(MT_HAUSHEN_PET_OTHER_INFO_SC) {}
	SCHuaShenSendPetAttrListInfo::SCHuaShenSendPetAttrListInfo() : header(MT_HUASHEN_PET_ATTRLIST_INFO_SC) {}
	CSHuaShenInfoReq::CSHuaShenInfoReq() : header(MT_HUASHEN_INFO_REQ_CS) {}
	SCHuaShenKnapsackInfoAck::SCHuaShenKnapsackInfoAck() : header(MT_HUASHEN_KNAPSACK_ALL_INFO_SC) {}
	SCHuaShenKnapsackItemChange::SCHuaShenKnapsackItemChange() : header(MT_HUASHEN_KNAPSACK_SINGLE_ITEM_CHANEGE_SC) {}
	CSSetHuaShenBattlePosition::CSSetHuaShenBattlePosition() : header(MT_HUASHEN_SET_BATTLE_POSITION_CS) {}
	SCSetHuaShenBattlePosition::SCSetHuaShenBattlePosition() : header(MT_HUASHEN_BATTLE_POSITION_SC) {}
	SCHuaShenSeasonInfo::SCHuaShenSeasonInfo() : header(MT_HUASHEN_SEASON_INFO_SC) {}
	SCHuaShenElementCrystalChangeNotice::SCHuaShenElementCrystalChangeNotice() : header(MT_HUASHEN_CHANGE_CRYSTAL_NOTICE_SC) {}
	SCHuaShenAttrPlanListInfo::SCHuaShenAttrPlanListInfo() : header(MT_HUASHEN_ATTR_PLAN_LIST_INFO_SC) {}
	CSHuaShenAttributePlanChangeName::CSHuaShenAttributePlanChangeName() : header(MT_HUASHEN_ATTR_PLAN_CHANGE_NAME_CS) {}
	SCHuaShenMedalTakeOffInfo::SCHuaShenMedalTakeOffInfo() : header(MT_HUASHEN_MEDAL_TAKE_OFF_SC) {}

	CSFuncPreviewClaimReward::CSFuncPreviewClaimReward() : header(MT_FUNC_PREVIEW_CLAIM_REWARD_CS) {}
	SCFuncPreviewInfo::SCFuncPreviewInfo() : header(MT_FUNC_PREVIEW_INFO_SC) {}

	CSRealmBreakReq::CSRealmBreakReq() : header(MT_REALM_BREAK_REQ_CS) {}
	SCRealmBreakAllInfo::SCRealmBreakAllInfo() : header(MT_REALM_BREAK_ALL_INFO_SC) {}
//	SCRealBreakStartTuNa::SCRealBreakStartTuNa() : header(MT_REALM_BREAK_START_TUNA_SC) {}
	SCRealmBreakItemInfo::SCRealmBreakItemInfo() : header(MT_REALM_BREAK_ITEM_INFO_SC) {}

	CSMapExplorationReq::CSMapExplorationReq() : header(MT_MAP_EXPLORATION_REQ_CS) {}
	SCMapExplorationInfo::SCMapExplorationInfo() : header(MT_MAP_EXPLORATION_INFO_SC) {}

	CSSetAutoCatchPet::CSSetAutoCatchPet() : header(MT_AUTO_CATCH_PET_CS) {}
	SCAutoCatchPetInfo::SCAutoCatchPetInfo() : header(MT_AUTO_CATCH_PET_SC) {}

	CSFormationSimpleInfo::CSFormationSimpleInfo() : header(MT_FORMATION_SIMPLE_INFO_CS) {}
	SCFormationSimpleInfo::SCFormationSimpleInfo() : header(MT_FORMATION_SIMPLE_INFO_SC) {}
	CSFormationDetailInfo::CSFormationDetailInfo() : header(MT_FORMATION_DETAIL_INFO_CS) {}
	SCFormationDetailInfo::SCFormationDetailInfo() : header(MT_FORMATION_DETAIL_INFO_SC) {}
	CSFormationRename::CSFormationRename() : header(MT_FORMATION_RENAME_CS) {}
	SCFormationRename::SCFormationRename() : header(MT_FORMATION_RENAME_SC) {}
	CSFormationSave::CSFormationSave() : header(MT_FORMATION_SAVE_CS) {}
	CSFormationDelete::CSFormationDelete() : header(MT_FORMATION_DELETE_CS) {}
	CSSetPetHelperFormation::CSSetPetHelperFormation() : header(MT_PET_HELPER_FORMATION_SET_CS) {}
	SCPetHelperFormationInfo::SCPetHelperFormationInfo() : header(MT_PET_HELPER_FORMATION_INFO_SC) {}
	CSPetHelperFormationQuery::CSPetHelperFormationQuery() : header(MT_PET_HELPER_FORMATION_QUERY_CS) {}
	SCCourageChallengePetHelperNotice::SCCourageChallengePetHelperNotice() : header(MT_COURAGE_CHALLENGE_PET_HELPER_NOTICE_SC) {}

	SCRoleGiftGatherDreamList::SCRoleGiftGatherDreamList() : header(MT_ROLE_GIFT_GATHER_DREAM_LIST_SC) {}
	SCRoleCanPublishGahterDreamItemList::SCRoleCanPublishGahterDreamItemList() : header(MT_ROLE_CAN_PUBLISH_GATHER_DREAM_LIST_SC) {}
	SCRoleGatherDreamRecordInfoList::SCRoleGatherDreamRecordInfoList() : header(MT_ROLE_GAHTER_DREAM_RECORD_INFO_LIST_SC) {}

	//神秘(命格)商店
	SCMysteryStoreReq::SCMysteryStoreReq() : header(MT_MYSTERY_STORE_OP_CS) {}
	SCMysteryStoreInfo::SCMysteryStoreInfo() : header(MT_MYSTERY_STORE_INFO_SC) {}

	//山海BOSS
	CSShanHaiBossReq::CSShanHaiBossReq() : header(MT_SHAN_HAI_BOSS_OP_CS) {}
	SCShanHaiBossInfo::SCShanHaiBossInfo() : header(MT_SHAN_HAI_BOSS_NPC_INFO_SC) {}
	SCShanHaiBossRoleInfo::SCShanHaiBossRoleInfo() : header(MT_SHAN_HAI_BOSS_ROLE_INFO_SC) {}
	//SCShanHaiBossPokerInfo::SCShanHaiBossPokerInfo() : header(MT_SHAN_HAI_BOSS_POKER_INFO_SC) {}
	SCShanHaiBossSingleInfo::SCShanHaiBossSingleInfo() : header(MT_SHAN_HAI_BOSS_SINGLE_INFO_SC) {}
	
	SCRACaiShenJuBaoInfo::SCRACaiShenJuBaoInfo() : header(MT_RAND_ACTIVITY_CAI_SHEN_JU_BAO_INFO_SC) {}
	SCRADingZhiBaoXiangInfo::SCRADingZhiBaoXiangInfo() : header(MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_INFO_SC) {}
	SCRAZhuanShuLiBaoInfo::SCRAZhuanShuLiBaoInfo() : header(MT_RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_INFO_SC) {}
	SCRAZhuanShuLiBaoDayInfo::SCRAZhuanShuLiBaoDayInfo() : header(MT_RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_DAY_INFO_SC) {}
	SCRAJingYanBiaoShengInfo::SCRAJingYanBiaoShengInfo() : header(MT_RAND_ACTIVITY_JING_YAN_BIAO_SHENG_INFO_SC) {}
	SCRAXianShiBaoHeInfo::SCRAXianShiBaoHeInfo() : header(MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_SC) {}
	SCRAXianShiBaoDaiInfo::SCRAXianShiBaoDaiInfo() : header(MT_RAND_ACTIVITY_XIAN_SHI_BAO_DAI_SC) {}

	SCRADingZhiBaoXiang2Info::SCRADingZhiBaoXiang2Info() : header(MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_2_INFO_SC) {}
	SCRAXianShiBaoHe2Info::SCRAXianShiBaoHe2Info() : header(MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_2_SC) {}
	SCRAXianShiBaoDai2Info::SCRAXianShiBaoDai2Info() : header(MT_RAND_ACTIVITY_XIAN_SHI_BAO_DAI_2_SC) {}

	CSGetCurPos::CSGetCurPos() : header(MT_GET_CUR_POS_CS) {}
	SCOjbSpeedChangeNotice::SCOjbSpeedChangeNotice() : header(MT_OBJ_SPEED_CHANGE_NOTICE) {}
	SCGetCurPos::SCGetCurPos() : header(MT_GET_CUR_POS_SC) {}

	SCNoticeClientReloadSensitiveWords::SCNoticeClientReloadSensitiveWords() : header(MT_NOTICE_CLIENT_RELOAD_SENSITIVE_WORDS_SC) {}
	SCNoticeClientNewMessageFromCustomService::SCNoticeClientNewMessageFromCustomService() : header(MT_NEW_MESSAGE_FROM_CUSTOM_SERVICE) {}
	SCCmdRoute::SCCmdRoute() : header(MT_CMD_ROUTE_SC) {}
	CSCatchFoxSpecialLogicReq::CSCatchFoxSpecialLogicReq() : header(MT_NEW_TASK_CATCH_FOX_REQ_CS) {}

	//旧等级助力
	SCLevelComplementInfo::SCLevelComplementInfo() : header(MT_ROLE_LEVEL_COMPLEMENT_SC) {}
	CSLevelComplementReq::CSLevelComplementReq() : header(MT_ROLE_LEVEL_COMPLEMENT_CS) {}

	//新等级助力
	CSRoleNewLevelComplementReq::CSRoleNewLevelComplementReq () : header(MT_ROLE_NEW_LEVEL_COMPLEMENT_REQ_CS) {}
	SCRoleNewLevelComplementInfo::SCRoleNewLevelComplementInfo() : header(MT_ROLE_NEW_LEVEL_COMPLEMENT_INFO_SC) {}
	SCNewLevelComplementSpeicalInfo::SCNewLevelComplementSpeicalInfo() : header(MT_NEW_LEVEL_COMPLEMENT_SPEICAL_INFO_SC) {}

	CSWenXiangLouReq::CSWenXiangLouReq() : header(MT_WEN_XIANG_LOU_FB_REQ_CS) {}
	SCWenXiangLouNpcInfo::SCWenXiangLouNpcInfo() : header(MT_WEN_XIANG_LOU_NPC_INFO_SC) {}
	SCWenXiangLouReqSceneInfo::SCWenXiangLouReqSceneInfo() : header(MT_WEN_XIANG_LOU_SCENE_INFO_SC) {}

	SCAnecdoteTaskInfo::SCAnecdoteTaskInfo() : header(MT_ANECDOTE_TASK_INFO_SC) {}
	SCAnecdoteTaskInfoList::SCAnecdoteTaskInfoList() : header(MT_ANECDOTE_TASK_INFO_LIST_SC) {}
	SCAnecdoteTaskBattleFail::SCAnecdoteTaskBattleFail() : header(MT_ANECDOTE_TASK_BATTLE_FAIL_SC) {}
	SCAnecdoteTaskCompleteNotice::SCAnecdoteTaskCompleteNotice() : header(MT_ANECDOTE_TASK_COMPLETE_TASK_NOTICE_SC) {}
	CSAnecdoteTaskOperate::CSAnecdoteTaskOperate() : header(MT_ANECDOTE_TASK_OPERATE_REQ_CS) {}
	SCAnecdoteTaskSpecialInfo::SCAnecdoteTaskSpecialInfo() : header(MT_ANECDOTE_TASK_SPECIAL_INFO_SC) {}
	CSAnecdoteTaskSpecialReq::CSAnecdoteTaskSpecialReq() : header(MT_ANECDOTE_TASK_SPECIAL_SET_CS) {}
	SCAnecdoteBigEventInfo::SCAnecdoteBigEventInfo() : header(MT_ANECDOTE_BIG_EVENT_INFO_SC) {}
	CSNormalTaskFinishAnedoteGame::CSNormalTaskFinishAnedoteGame() : header(MT_NORMAL_TASK_FINISH_ANEDOTE_GAME_CS) {}
	SCResearchTaskNoticInfo::SCResearchTaskNoticInfo() : header(MT_RESEARCH_TASK_ROUND_INFO_SC) {}
	CSNewTaskAction::CSNewTaskAction() : header(MT_NEW_TASK_ACTION_REQ_CS) {}
	CSNewTaskGiveNPCTaskNeedItems::CSNewTaskGiveNPCTaskNeedItems() : header(MT_NEW_TASK_GIVE_NPC_ITEM_CS) {}
	SCNewTaskPartnerSoarInfo::SCNewTaskPartnerSoarInfo() : header(MT_NEW_TASK_PARTNER_SOAR_INFO_SC) {}

	SCDuChuangZeiYingSceneInfo::SCDuChuangZeiYingSceneInfo() : header(MT_NEW_TASK_DU_CHUANG_ZEI_YING_SCENE_INFO_SC) {}
	SCDuChuangZeiYingNotice::SCDuChuangZeiYingNotice() : header(MT_NEW_TASK_DU_CHUANG_ZEI_YING_NOTICE_SC) {}

	//翻牌管理器
	CSDrawReq::CSDrawReq() : header(MT_DRAW_REQ_CS) {}
	SCRoleDrawInfo::SCRoleDrawInfo() : header(MT_ROLE_DRAW_INFO_SC) {}
	SCDrawPokerInfo::SCDrawPokerInfo() : header(MT_DRAW_POKER_INFO_SC) {}

	//梦渊笔录
	CSDreamNotesChallengeOp::CSDreamNotesChallengeOp() : header(MT_DREAM_NOTES_CHALLENGE_CS) {}
	SCDreamNotesChallengeInfo::SCDreamNotesChallengeInfo() : header(MT_DREAM_NOTES_CHALLENGE_ROLE_INFO_SC) {}
	SCDreamNotesChallengeSceneInfo::SCDreamNotesChallengeSceneInfo() : header(MT_DREAM_NOTES_CHALLENGE_SCENE_INFO_SC) {}
	SCDreamNotesChallengeNotice::SCDreamNotesChallengeNotice() : header(MT_DREAM_NOTES_CHALLENGE_NOTICE_SC) {}

	SCRAShenShouJiangLinInfo::SCRAShenShouJiangLinInfo() : header(MT_SHEN_SHOU_JIANG_LIN_INFO_SC) {}
	SCRAShenShouJiangLinDrawResult::SCRAShenShouJiangLinDrawResult() : header(MT_SHEN_SHOU_JIANG_LIN_DRWA_RESULT_SC) {}
	SCRAShenShouJiangLinBuyInfo::SCRAShenShouJiangLinBuyInfo() : header(MT_SHEN_SHOU_JIANG_LIN_BUY_INFO_SC) {}

	SCRALingYuJiJinInfo::SCRALingYuJiJinInfo() : header(MT_LING_YU_JI_JIN_INFO_SC) {}

	// 校场演练
	SCSchoolExerciseInfo::SCSchoolExerciseInfo() : header(MT_SCHOOL_EXERCISE_INFO_SC) {}
	CSSchoolExerciseChallenge::CSSchoolExerciseChallenge() : header(MT_SCHOOL_EXERCISE_CHALLENGE_CS) {}
	CSSchoolExerciseFirstOpenUI::CSSchoolExerciseFirstOpenUI() : header(MT_SCHOOL_EXERCISE_FIRST_OPEN_UI) {}
	SCSchoolExerciseChallengeSucc::SCSchoolExerciseChallengeSucc() : header(MT_SCHOOL_EXERCISE_CHALLENGE_SUCC_SC) {}
	
	SCRAMiJingQiWenShopInfo::SCRAMiJingQiWenShopInfo() : header(MT_MI_JING_QI_WEN_SHOP_INFO_SC) {}
	SCRAMiJingQiWenQuestionInfo::SCRAMiJingQiWenQuestionInfo() : header(MT_MI_JING_QI_WEN_QUESTION_INFO_SC) {}
	CSRAMiJingQiWenQuestionReq::CSRAMiJingQiWenQuestionReq() : header(MT_MI_JING_QI_WEN_QUESTION_REQ_CS) {}
	SCRAFortuneMazeBoxNum::SCRAFortuneMazeBoxNum() : header(MT_RA_FORTUNE_MAZE_BOX_NUM_SC) {}

	SCRAChunRiDengFengInfo::SCRAChunRiDengFengInfo() : header(MT_RA_CHUN_RI_DENG_FENG_INFO_SC) {}
	SCRAZuHeHeLiInfo::SCRAZuHeHeLiInfo() : header(MT_RA_ZU_HE_HE_LI_INFO_SC) {}
	SCRAOneYuanGouInfo::SCRAOneYuanGouInfo() : header(MT_RA_ONE_YUAN_GOU_INFO_SC) {}

	CSRAMiJingQiWenBuyItemReq::CSRAMiJingQiWenBuyItemReq() : header(MT_MI_JING_QI_WEN_BUY_ITEM_REQ_CS) {}


	SCMiJingQiWenFbObjInfo2::SCMiJingQiWenFbObjInfo2() : header(MT_RA_MI_JING_QI_WEN_2_OBJ_INFO_SC) {}
	SCRAMiJingQiWenShopInfo2::SCRAMiJingQiWenShopInfo2() : header(MT_MI_JING_QI_WEN_2_SHOP_INFO_SC) {}
	SCRAMiJingQiWenQuestionInfo2::SCRAMiJingQiWenQuestionInfo2() : header(MT_MI_JING_QI_WEN_2_QUESTION_INFO_SC) {}
	CSRAMiJingQiWenQuestionReq2::CSRAMiJingQiWenQuestionReq2() : header(MT_MI_JING_QI_WEN_2_QUESTION_REQ_CS) {}
	CSRAMiJingQiWenBuyItemReq2::CSRAMiJingQiWenBuyItemReq2() : header(MT_MI_JING_QI_WEN_2_BUY_ITEM_REQ_CS) {}

	SCCloseWindosRet::SCCloseWindosRet() : header(MT_CLOSE_WINDOS_RET_SC) {}
	CSCloseWindosReq::CSCloseWindosReq() : header(MT_CLOSE_WINDOS_REQ_CS) {}

	SCRALingPaiTianCiInfo::SCRALingPaiTianCiInfo() : header(MT_RA_LING_PAI_TIAN_CI_INFO_SC) {}
	SCRALingPaiTianCi2Info::SCRALingPaiTianCi2Info() : header(MT_RA_LING_PAI_TIAN_CI_2_INFO_SC) {}
	SCRALingPaiTianCi3Info::SCRALingPaiTianCi3Info() : header(MT_RA_LING_PAI_TIAN_CI_3_INFO_SC) {}
	SCRAFashionShopInfo::SCRAFashionShopInfo() : header(MT_RA_FASHION_SHOP_INFO_SC) {}
	SCRelicLootingTampInfo::SCRelicLootingTampInfo() : header(MT_RELIC_LOOTING_TIME_SC) {}
	SCRelicLootingFbSkillInfo::SCRelicLootingFbSkillInfo() : header(MT_RELIC_LOOTING_SKILL_SC) {}
	CSRelicLootingMatchReq::CSRelicLootingMatchReq() : header(MT_RELIC_LOOTING_MATH_CS) {}
	SCRelicLootingTeamInfoReq::SCRelicLootingTeamInfoReq() : header(MT_RELIC_LOOTING_TEAM_SC) {}
	CSRelicLootingPlayerReq::CSRelicLootingPlayerReq() : header(MT_RELIC_LOOTING_PLAYER_REQ_CS) {}
	SCRelicLootingObjInfo::SCRelicLootingObjInfo() : header(MT_RELIC_LOOTING_OBJ_INFO_SC) {}
	SCRelicLootingMemberPosInfo::SCRelicLootingMemberPosInfo() : header(MT_RELIC_LOOTING_MEMBER_POS_SC) {}
	SCRelicLootingMemberTransmitInfo::SCRelicLootingMemberTransmitInfo() : header(MT_RELIC_LOOTING_MEMBER_TS_SC) {}
	SCRelicLootingMarkInfo::SCRelicLootingMarkInfo() : header(MT_RELIC_LOOTING_MEMBER_MARK_SC) {}
	SCRelicLootingFBFinishInfo::SCRelicLootingFBFinishInfo() : header(MT_RELIC_LOOTING_FB_FINISH_SC) {}
	SCRelicLootingRankInfo::SCRelicLootingRankInfo() : header(MT_RELIC_LOOTING_RANK_LIST_SC) {}
	SCRelicLootingAchievementInfo::SCRelicLootingAchievementInfo() : header(MT_RELIC_LOOTING_ACHIEVEMENT_SC) {}
	SCRelicLootingEffectInfo::SCRelicLootingEffectInfo() : header(MT_RELIC_LOOTING_SPECIAL_EFFECT_SC) {}

	SCPetMonopolyInfo::SCPetMonopolyInfo() : header(MT_PET_MONOPOLY_INFO_SC) {}
	CSPetMonopolyOperate::CSPetMonopolyOperate() : header(MT_PET_MONOPOLY_OPERATE_CS) {}
	SCPetMonopolyExpectPetChangeNotice::SCPetMonopolyExpectPetChangeNotice() : header(MT_PET_MONOPOLY_EXPECT_PET_SET_SC) {}
	SCPetMonopolyRollDice::SCPetMonopolyRollDice() : header(MT_PET_MONOPOLY_ROLL_DICE_RESULT_SC) {}
	SCPetMonopolyCurGridChangeNotice::SCPetMonopolyCurGridChangeNotice() : header(MT_PET_MONOPOLY_CUR_GRID_CHANGE_NOTICE_SC) {}
	SCPetMonopolyRewardRecord::SCPetMonopolyRewardRecord() : header(MT_PET_MONOPOLY_REWARD_RECORD_SC) {}
	SCPetMonopolyEnterNextLevelNotice::SCPetMonopolyEnterNextLevelNotice() : header(MT_PET_MONOPOLY_ENTER_NEXT_LEVEL_SC) {}
	SCPetMonopolyTurntable::SCPetMonopolyTurntable() : header(MT_PET_MONOPOLY_TURN_TABLE_SC) {}
	SCPetMonopolyTurntableResult::SCPetMonopolyTurntableResult() : header(MT_PET_MONOPOLY_TURN_TABLE_RESULT_SC) {}
	SCPetMonopolySpecialItem::SCPetMonopolySpecialItem() : header(MT_PET_MONOPOLY_SPECIAL_ITEM) {}
	SCPetMonopolyWishCardGrid::SCPetMonopolyWishCardGrid() : header(MT_PET_MONOPOLY_SPECIAL_ITEM_USE) {}

	CSCloudArenaOperate::CSCloudArenaOperate() : header(MT_CLOUD_ARENA_OPERATE_CS) {}
	SCCloudArenaSelfInfo::SCCloudArenaSelfInfo() : header(MT_CLOUD_ARENA_SELF_INFO_SC) {}
	SCCloudArenaTargetInfo::SCCloudArenaTargetInfo() : header(MT_CLOUD_ARENA_TARGET_INFO_SC) {}
	SCCloudArenaNewFightRecordNotice::SCCloudArenaNewFightRecordNotice() : header(MT_CLOUD_ARENA_NEW_FIGHT_RECORD_NOTICE_SC) {}
	SCCloudArenaFightRecordList::SCCloudArenaFightRecordList() : header(MT_CLOUD_ARENA_FIGHT_RECORD_LIST_SC) {}
	SCCloudArenaRankList::SCCloudArenaRankList() : header(MT_CLOUD_ARENA_RANK_LIST_SC) {}
	SCCloudArenaNoticeSeasonInitialScore::SCCloudArenaNoticeSeasonInitialScore() : header(MT_CLOUD_ARENA_SEASON_INITIAL_SCORE_NOTICE) {}
	SCCloudArenaNoticeDanChange::SCCloudArenaNoticeDanChange() : header(MT_CLOUD_ARENA_NOTICE_DAN_CHANGE) {}
	SCCloudArenaAchievementInfo::SCCloudArenaAchievementInfo() : header(MT_CLOUD_ARENA_ACHIEVEMENT_INFO) {}
	SCCloudArenaAchievementUpdateNotice::SCCloudArenaAchievementUpdateNotice() : header(MT_CLOUD_ARENA_ACHIEVEMENT_UPDATE_NOTICE) {}
	SCCloudArenaRoleSeasonHighestData::SCCloudArenaRoleSeasonHighestData() : header(MT_CLOUD_ARENA_ROLE_SEASON_HIGHEST_DATA_SC) {}
	SCCloudArenaRealTimeInfo::SCCloudArenaRealTimeInfo() : header(MT_CLOUD_ARENA_REAL_TIME_INFO) {}
	CSCloudArenaRealTimeInfo::CSCloudArenaRealTimeInfo() : header(MT_CLOUD_ARENA_REAL_TIME_INFO_REQ) {}
	SCCloudArenaFormationInfo::SCCloudArenaFormationInfo() : header(MT_CLOUD_ARENA_FORMATION_INFO_SC) {}
	CSCloudArenaSetFormation::CSCloudArenaSetFormation() : header(MT_CLOUD_ARENA_SET_FORMATION_REQ) {}

	// 移动辅助
	SCMoveAssistReq::SCMoveAssistReq() : header(MT_MOVE_ASSIST_REQ_SC) {}
	CSMoveAssistRet::CSMoveAssistRet() : header(MT_MOVE_ASSIST_RET_CS) {}

	CSChivalrousRankListReq::CSChivalrousRankListReq() : header(MT_RANK_GET_CHIVALROUS_REQ_CS) {}
	SCChivalrousRankList::SCChivalrousRankList() : header(MT_RANK_GET_CHIVALROUS_RESP_SC) {}

	SCWorldLevelInfo::SCWorldLevelInfo() : header(MT_WORLD_LEVEL_INFO_SC) {}

	// 角色参与活动奖励  保留 3692  ~ 3694
	CSRoleJoinActivityRewardReq::CSRoleJoinActivityRewardReq() : header(MT_ROLE_JOIN_ACTIVITY_REWARD_REQ_CS) {}
	SCRoleJoinActivityRewardInfo::SCRoleJoinActivityRewardInfo() : header(MT_ROLE_JOIN_ACTIVITY_REWARD_INFO_SC) {}
	SCRoleOneKeyFinishRewardInfo::SCRoleOneKeyFinishRewardInfo(): header(MT_ROLE_JOIN_ACTIVITY_ONE_CLICK_FINISH_INFO_SC){}
	
	// 帮派活跃度 礼包及排行榜
	CSGuildActivePersonRank::CSGuildActivePersonRank() : header(MT_GUILD_ACTIVE_PERSON_RANK_CS) {}
	SCGuildActiveInfo::SCGuildActiveInfo() : header(MT_GUILD_ACTIVE_INFO_SC) {}
	SCGuildActiveInfoMember::SCGuildActiveInfoMember() : header(MT_GUILD_ACTIVE_INFO_MEMBER_SC) {}
	SCGuildActivePersonRank::SCGuildActivePersonRank() : header(MT_GUILD_ACTIVE_PERSON_RANK_SC) {}
	CSGuildActiveGetGift::CSGuildActiveGetGift() : header(MT_GUILD_ACTIVE_GET_GIFT_CS) {}

	// 自定义指令
	SCCustomInstructionInfo::SCCustomInstructionInfo() : header(MT_CUSTOM_INSTRUCTION_INFO_SC) {}
	CSCustomInstructionAdd::CSCustomInstructionAdd() : header(MT_CUSTOM_INSTRUCTION_ADD_CS) {}
	CSCustomInstructionRemove::CSCustomInstructionRemove() : header(MT_CUSTOM_INSTRUCTION_REMOVE_CS) {}
	CSCustomInstructionBatchRemove::CSCustomInstructionBatchRemove() : header(MT_CUSTOM_INSTRUCTION_BATCH_REMOVE_CS) {}

	// 家族灵脉
	CSGuildInbornReq::CSGuildInbornReq() : header(MT_GUILD_INBORN_REQ_CS) {}
	SCGuildInbornAllInfo::SCGuildInbornAllInfo() : header(MT_GUILD_INBORN_ALL_INFO_SC) {}
	SCGuildInbornSingleInfo::SCGuildInbornSingleInfo() : header(MT_GUILD_INBORN_SINGLE_INFO_SC) {}
	SCGuildInbornSingleTypeInfo::SCGuildInbornSingleTypeInfo() : header(MT_GUILD_INBORN_SINGLE_TYPE_INFO_SC) {}

	//神印之地
	CSPetGodFBReq::CSPetGodFBReq() : header(MT_PETY_GOD_FB_REQ_CS) {}
	SCPetGodFBRoleInfo::SCPetGodFBRoleInfo() : header(MT_PETY_GOD_FB_ROLE_INFO_SC) {}
	SCPetGodFBSceneInfo::SCPetGodFBSceneInfo() : header(MT_PETY_GOD_FB_SCENE_INFO_SC) {}

	//渡劫
	CSDuJieReq::CSDuJieReq() : header(MT_DU_JIE_REQ_CS) {}
	SCDuJieRoleInfo::SCDuJieRoleInfo() : header(MT_DU_JIE_ROLE_INFO) {}
	SCDuJieFBInfo::SCDuJieFBInfo() : header(MT_DU_JIE_FB_INFO) {}


	// 红包
	CSHongBaoClientReq::CSHongBaoClientReq() : header(MT_HONG_BAO_CLIENT_REQ_CS) {}
	SCHongBaoSendList::SCHongBaoSendList() : header(MT_HONG_BAO_SEND_LIST_SC) {}
	SCHongBaoGetList::SCHongBaoGetList() : header(MT_HONG_BAO_GET_LIST_SC) {}
	SCHongBaoBaseInfo::SCHongBaoBaseInfo() : header(MT_HONG_BAO_BASE_INFO_SC) {}
	SCHongBaoSendListChange::SCHongBaoSendListChange() : header(MT_HONG_BAO_SEND_LIST_CHANGE_SC) {}
	SCHongBaoGetListChange::SCHongBaoGetListChange() : header(MT_HONG_BAO_GET_LIST_CHANGE_SC) {}
	SCHongBaoBaseInfoChange::SCHongBaoBaseInfoChange() : header(MT_HONG_BAO_BASE_INFO_CHANGE_SC) {}

	// 伙伴--元神契约
	SCPartnerSpiritPactList::SCPartnerSpiritPactList() : header(MT_PARTNER_PACT_CHANGE_LIST_SC) {}
	SCPartnerPactBaseAttr::SCPartnerPactBaseAttr() : header(MT_PARTNER_PACT_BASE_ATTR_SC) {}
	SCPartnerPactInfo::SCPartnerPactInfo() : header(MT_PARTNER_PACT_PACTINFO_SC) {}

	// 周常
	SCWeekActiveDegreeInfo::SCWeekActiveDegreeInfo() : header(MT_WEEK_ACTIVE_DEGREE_INFO_SC) {}

	// 百鬼夜行
	SCHundredGhostInfo::SCHundredGhostInfo() : header(MT_HUNDRED_GHOST_INFO_SC) {}
	SCHundredGhostPokerInfo::SCHundredGhostPokerInfo() : header(MT_HUNDRED_GHOST_POKER_INFO_SC) {}

	//北斗七星
	CSBigDipperReq::CSBigDipperReq() : header(MT_BIG_DIPPER_REQ_CS) {}
	SCBigDipperInfo::SCBigDipperInfo() : header(MT_BIG_DIPPER_INFO_SC) {}
	SCBigDipperMyInfo::SCBigDipperMyInfo() : header(MT_BIG_DIPPER_MY_INFO_SC) {}
	SCFallenGodInfo::SCFallenGodInfo() : header(MT_FALLEN_GOD_INFO_SC) {}
	CSFallenGodReq::CSFallenGodReq() : header(MT_FALLEN_GOD_REQ_CS) {}

	//天地道痕
	CSTianDiDaoHenReq::CSTianDiDaoHenReq() : header(MT_TIAN_DI_DAO_HEN_REQ_CS) {}
	SCTianDiDaoHenRoleInfo::SCTianDiDaoHenRoleInfo() : header(MT_TIAN_DI_DAO_HEN_ROLE_INFO_SC) {}
	SCTianDiDaoHenOpenInfo::SCTianDiDaoHenOpenInfo() : header(MT_TIAN_DI_DAO_HEN_OPEN_INFO_SC) {}


	CSGiftReq::CSGiftReq() : header(MT_GIVE_GIFT_REQ_CS) {}
	SCGiftRoleInfo::SCGiftRoleInfo() : header(MT_GIVE_GIFT_ROLE_INFO_SC) {}
	SCGetGiftInfo::SCGetGiftInfo() : header(MT_GIVE_GIFT_INFO_SC) {}

	//获取资源下载奖励
	CSFetchRecourceDownloadReward::CSFetchRecourceDownloadReward() : header(MT_FETCH_RESOURCE_DOWNLOAD_REWARD_CS) {}
	SCFetchRecourceDownloadReward::SCFetchRecourceDownloadReward() : header(MT_FETCH_RESOURCE_DOWNLOAD_REWARD_SC) {}

	SCPrestigeTaskInfo::SCPrestigeTaskInfo() : header(MT_PRESTIGE_TASK_INFO_SC) {}
	CSPrestigeTaskSelectVillage::CSPrestigeTaskSelectVillage() : header(MT_PRESTIGE_TASK_SELECT_VILLAGE_CS) {}
	SCPrestigeTaskFinalNotice::SCPrestigeTaskFinalNotice() : header(MT_PRESTIGE_TASK_FINAL_NOTICE) {}
	CSPrestigeTaskReq::CSPrestigeTaskReq() : header(MT_PRESTIGE_TASK_REQ_CS) {}
	SCTaskWaitingList::SCTaskWaitingList() : header(MT_TASK_WAITING_LIST) {}

	CSSpecialAnecdoteReq::CSSpecialAnecdoteReq() : header(MT_SPECIAL_ANECDOTE_STAR_CS) {}
	SCSpecialAnecdoteList::SCSpecialAnecdoteList() : header(MT_SPECIAL_ANECDOTE_TASK_LIST_SC) {}
	SCSpecialAnecdoteSingleList::SCSpecialAnecdoteSingleList() : header(MT_SPECIAL_ANECDOTE_TASK_SINGLE_SC) {}

	CSSvipClientCustomData::CSSvipClientCustomData() : header(MT_SVIP_CLIENT_CUSTOM_DATA_CS) {}
	SCSvipClientCustomData::SCSvipClientCustomData() : header(MT_SVIP_CLIENT_CUSTOM_DATA_SC) {}

	SCRAZhenPinShangPu::SCRAZhenPinShangPu() : header(MT_RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_INFO_SC) {}
	SCRAZhenPinShangPuOpenDay::SCRAZhenPinShangPuOpenDay() : header(MT_RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_OPEN_DAY_SC) {}

	// 福牛报喜
	CSCowReportGoodNewsClientReq::CSCowReportGoodNewsClientReq() : header(MT_COW_REPORT_GOOD_NEWS_CLIENT_REQ_CS) {}
	SCCowReportGoodNewsRewardListInfo::SCCowReportGoodNewsRewardListInfo() : header(MT_COW_REPORT_GOOD_NEWS_REWARD_LIST_INFO_SC) {}
	SCCowReportGoodNewsInfo::SCCowReportGoodNewsInfo() : header(MT_COW_REPORT_GOOD_NEWS_INFO_SC) {}
	SCCowReportGoodNewsRoleBuyInfo::SCCowReportGoodNewsRoleBuyInfo() : header(MT_COW_REPORT_GOOD_NEWS_ROLE_BUY_INFO) {}
	

	// 寅虎纳福
	CSTigerBringsBlessingClientReq::CSTigerBringsBlessingClientReq() : header(MT_TIGER_BRINGS_BLESSING_CLIENT_REQ_CS) {}
	SCTigerBringsBlessingRewardListInfo::SCTigerBringsBlessingRewardListInfo() : header(MT_TIGER_BRINGS_BLESSING_REWARD_LIST_INFO_SC){}
	SCTigerBringsBlessingInfo::SCTigerBringsBlessingInfo() : header(MT_TIGER_BRINGS_BLESSING_INFO_SC){}
	SCTigerBringsBlessingRoleBuyInfo::SCTigerBringsBlessingRoleBuyInfo() : header(MT_TIGER_BRINGS_BLESSING_ROLE_BUY_INFO) {}

	//防诈骗奖励
	CSFangFraudRewardReq::CSFangFraudRewardReq() : header(MT_FANG_FRADU_REWARD_REQ_CS) {}
	SCFangFraudRewardInfo::SCFangFraudRewardInfo() : header(MT_FANG_FRADU_REWARD_INFO_SC) {}

	//属性丹
	CSRoleAttrPelletReq::CSRoleAttrPelletReq() : header(MT_ROLE_ATTR_PELLET_REQ_CS) {}
	SCRoleAttrPelletAllInfo::SCRoleAttrPelletAllInfo() : header(MT_ROLE_ATTR_PELLET_ALL_INFO_SC) {}
	SCRoleAttrPelletSingleInfo::SCRoleAttrPelletSingleInfo() : header(MT_ROLE_ATTR_PELLET_SINGLE_INFO_SC) {}
	//活跃转盘
	CSActiveTurntableReq::CSActiveTurntableReq() : header(MT_ACTIVE_TURNTABLE_REQ_CS) {}
	SCActiveTurntableInfo::SCActiveTurntableInfo() : header(MT_ACTIVE_TURNTABLE_INFO_SC) {}
	SCActiveTurntableRewardInfo::SCActiveTurntableRewardInfo() : header(MT_ACTIVE_TURNTABLE_REWARD_SC) {}

	CSCapacityTargetReq::CSCapacityTargetReq() : header(MT_CAPACITY_TARGET_REQ_CS) {}
	SCCapacityTargetInfo::SCCapacityTargetInfo() : header(MT_CAPACITY_TARGET_INFO_SC) {}

	//回归豪礼
	SCBackRewardInfo::SCBackRewardInfo() : header(MT_RAND_ACTIVITY_BACK_REWARD_INFO_SC) {}
	CSBackRewardReq::CSBackRewardReq() : header(MT_RAND_ACTIVITY_BACK_REWARD_REQ_CS) {}

	//决斗
	SCDuelBattleInfo::SCDuelBattleInfo() : header(MT_DUEL_BATTLE_INFO_SC) {}
	SCRoleDuelKillValueAOINotice::SCRoleDuelKillValueAOINotice() : header(MT_DUEL_KILL_VALUE_AOI_NOTICE_SC) {}

	CSAccessoriesMentOperaReq::CSAccessoriesMentOperaReq() : header(MT_ACCESSORIES_REQ_CS) {}
	SCAccessoriesMentInfo::SCAccessoriesMentInfo() : header(MT_ACCESSORIES_INFO_SC) {}
	SCAccessoriesMentChange::SCAccessoriesMentChange() : header(MT_ACCESSORIES_SIGNLE_SC) {}
	CSAccessoriesMentVocComposeReq::CSAccessoriesMentVocComposeReq() : header(MT_ACCESSORIES_VOC_COMPOSE_CS) {}
	SCAccessoriesSpecialEquip::SCAccessoriesSpecialEquip() : header(MT_ACCESSORIES_SPECAIL_EQUIP_SC) {}

	//连连乐翻天
	SCRAHappyLookInfo::SCRAHappyLookInfo() : header(MT_RAND_ACTIVITY_HAPPY_LOOK_INFO_SC) {}
	//登录送好礼
	SCRASignHappyInfo::SCRASignHappyInfo() : header(MT_RAND_ACTIVITY_SIGN_HAPPY_INFO_SC) {}
	
	// 天命
	SCTianMingInfo::SCTianMingInfo() : header(MT_TIAN_MING_INFO_SC) {}
	//阴阳眼
	CSYinYangEyeOpReq::CSYinYangEyeOpReq() : header(MT_YIN_YANG_EYE_OP_REQ_CS) {}
	SCYinYangEyeInfo::SCYinYangEyeInfo() : header(MT_YIN_YANG_EYE_INFO_SC) {}

	//万灵共盈
	SCRAWanLingGongYingInfo::SCRAWanLingGongYingInfo() : header(MT_RAND_ACTIVITY_WAN_LING_GONG_YING_INFO_SC) {}

	//魂器天成
	SCRAHorcruxTianChengRoleInfo::SCRAHorcruxTianChengRoleInfo() : header(MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_ROLE_INFO) {}
	SCRAHorcruxTianChengPoolInfo::SCRAHorcruxTianChengPoolInfo() : header(MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_POOL_INFO) {}
	SCRAHorcruxTianChengDrawResult::SCRAHorcruxTianChengDrawResult() : header(MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_DRAW) {}

	//整点抽奖
	SCRAIntegralDrawRoleInfo::SCRAIntegralDrawRoleInfo() : header(MT_RAND_ACTIVITY_INTEGRAL_DRAW_ROLE_INFO) {}
	SCRAIntegralDrawGameInfo::SCRAIntegralDrawGameInfo() : header(MT_RAND_ACTIVITY_INTEGRAL_DRAW_GAME_INFO) {}
	SCRAIntegralDrawResult::SCRAIntegralDrawResult() : header(MT_RAND_ACTIVITY_INTEGRAL_DRAW_RESULT) {}

	//无尽之塔
	SCEndlessTowerInfo::SCEndlessTowerInfo() : header(MT_ENDLESS_TOWER_INFO_SC) {}
	CSEndlessTowerReq::CSEndlessTowerReq() : header(MT_ENDLESS_TOWER_REQ_CS) {}

	//飞升
	SCFlyUpSystemInfo::SCFlyUpSystemInfo() : header(MT_FLY_UP_SYSTEM_INFO_SC) {}
	CSFlyUpSystemReq::CSFlyUpSystemReq() : header(MT_FLY_UP_SYSTEM_REQ_CS) {}
	SCFlyUpSystemAdvInfo::SCFlyUpSystemAdvInfo() : header(MT_FLY_UP_SYSTEM_ADV_INFO_SC) {}
	SCFlyUpSkillSigleInfo::SCFlyUpSkillSigleInfo() : header(MT_FLY_UP_SKILL_SIGLE_INFO_SC) {}

	//琉璃蜃境
	CSMirageTerritoryReq::CSMirageTerritoryReq() : header(MT_MIRAGE_TERRITORY_REQ_CS) {}
	SCMirageTerritoryInfo::SCMirageTerritoryInfo() : header(MT_MIRAGE_TERRITORY_INFO_SC) {}
	CSMirageTerritorySetPartnerReq::CSMirageTerritorySetPartnerReq() : header(MT_MIRAGE_TERRITORY_SET_PARTNER_REQ_CS) {}
	SCMirageTerritorySetPartnerInfo::SCMirageTerritorySetPartnerInfo() : header(MT_MIRAGE_TERRITORY_PARTNER_INFO_SC) {}

	//随机活动 山海聚宝
	SCRAShanHaiJuBaoInfo::SCRAShanHaiJuBaoInfo() : header(MT_SHAN_HAI_JU_BAO_ROLE_INFO_SC) {}
	SCRAShanHaiJuBaoDrawRet::SCRAShanHaiJuBaoDrawRet() : header(MT_SHAN_HAI_JU_BAO_DRAW_RET_SC) {}

	//随机活动 山海初遇
	SCRAShanHaiMetBase::SCRAShanHaiMetBase() : header(MT_SHAN_HAI_MET_ROLE_INFO_SC) {}
	SCRAShanHaiMetTakeParam::SCRAShanHaiMetTakeParam() : header(MT_SHAN_HAI_MET_TASK_PARAM_SC) {}
	SCRAShanHaiMetTaskParamChange::SCRAShanHaiMetTaskParamChange() : header(MT_SHAN_HAI_MET_TASK_CHANGE_PARAM_SC) {}
	
	//再战云泽
	SCRAZaiZhanYunZeBase::SCRAZaiZhanYunZeBase() : header(MT_ZAI_ZHAN_YUN_ZE_INFO_SC) {}

	//随机活动 纷享加倍
	SCRAColorfulDoubleBase::SCRAColorfulDoubleBase() : header(MT_COLORFUL_DOUBLE_ROLE_INFO_SC) {}
	SCRAColorfulDoubleTakeParam::SCRAColorfulDoubleTakeParam() : header(MT_COLORFUL_DOUBLE_TASK_PARAM_SC) {}
	SCRAColorfulDoubleTaskParamChange::SCRAColorfulDoubleTaskParamChange() : header(MT_COLORFUL_DOUBLE_TASK_CHANGE_PARAM_SC) {}

	
	// 好友亲密度
	SCFriendIntimacyListInfo::SCFriendIntimacyListInfo() : header(MT_FRIEND_INTIMACY_LIST_INFO_SC) {}
	SCFriendIntimacyInfoChange::SCFriendIntimacyInfoChange() : header(MT_FRIEND_INTIMACY_INFO_CHANGE_SC) {}

	//结义
	CSJieYiReq::CSJieYiReq() : header(MT_JIE_YI_REQ_CS) {}
	CSJieYiCreate::CSJieYiCreate() : header(MT_JIE_YI_CREATE_CS) {}
	CSJieYiCreateName::CSJieYiCreateName() : header(MT_JIE_YI_CREATE_NAME_CS) {}
	SCJieYiCreateInfo::SCJieYiCreateInfo() : header(MT_JIE_YI_CREATE_INFO_SC) {}
	SCJieYiCreateNameUpdate::SCJieYiCreateNameUpdate() : header(MT_JIE_YI_CREATE_NAME_UPDATE_SC) {}
	SCJieYiCreateConfirm::SCJieYiCreateConfirm() : header(MT_JIE_YI_CREATE_CONFIRM_SC) {}
	SCJieYiCreateCancel::SCJieYiCreateCancel() : header(MT_JIE_YI_CREATE_CANCEL_SC) {}
	SCJieYiZuInfo::SCJieYiZuInfo() : header(MT_JIE_YI_ZU_INFO_SC) {}
	CSJieYiZuMemberName::CSJieYiZuMemberName() : header(MT_JIE_YI_ZU_MEMBER_NAME_CS) {}
	CSJieYiZuText::CSJieYiZuText() : header(MT_JIE_YI_ZU_TEXT_CS) {}
	CSJieYiZuName::CSJieYiZuName() : header(MT_JIE_YI_ZU_NAME_CS) {}
	SCJieYiInviteRoute::SCJieYiInviteRoute() : header(MT_JIE_YI_ZU_ROUTE_SC) {}
	SCJieYiRoleXuanYanList::SCJieYiRoleXuanYanList() : header(MT_JIE_YI_ZU_ROLE_XUAN_YAN_LIST_SC) {}
	SCJieYiZuXuanYanList::SCJieYiZuXuanYanList() : header(MT_JIE_YI_ZU_XUAN_YAN_LIST_SC) {}
	CSJieYiZuXuanYanReq::CSJieYiZuXuanYanReq() : header(MT_JIE_YI_ZU_XUAN_YAN_REQ_CS) {}
	CSJieYiZuXuanYanReqList::CSJieYiZuXuanYanReqList() : header(MT_JIE_YI_ZU_XUAN_YAN_REQ_LIST_CS) {}
	SCJieYiZuLeaveNotice::SCJieYiZuLeaveNotice() : header(MT_JIE_YI_ZU_KICK_NOTICE_SC) {}
	SCJieYiZuRoleOnlineStatusNotice::SCJieYiZuRoleOnlineStatusNotice() : header(MT_JIE_YI_ZU_ROLE_ONLINE_STATUS_SC) {}

	//情缘
	CSQingYuanReq::CSQingYuanReq() : header(MT_QING_YUAN_REQ_CS) {}
	SCQingYuanInviteRoute::SCQingYuanInviteRoute() : header(MT_QING_YUAN_INVITE_ROUTE_SC) {}
	SCQingYuanInfo::SCQingYuanInfo() : header(MT_QING_YUAN_INFO_SC) {}
	SCQingYuanLeaveRoute::SCQingYuanLeaveRoute() : header(MT_QING_YUAN_LEAVE_ROUTE_SC) {}
	SCQingYuanInviteRet::SCQingYuanInviteRet() : header(MT_QING_YUAN_INVITE_RET_SC) {}
	SCQingYuanLeaveNotice::SCQingYuanLeaveNotice() : header(MT_QING_YUAN_LEAVE_NOTICE_SC) {}
	SCQingYuanCreateNotice::SCQingYuanCreateNotice() : header(MT_QING_YUAN_CREATE_NOTICE_SC) {}

	// 随机活动 招财进宝
	SCRAZhaoCaiJinBaoRoleInfo::SCRAZhaoCaiJinBaoRoleInfo() : header(MT_RAND_ACTIVITY_ZHAOCAIJINBAO_ROLE_INFO_SC) {}
	SCRAZhaoCaiJinBaoDrawResult::SCRAZhaoCaiJinBaoDrawResult() : header(MT_RAND_ACTIVITY_ZHAOCAIJINBAO_DRAW_RESULT_SC) {}
	SCRAZhaoCaiJinBaoRecordListInfo::SCRAZhaoCaiJinBaoRecordListInfo() : header(MT_RAND_ACTIVITY_ZHAOCAIJINBAO_RECORD_LIST_INFO_SC) {}

	//魔族在哪里
	SCRAMoZuZaiNaLiInfo::SCRAMoZuZaiNaLiInfo() : header(MT_RAND_ACTIVITY_MO_ZU_ZAI_NA_LI_SC) {}

	//超值纷享
	SCRAChaoZhiFenXiangInfo::SCRAChaoZhiFenXiangInfo() : header(MT_RAND_ACTIVITY_CHAOZHI_FENXIANG) {}
	// 职业晋阶 - 子阶
	SCAdvanceCareerSubInfo::SCAdvanceCareerSubInfo() : header(MT_ADVANCE_CAREER_SUB_INFO_SC) {}

	//山海花灯
	SCRAShanHaiHuaDengAllInfo::SCRAShanHaiHuaDengAllInfo() : header(MT_RAND_ACTIVITY_SHANHAI_HUADENG_ALL_INFO_SC) {}
	SCRAShanHaiHuaDengGameInfo::SCRAShanHaiHuaDengGameInfo() : header(MT_RAND_ACTIVITY_SHANHAI_HUADENG_GAME_INFO_SC) {}
	SCRAShanHaiHuaDengLightInfo::SCRAShanHaiHuaDengLightInfo() : header(MT_RAND_ACTIVITY_SHANHAI_HUADENG_LIGHT_INFO_SC) {}


	//山海宝箱 
	SCRAShanHaiBaoXiangInfo::SCRAShanHaiBaoXiangInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_BAO_XIANG_INFO_SC) {}
	//山海宝盒 
	SCRAShanHaiBaoHeInfo::SCRAShanHaiBaoHeInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_BAO_HE_INFO_SC) {}
	//山海宝袋 
	SCRAShanHaiBaoDaiInfo::SCRAShanHaiBaoDaiInfo() : header(MT_RAND_ACTIVITY_SHAN_HAI_BAO_DAI_INFO_SC) {}


	//万灵话本
	CSWanLingFBReq::CSWanLingFBReq() : header(MT_WAN_LING_FB_REQ_CS) {}
	SCWanLingFBRoleInfo::SCWanLingFBRoleInfo() : header(MT_WAN_LING_FB_ROLE_INFO_SC) {}
	SCWanLingFBInfo::SCWanLingFBInfo() : header(MT_WAN_LING_FB_INFO_SC) {}

	//吉星高照
	SCRAJiXingGaoZhaoRoleInfo::SCRAJiXingGaoZhaoRoleInfo() : header(MT_RAND_ACTIVITY_JIXING_GAOZHAO_ROLE_INFO_SC) {}
	SCRAJiXingGaoZhaoPoolInfo::SCRAJiXingGaoZhaoPoolInfo() : header(MT_RAND_ACTIVITY_JIXING_GAOZHAO_POOL_INFO_SC) {}
	SCRAJiXingGaoZhaoDrawResult::SCRAJiXingGaoZhaoDrawResult() : header(MT_RAND_ACTIVITY_JIXING_GAOZHAO_DRAW_SC) {}
	SCRAJiXingGaoZhaoRecordListInfo::SCRAJiXingGaoZhaoRecordListInfo() : header(MT_RAND_ACTIVITY_JIXING_GAOZHAO_RECORD_LIST_INFO_SC) {}
	SCRAJiXingGaoZhaoRecordInfo::SCRAJiXingGaoZhaoRecordInfo() : header(MT_RAND_ACTIVITY_JIXING_GAOZHAO_RECORD_INFO_SC) {}


	// 随机活动 限时累充
	SCRAXianShiLeiChongInfo::SCRAXianShiLeiChongInfo() : header(MT_XIAN_SHI_LEI_CHONG_INFO_SC) {}

	// 周末礼包
	SCRAWeekendGiftInfo::SCRAWeekendGiftInfo() : header(MT_WEEKEND_GIFT_INFO) {}

	// 超值献礼
	SCRAChaoZhiGiftInfo::SCRAChaoZhiGiftInfo() : header(MT_CHAO_ZHI_GIFT_INFO) {}


	// 修为
	CSXiuWeiReq::CSXiuWeiReq() : header(MT_XIU_WEI_REQ_CS) {}
	SCXiuWeiInfo::SCXiuWeiInfo() : header(MT_XIU_WEI_INFO_SC) {}
	SCXiuWeiInfoChange::SCXiuWeiInfoChange() : header(MT_XIU_WEI_INFO_CHANGE_SC) {}
	SCXiuWeiDanYaoLimitInfo::SCXiuWeiDanYaoLimitInfo() : header(MT_XIU_WEI_DAN_YAO_LIMIT_INFO_SC) {}

	// 九州秘宝
	CSTreasureTaskReq::CSTreasureTaskReq() : header(MT_TREASURE_TASK_REQ_CS) {}
	SCTreasureTaskInfo::SCTreasureTaskInfo() : header(MT_TREASURE_TASK_INFO_SC) {}

	//寅虎问世
	SCRAYinHuWenShiInfo::SCRAYinHuWenShiInfo() : header(MT_RAND_ACTIVITY_YINHU_WENSHI_INFO_SC) {}

	//辞牛迎新虎
	SCRACiNiuYingXinHuInfo::SCRACiNiuYingXinHuInfo() : header(MT_RAND_ACTIVITY_CINIU_YINGXINHU_INFO_SC) {}

	//虎行大运
	SCRAHuXingDaYunInfo::SCRAHuXingDaYunInfo() : header(MT_RAND_ACTIVITY_HU_XING_DA_YUN_INFO_SC) {}
	SCRAHuXingDaYunBeInvitedInfo::SCRAHuXingDaYunBeInvitedInfo() : header(MT_RAND_ACTIVITY_HU_XING_DA_YUN_USER_LIST_SC) {}

	//寅虎问世
	SCRAYinHuBaiSuiInfo::SCRAYinHuBaiSuiInfo() : header(MT_RAND_ACTIVITY_YIN_HU_BAI_SUI_INFO_SC) {}
	//竞技月卡
	SCRAJingJiYueKaAllInfo::SCRAJingJiYueKaAllInfo() : header(MT_RAND_ACTIVITY_JINGJI_YUEKA_ALL_INFO_SC) {}
	SCRAJingJiYueKaNotice::SCRAJingJiYueKaNotice() : header(MT_RAND_ACTIVITY_JINGJI_YUEKA_NOTICE_SC) {}
	SCRAJingJiYueKaRefreshNotice::SCRAJingJiYueKaRefreshNotice() : header(MT_RAND_ACTIVITY_JINGJI_YUEKA_REFRESH_NOTICE_SC) {}
	SCRAJingJiYueKaSingleTypeInfo::SCRAJingJiYueKaSingleTypeInfo() : header(MT_RAND_ACTIVITY_JINGJI_YUEKA_SINGLE_TYPE_INFO_SC) {}
	SCRAJingJiYueKaTaskListInfo::SCRAJingJiYueKaTaskListInfo() : header(MT_RAND_ACTIVITY_JINGJI_YUEKA_TASK_LIST_INFO_SC) {}

	//情缘 婚宴
	CSWeddingFBReq::CSWeddingFBReq() : header(MT_WEDDING_FB_REQ_CS) {}
	SCWeddingFBSceneInfo::SCWeddingFBSceneInfo() : header(MT_WEDDING_FB_SCENE_INFO_SC) {}
	SCWeddingFBRoleInfo::SCWeddingFBRoleInfo() : header(MT_WEDDING_FB_ROLE_SC) {}
	SCWeddingFBSync::SCWeddingFBSync() : header(MT_WEDDING_FB_SYNC_SC) {}

	SCWeddingRegistList::SCWeddingRegistList() : header(MT_WEDDING_REGIST_LIST_SC) {}
	SCWeddingInviteList::SCWeddingInviteList() : header(MT_WEDDING_INVITE_LIST_SC) {}
	SCWeddingApplyList::SCWeddingApplyList() : header(MT_WEDDING_APPLY_LIST_SC) {}
	SCWeddingChange::SCWeddingChange() : header(MT_WEDDING_CHANGE_SC) {}

	SCWeddingBaseInfo::SCWeddingBaseInfo() : header(MT_WEDDING_BASE_INFO_SC) {}
	SCWeddingStatusSync::SCWeddingStatusSync() : header(MT_WEDDING_STATUS_SYNC_SC){}
	SCWeddingShowInfo::SCWeddingShowInfo() : header(MT_WEDDING_SHOW_INFO_SC) {}

	SCWeddingSubAct_XubQingMiAi::SCWeddingSubAct_XubQingMiAi() : header(MT_WEDDING_FB_SUB_ACT_XUN_QING_MI_AI_SC) {}
	SCWeddingSubAct_XinXinXiangYin::SCWeddingSubAct_XinXinXiangYin() : header(MT_WEDDING_FB_SUB_ACT_XIN_XIN_XIANG_YIN_SC) {}
	SCWeddingSubAct_BaoWeiXiuQing::SCWeddingSubAct_BaoWeiXiuQing() : header(MT_WEDDING_FB_SUB_ACT_BAO_WEI_XIU_QIU_SC) {}
	SCWeddingSubAct_ZaoShengGuiZi::SCWeddingSubAct_ZaoShengGuiZi() : header(MT_WEDDING_FB_SUB_ACT_ZAO_SHENG_GUI_ZI_SC) {}

	SCXunYouCarStatusInfoSync::SCXunYouCarStatusInfoSync() : header(MT_XUN_YOU_CAR_STATUS_INFO_SYNC) {}

	

	//论剑九州
	SCWorldArenaInfo::SCWorldArenaInfo() : header(MT_WORLD_ARENA_INFO_SC) {}
	SCWorldArenaUserSignupInfo::SCWorldArenaUserSignupInfo() : header(MT_WORLD_ARENA_SIGNUP_INFO_SC) {}
	SCWorldArenaUserScheduleInfo::SCWorldArenaUserScheduleInfo() : header(MT_WORLD_ARENA_SCHEDULE_INFO_SC) {}
	CSWorldArenaOperate::CSWorldArenaOperate() : header(MT_WORLD_ARENA_OPERATE_CS) {}
	SCWorldArenaUserBaseInfo::SCWorldArenaUserBaseInfo() : header(MT_WORLD_ARENA_USER_BASE_INFO_SC) {}
	SCWorldArenaGroupRankList::SCWorldArenaGroupRankList() : header(MT_WORLD_ARENA_GROUP_RANK_LIST_SC) {}
	SCWorldArenaCountDownToFight::SCWorldArenaCountDownToFight() : header(MT_WORLD_ARENA_COUNT_DOWN_SC_TO_FIGHT) {}
	SCWorldArenaStartEnterSceneNotice::SCWorldArenaStartEnterSceneNotice() : header(MT_WORLD_ARENA_START_ENTER_SCENE_NOTICE) {}
	SCWorldArenaShowUpInfo::SCWorldArenaShowUpInfo() : header(MT_WORLD_ARENA_SHOWUP_INFO) {}

	//情缘时装
	SCRAQingYuanShiZhuangInfo::SCRAQingYuanShiZhuangInfo() : header(MT_RA_QING_YUAN_SHI_ZHUANG_INFO_SC) {}
	//情缘累充
	SCRAQingYuanLeiChongInfo::SCRAQingYuanLeiChongInfo() : header(MT_RA_QING_YUAN_LEI_CHONG_INFO_SC) {}

	SCMiniGameInfo::SCMiniGameInfo() : header(MT_MINI_GAME_INFO_SC) {}
	CSMiniGameFinishNextStage::CSMiniGameFinishNextStage() : header(MT_MINI_GAME_FINISH_STAGE_CS) {}
	CSMiniGameClaimPhaseReward::CSMiniGameClaimPhaseReward() : header(MT_MINI_GAME_CLAIM_PHASE_REWARD_CS) {}
	CSMiniGameStartBattle::CSMiniGameStartBattle() : header(MT_MINI_GAME_START_BATTLE) {}
	CSMiniGameSetMiniGameId::CSMiniGameSetMiniGameId() : header(MT_MINI_GAME_SET_MINI_GAME_ID) {}

	//英雄会武
	CSWorldTeamArenaReq::CSWorldTeamArenaReq() : header(MT_WORLD_TEAM_ARENA_REQ_CS) {}
	SCWorldTeamArenaAllSimpleInfo::SCWorldTeamArenaAllSimpleInfo() : header(MT_WORLD_TEAM_ARENA_ALL_SIGN_UP_TEAM_SC) {}
	CSWorldTeamArenaSignUpReq::CSWorldTeamArenaSignUpReq() : header(MT_WORLD_TEAM_ARENA_SIGN_UP_REQ_CS) {}
	SCWorldTeamArenaPreTeamSignUpInfo::SCWorldTeamArenaPreTeamSignUpInfo() : header(MT_WORLD_TEAM_ARENA_PRE_TEAM_SIGN_UP_INFO_SC) {}
	SCWorldTeamArenaNoticeInfo::SCWorldTeamArenaNoticeInfo() : header(MT_WORLD_TEAM_ARENA_NOTICE_INFO_SC) {}
	SCWorldTeamArenaTeamSignUpDetailedInfo::SCWorldTeamArenaTeamSignUpDetailedInfo() : header(MT_WORLD_TEAM_ARENA_TEAM_SIGN_UP_DETAILED_INFO_SC) {}
	SCActivityWorldTeamArenaInfo::SCActivityWorldTeamArenaInfo() : header(MT_WORLD_TEAM_ARENA_ACTIVITY_INFO_SC) {}
	CSActivityWorldTeamArenaSceneReq::CSActivityWorldTeamArenaSceneReq() : header(MT_WORLD_TEAM_ARENA_SCENE_OP_REQ_CS) {}
	SCActivityWorldTeamArenaRankAllInfo::SCActivityWorldTeamArenaRankAllInfo() : header(MT_WORLD_TEAM_ARENA_ALL_RANK_INFO_SC) {}
	SCActivityWorldTeamArenaMyInfo::SCActivityWorldTeamArenaMyInfo() : header(MT_WORLD_TEAM_ARENA_MY_RANK_INFO_SC) {}
	SCActivityWorldTeamArenaSpecialInfo::SCActivityWorldTeamArenaSpecialInfo() : header(MT_WORLD_TEAM_ARENA_SPECIAL_INFO_SC) {}
	SCActivityWorldTeamArenaTeamSpecialInfo::SCActivityWorldTeamArenaTeamSpecialInfo() : header(MT_WORLD_TEAM_ARENA_TEAM_SPECIAL_INFO_SC) {}
	SCActivityWorldTeamArenaFightInfo::SCActivityWorldTeamArenaFightInfo() : header(MT_WORLD_TEAM_ARENA_FIGHT_INFO_SC) {}
	CSActivityWorldTeamArenaRankReq::CSActivityWorldTeamArenaRankReq() : header(MT_WORLD_TEAM_ARENA_FINALLY_RANK_REQ_CS) {}
	SCActivityWorldTeamArenaFinallyRankInfo::SCActivityWorldTeamArenaFinallyRankInfo() : header(MT_WORLD_TEAM_ARENA_FINALLY_RANK_INFO_SC) {}
	SCActivityWorldTeamArenaOtherInfo::SCActivityWorldTeamArenaOtherInfo() : header(MT_WORLD_TEAM_ARENA_OTHER_INFO_SC) {}

	//天下至尊令
	CSPeaktokenRoleReq::CSPeaktokenRoleReq() : header(MT_PEAK_TOKEN_ROLE_REQ_CS) {}

	SCPeaktokenRoleStatusInfo::SCPeaktokenRoleStatusInfo() : header(MT_PEAK_TOKEN_ROLE_JOIN_STATUS_SC) {}
	SCPeaktokenRoleJoinInfo::SCPeaktokenRoleJoinInfo() : header(MT_PEAK_TOKEN_REWARD_INFO_SC) {}
	SCPeaktokenRoleInfo::SCPeaktokenRoleInfo() : header(MT_PEAK_TOKEN_ROLE_INFO_SC) {}


	// 收益统计
	CSIncomeRoleReq::CSIncomeRoleReq() : header(MT_INCOME_ROLE_REQ_CS) {}
	SCIncomeRoleInfo::SCIncomeRoleInfo() : header(MT_INCOME_ROLE_INFO_SC) {}

	//伙伴强化
	CSPartnerIntensifyReq::CSPartnerIntensifyReq() : header(MT_PARTNER_INTENSIFY_CS) {}
	SCPartnerIntensifyInfo::SCPartnerIntensifyInfo() : header(MT_PARTNER_INTENSIFY_INFO_SC) {}

	//师徒系统 优化 师徒传功 
	CSShiTuChuanGongFBReq::CSShiTuChuanGongFBReq() : header(MT_SHI_TU_CHAUN_GONG_FB_REQ) {}
	SCShiTuChuanGongGameInfo::SCShiTuChuanGongGameInfo() : header(MT_SHI_TU_CHAUN_GONG_GAME_INFO) {}
	SCShiTuChuanGongSceneInfo::SCShiTuChuanGongSceneInfo() : header(MT_SHI_TU_CHAUN_GONG_SCENE_INFO) {}
	SCShiTuSyncInfo::SCShiTuSyncInfo() : header(MT_SHI_TU_SYNC_INFO) {}
	SCShiTuOpNotice::SCShiTuOpNotice() : header(MT_SHI_TU_OP_RET_SC) {}
	
	//-------------------周期活动----------------------

	//情缘对对碰
	CSQingYuanDuiDuiPengReq::CSQingYuanDuiDuiPengReq() : header(MT_QINGYUAN_DUIDUIPENG_REQ_CS) {}
	SCQingYuanDuiDuiPengStartNotice::SCQingYuanDuiDuiPengStartNotice() : header(MT_QINGYUAN_DUIDUIPENG_START_SC) {}
	SCQingYuanDuiDuiRoleForm::SCQingYuanDuiDuiRoleForm() : header(MT_QINGYUAN_DUIDUIPENG_ROLE_FORM_SC) {}
	SCQingYuanDuiDuiFbInfo::SCQingYuanDuiDuiFbInfo() : header(MT_QINGYUAN_DUIDUIPENG_FB_INFO_SC) {}
	SCQingYuanDuiDuiFinish::SCQingYuanDuiDuiFinish() : header(MT_QINGYUAN_DUIDUIPENG_FINISH_SC) {}
	SCQingYuanDuiDuiRankInfo::SCQingYuanDuiDuiRankInfo() : header(MT_QINGYUAN_DUIDUIPENG_RANK_INFO_SC) {}
	SCQingYuanDuiDuiDayTimesInfo::SCQingYuanDuiDuiDayTimesInfo() : header(MT_QINGYUAN_DUIDUIPENG_DAY_TIMES_SC) {}

	SCWorldStatusHiddenInfo::SCWorldStatusHiddenInfo() : header(MT_WORLD_STATUS_HIDDEN_INFO_SC) {}

	SCZhouQiInfo::SCZhouQiInfo() : header(MT_ZHOU_QI_INFO_SC) {}

	//山海灯会
	LanternRiddieCSReq::LanternRiddieCSReq() : header(MT_LANTERN_RIDDIE_REQ_CS){}
	SALanternRiddieNPCInfo::SALanternRiddieNPCInfo() : header(MT_LANTERN_RIDDIE_NPC_INFO_SC){}
	SALanternRiddieTimeOutInfo::SALanternRiddieTimeOutInfo() :header(MT_LANTERN_RIDDIE_TIMEOUT_INFO_SC) {}
	SALanternRiddieAnswerResultInfo::SALanternRiddieAnswerResultInfo() : header(MT_LANTERN_RIDDIE_ANSWER_RESULT_INFO_SC) {}
	SALanternRiddieBaseInfo::SALanternRiddieBaseInfo() : header(MT_LANTERN_RIDDIE_BASE_INFO_SC) {}
	SCLanternRiddiePraiseInfo::SCLanternRiddiePraiseInfo() : header(MT_LANTERN_RIDDIE_PRAISE_INFO_SC) {}
	SCLanternRiddieNotifyInfo::SCLanternRiddieNotifyInfo() : header(MT_LANTERN_RIDDIE_NOTIFY_INFO_SC) {}
	SCLanternRiddieUseCardInfo::SCLanternRiddieUseCardInfo() : header(MT_LANTERN_RIDDIE_USE_CARD_INFO_SC) {}
	SCLanternRiddieLastResultInfo::SCLanternRiddieLastResultInfo() : header(MT_LANTERN_RIDDIE_LAST_RESULT_INFO_SC) {}

	//仙子去哪儿
	CSRoleWhereIsTheFairyOperaReq::CSRoleWhereIsTheFairyOperaReq() : header(MT_CA_WHERE_IS_THE_FAIRY_OPERA_REQ_CS) {}
	SCRoleWhereIsTheFairyInfo::SCRoleWhereIsTheFairyInfo() : header(MT_CA_ROLE_WHERE_IS_THE_FAIRY_INFO_SC) {}
	SCWhereIsTheFairyInfo::SCWhereIsTheFairyInfo() : header(MT_CA_WHERE_IS_THE_FAIRY_INFO_SC) {}

	//天宫寻宝
	CSTianGongXunBaoClientOpReq::CSTianGongXunBaoClientOpReq() : header(MT_CA_TIAN_GONG_XUN_BAO_OP_REQ_CS) {}
	SCTianGongXunBaoGridInfo::SCTianGongXunBaoGridInfo() : header(MT_CA_TIAN_GONG_XUN_BAO_INFO_SC) {}

	//情缘小铺
	CSLoversMarketReq::CSLoversMarketReq() : header(MT_LOVERS_MARKET_REQ_CS) {}
	SCLoversMarketInfo::SCLoversMarketInfo() : header(MT_LOVERS_MARKET_INFO_SC) {}

	CSTuZiKuaiPaoReq::CSTuZiKuaiPaoReq() : header(MT_TU_ZI_KUAI_PAO_REQ_CS) {}
	SCTuZiKuaiPaoSceneInfo::SCTuZiKuaiPaoSceneInfo() : header(MT_TU_ZI_KUAI_PAO_SCENE_INFO_SC) {}
	SCTuZiKuaiPaoInfo::SCTuZiKuaiPaoInfo() : header(MT_TU_ZI_KUAI_PAO_INFO_SC) {}
	SCTuZiKuaiPaoTuZiRest::SCTuZiKuaiPaoTuZiRest() : header(MT_TU_ZI_KUAI_PAO_REST_SC) {}
	SCTuZiKuaiPaoEncourageResult::SCTuZiKuaiPaoEncourageResult() : header(MT_TU_ZI_KUAI_PAO_ENCOURAGE_RESULT_SC) {}
	SCTuZiKuaiPaoTuZiRun::SCTuZiKuaiPaoTuZiRun() : header(MT_TU_ZI_KUAI_PAO_RUN_SC) {}
	SCTuZiKuaiPaoRoleInfo::SCTuZiKuaiPaoRoleInfo() : header(MT_TU_ZI_KUAI_PAO_ROLE_INFO_SC) {}
	SCTuZiKuaiPaoFinishNotice::SCTuZiKuaiPaoFinishNotice() : header(MT_TU_ZI_KUAI_PAO_FINISH_NOTICE_SC) {}
	SCTuZiKuaiPaoPosInfo::SCTuZiKuaiPaoPosInfo() : header(MT_TU_ZI_KUAI_PAO_POS_INFO_SC) {}
	
	//缘牵同心结
	CSConcentricKnotReq::CSConcentricKnotReq() : header(MT_CONCENTRIC_KNOT_REQ_CS) {}
	SCConcentricKnotAgreeNotice::SCConcentricKnotAgreeNotice() : header(MT_CONCENTRIC_KNOT_AGREE_SC) {}
	SCConcentricKnotStart::SCConcentricKnotStart() : header(MT_CONCENTRIC_KNOT_START_SC) {}
	SCConcentricKnotOpenCard::SCConcentricKnotOpenCard() : header(MT_CONCENTRIC_KNOT_OPEN_CARD_SC) {}
	SCConcentricKnotEndGame::SCConcentricKnotEndGame() : header(MT_CONCENTRIC_KNOT_END_GAME_SC) {}
	SCConcentricKnotInfo::SCConcentricKnotInfo() : header(ME_CONCENTRIC_KNOT_INFO_SC) {}

	//异兽临世
	SCGodBeastAdventInfo::SCGodBeastAdventInfo() :header(MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_INFO_SC){}
	SCGodBeastAdventOpenInfo::SCGodBeastAdventOpenInfo() : header(MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_OPEN_INFO_SC) {}
	SCGodBeastAdventBuyInfo::SCGodBeastAdventBuyInfo() : header(MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_BUY_INFO_SC) {}
	SCGodBeastAdventNotifyInfo::SCGodBeastAdventNotifyInfo() : header(MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_NOTIFY_INFO_SC) {}

	//随机活动-祈愿孔明灯
	SCRAQiYuanKongMingDengInfo::SCRAQiYuanKongMingDengInfo() : header(MT_RAND_ACTIVITY_QIYUAN_KONGMINGDENG_SC) {}

	//随机活动-寻梦蝴蝶
	SCRAXunMengHuDieInfo::SCRAXunMengHuDieInfo() : header(MT_RAND_ACTIVITY_XUNMENG_HUDIE_INFO_SC) {}

	//随机活动-播种有礼
	SCRABoZhongYouLiInfo::SCRABoZhongYouLiInfo() : header(MT_RAND_ACTIVITY_BO_ZHONG_YOU_LI_INFO_SC) {}

	//随机活动-元气翻牌
	SCRAYuanQiFanPaiInfo::SCRAYuanQiFanPaiInfo() : header(MT_RAND_ACTIVITY_YUAN_QI_FAN_PAI_INFO_SC) {}

	//随机活动-万灵筑礼
	SCRAWanLingZhuLiInfo::SCRAWanLingZhuLiInfo() : header(MT_RAND_ACTIVITY_WANLING_ZHULI_INFO_SC) {}

	//随机活动-包子来了
	SCRABaoZiLaiLeInfo::SCRABaoZiLaiLeInfo() : header(MT_RAND_ACTIVITY_BAO_ZI_LAI_LE_INFO_SC) {}

	SCRAJinYiTianXiang::SCRAJinYiTianXiang() : header(MT_JIN_YI_TIAN_XIANG_SC) {}

	//评价有礼
	CSFetchPingJiaRewardReq::CSFetchPingJiaRewardReq() : header(MT_PINGJIA_REWARD_REQ_CS) {}
	SCFetchPingJiaRewardInfo::SCFetchPingJiaRewardInfo() : header(MT_PINGJIA_REWARD_INFO_SC) {}

	SCRAYuanQiYanHuo::SCRAYuanQiYanHuo() : header(MT_YUAN_QI_YAN_HUO_INFO_SC) {}

	// 元气记事簿
	SCRAYuanQiJiShiBuInfo::SCRAYuanQiJiShiBuInfo() : header(MT_RAND_ACTIVITY_YUAN_QI_JISHIBU_INFO_SC) {}

	//乘风破浪
	SCRAChengFengPoLangOpenInfo::SCRAChengFengPoLangOpenInfo() : header(MT_RAND_ACTIVITY_CHENG_FENG_PO_LANG_OPEN_INFO_SC) {}
	SCRAChengFengPoLangInfo::SCRAChengFengPoLangInfo() : header(MT_RAND_ACTIVITY_CHENG_FENG_PO_LANG_INFO_SC) {}
	SCRAMiaoBiShengHuaInfo::SCRAMiaoBiShengHuaInfo() : header(MT_RAND_ACTIVITY_MIAO_BI_SHENG_HUA_INFO_SC) {}

	//鸿蒙天界
	CSHongMengTianJieReq::CSHongMengTianJieReq() : header(MT_HONGMENG_TIANJIE_REQ_CS) {}
	SCHongMengTianJieSceneInfo::SCHongMengTianJieSceneInfo() : header(MT_HONGMENG_TIANJIE_SCENE_INFO_SC) {}
	SCHongMengTianJieRoleInfo::SCHongMengTianJieRoleInfo() : header(MT_HONGMENG_TIANJIE_ROLE_INFO_SC) {}
	SCHongMengTianJieWorldInfo::SCHongMengTianJieWorldInfo() : header(MT_HONGMENG_TIANJIE_WORLD_INFO_SC) {}
	SCHongMengTianJieTalkInfo::SCHongMengTianJieTalkInfo() : header(MT_HONGMENG_TIANJIE_TALK_INFO_SC) {}
	SCHongMengTianJieRewardInfo::SCHongMengTianJieRewardInfo() : header(MT_HONGMENG_TIANJIE_REWARD_INFO_SC) {}
	SCHongMengTianJiePassTeamInfo::SCHongMengTianJiePassTeamInfo() : header(MT_HONGMENG_TIANJIE_PASS_TEAM_SC) {}

	CSZaoHuaEquipmentReq::CSZaoHuaEquipmentReq() : header(MT_ZAO_HUA_EQUIPMENT_REQ_CS) {}
	SCZaoHuaReFineInfo::SCZaoHuaReFineInfo() : header(MT_ZAO_HUA_EQUIPMENT_REFINE_SC) {}
	CSZaoHuaYuanHunZhuReq::CSZaoHuaYuanHunZhuReq() : header(MT_ZAO_HUA_YUAN_HUN_ZHU_REQ_CS) {}
	CSYuanHunZhuUpLevelReq::CSYuanHunZhuUpLevelReq() : header(MT_ZAO_HUA_YUAN_HUN_ZHU_UP_LEVEL_CS) {}
	SCZaoHuaLongZhuInfo::SCZaoHuaLongZhuInfo() : header(MT_ZAO_HUA_LONG_ZHU_INFO_SC) {}
	SCZaoHuaLongZhuSlotInfo::SCZaoHuaLongZhuSlotInfo() : header(MT_ZAO_HUA_LONG_ZHU_SLOT_INFO_SC) {}
	

	// 圣器
	CSHolyEquipOpReq::CSHolyEquipOpReq() : header(MT_HOLY_EQUIP_REQ_CS) {}
	CSHolyEquipLevelUpReq::CSHolyEquipLevelUpReq() : header(MT_HOLY_EQUIP_LEVEL_UP_REQ_CS) {}
	SCHolyBagListInfo::SCHolyBagListInfo() : header(MT_HOLY_EQUIP_HOLY_BAG_ALL_INFO_SC) {}
	SCFuShiBagListInfo::SCFuShiBagListInfo() : header(MT_HOLY_EQUIP_FUSHI_BAG_ALL_INFO_SC) {}
	SCHolyBagItemChange::SCHolyBagItemChange() : header(MT_HOLY_EQUIP_HOLY_BAG_SIGNLE_INFO_SC) {}
	SCSCHolyEquipLevelUpConsumeInfo::SCSCHolyEquipLevelUpConsumeInfo() : header(MT_HOLY_EQUIP_LEVEL_UP_SONCUME_INFO_SC) {}

	SCRAZongXiangManChengInfo::SCRAZongXiangManChengInfo() : header(MT_RAND_ACTIVITY_ZONG_XIANG_MAN_CHENG_INFO) {}
	SCRASaltySweetBattleInfo::SCRASaltySweetBattleInfo() : header(MT_RAND_ACTIVITY_SALTY_SWEET_BATTLE_INFO_SC) {}
	SCRASaltySweetBattleRoleInfo::SCRASaltySweetBattleRoleInfo() : header(MT_RAND_ACTIVITY_SALTY_SWEET_BATTLE_ROLE_INFO_SC) {}

	SCRALuckyBlindBoxInfo::SCRALuckyBlindBoxInfo() : header(MT_RAND_ACTIVITY_LUCKY_BLIND_BOX_INFO_SC) {}

	// 随机活动-端阳香囊
	SCRADuanYangXiangNang::SCRADuanYangXiangNang() : header(MT_RAND_ACTIVITY_DUAN_YANG_XIANG_NANG_INFO) {}

	// 随机活动-苍龙赐福
	SCRACangLongCiFuInfo::SCRACangLongCiFuInfo() : header(MT_RAND_ACTIVITY_CANG_LONG_CI_FU_INFO) {}
	// 随机活动-兰汤初沐
	SCRALanTangChuMuInfo::SCRALanTangChuMuInfo() : header(MT_RAND_ACTIVITY_LAN_TANG_CHU_MU_INFO) {}
	// 随机活动-宠技返利
	SCRAChongJiFanLiInfo::SCRAChongJiFanLiInfo() : header(MT_RAND_ACTIVITY_CHONG_JI_FAN_LI_INFO_SC) {}

	// 随机活动-三百抽皮肤
	SCRASanBaiChouInfo::SCRASanBaiChouInfo() : header(MT_RA_SANBAI_CHOU_INFO_SC) {}
	SCRASanBaiChouDrawInfo::SCRASanBaiChouDrawInfo() : header(MT_RA_SANBAI_CHOU_DRAW_SC) {}

	SCRADuanYanZiXuanInfo::SCRADuanYanZiXuanInfo() : header(MT_DUAN_YANG_ZI_XUAN_INFO_SC) {}
	SCRADuanYangFuLiInfo::SCRADuanYangFuLiInfo() : header(MT_DUAN_YANG_FU_LI_INFO_SC) {}

	SCRAJinLongJuBaoInfo::SCRAJinLongJuBaoInfo() : header(MT_JIN_LONG_JU_BAO_INFO_SC) {}
	SCRAJinLongJuBaoDrawRet::SCRAJinLongJuBaoDrawRet() : header(MT_JIN_LONG_JU_BAO_DRAW_RET_SC) {}

	SCTeamRoleRecruitInfo::SCTeamRoleRecruitInfo() : header(MT_TEAM_ROLE_RECRUIT_INFO_SC) {}
	CSTeamRoleRecruitAnswer::CSTeamRoleRecruitAnswer() : header(MT_TEAM_ROLE_RECRUIT_ANSWER_CS) {}
	SCHolyEquipFirstGainInfo::SCHolyEquipFirstGainInfo() : header(MT_HOLY_EQUIP_FIRST_GAIN_INFO_SC) {}
	SCHolyEquipFollowInfo::SCHolyEquipFollowInfo() : header(MT_HOLY_EQUIP_FOLLOW_INDEX_INFO_SC) {}

	// 科举考试
	CSKeJuExamCSReq::CSKeJuExamCSReq() : header(MT_KE_JU_EXAM_REQ_CS) {}
	SCKeJuExamNPCInfo::SCKeJuExamNPCInfo() : header(MT_KE_JU_EXAM_NPC_INFO_SC) {}
	SCKeJuExamTimeOutInfo::SCKeJuExamTimeOutInfo() : header(MT_KE_JU_EXAM_TIMEOUT_INFO_SC) {}
	SCKeJuExamAnswerResultInfo::SCKeJuExamAnswerResultInfo() : header(MT_KE_JU_EXAM_ANSWER_RESULT_INFO_SC) {}
	SCKeJuExamBaseInfo::SCKeJuExamBaseInfo() : header(MT_KE_JU_EXAM_BASE_INFO_SC) {}
	SCKeJuExamPraiseInfo::SCKeJuExamPraiseInfo() : header(MT_KE_JU_EXAM_PRAISE_INFO_SC) {}
	SCKeJuExamNotifyInfo::SCKeJuExamNotifyInfo() : header(MT_KE_JU_EXAM_NOTIFY_INFO_SC) {}
	SCKeJuExamUseCardInfo::SCKeJuExamUseCardInfo() : header(MT_KE_JU_EXAM_USE_CARD_INFO_SC) {}
	SCKeJuExamLastResultInfo::SCKeJuExamLastResultInfo() : header(MT_KE_JU_EXAM_LAST_RESULT_INFO_SC) {}

	CSGotoSchoolReq::CSGotoSchoolReq() : header(MT_GOTO_SCHOOL_REQ_CS) {}
	SCGotoSchoolInfo::SCGotoSchoolInfo() : header(MT_GOTO_SCHOOL_INFO_SC) {}

	CSZhouQiShopReq::CSZhouQiShopReq() : header(MT_ZHOU_QI_SHOP_REQ_CS) {}
	SCZhouQiShopInfo::SCZhouQiShopInfo() : header(MT_ZHOU_QI_SHOP_INFO_SC) {}
	
	CSPartnerPetRankReq::CSPartnerPetRankReq() : header(MT_PARTNER_PET_RANK_REQ_CS) {}
	SCPetTopRankInfo::SCPetTopRankInfo() : header(MT_PET_TOP_RANK_INFO_SC) {}
	SCPartnerTopRankInfo::SCPartnerTopRankInfo() : header(MT_PARTNER_TOP_RANK_INFO_SC) {}

	SCRAShenShouEnZeInfo::SCRAShenShouEnZeInfo() : header(MT_SHEN_SHOU_EN_ZE_INFO_SC) {}
	SCRAShenShouEnZeDrawResult::SCRAShenShouEnZeDrawResult() : header(MT_SHEN_SHOU_EN_ZE_DRAW_RESULT_SC) {}
	SCRAShenShouEnZeRecord::SCRAShenShouEnZeRecord() : header(MT_SHEN_SHOU_EN_ZE_RECORD_INFO_SC) {}

	SCRAXinFuQiangHua::SCRAXinFuQiangHua() : header(MT_RAND_ACTIVITY_XIN_FU_QIANG_HUA_INFO_SC) {}

	// 随机活动-七夕特惠
	SCRAQiXiTeHuiInfo::SCRAQiXiTeHuiInfo() : header(MT_RAND_ACTIVITY_QI_XI_TE_HUI_INFO) {}
	SCRAQiXiTeHuiSingleInfo::SCRAQiXiTeHuiSingleInfo() : header(MT_RAND_ACTIVITY_QI_XI_TE_HUI_SINGLE_INFO) {}
	SCRAQiXiTeHuiOtherInfo::SCRAQiXiTeHuiOtherInfo() : header(MT_RAND_ACTIVITY_QI_XI_TE_HUI_OTHER_INFO) {}

	SCRAXianShiBaoHe3Info::SCRAXianShiBaoHe3Info() : header(MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_3_SC) {}
	SCRAXianShiBaoHe4Info::SCRAXianShiBaoHe4Info() : header(MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_4_SC) {}
	SCRADingZhiBaoXiang3Info::SCRADingZhiBaoXiang3Info() : header(MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_3_INFO_SC) {}
	SCElementElfRewards::SCElementElfRewards() : header(MT_ELEMENT_ELF_REWARDS_SC) {}

	SCRAXinFuLingChongInfo::SCRAXinFuLingChongInfo() : header(MT_RAND_ACTIVITY_XIN_FU_LING_CHONG_INFO) {}

	CSFuYuReq::CSFuYuReq() : header(MT_FU_YU_OPERATE_REQ_CS) {}
	SCFuYuInfo::SCFuYuInfo() : header(MT_FU_YU_SINGLE_INFO_SC) {}
	SCAllFuYuInfo::SCAllFuYuInfo() : header(MT_FU_YU_ALL_INFO_SC) {}

	// 随机活动-魂器返利
	SCRAHunQiFanLiInfo::SCRAHunQiFanLiInfo() : header(MT_RAND_ACTIVITY_HUN_QI_FAN_LI_INFO_SC) {}
	// 随机活动-天衣耀世
	SCRATianYiYaoShiInfo::SCRATianYiYaoShiInfo() : header(MT_RAND_ACTIVITY_TIAN_YI_YAO_SHI_INFO_SC) {}

	SCRAShenCiHuaFuInfo::SCRAShenCiHuaFuInfo() : header(MT_RAND_ACTIVITY_SHEN_CI_HUA_FU_INFO_SC) {}
	SCRAShenCiHuaFuDrawRet::SCRAShenCiHuaFuDrawRet() : header(MT_RAND_ACTIVITY_SHEN_CI_HUA_FU_DRAW_RET_SC) {}

	//跨服英雄录
	CSKuaFuYingXiongLuReq::CSKuaFuYingXiongLuReq() : header(MT_KUA_FU_YING_XIONG_LU_REQ_CS) {}
	SCKuaFuYingXiongLuActivityInfo::SCKuaFuYingXiongLuActivityInfo() : header(MT_KUA_FU_YING_XIONG_LU_ACTIVITY_INFO_SC) {}
	SCKuaFuYingXiongLuRoleAllInfo::SCKuaFuYingXiongLuRoleAllInfo() : header(MT_KUA_FU_YING_XIONG_LU_ROLE_ALL_INFO_SC) {}
	SCKuaFuYingXiongLuRoleSingleRankInfo::SCKuaFuYingXiongLuRoleSingleRankInfo() : header(MT_KUA_FU_YING_XIONG_LU_ROLE_SINGLE_RANK_INFO_SC) {}
	SCKuaFuYingXiongLuRoleSingleTaskInfo::SCKuaFuYingXiongLuRoleSingleTaskInfo() : header(MT_KUA_FU_YING_XIONG_LU_ROLE_SINGLE_TASK_INFO_SC) {}
	SCKuaFuYingXiongLuRoleRankValue::SCKuaFuYingXiongLuRoleRankValue() : header(MT_KUA_FU_YING_XIONG_LU_ROLE_RANK_VALUE_SC) {}

	// 星图系统
	CSStarChartRequest::CSStarChartRequest() : header(MT_STAR_CHART_REQUEST_CS) {}
	SCStarChartAllInfo::SCStarChartAllInfo() : header(MT_STAR_CHART_INFO_SC) {}
	SCStarChartInstallSkillNotice::SCStarChartInstallSkillNotice() : header(MT_STAR_CHART_INSTALL_SKILL_NOTICE_SC) {}

	//弑神之塔
	CSNewCourageChallengeOp::CSNewCourageChallengeOp() : header(MT_NEW_COURAGE_CHALLENGE_REQ_CS) {}
	SCNewCourageChallengeLevelListInfo::SCNewCourageChallengeLevelListInfo() : header(MT_NEW_COURAGE_CHALLENGE_LEVEL_LIST_INFO_SC) {}
	SCNewCourageChallengeLevelSignleInfo::SCNewCourageChallengeLevelSignleInfo() : header(MT_NEW_COURAGE_CHALLENGE_LEVEL_SIGNLE_INFO_SC) {}
	SCNewCourageChallengeRewardInfo::SCNewCourageChallengeRewardInfo() : header(MT_NEW_COURAGE_CHALLENGE_REWARD_INFO_SC) {}

	// 月夕灯谜
	CSRAYueXiDengMiRequest::CSRAYueXiDengMiRequest() : header(MT_RA_YUE_XI_DENG_MI_REQ_CS) {}
	SCRAYueXiDengMiInfo::SCRAYueXiDengMiInfo() : header(MT_RA_YUE_XI_DENG_MI_INFO_SC) {}
	SCRAYueXiDengMiAnswerResult::SCRAYueXiDengMiAnswerResult() : header(MT_RA_YUE_XI_DENG_MI_ANSWER_RESULT_SC) {}
	// 月夕累充
	SCRAYueXiLeiChongInfo::SCRAYueXiLeiChongInfo() : header(MT_RA_YUE_XI_LEI_CHONG_INFO_SC) {}

	//随机活动 - 单笔返利
	SCRADanBiFanLiInfo::SCRADanBiFanLiInfo() : header(MT_RA_DAN_BI_FAN_LI_INFO_SC) {}
	SCRADanBiFanLiSingleInfo::SCRADanBiFanLiSingleInfo() : header(MT_RA_DAN_BI_FAN_LI_SINGLE_INFO_SC) {}

	SCRAZhuiYueShangDianInfo::SCRAZhuiYueShangDianInfo() : header(MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_INFO_SC) {}
	SCRAZhuiYueShangDianDrawRet::SCRAZhuiYueShangDianDrawRet() : header(MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_DRAW_RET_SC) {}
	SCRAZhuiYueShangDianDrawRecrod::SCRAZhuiYueShangDianDrawRecrod() : header(MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DRAW_RECORD_SC) {}

	//幻兽系统
	CSHuanShouReq::CSHuanShouReq() : header(MT_HUAN_SHOU_REQ_CS) {}
	SCHuanShouAllInfo::SCHuanShouAllInfo() : header(MT_HUAN_SHOU_ALL_INFO_SC) {}
	SCHuanShouSingleInfo::SCHuanShouSingleInfo() : header(MT_HUAN_SHOU_SINGLE_INFO_SC) {}
	SCHuanShouNoticeInfo::SCHuanShouNoticeInfo() : header(MT_HUAN_SHOU_NOTICE_INFO_SC) {}
	CSHuanShouDecomposeReq::CSHuanShouDecomposeReq() : header(MT_HUAN_SHOU_DECOMPOSE_REQ_CS) {}
	SCHuanShouOtherInfo::SCHuanShouOtherInfo() : header(MT_HUAN_SHOU_OTHER_INFO_SC) {}
	SCHuanShouAnnouncementInfo::SCHuanShouAnnouncementInfo() : header(MT_HUAN_SHOU_ANNOUNCEMENT_INFO_SC) {}

	SCHuanShouBagInfoAck::SCHuanShouBagInfoAck() : header(MT_HUAN_SHOU_BAG_ALL_INFO_SC) {}
	SCHuanShouBagSingleInfo::SCHuanShouBagSingleInfo() : header(MT_HUAN_SHOU_BAG_SINGLE_INFO_SC) {}

	//家族任务
	CSRoleGuildTaskReq::CSRoleGuildTaskReq() : header(MT_ROLE_GUILD_TASK_REQ_CS) {}
	SCRoleGuildTaskInfo::SCRoleGuildTaskInfo() : header(MT_ROLE_GUILD_TASK_INFO_SC) {}
	SCRoleGuildTaskChange::SCRoleGuildTaskChange() : header(MT_ROLE_GUILD_CHANGE_SC) {}

	SCGuaJiExpInfo::SCGuaJiExpInfo() : header(MT_GUA_JI_EXP_INFO_SC) {}

	CSRealNameRewardReq::CSRealNameRewardReq() : header(MT_REAL_NAME_REWARD_RECEIVE_CS) {}
	SCRealNameRewardInfo::SCRealNameRewardInfo() : header(MT_REAL_NAME_REWARD_RECEIVE_INFO_SC) {}
}