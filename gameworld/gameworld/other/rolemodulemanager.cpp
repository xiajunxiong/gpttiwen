#include "rolemodulemanager.hpp"
#include "world.h"
#include "gamelog.h"
#include "internalcomm.h"
#include "engineadapter.h"
#include "rmibackobjdef.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/struct/rolefirstkillparam.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "config/otherconfig/knapsack_config.hpp"
#include "config/otherconfig/professionconfig.hpp"

#include "protocol/msgrole.h"
#include "protocol/msgteam.h"
#include "protocol/msgother.h"
#include "protocol/msgguild.hpp"
#include "protocol/msgshanhaiboss.hpp"
#include "protocol/msgcloudarena.h"
#include "protocol/msgwildboss.h"

#include "global/team/team.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmember.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/cross/crossuserregister.hpp"

#include "item/money.h"
#include "scene/scene.h"
#include "item/knapsack.h"
#include "item/itempool.h"
#include "task/taskmanager.h"
#include "task/taskrecorder.h"
#include "obj/character/role.h"
#include "scene/speciallogic.hpp"
#include "friend/friendmanager.h"
#include "friend/friendrecommend.hpp"
#include "rmiclient/rmiloginclient.h"
#include "equipment/equipmentmanager.h"

#include "other/shop/shop.h"
#include "other/vip/vip.hpp"
#include "other/pet/pet.hpp"
#include "other/baby/baby.hpp"
#include "other/title/title.hpp"
#include "other/medal/medal.hpp"
#include "other/fabao/fabao.hpp"
#include "other/train/train.hpp"
#include "other/mentor/shitu.hpp"
#include "other/pet/petconfig.hpp"
#include "other/escort/escort.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "other/salary/salary.hpp"
#include "other/currencyconfig.hpp"
#include "other/monitor/monitor.hpp"
#include "other/partner/partner.hpp"
#include "other/surface/surface.hpp"
#include "other/funopen/funopen.hpp"
#include "other/route/mailroute.hpp"
#include "other/skill/roleskill.hpp"
#include "other/marriage/marriage.hpp"
#include "other/prestige/prestige.hpp"
#include "other/roledrop/roledrop.hpp"
#include "other/workshop/workshop.hpp"
#include "other/rolemail/rolemail.hpp"
#include "other/exchange/exchange.hpp"
#include "other/event/eventhandler.hpp"
#include "other/dayreward/dayreward.hpp"
#include "other/facescore/facescore.hpp"
#include "other/trevifountionconfig.hpp"
#include "other/funcguide/funcguide.hpp"
#include "other/taskchain/taskchain.hpp"
#include "other/schooltask/schooltask.h"
#include "other/rolecross/rolecross.hpp"
#include "other/mapunlock/mapunlock.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/capability/capability.hpp"
#include "other/collection/collection.hpp"
#include "other/offlineexp/offlineexp.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/monster_wave/monsterwave.h"
#include "other/treasure_map/treasuremap.h"
#include "other/researchtask/researchtask.h"
#include "other/loginsystem/loginsystem.hpp"
#include "other/buffmanager/buffmanager.hpp"
#include "other/role_setting/rolesetting.hpp"
#include "other/rolematerial/rolematerial.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/wst_question/wst_question.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "other/systemnotice/systemnotice.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/boss_chapter/boss_chapter.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/challengefb/rolechallengefb.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/roleattribute/roleattributeplan.hpp"
#include "other/roleconvertshop/roleconvertshop.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "other/specialappearance/specialappearance.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "item/elementcrystal/elementcrystal.hpp"
#include "other/funcpreview/funcpreview.hpp"
#include "other/skill/roleskill.hpp"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "other/auto_catch_pet/auto_catch_pet.hpp"
#include "other/trade_credit/trade_credit.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "other/roleheartskill/roleheartskill.hpp"
#include "skill/skillpool.hpp"
#include "other/formation/formation.hpp"
#include "other/huanjie/rolehuanjie.hpp"
#include "other/huanshou/rolehuanshou.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/roleattrpellet/roleattrpellet.hpp"
#include "global/xunbao/xunbaoconfig.hpp"
#include "global/shanhaibossmanager/shanhaibossconfig.hpp"

#include "protocol/msgskill.h"
#include "protocol/msgbraveground.h"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "global/jieyi/jieyizumanager.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityluckybag.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/roleactivity/impl/roleactivitycaishenjubao.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang.hpp"
#include "other/roleactivity/impl/roleactivityzhuanshulibao.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaodai.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang2.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang3.hpp"
#include "other/roleactivity/impl/roleactivityjinyitianxiang.hpp"
#include "other/roleactivity/impl/roleactivityyuanqiyanhuo.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe2.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe3.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe4.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaodai2.hpp"
#include "other/roleactivity/impl/roleactivityoneyuangou.hpp"
#include "other/roleactivity/impl/roleactivitylingyujijin.hpp"
#include "other/roleactivity/impl/roleactivityshenshoujianglin.hpp"
#include "other/roleactivity/impl/roleactivityzhenpinshangpu.hpp"
#include "other/roleactivity/impl/roleactivitypetexchange.hpp"
#include "other/train/trainconfig.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/roleactivity/impl/roleactivitycowexchange.hpp"
#include "other/roleactivity/impl/roleactivityspringfestivalstore.hpp"
#include "other/roleactivity/impl/roleactivitycowstore.hpp"
#include "config/activityconfig/braveground/activitybravegroundconfig.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/schoolexercise/schoolexercise.hpp"
#include "chat/chatconfig.h"
#include "other/roleactivity/impl/roleactivitysecrettreasure.hpp"
#include "other/roleactivity/impl/roleactivityzuheheli.hpp"
#include "other/roleactivity/impl/roleactivityfashionshop.hpp"
#include "other/roleactivity/impl/roleactivitycumulativerecharge.hpp"
#include "other/roleactivity/impl/roleactivitysummerstore.hpp"
#include "other/roleactivity/impl/roleactivitysuperoffshop.hpp"
#include "other/roleactivity/impl/roleactivitycoolbreeze.hpp"
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"
#include "other/roleactivity/impl/roleactivityqixitehui.hpp"
#include "other/roleactivity/impl/roleactivityxinfulingchong.hpp"
#include "other/roleactivity/impl/roleactivityshencihuafu.hpp"
#include "other/roleactivity/impl/roleactivitydanbifanli.hpp"
#include "other/roleactivity/impl/roleactivityzhuiyueshangdian.hpp"

#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "global/cloudarena/cloudarenaconfig.hpp"
#include "other/mapexploration/mapexploration.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "other/shop/shopconfig.h"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"
#include "other/custominstruction/custominstruction.hpp"
#include "other/rolezhuling/rolezhuling.hpp"
#include "other/formation/pethelperformation.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "other/petgodfb/petgodfb.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "protocol/msghongbao.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "other/roletiandidaohen/roletiandidaohen.hpp"
#include "other/rolegivegift/rolegivegift.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "other/prestige_task/prestige_task.hpp"
#include "other/svip/svip.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "servercommon/noticenum.h"
#include "other/duel/duel.hpp"
#include "other/duel/duelconfig.hpp"
#include "equipment/accessoriesmentmanager.h"
#include "other/tianming/tianming.hpp"
#include "other/dujie/dujie.hpp"
#include "other/yinyangeye/yinyangeye.hpp"
#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/robot/robotconfig.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/roleactivity/impl/roleactivityhorcruxtiancheng.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimeet.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "other/endlesstower/endlesstower.hpp"
#include "other/roleactivity/impl/roleactivityzhaocaijinbao.hpp"
#include "global/jieyi/jieyizuconfig.h"
#include "other/roleactivity/impl/roleactivityyunzeslotmachine.hpp"
#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"
#include "other/advancecareer/advancecareer.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaoxiang.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaohe.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaodai.hpp"
#include "other/wanlingfb/wanlingfb.hpp"
#include "other/roleactivity/impl/roleactivityjixinggaozhao.hpp"
#include "other/flyupsystem/flyupststemconfig.hpp"
#include "other/mirageterritory/mirageterritory.hpp"
#include "other/roleactivity/impl/roleactweekendgift.hpp"
#include "other/roleactivity/impl/roleactchaozhigift.hpp"
#include "other/xiuwei/xiuwei.hpp"
#include "other/treasuretask/treasuretask.hpp"
#include "other/roleactivity/impl/roleactivityyinhuwenshi.hpp"
#include "other/roleactivity/impl/roleactivityyinhubaisui.hpp"
#include "other/roleactivity/impl/roleactivityjingjiyueka.hpp"
#include "other/fallengod/fallengod.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanshizhuang.hpp"
#include "other/minigame/minigame.hpp"
#include "other/peaktoken/rolepeaktoken.hpp"
#include "other/income/income.hpp"
#include "global/qingyuan/qingyuanmanager.h"
#include "global/qingyuan/qingyuan.h"
#include "other/yaoxingshilian/yaoxingshilian.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "other/levelcomplement/newlevelcomplement.hpp"
#include "other/cycleactivity/sanxianguidong/sanxianguidong.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddie.hpp"
#include "other/cycleactivity/whereisthefairy/rolewhereisthefairy.hpp"
#include "other/cycleactivity/tiangongxunbao/tiangongxunbao.hpp"
#include "other/cycleactivity/loversmarket/loversmarket.hpp"
#include "other/cycleactivity/taoquannaji/taoquannaji.hpp"
#include "other/cycleactivity/concentricknot/concentricknot.hpp"
#include "other/rolehundredgodboss/rolehundredgodboss.hpp"
#include "protocol/cycleactivity/msgtuzikuaipao.hpp"
#include "other/cycleactivity/gotoschool/gotoschoolconfig.hpp"
#include "other/roleactivity/impl/roleactivitygodbeastadvent.hpp"
#include "other/roleactivity/impl/roleactivitywanlingzhuli.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"
#include "other/roleactivity/impl/roleactivityluckyblindbox.hpp"
#include "other/roleactivity/impl/roleactivityduanyangzixuan.hpp"
#include "other/roleactivity/impl/roleactivityduanyangfuli.hpp"
#include "other/roleactivity/impl/roleactivityjinlongjubao.hpp"
#include "other/roleactivity/impl/roleactivityshenshouenze.hpp"
#include "other/zaohuaequip/zaohuaequipmanager.hpp"
#include "other/hongmengtianjie/rolehongmengtianjie.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "other/cycleactivity/kejuexam/kejuexam.hpp"
#include "protocol/cycleactivity/msggotoschool.hpp"
#include "item/shengqi/holyitem.h"
#include "other/cycleactivity/zhouqishop/zhouqishop.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "other/elementelf/elementelf.hpp"
#include "other/fuyu/fuyumanager.hpp"
#include "other/roleactivity/impl/roleactivitytianyiyaoshi.hpp"
#include "other/kuafuyingxionglu/rolekuafuyingxionglu.hpp"
#include "other/starchart/starchart.h"
#include "other/couragechallenge/newcouragechallenge.hpp"
#include "other/fashioncollection/fashioncollection.hpp"
#include "other/roleguildtask/roleguildtask.hpp"

#define NEW_ROLE_MODULE(type, classname) \
	m##classname = new classname(); \
	m_role_module_ref_counter[type] += 1;

#define DELETE_ROLE_MODULE(type, classname) \
	delete m##classname; \
	m##classname = NULL; \
	m_role_module_ref_counter[type] -= 1;

const static int MIXED_ELEMENT_CRYSTAL_ITEM_ID[] = { 40004 ,40005,40006,40007 };
bool IsMixedElementCrystalId(int item_id)
{
	for (int i = 0; i < ARRAY_LENGTH(MIXED_ELEMENT_CRYSTAL_ITEM_ID); ++i)
	{
		if (MIXED_ELEMENT_CRYSTAL_ITEM_ID[i] == item_id)
		{
			return true;
		}
	}

	return false;
}

RoleModuleManager::RoleModuleManager(Role * role) : m_start_role_cmd_id(0), m_role(NULL), m_last_search_trade_market_time(0u),  m_next_send_auto_talk_timestamp(0),
	m_patrol_flag(false), m_next_get_role_cmd_time(0)
{
	m_role_cmd_handler_map["CmdBuyItem"] = &RoleModuleManager::CmdBuyItem;
	m_role_cmd_handler_map["ChongzhiCmd"] = &RoleModuleManager::CmdChongzhi;
	m_role_cmd_handler_map["CmdChongzhiBudan"] = &RoleModuleManager::CmdChongzhiBudan;

	memset(m_role_module_ref_counter, 0, sizeof(m_role_module_ref_counter));

	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_VIP, Vip);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTRIBUTE, RoleAttributePlan);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MONITOR, Monitor);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MONEY, Money);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_KNAPSACK, Knapsack);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_PET, Pet);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_PARTNER, Partner);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE, Prestige);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_COLLECTION, Collection);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TITLE, Title);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TRAIN, Train);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TASKRECORDER, TaskRecorder);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TASKMANAGER, TaskManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_WST_QUESTION, WstQuestion);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_SKILL, RoleSkill);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_DAY_REWARD, DayReward);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_EQUIPMENT, EquipmentManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_WORKSHOP, Workshop);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SURFACE, Surface);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MARRIAGE, Marriage);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_BABY, Baby);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FRIEND_RECOMMEND, FriendRecommend);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_SETTING, RoleSetting);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SHI_TU, ShiTu);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_NOTICE, SystemNotice);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_COURAGE_CHALLENGE, CourageChallenge);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MONSTER_WAVE, MonsterWave);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVITY_MANAGER, RoleActivityManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MI_YI, MiYi);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_TASK, SchoolTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_OFFLINE_EXP, OfflineExp);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ESCORT, Escort);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_MAP, TreasureMap);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TASK_CHAIN, TaskChain);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FABAO, FaBao);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_RESEARCH_TASK, ResearchTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_BUFF_MANAGER, BuffManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MEDAL, Medal);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CRYSTAL_FB, RoleCrystalFb);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SHOP, Shop);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_UNLOCK, MapUnlock);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_FB, RoleGatherFb);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_EXCHANGE, Exchange);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLECROSS, RoleCross);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_COURSE_OF_GROWTH, CourseOfGrowth);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SALARY, Salary);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_CHALLENGE_FB, RoleChallengeFb);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FACE_SCORE, FaceScore);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_GUIDE, FuncGuide);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_DROP, RoleDrop);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CONVERT_SHOP, RoleConvertShop);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SPECIAL_APPEARANCE, SpecialAppearance);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MAIL, RoleMail);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_LOGIN_SYSTEM, LoginSystem);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_BOSS_CHAPTER, BossChapter);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CAPABILITY, Capability);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MYSTERIOUS_TRIAL, MysteriousTrial);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_BOUNTY_TASK, BountyTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TRANSACTION_TASK, TransactionTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVE_SYSTEM, ActiveSystem);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_HUASHEN_MANAGER, HuaShenManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_PREVIEW, FuncPreview);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FIRST_KILL, RoleFirstKill);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_AUTO_CATCH_PET, AutoCatchPet);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TRADE_CREDIT, TradeCredit);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FORMATION, RoleFormation);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_DREAM, RoleGatherDream);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_JIE, RoleHuanJie);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ANECDOTE_TASK, AnecdoteTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_LEVEL_COMPLEMENT, LevelComplement);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_DREAM_NOTES_CHALLENGE, DreamNotesChallenge);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_EXERCISE, SchoolExercise);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_COMMON_SAVE_MANAGER, RoleCommonSaveManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SMART_MOUNTS_SYSTEM, SmartMountsSystem);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_REALM_BREAK, RealmBreak);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_PET_MONOPOLY, PetMonopoly);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_LUN_HUI_WANG_CHUAN, LunHuiWangChuan);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MATERIAL, RoleMaterial);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_EXPLORATION, MapExploration);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_JOIN_ACTIVITY_REWARD, RoleJoinActivityReward);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_CUSTOM_INSTRUCTION, CustomInstruction);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GUILD_INBORN, RoleGuildInborn);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HEART_SKILL, RoleHeartSkill);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ZHU_LING, RoleZhuLing);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_PET_HELPER_FORMATION, PetHelperFormation);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_NETHER_WORLD_PURGATORY, NetherWorldPurgatory);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_PET_GOD_FB, PetGodFB);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_BIG_DIPPER, BigDipper);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN, RoleTianDiDaoHen);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GIVE_GIFT, RoleGiveGift);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE_TASK, PrestigeTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SVIP, SVip);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTR_PELLET, RoleAttrPellet);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_DUEL, Duel);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ACCESSORIES, AccessoriesmentManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TIAN_MING, TianMing);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_DU_JIE, DuJie);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_YIN_YANG_EYE, YinYangEye);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ENDLESS_TOWER, EndlessTower);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ADVANCE_CAREER, AdvanceCareer);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_WAN_LING_FB, RoleWanLingFB);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LIFE_SKILL, RoleNewLifeSkill);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FLY_UP_SYSTEM, FlyUpSystem);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MIRAGE_TERRIROTY, MirageTerritory);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_XIU_WEI, XiuWei);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_TASK, TreasureTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FALLEN_GOD, FallenGod);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_MINI_GAME, MiniGame);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_PEAK_TOKEN, RolePeakToken);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_INCOME, RoleIncome);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LEVEL_COMPLEMENT, RoleNewLevelComplement);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_YAO_XING_SHI_LIAN, YaoXingShiLian);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_SAN_XIAN_GUI_DONG, SanXianGuiDong);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_LANTERN_RIDDIE, LanternRiddie);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_WHERE_IS_THE_FAIRY, RoleWhereIsTheFairy);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_GONG_XUN_BAO, RoleTianGongXunBao);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_LOVERS_MARKET, LoversMarket);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_TAO_QUAN_NA_JI, TaoQuanNaJi);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_CONCENTRIC_KNOT, ConcentricKnot);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUNDRED_GOD_BOSS, RoleHundredGodBoss);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ZAO_HUA_MANAGER, ZaoHuaEquipManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_HONGMENG_TIANJIE, RoleHongMengTianJie);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_HOLY_EQUIP, HolyEquipManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_KE_JU_EXAM, KeJuExam);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ZHOU_QI_SHOP, ZhouQiShop);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ELEMENT_ELF, ElementElf);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FU_YU, FuYuManager);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_KUA_FU_YING_XIONG_LU, RoleKuaFuYingXiongLu);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_STAR_CHART_SYSTEM, StarChart);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_NEW_COURAGE_CHALLENGE, NewCourageChallenge);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_FASHION_COLLECTION, FashionCollection);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_GUILD_TASK, RoleGuildTask);
	NEW_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_SHOU, RoleHuanShou);

	UNSTD_STATIC_CHECK(135 == ROLE_MODULE_TYPE_MAX);

	this->SetRole(role);
}

RoleModuleManager::~RoleModuleManager()
{
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_EXCHANGE, Exchange);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_VIP, Vip);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTRIBUTE, RoleAttributePlan);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MONITOR, Monitor);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MONEY, Money);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_KNAPSACK, Knapsack);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_PET, Pet);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_PARTNER, Partner);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE, Prestige);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_COLLECTION, Collection);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TITLE, Title);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TRAIN, Train);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TASKRECORDER, TaskRecorder);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TASKMANAGER, TaskManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_WST_QUESTION, WstQuestion);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_SKILL, RoleSkill);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_DAY_REWARD, DayReward);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_EQUIPMENT, EquipmentManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_WORKSHOP, Workshop);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SURFACE, Surface);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MARRIAGE, Marriage);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_BABY, Baby);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FRIEND_RECOMMEND, FriendRecommend);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_SETTING, RoleSetting);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SHI_TU, ShiTu);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_NOTICE, SystemNotice);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_COURAGE_CHALLENGE, CourageChallenge);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MONSTER_WAVE, MonsterWave);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVITY_MANAGER, RoleActivityManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MI_YI, MiYi);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_TASK, SchoolTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_OFFLINE_EXP, OfflineExp);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ESCORT, Escort);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_MAP, TreasureMap);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TASK_CHAIN, TaskChain);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FABAO, FaBao);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_RESEARCH_TASK, ResearchTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MEDAL, Medal);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CRYSTAL_FB, RoleCrystalFb);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_BUFF_MANAGER, BuffManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SHOP, Shop);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_UNLOCK, MapUnlock);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_FB, RoleGatherFb);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLECROSS, RoleCross);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_COURSE_OF_GROWTH, CourseOfGrowth);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SALARY, Salary);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_CHALLENGE_FB, RoleChallengeFb);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FACE_SCORE, FaceScore);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_GUIDE, FuncGuide);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_DROP, RoleDrop);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CONVERT_SHOP, RoleConvertShop);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SPECIAL_APPEARANCE, SpecialAppearance);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MAIL, RoleMail);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_LOGIN_SYSTEM, LoginSystem);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_BOSS_CHAPTER, BossChapter);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CAPABILITY, Capability);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MYSTERIOUS_TRIAL, MysteriousTrial);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_BOUNTY_TASK, BountyTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TRANSACTION_TASK, TransactionTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVE_SYSTEM, ActiveSystem);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_HUASHEN_MANAGER, HuaShenManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_PREVIEW, FuncPreview);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FIRST_KILL, RoleFirstKill);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_AUTO_CATCH_PET, AutoCatchPet);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TRADE_CREDIT, TradeCredit);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FORMATION, RoleFormation);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_DREAM, RoleGatherDream);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_JIE, RoleHuanJie);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ANECDOTE_TASK, AnecdoteTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_LEVEL_COMPLEMENT, LevelComplement);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_DREAM_NOTES_CHALLENGE, DreamNotesChallenge);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_EXERCISE, SchoolExercise);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_COMMON_SAVE_MANAGER, RoleCommonSaveManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SMART_MOUNTS_SYSTEM, SmartMountsSystem);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_REALM_BREAK, RealmBreak);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_PET_MONOPOLY, PetMonopoly);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_LUN_HUI_WANG_CHUAN, LunHuiWangChuan);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MATERIAL, RoleMaterial);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_EXPLORATION, MapExploration);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_JOIN_ACTIVITY_REWARD, RoleJoinActivityReward);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_CUSTOM_INSTRUCTION, CustomInstruction);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GUILD_INBORN, RoleGuildInborn);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HEART_SKILL, RoleHeartSkill);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ZHU_LING, RoleZhuLing);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_PET_HELPER_FORMATION, PetHelperFormation);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_NETHER_WORLD_PURGATORY, NetherWorldPurgatory);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_PET_GOD_FB, PetGodFB);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_BIG_DIPPER, BigDipper);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN, RoleTianDiDaoHen);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GIVE_GIFT, RoleGiveGift);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE_TASK, PrestigeTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SVIP, SVip);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTR_PELLET, RoleAttrPellet);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_DUEL, Duel);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ACCESSORIES, AccessoriesmentManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TIAN_MING, TianMing);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_DU_JIE, DuJie);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_YIN_YANG_EYE, YinYangEye);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ENDLESS_TOWER, EndlessTower);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ADVANCE_CAREER, AdvanceCareer);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_WAN_LING_FB, RoleWanLingFB);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LIFE_SKILL, RoleNewLifeSkill);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FLY_UP_SYSTEM, FlyUpSystem);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MIRAGE_TERRIROTY, MirageTerritory);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_XIU_WEI, XiuWei);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_TASK, TreasureTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FALLEN_GOD, FallenGod);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_MINI_GAME, MiniGame);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_PEAK_TOKEN, RolePeakToken);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_INCOME, RoleIncome);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LEVEL_COMPLEMENT, RoleNewLevelComplement);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_YAO_XING_SHI_LIAN, YaoXingShiLian);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_SAN_XIAN_GUI_DONG, SanXianGuiDong);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_LANTERN_RIDDIE, LanternRiddie);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_WHERE_IS_THE_FAIRY, RoleWhereIsTheFairy);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_GONG_XUN_BAO, RoleTianGongXunBao);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_LOVERS_MARKET, LoversMarket);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_TAO_QUAN_NA_JI, TaoQuanNaJi);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_CONCENTRIC_KNOT, ConcentricKnot);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUNDRED_GOD_BOSS, RoleHundredGodBoss);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ZAO_HUA_MANAGER, ZaoHuaEquipManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_HONGMENG_TIANJIE, RoleHongMengTianJie);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_HOLY_EQUIP, HolyEquipManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_KE_JU_EXAM, KeJuExam);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ZHOU_QI_SHOP, ZhouQiShop);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ELEMENT_ELF, ElementElf);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FU_YU, FuYuManager);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_KUA_FU_YING_XIONG_LU, RoleKuaFuYingXiongLu);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_STAR_CHART_SYSTEM, StarChart);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_NEW_COURAGE_CHALLENGE, NewCourageChallenge);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_FASHION_COLLECTION, FashionCollection);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_GUILD_TASK, RoleGuildTask);
	DELETE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_SHOU, RoleHuanShou);

	UNSTD_STATIC_CHECK(135 == ROLE_MODULE_TYPE_MAX);
	// 检查是否已把所有new了的模块都delete了

	bool is_error = false;
	for (int i = 0; i < ROLE_MODULE_TYPE_MAX; ++i)
	{
		if (m_role_module_ref_counter[i] != 0)
		{
			gamelog::g_log_serious_error.printf(LL_ERROR, "Role::DeleteAllModule role[%d,%s] module ref counter ERROR, module[%d] counter[%d]",
				this->GetRole()->GetUID(), this->GetRole()->GetName(), i, m_role_module_ref_counter[i]);
			is_error = true;
		}
	}

#ifdef _DEBUG
	assert(!is_error);
#endif
}

void RoleModuleManager::SetRole(Role* role)
{
	m_role = role;
	mRoleActivityManager->SetRole(this);
}

