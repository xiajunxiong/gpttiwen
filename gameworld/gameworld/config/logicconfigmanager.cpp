#include "logicconfigmanager.hpp"
#include "gamelog.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "chat/chatconfig.h"
#include "friend/friendconfig.h"
#include "servercommon/serverconfig/configstruct.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include <iosfwd>
#include <sstream>

#include "global/team/teamconfig.hpp"
#include "global/xunbao/xunbaoconfig.hpp"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "global/platformbattle/platformbattleconfig.hpp"
#include "global/server_first_kill/serverfirstkillconfig.hpp"

#include "config/reloadconfigmanager.hpp"

#include "config/otherconfig/guildconfig.hpp"
#include "config/otherconfig/entryconfig.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "config/otherconfig/petinbornconfig.hpp"
#include "config/otherconfig/colosseumconfig.hpp"
#include "config/otherconfig/knapsack_config.hpp"
#include "config/otherconfig/sceneextraconfig.hpp"
#include "config/otherconfig/realmbreakconfig.hpp"
#include "config/otherconfig/professionconfig.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "config/otherconfig/rolematerialconfig.hpp"
#include "config/otherconfig/newlifeskillconfig.hpp"
#include "config/otherconfig/guildweektaskconfig.hpp"
#include "config/otherconfig/roleheartskillconfig.hpp"
#include "config/otherconfig/duchuangzeiyingconfig.hpp"
#include "config/otherconfig/sceneminemonsterconfig.hpp"
#include "config/otherconfig/smartmountssystemconfig.hpp"
#include "config/otherconfig/dreamnoteschallengeconfig.hpp"
#include "config/otherconfig/initialbattleconfig.hpp"
#include "config/otherconfig/roleattrpelletconfig.hpp"
#include "config/otherconfig/hundredgodbossconfig.hpp"
#include "config/otherconfig/huanshouconfig.hpp"

#include "config/activityconfig/monthcardconfig.hpp"
#include "config/activityconfig/worldboss/worldbossconfig.hpp"
#include "config/activityconfig/worldboss/worldboss3config.hpp"
#include "config/activityconfig/silkroads/activitysilkroadsconfig.hpp"
#include "config/activityconfig/mazeracer/activitymazeracerconfig.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "config/activityconfig/braveground/activitybravegroundconfig.hpp"
#include "config/activityconfig/guildanswer/activityguildanswerconfig.hpp"
#include "config/activityconfig/chiefelection/activitychiefelectionconfig.hpp"
#include "config/activityconfig/worldteamarena/activityworldteamarenaconfig.hpp"
#include "config/activityconfig/chiefpeakbattle/activitychiefpeakbattlenconfig.hpp"
#include "config/activityconfig/guildhonorbattle/activityguildhonorbattleconfig.hpp"
#include "config/activityconfig/huanjiezhanchang/activityhuanjiezhanchangconfig.hpp"


#include "config/activityconfig/activityconfig.hpp"
#include "config/activityconfig/chongzhiconfig.hpp"
#include "config/activityconfig/randactivityopencfg.hpp"

