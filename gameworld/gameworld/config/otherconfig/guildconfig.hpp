#ifndef __GUILD_CONFIG_HPP__
#define __GUILD_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/guilddef.hpp"
#include "servercommon/partnerdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/petdef.h"
#include "servercommon/roleguildinborndef.hpp"
#include "servercommon/hongbaodef.hpp"
#include <map>
#include <vector>

const static int MAX_GATHER_DREAM_TYPE_NUM = PQT_MAX > MAX_PET_QUALITY_NUM ? PQT_MAX : MAX_PET_QUALITY_NUM;

struct GuildOtherCfg
{
	GuildOtherCfg() : create_cost(0), create_cost_type(0),level(0), exp_time(0), training_max_time(0), protect_time(0), max_coin(0LL), guild_boss_play_times(0),
	fund_return_rate(0), initial_funding(0), station_scene_id(0), change_badge_coin(0), pray_family_level(0), pray_give_maxnum(0), redbag_time_limit(0)
		,boss_round_limit(0), serve_build_capital(0), expend_coin(0), get_contribution(0), speed_up(0), speed_times(0), person_times(0), building_limit(0),
		post_title_id(0)
	{
		tan_he_zu_zhang_logout_time_length = 0;
		tan_he_zu_zhang_continued_time = 0;
	}

	int create_cost;
	int create_cost_type;
	int level;
	int exp_time;
	int training_max_time;
	int protect_time;
	long long max_coin;
	int guild_boss_play_times;
	int fund_return_rate;			// 家族降级返还资金比例 - 百分比 %
	int initial_funding;			// 家族初始资金
	int station_scene_id;			// 家族驻地的场景ID
	int guild_rename_need_itemid;	// 家族改名需要的道具
	int change_badge_coin;			// 改族徽需要的铜币消耗
	int pray_family_level;			// 集愿所需家族等级
	int pray_give_maxnum;			// 集愿每日最大赠予数量	
	unsigned int tan_he_zu_zhang_logout_time_length;		//族长离线多久能弹劾
	unsigned int tan_he_zu_zhang_continued_time;				//族长弹劾持续时间

	unsigned int redbag_time_limit;					// 红包存在时限
	int boss_round_limit;			// 试炼boss限制回合数
	int bless_consume;
	int bless_all_add;
	int rank_amount;
	int bless_open_level;
	int bless_open_fund;
	int serve_build_capital;		//建筑维护费
	int expend_coin;				//加速消耗铜币
	int get_contribution;			//加速获得贡献
	int speed_up;					//加速时间
	int speed_times;				//加速开启次数
	int person_times;				//单人加速次数
	int building_limit;				//每日建造次数限制上限
	std::pair<int, int> first_charge_create_cost;	//p1:首充后建立家族花费货币类型 p2:首充后简历家族花费货币数量
	int post_title_id;				//家族成员称号
};

struct GuildRoomCfg
{
	GuildRoomCfg() : level(0), exp(0), max_member(0), cost(0), hearsay(0), skill_points(0), life_skill_level_limit(0)
	{}

	int level;
	int exp;						// 升级所需家族资金
	int max_member;
	int cost;						// 家族每日维护
	int hearsay;
	int skill_points;				// 家族灵脉技能点数
	int life_skill_level_limit;		// 生活技能等级限制
};

enum PRIVILEGE_TYPE
{
	APPROVAL,					//审批	
	APPOINT,					//任命	
	KICK,						//踢人	
	NOTICE,						//修改公告
	IMPEACH,					//弹劾族长
	GUILD_TASK_MAIL_NOTICE,				//邮件通知做家族任务
	PRIVILEGE_TYPE_MAX,
};

struct GuildMemberTypeCfg
{
	GuildMemberTypeCfg()
	{
		member_type = 0;
		max_num = 0;
		memset(privilege, 0, sizeof(privilege));
		modify_target = 0;
		level_up = 0;
		build_limit = 0;
	}

	int member_type;
	int max_num;
	int privilege[PRIVILEGE_TYPE_MAX];
	int modify_target;					// 攻打目标
	int level_up;						// 升级家族权限
	int build_limit;					// 建筑建造/拆除权限
};