void RoleModuleManager::InitRoleModuleData(const RoleInitParam & init_param,
										   const RoleOtherInitParam & other_init_param,
										   const ItemListParam & item_list_param, 
										   const SkillListParam & skill_list_param, 
										   const MailListParam& maillist_param,
										   const RoleFirstKillParamList& first_kill_list,
										   const RandActivityRoleParamList & rand_activity_role_list,
										   const RoleCommonSaveDataParam & role_common_save_param, 
										   const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
										   const GodPrintItemListParam & god_print_item_list,
										   const NewCourageChallengeLevelParamList & new_courage_challenge_list)
{
	m_guild_common_param = other_init_param.guild_common_param;
	m_common_data_param = other_init_param.common_data_param;
	m_friend_common_param = other_init_param.friend_common_param;

	m_mini_game_id = init_param.mini_game_id;

	this->CommonDataCheck();

	mMoney->Init(this, init_param.coin_bind, init_param.immortal_coin, init_param.gold);
	mTrain->Init(this, other_init_param.train_param);
	mTitle->Init(this, other_init_param.title_param);
	mCollection->Init(this, other_init_param.collection_param);
	mPrestige->Init(this, other_init_param.prestige_param);
	mVip->Init(this, other_init_param.vip_param);
	mRoleAttributePlan->Init(this, other_init_param.attribute_plan_param);
	mMonitor->Init(this, other_init_param.monitor_param);	
	mMoney->InitOtherCoin(other_init_param.money_other_param);
	mWstQuestion->Init(this);
	mDayReward->Init(this, other_init_param.role_day_reward_param);
	mWorkshop->Init(this, other_init_param.workshop_param);
	mSurface->Init(this, other_init_param.surface_param);
	mFriendRecommend->Init(this);
	mSystemNotice->Init(this);
	mOfflineExp->Init(this, other_init_param.common_data_param.offline_exp,other_init_param.common_data_param.offline_min);
	mMapExploration->Init(this, other_init_param.map_exploration_param);
	mFlyUpSystem->Init(this, other_init_param.role_fly_up_param);	//飞升需要放在任务系统init之前(有些任务接取需要依赖于飞升信息）

	gamelog::g_log_debug.printf(LL_DEBUG, "RoleModuleManager::InitRoleModuleData role[%d,%s] gold[%lld] im_coin[%lld] coin_bind[%lld]", m_role->GetUID(), m_role->GetName(),
		init_param.gold, init_param.immortal_coin, init_param.coin_bind);

	int knapsack_param_count = 0;
	const ItemListParam::ItemListData* knapsack_param_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM] = { 0 };

	int pet_param_count = 0;
	const ItemListParam::ItemListData* pet_param_list[ItemNamespace::PET_GRID_MAX_COUNT] = { 0 };

	int equipment_param_num = 0;
	const ItemListParam::ItemListData* equipment_param_list[ItemNamespace::EQUIPMENT_GRID_MAX_COUNT] = { 0 };

	int jewelry_param_num = 0;
	const ItemListParam::ItemListData* jewelry_param_list[ItemNamespace::JEWELRY_GRID_MAX_COUNT] = { 0 };

	int huashen_knapsack_param_count = 0;
	const ItemListParam::ItemListData* huashen_knapsack_param_list[ItemNamespace::HUASHEN_ITEM_GRID_NUM_MAX] = { 0 };

	int huashen_pet_param_count = 0;
	const ItemListParam::ItemListData* huashen_pet_param_list[ItemNamespace::MAX_HUASHEN_PET_GRID_NUM] = { 0 };

	std::vector<const ItemListParam::ItemListData * > medal_item_list;

	int huashen_medal_param_count = 0;
	const ItemListParam::ItemListData* huashen_medal_param_list[ItemNamespace::MAX_HUASHEN_MEADL_INDEX_NUM] = { 0 };

	int accessories_param_count = 0;
	const ItemListParam::ItemListData* accessories_param_list[ItemNamespace::MAX_ACCESSORIES_INDEX_NUM] = { 0 };

	int zao_hua_param_count = 0;
	const ItemListParam::ItemListData* zao_hua_param_list[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM + ItemNamespace::MAX_ZAO_HUA_BAG_NUM] = { 0 };

	int sheng_qi_param_count = 0;
	const ItemListParam::ItemListData* sheng_qi_param_list[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM] = { 0 };

	int fu_shi_param_count = 0;
	const ItemListParam::ItemListData* fu_shi_param_list[ItemNamespace::MAX_RUNE_STONE_BAG_NUM] = { 0 };

	int huan_shou_param_count = 0;
	const ItemListParam::ItemListData* huan_shou_param_list[ItemNamespace::MAX_HUAN_SHOU_GRID_NUM] = { 0 };

	std::vector<const ItemListParam::ItemListData * > fu_yu_item_list;

	for (int i = 0; i < item_list_param.count; i++) // 将属于背包和装备的数据指针分别筛选出来
	{
		const ItemListParam::ItemListData & p_ild = item_list_param.item_list[i];
		if (p_ild.index < ItemNamespace::EQUIPMENT_INDEX_BEGIN && 
			knapsack_param_count < ARRAY_ITEM_COUNT(knapsack_param_list))
		{
			knapsack_param_list[knapsack_param_count++] = &p_ild;
		}
		else if (((p_ild.index >= ItemNamespace::PET_INDEX_BEGIN && p_ild.index < ItemNamespace::PET_INDEX_END) ||
			  (p_ild.index >= ItemNamespace::PET_STORAGE_INDEX_BEGIN && p_ild.index < ItemNamespace::PET_STORAG_INDEX_END)) && 
			pet_param_count < PET_GRID_MAX_COUNT)
		{
			pet_param_list[pet_param_count++] = &p_ild;
		}
		else if (((p_ild.index >= ItemNamespace::EQUIPMENT_INDEX_BEGIN && p_ild.index < ItemNamespace::EQUIPMENT_INDEX_END) ||
			  (p_ild.index >= ItemNamespace::HUASHEN_EQUIPEMNET_INDEX_BEGIN && p_ild.index < ItemNamespace::HUASHEN_EQUIPEMNET_INDEX_END)) &&
			 equipment_param_num < EQUIPMENT_GRID_MAX_COUNT)
		{
			equipment_param_list[equipment_param_num++] = &p_ild;
		}
		else if (((p_ild.index >= ItemNamespace::JEWELRY_INDEX_BEGIN && p_ild.index < ItemNamespace::JEWELRY_INDEX_END) ||
			  (p_ild.index >= ItemNamespace::HUASHEN_JEWELRY_INDEX_BEGIN && p_ild.index < ItemNamespace::HUASHEN_JEWELRY_INDEX_END)) &&
			 jewelry_param_num < JEWELRY_GRID_MAX_COUNT)
		{
			jewelry_param_list[jewelry_param_num++] = &p_ild;
		}
		else if (p_ild.index >= ItemNamespace::HUASHEN_ITEM_INDEX_BEGIN && p_ild.index < ItemNamespace::HUASHEN_ITEM_INDEX_END &&
			huashen_knapsack_param_count < ARRAY_ITEM_COUNT(huashen_knapsack_param_list))
		{
			huashen_knapsack_param_list[huashen_knapsack_param_count++] = &p_ild;
		}
		else if (p_ild.index >= ItemNamespace::HUASHEN_EQUIPEMNET_INDEX_BEGIN && p_ild.index < ItemNamespace::PET_HUASHEN_INDEX_END &&
			huashen_pet_param_count < ARRAY_ITEM_COUNT(huashen_pet_param_list))
		{
			huashen_pet_param_list[huashen_pet_param_count++] = &p_ild;
		}
		else if (ItemNamespace::MEDAL_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::MEDAL_INDEX_END)
		{
			medal_item_list.push_back(&p_ild);
		}
		else if (ItemNamespace::HUASHEN_MEDAL_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::HUASHEN_MEDAL_INDEX_END
			&& huashen_medal_param_count < ARRAY_ITEM_COUNT(huashen_medal_param_list))
		{
			huashen_medal_param_list[huashen_medal_param_count++] = &p_ild;
		}
		else if (ItemNamespace::ACCESSORIES_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::ACCESSORIES_INDEX_END
			&& accessories_param_count < ARRAY_ITEM_COUNT(accessories_param_list))
		{
			accessories_param_list[accessories_param_count++] = &p_ild;
		}
		else if (ItemNamespace::ZAO_HUA_EQUIPMENT_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::ZAO_HUA_BAG_INDEX_END
			&& zao_hua_param_count < ARRAY_ITEM_COUNT(zao_hua_param_list))
		{
			zao_hua_param_list[zao_hua_param_count++] = &p_ild;
		}
		else if (ItemNamespace::HOLY_EQUIP_BAG_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::HOLY_EQUIP_BAG_INDEX_END
			&& sheng_qi_param_count < ARRAY_ITEM_COUNT(sheng_qi_param_list))
		{
			sheng_qi_param_list[sheng_qi_param_count++] = &p_ild;
		}
		else if (ItemNamespace::RUNE_STONE_BAG_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::RUNE_STONE_BAG_INDEX_END
			&& fu_shi_param_count < ARRAY_ITEM_COUNT(fu_shi_param_list))
		{
			fu_shi_param_list[fu_shi_param_count++] = &p_ild;
		}
		else if (ItemNamespace::FU_YU_GRID_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::FU_YU_GRID_INDEX_END)
		{
			fu_yu_item_list.push_back(&p_ild);
		}
		else if (ItemNamespace::HUAN_SHOU_GRID_INDEX_BEGIN <= p_ild.index && p_ild.index < ItemNamespace::HUAN_SHOU_GRID_INDEX_END
			&& huan_shou_param_count < ARRAY_ITEM_COUNT(huan_shou_param_list))
		{
			huan_shou_param_list[huan_shou_param_count++] = &p_ild;
		}
		else
		{
			gamelog::g_log_serious_error.printf(LL_ERROR, "RoleModuleManager::InitRoleModuleData role[%d,%s] item_id[%d] num[%d] is_bind[%d]",
				m_role->GetUID(), m_role->GetName(), p_ild.item_wrapper.item_id, p_ild.item_wrapper.num, p_ild.item_wrapper.is_bind);
		}
	}

	mKnapsack->Init(this, knapsack_param_count, knapsack_param_list, other_init_param.knapsack_other_param);
	mFaBao->Init(this, other_init_param.fabao_param, other_init_param.fabao_draw_param);
	mEquipmentManager->Init(this, &other_init_param.equip_pther_param, equipment_param_num, equipment_param_list, jewelry_param_num, jewelry_param_list);
	mHuaShenManager->Init(this, huashen_knapsack_param_count, huashen_knapsack_param_list, other_init_param.role_huashen_param,
							huashen_pet_param_count, huashen_pet_param_list, huashen_medal_param_count, huashen_medal_param_list);
	mRoleSkill->Init(this, skill_list_param, other_init_param.skill_other_param);
	mMarriage->Init(this, other_init_param.marry_param);
	mBaby->Init(this, other_init_param.role_baby_param);
	mRoleSetting->Init(this, other_init_param.role_system_set_param);
	mShiTu->Init(this, other_init_param.shi_tu_param);
	mCourageChallenge->Init(this, other_init_param.courage_challenge_param, role_courage_challenge_list);
	mMonsterWave->Init(this, other_init_param.monster_wave_param);
	mRoleActivityManager->Init(this, other_init_param.roleactivity_common_param, rand_activity_role_list);
	mMiYi->Init(this, other_init_param.mi_yi_param);
	mSchoolTask->Init(this, other_init_param.school_task_param);
	mEscort->Init(this);
	mTreasureMap->Init(this, other_init_param.treasure_map_param);
	mTaskChain->Init(this, other_init_param.task_chain_param);
	mResearchTask->Init(this, other_init_param.research_task_param);
	mMedal->Init(this, other_init_param.medal_param, medal_item_list);
	mBuffManager->Init(this, other_init_param.buff_param);
	mRoleCrystalFb->Init(this, other_init_param.role_crystal_fb);
	mShop->Init(this, other_init_param.shop_param);
	mMapUnlock->Init(this, other_init_param.map_unlock_param);
	mRoleGatherFb->Init(this, other_init_param.role_gather_fb_param);
	mExchange->Init(this, other_init_param.exchange_param);
	mRoleCross->Init(this, other_init_param.rolecross_param);
	mSalary->Init(this, other_init_param.salary_param);
	mRoleChallengeFb->InitParam(this, other_init_param.challenge_fb_param);
	mFaceScore->Init(this, other_init_param.face_score_time_param,other_init_param.face_score_param, other_init_param.face_score_record_len, other_init_param.face_score_record_list);
	mFuncGuide->InitFuncGuideInfo(this, other_init_param.func_guide_param);
	mRoleDrop->Init(this, other_init_param.role_drop_param, 
		other_init_param.monster_group_drop_param,
		other_init_param.item_cool_down_drop_param);
	mRoleConvertShop->Init(this, other_init_param.convert_shop_param);
	mSpecialAppearance->Init(this, other_init_param.special_appearance_param);
	mRoleMail->Init(this, maillist_param);
	mLoginSystem->Init(this, other_init_param.role_login_system_param);
	mBossChapter->Init(this, other_init_param.boss_chapter_param);
	mCapability->Init(this, init_param.capability, init_param.max_capability, other_init_param.role_huashen_param.capability, other_init_param.role_huashen_param.max_capability);
	mPet->Init(this, other_init_param.pet_common_param, other_init_param.pet_skin_param, other_init_param.pet_auto_move_param,other_init_param.pet_strengthen_buff_param, pet_param_count, pet_param_list);
	mPartner->Init(this, 
		other_init_param.partner_param, 
		other_init_param.new_partner_param, 
		other_init_param.partner_gift_tree_param,
		other_init_param.partner_pact_param);
	mPet->InitGodPrintBag(other_init_param.god_print_bag_param, other_init_param.god_print_log_param, other_init_param.new_god_print_log_param);
	mPet->InitNewGodPrintBag(god_print_item_list);
	mMysteriousTrial->Init(this, other_init_param.role_mysterious_trial_param);
	mTransactionTask->Init(this, other_init_param.role_transaction_task_param);
	mActiveSystem->Init(this,
		other_init_param.role_active_system_param,
		other_init_param.week_base_param,
		other_init_param.week_task_param,
		other_init_param.active_turntable_param);
	mFlyUpSystem->Init(this, other_init_param.role_fly_up_param);
	mTaskRecorder->Init(this, other_init_param.taskrecrod_len, other_init_param.taskrecord_list,other_init_param.role_chosed_param);
	mTaskManager->Init(this, other_init_param.task_param, other_init_param.task_trace_param, other_init_param.task_wenxianglou_param);	// 必须放在任务记录的后面
	mBountyTask->Init(this, other_init_param.bounty_task_param);
	mFuncPreview->Init(this, other_init_param.func_preview_param);
	mRoleFirstKill->Init(this, first_kill_list);
	mAutoCatchPet->Init(this, other_init_param.auto_catch_pet_param);
	mTradeCredit->Init(this, other_init_param.trade_credit_param);
	mRoleFormation->Init(this, other_init_param.formation_param);
	mRoleGatherDream->Init(this, other_init_param.gather_dream_param);
	mRoleHuanJie->Init(this, other_init_param.role_huan_jie_param);
	mAnecdoteTask->Init(this, other_init_param.anecdote_task_param, other_init_param.anecdote_task_common_param, other_init_param.special_anecdote_param);
	mLevelComplement->Init(this, other_init_param.common_data_param);
	mDreamNotesChallenge->Init(this, other_init_param.dream_notes_challenge_param);
	mSchoolExercise->Init(this, other_init_param.school_exercise_param);
	mRoleCommonSaveManager->Init(this, role_common_save_param);
	mSmartMountsSystem->Init(this, other_init_param.smart_mounts_system_param);
	mRealmBreak->Init(this, other_init_param.realm_break_param);
	mPetMonopoly->Init(this, other_init_param.new_pet_monopoly_param, other_init_param.old_pet_monopoly_param);
	mLunHuiWangChuan->Init(this, other_init_param.lun_hui_wang_chuan_param);
	mRoleMaterial->Init(this, other_init_param.role_material_param);
	mRoleJoinActivityReward->Init(this, other_init_param.role_join_activity_reward_param);
	mCustomInstruction->Init(this, other_init_param.custom_instruction_param);
	mRoleGuildInborn->Init(this, other_init_param.role_guild_inborn_param);
	mRoleHeartSkill->Init(this, other_init_param.m_heart_skill_param);
	mPetHelperFormation->Init(this, other_init_param.pet_helper_formation_param);
	mRoleZhuLing->Init(this);
	mNetherWorldPurgatory->Init(this, other_init_param.nether_world_gatory_param);
	mPetGodFB->Init(this, other_init_param.pet_god_fb_param);
	mBigDipper->Init(this, other_init_param.big_dipper_param, other_init_param.big_dipper_help_param);
	mRoleTianDiDaoHen->Init(this, other_init_param.tian_di_dao_hen_param);
	mRoleGiveGift->Init(this);
	mPrestigeTask->Init(this, other_init_param.prestige_task_param);
	mSVip->Init(this);
	mRoleAttrPellet->Init(this, other_init_param.role_attr_pellet_param);
	mDuel->Init(this, other_init_param.duel_param);
	mAccessoriesmentManager->Init(this, accessories_param_count, accessories_param_list, m_common_data_param.accessories_special_equip_list);
	mTianMing->Init(this, other_init_param.tian_ming_param);
	mDuJie->Init(this, other_init_param.du_jie_param);
	mYinYangEye->Init(this, other_init_param.yin_yang_eye_param);
	mEndlessTower->Init(this, other_init_param.common_data_param.endless_tower_battle_seq, other_init_param.common_data_param.endless_tower_battle_tamp);
	mAdvanceCareer->Init(this, other_init_param.advance_career_param);
	mRoleWanLingFB->Init(this, other_init_param.wan_ling_fb_param);
	mRoleNewLifeSkill->Init(this, other_init_param.role_new_life_skill_param);
	mMirageTerritory->Init(this, other_init_param.role_mirage_territory_param);
	mTreasureTask->Init(this, other_init_param.treasure_task_param);
	mCourseOfGrowth->Init(this, other_init_param.course_of_growth_param, other_init_param.course_of_growth_len, other_init_param.course_of_growth_list, other_init_param.role_course_param);
	mXiuWei->Init(this, other_init_param.xiu_wei_param);
	mFallenGod->Init(this, other_init_param.fallen_god_param, other_init_param.fallen_god_help_param);
	mMiniGame->Init(this, other_init_param.mini_game_param);
	mRolePeakToken->Init(this, other_init_param.role_peak_token_param);
	mRoleIncome->Init(this, other_init_param.role_income_param);
	mRoleNewLevelComplement->Init(this, other_init_param.new_level_complement_param);
	mYaoXingShiLian->Init(this, other_init_param.yao_xing_shi_lian_param);
	mSanXianGuiDong->Init(this, other_init_param.common_data_param.san_xian_today_play_times, other_init_param.common_data_param.san_xian_pass_idx, other_init_param.common_data_param.choose_tamp);
	mLanternRiddie->Init(this, other_init_param.lantern_riddie_param);
	mRoleWhereIsTheFairy->Init(this);
	mRoleTianGongXunBao->Init(this);
	mLoversMarket->Init(this, other_init_param.lovers_market_param);
	mTaoQuanNaJi->Init(this, other_init_param.tao_quan_na_ji_param);
	mConcentricKnot->Init(this, other_init_param.concentric_knot_param);
	mRoleHundredGodBoss->Init(this);
	mZaoHuaEquipManager->Init(this, other_init_param.zao_hua_refine_param, other_init_param.zao_hua_long_zhu_param,zao_hua_param_count, zao_hua_param_list);
	mRoleHongMengTianJie->Init(this, other_init_param.role_hongmeng_tianjie_param);
	mHolyEquipManager->Init(this, other_init_param.role_holy_equip_common_param, sheng_qi_param_count, sheng_qi_param_list, fu_shi_param_count, fu_shi_param_list);
	mKeJuExam->Init(this, other_init_param.role_ke_ju_exam_param);
	mElementElf->Init(this, other_init_param.element_elf_param);
	mFuYuManager->Init(this, other_init_param.fu_yu_common_param, fu_yu_item_list);
	mRoleKuaFuYingXiongLu->Init(this, other_init_param.role_kua_fu_ying_xiong_lu_param);
	mStarChart->Init(this, other_init_param.star_chart_param);
	mNewCourageChallenge->Init(this, other_init_param.new_courage_challenge_param, new_courage_challenge_list);
	mFashionCollection->Init(this, other_init_param.fashion_collection_param);
	mRoleGuildTask->Init(this, other_init_param.role_guild_task_param);
	mRoleHuanShou->Init(this, huan_shou_param_count, huan_shou_param_list, other_init_param.role_huan_shou_param);

	UNSTD_STATIC_CHECK(135 == ROLE_MODULE_TYPE_MAX);
	
	// 初始赋值一些模块的数据

#ifdef _DEBUG
	if ((BATTLE_MOVE_TYPE_PERFORM_SKILL == m_common_data_param.battle_auto_first_move_type &&
		m_common_data_param.battle_auto_first_skill_level <= 0) ||
		(BATTLE_MOVE_TYPE_NORMAL_ATTACK != m_common_data_param.battle_auto_first_move_type && 
		BATTLE_MOVE_TYPE_NORMAL_DEFENCE != m_common_data_param.battle_auto_first_move_type &&
		BATTLE_MOVE_TYPE_PERFORM_SKILL != m_common_data_param.battle_auto_first_move_type &&
		BATTLE_MOVE_TYPE_AI_SKILL != m_common_data_param.battle_auto_first_move_type))
	{
		m_common_data_param.battle_auto_first_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		m_common_data_param.battle_auto_first_skill_id = 0;
		m_common_data_param.battle_auto_first_skill_level = 0;
	}
	if ((BATTLE_MOVE_TYPE_PERFORM_SKILL == m_common_data_param.battle_auto_second_move_type &&
		m_common_data_param.battle_auto_second_skill_level <= 0) ||
		(BATTLE_MOVE_TYPE_NORMAL_ATTACK != m_common_data_param.battle_auto_second_move_type &&
			BATTLE_MOVE_TYPE_NORMAL_DEFENCE != m_common_data_param.battle_auto_second_move_type &&
			BATTLE_MOVE_TYPE_PERFORM_SKILL != m_common_data_param.battle_auto_second_move_type &&
			BATTLE_MOVE_TYPE_AI_SKILL != m_common_data_param.battle_auto_second_move_type))
	{
		m_common_data_param.battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
		m_common_data_param.battle_auto_second_skill_id = 0;
		m_common_data_param.battle_auto_second_skill_level = 0;
	}
#endif
	if (BATTLE_MOVE_TYPE_PERFORM_SKILL == m_common_data_param.battle_auto_second_move_type)
	{
		const PetParam* fight_pet_param = this->GetPet()->GetFightPetParam();
		if (NULL != fight_pet_param)
		{
			bool found = false;
			const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(fight_pet_param->pet_id);
			if (NULL != pet_cfg)
			{
				int skill_id = m_common_data_param.battle_auto_second_skill_id;
				int skill_level = m_common_data_param.battle_auto_second_skill_level;
				for (size_t i = 0; i < pet_cfg->exclusive_skills.size(); ++i)
				{
					if (skill_id == pet_cfg->exclusive_skills[i].skill_id &&
						skill_level <= pet_cfg->exclusive_skills[i].skill_level)
					{
						found = true;
					}
				}
			}
			
			if (!found)
			{
				m_common_data_param.battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				m_common_data_param.battle_auto_second_skill_id = 0;
				m_common_data_param.battle_auto_second_skill_level = 0;
			}
		}
		else
		{
			const RoleSkill::SkillItem* skill = this->GetRoleSkill()->GetSkillBySkillId(m_common_data_param.battle_auto_second_skill_id);
			if (NULL == skill)
			{
				m_common_data_param.battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				m_common_data_param.battle_auto_second_skill_id = 0;
				m_common_data_param.battle_auto_second_skill_level = 0;
			}
			else if (skill->skill_show_level < m_common_data_param.battle_auto_second_skill_level)
			{
				m_common_data_param.battle_auto_second_skill_level = skill->skill_show_level;
			}
		}
	}
	if (m_common_data_param.is_first_create_find_fantasy != 1)
	{
		for (int i = 1; i <= LAYER_MAX_NUM; i++) 
		{
			m_common_data_param.find_fantasy_flag.SetBit(i);
		}
		m_common_data_param.is_first_create_find_fantasy = 1;
	}

	// 需要各系统数据 放在最下面
	this->MakeAppearanceData();
	mSmartMountsSystem->InitEnd();
	mRoleDrop->CheckTask();
	mRoleSkill->OnInitEnd();
	mTrain->CheckUpLevelItem();
	mTaskManager->InitEnd();
	mCourseOfGrowth->InitEnd();
}

void RoleModuleManager::GetRoleModuleData(RoleInitParam * out_init_param,
										  RoleOtherInitParam * out_other_init_param,
										  ItemListParam * out_item_list_param,
										  SkillListParam * out_skill_list_param,
										  MailListParam* out_maillist_param,
										  RoleFirstKillParamList* out_first_kill_list,
										  RandActivityRoleParamList  * out_rand_activity_role_list,
										  RoleCommonSaveDataParam * role_common_save_param,
										  RoleCourageChallengeLevelParamList * role_courage_challenge_list,
										  GodPrintItemListParam * god_print_item_list,
										  NewCourageChallengeLevelParamList * new_courage_challenge_list,
										  bool is_logout /* = false */ )
{
	mTaskRecorder->GetInitParam(out_other_init_param->taskrecord_list, &out_other_init_param->taskrecrod_len,&out_other_init_param->role_chosed_param);
	mTaskManager->GetTaskInitParam(&out_other_init_param->task_param, &out_other_init_param->task_trace_param, &out_other_init_param->task_wenxianglou_param);
	mTrain->GetOtherInitParam(&out_other_init_param->train_param);
	mTitle->GetOtherInitParam(&out_other_init_param->title_param);
	mCollection->GetOtherInitParam(&out_other_init_param->collection_param);
	mPartner->GetPartnerParam(&out_other_init_param->partner_param,
		&out_other_init_param->new_partner_param,
		&out_other_init_param->partner_gift_tree_param,
		&out_other_init_param->partner_pact_param);
	mPrestige->GetOtherInitParam(&out_other_init_param->prestige_param);
	mVip->GetOtherInitParam(&out_other_init_param->vip_param);
	mRoleAttributePlan->GetOtherInitParam(&out_other_init_param->attribute_plan_param);
	mMonitor->GetMonitorParam(&out_other_init_param->monitor_param);
	mMoney->GetInitOtherCoin(&out_other_init_param->money_other_param);
	mKnapsack->GetOtherInitParam(&out_other_init_param->knapsack_other_param);
	mRoleSkill->GetSkillOtherParam(&out_other_init_param->skill_other_param);
	mEquipmentManager->GetInitParam(&out_other_init_param->equip_pther_param);
	mDayReward->GetInitParam(&out_other_init_param->role_day_reward_param);
	mWorkshop->GetWorkshopParam(&out_other_init_param->workshop_param);
	mSurface->GetOtherInitParam(&out_other_init_param->surface_param);
	mOfflineExp->GetOtherInitParam(&m_common_data_param.offline_exp, &m_common_data_param.offline_min);
	mPet->GetInitParam(&out_other_init_param->pet_common_param, &out_other_init_param->pet_skin_param, &out_other_init_param->pet_auto_move_param, &out_other_init_param->pet_strengthen_buff_param, &out_other_init_param->god_print_bag_param, &out_other_init_param->god_print_log_param, god_print_item_list, &out_other_init_param->new_god_print_log_param);
	mMapExploration->GetOtherInitParam(&out_other_init_param->map_exploration_param);
	mNetherWorldPurgatory->GetInitParam(&out_other_init_param->nether_world_gatory_param);

	out_init_param->coin_bind = mMoney->GetCoinBind();
	out_init_param->immortal_coin = mMoney->GetImmortalCoin();
	out_init_param->gold = mMoney->GetGold();
	out_init_param->history_recharge_gold = mRoleActivityManager->GetCommonParam().history_chongzhi_num;
	out_init_param->mini_game_id = this->GetMiniGameId();
	if (out_init_param->mini_game_id < MINI_GAME_NONE || out_init_param->mini_game_id >= MINI_GAME_MAX)
	{
		out_init_param->mini_game_id = MINI_GAME_NONE;
	}

	gamelog::g_log_debug.printf(LL_DEBUG, "RoleModuleManager::GetRoleModuleData role[%d,%s] gold[%lld] im_coin[%lld] coin_bind[%lld]", m_role->GetUID(), m_role->GetName(),
		out_init_param->gold, out_init_param->immortal_coin, out_init_param->coin_bind);

	int size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mKnapsack->GetChangeItemList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mPet->GetChangeItemList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mEquipmentManager->GetChangeEquipmentList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mEquipmentManager->GetChangeJewelryList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mHuaShenManager->GetChangeItemList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mHuaShenManager->GetChangePetItemList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mMedal->GetChangeMedalItemList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mHuaShenManager->GetChangeMedalItemList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mAccessoriesmentManager->GetChangeAccessoriesmentList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mZaoHuaEquipManager->GetChangeZaoHuaEquipWearList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mZaoHuaEquipManager->GetChangeZaoHuaEquipBagList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mHolyEquipManager->GetChangeHolyEquipBagList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mHolyEquipManager->GetChangeFuShiBagList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mFuYuManager->GetChangeFuYuItemList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	size = ItemNamespace::MAX_GRID_NUM - out_item_list_param->count;
	mRoleHuanShou->GetChangeBagList(&size, &out_item_list_param->item_list[out_item_list_param->count]);
	out_item_list_param->count += size;

	out_skill_list_param->count = 0;
	mRoleSkill->GetChangeSkillList(out_skill_list_param);

	out_first_kill_list->count = 0;
	mRoleFirstKill->GetChangedFirstKillInfo(out_first_kill_list);

	out_rand_activity_role_list->count = 0;
	mRoleActivityManager->GetInitParam(&out_other_init_param->roleactivity_common_param, out_rand_activity_role_list , is_logout);

	role_common_save_param->count = 0;
	mRoleCommonSaveManager->GetInitParam(role_common_save_param, is_logout);

	role_courage_challenge_list->count = 0;
	mCourageChallenge->GetInitParam(&out_other_init_param->courage_challenge_param, role_courage_challenge_list);

	new_courage_challenge_list->count = 0;
	mNewCourageChallenge->GetInitParam(&out_other_init_param->new_courage_challenge_param, new_courage_challenge_list);

	god_print_item_list->count = 0;
	mPet->GetGodPrintChangeItemList(god_print_item_list);

	mMarriage->GetOtherInitParam(&out_other_init_param->marry_param);
	mBaby->GetOtherInitParam(&out_other_init_param->role_baby_param);
	mRoleSetting->GetOtherInitParam(&out_other_init_param->role_system_set_param);
	mShiTu->GetInitParam(&out_other_init_param->shi_tu_param);
	
	mMonsterWave->GetInitParam(&out_other_init_param->monster_wave_param);
	mMiYi->GetInitParam(&out_other_init_param->mi_yi_param);
	mSchoolTask->GetInitParam(&out_other_init_param->school_task_param);
	mTreasureMap->GetInitParam(&out_other_init_param->treasure_map_param);
	mResearchTask->GetInitParam(&out_other_init_param->research_task_param);
	mTaskChain->GetInitParam(&out_other_init_param->task_chain_param);
	mFaBao->GetOtherInitParam(&out_other_init_param->fabao_param, &out_other_init_param->fabao_draw_param);
	mMedal->GetInitParam(&out_other_init_param->medal_param);
	mBuffManager->GetInitParam(&out_other_init_param->buff_param);
	mRoleCrystalFb->GetInitParam(&out_other_init_param->role_crystal_fb);
	mShop->GetOtherInitParam(&out_other_init_param->shop_param);
	mMapUnlock->GetOtherInitParam(&out_other_init_param->map_unlock_param);
	mRoleGatherFb->GetInitParam(&out_other_init_param->role_gather_fb_param);
	mExchange->GetOtherInitParam(&out_other_init_param->exchange_param);
	mRoleCross->GetInitParam(&out_other_init_param->rolecross_param);
	mCourseOfGrowth->GetCourseOfGrowthParam(&out_other_init_param->course_of_growth_param, out_other_init_param->course_of_growth_list, &out_other_init_param->course_of_growth_len, 
		&out_other_init_param->role_course_param);
	mSalary->GetParam(&out_other_init_param->salary_param);
	mRoleChallengeFb->GetChallengeFbParam(&out_other_init_param->challenge_fb_param);
	mFaceScore->GetFaceScoreParam(&out_other_init_param->face_score_time_param,&out_other_init_param->face_score_param, out_other_init_param->face_score_record_list, &out_other_init_param->face_score_record_len);
	mFuncGuide->GetFuncGuideParam(&out_other_init_param->func_guide_param);
	mRoleDrop->GetInitParam(&out_other_init_param->role_drop_param, 
		&out_other_init_param->monster_group_drop_param,
		&out_other_init_param->item_cool_down_drop_param);
	mRoleConvertShop->GetInitParam(&out_other_init_param->convert_shop_param);
	mSpecialAppearance->GetInitParam(&out_other_init_param->special_appearance_param);
	mLoginSystem->GetOtherInitParam(&out_other_init_param->role_login_system_param);
	mBossChapter->GetInitParam(&out_other_init_param->boss_chapter_param);
	mBountyTask->GetInitParam(&out_other_init_param->bounty_task_param);
	mRoleMail->GetChangeMailList(out_maillist_param);
	mMysteriousTrial->GetInitParam(&out_other_init_param->role_mysterious_trial_param);
	mTransactionTask->GetInitParam(&out_other_init_param->role_transaction_task_param);
	mActiveSystem->GetInitParam(&out_other_init_param->role_active_system_param,
		&out_other_init_param->week_base_param,
		&out_other_init_param->week_task_param,
		&out_other_init_param->active_turntable_param);

	mHuaShenManager->GetInitParam(&out_other_init_param->role_huashen_param);
	mFuncPreview->GetInitParam(&out_other_init_param->func_preview_param);
	mAutoCatchPet->GetInitParam(&out_other_init_param->auto_catch_pet_param);
	mTradeCredit->GetInitParam(&out_other_init_param->trade_credit_param);
	mRoleFormation->GetInitParam(&out_other_init_param->formation_param);
	mRoleGatherDream->GetInitParam(&out_other_init_param->gather_dream_param);
	mRoleHuanJie->GetInitParam(&out_other_init_param->role_huan_jie_param);
	mAnecdoteTask->GetInitParam(&out_other_init_param->anecdote_task_param, &out_other_init_param->anecdote_task_common_param, &out_other_init_param->special_anecdote_param);
	mLevelComplement->GetInitParam(&m_common_data_param);
	mDreamNotesChallenge->GetInitParam(&out_other_init_param->dream_notes_challenge_param);
	mSchoolExercise->GetInitParam(&out_other_init_param->school_exercise_param);
	mSmartMountsSystem->GetInitParam(&out_other_init_param->smart_mounts_system_param);
	mRealmBreak->GetInitParam(&out_other_init_param->realm_break_param);
	mPetMonopoly->GetInitParam(&out_other_init_param->new_pet_monopoly_param);
	mLunHuiWangChuan->GetInitParam(&out_other_init_param->lun_hui_wang_chuan_param);
	mRoleMaterial->GetInitParam(&out_other_init_param->role_material_param);
	mRoleJoinActivityReward->GetInitParam(&out_other_init_param->role_join_activity_reward_param);
	mCustomInstruction->GetInitParam(&out_other_init_param->custom_instruction_param);
	mRoleGuildInborn->GetInitParam(&out_other_init_param->role_guild_inborn_param);
	mRoleHeartSkill->GetInitParam(&out_other_init_param->m_heart_skill_param);
	mPetHelperFormation->GetInitParam(&out_other_init_param->pet_helper_formation_param);
	mPetGodFB->GetInitParam(&out_other_init_param->pet_god_fb_param);
	mBigDipper->GetInitParam(&out_other_init_param->big_dipper_param, &out_other_init_param->big_dipper_help_param);
	mPrestigeTask->GetInitParam(&out_other_init_param->prestige_task_param);
	mRoleAttrPellet->GetOtherInitParam(&out_other_init_param->role_attr_pellet_param);
	mDuel->GetInitParam(&out_other_init_param->duel_param);
	mTianMing->GetInitParam(&out_other_init_param->tian_ming_param);
	mDuJie->GetInitParam(&out_other_init_param->du_jie_param);
	mYinYangEye->GetInitParam(&out_other_init_param->yin_yang_eye_param);
	mEndlessTower->GetInitParam(&m_common_data_param.endless_tower_battle_seq, &m_common_data_param.endless_tower_battle_tamp);
	mRoleNewLifeSkill->GetInitParam(&out_other_init_param->role_new_life_skill_param);
	mFlyUpSystem->GetInitParam(&out_other_init_param->role_fly_up_param);
	mMirageTerritory->GetInitParam(&out_other_init_param->role_mirage_territory_param);

	mRoleTianDiDaoHen->GetInitParam(&out_other_init_param->tian_di_dao_hen_param);
	mAdvanceCareer->GetOtherInitParam(&out_other_init_param->advance_career_param);
	mRoleWanLingFB->GetInitParam(&out_other_init_param->wan_ling_fb_param);
	mXiuWei->GetOtherInitParam(&out_other_init_param->xiu_wei_param);
	mTreasureTask->GetInitParam(&out_other_init_param->treasure_task_param);
	mPet->GetRolePetInfoParam(&out_other_init_param->role_pet_info_param);
	mPartner->GetRolePartnerInfoParam(&out_other_init_param->role_partner_info_param);
	mFallenGod->GetInitParam(&out_other_init_param->fallen_god_param, &out_other_init_param->fallen_god_help_param);
	mMiniGame->GetInitParam(&out_other_init_param->mini_game_param);
	mMirageTerritory->GetInfoParam(&out_other_init_param->role_mirage_territory_info_param);
	mRolePeakToken->GetInitParam(&out_other_init_param->role_peak_token_param);
	mRoleIncome->GetInitParam(&out_other_init_param->role_income_param);
	mRoleNewLevelComplement->GetInitParam(&out_other_init_param->new_level_complement_param);
	mYaoXingShiLian->GetInitParam(&out_other_init_param->yao_xing_shi_lian_param);
	mSanXianGuiDong->GetInitParam(&m_common_data_param.san_xian_today_play_times, &m_common_data_param.san_xian_pass_idx, &m_common_data_param.choose_tamp);
	mLanternRiddie->GetInitParam(&out_other_init_param->lantern_riddie_param);
	mLoversMarket->GetInitParam(&out_other_init_param->lovers_market_param);
	mTaoQuanNaJi->GetInitParam(&out_other_init_param->tao_quan_na_ji_param);
	mConcentricKnot->GetInitParam(&out_other_init_param->concentric_knot_param);
	mRoleHongMengTianJie->GetInitParam(&out_other_init_param->role_hongmeng_tianjie_param);
	mZaoHuaEquipManager->GetInitParam(&out_other_init_param->zao_hua_refine_param, &out_other_init_param->zao_hua_long_zhu_param);
	mHolyEquipManager->GetInitParam(&out_other_init_param->role_holy_equip_common_param);
	mKeJuExam->GetInitParam(&out_other_init_param->role_ke_ju_exam_param);
	mZhouQiShop->GetInitParam(&out_other_init_param->zhou_qi_shop_param);
	mElementElf->GetInitParam(&out_other_init_param->element_elf_param);
	mFuYuManager->GetInitParam(&out_other_init_param->fu_yu_common_param);
	mRoleKuaFuYingXiongLu->GetInitParam(&out_other_init_param->role_kua_fu_ying_xiong_lu_param);
	mStarChart->GetInitParam(&out_other_init_param->star_chart_param);
	mFashionCollection->GetOtherInitParam(&out_other_init_param->fashion_collection_param);
	mAccessoriesmentManager->GetInitParam(m_common_data_param.accessories_special_equip_list);
	mRoleGuildTask->GetInitParam(&out_other_init_param->role_guild_task_param);
	mRoleHuanShou->GetInitParam(&out_other_init_param->role_huan_shou_param);

	//需要放在化身后面,否则化身战力会被覆盖
	mCapability->GetCapabilityInitParam(&out_init_param->capability, &out_init_param->max_capability, &out_other_init_param->role_huashen_param.capability, 
			&out_other_init_param->role_huashen_param.max_capability);	

	UNSTD_STATIC_CHECK(135 == ROLE_MODULE_TYPE_MAX);

	m_common_data_param.maxhp_in_scene = this->GetAttrByType(BATTLE_ATTR_MAX_HP);
	m_common_data_param.curhp_in_scene = this->GetAttrByType(BATTLE_ATTR_CUR_HP);
	m_common_data_param.maxmp_in_scene = this->GetAttrByType(BATTLE_ATTR_MAX_MP);
	m_common_data_param.curmp_in_scene = this->GetAttrByType(BATTLE_ATTR_CUR_MP);

	out_other_init_param->common_data_param = m_common_data_param;
	out_other_init_param->guild_common_param = m_guild_common_param;
	out_other_init_param->appearance_param = m_appearance;

	out_other_init_param->friend_common_param = m_friend_common_param;
}

int RoleModuleManager::GetRoleUID() const
{
	return m_role->GetUID();
}

const char* RoleModuleManager::GetRoleName() const
{
	return m_role->GetName();
}

void RoleModuleManager::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	this->UpdateCapacityTarget(now_second);
	mKnapsack->Update(now_second);
	mMoney->GetAccountGoldFromDB(now_second);
	mTitle->Update(interval, now_second, now_dayid);
	mTaskManager->Update(now_second);
	mShiTu->Update(interval, now_second);
	mRoleActivityManager->Update(now_second);
	mResearchTask->Update(now_second);
	mMedal->Update(interval, now_second);
	mBuffManager->Update(interval, now_second, now_dayid);
	mFaceScore->Update(now_second);
	mTaskChain->TaskChainUpdate(now_second);
	mMonitor->Update(now_second);
	mPartner->Update(now_second);
	mHuaShenManager->Update(now_second);
	this->CheckHuanJieSeasonChange();
	mRoleMail->Update(now_second);
	mRoleCrystalFb->Update(now_second);
	mLevelComplement->Update(now_second);
	this->GetRoleCmdFromDB(now_second);	
	mDayReward->Update(now_second);
	mRealmBreak->Update(now_second);
	mLunHuiWangChuan->Update(now_second);
	mAnecdoteTask->Update((unsigned int)now_second);
	this->OnTopLevelUpdate((unsigned int)now_second);
	this->ResetShanHaiSomeData(interval, now_second, now_dayid);	
	mDuel->Update(interval, now_second, now_dayid);
	this->OnAutoTalkUpdate((unsigned int)now_second);
	mRoleNewLifeSkill->Update(now_second);
	mSurface->Update(interval, now_second);
	mSanXianGuiDong->UpDate(now_second);
	mLanternRiddie->Update(interval, now_second);
	mRoleTianGongXunBao->Update(now_second);
	mTaoQuanNaJi->Update(now_second);
	mRoleHundredGodBoss->Update((unsigned int)now_second);
	mRoleHongMengTianJie->Update((unsigned int)now_second);
	mKeJuExam->Update(interval, now_second);
	mFuYuManager->Update(interval, now_second);
	mRoleKuaFuYingXiongLu->OnUpdate((unsigned int)now_second);
	mRoleHuanShou->Update((unsigned int)now_second);
	mRoleGuildTask->Update(now_second);
}

