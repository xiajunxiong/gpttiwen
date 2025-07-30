#include "common/memory/mempool.h"


#include "global/colosseum/gladiator.hpp"
REGISTER_MEMPOOL(gamemempool, Gladiator, 64, "Gladiator")

// buff
#include "other/buffmanager/buffbase.hpp"
REGISTER_MEMPOOL(gamemempool, BuffBase, 64, "BuffBase");
REGISTER_MEMPOOL(gamemempool, BuffChangeAttr, 64, "BuffChangeAttr");
REGISTER_MEMPOOL(gamemempool, BuffExpUp, 64, "BuffExpUp");
REGISTER_MEMPOOL(gamemempool, BuffChangeMoveSpeed, 64, "BuffChangeMoveSpeed");

// ÓÊ¼þ
#include "global/mail/mailitem.hpp"
#include "global/mail/usermail.hpp"
REGISTER_MEMPOOL(gamemempool, MailItem, 64, "MailItem");
REGISTER_MEMPOOL(gamemempool, UserMail, 64, "UserMail");

#include "global/guild/guild.hpp"
#include "global/guild/guildmember.hpp"
REGISTER_MEMPOOL(gamemempool, GuildMember, 64, "GuildMember")
REGISTER_MEMPOOL(gamemempool, Guild, 64, "Guild");

