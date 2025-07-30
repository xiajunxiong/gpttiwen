#ifndef _FA_BAO_CONFIG_HPP_
#define _FA_BAO_CONFIG_HPP_

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/fabaodef.hpp"
#include <map>
#include <vector>

struct FaBaoOtherCfg
{
	FaBaoOtherCfg() :coin_exp(0), xianqi_item(0), item_exp(0), guide_monster_group(0)
	{
		speed_up_time = 0;
		speed_up_item = 0;
	}
	int coin_exp;
	int xianqi_item;
	int item_exp;
	int guide_monster_group;
	int speed_up_time;
	int speed_up_item;
	std::map<int, int> fa_bao_draw_active_index_to_seq;		//法宝抽奖炉子（炉子index）解锁 需要s 轮回忘川关数
};

struct FaBaoLevelItemCfg
{
	FaBaoLevelItemCfg() :level(0), need_advanced_level(0)
	{
		consume_item = 0;
		consume_num = 0;
		consume_coin = 0;
	}

	struct FaBaoAttrItem
	{
		int attr_type;
		int attr_value;
	};

	int level;
	int need_advanced_level;

	int consume_item;
	int consume_num;
	int consume_coin;
	std::vector<FaBaoAttrItem> attr_cfg;
};

struct FaBaoLevelCfg
{
	FaBaoLevelCfg() :fabao_id(0)
	{
	}

	int fabao_id;
	std::map<int, FaBaoLevelItemCfg> level_item;
};


struct FaBaoJinJieItemCfg
{
	FaBaoJinJieItemCfg()
	{
		jin_jie_level = 0;
		is_percentage = false;
		advanced_addition = 0;
		advanced_addition_2 = 0;
		consume_coin = 0;
	}

	struct FaBaoJinJieConsumeItem
	{
		int consume_item_id;
		int consume_item_num;
	};

	int jin_jie_level;
	bool is_percentage;
	int advanced_addition;
	int advanced_addition_2;
	int consume_coin;
	std::vector<FaBaoJinJieConsumeItem> consume_item;
};


struct FaBaoJinJieCfg
{
	FaBaoJinJieCfg() :fabao_id(0)
	{
	}

	struct FaBaoAttrItem
	{
		int attr_type;
		int attr_value;
	};

	int fabao_id;
	std::map<int, FaBaoJinJieItemCfg> level_item;
};

struct FaBaoInfoCfg
{
	FaBaoInfoCfg() :fabao_id(FABAO_INDEX_NO), fabao_type(FABAO_TYPE_NO), effect(-1),
		target(0), max_jinjie_level(0), max_advanced_addition(0), max_advanced_addition_2(0),
		active_item_id(0), active_item_num(0), is_open(false), color(0) {}

	int fabao_id;
	int fabao_type;
	int effect;
	int target;
	int max_jinjie_level;
	int max_advanced_addition;
	int max_advanced_addition_2;
	int active_item_id;
	int active_item_num;
	bool is_open;
	int color;
};

struct FaBaoDrawCfg
{
	FaBaoDrawCfg()
	{
		item_id = 0;
		consume_time = 0;
		rate = 0;
		transform_price_num = 0;
	}

	int item_id;
	int consume_time;
	int rate;
	int transform_price_num;		//
};

struct FaBaoDrawRewardItemCfg
{
	FaBaoDrawRewardItemCfg()
	{
		rate = 0;
		reward_type = 0;
		reward_id = 0;
		reward_num = 0;
	}

	int rate;
	int reward_type;
	int reward_id;
	int reward_num;
};

enum FA_BAO_DRAW_REWARD_TYPE
{
	FA_BAO_DRAW_REWARD_TYPE_FA_BAO = 1,
	FA_BAO_DRAW_REWARD_TYPE_ITEM = 2,
};

struct FaBaoDrawRewardCfg
{
	FaBaoDrawRewardCfg()
	{
	}

	std::map<int, int> rand_count;
	std::map<int, std::vector<FaBaoDrawRewardItemCfg> > rand_item;
};

struct FaBaoJiBanCfg
{
	FaBaoJiBanCfg() {}
	int	fetter_seq;
	int	id;
};

struct FaBaoJiBanGroupCfg
{
	FaBaoJiBanGroupCfg() : group(0) {}
	int group;
	std::vector<FaBaoJiBanCfg> m_jiban_vec;
};

