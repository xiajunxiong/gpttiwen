#ifndef __RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_CONFIG_HPP__
#define __RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAZhuanShuLiBaoItemCfg
{
	RAZhuanShuLiBaoItemCfg()
	{
		type = 0;
		need_chong_zhi_gold = 0;
		limit_buy_times = 0;

		role_level = 0;
		chongzhi_min = 0;
		chongzhi_max = 0;
		add_gold = 0;
	}

	int type;
	int need_chong_zhi_gold;
	int limit_buy_times;

	int role_level;
	int chongzhi_min;
	int chongzhi_max;
	int add_gold;
	std::vector<ItemConfigData> rewards;
};


struct RAZhuanShuLiBaoOtherCfg
{
	RAZhuanShuLiBaoOtherCfg()
	{
		duration_time = 0;
		cfg_ver = 0;
		is_weekend_gift_open = 0;
	}

	int duration_time;
	int cfg_ver;

	//(随机活动-周末礼包) -  是否开启
	//0 关闭
	//1 开启 且 (随机活动-周末礼包:开启中) 且 周末: 每日礼包将不会新增
	int is_weekend_gift_open;		
};

//专属礼包-每日礼包
struct RAZhuanShuLiBaoDayCfg
{
	RAZhuanShuLiBaoDayCfg() : phase(0), type(0), gold_num(0), limit_num(0), buy_money(0)
	{}

	int phase;
	int type;
	int gold_num;
	int limit_num;
	int buy_money;
	std::vector<ItemConfigData> item_list;
};

class RandActivityManager;
class RandActivityZhuanShuLiBaoConfig : public RandActivityConfig
{
public:
	RandActivityZhuanShuLiBaoConfig();
	virtual ~RandActivityZhuanShuLiBaoConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	std::vector<const RAZhuanShuLiBaoItemCfg *> GetRAZhuanShuLiBaoItemCfg(int min_role_level, int max_role_level, int chong_zhi_value) const;
	const RAZhuanShuLiBaoItemCfg * GetRAZhuanShuLiBaoItemCfg(int type) const;
	const RAZhuanShuLiBaoOtherCfg &  GetOtherCfg() const;

	bool IsExceedMaxDay(int phase, int login_day)const;
	const RAZhuanShuLiBaoDayCfg * GetDayGiftCfgByLogin(int phase, int login_day)const;
	const RAZhuanShuLiBaoDayCfg * GetDayGiftCfg(int phase, int type)const;
	int GetDayPhaseByLevel(int role_level)const;
	virtual int GetConfigVer() const;

private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitDayGiftCfg(TiXmlElement * RootElement);
	int InitDayPhaseCfg(TiXmlElement * RootElement);
	

	std::map<int, std::vector<RAZhuanShuLiBaoItemCfg> > m_item_cfg;
	RAZhuanShuLiBaoOtherCfg m_other_cfg;

	std::map<int, std::map<int, RAZhuanShuLiBaoDayCfg> > m_day_gift_cfg;	//每日礼包配置 key1:phase key2:type	
	std::map<int, int> m_day_gift_level_map;								//每日礼包等级段配置 key:min_level * 1000 + max_level second:phase
	std::set<int> m_day_phase_set;

};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__