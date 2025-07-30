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
		CONFIG_MODULE_TYPE_SHOP = CONFIG_MODULE_TYPE_MIN,					// �̵�
		CONFIG_MODULE_TYPE_PET,												// ����
		CONFIG_MODULE_TYPE_ROLE_EXP,										// ��ɫ���������Լӵ�
		CONFIG_MODULE_TYPE_CURRENCY,										// ��ˮ��
		CONFIG_MODULE_TYPE_PRESTIGE,										// ����
		CONFIG_MODULE_TYPE_COLLECTION,										// ��Ʒ
		//CONFIG_MODULE_TYPE_ACHIEVEMENT,										// �ɾ�
		CONFIG_MODULE_TYPE_PARTNER,											// ���
		CONFIG_MODULE_TYPE_GIFT_RAND_ITEM,									// ������
		CONFIG_MODULE_TYPE_TITLE,											// �ƺ�
		CONFIG_MODULE_TYPE_TRAIN,											// ����
		CONFIG_MODULE_TYPE_KNAPSACK,										// ����
		CONFIG_MODULE_TYPE_WST_QUESTION,									// ����ͨ�Ŀ���
		CONFIG_MODULE_TYPE_TREVI_FOUNTION,									// ��Ը��
		CONFIG_MODULE_TYPE_SUNDRY,											// ����
		CONFIG_MODULE_TYPE_PROFESSION,										// ְҵ����
		CONFIG_MODULE_TYPE_DAY_REWARD,										// ��������
		CONFIG_MODULE_TYPE_WORKSHOP,										// ����
		CONFIG_MODULE_TYPE_GEMSTONE,										// ��ʯ
		CONFIG_MODULE_TYPE_SURFACECONFIG,									// �û�
		CONFIG_MODULE_TYPE_MARRIAGE,										// ���
		CONFIG_MODULE_TYPE_BABY,											// ��Ů
		CONFIG_MODULE_TYPE_JEWELRY_CRAFT,									// ���δ���
		CONFIG_MODULE_TYPE_GUILD,											// ����
		CONFIG_MODULE_TYPE_FRIEND,											// ����
		CONFIG_MODULE_TYPE_SHI_TU,											// ʦͽ
		//CONFIG_MODULE_TYPE_SYSTEM_NOTICE,									// ����Ԥ��
		CONFIG_MODULE_TYPE_COURAGE_CHALLENGE,								// ������ս
		CONFIG_MODULE_TYPE_MONSTER_WAVE,									// ħ��
		CONFIG_MODULE_TYPE_RANDACTIVITY_OPEN,								// ��������
		CONFIG_MODULE_TYPE_WILD_BOSS,										// Ұ��boss
		CONFIG_MODULE_TYPE_MI_YI,											// ��ҽ��ı
		CONFIG_MODULE_TYPE_SCHOOL_TASK,										// ��������
		CONFIG_MODULE_TYPE_OFFLINE_EXP,										// ���߾���
		CONFIG_MODULE_TYPE_WORLD_BOSS,										// ����boss
		CONFIG_MODULE_TYPE_ESCORT,											// ����
		CONFIG_MODULE_TYPE_TREASURE_MAP,									// ��ͼ
		CONFIG_MODULE_TYPE_TASK_CHAIN,										// ������
		CONFIG_MODULE_TYPE_FABAO,											// ����
		CONFIG_MODULE_TYPE_RESEARCH_TASK,									// ���е��о�
		CONFIG_MODULE_TYPE_MEDAL,											// ѫ��
		CONFIG_MODULE_TYPE_ROLE_CRYSTAL_FB,									// ˮ������
		CONFIG_MODULE_TYPE_MAP_UNLOCK,										// ��ͼ����
		CONFIG_MODULE_TYPE_TEAM_FIGHT,										// С�Ӿ���
		CONFIG_MODULE_TYPE_MAZE_RACER,										// �Թ�����
		CONFIG_MODULE_TYPE_GATHER_FB,										// �ɼ���
		CONFIG_MODULE_TYPE_COLOSSEUM,										// ������
		CONFIG_MODULE_TYPE_EXCHANGE,										// �һ�
		CONFIG_MODULE_TYPE_BRAVE_GROUND,									// �´��ع�
		CONFIG_MODULE_TYPE_RAND_ACTIVITY,									// ����
		CONFIG_MODULE_TYPE_CHONGZHI,										// ��ֵ
		CONFIG_MODULE_TYPE_GUILD_FIGHT,										// ������Ҷ�
		CONFIG_MODULE_TYPE_CHIEF_ELECTION,									// ��ϯ��ѡ
		CONFIG_MODULE_TYPE_PARTNER_EQUIPMENT,								// �������
		CONFIG_MODULE_TYPE_CHIEF_PEAK_BATTLE,								// ��ϯ�۷�ս
		CONFIG_MODULE_TYPE_GUILD_HONOR_BATTLE,								// ������ҫս
		CONFIG_MODULE_TYPE_FUN_OPEN,										// ���ܿ���
		CONFIG_MODULE_TYPE_COURSE_OF_GROWTH,								// �ɳ�֮·
		CONFIG_MODULE_TYPE_PLATFORM_BATTLE,									// ��̨ս
		CONFIG_MODULE_TYPE_GUILD_WEEK_TASK,									// ����������
		CONFIG_MODULE_TYPE_CHAT,											// ����
		CONFIG_MODULE_TYPE_GUILD_ANSWER,									// �������
		CONFIG_MODULE_TYPE_CHALLENGE_FB,									// ��ս����
		CONFIG_MODULE_TYPE_FACE_SCORE,										// ��ֵϵͳ
		CONFIG_MODULE_TYPE_ADVANCE_FB,										// ���׸���
		CONFIG_MODULE_TYPE_FUNC_GUIDE,										// ��������
		CONFIG_MODULE_TYPE_ENTRY,											// װ������
		CONFIG_MODULE_TYPE_MEDAL_FB,										// ѫ�¸���
		CONFIG_MODULE_TYPE_SERVER_FIRST_KILL,								// ȫ����ɱ
		CONFIG_MODULE_TYPE_TRANSACTION_TASK,								// �������� - ˿��֮·
		CONFIG_MODULE_TYPE_ACTIVE_SYSTEM,									// ��Ծϵͳ
		CONFIG_MODULE_TYPE_WORLD_QUESTION,									// �������
		CONFIG_MODULE_TYPE_FANTASY_FB,										// ����þ�
		CONFIG_MODULE_TYPE_TEAM_CFG,
		CONFIG_MODULE_TYPE_XUN_BAO,											// Ѱ��
		CONFIG_MODULE_TYPE_ONLY_FIGHT,										// ��һ�а�(���˰������Ҷ�)
		CONFIG_MODULE_TYPE_ACTIVITY,										// �
		CONFIG_MODULE_TYPE_BATTLE_VIDEO,									// ս��¼��
		CONFIG_MODULE_TYPE_HUAN_JIE_ZHAN_CHANG,								// �ý�ս��
		CONFIG_MODULE_TYPE_PET_EQUIPMENT,									// ����װ��
		CONFIG_MODULE_TYPE_FUNC_PREVIEW_CONFIG,
		CONFIG_MODULE_TYPE_SCORE_SYSTEM,									// ����ϵͳ
		CONFIG_MODULE_TYPE_SKIN,											// Ƥ��
		CONFIG_MODULE_TYPE_MYSTERY_STORE,									// �����̵�
		CONFIG_MODULE_TYPE_SHAN_HAI_BOSS,									// ɽ������
		CONFIG_MODULE_TYPE_WENXIANGLOU,										// ����¥
		CONFIG_MODULE_TYPE_DU_CHUANG_ZEI_YING,								// ������
		CONFIG_MODULE_TYPE_LING_YAO_JIU_ZHU,								// ��ҩ����
		CONFIG_MODULE_TYPE_STAR_KING,										// ˾���Ǿ�
		CONFIG_MODULE_TYPE_CATCH_FOX,										// ץ�ú���
		CONFIG_MODULE_TYPE_YINYUANYIXIANQIAN,								// ��Եһ��ǣ
		CONFIG_MODULE_TYPE_BONUS,											// ��Ӫ-���⽱��
		CONFIG_MODULE_TYPE_LEVEL_COMPLEMENT,								// �·�����
		CONFIG_MODULE_TYPE_DREAM_NOTES_CHALLENGE,							// ��Ԩ��¼
		CONFIG_MODULE_TYPE_SCHOOL_EXERCISE,									// У������
		CONFIG_MODULE_TYPE_SUIT_LIST,										// װ����װ
		CONFIG_MODULE_TYPE_EXTRA_SCENE_CONFIG,								// ������������
		CONFIG_MODULE_TYPE_EQUIP_JEWERLY_UPGREADE,							// װ��ǿ��ϵͳ
		CONFIG_MODULE_TYPE_SMART_MOUNTS_SYSTEM,								// ����ϵͳ
		CONFIG_MODULE_TYPE_REALM_BREAK,										// ����ϵͳ
		CONFIG_MODULE_TYPE_PET_MONOPOLY,									// ����ؾ�
		CONFIG_MODULE_TYPE_LUN_HUI_WANG_CHUAN,								// �ֻ�����
		CONFIG_MODULE_TYPE_CLOUD_AREA,										// �ƶ˾���
		CONFIG_MODULE_TYPE_MATERIAL,										// ���龳(���ϸ���)
		CONFIG_MODULE_TYPE_RELIC_LOOTING,									// �ż��ᱦ
		CONFIG_MODULE_TYPE_INITIAL_BATTLE,									// ��ʼս��
		CONFIG_MODULE_TYPE_MAP_EXPLORATION,									// ��ͼ̽��
		CONFIG_MODULE_TYPE_MAIN_TASK_CATCH_FOX_FB_CONFIG,					// ��������ץ�ú���
		CONFIG_MODULE_TYPE_ROLE_HEART_SKILL,								// �ķ�
		CONFIG_MODULE_TYPE_PET_SOUL_EQUIPMENT,								// �������
		CONFIG_MODULE_TYPE_ZHU_LING,										// ����
		CONFIG_MODULE_TYPE_GOD_PRINT,										// ��ӡ
		CONFIG_MODULE_TYPE_NETHER_WORLD_PURGATORY,							// ��ڤ����
		CONFIG_MODULE_TYPE_PET_GOD_FB,										// ��ӡ֮��
		CONFIG_MODULE_TYPE_HUNDRED_GHOST_CONFIG,							// �ٹ�ҹ��
		CONFIG_MODULE_TYPE_MONTH_CARD,										// �¿�
		CONFIG_MODULE_TYPE_ACTIVITY_BIG_DIPPER,								// ��������
		CONFIG_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN,							// ��ص���
		CONFIG_MODULE_TYPE_ROLE_GIVE_GIFT,									// ��������
		CONFIG_MODULE_TYPE_PARTNER_GIFT_TREE,								// ����츳
		CONFIG_MODULE_TYPE_COW_REPORT_GOOD_NEWS,							// ��ţ��ϲ
		CONFIG_MODULE_TYPE_ROLE_ATTR_PELLET,								// ���Ե�
		CONFIG_MODULE_TYPE_DUEL,											// ����
		CONFIG_MODULE_TYPE_TIAN_MING,										// ����
		CONFIG_MODULE_TYPE_DU_JIE,											// �ɽ�
		CONFIG_MODULE_TYPE_YIN_YANG_EYE,									// ������
		CONFIG_MODULE_TYPE_PEISHI,											// ����
		CONFIG_MODULE_TYPE_FRIEND_GIFT,										// ����ϵͳ
		CONFIG_MODULE_TYPE_ENDLESS_TOWER,									// �޾�֮��
		CONFIG_MODULE_TYPE_JIE_YI,											// ����
		CONFIG_MODULE_TYPE_WAN_LING_FB,										// ���鸱��
		CONFIG_MODULE_TYPE_DIANHUASHI,										// �㻯ʯ
		CONFIG_MODULE_TYPE_NEW_LIFE_SKILL,									// �����
		CONFIG_MODULE_TYPE_FLY_UP,											// ����
		CONFIG_MODULE_TYPE_MIRAGE_TERRITORY,								// �����׾�
		CONFIG_MODULE_TYPE_SILK_ROADS,										// ˿��֮·
		CONFIG_MODULE_TYPE_XIU_WEI,											// ��Ϊ
		CONFIG_MODULE_TYPE_EQUIP_STAR,										// �Ǽ�װ��
		CONFIG_MODULE_TYPE_TREASURE_TASK,									// �����ر�
		CONFIG_MODULE_TYPE_RA_COMPENSATION,									// ������������
		CONFIG_MODULE_TYPE_TIGER_BRINGS_BLESSING,							// �����ɸ�
		CONFIG_MODULE_TYPE_QING_YUAN,										// ��Ե
		CONFIG_MODULE_TYPE_FALLEN_GOD,										// ����֮��
		CONFIG_MODULE_TYPE_MINI_GAME,										// С��Ϸ
		CONFIG_MODULE_TYPE_WORLD_ARENA,										// �۽�����
		CONFIG_MODULE_TYPE_PET_BADGE,										// �������
		CONFIG_MOUDLE_TYPE_WORLD_TEAM_ARENA,								// Ӣ�ۻ���
		CONFIG_MODULE_TYPE_PET_LIAN_YAO,									// ��������
		CONFIG_MODULE_TYPE_PEAK_TOKEN,										// ����������
		CONFIG_MODULE_TYPE_QING_YUAN_SHENG_HUI,								// ��Եʢ��
		CONFIG_MODULE_TYPE_YAO_XING_SHI_LIAN,								// ��������
		CONFIG_MODULE_TYPE_PET_INBORN,										// �����츳
		CONFIG_MODULE_TYPE_QINGYUAN_DUIDUIPENG,								// ��Ե�Զ���
		CONFIG_MODULE_TYPE_SAN_XING_GUI_DONG,								// ���ǹ鶴
		CONFIG_MODULE_TYPE_ZHOU_QI,											// ������������
		CONFIG_MODULE_TYPE_LANTERN_IRDDIE,									// ɽ���ƻ�
		CONFIG_MODULE_TYPE_WHERE_IS_THE_FAIRY,								// ����ȥ�Ķ�
		CONFIG_MODULE_TYPE_TIAN_GONG_XUN_BAO,								// �칬Ѱ��
		CONFIG_MODULE_TYPE_LOVERS_MARKET,									// ��ԵС��
		CONFIG_MODULE_TYPE_TU_ZI_KUAI_PAO,									// ���ӿ���
		CONFIG_MODULE_TYPE_CONCENTRIC_KNOT,									// Եǣͬ�Ľ�
		CONFIG_MODULE_TYPE_TAO_QUAN_NA_JI,									// ��Ȧ�ɼ�
		CONFIG_MODULE_TYPE_UTA_GUESS,										// ���µ�һ����[���] ����
		//CONFIG_MODULE_TYPE_LILIAN,	// ���������Ѳ��ã�
		CONFIG_MODULE_TYPE_FAST_FORWARD_BATTLE,	// ��ս
		CONFIG_MODULE_TYPE_HUNDRED_GOD_BOSS,								// ����֮ʦ
		CONFIG_MODULE_TYPE_ZAO_HUA_EQUIP,									// �컯װ��
		CONFIG_MODULE_TYPE_HONGMENG_TIANJIE,								// �������
		CONFIG_MODULE_TYPE_HOLY_EQUIP,										// ʥ��
		CONFIG_MODULE_TYPE_KE_JU_EXAM,										// �ƾٿ���
		CONFIG_MODULE_TYPE_GOTO_SCHOOL,										// ��Ҫ��ѧ
		CONFIG_MODULE_TYPE_ZHOU_QI_SHOP,									// �����̵�
		CONFIG_MODULE_TYPE_ANIMATION_SPEED,
		CONFIG_MODULE_TYPE_FU_YU,											// ����
		CONFIG_MODULE_TYPE_ELEMENT_ELF,
		CONFIG_MODULE_TYPE_KUA_FU_YING_XIONG_LU,							// ���Ӣ��¼
		CONFIG_MODULE_TYPE_STAR_CHART_SYSTEM,								// ��ͼϵͳ
		CONFIG_MODULE_TYPE_FASHION_COLLECTION,								// ʱװ����
		CONFIG_MODULE_TYPE_MINIGAME_NIULANG,								// С��Ϸ-ţ��֯Ů
		CONFIG_MODULE_TYPE_WORLD_BOSS_3,									// ����BOSS3(������)
		CONFIG_MODULE_TYPE_ROLE_GUILD_TASK,									// ��������
		CONFIG_MODULE_TYPE_HUAN_SHOU,										// ����
		CONFIG_MODULE_TYPE_PET_GAI_MING,									// �������

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

	//��ֹ�������ƣ��벻Ҫ���л����������������� ������������� ��Ϊ������ʱ��delete��ĳЩ��Ա������
	LogicConfigManager(const LogicConfigManager&);
	LogicConfigManager& operator=(const LogicConfigManager &);												

	ILogicConfig *m_logic_config_module_item_list[CONFIG_MODULE_TYPE_COUNT];		// �洢����������
	std::string m_logic_config_module_xml_path_list[CONFIG_MODULE_TYPE_COUNT];		// �洢��������·��
};

#endif // __LOGIC_CONFIG_MANAGER_HPP__


