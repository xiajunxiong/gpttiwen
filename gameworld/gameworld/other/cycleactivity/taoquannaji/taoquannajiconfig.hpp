#ifndef __TAO_QUAN_NA_JI_CONFIG_HPP__
#define __TAO_QUAN_NA_JI_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/taoquannajidef.hpp"
#include <map>
#include <vector>

struct NaJiItem
{
	NaJiItem() :item_id(0), item_type(0), score(0), sp_type(0) {}
	enum ITEM_EFFECT_TYPE
	{
		ITEM_EFFECT_TYPE_0 = 0,//无效果
		ITEM_EFFECT_TYPE_1 = 1,//消除同类型物品
		ITEM_EFFECT_TYPE_2 = 2,//消除以摆件为中心的九宫格内所有物品
	};
	int item_id;
	int item_type;
	int score;
	int sp_type;
};

struct NaJiMapNode
{
	NaJiMapNode() : item_id(0), item_num(0) {}
	
	int item_id;
	int item_num;
};

struct NaJiMapList
{
	NaJiMapList() : group(0), map_point_num(0) {}

	int group;
	int map_point_num;
	std::vector<NaJiMapNode> m_map_list;
};

struct NaJiOther
{
	NaJiOther() : circle_num(0), scene_id(0), reward_time(0), title_item_id(0), title_score(0) {}
	int circle_num;
	int	scene_id;
	int reward_time;
	int title_item_id;
	int	title_score;
};

struct NaJiReward
{
	NaJiReward() : score(0) {}
	int score;
	std::vector<ItemConfigData> m_reward_vec;
};

class TaoQuanNaJiConfig : public ILogicConfig
{
public:
	TaoQuanNaJiConfig();
	virtual ~TaoQuanNaJiConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const NaJiItem * GetItemCfg(int item_id);
	const NaJiReward * GetRewardCfg(int score);
	const NaJiMapList * RandMapCfg();
	const NaJiOther & GetOtherCfg() { return m_other_cfg; }
private:
	int InitItemList(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitMapListCfg(TiXmlElement *RootElement);

	std::vector<NaJiItem> m_item_list;
	NaJiOther m_other_cfg;
	std::vector<NaJiReward> m_reward_vec;
	std::map<int, NaJiMapList> m_map_group_vec;
};

#endif // !__COLLECTION_CONFIG_HPP__
