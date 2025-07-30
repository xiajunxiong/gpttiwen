#ifndef __RAND_ACTIVITY_CHUN_RI_DENG_FENG_CONFIG_HPP__
#define __RAND_ACTIVITY_CHUN_RI_DENG_FENG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAChunRiDengFengLayerCfg
{
	struct ItemCfg
	{
		ItemCfg() :rate(0), seq(0), is_into_next(0)
		{
		}

		int rate;
		int seq;
		int is_into_next;		
		ItemConfigData reward;
	};

	const RAChunRiDengFengLayerCfg::ItemCfg * GetRandItem(bool * is_succ = NULL, unsigned short get_flag = 0) const;

	RAChunRiDengFengLayerCfg()
	{
		layer = 0;
		last_layer_item_num = 0;
		rate_count = 0;
	}

	int layer;
	int last_layer_item_num;
	int rate_count;
	std::vector<ItemCfg> reward;
};

struct RAChunRiDengFengSectionCfg
{
	RAChunRiDengFengSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;
	std::map<int, RAChunRiDengFengLayerCfg> layer_cfg;
};

struct RAChunRiDengFengOtherCfg
{
	RAChunRiDengFengOtherCfg()
	{
		explore_comsume_item_id = 0;
		explore_comsume_num = 0;
		goto_last_layer_explore_times = 0;
		change_base_layer_explore_times = 0;
		base_layer = 0;
	}

	int explore_comsume_item_id;
	int explore_comsume_num;
	int goto_last_layer_explore_times;
	unsigned int change_base_layer_explore_times;
	int base_layer;
};


class RandActivityManager;
class RandActivityChunRiDengFengConfig : public RandActivityConfig
{
public:
	RandActivityChunRiDengFengConfig();
	virtual ~RandActivityChunRiDengFengConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);


	const RAChunRiDengFengOtherCfg & GetOtherCfg() const;
	const RAChunRiDengFengLayerCfg * GetLayerCfg(RandActivityManager * ramgr, int layer) const;
	bool CanGetLastLayerItem(RandActivityManager * ramgr, unsigned short get_flag) const;
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitLayerCfg(TiXmlElement * RootElement);


	std::vector<RAChunRiDengFengSectionCfg> m_section_cfg;
	RAChunRiDengFengOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__