void RoleModuleManager::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	mWstQuestion->OnResetModuleData(old_dayid, now_dayid);
	mResearchTask->OnResetModuleData(old_dayid, now_dayid);
	this->OnTreviFountainResetData(old_dayid, now_dayid);
	
	mDayReward->OnResetModuleData(old_dayid, now_dayid);
	mPrestige->OnResetModuleData(old_dayid, now_dayid);
	mBaby->OnResetData(old_dayid, now_dayid);
	mCourageChallenge->OnResetData(old_dayid, now_dayid);
	mMonsterWave->OnResetData(old_dayid, now_dayid);
	mSchoolTask->OnResetModuleData(old_dayid, now_dayid);
	mRoleActivityManager->OnResetModuleData(old_dayid, now_dayid);
	mEscort->OnResetModuleData(old_dayid, now_dayid);
	mTreasureMap->OnResetModuleData(old_dayid, now_dayid);
	mExchange->OnResetModuleData(old_dayid, now_dayid);
	mTransactionTask->OnResetModuleData(old_dayid, now_dayid);
	mBountyTask->OnResetModuleData(old_dayid, now_dayid);
	mRoleGatherFb->OnResetModuleData(old_dayid, now_dayid);
	mActiveSystem->OnResetModuleData(old_dayid, now_dayid);
	mMysteriousTrial->OnResetModuleData(old_dayid, now_dayid);
	mLoginSystem->OnResetModuleData(old_dayid, now_dayid);
	mMonitor->OnResetData(old_dayid, now_dayid);
	m_common_data_param.today_patrol_task == 1 ? m_common_data_param.today_patrol_task = 1 : m_common_data_param.today_patrol_task = 0, //m_common_data_param.today_patrol_task = 0;
	m_common_data_param.patrol_task_reward = 0;
	m_common_data_param.patrol_task_progress = 0;
	m_common_data_param.patrol_task_status = 0;
	m_common_data_param.patrol_task_coin = 0;
	m_common_data_param.today_open_find_fun = 0;
	{
		memset(m_guild_common_param.build_donate_count, 0, sizeof(m_guild_common_param.build_donate_count));
		Guild * guild = GuildManager::Instance().GetGuild(m_role->GetGuildID());
		if (NULL != guild)
		{
			guild->SendGuildBossInfo(m_role);
		}
	}

	mShop->OnResetModuleData(old_dayid, now_dayid);
	mMarriage->OnResetModuleData(old_dayid, now_dayid);
	mRoleCrystalFb->OnResetModuleData(old_dayid, now_dayid);
	mRoleConvertShop->OnResetModuleData(old_dayid, now_dayid);
}

void RoleModuleManager::OnResetModuleDataWeek()
{
	mShop->OnResetModuleDataWeek();
	mExchange->OnResetModuleDataWeek();
	mRoleConvertShop->OnResetModuleDataWeek();
}

void RoleModuleManager::OnResetModuleDataMonth()
{
	mRoleConvertShop->OnResetModuleDataMonth();
}

void RoleModuleManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	this->OnLoginCalCreateDay();
	this->GetCommonData().today_gold_shop_cost = 0;
	this->GetCommonData().today_find_reward_flag.Reset();
	memset(this->GetCommonData().today_channel_chat_count_list, 0, sizeof(this->GetCommonData().today_channel_chat_count_list));
	memset(this->GetCommonData().today_activity_type_find_count, 0, sizeof(this->GetCommonData().today_activity_type_find_count));
	this->OnDayChangeTuZiKuaiPao();
	this->GotoSchoolOnDayChange();

	mRoleGatherFb->OnDayChange(old_dayid, now_dayid);
	mPet->OnDayChange(old_dayid, now_dayid);
	mPrestige->OnDayChange(old_dayid, now_dayid);
	mPartner->OnPartnerDayChange(old_dayid, now_dayid);
	mTaskRecorder->OnTaskDayChange(old_dayid, now_dayid);
	mDayReward->OnDayChange(old_dayid, now_dayid);
	mKnapsack->OnDayChange(old_dayid, now_dayid);
	mTaskManager->OnTaskManagerDayChange();
	mSalary->SendDaySaveSalary(old_dayid,now_dayid);
	mRoleConvertShop->OnDayChange(old_dayid, now_dayid);
	mRoleActivityManager->OnDayChange(old_dayid, now_dayid);
	mLoginSystem->OnDayChange(old_dayid, now_dayid);
	mMysteriousTrial->OnDayChange(old_dayid, now_dayid);
	mResearchTask->OnDayChange(old_dayid, now_dayid);
	mMonsterWave->OnDayChange(old_dayid, now_dayid);
	mActiveSystem->OnDayChange(old_dayid, now_dayid);
	this->OnTreviFountainDayChange(old_dayid, now_dayid);
	mTreasureMap->OnDayChange(old_dayid, now_dayid);
	mSchoolTask->OnDayChange(old_dayid, now_dayid);
	mWstQuestion->OnDayChange(old_dayid, now_dayid);
	mEscort->OnDayChange(old_dayid, now_dayid);
	mBountyTask->OnDayChange(old_dayid, now_dayid);
	mTransactionTask->OnDayChange(old_dayid, now_dayid);
	mRoleGatherDream->OndayChange(old_dayid, now_dayid);
	mLevelComplement->OnDayChange(old_dayid, now_dayid);
	mDreamNotesChallenge->OnDayChange(old_dayid, now_dayid);
	this->DayChangeResetGuildData();
	this->OnFantasyDayChange(old_dayid, now_dayid);
	mRoleCrystalFb->OnDayChange(old_dayid, now_dayid);
	mDayReward->SendFindRewardAndNotifysInfo();
	mMiYi->OnDayChange(old_dayid, now_dayid);
	mLunHuiWangChuan->OnDayChange(old_dayid, now_dayid);	
	mRoleMaterial->OnDayChange(old_dayid, now_dayid);
	mRealmBreak->OnDayChange(old_dayid, now_dayid);
	mSmartMountsSystem->OnDayChange(old_dayid, now_dayid);
	mTradeCredit->OnDayChange(old_dayid, now_dayid);
	mRoleJoinActivityReward->OnDayChange();
	mRoleTianDiDaoHen->OnDayChange(old_dayid, now_dayid);
	mPrestigeTask->OnDayChange();
	mShiTu->OnDayChange(old_dayid, now_dayid);
	mDuel->OnDayChange(old_dayid, now_dayid);
	mDuJie->OnDayChange(old_dayid, now_dayid);
	mRoleWanLingFB->OnDayChange(old_dayid, now_dayid);
	mRoleGiveGift->OnDayChange(old_dayid, now_dayid);
	mRoleNewLifeSkill->OnDayChange(old_dayid, now_dayid);
	mFlyUpSystem->OnDayChange();
	mXiuWei->OnDayChange();
	mMirageTerritory->OnDayChange();
	mTreasureTask->OnDayChange();
	mRoleIncome->OnDayChange(old_dayid, now_dayid);
	mRolePeakToken->OnDayChange(old_dayid, now_dayid);
	mRoleNewLevelComplement->OnDayChange(old_dayid, now_dayid);
	mSanXianGuiDong->OnDayChange();
	mLanternRiddie->OnDayChange(old_dayid, now_dayid);
	mConcentricKnot->OnDayChange();
	mTaoQuanNaJi->OnDayChange();
	mRoleWhereIsTheFairy->OnDayChange(old_dayid, now_dayid);
	mRoleTianGongXunBao->OnDayChange(old_dayid, now_dayid);
	mKeJuExam->OnDayChange(old_dayid, now_dayid);
	mLoversMarket->OnDayChange();
	mElementElf->OnDayChange();
	mRoleKuaFuYingXiongLu->OnDayChange(old_dayid, now_dayid);
	mRoleGuildTask->OnDayChange();

	this->OnTopLevelDayChange(old_dayid, now_dayid);
	mNetherWorldPurgatory->OnDayChange();
	
	{
		LOGIC_CONFIG->GetGuildConfig()->ResetBuyCount(BUY_LIMIT_TYPE_DAY, m_guild_common_param.shop_buy_list);
		this->SendGuildShopInfo();
	}
	//关联找回,需要重置
	m_common_data_param.offine_day = 0;
	m_common_data_param.shan_hai_boss_challenge_time = 0;
	m_guild_common_param.guild_explore_flag = 0;
	m_guild_common_param.guild_boss_play_times = 0;
	m_guild_common_param.guild_boss_participate_reward_fetch_flag = 0;
	m_common_data_param.participate_activity_flag.Reset();
	m_guild_common_param.today_guild_qifu_times = 0;
	m_common_data_param.today_quit_guild_count = 0;
	m_common_data_param.equip_refresh_voc_times = 0;
	m_common_data_param.qingyuan_duiduipeng_times = 0;
	m_common_data_param.guild_build_join_speed_up_count = 0;
	m_common_data_param.autority_type_test_day_buy_limit = 0;
	m_common_data_param.today_add_guaji_exp = 0;
	mWorkshop->SendAllWorkshopInfo();
	this->SendQuitGuildInfo();
	this->SendGuaJiExpInfo();

	//每日获得红包数量重置
	memset(m_common_data_param.get_hongbao_day_times, 0, sizeof(m_common_data_param.get_hongbao_day_times));
	this->SendHongBaoInfo();
	this->ReCalcAttr(AttributeList::RECALC_REASON_GUILD_QIFU, __FUNCTION__);//每日刷新要重算祈福属性
}

void RoleModuleManager::OnWeekChange()
{
	this->OnWildBossWeekChange();
	m_common_data_param.shan_hai_boss_week_challenge_time = 0;
	this->BraveGroundOnWeekChange();
	mPrestige->OnWeekChange();
	mTaskRecorder->OnTaskWeekChange();
	mDayReward->OnWeekChange();
	mMiYi->OnWeekChange();
	mFaBao->OnWeekChange();
	mSalary->OnWeekChange();
	mSalary->OnWeekChange(true);
	mRoleChallengeFb->OnWeekChange();
	mRoleConvertShop->OnWeekChange();
	mShiTu->OnWeekChange();
	mRoleActivityManager->OnWeekChange();
	mRoleFirstKill->OnWeekChange();
	mRoleCrystalFb->OnWeekChange();
	mActiveSystem->OnWeekChange();
	mBigDipper->OnWeekChange();
	mFallenGod->OnWeekChange();
	this->WeekChangeResetGuildData();
	mRoleHongMengTianJie->OnWeekChange();

	{
		LOGIC_CONFIG->GetGuildConfig()->ResetBuyCount(BUY_LIMIT_TYPE_WEEK, m_guild_common_param.shop_buy_list);
		this->SendGuildShopInfo();
	}
}

void RoleModuleManager::OnTwoWeekChange(int week_falg)
{
	
}

void RoleModuleManager::OnMonthChange()
{
	mDayReward->OnMonthChange();
	mRoleActivityManager->OnMonthChange();
	mRoleConvertShop->OnMonthChange();
	mHuaShenManager->OnMonthChange();
	mRoleCrystalFb->OnMonthChange();
}

#define SPEED_CALC(reason, classname)\
	if (SPEED_RECALC_REASON_INIT == recalc_reason || reason == recalc_reason)\
	{\
		SPEED_CHANGE_RATIO[reason] = m##classname->GetSpeedChangeRate();\
		GetMax(SPEED_CHANGE_RATIO[reason], -100);\
	}
void RoleModuleManager::ReCalcSpeed(int recalc_reason, const char * recalc_source, bool is_login)
{
	static int SPEED_CHANGE_RATIO[SPEED_RECALC_REASON_MAX] = { 0 };		// 正数表示加速，负数表示减速，0 表示无修正；

	{
		SPEED_CALC(SPEED_RECALC_REASON_BUFF_MGR, BuffManager)
		SPEED_CALC(SPEED_RECALC_REASON_ESCORT, Escort)
	}

	/* =============================================================================== */
	int speed_ratio = 100;		// 速率基础倍率（百分比）；
	for (int reason_type = 0; reason_type < SPEED_RECALC_REASON_MAX; ++reason_type)
	{
		speed_ratio *= ((100 + SPEED_CHANGE_RATIO[reason_type]) / 100.0);
	}

	speed_ratio -= 100;			// 减去基础倍率
	m_role->SetSpeedRate(speed_ratio);
	if (is_login || SPEED_RECALC_REASON_INIT != recalc_reason) this->SendMoveSpeedChangeRate();
}

void RoleModuleManager::SendMoveSpeedChangeRate()
{
	Protocol::SCRoleSpeedChangeNotice notice;
	notice.obj_id = m_role->GetId();
	notice.move_speed = m_role->GetSpeedRate();
	if (NULL != m_role->GetScene())
	{
		m_role->GetScene()->GetZoneMatrix()->NotifyAreaMsg(m_role->GetObserHandle(), &notice, sizeof(notice));
	}
}

void RoleModuleManager::SendSpecialNotice(int op_type, int param1, int param2, int param3, int param4, int param5)
{
	if(op_type < 0 || op_type >= Protocol::SCSpecialNotice::SPECIAL_NOTICE_TYPE_MAX) return;

	Protocol::SCSpecialNotice info;
	info.op_type = op_type;
	info.param1 = param1;
	info.param2 = param2;
	info.param3 = param3;
	info.param4 = param4;
	info.param5 = param5;

	EngineAdapter::Instance().NetSend(this->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void RoleModuleManager::ReCalcAttr(int recalc_reason, const char* recalc_reason_str, bool is_init /*= false*/,int send_notice)
{
	int total_cap_before = this->GetCapability()->GetRoleCapability();

	bool recalc_all = (AttributeList::RECALC_REASON_INVALID == recalc_reason || is_init);

	Attribute cur_hp = m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP];
	Attribute cur_mp = m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP];
	if (is_init)
	{
		cur_hp = m_common_data_param.curhp_in_scene;
		cur_mp = m_common_data_param.curmp_in_scene;
	}

	//初始化
	m_attr_list.Reset();
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		m_attr_reason_list[i].Reset();
	}

	AttributeList skill_attr_list;			//某些技能是直接配的评分,则计算评分时,属性列表应该剔除这些技能所增加的属性
	{
		//PS: 如果有其他同时会出现小数数值的系统，需要一同计算，否则会出现误差
		//计算自身属性
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_BASE] = this->ReCalcSelfAttr(m_attr_list, AttributeList::RECALC_REASON_SELF == recalc_reason || recalc_all);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_ZAO_HUA].Add(mZaoHuaEquipManager->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_ZAO_HUA == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_POINT] = mRoleAttributePlan->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_ATTRIBUTE_PLAN == recalc_reason || recalc_all);
		const AttributeList basic_attr_list = m_attr_list;		// 自身属性表

		// 各个系统的加成
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_COLLE] = mCollection->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_COLLECTION == recalc_reason || recalc_all);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mTitle->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_TITLE == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mPartner->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_PARTNER == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mSurface->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_SURFACE == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mBaby->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_BABY == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_EQUIP].Add(mEquipmentManager->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_EQUIPMENT == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_FABAO].Add(mFaBao->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_FABAO == recalc_reason || recalc_all ||
			AttributeList::RECALC_REASON_SELF == recalc_reason).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_MEDAL] = mMedal->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_MEDAL == recalc_reason || recalc_all);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mFaceScore->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_FACE_SCORE == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_ADV] = this->ReCalcAttrProf(m_attr_list, AttributeList::RECALC_REASON_PROF_ADVAN == recalc_reason || recalc_all);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_SMS] = mSmartMountsSystem->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM == recalc_reason || recalc_all, skill_attr_list);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mRoleGuildInborn->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_GUILD_INBORN == recalc_reason || recalc_all, skill_attr_list).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mRoleHeartSkill->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_HEART_SKILL == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_PELLET].Add(mRoleAttrPellet->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_ATTR_PELLET == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_XIU_WEI].Add(mXiuWei->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_XIU_WEI == recalc_reason || recalc_all).m_attrs);	
 		m_attr_reason_list[ROLE_REASON_LIST_TYPE_HOLY_EQUIP].Add(mHolyEquipManager->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_HOLY_EQUIP == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_STAR_CHART].Add(mStarChart->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_STAR_CHART == recalc_reason || recalc_all).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mFashionCollection->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_FASHION_COLLECTION == recalc_reason || recalc_all).m_attrs);	// 外观升级(原幻化系统)
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_HUANSHOU].Add(mRoleHuanShou->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_HUAN_SHOU == recalc_reason || recalc_all).m_attrs);	

		// 角色技能放最后（百分比之前的最后一个计算，因为有被动技能是要按其他属性计算某个属性的值）
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_TRAIN].Add(mTrain->ReCalcAttr(m_attr_list, true).m_attrs);			//需要防御值计算，所以每次都需要重新计算一遍
		//原本人物被动技能仍需参与评分计算(星术师不在此处计算)
		if (PROF_TYPE_MAGICIAN != this->GetRole()->GetBaseProfession())
		{
			m_attr_reason_list[ROLE_REASON_LIST_TYPE_BASE].Add(mRoleSkill->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_ROLE_SKILL == recalc_reason || recalc_all).m_attrs);
		}
		///////////////////////////////////这后面就是百分比提升的属性了/////////////////////////////////////////////////
		const AttributeList copy_attr_list = m_attr_list;		// 总属性表

		// 只计算自身属性百分比加成的属性
		mEquipmentManager->ReCalcAttrPer(m_attr_list, basic_attr_list);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_FLY_UP].Add(mFlyUpSystem->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_FLY_UP_SYSTEM == recalc_reason || recalc_all).m_attrs);

		// 计算了各系统加成的百分比属性
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(m_role->GetScene()->GetSpecialLogic()->ReCalcAttrPer(m_attr_list, copy_attr_list).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_TRAIN].Add(mTrain->ReCalcAttrPer(m_attr_list, copy_attr_list).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_FABAO].Add(mFaBao->ReCalcAttrPer(m_attr_list, copy_attr_list).m_attrs);
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_ACCESSORIES].Add(mAccessoriesmentManager->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_ACCESSORIES == recalc_reason || recalc_all).m_attrs);

		m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(this->ReCalcAttrGuildQiFu(m_attr_list, AttributeList::RECALC_REASON_GUILD_QIFU == recalc_reason || recalc_all).m_attrs);


		for (int i = ROLE_REASON_LIST_TYPE_MIN; i < ROLE_REASON_LIST_TYPE_MAX; ++i)
		{
			LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute2(m_attr_reason_list[i]);
		}
	}

	LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute2(m_attr_list);

	//杀戮值按百分比降属性
	const DuelKillCfg* cfg = LOGIC_CONFIG->GetDuelConfig()->GetKillCfg(this->GetDuel()->GetKillValue());
	if (NULL != cfg)
	{
		double mutile = (100 - cfg->attr_down) / 100.0;

		m_attr_list.m_attrs[BATTLE_ATTR_ATTACK] *= mutile;
		m_attr_list.m_attrs[BATTLE_ATTR_DEFENSE] *= mutile;
		m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] *= mutile;
		m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] *= mutile;
		m_attr_list.m_attrs[BATTLE_ATTR_AGILITY] *= mutile;
		m_attr_list.m_attrs[BATTLE_ATTR_MENTAL] *= mutile;
		m_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] *= mutile;
	}
	
	if (m_role->GetSceneStatus()->IsInAutoRecoverScene())
	{
		m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP];
		m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP];
	}
	else
	{
		m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = GetMin(cur_hp, m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP]);
		m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = GetMax(1, m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP]);

		m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = GetMin(cur_mp, m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP]);
		m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = GetMax(0, m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP]);
	}
	
	AttributeList no_has_skill_attr_list = m_attr_list;
	//no_has_skill_attr_list.Deduct(skill_attr_list.m_attrs);
	mCapability->GetAttrListParam(no_has_skill_attr_list);
	EventHandler::Instance().OnRecaleReason(m_role, recalc_reason, is_init);

	memcpy(m_common_data_param.user_attr, m_attr_list.m_attrs, sizeof(m_common_data_param.user_attr));
	int cur_total_cap = this->GetCapability()->GetRoleCapability();
	int max_capability = this->GetCapability()->GetRoleMaxCapability();
	{
		gamelog::g_log_capability.printf(LL_INFO, "RoleModuleManager::ReCalcAttr user[%d,%s] reason[%d] reason_str[%s] is_init[%d] cur_capacity[%d] old_cap[%d] max_ cap[%d] change_cap[%d,%s]\
			normal:{ gongji:%d fangyu:%d hp:%d mp:%d minjie:%d jingshen:%d huifu:%d } \
			corrected:{ bisha:%d mingzhong:%d shanbi:%d fanji:%d mogong:%d mofang:%d bishazhi:%d jianrenzhi:%d} \
			anti:{ zhongdu:%d shuimian:%d shihua:%d hunluan:%d jiouzui:%d}", 
			this->GetRole()->GetUID(), this->GetRole()->GetName(), recalc_reason, (NULL == recalc_reason_str ? "nil" : recalc_reason_str),
			1 == is_init, cur_total_cap, total_cap_before, max_capability, cur_total_cap - total_cap_before, cur_total_cap < total_cap_before ? "down" : "up",
			m_attr_list.m_attrs[BATTLE_ATTR_ATTACK], m_attr_list.m_attrs[BATTLE_ATTR_DEFENSE], m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP], m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP],
			m_attr_list.m_attrs[BATTLE_ATTR_AGILITY], m_attr_list.m_attrs[BATTLE_ATTR_MENTAL], m_attr_list.m_attrs[BATTLE_ATTR_RECOVERY],
			m_attr_list.m_attrs[BATTLE_ATTR_CRITICAL], m_attr_list.m_attrs[BATTLE_ATTR_HIT], m_attr_list.m_attrs[BATTLE_ATTR_DODGE],
			m_attr_list.m_attrs[BATTLE_ATTR_COUNTER_ATTACK], m_attr_list.m_attrs[BATTLE_ATTR_MAGIC_ATTACK], m_attr_list.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE], 
			m_attr_list.m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE], m_attr_list.m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE],
			m_attr_list.m_attrs[BATTLE_ATTR_ANTI_POISON], m_attr_list.m_attrs[BATTLE_ATTR_ANTI_FREEZE], m_attr_list.m_attrs[BATTLE_ATTR_ANTI_PETRIFY],
			m_attr_list.m_attrs[BATTLE_ATTR_ANTI_CHAOS], m_attr_list.m_attrs[BATTLE_ATTR_ANTI_DRUNK]);

	}

	if (AttributeList::RECALC_REASON_INVALID != recalc_reason && cur_total_cap != total_cap_before)  //只记录玩家战力下降与战力大幅增长的情况
	{
		ROLE_LOG_QUICK10(LOG_TYPE_CAPABILITY_CHANGE, this->GetRole(), cur_total_cap - total_cap_before, cur_total_cap, recalc_reason_str, NULL, recalc_reason, 0, 0, 0, 0);
	}

	//BUFF会影响评分,所以buff最后进行计算 放在log后面是怕影响到属性数据与战力不一致
	m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mBuffManager->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_BUFF == recalc_reason || recalc_all).m_attrs);
	m_attr_reason_list[ROLE_REASON_LIST_TYPE_OTHER].Add(mRoleNewLifeSkill->ReCalcAttr(m_attr_list,  AttributeList::RECALC_REASON_LIFE_SKILL_PROPS == recalc_reason || recalc_all).m_attrs);


	//星术师人物被动技能不参与评分计算(转换属性修正)
	if (PROF_TYPE_MAGICIAN == this->GetRole()->GetBaseProfession())
	{
		m_attr_reason_list[ROLE_REASON_LIST_TYPE_BASE].Add(mRoleSkill->ReCalcAttr(m_attr_list, AttributeList::RECALC_REASON_ROLE_SKILL == recalc_reason || recalc_all).m_attrs);
	}

	//发送面板属性变化
	if (!is_init)
	{
		this->SendAttributeList(send_notice);
		this->GetTrain()->SendPromotedInfo();
	}

	this->ScoreTaskFinish();
}

const AttributeList & RoleModuleManager::ReCalcSelfAttr(AttributeList &base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_base_attr.Reset();

		const RoleBaseAttrCfg& cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetRoleBaseAttrCfg();

		m_base_attr.m_attrs[BATTLE_ATTR_ATTACK]		+= cfg.attack;
		m_base_attr.m_attrs[BATTLE_ATTR_DEFENSE]	+= cfg.defence;
		m_base_attr.m_attrs[BATTLE_ATTR_MAX_HP]		+= cfg.maxhp;
		m_base_attr.m_attrs[BATTLE_ATTR_MAX_MP]		+= cfg.maxmp;
		m_base_attr.m_attrs[BATTLE_ATTR_AGILITY]	+= cfg.agility;
		m_base_attr.m_attrs[BATTLE_ATTR_MENTAL]		+= cfg.mentality;
		m_base_attr.m_attrs[BATTLE_ATTR_RECOVERY]	+= cfg.recovery;
	}

	base_add.Add(m_base_attr.m_attrs);
	return m_base_attr;
}

const AttributeList & RoleModuleManager::ReCalcAttrProf(AttributeList & base_add, bool is_recalc)
{
 	if (is_recalc)
	{
		m_base_attr_prof.Reset();
		const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(this->GetRole()->GetProfession() , mAdvanceCareer->GetSubLevel());
		if (NULL == cfg)
		{
			return m_base_attr_prof;
		}
		m_base_attr_prof.Add(cfg->m_attrs);
	}
	base_add.Add(m_base_attr_prof.m_attrs);
	return m_base_attr_prof;
}

const AttributeList & RoleModuleManager::ReCalcAttrGuildQiFu(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc) 
	{
		m_attrs_guild_qifu.Reset();
		Guild * m_guild = m_role->GetGuild();
		if (NULL != m_guild)
		{
			if (m_guild->IsOpenQiFu())
			{
				GuildMemberManager*  m_member_manager = m_guild->GetMemberManager();
				if (NULL != m_member_manager)
				{
					GuildMember *m_member = m_member_manager->GetMember(m_role->GetUserId());
					if (NULL != m_member)
					{
						if (m_member->GetQiFuFlag() == 1)
						{
							//家族祈福属性
							int world_level = WorldStatus::Instance().GetWorldLevel();
							const BlessTimesAttr * bless_attr = LOGIC_CONFIG->GetGuildConfig()->GetBlessAttrCfg(world_level);
							if (NULL != bless_attr)
							{
								for (int bless_idx = 0; bless_idx < BlessTimesAttr::MAX_ATTR_LIST_NUM; bless_idx++)
								{
									if (bless_attr->attr_list[bless_idx].attr_value > 0)
									{
										if (bless_attr->attr_list[bless_idx].attr_type >= BATTLE_ATTR_NORMAL_VALUE_BEGIN && bless_attr->attr_list[bless_idx].attr_type <= BATTLE_ATTR_ANTI_VALUE_END)
										{
											m_attrs_guild_qifu.m_attrs[bless_attr->attr_list[bless_idx].attr_type] += bless_attr->attr_list[bless_idx].attr_value;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	base_add.Add(m_attrs_guild_qifu.m_attrs);
	return m_attrs_guild_qifu;
}

void RoleModuleManager::SendAttrListReason(short param1)
{
	Protocol::SCAttrListReason info; 
	info.reason_type = Protocol::SCAttrListReason::REASON_TYPE_ROLE;
	info.param1 = param1;
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		info.reason_list[i] = m_attr_reason_list[i];
	}

	this->NetSend(&info, sizeof(info));
}

void RoleModuleManager::SendAttributeList(short reason /*= 0*/)
{
	static Protocol::SCRoleAttrList msg;
	msg.attr_count = 0;
	msg.notify_reason = reason;
	msg.capability = this->GetCapability()->GetRoleCapability();
	static Protocol::SCRoleAttrList::AttrPair p;
	for (int i = BATTLE_ATTR_NORMAL_VALUE_BEGIN; i < BATTLE_ATTR_NORMAL_VALUE_END && msg.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		p.attr_type = i;
		p.attr_value = m_attr_list.m_attrs[i];
		msg.attr_list[msg.attr_count++] = p;
	}

	for (int i = BATTLE_ATTR_CORRECTED_VALUE_BEGIN; i < BATTLE_ATTR_CORRECTED_VALUE_END && msg.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		p.attr_type = i;
		p.attr_value = m_attr_list.m_attrs[i];
		msg.attr_list[msg.attr_count++] = p;
	}

	for (int i = BATTLE_ATTR_ANTI_VALUE_BEGIN; i < BATTLE_ATTR_ANTI_VALUE_END && msg.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		p.attr_type = i;
		p.attr_value = m_attr_list.m_attrs[i];
		msg.attr_list[msg.attr_count++] = p;
	}

	for (int i = BATTLE_ATTR_ELEMENT_BEGIN; i < BATTLE_ATTR_ELEMENT_END && msg.attr_count < BATTLE_ATTR_MAX; ++i)
	{
		p.attr_type = i;
		p.attr_value = m_attr_list.m_attrs[i];
		msg.attr_list[msg.attr_count++] = p;
	}

	int send_len = sizeof(msg) - (BATTLE_ATTR_MAX - msg.attr_count) * sizeof(Protocol::SCRoleAttrList::AttrPair);
	EngineAdapter::Instance().NetSend(this->GetRole()->GetNetId(), (const char*)&msg, send_len);
}

void RoleModuleManager::SendSingleAttribute(int attr_tpye, short reason)
{
	Protocol::SCRoleAttrList msg;
	msg.attr_count = 0;
	msg.notify_reason = reason;
	msg.capability = this->GetCapability()->GetRoleCapability();
	if (attr_tpye < BATTLE_ATTR_MIN || attr_tpye >= BATTLE_ATTR_MAX)
	{
		return;
	}

	Protocol::SCRoleAttrList::AttrPair p;
	p.attr_type = attr_tpye;
	p.attr_value = m_attr_list.m_attrs[attr_tpye];
	msg.attr_list[msg.attr_count++] = p;

	int send_len = sizeof(msg) - (BATTLE_ATTR_MAX - msg.attr_count) * sizeof(Protocol::SCRoleAttrList::AttrPair);
	EngineAdapter::Instance().NetSend(this->GetRole()->GetNetId(), (const char*)&msg, send_len);
}

void RoleModuleManager::OnChangeProfession(int old_prof, int new_prof)
{
	mZaoHuaEquipManager->ChangeProfession(old_prof, new_prof);
	mHolyEquipManager->ChangeProfession(old_prof, new_prof);
	mStarChart->ChangeProfession(old_prof, new_prof);
	mFashionCollection->ChangeProfession(old_prof, new_prof);
}

Attribute RoleModuleManager::GetAttrByType(int type)
{
	return m_attr_list.GetAttr(type);
}

void RoleModuleManager::GmSetAttr(int type, int value)
{
	if (type < 0 || type >= BATTLE_ATTR_MAX) return;

	m_common_data_param.gm_attr_list[type] = value;
}

void RoleModuleManager::SetCurHpMp(int curhp, int curmp)
{
	m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP] = curhp;
	m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP] = curmp;

	{
		static Protocol::SCRoleAttrList msg;
		msg.attr_count = 0;
		msg.notify_reason = Protocol::SCRoleAttrList::NOTIFY_REASON_SYNC_HPMP_IN_FB;
		msg.capability = this->GetCapability()->GetRoleCapability();
		msg.attr_list[msg.attr_count].attr_type = BATTLE_ATTR_CUR_HP;
		msg.attr_list[msg.attr_count].attr_value = m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP];
		++msg.attr_count;

		msg.attr_list[msg.attr_count].attr_type = BATTLE_ATTR_CUR_MP;
		msg.attr_list[msg.attr_count].attr_value = m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP];
		++msg.attr_count;

		int send_len = sizeof(msg) - (BATTLE_ATTR_MAX - msg.attr_count) * sizeof(Protocol::SCRoleAttrList::AttrPair);
		EngineAdapter::Instance().NetSend(this->GetRole()->GetNetId(), (const char*)&msg, send_len);
	}
	
	this->SendHpMpInfoToTeam();
}

bool RoleModuleManager::ChangeHp(int change_vale, bool is_percent)
{
	if (is_percent)
	{
		change_vale = (int)(1.0 * m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] * change_vale / BATTLE_PERCENT_NUM);
	}
	return AddAttr(BATTLE_ATTR_CUR_HP, change_vale, m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP]);
}

bool RoleModuleManager::ChangeMp(int change_vale, bool is_percent)
{
	if (is_percent)
	{
		change_vale = (int)(1.0 * m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] * change_vale / BATTLE_PERCENT_NUM);
	}
	return AddAttr(BATTLE_ATTR_CUR_MP, change_vale, m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP]);
}

void RoleModuleManager::SendHpMpInfoToTeam()
{
	Team* team = m_role->GetMyTeam();
	if (NULL != team)
	{
		Attribute cur_hp = m_attr_list.m_attrs[BATTLE_ATTR_CUR_HP];
		Attribute max_hp = m_attr_list.m_attrs[BATTLE_ATTR_MAX_HP];
		Attribute cur_mp = m_attr_list.m_attrs[BATTLE_ATTR_CUR_MP];
		Attribute max_mp = m_attr_list.m_attrs[BATTLE_ATTR_MAX_MP];

		Protocol::SCUpdateMemberInfo msg;
		msg.uid = m_role->GetUID();
		msg.is_partner = 0;
		msg.hp = cur_hp;
		msg.max_hp = max_hp;
		msg.mp = cur_mp;
		msg.max_mp = max_mp;
		msg.skin_id = 0;
		msg.level = m_role->GetLevel();

		team->SendToMember(&msg, sizeof(msg));
	}
}

bool RoleModuleManager::AddAttr(int type, Attribute value, Attribute max_value)
{
	if (type < 0 || type >= BATTLE_ATTR_MAX || value == 0)
	{
		return false;
	}

	if (value > 0 && m_attr_list.m_attrs[type] >= max_value)
	{
		return false;
	}

	IntHelpAdd(m_attr_list.m_attrs[type], max_value, value);

	this->SendSingleAttribute(type);

	if (BATTLE_ATTR_CUR_HP == type || BATTLE_ATTR_CUR_MP == type)
	{
		this->SendHpMpInfoToTeam();
	}
	
	return true;
}

void RoleModuleManager::Recover()
{
	BattleAttr max_hp = this->GetAttrByType(BATTLE_ATTR_MAX_HP);
	BattleAttr max_mp = this->GetAttrByType(BATTLE_ATTR_MAX_MP);

	this->SetCurHpMp(max_hp, max_mp);
}

