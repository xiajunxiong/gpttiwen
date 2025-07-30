#ifndef __RAND_ACTIVITY_MI_BAO_FANG_DIAN_CONFIG_HPP__
#define __RAND_ACTIVITY_MI_BAO_FANG_DIAN_CONFIG_HPP__

#include <set>
#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"

struct RAMiBaoFangDianOtherCfg
{
	RAMiBaoFangDianOtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
};

struct RAMiBaoFangDianShopCfg
{
	RAMiBaoFangDianShopCfg() :index(0), seq(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0)
	{}

	enum PRICE_TYPE
	{
		PRICE_TYPE_MIN = 0,
		PRICE_TYPE_GOLD = PRICE_TYPE_MIN,	// Ôª±¦
		PRICE_TYPE_SILVER_COIN = 1,			// ÁéÓñ
		PRICE_TYPE_COIN = 2,				// Í­±Ò
		
		PRICE_TYPE_MAX,	
	};

	int index;
	int seq;
	int limit_type;
	int buy_times;
	int today_times;
	int price_type;
	int price;
	ItemConfigData reward_item;
};

struct RAMiBaoFangDianSectionShopCfg
{
	RAMiBaoFangDianSectionShopCfg() : start_time(0), end_time(0), phase(0)
	{}

	int start_time;
	int end_time;
	int phase;
	std::vector<RAMiBaoFangDianShopCfg> shop_list;
};

struct RAMiBaoFangDianSeverSectionShopCfg
{
	RAMiBaoFangDianSeverSectionShopCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAMiBaoFangDianSectionShopCfg> section_list;
};

class RandActivityManager;
class RandActivityMiBaoFangDianConfig : public RandActivityConfig
{
public:
	RandActivityMiBaoFangDianConfig();
	virtual ~RandActivityMiBaoFangDianConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RAMiBaoFangDianShopCfg * GetShopCfg(RandActivityManager * ramgr, int seq, int cur_use_open_day)const;
	void GetPhase(RandActivityManager * ramgr, int & use_open_day, int & cur_phase)const;


	virtual int GetConfigVer() const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);

	union KeyChangeUn
	{
		int section[2];
		long long key;
	};

	long long GetKeyBySection(int section_start, int section_end) const;
	void GetSectionByKey(long long key, int & section_start, int & section_end) const;

	RAMiBaoFangDianOtherCfg m_other_cfg;

	std::vector<RAMiBaoFangDianSeverSectionShopCfg> m_shop_section_cfg;	
	std::map<long long, std::set<long long> > m_shop_phase_map;				//¼ìÑé½×¶ÎÌìÊý
};

#endif