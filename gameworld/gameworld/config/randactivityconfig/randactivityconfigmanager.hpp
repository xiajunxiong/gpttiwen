#ifndef __RAND_ACTIVITY_CONFIG_MANAGER_HPP__
#define __RAND_ACTIVITY_CONFIG_MANAGER_HPP__

#include "servercommon/activitydef.hpp"
#include "randactivityconfig.hpp"
#include "servercommon/mysterioustrialdef.hpp"
#include "servercommon/loginsystemdef.hpp"

static int const MAX_RAND_ACTIVITY_CFG_COUNT = RAND_ACTIVITY_TYPE_MAX;

struct RandActivityOtherCfg
{
	RandActivityOtherCfg() :extra_turntable_time(0), kill_boss_num(0), monthly_top_up_ingot(0), spend_ingots_once(0), mysterious_cfg_ver(0), month_investment_cfg_ver(0)
	{}

	int extra_turntable_time;			//下一次免费转盘的时间
	int kill_boss_num;					//可用击杀数换取一次转盘机会
	int monthly_top_up_ingot;			//月度投资所需充值的元宝
	int spend_ingots_once;				//消费转盘转动一次所需的元宝

	int mysterious_cfg_ver;				//神秘试炼版本号
	int month_investment_cfg_ver;		//月度投资版本号

	int universal_card_item_id;			//万能激活卡道具ID
};


struct RAPatrolMiningCfg			//巡逻与采集
{
	RAPatrolMiningCfg() :activity_type(0), rate(0)
	{}

	int activity_type;			//活动类型
	int rate;					//权重
	ItemConfigData item;		//与该活动关联的道具
};

struct RAFieldBossCfg			//野外BOSS
{
	RAFieldBossCfg() :activity_type(0), rate(0)
	{}

	int activity_type;			//活动类型
	int rate;					//权重
	ItemConfigData item;		//与该活动关联的道具
};

struct RACrystalBossCfg			//水晶副本BOSS
{
	RACrystalBossCfg() :activity_type(0), rate(0)
	{}

	int activity_type;			//活动类型
	int rate;					//权重
	ItemConfigData item;		//与该活动关联的道具
};

struct RAPatrolMiningMoreCfg			//多组巡逻与采集
{
	RAPatrolMiningMoreCfg() :activity_type(0), prize_id(0) {}

	int activity_type;			//活动类型
	int prize_id;				//奖池ID
	int weight;
};

struct RAFieldCrystalFBMoreCfg			//多组野外水晶副本
{
	RAFieldCrystalFBMoreCfg() :activity_type(0), prize_id(0) {}

	int activity_type;			//活动类型
	int prize_id;				//奖池ID
	int weight;
};

struct RAMorePrizeCfg			//运营活动奖励池
{
	RAMorePrizeCfg() {}

	int weight;
	ItemConfigData item;
};

struct ScoreReward
{
	ScoreReward() :seq(0), score(0), reward_count(0) {}

	int seq;
	int score;									//人物评分
	int reward_count;
	ItemConfigData reward[10];					//道具奖励
};

struct MonthInvestmentCfg
{
	MonthInvestmentCfg() :days(0), seq(0)
	{}

	int days;
	int seq;
	ItemConfigData item;
};

struct MonthPurchaseRewardCfg
{
	MonthPurchaseRewardCfg() :seq(0)
	{}

	int seq;
	ItemConfigData item;
};


struct MysteriousTrialCfg
{
	MysteriousTrialCfg() :start_time(0), seq(0), activity_type(0), parameter1(0), parameter2(0), parameter3(0), parameter4(0), reward_id(0)
	{}

	int start_time;							// 哪一天
	int seq;
	int activity_type;						// 活动类型
	int parameter1;							// 0:直接判断   1:累计  2:单次计算	3:直接等于
	int parameter2;							// 目标值
	int parameter3;							// 条件判断
	int parameter4;							// 条件判断
	int reward_id;							// 奖励ID
};

struct MysteriousTrialRewardCfg
{
	MysteriousTrialRewardCfg() : create_timestamp(0)
	{}

	unsigned int create_timestamp;
	std::vector<ItemConfigData> reward_list;
};

struct MysteriousTrialConfig
{
	MysteriousTrialConfig() {}

	MysteriousTrialCfg node_cfg[RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];
};

struct MysteriousTrialFinallyCfg
{
	MysteriousTrialFinallyCfg() :seq(0), request_score(0)
	{}