void RoleModuleManager::OnLoginCalOfflineDay(unsigned int login_timestamp, unsigned int last_save_timestamp)
{
	if(login_timestamp <= 0 || last_save_timestamp <= 0 || login_timestamp <= last_save_timestamp) return;

	int next_day_zero_hour = EngineAdapter::Instance().NextDayInterval(0, 0, 0);
	long long today_zero_hour_time = login_timestamp - (SECOND_PER_DAY - next_day_zero_hour);
	if (last_save_timestamp >= today_zero_hour_time)
	{
		m_common_data_param.offine_day = 0;
	}
	else if (last_save_timestamp + SECOND_PER_DAY >= today_zero_hour_time)
	{
		m_common_data_param.offine_day = 1;
	}
	else
	{
		m_common_data_param.offine_day = (today_zero_hour_time - last_save_timestamp) / SECOND_PER_DAY + 1;
	}
	return;
}

void RoleModuleManager::OnLoginCalCreateDay()
{
	long long now_timestamp = (long long)EngineAdapter::Instance().Time();
	if(now_timestamp < 0) return;

	int next_day_zero_hour = EngineAdapter::Instance().NextDayInterval(0, 0, 0);
	long long today_zero_hour_time = now_timestamp - (SECOND_PER_DAY - next_day_zero_hour);
	long long crate_time = m_role->GetRoleCreateTime();
	if (today_zero_hour_time <= crate_time)
	{
		m_common_data_param.role_create_count_day = 1;
	}
	else
	{
		int add_day = (((today_zero_hour_time - crate_time) % SECOND_PER_DAY) == 0) ? 1 : 2;
		m_common_data_param.role_create_count_day = (today_zero_hour_time - crate_time) / SECOND_PER_DAY + add_day;
	}

	gamelog::g_log_debug.printf(LL_INFO, "RoleModuleManager::OnLoginCalCreateDay role[%d,%s] create_days[%d]", this->GetRoleUID(), this->GetRoleName(), m_common_data_param.role_create_count_day);
}

void RoleModuleManager::OnRoleLogin()
{
	this->CheckZhouQiInfo();
	if (0 == m_common_data_param.is_brave_ground_reset_flag)
	{
		this->OnBraveGroundResetByFlag();
		m_common_data_param.is_brave_ground_reset_flag = 1;
	}
	if (0 == m_common_data_param.is_set_auto_talk_groups_flag)
	{
		m_common_data_param.is_set_auto_talk_groups_flag = 1;
		if (m_common_data_param.role_create_count_day <= 1 && this->GetRole()->GetLevel() < ROBOTCONFIG->GetOtherCfg().off_auto_talk_level)//新号且小于等级
		{
			//只会触发一次,所以玩家只会在本服被触发到,此时哈希值相同
			m_common_data_param.auto_talk_groups_id = ROBOTCONFIG->GetRandAutoGroupsId(GetStringHash(this->GetRole()->GetPlatName()), this->GetRole()->GetAccountUserNum());
			m_common_data_param.auto_talk_id = 1;
			this->SendAutoTalkInfo(false);
		}
	}
	if (this->GetRole()->GetLevel() < ROBOTCONFIG->GetOtherCfg().off_auto_talk_level)
	{
		const AutoTalkCfg * auto_talk_cfg = ROBOTCONFIG->GetAutoTalkCfg(m_common_data_param.auto_talk_groups_id, m_common_data_param.auto_talk_id);
		if (NULL != auto_talk_cfg)
		{
			m_next_send_auto_talk_timestamp = (unsigned int)EngineAdapter::Instance().Time() + auto_talk_cfg->interval;
		}
	}
	if (1 != m_common_data_param.is_again_set_top_level_refresh_time_flag)
	{
		m_common_data_param.is_again_set_top_level_refresh_time_flag = 1;
		unsigned int world_refresh_timestamp = WorldStatus::Instance().GetTopLevelRefreshTimestamp();
		if (0 != this->GetRole()->GetNextTopLevelRefreshTimestamp())
		{
			if (world_refresh_timestamp == TOP_LEVEL_REFRESH_BASE_TIMESTAMP)
			{
				this->GetRole()->SetTopLevelNextRefreshTimestamp(world_refresh_timestamp);
			}
			else
			{
				this->GetRole()->SetTopLevelNextRefreshTimestamp(1);		//玩家需要先重置
			}
			this->SendTopLevelAllInfo();
		}
	}
	//已完成一次寻宝指引则不在范围中
	if (0 == m_common_data_param.is_old_user_first_manual_pet_xun_bao_flag)
	{
		if (this->GetFuncGuide()->IsDrawGuidePet())
		{
			m_common_data_param.is_first_manual_pet_xun_bao_flag = 1;
		}
		m_common_data_param.is_old_user_first_manual_pet_xun_bao_flag = 1;
	}

	this->SendGuaJiExpInfo();
	this->SendRealNameRewardInfo();

	mTianMing->OnLogin();
	mAdvanceCareer->OnLogin();
	mXiuWei->OnLogin();
	mMirageTerritory->OnRoleLogin();
	mPet->OnRoleLogin();
	mRoleNewLifeSkill->OnRoleLogin();
	mDreamNotesChallenge->OnRoleLogin();
	mRoleIncome->OnRoleLogin();
	mLanternRiddie->OnLogin();
	mRoleTianGongXunBao->OnRoleLogin();
	mZaoHuaEquipManager->OnLogin();
	mHolyEquipManager->OnRoleLogin();
	mKeJuExam->OnLogin();
	mFuYuManager->OnRoleLogin();
	mRoleKuaFuYingXiongLu->OnRoleLogin();
	mStarChart->OnRoleLogin();
	mFashionCollection->OnRoleLogin();
	mRoleHuanShou->OnRoleLogin();
}

void RoleModuleManager::OnAutoTalkUpdate(unsigned int now_second)
{
	if(this->GetRole()->GetLevel() >= ROBOTCONFIG->GetOtherCfg().off_auto_talk_level || 0 == m_next_send_auto_talk_timestamp) return;

	if (m_next_send_auto_talk_timestamp <= now_second)
	{
		m_common_data_param.auto_talk_id += 1;
		const AutoTalkCfg * auto_talk_cfg = ROBOTCONFIG->GetAutoTalkCfg(m_common_data_param.auto_talk_groups_id, m_common_data_param.auto_talk_id);
		if (NULL == auto_talk_cfg)	//已经没有后续对话了
		{
			m_next_send_auto_talk_timestamp = 0;
			return;
		}
		m_next_send_auto_talk_timestamp = now_second + auto_talk_cfg->interval;
		this->SendAutoTalkInfo(false);
	}
}

void RoleModuleManager::SendAutoTalkInfo(bool is_init)
{
	Protocol::SCRoleAutoTalkInfo info;
	info.notice_type = is_init ? Protocol::SCRoleAutoTalkInfo::ROLE_AUTO_TALK_NOTICE_TYPE_INIT : Protocol::SCRoleAutoTalkInfo::ROLE_AUTO_TALK_NOTICE_TYPE_SEND;
	info.talk_groups_id = m_common_data_param.auto_talk_groups_id;
	info.talk_id = m_common_data_param.auto_talk_id;

	this->NetSend(&info, sizeof(info));
}

void RoleModuleManager::ChangeProfSendItem(int old_prof, int cur_prof)
{
	static MailContentParam contentparam; contentparam.Reset();

	int count = 0;
	const CareerAdvanceGiveItemCfg * item_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceGiveItemCfg(this->GetRole()->GetBaseProfession(), this->GetRole()->GetLevel());
	if (NULL != item_cfg)
	{
		for (int i = 0; count < MAX_ATTACHMENT_ITEM_NUM && i < (int)item_cfg->change_prof_item.size(); i++)
		{
			contentparam.item_list[count].item_id = item_cfg->change_prof_item[i].item_id;
			contentparam.item_list[count].is_bind = item_cfg->change_prof_item[i].is_bind;
			contentparam.item_list[count].num = item_cfg->change_prof_item[i].num;
			count++;
		}
	}

	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_mail_change_prof_subject);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_mail_change_prof_content, "");

	// 返还玩家已升级技能的所有铜币
	SkillListParam sp;
	this->GetRoleSkill()->GetSkillList(&sp);

	Int64 back_money = 0;
	for (int idx = 0; idx < sp.count; ++idx)
	{
		RoleSkill::SkillItem *skill = this->GetRoleSkill()->GetSkill(sp.skill_list[idx].index);
		if (skill == NULL) continue;

		const SkillCfg *skill_cfg = SkillPool::Instance().GetSkillCfg(skill->skill_id); 
		if (skill_cfg == NULL) continue;

		//0->1不需要消耗铜币,默认返还到1级
		back_money += SkillPool::Instance().GetSkillGoBackRetCoin(skill->skill_id, skill->skill_level, skill->skill_exp, old_prof, 1);

		skill->skill_exp = 0;
		skill->skill_level = 0;
		skill->skill_show_level = 0;
		skill->skill_id = -1;
		sp.skill_list[idx].skill_id = -1;

		this->GetRoleSkill()->SetDirty(sp.skill_list[idx].index, false);
	}
	if (back_money > 0)
	{
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = back_money;
	}

	int train_uplevel_comsume_item_num = this->GetCommonData().train_uplevel_comsume_item_num;
	if (train_uplevel_comsume_item_num > 0)
	{
		gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] train_uplevel_comsume_item_num:%d", __FUNCTION__, __LINE__,
			this->GetRole()->GetUID(), this->GetRole()->GetName(), train_uplevel_comsume_item_num);

		const static int SINGLE_ITEM_MAX_NUM = 30000;
		int need_num = (train_uplevel_comsume_item_num + SINGLE_ITEM_MAX_NUM - 1) / SINGLE_ITEM_MAX_NUM;

		if (count >= MAX_ATTACHMENT_ITEM_NUM)
		{
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(IntToUid(this->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
			}

			contentparam.Reset();
			count = 0;
		}

		for (int i = 0; i < need_num && count < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			int real_num = train_uplevel_comsume_item_num > SINGLE_ITEM_MAX_NUM ? SINGLE_ITEM_MAX_NUM : train_uplevel_comsume_item_num;
			train_uplevel_comsume_item_num -= SINGLE_ITEM_MAX_NUM;

			gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] real_num:%d", __FUNCTION__, __LINE__,
				this->GetRole()->GetUID(), this->GetRole()->GetName(), real_num);

			//返回修炼的物品
			const TrainOtherCfg & other_cfg = LOGIC_CONFIG->GetTrainConfig()->GetOtherCfg();
			contentparam.item_list[count].item_id = other_cfg.item_ret;
			contentparam.item_list[count].is_bind = 1;
			contentparam.item_list[count].num = real_num;
			count += 1;

			if (count >= MAX_ATTACHMENT_ITEM_NUM)
			{
				if (length1 > 0 && length2 > 0)
				{
					MailRoute::MailToUser(IntToUid(this->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
				}

				contentparam.Reset();
				count = 0;
			}
		}

		if (count > 0)
		{
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(IntToUid(this->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
			}

			contentparam.Reset();
			count = 0;
		}

		this->GetCommonData().train_uplevel_comsume_item_num = 0;
	}
	else
	{
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(IntToUid(this->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}


int RoleModuleManager::GetUid()
{
	return m_role->GetUID();
}

void RoleModuleManager::OnAddGuildEvent(int param1, int param2, int type)
{
	Guild * guild = this->GetRole()->GetGuild();
	if(NULL == guild) return;

	switch (type)
	{
	case GUILD_EVENTHANDLER_TYPE_XUNBAO:
	case GUILD_EVENTHANDLER_TYPE_ADD_PET:
		{
			const PetCfg *cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(param1);
			if(NULL == cfg) return;

			const GuildJournalCfg * journal_cfg = LOGIC_CONFIG->GetGuildConfig()->GetJournalCfg(type);
			if(NULL == journal_cfg) return;

			if (cfg->quality >= journal_cfg->condition)
			{
				guild->AddEventhandler(this->GetRole()->GetName(), EngineAdapter::Instance().Time(), type, param1);     // param1 - 宠物id
			}
		}
		break;
	case GUILD_EVENTHANDLER_TYPE_COURSE_OF_GROWTH:
		{
			const GuildJournalCfg * journal_cfg = LOGIC_CONFIG->GetGuildConfig()->GetJournalCfg(type);
			if(NULL == journal_cfg) return;

			if (param1 >= journal_cfg->condition && param2 == 1)
			{
				guild->AddEventhandler(this->GetRole()->GetName(), EngineAdapter::Instance().Time(), type, param1);   // param1 - 成就阶级
			}
		}
		break;
	case GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND:	
		{
			const ItemBase * item = ITEMPOOL->GetItem(param1);
			if(NULL == item || item->GetItemType() != I_TYPE_EQUIPMENT) return;

			const GuildJournalCfg * journal_cfg = LOGIC_CONFIG->GetGuildConfig()->GetJournalCfg(type);
			if (NULL == journal_cfg) return;

			if (item->GetLimitLevel() >= journal_cfg->condition)
			{
				guild->AddEventhandler(this->GetRole()->GetName(), EngineAdapter::Instance().Time(), type, param1);  //param1 - 物品ID
			}
		}
		break;
	case GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY:
		{
			const ItemBase * item = ITEMPOOL->GetItem(param1);
			if (NULL == item || item->GetItemType() != I_TYPE_JEWELRY) return;

			const GuildJournalCfg * journal_cfg = LOGIC_CONFIG->GetGuildConfig()->GetJournalCfg(type);
			if (NULL == journal_cfg) return;

			if (item->GetLimitLevel() >= journal_cfg->condition)
			{
				guild->AddEventhandler(this->GetRole()->GetName(), EngineAdapter::Instance().Time(), type, param1);  //param1 - 物品ID
			}
		}
		break;
	case GUILD_EVENTHANDLER_TYPE_MIYI:		
		{			
			if (this->GetMiYi()->CheckLayerFinish(param1))			//param1 当前层数 第一层即 0
			{
				const GuildJournalCfg * journal_cfg = LOGIC_CONFIG->GetGuildConfig()->GetJournalCfg(type);
				if (NULL == journal_cfg) return;
				if (param1 + 1 >= journal_cfg->condition)
				{
					guild->AddEventhandler(this->GetRole()->GetName(), EngineAdapter::Instance().Time(), type, param1 + 1);  //param1 - 层数 
				}
			}
		}
		break;
	case GUILD_EVENTHANDLER_TYPE_COURAGE_CHALLENGE:
		{
			const GuildJournalCfg * journal_cfg = LOGIC_CONFIG->GetGuildConfig()->GetJournalCfg(type);
			if (NULL == journal_cfg) return;

			if (param1 >= journal_cfg->condition && param1 > GUILD_JOURNAL_ADD_NUM)
			{
				int num = param1 - journal_cfg->condition;
				if (num % GUILD_JOURNAL_ADD_NUM == 0)
				{
					guild->AddEventhandler(this->GetRole()->GetName(), EngineAdapter::Instance().Time(), type, param1);  //param1 - 层数 
				}
			}	
		}
		break;
	default:
		break;
	}
}

void RoleModuleManager::DayChangeResetGuildData()
{	
	Guild * guild = m_role->GetGuild();
	if (NULL != guild)
	{
		if (this->GetCommonData().fetch_boss_chapter_rank_reward_day_id != EngineAdapter::Instance().DayID())
		{
			this->GetCommonData().is_fetch_boss_chapter_damage_rank_reward = 0;
			this->GetCommonData().fetch_boss_chapter_rank_reward_day_id = EngineAdapter::Instance().DayID();
		}

		GuildMember * member = guild->GetMemberManager()->GetMember(m_role->GetUserId());
		if (NULL != member)
		{
			member->SetGuildDayActiveRewardFlag(0);
		}
	}
	else
	{
		if (this->GetCommonData().fetch_boss_chapter_rank_reward_day_id != EngineAdapter::Instance().DayID())
		{
			this->GetCommonData().is_fetch_boss_chapter_damage_rank_reward = 0;
			this->GetCommonData().fetch_boss_chapter_rank_reward_day_id = EngineAdapter::Instance().DayID();
		}
	}

	this->GetCommonData().guild_day_active_reward_flag = 0;
	m_role->SendGuildActiveInfo();
}

void RoleModuleManager::WeekChangeResetGuildData()
{
	
}

void RoleModuleManager::OnXunBaoBelssReq(int index, int pet_id)
{
	if(index < 0 || index >= MAX_XUN_BAO_BLESS_PET_NUM) return;

	if (0 != pet_id)
	{
		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL == pet_cfg)
		{
			return;
		}
		switch (index)
		{
		case XUN_BAO_REWARD_TYPE_SILVER:
		{
			if (pet_cfg->quality != PET_QUALITY_SILVER) return;
		}
		break;
		case XUN_BAO_REWARD_TYPE_GOLD:
		{
			if (pet_cfg->quality != PET_QUALITY_GOLD) return;
		}
		break;
		default:
			break;
		}
	}

	m_common_data_param.xun_bao_bless_pet_id_list[index] = pet_id;
	this->SendXunBaoBelssInfo();
}

void RoleModuleManager::SendXunBaoBelssInfo()
{
	Protocol::SCXunBaoBelssInfo info;
	memcpy(info.xun_bao_bless_pet_id_list, m_common_data_param.xun_bao_bless_pet_id_list, sizeof(info.xun_bao_bless_pet_id_list));

	this->NetSend(&info, sizeof(info));
}

void RoleModuleManager::OnClientBehavior(int behavior_type)
{
	if(behavior_type < 0 || behavior_type >= CLIENT_BEHAVIOR_TYPE_MAX) return;
	
	ROLE_LOG_QUICK6(LOG_TYPE_CLIENT_BEHAVIOR, this->GetRole(), behavior_type, 0, NULL, NULL);
}

bool RoleModuleManager::CanEnterScene(int scene_id, bool is_notice /* = true */)
{
	bool can_enter = true;
	int limit_level = -1;
	if (!this->GetMapUnlock()->CheckMapIsUnlock(scene_id, &limit_level)) can_enter = false;

	if (!can_enter && is_notice)
	{
		if (limit_level != -1)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_scene_unlock_notice, limit_level);
			if (length > 0)
			{
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					this->NetSend((const char*)&sm, sendlen);
				}
			}
		}
	}
	
	return can_enter;
}

void RoleModuleManager::OnEnterScene(int scene_id)
{
	this->GetTaskManager()->OnEnterScene(scene_id);
	this->GetMapUnlock()->OnEnterScene(scene_id);
	this->GetResearchTask()->OnEnterScene(scene_id);
	this->GetTaskChain()->OnEnterScene(scene_id);
}

void RoleModuleManager::OnLeaveScene(int scene_id)
{
	
}

void RoleModuleManager::AddNeedMonster(int monster_id)
{
	NeedMonsterMap::iterator it = m_need_monster_map.find(monster_id);
	if (it == m_need_monster_map.end())
	{
		it = m_need_monster_map.insert(NeedMonsterMap::value_type(monster_id, 0)).first;
	}

	it->second += 1;
}

void RoleModuleManager::RemoveNeedMonster(int monster_id)
{
	NeedMonsterMap::iterator it = m_need_monster_map.find(monster_id);
	if (it != m_need_monster_map.end())
	{
		it->second -= 1;
		if (it->second <= 0)
		{
			m_need_monster_map.erase(it);
		}
	}
}

bool RoleModuleManager::IsNeedMonster(int monster_id)
{
	NeedMonsterMap::iterator it = m_need_monster_map.find(monster_id);
	if (it != m_need_monster_map.end() && it->second > 0)
	{
		return true;
	}

	return false;
}

void RoleModuleManager::OnSendScoreHintNotice(int recommend_score)
{
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_role_recommend_score_not_enough_notice, recommend_score);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(this->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
}

bool RoleModuleManager::IsMeetGuildFightRank(int rank)
{
	if(rank <= 0 || m_common_data_param.max_guild_fight_rank <= 0) return false;

	return m_common_data_param.max_guild_fight_rank <= rank;
}

void RoleModuleManager::OnSetGuildFightInfo(short guild_fight_rank)
{
	if(guild_fight_rank <= 0) return;

	m_common_data_param.max_guild_fight_rank = (0 == m_common_data_param.max_guild_fight_rank) ? guild_fight_rank : GetMin(guild_fight_rank, m_common_data_param.max_guild_fight_rank);
	this->GetCourseOfGrowth()->EventOperator(COG_CON_GUILD_FIGHT_RANK, 0, m_common_data_param.max_guild_fight_rank);
}

bool RoleModuleManager::IsMeetGuildHonorRank(int rank)
{
	if (rank <= 0 || m_common_data_param.max_guild_honor_rank <= 0) return false;

	return m_common_data_param.max_guild_honor_rank <= rank;
}

void RoleModuleManager::OnSetGuildHonorInfo(short guild_honor_rank)
{
	if (guild_honor_rank <= 0) return;

	m_common_data_param.max_guild_honor_rank = (0 == m_common_data_param.max_guild_honor_rank) ? guild_honor_rank : GetMin(guild_honor_rank, m_common_data_param.max_guild_honor_rank);
	this->GetCourseOfGrowth()->EventOperator(COG_CON_GUILD_HONOR_RANK, 0, m_common_data_param.max_guild_honor_rank);
}

void RoleModuleManager::OnSetColosseumInfo(int colosseum_rank)
{
	int old_colosseum_rank = m_common_data_param.colosseum_rank;
	m_common_data_param.colosseum_rank = colosseum_rank;

	EventHandler::Instance().OnRoleColosseumRankChange(this->GetRole(), old_colosseum_rank, m_common_data_param.colosseum_rank);
}

int RoleModuleManager::GetColosseumRank()
{
	return m_common_data_param.colosseum_rank;
}

void RoleModuleManager::OnFetchGuildHongBao()
{
	m_common_data_param.guild_hong_bao_fetch_count += 1;

	EventHandler::Instance().OnFetchGuildHongBao(this->GetRole());
}

void RoleModuleManager::OnCorrectGuildAnswer()
{
	m_common_data_param.guild_answer_correct_count += 1;

	EventHandler::Instance().OnCorrectGuildAnswer(this->GetRole());
}

bool RoleModuleManager::HasMovePrestigeData()
{
	return 0 != m_common_data_param.is_move_prestige_data_flag;
}

void RoleModuleManager::OnSetMovePrestigeDataFlag()
{
	m_common_data_param.is_move_prestige_data_flag = 1;
}

void RoleModuleManager::OnSetWorldTeamArenaTimestamp(unsigned int set_timestamp)
{
	m_common_data_param.last_set_world_team_aren_timestamp = GetMax(m_common_data_param.last_set_world_team_aren_timestamp, set_timestamp);
}

void RoleModuleManager::OnSetWorldTeamArenaInfo(int team_rank, unsigned int set_timestamp, const int * title_id)
{
	//时间是为了防止玩家跨服不在线导致回原服离线设置后玩家登陆回跨服拿到新称号
	if (set_timestamp < m_common_data_param.last_set_world_team_aren_timestamp)
	{
		return;
	}

	m_common_data_param.world_team_arena_rank = team_rank;
	if (NULL != title_id)
	{
		m_common_data_param.world_team_arena_title_id = *title_id;
	}
}

WorldTeamArenaRoleInfo RoleModuleManager::GetWorldTeamArenaRankInfo()
{
	WorldTeamArenaRoleInfo info;
	info.world_team_arena_rank = m_common_data_param.world_team_arena_rank;
	info.title_id = m_common_data_param.world_team_arena_title_id;
	return info;
}

void RoleModuleManager::OnChannelChat(int channel_type)
{
	if(channel_type < 0 || channel_type >= chatdef::CHANNEL_TYPE_MAX) return;

	if (m_common_data_param.today_channel_chat_count_list[channel_type] < MAX_CHANNEL_CHAT_TODAY_COUNT)
	{
		m_common_data_param.today_channel_chat_count_list[channel_type] += 1;
		m_common_data_param.channel_chat_count_list[channel_type] += 1;
	}
	EventHandler::Instance().OnChannelChat(this->GetRole(), channel_type);
}

int RoleModuleManager::GetChannelChatByType(int channel_type)
{
	if(channel_type < 0 || channel_type >= chatdef::CHANNEL_TYPE_MAX) return 0;

	return m_common_data_param.channel_chat_count_list[channel_type];
}

Guild * RoleModuleManager::GetGuild()
{
	return m_role->GetGuild();
}

bool RoleModuleManager::OnNewBuyGiftItem(int param1, int param2, int param3, const char * reason)
{
	if (!LOGIC_CONFIG->GetSundryConfig()->IsCanUseBuyGiftByType(param1))
	{
		return false;
	}

	int ret = 0, price = 0, chongzhi_num = 0;
	ret = this->BuyGiftItem(param1, param2, param3, &chongzhi_num);
	price = chongzhi_num / 10;	//获取出来的是元宝数量
	if (price <= 0 || 0 != ret)
	{
		return false;
	}
	int item_id = LOGIC_CONFIG->GetSundryConfig()->GetItemIdByChongZhiNum(chongzhi_num);
	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return false;
	}
	//不检测临时背包
	if (!this->GetKnapsack()->HasItem(item_id, 1, false))
	{
		return false;
	}

	ret = this->BuyGiftItem(param1, param2, param3);
	if (0 == ret && price > 0)
	{
		if (!this->GetKnapsack()->ConsumeItem(item_id, 1, __FUNCTION__))
		{
			gamelog::g_log_norex_use_gift_item.printf(LL_WARNING, "%s -> %s item[%d, %d] chongzhi_num[%d] param[1:%d 2:%d 3:%d] ret[%d] ", NULL == reason ? "nil" : reason, 
				__FUNCTION__, item_id, 1, chongzhi_num, param1, param2, param3, ret);
		}
		else
		{
			ROLE_LOG_QUICK10(LOG_TYPE_UNIVERSAL_ACTIVE_CARD, this->GetRole(), item_id, 1, __FUNCTION__, reason, param1, param2, param3,
				this->GetKnapsack()->Count(item_id, true, true) + 1, chongzhi_num);
		}
	}

	if (0 == ret)
	{
		EventHandler::Instance().OnAddChongzhiNewGiftBuy(this->GetRole(), chongzhi_num, this->IsCanSendMailByBuyGiftItem(param1), LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().is_add_day_chong);
	}
	gamelog::g_log_norex_use_gift_item.printf(LL_INFO, "%s -> %s item[%d, %d] chongzhi_num[%d] param[1:%d 2:%d 3:%d] ret[%d] ", NULL == reason ? "nil" : reason, 
		__FUNCTION__, item_id, 1, chongzhi_num, param1, param2, param3, ret);
	return (0 == ret);
}

int RoleModuleManager::OnBuyGiftItem(int param1, int param2, int param3, int use_item_id, const char * reason, bool is_need_consume)
{
	int ret = 0, price = 0, chongzhi_num = 0;
	if (is_need_consume)
	{
		ret = this->BuyGiftItem(param1, param2, param3, &chongzhi_num);
		price = chongzhi_num / 10;	//获取出来的是元宝数量
		if (price <= 0 || 0 != ret)
		{
			return -777;
		}
		//不检测临时背包
		if (this->GetKnapsack()->Count(use_item_id, false) < price)
		{
			this->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return -888;
		}
	}
	ret = this->BuyGiftItem(param1, param2, param3);
	
	if (is_need_consume && 0 == ret && price > 0)
	{
		if (!this->GetKnapsack()->ConsumeItem(use_item_id, price, __FUNCTION__))
		{
			gamelog::g_log_norex_use_gift_item.printf(LL_WARNING, "%s -> %s item[%d, %d] param[1:%d 2:%d 3:%d] ret[%d] ", NULL == reason ? "nil" : reason, __FUNCTION__, 
				use_item_id, price, param1, param2, param3, ret);
		}
		else
		{
			ROLE_LOG_QUICK10(LOG_TYPE_UNIVERSAL_ACTIVE_CARD, this->GetRole(), use_item_id, price, __FUNCTION__, reason, param1, param2, param3, 
				this->GetKnapsack()->Count(use_item_id, true, true) + price, 0);
		}
	}

	if (0 == ret)
	{
		EventHandler::Instance().OnAddChongzhi(this->GetRole(), chongzhi_num, this->IsCanSendMailByBuyGiftItem(param1));
	}
	gamelog::g_log_norex_use_gift_item.printf(LL_INFO, "%s -> %s item[%d, %d] param[1:%d 2:%d 3:%d] ret[%d] ", NULL == reason ? "nil" : reason, __FUNCTION__, use_item_id,
		price, param1, param2, param3, ret);
	return ret;
}

bool RoleModuleManager::IsCanSendMailByBuyGiftItem(int buy_gift_item_type) const
{
	int ret = false;
	switch (buy_gift_item_type)
	{
		case BUY_GIFT_ITEM_TYPE_CHONGZHI:
		{
			ret = true;
		}
		default:
		break;
	}
	UNSTD_STATIC_CHECK(62 == BUY_GIFT_ITEM_TYPE_MAX);

	return ret;
}

void RoleModuleManager::CommonDataCheck()
{
	if (0 == m_common_data_param.use_new_pet_status_array)
	{
		m_common_data_param.use_new_pet_status_array = 1;

		for (int i = 0; i < PET_STATUS_MAX_OBSOLETE; ++i)
		{
			m_common_data_param.new_pet_status_index[i] = m_common_data_param.pet_status_index_OBSOLETE[i];
		}
		UNSTD_STATIC_CHECK(PET_STATUS_MAX_OBSOLETE < NEW_PET_STATUS_MAX);
	}

	if (0 == m_common_data_param.next_week_timestamp)
	{
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_common_data_param.next_week_timestamp = now + EngineAdapter::Instance().NextWeekInterval(1, 0, 0, 0);
	}

	if (0 == m_common_data_param.next_month_timestamp)
	{
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_common_data_param.next_month_timestamp = now + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
	}

	if (0 == m_common_data_param.is_transfer_chat_group_data)
	{
		m_common_data_param.is_transfer_chat_group_data = 1;

		int num = 0;
		for (int i = 0; i < ARRAY_LENGTH(m_common_data_param.cross_chat_group_list_OBSOLETE); ++i)
		{
			m_common_data_param.cross_chat_group_list[num++] = m_common_data_param.cross_chat_group_list_OBSOLETE[i];
			gamelog::g_log_debug.printf(LL_INFO, "CommonDataCheck transfer cross chat group, role[%d,%s] corss_chat_group_id[%lld]",
				m_role->GetUID(), m_role->GetName(), m_common_data_param.cross_chat_group_list_OBSOLETE[i]);
		}
		for (int i = 0; i < ARRAY_LENGTH(m_common_data_param.local_chat_group_list_OBSOLETE); ++i)
		{
			m_common_data_param.cross_chat_group_list[num++] = m_common_data_param.local_chat_group_list_OBSOLETE[i];
			gamelog::g_log_debug.printf(LL_INFO, "CommonDataCheck transfer local chat group, role[%d,%s] chat_group_id[%lld]",
				m_role->GetUID(), m_role->GetName(), m_common_data_param.local_chat_group_list_OBSOLETE[i]);
		}
	}

	if (0u == m_common_data_param.cloud_arena_new_shop_timestamp)
	{
		unsigned int cur_season_start_timestamp = WorldStatus::Instance().GetCommonData().cloud_arena_cur_season_start_timestamp;
		unsigned int cur_season_end_timestamp = WorldStatus::Instance().GetCommonData().cloud_arena_cur_season_end_timestamp;
		unsigned int config_new_shop_open_timestamp = LOGIC_CONFIG->GetShopConfig()->GetShopOtherCfg().cloud_arena_new_shop_timestamp;
		m_common_data_param.cloud_arena_new_shop_timestamp = CalcCloudArenaNewShopOpenTimestamp(cur_season_start_timestamp, cur_season_end_timestamp, config_new_shop_open_timestamp);
		gamelog::g_log_cloud_arena.printf(LL_INFO, "RoleModuleManager::CommonDataCheck \
		role[%d,%s] start_new_shop_time[%u] season_start[%u] season_end[%u] \
		config_timestamp[%u]", this->GetUid(), this->GetRoleName(),
			m_common_data_param.cloud_arena_new_shop_timestamp,
			cur_season_start_timestamp, cur_season_end_timestamp,
			config_new_shop_open_timestamp);
	}

	if (0 == m_common_data_param.dislocation_reset_ver_flag)
	{
		m_common_data_param.world_team_arena_rank = 0;
		m_common_data_param.dislocation_reset_ver_flag += 1;
	}
	UNSTD_STATIC_CHECK(DISLOCATION_RESET_VER_FLAG == 1);
}

unsigned int RoleModuleManager::CalcCloudArenaNewShopOpenTimestamp(unsigned int cur_season_start_timestamp, unsigned int cur_season_end_timestamp, unsigned int config_new_shop_timestamp)
{
	if (cur_season_start_timestamp < config_new_shop_timestamp && config_new_shop_timestamp <= cur_season_end_timestamp)
	{
		return cur_season_end_timestamp; // 当赛季已开启但结束时间在config的时间后面
	}
	else
	{
		return config_new_shop_timestamp;
	}
}

int RoleModuleManager::ClearMixedElementCrystal()
{
	int count = 0;
	for (int i = 0; i < ARRAY_LENGTH(MIXED_ELEMENT_CRYSTAL_ITEM_ID); ++i)
	{
		int num = this->GetKnapsack()->Count(MIXED_ELEMENT_CRYSTAL_ITEM_ID[i], true, true);
		for (int c = 0; c < num; ++c)
		{
			if (this->GetKnapsack()->ConsumeKnapsackItem(MIXED_ELEMENT_CRYSTAL_ITEM_ID[i], 1, __FUNCTION__))
			{
				count += 1;
			}
		}
	}

	count += this->GetPartner()->ClearMixedElementCrystal();
	count += this->GetEquipmentManager()->ClearMixedElementCrystal();

	if (count > 0)
	{
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = count * 5000;

		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_clear_mixed_element_crystal_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_clear_mixed_element_crystal_content);

		MailRoute::MailToUser(this->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}

	return 0;
}

void RoleModuleManager::OnWildBossWeekChange()
{
	this->m_common_data_param.wild_boss_get_reward_times = 0;
}

void RoleModuleManager::OnSendWildBossOtherInfo()
{
	Protocol::SCWildBossOtherInfo info;
	info.reward_times = this->m_common_data_param.wild_boss_get_reward_times;

	this->NetSend(&info, sizeof(info));
}

void RoleModuleManager::OnBigDipperFightSucc()
{
	m_common_data_param.big_dipper_fight_succ_times += 1;
}

