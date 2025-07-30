#ifndef __MESSAGE_HANDLER_HPP__
#define __MESSAGE_HANDLER_HPP__

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "inetworkmodule.h"

class SceneManager;
class Scene;
class Role;
class MessageHandler
{
public:
	typedef void (MessageHandler::*HandleFunc)(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	static const int MAX_MSG_TYPE = MAX_UINT16 + 1;

	struct HandlerItem
	{
		HandlerItem() : func(0), data_size(0) {}
		HandlerItem(HandleFunc f, int s) : func(f), data_size(s) {}

		HandleFunc	func;
		int			data_size;
	};

public:
	MessageHandler();
	~MessageHandler();
	void Init(SceneManager* scene_manager);
	void Release();

	void HandleMessageFromClient(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, const char *msg, unsigned int length);
	void HandleMessageFromClientNoLogin(const GSNetID &netid, IP ip, const char *msg, unsigned int length);
	
protected:
	// clientÎ´µÇÂ¼Ç°
	void OnReqEnter(const GSNetID &netid, IP userip, const char* msg);		

	// clientµÇÂ¼ºó
	void OnTimeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnLogout(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAllInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTransportReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSpecialTransportReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTreasureChestReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnRoleMoveReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFlyByShoe(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGmCommand(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPetOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAbandonPetByList(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPetRename(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPetAssignAttributePoints(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnUnfullTeamListByTypeReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnUnfullChivalrousTeamListReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnChangeTeamLimit(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnCreateTeamReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnDismissTeamReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnInviteJoinTeamReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnExitTeamReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnKickOutOfTeamReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnJoinTeamReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnJoinTeamReqRet(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnInviteJoinTeamRet(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamMatch(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnChangeTeamLeader(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnChangeMustCheck(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTemporarilyPartReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamInvitationReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamInvitationRet(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamEnterFbReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamRecruitReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamSwitchToCrossReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnKnapsackOperaReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnUseItem(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnUseOptionalGift(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnUseOptionalGift2(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnUseGoldRecovery(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUseSpecialLogicItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnActiveSpecialLogicItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnHearsayInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnBuyCmdGiftReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnHeartbeatReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPrestigeShoping(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPrestigeDonateOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnTaskInfoSingltHandle(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTalkWithAppointedNPC(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGiveNPCTaskNeedItems(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSetupRaskTraceStatus(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPlayTaskGame(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnPartnerHandleReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPartnerSkillSave(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPartnerAlterList(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPartnerEqRequest(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPartnerSwitchFightPos(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void ONPartnerQualityUpgrade(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPartnerElementCrytalReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPartnerSkinReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnTitleOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTrainOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRolePetTrainSkillOp(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnBattleDemoFightStartReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleFightMonsterReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattlePrepareFinishReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleAnimationFinishReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleForceFinishFightReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleRoleOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleCountDownFinishReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWatchBattleReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleSetStrategyReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleClearStrategyReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleLeaveOBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRolePauseBattleReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleBattleUndoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnCollectionOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWstQuestionOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnExpCrystalOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnTreviFountainOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSkillOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnNewLifeSkillReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFishGroundReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFetchRewardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSurfaceOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFindRewardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFindRewardNotifysCancelReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSurfaceDecompose(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnEquipmentOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnJewelryHandleRequest(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnObtainOfflineExpReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWorkshopOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnConvertPetSkillReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnEquipVocChangeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnMarryReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnCreateGuildReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnQuitGuildReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnJoinGuildReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnJoinGuildAck(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildExpelOutReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildSetNoticeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetGuildList(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildPetTrainingUp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildPetTrainingReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetGuildTrainingInfo(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildAbdicationOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildImpeachmentOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildAppointmentOp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildShopBuy(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildInvitationReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildInvitationRep(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSetNeedApproveJoin(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetGuildBossReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildLevelUpReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildAnswerAnswerQuestion(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildRename(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildSetBannerReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBuildingGuildBuildReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnMarriageSeekingReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnBabyReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBabyRenameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBabyActiveReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnFriendReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFriendEditGruopReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFriendMoveGruopReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFriendRenameGruopReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnFriendRecommendListReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAddFriendRecommend(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRefreshFriendRecommend(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFuzzySearchRoleInfoByName(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnSmartMountsSystemReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSmartMountsSystemSkillReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnRoleCareerAdvanceReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleCareerChangeReq(const GSNetID &netid, Scene* scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleProfExtraInfoReq(const GSNetID &netid, Scene* scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnRoleCreateAttributePlanReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleChangeAttributePlanNameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleSwitchAttributePlanReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleAddAttributePointReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleClearAttributePointReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnRoleClientSystemSetReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleClientSystemSetChatGroupReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnShiTuMakeRelationReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnShiTuMakeRelationAck(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnShiTuBreakRelationReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnShiTuBreakRelationAck(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnShiTuReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnShiTuSeekingReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFirstKillReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnSendMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnDeleteMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnLockMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnUnlockMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetMailList(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnReadMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFetchMailAttachment(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnCleanMail(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnOneKeyFetchMailAttachment(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnRoleBattleModeFightReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleChallengeOthersReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleQieCuoWithOtherReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleQieCuoRequestReply(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFetchSystemNoticeAward(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnNPCHandleRequest(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnSetEncounterMineMonsterFlag(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSetBattlePosition(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSetBattleAuto(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSetBattleAutoMove(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnChangeAppearacne(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnChangeHeadshot(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAttrListReasonReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnCourageChallengeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnCourageChallengeRankListReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnResetNameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnChatRecordReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnEnterFBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnLeaveFBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnActivityEnterReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnActivityTeamFightReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnActivityMazeRacerReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnActivityGuildFightReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnActivityOnlyFightReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnActivitySilkRoadsReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnActivitySignUpReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnActivityAdvanceFetchReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnCmdBuyItemCheckReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnChannelChat(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnSingleChat(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnChatItemInfoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnSchoolTaskReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnResearchTaskRequest(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnEscortOp(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnWorldBossReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnWorldBoss2Req(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnWorldBoss3Req(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnTreasureMapReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnFaBaoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnMapUnlockRemoveAnimation(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	
	void OnTradeMarketUserShelveDataReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketAddItemOnShelveReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketRemoveItemOnShelveReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketCashOutReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketSearchByItemReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketSearchByTypeReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketBuyItemReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketGetRecentTransactionRecordsReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketGetFocusListReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketAddFocusReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketRemoveFocusReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketGetFocusItemInfoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketAdvertiseReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketSearchAdvertiseReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketGetSellingListReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketGetRoleTransactionRecordReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketRedoAddItemOnShelveReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketSearchPetForQuickBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketGetMerchandiseNumReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTradeMarketSearchMedalByEffectTypeAndColorReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnTaskChainOpReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTaskGamePlayReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnCommitTaskMoneyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnMedalReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnMedalOneKeyGradeReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnZhuLingReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnZhuLingDecomposeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnZhuLingUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnCrystalFbReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnShopBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnGoldShopBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnXunBaoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnXunBaoExchange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnXunBaoBelssReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnShopFastBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnGoldShopCancel(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnMarketReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnColosseumReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnExchangeReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnBraveGroundReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRandActivityOpReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnChongzhiOperateReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnQueryOthersInfo(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnFirstChongzhiReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnSevenColorReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRandActivityBonusOpReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnMakeUpReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRandActivityDuiHuanLingYuReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnActivityChiefElectionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnActivityGuildHonorReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnActivityGuildHonorReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnActivityChiefPeakBattleReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnActivityHuanJieZhanChangReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnActivityHuanJieZhanChangRoleRankListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnPlatformBattleReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnPlatformBattleReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPlatformBattleFirstInfoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnCourseOfGrowthRequest(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleSubscribeTopic(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamSwitchPositionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleSalaryInfoReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnSpeakerTalk(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnMysteriousTrialReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTransactionTaskReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnActiveSystemReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHeartSkillReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHeartSkillDisplaceReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnFaceOperatorReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnFuncGuideRequest(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamMemberLaunchVote(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnTeamMemberVote(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnRolePatrolReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleTaskPathFinding(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnNewChongzhiConfigInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTeamLeaderNpcReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnNpcShopBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnRoleSendBattleChatReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnConvertShopBuyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleBackToOriginServerReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnRoleTalkToNpcReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleLoginSystemReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnAuditBehaviorSubmitReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnBossChapterReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnBountyTaskTurnTableFetchRewardReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnEscapeFromCurrentPositionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnAutoEscapeFromCurrentPositionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnSwitchPlatformSceneReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnWorldQuestionReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRealtimeCommentReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnResearchTaskAutoStartReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnSpecialBattleJoyReq(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnBattleJoyRoleOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTeamLeaderSummonMemberReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTeamMemberReplySummon(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleStartCrossReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetPersonRank(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetPetRank(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetRankRefreshReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetPetRankDetailedInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTeamLeaderPartnerInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleGiveupReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleSurrenderReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTaskChosedReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnLeaderCheckMemberItemReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnReqJoinTeamInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnBattleReportSimpleInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleReportDetailInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleReportStopWatchingReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGroupLeaveReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGroupDismissReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGroupKickMember(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnCreateGroup(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnEnterGroup(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnInviteEnterGroup(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetGroupList(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetGroupInfo(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWildBossReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWildBossPokerReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnTaskOneKeyFinish(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSkillPositionModify(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnPartnerSkillActivate(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnPokerRewardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnGroupNoticeChangeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length); 

	void OnHuaShenMedalReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenEquipmentReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenJewelryHandleReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenAttrPlanPonitReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenPetReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenPetAssignAttrPointReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenSetBattlePositionReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHuaShenAttrPlanChangeNameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnStartNpcDemoBattleReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleRejoinBattleSceneReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnMiYiPokerReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnMiYiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnMedalTaskPlayVideoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnPetEquipmentReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnClaimFuncPreviewReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnRealmBreakReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnMaterialReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	
	void OnSetAutoCatchPetReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFormationSimpleInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFormationDetailInfoReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFormationRenameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFormationSaveReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFormationDeleteReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSetPetHelperFormationReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnQueryPetHelperFormationReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnMysteryStoreReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGuildInbornReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnShanHaiBossReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnGetCurPosReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnCrysalFbFastPassReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnChatSimpleRecordReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnWenXiangLouReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnNewTaskActionReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnNewTaskGiveNPCItems(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnNormalTaskFinishAnedoteGameReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAnecdoteTaskOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAnecdoteTaskSpecialReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSpeciallogicCatchFoxReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnDrawManagerReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSpeciallogicMiJingQiWenReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSpeciallogicMiJingQiWenBuyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnSpeciallogicMiJingQiWenReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnSpeciallogicMiJingQiWenBuyReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnLevelComplementReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnNewLevelComplementReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnDreamNotesChallengeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnSchoolExerciseChallengeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnSchoolExerciseFirstOpenUI(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWorkShopDecomposeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTempChatGroupBecomeOfficialReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleRoleModifyAnimationSpeedReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnTeamLeaderCloseWindosReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnHandoverChatGroupLeadershipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnActiveSystemFetchExtraRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnMonthCardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPetMonopolyOperateReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnLunHuiWangChuanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnCloudArenaOperateReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnCloudArenaSetFormationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnCloudArenaRealTimeInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnRelicLootingMachReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnRelicLootingUseSkillReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnRoleInfoNoticeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnRoleTriggerPetDeadSummonGuideReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnRoleFetchHistoryChivalrousRewardReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnQueryWeekChivalrousRankListReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleJoinActivityRewardReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnGuildActivePersonRankReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnGuildActiveGetGiftReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnRoleTopLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	// ÒÆ¶¯¸¨Öú
	void OnMoveAssistReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnMapExplorationReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnSetAutoAddPointFlagReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnAddCustomInstructionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnRemoveCustomInstructionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnBattleSetCustomInstructionReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnBatchRemoveCustomInstructionReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnPetSoulUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void SetPetGodPrintWearSchemeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPetGodPrintUseItemUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnNetherWorldPurgatoryReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	
	//ÉñÓ¡Ö®µØ
	void OnPetGodFBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	//¶É½Ù
	void OnDuJieReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	

	void OnHongBaoClientReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPrestigeTaskSelectVillageReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPrestigeTaskReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnBigDipperReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnFallenGodReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	//
	void OnTianDiDaoHenReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnGiftReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnCapacityTargetReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	// »ñÈ¡×ÊÔ´ÏÂÔØ½±Àø
	void OnFetchRecourceDownloadReward(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnSVipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnMedalUpGradeSpecialReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	// ·ÀÕ©Æ­½±Àø»Ø´ð
	void OnFangFraudRewardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnRoleAttrPelletReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnActiveTurntableReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnBackRewardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAccessoriseReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnAccessoriseVocChangeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnYinYangEyeOpReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnEndlessTowerOpReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnQueryItemRecommendPriceReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnJieYiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnJieYiCreate(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnJieYiCreateName(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnJieYiMemberName(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnJieYiText(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnJieYiName(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnJieYiXuanYanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnJieYiXuanYanListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnQingYuanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);


	void OnCowReportGoodNewsClientReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTigerBringsBlessingClientReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnWanLingFBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnWeddingFBReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnBattleSkipReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnBattleFastForwardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnFlyUpSystemReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnMirageTerritoryReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnMirageTerritoryPartnerReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnRoleChangeColorReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnXiuWeiReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnTreasureTaskReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWorldTeamArenaReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWorldTeamArenaSignUpReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWorldTeamArenaSceneReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWorldTeamArenaFinallyRankReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnTeamLeaderEjectWindowsReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnWorldArenaRoleOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnMiniGameFetchPhaseRewardReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnMiniGameClearNextStageReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnMiniGameStartBattleReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);
	void OnMiniGameSetMiniGameIdReq(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, void* msg, int length);

	void OnGetUTAChampionRecord(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnUTAGuessClientReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);


	void OnPetComposeBatch(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPetComposeAndDecomposeBatch(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPetBadgeDecompose(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTATeamListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTATeamDetailInfoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAFetchWinBoxReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAMyTeamQualificationSchedulesReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAGetTeamQualificationRankListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAGetMainScheduleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAFinalRankListReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAGetQualificationSchedulesReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAGetMainSchedulesOverviewReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAEnterBattleSceneReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAWatchVideoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnGuildHallOfFameReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnTaskSkipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnPeaktokenRoleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnIncomeRoleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPetStrengthenRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnPartnerIntensifyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnUTAStartOBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnQingYuanShengHuiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnQingYuanShengHuiReq2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnTuZiKuaiPaoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnGotoSchoolReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnYaoXingShiLianReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnShiTuChuanGongFBReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnQingYuanDuiDuiPengReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnSanXianGuiDongReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnLanternRiddieReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnWhereIsTheFairyReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnTianGongXunBaoReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	
	void OnTaoQuanNaJiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnHundredGodBossReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnLoversMarketReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnConcentricKnotReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnFetchPingJiaRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnZaoHuaEquipmentOperateReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnZaoHuaEquipmentDiscomposeReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);
	void OnHongMengTianJieReq(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, void *msg, int length);

	void OnZaoHuaEquipmentReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnZaoHuaYuanHunZhuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnZaoHuaYuanHunZhuUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnHolyEquipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnHolyItemLevelUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnRoleAnswerTeamRecruitReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnKeJuExamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnPartnerPetRankReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnKuaFuYingXiongLuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnFuYuManagerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnQueryUserChatRecord(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnNewCourageChallengeOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnStarChartOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnFashionCollectionOpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnRAYueXiDengMiReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnHuanShouReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	void OnHuanShouDecomposeReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnAnecdoteSpecialReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnRoleGuildTaskReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);

	void OnReceiveRealNameRewardReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, void * msg, int length);
	
	HandlerItem m_msg_handler_list[MAX_MSG_TYPE];

	SceneManager* m_scene_manager;
};

#endif