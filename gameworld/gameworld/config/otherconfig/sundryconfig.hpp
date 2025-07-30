#ifndef __OTHER_CONFIG_HPP__
#define __OTHER_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"
#include <vector>
#include <map>
#include <set>

struct SkillGrassCfg
{
	SkillGrassCfg() : grass_type(0), grass_item_id(0), value_coin(0) {}

	short grass_type;
	ItemID grass_item_id;
	int value_coin;
};

struct ConstantCfg
{
	ConstantCfg() : max_npc_touch_distance(0), drop_card_max_num(0), name_change_item(0), name_change_num(0),
		move_speed(0), init_crystal(0), max_battle_add_intimacy(0), chivarous_salary_need_level(0),
		chivarous_salary_need_level_diff(0), partner_robot_skill_id(0), 
		partner_robot_skill_id_1(0), partner_robot_partner_id(0) , change_appearance_item(0){}

	int max_npc_touch_distance;
	int drop_card_max_num;
	ItemID name_change_item;
	short name_change_num;
	int move_speed;
	ItemID init_crystal;
	int max_battle_add_intimacy;
	int chivarous_salary_need_level;
	int chivarous_salary_need_level_diff;
	UInt16 partner_robot_skill_id;
	UInt16 partner_robot_skill_id_1;
	UInt16 partner_robot_partner_id;
	ItemID change_appearance_item;
};

struct GiveFlowerCfg
{
	GiveFlowerCfg()
	{
		seq = 0;
		consume_item_id = 0;
		add_value = 0;
	}

	int seq;
	int consume_item_id;
	int add_value;
};


enum SKILL_GRASS_TYPE
{
	SKILL_GRASS_TYPE_SMALL = 0,
	SKILL_GRASS_TYPE_MEDIUM,
	SKILL_GRASS_TYPE_LARGE,
	SKILL_GRASS_TYPE_NUM,
};

struct CaptainSalaryCfg
{
	CaptainSalaryCfg() : seq(0), week_limit_times(0), reward_coin_type(0), reward_coin(0), reward_type(0), param(0) {}

	int seq;				//下发给客户端的
	int week_limit_times;
	int reward_coin_type;
	int reward_coin;
	int reward_type;
	int param;

	//ps reward_type + param唯一确定一个cfg项
};

struct ChivalrousSalaryCfg
{
	ChivalrousSalaryCfg() : seq(0), week_limit_times(0), reward_chivarous(0), reward_type(0), param(0), param2(0) , version(0){}

	int seq;				//下发给客户端的
	int week_limit_times;
	int reward_chivarous;
	int reward_type;
	int param;
	int param2;
	int version;

	//ps reward_type + param唯一确定一个cfg项
};

struct  MonsterGroupLinkageTaskCfg
{
	MonsterGroupLinkageTaskCfg(): task_id(0), reward_type(0)
	{}

	int task_id;
	int reward_type;
};

struct SalaryCfgKey
{
	SalaryCfgKey() : type(0), param(0), param2(0) {}
	SalaryCfgKey(int _type, int _param, int _param2) : type(_type), param(_param), param2(_param2) {}

	bool operator < (const SalaryCfgKey& _key) const
	{
		if (type != _key.type) return type < _key.type;

		if (param != _key.param) return param < _key.param;

		return param2 < _key.param2;
	}

	int type;
	int param;
	int param2;
};

/*
struct EquipmentTreasureAffirm
{
	EquipmentTreasureAffirm() : need_attr_perc(0), equip_attr_type(0){}

	int need_attr_perc;		// 所需装备属性到值
	int equip_attr_type;	// 进行检测的装备属性类型
};*/

struct PatrolTaskCfg
{
	/*PatrolTaskCfg():kill_num(0),cost_type(0),reward(0){}
	int kill_num;
	int cost_type;
	int reward;*/

	PatrolTaskCfg() :level(0), kill_num(0), coin(0) {}
	int level;
	int kill_num;
	int coin;
};

struct SundryOtherCfg
{
	SundryOtherCfg()
	{
		recovery_need_gold = 0;
		treasure_map_notice_item = 0;
		patrol_open_level = 0;
		formation_start_num = 0;
		server_check = 0;
		download_reward_count = 0;
		pingjia_level = 0;
		is_add_day_chong = false;
	}

	int recovery_need_gold;
	int treasure_map_notice_item;
	int patrol_open_level;
	int formation_start_num;				// 初始陣容數量
	ItemConfigData create_role_reward;		// 新手礼包
	int server_check;						// 检测热更逻辑或战斗时间标识 0:不检测 1:检测
	int auto_level_difference;				// 开启自动等级差

