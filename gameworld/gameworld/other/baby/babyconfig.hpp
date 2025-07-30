#ifndef _BABY_CONFIG_HPP_
#define _BABY_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"

#include "servercommon/babydef.hpp"

#include <vector>

struct BabyOtherCfg
{
	BabyOtherCfg()
	{
		create_need_role_level = 0;
		uplvl_item_id = 0;
		add_exp = 0;
		rename_item_id = 0;

		contribute_coin_num = 0;
		contribute_coin_jifen = 0;

		contribute_xianyu_num = 0;
		contribute_xianyu_jifen = 0;

		contribute_gold_num = 0;
		contribute_gold_jifen = 0;

		lottery_times = 0;
		worship_times = 0;
		duet_play_times = 0;

		single_need_ji_fen = 0;	//单人求子 需要积分
		memset(stage_condition, 0, sizeof(stage_condition));
	}
		
	int create_need_role_level;
	int uplvl_item_id;
	int add_exp;
	int rename_item_id;

	int contribute_coin_num;
	int contribute_coin_jifen;

	int contribute_xianyu_num;
	int contribute_xianyu_jifen;

	int contribute_gold_num;
	int contribute_gold_jifen;

	int lottery_times;	
	int worship_times;  
	int duet_play_times;	

	int single_need_ji_fen;	//单人求子 需要积分
	int stage_condition[CREATE_BABY_STAGE_MAX];
};

struct BabyLevelCfg
{
	BabyLevelCfg()
	{

	}
	int level;
	int exp;

	int maxhp;
	int maxmp;
	int gongji;
	int fangyu;
	int minjie;
	int jingshen;
	int huifu;
};

struct BabyDrawCfg
{
	BabyDrawCfg()
	{
		rate_count = 0;
	}

	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			add_ji_fen = 0;
		}
		int rate;
		int add_ji_fen;
	};

	int rate_count;
	std::vector<ItemCfg> item_cfg;
};


class BabyConfig : public ILogicConfig
{
public:
	BabyConfig();
	~BabyConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const BabyOtherCfg & GetOtherCfg();
	const BabyLevelCfg * GetBabyLevelCfg(int level);
	int GetBabyMaxLevel();
	BabyDrawCfg::ItemCfg * GetRandDrawItem();
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBabyLevelCfg(TiXmlElement *RootElement);
	int InitDrawCfg(TiXmlElement *RootElement);

	BabyOtherCfg m_other_cfg;
	std::vector<BabyLevelCfg> m_baby_lvl_cfg;
	BabyDrawCfg m_draw_cfg;
};


#endif