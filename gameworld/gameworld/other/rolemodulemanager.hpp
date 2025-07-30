#ifndef __ROLE_MODULE_MANAGER_HPP__
#define __ROLE_MODULE_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/commondata.hpp"
#include "servercommon/guilddef.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include "attributelist.hpp"
#include "item/itemextern.hpp"
#include "protocol/msgrole.h"
#include "servercommon/frienddef.hpp"

#include <string>

#define DEFINE_ROLE_MODULE(type, classname) \
	classname * m##classname;

#define GET_ROLE_MODULE(type, classname) \
	classname * Get##classname() const { return m##classname; }

bool IsMixedElementCrystalId(int item_id);

class Guild;
struct RoleCommonSaveXunBao;
struct WorldTeamArenaRoleInfo;
class Role;
class Vip;
class RoleOtherInitParam;
class RoleInitParam;
class RoleAttributePlan;
class Monitor;
class Money;
class Knapsack;
class ItemListParam;
class SkillListParam;
class Pet;
class Partner;
class Prestige;
class Collection;
class Title;
class Train;
class TaskManager;
class TaskRecorder;
class WstQuestion;
class RoleSkill;
class DayReward;
class EquipmentManager;
class Workshop;
class Surface;
class Marriage;
class Baby;
class FriendListParam;
class BlacklistsParam;
class FriendRecommend;
class RoleSetting;
class ShiTu;
class SystemNotice;
class CourageChallenge;
class MonsterWave;
class RoleActivityManager;
class AchievementListParam;
class MiYi;
class SchoolTask;
class OfflineExp;
class Escort;
class TreasureMap;
class TaskChain;
class FaBao;
class ResearchTask;
class Medal;
class RoleCrystalFb;
class BuffManager;
class Shop;
class MapUnlock;
class RoleGatherFb;
class Exchange;
class RoleCross;
class CourseOfGrowth;
class Salary;
class RoleChallengeFb;
class FaceScore;
class FuncGuide;
class RoleDrop;
class RoleConvertShop;
class SpecialAppearance;
class RoleMail;
class MailListParam;
class LoginSystem;
class BossChapter;
class Capability;
class MysteriousTrial;
class BountyTask;
class TransactionTask;
class ActiveSystem;
class HuaShenManager;
class FuncPreview;
class RoleFirstKillParamList;
class RoleFirstKill;
class AutoCatchPet;
class TradeCredit;
class RoleFormation;
class RoleGatherDream;
class RoleHuanJie;
class RandActivityRoleParamList;
class AnecdoteTask;
class LevelComplement;
class DreamNotesChallenge;
class SchoolExercise;
class RoleCommonSaveManager;
class RoleCommonSaveDataParam;
class SmartMountsSystem;
class RealmBreak;
class PetMonopoly;
class LunHuiWangChuan;
class RoleMaterial;
class RoleCourageChallengeLevelParamList;
class MapExploration;
class RoleJoinActivityReward;
class CustomInstruction;
class RoleGuildInborn;
class RoleHeartSkill;
class RoleZhuLing;
class PetHelperFormation;
class NetherWorldPurgatory;
class PetGodFB;
class BigDipper;
class RoleTianDiDaoHen;
class RoleGiveGift;
class PrestigeTask;
class SVip;
class RoleAttrPellet;
class GodPrintItemListParam;
class Duel;
class AccessoriesmentManager;
class TianMing;
class DuJie;
class YinYangEye;
class FlyUpSystem;
class EndlessTower;
class AdvanceCareer;
class RoleWanLingFB;
class RoleNewLifeSkill;
class MirageTerritory;
class XiuWei;
class TreasureTask;
class FallenGod;
class MiniGame;
class RolePeakToken;
class RoleIncome;
class RoleNewLevelComplement;
class YaoXingShiLian;
class SanXianGuiDong;
class LanternRiddie;
class RoleWhereIsTheFairy;
class RoleTianGongXunBao;
class LoversMarket;
class TaoQuanNaJi;
class ConcentricKnot;
class RoleHundredGodBoss;
class ZaoHuaEquipManager;
class RoleHongMengTianJie;
class HolyEquipManager;
class KeJuExam;
class ZhouQiShop;
class ElementElf;
class FuYuManager;
class RoleKuaFuYingXiongLu;
class StarChart;
class NewCourageChallenge;
class NewCourageChallengeLevelParamList;
class FashionCollection;
class RoleGuildTask;
class RoleHuanShou;

class RoleModuleManager
{
public:
	typedef bool (RoleModuleManager:: * DBRoleCommandHandler)(std::istringstream &, std::string &, unsigned long long);
	typedef std::map<std::string, RoleModuleManager::DBRoleCommandHandler> DBRoleCommandHandlerMap;