struct GuildPetTrainingCfg
{
	GuildPetTrainingCfg()
	{
		seq = 0;
		type = 0;
		level = 0;
		guild_level = 0;
		gongxian_cost = 0;
		coin_type = 0;
		coin_cost = 0;
		add_param = 0;
	}

	int seq;
	int type;
	int level;
	int guild_level;
	int gongxian_cost;
	int coin_type;
	int coin_cost;
	int add_param;
};

enum BUY_LIMIT_TYPE
{
	BUY_LIMIT_TYPE_UNLIMITED = -1,
	BUY_LIMIT_TYPE_DAY = 0,
	BUY_LIMIT_TYPE_WEEK = 1,

	BUY_LIMIT_TYPE_MAX
};

enum BUY_COST_TYPE
{
	BUY_COST_TYPE_GOLD = 0,
	BUY_COST_TYPE_IMMORTAL_COIN,
	BUY_COST_TYPE_BIND_COIN,
};

struct GuildShopCfg
{
	GuildShopCfg()
	{
		seq = 0;
		level = 0;
		item_id = 0;
		present_cost = 0;
		gongxian_cost = 0;
		buy_type = 0;
		buy_num = 0;
		cost_type = 0;
	}
	int seq;
	int level;
	int item_id;
	int present_cost;
	int gongxian_cost;
	int buy_type;
	int buy_num;
	int cost_type;
};

struct GuildBossCfg
{
	GuildBossCfg() : world_level(0)
	{

	}

	struct GuildBossCfgItem
	{
		GuildBossCfgItem() : boss_index(0), boss_group_id(0), join_coin(0), join_gongxian(0), family_coin(0), immortal(0), item_count(0)
		{}

		int boss_index;
		int boss_group_id;
		int join_coin;
		int join_gongxian;
		int family_coin;
		int immortal;		//灵玉奖励 

		int item_count;
		ItemConfigData kill_reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];
	};

	int world_level;
	GuildBossCfgItem guild_boss_cfg;
};

struct GuildBossChapterRewardCfg
{
	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			coin = 0;
			gongxian = 0;
			guild_coin_per_join = 0;
			item_count = 0;
		}

		int rate;
		int coin;
		int gongxian;
		int guild_coin_per_join;
		int item_count;
		ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];
	};

	GuildBossChapterRewardCfg()
	{
		rate_count = 0;
	}

	int rate_count;
	std::vector<ItemCfg> group_reward;
};

struct GuildJournalCfg
{
	GuildJournalCfg():type(0), condition(0)
	{}

	int type;
	int condition;
};

struct GuildBaoDiRewardCfg			//保底奖励
{
	GuildBaoDiRewardCfg() : exp(0), bind_coin(0), reward_group_id(0)
	{}

	int exp;
	int bind_coin;
	int reward_group_id;		//奖励组ID
};

struct GuildRankRewardCfg
{
	GuildRankRewardCfg() : min_rank(0), max_rank(0), exp(0), bind_coin(0), reward_group_id(0), immortal(0)
	{}

	int min_rank;
	int max_rank;
	int exp;
	int bind_coin;
	int reward_group_id;
	int immortal;			//灵玉奖励
};

struct GuildGatherDreamCfg
{
	GuildGatherDreamCfg() : is_can_pray(0), pray_patch_num(0), coin(0), gongxian(0), guild_exp(0)
	{}

	int is_can_pray;			//能否祈愿
	int pray_patch_num;			//单次集愿碎片数量
	int coin;					
	int gongxian;				
	int guild_exp;				//家族资金
	ItemConfigData reward_item;
};

struct GuildActiveGiftCfg												// 活跃度礼包
{
	struct GiftCfg
	{
		GiftCfg():active_limit(0), exp(0), count(0){}

		int active_limit;													// 活跃要求
		int exp;															// 经验奖励
		int count;															// 奖励配置表长
		ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];		// 奖励配置
	};

	GuildActiveGiftCfg() { this->Reset(); }

	void Reset()
	{
		level_min = 0;
		level_max = 0;
		gift_cfg_vec.clear();
	}

	int level_min;
	int level_max;

	std::vector<GiftCfg> gift_cfg_vec;
};

struct GuildActiveRankRewardCfg											// 家族活跃度排名奖励
{
	GuildActiveRankRewardCfg() :rank_min(0), rank_max(0) , gongxian(0) , coin(0), count(0) {}

