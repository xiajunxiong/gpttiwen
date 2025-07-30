#ifndef __RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_CONFIG_HPP__
#define __RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityzhuiyueshangdianparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>


struct  RandActivityZhuiYueShangDianCfg
{
	int selec_reward_group_id;
	int special_reward_group_id;
	int draw_need_item_id;
	int draw_need_item_num;
};

struct  RandActivityZhuiYueShangDianSectionCfg
{
	RandActivityZhuiYueShangDianSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	RandActivityZhuiYueShangDianCfg  cfg;
};

struct  RandActivityZhuiYueShangDianRewardGroupItemCfg
{
	int reward_group_id;
	int reward_seq;
	int rate;
	int bao_di_id;
	int bao_di_times;
	bool is_broadcast;
	std::vector<ItemConfigData> rewards;
};


struct  RandActivityZhuiYueShangDianRewardGroupCfg
{
	int reward_group_id;
	std::map<int, RandActivityZhuiYueShangDianRewardGroupItemCfg> item_list;
};

struct  RandActivityZhuiYueShangDianBuyItemCfg
{
	RandActivityZhuiYueShangDianBuyItemCfg()
	{
		seq = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;

		is_need_chong_zhi = 0;
		need_chong_zhi_gold = 0;
		money_type = 0;
		money_value = 0;		
	}

	int seq;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;

	int is_need_chong_zhi;
	int need_chong_zhi_gold;
	int money_type;
	int money_value;

	std::vector<ItemConfigData> rewards;
};

struct  RandActivityZhuiYueShangDianBuyItemSectionCfg
{
	RandActivityZhuiYueShangDianBuyItemSectionCfg() :section_start(0), section_end(0)
	{

	}

	int section_start;
	int section_end;

	std::map<int, RandActivityZhuiYueShangDianBuyItemCfg> item_list;
};


struct  RandActivityZhuiYueShangDianOtherCfg
{
	int cfg_ver;	
};

class RandActivityManager;
class RandActivityZhuiYueShangDianConfig : public RandActivityConfig
{
public:
	RandActivityZhuiYueShangDianConfig();
	virtual ~RandActivityZhuiYueShangDianConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RandActivityZhuiYueShangDianOtherCfg & GetOtherCfg() const;
	const RandActivityZhuiYueShangDianCfg * GetRandActivityZhuiYueShangDianCfg(RandActivityManager * ramgr) const;	
	const RandActivityZhuiYueShangDianRewardGroupItemCfg * GetRandActivityZhuiYueShangDianRewardGroupItemCfg(RandActivityManager * ramgr, int reward_seq) const;	
	const RandActivityZhuiYueShangDianRewardGroupItemCfg * GetRandActivityZhuiYueShangDianSpecialRewardItemCfg(RandActivityManager * ramgr) const;	//ªÒ»°¥ÛΩ±≈‰÷√
	const RandActivityZhuiYueShangDianRewardGroupCfg * GetRandActivityZhuiYueShangDianRewardGroupCfg(RandActivityManager * ramgr) const;	
	const RandActivityZhuiYueShangDianBuyItemCfg * GetRandActivityZhuiYueShangDianBuyItemCfg(RandActivityManager * ramgr, int buy_seq) const;
	
	virtual int GetConfigVer() const;
private: 
	int InitOthercfg(TiXmlElement * RootElement);
	int InitRandActivityZhuiYueShangDianCfg(TiXmlElement * RootElement);
	int InitRandActivityZhuiYueShangDianRewardGroupItemCfg(TiXmlElement * RootElement);
	int InitRandActivityZhuiYueShangDianBuyItemCfg(TiXmlElement * RootElement);

	std::vector<RandActivityZhuiYueShangDianSectionCfg> m_section_cfg;
	std::vector<RandActivityZhuiYueShangDianBuyItemSectionCfg> m_buy_section_cfg;
	std::map<int, RandActivityZhuiYueShangDianRewardGroupCfg> m_reward_group_cfg;
	
	RandActivityZhuiYueShangDianOtherCfg m_other_cfg;
};

#endif	