	enum ROLE_MODULE_TYPE
	{
		ROLE_MODULE_TYPE_VIP,
		ROLE_MODULE_TYPE_ROLE_ATTRIBUTE,
		ROLE_MODULE_TYPE_MONITOR,
		ROLE_MODULE_TYPE_MONEY,
		ROLE_MODULE_TYPE_KNAPSACK,
		ROLE_MODULE_TYPE_PET,
		ROLE_MODULE_TYPE_ACHIEVEMENT,
		ROLE_MODULE_TYPE_PARTNER,
		ROLE_MODULE_TYPE_PRESTIGE,
		ROLE_MODULE_TYPE_COLLECTION,
		ROLE_MODULE_TYPE_TITLE,
		ROLE_MODULE_TYPE_TRAIN,
		ROLE_MODULE_TYPE_TASKRECORDER,
		ROLE_MODULE_TYPE_TASKMANAGER,
		ROLE_MODULE_TYPE_WST_QUESTION,
		ROLE_MODULE_TYPE_ROLE_SKILL,
		ROLE_MODULE_TYPE_DAY_REWARD,
		ROLE_MODULE_TYPE_EQUIPMENT,
		ROLE_MODULE_TYPE_WORKSHOP,
		ROLE_MODULE_TYPE_SURFACE,
		ROLE_MODULE_TYPE_MARRIAGE,
		ROLE_MODULE_TYPE_BABY,
		ROLE_MODULE_TYPE_FRIEND_MANAGER,
		ROLE_MODULE_TYPE_FRIEND_RECOMMEND,
		ROLE_MODULE_TYPE_SYSTEM_SETTING,
		ROLE_MODULE_TYPE_SHI_TU,
		ROLE_MODULE_TYPE_SYSTEM_NOTICE,
		ROLE_MODULE_TYPE_COURAGE_CHALLENGE,
		ROLE_MODULE_TYPE_MONSTER_WAVE,
		ROLE_MODULE_TYPE_ACTIVITY_MANAGER,
		ROLE_MODULE_TYPE_MI_YI,
		ROLE_MODULE_TYPE_SCHOOL_TASK,
		ROLE_MODULE_TYPE_OFFLINE_EXP,
		ROLE_MODULE_TYPE_ESCORT,
		ROLE_MODULE_TYPE_TREASURE_MAP,
		ROLE_MODULE_TYPE_TASK_CHAIN,
		ROLE_MODULE_TYPE_FABAO,
		ROLE_MODULE_TYPE_RESEARCH_TASK,
		ROLE_MODULE_TYPE_MEDAL,
		ROLE_MODULE_TYPE_ROLE_CRYSTAL_FB,
		ROLE_MODULE_TYPE_BUFF_MANAGER,
		ROLE_MODULE_TYPE_SHOP,
		ROLE_MODULE_TYPE_MAP_UNLOCK,
		ROLE_MODULE_TYPE_GATHER_FB,
		ROLE_MODULE_TYPE_EXCHANGE,
		ROLE_MODULE_TYPE_ROLECROSS,
		ROLE_MODULE_TYPE_COURSE_OF_GROWTH,
		ROLE_MODULE_TYPE_SALARY,
		ROLE_MODULE_TYPE_CHALLENGE_FB,
		ROLE_MODULE_TYPE_FACE_SCORE,
		ROLE_MODULE_TYPE_FUNC_GUIDE,
		ROLE_MODULE_TYPE_ROLE_DROP,
		ROLE_MODULE_TYPE_ROLE_CONVERT_SHOP,
		ROLE_MODULE_TYPE_SPECIAL_APPEARANCE,
		ROLE_MODULE_TYPE_ROLE_MAIL,
		ROLE_MODULE_TYPE_ROLE_LOGIN_SYSTEM,
		ROLE_MODULE_TYPE_BOSS_CHAPTER,
		ROLE_MODULE_TYPE_ROLE_CAPABILITY,
		ROLE_MODULE_TYPE_MYSTERIOUS_TRIAL,
		ROLE_MODULE_TYPE_BOUNTY_TASK,
		ROLE_MODULE_TYPE_TRANSACTION_TASK,
		ROLE_MODULE_TYPE_ACTIVE_SYSTEM,
		ROLE_MODULE_TYPE_HUASHEN_MANAGER,
		ROLE_MODULE_TYPE_FUNC_PREVIEW,
		ROLE_MODULE_TYPE_ROLE_FIRST_KILL,
		ROLE_MODULE_TYPE_AUTO_CATCH_PET,
		ROLE_MODULE_TYPE_TRADE_CREDIT,
		ROLE_MODULE_TYPE_ROLE_FORMATION,
		ROLE_MODULE_TYPE_GATHER_DREAM,
		ROLE_MODULE_TYPE_ROLE_HUAN_JIE,
		ROLE_MODULE_TYPE_ANECDOTE_TASK,
		ROLE_MODULE_TYPE_LEVEL_COMPLEMENT,
		ROLE_MODULE_TYPE_DREAM_NOTES_CHALLENGE,
		ROLE_MODULE_TYPE_SCHOOL_EXERCISE,
		ROLE_MODULE_TYPE_COMMON_SAVE_MANAGER,
		ROLE_MODULE_TYPE_SMART_MOUNTS_SYSTEM,
		ROLE_MODULE_TYPE_REALM_BREAK,
		ROLE_MODULE_TYPE_PET_MONOPOLY,
		ROLE_MODULE_TYPE_LUN_HUI_WANG_CHUAN,
		ROLE_MODULE_TYPE_ROLE_MATERIAL,
		ROLE_MODULE_TYPE_MAP_EXPLORATION,
		ROLE_MODULE_TYPE_ROLE_JOIN_ACTIVITY_REWARD,						// 角色参与限时活动奖励
		ROLE_MODULE_TYPE_CUSTOM_INSTRUCTION,
		ROLE_MODULE_TYPE_ROLE_GUILD_INBORN,
		ROLE_MODULE_TYPE_ROLE_HEART_SKILL,
		ROLE_MODULE_TYPE_ZHU_LING,
		ROLE_MODULE_TYPE_PET_HELPER_FORMATION,
		ROLE_MODULE_TYPE_NETHER_WORLD_PURGATORY,
		ROLE_MODULE_TYPE_PET_GOD_FB,				//神印之地
		ROLE_MODULE_TYPE_BIG_DIPPER,
		ROLE_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN,
		ROLE_MODULE_TYPE_ROLE_GIVE_GIFT,
		ROLE_MODULE_TYPE_PRESTIGE_TASK,
		ROLE_MODULE_TYPE_SVIP,
		ROLE_MODULE_TYPE_ROLE_ATTR_PELLET,
		ROLE_MODULE_TYPE_DUEL,
		ROLE_MODULE_TYPE_TIAN_MING,					// 天命
		ROLE_MODULE_TYPE_DU_JIE,		//渡劫
		ROLE_MODULE_TYPE_YIN_YANG_EYE,
		ROLE_MODULE_TYPE_ACCESSORIES,
		ROLE_MODULE_TYPE_ENDLESS_TOWER,
		ROLE_MODULE_TYPE_ADVANCE_CAREER,		// 职业晋级
		ROLE_MODULE_TYPE_WAN_LING_FB,			// 万灵话本
		ROLE_MODULE_TYPE_ROLE_NEW_LIFE_SKILL,
		ROLE_MODULE_TYPE_FLY_UP_SYSTEM,
		ROLE_MODULE_TYPE_MIRAGE_TERRIROTY,
		ROLE_MODULE_TYPE_XIU_WEI,				// 修为
		ROLE_MODULE_TYPE_TREASURE_TASK,			// 九州秘宝
		ROLE_MODULE_TYPE_FALLEN_GOD,
		ROLE_MODULE_TYPE_MINI_GAME,
		ROLE_MODULE_TYPE_ROLE_PEAK_TOKEN,		// 天下至尊令
		ROLE_MODULE_TYPE_ROLE_INCOME,			// 收益统计
		ROLE_MODULE_TYPE_ROLE_NEW_LEVEL_COMPLEMENT,	//等级助力
		ROLE_MODULE_TYPE_YAO_XING_SHI_LIAN,		// 妖行试炼
		ROLE_MODULE_TYPE_SAN_XIAN_GUI_DONG,		// 三仙归洞
		ROLE_MODULE_TYPE_LANTERN_RIDDIE,		// 山海灯会
		ROLE_MODULE_TYPE_ROLE_WHERE_IS_THE_FAIRY,	//仙子去哪儿
		ROLE_MODULE_TYPE_ROLE_TIAN_GONG_XUN_BAO,	//天宫寻宝
		ROLE_MODULE_TYPE_LOVERS_MARKET,			// 情缘小铺
		ROLE_MODULE_TYPE_TAO_QUAN_NA_JI,		//套圈纳吉
		ROLE_MODULE_TYPE_CONCENTRIC_KNOT,		// 缘牵同心结
		ROLE_MODULE_TYPE_ROLE_HUNDRED_GOD_BOSS,	// 百神之师
		ROLE_MODULE_TYPE_ZAO_HUA_MANAGER,		// 造化装备
		ROLE_MODULE_TYPE_HONGMENG_TIANJIE,		// 鸿蒙天界
		ROLE_MODULE_TYPE_HOLY_EQUIP,			// 圣器
		ROLE_MODULE_TYPE_KE_JU_EXAM,			// 科举考试
		ROLE_MODULE_TYPE_ZHOU_QI_SHOP,			// 周期商店
		ROLE_MODULE_TYPE_ELEMENT_ELF,			// 元素精灵（彩蛋系统）
		ROLE_MODULE_TYPE_FU_YU,					// 符玉
		ROLE_MODULE_TYPE_ROLE_KUA_FU_YING_XIONG_LU,	//跨服英雄录
		ROLE_MODULE_TYPE_STAR_CHART_SYSTEM,		// 星图系统
		ROLE_MODULE_TYPE_NEW_COURAGE_CHALLENGE,	// 弑神之塔
		ROLE_MODULE_TYPE_FASHION_COLLECTION,	// 外观升级
		ROLE_MODULE_TYPE_GUILD_TASK,			// 家族任务
		ROLE_MODULE_TYPE_ROLE_HUAN_SHOU,		// 幻兽系统

