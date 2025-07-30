#ifndef __LOGIC_CONFIG_MANAGER_HPP__
#define __LOGIC_CONFIG_MANAGER_HPP__

#include "servercommon/configcommon.h"
#include <string>

#define LOGIC_CONFIG_NEW_MODULE_ITEM(module_type, class_name, path)\
{\
	m_logic_config_module_item_list[module_type] = new class_name();\
	m_logic_config_module_xml_path_list[module_type] += "logicconfig/";\
	m_logic_config_module_xml_path_list[module_type] += path;\
}

#define LOGIC_CONFIG_GET_MODULE(module_type, class_name)\
	inline class_name* Get##class_name() const { return (class_name*)m_logic_config_module_item_list[module_type]; }

#define LOGIC_CONFIG LogicConfigManager::Instance()

class ShopConfig;
class PetConfig;
class RoleExpConfig;
class PartnerConfig;
class Currencyconfig;
class PrestigeConfig;
class CollectionConfig;
class GiftRandItemconfig;
class TitleConfig;
class TrainConfig;
class KnapsackConfig;
class WstQuestionConfig;
class TreviFountionconfig;
class SundryConfig;
class ProfessionConfig;
class DayRewardConfig;
class WorkshopConfig;
class GemstoneConfig;
class SurfaceConfig;
class MarriageConfig;
class BabyConfig;
class JewelryConfig;
class GuildConfig;
class FriendConfig;
class ShiTuConfig;
class SystemNoticeConfig;
class CourageChallengeConfig;
class MonsterWaveConfig;
class RandActivityOpenCfg;
class WildBossConfig;
class MedalConfig;
class MiYiConfig;
class SchoolTaskConfig;
class OfflineExpConfig;
class WorldBossConfig;
class EscortConfig;
class TreasureMapConfig;
class TaskChainConfig;
class FaBaoConfig;
class ResearchTaskConfig;
class RoleCrystalFbConfig;
class SceneMineMonsterConfig;
class ActivityTeamFightConfig;
class MapUnlockConfig;
class ActivityMazeRacerConfig;
class RoleGatherFbConfig;
class ActivityBraveGroundConfig;
class ColosseumConfig;
class ExchangeConfig;
class RandActivityConfigManager;
class ChongzhiConfig;
class ActivityGuildFightConfig;
class ActivityChiefElectionConfig;
class PartnerEquipmentConfig;
class ActivityChiefPeakBattleConfig;
class ActivityGuildHonorBattleConfig;
class FunOpenConfig;
class CourseOfGrowthConfig;
class PlatformBattleConfig;
class GuildWeekTaskConfig;
class ChatConfig;
class ActivityGuildAnswerConfig;
class ChallengeFbConfig;
class FaceScoreConfig;
class RobotConfig;
class AdvanceFbConfig;
class FuncGuideConfig;
class EntryConfig;
class MedalFbConfig;
class ServerFirstKillConfig;
class TransactionTaskConfig;
class ActiveSystemConfig;
class WorldQuestionConfig;
class FantasyFbConfig;
class TeamConfig;
class XunBaoConfig;
class ActivityOnlyFightConfig;
class LinkageGiftConfig;
class ActivityConfig;
class BattleVideoConfig;
class ActivityHuanJieZhanChangConfig;
class PetEquipMentConfig;
class FuncPreviewConfig;
class ScoreSystemConfig;
class SkinConfig;
class ShanHaiBossConfig;
class WenXiangLouFbConfig;
class MysteryStoreConfig;
class DuChuangZeiYingConfig;
class LingYaoJiuZhuFbConfig;
class StarKingFbConfig;
class CatchFoxFbConfig;
class YYYXQConfig;
class RandActivityBonusConfig;
class RoleLevelComplementConfig;
class DreamNotesChallengeConfig;
class SchoolExerciseConfig;
class SuitListConfig;
class SceneExtraConfig;
class EquipmentJewerlyUpgreadeConfig;
class SmartMountsSystemConfig;
class RealmBreakConfig;
class PetMonopolyConfig;
class LunHuiWangChuanConfig;
class ActivityRelicLootingConfig;
class CloudArenaConfig;
class MaterialConfig;
class InitialBattleConfig;
class MapExplorationConfig;
class MainTaskCatchFoxFbConfig;
class PetSoulEquipMentConfig;
class RoleHeartSkillConfig;
class ZhuLingConfig;
class PetGodPrintEquipMentConfig;
class NetherWorldPurgatoryConfig;
class PetGodFbConfig;
class HundredGhostConfig;
class MonthCardConfig;
class ActivityBigDipperConfig;
class TianDiDaoHenConfig;
class RoleGiveGiftConfig;
class PartnerGiftTreeConfig;
class CowReportGoodNewsConfig;
class RoleAttrPelletConfig;
class DuelConfig;
class TianMingConfig;
class DuJieConfig;
class PeiShiConfig;
class YinYangEyeConfig;
class FriendGiftConfig;
class EndlessTowerConfig;
class JieYiConfig;
class WanLingFBConfig;
class DianHuaShiConfig;
class NewLifeSkillConfig;
class FlyUpSystemConfig;
class MirageTerritoryConfig;
class ActivitySilkRoadsConfig;
class XiuWeiConfig;
class EquipStarConfig;
class TreasureTaskConfig;
class RACompensationConfig;
class TigerBringsBlessingConfig;
class QingYuanConfig;
class FallenGodConfig;
class MiniGameConfig;
class WorldArenaConfig;
class PetBadgeConfig;
class ActivityWorldTeamArenaConfig;
class PetLianYaoConfig;
class PeakTokenConfig;
class ActivityQingYuanShengHuiConfig;
class YaoXingShiLianConfig;
class PetInbornConfig;
class CycleActLanternRiddleConfig;
class QingYuanDuiDuiPengConfig;
class SanXianConfig;
class ZhouQiConfig;
class WhereIsTheFairyConfig;
class TianGongXunBaoConfig;
class LoversMarketConfig;
class TuZiKuaiPaoConfig;
class TaoQuanNaJiConfig;
class ConcentricKnotConfig;
class UTAGuessConfig;
class LilianConfig;
class HundredGodBossConfig;
class FastForwardBattleConfig;
class ZaoHuaEquipConfig;
class HongMengTianJieConfig;
class HolyEquipMentConfig;
class CycleKeJuExamConfig;
class GotoSchoolConfig;
class ZhouQiShopConfig;
class AnimationSpeedConfig;
class ElementElfConfig;
class FuYuConfig;
class KuaFuYingXiongLuConfig;
class StarChartConfig;
class FashionCollectionConfig;
class MiniGameNiuLangConfig;
class WorldBoss3Config;
class RoleGuildTaskConfig;
class HuanShouConfig;
class PetGaiMingConfig;