#include "config/randactivityconfig/randactivitybonusconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "other/shop/shopconfig.h"
#include "other/pet/petconfig.hpp"
#include "other/currencyconfig.hpp"
#include "other/baby/babyconfig.hpp"
#include "other/miyi/miyiconfig.hpp"
#include "other/title/titleconfig.hpp"
#include "other/train/trainconfig.hpp"
#include "other/fabao/fabaoconfig.hpp"
#include "other/medal/medalconfig.hpp"
#include "other/mentor/shituconfig.hpp"
#include "other/giftranditemconfig.hpp"
#include "other/trevifountionconfig.hpp"
#include "other/escort/escortconfig.hpp"
#include "other/medal/medalfbconfig.hpp"
#include "other/partner/partnerconfig.hpp"
#include "other/surface/surfaceconfig.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/workshop/jewelryconfig.hpp"
#include "other/prestige/prestigeconfig.hpp"
#include "other/workshop/workshopconfig.hpp"
#include "other/workshop/gemstoneconfig.hpp"
#include "other/marriage/marriageconfig.hpp"
#include "other/exchange/exchangeconfig.hpp"
#include "other/funcguide/funcguideconfig.h"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/taskchain/taskchainconfig.hpp"
#include "other/facescore/facescoreconfig.hpp"
#include "other/mapunlock/mapunlockconfig.hpp"
#include "other/fantasyfb/fantasyfbconfig.hpp"
#include "other/advancefb/advancefbconfig.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/collection/collectionconfig.hpp"
#include "other/schooltask/schooltaskconfig.hpp"
#include "other/offlineexp/offlineexpconfig.hpp"
#include "other/monster_wave/monsterwaveconfig.h"
#include "other/treasure_map/treasuremapconfig.h"
#include "other/rolegatherfb/gatherfbconfig.hpp"
#include "other/challengefb/challengefbconfig.hpp"
#include "other/partner/partnerequipmentconfig.hpp"
#include "other/wst_question/wstquestionconfig.hpp"
#include "other/systemnotice/systemnoticeconfig.hpp"
#include "other/researchtask/researchtaskconfig.hpp"
#include "other/activesystem/activesystemconfig.hpp"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "other/world_question/worldquestionconfig.hpp"
#include "other/courseofgrowth/courseofgrowthconfig.hpp"
#include "other/transactiontask/transactiontaskconfig.hpp"
#include "other/couragechallenge/couragechallengeconfig.h"
#include "battle/battle_video_config.hpp"
#include "other/workshop/petequipmentconfig.hpp"
#include "servercommon/performancecatch.hpp"
#include "other/funcpreview/funcpreviewconfig.hpp"
#include "other/skin/skinconfig.hpp"
#include "other/partner/mysterystoreconfig.hpp"
#include "global/shanhaibossmanager/shanhaibossconfig.hpp"
#include "other/wenxianglou/wenxiangloufbconfig.hpp"
#include "other/lyjz/lingyaojiuzhuconfig.hpp"
#include "other/smxj/starkingconfig.hpp"
#include "other/catchfox/catchfoxconfig.hpp"
#include "other/yyyxq/yingyuanyixianqianconfig.hpp"
#include "other/levelcomplement/levelcomplementconfig.hpp"
#include "other/workshop/godequipupconfig.hpp"
#include "other/schoolexercise/schoolexerciseconfig.hpp"
#include "other/workshop/suitlistconfig.hpp"
#include "other/workshop/equipmentjewerlyupgreadeconfig.hpp"
#include "other/petmonopoly/petmonopolyconfig.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuanconfig.hpp"
#include "config/activityconfig/reliclooting/activityreliclootingconfig.hpp"
#include "global/cloudarena/cloudarenaconfig.hpp"
#include "other/mapexploration/mapexplorationconfig.hpp"
#include "other/catchfox/maintaskcatchfoxconfig.hpp"
#include "other/workshop/petsoulequipmentconfig.hpp"
#include "other/rolezhuling/zhulingconfig.hpp"
#include "other/workshop/petgodprintequipmentconfig.hpp"
#include "other/netherworldpurgatory/netherworldpurgatoryconfig.hpp"
#include "other/petgodfb/petgodfbconfig.hpp"
#include "config/activityconfig/hundredghost/hundredghostconfig.hpp"
#include "config/activityconfig/activitybigdipper/activitybigdipperconfig.hpp"
#include "global/tiandidaohen/tiandidaohenconfig.hpp"
#include "other/rolegivegift/rolegivegiftconfig.hpp"
#include "other/partner/partnergifttreeconfig.hpp"
#include "other/duel/duelconfig.hpp"
#include "other/tianming/tianmingconfig.hpp"
#include "other/dujie/dujieconfig.hpp"
#include "other/peishi/peishiconfig.hpp"
#include "other/yinyangeye/yinyangeyeconfig.hpp"
#include "other/wanlingfb/wanlingfbconfig.hpp"
#include "other/pet/petlianyaoconfig.hpp"
#include "global/jieyi/jieyizuconfig.h"
#include "other/cycleactivity/tuzikuaipao/tuzikuaipaoconfig.hpp"
#include "other/cycleactivity/gotoschool/gotoschoolconfig.hpp"

#include "config/logicconfigreloader.hpp"
#include "servercommon/multithread/mutex_guard.h"
#include "common/platform/thread/mutex.h"
#include "common/platform/thread/thread.h"
#include "other/endlesstower/endlesstowerconfig.hpp"
#include "other/dianhuashi/dianhuashiconfig.hpp"