		ROLE_MODULE_TYPE_MAX,
	};

	struct ClientPlayMode  //!< \see PLAY_MODE
	{
		ClientPlayMode() : type(0), param1(0), param2(0) {}

		enum PLAY_MODE
		{
			PLAY_MODE_NONE = 0,			// 无玩法
			PLAY_MODE_MINE_MONSTER = 1, // 巡逻 
			PLAY_MODE_TASK = 2,			// 任务  p1: 任务ID
			PLAY_MODE_ACTIVITY = 3,		// 活动
		};

		int type;
		int param1;
		int param2;
	};

	enum SPEED_RECALC_REASON_TYPE
	{
		SPEED_RECALC_REASON_INIT = 0,				// 初始化
		SPEED_RECALC_REASON_BUFF_MGR = 1,			// Buff
		SPEED_RECALC_REASON_ESCORT = 2,				// 运镖

		SPEED_RECALC_REASON_MAX
	};

	typedef std::map<int, int> NeedMonsterMap;

public:
	RoleModuleManager(Role* role);
	~RoleModuleManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void  SetRole(Role* role);
	inline Role* GetRole() const { return m_role; }

	void InitRoleModuleData(const RoleInitParam& init_param, 
		const RoleOtherInitParam& other_init_param,
		const ItemListParam& item_list_param,
		const SkillListParam& skill_list_param,
		const MailListParam &maillist_param,
		const RoleFirstKillParamList &first_kill_list,
		const RandActivityRoleParamList & rand_activity_role_list,
		const RoleCommonSaveDataParam & role_common_save_param, 
		const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
		const GodPrintItemListParam & god_print_item_list,
		const NewCourageChallengeLevelParamList & new_courage_challenge_list);

	void GetRoleModuleData(RoleInitParam* out_init_param, 
		RoleOtherInitParam* out_other_init_param,
		ItemListParam* out_item_list_param,
		SkillListParam* out_skill_list_param,
		MailListParam* out_maillist_param,
		RoleFirstKillParamList* out_first_kill_list,
		RandActivityRoleParamList * rand_activity_role_list ,
		RoleCommonSaveDataParam * role_common_save_param,
		RoleCourageChallengeLevelParamList * role_courage_challenge_list,
		GodPrintItemListParam * god_print_item_list,
		NewCourageChallengeLevelParamList * new_courage_challenge_list,
		bool is_logout = false );

	int GetRoleUID() const;
	const char* GetRoleName() const;

	void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnResetModuleDataWeek();
	void OnResetModuleDataMonth();

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnTwoWeekChange(int week_flag);
	void OnMonthChange();

	void ReCalcSpeed(int recalc_reason, const char * recalc_source, bool is_login = false);
	void SendMoveSpeedChangeRate();
	void SendSpecialNotice(int op_type, int param1 = 0, int param2 = 0, int param3 = 0, int param4 = 0, int param5 = 0);	//发送特殊信息系统栏通知