void RoleModuleManager::OnTopLevelUpdate(unsigned int now_second)
{
	if (this->GetRole()->GetLevel() < MAX_ROLE_LEVEL || this->GetRole()->GetNextTopLevelRefreshTimestamp() <= 0) return;

	unsigned int world_refresh_timestamp = WorldStatus::Instance().GetTopLevelRefreshTimestamp();
	if(world_refresh_timestamp <= 0) return;

	//已达到重置时间,此时还没有玩家去激活新赛季
	if (now_second >= this->GetRole()->GetNextTopLevelRefreshTimestamp())
	{
		//未领取的通过邮件发放
		for (int i = 1; i <= this->GetRole()->GetTopLevel() && i <= MAX_ROLE_TOP_LEVEL_NUM; i++)
		{
			if (m_common_data_param.top_level_reward_fetch_flag.IsBitSet(i)) continue;

			const RoleTopLevelCfg * role_top_level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(i);
			if (NULL == role_top_level_cfg) continue;

			if (!role_top_level_cfg->reward_list.empty())
			{
				this->GetKnapsack()->SendMail(&role_top_level_cfg->reward_list[0], (int)role_top_level_cfg->reward_list.size(), SEND_MAIL_TYPE_TOP_LEVEL);
			}
		}

		m_common_data_param.is_refresh_first_notice_top_level_flag = 0;
		this->GetRole()->SetTopLevelNextRefreshTimestamp(0);
		m_common_data_param.top_level_reward_fetch_flag.Reset();
		gamelog::g_log_role_top_level.printf(LL_INFO, "%s user[%d, %s] top_level[%d]", __FUNCTION__, this->GetRoleUID(), this->GetRoleName(), this->GetRole()->GetTopLevel());
		this->GetRole()->RoleChangeLevel(1, false);
		this->SendTopLevelAllInfo();
	}
}

void RoleModuleManager::OnTopLevelDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid || this->GetRole()->GetLevel() < MAX_ROLE_LEVEL || this->GetRole()->GetTopLevel() <= 0) return;

	//检测是否有未领取的奖励,防止积攒过多导致最后重置时邮件数量超过上限
	bool is_send = false;
	for (int i = 1; i <= this->GetRole()->GetTopLevel() && i <= MAX_ROLE_TOP_LEVEL_NUM; i++)
	{
		if(m_common_data_param.top_level_reward_fetch_flag.IsBitSet(i - 1)) continue;

		const RoleTopLevelCfg * role_top_level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(i);
		if (NULL == role_top_level_cfg) continue;

		if (!role_top_level_cfg->reward_list.empty())
		{
			this->GetKnapsack()->SendMail(&role_top_level_cfg->reward_list[0], (int)role_top_level_cfg->reward_list.size(), SEND_MAIL_TYPE_TOP_LEVEL);
		}
		m_common_data_param.top_level_reward_fetch_flag.SetBit(i - 1);
		is_send = true;
	}
	if (is_send)
	{
		this->SendTopLevelAllInfo();
	}
}

void RoleModuleManager::OnFetchTopLevelReward(int top_level)
{	
	if (top_level <= 0 || top_level > MAX_ROLE_TOP_LEVEL_NUM) return;

	if (top_level > this->GetRole()->GetTopLevel())
	{
		this->NoticeNum(errornum::EN_ROLE_TOP_LEVEL_NOT_ENOUGH_FETCH_FAILD);
		return;
	}

	const RoleTopLevelCfg * role_top_level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(top_level);
	if(NULL == role_top_level_cfg) return;

	if (m_common_data_param.top_level_reward_fetch_flag.IsBitSet(top_level - 1))
	{
		this->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!role_top_level_cfg->reward_list.empty())
	{
		if (!this->GetKnapsack()->PutList((short)role_top_level_cfg->reward_list.size(), &role_top_level_cfg->reward_list[0], PUT_REASON_TOP_LEVEL))
		{
			this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_common_data_param.top_level_reward_fetch_flag.SetBit(top_level - 1);
	gamelog::g_log_role_top_level.printf(LL_INFO, "%s user[%d, %s] top_level[%d] fetch_level[%d]", __FUNCTION__, this->GetRoleUID(), this->GetRoleName(), this->GetRole()->GetTopLevel(), top_level);
	this->SendTopLevelNotice(Protocol::SCRoleTopLevelNoticce::ROLE_TOP_LEVEL_NOTICE_TYPE_FETCH, 1, top_level);
}

void RoleModuleManager::OnFetchTopLevelAchieveReward(int seq)
{
	const RoleTopLevelAchieveCfg * achieve_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetToplevelAchieveCfg(seq);
	if (NULL == achieve_cfg)
	{
		return;
	}

	if(m_common_data_param.top_level_achieve_flag.IsBitSet(seq))
	{
		this->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	bool is_conform = true;
	switch (achieve_cfg->achieve_type)
	{
		case RoleTopLevelAchieveCfg::ACHIEVE_TYPE_LEVEL:
		{
			if (this->GetRole()->GetTopLevel() < achieve_cfg->param)
			{
				is_conform = false;
			}
		}
		break;
		default:
		return;
	}
	if (!is_conform)
	{
		this->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}
	if (!this->GetKnapsack()->PutVec(achieve_cfg->item_list, PUT_REASON_TOP_LEVEL))
	{
		this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_common_data_param.top_level_achieve_flag.SetBit(seq);
	this->SendTopLevelNotice(Protocol::SCRoleTopLevelNoticce::ROLE_TOP_LEVEL_NOITCE_TYPE_FETCH_ACHIEVE, 1, seq);
	gamelog::g_log_role_top_level.printf(LL_INFO, "%s user[%d, %s] top_level[%d] seq[%d]", __FUNCTION__, this->GetRoleUID(), this->GetRoleName(), this->GetRole()->GetTopLevel(), seq);
}

void RoleModuleManager::OnSetTopLevelNoticeFlag()
{
	if(0 != m_common_data_param.is_refresh_first_notice_top_level_flag) return;

	m_common_data_param.is_refresh_first_notice_top_level_flag = 1;
	//不能使用通用的设置等级接口,因为这里是没打开过该界面,经验会一直累计,需要在这里进行计算
	this->GetRole()->OnRoleResetTopLevelAndCheck();
	m_common_data_param.top_level_reward_fetch_flag.Reset();
	gamelog::g_log_role_top_level.printf(LL_INFO, "%s user[%d, %s] top_level[%d]", __FUNCTION__, this->GetRoleUID(), this->GetRoleName(), this->GetRole()->GetTopLevel());
	this->GetRole()->SetTopLevelNextRefreshTimestamp(WorldStatus::Instance().GetTopLevelRefreshTimestamp());
	this->SendTopLevelAllInfo();
}

bool RoleModuleManager::IsOpenTopLevelNotice()
{
	 return 0 != m_common_data_param.is_refresh_first_notice_top_level_flag; 
}

void RoleModuleManager::SendTopLevelAllInfo()
{
	Protocol::SCRoleTopLevelAllInfo info;
	info.is_refresh_first_notice_top_level_flag = m_common_data_param.is_refresh_first_notice_top_level_flag;
	info.reserve_ch = 0;
	info.reserve_sh = 0;
	info.top_level = this->GetRole()->GetTopLevel();
	info.next_top_level_refresh_timestamp = this->GetRole()->GetNextTopLevelRefreshTimestamp();
	info.role_top_level_reward_fetch_flag = m_common_data_param.top_level_reward_fetch_flag;
	info.top_level_achieve_flag = m_common_data_param.top_level_achieve_flag;

	this->NetSend(&info, sizeof(info));
}

void RoleModuleManager::SendTopLevelNotice(int notice_type, short param1, int param2)
{
	Protocol::SCRoleTopLevelNoticce info;
	info.notice_type = notice_type;
	info.param1 = param1;
	info.param2 = param2;

	this->NetSend(&info, sizeof(info));
}

void RoleModuleManager::OnTreviFountainBlessing()
{
	int max_count = LOGIC_CONFIG->GetTreviFountionconfig()->GetOtherCfg()->wish_num;
	if (m_common_data_param.trevi_fountain_today_use_count >= max_count)
	{
		m_role->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
		return;
	}

	const WishRewardCfg * reward_cfg = LOGIC_CONFIG->GetTreviFountionconfig()->GetRewardCfg(m_role->GetLevel());
	if (NULL == reward_cfg) return;

	if (NULL != ITEMPOOL->GetItem(reward_cfg->reward.item_id) && !m_role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward_cfg->reward.item_id, reward_cfg->reward.num))
	{
		m_role->GetRoleModuleManager()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}


	this->GetMoney()->AddCoinBind(reward_cfg->coin, __FUNCTION__);
	m_role->AddExp(reward_cfg->exp);

	//if (0 == m_common_data_param.trevi_fountain_buff_id)取消一天只有一个buff 的设定
	{
		const WishBuffCfg * buff_cfg = LOGIC_CONFIG->GetTreviFountionconfig()->RandBuff(reward_cfg->buff_group);
		if (NULL == buff_cfg) return;

		unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
		m_common_data_param.trevi_fountain_buff_id[m_common_data_param.trevi_fountain_today_use_count] = buff_cfg->buff_id;
		m_common_data_param.trevi_fountain_buff_expiration_time[m_common_data_param.trevi_fountain_today_use_count] = now_time + buff_cfg->buff_time;

		BuffChangeAttr *buff = new BuffChangeAttr(this, now_time, m_common_data_param.trevi_fountain_buff_expiration_time[m_common_data_param.trevi_fountain_today_use_count], ROLE_BUFF_ORIGIN_TYPE_TREVI_FOUNTAIN_BLESSING);
		buff->AddAttr(buff_cfg->param_list[0], buff_cfg->param_list[1]);
		buff->AddAttr(buff_cfg->param_list[2], buff_cfg->param_list[3]);
			
		buff->SetSave(true);
		this->GetBuffManager()->AddBuff(buff);
	}

	m_role->GetRoleModuleManager()->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_TREVIFOUNTION);

	m_common_data_param.trevi_fountain_today_use_count++;
	this->SendTreviFountainInfo();
	EventHandler::Instance().OnMakeAWishOnce(m_role);
}

void RoleModuleManager::SendTreviFountainInfo()
{
	Protocol::SCTreviFountainInfo info;
	info.count = m_common_data_param.trevi_fountain_today_use_count;
	for (int i = 0; i < MAX_TREVI_FOUNTION_TODAY_BUFF_NUM && i < info.count; i++)
	{
		info.send_info[i].trevi_fountain_buff_id = m_common_data_param.trevi_fountain_buff_id[i];
		info.send_info[i].trevi_fountain_buff_expiration_time = m_common_data_param.trevi_fountain_buff_expiration_time[i];
	}
	int length = sizeof(info) - sizeof(Protocol::SCTreviFountainInfo::buff_info) * (MAX_TREVI_FOUNTION_TODAY_BUFF_NUM - info.count);
	EngineAdapter::Instance().NetSend(m_role->GetNetId(), (const char*)&info, length);
}

void RoleModuleManager::OnTreviFountainResetData(int old_dayid, int now_dayid)
{
}

void RoleModuleManager::OnTreviFountainDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	this->OnTreviFountainFindDataReset();

	if (!FunOpen::IsFunOpen(m_role, FUN_TYPE_WISH_POOL)) return;

	m_common_data_param.trevi_fountain_today_use_count = 0;
	memset(m_common_data_param.trevi_fountain_buff_id, 0, sizeof(m_common_data_param.trevi_fountain_buff_id));
	memset(m_common_data_param.trevi_fountain_buff_expiration_time, 0, sizeof(m_common_data_param.trevi_fountain_buff_expiration_time));
	this->SendTreviFountainInfo();
}

void RoleModuleManager::OnTreviFountainFindDataReset()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN);
	if (NULL == cfg || this->GetRole()->GetLevel() < cfg->find_level) return;

	int day_max_num = LOGIC_CONFIG->GetTreviFountionconfig()->GetOtherCfg()->wish_num;
	if (m_common_data_param.offine_day < 2)
	{
		if (day_max_num >= m_common_data_param.trevi_fountain_today_use_count)
		{
			m_common_data_param.trevi_fountain_can_find_count = day_max_num - m_common_data_param.trevi_fountain_today_use_count;
		}
		else
		{
			m_common_data_param.trevi_fountain_can_find_count = 0;
		}
	}
	else
	{
		m_common_data_param.trevi_fountain_can_find_count = day_max_num;
	}
}

bool RoleModuleManager::OnTreviFountainFindReward(int consume_type, int count)
{
	if (count > m_common_data_param.trevi_fountain_can_find_count)
	{
		m_role->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	int exp = 0;
	int coin = 0;
	for (int i = 0; i < count; i++)
	{
		int temp_exp = 0;
		int temp_coin = 0;
		if (LOGIC_CONFIG->GetTreviFountionconfig()->GetFindReward(m_role->GetLevel(), consume_type, &temp_coin, &temp_exp))
		{
			if (temp_exp > 0)exp += temp_exp;
			if (temp_coin > 0)coin += temp_coin;
		}
		else
		{
			m_role->NoticeNum(errornum::EN_FIND_REWARD_ERROR);
			return false;
		}
	}

	mMoney->AddCoinBind(coin, __FUNCTION__);
	m_role->AddExp(exp, ADD_EXP_TYPE_FIND_REWARD, __FUNCTION__);

	m_common_data_param.trevi_fountain_can_find_count -= count;
	this->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN] += count;
	this->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN);
	return true;
}

void RoleModuleManager::OnFantasyResetData(int old_dayid, int now_dayid)
{
}

void RoleModuleManager::OnFantasyDayChange(int old_dayid, int now_dayid)
{
	if (old_dayid == now_dayid) return;
	this->OnFantasyFindDataReset();

	if (!FunOpen::IsFunOpen(m_role, FUN_TYPE_FANTASY_FB)) return;

	m_common_data_param.fantasy_fb_flag.Reset();
	this->SendFantasyFbFinishInfo();
}

void RoleModuleManager::OnFantasyFindDataReset()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_FANTASY_FB);
	if (NULL == cfg || this->GetRole()->GetLevel() < cfg->find_level) return;

	if (m_common_data_param.offine_day < 2)
	{
		m_common_data_param.find_fantasy_flag = m_common_data_param.fantasy_fb_flag;
	}
	else
	{
		m_common_data_param.find_fantasy_flag.Reset();
	}
}