#include "config/otherconfig/friendgiftconfig.hpp"
#include "other/flyupsystem/flyupststemconfig.hpp"
#include "other/mirageterritory/mirageterritoryconfig.hpp"

#include "global/cowreportgoodnews/cowreportgoodnewsconfig.hpp"
#include "other/xiuwei/xiuweiconfig.hpp"
#include "other/equipstar/equipstarconfig.hpp"
#include "other/treasuretask/treasuretaskconfig.hpp"
#include "config/randactivityconfig/randactivitycompensationconfig.hpp"
#include "global/tigerbringsblessing/tigerbringsblessingconfig.hpp"
#include "qingyuan/qingyuanconfig.hpp"
#include "other/fallengod/fallengodconfig.hpp"
#include "other/minigame/minigameconfig.hpp"
#include "global/worldarena/worldarenaconfig.hpp"
#include "config/activityconfig/qingyuanshenghui/activityqingyuanshenghuiconfig.hpp"
#include "servercommon/serverconfig/bigcrossconfig/utaguessconfig.hpp"

#include "other/pet/petbadgeconfig.hpp"
#include "other/peaktoken/peaktokenconfig.hpp"
#include "other/yaoxingshilian/yaoxingshilianconfig.hpp"
#include "other/cycleactivity/whereisthefairy/whereisthefairyconfig.hpp"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengconfig.hpp"
#include "other/cycleactivity/sanxianguidong/sanxianguidongconfig.hpp"
#include "global/zhouqimanager/zhouqiconfig.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddieconfig.hpp"
#include "other/cycleactivity/tiangongxunbao/tiangongxunbaoconfig.hpp"
#include "other/cycleactivity/loversmarket/loversmarketconfig.hpp"
#include "other/cycleactivity/taoquannaji/taoquannajiconfig.hpp"
#include "other/cycleactivity/concentricknot/concentricknotconfig.hpp"
#include "config/otherconfig/lilianconfig.hpp"
#include "config/otherconfig/fastforwardbattleconfig.hpp"
#include "config/otherconfig/kuafuyingxiongluconfig.hpp"
#include "other/zaohuaequip/zaohuaequipconfig.hpp"
#include "other/hongmengtianjie/hongmengtianjieconfig.hpp"
#include "other/shengqi/holyequipmentconfig.hpp"
#include "other/cycleactivity/kejuexam/kejuexamconfig.hpp"
#include "other/cycleactivity/zhouqishop/zhouqishopconfig.hpp"
#include "config/otherconfig/animationspeedconfig.hpp"
#include "config/otherconfig/elementelfconfig.hpp"
#include "other/fuyu/fuyuconfig.hpp"
#include "other/starchart/starchartconfig.hpp"
#include "other/fashioncollection/fashioncollectionconfig.hpp"
#include "other/minigame/minigame_niulang_config.hpp"
#include "other/roleguildtask/roleguildtaskconfig.hpp"
#include "other/pet/petgaimingconfig.hpp"

#define INIT_LOGIC_CONFIG(cfg, path, err)\
	{\
		if (!(cfg).Init((path), (err)))\
		{\
			gamelog::g_log_world.printf(LL_ERROR, err->c_str());\
			check_res_mode_ret = false;\
			if (!check_res_mode)\
			{\
				return false;\
			}\
		}\
	}

