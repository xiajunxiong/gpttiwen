/******************************************************************************************
FileName: gm.hpp
Author: dreamwang
Description: gm系统
Version: 1.0
History:
<author>    <time>					<version >    <desc>
dreamwang   2011/06/29/ 09:04:05     1.0           创建
*****************************************************************************************/

#ifndef __GM_HPP__
#define __GM_HPP__

#include <stdio.h>
#include "servercommon/serverdef.h"
#include "protocol/msggm.hpp"
#include <vector>
#include <string>

static const int MAX_HELP_INFO_LEN = 1024;

class Scene;
class Role;

class Gm
{
public:
	static Gm & Instance();
	static std::vector<std::string> SplitString(const std::string& src_str, const std::string& seperator);

	// 处理GM命令 
	bool OnCommand(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	void Update(unsigned long interval, time_t now_second);

private:
	Gm();
	~Gm();

	bool GmTuZiKuaiPao(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmQingYuanShengHui(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmLoadAllUserDataCalcSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	//bool MoveRole(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GuildHonorBattle(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool ResetPos(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool ResetPos2(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmBraveGround(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool ProtocolTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool UseItem(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddItem(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddTestEquipment(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool ClearBag(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool AddExp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddMoney(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool ClearMoney(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddPet(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddPetExp(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddMaxPet(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool TestFight(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool TestFightMonster(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool TestTeamFightMonster(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool TestPrestige(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool PartnerHandle(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool Test(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool OnDayChange(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool OnWeekChange(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool OnResetModuleData(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool OnResetModuleDataWeek(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool ForceFightFinish(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool BattleRoleOperate(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool TaskManagerOrder(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool SkillOp(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmOnDig(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmOnLumber(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmOnPickFlower(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmOnMakeEquipment(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmAddtitle(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmOnWorkshopRequest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmOnSystemNoticeReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmBaby(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAdvanceCareer(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMAddGuildExp(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMAddGuildGongxian(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSetRoleLevel(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddExpCrystal(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmMailTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmGoTo(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSetMineMonsterEncounterFlag(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmReloadConfig(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmReloadConfig2(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmEnterFB(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmLeaveFB(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmSetAttr(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool ActivityNextState(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMAchievement(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool AddHonorRollData(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	
	bool GmTestSinglePVP(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmKillMonster(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTestSchoolTask(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTestEscort(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAddSaleItem(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRemoveSaleItem(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmFindReward(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPrintMapBoundry(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPrintMapIsNeerBoundry(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmFaBaoReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);	
	bool GmSystemNoticeTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmCreateWildBoss(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTestBuff(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSceneOrder(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAddGatherFbPower(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	
	bool GmCreateMineMonsterNPC(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmCreateMineMonsterNPCById(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmFightMineMonster(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	
	bool GmAddTeamRobot(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmMazeRacerWaitingHallTransImmediately(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmMazeRacerOperator(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmKickRole(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAddOtherCoin(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	
	bool GmQuickBuySearchPet(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAddChongzhi(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStartCross(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStopCross(const GSNetID &netid, Scene* scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmPrintfUID(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStartOB(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTeamRecruit(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTeamSwitchToCross(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStartTeamMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStartUserMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStartUserCrossMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmCOGAddProg(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSetCourseOfGrowthProgress(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmQueryOther(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmGuildInviteOther(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPartnerStrategy(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmColosseumOrder(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmColosseumHandler(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPrintfAllTitle(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSalaryTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmClearCmdDisplay(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmFlyawayRoleLeaveBattle(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmForceFinishBattleMode(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAddMaxAttrEquipment(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmResearchTaskHandler(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmGuildModuleOrder(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmGuildOrder(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPlatformBattle(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmShadowFight(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRoleHandler(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStartTeamLeaderVote(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTeamMemberVote(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSetChiefElection(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTaskChainOrder(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmCreateTeam(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPauseBattle(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GetPos(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmCreateRobot(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTestServerSchoolTaskList(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool RePos(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmCrossSingleChat(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPartnerSwitch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmConvertShopBuy(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRoleTradeMarketStatus(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRandActivityReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRandActivityAdd(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmMysteriousTrial(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSetMysteriousTrial(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmBossChapterReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmBountyTaskReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmEscapeCurrentPosition(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSwitchPlatform(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTransactionTask(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmActiveSystem(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRandActivityInfo(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTaskChainTaskApt(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMWorldQuestionReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmGetMerchandiseNum(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmTestMatch(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRAExpelMonsterTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);

	bool GmAddActive(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAutoResearchTask(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPetOp(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSpecialReqMod(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTeamLeaderSummonMember(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRoleCmd(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMPatrolTaskTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMOneKeyFinish(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMPartnerTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GMRandActivityTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmForbidRole(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRemoveVideo(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmHuaShenInfoReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmStartNpcDemoBattleReq(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmHuanJieZhanChange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTriggerUnknownReasonDisconnect(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmHuaShenMedalReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmCheckRoleInBattle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestProtobuf(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSimulatePokerOper(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRoleSkillTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRoleResetGodToken(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmGenUniqueID(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmCheckDayIndex_6(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmRefreshGuildWeekTask(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPartnerSkinTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAutoCatchPet(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmFacesCoreTest(const GSNetID &netid, Scene *scene, Role* role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmPetSkin(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFormationRename(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFormationDetailInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFormationSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestCommonSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmChiefElectionSeaElection(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool IsRandActivityOpen(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmSetTianJiangHaoLi(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmAddLuckCoin(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmBuyToken(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmInder(const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	bool GmTestRoleCommonSave(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddShanHaiSoulFragment(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmOnWeekChangeFlexible(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFastforwardOnlyFightEndEnterTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFastforwardGuildFightEndEnterTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRetrieveExp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGuildNoLeader(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRoleBuyItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRoleBuyItem2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmMiYiSetPassCount(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddMatchUser(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestNetwork(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmResetShanHaiChallengeCount(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSendAttrListReason(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAnecdoteReset(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmMyDraw(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGetSkillGoBackRetCoin(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetMazeRaceActivityRankTimestamp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmTestTeamFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRandActivityMakeUp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetRandActivityEndTimestamp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetSpecialLogicActivityEndTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSchoolExercise(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTanHe(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTempGroupBecomeOfficial(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGuideReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmPrintfStack(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestPutList(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddCredit(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRAbonusReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	
	bool GmAddGoodGiftTurnTableNum(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmfortunemazeTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	
	bool GmChatGroupHandOver(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmChatGroupHandOver2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFetchActiveSystemExtraReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetColosseumRefreshTampNow(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetDreamNotesChallengeLevel(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetRealmBreak(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmPetMonopoly(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRefreshLunHuiWangChuanNow(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmCloudArena(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRefreshShanHai(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFixDice(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmJumpToLayer(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddCloudArenaTimes(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddCloudArenaScore(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmRefreshShanHaiChallenge(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmActiveAllFabao(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMRobottry(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMResetCloudArenaRefreshCD(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMCloudArenaRandomFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMRandomPetHelperFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetSmartMountsSkill(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestCloudArenaTitle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmCheckUniqueID(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestChivalrous(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetWildBossRewardTimes(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetFaBaoTranf(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestLoginBroadcast(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetSmartMountsEnergy(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmCheckItemWrongColumn(const GSNetID& netid, Scene* scene, Role* role, ObjID obj_id, Protocol::CSGMCommand* command);
	bool GmGetGuildActiveReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGetGuildActivePersonRank(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGetXunBaoInfo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetPartnerTranf(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmGuildActiveAddDay(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddGuildActive(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMQueryPetHelperFormation(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmCourageChallengeOpenUI(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmHongBaoTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetZhuanShuLiBaoDayGift(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMWeekChangeWorldStatus(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmPrestigeTaskTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGiveGift(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmQiongLouYuYanReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetRandActivityTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetGodEquipErrUpLevelFlag (const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTianDiDaoHen(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestHuanJieZhanChangMatch1(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddWeekActiveDegree(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddActiveTurntableTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestHuanJieZhanChangMatch2(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetRABackReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmChangeKillValue(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmPetTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmConvertSkillBook(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestUseSpecialItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GMAddTianMingExp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmResetTask(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMResetFlag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GMTestAnecdoteBigEvent(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmQueryTradeItemPrice(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmPrintUserStatus(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGiveFlower(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmJieYi(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmQingYuan(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmResetGmAttr(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestCapability(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmEndlessRankReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmCowReportGoodNewsResetData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAdvanceCareerTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddFumoItem(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmForbidTalk(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSoaringSkill(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmPetFeiSheng(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmResetGuildBossTimes(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTaskFinish(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTigerBringsBlessingResetData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmRetieveMoney(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmWeddingNextLink(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmWeddingTry(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmXunYouCar(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmWorldArenaOperate(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmWorldArenaControl(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmMiniGameClearNextStage(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmMiniGameFetchPhaseReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmWorldTeamArenaSignUp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmWorldTeamArenaOp(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmFallenGodReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddUTAChampionRecord(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmUTAChampionRecordReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmFuMoUpLevelReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmWorldDayChange(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmGuildQiFuAdd(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmStopServer(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmYaoXingSet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	
	bool GmTestXinFuBiPinReward(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestRedisGET(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestRedisSET(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAsyncTaskTest(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmTryEnterUTAScene(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmUTAWatchVideo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmUTAStartOB(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmFastForwardBattle(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmTestTime(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);


	bool GmGodBeastAdventAddLike(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmSaveData(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmEquipReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmHolyEquipClear(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool salySet(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmKeJuExamReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmHallowStudySkill(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmHallowSetGift(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmKuaFuYingXiongLu(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmClearFightCount(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmTestElementElfPosibility(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmFuYuManagerReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmAddFuYuReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmRoleFlyUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmPartnerFlyUpReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmQueryUserChatRecord(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmWatchVideo(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetWaiguaMoveFlag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmMiniGameBattleReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmStartChartReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmResetStartChartReq(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmSetFunctionGuideFlag(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);
	bool GmClearAllTask(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	bool GmHuanShouActive(const GSNetID & netid, Scene * scene, Role * role, ObjID obj_id, Protocol::CSGMCommand * command);

	// ----------------------------------------------------------------------------------------------------------------------------------
	typedef bool (Gm::*CommandFunc) (const GSNetID &netid, Scene *scene, Role *role, ObjID obj_id, Protocol::CSGMCommand *command);
	struct CommandFuncItem
	{
		char type[Protocol::MAX_GM_COMMAND_TYPE_LEN];
		CommandFunc pfnCommandHandler;
		int param_count;
		char help[MAX_HELP_INFO_LEN];
	};

	CommandFuncItem* GetCommandFuncItem()
	{
		static CommandFuncItem stCommandFuncItem[] =
		{
			{ "cak",&Gm::GmClearAllTask , 0,"" },
			{ "s",&Gm::GmSetFunctionGuideFlag , 0,"" },
			{ "resetstarchart",&Gm::GmResetStartChartReq , 0,"" },
			{ "starchart",&Gm::GmStartChartReq , 0,"" },
			{ "partnerflyup",&Gm::GmPartnerFlyUpReq , 3,"format: partner_id fly_up_level modify_role_flyup" },
			{ "roleflyup",&Gm::GmRoleFlyUpReq , 1,"format: fly_up_level" },
			{ "addfuyu",&Gm::GmAddFuYuReq , 2,"format: item_id sp_group" },
			{ "colosseumclearcount",&Gm::GmClearFightCount , 0,"" },
			{ "fuyureq",&Gm::GmFuYuManagerReq , 5,"format: op_type param1 param2 param3 param4" },
			{ "salyset",&Gm::salySet , 0,"" },
			{ "kejuexam",&Gm::GmKeJuExamReq , 2,"format: op_type param1" },
			{ "gmequipreq",&Gm::GmEquipReq , 3,"format: column index equip_index" },
			{ "clearholybag",&Gm::GmHolyEquipClear , 1,"format:op_type" },
			{ "savedata",&Gm::GmSaveData , 0,"" },
			{ "yslsaddlike",&Gm::GmGodBeastAdventAddLike , 1,"format:add_num" },
			{ "gmyx",&Gm::GmYaoXingSet , 2,"format:type param" },
			{ "gmfumolevel",&Gm::GmFuMoUpLevelReq , 3,"format:posi gird level" },
			{ "worlddaychange",&Gm::GmWorldDayChange , 0,"format:" },
			{ "qifuguild",&Gm::GmGuildQiFuAdd , 2,"format:type p1" },
			{"worldteamarenaop",&Gm::GmWorldTeamArenaOp , 2,"format:op_type p1"},
			{"teamarenaallsignup",&Gm::GmWorldTeamArenaSignUp , 0,"format:"},
			{"retievemoney",&Gm::GmRetieveMoney , 5,"format:p1 p2 p3 p4 p5"},
			{ "fallengod",&Gm::GmFallenGodReq,2,"type param"},
			{ "taskfinish",&Gm::GmTaskFinish ,2,"" },
			{ "petfeisheng",&Gm::GmPetFeiSheng ,3,"" },
			{ "addfumo",&Gm::GmAddFumoItem ,2,"sp_id level" },
			{ "endlessrank",&Gm::GmEndlessRankReward ,2,"type param" },
			{ "resetflag",&Gm::GMResetFlag ,2,"flag_type param" },
			{"usespecialitem",&Gm::GmTestUseSpecialItem ,2,"special_item_id item_param"},
			{ "pettest", &Gm::GmPetTest,3,""},
			{"skillbookconvert", &Gm::GmConvertSkillBook,2,"skillbook_1_item_id skillbook_2_item_id"},
			{ "tiandidaohen", &Gm::GmTianDiDaoHen,0,""},
			{ "errlevel", &Gm::GmSetGodEquipErrUpLevelFlag,0,""},
			{ "randactivityday", &Gm::GmSetRandActivityTime, 2, "ratype day"},
			{ "give", &Gm::GmGiveGift, 2, ""},
			{ "addworldweek", &Gm::GMWeekChangeWorldStatus , 0, "type"},
			{ "partner", &Gm::GmSetPartnerTranf , 2, "type param" },
			{ "xunbaoinfo", &Gm::GmGetXunBaoInfo , 2, "" },
			{ "fabao", &Gm::GmSetFaBaoTranf , 1, "format tranf" },
			{ "testloginbroadcast", &Gm::GmTestLoginBroadcast, 1, "format:type" },
			{ "refreshlhwc", &Gm::GmRefreshLunHuiWangChuanNow , 2, "format type param" },
			{ "colosseumrefresh", &Gm::GmSetColosseumRefreshTampNow , 0, "format" },
			{ "qionglouyuyan", &Gm::GmQiongLouYuYanReq , 6, "format : type param1 - 5" },
			{ "addcredit", &Gm::GmAddCredit , 1, "format : credit_num" },
			{ "tanhe",&Gm::GmTanHe ,0,"format" },
			{ "ramaze",&Gm::GmfortunemazeTest ,1,"format:times" },
			{ "joy",&Gm::GmSpecialReqMod ,1,"format:type" },
			{ "expelmonster",&Gm::GmRAExpelMonsterTest ,1,"format:type" },
			{ "worldquestion",&Gm::GMWorldQuestionReq ,1,"format:type"},
			{ "taskchain", &Gm::GmTaskChainTaskApt, 1, "format:task_type" },
			{ "trademarketstatus", &Gm::GmRoleTradeMarketStatus, 0, "format:" },
			{ "taskchainorder", &Gm::GmTaskChainOrder, 2, "format:op_type param" },
			{ "roleorder", &Gm::GmRoleHandler, 2, "format:op_type param" },
			{ "guildorder", &Gm::GmGuildOrder, 3, "format:guild_id op_type param" },
			{ "guildmgrorder", &Gm::GmGuildModuleOrder, 3, "format:guild_id op_type param" },
			{ "zhenzhongorder", &Gm::GmResearchTaskHandler, 2, "format:op_type param" },
			{ "addmaxequip", &Gm::GmAddMaxAttrEquipment, 4, "format:item_id attr_times sp_id attr_inc" },
			{ "clearcmd", &Gm::GmClearCmdDisplay, 0, "format:" },
			{ "printtitle", &Gm::GmPrintfAllTitle, 0, "format:" },
			{ "jjcorder", &Gm::GmColosseumOrder, 1, "format:op_type" },
			{ "jjchandler", &Gm::GmColosseumHandler, 3, "format:op_type param1 param2" },
			{ "guildinvite", &Gm::GmGuildInviteOther, 1, "format:target_uid" },
			{ "queryother", &Gm::GmQueryOther, 1, "format:target_uid" },
			{ "cogaddprog", &Gm::GmCOGAddProg, 2, "format:seq add_num" },
			{ "cogsetprog", &Gm::GmSetCourseOfGrowthProgress, 2, "format:prog star" },
			{ "printfuid", &Gm::GmPrintfUID, 0, "format:" },
			{ "gmbraveground", &Gm::GmBraveGround, 2, "format:op_type add_level" },
			{ "addothercoin", &Gm::GmAddOtherCoin, 2, "format:num coin_type" },
			{ "mazeraceroperator", &Gm::GmMazeRacerOperator, 3, "op_type param1 param2" },
			{ "transimmediately", &Gm::GmMazeRacerWaitingHallTransImmediately, 0, "" },
			{ "addteamrobot", &Gm::GmAddTeamRobot, 0, "" },
			{ "addgatherfbpower", &Gm::GmAddGatherFbPower, 1, "format:value" },
			{ "sceneorder", &Gm::GmSceneOrder, 2, "format:op_type scene_id" },
			{ "ptest", &Gm::ProtocolTest, 2, "format:pnum data" },
			{ "buff", &Gm::GmTestBuff, 3, "format:" },
			{ "systemnotice", &Gm::GmSystemNoticeTest, 3, "format:msg_type display_type param" },
			{ "createwildboss", &Gm::GmCreateWildBoss, 0, "format:" },
			{ "fabaoreq", &Gm::GmFaBaoReq ,3,"format: req_type param1 param2" },
			{ "findreward", &Gm::GmFindReward, 3, "format:act_type find_times consume_type" },
			{ "escort", &Gm::GmTestEscort, 2, "format:" },
			{ "schooltask", &Gm::GmTestSchoolTask, 2, "format:" },
			{ "killmonster", &Gm::GmKillMonster, 2, "format:" },
			{ "addhonorrolldata", &Gm::AddHonorRollData, 2, "format:" },
			{ "achievenemt", &Gm::GMAchievement, 5, "format:" },
			{ "mailtest", &Gm::GmMailTest, 1, "format:" },
			{ "addguildgongxian", &Gm::GMAddGuildGongxian, 1, "format: value" },
			{ "addguildexp", &Gm::GMAddGuildExp, 1, "format: value" },
			{ "addtitle", &Gm::GmAddtitle, 1, "format: title_id" },
			{ "taskorder", &Gm::TaskManagerOrder, 3, "format: op_type task_id param" },
			{ "activepartner", &Gm::PartnerHandle, 1, "format: activepartner partner_id" },
			{ "test", &Gm::Test, 4, "format" },
			{ "prestige", &Gm::TestPrestige, 3, "format:" },
			//{ "move", &Gm::MoveRole, 4, "format: move tar_scene_id tar_scene_key x y" },
			{ "resetpos", &Gm::ResetPos, 0, "format:" },
			{ "resetpos2", &Gm::ResetPos2, 2, "format:" },
			{ "guildhonorbattle", &Gm::GuildHonorBattle, 2, "format:" },

			{ "useitem", &Gm::UseItem, 5, "format: column index num " },
			{ "additem", &Gm::AddItem, 3, "format: additem item_id item_num is_bind" },
			{ "clearbag", &Gm::ClearBag, 0, "format: clearbag" },

			{ "addexp", &Gm::AddExp, 1, "format:exp" },
			{ "addmoney", &Gm::AddMoney, 1, "format: addmoney money" },
			{ "clearmoney", &Gm::ClearMoney, 1, "format:money_type" },
			{ "addpet", &Gm::AddPet, 2, "format: pet_id str_level" },
			{ "addpetexp", &Gm::AddPetExp, 2, "format: pet_index, add_exp" },
			{ "addmaxpet", &Gm::AddMaxPet, 3, "format: pet_id, att_type ap_val" },
			{ "testfight", &Gm::TestFight, 0, "format" },
			{ "testfightmonster", &Gm::TestFightMonster, 1, "format:monster_group_id" },
			{ "testfightteam", &Gm::TestTeamFightMonster, 1, "format:monster_group_id" },

			{ "addday", &Gm::OnDayChange, 0, "format" },
			{ "addweek", &Gm::OnWeekChange, 0, "format" },
			{ "resetdata", &Gm::OnResetModuleData, 0, "format" },
			{ "resetdataweek", &Gm::OnResetModuleDataWeek, 0, "format" },

			{ "forcefightfinish", &Gm::ForceFightFinish, 0, "" },
			{ "battleroleoperate", &Gm::BattleRoleOperate, 5, "format: operate_type p1 p2 p3 p4" },
			{ "skillop", &Gm::SkillOp, 4, "format:operate_type p1 p2 p3" },
			{ "gmondig", &Gm::GmOnDig, 0, "" },
			{ "gmonlumber", &Gm::GmOnLumber, 0, "" },
			{ "gmonpickflower", &Gm::GmOnPickFlower, 0, "" },
			{ "gmonmakeequip", &Gm::GmOnMakeEquipment, 1, "format:item_id" },
			{ "gmbaby", &Gm::GmBaby, 4, "format:op_type param1 param2 param3" },
			{ "careeradvance", &Gm::GmAdvanceCareer, 1, "format:new_prof" },
			{ "setrolelevel", &Gm::GmSetRoleLevel, 2, "format:to_level is_role_level" },
			{ "addsaleitem", &Gm::GmAddSaleItem, 4, "format:column index sale_num price" },
			{ "removesaleitem", &Gm::GmRemoveSaleItem, 1, "format:sale_index" },
			{ "randactivityinfo", &Gm::GmRandActivityInfo, 1, "format:activity_type"},
			{ "merchantnum", &Gm::GmGetMerchandiseNum, 2, "format:big_type is_announce"},
			{ "forbidrole", &Gm::GmForbidRole, 1, "format:role_id"},
			{ "removevideo", &Gm::GmRemoveVideo, 2, "format:monster_group_id is_server_first_kill"},
			// -----------------------------------------------------------------------------------
			// 功能测试
			{ "workshoptest", &Gm::GmOnWorkshopRequest, 5, "format:op_type param1 param2 param3 param4" },
			{ "addexpcrystal", &Gm::AddExpCrystal, 0, "format:" },
			{ "fetchsn", &Gm::GmOnSystemNoticeReq, 1, "format:award_seq" },
			{ "setmineflag", &Gm::GmSetMineMonsterEncounterFlag, 1, "format:flag" },

			{ "goto", &Gm::GmGoTo, 3, "format:scene_id pos_x pos_y" },
			{ "reloadconfig", &Gm::GmReloadConfig, 1, "format:config_type" },
			{ "reloadconfig2", &Gm::GmReloadConfig2, 3, "format:config_type param1 param2"},

			{ "enterfb", &Gm::GmEnterFB, 3, "format:fb_type param1 param2" },
			{ "leavefb", &Gm::GmLeaveFB, 0, "format:" },

			{ "gmsetattr", &Gm::GmSetAttr, 2, "format:type value" },
			{ "activitynextstate", &Gm::ActivityNextState, 1, "format:act_type" },

			{ "testpvp", &Gm::GmTestSinglePVP, 0, "format:" },
			{ "printmap", &Gm::GmPrintMapBoundry, 3, "format:x y is_print_boundry" },
			{ "printmap2", &Gm::GmPrintMapIsNeerBoundry, 2, "format:x y"},
			{ "minenpc", &Gm::GmCreateMineMonsterNPC, 0, "format:" },
			{ "minenpc_id", &Gm::GmCreateMineMonsterNPCById, 1, "format:npc_id" },
			{ "kickme", &Gm::GmKickRole, 0, "format:" },
			{ "fightmine", &Gm::GmFightMineMonster, 1, "format:is_all_lv1" },
			{ "gmsearchpet", &Gm::GmQuickBuySearchPet, 3, "format:item_id quality order_type" },
			{ "addchongzhi", &Gm::GmAddChongzhi, 1, "format:gold_num" },
			{ "startcross", &Gm::GmStartCross, 0, "format:" },
			{ "stopcross", &Gm::GmStopCross, 0, "format:" },
			{ "ob", &Gm::GmStartOB, 1, "format:uid" },
			{ "teamrecruit", &Gm::GmTeamRecruit, 0, "format:" },
			{ "teamstartcross", &Gm::GmTeamSwitchToCross, 0, "format:" },
			{ "teammatch", &Gm::GmStartTeamMatch, 0, "format:" },
			{ "usermatch", &Gm::GmStartUserMatch, 1, "format:team_type" },
			{ "usercrossmatch", &Gm::GmStartUserCrossMatch, 1, "format:team_type" },
			{ "partnerstrategy", &Gm::GmPartnerStrategy, 2, "format:strategy_type target_grid_idx" },
			{ "salarytest", &Gm::GmSalaryTest, 5, "format:exe_type param_1 param_2 param_3 param_4" },
			{ "leavebattle", &Gm::GmFlyawayRoleLeaveBattle, 1, "format:is_leave_team" },
			{ "finishbattlemode", &Gm::GmForceFinishBattleMode, 1, "format:battle_mode" },
			{ "platformbattle", &Gm::GmPlatformBattle, 2, "format:type p1" },
			{ "shadowfight", &Gm::GmShadowFight, 0, "format:" },
			{ "startvote", &Gm::GmStartTeamLeaderVote, 0, "format:" },
			{ "vote", &Gm::GmTeamMemberVote, 1, "format:is_yes" },
			{ "createteam", &Gm::GmCreateTeam, 0, "format:" },
			{ "pausebattle", &Gm::GmPauseBattle, 1, "format:is_pause" },
			{ "getpos", &Gm::GetPos, 0, "format:" }, 
			{"createrobot", &Gm::GmCreateRobot, 0, "format:"},
			{"randschooltask", &Gm::GmTestServerSchoolTaskList, 0, "format:"},
			{ "repos", &Gm::RePos, 2, "format:" },
			{"partnerswitch", &Gm::GmPartnerSwitch, 2, "format:partnerA_id partnerB_id"},
			{"singlechat", &Gm::GmCrossSingleChat, 2, "format:to_userid chat_content"},

			{"convertbuy", &Gm::GmConvertShopBuy, 2, "format:convert_type seq"},
			{ "randactivity",&Gm::GmRandActivityReq, 5 , "format:rand_activity_type opera_type param_1 param_2 param_3" },
			{ "addrandactivitynum", &Gm::GmRandActivityAdd, 2 , "format:free_num kill_num"},
			{ "addmysterious", &Gm::GmMysteriousTrial, 3 , "format:day seq num" },
			{ "setmysterious", &Gm::GmSetMysteriousTrial, 0, "format:" },
			{"bosschapter", &Gm::GmBossChapterReq, 2, "format:type param"},
			{"bountytask", &Gm::GmBountyTaskReq, 1, "format:type"},
			{ "transactiontask", &Gm::GmTransactionTask, 3, "format:op_type index refer_num"},
			{ "activesystem", &Gm::GmActiveSystem, 2, "format:op_type index"},

			{"escape", &Gm::GmEscapeCurrentPosition, 0, "format:"},
			{"switchplatform", &Gm::GmSwitchPlatform, 1, "format:scene_cfg"},

			{"testmatch", &Gm::GmTestMatch, 0, "format:"},
			{ "addactive", &Gm::GmAddActive, 1, "format:add_num" },
			{"autoresearch", &Gm::GmAutoResearchTask, 0, "format"},
			{ "petop", &Gm::GmPetOp, 4, "format:op_type p1 p2 p3"},
			{"summon", &Gm::GmTeamLeaderSummonMember, 1, "format:role_id"},
			{ "rolecmd", &Gm::GmRoleCmd, 5, "format:string chongzhi buy_type param1 param2"},
			{"patroltasktest",&Gm::GMPatrolTaskTest, 2, "format:type param_1" }, 
			{ "onekeyfinish",&Gm::GMOneKeyFinish, 1, "format:exe_type" },
			{ "partnertest",&Gm::GMPartnerTest, 5, "format:exe_type param_1 param_2 param_3 param_4" },
			{ "randactivitytest",&Gm::GMRandActivityTest, 0, "format:" },
			{ "huasheninforeq",&Gm::GmHuaShenInfoReq, 2, "format:op_type param1" },
			{ "npcdemo", &Gm::GmStartNpcDemoBattleReq, 1, "format:npc_demo_seq"},
			{ "huanjie",&Gm::GmHuanJieZhanChange, 3, "format:op_type param1 param2" },
			{ "loop", &Gm::GmTriggerUnknownReasonDisconnect, 1, "format:loop_times"},
			{ "huashenmedalreq", &Gm::GmHuaShenMedalReq , 3, "format:req_type param1 param2"},
			{ "testpb", &Gm::GmTestProtobuf, 1, "format:"},
			{ "pokertest", &Gm::GmSimulatePokerOper, 3, "format:exetype param1 param2" },
			{ "roleskill", &Gm::GmRoleSkillTest, 3, "format:exe_type param1 param2" },
			{ "resetgodtoken", &Gm::GmRoleResetGodToken, 0, "format:" },
			{ "genuid", &Gm::GmGenUniqueID, 1, "format:times" },
			{ "dayindex", &Gm::GmCheckDayIndex_6, 0, "format:"},
			{ "checkroleinbattle", &Gm::GmCheckRoleInBattle, 3, "format:type"},
			{ "refreshguildweektask", &Gm::GmRefreshGuildWeekTask, 0, "format:" },
			{ "partnerskin", &Gm::GmPartnerSkinTest, 3, "format:exe_type p1 p2" },
			{ "autocatchpet", &Gm::GmAutoCatchPet, 4, "format:monster_id common silver gold"},
			{ "facescore", &Gm::GmFacesCoreTest, 2, "format:opertype param_1" },
			{ "petskin", &Gm::GmPetSkin, 4, "format:type p1 p2 p3" },
			{ "formationrename", &Gm::GmFormationRename, 2, "format:seq name"},
			{ "formationinfo", &Gm::GmFormationDetailInfo, 1, "format:seq"},
			{ "formationsave", &Gm::GmFormationSave, 0, "format:"},
			{ "testcs", &Gm::GmTestCommonSave, 5, "format:op save_type p1 p2 p3" },
			{ "shouxihx", &Gm::GmChiefElectionSeaElection, 4, "format:" }, 
			{ "israndactivityopen", &Gm::IsRandActivityOpen,1 , "format: rand_activity_type" },
			{ "tianjianghaoli", &Gm::GmSetTianJiangHaoLi,2 , "format:begin_time end_time" },
			{ "addluckcoin", &Gm::GmAddLuckCoin, 1 , "format:count" },
			{ "buytoken", &Gm::GmBuyToken, 0, "format:" },
			{ "inorder", &Gm::GmInder, 3, "format:start end column"}, 
			{ "testrcs", &Gm::GmTestRoleCommonSave,4, "format:op save_type p1 p2" },
			{ "addshanhaisoul", &Gm::GmAddShanHaiSoulFragment, 1 , "format:star" },
			{ "wcf", &Gm::GmOnWeekChangeFlexible,2, "format:op_type p1" },
			{ "onlyfightff", &Gm::GmFastforwardOnlyFightEndEnterTime , 0, "format"},
			{ "guildfightff", &Gm::GmFastforwardGuildFightEndEnterTime , 0, "format"},
			{ "retrieveexp", &Gm::GmRetrieveExp, 1, "format:retrieve_exp"},
			{ "guildnoleader", &Gm::GmGuildNoLeader, 0, "format:"},
			{ "rolebuyitem", &Gm::GmGuildNoLeader, 0, "format:" },
			{ "buyitem", &Gm::GmRoleBuyItem, 4, "format:chongzhi buy_type param1 param2" },
			{ "buyitem2", &Gm::GmRoleBuyItem2, 1, "buy_item_type" },
			{ "setmiyipasscount", &Gm::GmMiYiSetPassCount, 1, "format:pass_count"},
			{ "addmatchuser", &Gm::GmAddMatchUser, 0, "format:"},
			{ "loadalluser", &Gm::GmLoadAllUserDataCalcSave, 0, "format:"},
			{ "qingyuanshenghui", &Gm::GmQingYuanShengHui,2, "format:" },
			{ "tuzikuaipao", &Gm::GmTuZiKuaiPao,2, "format:" },
			{ "ramakeup", &Gm::GmRandActivityMakeUp, 2, "format:ra_type make_up_type" },
			{ "setraendtime", &Gm::GmSetRandActivityEndTimestamp, 2, "format:rand_activity_type end_day"},
			{ "setfindendtime", &Gm::GmSetSpecialLogicActivityEndTime, 2, "format:type end_day"},

			{ "testnetwork", &Gm::GmTestNetwork, 2, "format:package_count package_length"},
			{ "resetshanhaicount", &Gm::GmResetShanHaiChallengeCount, 0 , "format:" },
			{ "reasonattrlist", &Gm::GmSendAttrListReason , 2, "format:reason_type param1"},
			{ "resetanecdote", &Gm::GmAnecdoteReset, 1, "format:anecdote_type"},
			{ "mydraw", &Gm::GmMyDraw, 0, "format:" },
			{ "teamformation", &Gm::GmTestTeamFormation, 0, "format:"},
			{ "getskillgobackretcoin", &Gm::GmGetSkillGoBackRetCoin, 6, "format:skill_id skill_level cur_level_exp go_back_level is_judge_prof_skill prof" },
			{ "exercise", &Gm::GmSchoolExercise, 1, "format:seq"},
			{ "setmazeraceactranktime", &Gm::GmSetMazeRaceActivityRankTimestamp, 0, "format:"},
			{ "zhuanzheng", &Gm::GmTempGroupBecomeOfficial, 1, "format:groupname"},
			{ "guidereq", &Gm::GmGuideReq, 2, "format:req_type param"},
			{ "printstack", &Gm::GmPrintfStack, 0, "format:"},
			{ "putlist", &Gm::GmTestPutList, 1, "format:"},
			{ "addgoodgiftnum", &Gm::GmAddGoodGiftTurnTableNum, 1, "format:num"},
			{ "rabonusreq", &Gm::GmRAbonusReq, 3, "format:rand_activity_type op_type param" },
			{ "chatgrouphandover", &Gm::GmChatGroupHandOver, 2, "format:group_id new_leader_uid"},
			{ "chatgrouphandover2", &Gm::GmChatGroupHandOver2, 1, "format:new_leader_uid"},
			{ "fetchactiveextra", &Gm::GmFetchActiveSystemExtraReward, 1, "format:type"},
			{ "setdreamnoteslevel", &Gm::GmSetDreamNotesChallengeLevel , 1, "format:level"},
			{ "setrealmbreak", &Gm::GmSetRealmBreak , 2, "format:op_type param1" },

			{ "petmonopoly", &Gm::GmPetMonopoly, 3, "format:opera_type param1 param2"},
			{ "cloudarena", &Gm::GmCloudArena, 3, "format:opera_type param1 param2" },
			{ "refreshshanhai", &Gm::GmRefreshShanHai, 0, "format:" },
			{ "fixdice", &Gm::GmFixDice, 1, "format:fix_dice"},
			{ "addcloudarenatimes", &Gm::GmAddCloudArenaTimes, 1, "format:add_times"},
			{ "addcloudarenascore", &Gm::GmAddCloudArenaScore, 1, "format:add_score"},
			{ "refreshshanhaichallenge", &Gm::GmRefreshShanHaiChallenge, 0, "format:" },
			{ "robottry", &Gm::GMRobottry, 3, "format: robot_type x y" },
			{ "jumptolayer", &Gm::GmJumpToLayer, 1, "format:layer"},
			{ "activeallfabao", &Gm::GmActiveAllFabao, 0, "format:"},
			{ "cloudarenacd", &Gm::GMResetCloudArenaRefreshCD, 0, "format:"},
			{ "cloudarenaformation", &Gm::GMCloudArenaRandomFormation, 0, "format"},
			{ "randompethelperformation", &Gm::GMRandomPetHelperFormation, 1, "format:system_type"},
			{ "setsmartmounts", &Gm::GmSetSmartMountsSkill, 3, "format:smart_mounts_index skill_index skill_id"},
			{ "cloudarenatitle", &Gm::GmTestCloudArenaTitle, 2, "format:last_season_rank,expire_seconds"},
			{ "checkuniqueid", &Gm::GmCheckUniqueID, 1, "format:unique_id"},
			{ "testchivalrous", &Gm::GmTestChivalrous, 1, "format:type"},
			{ "setwildbosstimes", &Gm::GmSetWildBossRewardTimes, 1, "format:times" },			
			{ "setsmartmountsenergy", &Gm::GmSetSmartMountsEnergy, 2, "format:index energy" },

			{ "columntransfer", &Gm::GmCheckItemWrongColumn, 0, "format:type"},
			{ "getguildactivereward" , &Gm::GmGetGuildActiveReward , 0 , "format:"},
			{ "getguildactivepersonrank" , &Gm::GmGetGuildActivePersonRank , 0 , "format:" },
			{ "guildactiveaddday", &Gm::GmGuildActiveAddDay, 0, "format:" },
			{ "addguildactive" ,&Gm::GmAddGuildActive, 1, "format:val" },

			{ "querypethelperformation", &Gm::GMQueryPetHelperFormation, 1, "format:system_type" },
			{ "couragechallengeopenui", &Gm::GmCourageChallengeOpenUI, 0, "format:"},

			{ "hongbaotest", &Gm::GmHongBaoTest, 3, "format:req_type  param1  param2" },
			{ "guildactiveaddday", &Gm::GmGuildActiveAddDay, 0, "format:" }, 
			{ "addguildactive" ,&Gm::GmAddGuildActive, 1, "format:val" },
			{ "setzhuanshulibao" ,&Gm::GmSetZhuanShuLiBaoDayGift, 0, "format:" },
			{ "prestigetask", &Gm::GmPrestigeTaskTest, 2, "format:"},
			{ "hjzcmatch1", &Gm::GmTestHuanJieZhanChangMatch1, 1, "format:"},
			{ "hjzcmatch2", &Gm::GmTestHuanJieZhanChangMatch2, 4, "format:total_time_s interval_s add_team_num match_interval_s"},
			{ "addweekactivedegree", &Gm::GmAddWeekActiveDegree, 1, "format:" },
			{ "addactiveturntabletime", &Gm::GmAddActiveTurntableTime, 1, "format:" },
			{ "setrabackreward", &Gm::GmSetRABackReward, 2, "format: "},
			{ "changekillvalue", &Gm::GmChangeKillValue, 1, "format:num" },
			{ "addtianmingexp", &Gm::GMAddTianMingExp, 1, "format:exp" },
			{ "resettask", &Gm::GmResetTask, 1, "format:task_id"},
			{ "bigevent", &Gm::GMTestAnecdoteBigEvent, 1, "format:seq"},
			{ "printuserstatus", &Gm::GmPrintUserStatus, 0, "format:"},
			{ "giveflower", &Gm::GmGiveFlower ,4 , "format:target_id , item_id , num, is_auto"},
			{ "jieyi", &Gm::GmJieYi ,3 , "format:" },
			{ "qy", &Gm::GmQingYuan ,3 , "format:" },
			{ "resetgmattr", &Gm::GmResetGmAttr, 0, "format:"},
			{ "testcapability", &Gm::GmTestCapability, 0, "format:"},
			{ "resetcowreportgoodnewsdata" , &Gm::GmCowReportGoodNewsResetData , 0 , "format:"},
			{ "advancecareertest" , &Gm::GmAdvanceCareerTest , 2 , "format:to_prof to_color" },
			{ "queryprice", &Gm::GmQueryTradeItemPrice, 2, "format:" },
			{ "forbidtalk", &Gm::GmForbidTalk, 2, "format:seconds type"},
			{ "soarskill", &Gm::GmSoaringSkill, 2, "format:skill_id skill_lv"},
			{ "resetguildbosstimes", &Gm::GmResetGuildBossTimes, 0, "format:"},
			{ "resettigerbringsblessingdata" , &Gm::GmTigerBringsBlessingResetData , 0 , "format:" },
			{ "weddingnextlink", &Gm::GmWeddingNextLink, 0, "format:" },
			{ "weddingtry", &Gm::GmWeddingTry, 0, "format:" },
			{ "xunyoucar", &Gm::GmXunYouCar, 1, "format: xun_you_type" },
			{ "redisset", &Gm::GmTestRedisSET, 2, "format:test_times character_num"},
			{ "redisget", &Gm::GmTestRedisGET, 2, "format:test_times character_num" },
			{ "asynctasktest", &Gm::GmAsyncTaskTest, 0, "format:"},
			{ "stopserver", &Gm::GmStopServer, 0, "format:"},
			{"xinfubipin", &Gm::GmTestXinFuBiPinReward, 0, "format:"},

			{ "waoper", &Gm::GmWorldArenaOperate, 3, "format:type p1 p2" },
			{ "wactrl", &Gm::GmWorldArenaControl, 3, "format:type p1 p2" },
			{ "minigamestage", &Gm::GmMiniGameClearNextStage, 0, "format:" },
			{ "minigamephase", &Gm::GmMiniGameFetchPhaseReward, 1, "format:phaseseq" },
			{ "addutacr", &Gm::GmAddUTAChampionRecord , 1 , "format: rank " },
			{ "getutacr" ,&Gm::GmUTAChampionRecordReq  , 2 , "format: begin_index  end_index " },
			{ "utaenterscene", &Gm::GmTryEnterUTAScene, 0, "format"},
			{ "utawatchvideo", &Gm::GmUTAWatchVideo, 1, "format:video_id"},
			{ "utaob", &Gm::GmUTAStartOB, 1, "format:schedule_id"},
			{ "fastforwardbattle", &Gm::GmFastForwardBattle, 0, "format:"},
			{ "testtime", &Gm::GmTestTime, 1, "format:timestamp"},
			{ "setholyskill", &Gm::GmHallowStudySkill, 2, "format:skill_id hole[0,5)"},
			{ "setholygift", &Gm::GmHallowSetGift, 2, "format:gift hole[0,2)"},
			{ "elfshowuptest", &Gm::GmTestElementElfPosibility , 2, "format:today_showup_times rand_times"},

			{ "kuafuyingxionglu", &Gm::GmKuaFuYingXiongLu, 1, "format: op_type" },

			{ "chatrecord", &Gm::GmQueryUserChatRecord, 0, "format:"},
			{ "watchvideo", &Gm::GmWatchVideo, 1, "format:file_id"},
			{"nocheckmove", &Gm::GmSetWaiguaMoveFlag, 1, "format:is_no_check"},
			{ "minigamebattle", &Gm::GmMiniGameBattleReq, 4, "format:" },
			{ "activehuanshou", &Gm::GmHuanShouActive, 1, "format:huanshou_id" },
			{ "null", NULL, 0, "format:" }
		};
		
		return &stCommandFuncItem[0];
	}

	bool MakeupResult(const GSNetID &netid, const char *commandType, bool ret, const char *addition_str);

	// ------------------------------------------------------------------------------

	// @brief 按空格分析字符串 并分解到一个数组中
	// @src 源字符串 
	// @result 目的缓冲区
	// @spilt 分解字符串 
	// @return 返回多少个参数 
	int ParseCommandParam(char *src, char **result, const char *spilt);

	void TransferCommandToBigCross(Role *role, Protocol::CSGMCommand *command);
};

#endif  // __GM_HPP__