bool RoleModuleManager::OnFantasyFindReward(int consume_type, int count)
{
	int layer[LAYER_MAX_NUM] = { 0 };
	int find_count = this->GetFantasyFindCount(consume_type, layer, LAYER_MAX_NUM);
	if (count > find_count)
	{
		m_role->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const FantasyFbFindLevelGroupCfg* find_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetFindCfg(this->GetRole()->GetLevel(), consume_type);
	if(NULL == find_cfg || (int)find_cfg->count <= 0) return false;
	
	std::vector<ItemConfigData> item_list;
	int add_exp_count = 0;
	int add_coin_count = 0;
	int add_family_count = 0;
	int use_count = 0;
	for (int i = 0; i < (int)find_cfg->count && i < count && i < find_count; i++)
	{
		const FantasyFBFindRewardGroupCfg * reward_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetFindRewardCfg(find_cfg->node[layer[i] - 1].group_id);
		if (reward_cfg != NULL)
		{
			item_list.push_back(reward_cfg->reward);
		}
		add_exp_count += find_cfg->node[layer[i] - 1].exp;
		add_coin_count += find_cfg->node[layer[i] - 1].coin;
		add_family_count += find_cfg->node[layer[i] - 1].family_gongxian;
		use_count++;
	}
	if (!item_list.empty())
	{
		if (!this->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
		{
			this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
		if (!this->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
		{
			this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
	}

	if (add_exp_count > 0)
	{
		this->GetRole()->AddExp(add_exp_count, ADD_EXP_TYPE_FIND_REWARD);
	}
	this->GetMoney()->AddCoinBind(add_coin_count, __FUNCTION__);
	if (NULL != m_role->GetGuild())
	{
		this->GetMoney()->AddOtherCoin(add_family_count, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}

	for (int i = 0; i < count && i < find_count; i++)
	{
		m_common_data_param.find_fantasy_flag.SetBit(layer[i]);
	}
	this->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_FANTASY_FB] += use_count;
	this->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_FANTASY_FB);
	return true;
}

int RoleModuleManager::GetFantasyFindCount(int comsume_type, int * layer, int len)
{
	int find_count = 0;
	const FantasyFbFindLevelGroupCfg * cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetFindCfg(this->GetRole()->GetLevel(), comsume_type);
	if(NULL == cfg || (int)cfg->count <= 0) return find_count;
	
	for (int i = 1; i <= cfg->count && i <= LAYER_MAX_NUM; i++)
	{
		if (!m_common_data_param.find_fantasy_flag.IsBitSet(i))
		{
			if (layer != NULL && len > find_count)
			{
				layer[find_count] = i;	
			}
			find_count++;
		}
	}

	return find_count;
}

int RoleModuleManager::GetFantasyTodayFinishNum(int * layer, int length, int *ret_len)
{
	const FantasyFbKillBossRewardCfg * kill_reward_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetRewardCfg(m_role->GetLevel());
	if(NULL == kill_reward_cfg ||
		kill_reward_cfg->count <= 0) return 0;
	int finish_num = 0;
	if (ret_len != NULL)
	{
		*ret_len = 0;
	}
	for (int i = 1; i <= kill_reward_cfg->count && i <= LAYER_MAX_NUM; i++)
	{
		if (m_common_data_param.fantasy_fb_flag.IsBitSet(i))
		{
			finish_num++;
		}
		else
		{
			if (layer != NULL && NULL != ret_len && length > *ret_len)
			{
				layer[*ret_len++]  = i;
			}
		}
	}

	return finish_num;
}

void RoleModuleManager::OneKeyFinishFantasyFB()
{
	const DayRewardFindCfg *dr_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_FANTASY_FB);if (!dr_cfg) return;
	const FantasyFbKillBossRewardCfg * kill_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetRewardCfg(this->GetRole()->GetLevel());if (!kill_cfg) return;
	const DayRewardFindPriceCfg * price_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindRewardPriceCfg(DAY_ACTIVITY_TYPE_FANTASY_FB, this->GetRole()->GetLevel());
	if (NULL == price_cfg) return;
	
	int shengyu = kill_cfg->count - this->GetFantasyTodayFinishNum();// 剩余的次数

	if (shengyu == 0)
	{
		for (int i = 1; i <= LAYER_MAX_NUM; i++)
		{
			if (m_common_data_param.fantasy_fb_flag.IsBitSet(i))
			{
				gamelog::g_log_task.printf(LL_INFO, "role_id[%d]--fantasy fb has finish level[%d]  without onekey finish!: ", this->GetRole()->GetUID(),i);
			}
		}
		return;
	}

	int consume_money = 0;
	int consume_type = LOGIC_CONFIG->GetFantasyFbConfig()->GetFindType(this->GetRole()->GetLevel());
	if (consume_type != RB_TYPE_COIN) return;

	// 先扣钱再说
	switch (consume_type)
	{
	case RB_TYPE_COIN:
		{
			consume_money = (price_cfg->coin_price * shengyu * price_cfg->coin_scale) / 100;
			if (!this->GetMoney()->CoinBindMoreThan(consume_money))
			{
				NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			this->GetMoney()->UseCoinBind(consume_money, __FUNCTION__);
		}
		break;
/*	case RB_TYPE_GOLD:
		{
			consume_money = shengyu * dr_cfg->gold_price;
			if (!this->GetMoney()->GoldMoreThan(consume_money))
			{
				NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			this->GetMoney()->UseGold(consume_money, __FUNCTION__);
		}
		break;*/
	default:
		return;
		break;
	}

	for (int i = 1; i <= LAYER_MAX_NUM; i++)
	{
		if (!m_common_data_param.fantasy_fb_flag.IsBitSet(i))
		{
			m_common_data_param.fantasy_fb_flag.SetBit(i);	//设置标志位
			const FantasyFBRewardCfg & reward_cfg = kill_cfg->node[i - 1];

			this->GetRole()->AddExp(reward_cfg.reward_exp, ADD_EXP_TYPE_FANTASY_FB);
			this->GetLevelComplement()->OnAddExp(reward_cfg.reward_exp);
			EventHandler::Instance().OnFinishLevelComplementRelatedFun(this->GetRole(), reward_cfg.reward_exp, __FUNCTION__);
			gamelog::g_log_task.printf(LL_INFO, "role_id[%d]--fantasy fb has finish level[%d] with onekey finish! get exp[%d]: ", this->GetRole()->GetUID(),i, reward_cfg.reward_exp);
		}
	}

	EventHandler::Instance().OnFantasyFBReferReward(this->GetRole());
	this->SendFantasyFbFinishInfo();
}

void RoleModuleManager::SendFantasyFbFinishInfo()
{
	Protocol::SCFantasyFBRewardInfo info;
	info.op_type = Protocol::SCFantasyFBRewardInfo::FANTASY_FB_TYPE_INFO;
	info.fantasy_finish_flag = m_common_data_param.fantasy_fb_flag;

	int len = sizeof(info) - sizeof(info.reward_count) - sizeof(info.reward_list);
	this->NetSend(&info, len);
}

void RoleModuleManager::SendGuildShopInfo(int seq)
{
	static Protocol::SCGuildShopInfo info;
	info.count = 0;
	if (seq >= 0 && seq < GUILD_SHOP_COUNT_MAX)
	{
		info.shop_buy_list[info.count].seq = seq;
		info.shop_buy_list[info.count].buy_count = m_guild_common_param.shop_buy_list[seq];
		info.count++;
	}
	else
	{
		for (int i = 0; i < GUILD_SHOP_COUNT_MAX; ++i)
		{
			info.shop_buy_list[info.count].seq = i;
			info.shop_buy_list[info.count].buy_count = m_guild_common_param.shop_buy_list[i];
			info.count++;
		}
	}

	int len = sizeof(info) - (GUILD_SHOP_COUNT_MAX - info.count) * sizeof(Protocol::SCGuildShopInfo::ShopNode);
	this->NetSend((const char*)&info, len);
}

bool RoleModuleManager::NetSend(const void * data, unsigned int length)
{
	return EngineAdapter::Instance().NetSend(m_role->GetNetId(), data, length);
}

void RoleModuleManager::SendAllRoleInfo()
{
	this->GotoSchoolSendInfo();
	this->SendTuZiKuaiPaoInfo();
	this->SendGuildShopInfo();
	this->GetSurface()->SendAllInfo();
	this->GetDayReward()->SendAllInfo();
	this->GetWstQuestion()->SendAllInfo();
	this->GetWstQuestion()->SendWstInfo();
	this->GetCollection()->SendAllInfo();
	this->GetCollection()->SendIncludedAllInfo();
	this->GetTrain()->SendAllInfo();
	this->GetTrain()->SendPromotedInfo();
	this->GetTrain()->SendRedDotFlagInfo();
	this->GetTitle()->SendAllInfo();
	this->GetTitle()->SendOtherInfo();
	this->GetPrestige()->SendAllInfo();
	this->SendAttributeList(Protocol::SCRoleAttrList::NOTIFY_REASON_SYNC_HPMP_IN_FB);
	this->GetRoleAttributePlan()->SendPlanInfo();
	this->GetMoney()->Send(MONEY_TYPE_ALL, true);
	this->GetKnapsack()->SendKnapsackInfoToRole();
	this->GetPet()->SendFeiShengInfo();
	this->GetPet()->SendYiWenLuRewardInfo();
	this->GetPet()->SendAllInfo();
	this->GetPet()->SendPetIdRecordList();
	this->GetPet()->SendPetOtherInfo();
	this->GetPet()->SendPetSkinInfo();
	this->GetPet()->SendGodPrintBagInfo();
	this->GetPet()->SendPetBenedictionAllInfo();
	this->GetPet()->SendInfoStrengthenBuff();
	this->GetPet()->SendLianYaoCommonInfo();
	this->GetPartner()->SendAllPartnerInfo();
	this->GetPartner()->SendAllSkinInfo();
	this->GetPartner()->SendGiftTreeInfo();
	//this->GetTaskManager()->SendSummaryTaskList(); // 改为在TaskManager::Init时发
	this->GetTaskManager()->OnTeamConvert();
	this->GetTaskManager()->OnSendPartnerSoarInfo();
	this->GetEquipmentManager()->SendEquipmentInfoToRole();
	this->GetEquipmentManager()->SendAllJewelryInfo();
	this->GetEquipmentManager()->SendEquipMentLockInfo();
	this->GetEquipmentManager()->SendUpGreadeInfo();
	this->GetRoleSkill()->SendSkillList();
	this->GetRoleSkill()->SendLifeSkillInfo();
	this->GetWorkshop()->SendAllWorkshopInfo();
	this->GetMarriage()->SendInfo();
	this->GetBaby()->SendInfo();
	this->GetFriendRecommend()->SendFriendRecommend();
	this->GetShiTu()->SendRelationInfo();
	this->GetShiTu()->SendShiTuCommonInfo();
	this->GetCourageChallenge()->SendAllInfo();
	this->GetMonsterWave()->SendInfo();
	this->GetMiYi()->SendInfo();
	this->GetMiYi()->SendMiyiInfo();
	this->GetSchoolTask()->SendAllInfo();
	this->GetTreasureMap()->SendInfo();
	this->GetEscort()->SendAllInfo();
	this->GetTaskChain()->SendTaskInfo();
	this->GetMedal()->SendInfo();
	this->GetRoleCrystalFb()->SendInfo();
	this->GetShop()->SendAllInfo();
	this->GetRoleGatherFb()->SendInfo();
	this->SendTreviFountainInfo();
	this->GetExchange()->SendAllInfo();
	this->GetCourseOfGrowth()->SendCOGAllInfo();
	this->GetSalary()->SendAllInfo(SALARY_TYPE_CAPTAIN_SALARY);
	this->GetSalary()->SendAllInfo(SALARY_TYPE_CHIVAROUS_REWARD);
	this->GetFaceScore()->SendFaceAllInfo();
	this->GetFaceScore()->SendResTimeAllInfo();
	this->GetRoleConvertShop()->SendAllInfo();
	this->GetKnapsack()->SendExpCrystalInfo();
	this->GetSalary()->SendTodaySalary();
	this->GetRoleActivityManager()->SendAllInfo();
	this->GetBossChapter()->SendAllInfo();
	this->GetBountyTask()->SendInfo();
	this->GetBountyTask()->SendTaskInfo();
	this->SendFantasyFbFinishInfo();
	this->GetHuaShenManager()->SendSeasonInfo();
	this->GetFuncPreview()->SendInfo();
	this->GetAutoCatchPet()->SendAutoCatchInfo();
	this->GetShiTu()->SendShiTuCommonInfo();
	this->SendXunBaoInfo();
	this->GetTradeCredit()->SendCreditInfo();
	this->GetRoleGatherDream()->SendGatherDreamList();
	this->GetRoleHuanJie()->SendInfo();
	this->SendBraveGroundInfo();
	this->GetRoleCross()->SendCrossConnectInfo();
	this->GetAnecdoteTask()->SendAllInfo();
	this->GetSchoolExercise()->SendInfo();
	this->GetDreamNotesChallenge()->SendInfo();
	this->SendXunBaoBelssInfo();
	this->GetSmartMountsSystem()->SendAllInfo();
	this->GetSmartMountsSystem()->SendOtherInfo();
	this->GetPetMonopoly()->SendInfo();
	this->GetLunHuiWangChuan()->SendInfo();
	this->GetRoleMaterial()->SendAllInfo();
	this->GetRoleMaterial()->SendOtherInfo();
	this->GetFaBao()->SendFaBaoDrawInfo();
	this->GetRealmBreak()->SendAllInfo();
	this->GetRealmBreak()->SendItemInfo();
	this->SendCloudArenaSeasonHighestData();
	this->GetSalary()->SendHistoryChivalrousInfo();
	this->GetCustomInstruction()->SendInstructionInfo();
	this->GetRoleGuildInborn()->SendAllInfo();
	this->GetRoleHeartSkill()->SendAllInfo();
	this->SendTopLevelAllInfo();
	this->SendQuitGuildInfo();
	this->GetNetherWorldPurgatory()->SendInfo();
	this->GetPetGodFB()->SendInfo();
	this->GetActiveSystem()->SendWeekInfo();
	this->GetActiveSystem()->OnSendSpecialActiveInfo();
	this->GetRoleTianDiDaoHen()->SendInfo();
	this->GetRoleGiveGift()->SendRoleInfo();
	this->GetPrestigeTask()->SendAllInfo();
	this->SendResourceDownload();
	this->SendFangFraudInfo();
	this->GetRoleAttrPellet()->OnSendAllInfo();
	this->GetActiveSystem()->SendTurntableInfo();
	this->GetAccessoriesmentManager()->SendAccessoriesMentInfo();
	this->GetAccessoriesmentManager()->SendSpecialEquiplist();
	this->GetPetMonopoly()->SendSpecialInfo();
	this->SendCapacityTargetInfo();
	this->GetRoleWanLingFB()->SendInfo();
	this->SendAutoTalkInfo(true);
	this->GetRoleNewLifeSkill()->SendAllInfo();
	this->GetRoleNewLifeSkill()->SendPropsAllInfo();
	this->GetTreasureTask()->SendInfo();
	this->GetRolePeakToken()->SendInfo();
	this->GetRoleNewLevelComplement()->SendInfo();
	this->GetRoleWhereIsTheFairy()->SendAllInfo();
	this->SendPingJiaInfo();
	this->GetZaoHuaEquipManager()->SendRefineInfo();
	this->GetZaoHuaEquipManager()->LongZhuSendInfo();
	this->GetRoleHongMengTianJie()->SendInfo();
	this->GetNewCourageChallenge()->SendAllInfo();
	this->GetRoleHuanShou()->OnSendBagAllInfo();
	this->GetRoleHuanShou()->SendHuanShouAllInfo();
	this->GetRoleHuanShou()->SendOtherInfo();
	this->GetRoleGuildTask()->SendInfo();
}

void RoleModuleManager::SendHearsayInfo(int op_type, int param1)
{
	Protocol::SCHearsayInfo info;
	info.op_type = op_type;
	info.param1 = param1;

	this->NetSend(&info, sizeof(info));
}

void RoleModuleManager::CollectRoleBattleData(RoleBattleData* out_data, bool need_partner, 
	bool must_use_max_hp_mp /* = true */, bool need_pet /* = true */, bool use_system_food /* = false */)
{
	if (NULL == out_data) return;

	out_data->is_auto = this->GetCommonData().battle_is_auto;
	out_data->role_stand_row = this->GetCommonData().battle_position;
	out_data->is_robot = 0;
	out_data->auto_first_move_type = this->GetCommonData().battle_auto_first_move_type;
	out_data->auto_first_skill_id = this->GetCommonData().battle_auto_first_skill_id;
	out_data->auto_first_skill_level = this->GetCommonData().battle_auto_first_skill_level;
	out_data->auto_second_move_type = this->GetCommonData().battle_auto_second_move_type;
	out_data->auto_second_skill_id = this->GetCommonData().battle_auto_second_skill_id;
	out_data->auto_second_skill_level = this->GetCommonData().battle_auto_second_skill_level;

	bool is_use_cur_hp_mp = !m_role->GetSceneStatus()->IsInAutoRecoverScene();
	if (must_use_max_hp_mp)
	{
		is_use_cur_hp_mp = false;
	}

	// 获取人物数据
	this->GetRoleBattleData(&out_data->role_character_data, is_use_cur_hp_mp, use_system_food);

	// 获取宠物数据
	out_data->pet_num = 0;
	out_data->fight_pet_idx = -1;
	if (need_pet)
	{
		this->GetPetBattleData(ROLE_BATTLE_PET_NUM, &out_data->pet_num,
			out_data->pet_character_data, &out_data->fight_pet_idx, is_use_cur_hp_mp);
	}
	
	// 获取伙伴数据
	out_data->huanshou_level = this->GetRoleHuanShou()->GetFushenLevel();
	out_data->partner_num = 0;
	if (need_partner)
	{
		this->GetPartnerBattleData(ARRAY_ITEM_COUNT(out_data->partner_character_data),
			&out_data->partner_num, out_data->partner_character_data, is_use_cur_hp_mp);
	}	

	// 获取装备数据
	out_data->weapon_attr_num = 0;
	this->GetEquipmentManager()->GetWeaponAttr(BATTLE_WEAPON_ATTR_NUM,
		&out_data->weapon_attr_num, out_data->weapon_attr_list);

	out_data->weapon_sp_num = 0;
	out_data->other_sp_num = 0;
	this->GetEquipmentManager()->GetEquipSpecialEffect(BATTLE_WEAPON_EFFECT_NUM,
		&out_data->weapon_sp_num, out_data->weapon_sp_list, BATTLE_OTHER_EQUIP_EFFECT_NUM,
		&out_data->other_sp_num, out_data->other_sp_list);

	// 获取灵骑特效数据
	this->GetSmartMountsSystem()->GetUserSpecialEffectList(BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);
	// 获取勋章数据
	this->GetMedal()->GetBattleDate(out_data);
	// 获得家族灵脉数据
	this->GetRoleGuildInborn()->GetPassiveSkillByType(GUILD_INBORN_ADD_TARGET_TYPE_ROLE, BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);
	//心法主动技能
	out_data->heart_skill_is_vaild = 1;
	out_data->heart_skill_count = 0;
	this->GetRoleHeartSkill()->GetEquipInitativeSkill(ARRAY_ITEM_COUNT(out_data->heart_skill_list), &out_data->heart_skill_count, out_data->heart_skill_list);
	//心法被动技能
	this->GetRoleHeartSkill()->GetEquipPassiveSkill(BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);

	// 获取法宝数据
	out_data->fabao_num = 0;
	this->GetFaBao()->GetFaBaoList(&out_data->fabao_num,out_data->fabao_list);

	//配饰特效
	this->GetAccessoriesmentManager()->GetUserSpecialEffectList(BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);
	
	//修为特效
	this->GetXiuWei()->OnGetBattleEffect(XIU_WEI_CHARACTER_TYPE_ROLE, BATTLE_OTHER_EQUIP_EFFECT_NUM,  &out_data->other_sp_num, out_data->other_sp_list);

	//星图特效
	this->GetStarChart()->OnGetRoleBattleEffect(BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);

#ifdef _DEBUG
	assert(out_data->fabao_num <= BATTLE_FABAO_NUM_MAX);
#endif

	out_data->appearance = m_appearance;
	GuildHonorBattleManager::Instance().GetGuildHonorRankkbuff(this->GetRole()->GetGuildID(), out_data->guild_buff_list);
	out_data->top_level = this->GetRole()->GetTopLevel();

	out_data->pet_helper_num = 0;

	out_data->jieyi_num = 0;
	this->GetJieyiList(&out_data->jieyi_num, out_data->jieyi_uid_list, ARRAY_ITEM_COUNT(out_data->jieyi_uid_list));
	
	out_data->jieyi_skill_num = 0;
	this->GetJieyiSkill(&out_data->jieyi_skill_num, out_data->jieyi_skill_list, ARRAY_ITEM_COUNT(out_data->jieyi_skill_list));

	this->AddJinjieSpidToRoleBattleData(out_data, this->GetProfAdvanceSpecialEffectId());

	out_data->soaring_skill_id = 0;
	out_data->soaring_skill_lv = 0;
	this->GetSoaringSkill(&out_data->soaring_skill_id, &out_data->soaring_skill_lv);

	out_data->qingyuan_target_uid = this->GetQingyuanTargetUID();
	out_data->qingyuan_skill_num = 0;
	this->GetQingyuanSkill(&out_data->qingyuan_skill_num, out_data->qingyuan_skill_list, ARRAY_ITEM_COUNT(out_data->qingyuan_skill_list));

	out_data->career_suppress_value = this->GetCareerSuppressValue();
	out_data->change_card_skill_id = 0;
	out_data->change_card_seq = 0;
	this->GetRoleNewLifeSkill()->GetChangeCardSkill(out_data->change_card_skill_id, out_data->change_card_seq);

	out_data->role_extra_capability = this->GetCapability()->GetRoleExtraCapability();

	out_data->hallow_num = 0;
	out_data->fight_hallow_idx = -1;
	this->GetBattleHallows(&out_data->hallow_num, out_data->hallow_list, 
		&out_data->fight_hallow_idx, 
		ARRAY_ITEM_COUNT(out_data->hallow_list));

	//幻兽
	out_data->huanshou_skill_info.Reset();
	this->GetRoleHuanShou()->OnGetFuShenSkill(&out_data->huanshou_skill_info, out_data->other_sp_list, BATTLE_OTHER_EQUIP_EFFECT_NUM, &out_data->other_sp_num);
}

void RoleModuleManager::AddJinjieSpidToRoleBattleData(RoleBattleData* out_data, int jinjie_spid)
{
 	if (NULL == out_data) return;
	if (jinjie_spid <= 0) return;

	if (out_data->other_sp_num < ARRAY_ITEM_COUNT(out_data->other_sp_list))
	{
		out_data->other_sp_list[out_data->other_sp_num++] = jinjie_spid;
	}

	for (int i = 0; i < out_data->pet_num && i < ARRAY_ITEM_COUNT(out_data->pet_character_data); ++i)
	{
		PetBattleData& pbd = out_data->pet_character_data[i];
		for (int k = 0; k < ARRAY_ITEM_COUNT(pbd.other_sp_list); ++k)
		{
			if (0 == pbd.other_sp_list[k])
			{
				pbd.other_sp_list[k] = jinjie_spid;
				break;
			}
		}
	}

	for (int i = 0; i < out_data->pet_helper_num && i < ARRAY_ITEM_COUNT(out_data->pet_helper_character_data); ++i)
	{
		PetBattleData& pbd = out_data->pet_helper_character_data[i].pet_data;
		for (int k = 0; k < ARRAY_ITEM_COUNT(pbd.other_sp_list); ++k)
		{
			if (0 == pbd.other_sp_list[k])
			{
				pbd.other_sp_list[k] = jinjie_spid;
				break;
			}
		}
	}

	for (int i = 0; i < out_data->partner_num && i < ARRAY_ITEM_COUNT(out_data->partner_character_data); ++i)
	{
		PartnerBattleData& pbd = out_data->partner_character_data[i];
		if (pbd.other_sp_num < ARRAY_ITEM_COUNT(pbd.other_sp_list))
		{
			pbd.other_sp_list[pbd.other_sp_num++] = jinjie_spid;
		}
	}
}

bool RoleModuleManager::GetRoleBattleData(BattleCharacterData* out_data, bool is_use_cur_hp_mp, bool use_system_food)
{
	if (NULL == out_data)
	{
		return false;
	}

	out_data->Reset();

	out_data->character_id = this->GetRole()->GetUID();
	out_data->character_type = BATTLE_CHARACTER_TYPE_ROLE;
	out_data->level = this->GetRole()->GetLevel();
	F_STRNCPY(out_data->name, this->GetRole()->GetName(), sizeof(out_data->name));

	out_data->profession = this->GetRole()->GetProfession();
	out_data->race = RACE_TYPE_HUMAN;

	out_data->is_use_cur_hp_mp = (is_use_cur_hp_mp ? 1 : 0);

	ItemID weapon_id = 0;
	short weapon_type = WEAPON_TYPE_NONE;
	this->GetEquipmentManager()->GetCurWeapon(&weapon_id, &weapon_type);
	out_data->weapon_type = static_cast<char>(weapon_type);
	out_data->weapon_id = weapon_id;

	memcpy(out_data->attr_list, m_attr_list.m_attrs, sizeof(out_data->attr_list));

	ItemID element_crystal_id = this->GetEquipmentManager()->GetElementCrystalItemId();
	int crstal_remain_times = this->GetEquipmentManager()->GetElementCrystalRemainTimes();
	if (element_crystal_id > 0 && crstal_remain_times > 0)
	{
		const ItemBase* element_item = ITEMPOOL->GetItem(element_crystal_id);
		if (NULL != element_item)
		{
			const ElementCrystal* element_crystal = (const ElementCrystal*)element_item;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = element_crystal->GetElementFire();
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WATER] = element_crystal->GetElementWater();
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WIND] = element_crystal->GetElementWind();
			out_data->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = element_crystal->GetElementEarth();
			out_data->wear_element_type = element_crystal->GetElementType();
		}
		else
		{
			out_data->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
			out_data->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
		}
	}
	else
	{
		out_data->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
		out_data->attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
		out_data->attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
		out_data->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
	}

	this->AddGmAttr(out_data);
	if (use_system_food)
	{
		this->MinusOriginFoodAttr(out_data);
		this->AddFoodAttr(out_data);
	}
	
	for (int i = BATTLE_ATTR_ELEMENT_BEGIN; i < BATTLE_ATTR_ELEMENT_END; ++i)
	{		
		out_data->attr_list[i] = GetMax(out_data->attr_list[i], 0);
		out_data->attr_list[i] = GetMin(out_data->attr_list[i], BATTLE_ELEMENT_MAX);	
	}

	for (int skill_idx = 0; skill_idx < MAX_SKILL_NUM && out_data->skill_num < MAX_BATTLE_SKILL_NUM; ++skill_idx)
	{
		RoleSkill::SkillItem* skill = this->GetRoleSkill()->GetSkill(skill_idx);
		if (NULL == skill || skill->Invalid()) continue;
		if (skill->skill_show_level <= 0) continue;

		out_data->skill_list[out_data->skill_num].skill_id = skill->skill_id;
		out_data->skill_list[out_data->skill_num].skill_level = skill->skill_show_level;
		++out_data->skill_num;
	}
	out_data->capability = this->GetCapability()->GetRoleCapability();
	out_data->fly_flag = this->GetFlyUpSystem()->GetFlyUpFlag();


	return true;
}

bool RoleModuleManager::GetPetBattleData(short max_pet_num, short* out_pet_num, PetBattleData* out_data_list, 
	short* out_fight_pet_idx, bool is_use_cur_hp_mp)
{
	if (NULL == out_pet_num || NULL == out_data_list || NULL == out_fight_pet_idx) return false;

	int fight_pet_idx = -1;
	int standby_pet_idx1 = -1;
	int standby_pet_idx2 = -1;
	int standby_pet_idx3 = -1;
	int standby_pet_idx4 = -1;
	int standby_pet_idx5 = -1;

	this->GetPet()->GetFightIndex(&fight_pet_idx, &standby_pet_idx1, &standby_pet_idx2, &standby_pet_idx3, &standby_pet_idx4, &standby_pet_idx5);

	*out_pet_num = 0;

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetPet()->GetBattleCharacterData(fight_pet_idx, &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_fight_pet_idx = *out_pet_num;
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetPet()->GetBattleCharacterData(standby_pet_idx1, &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetPet()->GetBattleCharacterData(standby_pet_idx2, &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetPet()->GetBattleCharacterData(standby_pet_idx3, &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetPet()->GetBattleCharacterData(standby_pet_idx4, &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}

	if (*out_pet_num < max_pet_num)
	{
		if (this->GetPet()->GetBattleCharacterData(standby_pet_idx5, &out_data_list[*out_pet_num], is_use_cur_hp_mp))
		{
			*out_pet_num += 1;
		}
	}
	
	return true;
}

bool RoleModuleManager::GetPartnerBattleData(int max_partner_num, short* out_partner_num, PartnerBattleData* out_data_list, bool is_use_cur_hp_mp)
{
	if (NULL == out_partner_num || NULL == out_data_list) return false;

	static int fight_partner_list[SIDE_MAX_ROLE_NUM - 1];
	max_partner_num = GetMin(SIDE_MAX_ROLE_NUM - 1, max_partner_num);

	int fight_partner_num = 0;
	this->GetPartner()->GetSynFightPartnerIdList(max_partner_num, fight_partner_list, &fight_partner_num);

	for (int i = 0; i < fight_partner_num && i < max_partner_num; ++i)
	{
		if (this->GetPartner()->GetBattleCharacterData(fight_partner_list[i], &out_data_list[*out_partner_num], fight_partner_list, fight_partner_num, is_use_cur_hp_mp))
		{
			UNSTD_STATIC_CHECK(ARRAY_LENGTH(BATTLE_POS_RELATION) == SIDE_MAX_ROLE_NUM);
			out_data_list[*out_partner_num].battle_column = BATTLE_POS_RELATION[i + 1];
			*out_partner_num += 1;
		}
	}

	return true;
}

void RoleModuleManager::AddGmAttr(BattleCharacterData* out_data)
{
	for (int i = 0; i < BATTLE_ATTR_MAX; ++i)
	{
		if (BATTLE_ATTR_CUR_HP == i)
		{
			out_data->attr_list[i] += m_common_data_param.gm_attr_list[BATTLE_ATTR_MAX_HP];
		}
		else if (BATTLE_ATTR_CUR_MP == i)
		{
			out_data->attr_list[i] += m_common_data_param.gm_attr_list[BATTLE_ATTR_MAX_MP];
		}
		else
		{
			out_data->attr_list[i] += m_common_data_param.gm_attr_list[i];
		}
	}
}

void RoleModuleManager::MinusOriginFoodAttr(BattleCharacterData* out_data)
{
	this->GetRoleNewLifeSkill()->MinusOriginFoodAttr(out_data);
}

void RoleModuleManager::AddFoodAttr(BattleCharacterData* out_data)
{
	RoleNewLifeSkill::AddSystemFoorAttr(out_data);
}

void RoleModuleManager::MakeAppearanceData()
{
	m_appearance.Reset();
	m_appearance.avatar_type = m_role->GetAvatarType();
	m_appearance.prof_base = m_role->GetAppearanceProf();
	m_appearance.color = m_role->GetColor();
	m_appearance.advance_times = m_role->GetAdvanceTimes();
	m_appearance.headshot_id = m_role->GetHeadshotID();
	this->GetEquipmentManager()->GetCurWeapon(&m_appearance.weapon_id, NULL, &m_appearance.sub_weapon_id);
	this->GetSurface()->GetWearSurfaceList(m_appearance.wear_surface);
	m_appearance.smart_mounts_index = this->GetSmartMountsSystem()->GetCurRideSmartMounts();
	m_appearance.is_use_advanced_flag = this->GetSmartMountsSystem()->GetCurRideSmartMountsImage();
	m_appearance.is_use_change_card = this->GetRoleNewLifeSkill()->GetUseChangeCard();
	m_appearance.change_card = this->GetRoleNewLifeSkill()->GetNewLifeSkillPropsChangeCard();
	m_appearance.fly_flag = this->GetFlyUpSystem()->GetFlyUpFlag();
	m_appearance.zaohua_app_id = this->GetZaoHuaEquipManager()->GetVigorous();
	m_appearance.is_use_huanshou = this->GetRoleHuanShou()->GetCurUseImage();
	m_appearance.huanshou_fushen_id = this->GetRoleHuanShou()->GetFuShenID();
	m_appearance.reserve_ll = 0;
}

void RoleModuleManager::NoticeNum(int errornum)
{
#ifdef _DEBUG
	if(errornum == 100012/*errornum::EN_INPUT_PARAM_ERROR*/)gamelog::g_log_notice.printf(LL_INFO, "user[%d,%s] notice[%d] trace:[%s]", this->GetUid(), this->GetRoleName(), errornum, PrintStackBackTrace(false).c_str());
#endif
	m_role->NoticeNum(errornum);
}

bool RoleModuleManager::OnRoleCmd(const std::string & cmd,unsigned long long create_time)
{
	bool ret = false;

	std::istringstream iss;
	iss.str(cmd);

	std::string c;
	if ((iss >> c).fail()) return ret;

	DBRoleCommandHandlerMap::iterator it = m_role_cmd_handler_map.find(c);
	if (it != m_role_cmd_handler_map.end())
	{
		std::string error_msg;
		DBRoleCommandHandler f = it->second;
		ret = (this->*f)(iss, error_msg, create_time);

		gamelog::g_log_role_cmd.printf(LL_INFO, "OnRoleCmd role[%d,%s] cmd[%s] result[%s] error_msg[%s]",
			m_role->GetUID(), m_role->GetName(), cmd.c_str(), ret ? "true" : "false", error_msg.c_str());
	}

	return ret;
}

void RoleModuleManager::SetGetRoleCmdInfo(time_t next_time, long long start_cmd_id)
{
	if (start_cmd_id > m_start_role_cmd_id)
	{
		m_start_role_cmd_id = start_cmd_id;
	}

	m_next_get_role_cmd_time = next_time;

	gamelog::g_log_role_cmd.printf(LL_INFO, "RoleModuleManager::SetGetRoleCmdInfo role[%d,%s] next_time[%u] start_cmd_id[%lld]",
		this->GetUid(), this->GetRoleName(), m_next_get_role_cmd_time, m_start_role_cmd_id);
}

void RoleModuleManager::GetRoleCmdNow()
{
	m_next_get_role_cmd_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	gamelog::g_log_role_cmd.printf(LL_INFO, "RoleModuleManager::GetRoleCmdNow role[%d,%s] next_time[%u]", this->GetUid(),
		this->GetRoleName(), m_next_get_role_cmd_time);
}

bool RoleModuleManager::IsXunBaoFirstDraw(int draw_type)
{
	if (draw_type < 0 || draw_type >= (int)sizeof(m_common_data_param.xun_bao_first_draw_flag) * 8)
	{
		return false;
	}

	return !(m_common_data_param.xun_bao_first_draw_flag & (1LL << draw_type));
}

void RoleModuleManager::SetXunBaoFirstDraw(int draw_type)
{
	if (draw_type < 0 || draw_type >= (int)(sizeof(m_common_data_param.xun_bao_first_draw_flag) * 8))
	{
		return;
	}

	m_common_data_param.xun_bao_first_draw_flag |= (1LL << draw_type);
}

int RoleModuleManager::GetXunBaoFirstDrawTimes(int draw_type)
{
	if (draw_type < 0 || draw_type >= ARRAY_LENGTH(m_common_data_param.xun_bao_first_draw_times))
	{
		return 0;
	}

	return m_common_data_param.xun_bao_first_draw_times[draw_type];
}

void RoleModuleManager::AddXunBaoFirstDrawTimes(int draw_type, int add_times)
{
	if (draw_type < 0 || draw_type >= ARRAY_LENGTH(m_common_data_param.xun_bao_first_draw_times))
	{
		return;
	}

	m_common_data_param.xun_bao_first_draw_times[draw_type] += add_times;
	if (m_common_data_param.xun_bao_first_draw_times[draw_type] > XUN_BAO_FIRST_DRAW_TIMES)
	{
		m_common_data_param.xun_bao_first_draw_times[draw_type] = XUN_BAO_FIRST_DRAW_TIMES;
	}

	RoleActivityCoolBreeze * cool_breeze = static_cast<RoleActivityCoolBreeze *>
		(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL != cool_breeze)
	{
		cool_breeze->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_XUNBAO_NUM, add_times, 0);
	}
}

bool RoleModuleManager::IsXunBaoSecondDraw(int draw_type)
{
	if (draw_type < 0 || draw_type >= (int)sizeof(m_common_data_param.xun_bao_second_draw_flag) * 8)
	{
		return false;
	}

	return !(m_common_data_param.xun_bao_second_draw_flag & (1LL << draw_type));
}

void RoleModuleManager::SetXunBaoSecondDraw(int draw_type)
{
	if (draw_type < 0 || draw_type >= (int)(sizeof(m_common_data_param.xun_bao_second_draw_flag) * 8))
	{
		return;
	}

	m_common_data_param.xun_bao_second_draw_flag |= (1LL << draw_type);
}

int RoleModuleManager::GetXunBaoDrawCumulativeTimes(int group_id)
{
	int index = group_id - 1;
	if (0 <= index && index < ARRAY_LENGTH(m_common_data_param.xun_bao_group_draw_times))
	{
		return m_common_data_param.xun_bao_group_draw_times[index];
	}
	
	return 0;
}

void RoleModuleManager::SetXunBaoDrawCumulativeTimes(int group_id, int times)
{
	int index = group_id - 1;
	if (0 <= index && index < ARRAY_LENGTH(m_common_data_param.xun_bao_group_draw_times))
	{
		m_common_data_param.xun_bao_group_draw_times[index] = times;
	}
}

RoleCommonSaveXunBao * RoleModuleManager::GetXunBaoInfo(int draw_type, bool set_dirty)
{
	RoleCommonSave * rcs = this->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XUN_BAO);
	if (rcs == NULL)
	{
		return NULL;
	}

	RoleCommonSaveXunBao * xun_bao_info = rcs->GetTypeData<RoleCommonSaveXunBao>(draw_type, true);
	if (xun_bao_info == NULL)
	{
		return NULL;
	}

	if (set_dirty)
	{
		rcs->SetDataChange(draw_type);
	}

	return xun_bao_info;
}

void RoleModuleManager::SetXunBaoInfoDirty(int draw_type)
{
	this->GetXunBaoInfo(draw_type);
}

void RoleModuleManager::SendXunBaoInfo()
{
	Protocol::SCXunBaoDrawTimesInfo protocol;
	for (int i = 0; i < ARRAY_LENGTH(protocol.draw_times); ++i)
	{
		protocol.draw_times[i] = 0;
	}
	protocol. first_draw_flag=0;
	protocol.second_draw_flag=0;		//!< 寻宝id 是否获取第二次首抽奖励标志

	for (int i = 0; i < ARRAY_LENGTH(protocol.free_draw_time); ++i)
	{
		protocol.free_draw_time[i] = 0;
	}

	for (int i = 0; i < ARRAY_LENGTH(protocol.xi_you_floor_curr_times); ++i)
	{
		protocol.xi_you_floor_curr_times[i] = 0;
	}


	for (int i = 0; i < ARRAY_LENGTH(m_common_data_param.xun_bao_group_draw_times) && i < ARRAY_LENGTH(protocol.draw_times); ++i)
	{
		protocol.draw_times[i] = m_common_data_param.xun_bao_group_draw_times[i];
	}
	protocol.first_draw_flag = m_common_data_param.xun_bao_first_draw_flag;
	protocol.second_draw_flag = m_common_data_param.xun_bao_second_draw_flag;

	memset(protocol.free_draw_time, 0, sizeof(protocol.free_draw_time));
	std::set<int> all_draw_type = LOGIC_CONFIG->GetXunBaoConfig()->GetXunBaoAllDrawTypeCfg();
	for (std::set<int>::iterator iter = all_draw_type.begin(); iter != all_draw_type.end(); ++iter)
	{
		int draw_type = *iter;
		if (draw_type < 0 || draw_type >= ARRAY_LENGTH(protocol.free_draw_time))
		{
			continue;
		}

		protocol.free_draw_time[draw_type] = 0;
		RoleCommonSaveXunBao * xun_bao_info = this->GetXunBaoInfo(draw_type, false);
		if (xun_bao_info == NULL)
		{
			continue;
		}

		if (xun_bao_info->free_time == 0)
		{
			XunBaoDrawCfg * draw_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetXunBaoDrawCfg(draw_type);
			if (draw_cfg == NULL)
			{
				continue;
			}

			if (draw_cfg->free_draw_interval > 0)
			{
				xun_bao_info->free_time = EngineAdapter::Instance().Time() + draw_cfg->free_draw_interval;
			}

			if (draw_cfg->first_free_draw_interval > 0)
			{
				xun_bao_info->free_time = EngineAdapter::Instance().Time() + draw_cfg->first_free_draw_interval;
			}

			
			this->SetXunBaoInfoDirty(draw_type);
		}

		protocol.free_draw_time[draw_type] = xun_bao_info->free_time;
	}

	for (std::set<int>::iterator iter = all_draw_type.begin(); iter != all_draw_type.end(); ++iter)
	{
		int draw_type = *iter;
		if (draw_type < 0 || draw_type >= ARRAY_LENGTH(protocol.xi_you_floor_curr_times))
		{
			continue;
		}

		RoleCommonSaveXunBao * xun_bao_info = this->GetXunBaoInfo(draw_type, false);
		if (xun_bao_info == NULL)
		{
			continue;
		}

		protocol.xi_you_floor_curr_times[draw_type] = xun_bao_info->xi_you_floor_curr_times;
	}

	this->NetSend(&protocol, sizeof(protocol));
}

bool RoleModuleManager::ConsumeSuffList(const std::vector<ItemExtern::ItemConsumeConfig> & stuff_list,const char * reson)
{
	if (reson == NULL)
	{
		reson = "";
	}

	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(this->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		this->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return false;
	}

	if (consume_list.need_gold_num > 0 && !this->GetMoney()->GoldMoreThan(consume_list.need_gold_num))
	{
		this->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return false;
	}

	if (consume_list.count > 0 && !this->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, reson))
	{
		return false;
	}

	if (consume_list.need_gold_num > 0 && !this->GetMoney()->UseGold(consume_list.need_gold_num, reson))
	{
		return false;
	}

	return true;
}

void RoleModuleManager::SetBraveGroundMaxReachedLevel(int reached_level)
{
	if (reached_level > m_common_data_param.brave_ground_max_reached_level)
	{
		m_common_data_param.brave_ground_max_reached_level = reached_level;
		EventHandler::Instance().OnBraveGroundPassNewLevel(this->GetRole(), m_common_data_param.brave_ground_max_reached_level);
		this->SendBraveGroundInfo();
	}
	if (reached_level > m_common_data_param.brave_ground_max_reached_level_by_week)
	{
		m_common_data_param.brave_ground_max_reached_level_by_week = reached_level;
		EventHandler::Instance().OnBraveGroundPassLevelChange(this->GetRole(), m_common_data_param.brave_ground_max_reached_level_by_week);
	}
}

int RoleModuleManager::GetBraveGroundMaxLevelByWeek()
{
	return m_common_data_param.brave_ground_max_reached_level_by_week;
}

void RoleModuleManager::SendBraveGroundInfo()
{
	Protocol::SCBraveGroundRoleInfo pro;
	pro.max_reached_level = m_common_data_param.brave_ground_max_reached_level;
	pro.sao_dang_reward_level = m_common_data_param.brave_ground_sao_dang_reward_level;
	pro.is_fetch_sao_dang_reward = m_common_data_param.brave_ground_sao_dang_is_fetch_sao_dang_reward;
	this->NetSend(&pro, sizeof(pro));
}

void RoleModuleManager::BraveGroundOnWeekChange()
{
	//A3 需求54971  去掉了扫荡

}

void RoleModuleManager::OnBraveGroundResetByFlag()
{
	//A3 需求54971  去掉了扫荡
}

void RoleModuleManager::CheckHuanJieSeasonChange()
{
	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity == NULL)
	{
		return;
	}

	int season_num = activity->GetSeasonNum();
	if (season_num == 0)
	{
		return;
	}

	if (m_common_data_param.huan_jie_season_num == 0)
	{
		m_common_data_param.huan_jie_season_num = season_num;
	}

	if (m_common_data_param.huan_jie_season_num != season_num)
	{
		gamelog::g_log_huan_jie_zhan_chang.printf(LL_INFO, "%s line:%d act_season_num:%d role_season_num:%d", __FUNCTION__, __LINE__, season_num, m_common_data_param.huan_jie_season_num);

		m_common_data_param.huan_jie_season_num = season_num;
		this->OnHuanJieSeasonChange();
	}
}

void RoleModuleManager::OnHuanJieSeasonChange()
{
	mRoleConvertShop->OnHuanJieChange();
}

bool RoleModuleManager::HasRemainShanHaiBossDayTimes()
{
	int daily_reward_times = this->GetShanHaiBossChallengeTime();
	return daily_reward_times < LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().day_time;
}

void RoleModuleManager::SetShanHaiBossChallengeDayId()
{
	if (m_common_data_param.shan_hai_boss_challenge_time < LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().day_time)
	{
		m_common_data_param.shan_hai_boss_challenge_day_id = EngineAdapter::Instance().DayID();
	}
}

bool RoleModuleManager::IsChallengeShanHaiBoss(unsigned char cfg_seq)
{
	if (0 > cfg_seq || cfg_seq >= MAX_UINT8) return false;

	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		if (cfg_seq == m_common_data_param.shan_hai_boss_is_challenge_seq[i])
		{
			return true;
		}
	}
	return false;
}

void RoleModuleManager::ResetShanHaiSomeData(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	if ((UInt32)now_second >= m_common_data_param.shan_hai_boss_next_reset_time)
	{
		m_common_data_param.shan_hai_boss_next_reset_time = ShanHaiBossManager::Instance().GetNextRefreshTime();
		for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
		{
			m_common_data_param.shan_hai_boss_is_challenge_seq[i] = MAX_UINT8;
		}
	}
}

void RoleModuleManager::OnKillShanHaiMonster(int monster_id, int cfg_seq)
{
	if (m_common_data_param.shan_hai_boss_challenge_day_id == EngineAdapter::Instance().DayID() || 0 == m_common_data_param.shan_hai_boss_challenge_day_id)
	{
		m_common_data_param.shan_hai_boss_challenge_time++;
	}
	m_common_data_param.shan_hai_boss_challenge_day_id = 0;
	m_common_data_param.shan_hai_boss_week_challenge_time++;

	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		if (MAX_UINT8 == m_common_data_param.shan_hai_boss_is_challenge_seq[i])
		{
			m_common_data_param.shan_hai_boss_is_challenge_seq[i] = cfg_seq;
			break;
		}
	}
	m_common_data_param.shan_hai_boss_pass_times++;
}

void RoleModuleManager::GmResetShanHaiDailyChallenge()
{
	m_common_data_param.shan_hai_boss_challenge_time = 0;
}

void RoleModuleManager::ResetSingleChatNumData()
{
	m_common_data_param.single_chat_num_last_time = 0;
	memset(m_common_data_param.single_chat_num_list, 0, sizeof(m_common_data_param.single_chat_num_list));
}

unsigned int RoleModuleManager::GetSingleChatNumTime()
{
	return m_common_data_param.single_chat_num_last_time;
}

int RoleModuleManager::GetSingleChatNumNum()
{
	int num = 0;
	for (int i = 0; i < SINGLE_CHAT_NUM_LIMIT_MAX; ++i)
	{
		if (0 < m_common_data_param.single_chat_num_list[i])
		{
			num++;
		}
	}
	return num;
}

bool RoleModuleManager::IsInSingleChatNumList(int to_uid)
{
	for (int i = 0; i < SINGLE_CHAT_NUM_LIMIT_MAX; ++i)
	{
		if (to_uid == m_common_data_param.single_chat_num_list[i])
		{
			return true;
		}
	}
	return false;
}

void RoleModuleManager::SetSingleChatNumTime(unsigned int time)
{
	m_common_data_param.single_chat_num_last_time = time;
}

void RoleModuleManager::AddSingleChatNumUser(int to_uid)
{
	for (int i = 0; i < SINGLE_CHAT_NUM_LIMIT_MAX; ++i)
	{
		if (to_uid == m_common_data_param.single_chat_num_list[i])
		{
			return;
		}
	}

	for (int idx = 0; idx < SINGLE_CHAT_NUM_LIMIT_MAX; ++idx)
	{
		if (0 == m_common_data_param.single_chat_num_list[idx])
		{
			m_common_data_param.single_chat_num_list[idx] = to_uid;
			break;
		}
	}
}

void RoleModuleManager::AddTodayQuitGuildCount()
{
	m_common_data_param.today_quit_guild_count++;
// 	if (m_common_data_param.today_quit_guild_count >= ONE_DAY_QUIT_GUILD_LIMIT_NUM)
// 	{
// 		m_common_data_param.next_join_guild_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + ONE_DAY_QUIT_GUILD_LIMIT_TIME;
// 	}
	this->SendQuitGuildInfo();
}

bool RoleModuleManager::CanApplyJoinGuild()
{
	if (static_cast<unsigned int>(EngineAdapter::Instance().Time()) <= m_common_data_param.next_join_guild_timestamp)
	{
		return false;
	}
	return true;
}

void RoleModuleManager::SendQuitGuildInfo()
{
	Protocol::SCGuildQuitInfo info;
	info.quit_count = m_common_data_param.today_quit_guild_count;
	info.next_join_time = m_common_data_param.next_join_guild_timestamp;
	this->NetSend(&info, sizeof(info));
}

int RoleModuleManager::GetMaxChongZhiRecord()
{
	int max = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_common_data_param.chong_zhi_record); ++i)
	{
		if (m_common_data_param.chong_zhi_record[i] > max)
		{
			max = m_common_data_param.chong_zhi_record[i];
		}
	}

	return max;
}

void RoleModuleManager::OnAddChongZhiRecord(int chongzhi)
{
	int index = m_common_data_param.next_chong_zhi_record_index % ARRAY_LENGTH(m_common_data_param.chong_zhi_record);
	m_common_data_param.chong_zhi_record[index] = (unsigned int)chongzhi;
	m_common_data_param.next_chong_zhi_record_index += 1;
}

bool RoleModuleManager::GmBuyItem(std::istringstream & iss)
{
	std::string error_str;
	return this->CmdBuyItem(iss, error_str, EngineAdapter::Instance().Time());
}

bool RoleModuleManager::GmBuyItem2(int buy_item_type)
{
	std::map<int, int> m;	
	m[BUY_TIME_TYPE_ZHEN_PIN_SHANG_PU] = ROLE_COMMON_SAVE_DATA_TYPE_ZHEN_PIN_SHANG_PU;
	m[BUY_TIME_TYPE_LUCKY_BAG_BATCH] = ROLE_COMMON_SAVE_DATA_TYPE_LUCK_BAG_BATCH;
	m[BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_2] = ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_2;
	m[BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_2] = ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI_2;
	m[BUY_TIME_TYPE_XIAN_SHI_BAO_HE_2] = ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_2;
	m[BUY_TIME_TYPE_JIN_YI_TIAN_XIANG] = ROLE_COMMON_SAVE_DATA_TYPE_JIN_YI_TIAN_XIANG;
	m[BUY_TIME_TYPE_YUAN_QI_YAN_HUO] = ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_YAN_HUO;
	m[BUY_TIME_TYPE_LUCKY_BLIND_BOX] = ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BLIND_BOX;
	m[BUY_TIME_TYPE_DUAN_YANG_ZI_XUAN] = ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_ZI_XUAN;
	m[BUY_TIME_TYPE_DUAN_YANG_FU_LI] = ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_FU_LI;
	m[BUY_TIME_TYPE_JIN_LONG_JU_BAO] = ROLE_COMMON_SAVE_DATA_TYPE_JIN_LONG_JU_BAO;
	m[BUY_TIME_TYPE_SHEN_SHOU_EN_ZE] = ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_EN_ZE;
	m[BUY_TIME_TYPE_QI_XI_TE_HUI] = ROLE_COMMON_SAVE_DATA_TYPE_QI_XI_TE_HUI;
	m[BUY_TIME_TYPE_XIAN_SHI_BAO_HE_3] = ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_3;
	m[BUY_TIME_TYPE_XIAN_SHI_BAO_HE_4] = ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_4;
	m[BUY_TIME_TYPE_PET_EXCHANGE_SHOP] = ROLE_COMMON_SAVE_DATA_TYPE_PET_EXCHANGE_SHOP;
	m[BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_3] = ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_3;
	m[BUY_TIME_TYPE_SHEN_CI_HUA_FU] = ROLE_COMMON_SAVE_DATA_TYPE_SHEN_CI_HUA_FU;
	m[BUY_TIME_TYPE_TIAN_YI_YAO_SHI] = ROLE_COMMON_SAVE_DATA_TYPE_TIAN_YI_YAO_SHI;
	m[BUY_TIME_TYPE_DAN_BI_FAN_LI] = ROLE_COMMON_SAVE_DATA_TYPE_DAN_BI_FAN_LI;

	m[BUY_TIME_TYPE_ZHUI_YUE_SHANG_DIAN] = ROLE_COMMON_SAVE_DATA_TYPE_ZHUI_YUE_SHANG_DIAN;
	
	int chong_zhi_gold = 0;
	unsigned int param[2] = {};
	long long uuid = 0;
	bool b = false;
	if (buy_item_type == 0)
	{
		for (std::map<int, int>::iterator iter = m.begin(); iter != m.end(); ++iter)
		{
			buy_item_type = iter->first;
			int save_type = iter->second;
			b = CmdBuyRecordManager::Instance().GetAnyCmdBuyRecordUniqueIdAndNeedGold(this->GetRole(), save_type, uuid, chong_zhi_gold);
			if (b)
			{
				break;
			}
		}
	}
	else
	{
		std::map<int, int>::iterator iter = m.find(buy_item_type);
		if (iter!=m.end())
		{
			int save_type = iter->second;
			b = CmdBuyRecordManager::Instance().GetAnyCmdBuyRecordUniqueIdAndNeedGold(this->GetRole(), save_type, uuid, chong_zhi_gold);
		}

	}

	if (b)
	{
		LongLongToUInt(uuid, param);
		std::string s = STRING_SPRINTF("%d %d %u %u", chong_zhi_gold, buy_item_type, param[0], param[1]);
		std::istringstream ss;
		ss.str(s);
		return GmBuyItem(ss);
	}

	return false;
}

void RoleModuleManager::GmShanHaiBossAddSoulFragment(int star)
{
	if (0 >= star && star > SHAN_HAI_BOSS_MAX_GRADE)
	{
		return;
	}

	for (int i = 0; i < SHAN_HAI_BOSS_MAX_TYPE; ++i)
	{
		m_common_data_param.shan_hai_boss_soul_fragment[star - 1][i] += 1;
	}
}

void RoleModuleManager::GmResetShanHaiChanllengeCount()
{
	m_common_data_param.shan_hai_boss_challenge_time = 0;
	m_common_data_param.shan_hai_boss_week_challenge_time = 0;
}

bool RoleModuleManager::FindEmptyCrossGroupIndex(ARG_OUT int* index)
{
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; ++i)
	{
		if (m_common_data_param.cross_chat_group_list[i] > 0) continue;
		
		if (NULL != index)
		{
			*index = i;
		}

		return true;
	}

	return false;
}