LogicConfigManager::LogicConfigManager()
{
	for (int module_type = CONFIG_MODULE_TYPE_MIN; module_type < CONFIG_MODULE_TYPE_COUNT; ++module_type)
	{
		m_logic_config_module_item_list[module_type] = NULL;
	}

	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_RAND_ACTIVITY, RandActivityConfigManager, "randactivityconfig_1.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_EXCHANGE, ExchangeConfig, "exchange_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_COLOSSEUM, ColosseumConfig, "arena_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ESCORT, EscortConfig, "yunbiao.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SHOP, ShopConfig, "shop_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET, PetConfig, "pet_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ROLE_EXP, RoleExpConfig, "roleexp.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CURRENCY, Currencyconfig, "exp_crystal_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PRESTIGE, PrestigeConfig, "prestige_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_COLLECTION, CollectionConfig, "collection.xml");
	//LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ACHIEVEMENT, AchievementConfig, "achievement.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PARTNER, PartnerConfig, "partner_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GIFT_RAND_ITEM, GiftRandItemconfig, "gift_randitem.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TITLE, TitleConfig, "titleconfig.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TRAIN, TrainConfig, "train.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_KNAPSACK, KnapsackConfig, "bag_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WST_QUESTION, WstQuestionConfig, "wst_question_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TREVI_FOUNTION, TreviFountionconfig, "trevi_fountain.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SUNDRY, SundryConfig, "sundries.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PROFESSION, ProfessionConfig, "career_advancements.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_DAY_REWARD, DayRewardConfig, "day_reward.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WORKSHOP, WorkshopConfig, "compose.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GEMSTONE, GemstoneConfig, "jewel_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SURFACECONFIG, SurfaceConfig, "surface_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MARRIAGE, MarriageConfig, "marriage_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_BABY, BabyConfig, "children_cfg.xml"); 
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_JEWELRY_CRAFT, JewelryConfig, "ornament_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GUILD, GuildConfig, "family.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FRIEND, FriendConfig, "friend_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SHI_TU, ShiTuConfig, "mentor_cfg.xml");
	//LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SYSTEM_NOTICE, SystemNoticeConfig, "function_notice.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_COURAGE_CHALLENGE, CourageChallengeConfig, "courage_challenge.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MONSTER_WAVE, MonsterWaveConfig, "monster_cave.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_RANDACTIVITY_OPEN, RandActivityOpenCfg, "randactivityopencfg_1.xml");//TODO
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WILD_BOSS, WildBossConfig, "monster_yewaiboss.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MI_YI, MiYiConfig, "dexter.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SCHOOL_TASK, SchoolTaskConfig, "jobtask.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_OFFLINE_EXP, OfflineExpConfig, "offline_exp.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WORLD_BOSS, WorldBossConfig, "monster_taofaboss.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TREASURE_MAP, TreasureMapConfig, "treasure_map.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TASK_CHAIN, TaskChainConfig, "task_chain.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FABAO, FaBaoConfig, "talisman_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_RESEARCH_TASK, ResearchTaskConfig, "zhenzhong_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MEDAL, MedalConfig, "medal_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ROLE_CRYSTAL_FB, RoleCrystalFbConfig, "crystal_scene.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MAP_UNLOCK, MapUnlockConfig, "world_map.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TEAM_FIGHT, ActivityTeamFightConfig, "team_fight_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MAZE_RACER, ActivityMazeRacerConfig, "maze_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GATHER_FB, RoleGatherFbConfig, "collect_scene.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_BRAVE_GROUND, ActivityBraveGroundConfig, "drop_hundred_floor.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CHONGZHI, ChongzhiConfig, "recharge_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GUILD_FIGHT, ActivityGuildFightConfig, "family_war.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CHIEF_ELECTION, ActivityChiefElectionConfig, "chief_election.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PARTNER_EQUIPMENT, PartnerEquipmentConfig, "partner_equipment_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CHIEF_PEAK_BATTLE, ActivityChiefPeakBattleConfig, "chief_top.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GUILD_HONOR_BATTLE, ActivityGuildHonorBattleConfig, "family_honor.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FUN_OPEN, FunOpenConfig, "funopen.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_COURSE_OF_GROWTH, CourseOfGrowthConfig, "accomplishment.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PLATFORM_BATTLE, PlatformBattleConfig, "leitai_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GUILD_WEEK_TASK, GuildWeekTaskConfig, "family_mission.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CHAT, ChatConfig, "chat.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GUILD_ANSWER, ActivityGuildAnswerConfig, "family_question_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CHALLENGE_FB, ChallengeFbConfig, "challenge_fuben.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FACE_SCORE, FaceScoreConfig, "face_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ADVANCE_FB, AdvanceFbConfig, "career_advancements_scene.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FUNC_GUIDE, FuncGuideConfig, "function_guide.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ENTRY, EntryConfig, "entry_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MEDAL_FB, MedalFbConfig, "medaltask_scene.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SERVER_FIRST_KILL, ServerFirstKillConfig, "first_kill.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TRANSACTION_TASK, TransactionTaskConfig, "transaction_task.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ACTIVE_SYSTEM, ActiveSystemConfig, "activedegree.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WORLD_QUESTION, WorldQuestionConfig, "world_question_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FANTASY_FB, FantasyFbConfig, "monster_dreamland.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TEAM_CFG, TeamConfig, "team_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_XUN_BAO, XunBaoConfig, "treasure_hunt.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ONLY_FIGHT, ActivityOnlyFightConfig, "enemyhundred_war.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ACTIVITY, ActivityConfig, "daily_activity.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_BATTLE_VIDEO, BattleVideoConfig, "videotape.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_HUAN_JIE_ZHAN_CHANG, ActivityHuanJieZhanChangConfig, "illusory_battlefield.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_EQUIPMENT, PetEquipMentConfig, "pet_equipment.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FUNC_PREVIEW_CONFIG, FuncPreviewConfig, "function_notice.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SCORE_SYSTEM, ScoreSystemConfig, "score_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SKIN, SkinConfig, "skin.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MYSTERY_STORE, MysteryStoreConfig, "mystery_store.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SHAN_HAI_BOSS, ShanHaiBossConfig, "monster_shxyboss.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WENXIANGLOU, WenXiangLouFbConfig, "brothel_murder.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_DU_CHUANG_ZEI_YING, DuChuangZeiYingConfig, "bandit_camp.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_LING_YAO_JIU_ZHU, LingYaoJiuZhuFbConfig, "lyjz_story.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_STAR_KING, StarKingFbConfig, "smxj_story.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CATCH_FOX, CatchFoxFbConfig, "catch_fox.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_YINYUANYIXIANQIAN, YYYXQConfig, "yyyxq_story.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_BONUS, RandActivityBonusConfig, "randactivity/extra_bonus.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_LEVEL_COMPLEMENT, RoleLevelComplementConfig, "level_complement.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_DREAM_NOTES_CHALLENGE, DreamNotesChallengeConfig, "dream_notes_challenge.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SCHOOL_EXERCISE, SchoolExerciseConfig, "school_exercise.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SUIT_LIST, SuitListConfig, "suit_list.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_EXTRA_SCENE_CONFIG, SceneExtraConfig, "world_map.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_EQUIP_JEWERLY_UPGREADE, EquipmentJewerlyUpgreadeConfig, "equip_hole_strengthen.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SMART_MOUNTS_SYSTEM, SmartMountsSystemConfig, "bicycle_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_REALM_BREAK, RealmBreakConfig, "break_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_MONOPOLY, PetMonopolyConfig, "pet_island.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_LUN_HUI_WANG_CHUAN, LunHuiWangChuanConfig, "challenge_test.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_RELIC_LOOTING, ActivityRelicLootingConfig, "treasure_contend.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CLOUD_AREA, CloudArenaConfig, "cloud_battlefield.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MATERIAL, MaterialConfig, "material_scene.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_INITIAL_BATTLE, InitialBattleConfig, "npc_demo.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MAP_EXPLORATION, MapExplorationConfig, "map_explore.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MAIN_TASK_CATCH_FOX_FB_CONFIG, MainTaskCatchFoxFbConfig, "main_catch_fox.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_SOUL_EQUIPMENT, PetSoulEquipMentConfig, "pet_weapon.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ROLE_HEART_SKILL, RoleHeartSkillConfig, "heart_skill_cfg.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ZHU_LING, ZhuLingConfig, "equip_spirit.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GOD_PRINT, PetGodPrintEquipMentConfig, "pet_god_print.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_NETHER_WORLD_PURGATORY, NetherWorldPurgatoryConfig, "netherworld_purgatory.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_GOD_FB, PetGodFbConfig, "pet_god_scene.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_HUNDRED_GHOST_CONFIG, HundredGhostConfig, "ghosts_at_night.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MONTH_CARD, MonthCardConfig, "randactivity/month_card.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ACTIVITY_BIG_DIPPER, ActivityBigDipperConfig, "big_dipper.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN, TianDiDaoHenConfig, "god_land_traces.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ROLE_GIVE_GIFT, RoleGiveGiftConfig, "god_land_traces.xml");		//策划不开新表
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PARTNER_GIFT_TREE, PartnerGiftTreeConfig, "partner_inborn.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ROLE_ATTR_PELLET, RoleAttrPelletConfig, "attribute_pellet.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_DUEL, DuelConfig, "duel.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TIAN_MING, TianMingConfig, "peishi.xml");// 天命的配置策划放在了配饰系统里
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_DU_JIE, DuJieConfig, "cross_robbery.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_YIN_YANG_EYE, YinYangEyeConfig, "yinyang_vision.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PEISHI, PeiShiConfig, "peishi.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FRIEND_GIFT, FriendGiftConfig, "friendly_gift.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ENDLESS_TOWER, EndlessTowerConfig, "endless_tower.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_JIE_YI, JieYiConfig, "deep_friendship.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_COW_REPORT_GOOD_NEWS, CowReportGoodNewsConfig, "lucky_cow.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WAN_LING_FB, WanLingFBConfig, "wanling_huaben.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_DIANHUASHI, DianHuaShiConfig, "equipment_dianhua.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_NEW_LIFE_SKILL, NewLifeSkillConfig, "life_system.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FLY_UP, FlyUpSystemConfig, "role_feisheng.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MIRAGE_TERRITORY, MirageTerritoryConfig, "mirage_territory.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SILK_ROADS, ActivitySilkRoadsConfig, "silk_roads.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_XIU_WEI, XiuWeiConfig, "xiuwei.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_EQUIP_STAR, EquipStarConfig, "equip_star.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TREASURE_TASK, TreasureTaskConfig, "treasure_task.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_RA_COMPENSATION, RACompensationConfig, "randactivity/xianshileichongbufa.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TIGER_BRINGS_BLESSING, TigerBringsBlessingConfig, "yinhunafu.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_QING_YUAN, QingYuanConfig, "get_married.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FALLEN_GOD, FallenGodConfig, "dead_god_land.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MINI_GAME, MiniGameConfig, "minigame.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WORLD_ARENA, WorldArenaConfig, "worldarena.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_BADGE, PetBadgeConfig, "pet_badges.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MOUDLE_TYPE_WORLD_TEAM_ARENA, ActivityWorldTeamArenaConfig, "hero_competition.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_LIAN_YAO, PetLianYaoConfig, "pet_lianyao.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PEAK_TOKEN, PeakTokenConfig, "supreme_token.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_QING_YUAN_SHENG_HUI, ActivityQingYuanShengHuiConfig, "Love_meeting.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_YAO_XING_SHI_LIAN, YaoXingShiLianConfig, "pet_tried.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_INBORN, PetInbornConfig, "pet_inborn.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_QINGYUAN_DUIDUIPENG, QingYuanDuiDuiPengConfig, "lovers_match.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_SAN_XING_GUI_DONG, SanXianConfig, "sanxian.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ZHOU_QI, ZhouQiConfig, "period_activity.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_LANTERN_IRDDIE, CycleActLanternRiddleConfig, "lantern_riddle.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WHERE_IS_THE_FAIRY, WhereIsTheFairyConfig, "where_is_the_fairy.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TIAN_GONG_XUN_BAO, TianGongXunBaoConfig, "tiangongxunbao.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_LOVERS_MARKET, LoversMarketConfig, "lovers_market.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TU_ZI_KUAI_PAO, TuZiKuaiPaoConfig, "rabbit_run.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_CONCENTRIC_KNOT, ConcentricKnotConfig, "concentric_knot.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_TAO_QUAN_NA_JI, TaoQuanNaJiConfig, "lasso.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_UTA_GUESS, UTAGuessConfig, "universe_arena.xml");
	//LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_LILIAN, LilianConfig, "experience.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FAST_FORWARD_BATTLE, FastForwardBattleConfig, "rapid_strike.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_HUNDRED_GOD_BOSS, HundredGodBossConfig, "hundred_god_boss.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ZAO_HUA_EQUIP, ZaoHuaEquipConfig, "zaohua_equip_up.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_HONGMENG_TIANJIE, HongMengTianJieConfig, "promeval_paradise.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_HOLY_EQUIP, HolyEquipMentConfig, "shengqi.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_KE_JU_EXAM, CycleKeJuExamConfig, "keju_examination.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_GOTO_SCHOOL, GotoSchoolConfig, "go_to_school.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ZHOU_QI_SHOP, ZhouQiShopConfig, "study_market.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ANIMATION_SPEED, AnimationSpeedConfig, "double_speed_battle.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ELEMENT_ELF, ElementElfConfig, "element_elf.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FU_YU, FuYuConfig, "fuyu.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_KUA_FU_YING_XIONG_LU, KuaFuYingXiongLuConfig, "randactivity/kuafuyingxionglu.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_STAR_CHART_SYSTEM, StarChartConfig, "starmap.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_FASHION_COLLECTION, FashionCollectionConfig, "fashion.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_MINIGAME_NIULANG, MiniGameNiuLangConfig, "cowboy.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_WORLD_BOSS_3, WorldBoss3Config, "monster_tianshen.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_ROLE_GUILD_TASK, RoleGuildTaskConfig, "family_daily_mission.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_HUAN_SHOU, HuanShouConfig, "huanshou.xml");
	LOGIC_CONFIG_NEW_MODULE_ITEM(CONFIG_MODULE_TYPE_PET_GAI_MING, PetGaiMingConfig, "pet_remould.xml");

	UNSTD_STATIC_CHECK(CONFIG_MODULE_TYPE_COUNT == 180);
	
	for (int module_type = CONFIG_MODULE_TYPE_MIN; module_type < CONFIG_MODULE_TYPE_COUNT; ++module_type)
	{
		if (NULL == m_logic_config_module_item_list[module_type])
		{
			assert(0);
		}
	}
}