#include "global/rmibackobjdef.h"
#include "rmibackobjdef.h"
REGISTER_MEMPOOL(gamemempool, RMICreateGuildBackObjectImpl, 64, "RMICreateGuildBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIDismissGuildBackObjectImpl, 64, "RMIDismissGuildBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIInitGuildBackObjectImpl, 64, "RMIInitGuildBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveGuildBackObjectImpl, 64, "RMISaveGuildBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIInitOriginUserCacheBackObjectImpl, 64, "RMIInitOriginUserCacheBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIWriteCmdResultBackObjectImpl, 8, "RMIWriteCmdResultBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIRoleQueryBackObjectImpl, 16, "RMIRoleQueryBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIQueryRetrievePetBackObjectImpl, 4, "RMIQueryRetrievePetBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitHongBaoBackObjectImpl, 64, "RMIInitHongBaoBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveHongBaoBackObjectImpl, 64, "RMISaveHongBaoBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitFriendIntimacyBackObjectImpl, 64, "RMIInitFriendIntimacyBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveFriendIntimacyBackObjectImpl, 64, "RMISaveFriendIntimacyBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitCowReportGoodNewsBuyListBackObjectImpl, 64, "RMIInitCowReportGoodNewsBuyListBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveCowReportGoodNewsBuyListBackObjectImpl, 64, "RMISaveCowReportGoodNewsBuyListBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitFriendBackObjectImpl, 64, "RMIInitFriendBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveFriendBackObjectImpl, 64, "RMISaveFriendBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitBlackBackObjectImpl, 64, "RMIInitBlackBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveBlackBackObjectImpl, 64, "RMISaveBlackBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitTigerBringsBlessingBuyListBackObjectImpl, 64, "RMIInitTigerBringsBlessingBuyListBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveTigerBringsBlessingBuyListBackObjectImpl, 64, "RMISaveTigerBringsBlessingBuyListBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitWeddingBackObjectImpl, 64, "RMIInitWeddingBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveWeddingBackObjectImpl, 64, "RMISaveWeddingBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitWorldArenaBaseDataBackObjectImpl, 8, "RMIInitWorldArenaBaseDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveWorldArenaBaseDataBackObjectImpl, 8, "RMISaveWorldArenaBaseDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitWorldArenaUserDataBackObjectImpl, 8, "RMIInitWorldArenaUserDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveWorldArenaUserDataBackObjectImpl, 8, "RMISaveWorldArenaUserDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitWorldArenaScheduleDataBackObjectImpl, 8, "RMIInitWorldArenaScheduleDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveWorldArenaScheduleDataBackObjectImpl, 8, "RMISaveWorldArenaScheduleDataBackObjectImpl");

#include "global/team/team.hpp"
#include "global/team/teammatch.hpp"
#include "global/team/temporary_chat_group.hpp"
REGISTER_MEMPOOL(gamemempool, Team, 64, "Team");
REGISTER_MEMPOOL(gamemempool, MatchTeamInfo, 64, "MatchTeamInfo");
REGISTER_MEMPOOL(gamemempool, MatchUserInfo, 64, "MatchUserInfo");
REGISTER_MEMPOOL(gamemempool, TemporaryChatGroup, 64, "TemporaryChatGroup");

#include "global/rmibackobjdef.h"
REGISTER_MEMPOOL(gamemempool, RMIInitWorldStatusBackObjectImpl, 64, "RMIInitWorldStatusBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveWorldStatusBackObjectImpl, 64, "RMISaveWorldStatusBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIHeartbeatCheckBackImpl, 64, "RMIHeartbeatCheckBackImpl")

#include "rmibackobjdef.h"
REGISTER_MEMPOOL(gamemempool, RMIRoleInitBackObjectImpl, 64, "RMIRoleInitBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMIRoleSaveStatusBackObjectImpl, 64, "RMIRoleSaveStatusBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIRoleSaveBackObjectImplRole, 64, "RMIRoleSaveBackObjectImplRole");
REGISTER_MEMPOOL(gamemempool, RMIRoleSaveBackObjImplLocal, 64, "RMIRoleSaveBackObjImplLocal");
REGISTER_MEMPOOL(gamemempool, RMIGetGoldBackObjectImpl, 64, "RMIGetGoldBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIChangeGoldBackObjectImpl, 64, "RMIChangeGoldBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIChangeGoldBackObjectImplErrorAdd, 64, "RMIChangeGoldBackObjectImplErrorAdd");
REGISTER_MEMPOOL(gamemempool, RMIRoleMuteBackObjectImpl, 64, "RMIRoleMuteBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIRoleCmdSetAuthorityBackObjectImpl, 64, "RMIRoleCmdSetAuthorityBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIGetRoleCmdImpl, 64, "RMIGetRoleCmdImpl");
REGISTER_MEMPOOL(gamemempool, RMIComfirmRoleCmdImpl, 64, "RMIComfirmRoleCmdImpl");
REGISTER_MEMPOOL(gamemempool, RMIComfirmRoleCmdErrorImpl, 64, "RMIComfirmRoleCmdErrorImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitOfflineRegisterBackObjectImpl, 64, " RMIInitOfflineRegisterBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveOfflineRegisterBackObjectImpl, 64, " RMISaveOfflineRegisterBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitBabyDataBackObjectImpl, 64, " RMIInitBabyDataBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveBabyDataBackObjectImpl, 64, " RMISaveBabyDataBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMISaveMailBackObjectImpl, 64, "RMISaveMailBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIInitMailsBackObjectImpl, 64, "RMIInitMailsBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIGetSystemMailsBackObjectImpl, 64, "RMIGetSystemMailsBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIRemoveSystemMailsBackObjectImpl, 64, "RMIRemoveSystemMailsBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIQueryNameExistBackObjectImpl, 64, "RMIQueryNameExistBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMIResetNameBackObjectImpl, 64, "RMIResetNameBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitActivityDataBackImpl, 64, "RMIInitActivityDataBackImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveActivityDataBackImpl, 64, "RMISaveActivityDataBackImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitTradeMarketDataBackObjectImpl, 64, "RMIInitTradeMarketDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveTradeMarketDataBackObjectImpl, 64, "RMISaveTradeMarketDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitUserTradeRecordBackObjectImpl, 64, "RMIInitUserTradeRecordBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveUserTradeRecordBackObjectImpl, 64, "RMISaveUserTradeRecordBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitSystemTradeRecordBackObjectImpl, 64, "RMIInitSystemTradeRecordBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveSystemTradeRecordBackObjectImpl, 64, "RMISaveSystemTradeRecordBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitSystemRecycleRuleBackObjectImpl, 64, "RMIInitSystemRecycleRuleBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveSystemRecycleRuleBackObjectImpl, 64, "RMISaveSystemRecycleRuleBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitSystemAutoUpshelveBackObjectImpl, 16, "RMIInitSystemAutoUpshelveBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveSystemAutoUpshelveBackObjectImpl, 16, "RMISaveSystemAutoUpshelveBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitPersonRankBackObjectImpl, 64, " RMIInitPersonRankBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISavePersonRankBackObjectImpl, 64, " RMISavePersonRankBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitTeamRankBackObjectImpl, 64, " RMIInitTeamRankBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveTeamRankBackObjectImpl, 64, " RMISaveTeamRankBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitChallengeUserObjectImpl, 64, " RMIInitChallengeUserObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveChallengeUserObjectImpl, 64, " RMISaveChallengeUserObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitActivityRoleDataBackObjectImpl, 64, " RMIInitActivityRoleDataBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveActivityRoleDataBackObjectImpl, 64, " RMISaveActivityRoleDataBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitCommonDataBackObjectImpl, 64, " RMIInitCommonDataBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveCommonDataBackObjectImpl, 64, " RMISaveCommonDataBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitPlatformBattleBackObjectImpl, 64, " RMIInitPlatformBattleBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISavePlatformBattleBackObjectImpl, 64, " RMISavePlatformBattleBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitServerFirstKillBackObjectImpl, 64, " RMIInitServerFirstKillBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveServerFirstKillBackObjectImpl, 64, " RMISaveServerFirstKillBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitAnimalRaceBackObjectImpl, 64, " RMIInitAnimalRaceBackObjectImpl")
REGISTER_MEMPOOL(gamemempool, RMISaveAnimalRaceBackObjectImpl, 64, " RMISaveAnimalRaceBackObjectImpl")

REGISTER_MEMPOOL(gamemempool, RMIHiddenGetRoleInfoImpl, 32, "RMIHiddenGetRoleInfoImpl")

REGISTER_MEMPOOL(gamemempool, RMIInitBattleVideoDataBackObjectImpl, 4, "RMIInitBattleVideoDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveBattleVideoDataBackObjectImpl, 4, "RMISaveBattleVideoDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitChatGroupBackObjectImpl, 32, "RMIInitChatGroupBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveChatGroupBackObjectImpl, 32, "RMISaveChatGroupBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitCrossChatGroupBackObjectImpl, 32, "RMIInitCrossChatGroupBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveCrossChatGroupBackObjectImpl, 32, "RMISaveCrossChatGroupBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitPetRankBackObjectImpl, 32, "RMIInitPetRankBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISavePetRankBackObjectImpl, 32, "RMISavePetRankBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitPartnerRankBackObjectImpl, 32, "RMIInitPartnerRankBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISavePartnerRankBackObjectImpl, 32, "RMISavePartnerRankBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitShiTuSeekingDataBackObjectImpl, 32, "RMIInitShiTuSeekingDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveShiTuSeekingDataBackObjectImpl, 32, "RMISaveShiTuSeekingDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitQuanMinDiscountBackObjectImpl, 64, "RMIInitQuanMinDiscountBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveQuanMinDiscountBackObjectImpl, 64, "RMISaveQuanMinDiscountBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitCourageChallengeRankBackObjectImpl, 64, "RMIInitCourageChallengeRankBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveCourageChallengeRankBackObjectImpl, 64, "RMISaveCourageChallengeRankBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitGatherDreamRecordBackObjectImpl, 64, "RMIInitGatherDreamRecordBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveGatherDreamRecordBackObjectImpl, 64, "RMISaveGatherDreamRecordBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitCloudArenaDataBackObjectImpl, 4, "RMIInitCloudArenaDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveCloudArenaDataBackObjectImpl, 4, "RMISaveCloudArenaDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitCloudArenaUserDataBackObjectImpl, 4, "RMIInitCloudArenaUserDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveCloudArenaUserDataBackObjectImpl, 4, "RMISaveCloudArenaUserDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitRAPaiMaiHangBackObjectImpl, 64, "RMIInitRAPaiMaiHangBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveRAPaiMaiHangBackObjectImpl, 64, "RMISaveRAPaiMaiHangBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitMarketItemDataBackObjectImpl, 64, "RMIInitMarketItemDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveMarketItemDataBackObjectImpl, 64, "RMISaveMarketItemDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitWorldTeamArenaSignUpDataBackObjectImpl, 64, "RMIInitWorldTeamArenaSignUpDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveWorldTeamArenaSignUpDataBackObjectImpl, 64, "RMISaveWorldTeamArenaSignUpDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitWorldTeamArenaTeamRankDataBackObjectImpl, 64, "RMIInitWorldTeamArenaTeamRankDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveWorldTeamArenaTeamRankDataBackObjectImpl, 64, "RMISaveWorldTeamArenaTeamRankDataBackObjectImpl");

REGISTER_MEMPOOL(gamemempool, RMIInitOfflineUserBattleDataBackObjectImpl, 16, "RMIInitOfflineUserBattleDataBackObjectImpl");
REGISTER_MEMPOOL(gamemempool, RMISaveOfflineUserBattleDataBackObjectImpl, 16, "RMISaveOfflineUserBattleDataBackObjectImpl");

#include "global/offlineregister/offlineregister.hpp"
REGISTER_MEMPOOL(gamemempool, OfflineRegister, 64, "OfflineRegister")

#include "scene/aoi/obseritem.h"
REGISTER_MEMPOOL(gamemempool, ObserItem, 64, "ObserItem");

#include "timer/forcekickuserdelay.hpp"
REGISTER_MEMPOOL(gamemempool, ForceKickUserDelay, 64, "ForceKickUserDelay");

#include "scene/scene.h"
REGISTER_MEMPOOL(gamemempool, Scene, 128, "Scene");

#include "other/msgcachemanager/msgcachemanager.hpp"
REGISTER_MEMPOOL(gamemempool, MsgCacheBuf, 128, "MsgCacheBuf");

//role module
#include "item/money.h"
#include "item/knapsack.h"

#include "task/taskrecorder.h"
#include "task/taskmanager.h"

#include "friend/friendrecommend.hpp"

#include "equipment/equipmentmanager.h"

#include "other/rolemodulemanager.hpp"
#include "other/vip/vip.hpp"
#include "other/roleattribute/roleattributeplan.hpp"
#include "other/monitor/monitor.hpp"
#include "other/pet/pet.hpp"
#include "other/prestige/prestige.hpp"
#include "other/partner/partner.hpp"
#include "other/collection/collection.hpp"
#include "other/title/title.hpp"
#include "other/train/train.hpp"
#include "other/wst_question/wst_question.hpp"
#include "other/skill/roleskill.hpp"
#include "other/dayreward/dayreward.hpp"
#include "other/workshop/workshop.hpp"
#include "other/surface/surface.hpp"
#include "other/marriage/marriage.hpp"
#include "other/baby/baby.hpp"
#include "other/role_setting/rolesetting.hpp"
#include "other/mentor/shitu.hpp"
#include "other/systemnotice/systemnotice.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/monster_wave/monsterwave.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "other/schooltask/schooltask.h"
#include "other/offlineexp/offlineexp.hpp"
#include "other/escort/escort.hpp"
#include "other/treasure_map/treasuremap.h"
#include "other/taskchain/taskchain.hpp"
#include "other/fabao/fabao.hpp"
#include "other/researchtask/researchtask.h"
#include "other/medal/medal.hpp"
#include "other/buffmanager/buffmanager.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/shop/shop.h"
#include "other/mapunlock/mapunlock.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/exchange/exchange.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/salary/salary.hpp"
#include "other/challengefb/rolechallengefb.hpp"
#include "other/facescore/facescore.hpp"
#include "other/funcguide/funcguide.hpp"
#include "other/roledrop/roledrop.hpp"
#include "other/roleconvertshop/roleconvertshop.hpp"
#include "other/specialappearance/specialappearance.hpp"
#include "other/rolemail/rolemail.hpp"
#include "other/loginsystem/loginsystem.hpp"
#include "other/boss_chapter/boss_chapter.hpp"
#include "other/capability/capability.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/funcpreview/funcpreview.hpp"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "other/auto_catch_pet/auto_catch_pet.hpp"
#include "other/trade_credit/trade_credit.hpp"
#include "other/formation/formation.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "other/huanjie/rolehuanjie.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/schoolexercise/schoolexercise.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "other/rolematerial/rolematerial.hpp"
#include "other/mapexploration/mapexploration.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"
#include "other/custominstruction/custominstruction.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/roleheartskill/roleheartskill.hpp"
#include "other/rolezhuling/rolezhuling.hpp"
#include "other/formation/pethelperformation.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "other/petgodfb/petgodfb.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "other/roletiandidaohen/roletiandidaohen.hpp"
#include "other/rolegivegift/rolegivegift.hpp"
#include "other/prestige_task/prestige_task.hpp"
#include "other/svip/svip.hpp"
#include "other/roleattrpellet/roleattrpellet.hpp"
#include "other/duel/duel.hpp"
#include "equipment/accessoriesmentmanager.h"
#include "other/tianming/tianming.hpp"
#include "other/dujie/dujie.hpp"
#include "other/yinyangeye/yinyangeye.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/endlesstower/endlesstower.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "other/wanlingfb/wanlingfb.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/mirageterritory/mirageterritory.hpp"
#include "other/xiuwei/xiuwei.hpp"
#include "other/treasuretask/treasuretask.hpp"
#include "other/fallengod/fallengod.hpp"
#include "other/minigame/minigame.hpp"
#include "other/peaktoken/rolepeaktoken.hpp"
#include "other/income/income.hpp"
#include "other/yaoxingshilian/yaoxingshilian.hpp"
#include "other/levelcomplement/newlevelcomplement.hpp"
#include "other/cycleactivity/sanxianguidong/sanxianguidong.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddie.hpp"
#include "other/cycleactivity/whereisthefairy/rolewhereisthefairy.hpp"
#include "other/cycleactivity/tiangongxunbao/tiangongxunbao.hpp"
#include "other/cycleactivity/loversmarket/loversmarket.hpp"
#include "other/cycleactivity/taoquannaji/taoquannaji.hpp"
#include "other/cycleactivity/concentricknot/concentricknot.hpp"
#include "other/rolehundredgodboss/rolehundredgodboss.hpp"
#include "other/zaohuaequip/zaohuaequipmanager.hpp"
#include "other/hongmengtianjie/rolehongmengtianjie.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "other/cycleactivity/kejuexam/kejuexam.hpp"
#include "other/cycleactivity/zhouqishop/zhouqishop.hpp"
#include "other/elementelf/elementelf.hpp"
#include "other/fuyu/fuyumanager.hpp"
#include "other/kuafuyingxionglu/rolekuafuyingxionglu.hpp"
#include "other/starchart/starchart.h"
#include "other/couragechallenge/newcouragechallenge.hpp"
#include "other/fashioncollection/fashioncollection.hpp"
#include "other/roleguildtask/roleguildtask.hpp"
#include "other/huanshou/rolehuanshou.hpp"

REGISTER_MEMPOOL(gamemempool, Exchange, 64, "Exchange");
REGISTER_MEMPOOL(gamemempool, Shop, 64, "Shop");
REGISTER_MEMPOOL(gamemempool, BuffManager, 64, "BuffManager");
REGISTER_MEMPOOL(gamemempool, RoleModuleManager, 64, "RoleModuleManager");
REGISTER_MEMPOOL(gamemempool, Vip, 64, "Vip");
REGISTER_MEMPOOL(gamemempool, RoleAttributePlan, 64, "RoleAttribute");
REGISTER_MEMPOOL(gamemempool, Monitor, 64, "Monitor");
REGISTER_MEMPOOL(gamemempool, Money, 64, "Money");
REGISTER_MEMPOOL(gamemempool, Knapsack, 64, "Knapsack");
REGISTER_MEMPOOL(gamemempool, Pet, 64, "Pet");
REGISTER_MEMPOOL(gamemempool, Partner, 64, "Partner");
REGISTER_MEMPOOL(gamemempool, Prestige, 64, "Prestige");
REGISTER_MEMPOOL(gamemempool, Collection, 64, "Collection");
REGISTER_MEMPOOL(gamemempool, Title, 64, "Title");
REGISTER_MEMPOOL(gamemempool, Train, 64, "Train");
REGISTER_MEMPOOL(gamemempool, TaskRecorder, 64, "TaskRecorder");
REGISTER_MEMPOOL(gamemempool, TaskManager, 64, "TaskManager");
REGISTER_MEMPOOL(gamemempool, WstQuestion, 64, "WstQuestion");
REGISTER_MEMPOOL(gamemempool, RoleSkill, 64, "RoleSkill");
REGISTER_MEMPOOL(gamemempool, DayReward, 64, "DayReward");
REGISTER_MEMPOOL(gamemempool, EquipmentManager, 64, "EquipmentManager");
REGISTER_MEMPOOL(gamemempool, Workshop, 64, "Workshop");
REGISTER_MEMPOOL(gamemempool, Surface, 64, "Surface");
REGISTER_MEMPOOL(gamemempool, Marriage, 64, "Marriage");
REGISTER_MEMPOOL(gamemempool, Baby, 64, "Baby");
REGISTER_MEMPOOL(gamemempool, FriendRecommend, 64, "FriendRecommend");
REGISTER_MEMPOOL(gamemempool, RoleSetting, 64, "RoleSetting");
REGISTER_MEMPOOL(gamemempool, ShiTu, 64, "ShiTu");
//REGISTER_MEMPOOL(gamemempool, SystemNotice, 64, "SystemNotice");
REGISTER_MEMPOOL(gamemempool, CourageChallenge, 64, "CourageChallenge");
REGISTER_MEMPOOL(gamemempool, MonsterWave, 64, "MonsterWave");
REGISTER_MEMPOOL(gamemempool, RoleActivityManager, 64, "RoleActivityManager");
REGISTER_MEMPOOL(gamemempool, MiYi, 64, "MiYi");
REGISTER_MEMPOOL(gamemempool, SchoolTask, 64, "SchoolTask");
REGISTER_MEMPOOL(gamemempool, OfflineExp, 64, "OfflineExp");
REGISTER_MEMPOOL(gamemempool, Escort, 64, "Escort");
REGISTER_MEMPOOL(gamemempool, TreasureMap, 64, "TreasureMap");
REGISTER_MEMPOOL(gamemempool, TaskChain, 64, "TaskChain");
REGISTER_MEMPOOL(gamemempool, FaBao, 64, "FaBao");
REGISTER_MEMPOOL(gamemempool, ResearchTask, 64, "ResearchTask")
REGISTER_MEMPOOL(gamemempool, Medal, 64, "Medal");
REGISTER_MEMPOOL(gamemempool, RoleCrystalFb, 64, "RoleCrystalFb");
//REGISTER_MEMPOOL(gamemempool, MapUnlock, 64, "MapUnlock")
REGISTER_MEMPOOL(gamemempool, RoleGatherFb, 64, "RoleGatherFb")
REGISTER_MEMPOOL(gamemempool, RoleCross, 64, "RoleCross")
REGISTER_MEMPOOL(gamemempool, CourseOfGrowth, 64, "CourseOfGrowth")
REGISTER_MEMPOOL(gamemempool, Salary, 64, "Salary");
REGISTER_MEMPOOL(gamemempool, RoleChallengeFb, 64, "RoleChallengeFb");
REGISTER_MEMPOOL(gamemempool, FaceScore, 64, "FaceScore");
REGISTER_MEMPOOL(gamemempool, FuncGuide, 64, "FuncGuide");
REGISTER_MEMPOOL(gamemempool, RoleDrop, 64, "RoleDrop");
REGISTER_MEMPOOL(gamemempool, RoleConvertShop, 64, "RoleConvertShop");
REGISTER_MEMPOOL(gamemempool, SpecialAppearance, 64, "SpecialAppearance");
REGISTER_MEMPOOL(gamemempool, RoleMail, 64, "RoleMail");
REGISTER_MEMPOOL(gamemempool, LoginSystem, 64, "RoleLoginSystem");
REGISTER_MEMPOOL(gamemempool, BossChapter, 64, "BossChapter");
REGISTER_MEMPOOL(gamemempool, Capability, 64, "RoleCapability");
REGISTER_MEMPOOL(gamemempool, MysteriousTrial, 64, "RoleMysteriousTrial");
REGISTER_MEMPOOL(gamemempool, BountyTask, 64, "BountyTask");
REGISTER_MEMPOOL(gamemempool, TransactionTask, 64, "RoleTransactionTask");
REGISTER_MEMPOOL(gamemempool, ActiveSystem, 64, "RoleActiveSystem");
REGISTER_MEMPOOL(gamemempool, HuaShenManager, 64, "HuaShenManager");
REGISTER_MEMPOOL(gamemempool, FuncPreview, 64, "FuncPreview");
REGISTER_MEMPOOL(gamemempool, RoleFirstKill, 64, "RoleFirstKill");
REGISTER_MEMPOOL(gamemempool, AutoCatchPet, 64, "AutoCatchPet");
REGISTER_MEMPOOL(gamemempool, TradeCredit, 64, "TradeCredit");
REGISTER_MEMPOOL(gamemempool, RoleFormation, 64, "RoleFormation");
REGISTER_MEMPOOL(gamemempool, RoleGatherDream, 64, "RoleGatherDream");
REGISTER_MEMPOOL(gamemempool, RoleHuanJie, 64, "RoleHuanJie");
REGISTER_MEMPOOL(gamemempool, AnecdoteTask, 64, "AnecdoteTask");
REGISTER_MEMPOOL(gamemempool, LevelComplement, 64, "LevelComplement");
REGISTER_MEMPOOL(gamemempool, DreamNotesChallenge, 64, "DreamNotesChallenge");
REGISTER_MEMPOOL(gamemempool, SchoolExercise, 64, "SchoolExercise");
REGISTER_MEMPOOL(gamemempool, RoleCommonSaveManager, 64, "RoleCommonSaveManager");
REGISTER_MEMPOOL(gamemempool, SmartMountsSystem, 64, "SmartMountsSystem");
REGISTER_MEMPOOL(gamemempool, RealmBreak, 64, "RealmBreak");
REGISTER_MEMPOOL(gamemempool, PetMonopoly, 64, "PetMonopoly");
REGISTER_MEMPOOL(gamemempool, LunHuiWangChuan, 64, "LunHuiWangChuan");
REGISTER_MEMPOOL(gamemempool, RoleMaterial, 64, "RoleMaterial");
REGISTER_MEMPOOL(gamemempool, MapExploration, 64, "MapExploration");
REGISTER_MEMPOOL(gamemempool, RoleJoinActivityReward, 64, "RoleJoinActivityReward");
REGISTER_MEMPOOL(gamemempool, CustomInstruction, 64, "CustomInstruction");
REGISTER_MEMPOOL(gamemempool, RoleGuildInborn, 64, "RoleGuildInborn");
REGISTER_MEMPOOL(gamemempool, RoleHeartSkill, 64, "RoleHeartSkill");
REGISTER_MEMPOOL(gamemempool, RoleZhuLing, 64, "RoleZhuLing");
REGISTER_MEMPOOL(gamemempool, PetHelperFormation, 64, "PetHelperFormation");
REGISTER_MEMPOOL(gamemempool, NetherWorldPurgatory, 64, "NetherWorldPurgatory");
REGISTER_MEMPOOL(gamemempool, PetGodFB, 64, "PetGodFB");
REGISTER_MEMPOOL(gamemempool, BigDipper, 64, "BigDipper");
REGISTER_MEMPOOL(gamemempool, RoleTianDiDaoHen, 64, "RoleTianDiDaoHen");
REGISTER_MEMPOOL(gamemempool, RoleGiveGift, 64, "RoleGiveGift");
REGISTER_MEMPOOL(gamemempool, PrestigeTask, 64, "PrestigeTask");
REGISTER_MEMPOOL(gamemempool, SVip, 64, "SVip");
REGISTER_MEMPOOL(gamemempool, RoleAttrPellet, 64, "RoleAttrPellet");
REGISTER_MEMPOOL(gamemempool, Duel, 64, "Duel");
REGISTER_MEMPOOL(gamemempool, AccessoriesmentManager, 64, "AccessoriesmentManager");
REGISTER_MEMPOOL(gamemempool, TianMing, 64, "TianMing");
REGISTER_MEMPOOL(gamemempool, DuJie, 64, "DuJie");
REGISTER_MEMPOOL(gamemempool, YinYangEye, 64, "YinYangEye");
REGISTER_MEMPOOL(gamemempool, EndlessTower, 64, "EndlessTower");
REGISTER_MEMPOOL(gamemempool, AdvanceCareer, 64, "AdvanceCareer");
REGISTER_MEMPOOL(gamemempool, RoleWanLingFB, 64, "WamLingFB");
REGISTER_MEMPOOL(gamemempool, RoleNewLifeSkill, 64, "RoleNewLifeSkill");
REGISTER_MEMPOOL(gamemempool, FlyUpSystem, 64, "FlyUpSystem");
REGISTER_MEMPOOL(gamemempool, MirageTerritory, 64, "MirageTerritory");
REGISTER_MEMPOOL(gamemempool, XiuWei, 64, "XiuWei");
REGISTER_MEMPOOL(gamemempool, TreasureTask, 64, "TreasureTask");
REGISTER_MEMPOOL(gamemempool, FallenGod, 64, "FallenGod");
REGISTER_MEMPOOL(gamemempool, MiniGame, 64, "MiniGame");
REGISTER_MEMPOOL(gamemempool, RolePeakToken, 64, "RolePeakToken");
REGISTER_MEMPOOL(gamemempool, RoleIncome, 64, "RoleIncome");
REGISTER_MEMPOOL(gamemempool, RoleNewLevelComplement, 64, "RoleNewLevelComplement");
REGISTER_MEMPOOL(gamemempool, YaoXingShiLian, 64, "YaoXingShiLian");
REGISTER_MEMPOOL(gamemempool, SanXianGuiDong, 64, "SanXianGuiDong");
REGISTER_MEMPOOL(gamemempool, LanternRiddie, 64, "LanternRiddie");
REGISTER_MEMPOOL(gamemempool, RoleWhereIsTheFairy, 64, "RoleWhereIsTheFairy");
REGISTER_MEMPOOL(gamemempool, RoleTianGongXunBao, 64, "RoleTianGongXunBao");
REGISTER_MEMPOOL(gamemempool, LoversMarket, 64, "LoversMarket");
REGISTER_MEMPOOL(gamemempool, TaoQuanNaJi, 64, "TaoQuanNaJi");
REGISTER_MEMPOOL(gamemempool, ConcentricKnot, 64, "ConcentricKnot");
REGISTER_MEMPOOL(gamemempool, RoleHundredGodBoss, 64, "RoleHundredGodBoss");
REGISTER_MEMPOOL(gamemempool, ZaoHuaEquipManager, 64, "ZaoHuaEquipManager");
REGISTER_MEMPOOL(gamemempool, RoleHongMengTianJie, 64, "RoleHongMengTianJie");
REGISTER_MEMPOOL(gamemempool, HolyEquipManager, 64, "HolyEquipManager");
REGISTER_MEMPOOL(gamemempool, KeJuExam, 64, "KeJuExam");
REGISTER_MEMPOOL(gamemempool, ZhouQiShop, 64, "ZhouQiShop");
REGISTER_MEMPOOL(gamemempool, ElementElf, 64, "ElementElf");
REGISTER_MEMPOOL(gamemempool, FuYuManager, 64, "FuYuManager");
REGISTER_MEMPOOL(gamemempool, RoleKuaFuYingXiongLu, 64, "RoleKuaFuYingXiongLu");
REGISTER_MEMPOOL(gamemempool, StarChart, 64, "StarChart");
REGISTER_MEMPOOL(gamemempool, NewCourageChallenge, 64, "NewCourageChallenge");
REGISTER_MEMPOOL(gamemempool, FashionCollection, 64, "FashionCollection");
REGISTER_MEMPOOL(gamemempool, RoleGuildTask, 64, "RoleGuildTask");
REGISTER_MEMPOOL(gamemempool, RoleHuanShou, 64, "RoleHuanShou");

UNSTD_STATIC_CHECK(135 == RoleModuleManager::ROLE_MODULE_TYPE_MAX);
//item
#include "item/itembase.h"
REGISTER_MEMPOOL(gamemempool, NetValueItemParam, 1024, "NetValueItemParam");

#include "scene/changescenedelayer.h"
REGISTER_MEMPOOL(gamemempool, ChangeSceneDelayer, 64, "ChangeSceneDelayer");

// battle business local impl
#include "battle/battle_business_local_impl/bbl_impl_common.hpp"
#include "battle/battle_business_local_impl/bbl_impl_field_boss.hpp"
#include "battle/battle_business_local_impl/bbl_impl_world_boss.hpp"
#include "battle/battle_business_local_impl/bbl_impl_world_boss2.hpp"
#include "battle/battle_business_local_impl/bbl_impl_world_boss3.hpp"
#include "battle/battle_business_local_impl/bbl_impl_platform_fight.hpp"
#include "battle/battle_business_local_impl/bbl_impl_colosseum.hpp"
#include "battle/battle_business_local_impl/bbl_impl_chief_election.hpp"
#include "battle/battle_business_local_impl/bbl_impl_chief_peak_battle.hpp"
#include "battle/battle_business_local_impl/bbl_impl_huan_jie_zhan_chang_tian_ti.hpp"
#include "battle/battle_business_local_impl/bbl_impl_cloud_arena.hpp"

REGISTER_MEMPOOL(gamemempool, BBLImplCommon, 64, "BBLImplCommon");
REGISTER_MEMPOOL(gamemempool, BBLImplFieldBoss, 16, "BBLImplFieldBoss");
REGISTER_MEMPOOL(gamemempool, BBLImplWorldBoss, 64, "BBLImplWorldBoss");
REGISTER_MEMPOOL(gamemempool, BBLImplWorldBoss2, 64, "BBLImplWorldBoss2");
REGISTER_MEMPOOL(gamemempool, BBLImplWorldBoss3, 64, "BBLImplWorldBoss3");
REGISTER_MEMPOOL(gamemempool, BBLImplPlatformFight, 64, "BBLImplPlatformFight");
REGISTER_MEMPOOL(gamemempool, BBLImplColosseum, 64, "BBLImplColosseum");
REGISTER_MEMPOOL(gamemempool, BBLImplChiefElection, 64, "BBLImplChiefElection");
REGISTER_MEMPOOL(gamemempool, BBLImplChiefPeakBattle, 64, "BBLImplChiefPeakBattle");
REGISTER_MEMPOOL(gamemempool, BBLImplHuanJieZhanChang, 64, "BBLImplHuanJieZhanChang");
REGISTER_MEMPOOL(gamemempool, BBLImplCloudArena, 64, "BBLImplCloudArena");

//character
#include "obj/character/role.h"
#include "obj/character/npc_monster.h"
#include "obj/character/npc_gather.h"
#include "obj/otherobj/specialtransport.h"
#include "obj/otherobj/treasurechest.h"
#include "obj/character/npc_chosed.h"
#include "obj/character/robot.h"
#include "obj/otherobj/xunyoucar.hpp"

REGISTER_MEMPOOL(gamemempool, Role, 64, "Role");
REGISTER_MEMPOOL(gamemempool, NPCMonster, 64, "NPCMonster");
REGISTER_MEMPOOL(gamemempool, NPCGather, 64, "NPCGather");
REGISTER_MEMPOOL(gamemempool, SpecialTransport, 64, "SpecialTransport");
REGISTER_MEMPOOL(gamemempool, TreasureChest, 64, "TreasureChest");
REGISTER_MEMPOOL(gamemempool, NPCChosed, 64, "NPCChosed");
REGISTER_MEMPOOL(gamemempool, Robot, 64, "Robot");
REGISTER_MEMPOOL(gamemempool, XunYouCar, 16, "XunYouCar");

#include "chat/msgboarditem.h"
REGISTER_MEMPOOL(gamemempool, MsgBoardItem, 64, "MsgBoardItem");

#include "global/trademarket/usersaleitem.hpp"
#include "global/trademarket/usertradeinfo.hpp"
#include "global/trademarket/systemsaleitem.hpp"
#include "global/trademarket/systemtradeinfo.hpp"
#include "global/trademarket/saleitembase.hpp"
#include "global/trademarket/systemrecyclerule.hpp"
#include "global/trademarket/autoupshelveinfo.hpp"
#include "global/trademarket/usertraderecord.hpp"
REGISTER_MEMPOOL(gamemempool, UserSaleItem, 256, "UserSaleItem");
REGISTER_MEMPOOL(gamemempool, UserTradeInfo, 64, "UserTradeInfo");
REGISTER_MEMPOOL(gamemempool, SystemSaleItem, 256, "SystemSaleItem");
REGISTER_MEMPOOL(gamemempool, SystemTradeInfo, 4, "SystemTradeInfo");
REGISTER_MEMPOOL(gamemempool, SaleItemBase, 4, "SaleItemBase");
REGISTER_MEMPOOL(gamemempool, RecycleRuleInfo, 8, "RecycleRuleInfo");
REGISTER_MEMPOOL(gamemempool, AutoUpshelveInfo, 32, "AutoUpshelveInfo");
REGISTER_MEMPOOL(gamemempool, UserTradeRecord, 64, "UserTradeRecord");

#include "servercommon/userprotocal/msgbattle.h"
using namespace Protocol;
REGISTER_MEMPOOL(gamemempool, SCBattleInfo, 16, "SCBattleInfo");
REGISTER_MEMPOOL(gamemempool, SCBattleRoundResult, 64, "SCBattleRoundResult");

#include "other/roleactivity/impl/roleactivitytimelimitstore.hpp"
#include "other/roleactivity/impl/roleactivitytruntable.hpp"
#include "other/roleactivity/impl/roleactivitydiscountstore.hpp"
#include "other/roleactivity/impl/roleactivityconsumecarousel.hpp"
#include "other/roleactivity/impl/roleactivityexpelmonster.hpp"
#include "other/roleactivity/impl/roleactivityheritagetreasure.hpp"
#include "other/roleactivity/impl/roleactivitysevencolorpicture.hpp"
#include "other/roleactivity/impl/roleactivitysaleshop.hpp"
#include "other/roleactivity/impl/roleactivityhappyup.hpp"
#include "other/roleactivity/impl/roleactivityanimalrace.hpp"
#include "other/roleactivity/impl/roleactivityluckybag.hpp"
#include "other/roleactivity/impl/roleactivityreducetaxrate.hpp"
#include "other/roleactivity/impl/roleactivitytianjianghaoli.hpp"
#include "other/roleactivity/impl/roleactivitysealpromote.hpp"
#include "other/roleactivity/impl/roleactivitydropdouble.hpp"
#include "other/roleactivity/impl/roleactivityjoyseeksfavors.hpp"
#include "other/roleactivity/impl/roleactivitycollectword.hpp"
#include "other/roleactivity/impl/roleactivitydiscountcarnival.hpp"
#include "other/roleactivity/impl/roleactivitypartnerexchange.hpp"
#include "other/roleactivity/impl/roleactivityluckydiscount.hpp"
#include "other/roleactivity/impl/roleactivitypetexchange.hpp"
#include "other/roleactivity/impl/roleactivityjiandanqinxin.hpp"
#include "other/roleactivity/impl/roleactivitytujiantreasure.hpp"
#include "other/roleactivity/impl/roleactivityadventuregift.hpp"
#include "other/roleactivity/impl/roleactivitylinkagegift.hpp"
#include "other/roleactivity/impl/roleactivityluckyturntable.hpp"
#include "other/roleactivity/impl/roleactivitytianmingdivination.hpp"
#include "other/roleactivity/impl/roleactivityquanmindiscount.hpp"
#include "other/roleactivity/impl/roleactivitysports.hpp"
#include "other/roleactivity/impl/roleactivitytujiantreasure.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/roleactivity/impl/roleactivityelementcards.hpp"
#include "other/roleactivity/impl/roleactivitysignincontinuity.hpp"
#include "other/roleactivity/impl/roleactivitycaishenjubao.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang2.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang3.hpp"
#include "other/roleactivity/impl/roleactivityzhuanshulibao.hpp"
#include "other/roleactivity/impl/roleactivityjingyanbiaosheng.hpp"
#include "other/roleactivity/impl/roleactivityyuanqiyanhuo.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe2.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe3.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe4.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaodai.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaodai2.hpp"
#include "other/roleactivity/impl/roleactivitygivepetdraw.hpp"
#include "other/roleactivity/impl/roleactivitycijiuyingxin.hpp"
#include "other/roleactivity/impl/roleactivityniuxingdayun.hpp"
#include "other/roleactivity/impl/roleactivityspringfestivalstore.hpp"
#include "other/roleactivity/impl/roleactivitymonpolygift.hpp"
#include "other/roleactivity/impl/roleactivitycowexchange.hpp"
#include "other/roleactivity/impl/roleactivitycowstore.hpp"
#include "other/roleactivity/impl/roleactivitycowluckyexchange.hpp"
#include "other/roleactivity/impl/roleactivitylittleyeargift.hpp"
#include "other/roleactivity/impl/roleactivitygivesecretkey.hpp"
#include "other/roleactivity/impl/roleactivitycumulativerecharge.hpp"
#include "other/roleactivity/impl/roleactivitypetloot.hpp"
#include "other/roleactivity/impl/roleactivitytrademarketreturngold.hpp"
#include "other/roleactivity/impl/roleactivityshenshoujianglin.hpp"
#include "other/roleactivity/impl/roleactivitygoodgiftturn.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/roleactivity/impl/roleactivitysecrettreasure.hpp"
#include "other/roleactivity/impl/roleactivityhelpothers.hpp"
#include "other/roleactivity/impl/roleactivityhelpothers2.hpp"
#include "other/roleactivity/impl/roleactivitymijingqiwen.hpp"
#include "other/roleactivity/impl/roleactivitymijingqiwen2.hpp"
#include "other/roleactivity/impl/roleactivitychunridengfeng.hpp"
#include "other/roleactivity/impl/roleactivitylaodonghaoli.hpp"
#include "other/roleactivity/impl/roleactivityzuheheli.hpp"
#include "other/roleactivity/impl/roleactivitylingpaitianci.hpp"
#include "other/roleactivity/impl/roleactivityfashionshop.hpp"
#include "other/roleactivity/impl/roleactivityoneyuangou.hpp"
#include "other/roleactivity/impl/roleactivitylingyujijin.hpp"
#include "other/roleactivity/impl/roleactivitylingpaitianci2.hpp"
#include "other/roleactivity/impl/roleactivitysummerlilian.hpp"
#include "other/roleactivity/impl/roleactivitycolorfullantern.hpp"
#include "other/roleactivity/impl/roleactivitysummerstore.hpp"
#include "other/roleactivity/impl/roleactivityluckysign.hpp"
#include "other/roleactivity/impl/roleactivityqionglouyuyan.hpp"
#include "other/roleactivity/impl/roleactivitysuperoffshop.hpp"
#include "other/roleactivity/impl/roleactivityyuanqiturning.hpp"
#include "other/roleactivity/impl/roleactivitycoolbreeze.hpp"
#include "other/roleactivity/impl/roleactivitylotusmibao.hpp"
#include "other/roleactivity/impl/roleactivitymazesummer.hpp"
#include "other/roleactivity/impl/roleactivitymibaofangdian.hpp"
#include "other/roleactivity/impl/roleactivityzhenpinshangpu.hpp"
#include "other/roleactivity/impl/roleactivitypaimaihang.hpp"
#include "other/roleactivity/impl/roleactivitylingpaitianci3.hpp"
#include "other/roleactivity/impl/roleactivitybackreward.hpp"
#include "other/roleactivity/impl/roleactivityservercompetition.hpp"
#include "other/roleactivity/impl/roleactivityhappylook.hpp"
#include "other/roleactivity/impl/roleactivitysignhappy.hpp"
#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/roleactivity/impl/roleactivityhorcruxtiancheng.hpp"
#include "other/roleactivity/impl/roleactivityintegraldraw.hpp"
#include "other/roleactivity/impl/roleactivityshanhaiquestion.hpp"
#include "other/roleactivity/impl/roleactivityshanhaijubao.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimeet.hpp"
#include "other/roleactivity/impl/roleactivityfateentanglement.hpp"
#include "other/roleactivity/impl/roleactivityzhaocaijinbao.hpp"
#include "other/roleactivity/impl/roleactivityyunzeterritory.hpp"
#include "other/roleactivity/impl/roleactivityyunzebloomingrecord.hpp"
#include "other/roleactivity/impl/roleactivityzaizhanyunze.hpp"
#include "other/roleactivity/impl/roleactivityyunzeslotmachine.hpp"
#include "other/roleactivity/impl/roleactivitymozuzainali.hpp"
#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"
#include "other/roleactivity/impl/roleactivityyunzeersanshi.hpp"
#include "other/roleactivity/impl/roleactivityshanhaizhengfeng.hpp"
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimibao.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaoxiang.hpp"
#include "other/roleactivity/impl/roleactivityshanhaihuadeng.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaohe.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaodai.hpp"
#include "other/roleactivity/impl/roleactivityjixinggaozhao.hpp"
#include "other/roleactivity/impl/roleactivityyunzewish.hpp"
#include "other/roleactivity/impl/roleactivityyunzehaoli.hpp"
#include "other/roleactivity/impl/roleactivityyunzecutepet.hpp"
#include "other/roleactivity/impl/roleactivityyunzecatchelves.hpp"
#include "other/roleactivity/impl/roleactivityxianshileichong.hpp"
#include "other/roleactivity/impl/roleactweekendgift.hpp"
#include "other/roleactivity/impl/roleactchaozhigift.hpp"
#include "other/roleactivity/impl/roleactivitytigerauspicioussnow.hpp"
#include "other/roleactivity/impl/roleactivitytigerfisheveryyear.hpp"
#include "other/roleactivity/impl/roleactivityyinhuwenshi.hpp"
#include "other/roleactivity/impl/roleactivitytigerfindspring.hpp"
#include "other/roleactivity/impl/roleactivitytigerteasenewyear.hpp"
#include "other/roleactivity/impl/roleactivityciniuyingxinhu.hpp"
#include "other/roleactivity/impl/roleactivitytigerluckymoney.hpp"
#include "other/roleactivity/impl/roleactivityhuxingdayun.hpp"
#include "other/roleactivity/impl/roleactivityyinhubaisui.hpp"
#include "other/roleactivity/impl/roleactivityjingjiyueka.hpp"
#include "other/roleactivity/impl/roleactivityronglianyouli.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanshizhuang.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanleichong.hpp"

#include "other/roleactivity/impl/roleactivitygodbeastadvent.hpp"
#include "other/roleactivity/impl/roleactivityyuanqispeed.hpp"
#include "other/roleactivity/impl/roleactivityyuanqihelp.hpp"
#include "other/roleactivity/impl/roleactivityqiyuankongmingdeng.hpp"
#include "other/roleactivity/impl/roleactivityxunmenghudie.hpp"
#include "other/roleactivity/impl/roleactivitybozhongyouli.hpp"
#include "other/roleactivity/impl/roleactivityyuanqifanpai.hpp"
#include "other/roleactivity/impl/roleactivitywanlingzhuli.hpp"
#include "other/roleactivity/impl/roleactivitybaozilaile.hpp"
#include "other/roleactivity/impl/roleactivityjinyitianxiang.hpp"
#include "other/roleactivity/impl/roleactivitymiaobishenghua.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"
#include "other/roleactivity/impl/roleactivitychengfengpolang.hpp"
#include "other/roleactivity/impl/roleactivityzongxiangmancheng.hpp"
#include "other/roleactivity/impl/roleactivitysaltysweetbattle.hpp"
#include "other/roleactivity/impl/roleactivityluckyblindbox.hpp"
#include "other/roleactivity/impl/roleactivityduanyangxiangnang.hpp"
#include "other/roleactivity/impl/roleactivityduanyangzixuan.hpp"
#include "other/roleactivity/impl/roleactivityduanyangfuli.hpp"
#include "other/roleactivity/impl/roleactivitycanglongcifu.hpp"
#include "other/roleactivity/impl/roleactivitylantangchumu.hpp"
#include "other/roleactivity/impl/roleactivitysanbaichou.hpp"
#include "other/roleactivity/impl/roleactivityjinlongjubao.hpp"
#include "other/roleactivity/impl/roleactivitychongjifanli.hpp"
#include "other/roleactivity/impl/roleactivityshenshouenze.hpp"
#include "other/roleactivity/impl/roleactivityxinfuqianghua.hpp"
#include "other/roleactivity/impl/roleactivityqixitehui.hpp"
#include "other/roleactivity/impl/roleactivitycharismatictanabata.hpp"
#include "other/roleactivity/impl/roleactivityxinfulingchong.hpp"
#include "other/roleactivity/impl/roleactivityhunqifanli.hpp"
#include "other/roleactivity/impl/roleactivityshencihuafu.hpp"
#include "other/roleactivity/impl/roleactivityzhuiyueshangdian.hpp"
#include "other/roleactivity/impl/roleactivitytianyiyaoshi.hpp"
#include "other/roleactivity/impl/roleactivityyuexidengmi.hpp"
#include "other/roleactivity/impl/roleactivityyuexileichong.hpp"
#include "other/roleactivity/impl/roleactivitydanbifanli.hpp"

REGISTER_MEMPOOL(gamemempool, RoleActivityTimeLimitStore, 64, "RoleActivityTimeLimitStore");
REGISTER_MEMPOOL(gamemempool, RoleActivityTrunTable, 64, "RoleActivityTrunTable");
REGISTER_MEMPOOL(gamemempool, RoleActivityDiscountStore, 64, "RoleActivityDiscountStore");
REGISTER_MEMPOOL(gamemempool, RoleActivityConsumeCarousel, 64, "RoleActivityConsumeCarousel");
REGISTER_MEMPOOL(gamemempool, RoleActivityExpelMonster, 64, "RoleActivityExpelMonster");
REGISTER_MEMPOOL(gamemempool, RoleActivityHeritageTreasure, 64, "RoleActivityHeritageTreasure");
REGISTER_MEMPOOL(gamemempool, RoleActivitySevenColorPicture, 64, "RoleActivitySevenColorPicture");
REGISTER_MEMPOOL(gamemempool, RoleActivitySaleShop, 64, "RoleActivitySaleShop");
REGISTER_MEMPOOL(gamemempool, RoleActivityAnimalRace, 64, "RoleActivityAnimalRace");
REGISTER_MEMPOOL(gamemempool, RoleActivityLuckyBag, 64, "RoleActivityLuckyBag");
REGISTER_MEMPOOL(gamemempool, RoleActivityReduceTaxRate, 64, "RoleActivityReduceTaxRate");
REGISTER_MEMPOOL(gamemempool, RoleActivityTianJiangHaoLi, 64, "RoleActivityTianJiangHaoLi");
REGISTER_MEMPOOL(gamemempool, RoleActivitySealPromote, 64, "RoleActivitySealPromote");
REGISTER_MEMPOOL(gamemempool, RoleActivityDropDouble, 64, "RoleActivityDropDouble");
REGISTER_MEMPOOL(gamemempool, RoleActivityJoySeeksFavors, 64, "RoleActivityJoySeeksFavors");
REGISTER_MEMPOOL(gamemempool, RoleActivityCollectWord, 64, "RoleActivityCollectWord");
REGISTER_MEMPOOL(gamemempool, RoleActivityDiscountCarnival, 64, "RoleActivityDiscountCarnival");
REGISTER_MEMPOOL(gamemempool, RoleActivityPartnerExchange, 64, "RoleActivityPartnerExchange");
REGISTER_MEMPOOL(gamemempool, RoleActivityLuckyDiscount, 64, "RoleActivityLuckyDiscount");
REGISTER_MEMPOOL(gamemempool, RoleActivityPetExchange, 64, "RoleActivityPetExchange");
REGISTER_MEMPOOL(gamemempool, RoleActivityJianDanQinXin, 64, "RoleActivityJianDanQinXin");
REGISTER_MEMPOOL(gamemempool, RoleActivityAdventureGift, 64, "RoleActivityAdventureGift");
REGISTER_MEMPOOL(gamemempool, RoleActivityLinkageGift, 64, "RoleActivityLinkageGift");
REGISTER_MEMPOOL(gamemempool, RoleActivityLuckyTurnTable, 64, "RoleActivityLuckyTurnTable");
REGISTER_MEMPOOL(gamemempool, RoleActivityTianMingDivination, 64, "RoleActivityTianMingDivination");
REGISTER_MEMPOOL(gamemempool, RoleActivityQuanMinDiscount, 64, "RoleActivityQuanMinDiscount");
REGISTER_MEMPOOL(gamemempool, RoleActivitySports, 64, "RoleActivitySports");
REGISTER_MEMPOOL(gamemempool, RoleActivityTuJianTreasure, 64, "RoleActivityTuJianTreasure");
REGISTER_MEMPOOL(gamemempool, RoleActivityGodToken, 64, "RoleActivityGodToken");
REGISTER_MEMPOOL(gamemempool, RoleActivityElementCards, 64, "RoleActivityElementCards");
REGISTER_MEMPOOL(gamemempool, RoleActivitySignInContinuity, 64, "RoleActivitySignInContinuity");
REGISTER_MEMPOOL(gamemempool, RoleActivityCaiShenJuBao, 64, "RoleActivityCaiShenJuBao");
REGISTER_MEMPOOL(gamemempool, RoleActivityDingZhiBaoXiang, 64, "RoleActivityDingZhiBaoXiang");
REGISTER_MEMPOOL(gamemempool, RoleActivityDingZhiBaoXiang2, 64, "RoleActivityDingZhiBaoXiang2");
REGISTER_MEMPOOL(gamemempool, RoleActivityDingZhiBaoXiang3, 64, "RoleActivityDingZhiBaoXiang3");
REGISTER_MEMPOOL(gamemempool, RoleActivityZhuanShuLiBao, 64, "RoleActivityZhuanShuLiBao");
REGISTER_MEMPOOL(gamemempool, RoleActivityJingYanBiaoSheng, 64, "RoleActivityJingYanBiaoSheng");
REGISTER_MEMPOOL(gamemempool, RoleActivityXianShiBaoHe, 64, "RoleActivityXianShiBaoHe");
REGISTER_MEMPOOL(gamemempool, RoleActivityXianShiBaoHe2, 64, "RoleActivityXianShiBaoHe2");
REGISTER_MEMPOOL(gamemempool, RoleActivityXianShiBaoHe3, 64, "RoleActivityXianShiBaoHe3");
REGISTER_MEMPOOL(gamemempool, RoleActivityXianShiBaoHe4, 64, "RoleActivityXianShiBaoHe4");
REGISTER_MEMPOOL(gamemempool, RoleActivityXianShiBaoDai, 64, "RoleActivityXianShiBaoDai");
REGISTER_MEMPOOL(gamemempool, RoleActivityXianShiBaoDai2, 64, "RoleActivityXianShiBaoDai2");
REGISTER_MEMPOOL(gamemempool, RoleActivityGivePetDraw, 64, "RoleActivityGivePetDraw");
REGISTER_MEMPOOL(gamemempool, RoleActivityCiJiuYingXin, 64, "RoleActivityCiJiuYingXin");
REGISTER_MEMPOOL(gamemempool, RoleActivityNiuXingDaYun, 64, "RoleActivityNiuXingDaYun");
REGISTER_MEMPOOL(gamemempool, RoleActivitySpringFestivalStore, 64, "RoleActivitySpringFestivalStore");
REGISTER_MEMPOOL(gamemempool, RoleActivityMonpolyGift, 64, "RoleActivityMonpolyGift");
REGISTER_MEMPOOL(gamemempool, RoleActivityCowExchange, 64, "RoleActivityCowExchange");
REGISTER_MEMPOOL(gamemempool, RoleActivityCowStore, 64, "RoleActivityCowStore");
REGISTER_MEMPOOL(gamemempool, RoleActivityCowLuckyExchange, 64, "RoleActivityCowLuckyExchange");
REGISTER_MEMPOOL(gamemempool, RoleActivityLittleYearGift, 64, "RoleActivityLittleYearGift");
REGISTER_MEMPOOL(gamemempool, RoleActivityGiveSecretKey, 64, "RoleActivityGiveSecretKey");
REGISTER_MEMPOOL(gamemempool, RoleActivityCumulativeRecharge, 64, "RoleActivityCumulativeRecharge");
REGISTER_MEMPOOL(gamemempool, RoleActivityPetLoot, 64, "RoleActivityPetLoot");
REGISTER_MEMPOOL(gamemempool, RoleActivityTradeMarketReturnGold, 64, "RoleActivityTradeMarketReturnGold");
REGISTER_MEMPOOL(gamemempool, RoleActivityShenShouJiangLin, 64, "RoleActivityShenShouJiangLin");
REGISTER_MEMPOOL(gamemempool, RoleActivityGoodGiftTurn, 64, "RoleActivityGoodGiftTurn");
REGISTER_MEMPOOL(gamemempool, RoleActivitySecretTreasure, 64, "RoleActivitySecretTreasure");
REGISTER_MEMPOOL(gamemempool, RoleActivityHelpOthers, 64, "RoleActivityHelpOthers");
REGISTER_MEMPOOL(gamemempool, RoleActivityHelpOthers2, 64, "RoleActivityHelpOthers2");
REGISTER_MEMPOOL(gamemempool, RoleActivityFortuneMaze, 64, "RoleActivityFortuneMaze");
REGISTER_MEMPOOL(gamemempool, RoleActivityMiJingQiWen, 64, "RoleActivityMiJingQiWen");
REGISTER_MEMPOOL(gamemempool, RoleActivityMiJingQiWen2, 64, "RoleActivityMiJingQiWen2");
REGISTER_MEMPOOL(gamemempool, RoleActivityChunRiDengFeng, 64, "RoleActivityChunRiDengFeng");
REGISTER_MEMPOOL(gamemempool, RoleActivityLaoDongHaoLi, 64, "RoleActivityChunRiDengFeng");
REGISTER_MEMPOOL(gamemempool, RoleActivityZuHeHeLi, 64, "RoleActivityZuHeHeLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityLingPaiTianCi, 64, "RoleActivityLingPaiTianCi");
REGISTER_MEMPOOL(gamemempool, RoleActivityFashionShop, 64, "RoleActivityFashionShop");
REGISTER_MEMPOOL(gamemempool, RoleActivityOneYuanGou, 64, "RoleActivityOneYuanGou");
REGISTER_MEMPOOL(gamemempool, RoleActivityLingYuJiJin, 64, "RoleActivityLingYuJiJin");
REGISTER_MEMPOOL(gamemempool, RoleActivityLingPaiTianCi2, 64, "RoleActivityLingPaiTianCi2");
REGISTER_MEMPOOL(gamemempool, RoleActivitySummerLiLian, 64, "RoleActivitySummerLiLian");
REGISTER_MEMPOOL(gamemempool, RoleActivityColorfulLantern, 64, "RoleActivityColorfulLantern");
REGISTER_MEMPOOL(gamemempool, RoleActivitySummerStore, 64, "RoleActivitySummerStore");
REGISTER_MEMPOOL(gamemempool, RoleActivityLuckySign, 64, "RoleActivityLuckySign");
REGISTER_MEMPOOL(gamemempool, RoleActivityQiongLouYuYan, 64, "RoleActivityQiongLouYuYan");
REGISTER_MEMPOOL(gamemempool, RoleActivitySuperOffShop, 64, "RoleActivitySuperOffShop");
REGISTER_MEMPOOL(gamemempool, RoleActivityYuanQiTurning, 64, "RoleActivityYuanQiTurning");
REGISTER_MEMPOOL(gamemempool, RoleActivityCoolBreeze, 64, "RoleActivityCoolBreeze");
REGISTER_MEMPOOL(gamemempool, RoleActivityLotusMiBao, 64, "RoleActivityLotusMiBao");
REGISTER_MEMPOOL(gamemempool, RoleActivityMazeSummer, 64, "RoleActivityMazeSummer");
REGISTER_MEMPOOL(gamemempool, RoleActivityMiBaoFangDian, 64, "RoleActivityMiBaoFangDian");
REGISTER_MEMPOOL(gamemempool, RoleActivityZhenPinShangPu, 64, "RoleActivityZhenPinShangPu");
REGISTER_MEMPOOL(gamemempool, RoleActivityPaiMaiHang, 64, "RoleActivityPaiMaiHang");
REGISTER_MEMPOOL(gamemempool, RoleActivityLingPaiTianCi3, 64, "RoleActivityLingPaiTianCi3");
REGISTER_MEMPOOL(gamemempool, RoleActivityBackReward, 64, "RoleActivityBackReward");
REGISTER_MEMPOOL(gamemempool, RoleActivityServerCompetition, 64, "RoleActivityServerCompetition");
REGISTER_MEMPOOL(gamemempool, RoleActivityHappyLook, 64, "RoleActivityHappyLook");
REGISTER_MEMPOOL(gamemempool, RoleActivitySignHappy, 64, "RoleActivitySignHappy");
REGISTER_MEMPOOL(gamemempool, RoleActivityWanLingGongYing, 64, "RoleActivityWanLingGongYing");
REGISTER_MEMPOOL(gamemempool, RoleActivityHorcruxTianCheng, 64, "RoleActivityHorcruxTianCheng");
REGISTER_MEMPOOL(gamemempool, RoleActivityIntegralDraw, 64, "RoleActivityIntegralDraw");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiQuestion, 64, "RoleActivityShanHaiQuestion");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiJuBao, 64, "RoleActivityShanHaiJuBao");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiMet, 64, "RoleActivityShanHaiMet");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiMeet, 64, "RoleActivityShanHaiMeet");
REGISTER_MEMPOOL(gamemempool, RoleActivityFateEntanglement, 64, "RoleActivityFateEntanglement");
REGISTER_MEMPOOL(gamemempool, RoleActivityZhaoCaiJinBao, 64, "RoleActivityZhaoCaiJinBao");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeTerritory, 64, "RoleActivityYunZeTerritory");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeBloomingRecord, 64, "RoleActivityYunZeBloomingRecord");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeSlotMachine, 64, "RoleActivityYunZeSlotMachine");
REGISTER_MEMPOOL(gamemempool, RoleActivityZaiZhanYunZe, 64, "RoleActivityZaiZhanYunZe");
REGISTER_MEMPOOL(gamemempool, RoleActivityMoZuZaiNaLi, 64, "RoleActivityMoZuZaiNaLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityChaoZhiFenXiang, 64, "RoleActivityChaoZhiFenXiang");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeErSanShi, 64, "RoleActivityYunZeErSanShi");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiZhengFeng, 64, "RoleActivityShanHaiZhengFeng");
REGISTER_MEMPOOL(gamemempool, RoleActivityColorfulDouble, 64, "RoleActivityColorfulDouble");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiMiBao, 64, "RoleActivityShanHaiMiBao");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiHuaDeng, 64, "RoleActivityShanHaiHuaDeng");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiBaoXiang, 64, "RoleActivityShanHaiBaoXiang");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiBaoHe, 64, "RoleActivityShanHaiBaoHe");
REGISTER_MEMPOOL(gamemempool, RoleActivityShanHaiBaoDai, 64, "RoleActivityShanHaiBaoDai");
REGISTER_MEMPOOL(gamemempool, RoleActivityJiXingGaoZhao, 64, "RoleActivityJiXingGaoZhao");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeWish, 64, "RoleActivityYunZeWish");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeHaoLi, 64, "RoleActivityYunZeHaoLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeCutePet, 64, "RoleActivityYunZeCutePet");
REGISTER_MEMPOOL(gamemempool, RoleActivityYunZeCatchElves, 64, "RoleActivityYunZeCatchElves");
REGISTER_MEMPOOL(gamemempool, RoleActivityXianShiLeiChong, 64, "RoleActivityXianShiLeiChong");
REGISTER_MEMPOOL(gamemempool, RoleACTWeekendGift, 64, "RoleACTWeekendGift");
REGISTER_MEMPOOL(gamemempool, RoleACTChaoZhiGift, 64, "RoleACTChaoZhiGift");
REGISTER_MEMPOOL(gamemempool, RoleActivityTigerFishEveryYear, 64, "RoleActivityTigerFishEveryYear");
REGISTER_MEMPOOL(gamemempool, RoleActivityTigerFindSpring, 64, "RoleActivityTigerFindSpring");
REGISTER_MEMPOOL(gamemempool, RoleActivityTigerTeaseNewYear, 64, "RoleActivityTigerTeaseNewYear");
REGISTER_MEMPOOL(gamemempool, RoleActivityTigerLuckyMoney, 64, "RoleActivityTigerLuckyMoney");
REGISTER_MEMPOOL(gamemempool, RoleActivityYinHuWenShi, 64, "RoleActivityYinHuWenShi");
REGISTER_MEMPOOL(gamemempool, RoleActivityCiNiuYingXinHu, 64, "RoleActivityCiNiuYingXinHu");
REGISTER_MEMPOOL(gamemempool, RoleActivityHuXingDaYun, 64, "RoleActivityHuXingDaYun");
REGISTER_MEMPOOL(gamemempool, RoleActivityYinHuBaiSui, 64, "RoleActivityYinHuBaiSui");
REGISTER_MEMPOOL(gamemempool, RoleActivityTigerAuspiciousSnow, 64, "RoleActivityTigerAuspiciousSnow");
REGISTER_MEMPOOL(gamemempool, RoleActivityJingJiYueKa, 64, "RoleActivityJingJiYueKa");
REGISTER_MEMPOOL(gamemempool, RoleActivityRongLianYouLi, 64, "RoleActivityRongLianYouLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityQingYuanShiZhuang, 64, "RoleActivityQingYuanShiZhuang");
REGISTER_MEMPOOL(gamemempool, RoleActivityQingYuanLeiChong, 64, "RoleActivityQingYuanLeiChong");
REGISTER_MEMPOOL(gamemempool, RoleActivityGodBeastAdvent, 64, "RoleActivityGodBeastAdvent");
REGISTER_MEMPOOL(gamemempool, RoleActivityYuanQiSpeed, 64, "RoleActivityYuanQiSpeed");
REGISTER_MEMPOOL(gamemempool, RoleActivityYuanQiHelp, 64, "RoleActivityYuanQiHelp");
REGISTER_MEMPOOL(gamemempool, RoleActivityQiYuanKongMingDeng, 64, "RoleActivityQiYuanKongMingDeng");
REGISTER_MEMPOOL(gamemempool, RoleActivityXunMengHuDie, 64, "RoleActivityXunMengHuDie");
REGISTER_MEMPOOL(gamemempool, RoleActivityBoZhongYouLi, 64, "RoleActivityBoZhongYouLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityYuanQiFanPai, 64, "RoleActivityYuanQiFanPai");
REGISTER_MEMPOOL(gamemempool, RoleActivityWanLingZhuLi, 64, "RoleActivityWanLingZhuLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityBaoZiLaiLe, 64, "RoleActivityBaoZiLaiLe");
REGISTER_MEMPOOL(gamemempool, RoleActivityJinYiTianXiang, 64, "RoleActivityJinYiTianXiang");
REGISTER_MEMPOOL(gamemempool, RoleActivityMiaoBiShengHua, 64, "RoleActivityMiaoBiShengHua");
REGISTER_MEMPOOL(gamemempool, RoleActivityYuanQiYanHuo, 64, "RoleActivityYuanQiYanHuo");
REGISTER_MEMPOOL(gamemempool, RoleActivityYuanQiJiShiBu, 64, "RoleActivityYuanQiYanHuo");
REGISTER_MEMPOOL(gamemempool, RoleActivityChengFengPoLang, 64, "RoleActivityChengFengPoLang");
REGISTER_MEMPOOL(gamemempool, RoleActivityZongXiangManCheng, 64, "RoleActivityZongXiangManCheng");
REGISTER_MEMPOOL(gamemempool, RoleActivitySaltySweetBattle, 64, "RoleActivitySaltySweetBattle");
REGISTER_MEMPOOL(gamemempool, RoleActivityDuanYangXiangNang, 64, "RoleActivityDuanYangXiangNang");
REGISTER_MEMPOOL(gamemempool, RoleActivityDuanYangZiXuan, 64, "RoleActivityDuanYangZiXuan");
REGISTER_MEMPOOL(gamemempool, RoleActivityDuanYangFuLi, 64, "RoleActivityDuanYangFuLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityCangLongCiFu, 64, "RoleActivityCangLongCiFu");
REGISTER_MEMPOOL(gamemempool, RoleActivityLuckyBlindBox, 64, "RoleActivityLuckyBlindBox");
REGISTER_MEMPOOL(gamemempool, RoleActivityLanTangChuMu, 64, "RoleActivityLanTangChuMu");
REGISTER_MEMPOOL(gamemempool, RoleActivitySanBaiChou, 64, "RoleActivitySanBaiChou");
REGISTER_MEMPOOL(gamemempool, RoleActivityJinLongJuBao, 64, "RoleActivityJinLongJuBao");
REGISTER_MEMPOOL(gamemempool, RoleActivityChongJiFanLi, 64, "RoleActivityChongJiFanLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityShenShouEnZe, 64, "RoleActivityShenShouEnZe");
REGISTER_MEMPOOL(gamemempool, RoleActivityXinFuQiangHua, 64, "RoleActivityXinFuQiangHua");
REGISTER_MEMPOOL(gamemempool, RoleActivityQiXiTeHui, 64, "RoleActivityQiXiTeHui");
REGISTER_MEMPOOL(gamemempool, RoleActivityCharismaticTanabata, 64, "RoleActivityCharismaticTanabata");
REGISTER_MEMPOOL(gamemempool, RoleActivityXinFuLingChong, 64, "RoleActivityXinFuLingChong");
REGISTER_MEMPOOL(gamemempool, RoleActivityHunQiFanLi, 64, "RoleActivityHunQiFanLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityShenCiHuaFu, 64, "RoleActivityShenCiHuaFu");
REGISTER_MEMPOOL(gamemempool, RoleActivityTianYiYaoShi, 64, "RoleActivityTianYiYaoShi");
REGISTER_MEMPOOL(gamemempool, RoleActivityYueXiDengMi, 64, "RoleActivityYueXiDengMi");
REGISTER_MEMPOOL(gamemempool, RoleActivityYueXiLeiChong, 64, "RoleActivityYueXiLeiChong");
REGISTER_MEMPOOL(gamemempool, RoleActivityDanBiFanLi, 64, "RoleActivityDanBiFanLi");
REGISTER_MEMPOOL(gamemempool, RoleActivityZhuiYueShangDian, 64, "RoleActivityZhuiYueShangDian");

UNSTD_STATIC_CHECK(178 == RAND_ACTIVITY_TYPE_END - RAND_ACTIVITY_TYPE_BEGIN);

#include "servercommon/rankdef.hpp"
#include "global/rank/guildrank.hpp"

REGISTER_MEMPOOL(gamemempool, PersonRankItem, 128, "PersonRankItem");
REGISTER_MEMPOOL(gamemempool, PetRankItem, 128, "PetRankItem");
REGISTER_MEMPOOL(gamemempool, GuildRankItem, 128, "GuildRankItem");
REGISTER_MEMPOOL(gamemempool, PartnerRankItem, 128, "PartnerRankItem");

#include "obj/MoveAssist/MoveAssist.hpp"
REGISTER_MEMPOOL(gamemempool, MoveAssist, 64, "MoveAssist");

//#include "servercommon/matchmanager/matchdef.hpp"
//REGISTER_MEMPOOL(gamemempool, MatchTeam, 16, "MatchTeam");


#include "asynctaskimpl/asyncredistest.hpp"
REGISTER_MEMPOOL(gamemempool, AsyncRedisTest, 4, "AsyncRedisTest");

#include "global/bigcross/utaobfightdata.hpp"
REGISTER_MEMPOOL(gamemempool, UTAOBFightData, 32, "UTAOBFightData");

#include "timer/delayqueryuserchatrecord.hpp"
REGISTER_MEMPOOL(gamemempool, DelayQueryUserChatRecordTimer, 64, "DelayQueryUserChatRecordTimer");