	void ReCalcAttr(int recalc_reason, const char * recalc_reason_str, bool is_init = false, int send_notice = 0);
	const AttributeList & ReCalcSelfAttr(AttributeList &base_add, bool is_recalc);
	const AttributeList & ReCalcAttrProf(AttributeList &base_add, bool is_recalc);		//职业进阶 / 转职
	const AttributeList & ReCalcAttrGuildQiFu(AttributeList &base_add, bool is_recalc);	//家族祈福
	void SendAttrListReason(short param1);			//下发角色属性来源列表
	void SendAttributeList(short reason = 0);
	void SendSingleAttribute(int attr_tpye, short reason = 0);
	void OnChangeProfession(int old_prof, int new_prof);
	inline const AttributeList* GetAttrList() { return &m_attr_list; }
	Attribute GetAttrByType(int type);
	void GmSetAttr(int type, int value);
	void SetCurHpMp(int curhp, int curmp);
	bool ChangeHp(int change_vale, bool is_percent = false);
	bool ChangeMp(int change_vale, bool is_percent = false);
	void SendHpMpInfoToTeam();
	bool AddAttr(int type, Attribute value, Attribute max_value);
	void Recover();
	void OnLoginCalOfflineDay(unsigned int login_timestamp, unsigned int last_save_timestamp);	//计算玩家上次离线与本次登陆过去的天数
	void OnLoginCalCreateDay();									//计算玩家当前创建角色第几天
	void OnRoleLogin();
	
	/**
	* \description 获取创角天数，值从1开始，以0点为界计算天数
	*/
	inline int GetCreateDays() const { return m_common_data_param.role_create_count_day; }
	
	void OnAutoTalkUpdate(unsigned int now_second);
	void SendAutoTalkInfo(bool is_init = true);
	
	void ChangeProfSendItem(int old_prof, int cur_prof);						
	int GetUid();
	void OnAddGuildEvent(int param1, int param2, int type);
	void DayChangeResetGuildData();	
	void WeekChangeResetGuildData();
	void OnXunBaoBelssReq(int index, int pet_id);
	void SendXunBaoBelssInfo();
	void OnClientBehavior(int behavior_type);

	bool CanEnterScene(int scene_id, bool is_notice = true);
	void OnEnterScene(int scene_id);
	void OnLeaveScene(int scene_id);
	inline CommonDataParam&	GetCommonData() { return m_common_data_param; }
	inline GuildCommonParam&	GetGuildCommonData() { return m_guild_common_param; }
	inline FriendCommonParam & GetFriendCommonData() { return m_friend_common_param; }

	unsigned int GetLastSearchTradeMarketTime() const { return m_last_search_trade_market_time; }
	void OnSearchTradeMarket(unsigned int now) { m_last_search_trade_market_time = now; }

	//bool IsPatrolling() const { return ClientPlayMode::PLAY_MODE_MINE_MONSTER == m_client_play_mode.type; }
	bool IsPatrolling() const { return m_patrol_flag; }
	void SetPatrolFlag(bool is_patrol) { m_patrol_flag = is_patrol; }

	void AddNeedMonster(int monster_id);
	void RemoveNeedMonster(int monster_id);
	bool IsNeedMonster(int monster_id);

	void OnSendScoreHintNotice(int recommend_score);

	bool IsMeetGuildFightRank(int rank);
	void OnSetGuildFightInfo(short guild_fight_rank);
	bool IsMeetGuildHonorRank(int rank);
	void OnSetGuildHonorInfo(short guild_honor_rank);
	void OnSetColosseumInfo(int colosseum_rank);
	int GetColosseumRank();
	void OnFetchGuildHongBao();
	void OnCorrectGuildAnswer();

	bool HasMovePrestigeData();
	void OnSetMovePrestigeDataFlag();

	void OnSetWorldTeamArenaTimestamp(unsigned int set_timestamp);
	void OnSetWorldTeamArenaInfo(int team_rank, unsigned int set_timestamp, const int * title_id = NULL);
	WorldTeamArenaRoleInfo GetWorldTeamArenaRankInfo();

	void OnChannelChat(int channel_type);
	int GetChannelChatByType(int channel_type);

	Guild * GetGuild();

	//新型万能卡类型
	bool OnNewBuyGiftItem(int param1, int param2, int param3, const char * reason);
	//一元万能卡类型
	int OnBuyGiftItem(int param1, int param2, int param3, int use_item_id, const char * reason, bool is_need_consume);
	bool IsCanSendMailByBuyGiftItem(int buy_gift_item_type) const;

	void CommonDataCheck();
	static unsigned int CalcCloudArenaNewShopOpenTimestamp(unsigned int cur_season_start_timestamp,
		unsigned int cur_season_end_timestamp, unsigned int config_new_shop_timestamp);