LogicConfigManager::~LogicConfigManager()
{
	for (int module_type = CONFIG_MODULE_TYPE_MIN; module_type < CONFIG_MODULE_TYPE_COUNT; ++module_type)
	{
		if (NULL != m_logic_config_module_item_list[module_type])
		{
			delete m_logic_config_module_item_list[module_type];
			m_logic_config_module_item_list[module_type] = NULL;
		}
	}
}

LogicConfigManager *gs_logicconfig_manager = NULL;
LogicConfigManager * LogicConfigManager::Instance()
{
	if (NULL == gs_logicconfig_manager)
	{
		gs_logicconfig_manager = new LogicConfigManager();
	}

	return gs_logicconfig_manager;
}

extern LogicConfigManager *g_async_logicconfig_manager;
extern volatile bool g_async_reload_logicconfig_done;
extern MutexLock g_LogicConfigReloadMutex;
void LogicConfigManager::CheckLogicConfigAsyncReload()
{
	if (!g_async_reload_logicconfig_done) return;

	barrier();
	if (NULL == g_async_logicconfig_manager) return; 

	MutexGuard mutex_guard(&g_LogicConfigReloadMutex);

	barrier();
	if (NULL == g_async_logicconfig_manager) return;

	if (NULL != gs_logicconfig_manager)
	{
		delete gs_logicconfig_manager;
	}
	gs_logicconfig_manager = g_async_logicconfig_manager;
	g_async_logicconfig_manager = NULL;
	g_async_reload_logicconfig_done = false;

	gamelog::g_log_world.printf(LL_INFO, "LogicConfigManager::CheckLogicConfigAsyncReload substitute cfg SUCC!!!");
}