class LogicConfigManager
{
	friend class LogicConfigReloader;
	enum MODULE_ITEM_TYPE
	{
		CONFIG_MODULE_TYPE_MIN = 0,
		CONFIG_MODULE_TYPE_SHOP = CONFIG_MODULE_TYPE_MIN,					// 商店
		CONFIG_MODULE_TYPE_PET,												// 宠物
		CONFIG_MODULE_TYPE_ROLE_EXP,										// 角色经验与属性加点
		CONFIG_MODULE_TYPE_CURRENCY,										// 打卡水晶
		CONFIG_MODULE_TYPE_PRESTIGE,										// 声望
		CONFIG_MODULE_TYPE_COLLECTION,										// 藏品
		//CONFIG_MODULE_TYPE_ACHIEVEMENT,										// 成就
		CONFIG_MODULE_TYPE_PARTNER,											// 伙伴
		CONFIG_MODULE_TYPE_GIFT_RAND_ITEM,									// 随机礼包
		CONFIG_MODULE_TYPE_TITLE,											// 称号
		CONFIG_MODULE_TYPE_TRAIN,											// 修炼
		CONFIG_MODULE_TYPE_KNAPSACK,										// 背包
		CONFIG_MODULE_TYPE_WST_QUESTION,									// 万事通的考验
		CONFIG_MODULE_TYPE_TREVI_FOUNTION,									// 许愿池
		CONFIG_MODULE_TYPE_SUNDRY,											// 杂项
		CONFIG_MODULE_TYPE_PROFESSION,										// 职业进阶
		CONFIG_MODULE_TYPE_DAY_REWARD,										// 奖励界面
		CONFIG_MODULE_TYPE_WORKSHOP,										// 工坊
		CONFIG_MODULE_TYPE_GEMSTONE,										// 宝石
		CONFIG_MODULE_TYPE_SURFACECONFIG,									// 幻化
		CONFIG_MODULE_TYPE_MARRIAGE,										// 结婚
		CONFIG_MODULE_TYPE_BABY,											// 子女
		CONFIG_MODULE_TYPE_JEWELRY_CRAFT,									// 灵饰打造
		CONFIG_MODULE_TYPE_GUILD,											// 家族
		CONFIG_MODULE_TYPE_FRIEND,											// 好友
		CONFIG_MODULE_TYPE_SHI_TU,											// 师徒
		//CONFIG_MODULE_TYPE_SYSTEM_NOTICE,									// 功能预告
		CONFIG_MODULE_TYPE_COURAGE_CHALLENGE,								// 勇气挑战
		CONFIG_MODULE_TYPE_MONSTER_WAVE,									// 魔窟
		CONFIG_MODULE_TYPE_RANDACTIVITY_OPEN,								// 随机活动开启
		CONFIG_MODULE_TYPE_WILD_BOSS,										// 野外boss
		CONFIG_MODULE_TYPE_MI_YI,											// 密医阴谋
		CONFIG_MODULE_TYPE_SCHOOL_TASK,										// 门派任务
		CONFIG_MODULE_TYPE_OFFLINE_EXP,										// 离线经验
		CONFIG_MODULE_TYPE_WORLD_BOSS,										// 世界boss
		CONFIG_MODULE_TYPE_ESCORT,											// 运镖
		CONFIG_MODULE_TYPE_TREASURE_MAP,									// 宝图
		CONFIG_MODULE_TYPE_TASK_CHAIN,										// 任务链
		CONFIG_MODULE_TYPE_FABAO,											// 法宝
		CONFIG_MODULE_TYPE_RESEARCH_TASK,									// 真中的研究
		CONFIG_MODULE_TYPE_MEDAL,											// 勋章
		CONFIG_MODULE_TYPE_ROLE_CRYSTAL_FB,									// 水晶副本
		CONFIG_MODULE_TYPE_MAP_UNLOCK,										// 地图解锁
		CONFIG_MODULE_TYPE_TEAM_FIGHT,										// 小队竞技
		CONFIG_MODULE_TYPE_MAZE_RACER,										// 迷宫竞速
		CONFIG_MODULE_TYPE_GATHER_FB,										// 采集场
		CONFIG_MODULE_TYPE_COLOSSEUM,										// 竞技场
		CONFIG_MODULE_TYPE_EXCHANGE,										// 兑换
		CONFIG_MODULE_TYPE_BRAVE_GROUND,									// 勇闯地宫
		CONFIG_MODULE_TYPE_RAND_ACTIVITY,									// 随机活动
		CONFIG_MODULE_TYPE_CHONGZHI,										// 充值
		CONFIG_MODULE_TYPE_GUILD_FIGHT,										// 家族大乱斗
		CONFIG_MODULE_TYPE_CHIEF_ELECTION,									// 首席竞选
		CONFIG_MODULE_TYPE_PARTNER_EQUIPMENT,								// 伙伴命格卡
		CONFIG_MODULE_TYPE_CHIEF_PEAK_BATTLE,								// 首席巅峰战
		CONFIG_MODULE_TYPE_GUILD_HONOR_BATTLE,								// 工会荣耀战
		CONFIG_MODULE_TYPE_FUN_OPEN,										// 功能开启
		CONFIG_MODULE_TYPE_COURSE_OF_GROWTH,								// 成长之路
		CONFIG_MODULE_TYPE_PLATFORM_BATTLE,									// 擂台战
		CONFIG_MODULE_TYPE_GUILD_WEEK_TASK,									// 家族周任务
		CONFIG_MODULE_TYPE_CHAT,											// 聊天
		CONFIG_MODULE_TYPE_GUILD_ANSWER,									// 家族答题
		CONFIG_MODULE_TYPE_CHALLENGE_FB,									// 挑战副本
		CONFIG_MODULE_TYPE_FACE_SCORE,										// 颜值系统
		CONFIG_MODULE_TYPE_ADVANCE_FB,										// 进阶副本
		CONFIG_MODULE_TYPE_FUNC_GUIDE,										// 功能引导
		CONFIG_MODULE_TYPE_ENTRY,											// 装备词条
		CONFIG_MODULE_TYPE_MEDAL_FB,										// 勋章副本
		CONFIG_MODULE_TYPE_SERVER_FIRST_KILL,								// 全服首杀
		CONFIG_MODULE_TYPE_TRANSACTION_TASK,								// 购买任务 - 丝绸之路
		CONFIG_MODULE_TYPE_ACTIVE_SYSTEM,									// 活跃系统
		CONFIG_MODULE_TYPE_WORLD_QUESTION,									// 世界答题
		CONFIG_MODULE_TYPE_FANTASY_FB,										// 四象幻境
		CONFIG_MODULE_TYPE_TEAM_CFG,
		CONFIG_MODULE_TYPE_XUN_BAO,											// 寻宝
		CONFIG_MODULE_TYPE_ONLY_FIGHT,										// 以一敌百(个人版家族大乱斗)
		CONFIG_MODULE_TYPE_ACTIVITY,										// 活动
		CONFIG_MODULE_TYPE_BATTLE_VIDEO,									// 战斗录像
		CONFIG_MODULE_TYPE_HUAN_JIE_ZHAN_CHANG,								// 幻界战场
		CONFIG_MODULE_TYPE_PET_EQUIPMENT,									// 宠物装备
		CONFIG_MODULE_TYPE_FUNC_PREVIEW_CONFIG,
		CONFIG_MODULE_TYPE_SCORE_SYSTEM,									// 评分系统
		CONFIG_MODULE_TYPE_SKIN,											// 皮肤
		CONFIG_MODULE_TYPE_MYSTERY_STORE,									// 神秘商店
		CONFIG_MODULE_TYPE_SHAN_HAI_BOSS,									// 山海降妖
		CONFIG_MODULE_TYPE_WENXIANGLOU,										// 温香楼
		CONFIG_MODULE_TYPE_DU_CHUANG_ZEI_YING,								// 独闯贼
		CONFIG_MODULE_TYPE_LING_YAO_JIU_ZHU,								// 灵药救主
		CONFIG_MODULE_TYPE_STAR_KING,										// 司命星君
		CONFIG_MODULE_TYPE_CATCH_FOX,										// 抓拿狐妖
		CONFIG_MODULE_TYPE_YINYUANYIXIANQIAN,								// 姻缘一线牵
		CONFIG_MODULE_TYPE_BONUS,											// 运营-额外奖励
		CONFIG_MODULE_TYPE_LEVEL_COMPLEMENT,								// 新服助力
		CONFIG_MODULE_TYPE_DREAM_NOTES_CHALLENGE,							// 梦渊笔录
		CONFIG_MODULE_TYPE_SCHOOL_EXERCISE,									// 校场演练
		CONFIG_MODULE_TYPE_SUIT_LIST,										// 装备套装
		CONFIG_MODULE_TYPE_EXTRA_SCENE_CONFIG,								// 场景额外配置
		CONFIG_MODULE_TYPE_EQUIP_JEWERLY_UPGREADE,							// 装备强化系统
		CONFIG_MODULE_TYPE_SMART_MOUNTS_SYSTEM,								// 灵骑系统
		CONFIG_MODULE_TYPE_REALM_BREAK,										// 境界系统
		CONFIG_MODULE_TYPE_PET_MONOPOLY,									// 灵宠秘境
		CONFIG_MODULE_TYPE_LUN_HUI_WANG_CHUAN,								// 轮回忘川
		CONFIG_MODULE_TYPE_CLOUD_AREA,										// 云端竞技
		CONFIG_MODULE_TYPE_MATERIAL,										// 玉虚境(材料副本)
		CONFIG_MODULE_TYPE_RELIC_LOOTING,									// 遗迹夺宝
		CONFIG_MODULE_TYPE_INITIAL_BATTLE,									// 初始战斗
		CONFIG_MODULE_TYPE_MAP_EXPLORATION,									// 地图探索
		CONFIG_MODULE_TYPE_MAIN_TASK_CATCH_FOX_FB_CONFIG,					// 主线任务抓拿狐妖
		CONFIG_MODULE_TYPE_ROLE_HEART_SKILL,								// 心法
		CONFIG_MODULE_TYPE_PET_SOUL_EQUIPMENT,								// 宠物魂器
		CONFIG_MODULE_TYPE_ZHU_LING,										// 铸灵
		CONFIG_MODULE_TYPE_GOD_PRINT,										// 神印
		CONFIG_MODULE_TYPE_NETHER_WORLD_PURGATORY,							// 幽冥炼狱
		CONFIG_MODULE_TYPE_PET_GOD_FB,										// 神印之地
		CONFIG_MODULE_TYPE_HUNDRED_GHOST_CONFIG,							// 百鬼夜行
		CONFIG_MODULE_TYPE_MONTH_CARD,										// 月卡
		CONFIG_MODULE_TYPE_ACTIVITY_BIG_DIPPER,								// 北斗七星
		CONFIG_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN,							// 天地道痕
		CONFIG_MODULE_TYPE_ROLE_GIVE_GIFT,									// 赠送礼物
		CONFIG_MODULE_TYPE_PARTNER_GIFT_TREE,								// 伙伴天赋
		CONFIG_MODULE_TYPE_COW_REPORT_GOOD_NEWS,							// 福牛报喜
		CONFIG_MODULE_TYPE_ROLE_ATTR_PELLET,								// 属性丹
		CONFIG_MODULE_TYPE_DUEL,											// 决斗
		CONFIG_MODULE_TYPE_TIAN_MING,										// 天命
		CONFIG_MODULE_TYPE_DU_JIE,											// 渡劫
		CONFIG_MODULE_TYPE_YIN_YANG_EYE,									// 阴阳眼
		CONFIG_MODULE_TYPE_PEISHI,											// 配饰
		CONFIG_MODULE_TYPE_FRIEND_GIFT,										// 赠送系统
		CONFIG_MODULE_TYPE_ENDLESS_TOWER,									// 无尽之塔
		CONFIG_MODULE_TYPE_JIE_YI,											// 结义
		CONFIG_MODULE_TYPE_WAN_LING_FB,										// 万灵副本
		CONFIG_MODULE_TYPE_DIANHUASHI,										// 点化石
		CONFIG_MODULE_TYPE_NEW_LIFE_SKILL,									// 生活技能
		CONFIG_MODULE_TYPE_FLY_UP,											// 飞升
		CONFIG_MODULE_TYPE_MIRAGE_TERRITORY,								// 琉璃蜃境
		CONFIG_MODULE_TYPE_SILK_ROADS,										// 丝绸之路
		CONFIG_MODULE_TYPE_XIU_WEI,											// 修为
		CONFIG_MODULE_TYPE_EQUIP_STAR,										// 星级装备
		CONFIG_MODULE_TYPE_TREASURE_TASK,									// 九州秘宝
		CONFIG_MODULE_TYPE_RA_COMPENSATION,									// 随机活动补偿配置
		CONFIG_MODULE_TYPE_TIGER_BRINGS_BLESSING,							// 寅虎纳福
		CONFIG_MODULE_TYPE_QING_YUAN,										// 情缘
		CONFIG_MODULE_TYPE_FALLEN_GOD,										// 殒神之地
		CONFIG_MODULE_TYPE_MINI_GAME,										// 小游戏
		CONFIG_MODULE_TYPE_WORLD_ARENA,										// 论剑九州
		CONFIG_MODULE_TYPE_PET_BADGE,										// 宠物徽章
		CONFIG_MOUDLE_TYPE_WORLD_TEAM_ARENA,								// 英雄会武
		CONFIG_MODULE_TYPE_PET_LIAN_YAO,									// 宠物炼妖
		CONFIG_MODULE_TYPE_PEAK_TOKEN,										// 天下至尊令
		CONFIG_MODULE_TYPE_QING_YUAN_SHENG_HUI,								// 情缘盛会
		CONFIG_MODULE_TYPE_YAO_XING_SHI_LIAN,								// 妖行试炼
		CONFIG_MODULE_TYPE_PET_INBORN,										// 宠物天赋
		CONFIG_MODULE_TYPE_QINGYUAN_DUIDUIPENG,								// 情缘对对碰
		CONFIG_MODULE_TYPE_SAN_XING_GUI_DONG,								// 三星归洞
		CONFIG_MODULE_TYPE_ZHOU_QI,											// 周期期数配置
		CONFIG_MODULE_TYPE_LANTERN_IRDDIE,									// 山海灯会
		CONFIG_MODULE_TYPE_WHERE_IS_THE_FAIRY,								// 仙子去哪儿
		CONFIG_MODULE_TYPE_TIAN_GONG_XUN_BAO,								// 天宫寻宝
		CONFIG_MODULE_TYPE_LOVERS_MARKET,									// 情缘小铺
		CONFIG_MODULE_TYPE_TU_ZI_KUAI_PAO,									// 兔子快跑
		CONFIG_MODULE_TYPE_CONCENTRIC_KNOT,									// 缘牵同心结
		CONFIG_MODULE_TYPE_TAO_QUAN_NA_JI,									// 套圈纳吉
		CONFIG_MODULE_TYPE_UTA_GUESS,										// 天下第一比武[组队] 竞猜
		//CONFIG_MODULE_TYPE_LILIAN,	// 历练（暂已不用）
		CONFIG_MODULE_TYPE_FAST_FORWARD_BATTLE,	// 速战
		CONFIG_MODULE_TYPE_HUNDRED_GOD_BOSS,								// 百神之师
		CONFIG_MODULE_TYPE_ZAO_HUA_EQUIP,									// 造化装备
		CONFIG_MODULE_TYPE_HONGMENG_TIANJIE,								// 鸿蒙天界
		CONFIG_MODULE_TYPE_HOLY_EQUIP,										// 圣器
		CONFIG_MODULE_TYPE_KE_JU_EXAM,										// 科举考试
		CONFIG_MODULE_TYPE_GOTO_SCHOOL,										// 我要上学
		CONFIG_MODULE_TYPE_ZHOU_QI_SHOP,									// 周期商店
		CONFIG_MODULE_TYPE_ANIMATION_SPEED,
		CONFIG_MODULE_TYPE_FU_YU,											// 符玉
		CONFIG_MODULE_TYPE_ELEMENT_ELF,
		CONFIG_MODULE_TYPE_KUA_FU_YING_XIONG_LU,							// 跨服英雄录
		CONFIG_MODULE_TYPE_STAR_CHART_SYSTEM,								// 星图系统
		CONFIG_MODULE_TYPE_FASHION_COLLECTION,								// 时装升级
		CONFIG_MODULE_TYPE_MINIGAME_NIULANG,								// 小游戏-牛郎织女
		CONFIG_MODULE_TYPE_WORLD_BOSS_3,									// 世界BOSS3(天神降临)
		CONFIG_MODULE_TYPE_ROLE_GUILD_TASK,									// 家族任务
		CONFIG_MODULE_TYPE_HUAN_SHOU,										// 幻兽
		CONFIG_MODULE_TYPE_PET_GAI_MING,									// 宠物改命