	GET_ROLE_MODULE(ROLE_MODULE_TYPE_EXCHANGE, Exchange);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_BUFF_MANAGER, BuffManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TASK_CHAIN, TaskChain);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_VIP, Vip);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTRIBUTE, RoleAttributePlan);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MONITOR, Monitor);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MONEY, Money);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_KNAPSACK, Knapsack);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_PET, Pet);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_PARTNER, Partner);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE, Prestige);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_COLLECTION, Collection);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TITLE, Title);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TRAIN, Train);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TASKRECORDER, TaskRecorder);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TASKMANAGER, TaskManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_WST_QUESTION, WstQuestion);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_SKILL, RoleSkill);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_DAY_REWARD, DayReward);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_EQUIPMENT, EquipmentManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_WORKSHOP, Workshop);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SURFACE, Surface);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MARRIAGE, Marriage);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_BABY, Baby);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FRIEND_RECOMMEND, FriendRecommend); 
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_SETTING, RoleSetting);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SHI_TU, ShiTu);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_NOTICE, SystemNotice);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_COURAGE_CHALLENGE, CourageChallenge);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MONSTER_WAVE, MonsterWave);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVITY_MANAGER, RoleActivityManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MI_YI, MiYi);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_TASK, SchoolTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_OFFLINE_EXP, OfflineExp);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ESCORT, Escort);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_MAP, TreasureMap);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FABAO, FaBao);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_RESEARCH_TASK, ResearchTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MEDAL, Medal);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CRYSTAL_FB, RoleCrystalFb);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SHOP, Shop);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_UNLOCK, MapUnlock);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_FB, RoleGatherFb);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLECROSS, RoleCross);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_COURSE_OF_GROWTH, CourseOfGrowth);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SALARY, Salary);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_CHALLENGE_FB, RoleChallengeFb);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FACE_SCORE, FaceScore);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_GUIDE, FuncGuide);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_DROP, RoleDrop);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CONVERT_SHOP, RoleConvertShop);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SPECIAL_APPEARANCE, SpecialAppearance);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MAIL, RoleMail);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_LOGIN_SYSTEM, LoginSystem);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_BOSS_CHAPTER, BossChapter);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CAPABILITY, Capability);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MYSTERIOUS_TRIAL, MysteriousTrial);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_BOUNTY_TASK, BountyTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TRANSACTION_TASK, TransactionTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVE_SYSTEM, ActiveSystem);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_HUASHEN_MANAGER, HuaShenManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_PREVIEW, FuncPreview);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FIRST_KILL, RoleFirstKill);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_AUTO_CATCH_PET, AutoCatchPet);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TRADE_CREDIT, TradeCredit);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FORMATION, RoleFormation);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_DREAM, RoleGatherDream);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_JIE, RoleHuanJie);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ANECDOTE_TASK, AnecdoteTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_LEVEL_COMPLEMENT, LevelComplement);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_DREAM_NOTES_CHALLENGE, DreamNotesChallenge);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_EXERCISE, SchoolExercise);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_COMMON_SAVE_MANAGER, RoleCommonSaveManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SMART_MOUNTS_SYSTEM, SmartMountsSystem);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_REALM_BREAK, RealmBreak);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_PET_MONOPOLY, PetMonopoly);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_LUN_HUI_WANG_CHUAN, LunHuiWangChuan);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MATERIAL, RoleMaterial);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_EXPLORATION, MapExploration);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_JOIN_ACTIVITY_REWARD, RoleJoinActivityReward);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_CUSTOM_INSTRUCTION, CustomInstruction);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GUILD_INBORN, RoleGuildInborn);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HEART_SKILL, RoleHeartSkill);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ZHU_LING, RoleZhuLing);
	//GET_ROLE_MODULE(ROLE_MODULE_TYPE_TURNTABLE, Turntable);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_PET_HELPER_FORMATION, PetHelperFormation);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_NETHER_WORLD_PURGATORY, NetherWorldPurgatory);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_PET_GOD_FB, PetGodFB);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_BIG_DIPPER, BigDipper);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN, RoleTianDiDaoHen);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GIVE_GIFT, RoleGiveGift);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE_TASK, PrestigeTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SVIP, SVip);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTR_PELLET, RoleAttrPellet);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_DUEL, Duel);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ACCESSORIES, AccessoriesmentManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TIAN_MING, TianMing);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_DU_JIE, DuJie);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_YIN_YANG_EYE, YinYangEye);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ENDLESS_TOWER, EndlessTower);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ADVANCE_CAREER, AdvanceCareer);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_WAN_LING_FB, RoleWanLingFB);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LIFE_SKILL, RoleNewLifeSkill);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FLY_UP_SYSTEM, FlyUpSystem);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MIRAGE_TERRIROTY, MirageTerritory);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_XIU_WEI, XiuWei);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_TASK, TreasureTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FALLEN_GOD, FallenGod);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_MINI_GAME, MiniGame);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_PEAK_TOKEN, RolePeakToken);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_INCOME, RoleIncome);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LEVEL_COMPLEMENT, RoleNewLevelComplement);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_YAO_XING_SHI_LIAN, YaoXingShiLian);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_SAN_XIAN_GUI_DONG, SanXianGuiDong);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_LANTERN_RIDDIE, LanternRiddie);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_WHERE_IS_THE_FAIRY, RoleWhereIsTheFairy);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_GONG_XUN_BAO, RoleTianGongXunBao);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_LOVERS_MARKET, LoversMarket);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_TAO_QUAN_NA_JI, TaoQuanNaJi);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_CONCENTRIC_KNOT, ConcentricKnot);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUNDRED_GOD_BOSS, RoleHundredGodBoss);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ZAO_HUA_MANAGER, ZaoHuaEquipManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_HONGMENG_TIANJIE, RoleHongMengTianJie);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_HOLY_EQUIP, HolyEquipManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_KE_JU_EXAM, KeJuExam);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ZHOU_QI_SHOP, ZhouQiShop);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ELEMENT_ELF, ElementElf);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FU_YU, FuYuManager);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_KUA_FU_YING_XIONG_LU, RoleKuaFuYingXiongLu);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_STAR_CHART_SYSTEM, StarChart);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_NEW_COURAGE_CHALLENGE, NewCourageChallenge);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_FASHION_COLLECTION, FashionCollection);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_GUILD_TASK, RoleGuildTask);
	GET_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_SHOU, RoleHuanShou);

	UNSTD_STATIC_CHECK(135 == ROLE_MODULE_TYPE_MAX); 

	int ClearMixedElementCrystal();				//线上老玩家所获得的混合元素水晶将自动回收，且需要按照每个混合元素水晶转换为5000铜币通过邮件返还 ;  需要检查的地方有背包,仓库,角色身上佩戴水晶,已激活伙伴身上佩戴水晶

	//野外首领
	void OnWildBossWeekChange();
	void OnSendWildBossOtherInfo();
	
	//北斗七星
	void OnBigDipperFightSucc();
	int GetBigDipperFightSuccTimes() { return m_common_data_param.big_dipper_fight_succ_times; }

	//巅峰等级
	void OnTopLevelUpdate(unsigned int now_second);
	void OnTopLevelDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnFetchTopLevelReward(int top_level);
	void OnFetchTopLevelAchieveReward(int seq);
	void OnSetTopLevelNoticeFlag();
	bool IsOpenTopLevelNotice();
	void SendTopLevelAllInfo();
	void SendTopLevelNotice(int notice_type, short param1, int param2);

	/////////////////////////许愿池///////////////////////////////////////
	void OnTreviFountainBlessing();
	void SendTreviFountainInfo();
	void OnTreviFountainResetData(int old_dayid, int now_dayid);
	void OnTreviFountainDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnTreviFountainFindDataReset();
	bool OnTreviFountainFindReward(int consume_type, int count);


	/////////////////////////四象幻境///////////////////////////////////////
	void OnFantasyResetData(int old_dayid, int now_dayid);
	void OnFantasyDayChange(int old_dayid, int now_dayid);
	void OnFantasyFindDataReset();
	bool OnFantasyFindReward(int consume_type, int count);
	int GetFantasyFindCount(int comsume_type, int *layer = NULL, int len = 0);
	int GetFantasyTodayFinishNum(int *layer = NULL, int length = 0, int *ret_len = NULL);
	void OneKeyFinishFantasyFB();
	void SendFantasyFbFinishInfo();

	void SendGuildShopInfo(int seq = -1);
	bool NetSend(const void *data, unsigned int length);
	void SendAllRoleInfo();
	void SendHearsayInfo(int op_type, int param1 = 0);


	/////////////////////////组装战斗数据//////////////////////////////////
	void CollectRoleBattleData(RoleBattleData* out_data, bool need_partner, bool must_use_max_hp_mp = true, bool need_pet = true, bool use_system_food = false);
	void AddJinjieSpidToRoleBattleData(RoleBattleData* out_data, int jinjie_spid);
	bool GetRoleBattleData(BattleCharacterData* out_data, bool is_use_cur_hp_mp, bool use_system_food);
	bool GetPetBattleData(short max_pet_num, short* out_pet_num, PetBattleData* out_data_list,
		short* out_fight_pet_idx, bool is_use_cur_hp_mp);
	bool GetPartnerBattleData(int max_partner_num, short* out_partner_num, PartnerBattleData* out_data_list, bool is_use_cur_hp_mp);
	void AddGmAttr(BattleCharacterData* out_data);
	void MinusOriginFoodAttr(BattleCharacterData* out_data);
	void AddFoodAttr(BattleCharacterData* out_data);

	const Appearance& GetAppearance() { return m_appearance; }
	void MakeAppearanceData();
	void NoticeNum(int notice_num);

	//----------------------------------role_cmd--------------------------
	bool  OnRoleCmd(const std::string & cmd,unsigned long long create_time);
	void SetGetRoleCmdInfo(time_t next_time,long long start_cmd_id);
	void GetRoleCmdNow();
	//-------------寻宝
	bool IsXunBaoFirstDraw(int draw_type);
	void SetXunBaoFirstDraw(int draw_type);
	int GetXunBaoFirstDrawTimes(int draw_type);
	void AddXunBaoFirstDrawTimes(int draw_type, int add_times);

	bool IsXunBaoSecondDraw(int draw_type);
	void SetXunBaoSecondDraw(int draw_type);

	int GetXunBaoDrawCumulativeTimes(int group_id);
	void SetXunBaoDrawCumulativeTimes(int group_id, int times);
	RoleCommonSaveXunBao * GetXunBaoInfo(int draw_type, bool set_dirty = true);
	void SetXunBaoInfoDirty(int draw_type);
	void SendXunBaoInfo();
	//----------------
	bool ConsumeSuffList(const std::vector<ItemExtern::ItemConsumeConfig> & stuff_list,const char * reson);
	//---------------------------------勇闯地宫-------------------------------------------
	void SetBraveGroundMaxReachedLevel(int reached_level);
	int GetBraveGroundMaxLevelByWeek();
	void SendBraveGroundInfo();
	void BraveGroundOnWeekChange();
	void OnBraveGroundResetByFlag();
	//--------------------------幻界战场------------------------
	void CheckHuanJieSeasonChange();
	void OnHuanJieSeasonChange();
	//--------------------------山海降妖------------------------
	int GetShanHaiBossChallengeTime() { return m_common_data_param.shan_hai_boss_challenge_time; }
	int GetShanHaiBossWeekChallengeTime() { return m_common_data_param.shan_hai_boss_week_challenge_time; }
	bool HasRemainShanHaiBossDayTimes();
	void SetShanHaiBossChallengeDayId();
	bool IsChallengeShanHaiBoss(unsigned char cfg_seq);
	void ResetShanHaiSomeData(unsigned long interval, time_t now_second, unsigned int now_dayid);
	void OnKillShanHaiMonster(int monster_id, int cfg_seq);
	void OnShanHaiRunAwayOrForceFinishBattle() { m_common_data_param.shan_hai_boss_challenge_day_id = 0; }
	int GetPassShanHaiTimes() { return m_common_data_param.shan_hai_boss_pass_times; }
	void GmResetShanHaiDailyChallenge();
	//--------------------------防拉人-私聊限制------------------------
	void ResetSingleChatNumData();
	unsigned int GetSingleChatNumTime();
	int GetSingleChatNumNum();
	bool IsInSingleChatNumList(int to_uid);
	void SetSingleChatNumTime(unsigned int time);
	void AddSingleChatNumUser(int to_uid);
	//-------------------------退出家族限制---------------------------
	void AddTodayQuitGuildCount();
	bool CanApplyJoinGuild();
	void SendQuitGuildInfo();

	//------------
	int GetMaxChongZhiRecord();
	void OnAddChongZhiRecord(int chongzhi);

	bool GmBuyItem(std::istringstream & iss);
	bool GmBuyItem2(int buy_item_type);

	void GmShanHaiBossAddSoulFragment(int star);
	void GmResetShanHaiChanllengeCount();

	bool FindEmptyCrossGroupIndex(ARG_OUT int* index);
	bool IsInCreateChatGroupCD(ARG_OUT unsigned int* remain_minutes);
	void SetCreateChatGroupCD();

	void UpdateCloudArenaData(unsigned int season_timestamp, int last_season_rank, int season_highest_dan_id, int season_highest_rank, int season_highest_score);
	void CalculateCloudArenaTitle(int last_season_rank, unsigned int expire_timestamp);
	void SendCloudArenaSeasonHighestData();
	bool IsOpenCloudArenaNewShop();

	std::string GetRoleInfoString()const;

	// 红包
	void AddHongBaoTimes(int _hongbao_type , const char * _reason , int _add_times = 1);
	bool UseHongBaoTimes(int _hongbao_type , const char * _reason, int _use_times = 1);
	int GetHongBaoTimes(int _hongbao_type);
	void SendHongBaoInfo();
	void SendHongBaoInfoChange(int _hongbao_type);

	// 资源下载
	void FetchResourceDownloadReward();
	void SendResourceDownload();

	// 领取防诈骗回答奖励
	void FetchFangFraudReward(int count, short *list);
	void SendFangFraudInfo();

	//系统邀战 战力目标
	void UpdateCapacityTarget(time_t now);
	void CheckStartCapacityTarget(int task_id);
	void CapacityTargetFetchReward();
	void StartCapacityTarget();
	void SendCapacityTargetInfo();

	int GetProfAdvanceSpecialEffectId();
	int GetRoleFeiShengTimes();

	//评分任务达成记录日志
	void ScoreTaskFinish();

	int GetMiniGameId() const;
	void SetMiniGameId(int mini_game_id);

	void SendTuZiKuaiPaoInfo();
	int GetTuZiKuaiPaoRewardTimes();
	void SetTuZiKuaiPaoRewardTimes(int reward_times);
	void OnDayChangeTuZiKuaiPao();

	// 领取评分有礼奖励
	void FetchPingJiaReward();
	void SendPingJiaInfo();

	//周期活动
	void CheckZhouQiInfo();
	void OnZhouQiInfoChange(int old_zhou_qi, unsigned int old_zhou_end_time);

	//我要上学
	void GotoSchoolGetReward(int reward_type);
	void GotoSchoolOnDayChange();
	void GotoSchoolSendInfo();

	void SendGuaJiExpInfo();

	void ReceiveRealNameReward();
	void SendRealNameRewardInfo();

	void GmSetWaiguaCheckFlag(bool flag);
	bool CheckWaiguaMove() const { return 0 == m_common_data_param.gm_check_waigua_flag; }