bool LogicConfigManager::AsyncReload(const std::string &path, std::string *err)
{
	LogicConfigReloader* reloader = new LogicConfigReloader(path);
	Thread* thread = new Thread();
	bool ret = thread->Run(LogicConfigReloader::StartReload, reloader);
	if (ret)
	{
		gamelog::g_log_world.printf(LL_INFO, "LogicConfigManager::AsyncReload Succ Cur Thread Id[%lu]", thread->GetThreadID());
	}

	thread->Detach();

	return ret;
}

void LogicConfigManager::ReleaseConfig()
{
	delete gs_logicconfig_manager;
	gs_logicconfig_manager = NULL;
}

bool LogicConfigManager::Init(const std::string &path, std::string *err, bool check_res_mode, bool is_from_reload /* = false */)
{
	bool check_res_mode_ret = true;
	bool is_check_reload_times = false;
	long long elapse_time = 0;
	if (is_from_reload)
	{
		is_check_reload_times = (0 != LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().server_check) ? true : false;
	}

	for (int module_type = CONFIG_MODULE_TYPE_MIN; module_type < CONFIG_MODULE_TYPE_COUNT; ++module_type)
	{
		if (NULL == m_logic_config_module_item_list[module_type]) // 模块未初始化，强制报错
		{
			char errinfo[1024] = { 0 };
			SNPRINTF(errinfo, sizeof(errinfo), "LogicConfigManager::Init m_logic_config_module_item_list[%d] == NULL", module_type);
			*err = errinfo;

			gamelog::g_log_world.printf(LL_ERROR, err->c_str());
			check_res_mode_ret = false;
			if (!check_res_mode)
			{
				return false;
			}
		}

		ReloadConfigManager::Instance().OnStartCalcTime(is_check_reload_times, PERFORMANCE_TEST_LOGIC_CONFIG_RELOAD);
		INIT_LOGIC_CONFIG(*m_logic_config_module_item_list[module_type], path + m_logic_config_module_xml_path_list[module_type], err);
		elapse_time += ReloadConfigManager::Instance().OnEndCalcTime(is_check_reload_times, PERFORMANCE_TEST_LOGIC_CONFIG_RELOAD, "LogicConfig module", module_type);
		
		if (!check_res_mode_ret)
			return false;
	}
	if (is_check_reload_times)
	{
		printf("Reload %s elapse[%lld]us=[%lld]ms\n", __FUNCTION__, elapse_time, elapse_time / 1000);
		gamelog::g_log_debug.printf(LL_INFO, "Reload %s elapse[%lld]us=[%lld]ms", __FUNCTION__, elapse_time, elapse_time / 1000);
	}
	if (check_res_mode && !check_res_mode_ret) return false;

	return true;
}