		CONFIG_MODULE_TYPE_COUNT
	};

public:
	static LogicConfigManager * Instance();
	static void CheckLogicConfigAsyncReload();
	static bool AsyncReload(const std::string &path, std::string *err);
	static void ReleaseConfig();

	bool Init(const std::string &path, std::string *err, bool check_res_mode, bool is_from_reload = false);
	static bool Reload(const std::string &path, std::string *err);

	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_RAND_ACTIVITY, RandActivityConfigManager);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_EXCHANGE, ExchangeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TASK_CHAIN, TaskChainConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ESCORT, EscortConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SHOP, ShopConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET, PetConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ROLE_EXP, RoleExpConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CURRENCY, Currencyconfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PRESTIGE, PrestigeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_COLLECTION, CollectionConfig);
	//LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ACHIEVEMENT, AchievementConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PARTNER, PartnerConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GIFT_RAND_ITEM, GiftRandItemconfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TITLE, TitleConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TRAIN, TrainConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_KNAPSACK, KnapsackConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WST_QUESTION, WstQuestionConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TREVI_FOUNTION, TreviFountionconfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SUNDRY, SundryConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PROFESSION, ProfessionConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_DAY_REWARD, DayRewardConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WORKSHOP, WorkshopConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GEMSTONE, GemstoneConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SURFACECONFIG, SurfaceConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MARRIAGE, MarriageConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_BABY, BabyConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_JEWELRY_CRAFT, JewelryConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GUILD, GuildConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FRIEND, FriendConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SHI_TU, ShiTuConfig);
	//LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SYSTEM_NOTICE, SystemNoticeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_COURAGE_CHALLENGE, CourageChallengeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MONSTER_WAVE, MonsterWaveConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_RANDACTIVITY_OPEN, RandActivityOpenCfg);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WILD_BOSS, WildBossConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MI_YI, MiYiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SCHOOL_TASK, SchoolTaskConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_OFFLINE_EXP, OfflineExpConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WORLD_BOSS, WorldBossConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TREASURE_MAP, TreasureMapConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FABAO, FaBaoConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_RESEARCH_TASK, ResearchTaskConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MEDAL, MedalConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ROLE_CRYSTAL_FB, RoleCrystalFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MAP_UNLOCK, MapUnlockConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TEAM_FIGHT, ActivityTeamFightConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MAZE_RACER, ActivityMazeRacerConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GATHER_FB, RoleGatherFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_COLOSSEUM, ColosseumConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_BRAVE_GROUND, ActivityBraveGroundConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CHONGZHI, ChongzhiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GUILD_FIGHT, ActivityGuildFightConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CHIEF_ELECTION, ActivityChiefElectionConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PARTNER_EQUIPMENT, PartnerEquipmentConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CHIEF_PEAK_BATTLE, ActivityChiefPeakBattleConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GUILD_HONOR_BATTLE, ActivityGuildHonorBattleConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FUN_OPEN, FunOpenConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_COURSE_OF_GROWTH, CourseOfGrowthConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PLATFORM_BATTLE, PlatformBattleConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GUILD_WEEK_TASK, GuildWeekTaskConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CHAT, ChatConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GUILD_ANSWER, ActivityGuildAnswerConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CHALLENGE_FB, ChallengeFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FACE_SCORE, FaceScoreConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ADVANCE_FB, AdvanceFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FUNC_GUIDE, FuncGuideConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ENTRY, EntryConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MEDAL_FB, MedalFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SERVER_FIRST_KILL, ServerFirstKillConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TRANSACTION_TASK, TransactionTaskConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ACTIVE_SYSTEM, ActiveSystemConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WORLD_QUESTION, WorldQuestionConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FANTASY_FB, FantasyFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TEAM_CFG, TeamConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_XUN_BAO, XunBaoConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ONLY_FIGHT, ActivityOnlyFightConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ACTIVITY, ActivityConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_BATTLE_VIDEO, BattleVideoConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_HUAN_JIE_ZHAN_CHANG, ActivityHuanJieZhanChangConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_EQUIPMENT, PetEquipMentConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FUNC_PREVIEW_CONFIG, FuncPreviewConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SCORE_SYSTEM, ScoreSystemConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SKIN, SkinConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SHAN_HAI_BOSS, ShanHaiBossConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WENXIANGLOU, WenXiangLouFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MYSTERY_STORE, MysteryStoreConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_DU_CHUANG_ZEI_YING, DuChuangZeiYingConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_LING_YAO_JIU_ZHU, LingYaoJiuZhuFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_STAR_KING, StarKingFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CATCH_FOX, CatchFoxFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_YINYUANYIXIANQIAN, YYYXQConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_BONUS, RandActivityBonusConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_LEVEL_COMPLEMENT, RoleLevelComplementConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_DREAM_NOTES_CHALLENGE, DreamNotesChallengeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SCHOOL_EXERCISE, SchoolExerciseConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SUIT_LIST, SuitListConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_EXTRA_SCENE_CONFIG, SceneExtraConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_EQUIP_JEWERLY_UPGREADE, EquipmentJewerlyUpgreadeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SMART_MOUNTS_SYSTEM, SmartMountsSystemConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_REALM_BREAK, RealmBreakConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_MONOPOLY, PetMonopolyConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_LUN_HUI_WANG_CHUAN, LunHuiWangChuanConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_RELIC_LOOTING, ActivityRelicLootingConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CLOUD_AREA, CloudArenaConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MATERIAL, MaterialConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_INITIAL_BATTLE, InitialBattleConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MAP_EXPLORATION, MapExplorationConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MAIN_TASK_CATCH_FOX_FB_CONFIG, MainTaskCatchFoxFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ROLE_HEART_SKILL, RoleHeartSkillConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_SOUL_EQUIPMENT, PetSoulEquipMentConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ZHU_LING, ZhuLingConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GOD_PRINT, PetGodPrintEquipMentConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_NETHER_WORLD_PURGATORY, NetherWorldPurgatoryConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_GOD_FB, PetGodFbConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_HUNDRED_GHOST_CONFIG, HundredGhostConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MONTH_CARD, MonthCardConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ACTIVITY_BIG_DIPPER, ActivityBigDipperConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN, TianDiDaoHenConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ROLE_GIVE_GIFT, RoleGiveGiftConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PARTNER_GIFT_TREE, PartnerGiftTreeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_COW_REPORT_GOOD_NEWS, CowReportGoodNewsConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ROLE_ATTR_PELLET, RoleAttrPelletConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_DUEL, DuelConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TIAN_MING, TianMingConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_DU_JIE, DuJieConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_YIN_YANG_EYE, YinYangEyeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PEISHI, PeiShiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FRIEND_GIFT, FriendGiftConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ENDLESS_TOWER, EndlessTowerConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_JIE_YI, JieYiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WAN_LING_FB, WanLingFBConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_DIANHUASHI, DianHuaShiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_NEW_LIFE_SKILL, NewLifeSkillConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FLY_UP, FlyUpSystemConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MIRAGE_TERRITORY, MirageTerritoryConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SILK_ROADS, ActivitySilkRoadsConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_XIU_WEI, XiuWeiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_EQUIP_STAR, EquipStarConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TREASURE_TASK, TreasureTaskConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_RA_COMPENSATION, RACompensationConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TIGER_BRINGS_BLESSING, TigerBringsBlessingConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_QING_YUAN, QingYuanConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FALLEN_GOD, FallenGodConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MINI_GAME, MiniGameConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WORLD_ARENA, WorldArenaConfig);	
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_BADGE, PetBadgeConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MOUDLE_TYPE_WORLD_TEAM_ARENA, ActivityWorldTeamArenaConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_LIAN_YAO, PetLianYaoConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PEAK_TOKEN, PeakTokenConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_QING_YUAN_SHENG_HUI, ActivityQingYuanShengHuiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_YAO_XING_SHI_LIAN, YaoXingShiLianConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_INBORN, PetInbornConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_QINGYUAN_DUIDUIPENG, QingYuanDuiDuiPengConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_SAN_XING_GUI_DONG, SanXianConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ZHOU_QI, ZhouQiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_LANTERN_IRDDIE, CycleActLanternRiddleConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WHERE_IS_THE_FAIRY, WhereIsTheFairyConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TIAN_GONG_XUN_BAO, TianGongXunBaoConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_LOVERS_MARKET, LoversMarketConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_CONCENTRIC_KNOT, ConcentricKnotConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TU_ZI_KUAI_PAO, TuZiKuaiPaoConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_TAO_QUAN_NA_JI, TaoQuanNaJiConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_UTA_GUESS, UTAGuessConfig);
	//LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_LILIAN, LilianConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FAST_FORWARD_BATTLE, FastForwardBattleConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_HUNDRED_GOD_BOSS, HundredGodBossConfig);
	
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ZAO_HUA_EQUIP, ZaoHuaEquipConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_HONGMENG_TIANJIE, HongMengTianJieConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_HOLY_EQUIP, HolyEquipMentConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_KE_JU_EXAM, CycleKeJuExamConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_GOTO_SCHOOL, GotoSchoolConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ZHOU_QI_SHOP, ZhouQiShopConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ANIMATION_SPEED, AnimationSpeedConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ELEMENT_ELF, ElementElfConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FU_YU, FuYuConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_KUA_FU_YING_XIONG_LU, KuaFuYingXiongLuConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_STAR_CHART_SYSTEM, StarChartConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_FASHION_COLLECTION, FashionCollectionConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_MINIGAME_NIULANG, MiniGameNiuLangConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_WORLD_BOSS_3, WorldBoss3Config);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_ROLE_GUILD_TASK, RoleGuildTaskConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_HUAN_SHOU, HuanShouConfig);
	LOGIC_CONFIG_GET_MODULE(CONFIG_MODULE_TYPE_PET_GAI_MING, PetGaiMingConfig);

private:
	LogicConfigManager();
	~LogicConfigManager();

	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
	LogicConfigManager(const LogicConfigManager&);
	LogicConfigManager& operator=(const LogicConfigManager &);												

	ILogicConfig *m_logic_config_module_item_list[CONFIG_MODULE_TYPE_COUNT];		// 存储所有配置类
	std::string m_logic_config_module_xml_path_list[CONFIG_MODULE_TYPE_COUNT];		// 存储所有配置路径
};

#endif // __LOGIC_CONFIG_MANAGER_HPP__