bool RoleModuleManager::IsInCreateChatGroupCD(ARG_OUT unsigned int* remain_minutes)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now < m_common_data_param.create_chat_group_cd_timestamp)
	{
		if (NULL != remain_minutes)
		{
			unsigned int remain_sec = m_common_data_param.create_chat_group_cd_timestamp - now;
			unsigned int remain_min = remain_sec / SECOND_PER_MIN;
			if (remain_sec % SECOND_PER_MIN > 0)
			{
				remain_min += 1;
			}
			*remain_minutes = remain_min;
		}
		return true;
	}
	return false;
}

void RoleModuleManager::SetCreateChatGroupCD()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_common_data_param.create_chat_group_cd_timestamp = now + LOGIC_CONFIG->GetChatConfig()->GetOtherCfg().create_chat_group_cd_seconds;
}

void RoleModuleManager::UpdateCloudArenaData(unsigned int season_timestamp, int last_season_rank, int season_highest_dan_id, int season_highest_rank, int season_highest_score)
{
	unsigned int old_season_timestamp = m_common_data_param.cloud_arena_season_timestamp;
	m_common_data_param.cloud_arena_season_timestamp = season_timestamp;
	m_common_data_param.cloud_arena_last_season_rank = last_season_rank;
	m_common_data_param.cloud_arena_season_highest_dan_id = season_highest_dan_id;
	m_common_data_param.cloud_arena_season_highest_rank = season_highest_rank;
	m_common_data_param.cloud_arena_season_highest_score = season_highest_score;

	gamelog::g_log_cloud_arena.printf(LL_INFO, "RoleModuleManager::UpdateCloudArenaData role[%d,%s] \
		old_season_timestamp[%u] season_timestamp[%u] last_season_rank[%d] \
		season_highest_dan_id[%d] season_highest_rank[%d] season_highest_score[%d]",
		this->GetRoleUID(), this->GetRoleName(), old_season_timestamp, season_timestamp,
		last_season_rank, season_highest_dan_id, season_highest_rank, season_highest_score);
	
	EventHandler::Instance().OnRoleCloudArenaChange(this->GetRole(), m_common_data_param.cloud_arena_season_highest_score);
	if (old_season_timestamp > 0 && old_season_timestamp != season_timestamp)
	{
		this->GetRoleConvertShop()->OnCloudArenaSeasonChange();
	}

	this->CalculateCloudArenaTitle(last_season_rank, season_timestamp + CLOUD_ARENA_SEASON_DURATION_S);

	this->SendCloudArenaSeasonHighestData();
}

void RoleModuleManager::CalculateCloudArenaTitle(int last_season_rank, unsigned int expire_timestamp)
{
	int new_title_id = 0;
	const CloudArenaSeasonRewardCfg* cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetSeasonRewardCfg(last_season_rank);
	if (NULL != cfg)
	{
		new_title_id = cfg->title_id;
	}

	std::set<int> exclude_title_set;
	exclude_title_set.insert(new_title_id);
	mTitle->RemoveTitleBySrcType(TITLESOURCETYPE_CLOUD_ARENA, __FUNCTION__, TITLE_NOTICE_TYPE_NO_NOTICE, exclude_title_set);

	if (new_title_id > 0)
	{
		unsigned int due_time = expire_timestamp;
		if (-1 != mTitle->AddTitle(new_title_id, __FUNCTION__, due_time))
		{
			gamelog::g_log_cloud_arena.printf(LL_INFO, "RoleModuleManager::CalculateCloudArenaTitle role[%d,%s] add_title_id[%d]",
				this->GetRoleUID(), this->GetRoleName(), new_title_id);
		}
	}
}

void RoleModuleManager::SendCloudArenaSeasonHighestData()
{
	Protocol::SCCloudArenaRoleSeasonHighestData msg;
	msg.season_highest_dan_id = static_cast<short>(m_common_data_param.cloud_arena_season_highest_dan_id);
	msg.season_highest_rank = m_common_data_param.cloud_arena_season_highest_rank;
	msg.season_highest_score = static_cast<short>(m_common_data_param.cloud_arena_season_highest_score);
	msg.is_show_new_shop = this->IsOpenCloudArenaNewShop();
	EngineAdapter::Instance().NetSend(this->GetRole()->GetNetId(), &msg, sizeof(msg));
}

bool RoleModuleManager::IsOpenCloudArenaNewShop()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (m_common_data_param.cloud_arena_new_shop_timestamp > 0u &&
		now >= m_common_data_param.cloud_arena_new_shop_timestamp)
	{
		return true;
	}

	return false;
}

std::string RoleModuleManager::GetRoleInfoString() const
{
	std::string ret;
	ret = STRING_SPRINTF("role[%d %s] ", m_role->GetUID(), m_role->GetName());

	return ret;
}


void RoleModuleManager::AddHongBaoTimes(int _hongbao_type, const char * _reason, int _add_times /*= 1*/)
{
	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13)
	if (_hongbao_type <= 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
	{
		return ;
	}

	m_common_data_param.hongbao_times[_hongbao_type] += _add_times;
	m_common_data_param.get_hongbao_day_times[_hongbao_type] += _add_times;
	m_common_data_param.get_hongbao_total_times[_hongbao_type] += _add_times;

	this->SendHongBaoInfoChange(_hongbao_type);

	gamelog::g_log_hong_bao.printf(LL_INFO, "RoleModuleManager::AddHongBaoTimes  use[%d %s] reseon:%s  hongbao_type:%d  add_times:%d  times:%d  "
		"day_times:%d  total_times:%d",
		m_role->GetUID(), m_role->GetName(), _reason, _hongbao_type, _add_times, m_common_data_param.hongbao_times[_hongbao_type],
		m_common_data_param.get_hongbao_day_times[_hongbao_type] , m_common_data_param.get_hongbao_total_times[_hongbao_type]);
}

bool RoleModuleManager::UseHongBaoTimes(int _hongbao_type, const char * _reason, int _use_times /*= 1*/)
{
	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13)
	if (_hongbao_type <= 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
	{
		return false;
	}

	if (m_common_data_param.hongbao_times[_hongbao_type] - _use_times < 0)
	{
		gamelog::g_log_hong_bao.printf(LL_INFO, "RoleModuleManager::UseHongBaoTimes[error]  use[%d %s] reseon:%s  hongbao_type:%d  use_times:%d  times:%d",
			m_role->GetUID(), m_role->GetName(), _reason, _hongbao_type, _use_times, m_common_data_param.hongbao_times[_hongbao_type]);
		return false;
	}

	m_common_data_param.hongbao_times[_hongbao_type] -= _use_times;

	this->SendHongBaoInfoChange(_hongbao_type);

	gamelog::g_log_hong_bao.printf(LL_INFO, "RoleModuleManager::UseHongBaoTimes  use[%d %s] reseon:%s  hongbao_type:%d  use_times:%d  times:%d",
		m_role->GetUID() , m_role->GetName() , _reason, _hongbao_type , _use_times , m_common_data_param.hongbao_times[_hongbao_type]);

	return true;
}

int RoleModuleManager::GetHongBaoTimes(int _hongbao_type)
{
	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13)
	if (_hongbao_type <= 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
	{
		return 0;
	}

	return m_common_data_param.hongbao_times[_hongbao_type];
}

void RoleModuleManager::SendHongBaoInfo()
{
	Protocol::SCHongBaoSendList info;
	info.count = 0;
	for (int i = 0; i < MAX_HONG_BAO_TYPE_COUNT && i < HONGBAO_TYPE_MAX; ++i)
	{
		if (0 != m_common_data_param.hongbao_times[i] || 
			0 != m_common_data_param.get_hongbao_day_times[i] || 
			0 != m_common_data_param.get_hongbao_total_times[i])
		{
			info.send_list[info.count].type = i;
			info.send_list[info.count].count = m_common_data_param.hongbao_times[i];
			info.send_list[info.count].day_count = m_common_data_param.get_hongbao_day_times[i];
			info.send_list[info.count].total_count = m_common_data_param.get_hongbao_total_times[i];
			++info.count;
		}
	}

	int length = sizeof(info) - (MAX_HONG_BAO_TYPE_COUNT - info.count) * m_common_data_param.hongbao_times[0];
	EngineAdapter::Instance().NetSend(m_role->GetNetId(), (const char *)&info, length);
}

void RoleModuleManager::SendHongBaoInfoChange(int _hongbao_type)
{
	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13);
	if (_hongbao_type <= 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
	{
		return ;
	}


	Protocol::SCHongBaoSendListChange info;
	info.hongbao_type = _hongbao_type;
	info.num = m_common_data_param.hongbao_times[_hongbao_type];
	info.day_count = m_common_data_param.get_hongbao_day_times[_hongbao_type];
	info.total_count = m_common_data_param.get_hongbao_total_times[_hongbao_type];

	EngineAdapter::Instance().NetSend(m_role->GetNetId(), (const char *)&info, sizeof(info));
}