const static int MAX_MAIL_ID_BUFF_LIST = 30;
struct FaBaoSecondReturnBackCfg
{
	FaBaoSecondReturnBackCfg() :uid(0), item_id(0), item_num(0) 
	{
		memset(mail_id, 0, sizeof(mail_id));
	}	
	int uid;
	int item_id;
	int item_num;
	char mail_id[MAX_MAIL_ID_BUFF_LIST];
};

struct FaBaoSecondReturnBackListCfg
{
	FaBaoSecondReturnBackListCfg() {}
	std::vector<FaBaoSecondReturnBackCfg> m_user_back_vec;
};

struct FaBaoSecondBackCfg
{
	FaBaoSecondBackCfg() :item_id(0), present_price(0), original_price(0) {}
	int item_id;
	int present_price;
	int original_price;
};

class FaBaoConfig : public ILogicConfig
{
public:
	FaBaoConfig();
	virtual ~FaBaoConfig();

	virtual bool Init(const std::string & configname, std::string * err);
	const FaBaoOtherCfg & GetFaBaoOtehrCfg() { return m_other_cfg; }
	const FaBaoInfoCfg * GetFaBaoInfoCfg(int fabao_index);

	const FaBaoLevelItemCfg * GetFaBaoLevelItemCfg(int fabao_id, int fabao_level);
	const FaBaoJinJieItemCfg * GetFaBaoJinJieItemCfg(int fabao_id, int jin_jie_level);
	const FaBaoLevelItemCfg * GetOldFaBaoLevelItemCfg(int fabao_id, int fabao_level);
	const FaBaoJinJieItemCfg * GetOldFaBaoJinJieItemCfg(int fabao_id, int jin_jie_level);

	int GetFaBaoMaxJiBanGroup() { return (int)m_fabao_jiban_cfg.size(); }
	const FaBaoJiBanGroupCfg * GetFaBaoJiBanByGroupCfg(int group_id);
	const FaBaoJiBanGroupCfg * GetFaBaoJiBanByIdCfg(int fabao_id);

	const FaBaoDrawCfg * GetFaBaoDrawCfg(int item_id);
	const FaBaoDrawRewardItemCfg * GetFaBaoDrawRewardCfg(int item_id, int reward_type);

	const FaBaoSecondReturnBackListCfg * GetUserFaBaoSecondRetrunCfg(int uid);
	const FaBaoSecondBackCfg * GetFaBaoItemIdBackCfg(int item_id);

private:
	int InitFaBaoOtherCfg(TiXmlElement * RootElement);
	int InitFaBaoInfoCfg(TiXmlElement * RootElement);
	int InitFaBaoLevelInfoCfg(TiXmlElement * RootElement);
	int InitFaBaoJinJieCfg(TiXmlElement * RootElement);
	int InitNewFaBaoLevelInfoCfg(TiXmlElement * RootElement);
	int InitNewFaBaoJinJieCfg(TiXmlElement * RootElement);
	int InitFaBaoDrawCfg(TiXmlElement * RootElement);
	int InitFaBaoDrawRewardCfg(TiXmlElement * RootElement);
	int InitFaBaoJiBanCfg(TiXmlElement * RootElement);
	int InitFaBaoSecondReturnCfg(TiXmlElement * RootElement);
	int InitFaBaoSecondBackItemCfg(TiXmlElement * RootElement);

	FaBaoOtherCfg m_other_cfg;
	std::map<int, FaBaoInfoCfg> m_fabao_cfg;

	std::map<int, FaBaoLevelCfg> m_level_cfg;
	std::map<int, FaBaoJinJieCfg> m_jin_jie_cfg;
	std::map<int, FaBaoLevelCfg> m_new_level_cfg;
	std::map<int, FaBaoJinJieCfg> m_new_jin_jie_cfg;
	std::map<int, FaBaoJiBanGroupCfg> m_fabao_jiban_cfg;
	FaBaoJiBanGroupCfg * fabao_jiban_by_fabao_id[MAX_FABAO_COUNT];
	std::map<int, FaBaoDrawCfg> m_draw_cfg;
	std::map<int, FaBaoDrawRewardCfg> m_draw_reward_cfg;

	std::map<int, FaBaoSecondReturnBackListCfg> m_back_user_vec;
	FaBaoSecondBackCfg * back_item_list[MAX_UINT16];
	std::vector<FaBaoSecondBackCfg> m_back_item_vec;
};



#endif // _WUSHUANGDIANCONFIG_HPP_