	int seq;
	int request_score;				// 目标值
	ItemConfigData item;			// 奖励道具
};

struct MysteriousTraialBuyCfg			//神秘试炼每日折扣
{
	MysteriousTraialBuyCfg() : start_time(0), price(0)
	{}

	int start_time;			// 开始时间(哪一天)
	int price;				// 价格
	ItemConfigData item;
};

struct RALoginSystemRewardCfg
{
	RALoginSystemRewardCfg(): seq(0), login_days(0)
	{}

	int seq;
	int login_days;
	std::vector<ItemConfigData> reward_list;
};

struct RALoginSystemRewardGroupCfg
{
	RALoginSystemRewardGroupCfg() : open_server_timestamp(0)
	{}

	unsigned int open_server_timestamp;
	std::map<int, RALoginSystemRewardCfg> login_reward;
};


struct InvalidTimeActivityCfg					//过期物品信息
{
	InvalidTimeActivityCfg() :seq(-1), item_id(0), activity_id(0) {};

	int seq;
	ItemID item_id;
	int activity_id;

	static int count;		//记录过期物品数量
};

struct InvalidTimeItemCfg
{
	InvalidTimeItemCfg() :activity_id(), local_activity(false) {};

	int activity_id;
	bool local_activity;
};

class RandActivityConfigManager : public ILogicConfig
{
public:
	RandActivityConfigManager();
	virtual ~RandActivityConfigManager();

	virtual bool Init(const std::string &configname, std::string *err);
	bool InitSeoarateCfg(const std::string &configname, std::string *err);

	const RandActivityConfig * GetRandActivityConfig(int rand_activity_type);

	const RandActivityOtherCfg & GetRandActivityOtherCfg()const { return m_rand_activity_other_cfg; }

	int GetActOpenDay(RandActivityManager* ramgr, int activity_type) const;				//当前到活动开始时间天数
	int GetActOpenDayByTime(RandActivityManager * ramgr, int activity_type,time_t time) const;	//参数time到活动开始时间天数
	int GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const;				//当前到开服时间的天数

	// [不确定|无法划分区域]--------------------------------
	//---------------------------------与活动关联,在玩法中给玩家活动道具----------------------
	bool GetPatrolMiningCfg(RandActivityManager* ramgr, std::vector<ItemConfigData>* list);
	bool GetFieldBossCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list);
	bool GetCrystalBossCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list);
	//---------------------------------与活动关联,在玩法中给玩家活动道具(多组)----------------------
	bool GetPatrolMiningMoreCfg(RandActivityManager* ramgr, std::vector<ItemConfigData>* list);
	bool GetFieldCrystalFBMoreCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list);
	const RAMorePrizeCfg* GetMorePrizeCfg(int prize_id);
	///////////////////////////////////////  月度投资  ///////////////////////////////////
	const MonthInvestmentCfg * GetMonthInvestment(int seq)const;
	const std::vector<MonthPurchaseRewardCfg> * GetMonthPurchaseReward()const { return &m_month_purchase_reward; }
	///////////////////////////////////////  评分活动  ///////////////////////////////////////
	const ScoreReward* GetScoreReward(int seq);
	////////////////////////////////////  神秘试炼(七天活动(模块功能))  /////////////////////////////////
	const MysteriousTrialCfg* GetMysteriousTrialCfg(int type, int day, int seq)const;
	const std::map<int, MysteriousTrialConfig >* GetMysteriousTrialCfg(int type)const;
	const MysteriousTrialRewardCfg * GetMysteriousTrialRewardCfg(int reward_id, unsigned int create_timestamp) const;
	const MysteriousTrialFinallyCfg * GetMysteriousTrialFinallyReward(int type, int seq)const;
	const MysteriousTraialBuyCfg * GetMysteriousTrialBuyCfg(int type, int day)const;
	////////////////////////////////////////// 累计登陆  /////////////////////////////////////
	const RALoginSystemRewardCfg* GetLoginSystemRewardBySeq(int login_day, const UniqueServerID& usid)const;
	////////////////////////////////////////// 过期物品  /////////////////////////////////////
	const InvalidTimeActivityCfg* GetInvalidTimeActivity(int _seq);
	bool IsItemInvalid(ItemID _item_id);			// 输入的id是否在过期结构中
	int GetActivityId(ItemID _item_id);				// 这个函数不要用在背包的update函数里，时间复杂度太高
	const InvalidTimeItemCfg * GetGroupListActivityId(ItemID _item_id);
