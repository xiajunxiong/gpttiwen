#ifndef __HONGMENG_TIANJIE_CONFIG_HPP__
#define __HONGMENG_TIANJIE_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/hongmengtianjiedef.hpp"

#include <map>

struct HongMengTianJieOtherCfg
{
	HongMengTianJieOtherCfg() : start_level(0), level_limit(0), help_times(0), boss_reward_limit(0), help_reward_limit(0)
	{
		memset(reward_times, 0, sizeof(reward_times));
	}

	int start_level;			// 开启世界等级
	int level_limit;

	std::vector<int> boss_reward_repeat;			// 首领奖励重置时间
	std::vector<int> help_reward_repeat;			// 助战奖励重置时间
	int reward_times[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];
	int help_times;				// 助战次数

	int boss_reward_limit;		// 首领次数上限
	int help_reward_limit;		// 助战次数上限
};

struct HongMengTianJieFbCfg
{
	HongMengTianJieFbCfg():fb_id(0), equipment_requirement(0), scene_id(0), door_pos(0, 0), first_difficult(0), down_percent(0), min_difficult(0) {}

	int fb_id;
	int equipment_requirement;			// 装等需求
	int scene_id;
	Posi door_pos;

	int first_difficult;				// 初始难度
	int down_percent;					// 下调比例
	int min_difficult;					// 最低难度
};

struct HongMengTianJieNpcCfg
{
	struct NpcData
	{
		int npc_id;
		int monster_group;
		Posi pos;
	};

	HongMengTianJieNpcCfg()
	{
		memset(npc_param, 0, sizeof(npc_param));
		memset(&leave_pos, 0, sizeof(leave_pos));
	}
	
	NpcData npc_param[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];

	Posi leave_pos;
};

struct HongMengTianJieRewardCfg
{
	std::vector<ItemConfigData> person_reward;			// 个人首通奖励
	std::vector<ItemConfigData> world_reward;			// 全服首通奖励
	std::vector<ItemConfigData> help_reward;			// 助战奖励

	std::vector<int> success_reward[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];
};

struct HongMengTianJieItemCfg
{
	ItemConfigData item;
	bool is_broadcast;
};

struct HongMengTianJieGroupCfg
{
	HongMengTianJieGroupCfg():max_rate(0) {}

	int max_rate;
	std::map<int, HongMengTianJieItemCfg> reward_list;
};

struct HongMengTianJieStoreCfg
{
	HongMengTianJieStoreCfg() : item_id(0), is_bind(true), num(0), price(0), conver_item_id(0), limit_seq(0), limit_convert_count(0) {}

	ItemID item_id;
	bool is_bind;
	int num;
	int price;
	ItemID conver_item_id;
	int limit_seq;
	int limit_convert_count;
};

class HongMengTianJieConfig : public ILogicConfig
{
public:
	HongMengTianJieConfig();
	virtual ~HongMengTianJieConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HongMengTianJieOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const HongMengTianJieFbCfg * GetFBCfg(int fb_type, int fb_seq);
	const HongMengTianJieNpcCfg * GetNpcCfg(int fb_id);
	const HongMengTianJieRewardCfg * GetRewardCfg(int fb_id);
	const HongMengTianJieItemCfg * GetGroupReward(int group_id);
	const HongMengTianJieStoreCfg * GetStoreCfg(int seq);
private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitFbCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitNpcCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRewardCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitGroupCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitStoreCfg(TiXmlElement * RootElement, std::string& errormsg);


	HongMengTianJieOtherCfg m_other_cfg;
	std::map<int, std::vector<HongMengTianJieFbCfg> > m_fb_cfg;
	std::vector<HongMengTianJieNpcCfg> m_npc_cfg;
	std::vector<HongMengTianJieRewardCfg> m_reward_cfg;
	std::map<int, HongMengTianJieGroupCfg> m_group_cfg;
	std::vector<HongMengTianJieStoreCfg> m_store_cfg;
};

#endif