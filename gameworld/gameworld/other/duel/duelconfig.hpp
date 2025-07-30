#ifndef __DUEL_CONFIG_HPP__
#define __DUEL_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/dueldef.h"

struct DuelMapCfg
{
	DuelMapCfg()
	{
		revive_map = 0;
	}

	int revive_map;
	Posi pos;
};

struct DuelKillCfg
{
	DuelKillCfg()
	{
		kill_level = 0;
		min_value = 0;
		max_value = 0;
		title_id = 0;
		attr_down = 0;
	}

	int kill_level;
	int min_value;
	int max_value;
	int title_id;
	int attr_down;				// 全属性降低（百分比）
};

struct DuelOtherCfg
{
	DuelOtherCfg()
	{
		kill_value_base = 0;
		kill_value_multiple = 0;
		reduce_kill_value_time = 0;
		reduce_exp = 0;
		reduce_coin = 0;
		item_reduce_kill_value = 0;
		item_reduce_daily_times = 0;
		loss_reduce_kill_value = 0;
	}

	int kill_value_base;						// 决斗胜利增加杀戮值基础值 （基础数值+倍数*等级差）
	int kill_value_multiple;					// 决斗胜利增加杀戮值倍数（基础数值+倍数*等级差）
	int reduce_kill_value_time;					// 每x分钟自动减少一点杀戮值
	int reduce_exp;								// 决斗失败损失经验值（百分比）
	int reduce_coin;							// 决斗失败损失铜币
	int item_reduce_kill_value;					// 道具减少的杀戮值
	int item_reduce_daily_times;				// 每日可使用道具次数
	int loss_reduce_kill_value;
	int duel_item_id;							// 决斗令
};

// -----------------------------------------------------------------------------------
class DuelConfig : public ILogicConfig
{
public:
	DuelConfig();
	virtual ~DuelConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const DuelMapCfg* GetMapCfg(int scene_id)const;
	const DuelKillCfg* GetKillCfg(int kill_vaule)const;
	const DuelOtherCfg& GetOtherCfg()const { return m_other_cfg; }

	const bool SceneCanDuel(int scene_id)const;

private:
	int InitMapCfg(TiXmlElement * RootElement);
	int InitKillCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, DuelMapCfg> m_map_cfg;
	std::vector<DuelKillCfg> m_kill_cfg;
	DuelOtherCfg m_other_cfg;

};

#endif