void RoleModuleManager::FetchResourceDownloadReward()
{
	if (m_common_data_param.fetch_resource_download_reward_flag)
	{
		this->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!mKnapsack->CheckForPutList(LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().download_reward_count,
		LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().download_reward))
	{
		this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	mKnapsack->PutList(LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().download_reward_count,
		LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().download_reward, PUT_REASON_FETCH_RESOURCE_DOWNLOAD_REWARD);

	m_common_data_param.fetch_resource_download_reward_flag = 1;

	this->SendResourceDownload();
}

void RoleModuleManager::SendResourceDownload()
{
	Protocol::SCFetchRecourceDownloadReward  reward;
	reward.flag = m_common_data_param.fetch_resource_download_reward_flag;
	EngineAdapter::Instance().NetSend(m_role->GetNetId(), (const char *)&reward, sizeof(reward));
}

void RoleModuleManager::FetchFangFraudReward(int count, short * list)
{
	if (m_common_data_param.fetch_fang_fraud_reward_flag)
	{
		this->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	bool is_succ = true;
	for (int i = 0; i < count && Protocol::CSFangFraudRewardReq::MAX_REQ_NUM; i++)
	{
		int index = list[i];

		if (!LOGIC_CONFIG->GetSundryConfig()->IsFangFraudSucc(index))
		{
			is_succ = false;
			break;
		}
	}

	if (!is_succ)
	{
		this->NoticeNum(errornum::EN_FANG_FRAUD_ERROR);
		return;
	}

	this->NoticeNum(noticenum::NT_FANG_FRAUD_SUCC);

	if (!mKnapsack->CheckForPutList(LOGIC_CONFIG->GetSundryConfig()->GetFangFraudRewardCount(),
		LOGIC_CONFIG->GetSundryConfig()->GetFangFraudReward(), false))
	{
		mKnapsack->SendMail(LOGIC_CONFIG->GetSundryConfig()->GetFangFraudReward(), 
			LOGIC_CONFIG->GetSundryConfig()->GetFangFraudRewardCount(), SEND_MAIL_TYPE_FANG_FRAUD_REWARD, true);
	}
	else
	{
		mKnapsack->PutList(LOGIC_CONFIG->GetSundryConfig()->GetFangFraudRewardCount(),
			LOGIC_CONFIG->GetSundryConfig()->GetFangFraudReward(), PUT_REASON_FANG_FRUAD_REWARD);
	}

	m_common_data_param.fetch_fang_fraud_reward_flag = 1;

	this->SendFangFraudInfo();
}

void RoleModuleManager::SendFangFraudInfo()
{
	Protocol::SCFangFraudRewardInfo info;
	info.flag = m_common_data_param.fetch_fang_fraud_reward_flag;

	EngineAdapter::Instance().NetSend(m_role->GetNetId(), (const char *)&info, sizeof(info));
}

void RoleModuleManager::UpdateCapacityTarget(time_t now)
{
	const RobotOtherCfg & other_cfg = ROBOTCONFIG->GetOtherCfg();
	if (!m_common_data_param.capacity_target_is_finish && m_common_data_param.capacity_target_end_time != 0 && now >= m_common_data_param.capacity_target_end_time)
	{		
		m_common_data_param.capacity_target_value = this->GetRole()->GetCapabilityWithFightPetAndPartner();
		m_common_data_param.capacity_target_robot_value = other_cfg.capacity_target_robot_value;
		m_common_data_param.capacity_target_is_finish = 1;

		this->SendCapacityTargetInfo();
	}

	if (m_common_data_param.capacity_target_is_finish && !m_common_data_param.capacity_target_is_fetch_reward
	    && now >= m_common_data_param.capacity_target_end_time + SECOND_PER_DAY)
	{
		m_common_data_param.capacity_target_is_fetch_reward = 1;

		bool is_win = m_common_data_param.capacity_target_value >= m_common_data_param.capacity_target_robot_value;
		std::vector<ItemConfigData> rewards = is_win ? other_cfg.capacity_target_win_reward : other_cfg.capacity_target_lose_reward;

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();

		FillMailContentParam(contentparam, rewards);
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_capacity_target_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_capacity_target_reward_content, is_win ? 1 : 2);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(this->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}

		this->SendCapacityTargetInfo();
	}
}


void RoleModuleManager::CheckStartCapacityTarget(int task_id)
{
	const RobotOtherCfg & other_cfg = ROBOTCONFIG->GetOtherCfg();
	if (other_cfg.start_capacity_target_task_id == task_id)
	{
		m_common_data_param.capacity_target_can_start = true;
		this->SendCapacityTargetInfo();
	}
}

void RoleModuleManager::CapacityTargetFetchReward()
{
	if (m_common_data_param.capacity_target_is_fetch_reward != 0 || !m_common_data_param.capacity_target_is_finish)
	{
		return;
	}
	const RobotOtherCfg & other_cfg = ROBOTCONFIG->GetOtherCfg();
	std::vector<ItemConfigData> rewards = m_common_data_param.capacity_target_value >= m_common_data_param.capacity_target_robot_value
		? other_cfg.capacity_target_win_reward : other_cfg.capacity_target_lose_reward;

	if (!this->GetKnapsack()->PutVec(rewards, PUT_REASON_CAPACITY_TARGET))
	{
		this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_common_data_param.capacity_target_is_fetch_reward = 1;
	this->SendCapacityTargetInfo();
}

void RoleModuleManager::StartCapacityTarget()
{
	if (!m_common_data_param.capacity_target_can_start || m_common_data_param.capacity_target_end_time != 0)
	{
		return;
	}

	const RobotOtherCfg & other_cfg = ROBOTCONFIG->GetOtherCfg();
	const RobotRoleCfg * robot_role = ROBOTCONFIG->RandRobotByLevel(other_cfg.capacity_target_robot_level, ROBOT_CONFIG_USE_TYPE_MISSION_ROBOT);
	if (NULL == robot_role)
	{
		return;
	}

	int m_profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
	int m_avatar_type = AppearanceConfig::Instance().RandAvatarByProfession(m_profession);
	ROBOTCONFIG->GetRandomName(SEX_ARRAY[m_avatar_type], m_common_data_param.capacity_target_robot_role_name, false);
	m_common_data_param.capacity_target_robot_advance_times = robot_role->advance_times;
	m_common_data_param.capacity_target_robot_avatar_type = m_avatar_type;
	m_common_data_param.capacity_target_robot_headshot_id = 0;
	m_common_data_param.capacity_target_robot_color = rand() % INIT_APPREARANCE_COLOR_NUM;;
	m_common_data_param.capacity_target_robot_prof = robot_role->base_prof;
	m_common_data_param.capacity_target_robot_weapon_id = robot_role->weapon_id;
	m_common_data_param.capacity_target_robot_sub_weapon_id = 0;
	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	m_common_data_param.capacity_target_end_time = now + other_cfg.capacity_target_time;
	this->SendCapacityTargetInfo();
}

void RoleModuleManager::SendCapacityTargetInfo()
{
	const RobotOtherCfg & other_cfg = ROBOTCONFIG->GetOtherCfg();

	Protocol::SCCapacityTargetInfo protocol;
	memcpy(protocol.role_name, m_common_data_param.capacity_target_robot_role_name, sizeof(protocol.role_name));
	protocol.advance_times = m_common_data_param.capacity_target_robot_advance_times;
	protocol.avatar_type = m_common_data_param.capacity_target_robot_avatar_type;
	protocol.headshot_id = m_common_data_param.capacity_target_robot_headshot_id;
	protocol.color = m_common_data_param.capacity_target_robot_color;
	protocol.prof = m_common_data_param.capacity_target_robot_prof;
	protocol.weapon_id = m_common_data_param.capacity_target_robot_weapon_id;
	protocol.sub_weapon_id = m_common_data_param.capacity_target_robot_sub_weapon_id;	
	protocol.capacity_target_end_time= m_common_data_param.capacity_target_end_time;		
	protocol.capacity_target_can_start = m_common_data_param.capacity_target_can_start;
	protocol.capacity_target_is_finish = m_common_data_param.capacity_target_is_finish;
	protocol.capacity_target_is_fetch_reward = m_common_data_param.capacity_target_is_fetch_reward;
	protocol.capacity_target_value = m_common_data_param.capacity_target_value; 
	protocol.capacity_target_robot_value = m_common_data_param.capacity_target_robot_value;
	protocol.capacity_target_begin_time = protocol.capacity_target_end_time != 0 ? protocol.capacity_target_end_time - other_cfg.capacity_target_time : 0;
	this->NetSend(&protocol, sizeof(protocol));
}

void RoleModuleManager::ScoreTaskFinish()
{
	const std::vector<ScoreTaskCfg>& scoretask_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetScoreTask();

	for (unsigned int i = 0; i < scoretask_cfg.size(); ++i)
	{
		if (!m_common_data_param.task_finish.IsBitSet(i) && this->GetRole()->GetLevel() <= scoretask_cfg[i].level && this->GetCapability()->GetRoleCapability() >= scoretask_cfg[i].role_score)
		{
			m_common_data_param.task_finish.SetBit(i);
			gamelog::g_log_score_task.printf(LL_INFO, "%s plat_name[%s] role[%d, %s] level[%d] score[%d] money[%d]", __FUNCTION__,
				m_role->GetPlatName(), this->GetUid(), this->GetRoleName(), this->GetRole()->GetLevel(), this->GetCapability()->GetRoleCapability(), scoretask_cfg[i].money);
		
			if (CrossConfig::Instance().IsHiddenServer())
			{
				World::GetInstWorld()->SyncLogToOriginServer(this->GetRole()->GetUserId().db_index, SYNC_LOG_TYPE_SCORE_TASK, "%s plat_name[%s] role[%d, %s] level[%d] score[%d] money[%d]",
					__FUNCTION__, m_role->GetPlatName(), this->GetUid(), this->GetRoleName(), this->GetRole()->GetLevel(), this->GetCapability()->GetRoleCapability(), scoretask_cfg[i].money);
			}
		}	
	}
}

void RoleModuleManager::GotoSchoolGetReward(int reward_type)
{
	const GotoSchoolOtherConfig & other_cfg = LOGIC_CONFIG->GetGotoSchoolConfig()->GetOtherCfg();
	if (m_common_data_param.goto_school_get_reward_times >= other_cfg.can_get_reward_times)
	{
		return;
	}

	const std::vector<ItemConfigData> * ptr = MapValuesConstPtr(other_cfg.rewards, reward_type);
	if (!ptr)
	{
		return;
	}

	const std::vector<ItemConfigData> & rewards = *ptr;
	if (!this->GetKnapsack()->PutVec(rewards, PUT_REASON_GOTO_SCHOOL))
	{
		this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_common_data_param.goto_school_get_reward_times += 1;
	this->GotoSchoolSendInfo();
}

void RoleModuleManager::GotoSchoolOnDayChange()
{
	m_common_data_param.goto_school_get_reward_times = 0;
	this->GotoSchoolSendInfo();
}

void RoleModuleManager::GotoSchoolSendInfo()
{
	Protocol::SCGotoSchoolInfo protocol;
	protocol.get_reward_times = m_common_data_param.goto_school_get_reward_times;
	this->NetSend(&protocol, sizeof(protocol));
}

void RoleModuleManager::SendGuaJiExpInfo()
{
	Protocol::SCGuaJiExpInfo msg;
	msg.today_add_guaji_exp = m_common_data_param.today_add_guaji_exp;

	this->NetSend(&msg, sizeof(msg));
}

void RoleModuleManager::ReceiveRealNameReward()
{
	if (m_common_data_param.receive_real_name_reward_flag != 0)
	{
		return;
	}

	const std::vector<ItemConfigData>* rewards = GLOBALCONFIG->GetRealNameReward();
	if (NULL == rewards) return;

	if (!this->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*rewards))
	{
		this->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	this->GetKnapsack()->PutVec(*rewards, PUT_REASON_REAL_NAME_REWARD);

	m_common_data_param.receive_real_name_reward_flag = 1;
	this->SendRealNameRewardInfo();
	gamelog::g_log_knapsack.printf(LL_INFO, "%s role[%d, %s] receive_real_name_reward_flag[%d] rewards{%s}", __FUNCTION__,
		this->GetUid(), this->GetRoleName(), m_common_data_param.receive_real_name_reward_flag, Knapsack::RewardsLog(*rewards).c_str());
}

void RoleModuleManager::SendRealNameRewardInfo()
{
	static Protocol::SCRealNameRewardInfo msg;
	msg.receive_flag = m_common_data_param.receive_real_name_reward_flag;
	this->NetSend(&msg, sizeof(msg));
}

void RoleModuleManager::GmSetWaiguaCheckFlag(bool flag)
{
	m_common_data_param.gm_check_waigua_flag = flag ? 1 : 0;
	gamelog::g_log_gm.printf(LL_INFO, "RoleModuleManager::GmSetWaiguaCheckFlag role[%d,%s] flag[%s]",
		this->GetUid(), this->GetRoleName(), flag ? "T" : "F");
}

void RoleModuleManager::GetRoleCmdFromDB(time_t now_second)
{
	if (static_cast<unsigned int>(now_second) <= m_next_get_role_cmd_time)
	{
		return;
	}

	m_next_get_role_cmd_time = static_cast<unsigned int>(now_second) + 90;
	gamelog::g_log_role_cmd.printf(LL_INFO, "RoleModuleManager::GetRoleCmdFromDB role[%d,%s] next_time[%u]", this->GetUid(),
		this->GetRoleName(), m_next_get_role_cmd_time);

	int role_id = this->GetRole()->GetUID();

	RMIGetRoleCmdImpl * ggboi = new RMIGetRoleCmdImpl();
	ggboi->role_id = role_id;

	const rmi::Session * session = &InternalComm::Instance().GetDBHandler().GetAcounterDB();
	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo * cui = CrossUserRegister::Instance().GetCrossUserInfo(role_id);
		if (NULL == cui)
		{
			delete ggboi;
			return;
		}

		UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
		const DBHandler * db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
		if (NULL == db_handler)
		{
			delete ggboi;
			return;
		}

		session = &db_handler->GetAcounterDB();
	}

	RMILoginClient lc;
	lc.__bind_session(*session);
	lc.GetRoleCmd(role_id, m_start_role_cmd_id, ggboi);
}

bool RoleModuleManager::CmdBuyItem(std::istringstream & iss, std::string & error_msg, unsigned long long create_time)
{
	int buy_type = 0;
	int chong_zhi_gold = 0;
	int param1 = 0;
	int param2 = 0;
	long long unique_id = 0;

	if ((iss >> chong_zhi_gold >> buy_type).fail())
	{
		return false;
	}

	switch (buy_type)
	{
	case BUY_TIME_TYPE_ZU_HE_HE_LI_2:
	case BUY_TIME_TYPE_FASHION_SHOP:
	case BUY_TIME_TYPE_ONE_YUAN_GOU:
	case BUY_TIME_TYPE_LING_YU_JI_JIN:
	case BUY_TIME_TYPE_SUMMER_STORE:
	case BUY_TIME_TYPE_SUPER_OFF_SHOP:
	case BUY_TIME_TYPE_SHEN_SHOU_JIANG_LIN:
	case BUY_TIME_TYPE_LUCKY_BAG_NEW:
	case BUY_TIME_TYPE_CAI_SHEN_JU_BAO_NEW:
	case BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_NEW:
	case BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_NEW:
	case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_NEW:
	case BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_NEW:
	case BUY_TIME_TYPE_MYSTERIOUS_TRIAL_NEW:
	case BUY_TIME_TYPE_MONTH_PURCHASE_NEW:
	case BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT:
	case BUY_TIME_TYPE_FIRST_RECHARGE_EXCLUSIVE:
	case BUY_TIME_TYPE_MONTH_CARD:
	case BUY_TIME_TYPE_ZHEN_PIN_SHANG_PU:
	case BUY_TIME_TYPE_LUCKY_BAG_BATCH:
	case BUY_TIME_TYPE_WAN_LING_GONG_YING:
	case BUY_TIME_TYPE_HORCRUX_TIANCHENG:
	case BUY_TIME_TYPE_SHAN_HAI_MET:
	case BUY_TIME_TYPE_SHAN_HAI_MEET:
	case BUY_TIME_TYPE_ZHAOCAIJINBAO:
	case BUY_TIME_TYPE_YUN_ZE_SLOT_MACHINE:
	case BUY_TYME_TYPE_CHAOZHI_FENXIANG:
	case BUY_TIME_TYPE_COLORFUL_DOUBLE:
	case BUY_TIME_TYPE_SHAN_HAI_BAO_XIANG:
	case BUY_TIME_TYPE_SHAN_HAI_BAO_HE:
	case BUY_TIME_TYPE_SHAN_HAI_BAO_DAI:
	case BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_2:
	case BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_3:
	case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_2:
	case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_3:
	case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_4:
	case BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_2:
	case BUY_TIME_TYPE_JIXING_GAOZHAO:
	case BUY_TIME_TYPE_WEEKEND_GIFT:
	case BUY_TIME_TYPE_CHAO_ZHI_GIFT:
	case BUY_TIME_TYPE_YINHU_WENSHI:
	case BUY_TIME_TYPE_YIN_HU_BAI_SUI:
	case BUY_TIME_TYPE_JINGJI_YUEKA:
	case BUY_TIME_TYPE_QING_YUAN_SHIZHUANG:
	case BUY_TIME_TYPE_YAO_XING_SHI_LIAN:
	case BUY_TIME_TYPE_GOD_BEAST_ADVENT:
	case BUY_TIME_TYPE_WAN_LING_ZHU_LI:
	case BUY_TIME_TYPE_JIN_YI_TIAN_XIANG:
	case BUY_TIME_TYPE_YUAN_QI_JISHIBU:
	case BUY_TIME_TYPE_YUAN_QI_YAN_HUO:
	case BUY_TIME_TYPE_DUAN_YANG_ZI_XUAN:
	case BUY_TIME_TYPE_LUCKY_BLIND_BOX:
	case BUY_TIME_TYPE_DUAN_YANG_FU_LI:
	case BUY_TIME_TYPE_JIN_LONG_JU_BAO:
	case BUY_TIME_TYPE_SHEN_SHOU_EN_ZE:
	case BUY_TIME_TYPE_QI_XI_TE_HUI:
	case BUY_TIME_TYPE_PET_EXCHANGE_SHOP:
	case BUY_TIME_TYPE_XIN_FU_LING_CHONG:
	case BUY_TIME_TYPE_SHEN_CI_HUA_FU:
	case BUY_TIME_TYPE_TIAN_YI_YAO_SHI:
	case BUY_TIME_TYPE_NEW_COURAGE_CHALLENGE:
	case BUY_TIME_TYPE_DAN_BI_FAN_LI:
	case BUY_TIME_TYPE_ZHUI_YUE_SHANG_DIAN:
	{
		long long p1 = 0;
		long long p2 = 0;

		if ((iss >> p1 >> p2).fail())
		{
			return false;
		}
		unique_id = (p2 << 32) | p1;
	}
	break;
	default:
	{
		if ((iss >> param1 >> param2).fail())
		{
			return false;
		}
	}
	break;
	}

	bool ret = false;
	switch (buy_type)
	{
	case BUY_TIME_TYPE_NEW_COURAGE_CHALLENGE:
		{
			int err_mum = this->GetNewCourageChallenge()->OnCmdBuyItem(chong_zhi_gold, unique_id);
			if (err_mum == 0)
			{
				ret = true;
			}
			else
			{
				gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
					chong_zhi_gold, unique_id, err_mum);
			}
		}
		break;
		case BUY_TIME_TYPE_DAN_BI_FAN_LI:
		{
			RoleActivityDanBiFanLi * role_activity = static_cast<RoleActivityDanBiFanLi *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);
				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_XIN_FU_LING_CHONG:
		{
			RoleActivityXinFuLingChong * role_activity = static_cast<RoleActivityXinFuLingChong *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnCmdBuyItem(unique_id, chong_zhi_gold);
				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_SHEN_CI_HUA_FU:
		{
			RoleActivityShenCiHuaFu * role_activity = static_cast<RoleActivityShenCiHuaFu *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_ra_shen_ci_hua_fu.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_ZHUI_YUE_SHANG_DIAN:
		{
			RoleActivityZhuiYueShangDian * role_activity = static_cast<RoleActivityZhuiYueShangDian *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_zhui_yue_shang_dian.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_TIAN_YI_YAO_SHI:
		{
			RoleActivityTianYiYaoShi * role_activity = static_cast<RoleActivityTianYiYaoShi *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_SHEN_SHOU_EN_ZE:
			{
				RoleActivityShenShouEnZe * role_activity = static_cast<RoleActivityShenShouEnZe *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_JIN_LONG_JU_BAO:
			{
				RoleActivityJinLongJuBao * role_activity = static_cast<RoleActivityJinLongJuBao *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_jin_long_ju_bao.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_PET_EXCHANGE_SHOP:
			{
			RoleActivityPetExchange * role_activity = static_cast<RoleActivityPetExchange *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_PET_EXCHANGE));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
					gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_QI_XI_TE_HUI:
		{
			RoleActivityQiXiTeHui * role_activity = static_cast<RoleActivityQiXiTeHui *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QI_XI_TE_HUI));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);
				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_qi_xi_te_hui.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_YUAN_QI_YAN_HUO:
			{
				RoleActivityYuanQiYanHuo * role_activity = static_cast<RoleActivityYuanQiYanHuo *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_yuan_qi_yan_huo.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
	case BUY_TIME_TYPE_YUAN_QI_JISHIBU:
		{
			RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);

				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_lucky_bag.printf(LL_INFO, "%s yuan_qi_jishibu line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_JIN_YI_TIAN_XIANG:
			{
				RoleActivityJinYiTianXiang * role_activity = static_cast<RoleActivityJinYiTianXiang *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_jin_yi_tian_xiang.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_WAN_LING_ZHU_LI:
			{
				RoleActivityWanLingZhuLi * role_activity = static_cast<RoleActivityWanLingZhuLi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WANLING_ZHULI));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_lucky_bag.printf(LL_INFO, "%s wanling_zhuli line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_DUAN_YANG_FU_LI:
			{
				RoleActivityDuanYangFuLi * role_activity = static_cast<RoleActivityDuanYangFuLi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_duan_yang_fu_li.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_DUAN_YANG_ZI_XUAN:
			{
				RoleActivityDuanYangZiXuan * role_activity = static_cast<RoleActivityDuanYangZiXuan *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_duan_yang_zi_xuan.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_LUCKY_BLIND_BOX:
			{
				RoleActivityLuckyBlindBox * role_activity = static_cast<RoleActivityLuckyBlindBox *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuy(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_lucky_blind_box.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_YAO_XING_SHI_LIAN:
		{
				int err_mum = this->GetYaoXingShiLian()->OnCmd(chong_zhi_gold, unique_id);
				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_cycleactivity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
		}
		break;
		case BUY_TIME_TYPE_LUCKY_BAG_BATCH:
			{
				RoleActivityLuckyBag * role_activity = static_cast<RoleActivityLuckyBag *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_BAG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem2(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_lucky_bag.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_ZHEN_PIN_SHANG_PU:
		{
			RoleActivityZhenPinShangPu * role_activity = static_cast<RoleActivityZhenPinShangPu *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU));
			if (NULL != role_activity)
			{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_zhen_pin_shang_pu.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										       chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_MONTH_CARD:
		{
			int err_num = this->GetRoleActivityManager()->OnBuyCmdMonthCard(chong_zhi_gold, unique_id);
			if (err_num == 0)
			{
				ret = true;
			}
			else
			{
				gamelog::g_log_month_week_card.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
					chong_zhi_gold, unique_id, err_num);
			}
		}
		break;
		case BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT:
		{
			RoleActivityZhuanShuLiBao * role_activity = static_cast<RoleActivityZhuanShuLiBao *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO));
			if (NULL != role_activity)
			{
				int err_mum = role_activity->OnBuyDayItem(chong_zhi_gold, unique_id);

				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_zhuan_shu_li_bao.printf(LL_INFO, "%s line:%d | role[%d %s] buy_type[%d] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						buy_type, chong_zhi_gold, unique_id, err_mum);
				}
			}
		}
		break;
		case BUY_TIME_TYPE_FIRST_RECHARGE_EXCLUSIVE:
		{
			int err_num = this->GetRoleActivityManager()->OnFirstCmdBuyItem(chong_zhi_gold, unique_id, GetFirstRechargeCmdTypeByBuyType(buy_type));
			if (err_num == 0)
			{
				ret = true;
			}
			else
			{
				gamelog::g_log_first_recharge.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld buy_type:%d ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
					chong_zhi_gold, unique_id, buy_type, err_num);
			}
		}
		break;
		case BUY_TIME_TYPE_SUPER_OFF_SHOP:
			{
				RoleActivitySuperOffShop * role_activity = static_cast<RoleActivitySuperOffShop *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP));
				if (NULL != role_activity)
				{
					long long p1 = 0;
					long long p2 = 0;

					if ((iss >> p1 >> p2).fail())
					{
						return false;
					}

					long long unique_id = (p2 << 32) | p1;
					ret = role_activity->OnCmdBuyItem(unique_id, chong_zhi_gold);
				}
			}
			break;
		case BUY_TIME_TYPE_SUMMER_STORE:
			{
				RoleActivitySummerStore * role_activity = static_cast<RoleActivitySummerStore *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SUMMER_STORE));
				if (NULL != role_activity)
				{
					long long p1 = 0;
					long long p2 = 0;

					if ((iss >> p1 >> p2).fail())
					{
						return false;
					}

					long long unique_id = (p2 << 32) | p1;
					ret = role_activity->OnCmdBuyItem(unique_id, chong_zhi_gold);
				}
			}
			break;
	
		case BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_NEW:
			{
				RoleActivityXianShiBaoDai * role_activity = static_cast<RoleActivityXianShiBaoDai *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_xian_shi_bao_dai.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										      chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_2:
			{
				RoleActivityXianShiBaoDai2 * role_activity = static_cast<RoleActivityXianShiBaoDai2 *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_xian_shi_bao_dai_2.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_NEW:
			{
				RoleActivityXianShiBaoHe * role_activity = static_cast<RoleActivityXianShiBaoHe *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_xian_shi_bao_he.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										       chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_2:
			{
				RoleActivityXianShiBaoHe2 * role_activity = static_cast<RoleActivityXianShiBaoHe2 *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_xian_shi_bao_he_2.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_3:
			{
				RoleActivityXianShiBaoHe3 * role_activity = static_cast<RoleActivityXianShiBaoHe3 *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_xian_shi_bao_he_3.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_XIAN_SHI_BAO_HE_4:
			{
				RoleActivityXianShiBaoHe4 * role_activity = static_cast<RoleActivityXianShiBaoHe4 *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_xian_shi_bao_he_4.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_NEW:
			{
				RoleActivityZhuanShuLiBao * role_activity = static_cast<RoleActivityZhuanShuLiBao *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_zhuan_shu_li_bao.printf(LL_INFO, "%s line:%d | role[%d %s] buy_type[%d] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							buy_type, chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_NEW:
			{
				RoleActivityDingZhiBaoXiang * role_activity = static_cast<RoleActivityDingZhiBaoXiang *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_ding_zhi_bao_xiang.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										      chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_2:
			{
				RoleActivityDingZhiBaoXiang2 * role_activity = static_cast<RoleActivityDingZhiBaoXiang2 *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_ding_zhi_bao_xiang_2.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_3:
			{
				RoleActivityDingZhiBaoXiang3 * role_activity = static_cast<RoleActivityDingZhiBaoXiang3 *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_ding_zhi_bao_xiang_3.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_CAI_SHEN_JU_BAO_NEW:
			{
				RoleActivityCaiShenJuBao * role_activity = static_cast<RoleActivityCaiShenJuBao*>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_cai_shen_ju_bao.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_LUCKY_BAG_NEW:
			{
				RoleActivityLuckyBag * role_activity = static_cast<RoleActivityLuckyBag *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_BAG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_lucky_bag.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										   chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_MYSTERIOUS_TRIAL_NEW:
		{
			int err_mum = this->GetMysteriousTrial()->OnCmdBuyItem(chong_zhi_gold, unique_id);
			if (err_mum == 0)
			{
				ret = true;
			}
			else
			{
				gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
					chong_zhi_gold, unique_id, err_mum);
			}
		}
		break;
		case BUY_TIME_TYPE_SHEN_SHOU_JIANG_LIN:
			{
				RoleActivityShenShouJiangLin * role_activity = static_cast<RoleActivityShenShouJiangLin *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										   chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_LING_YU_JI_JIN:
			{
				RoleActivityLingYuJiJin * role_activity = static_cast<RoleActivityLingYuJiJin *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LING_YU_JI_JIN));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_ling_yu_ji_jin.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										   chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_ONE_YUAN_GOU:
			{
				RoleActivityOneYuanGou * role_activity = static_cast<RoleActivityOneYuanGou *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ONE_YUAN_GOU));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_one_yuan_gou.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										   chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_FASHION_SHOP:
			{
				RoleActivityFashionShop * role_activity = static_cast<RoleActivityFashionShop *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FASHION_SHOP));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_fashion_shop.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
										   chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_CUMULATIVE_RECHARGE:
		{
			RoleActivityCumulativeRecharge * role_activity = static_cast<RoleActivityCumulativeRecharge *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyItem(chong_zhi_gold, param1);	
			}
		}
		break;
		case BUY_TIME_TYPE_ZU_HE_HE_LI_2:
			{
				RoleActivityZuHeHeLi * role_activity = static_cast<RoleActivityZuHeHeLi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZU_HE_HE_LI));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyItem2(chong_zhi_gold, unique_id);
				}
			}
			break;
		case BUY_TIME_TYPE_ZU_HE_HE_LI:
			{
				RoleActivityZuHeHeLi * role_activity = static_cast<RoleActivityZuHeHeLi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZU_HE_HE_LI));
	
				if (NULL != role_activity)
				{
					int err_num = 0;
					ret = role_activity->OnBuyItem(param1, chong_zhi_gold, create_time, err_num);
				}
			}
			break;
		case BUY_TIME_TYPE_SECRET_TREASURE:
		{
			RoleActivitySecretTreasure * role_activity = static_cast<RoleActivitySecretTreasure *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnCmdBuyItem(param1, chong_zhi_gold);
			}
		}
		break;
		case BUY_TIME_TYPE_COW_STORE:
		{
			RoleActivityCowStore * role_activity = static_cast<RoleActivityCowStore *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COW_STORE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnCmdBuyItem(param1, chong_zhi_gold);
			}
		}
		break;
		case BUY_TIME_TYPE_SPRING_FAESTIVAL_STORE:
		{
			RoleActivitySpringFestivalStore * role_activity = static_cast<RoleActivitySpringFestivalStore *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnCmdBuyItem(param1, chong_zhi_gold);
			}
		}
		break;
		case BUY_TIME_TYPE_COW_EXCHANGE:
		{
			RoleActivityCowExchange * role_activity = static_cast<RoleActivityCowExchange *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COW_EXCHANGE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnCmdBuyItem(param1, chong_zhi_gold);
			}
		}
		break;
		case BUY_TIME_TYPE_XIAN_SHI_BAO_DAI:
			{
				RoleActivityXianShiBaoDai * role_activity = static_cast<RoleActivityXianShiBaoDai *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyItemOld(param1, chong_zhi_gold);
				}
			}
			break;

		case BUY_TIME_TYPE_XIAN_SHI_BAO_HE:
			{
				RoleActivityXianShiBaoHe * role_activity = static_cast<RoleActivityXianShiBaoHe *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyItemOld(param1, chong_zhi_gold);
				}
			}
			break;
		case BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO:
			{
				RoleActivityZhuanShuLiBao * role_activity = static_cast<RoleActivityZhuanShuLiBao *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyItemOld(param1, chong_zhi_gold, create_time);
				}
			}
			break;

		case BUY_TIME_TYPE_DING_ZHI_BAO_XIANG:
			{
				RoleActivityDingZhiBaoXiang * role_activity = static_cast<RoleActivityDingZhiBaoXiang *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyItemOld(param1, param2, chong_zhi_gold);
				}
			}
			break;
		case BUY_TIME_TYPE_CAI_SHEN_JU_BAO:
			{
				RoleActivityCaiShenJuBao * role_activity = static_cast<RoleActivityCaiShenJuBao *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyItem(param1, chong_zhi_gold);
				}
			}
			break;
		case BUY_TIME_TYPE_LUCKY_BAG:
			{
				RoleActivityLuckyBag * role_activity = static_cast<RoleActivityLuckyBag *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_BAG));
				if (NULL != role_activity)
				{
					ret = role_activity->OnRandActivityLuckyBagBuy(param1, chong_zhi_gold);
				}
			}
			break;
		case BUY_TIME_TYPE_MYSTERIOUS_TRIAL:
			{
				ret = this->GetMysteriousTrial()->OnBuyEveryDayDiscountReward(param1, chong_zhi_gold, param2);
			}
			break;
		case BUY_TIME_TYPE_MONTH_PURCHASE:
			{	
				ret = (bool)this->GetRoleActivityManager()->OnMonthInvestmentOp(param1, chong_zhi_gold);
			}
			break;
		case BUY_TIME_TYPE_GOD_TOKEN:
			{
				RoleActivityGodToken * role_activity = static_cast<RoleActivityGodToken *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
				if (NULL != role_activity)
				{
					ret = role_activity->OnRAGodTokenBuyToken();
				}
			}
			break;
		case BUY_TIME_TYPE_MONTH_PURCHASE_NEW:
			{
				int err_mum = this->GetRoleActivityManager()->OnCmdBuyMonthInvestment(chong_zhi_gold, unique_id);
				if (err_mum == 0)
				{
					ret = true;
				}
				else
				{
					gamelog::g_log_month_investment.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
						chong_zhi_gold, unique_id, err_mum);
				}
				}
			break;
		case BUY_TIME_TYPE_WAN_LING_GONG_YING:
			{
				RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_HORCRUX_TIANCHENG:
			{
				RoleActivityHorcruxTianCheng * role_activity = static_cast<RoleActivityHorcruxTianCheng *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_SHAN_HAI_MEET:
			{
				RoleActivityShanHaiMeet * role_activity = static_cast<RoleActivityShanHaiMeet *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_SHAN_HAI_MET:
			{
				RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_ZHAOCAIJINBAO:
			{
				RoleActivityZhaoCaiJinBao * role_activity = static_cast<RoleActivityZhaoCaiJinBao *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_YUN_ZE_SLOT_MACHINE:
			{
				RoleActivityYunZeSlotMachine * role_activity = static_cast<RoleActivityYunZeSlotMachine *>
						(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TYME_TYPE_CHAOZHI_FENXIANG:
			{
				RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_COLORFUL_DOUBLE:
			{
				RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;

		case BUY_TIME_TYPE_SHAN_HAI_BAO_XIANG:
			{
				RoleActivityShanHaiBaoXiang * role_activity = static_cast<RoleActivityShanHaiBaoXiang *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[SHAN_HAI_BAO_XIANG] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_SHAN_HAI_BAO_HE:
			{
				RoleActivityShanHaiBaoHe * role_activity = static_cast<RoleActivityShanHaiBaoHe *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[SHAN_HAI_BAO_HE] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_SHAN_HAI_BAO_DAI:
			{
				RoleActivityShanHaiBaoDai * role_activity = static_cast<RoleActivityShanHaiBaoDai *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[SHAN_HAI_BAO_DAI] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_JIXING_GAOZHAO:
			{
				RoleActivityJiXingGaoZhao * role_activity = static_cast<RoleActivityJiXingGaoZhao *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[JIXING_GAOZHAO] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_WEEKEND_GIFT:
			{
				RoleACTWeekendGift * role_activity = static_cast<RoleACTWeekendGift *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WEEKEND_GIFT));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->BuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[WEEKEND_GIFT] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d",
							__FUNCTION__, __LINE__,
							this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_CHAO_ZHI_GIFT:
			{
				RoleACTChaoZhiGift * role_activity = static_cast<RoleACTChaoZhiGift *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->BuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[CHAO_ZHI_GIFT] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d",
							__FUNCTION__, __LINE__,
							this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_YINHU_WENSHI:
			{
				RoleActivityYinHuWenShi * role_activity = static_cast<RoleActivityYinHuWenShi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YINHU_WENSHI));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[YINHU_WENSHI] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_YIN_HU_BAI_SUI:
			{
				RoleActivityYinHuBaiSui * role_activity = static_cast<RoleActivityYinHuBaiSui *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[YIN_HU_BAI_SUI] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
			case BUY_TIME_TYPE_JINGJI_YUEKA:
			{
				RoleActivityJingJiYueKa * role_activity = static_cast<RoleActivityJingJiYueKa *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[JINGJI_YUEKA] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_QING_YUAN_SHIZHUANG:
			{
				RoleActivityQingYuanShiZhuang * role_activity = static_cast<RoleActivityQingYuanShiZhuang *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);

					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[QING_YUAN_SHI_ZHUANG] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		case BUY_TIME_TYPE_GOD_BEAST_ADVENT:
			{
				RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
				if (NULL != role_activity)
				{
					int err_mum = role_activity->OnCmdBuyItem(chong_zhi_gold, unique_id);
						
					if (err_mum == 0)
					{
						ret = true;
					}
					else
					{
						gamelog::g_log_role_activity.printf(LL_INFO, "%s[YI_SHOU_LIN_SHI] line:%d | role[%d %s] chong_zhi_gold:%d unique_id:%lld ret_num:%d", __FUNCTION__, __LINE__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
							chong_zhi_gold, unique_id, err_mum);
					}
				}
			}
			break;
		default:
			break;
	}

	if (ret)
	{
		EventHandler::Instance().OnAddChongzhi(this->GetRole(), chong_zhi_gold, false);
	}
	gamelog::g_log_role_cmd.printf(LL_INFO, "[RoleModuleManager::CmdBuyItem] [result[%d] role_id[%d] buy_type[%d] [cmd:%s]]",
		ret ? 1 : 0, this->GetUid(), buy_type, iss.str().c_str());
	return ret;
}


bool RoleModuleManager::CmdChongzhi(std::istringstream & iss, std::string & error_msg, unsigned long long create_time)
{
	bool succ = false;
	int role_id = 0, chongzhi_num = 0;

	if (!(iss >> role_id >> chongzhi_num).fail())
	{
		if (chongzhi_num > 0)
		{
			m_role->GetRoleModuleManager()->GetRoleActivityManager()->CheckChongzhiExtraRewardByNum(chongzhi_num);
			succ = true;
		}
		else
		{
			error_msg = "Chongzhi_num is error";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool RoleModuleManager::CmdChongzhiBudan(std::istringstream & iss, std::string & error_msg, unsigned long long create_time)
{
	bool succ = false;
	int role_id = 0, chongzhi_num = 0;

	if (!(iss >> role_id >> chongzhi_num).fail())
	{
		if (chongzhi_num > 0)
		{
			EventHandler::Instance().OnAddChongzhi(this->GetRole(), chongzhi_num, false);
			succ = true;
		}
		else
		{
			error_msg = "Chongzhi_num is error";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

void RoleModuleManager::GetJieyiList(ARG_OUT int* jieyi_num, ARG_OUT int* jieyi_uid_list, int max_jieyi_num)
{
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(this->GetUid());
	if (!jie_yi_zu)
	{
		return;
	}

	std::vector<int> vec = jie_yi_zu->GetMemberList();
	for (int i = 0; i < (int)vec.size(); ++i)
	{
		if (*jieyi_num >= max_jieyi_num)
		{
			break;
		}

		if (vec[i] == m_role->GetUID())
		{
			continue;
		}

		jieyi_uid_list[(*jieyi_num)++] = vec[i];
	}
}

void RoleModuleManager::GetJieyiSkill(ARG_OUT int* jieyi_skill_num, ARG_OUT int* jieyi_skill_list, int max_skill_num)
{
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(this->GetUid());
	if (!jie_yi_zu)
	{
		return;
	}

	std::vector<JieYiSkillCfg> vec = LOGIC_CONFIG->GetJieYiConfig()->GetJieYiSkill(jie_yi_zu->GetJieYiMutuaIntimacyMin(), jie_yi_zu->GetJieYiCreateDay());
	for (int i = 0; i < (int)vec.size(); ++i)
	{
		if (*jieyi_skill_num >= max_skill_num)
		{
			break;
		}

		jieyi_skill_list[(*jieyi_skill_num)++] = vec[i].skill_id;
	}
}

int RoleModuleManager::GetProfAdvanceSpecialEffectId()
{
	const CareerAdvanceCfg* curr_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_role->GetProfession(), mAdvanceCareer->GetSubLevel());
	if (NULL == curr_cfg)
	{
		return 0;
	}

	return curr_cfg->all_sp_bonus;
}

int RoleModuleManager::GetRoleFeiShengTimes()
{
	return mFlyUpSystem->GetFlyUpFlag();
}

int RoleModuleManager::GetMiniGameId() const
{
	return m_mini_game_id;
}

void RoleModuleManager::SetMiniGameId(int mini_game_id)
{
	m_mini_game_id = mini_game_id;
}

void RoleModuleManager::SendTuZiKuaiPaoInfo()
{
	Protocol::SCTuZiKuaiPaoInfo protocol;
	protocol.reward_times = this->GetTuZiKuaiPaoRewardTimes();
	this->NetSend(&protocol,sizeof(protocol));
}

int RoleModuleManager::GetTuZiKuaiPaoRewardTimes()
{
	return m_common_data_param.tu_zi_kuai_pao_reward_times;
}

void RoleModuleManager::SetTuZiKuaiPaoRewardTimes(int reward_times)
{
	m_common_data_param.tu_zi_kuai_pao_reward_times = reward_times;
	this->SendTuZiKuaiPaoInfo();
}

void RoleModuleManager::OnDayChangeTuZiKuaiPao()
{
	m_common_data_param.tu_zi_kuai_pao_reward_times = 0;
}

void RoleModuleManager::FetchPingJiaReward()
{
	if (m_common_data_param.pingjia_reward_mark != 0)
	{
		this->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (mKnapsack->CheckForPut2(LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().pinjia_reward, false, false))
	{
		mKnapsack->Put(LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().pinjia_reward, PUT_REASON_FETCH_PINJIA_REWARD);
	}
	else
	{
		mKnapsack->SendMail(&LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().pinjia_reward, 1, SEND_MAIL_TYPE_PINJIA_YOULI, true);
	}

	m_common_data_param.pingjia_reward_mark = 1;

	this->SendPingJiaInfo();
}

void RoleModuleManager::SendPingJiaInfo()
{
	Protocol::SCFetchPingJiaRewardInfo reward;
	reward.flag = m_common_data_param.pingjia_reward_mark;
	EngineAdapter::Instance().NetSend(m_role->GetNetId(), (const char *)&reward, sizeof(reward));
}

void RoleModuleManager::CheckZhouQiInfo()
{
	int zhou_qi_num = 0;
	unsigned int zhou_qi_end_time = 0;
	ZhouQiManager::Instance().GetZhouQiInfo(&zhou_qi_num, &zhou_qi_end_time);
	if (zhou_qi_num == 0)
	{
		return;
	}

	gamelog::g_zhou_qi.printf(LL_INFO, "%s role[%d,%s] role_zhou_qi_info[%d,%u] zhou_qi_info[%d,%u]", __FUNCTION__, this->GetRole()->GetUID(), this->GetRole()->GetName(),
		m_common_data_param.zhou_qi_num, m_common_data_param.zhou_qi_end_time, zhou_qi_num, zhou_qi_end_time);

	if (m_common_data_param.zhou_qi_num == 0)
	{
		m_common_data_param.zhou_qi_num = zhou_qi_num;
		m_common_data_param.zhou_qi_end_time = zhou_qi_end_time;
	}

	if (zhou_qi_end_time != m_common_data_param.zhou_qi_end_time)
	{
		this->OnZhouQiInfoChange(m_common_data_param.zhou_qi_num, m_common_data_param.zhou_qi_end_time);

		m_common_data_param.zhou_qi_num = zhou_qi_num;
		m_common_data_param.zhou_qi_end_time = zhou_qi_end_time;
	}
}

void RoleModuleManager::OnZhouQiInfoChange(int old_zhou_qi, unsigned int old_zhou_end_time)
{
}

void RoleModuleManager::GetSoaringSkill(ARG_OUT UInt16* skill_id, ARG_OUT short* skill_lv)
{
	int fly_up_level = this->GetFlyUpSystem()->GetFlyUpSkillLevel();
	if (fly_up_level > 0)
	{
		const FlyUpSkillCfg * skill_cfg = LOGIC_CONFIG->GetFlyUpSystemConfig()->GetSkillCfg(m_role->GetAppearanceProf());
		if (NULL != skill_cfg)
		{
			if (NULL != skill_id) *skill_id = skill_cfg->ascension_skills;
			if (NULL != skill_lv) *skill_lv = fly_up_level;
		}
	}
	else if (m_common_data_param.gm_soaring_skill_id > 0 && m_common_data_param.gm_soaring_skill_lv > 0)
	{
		if (NULL != skill_id) *skill_id = m_common_data_param.gm_soaring_skill_id;
		if (NULL != skill_lv) *skill_lv = m_common_data_param.gm_soaring_skill_lv;
	}
}

int RoleModuleManager::GetQingyuanTargetUID()
{
	int role_id = this->GetUid();
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
	if (qy)
	{
		return qy->GetOtherSideRoleId(role_id);
	}

	return 0;
}

void RoleModuleManager::GetQingyuanSkill(ARG_OUT int* qingyuan_skill_num, ARG_OUT int* qingyuan_skill_list, int max_skill_num)
{
	int role_id = this->GetUid();
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
	if (qy)
	{
		std::set<int> s = qy->GetSkillIdList(role_id);

		for (std::set<int>::iterator iter = s.begin(); iter != s.end(); ++iter)
		{
			if (*qingyuan_skill_num >= max_skill_num)
			{
				break;
			}

			qingyuan_skill_list[(*qingyuan_skill_num)++] = *iter;
		}
	}
}

int RoleModuleManager::GetCareerSuppressValue()
{
	const CareerAdvanceCfg*  node = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_role->GetProfession(), this->GetAdvanceCareer()->GetSubLevel());
	if (NULL != node)
	{
		return node->job_seq;
	}
	return 0;
}

void RoleModuleManager::GetBattleHallows(ARG_OUT short* hallow_num, 
	ARG_OUT BattleHallowInfo* hallows, ARG_OUT short* fight_hallow_idx, 
	int max_hallow_num)
{
	mHolyEquipManager->GetBattleHallows(hallow_num, hallows, fight_hallow_idx, max_hallow_num);
}

int RoleModuleManager::BuyGiftItem(int param1, int param2, int param3, ARG_OUT int * price)
{
	int ret = 0;

	switch (param1)
	{
	case BUY_GIFT_ITEM_TYPE_NEW_COURAGE_CHALLENGE:
		{
			ret = this->GetNewCourageChallenge()->OnBuyGiftItem(price);
		}
		break;
		case BUY_GIFT_ITEM_TYPE_MONTH_CARD:
		{
			ret = this->GetRoleActivityManager()->OnMonthBuyGiftItem(param2, param3, price);
		}
		break;
		case BUY_GIFT_ITEM_TYPE_LUCKY_BAG:
		{
			RoleActivityLuckyBag * role_activity = static_cast<RoleActivityLuckyBag *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_BAG));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_MYSTERIOUS_TRIAL:
		{
			ret = this->GetMysteriousTrial()->OnBuyGiftItem(param3, price);
		}
		break;
		case BUY_GIFT_ITEM_TYPE_GOD_TOKEN:
		{
			RoleActivityGodToken * role_activity = static_cast<RoleActivityGodToken *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_CAI_SHEN_JU_BAO:
		{
			RoleActivityCaiShenJuBao * role_activity = static_cast<RoleActivityCaiShenJuBao *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG:
		{
			RoleActivityDingZhiBaoXiang * role_activity = static_cast<RoleActivityDingZhiBaoXiang *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO:
		{
			RoleActivityZhuanShuLiBao * role_activity = static_cast<RoleActivityZhuanShuLiBao *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE:
		{
			RoleActivityXianShiBaoHe * role_activity = static_cast<RoleActivityXianShiBaoHe *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_DAI:
		{
			RoleActivityXianShiBaoDai * role_activity = static_cast<RoleActivityXianShiBaoDai *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_ZU_HE_HE_LI:
		{
			RoleActivityZuHeHeLi * role_activity = static_cast<RoleActivityZuHeHeLi *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZU_HE_HE_LI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, param3, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SPRING_FAESTIVAL_STORE:
		{
			RoleActivitySpringFestivalStore * role_activity = static_cast<RoleActivitySpringFestivalStore *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_COW_STORE:
		{
			RoleActivityCowStore * role_activity = static_cast<RoleActivityCowStore *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COW_STORE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_FASHION_SHOP:
		{
			RoleActivityFashionShop * role_activity = static_cast<RoleActivityFashionShop *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FASHION_SHOP));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_LING_YU_JI_JIN:
		{
			RoleActivityLingYuJiJin * role_activity = static_cast<RoleActivityLingYuJiJin *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LING_YU_JI_JIN));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_CUMULATIVE_RECHARG:
		{
			RoleActivityCumulativeRecharge * role_activity = static_cast<RoleActivityCumulativeRecharge *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_XIANG:
		{
			RoleActivityShanHaiBaoXiang * role_activity = static_cast<RoleActivityShanHaiBaoXiang *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_DAI:
		{
			RoleActivityShanHaiBaoDai * role_activity = static_cast<RoleActivityShanHaiBaoDai *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_HE:
		{
			RoleActivityShanHaiBaoHe * role_activity = static_cast<RoleActivityShanHaiBaoHe *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SHAN_HAI_MET:
		{
			RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SHAN_HAI_MEET:
		{
			RoleActivityShanHaiMeet * role_activity = static_cast<RoleActivityShanHaiMeet *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_ZHAO_CAI_JIN_BAO:
		{
			RoleActivityZhaoCaiJinBao * role_activity = static_cast<RoleActivityZhaoCaiJinBao *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SHEN_SHOU_JIANG_LIN:
		{
			RoleActivityShenShouJiangLin * role_activity = static_cast<RoleActivityShenShouJiangLin *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_WAN_LING_GONG_YING:
		{
			RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_HORCRUX_TIANCHENG:
		{
			RoleActivityHorcruxTianCheng * role_activity = static_cast<RoleActivityHorcruxTianCheng *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_YUN_ZE_SLOT_MACHINE:
		{
			RoleActivityYunZeSlotMachine * role_activity = static_cast<RoleActivityYunZeSlotMachine *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_CHAOZHI_FENXIANG:
		{
			RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG_2:
		{
			RoleActivityDingZhiBaoXiang2 * role_activity = static_cast<RoleActivityDingZhiBaoXiang2 *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG_3:
		{
			RoleActivityDingZhiBaoXiang3 * role_activity = static_cast<RoleActivityDingZhiBaoXiang3 *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_2:
		{
			RoleActivityXianShiBaoHe2 * role_activity = static_cast<RoleActivityXianShiBaoHe2 *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_3:
		{
			RoleActivityXianShiBaoHe3 * role_activity = static_cast<RoleActivityXianShiBaoHe3 *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_4:
		{
			RoleActivityXianShiBaoHe4 * role_activity = static_cast<RoleActivityXianShiBaoHe4 *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_DAI_2:
		{
			RoleActivityXianShiBaoDai2 * role_activity = static_cast<RoleActivityXianShiBaoDai2 *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_JIXIANG_GAOZHAO:
		{
			RoleActivityJiXingGaoZhao * role_activity = static_cast<RoleActivityJiXingGaoZhao *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_CHAO_ZHI_GIFT:
		{
			RoleACTChaoZhiGift * role_activity = static_cast<RoleACTChaoZhiGift *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_YINHU_WENSHI:
		{
			RoleActivityYinHuWenShi * role_activity = static_cast<RoleActivityYinHuWenShi *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YINHU_WENSHI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_YIN_HU_BAI_SUI:
		{
			RoleActivityYinHuBaiSui * role_activity = static_cast<RoleActivityYinHuBaiSui *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT:
		{
			RoleActivityZhuanShuLiBao * role_activity = static_cast<RoleActivityZhuanShuLiBao *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftDayItem(param2, param3, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_COLORFUL_DOUBLE:
			{
				RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(param2, price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_YAO_XING_SHI_LIAN:
			{
				ret = this->GetYaoXingShiLian()->OnBuyGiftItem(param2, param3, price);
			}
			break;
		case BUY_GIFT_ITEM_TYPE_WANLING_ZHULI:
			{
				RoleActivityWanLingZhuLi * role_activity = static_cast<RoleActivityWanLingZhuLi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WANLING_ZHULI));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_JIN_YI_TIAN_XIANG:
			{
				RoleActivityJinYiTianXiang * role_activity = static_cast<RoleActivityJinYiTianXiang *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_YUAN_QI_YAN_HUO:
			{
				RoleActivityYuanQiYanHuo * role_activity = static_cast<RoleActivityYuanQiYanHuo *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_YUAN_QI_JISHIBU:
			{
				RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_LUCKY_BAG_BATCH:
			{
				RoleActivityLuckyBag * role_activity = static_cast<RoleActivityLuckyBag *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_BAG));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem2(param2, price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_CHONGZHI:
			{
				ret = this->GetRoleActivityManager()->OnBuyGiftItemChongZhi(param2, price);
			}
			break;
		case BUY_GIFT_ITEM_TYPE_FIRST_RECHARGE_EXCLUSIVE:
			{
				ret = this->GetRoleActivityManager()->OnFirstCmdBuyGiftItem(param2, price);
			}
			break;
		case BUY_GIFT_ITEM_TYPE_LUCKY_BLIND_BOX:
			{
				RoleActivityLuckyBlindBox * role_activity = static_cast<RoleActivityLuckyBlindBox *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(param2, price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_DUAN_YANG_FU_LI:
			{
				RoleActivityDuanYangFuLi * role_activity = static_cast<RoleActivityDuanYangFuLi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(param2, param3, price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_DUAN_YANG_ZI_XUAN:
			{
				RoleActivityDuanYangZiXuan * role_activity = static_cast<RoleActivityDuanYangZiXuan *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_JIN_LONG_JU_BAO:
			{
				RoleActivityJinLongJuBao * role_activity = static_cast<RoleActivityJinLongJuBao *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(param2, price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_SHEN_SHOU_EN_ZE:
			{
				RoleActivityShenShouEnZe * role_activity = static_cast<RoleActivityShenShouEnZe *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(param2, price);
				}
			}
			break;
		case BUY_GIFT_ITEM_TYPE_QI_XI_TE_HUI:
			{
				RoleActivityQiXiTeHui * role_activity = static_cast<RoleActivityQiXiTeHui *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QI_XI_TE_HUI));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(param2, price);
				}
			}
		break;
		case BUY_GIFT_ITEM_TYPE_PET_EXCHANGE_SHOP:
			{
			RoleActivityPetExchange * role_activity = static_cast<RoleActivityPetExchange *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_PET_EXCHANGE));
				if (NULL != role_activity)
				{
					ret = role_activity->OnBuyGiftItem(param2, price);
				}
			}
		break;		
		case BUY_GIFT_ITEM_TYPE_XIN_FU_LING_CHONG:
		{
			RoleActivityXinFuLingChong * role_activity = static_cast<RoleActivityXinFuLingChong *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_QING_YUAN_SHIZHUANG:
		{
			RoleActivityQingYuanShiZhuang * role_activity = static_cast<RoleActivityQingYuanShiZhuang *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_ZHEN_PIN_SHANG_PU:
		{
			RoleActivityZhenPinShangPu * role_activity = static_cast<RoleActivityZhenPinShangPu *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, param3, price);
			}
		}
		break;

		case BUY_GIFT_ITEM_TYPE_TIAN_YI_YAO_SHI:
		{
			RoleActivityTianYiYaoShi * role_activity = static_cast<RoleActivityTianYiYaoShi *>
					(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_SHEN_CI_HUA_FU:
		{
			RoleActivityShenCiHuaFu * role_activity = static_cast<RoleActivityShenCiHuaFu *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_DAN_BI_FAN_LI:
		{
			RoleActivityDanBiFanLi * role_activity = static_cast<RoleActivityDanBiFanLi *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, param3, price);
			}
		}
		break;
		case BUY_GIFT_ITEM_TYPE_ZHUI_YUE_SHANG_DIAN:
		{
			RoleActivityZhuiYueShangDian * role_activity = static_cast<RoleActivityZhuiYueShangDian *>
				(this->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI));
			if (NULL != role_activity)
			{
				ret = role_activity->OnBuyGiftItem(param2, price);
			}
		}
		break;
		default:
			break;
	}

	//直购加了这里也顺便加下,还需要通知客户端也加下逻辑
	UNSTD_STATIC_CHECK(BUY_GIFT_ITEM_TYPE_MAX == 62);
	UNSTD_STATIC_CHECK(BUY_TIME_TYPE_MAX == 79);
	return ret;
}

void RMIGetRoleCmdImpl::GetGetRoleCmdRet(long long role_cmd_id, const std::string & cmd, unsigned long long create_time)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role == NULL)
	{
		return;
	}

	role->GetRoleModuleManager()->SetGetRoleCmdInfo(EngineAdapter::Instance().Time(), role_cmd_id);

	RMIComfirmRoleCmdImpl * ggboi = new RMIComfirmRoleCmdImpl();
	ggboi->cmd = cmd;
	ggboi->create_time = create_time;
	ggboi->role_id = role_id;
	ggboi->role_cmd_id = role_cmd_id;

	const rmi::Session * session = &InternalComm::Instance().GetDBHandler().GetAcounterDB();
	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo * cui = CrossUserRegister::Instance().GetCrossUserInfo(role_id);
		if (NULL == cui)
		{
			delete ggboi;
			return;
		}

		UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
		const DBHandler * db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
		if (NULL == db_handler)
		{
			delete ggboi;
			return;
		}

		session = &db_handler->GetAcounterDB();
	}

	RMILoginClient lc;
	lc.__bind_session(*session);
	lc.ComfirmRoleCmd(true, role_cmd_id, role_id, ggboi);
}

void RMIComfirmRoleCmdImpl::ChangeRoleCmdComfirmTimeRet(int result)
{
	gamelog::g_log_role_cmd.printf(LL_INFO, "[RMIComfirmRoleCmdImpl::ChangeRoleCmdComfirmTimeRet] [result[%d] role_id[%d] role_cmd_id[%lld] [cmd:%s]]",
					result, role_id, role_cmd_id, cmd.c_str());

	if (0 == result)
	{
		bool is_succ = false;

		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (NULL != role)
		{
			is_succ = role->GetRoleModuleManager()->OnRoleCmd(cmd, create_time);
		}

		if (!is_succ)
		{
			RMIComfirmRoleCmdErrorImpl * cgboi = new RMIComfirmRoleCmdErrorImpl();
			cgboi->cmd = cmd;
			cgboi->role_id = role_id;
			cgboi->role_cmd_id = role_cmd_id;
	

			const rmi::Session * session = &InternalComm::Instance().GetDBHandler().GetAcounterDB();

			if (CrossConfig::Instance().IsHiddenServer())
			{
				const CrossUserRegister::CrossUserInfo * cui = CrossUserRegister::Instance().GetCrossUserInfo(role_id);
				if (NULL == cui)
				{
					delete cgboi;
					return;
				}

				UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
				const DBHandler * db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
				if (NULL == db_handler)
				{
					delete cgboi;
					return;
				}

				session = &db_handler->GetAcounterDB();
			}

			RMILoginClient lc;
			lc.__bind_session(*session);
			lc.ComfirmRoleCmd(false, role_cmd_id, role_id, cgboi);

			gamelog::g_log_role_cmd.printf(LL_INFO, "[RMIComfirmRoleCmdErrorImpl role_id[%d],role_cmd_id[%lld],cmd_str[%s]",
							role_id, role_cmd_id, cmd.c_str());
		}
	}
}



//void RMIComfirmRoleCmdErrorImpl::__exception(int error)
//{
//	printf("RMIComfirmRoleCmd __exception: %d !!!\n", error);
//}

void RMIComfirmRoleCmdErrorImpl::ChangeRoleCmdComfirmTimeRet(int result)
{
	gamelog::g_log_role_cmd.printf(LL_INFO, "[ChangeRoleCmdComfirmTimeRet::ChangeRoleCmdComfirmTimeRet] [result[%d] role_id[%d] role_cmd_id[%lld] [cmd:%s]]",
					result, role_id, role_cmd_id, cmd.c_str());
}