	int download_reward_count;
	ItemConfigData download_reward[5];		// 资源下载奖励

	ItemConfigData pinjia_reward;			// 评价有礼奖励
	int pingjia_level;
	bool is_add_day_chong;					// 新型万能直购卡是否计入每日累充(其它全部不计入)
};

struct ChivalrousAchievementCfg
{
	static const int MAX_REWARD_NUM = 5;

	ChivalrousAchievementCfg() : seq(0), need_chivalrous_value(0), reward_num(0) {}

	int seq;
	int need_chivalrous_value;
	int reward_num;
	ItemConfigData item_list[MAX_REWARD_NUM];
};

struct ChivalrousTeamInfoCfg
{
	ChivalrousTeamInfoCfg() : monster_group_id(0), need_item_id(0)
	{}

	int monster_group_id;
	int need_item_id;		//当前秘宝阁历练(精英副本)需要
};

enum FAKE_ANNOUNCEMENT_TYPE
{
	FAKE_ANNOUNCEMENT_TYPE_ROLLING_NOTICE = 0,		//滚动公告(跑马灯)
	FAKE_ANNOUNCEMENT_TYPE_SYSTEM_CHAT = 1,			//系统聊天栏
	FAKE_ANNOUNCEMENT_TYPE_ALL = 2,					//跑马灯+系统栏

	FAKE_ANNOUNCEMENT_TYPE_MAX
};

struct FakeAnnouncementCfg
{
	FakeAnnouncementCfg() : min_level(0), max_level(0), announcement_type(0),
		random_times(0), is_random_name(false), item_id(0)
	{}

	int min_level;
	int max_level;
	int announcement_type;
	int random_times;
	std::string descripte_1;
	bool is_random_name;
	std::string descripte_2;
	int item_id;
	std::string descripte_3;
};

static const int MAX_EQUIP_LEVEL = ItemNamespace::EQUIPMENT_MAX_LEVEL + 1;

//杂项配置 用于那些零碎的小需求
class Role;
class SundryConfig : public ILogicConfig
{
public:
	enum REWARD_CON
	{
		REWARD_CON_BATTLE_WIN = 0,	// 战斗胜利
		REWARD_CON_FB_CLEAR = 1,	// 副本通关
	};

	typedef std::map<SalaryCfgKey, CaptainSalaryCfg> CaptainSalaryCfgMap;
	typedef std::map<SalaryCfgKey, ChivalrousSalaryCfg> ChivarousSalaryCfgMap;
	static const int CHIVALROUS_ACHIEVE_CFG_NUM = 10;
public:
	SundryConfig();
	virtual ~SundryConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ConstantCfg & GetConstantCfg() { return m_constant_cfg; }

	const GiveFlowerCfg * GetGiveFlowerCfg(int seq);

	const CaptainSalaryCfg* GetCaptainSalaryCfg(int reward_type, int param, int param2 = 0);
	const ChivalrousSalaryCfg* GetChivalrousSalaryCfg(int reward_type, int param, int param2 = 0);
	const ChivalrousSalaryCfg* GetChivalrousSalaryCfgByMonsterGroup(int monster_group_id);
	const MonsterGroupLinkageTaskCfg* GetMonsterGroupLinkageTaskCfg(int monster_group_id);

	const CaptainSalaryCfgMap& GetCaptainSalaryCfgMap() const { return m_captain_salary_cfg_map; }
	const ChivarousSalaryCfgMap& GetChivalrousSalaryCfgMap() const { return m_chivalrous_salary_cfg_map; }

	const PatrolTaskCfg * GetPatrolTaskCfg() { return &patrol_task_cfg; }

	ItemID GetElementItem(int element_type);

	std::map<int, int>& GetLevelToCoin() { return m_level_to_coin; }
	const int GetCoinByLevel(int _role_level);
	ItemID GetCreateRoleReward() { return m_other_cfg.create_role_reward.item_id; }

	const SundryOtherCfg & GetOtherCfg();
	const ChivalrousAchievementCfg* GetChivalrousAchieveCfg(int seq);
	const ChivalrousTeamInfoCfg *  GetChivalrousMonsterGroupByTeamType(int team_type);
	bool IsChivalrousTeamType(int team_type);
	const SalaryCfgKey * GetSalaryCfgKeyByMonsterGroupID(int monster_group_id);

	bool IsFangFraudSucc(int index);
	int GetFangFraudRewardCount() { return m_fang_fraud_reward_count; }
	const ItemConfigData * GetFangFraudReward() { return m_fang_fraud_reward; }