bool LogicConfigManager::Reload(const std::string &path, std::string *err)
{
	LogicConfigManager *temp = new LogicConfigManager();

	if (!temp->Init(path, err, false, true))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != gs_logicconfig_manager) delete gs_logicconfig_manager;

	gs_logicconfig_manager = temp;

	return true;
}


int ReadItemConfigData(TiXmlElement * data_element, std::string str, ItemConfigData ARG_OUT & item_data, bool need_exist)
{
	TiXmlElement * item_element = data_element->FirstChildElement(str.c_str());
	if (!item_element)
	{
		if (need_exist)
		{
			return -__LINE__;
		}
	}
	else
	{
		ItemConfigData item;
		if (!item.ReadConfig(item_element))
		{
			return -__LINE__;
		}

		item_data = item;
	}

	return 0;
}

int ReadItemConfigData2(TiXmlElement * data_element, std::string str, std::vector<ItemConfigData> ARG_OUT & item_data_vec, bool need_exist)
{
	TiXmlElement * item_element = data_element->FirstChildElement(str.c_str());
	if (!item_element)
	{
		if (need_exist)
		{
			return -__LINE__;
		}
	}
	else
	{
		ItemConfigData item;
		if (!item.ReadConfig(item_element))
		{
			return -__LINE__;
		}

		item_data_vec.push_back(item);
	}

	return 0;
}