	int rank_min;														// 排名下限
	int rank_max;														// 排名上限

	int gongxian;														// 贡献
	int coin;															// 铜币
	int count;															// 奖励配置表长
	ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];		// 奖励配置
};

struct GuildActivePersonRankRewardCfg									// 家族个人活跃度排名奖励
{
	GuildActivePersonRankRewardCfg() :rank_min(0), rank_max(0), title(0), gongxian(0), coin(0), count(0){}

	int rank_min;														// 排名下限
	int rank_max;														// 排名上限

	int title;															// 称号
	int gongxian;														// 贡献
	int coin;															// 铜币
	int count;															// 奖励配置表长
	ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];		// 奖励配置
};

struct GuildInbornLevelCfg
{
	GuildInbornLevelCfg() : inborn_skill_id(0), prepose_seq(0), level_limit(0), guild_level_limit(0), reset_consume(0), skill_grade(0), gain_target(0)
	{}

	int inborn_skill_id;
	int prepose_seq;
	int level_limit;
	int guild_level_limit;
	int reset_consume;
	int skill_grade;
	int gain_target;
	std::vector<ItemConfigData> consume_list;	
};

struct GuildInbornCfg
{
	GuildInbornCfg() : max_level(0)
	{}

	int max_level;
	GuildInbornLevelCfg level_list[NAX_GUILD_INBORN_SKILL_LEVEL_NUM];
};

struct GuildHongBaoCfg
{
	struct HongBaoCfg
	{
		HongBaoCfg() :lingyu_sum(0), redbag_num(0), lingyu_min(0), lingyu_max(0), lingyu_change(0), is_take_effect(false), day_times_limit(-1), need_chong_zhi(0), redbag_heroism(0) {}

		int lingyu_sum;					// 灵玉总数
		int redbag_num;					// 红包个数
		int lingyu_min;					// 灵玉最小值
		int lingyu_max;					// 灵玉最大值
		int lingyu_change;				// 灵玉变化值

		bool is_take_effect;			// 是否生效 [为了兼容旧数据 , 该字段表示该类型红包是否还会再获得] 
		int day_times_limit;			// 每日可获得次数限制 , -1表示不受限制

		int need_chong_zhi;				// 所需充值金额[充值红包为单笔充值 , 累充红包为累计充值]
		int redbag_heroism;				// 红包对应家族豪气值
	};

	HongBaoCfg hong_bao_cfg[HONGBAO_GUILD_TYPE_MAX];
};

struct HallOfFameRank
{
	HallOfFameRank() :type(0), rank_min(0), rank_max(0), title(0), gongxian(0), coin(0) {}
	int type;
	int rank_min;
	int rank_max;
	int title;
	int gongxian;
	int coin;
};

struct BlessTimesList
{
	BlessTimesList() :bless_list(0), gongxian_reward(0), cost_type(0), bless_consume(0), bless_add_once(0) {}
	int bless_list;
	int gongxian_reward;
	int cost_type;
	int bless_consume;
	int bless_add_once;
};

struct BlessAttrInfo
{
	BlessAttrInfo() :attr_type(0), attr_value(0) {}
	int attr_type;
	int attr_value;
};

struct BlessTimesAttr
{
	const static int MAX_ATTR_LIST_NUM = 6;
	BlessTimesAttr() :world_level(0){}
	int world_level;
	BlessAttrInfo attr_list[MAX_ATTR_LIST_NUM];
};

struct WarthAddInfo
{
	WarthAddInfo() :once_warth_add(0), captain_warth_add(0) {}
	int once_warth_add;
	int captain_warth_add;
};

const static int MAX_BLESS_TIMES = 5;//祈福次数

enum GUILD_BUILD_TYPE
{
	GUILD_BUILD_TYPE_HALL = 1,			// 大厅
	GUILD_BUILD_TYPE_STUDY = 2,			// 书房
	GUILD_BUILD_TYPE_GUEST_ROOM = 3,	// 厢房
	GUILD_BUILD_TYPE_VAULT = 4,			// 金库
	GUILD_BUILD_TYPE_STORE = 5,			// 商铺

	GUILD_BUILD_TYPE_MAX,
};

