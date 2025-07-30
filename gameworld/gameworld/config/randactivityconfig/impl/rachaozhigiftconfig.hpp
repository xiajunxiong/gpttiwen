#ifndef __RA_CHAO_ZHI_GIFT_CONFIG_HPP__
#define __RA_CHAO_ZHI_GIFT_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rachaozhigiftdef.hpp"

class RandActivityManager;

namespace RAChaoZhiGift
{
	//其他
	struct OtherCfg
	{
		OtherCfg()
		{
			cfg_ver = 0;
			can_buy_day = 0;
		}
		int cfg_ver;
		int can_buy_day;		//允许购买  活动开启前N天 
	};

	//直购
	struct CmdBuyCfg
	{
		CmdBuyCfg()
		{
			buy_money = 0;
		}

		int buy_money;		//购买价格(元宝)
	};

	//奖池配置
	struct RewardPoolItem
	{
		RewardPoolItem()
		{
			power = 0;

			reward_min = 0;
			reward_max = 0;
		}

		int power;	//奖励权重

		//奖励数量 闭区间(均匀)
		int reward_min;
		int reward_max;
	};


	//礼包配置
	struct GiftCfg
	{
		GiftCfg()
		{
			gift_index = 0;
			can_fetch_act_day = 0;

			reward_id = 0;
			is_bind = 0;

			reward_min = 0;
			reward_max = 0;

			total_power = 0;
		}

		const ItemConfigData RandRewardItem() const;

		int gift_index;
		int can_fetch_act_day;		//礼包领取时需求的活动开启天数

		ItemID reward_id;	//奖励物品id
		int is_bind;		//物品绑定

		//奖励数量 闭区间(验证用)
		int reward_min;
		int reward_max;

		//奖池<权重_begin, 配置>
		int total_power;
		std::map<int, RewardPoolItem> reward_pool;
	};
	//礼包map<礼包index, 礼包配置>
	typedef std::map<int, GiftCfg> GiftIndexMap;
};

class RAChaoZhiGiftConfig : public RandActivityConfig
{
public:
	RAChaoZhiGiftConfig();
	virtual ~RAChaoZhiGiftConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAChaoZhiGift::OtherCfg &GetOtherCfg() const { return m_other_cfg; };
	const RAChaoZhiGift::CmdBuyCfg * GetCmdBuyCfg(RandActivityManager* ramgr) const;
	const RAChaoZhiGift::GiftCfg * GetGiftCfg(RandActivityManager* ramgr, unsigned int act_begin_time, int gift_index) const;

private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitCmdBuyCfg(TiXmlElement * RootElement);
	int InitGiftCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);

	RAChaoZhiGift::OtherCfg m_other_cfg;

	//直购阶段配置<活动开启时的开服天数, 直购配置>
	std::map<int, RAChaoZhiGift::CmdBuyCfg> m_cmd_buy_day_section_cfg;

	//礼包阶段配置<活动开启时的开服天数, 礼包map>
	std::map<int, RAChaoZhiGift::GiftIndexMap> m_gift_day_section_cfg;

	
};

#endif