int ReadItemConfigDataList(TiXmlElement * data_element, std::string str, std::vector<ItemConfigData>ARG_OUT & item_data_vec, bool need_exist)
{
	TiXmlElement * item_list_element = data_element->FirstChildElement(std::string(str + "_list").c_str());
	if (!item_list_element)
	{
		if (need_exist)
		{
			return -__LINE__;
		}
	}
	else
	{
		TiXmlElement * item_element = item_list_element->FirstChildElement(str.c_str());
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -__LINE__;
			}

			item_data_vec.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}

	return 0;
}

int ReadCfgListValue(TiXmlElement * data_element, std::string str, std::vector<int>ARG_OUT & vec, bool is_fliter_zero)
{
	TiXmlElement * value_list_element = data_element->FirstChildElement(str.c_str());
	if (NULL != value_list_element)
	{
		TiXmlElement * node_element = value_list_element->FirstChildElement("node");
		while (node_element)
		{
			int value = 0;
			if (!GetNodeValue(node_element, value))
			{
				return -__LINE__;
			}

			if (is_fliter_zero)
			{
				if (value != 0)
				{
			vec.push_back(value);
				}
			}
			else
			{
				vec.push_back(value);
			}

			
			node_element = node_element->NextSiblingElement();
		}
	}

	return 0;
}