private:
	int InitRandActivityOtherCfg(TiXmlElement *RootElement);
	int InitPatrolMiningCfg(TiXmlElement *RootElement);
	int InitFieldBossCfg(TiXmlElement *RootElement);
	int InitCrystalBossCfg(TiXmlElement * RootElement);
	int InitPatrolMiningMoreCfg(TiXmlElement *RootElement);
	int InitFieldCrystalFBCfg(TiXmlElement * RootElement);
	int InitMorePrizeCfg(TiXmlElement * RootElement);
	int InitScoreRewardCfg(TiXmlElement * RootElement);
	int InitMonthInvestment(TiXmlElement * RootElement);
	int InitMonthPurchaseReward(TiXmlElement * RootElement);
	int InitMysteriousTrialRewardfg(TiXmlElement *RootElement);
	int InitMysteriousTrialCfg(TiXmlElement *RootElement);
	int InitMysteriousTrialFinallyCfg(TiXmlElement *RootElement);
	int InitMysteriousTrialBuyCfg(TiXmlElement *RootElement);
	int InitLoginSystemRewardCfg(TiXmlElement * RootElement);
	int InitInvalidTimeActivity(TiXmlElement * RootElement);

	void Release();

	inline bool CheckRandActivity(int activity_type)
	{
		return activity_type - RAND_ACTIVITY_TYPE_BEGIN >= MAX_RAND_ACTIVITY_CFG_COUNT || activity_type - RAND_ACTIVITY_TYPE_BEGIN < 0;
	}

	RandActivityOtherCfg m_rand_activity_other_cfg;

	RandActivityConfig * m_rand_activity_cfg_arr[MAX_RAND_ACTIVITY_CFG_COUNT];

	// 下面是通用 或者 没有活动号的

	std::vector<RAPatrolMiningCfg> m_patrol_mining_cfg;									// 与活动相关联,给活动道具(巡逻与采集)
	std::map<int, std::vector<RAFieldBossCfg> > m_field_boss_map;						// 以boss_id为key 与活动相关联,给活动道具(野外BOSS)
	std::map<int, std::vector<RACrystalBossCfg> > m_crystal_boss_map;					// 以boss_id为key 与活动相关联,给活动道具(水晶副本boss)

	std::vector<RAPatrolMiningMoreCfg> m_patrol_mining_more_cfg;						// 与活动相关联,给活动道具(多组巡逻与采集)
	std::map<int, std::vector<RAFieldCrystalFBMoreCfg> > m_field_crystal_fb_map;		// 以boss_id为key 与活动相关联,给活动道具(多组野外水晶副本)
	std::map<int, std::vector<RAMorePrizeCfg> > m_more_prize_map;						// 以prize_id(奖池ID)为key 与活动相关联,给活动道具

	std::vector<ScoreReward> m_score_reward_vec;										// 评分活动

	MonthInvestmentCfg m_month_investment_config[RA_MONTH_INVESTMENT_MAX_NUM];			// 月度投资天数奖励
	std::vector<MonthPurchaseRewardCfg> m_month_purchase_reward;						// 月度投资购买奖励

	std::map<int, std::map<int, MysteriousTrialConfig> > m_mysterious_cfg;								// 神秘试炼普通奖励 key1: type vec:以day(start_time)为键		
	std::map<int, std::map<int, MysteriousTrialFinallyCfg> > m_mysterious_trial_finally_cfg;			// 神秘试炼最终奖励 key1: type key2:以seq为键
	std::map<int, std::map<int, MysteriousTraialBuyCfg> > m_mysterious_trial_buy_cfg;					// 神秘试炼每日折扣 key1:type key2:start_time --7
	std::map<int, std::vector<MysteriousTrialRewardCfg> > m_mysterious_trial_reward_cfg;				// 神秘试炼奖励配置 key:reward_id

	std::vector<RALoginSystemRewardGroupCfg> m_login_system_vec;

	std::map<int, InvalidTimeActivityCfg> m_seq_to_id_info;									// 索引       -->   过期物品信息
	std::map<ItemID, bool> m_id_to_exsit;													// item_id    -->   是否过期
	std::map<ItemID, int> m_itemid_to_actiid;												// item_id    -->   活动物品id
	InvalidTimeItemCfg item_activity[65535];																// item_id	  -->   活动物品id
};

#endif	//__RAND_ACTIVITY_CONFIG_MANAGER_HPP__