struct GuildBaseConstructCfg
{
	GuildBaseConstructCfg():up_expend_capital(0),up_expend_time(0),paramet_1(0)
	{}

	int up_expend_capital;	// 建筑升级所需资金
	int up_expend_time;		// 建筑升级需要的时间
	int paramet_1;			// 建筑效果
};

struct GuildConstructCfg
{
	GuildConstructCfg() :build_type(0)
	{}

	int build_type;
	std::map<int, GuildBaseConstructCfg> base_list;		//key-建筑等级
};


class GuildBossKey
{
public:
	bool operator < (const GuildBossKey& other) const
	{
		if (chapter_id != other.chapter_id)
		{
			return chapter_id < other.chapter_id;
		}
		else
		{
			return boss_index < other.boss_index;
		}
	}

	int chapter_id;
	int boss_index;
};

class GuildConfig : public ILogicConfig
{
public:
	GuildConfig();
	virtual ~GuildConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const GuildOtherCfg* GetOtherCfg() { return &m_other_cfg; }
	const GuildOtherCfg& GetOtherCfgRef() { return m_other_cfg; }
	const GuildRoomCfg* GetRoomCfg(int level);
	const GuildRoomCfg* GetRoomCfgByLevel(int level);
	const GuildMemberTypeCfg* GetMemberTypeCfg(int type);
	const GuildPetTrainingCfg* GetPetTrainingCfg(int type, int level);
	const GuildJournalCfg* GetJournalCfg(int type);
	const GuildBaseConstructCfg* GetConstructCfg(int build_type, int build_level);

	
	int GetMaxBuildLevel(bool is_hall = false)const { return is_hall ? m_hall_max_level : m_other_build_max_level; }

	//------------------家族试炼
	const GuildBossCfg::GuildBossCfgItem * GetBossCfg(int chapter_id);
	const GuildBossCfg* GetBossChapterCfg(int chapter_id);
	int GetBossChapterLimitWorldLevel(int chapter_id);
	int GetBossChapterByWorldLevel(int world_level);
	bool IsVaildGuildBossChapterId(int chapter_id);
	const GuildBossChapterRewardCfg::ItemCfg * GetGuildBossChapterRankReward(int reward_group_id);
	const GuildRankRewardCfg * GetRankRewardCfg(int chapter_id, int rank);
	//---------------------------家族集愿
	const GuildGatherDreamCfg * GetGatherDreamCfg(int quality, int type);

	int GetPetTrainingExp(int level);
	int GetPetTrainingGongXian(int level);

	void ResetBuyCount(int type, unsigned short shop_buy_list[GUILD_SHOP_COUNT_MAX]);
	const GuildShopCfg* GetShopCfg(int seq);
	int GetGuildBuildInit() { return m_guild_build_init; }

	// 家族活跃度 礼包及排行榜
	const GuildActiveGiftCfg::GiftCfg  * GetGuildActiveGiftCfg(int _seq , int _level);
	const GuildActiveRankRewardCfg * GetGuildActiveRankRewardCfg(int _rank);
	const GuildActivePersonRankRewardCfg * GetGuildActivePersonRankRewardCfg(int _rank);

	//家族灵脉
	const GuildInbornLevelCfg * GetGuildInbornCfg(int inborn_type, int type_seq, int level);
	int GetMaxGuildInbornLevel(int inborn_type, int type_seq) const;
	//重置数据,返还道具(后置链+原始目标)
	void GetGuildInbornResetSkill(int inborn_type, int seq, GuildInbornSkill * learn_list, int learn_count, ARG_OUT std::vector<ItemConfigData> & return_item_list) const;

	const GuildHongBaoCfg & GetGuildHongBaoCfg() { return m_hongbao_cfg; }
	const GuildHongBaoCfg::HongBaoCfg * GetGuildHongBaoCfgByType(int _hong_bao_type);

