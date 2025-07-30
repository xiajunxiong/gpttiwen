#ifndef __TREVIFOUNTION_CONFIG_HPP__
#define __TREVIFOUNTION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "other/attributelist.hpp"
#include "servercommon/attributedef.hpp"
#include <vector>
#include <map>
#include "servercommon/struct/itemlistparam.h"

struct WishRewardCfg
{
	WishRewardCfg()
	{
		level = 0;
		exp = 0;
		coin = 0;
		buff_group = 0;
	}
	int level;
	int exp;
	int coin;
	int buff_group;
	ItemConfigData reward;
};

static const int WISH_BUFF_PARAM_NUM = 4;
struct WishBuffCfg
{
	WishBuffCfg() 
	{
		buff_id = 0;
		buff_type = 0;
		buff_time = 0;
		buff_rate = 0;
		memset(param_list, -1, sizeof(param_list));
	}
	int buff_id;
	int buff_type;
	int buff_time;
	int buff_rate;
	int param_list[WISH_BUFF_PARAM_NUM];
};

struct WishOtherCfg
{
	WishOtherCfg()
	{}
	int wish_num;
};

struct TreviFountionFindCfg
{
	TreviFountionFindCfg()
	{
		min_level=0;
		max_level=0;
		exp=0;
		coin=0;
	}
	int min_level;
	int max_level;
	int exp;
	int coin;
};

class TreviFountionconfig : public ILogicConfig
{
public:
	TreviFountionconfig();
	virtual ~TreviFountionconfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const WishBuffCfg* RandBuff(int buff_group);
	const WishBuffCfg* GetBuffCfg(int buff_id);
	const WishRewardCfg* GetRewardCfg(int level);
	const WishOtherCfg* GetOtherCfg() { return &m_other_cfg; }
	bool GetFindReward(int level, int type, int *coin, int *exp);

private:
	int InitWishRewardCfg(TiXmlElement *RootElement); 
	int InitBuffCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitFindRewardCfg(TiXmlElement *RootElement);

	std::map<int, WishRewardCfg> m_reward_map;
	std::map<int, WishBuffCfg> m_buff_map;

	std::map<int,int> m_total_rate;
	std::map<int, std::map<int, WishBuffCfg> > m_buff_group_map;

	WishOtherCfg m_other_cfg;

	std::map<int, std::map<int, TreviFountionFindCfg> > m_find_reward_map;

};

#endif