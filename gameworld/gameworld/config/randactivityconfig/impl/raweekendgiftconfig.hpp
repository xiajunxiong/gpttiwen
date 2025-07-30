#ifndef __RA_WEEKEND_GIFT_CONFIG_HPP__
#define __RA_WEEKEND_GIFT_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/raweekendgiftdef.hpp"

class RandActivityManager;

namespace RAWeekendGift
{
	//这个和专属礼包读的同一个分页
	struct OtherCfg
	{
		OtherCfg()
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


	/*
	结构 开服时间 map:	开服天数 
						等级阶段 map:	角色等级
										阶段配置vec

	*/
	//阶段配置
	struct PhaseCfg
	{
		PhaseCfg()
		{
			day_section_begin = 0;
			role_level_section_begin = 0;

			chong_zhi_min = 0;
			chong_zhi_max = 0;

			gift_group_id = 0;
		}

		int day_section_begin;		//阶段的起始开服天数
		int role_level_section_begin;//角色等级段 起始
		
		//角色充值闭区间
		int chong_zhi_min;
		int chong_zhi_max;

		//礼包组id
		int gift_group_id;
	};

	//阶段配置vec
	typedef std::vector<PhaseCfg> PhaseCfgVec;

	//等级分段map<level_begin, 阶段配置vec>
	typedef std::map<int, PhaseCfgVec> SectionLevelMap;

	//开服天数分段map<开服天数, 等级分段map>
	typedef std::map<int, SectionLevelMap> SectionDayMap;

	//礼包配置
	struct GiftCfg
	{
		GiftCfg()
		{
			gift_type_index = 0;
			limit_num = 0;
			buy_money = 0;
		}
		int gift_type_index;	//礼包索引

		int limit_num;			//限购数量

		int buy_money;			//购买价值(元宝)

		//奖励
		std::vector<ItemConfigData> item_vec;	//物品
	};

	//奖励组配置
	struct GroupCfg
	{
		GroupCfg()
		{
			group_id = 0;
			total_power = 0;
		}
		int group_id;

		int total_power;

		//礼包池<权重end, 礼包索引>
		typedef std::map<int, int> RandPool;
		RandPool gift_index_rand_pool;
	};
	//奖励组map <奖励组id, 奖励组配置>
	typedef std::map<int, GroupCfg> GroupIDMap;
};

class RAWeekendGiftConfig : public RandActivityConfig
{
public:
	RAWeekendGiftConfig();
	virtual ~RAWeekendGiftConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAWeekendGift::OtherCfg & GetOtherCfg() const { return m_other_cfg; };

	//活动满足条件的随机礼包索引
	const int GetRandGiftIndex(RandActivityManager* ramgr, int role_level, int chong_zhi_value) const;

	const RAWeekendGift::GiftCfg * GetGiftCfg(int gift_type_index) const;

private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitGiftCfg(TiXmlElement * RootElement);
	int InitPhaseCfg(TiXmlElement * RootElement);

	RAWeekendGift::OtherCfg m_other_cfg;

	//礼包配置vec[礼包索引]
	std::vector<RAWeekendGift::GiftCfg> m_gift_cfg_vec;
	RAWeekendGift::GroupIDMap m_group_id_map;
	RAWeekendGift::SectionDayMap m_section_day_cfg;


};
#endif