	const HallOfFameRank * GetRankTypeCfg(int type, int rank_idx);
	const WarthAddInfo* GetWarthAdd(int battle_mode);
	const BlessTimesList * GetBlessBaseCfg(int times);
	const BlessTimesAttr * GetBlessAttrCfg(int world_level);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRoomCfg(TiXmlElement * RootElement);
	int InitMemberTypeCfg(TiXmlElement * RootElement);
	int InitPetTrainingCfg(TiXmlElement * RootElement);
	int InitTrainingExpCfg(TiXmlElement * RootElement);
	int InitGuildShopCfg(TiXmlElement * RootElement);
	int InitGuildBossCfg(TiXmlElement* RootElement);
	int InitGuildBossChapterRewardCfg(TiXmlElement* RootElement);
	int InitGuildJournalCfg(TiXmlElement *RootElement);
	int InitGuildRankRewardCfg(TiXmlElement *RootElement);
	int InitGuildGatherDreamCfg(TiXmlElement *RootElement);
	int InitGuildActiveGiftCfg(TiXmlElement *RootElement);
	int InitGuildActiveRankRewardCfg(TiXmlElement *RootElement);
	int InitGuildActivePersonRankRewardCfg(TiXmlElement *RootElement);
	int InitGuildInbornCfg(TiXmlElement *RootElement);
	int InitGuildHongbaoCfg(TiXmlElement *RootElement);
	int InitGuildConstructCfg(TiXmlElement *RootElement);

	int InitGuildHofSortCfg(TiXmlElement *RootElement);
	int InitGuildWarthGetCfg(TiXmlElement *RootElement);
	int InitGuildBlessBaseCfg(TiXmlElement *RootElement);
	int InitGuildBlessRewardCfg(TiXmlElement *RootElement);

	int GetGuildInbornKey(int inborn_type, int type_seq) const;
	void CalcGuildInbornResetSkill(int guild_inborn_key, GuildInbornSkill * learn_list, int learn_count, ARG_OUT std::map<ItemID, int> & return_item_map) const;
	void GetGuildInbornReturnItem(int guild_inborn_key, int skill_level, ARG_OUT std::map<ItemID, int> & return_item_map) const;

	GuildOtherCfg m_other_cfg;
	std::map<int, GuildRoomCfg> m_room_map;
	std::map<int, GuildMemberTypeCfg> m_member_type_map;
	std::map<int, std::map<int, GuildPetTrainingCfg> > m_pet_training_map;
	std::map<int, int> m_training_exp_map;
	std::map<int, int> m_training_gongxian_map;
	std::map<int, GuildShopCfg> m_shop_map;
	int m_guild_build_init;

	 std::map<int, GuildBossCfg> m_boss_cfg_list;
	 std::map<int, GuildBossChapterRewardCfg> m_guild_boss_chapter_item_reward_map;				//道具奖励组 key:奖励组ID

	 std::vector<GuildJournalCfg> m_journal_cfg;      //家族日志

	 GuildRankRewardCfg m_rank_reward_cfg[MAX_GUILD_CHAPTER_BOSS_NUM][MAX_GUILD_BOSS_CHAPTER_RANK_NUM];	//试炼排名奖励 第一个为章节ID,第二个为序号
	 GuildGatherDreamCfg m_gather_dream_cfg[GUILD_GATHER_DREAM_ITEM_TYPE_MAX][MAX_GATHER_DREAM_TYPE_NUM];		//家族集愿配置

	 // 活跃度礼包
	 std::vector<GuildActiveGiftCfg > m_active_gift_cfg;								// 活跃度礼包配置
	 std::vector<GuildActiveRankRewardCfg > m_active_rank_reward_cfg;					// 家族活跃度排行榜奖励
	 std::vector<GuildActivePersonRankRewardCfg > m_active_person_rank_reward_cfg;		// 家族个人活跃度排行榜奖励

	 GuildInbornCfg m_guild_inborn_list[MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM][MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM];	//家族灵脉

	 std::map<int, std::vector<int> > m_guild_inborn_relation_map;		// 家族灵脉关系链(后置链) vector_value:inborn_type * 100 + type_seq
	 GuildHongBaoCfg m_hongbao_cfg;

	 std::map<int, std::vector<HallOfFameRank> > m_hof_sort_map;
	 std::map<int, WarthAddInfo> m_warth_map;
	 BlessTimesList bless_time_config[MAX_BLESS_TIMES];
	 std::map<int, BlessTimesAttr> m_bless_attr_map;

	 std::vector<GuildConstructCfg> m_guild_construct_cfg;			// 家族内政建设
	 int m_hall_max_level;
	 int m_other_build_max_level;
};

#endif