#ifndef __RAND_ACTIVITY_JIN_YI_TIAN_XIANG_CONFIG_HPP__
#define __RAND_ACTIVITY_JIN_YI_TIAN_XIANG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAJinYiTianXiangItemCfg
{
	RAJinYiTianXiangItemCfg()
	{
		rate = 0;
		seq = 0;
		is_broadcast = 0;
	}

	int rate;
	int seq;
	int is_broadcast;
	std::vector<ItemConfigData> rewards;
};

struct RAJinYiTianXiangSectionCfg
{
	RAJinYiTianXiangSectionCfg() :section_start(0), section_end(0)
	{
		rate_count = 0;
	}

	int section_start;
	int section_end;

	int rate_count;
	std::vector<RAJinYiTianXiangItemCfg> item_list;
};

struct RAJinYiTianXiangBuyCfg
{
	RAJinYiTianXiangBuyCfg()
	{
		buy_seq = 0;
		need_chong_zhi_gold = 0;
		money_type = 0;
		money_value = 0;
		is_need_chong_zhi = 0;
		add_draw_value = 0;
	}

	int buy_seq;
	int need_chong_zhi_gold;
	int money_type;
	int money_value;
	int is_need_chong_zhi;
	int add_draw_value;
};

struct RAJinYiTianXiangConsumeCfg
{
	RAJinYiTianXiangConsumeCfg()
	{
		draw_times = 0;
		draw_consume_value = 0;
		buy_seq = 0;
	}

	int draw_times;
	int draw_consume_value;
	int buy_seq;
};

struct RAJinYiTianXiangDrawSectionCfgList
{
	std::vector<RAJinYiTianXiangSectionCfg> sections_cfg;
};

struct RAJinYiTianXiangOtherCfg
{
	int cfg_ver;
	int mark;
};

class RandActivityManager;
class RandActivityJinYiTianXiangConfig : public RandActivityConfig
{
public:
	RandActivityJinYiTianXiangConfig();
	virtual ~RandActivityJinYiTianXiangConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAJinYiTianXiangOtherCfg & GetOtherCfg() const;
	const RAJinYiTianXiangItemCfg * GetRandRewardCfg(RandActivityManager * ramgr, unsigned int exclude_flag) const;
	const RAJinYiTianXiangItemCfg * GetRandRewardCfgByMarkAndTime(RandActivityManager * ramgr, int mark, time_t activity_open_time, unsigned int exclude_flag) const;
	const RAJinYiTianXiangBuyCfg * GetRAJinYiTianXiangBuyCfg(int buy_seq) const;
	const RAJinYiTianXiangConsumeCfg * GetDrawConsumeCfg(int draw_times) const;

	virtual int GetConfigVer() const;
private:
	int InitDrawBuyCfg(TiXmlElement * RootElement);
	int InitDrawConsumeCfg(TiXmlElement * RootElement);
	int InitDrawCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, RAJinYiTianXiangDrawSectionCfgList > m_section_list_cfg;
	std::map<int, RAJinYiTianXiangBuyCfg> m_buy_cfg;
	std::map<int, RAJinYiTianXiangConsumeCfg> m_consume_cfg;
	RAJinYiTianXiangOtherCfg m_other_cfg;
};

#endif	