	void GetFakeAnnouncement(int & times_count, std::vector<FakeAnnouncementCfg> & out_list);

	bool IsCanUseBuyGiftByType(int buy_gift_type) const;
	int GetChongZhiNumByItemId(int item_id) const;
	int GetItemIdByChongZhiNum(int chong_zhi_num) const;

	const std::pair<double, int>* GetMaxGuaJiExpCfg(int level) const;
	const std::pair<int, int>* GetFatigueCfg(int fatigue_number)const;

private:
	const CaptainSalaryCfg* GetCaptainSalaryCfg(const SalaryCfgKey& key);
	const ChivalrousSalaryCfg* GetChivalrousSalaryCfg(const SalaryCfgKey& key);

	int InitConstantCfg(TiXmlElement * RootElement);
	int InitGiveFlowerCfg(TiXmlElement * RootElement);
	int InitCaptainSalaryCfg(TiXmlElement* RootElement);
	int InitChivalrousSalaryCfg(TiXmlElement* RootElement);
	int InitElementCfg(TiXmlElement* RootElement);
	int InitPatrolTaskCfg(TiXmlElement* RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitMonsterGroupLinkageTaskCfg(TiXmlElement * RootElement);
	int InitChivalrousMonsterCfg(TiXmlElement * RootElement);
	int InitChivalrousAchieveCfg(TiXmlElement* RootElement);
	int InitFangFraudCfg(TiXmlElement* RootElement);
	int InitFangFraudRewardCfg(TiXmlElement* RootElement);
	int InitFakeAnnouncementCfg(TiXmlElement* RootElement);
	int InitSupportUniversalCardCfg(TiXmlElement* RootElement, std::string& errormsg);
	int InitUniversalCardValueCfg(TiXmlElement* RootElement, std::string& errormsg);
	int InitOnHookExpCfg(TiXmlElement* RootElement, std::string& errormsg);
	int InitFatigueCfg(TiXmlElement* RootElement, std::string& errormsg);

	ConstantCfg m_constant_cfg;

	std::vector<GiveFlowerCfg> m_give_flower_cfg;

	CaptainSalaryCfgMap m_captain_salary_cfg_map;
	ChivarousSalaryCfgMap m_chivalrous_salary_cfg_map;
	std::map<int, MonsterGroupLinkageTaskCfg> m_monster_group_linage_task;			//以怪物组ID为key
	std::map<int, ChivalrousSalaryCfg> m_chivalrous_salary_by_monster_group_map;	//侠义值配置 key:怪物组ID
	std::map<int, ChivalrousTeamInfoCfg> m_teamtype_monstergroup_hash;
	std::map<int, SalaryCfgKey> m_monstergroup_key_hash;							//侠义值对应参数信息 key:怪物组ID

	ItemID m_element_item_list[ELEMENT_TYPE_MAX];

	PatrolTaskCfg patrol_task_cfg;

	SundryOtherCfg m_other_cfg;

	std::map<int, int> m_level_to_coin;

	int m_chivalrous_achieve_cfg_num;
	ChivalrousAchievementCfg m_chivalrous_achieve_cfg[CHIVALROUS_ACHIEVE_CFG_NUM];

	std::vector<bool> m_fang_fraud_cfg;
	int m_fang_fraud_reward_count;
	ItemConfigData m_fang_fraud_reward[8];

	int m_original_fake_announcement_times_count;	//假公告总次数(原服)
	int m_hidden_fake_announcement_times_count;		//假公告总次数(跨服)
	std::vector<FakeAnnouncementCfg> m_original_fake_announcement_list;	//假公告配置(原服)
	std::vector<FakeAnnouncementCfg> m_hidden_fake_announcement_list;	//假公告配置(跨服)

	std::set<int> m_buy_gift_cmd_type_set;			//支持新型万能直购卡类型列表 value:type \see BUY_GIFT_ITEM_TYPE_MAX
	std::map<int, int> m_buy_gift_item_map;			//万能卡价值配置 key:道具ID second:value_num(chongzhi_num)
	std::map<int, int> m_buy_gift_item_cfg;			//万能卡价值配置 key:value_num(chongzhi_num) second:道具ID

	std::map<int, std::pair<double, int> > m_max_guaji_exp_cfg;		// 挂机经验上限配置, key-等级, pair<经验, 疲劳值>
	std::vector<std::pair<int, int> > m_fatigue_cfg;				// 挂机经验衰减比率 pair<疲劳值, 比例>
};

#endif