private:
	void GetRoleCmdFromDB(time_t now_second);
	bool CmdBuyItem(std::istringstream & iss, std::string & error_msg, unsigned long long create_time);
	bool CmdChongzhi(std::istringstream & iss, std::string & error_msg, unsigned long long create_time);
	bool CmdChongzhiBudan(std::istringstream & iss, std::string & error_msg, unsigned long long create_time);

	void GetJieyiList(ARG_OUT int* jieyi_num, ARG_OUT int* jieyi_uid_list, int max_jieyi_num);
	void GetJieyiSkill(ARG_OUT int* jieyi_skill_num, ARG_OUT int* jieyi_skill_list, int max_skill_num);

	void GetSoaringSkill(ARG_OUT UInt16* skill_id, ARG_OUT short* skill_lv);
	int BuyGiftItem(int param1, int param2, int param3, ARG_OUT int * price = NULL);
	int GetQingyuanTargetUID();
	void GetQingyuanSkill(ARG_OUT int* qingyuan_skill_num, ARG_OUT int* qingyuan_skill_list, int max_skill_num);
	int GetCareerSuppressValue();
	void GetBattleHallows(ARG_OUT short* hallow_num, ARG_OUT BattleHallowInfo* hallows, ARG_OUT short* fight_hallow_idx, int max_hallow_num);

	long long m_start_role_cmd_id;

	Role* m_role;

	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_EXCHANGE, Exchange);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TASK_CHAIN, TaskChain);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_VIP, Vip);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTRIBUTE, RoleAttributePlan);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MONITOR, Monitor);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MONEY, Money);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_KNAPSACK, Knapsack);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_PET, Pet);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_PARTNER, Partner);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE, Prestige);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_COLLECTION, Collection);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TITLE, Title);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TRAIN, Train);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TASKRECORDER, TaskRecorder);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TASKMANAGER, TaskManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_WST_QUESTION, WstQuestion);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_SKILL, RoleSkill);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_DAY_REWARD, DayReward);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_EQUIPMENT, EquipmentManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_WORKSHOP, Workshop);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SURFACE, Surface);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MARRIAGE, Marriage);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_BABY, Baby);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FRIEND_RECOMMEND, FriendRecommend)
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_SETTING, RoleSetting);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SHI_TU, ShiTu);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SYSTEM_NOTICE, SystemNotice);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_COURAGE_CHALLENGE, CourageChallenge);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MONSTER_WAVE, MonsterWave);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVITY_MANAGER, RoleActivityManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MI_YI, MiYi);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_TASK, SchoolTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_OFFLINE_EXP, OfflineExp);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ESCORT, Escort);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_MAP, TreasureMap);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FABAO, FaBao);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_RESEARCH_TASK, ResearchTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MEDAL, Medal);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CRYSTAL_FB, RoleCrystalFb);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_BUFF_MANAGER, BuffManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SHOP, Shop);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_UNLOCK, MapUnlock);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_FB, RoleGatherFb);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLECROSS, RoleCross);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_COURSE_OF_GROWTH, CourseOfGrowth);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SALARY, Salary);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_CHALLENGE_FB, RoleChallengeFb);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FACE_SCORE, FaceScore);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_GUIDE, FuncGuide);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_DROP, RoleDrop);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CONVERT_SHOP, RoleConvertShop);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SPECIAL_APPEARANCE, SpecialAppearance);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MAIL, RoleMail);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_LOGIN_SYSTEM, LoginSystem);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_BOSS_CHAPTER, BossChapter);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_CAPABILITY, Capability);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MYSTERIOUS_TRIAL, MysteriousTrial);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_BOUNTY_TASK, BountyTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TRANSACTION_TASK, TransactionTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ACTIVE_SYSTEM, ActiveSystem);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_HUASHEN_MANAGER, HuaShenManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FUNC_PREVIEW, FuncPreview);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FIRST_KILL, RoleFirstKill);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_AUTO_CATCH_PET, AutoCatchPet);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TRADE_CREDIT, TradeCredit);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_FORMATION, RoleFormation);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_GATHER_DREAM, RoleGatherDream);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_JIE, RoleHuanJie);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ANECDOTE_TASK, AnecdoteTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_LEVEL_COMPLEMENT, LevelComplement);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_DREAM_NOTES_CHALLENGE, DreamNotesChallenge);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SCHOOL_EXERCISE, SchoolExercise);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_COMMON_SAVE_MANAGER, RoleCommonSaveManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SMART_MOUNTS_SYSTEM, SmartMountsSystem);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_REALM_BREAK, RealmBreak);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_PET_MONOPOLY, PetMonopoly);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_LUN_HUI_WANG_CHUAN, LunHuiWangChuan);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_MATERIAL, RoleMaterial);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MAP_EXPLORATION, MapExploration);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_JOIN_ACTIVITY_REWARD, RoleJoinActivityReward);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_CUSTOM_INSTRUCTION, CustomInstruction);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GUILD_INBORN, RoleGuildInborn);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HEART_SKILL, RoleHeartSkill);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ZHU_LING, RoleZhuLing);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_PET_HELPER_FORMATION, PetHelperFormation);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_NETHER_WORLD_PURGATORY, NetherWorldPurgatory);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_PET_GOD_FB, PetGodFB);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_BIG_DIPPER, BigDipper);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_DI_DAO_HEN, RoleTianDiDaoHen);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_GIVE_GIFT, RoleGiveGift);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_PRESTIGE_TASK, PrestigeTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SVIP, SVip);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_ATTR_PELLET, RoleAttrPellet);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_DUEL, Duel);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ACCESSORIES, AccessoriesmentManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TIAN_MING, TianMing);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_DU_JIE, DuJie);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_YIN_YANG_EYE, YinYangEye);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ENDLESS_TOWER, EndlessTower);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ADVANCE_CAREER, AdvanceCareer);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_WAN_LING_FB, RoleWanLingFB);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LIFE_SKILL, RoleNewLifeSkill);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FLY_UP_SYSTEM, FlyUpSystem);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MIRAGE_TERRIROTY, MirageTerritory);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_XIU_WEI, XiuWei);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TREASURE_TASK, TreasureTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FALLEN_GOD, FallenGod);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_MINI_GAME, MiniGame);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_PEAK_TOKEN, RolePeakToken);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_INCOME, RoleIncome);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_NEW_LEVEL_COMPLEMENT, RoleNewLevelComplement);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_YAO_XING_SHI_LIAN, YaoXingShiLian);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_SAN_XIAN_GUI_DONG, SanXianGuiDong);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_LANTERN_RIDDIE, LanternRiddie);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_WHERE_IS_THE_FAIRY, RoleWhereIsTheFairy);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_TIAN_GONG_XUN_BAO, RoleTianGongXunBao);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_LOVERS_MARKET, LoversMarket);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_TAO_QUAN_NA_JI, TaoQuanNaJi);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_CONCENTRIC_KNOT, ConcentricKnot);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUNDRED_GOD_BOSS, RoleHundredGodBoss);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ZAO_HUA_MANAGER, ZaoHuaEquipManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_HONGMENG_TIANJIE, RoleHongMengTianJie);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_HOLY_EQUIP, HolyEquipManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_KE_JU_EXAM, KeJuExam);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ZHOU_QI_SHOP, ZhouQiShop);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ELEMENT_ELF, ElementElf);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FU_YU, FuYuManager);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_KUA_FU_YING_XIONG_LU, RoleKuaFuYingXiongLu);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_STAR_CHART_SYSTEM, StarChart);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_NEW_COURAGE_CHALLENGE, NewCourageChallenge);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_FASHION_COLLECTION, FashionCollection);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_GUILD_TASK, RoleGuildTask);
	DEFINE_ROLE_MODULE(ROLE_MODULE_TYPE_ROLE_HUAN_SHOU, RoleHuanShou);

	UNSTD_STATIC_CHECK(135 == ROLE_MODULE_TYPE_MAX);

	GuildCommonParam m_guild_common_param;							// 家族通用数据
	CommonDataParam	m_common_data_param;							// 通用数据
	AttributeList m_attr_list;										// 玩家总属性
	AttributeList m_attrs_guild_qifu;								// 家族祈福属性
	AttributeList m_base_attr;										// 基础属性
	AttributeList m_trevi_fountain_buff_attrs_add;					// 许愿池buff
	AttributeList m_base_attr_prof;									// 职业进阶属性

	AttributeList m_attr_reason_list[MAX_REASON_TYPE_LIST_NUM];		// 角色属性来源列表

	int	m_role_module_ref_counter[ROLE_MODULE_TYPE_MAX];

	unsigned int m_last_search_trade_market_time;
	unsigned int m_next_send_auto_talk_timestamp;					// 下次下发自动对话时间戳

	Appearance m_appearance;										// 外观（不会从数据库向游戏覆盖数据， init 时会重新赋值）

	ClientPlayMode m_client_play_mode;

	bool m_patrol_flag;

	NeedMonsterMap m_need_monster_map;

	unsigned int m_next_get_role_cmd_time;
	DBRoleCommandHandlerMap m_role_cmd_handler_map;

	// 好友通用数据 , 好友系统调整后 这东西不知道该放哪了
	FriendCommonParam m_friend_common_param;

	int m_mini_game_id;
};

#endif